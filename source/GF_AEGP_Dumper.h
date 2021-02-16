#pragma once
#ifndef GF_AEGP_DUMPER_H
#define GF_AEGP_DUMPER_H

#include "GF_GlobalTypes.h"
#include "GF_GlobalClasses.h"
#include "GF_AEGP_Relinker.h"
#include "GF_AEGP_Main.h"
#include "GF_AEGP_Strings.h"
#include "NodeObjects/AeConteiner.h"
#include "AeGfsFileBuilder.h"

#include "BatchRelinker/AeBatchRelinker.h"

class GF_AEGP_Relinker;

class GF_Dumper 
{
public:
	GF_Dumper(SPBasicSuite *basicSuite, AEGP_PluginID pI, beamerParamsStruct *GF_params);
	~GF_Dumper();

	ErrorCodesAE newCopyRelinkFootages();
	ErrorCodesAE newCopyCollectFonts();
	ErrorCodesAE newCollectEffectsInfo();

	ErrorCodesAE newDumpProject();
	ErrorCodesAE newBatchDumpProject();
	static ErrorCodesAE PreCheckProject(SPBasicSuite *pb, AEGP_PluginID pluginId, beamerParamsStruct *GF_params);
	ErrorCodesAE PrepareProject();

	ErrorCodesAE DumpQueeItems(fs::path outputPath);
	ErrorCodesAE DumpQueeItem(A_long itemIndex, fs::path outputPath);
	ErrorCodesAE DumpOutputModules(AEGP_RQItemRefH &rq_ItemRef, A_long outModulesNumber, gfsRqItem *parentItem, fs::path outputPath);
	ErrorCodesAE setConteiner(AeSceneConteiner &sc);

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

	A_UTF16Char newRemotePath[AEGP_MAX_PATH_SIZE];
	A_UTF16Char dialogText[128];
	A_char projectName[AEGP_MAX_PROJ_NAME_SIZE];


	beamerParamsStruct bps;
	A_char tmp_message[512];
	AeSceneConteiner* sc;
};
#endif 
