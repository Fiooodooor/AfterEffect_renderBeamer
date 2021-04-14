#include "GF_AEGP_Dumper.h"



GF_Dumper::GF_Dumper(SPBasicSuite *basicSuite, AEGP_PluginID pI, beamerParamsStruct *GF_params)
    : rootProjH(nullptr)
	, sP(basicSuite)
    , suites(sP)       
	, pluginId(pI)
	, relinkerProg(nullptr)
	, relinker(basicSuite, pI)
    , bps(*GF_params)
	, tmp_message("")
	, sc(nullptr)
	, smart_collect(0)
{
    projectName[0] = '\0';
}

GF_Dumper::~GF_Dumper()
{
    A_Err err = A_Err_NONE;

    if (relinkerProg)
        suites.AppSuite6()->PF_DisposeAppProgressDialog(relinkerProg);
	
    if (smart_collect != 0) {       
        ERR(suites.ProjSuite6()->AEGP_OpenProjectFromPath(bps.original_project, &rootProjH));
    }
}

ErrorCodesAE GF_Dumper::PreCheckProject(SPBasicSuite *pb, AEGP_PluginID pluginId, beamerParamsStruct *GF_params)
{
	AEGP_SuiteHandler suites(pb);
	ERROR_CATCH_START_MOD(CallerModuleName::PreCheckModule)
		A_Boolean projectDirty = TRUE;
		AEGP_ProjectH rootProjH = nullptr;
		AEGP_MemHandle memoryH = nullptr;
		A_long numberOfProjects = 0L;
		FS_ERROR_CODE(LogCreateError)

		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetNumProjects(&numberOfProjects))

		if (numberOfProjects < 1) { throw PluginError(_ErrorCaller, ErrorCodesAE::ProjectNotOpened); }

		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetProjectByIndex(0, &rootProjH))
		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_ProjectIsDirty(rootProjH, &projectDirty))
		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetProjectPath(rootProjH, &memoryH))
		ERROR_THROW_AE_MOD(rbUtilities::copyMemhUTF16ToString(pb, memoryH, GF_params->projectPath))

		if (projectDirty == TRUE || GF_params->projectPath.empty())
		{
			throw PluginError(_ErrorCaller, ProjectNotSaved);
		}
        GF_params->bp.originalProject = fs::path(GF_params->projectPath).lexically_normal();
		rbUtilities::toUTF16(GF_params->projectPath.c_str(), GF_params->original_project, AEGP_MAX_PATH_SIZE);
		GF_params->bp.projectFilenameCorrect = fs::path(GF_params->bp.originalProject.filename()).replace_extension(".aepx");
		rbUtilities::pathStringFixIllegal(GF_params->bp.projectFilenameCorrect, false, false);
        rbUtilities::getTimeString(GF_params->timeString, 20, true);	       
		rbUtilities::getEnvVariable(std::string(ENV_HOME_DIR), GF_params->beamerScript);
		GF_params->bp.tempLogPath = fs::path(GF_params->beamerScript) / fs::path(".renderbeamer") / fs::path("log") / fs::path("aftereffects");
		GF_params->beamerScript /= BEAMER_SCRIPT;
	
		if (!fs::exists(GF_params->beamerScript)) {
			throw PluginError(_ErrorCaller, ErrorCodesAE::GetLocalBeamerPath);
		}
		fs::create_directories(GF_params->bp.tempLogPath, LogCreateError);
		GF_params->bp.tempLogFile = "Log_AE_renderBeamer_";
        GF_params->bp.tempLogFile += fs::path(GF_params->bp.projectFilenameCorrect.filename()).replace_extension();
		GF_params->bp.tempLogFile += std::wstring(GF_params->timeString) + L".txt";
		GF_params->bp.tempLogPath /= GF_params->bp.tempLogFile.c_str();
        GF_params->bp.tempLogPath = GF_params->bp.tempLogPath.lexically_normal();
		
		rbProj()->createLogger(GF_params->bp.tempLogPath.wstring().c_str(), std::fstream::out | std::fstream::trunc);
		rbProj()->logg(L"PreChecker", L"Info", L"Pre check phase I success. Looking for active rq items");

		rbProj()->logg(L"PreChecker", L"TempLogPath", GF_params->bp.tempLogPath.lexically_normal().wstring().c_str());
		rbProj()->logg(L"PreChecker", L"ProjectPath", GF_params->projectPath.c_str());
		rbProj()->logg(L"PreChecker", L"FixedProjectPath", GF_params->bp.projectFilenameCorrect.wstring().c_str());
	ERROR_CATCH_END_RETURN(suites)
}

ErrorCodesAE GF_Dumper::PrepareProject()
{
	ERROR_CATCH_START_MOD(CallerModuleName::PrePreparationModule)
		rbProj()->logg("PrepareProject", "Info", "Prepare project method start. ");
		AEGP_MemHandle memoryH = nullptr;
		FS_ERROR_CODE(fsError)

		rbUtilities::toUTF16(L"Prepating project and collecting data...", dialogText, 128);
		suites.AppSuite6()->PF_CreateNewAppProgressDialog(dialogText, nullptr, TRUE, &relinkerProg);
		GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, 1, 100))

		bps.beamerTmpFile = fs::temp_directory_path(fsError);
		if (fsError.value() != 0)
			throw PluginError(_ErrorCaller, GetLocalTempDirectory);
		bps.beamerTmpFile /= "beamerDataExchange.dat";
		
		rbProj()->logg(L"PrepareProject", L"DataExchange", bps.beamerTmpFile.wstring().c_str());
	
		ERROR_THROW_AE_MOD(suites.UtilitySuite6()->AEGP_GetPluginPaths(pluginId, AEGP_GetPathTypes_ALLUSER_PLUGIN, &memoryH))
		ERROR_THROW_AE_MOD(rbUtilities::copyMemhUTF16ToString(sP, memoryH, bps.pluginPath))
		if (bps.pluginPath.empty())
		{
			throw PluginError(_ErrorCaller, GetLocalUsersPlugin);
		}
        bps.fontLibPath = bps.pluginPath + std::wstring(FONT_LIB_NAME);
		bps.c4d_LibPath = bps.pluginPath + std::wstring(C4D_LIB_NAME);
		rbProj()->logg(L"renderBreamer", L"Library_C4D_Relinker", (std::wstring(fs::exists(bps.c4d_LibPath) ? L"OK : " : L"ERR : ") + bps.c4d_LibPath).c_str());
		rbProj()->logg(L"renderBreamer", L"Library_Font_Relinker", (std::wstring(fs::exists(bps.fontLibPath) ? L"OK : " : L"ERR : ") + bps.fontLibPath).c_str());
		
		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetProjectByIndex(0, &rootProjH))
		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetProjectName(rootProjH, &projectName[0]))
		ERROR_THROW_AE_MOD(rbUtilities::execScript(sP, pluginId, "app.version", bps.versionStr, 32))

		bps.bp.projectRootCorrect = bps.bp.originalProject.filename().c_str();
		rbUtilities::pathStringFixIllegal(bps.bp.projectRootCorrect, false, true);

		if (rbUtilities::execBeamerCmd(bps, BeamerMask_GetUser, bps.rmtUser, 14) != NoError) {			
			throw PluginError(_ErrorCaller, BeamerGetUserName);
		}
	
		rbProj()->logg(L"PrepareProject", L"User", bps.rmtUser);
	
		if (rbUtilities::execBeamerCmd(bps, BeamerMask_GetTemp, bps.beamerTmpPath, AEGP_MAX_PATH_SIZE) != NoError) {
            bps.bp.tempPrefix = bps.bp.originalProject.parent_path().lexically_normal().c_str();
		}
		else {
            bps.bp.tempPrefix = bps.beamerTmpPath;
		}
	
		rbProj()->logg(L"PrepareProject", L"TmpPathPrefix", bps.bp.tempPrefix.wstring().c_str());
			
		bps.bp.tempSufix = MAIN_BEAMER_TEMP;
        bps.bp.tempSufix += bps.timeString;
		bps.bp.relinkedSceneRoot = bps.bp.tempPrefix / bps.bp.tempSufix / bps.bp.projectRootCorrect;
		rbProj()->logg(L"PrepareProject", L"relinkedSceneRoot", bps.bp.relinkedSceneRoot.wstring().c_str());
	
        bps.bp.relProjPath = bps.bp.relinkedSceneRoot.lexically_normal().c_str();
		bps.bp.relProjPath /= MAIN_PROJECT_DIR1;
		rbProj()->logg(L"PrepareProject", L"relinkedProjectRoot", bps.bp.relProjPath.wstring().c_str());
	
		bps.bp.remoteProjectPath = bps.bp.projectRootCorrect.c_str();
		bps.bp.remoteProjectPath /= MAIN_PROJECT_DIR1;
		bps.bp.remoteProjectPath /= bps.bp.projectFilenameCorrect.c_str();
		bps.bp.remoteProjectPath.replace_extension(".aepx");
	
		if (rbUtilities::execBeamerCmd(bps, BeamerMask_CheckScene, bps.beamerVersionFilename, AEGP_MAX_PATH_SIZE) == NoError) {
			bps.bp.projectFilenameCorrect = bps.beamerVersionFilename;
		}
        bps.bp.remoteProjectPath.remove_filename();
		bps.bp.remoteProjectPath /= bps.bp.projectFilenameCorrect.filename().c_str();

		bps.bp.remote_renders_path = bps.bp.projectRootCorrect.c_str();
		bps.bp.remote_renders_path /= bps.bp.projectRootCorrect.c_str();
		bps.bp.remote_renders_path += "-Renders";
		bps.bp.remoteFootagePath = "U:";
		bps.bp.remoteFootagePath /= bps.rmtUser;
		bps.bp.remoteFootagePath /= bps.bp.projectRootCorrect.c_str();
		bps.bp.remoteFootagePath /= MAIN_PROJECT_DIR1;	
		bps.bp.remoteFootagePath /= MAIN_FOOTAGE_DIR1;
		bps.bp.remoteFontsPath = bps.bp.projectRootCorrect.c_str();
		bps.bp.remoteFontsPath /= MAIN_PROJECT_DIR1;
		bps.bp.remoteFontsPath /= MAIN_FOONT_DIR1;

		bps.bp.rqMainOutput = bps.bp.relinkedSceneRoot.lexically_normal().c_str();
		bps.bp.rqMainOutput /= bps.bp.projectRootCorrect.c_str();
		bps.bp.rqMainOutput += "-Renders";	

        bps.bp.footageMainOutput = bps.bp.relProjPath.lexically_normal().c_str();
		bps.bp.footageMainOutput /= MAIN_FOOTAGE_DIR1;
		bps.bp.fontsMainOutput = bps.bp.relProjPath.lexically_normal().c_str();
		bps.bp.fontsMainOutput /= MAIN_FOONT_DIR1;
        bps.bp.logsMainOutput = bps.bp.relProjPath.lexically_normal().c_str();
		bps.bp.logsMainOutput /= MAIN_LOGS_DIR1;
        bps.bp.relGfsFile = bps.bp.relProjPath.lexically_normal().c_str();
		bps.bp.relGfsFile /= fs::path(bps.bp.projectFilenameCorrect.filename()).replace_extension(".gfs");

        bps.bp.relinkedSavePath = bps.bp.relProjPath.lexically_normal().c_str();
		bps.bp.relinkedSavePath /= bps.bp.projectFilenameCorrect.filename().c_str();
        bps.bp.relinkedSavePath.replace_extension(".aepx");

		rbProj()->logg(L"PrepareProject", L"CreateDir", bps.bp.relProjPath.lexically_normal().wstring().c_str());
		fs::create_directories(bps.bp.relProjPath.lexically_normal());
		rbProj()->logg(L"PrepareProject", L"CreateDir", bps.bp.footageMainOutput.lexically_normal().wstring().c_str());
		fs::create_directories(bps.bp.footageMainOutput.lexically_normal());
		rbProj()->logg(L"PrepareProject", L"CreateDir", bps.bp.fontsMainOutput.lexically_normal().wstring().c_str());
		fs::create_directories(bps.bp.fontsMainOutput.lexically_normal());
		rbProj()->logg(L"PrepareProject", L"CreateDir", bps.bp.logsMainOutput.lexically_normal().wstring().c_str());
		fs::create_directories(bps.bp.logsMainOutput.lexically_normal());
		rbProj()->logg(L"PrepareProject", L"CreateDir", bps.bp.rqMainOutput.lexically_normal().wstring().c_str());
		fs::create_directories(bps.bp.rqMainOutput.lexically_normal());
		bps.bp.logsMainOutput /= bps.bp.tempLogFile.c_str();

		rbProj()->logg(L"PrepareProject", L"Info", L"Project paths prepared success.");
		rbUtilities::getVersionString(tmp_message, 512);
		rbProj()->logg("PrepareProject", "VersionString", tmp_message);

		ERROR_THROW_AE_MOD(AeGfsFileCreator::getInstance()->InitGfsFileBuilder(bps))
			
		rbProj()->logg("PrepareProject", "ProjectName", projectName);
		rbProj()->logg(L"PrepareProject", L"SourcePath", bps.bp.originalProject.lexically_normal().wstring().c_str());
		rbProj()->logg(L"PrepareProject", L"RelinkedPath", bps.bp.relProjPath.lexically_normal().wstring().c_str());
		rbProj()->logg(L"PrepareProject", L"RelinkedSavePath", bps.bp.relinkedSavePath.lexically_normal().wstring().c_str());
		rbProj()->logg(L"PrepareProject", L"TmpPath", bps.beamerTmpPath);

		suites.AppSuite6()->PF_DisposeAppProgressDialog(relinkerProg);
		rbUtilities::toUTF16(L"Collecting scene info, assets and other data...", dialogText, 128);
		ERROR_THROW_AE_MOD(suites.AppSuite6()->PF_CreateNewAppProgressDialog(dialogText, nullptr, FALSE, &relinkerProg))
		GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, 0, 5))
	ERROR_CATCH_END_RETURN(suites)
}

ErrorCodesAE GF_Dumper::newBatchDumpProject()
{
	ERROR_CATCH_START_MOD(CallerModuleName::ProjectDumperModule)

	relinker.RelinkerInitialize(bps, TRUE);
	bps.currentItem = 0;
	bps.colectedItems = 5 + static_cast<A_long>(sc->fontsList.size());
	
	GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, ++bps.currentItem, bps.colectedItems))
	ERROR_RETURN(DumpUiQueueItems(bps.bp.rqMainOutput))
	GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, ++bps.currentItem, bps.colectedItems))
	ERROR_RETURN(newCopyCollectFonts())
	GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, ++bps.currentItem, bps.colectedItems))
	ERROR_RETURN(newCollectEffectsInfo())
	GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, ++bps.currentItem, bps.colectedItems))
	
	AeGfsFileCreator::getInstance()->GenerateAndSaveDocument();
	relinker.RelinkProject(rootProjH);

	while(!sc->renderFootageSortedList.empty())
	{
		delete sc->renderFootageSortedList.back();
		sc->renderFootageSortedList.pop_back();
	}
	GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, ++bps.currentItem, bps.colectedItems))
	suites.AppSuite6()->PF_DisposeAppProgressDialog(relinkerProg);
	
	rbUtilities::toUTF16(L"Relinking scene and copying asset files...", dialogText, 128);
	ERROR_THROW_AE_MOD(suites.AppSuite6()->PF_CreateNewAppProgressDialog(dialogText, nullptr, FALSE, &relinkerProg))
	
	AeBatchRelinker batchRelinker(sP, relinker.GetC4dLibloader(), *rbProj(), relinkerProg, bps.bp.relinkedSavePath, bps.bp.remoteFootagePath.parent_path());
	ERROR_RETURN(batchRelinker.ParseAepxXmlDocument())
	ERROR_RETURN(batchRelinker.CopyAndRelinkFiles(bps.bp.footageMainOutput, bps.bp.remoteFootagePath))
		
	if (rbUtilities::execBeamerCmd(bps, BeamerMask_SendTaskEncoded) != NoError) {
		throw  PluginError(_ErrorCaller, BeamerSendTaskFailed);
	}
	rbProj()->logger.flush();
	rbProj()->logger.close();

	FS_ERROR_CODE(LogCoppyError)
	fs::copy(bps.bp.tempLogPath, bps.bp.logsMainOutput, LogCoppyError);
	ERROR_CATCH_END_RETURN(suites)
}

ErrorCodesAE GF_Dumper::newCopyCollectFonts()
{
	ERROR_CATCH_START_MOD(CallerModuleName::CopyCollectFontsModule)
	
	if (sc->fontsList.empty())
		return NoError;
	
	for (auto *node : sc->fontsList)
	{
		rbProj()->loggA(6, "Processing font name", node->getFont(), "Family", node->getFamily(), "File", node->getLocation());
		A_long it = 0;
		std::vector<std::string> fontsContainer;
		GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, ++bps.currentItem, bps.colectedItems))
		if (relinker.CopyFont(node, it++, fontsContainer) == NoError) 
		{
			for (unsigned long long i = 0; i < fontsContainer.size(); i++) 
			{				
                auto *gfsNode = new gfsFontNode({ i, std::string(node->getFont()), fontsContainer.at(i) });
				if(AeGfsFileCreator::getInstance()->PushFontNode(gfsNode) == NoError)
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
	ERROR_CATCH_START_MOD(CallerModuleName::CollectEffectsModule)
	
	if (sc->effectsList.empty())
		return NoError;
	
	for (auto *node : sc->effectsList)
	{
		auto *gfsEffect = new gfsEffectNode({ node->getKey(), node->getEffectName(), node->getEffectMatchN(), node->getEffectCategory() });
		if(AeGfsFileCreator::getInstance()->PushEffectNode(gfsEffect) == NoError)
        {
			rbProj()->loggA(9, "EffectCollector", "Name:", node->getEffectName(), "MatchName:", node->getEffectMatchN(), "Category:", node->getEffectCategory(), "InstallKey:", std::to_string(static_cast<int>(node->getKey())).c_str());
        }
	}
	ERROR_CATCH_END_LOGGER_RETURN("EffectsCollecting")
}

ErrorCodesAE GF_Dumper::DumpUiQueueItems(const fs::path& outputPath) const
{
	ERROR_CATCH_START_MOD(CallerModuleName::QueueCollectModule)
		while (!sc->gfsRqItemsList.empty())
		{	
			AeGfsFileCreator::getInstance()->PushRenderQueueItem(sc->gfsRqItemsList.back());
			sc->gfsRqItemsList.pop_back();
		}
	ERROR_CATCH_END_LOGGER_RETURN("DumpUiQueueItems")
}

ErrorCodesAE GF_Dumper::setConteiner(AeSceneConteiner &aesc)
{
	this->sc = &aesc;
	if (!aesc.gfsRqItemsList.empty())
		this->smart_collect = aesc.gfsRqItemsList.front()->smart_collect;
	return NoError;
}
