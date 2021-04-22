#include "AeSceneCollector.h"
#include <clocale>

#define SMART_COLLECT_ON 1

AeSceneCollector::AeSceneCollector(AEGP_PluginID PluginId, SPBasicSuite *Sp, AEGP_ProjectH ProjectH, AeSceneConteiner &theCt)
	: ct(&theCt)
	, sp(Sp)
	, pluginId(PluginId)
	, projectH(ProjectH)
	, pbh(nullptr)
	, pbh_items_number(0)
	, smart_collect(0)
	, continue_on_missing(1)
{
	ERROR_CATCH_START
	std::setlocale(LC_ALL, ".utf8");
	ERROR_CATCH_END_NO_INFO
}
ErrorCodesAE AeSceneCollector::AeSceneCollect(A_Boolean useUiExporter)
{
	AEGP_SuiteHandler suites(sp);
	ERROR_CATCH_START		
		ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetApplicationBlob(AEGP_PersistentType_MACHINE_SPECIFIC, &pbh))
		ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetLong(pbh, "renderBeamer", "rq_items", 0, &pbh_items_number))
		if (_ErrorCode == NoError && pbh_items_number > 0)
		{
			//useUiExporter = TRUE;
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetLong(pbh, "renderBeamer", "smart_collect", 0, &smart_collect))
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetLong(pbh, "renderBeamer", "ignore_missings", 1, &continue_on_missing))

			suites.PersistentDataSuite4()->AEGP_DeleteEntry(pbh, "renderBeamer", "smart_collect");
			suites.PersistentDataSuite4()->AEGP_DeleteEntry(pbh, "renderBeamer", "ignore_missings");
			suites.PersistentDataSuite4()->AEGP_DeleteEntry(pbh, "renderBeamer", "rq_items");
		}
		if (_ErrorCode == NoError)
		{
			if (smart_collect == 0)
				ERROR_AE(AeNormalCollect(useUiExporter))
			else
				ERROR_AE(AeSmartCollect(useUiExporter))
		}
		
	ERROR_CATCH_END_RETURN(suites)
}

ErrorCodesAE AeSceneCollector::AeNormalCollect(A_Boolean useUiExporter)
{
	AEGP_SuiteHandler suites(sp);
	ERROR_CATCH_START
		ERROR_AE(collectSceneItems())
		ERROR_AE(collectToRender())
		ERROR_AE(collectSceneRenderQueueItems())		
    ERROR_CATCH_END_RETURN(suites)
}
ErrorCodesAE AeSceneCollector::AeSmartCollect(A_Boolean useUiExporter)
{
	AEGP_SuiteHandler suites(sp);
	ERROR_CATCH_START
		ERROR_AE(collectSceneItems())
		ERROR_AE(collectSceneRenderQueueItems())		
		ERROR_AE(smartCollectToRender())
		ERROR_AE(generateDebugItemsInfo())
	ERROR_CATCH_END_RETURN(suites)
}

ErrorCodesAE AeSceneCollector::collectSceneItems()
{
	A_long itemNr = 1;
	ErrorCodesAE _ErrorCode = NoError;
	AEGP_SuiteHandler suites(sp);
	AEGP_ItemH itemH = nullptr;

	ERROR_AEER(suites.ItemSuite9()->AEGP_GetFirstProjItem(projectH, &itemH))
	while (itemH && _ErrorCode == NoError) {
		collectSceneItem(new AeObjectNode(pluginId, sp, itemH, itemNr++));
		ERROR_AEER(suites.ItemSuite9()->AEGP_GetNextProjItem(projectH, itemH, &itemH))
	}
    return _ErrorCode;
}

ErrorCodesAE AeSceneCollector::collectSceneRenderQueueItems()
{
	gfsRqItem *gfs_rq_node = nullptr;
	gfsRqItemOutput *gfs_rq_node_out = nullptr;
	A_char frameScript[400];
	AEGP_SuiteHandler suites(sp);
	ErrorCodesAE _ErrorCode = NoError;
	AEGP_RenderItemStatusType renderState = AEGP_RenderItemStatus_NONE;
	AEGP_RQItemRefH rqItemH = nullptr;
	AEGP_OutputModuleRefH rq_ItemOutModuleRef = nullptr;
	AEGP_OutputTypes outType = 0;
	AEGP_CompH compH = nullptr;
	AEGP_ItemH itemH = nullptr;
	AEGP_MemHandle memH1 = nullptr, memH2 = nullptr;	
	A_long totalQueued = 0, i = 0, rqOutputs = 0, rqAdded = 0;
	
	ERROR_AEER(suites.RQItemSuite3()->AEGP_GetNumRQItems(&totalQueued))
	while (i < totalQueued && _ErrorCode == NoError) {
		ERROR_AEER(suites.RQItemSuite3()->AEGP_GetRQItemByIndex(i++, &rqItemH))
		ERROR_AEER(suites.RQItemSuite3()->AEGP_GetRenderState(rqItemH, &renderState))
		if (renderState == AEGP_RenderItemStatus_QUEUED && _ErrorCode == NoError) {
			ERROR_AEER(suites.RQItemSuite3()->AEGP_GetNumOutputModulesForRQItem(rqItemH, &rqOutputs))
			if (rqOutputs > 0 && _ErrorCode == NoError)
			{
				gfs_rq_node = new gfsRqItem({1,0,0,0,0,0,"","1to10s1","29.97",{}});
				gfs_rq_node_out = new gfsRqItemOutput({ 1, "PNG", "PNG 16bit", "", "", "", "", 1, 0, 0, 0, {48000, AEGP_SoundEncoding_UNSIGNED_PCM, 2, 2}, "" });
				if (!gfs_rq_node || !gfs_rq_node_out)
					return AE_ErrAlloc;
				
				gfs_rq_node->rq_id = i;
				gfs_rq_node->rq_out_modules_n = rqOutputs;
				gfs_rq_node->renderable = (renderState == AEGP_RenderItemStatus_QUEUED);
				
				ERROR_AEER(suites.RQItemSuite3()->AEGP_GetCompFromRQItem(rqItemH, &compH))
				ERROR_AEER(suites.CompSuite11()->AEGP_GetItemFromComp(compH, &itemH))				
				ERROR_AEER(suites.ItemSuite9()->AEGP_GetItemID(itemH, &gfs_rq_node->composition_id))

				suites.ANSICallbacksSuite1()->sprintf(frameScript, "var rqItem=app.project.renderQueue.item(%d);var rqFps=rqItem.getSettings(GetSettingsFormat.NUMBER)[rqItem.getSettings(GetSettingsFormat.STRING)[\"Frame Rate\"]];(Math.round(rqItem.timeSpanStart*rqFps)).toString()+\"to\"+(Math.round((rqItem.timeSpanStart+rqItem.timeSpanDuration)*rqFps)-1).toString()+\"s1\";", gfs_rq_node->rq_id);
				ERROR_AE(rbUtilities::execScript(sp, pluginId, frameScript, gfs_rq_node->frame_range, 32))

				suites.ANSICallbacksSuite1()->sprintf(frameScript, "var rqItem=app.project.renderQueue.item(%d);rqItem.getSettings(GetSettingsFormat.NUMBER)[rqItem.getSettings(GetSettingsFormat.STRING)[\"Frame Rate\"]];", gfs_rq_node->rq_id);
				ERROR_AE(rbUtilities::execScript(sp, pluginId, frameScript, gfs_rq_node->fps, 32))

				ERROR_AEER(suites.ItemSuite9()->AEGP_GetItemDimensions(itemH, &gfs_rq_node->width, &gfs_rq_node->height))
				ERROR_AEER(suites.ItemSuite9()->AEGP_GetItemName(pluginId, itemH, &memH1))
				ERROR_AEER(rbUtilities::copyMemhUTF16ToString(sp, memH1, gfs_rq_node->name))
				if (gfs_rq_node->name.size() < 3)
					gfs_rq_node->name = std::string("RQ_ITEM_NR_") + std::to_string(i);

				ERROR_AEER(suites.OutputModuleSuite4()->AEGP_GetOutputModuleByIndex(rqItemH, gfs_rq_node_out->rq_out_id - 1, &rq_ItemOutModuleRef))
				ERROR_AEER(suites.OutputModuleSuite4()->AEGP_GetEnabledOutputs(rqItemH, rq_ItemOutModuleRef, &outType))
				ERROR_AEER(suites.OutputModuleSuite4()->AEGP_GetOutputFilePath(rqItemH, rq_ItemOutModuleRef, &memH1))
				ERROR_AEER(rbUtilities::copyMemhUTF16ToPath(sp, memH1, gfs_rq_node_out->outputFile))
				if (gfs_rq_node_out->outputFile.has_extension() && gfs_rq_node_out->outputFile.extension().string().size() < 7)
					suites.ANSICallbacksSuite1()->sprintf(gfs_rq_node_out->file_ext, gfs_rq_node_out->outputFile.extension().string().c_str()+1);
				ERROR_AEER(suites.OutputModuleSuite4()->AEGP_GetExtraOutputModuleInfo(rqItemH, rq_ItemOutModuleRef, &memH1, &memH2, &gfs_rq_node_out->is_out_file_sequence, &gfs_rq_node_out->is_out_file_multi_frame))
				if (_ErrorCode == NoError) {
					suites.MemorySuite1()->AEGP_FreeMemHandle(memH1);
					suites.MemorySuite1()->AEGP_FreeMemHandle(memH2);
				}
				
				if (outType & AEGP_OutputType_AUDIO)
				{
					ERROR_AEER(suites.OutputModuleSuite4()->AEGP_GetSoundFormatInfo(rqItemH, rq_ItemOutModuleRef, &gfs_rq_node_out->soundFormat, &gfs_rq_node_out->audio_out_enabled))
					if (_ErrorCode == NoError)
					{
						gfs_rq_node_out->audio_available_in_comp = true;
						if (gfs_rq_node_out->audio_out_enabled > 0)
							gfs_rq_node_out->audio_out_enabled = 1;
						else
							gfs_rq_node_out->audio_out_enabled = 0;
					}
				}
				
		#ifdef SMART_COLLECT_ON
					if (smart_collect != 0) {
						ERROR_AEER(collectSceneRqItem(new AeObjectNode(pluginId, sp, itemH, 0)))
					}
		#endif
				if (_ErrorCode == NoError)
				{
					gfs_rq_node->output_mods.push_back(gfs_rq_node_out);
					ct->gfsRqItemsList.push_back(gfs_rq_node);
					gfs_rq_node_out = nullptr;
					gfs_rq_node = nullptr;					
					rqAdded++;
				}
			}
			_ErrorCode = NoError;
		}
	}
	ERROR_AE(rqAdded > 0 ? NoError : NoValidRqItems)
    return _ErrorCode;
}
//AEGP_ObjectType_AV
ErrorCodesAE AeSceneCollector::collectSceneItem(AeObjectNode *node)
{
	if (!node)
		return NullPointerResult;
	
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
	return NoError;
}
// as of AE6, solids are now just AEGP_ItemType_FOOTAGE with AEGP_FootageSignature_SOLID

ErrorCodesAE AeSceneCollector::collectSceneRqItem(AeObjectNode *node)
{
	if (!node)
		return NullPointerResult;
	auto const beg = rqCompositionSortedList.cbegin();
	auto const end = rqCompositionSortedList.cend();

	auto it = beg;
	while (it != end) {
		if ((*node) == it)
			break;
		++it;
	}
	if (it == end) {
		rqCompositionSortedList.push_back(node);
		return NoError;
	}
	delete node;
	node = nullptr;
	return NoError;
}

ErrorCodesAE AeSceneCollector::collectToRender()
{
	while (!compositionsSortedList.empty())
	{
		auto *comp_node = new AeCompNode(compositionsSortedList.front());
		comp_node->generateLayers();
		renderCompositionPushBack(comp_node);
        compositionsSortedList.pop_front();
	}
	while (!footageSortedList.empty())
	{
		auto *footage_node = new AeFootageNode(footageSortedList.front());
		renderFootagePushBack(footage_node);
        footageSortedList.pop_front();
	}
	return NoError;
}
ErrorCodesAE AeSceneCollector::smartCollectToRender()
{
	while (!rqCompositionSortedList.empty())
	{
		auto *node = rqCompositionSortedList.front();
		
		auto it = compositionsSortedList.cbegin();
		auto end = compositionsSortedList.cend();
		
		while (it != end) {
			if (*node == it)
				break;
			++it;
		}
		if (it != end) {
			auto *comp_node = new AeCompNode(*it);			
			compositionsSortedList.erase(it); 
			comp_node->generateLayers();
			renderCompositionPushBack(comp_node);
			collectCompositionLayersToRender(comp_node);
		}
		delete node;
		rqCompositionSortedList.pop_front();
	}
	return smartTrimProject();
}
ErrorCodesAE AeSceneCollector::renderCompositionPushBack(AeCompNode *node)
{
	for (auto *effect : node->getEffectsList())
	{
		auto it = ct->effectsList.cbegin();
		auto end = ct->effectsList.cend();
		while (it != end) {
			if (effect == (*it))
				break;
			++it;
		}
		if (it == end) {
			effect->loadEffectInfo(node->getSp());
			ct->effectsList.push_back(effect);
		}		
	}	
	for (auto *font : node->getFontsList())
	{
		auto it = ct->fontsList.cbegin();
		auto end = ct->fontsList.cend();
		while (it != end) {
			if (*font == it)
				break;
			++it;
		}
		if (it == end) {
			ct->fontsList.push_back(font);
		}
	}
	ct->renderCompositionSortedList.push_back(node);
	return NoError;
}

ErrorCodesAE AeSceneCollector::collectCompositionLayersToRender(AeCompNode *node)
{
	if (!node)
		return NullPointerResult;
	
	for (auto *it : node->getLayersList()) {
		if (it->doesLayerHaveSource())
		{
			auto *obj_node = new AeObjectNode(node->getPluginId(), node->getSp(), it->getLayerSource(), 0);
			if (!obj_node)
				continue;
			if (obj_node->getItemType() == AEGP_ItemType_COMP)
			{
				collectSceneRqItem(obj_node);
			}
			else if (obj_node->getItemType() == AEGP_ItemType_FOOTAGE)
			{
				collectFootagesToRender(obj_node);
			}
			else
				delete obj_node;
		}
	}
	return NoError;
}
ErrorCodesAE AeSceneCollector::collectFootagesToRender(AeObjectNode *object_node)
{
	if (!object_node)
		return NullPointerResult;
	auto it = footageSortedList.cbegin();
	const auto end = footageSortedList.cend();

	while (it != end) {
		if (*object_node == it)
			break;
		++it;
	}
	if (it != end) {
		auto *footage_node = new AeFootageNode(*it);
		footageSortedList.erase(it);
		renderFootagePushBack(footage_node);
	}
	delete object_node;
	object_node = nullptr;
	return NoError;
}
ErrorCodesAE AeSceneCollector::renderFootagePushBack(AeFootageNode *footage_node)
{
	if (!footage_node)
		return NullPointerResult;

	footage_node->generateFootData();
	if (footage_node->isFooMissing()) {
		ct->renderFootageMissingList.push_back(footage_node);
	}
	else {
		ct->renderFootageSortedList.push_back(footage_node);
	}
	return NoError;
}

ErrorCodesAE AeSceneCollector::smartTrimProject()
{
	while (!compositionsSortedList.empty()) 
	{		
		footageSortedList.push_back(compositionsSortedList.front());		
		compositionsSortedList.pop_front();
	}
	footageSortedList.sort();
	while (!footageSortedList.empty()) 
	{		
		if (footageSortedList.front()) {
			footageSortedList.front()->deleteItem();
			delete footageSortedList.front();
		}
		footageSortedList.pop_front();
	}
	return NoError;
}

ErrorCodesAE AeSceneCollector::generateDebugItemsInfo()
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
	return NoError;
}
