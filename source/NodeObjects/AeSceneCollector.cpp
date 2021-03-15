#include "AeSceneCollector.h"

AeSceneCollector::AeSceneCollector(AEGP_PluginID PluginId, SPBasicSuite *Sp, AEGP_ProjectH ProjectH, AeSceneConteiner &theCt)
	: ct(&theCt)
	, sp(Sp)
	, pluginId(PluginId)
	, projectH(ProjectH)	
{
}
ErrorCodesAE AeSceneCollector::AeNormalCollect(A_Boolean useUiExporter)
{
	AEGP_SuiteHandler suites(sp);
	ERROR_CATCH_START
		ERROR_AE(collectSceneItems())
		ERROR_AE(collectToRender())
		if (useUiExporter != 0) {
			ERROR_AE(collectSceneUiRenderQueueItems())
		}
    ERROR_CATCH_END_RETURN(suites)
}
ErrorCodesAE AeSceneCollector::AeSmartCollect(A_Boolean useUiExporter)
{
	AEGP_SuiteHandler suites(sp);
	ERROR_CATCH_START
		ERROR_AE(collectSceneItems())
		if(useUiExporter != 0) {
			ERROR_AE(collectSceneUiRenderQueueItems())
		}
		else {
			ERROR_AE(collectSceneRenderQueueItems())
		}
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
	AEGP_SuiteHandler suites(sp);
	ErrorCodesAE _ErrorCode = NoError;
	AEGP_RenderItemStatusType renderState = AEGP_RenderItemStatus_NONE;
	AEGP_RQItemRefH rqItemH = nullptr;
	AEGP_CompH compH = nullptr;
	AEGP_ItemH itemH = nullptr;
	A_long totalQueued = 0, i = 0, rqOutputs = 0, rqAdded = 0;
	ERROR_AEER(suites.RQItemSuite3()->AEGP_GetNumRQItems(&totalQueued))

	while (i < totalQueued && _ErrorCode == NoError) {
		ERROR_AEER(suites.RQItemSuite3()->AEGP_GetRQItemByIndex(i++, &rqItemH))
		ERROR_AEER(suites.RQItemSuite3()->AEGP_GetRenderState(rqItemH, &renderState))
		if (renderState == AEGP_RenderItemStatus_QUEUED && _ErrorCode == NoError) {
			ERROR_AEER(suites.RQItemSuite3()->AEGP_GetNumOutputModulesForRQItem(rqItemH, &rqOutputs))
			if (rqOutputs > 0 && _ErrorCode == NoError)
			{
				ERROR_AEER(suites.RQItemSuite3()->AEGP_GetCompFromRQItem(rqItemH, &compH))
				ERROR_AEER(suites.CompSuite11()->AEGP_GetItemFromComp(compH, &itemH))
				ERROR_AEER(collectSceneRqItem(new AeObjectNode(pluginId, sp, itemH, 0)))
				if(_ErrorCode == NoError)
					rqAdded++;
			}
			_ErrorCode = NoError;
		}
	}
	ERROR_AE(rqAdded > 0 ? NoError : NoValidRqItems)
    return _ErrorCode;
}
ErrorCodesAE AeSceneCollector::collectSceneUiRenderQueueItems()
{
	AEGP_SuiteHandler suites(sp);
	ErrorCodesAE _ErrorCode = NoError;
	AEGP_PersistentBlobH bh = nullptr;
	A_long rq_items=0, it=0, temp_long=0;
	std::string it_str, memory_buff, memory_buff1;
	AEGP_RQItemRefH	rq_ItemRef = nullptr;
	AEGP_ItemH rq_ItemH = nullptr;
	AEGP_CompH rq_ItemComposition = nullptr;
	AEGP_OutputModuleRefH rq_ItemOutModuleRef = nullptr;
	AEGP_MemHandle mem_handle = nullptr, mem_handle1 = nullptr;
	gfsRqItem *gfs_rq_node = nullptr;
	gfsRqItemOutput *gfs_rq_node_out = nullptr;	
	
	ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetApplicationBlob(AEGP_PersistentType_MACHINE_SPECIFIC, &bh))
	ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetLong(bh, "renderBeamer", "rq_items", 0, &rq_items))
	if(_ErrorCode == NoError && rq_items > 0)
	{
		while(it < rq_items)
		{
			gfs_rq_node = new gfsRqItem;
			gfs_rq_node_out = new gfsRqItemOutput({ 0, "png", "-", 1, 0, 0, 0, {48000, AEGP_SoundEncoding_UNSIGNED_PCM, 2, 2}, "" });
			if (!gfs_rq_node || !gfs_rq_node_out)
				return AE_ErrAlloc;
			it_str = std::to_string(it);
			
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetLong(bh, "renderBeamer", ("rq_id_" + it_str).c_str(), 0, &gfs_rq_node->indexNr))
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetLong(bh, "renderBeamer", ("width_" + it_str).c_str(), 0, &gfs_rq_node->width))
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetLong(bh, "renderBeamer", ("height_" + it_str).c_str(), 0, &gfs_rq_node->height))
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetString(bh, "renderBeamer", ("frame_range_" + it_str).c_str(), "0to1s1", AEGP_MAX_ITEM_NAME_SIZE, &gfs_rq_node->frame_string[0], NULL))
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetString(bh, "renderBeamer", ("fps_" + it_str).c_str(), "25", AEGP_MAX_ITEM_NAME_SIZE, &gfs_rq_node->fps[0], NULL))
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetLong(bh, "renderBeamer", ("rq_out_id_" + it_str).c_str(), 0, &gfs_rq_node_out->indexNr))
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetLong(bh, "renderBeamer", ("smart_collect_" + it_str).c_str(), 0, &temp_long))
			if(_ErrorCode == NoError) gfs_rq_node->smart_collect = static_cast<A_Boolean>(temp_long);
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetLong(bh, "renderBeamer", ("ignore_missings_" + it_str).c_str(), 0, &temp_long))
			if (_ErrorCode == NoError) gfs_rq_node->continue_on_missing = static_cast<A_Boolean>(temp_long);
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetLong(bh, "renderBeamer", ("audio_depth_" + it_str).c_str(), 0, &gfs_rq_node_out->soundFormat.bytes_per_sampleL))
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetLong(bh, "renderBeamer", ("audio_channels_" + it_str).c_str(), 0, &gfs_rq_node_out->soundFormat.num_channelsL))
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetFpLong(bh, "renderBeamer", ("audio_sample_rate_" + it_str).c_str(), 0, &gfs_rq_node_out->soundFormat.sample_rateF))
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetLong(bh, "renderBeamer", ("out_audio_enabled_" + it_str).c_str(), 0, &temp_long))
			if (_ErrorCode == NoError) gfs_rq_node_out->outputAudioEnabled = static_cast<A_Boolean>(temp_long);
			ERROR_AEER(suites.PersistentDataSuite4()->AEGP_GetLong(bh, "renderBeamer", ("comp_has_audio_" + it_str).c_str(), 0, &temp_long))
			if (_ErrorCode == NoError) gfs_rq_node_out->outputAudioSetToUse = static_cast<A_Boolean>(temp_long);

			ERROR_AEER(suites.RQItemSuite3()->AEGP_GetRQItemByIndex(gfs_rq_node->indexNr - 1, &rq_ItemRef))
			ERROR_AEER(suites.RQItemSuite3()->AEGP_GetCompFromRQItem(rq_ItemRef, &rq_ItemComposition))
			ERROR_AEER(suites.CompSuite10()->AEGP_GetItemFromComp(rq_ItemComposition, &rq_ItemH))
			ERROR_AEER(suites.ItemSuite7()->AEGP_GetItemName(rq_ItemH, gfs_rq_node->compositio_name))
			ERROR_AEER(suites.OutputModuleSuite4()->AEGP_GetOutputModuleByIndex(rq_ItemRef, gfs_rq_node_out->indexNr - 1, &rq_ItemOutModuleRef))
			ERROR_AEER(suites.OutputModuleSuite4()->AEGP_GetOutputFilePath(rq_ItemRef, rq_ItemOutModuleRef, &mem_handle))
			ERROR_AEER(rbUtilities::copyMemhUTF16ToString(sp, mem_handle, memory_buff))
			if (_ErrorCode == NoError) gfs_rq_node_out->outputFile = memory_buff;
			ERROR_AEER(suites.OutputModuleSuite4()->AEGP_GetExtraOutputModuleInfo(rq_ItemRef, rq_ItemOutModuleRef, &mem_handle, &mem_handle1, &gfs_rq_node_out->outFileIsSeq, &gfs_rq_node_out->outFileIsMultiframe))
			ERROR_AEER(rbUtilities::copyMemhUTF16ToString(sp, mem_handle, memory_buff))
			ERROR_AEER(rbUtilities::copyMemhUTF16ToString(sp, mem_handle1, memory_buff1))
			if (_ErrorCode == NoError) RB_STRNCPTY(gfs_rq_node_out->outputType, memory_buff.c_str(), 46);
			if (_ErrorCode == NoError) RB_STRNCPTY(gfs_rq_node_out->outputInfo, memory_buff1.c_str(), 94);

			if (gfs_rq_node->smart_collect != 0) {
				ERROR_AEER(collectSceneRqItem(new AeObjectNode(pluginId, sp, rq_ItemH, 0)))
			}
			if (_ErrorCode == NoError)
			{
				gfs_rq_node->output_mods.push_back(gfs_rq_node_out);
				ct->gfsRqItemsList.push_back(gfs_rq_node);
				gfs_rq_node_out = nullptr;
				gfs_rq_node = nullptr;
			}

			suites.PersistentDataSuite4()->AEGP_DeleteEntry(bh, "renderBeamer", ("rq_id_" + it_str).c_str());
			suites.PersistentDataSuite4()->AEGP_DeleteEntry(bh, "renderBeamer", ("name_" + it_str).c_str());
			suites.PersistentDataSuite4()->AEGP_DeleteEntry(bh, "renderBeamer", ("width_" + it_str).c_str());
			suites.PersistentDataSuite4()->AEGP_DeleteEntry(bh, "renderBeamer", ("height_" + it_str).c_str());
			suites.PersistentDataSuite4()->AEGP_DeleteEntry(bh, "renderBeamer", ("frame_range_" + it_str).c_str());
			suites.PersistentDataSuite4()->AEGP_DeleteEntry(bh, "renderBeamer", ("fps_" + it_str).c_str());
			suites.PersistentDataSuite4()->AEGP_DeleteEntry(bh, "renderBeamer", ("rq_out_id_" + it_str).c_str());
			suites.PersistentDataSuite4()->AEGP_DeleteEntry(bh, "renderBeamer", ("output_full_path_" + it_str).c_str());
			suites.PersistentDataSuite4()->AEGP_DeleteEntry(bh, "renderBeamer", ("audio_depth_" + it_str).c_str());
			suites.PersistentDataSuite4()->AEGP_DeleteEntry(bh, "renderBeamer", ("audio_channels_" + it_str).c_str());
			suites.PersistentDataSuite4()->AEGP_DeleteEntry(bh, "renderBeamer", ("audio_sample_rate_" + it_str).c_str());
			suites.PersistentDataSuite4()->AEGP_DeleteEntry(bh, "renderBeamer", ("out_audio_enabled_" + it_str).c_str());
			suites.PersistentDataSuite4()->AEGP_DeleteEntry(bh, "renderBeamer", ("comp_has_audio_" + it_str).c_str());
			++it;
		}
	}
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
	return NoError;
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
ErrorCodesAE AeSceneCollector::collectFootagesToRender(AeObjectNode *node)
{
	if (!node)
		return NullPointerResult;
	auto it = footageSortedList.cbegin();
	auto end = footageSortedList.cend();

	while (it != end) {
		if ((*node) == it)
			break;
		++it;
	}
	if (it != end) {
		auto *ftNode = new AeFootageNode(*it);
		footageSortedList.erase(it);
		renderFootagePushBack(ftNode);
	}
	delete node;
	node = nullptr;
	return NoError;
}
ErrorCodesAE AeSceneCollector::renderFootagePushBack(AeFootageNode *node)
{
	if (!node)
		return NullPointerResult;

	node->generateFootData();
	if (node->isFooMissing()) {
		ct->renderFootageMissingList.push_back(node);
	}
	else {
		ct->renderFootageSortedList.push_back(node);
	}
	return NoError;
}

ErrorCodesAE AeSceneCollector::smartTrimProject()
{
	footageSortedList.sort();
	while (!footageSortedList.empty()) 
	{		
		if (footageSortedList.front()) {
			footageSortedList.front()->deleteItem();
			delete footageSortedList.front();
		}
		footageSortedList.pop_front();
	}
	compositionsSortedList.sort();
	while (!compositionsSortedList.empty()) {		
		if (compositionsSortedList.front()) {
			compositionsSortedList.front()->deleteItem();
			delete compositionsSortedList.front();
		}
		compositionsSortedList.pop_front();
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
