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
	ErrorCodesAE AeNormalCollect();
	ErrorCodesAE AeSmartCollect();
	void generateDebugItemsInfo();

	AeSceneConteiner* ct;

protected:
	ErrorCodesAE collectSceneItems();
	ErrorCodesAE collectSceneRenderQueueItems();
	ErrorCodesAE collectSceneItem(AeObjectNode *node);
	ErrorCodesAE collectSceneRqItem(AeObjectNode *node);
	ErrorCodesAE collectToRender();
	ErrorCodesAE smartCollectToRender();

	ErrorCodesAE smartTrimProject();
	ErrorCodesAE collectCompositionLayersToRender(AeCompNode *node);
	ErrorCodesAE collectFootagesToRender(AeObjectNode *node);
	ErrorCodesAE renderCompositionPushBack(AeCompNode *node);
	ErrorCodesAE renderFootagePushBack(AeFootageNode *node);

	aeObjNodes compositionsSortedList;
	aeObjNodes footageSortedList;
	aeObjNodes rqCompositionSortedList;

	SPBasicSuite *sp;
	AEGP_PluginID pluginId;
	AEGP_ProjectH projectH;
};

#endif