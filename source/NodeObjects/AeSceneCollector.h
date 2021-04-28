#pragma once
#ifndef AESCENECOLLECTOR_H
#define AESCENECOLLECTOR_H

#include "../GF_GlobalTypes.h"
#include "AeObjectNode.h"
#include "AeCompNode.h"
#include "AeFooNode.h"
#include "AeConteiner.h"

namespace RenderBeamer {

class AeSceneCollector
{
public:	
	AeSceneCollector(AEGP_PluginID pluginId, SPBasicSuite *sp, AEGP_ProjectH projectH, AeSceneContainer &theCt);
	ErrorCodesAE collectSceneRenderQueueItems();
	ErrorCodesAE AeSceneCollect(A_Boolean useUiExporter=0);
	ErrorCodesAE AeNormalCollect(A_Boolean useUiExporter=0);
	ErrorCodesAE AeSmartCollect(A_Boolean useUiExporter=0);
	ErrorCodesAE generateDebugItemsInfo();

	AeSceneContainer* ct;

protected:
	ErrorCodesAE collectSceneItems();	
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
	AEGP_PersistentBlobH pbh;
	A_long pbh_items_number, smart_collect, continue_on_missing;
};
} // namespace RenderBeamer
#endif
