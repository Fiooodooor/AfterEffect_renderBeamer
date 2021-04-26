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
	GF_Dumper(SPBasicSuite *basicSuite, AEGP_PluginID pI);
	~GF_Dumper();

	ErrorCodesAE newCopyCollectFonts();
	ErrorCodesAE newCollectEffectsInfo() const;

	ErrorCodesAE newBatchDumpProject();
	static ErrorCodesAE PreCheckProject(SPBasicSuite *pb, AEGP_PluginID pluginId, beamerParamsStruct &globals_and_paths);
	ErrorCodesAE PrepareProject();
	ErrorCodesAE DumpUiQueueItems(const fs::path& outputPath) const;
	ErrorCodesAE SetupUiQueueItems();
	
	ErrorCodesAE setPathsStruct(beamerParamsStruct &globals_and_paths);
	ErrorCodesAE setConteiner(AeSceneContainer &aesc);

	static rbProjectClass *rbProj() {
		static rbProjectClass rbProjIt;
		return &rbProjIt;
	}
	AEGP_ProjectH rootProjH;

	PF_AppProgressDialogP *get_progress_dialog(bool force_new=false, bool indeterminate=true, int dialog_text_nr=0);

private:
	SPBasicSuite *sP;
	AEGP_SuiteHandler suites;
	AEGP_PluginID pluginId;
	PF_AppProgressDialogP dumper_progressbar_;
	GF_AEGP_Relinker relinker;
	AeGfsFileCreator *gfs_creator = nullptr;

	A_UTF16Char progress_dialog_text[128];
	A_char projectName[AEGP_MAX_PROJ_NAME_SIZE]{};

	beamerParamsStruct *bps;
	A_char tmp_message[512];
	AeSceneContainer* sc;
	A_Boolean smart_collect;
};
#endif 
