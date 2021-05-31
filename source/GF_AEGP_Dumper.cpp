#include "GF_AEGP_Dumper.h"
#ifdef AE_OS_WIN
    #include "Socket/socket_win.h"
#else
    #include "Socket/socket_macos.h"
#endif
#include "Socket/gfs_rq_node_wrapper.h"

namespace RenderBeamer {

GF_Dumper::GF_Dumper(SPBasicSuite *basic_suite, AEGP_PluginID plugin_aegp_id)
	: rootProjH(nullptr)
	  , sP(basic_suite)
	  , suites(sP)
	  , pluginId(plugin_aegp_id)
	  , dumper_progressbar_(nullptr)
	  , relinker(basic_suite, plugin_aegp_id)
	  , progress_dialog_text{}
	  , bps(nullptr)
	  , tmp_message{}
	  , sc(nullptr)
	  , smart_collect(0)
{
	projectName[0] = '\0';
}

GF_Dumper::~GF_Dumper()
{
    A_Err err = A_Err_NONE;
    if(gfs_creator)
        delete gfs_creator;

    if (dumper_progressbar_)
        suites.AppSuite6()->PF_DisposeAppProgressDialog(dumper_progressbar_);
    
    if (bps && bps->original_project[0])
        ERR(suites.ProjSuite6()->AEGP_OpenProjectFromPath(bps->original_project, &rootProjH));
    
}

ErrorCodesAE GF_Dumper::PreCheckProject(SPBasicSuite *pb, AEGP_PluginID pluginId, beamerParamsStruct &globals_and_paths)
{
	AEGP_SuiteHandler suites(pb);
	ERROR_CATCH_START_MOD(PreCheckModule)
		A_Boolean projectDirty = TRUE;
		AEGP_ProjectH rootProjH = nullptr;
		AEGP_MemHandle memoryH = nullptr;
		A_long numberOfProjects = 0L;
		FS_ERROR_CODE(LogCreateError)

		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetNumProjects(&numberOfProjects))

		if (numberOfProjects < 1) { throw PluginError(_ErrorCaller, ProjectNotOpened); }

		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetProjectByIndex(0, &rootProjH))
		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_ProjectIsDirty(rootProjH, &projectDirty))
		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetProjectPath(rootProjH, &memoryH))
		ERROR_THROW_AE_MOD(rbUtilities::copyMemhUTF16ToString(pb, memoryH, globals_and_paths.projectPath))

		if (projectDirty == TRUE || globals_and_paths.projectPath.empty())
		{
			throw PluginError(_ErrorCaller, ProjectNotSaved);
		}
        globals_and_paths.bp.originalProject = fs::path(globals_and_paths.projectPath).lexically_normal();
		rbUtilities::toUTF16(globals_and_paths.projectPath.c_str(), globals_and_paths.original_project, AEGP_MAX_PATH_SIZE);
		globals_and_paths.bp.projectFilenameCorrect = fs::path(globals_and_paths.bp.originalProject.filename()).replace_extension(".aepx");
		rbUtilities::pathStringFixIllegal(globals_and_paths.bp.projectFilenameCorrect, false, false);
        rbUtilities::getTimeString(globals_and_paths.timeString, 20, true);
		rbUtilities::getEnvVariable(std::string(ENV_HOME_DIR), globals_and_paths.beamerScript);
		globals_and_paths.bp.tempLogPath = fs::path(globals_and_paths.beamerScript) / fs::path(".renderbeamer") / fs::path("log") / fs::path("aftereffects");
		globals_and_paths.beamerScript /= BEAMER_SCRIPT;
	
		if (!fs::exists(globals_and_paths.beamerScript)) {
			throw PluginError(_ErrorCaller, GetLocalBeamerPath);
		}
		fs::create_directories(globals_and_paths.bp.tempLogPath, LogCreateError);
		globals_and_paths.bp.tempLogFile = "Log_AE_renderBeamer_";
        globals_and_paths.bp.tempLogFile += fs::path(globals_and_paths.bp.projectFilenameCorrect.filename()).replace_extension();
		globals_and_paths.bp.tempLogFile += std::wstring(globals_and_paths.timeString) + L".txt";
		globals_and_paths.bp.tempLogPath /= globals_and_paths.bp.tempLogFile.c_str();
        globals_and_paths.bp.tempLogPath = globals_and_paths.bp.tempLogPath.lexically_normal();
		
		rbProj()->createLogger(globals_and_paths.bp.tempLogPath.wstring().c_str(), std::fstream::out | std::fstream::trunc);
		rbProj()->logg(L"PreChecker", L"Info", L"Pre check phase I success. Looking for active rq items");

		rbProj()->logg(L"PreChecker", L"TempLogPath", globals_and_paths.bp.tempLogPath.lexically_normal().wstring().c_str());
		rbProj()->logg(L"PreChecker", L"ProjectPath", globals_and_paths.projectPath.c_str());
		rbProj()->logg(L"PreChecker", L"FixedProjectPath", globals_and_paths.bp.projectFilenameCorrect.wstring().c_str());
	ERROR_CATCH_END_RETURN(suites)
}

ErrorCodesAE GF_Dumper::PrepareProject()
{
	ERROR_CATCH_START_MOD(PrePreparationModule)
		rbProj()->logg("PrepareProject", "Info", "Prepare project method start. ");
		AEGP_MemHandle memoryH = nullptr;
		FS_ERROR_CODE(fsError)		
		MAIN_PROGRESS(dumper_progressbar_, 4, 10)

		bps->beamerTmpFile = fs::temp_directory_path(fsError);
		if (fsError.value() != 0)
			throw PluginError(_ErrorCaller, GetLocalTempDirectory);
		bps->beamerTmpFile /= "beamerDataExchange.dat";
		
		rbProj()->logg(L"PrepareProject", L"DataExchange", bps->beamerTmpFile.wstring().c_str());
	
		ERROR_THROW_AE_MOD(suites.UtilitySuite6()->AEGP_GetPluginPaths(pluginId, AEGP_GetPathTypes_ALLUSER_PLUGIN, &memoryH))
		ERROR_THROW_AE_MOD(rbUtilities::copyMemhUTF16ToString(sP, memoryH, bps->pluginPath))
		if (bps->pluginPath.empty())
		{
			throw PluginError(_ErrorCaller, GetLocalUsersPlugin);
		}
        bps->fontLibPath = bps->pluginPath + std::wstring(FONT_LIB_NAME);
		bps->c4d_LibPath = bps->pluginPath + std::wstring(C4D_LIB_NAME);
		rbProj()->logg(L"renderBreamer", L"Library_C4D_Relinker", (std::wstring(fs::exists(bps->c4d_LibPath) ? L"OK : " : L"ERR : ") + bps->c4d_LibPath).c_str());
		rbProj()->logg(L"renderBreamer", L"Library_Font_Relinker", (std::wstring(fs::exists(bps->fontLibPath) ? L"OK : " : L"ERR : ") + bps->fontLibPath).c_str());
		
		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetProjectByIndex(0, &rootProjH))
		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetProjectName(rootProjH, &projectName[0]))
		ERROR_THROW_MOD(rbUtilities::execScript(sP, pluginId, "app.version", bps->versionStr, 32))

		MAIN_PROGRESS(dumper_progressbar_, 5, 10)
		bps->bp.projectRootCorrect = bps->bp.originalProject.filename().c_str();
		rbUtilities::pathStringFixIllegal(bps->bp.projectRootCorrect, false, true);

		if (rbUtilities::execBeamerCmd(bps, BeamerMask_GetUser, bps->rmtUser, 14) != NoError) {			
			throw PluginError(_ErrorCaller, BeamerGetUserName);
		}		
		rbProj()->logg(L"PrepareProject", L"User", bps->rmtUser);
	
		if (rbUtilities::execBeamerCmd(bps, BeamerMask_GetLocalPort, bps->socketPort, 14) != NoError) {
			WSTRNCPY(bps->socketPort, L"32784", 6)
		}
		MAIN_PROGRESS(dumper_progressbar_, 6, 10)
		bps->socketPort_long = wcstol(bps->socketPort, nullptr, 10);
		rbProj()->logg(L"PrepareProject", L"LocalPortWide", bps->socketPort);
		rbProj()->logg("PrepareProject", "LocalPortLong", std::to_string(bps->socketPort_long).c_str());
		if(bps->socketPort_long <= 0)
		{
			bps->socketPort_long = 32784;
			rbProj()->loggErr("PrepareProject", "LocalPort::ChangingToDefault", std::to_string(bps->socketPort_long).c_str());
		}
	
		if (rbUtilities::execBeamerCmd(bps, BeamerMask_GetTemp, bps->beamerTmpPath, AEGP_MAX_PATH_SIZE) != NoError) {
            bps->bp.tempPrefix = bps->bp.originalProject.parent_path().lexically_normal().c_str();
		}
		else {
            bps->bp.tempPrefix = bps->beamerTmpPath;
		}
		MAIN_PROGRESS(dumper_progressbar_, 7, 10)
		rbProj()->logg(L"PrepareProject", L"TmpPathPrefix", bps->bp.tempPrefix.wstring().c_str());
			
		bps->bp.tempSufix = MAIN_BEAMER_TEMP;
        bps->bp.tempSufix += bps->timeString;
		bps->bp.relinkedSceneRoot = bps->bp.tempPrefix / bps->bp.tempSufix / bps->bp.projectRootCorrect;
		rbProj()->logg(L"PrepareProject", L"relinkedSceneRoot", bps->bp.relinkedSceneRoot.wstring().c_str());
	
        bps->bp.relProjPath = bps->bp.relinkedSceneRoot.lexically_normal().c_str();
		bps->bp.relProjPath /= MAIN_PROJECT_DIR1;
		rbProj()->logg(L"PrepareProject", L"relinkedProjectRoot", bps->bp.relProjPath.wstring().c_str());
	
		bps->bp.remoteProjectPath = bps->bp.projectRootCorrect.c_str();
		bps->bp.remoteProjectPath /= MAIN_PROJECT_DIR1;
		bps->bp.remoteProjectPath /= bps->bp.projectFilenameCorrect.c_str();
		bps->bp.remoteProjectPath.replace_extension(".aepx");
	
		if (rbUtilities::execBeamerCmd(bps, BeamerMask_CheckScene, bps->beamerVersionFilename, AEGP_MAX_PATH_SIZE) == NoError) {
			bps->bp.projectFilenameCorrect = bps->beamerVersionFilename;
		}
        bps->bp.remoteProjectPath.remove_filename();
		bps->bp.remoteProjectPath /= bps->bp.projectFilenameCorrect.filename().c_str();

		bps->bp.remote_renders_path = bps->bp.projectRootCorrect.c_str();
		bps->bp.remote_renders_path /= bps->bp.projectRootCorrect.c_str();
		bps->bp.remote_renders_path += "-Renders";
		bps->bp.remoteFootagePath = "U:";
		bps->bp.remoteFootagePath += SEP;
		bps->bp.remoteFootagePath /= bps->rmtUser;
		bps->bp.remoteFootagePath /= bps->bp.projectRootCorrect.c_str();
		bps->bp.remoteFootagePath /= MAIN_PROJECT_DIR1;	
		bps->bp.remoteFootagePath /= MAIN_FOOTAGE_DIR1;
		bps->bp.remoteFontsPath = bps->bp.projectRootCorrect.c_str();
		bps->bp.remoteFontsPath /= MAIN_PROJECT_DIR1;
		bps->bp.remoteFontsPath /= MAIN_FOONT_DIR1;
		MAIN_PROGRESS(dumper_progressbar_, 8, 10)

		bps->bp.rqMainOutput = bps->bp.relinkedSceneRoot.lexically_normal().c_str();
		bps->bp.rqMainOutput /= bps->bp.projectRootCorrect.c_str();
		bps->bp.rqMainOutput += "-Renders";	

        bps->bp.footageMainOutput = bps->bp.relProjPath.lexically_normal().c_str();
		bps->bp.footageMainOutput /= MAIN_FOOTAGE_DIR1;
		bps->bp.fontsMainOutput = bps->bp.relProjPath.lexically_normal().c_str();
		bps->bp.fontsMainOutput /= MAIN_FOONT_DIR1;
        bps->bp.logsMainOutput = bps->bp.relProjPath.lexically_normal().c_str();
		bps->bp.logsMainOutput /= MAIN_LOGS_DIR1;
        bps->bp.relGfsFile = bps->bp.relProjPath.lexically_normal().c_str();
		bps->bp.relGfsFile /= fs::path(bps->bp.projectFilenameCorrect.filename()).replace_extension(".gfs");

        bps->bp.relinkedSavePath = bps->bp.relProjPath.lexically_normal().c_str();
		bps->bp.relinkedSavePath /= bps->bp.projectFilenameCorrect.filename().c_str();
        bps->bp.relinkedSavePath.replace_extension(".aepx");

		rbProj()->logg(L"PrepareProject", L"CreateDir", bps->bp.relProjPath.lexically_normal().wstring().c_str());
		fs::create_directories(bps->bp.relProjPath.lexically_normal());
		rbProj()->logg(L"PrepareProject", L"CreateDir", bps->bp.footageMainOutput.lexically_normal().wstring().c_str());
		fs::create_directories(bps->bp.footageMainOutput.lexically_normal());
		rbProj()->logg(L"PrepareProject", L"CreateDir", bps->bp.fontsMainOutput.lexically_normal().wstring().c_str());
		fs::create_directories(bps->bp.fontsMainOutput.lexically_normal());
		rbProj()->logg(L"PrepareProject", L"CreateDir", bps->bp.logsMainOutput.lexically_normal().wstring().c_str());
		fs::create_directories(bps->bp.logsMainOutput.lexically_normal());
		rbProj()->logg(L"PrepareProject", L"CreateDir", bps->bp.rqMainOutput.lexically_normal().wstring().c_str());
		fs::create_directories(bps->bp.rqMainOutput.lexically_normal());
		bps->bp.logsMainOutput /= bps->bp.tempLogFile.c_str();

		rbProj()->logg(L"PrepareProject", L"Info", L"Project paths prepared success.");
		rbUtilities::getVersionString(tmp_message, 512);
		rbProj()->logg("PrepareProject", "VersionString", tmp_message);
		MAIN_PROGRESS(dumper_progressbar_, 9, 10)
			
		rbProj()->logg("PrepareProject", "ProjectName", projectName);
		rbProj()->logg("PrepareProject", "SourcePath", bps->bp.originalProject.lexically_normal().string().c_str());
		rbProj()->logg("PrepareProject", "RelinkedPath", bps->bp.relProjPath.lexically_normal().string().c_str());
		rbProj()->logg("PrepareProject", "RelinkedSavePath", bps->bp.relinkedSavePath.lexically_normal().string().c_str());
		rbProj()->logg(L"PrepareProject", L"TmpPath", bps->beamerTmpPath);
	ERROR_CATCH_END_RETURN(suites)
}

ErrorCodesAE GF_Dumper::newBatchDumpProject()
{
	ERROR_CATCH_START_MOD(ProjectDumperModule)
	PF_AppProgressDialogP *dlg_ptr = get_progress_dialog(true, false, 1);
	MAIN_PROGRESS_THROW(*dlg_ptr, 0, 5)

	ERROR_THROW_2(relinker.RelinkerInitialize(bps, TRUE));
	gfs_creator = new AeGfsFileCreator();
	gfs_creator->InitGfsFileBuilder(bps);
	bps->currentItem = 0;
	bps->colectedItems = 5 + static_cast<A_long>(sc->fontsList.size());
	
	MAIN_PROGRESS_THROW(dumper_progressbar_, ++bps->currentItem, bps->colectedItems)
	ERROR_RETURN(DumpUiQueueItems(bps->bp.rqMainOutput))
	MAIN_PROGRESS_THROW(dumper_progressbar_, ++bps->currentItem, bps->colectedItems)
	ERROR_RETURN(newCopyCollectFonts())
	MAIN_PROGRESS_THROW(dumper_progressbar_, ++bps->currentItem, bps->colectedItems)
	ERROR_RETURN(newCollectEffectsInfo())
	MAIN_PROGRESS_THROW(dumper_progressbar_, ++bps->currentItem, bps->colectedItems)
	relinker.unloadFontLibrary();
	ERROR_THROW_2(gfs_creator->GenerateAndSaveDocument());
    MAIN_PROGRESS_THROW(dumper_progressbar_, ++bps->currentItem, bps->colectedItems)
	ERROR_THROW_2(relinker.RelinkProject(rootProjH));

	MAIN_PROGRESS_THROW(dumper_progressbar_, ++bps->currentItem, bps->colectedItems)
	dlg_ptr = get_progress_dialog(true, false, 2);
	
	AeBatchRelinker batchRelinker(sP, relinker.GetC4dLibloader(), rbProj(), dlg_ptr, bps->bp.relinkedSavePath);
	ERROR_THROW_2(batchRelinker.ParseAepxXmlDocument())
	ERROR_THROW_2(batchRelinker.CopyAndRelinkFiles(bps->bp.footageMainOutput, bps->bp.remoteFootagePath))
	rbProj()->logger.flush();
	FS_ERROR_CODE(LogCoppyError)
	fs::copy(bps->bp.tempLogPath, bps->bp.logsMainOutput, LogCoppyError);

	if (rbUtilities::execBeamerCmd(bps, BeamerMask_SendTaskEncoded) != NoError) {
		throw  PluginError(_ErrorCaller, BeamerSendTaskFailed);
	}
	rbProj()->logger.flush();
	rbProj()->logger.close();
	ERROR_CATCH_END_RETURN(suites)
}

ErrorCodesAE GF_Dumper::newCopyCollectFonts()
{
	ERROR_CATCH_START_MOD(CopyCollectFontsModule)
	
	if (sc->fontsList.empty())
		return NoError;
	A_long it = 0;
	for (auto *node : sc->fontsList)
	{
		rbProj()->loggA(6, "Processing font name", node->getFont(), "Family", node->getFamily(), "File", node->getLocation());
		std::vector<std::string> fontsContainer;
		MAIN_PROGRESS_THROW(dumper_progressbar_, ++bps->currentItem, bps->colectedItems)
		if (relinker.CopyFont(node, it++, fontsContainer) == NoError) 
		{
			for (unsigned long long i = 0; i < fontsContainer.size(); i++) 
			{				
                auto *gfsNode = new gfsFontNode({ i, std::string(node->getFont()), fontsContainer.at(i) });
				if(gfs_creator->PushFontNode(gfsNode) == NoError)
					rbProj()->loggA(6, "Collected font", std::to_string(i).c_str(), node->getFont(), node->getFamily(), node->getLocation(), FS_U8STRING(node->path.filename()).c_str());
			}
		}
		else {
			rbProj()->loggA(6, "ERROR collecting font name", node->getFont(), "Family", node->getFamily(), "File", node->getLocation());
		}
	}
	ERROR_CATCH_END_LOGGER_RETURN("FontsCollecting")
}
ErrorCodesAE GF_Dumper::newCollectEffectsInfo() const
{
	ERROR_CATCH_START_MOD(CollectEffectsModule)
	
	if (sc->effectsList.empty())
		return NoError;
	
	for (auto *node : sc->effectsList)
	{
		auto *gfsEffect = new gfsEffectNode({ node->getKey(), node->getEffectNameSafe(), node->getEffectMatchNSafe(), node->getEffectCategorySafe() });
		if(gfs_creator->PushEffectNode(gfsEffect) == NoError)
        {
			rbProj()->loggA(9, "EffectCollector", "Name:", node->getEffectName(), "MatchName:", node->getEffectMatchN(), "Category:", node->getEffectCategory(), "InstallKey:", std::to_string(static_cast<int>(node->getKey())).c_str());
        }
	}
	ERROR_CATCH_END_LOGGER_RETURN("EffectsCollecting")
}
ErrorCodesAE GF_Dumper::SetupUiQueueItems()
{
	ERROR_CATCH_START_MOD(QueueCollectModule)
	platform_socket connector;
	ERROR_LONG_ERR(connector.start_session(bps->socketPort_long, bps->bp.projectRootCorrect.string()))
	if (_ErrorCode == NoError)
	{
		std::string send_buffer;
		char read_buffer[49151];
		ERROR_AE(gfs_rq_node_wrapper::serialize(*sc, send_buffer))
		//rbProj()->logg("connector.write", "buffer", send_buffer.c_str());
		ERROR_AE(connector.write(send_buffer.c_str(), static_cast<unsigned long>(send_buffer.length())) > 0 ? NoError : ErrorResult)

		MAIN_PROGRESS_THROW(dumper_progressbar_, 10, 20)
		while (connector.is_connected() && _ErrorCode == NoError)
		{
			if (connector.read(read_buffer, 49150) > 0) {
				std::string data_read(read_buffer);
				rbProj()->logg("connector.read", "success", data_read.c_str());
				if(data_read == std::string("QUIT\n") || data_read == std::string("DATA=QUIT\n"))
				{
					_ErrorCode = UserDialogCancel;
                    break;
				}
				if(gfs_rq_node_wrapper::deserialize(*sc, data_read) != NoError)
                {
                    rbProj()->logg("SetupUiQueueItems", "Failed", "There was an error with data serialization. Continuing with defaults.");
                }
				break;
			}
			MAIN_PROGRESS_THROW(dumper_progressbar_, 11, 20)
		}
	}
	if(_ErrorCode == NoError) this->smart_collect = sc->smart_collect;
	ERROR_CATCH_END_LOGGER_RETURN("SetupUiQueueItems")
}
ErrorCodesAE GF_Dumper::DumpUiQueueItems(const fs::path& outputPath) const
{
	ERROR_CATCH_START_MOD(QueueCollectModule)
		while (!sc->gfsRqItemsList.empty())
		{
			if (sc->gfsRqItemsList.back()->renderable != 0) {
				gfs_creator->PushRenderQueueItem(sc->gfsRqItemsList.back());
			}
			else {
				delete sc->gfsRqItemsList.back();
			}
			sc->gfsRqItemsList.pop_back();
		}
	gfs_creator->ignore_missings_assets = sc->ignore_missings_assets;
	gfs_creator->smart_collect = sc->smart_collect;
	ERROR_CATCH_END_LOGGER_RETURN("DumpUiQueueItems")
}

ErrorCodesAE GF_Dumper::setConteiner(AeSceneContainer &aesc)
{
	this->sc = &aesc;	
	return NoError;
}

ErrorCodesAE GF_Dumper::setPathsStruct(beamerParamsStruct &globals_and_paths)
{
	bps = &globals_and_paths;
	return NoError;
}
PF_AppProgressDialogP *GF_Dumper::get_progress_dialog(bool force_new, bool indeterminate, int dialog_text_nr)
{
	PF_Err Err = PF_Err_NONE;
	if(force_new && dumper_progressbar_) {
		suites.AppSuite6()->PF_DisposeAppProgressDialog(dumper_progressbar_);
		dumper_progressbar_ = nullptr;
	}
	if (!dumper_progressbar_) {
		if (dialog_text_nr == 0){
			rbUtilities::toUTF16(L"Pre-checking environment, render queue items and beamer...", progress_dialog_text, 128);
		}
		else if (dialog_text_nr == 1) {
			rbUtilities::toUTF16(L"Collecting scene info, assets and other data...", progress_dialog_text, 128);
		}
		else{
			rbUtilities::toUTF16(L"Relinking scene and copying asset files...", progress_dialog_text, 128);
		}
		Err = suites.AppSuite6()->PF_CreateNewAppProgressDialog(progress_dialog_text, nullptr, (indeterminate ? TRUE : FALSE), &dumper_progressbar_);
	}

	return Err == PF_Err_NONE ? &dumper_progressbar_ : nullptr;
}

}
