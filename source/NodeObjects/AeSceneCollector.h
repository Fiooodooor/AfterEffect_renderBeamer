#pragma once
#ifndef AESCENECOLLECTOR_H
#define AESCENECOLLECTOR_H

#include "../GF_GlobalTypes.h"
#include "AeObjectNode.h"
#include "AeCompNode.h"
#include "AeFooNode.h"
#include "AeConteiner.h"


class AeSceneCollector
{
public:	
	AeSceneCollector(AEGP_PluginID pluginId, SPBasicSuite *sp, AEGP_ProjectH projectH, AeSceneConteiner &theCt);
	ErrorCodesAE AeNormalCollect(A_Boolean useUiExporter=0);
	ErrorCodesAE AeSmartCollect(A_Boolean useUiExporter=0);
	ErrorCodesAE generateDebugItemsInfo();

	AeSceneConteiner* ct;

protected:
	ErrorCodesAE collectSceneItems();
	ErrorCodesAE collectSceneRenderQueueItems();
	ErrorCodesAE collectSceneUiRenderQueueItems();
	ErrorCodesAE collectSceneItem(AeObjectNode *node);
	ErrorCodesAE collectSceneRqItem(AeObjectNode *node);
	ErrorCodesAE collectToRender();
	ErrorCodesAE smartCollectToRender();

	ErrorCodesAE smartTrimProject();
	ErrorCodesAE collectCompositionLayersToRender(AeCompNode *node);
	ErrorCodesAE collectFootagesToRender(AeObjectNode *object_node);
	ErrorCodesAE renderCompositionPushBack(AeCompNode *node);
	ErrorCodesAE renderFootagePushBack(AeFootageNode *footage_node);

	aeObjNodes compositionsSortedList;
	aeObjNodes footageSortedList;
	aeObjNodes rqCompositionSortedList;

	SPBasicSuite *sp;
	AEGP_PluginID pluginId;
	AEGP_ProjectH projectH;
};

#endif