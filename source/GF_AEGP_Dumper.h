#pragma once
#ifndef GF_AEGP_DUMPER_H
#define GF_AEGP_DUMPER_H

#include "GF_GlobalTypes.h"
#include "GF_GlobalClasses.h"
#include "GF_AEGP_Relinker.h"
#include "NodeObjects/AeConteiner.h"
#include "AeGfsFileCreator.h"
#include "BatchRelinker/AeBatchRelinker.h"

class GF_AEGP_Relinker;

class GF_Dumper 
{
public:
	GF_Dumper(SPBasicSuite *basicSuite, AEGP_PluginID pI, beamerParamsStruct *GF_params);
	~GF_Dumper();

	ErrorCodesAE newCopyRelinkFootages();
	ErrorCodesAE newCopyCollectFonts();
	ErrorCodesAE newCollectEffectsInfo() const;

	ErrorCodesAE newDumpProject();
	ErrorCodesAE newBatchDumpProject(bool is_ui_caller = false);
	static ErrorCodesAE PreCheckProject(SPBasicSuite *pb, AEGP_PluginID pluginId, beamerParamsStruct *GF_params);
	ErrorCodesAE PrepareProject();

	ErrorCodesAE DumpUiQueueItems(const fs::path& outputPath) const;
	ErrorCodesAE DumpQueueItems(const fs::path& outputPath);
	ErrorCodesAE DumpQueueItem(A_long itemIndex, const fs::path& outputPath);
	ErrorCodesAE DumpOutputModules(AEGP_RQItemRefH &rq_ItemRef, A_long outModulesNumber, gfsRqItem *parentItem, fs::path outputPath);
	ErrorCodesAE setConteiner(AeSceneConteiner &aesc);

	static rbProjectClass *rbProj() {
		static rbProjectClass rbProjIt;
		return &rbProjIt;
	}
	AEGP_ProjectH rootProjH;

private:
	SPBasicSuite *sP;
	AEGP_SuiteHandler suites;
	AEGP_PluginID pluginId;
	PF_AppProgressDialogP relinkerProg;
	GF_AEGP_Relinker relinker;

	A_UTF16Char dialogText[128];
	A_char projectName[AEGP_MAX_PROJ_NAME_SIZE];


	beamerParamsStruct bps;
	A_char tmp_message[512];
	AeSceneConteiner* sc;
};
#endif 
