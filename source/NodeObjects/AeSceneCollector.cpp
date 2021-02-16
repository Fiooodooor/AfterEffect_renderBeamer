#include "AeSceneCollector.h"

AeSceneCollector::AeSceneCollector(AEGP_PluginID PluginId, SPBasicSuite *Sp, AEGP_ProjectH ProjectH, AeSceneConteiner &theCt)
	: pluginId(PluginId)
	, sp(Sp)
	, projectH(ProjectH)
	, ct(&theCt)
{
}
ErrorCodesAE AeSceneCollector::AeNormalCollect()
{
	collectSceneItems();
	collectToRender();
	return ErrorCodesAE::NoError;
}
ErrorCodesAE AeSceneCollector::AeSmartCollect()
{
	collectSceneItems();
	collectSceneRenderQueueItems();
	smartCollectToRender();
	generateDebugItemsInfo();
	return ErrorCodesAE::NoError;
}

ErrorCodesAE AeSceneCollector::collectSceneItems()
{
	A_long itemNr = 1;
	AEGP_SuiteHandler suites(sp);
	AeObjectNode* node;
	AEGP_ItemH itemH = NULL;

	suites.ItemSuite9()->AEGP_GetFirstProjItem(projectH, &itemH);
	while (itemH) {
		node = new AeObjectNode(pluginId, sp, itemH, itemNr++);
		collectSceneItem(node);
		suites.ItemSuite9()->AEGP_GetNextProjItem(projectH, itemH, &itemH);
	}
	return ErrorCodesAE::NoError;
}

ErrorCodesAE AeSceneCollector::collectSceneRenderQueueItems()
{
	AEGP_SuiteHandler suites(sp);
	AEGP_RenderItemStatusType renderState = AEGP_RenderItemStatus_NONE;
	AEGP_RQItemRefH rqItemH = NULL;
	AEGP_CompH compH = NULL;
	AEGP_ItemH itemH = NULL;
	A_long totalQueed = 0, i = 0, rqOutputs = 0, rqAdded = 0;
	suites.RQItemSuite3()->AEGP_GetNumRQItems(&totalQueed);

	while (i < totalQueed) {
		suites.RQItemSuite3()->AEGP_GetRQItemByIndex(i++, &rqItemH);
		suites.RQItemSuite3()->AEGP_GetRenderState(rqItemH, &renderState);
		if (renderState == AEGP_RenderItemStatus_QUEUED) {
			suites.RQItemSuite3()->AEGP_GetNumOutputModulesForRQItem(rqItemH, &rqOutputs);
			if (rqOutputs > 0) {
				rqAdded++;
				suites.RQItemSuite3()->AEGP_GetCompFromRQItem(rqItemH, &compH);
				suites.CompSuite11()->AEGP_GetItemFromComp(compH, &itemH);
				collectSceneRqItem(new AeObjectNode(pluginId, sp, itemH, 0));
			}
		}
	}

	return !(rqAdded > 0) ? ErrorCodesAE::NoError : ErrorCodesAE::NoValidRqItems;
}
//AEGP_ObjectType_AV
ErrorCodesAE AeSceneCollector::collectSceneItem(AeObjectNode *node)
{
	if (!node)
		return ErrorCodesAE::NullPointerResult;
	
	switch(node->getItemType())
	{			
		case AEGP_ItemType_FOLDER:
			ct->renderOtherSortedList.push_back(node);
			break;
		case AEGP_ItemType_COMP:
			compositionsSortedList.push_back(node);
			break;
		case AEGP_ItemType_FOOTAGE:
			if(node->isItemFooSolid())
				ct->renderOtherSortedList.push_back(node);
			else
				footageSortedList.push_back(node);
			break;
		case AEGP_ItemType_NONE:
		case AEGP_ItemType_NUM_TYPES1:
		default:
			ct->renderOtherSortedList.push_back(node);
			break;
	}
	return ErrorCodesAE::NoError;
}
// as of AE6, solids are now just AEGP_ItemType_FOOTAGE with AEGP_FootageSignature_SOLID

ErrorCodesAE AeSceneCollector::collectSceneRqItem(AeObjectNode *node)
{
	if (!node)
		return ErrorCodesAE::NullPointerResult;
	aeObjNodesIt beg = rqCompositionSortedList.cbegin();
	aeObjNodesIt end = rqCompositionSortedList.cend();

	aeObjNodesIt it = beg;
	while (it != end) {
		if ((*node) == it)
			break;
		it++;
	}
	if (it == end) {
		rqCompositionSortedList.push_back(node);
		return ErrorCodesAE::NoError;
	}
	delete node;
	node = NULL;
	return ErrorCodesAE::NoError;
	//return 2;
}

ErrorCodesAE AeSceneCollector::collectToRender()
{
	AeCompNode *cmpNode;
	AeFootageNode * ftNode;
	while (!compositionsSortedList.empty())
	{
		cmpNode = new AeCompNode(compositionsSortedList.front());
		cmpNode->generateLayers();
		renderCompositionPushBack(cmpNode);
        compositionsSortedList.pop_front();
	}
	while (!footageSortedList.empty())
	{
		ftNode = new AeFootageNode(footageSortedList.front());
		renderFootagePushBack(ftNode);
        footageSortedList.pop_front();
	}
	return ErrorCodesAE::NoError;
}
ErrorCodesAE AeSceneCollector::smartCollectToRender()
{
	AeObjectNode *node;
	AeCompNode *cmpNode;
	aeObjNodesIt end, it;

	while (!rqCompositionSortedList.empty())
	{
		node = rqCompositionSortedList.front();
		
		it = compositionsSortedList.cbegin();
		end = compositionsSortedList.cend();
		while (it != end) {
			if ((*node) == it)
				break;
			it++;
		}
		if (it != end) {
			cmpNode = new AeCompNode(*it);
			compositionsSortedList.erase(it); 
			cmpNode->generateLayers();
			renderCompositionPushBack(cmpNode);
			collectCompositionLayersToRender(cmpNode);
		}
		rqCompositionSortedList.pop_front();
	}
	return smartTrimProject();
}
ErrorCodesAE AeSceneCollector::renderCompositionPushBack(AeCompNode *node)
{
	std::list<AeEffectNode*>::const_iterator it, end;
	for (auto effect : node->getEffectsList())
	{
		it = ct->effectsList.cbegin();
		end = ct->effectsList.cend();
		while (it != end) {
			if (effect == (*it))
				break;
			it++;
		}
		if (it == end) {
			effect->loadEffectInfo(node->getSp());
			ct->effectsList.push_back(effect);
		}
	}
	std::list<AeFontNode*>::const_iterator fit, fend;
	for (auto font : node->getFontsList())
	{
		fit = ct->fontsList.cbegin();
		fend = ct->fontsList.cend();
		while (fit != fend) {
			if (*font == fit)
				break;
			fit++;
		}
		if (fit == fend) {
			ct->fontsList.push_back(font);
		}
	}
	ct->renderCompositionSortedList.push_back(node);
	return ErrorCodesAE::NoError;
}

ErrorCodesAE AeSceneCollector::collectCompositionLayersToRender(AeCompNode *node)
{
	if (!node)
		return ErrorCodesAE::NullPointerResult;
	
	for (auto it : node->getLayersList()) {
		if (it->doesLayerHaveSource())
		{
			AeObjectNode* objNode = new AeObjectNode(node->getPluginId(), node->getSp(), it->getLayerSource(), 0);
			if (!objNode)
				continue;
			if (objNode->getItemType() == AEGP_ItemType_COMP)
			{
				collectSceneRqItem(objNode);
			}
			else if (objNode->getItemType() == AEGP_ItemType_FOOTAGE)
			{
				collectFootagesToRender(objNode);
			}
			else
				delete objNode;
		}
	}
	return ErrorCodesAE::NoError;
}
ErrorCodesAE AeSceneCollector::collectFootagesToRender(AeObjectNode *node)
{
	if (!node)
		return ErrorCodesAE::NullPointerResult;
	AeFootageNode * ftNode;
	aeObjNodesIt it = footageSortedList.cbegin();
	aeObjNodesIt end = footageSortedList.cend();

	while (it != end) {
		if ((*node) == it)
			break;
		it++;
	}
	if (it != end) {
		ftNode = new AeFootageNode(*it);
		footageSortedList.erase(it);
		renderFootagePushBack(ftNode);
	}
	delete node;
	node = NULL;
	return ErrorCodesAE::NoError;
}
ErrorCodesAE AeSceneCollector::renderFootagePushBack(AeFootageNode *node)
{
	if (!node)
		return ErrorCodesAE::NullPointerResult;

	node->generateFootData();
	if (node->isFooMissing()) {
		ct->renderFootageMissingList.push_back(node);
	}
	else {
		ct->renderFootageSortedList.push_back(node);
	}
	return ErrorCodesAE::NoError;
}

ErrorCodesAE AeSceneCollector::smartTrimProject()
{
	AeObjectNode* obNode = NULL;
	footageSortedList.sort();
	while (!footageSortedList.empty()) 
	{
		obNode = footageSortedList.front();
		footageSortedList.pop_front();
		if (obNode) {
			obNode->deleteItem();			
			delete obNode;
		}
		obNode = NULL;
	}
	compositionsSortedList.sort();
	while (!compositionsSortedList.empty()) {
		obNode = compositionsSortedList.front();
		compositionsSortedList.pop_front();
		if (obNode) {
			obNode->deleteItem();
			delete obNode;
		}
		obNode = NULL;
	}
	return ErrorCodesAE::NoError;
}

void AeSceneCollector::generateDebugItemsInfo()
{
/*	FILE *out = fopen("d:/_1Dump.txt", "w");
	if (out) {
		fprintf(out, "\nComposition list, size: %zd \n", ct->renderCompositionSortedList.size());
		for (auto it : ct->renderCompositionSortedList) {
			it->printInfo(out);
			fprintf(out, "\n");
		}	
		fprintf(out, "\nTo-Relink itemH based objects list sorted, size: %zd \n", ct->renderFootageSortedList.size());
		for (auto it : ct->renderFootageSortedList) {
			it->printInfo(out);
			fprintf(out, "\n");
		}
		fclose(out);
	}*/
}
