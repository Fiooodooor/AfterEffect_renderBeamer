#include "GF_AEGP_Dumper.h"



GF_Dumper::GF_Dumper(SPBasicSuite *basicSuite, AEGP_PluginID pI, beamerParamsStruct *GF_params)
    : sP(basicSuite)
    , suites(sP)
    , pluginId(pI)
    , relinkerProg(NULL)
	, rootProjH(NULL)
	, relinker(basicSuite, pI)
    , bps(*GF_params)
{
    projectName[0] = '\0';
}

GF_Dumper::~GF_Dumper()
{
    A_Err err = A_Err_NONE;
    if (rbProj()->logger.is_open()) {
        rbProj()->logger.flush();
        rbProj()->logger.close();
    }
    if (relinkerProg)
        suites.AppSuite6()->PF_DisposeAppProgressDialog(relinkerProg);

	//reinterpret_cast<const A_UTF16Char*>(bps.bp.originalProject.u16string().c_str());
    if (fs::exists(bps.bp.originalProject)) {
        A_UTF16Char OryginalProject[AEGP_MAX_PATH_SIZE];
        rbUtilities::toUTF16(bps.bp.originalProject.wstring().c_str(), OryginalProject, AEGP_MAX_PATH_SIZE);
        ERR(suites.ProjSuite6()->AEGP_OpenProjectFromPath(OryginalProject, &rootProjH));
    }
}

ErrorCodesAE GF_Dumper::PreCheckProject(SPBasicSuite *pb, AEGP_PluginID pluginId, beamerParamsStruct *GF_params)
{
	AEGP_SuiteHandler suites(pb);
	ERROR_CATCH_START_MOD(CallerModuleName::PreCheckModule)
		A_Boolean projectDirty = TRUE;
		AEGP_ProjectH rootProjH = NULL;
		AEGP_MemHandle memoryH = NULL;
		AEGP_RQItemRefH currentItem = NULL;
		AEGP_RenderItemStatusType renderState = AEGP_RenderItemStatus_NONE;
		A_Boolean queed = FALSE;
		A_long i = 0L, rqItems = 0L, outModules = 0L, numberOfProjects = 0L;
		FS_ERROR_CODE(LogCreateError);

		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetNumProjects(&numberOfProjects));

		if (numberOfProjects < 1) { throw PluginError(_ErrorCaller, ErrorCodesAE::ProjectNotOpened); }

		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetProjectByIndex(0, &rootProjH));
		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_ProjectIsDirty(rootProjH, &projectDirty));
		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetProjectPath(rootProjH, &memoryH));
		ERROR_THROW_AE_MOD(rbUtilities::copyMemhUTF16ToString(pb, memoryH, GF_params->projectPath));

		if (projectDirty == TRUE || GF_params->projectPath.empty())
		{
			throw PluginError(_ErrorCaller, ErrorCodesAE::ProjectNotSaved);
		}
        GF_params->bp.originalProject = fs::path(GF_params->projectPath).lexically_normal();
		GF_params->bp.projectFilenameCorrect = GF_params->bp.originalProject.filename();
		rbUtilities::pathStringFixIllegal(GF_params->bp.projectFilenameCorrect, false, false);
        rbUtilities::getTimeString(GF_params->timeString, 20, true);
    
	#ifdef AE_OS_WIN        
		rbUtilities::getEnvVariable("USERPROFILE", GF_params->userPath, AEGP_MAX_PATH_SIZE);		
	#elif defined AE_OS_MAC
		strcpy(GF_params->userPath, getenv("HOME"));
	#endif
		RB_SWPRINTF(GF_params->beamerScript, AEGP_MAX_PATH_SIZE, L"%hs%ls", GF_params->userPath, BEAMER_SCRIPT);

		if (GF_params->userPath[0] == '\0' || !fs::exists(GF_params->beamerScript)) {
			throw PluginError(_ErrorCaller, ErrorCodesAE::GetLocalBeamerPath);
		}
		GF_params->bp.tempLogPath = fs::path(GF_params->userPath) / fs::path(".renderbeamer") / fs::path("log") / fs::path("aftereffects");

		fs::create_directories(GF_params->bp.tempLogPath, LogCreateError);
		GF_params->bp.tempLogFile = "Log_AE_renderBeamer_";
        GF_params->bp.tempLogFile += fs::path(GF_params->bp.projectFilenameCorrect.filename()).replace_extension();
		GF_params->bp.tempLogFile += std::wstring(GF_params->timeString) + L".txt";
		GF_params->bp.tempLogPath /= GF_params->bp.tempLogFile.c_str();
        GF_params->bp.tempLogPath = GF_params->bp.tempLogPath.lexically_normal();
		
		GF_Dumper::rbProj()->createLogger(GF_params->bp.tempLogPath.wstring().c_str(), std::fstream::out | std::fstream::trunc);
		GF_Dumper::rbProj()->logg(L"PreChecker", L"Info", L"Pre check phase I success. Looking for active rq items");

		ERROR_THROW_AE_MOD(suites.RQItemSuite3()->AEGP_GetNumRQItems(&rqItems));
		while (i < rqItems) {
			ERROR_THROW_AE_MOD(suites.RQItemSuite3()->AEGP_GetRQItemByIndex(i, &currentItem));
			ERROR_THROW_AE_MOD(suites.RQItemSuite3()->AEGP_GetRenderState(currentItem, &renderState));
			if (renderState == AEGP_RenderItemStatus_QUEUED) {
				queed = TRUE;
				ERROR_THROW_AE_MOD(suites.RQItemSuite3()->AEGP_GetNumOutputModulesForRQItem(currentItem, &outModules));
				if (outModules > 0)
					break;
			}
			i++;
		}
		GF_Dumper::rbProj()->logg(L"PreChecker", L"TempLogPath", GF_params->bp.tempLogPath.lexically_normal().wstring().c_str());
		GF_Dumper::rbProj()->logg(L"PreChecker", L"ProjectPath", GF_params->projectPath.c_str());
		GF_Dumper::rbProj()->logg(L"PreChecker", L"FixedProjectPath", GF_params->bp.projectFilenameCorrect.wstring().c_str());
		GF_Dumper::rbProj()->logg(L"PreChecker", L"Info", (( rqItems != 0 && rqItems != i) ? L"All ok. Got valid rqItem" : L"No valid rqItems"));
		if (rqItems == 0) { throw PluginError(_ErrorCaller, ErrorCodesAE::NoValidRqItems); }
		else if (i == rqItems && queed == TRUE) { throw PluginError(_ErrorCaller, ErrorCodesAE::NoRqItemOutputs); }
		else if (i == rqItems && queed == FALSE) { throw PluginError(_ErrorCaller, ErrorCodesAE::NoRqItemsQueed); }
	ERROR_CATCH_END_RETURN(suites)
}

ErrorCodesAE GF_Dumper::PrepareProject()
{
	ERROR_CATCH_START_MOD(CallerModuleName::PrePreparationModule)
		GF_Dumper::rbProj()->logg("PrepareProject", "Info", "Prepare project method start. ");
		AEGP_MemHandle memoryH = NULL;
		FS_ERROR_CODE(fsError);

		rbUtilities::toUTF16(L"Prepating project and collecting data...", dialogText, 128);
		suites.AppSuite6()->PF_CreateNewAppProgressDialog(dialogText, NULL, TRUE, &relinkerProg);
		GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, 1, 100));

		bps.beamerTmpFile = fs::temp_directory_path(fsError);
		bps.beamerTmpFile /= "beamerDataExchange.dat";
		if (fsError.value() != 0)
		{
			throw PluginError(_ErrorCaller, GetLocalTempDirectory);
		}
		rbProj()->logg(L"PrepareProject", L"DataExchange", bps.beamerTmpFile.wstring().c_str());
	
		ERROR_THROW_AE_MOD(suites.UtilitySuite6()->AEGP_GetPluginPaths(pluginId, AEGP_GetPathTypes_ALLUSER_PLUGIN, &memoryH));
		ERROR_THROW_AE_MOD(rbUtilities::copyMemhUTF16ToString(sP, memoryH, bps.pluginPath));
		if (bps.pluginPath.empty())
		{
			throw PluginError(_ErrorCaller, GetLocalUsersPlugin);
		}
        bps.fontLibPath = bps.pluginPath + std::wstring(FONT_LIB_NAME);
		bps.c4d_LibPath = bps.pluginPath + std::wstring(C4D_LIB_NAME);
		rbProj()->logg(L"renderBreamer", L"Library_C4D_Relinker", (std::wstring(fs::exists(bps.c4d_LibPath) ? L"OK : " : L"ERR : ") + bps.c4d_LibPath).c_str());
		rbProj()->logg(L"renderBreamer", L"Library_Font_Relinker", (std::wstring(fs::exists(bps.fontLibPath) ? L"OK : " : L"ERR : ") + bps.fontLibPath).c_str());
		
		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetProjectByIndex(0, &rootProjH));
		ERROR_THROW_AE_MOD(suites.ProjSuite6()->AEGP_GetProjectName(rootProjH, &projectName[0]));
		ERROR_THROW_AE_MOD(rbUtilities::execScript(sP, pluginId, "app.version", bps.versionStr, 32));

		bps.bp.projectRootCorrect = bps.bp.originalProject.filename().c_str();
		rbUtilities::pathStringFixIllegal(bps.bp.projectRootCorrect, false, true);

		if (rbUtilities::execBeamerCmd(sP, BeamerMask_GetUser, bps.rmtUser, 14, bps) != NoError) {			
			throw PluginError(_ErrorCaller, BeamerGetUserName);
		}
	
		rbProj()->logg(L"PrepareProject", L"User", bps.rmtUser);
	
		if (rbUtilities::execBeamerCmd(sP, BeamerMask_GetTemp, bps.beamerTmpPath, AEGP_MAX_PATH_SIZE, bps) != NoError) {
            bps.bp.tempPrefix = bps.bp.originalProject.parent_path().lexically_normal().c_str();
		}
		else {
            bps.bp.tempPrefix = bps.beamerTmpPath;
		}
	
		rbProj()->logg(L"PrepareProject", L"TmpPathPrefix", bps.bp.tempPrefix.wstring().c_str());
			
		bps.bp.tempSufix = MAIN_BEAMER_TEMP;
        bps.bp.tempSufix += bps.timeString;
		bps.bp.relinkedSceneRoot = bps.bp.tempPrefix / bps.bp.tempSufix / bps.bp.projectRootCorrect.c_str();
    
		bps.bp.relProjPath = bps.bp.relinkedSceneRoot.lexically_normal().c_str();
		bps.bp.relProjPath /= MAIN_PROJECT_DIR1;

		bps.bp.projectCheckVersion = bps.bp.projectRootCorrect.c_str();
		bps.bp.projectCheckVersion /= MAIN_PROJECT_DIR1;
		bps.bp.projectCheckVersion /= bps.bp.projectFilenameCorrect.c_str();

		if (rbUtilities::execBeamerCmd(sP, BeamerMask_CheckScene, bps.beamerVersionFilename, AEGP_MAX_PATH_SIZE, bps) == NoError) {
			bps.bp.projectFilenameCorrect = bps.beamerVersionFilename;
		}

		bps.bp.remotePath = bps.bp.projectRootCorrect.c_str();
		bps.bp.remotePath /= bps.bp.projectRootCorrect.c_str();
		bps.bp.remotePath += "-renders";
		bps.bp.remoteFootagePath = "U:";
		bps.bp.remoteFootagePath /= bps.rmtUser;
		bps.bp.remoteFootagePath /= bps.bp.projectRootCorrect.c_str();
		bps.bp.remoteFootagePath /= MAIN_PROJECT_DIR1;	
		bps.bp.remoteFootagePath /= MAIN_FOOTAGE_DIR1;
		bps.bp.remoteFontsPath = bps.bp.projectRootCorrect.c_str();
		bps.bp.remoteFontsPath /= MAIN_PROJECT_DIR1;
		bps.bp.remoteFontsPath /= MAIN_FOONT_DIR1;

		bps.bp.remoteProjectPath = bps.bp.projectRootCorrect.c_str();
		bps.bp.remoteProjectPath /= MAIN_PROJECT_DIR1;
		bps.bp.remoteProjectPath /= bps.bp.projectFilenameCorrect;

		bps.bp.rqMainOutput = bps.bp.relinkedSceneRoot.lexically_normal().c_str();

        bps.bp.footageMainOutput = bps.bp.relProjPath.lexically_normal();
		bps.bp.footageMainOutput /= MAIN_FOOTAGE_DIR1;
		bps.bp.fontsMainOutput = bps.bp.relProjPath.lexically_normal();
		bps.bp.fontsMainOutput /= MAIN_FOONT_DIR1;
        bps.bp.logsMainOutput = bps.bp.relProjPath.lexically_normal();
		bps.bp.logsMainOutput /= MAIN_LOGS_DIR1;
        bps.bp.relGfsFile = bps.bp.relProjPath.lexically_normal().c_str();
		bps.bp.relGfsFile /= fs::path(bps.bp.projectFilenameCorrect.filename()).replace_extension(".gfs");

        bps.bp.relinkedSavePath = bps.bp.relProjPath.lexically_normal();
		bps.bp.relinkedSavePath /= bps.bp.projectFilenameCorrect.filename();
        bps.bp.relinkedSavePath.replace_extension(bps.bp.originalProject.extension());

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

		ERROR_THROW_AE_MOD(AeGfsFileCreator::getInstance()->InitGfsFileBuilder(bps));
			
		rbProj()->logg("PrepareProject", "ProjectName", projectName);
		rbProj()->logg(L"PrepareProject", L"SourcePath", bps.bp.originalProject.lexically_normal().wstring().c_str());
		rbProj()->logg(L"PrepareProject", L"RelinkedPath", bps.bp.relProjPath.lexically_normal().wstring().c_str());
		rbProj()->logg(L"PrepareProject", L"RelinkedSavePath", bps.bp.relinkedSavePath.lexically_normal().wstring().c_str());
		rbProj()->logg(L"PrepareProject", L"TmpPath", bps.beamerTmpPath);

		suites.AppSuite6()->PF_DisposeAppProgressDialog(relinkerProg);
		rbUtilities::toUTF16(L"Collect, relink and copy data...", dialogText, 128);
		suites.AppSuite6()->PF_CreateNewAppProgressDialog(dialogText, NULL, FALSE, &relinkerProg);
	
		ERROR_CATCH_END_RETURN(suites)
}

ErrorCodesAE GF_Dumper::newBatchDumpProject()
{
	ERROR_CATCH_START_MOD(CallerModuleName::ProjectDumperModule)

	relinker.RelinkerInitialize(bps, *rbProj(), TRUE);

	GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, 1, 4));
	ERROR_RETURN(DumpQueeItems(bps.bp.rqMainOutput));
	GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, 2, 4));
	ERROR_RETURN(newCopyCollectFonts());
	GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, 3, 4));
	ERROR_RETURN(newCollectEffectsInfo());
	
	AeGfsFileCreator::getInstance()->GenerateAndSaveDocument();
	relinker.RelinkProject(rootProjH);

	while(!sc->renderFootageSortedList.empty())
	{
		delete sc->renderFootageSortedList.back();
		sc->renderFootageSortedList.pop_back();
	}

	AeBatchRelinker batchRelinker(sP, relinker.GetC4dLibloader(), *rbProj(), relinkerProg, bps.bp.relinkedSavePath, bps.bp.remoteFootagePath.parent_path());
	ERROR_RETURN(batchRelinker.ParseAepxXmlDocument())
	ERROR_RETURN(batchRelinker.CopyAndRelinkFiles(bps.bp.footageMainOutput, bps.bp.remoteFootagePath))
	
	rbProj()->logger.flush();
	rbProj()->logger.close();
	
	FS_ERROR_CODE(LogCoppyError);
	fs::copy(bps.bp.tempLogPath, bps.bp.logsMainOutput, LogCoppyError);
	
	if (rbUtilities::execBeamerCmd(sP, BeamerMask_SendTaskEncoded, NULL, 0, bps) != NoError) {
		throw  PluginError(_ErrorCaller, ErrorCodesAE::BeamerSendTaskFailed);
	}
	ERROR_CATCH_END_RETURN(suites)
}

ErrorCodesAE GF_Dumper::newDumpProject()
{
	//suites.AEGP_ProjSuite6()->AEGP_GetProjectTimeDisplay();

	ERROR_CATCH_START_MOD(CallerModuleName::ProjectDumperModule)
	
	sc->countCollectedNr();
	bps.colectedItems = sc->getCollectedNr();
	bps.currentItem = 0;

	relinker.RelinkerInitialize(bps, *rbProj());

	GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, bps.currentItem, bps.colectedItems));

	rbProj()->logg(L"MainDumper", L"Relinker", L"Starting copy and relink process.");
	ERROR_RETURN(newCopyRelinkFootages());
	rbProj()->logg(L"MainDumper", L"Relinker", L"Finished relinking. Begin processing render queue items.");
	ERROR_RETURN(DumpQueeItems(bps.bp.rqMainOutput));

	GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, bps.currentItem, bps.colectedItems));
	rbProj()->logg(L"MainDumper", L"Collector", L"Collecting fonts.");
	ERROR_RETURN(newCopyCollectFonts());
	rbProj()->logg(L"MainDumper", L"Collector", L"Collecting effects list.");
	ERROR_RETURN(newCollectEffectsInfo());

	AeGfsFileCreator::getInstance()->GenerateAndSaveDocument();

	rbProj()->logg(L"MainDumper", L"Collector", L"Saving project changes.");
	relinker.RelinkProject(rootProjH);

	rbProj()->logg(L"MainDumper", L"Collector", L"Calling renderBeamer - Sending script for beamer upload process start");
	rbProj()->logger.flush();
	rbProj()->logger.close();
	FS_ERROR_CODE(LogCoppyError);
	fs::copy(bps.bp.tempLogPath, bps.bp.logsMainOutput, LogCoppyError);

	if (rbUtilities::execBeamerCmd(sP, BeamerMask_SendTaskEncoded, NULL, 0, bps) != NoError) {
		throw  PluginError(_ErrorCaller, ErrorCodesAE::BeamerSendTaskFailed);
	}
	ERROR_CATCH_END_RETURN(suites)
}
ErrorCodesAE GF_Dumper::newCopyRelinkFootages()
{
	ERROR_CATCH_START
	AeFootageNode *node = NULL, *tmpNode = NULL;

	for (A_long i = 0; i < static_cast<A_long>(sc->renderFootageSortedList.size()); ++i)
	{
		auto it = sc->renderFootageSortedList.begin();
		if(i > 0)
			std::advance(it, i);
		node = *it;
		for (A_long j = 0; j < i; ++j) {
			auto its = sc->renderFootageSortedList.begin();
			if (j > 0)
				std::advance(its, j);
			tmpNode = *its;
			if (tmpNode && node && tmpNode->copyOf == -1 && tmpNode->path == node->path)
			{
				node->copyOf = j;
				node->copyFootageNode = tmpNode;
				break;
			}
		}
		//bps.currentItem += node->size;
		++bps.currentItem;
		GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, bps.currentItem, bps.colectedItems));
		try {
			ERROR_THROW(relinker.newRelinkFootage(node))
			node->relinked = 1;
			rbProj()->logg(node->path.wstring().c_str(), L"RELINK_OK", node->pathRelinked.wstring().c_str());
		}
		catch (std::exception &err) {
			node->relinked = -1;
			rbProj()->loggErr(FS_U8STRING(node->path).c_str(), "RELINK_ERR", err.what());
			sc->renderFootageMissingList.push_back(node);
		}
	}
	ERROR_CATCH_END_RETURN(suites)
}

ErrorCodesAE GF_Dumper::newCopyCollectFonts()
{
	ERROR_CATCH_START
	A_long it = 0;
	gfsFontNode *gfsNode = NULL;
	std::vector<std::string> fontsContainer;

	for (auto node : sc->fontsList)
	{
		rbProj()->logger << L"Processing font name=\"" << node->getFont() << "\" family=\"" << node->getFamily() << "\" file=\"" << node->getLocation() << L"\" " << std::endl;
		it = 0;
		gfsNode = NULL;		
		fontsContainer.clear();
		GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(relinkerProg, ++bps.currentItem, bps.colectedItems));
		if (relinker.CopyFont(node, it++, fontsContainer) == NoError) 
		{
			for (int iter = 0; iter < fontsContainer.size(); iter++) 
			{
                gfsNode = new gfsFontNode({ iter, std::string(node->getFont()), fontsContainer.at(iter) });
				if(AeGfsFileCreator::getInstance()->PushFontNode(gfsNode) == NoError)
					rbProj()->loggA(6, "Collected font=", std::to_string(iter).c_str(), node->getFont(), node->getFamily(), node->getLocation(), FS_U8STRING(gfsNode->fontFile.filename()).c_str());
			}
		}
		else {
			rbProj()->logger << L"ERROR collecting font name=\"" << node->getFont() << "\" family=\"" << node->getFamily() << "\" file=\"" << node->getLocation() << L"\" " << std::endl;
		}
	}
	ERROR_CATCH_END_RETURN(suites)
}
ErrorCodesAE GF_Dumper::newCollectEffectsInfo()
{
	ERROR_CATCH_START_MOD(CallerModuleName::CollectEffectsModule)
	rbProj()->logger << std::endl;
	gfsEffectNode *gfsEffect;
	for (auto node : sc->effectsList)
	{
		gfsEffect = new gfsEffectNode({ node->getKey(), node->getEffectName(), node->getEffectMatchN(), node->getEffectCategory() });
		if(AeGfsFileCreator::getInstance()->PushEffectNode(gfsEffect) == NoError)
        {
            rbProj()->logg(L"Collector", L"Effect", L"Name:", false);
            rbProj()->logger << node->getEffectName() << L",  MatchName:" << node->getEffectMatchN() << L",  Catgegory:" << node->getEffectCategory() << L",  InstallKey:" << node->getKey() << std::endl;
        }
	}
	ERROR_CATCH_END_LOGGER_RETURN("EffectsCollecting")
}

ErrorCodesAE GF_Dumper::DumpQueeItems(fs::path outputPath)
{
	ERROR_CATCH_START_MOD(CallerModuleName::QueueCollectModule)   
        A_long rq_ItemsN = 0L, rq_Index = 0L;
		ERROR_THROW_AE_MOD(suites.RQItemSuite3()->AEGP_GetNumRQItems(&rq_ItemsN));
        while (rq_Index < rq_ItemsN)
        {
			ERROR_THROW_MOD(DumpQueeItem(rq_Index++, fs::path(outputPath)));
        }		
	ERROR_CATCH_END_RETURN(suites)
}

ErrorCodesAE GF_Dumper::DumpQueeItem(A_long itemIndex, fs::path outputPath)
{
	ERROR_CATCH_START_MOD(CallerModuleName::QueueCollectModule)

	gfsRqItem*					gfsItem = NULL;
	A_char						frameScript[400];
    AEGP_ItemH					rq_ItemH = NULL;
    AEGP_CompH					rq_ItemComposition = NULL;
    AEGP_RQItemRefH				rq_ItemRef = NULL;
    AEGP_RenderItemStatusType	rq_ItemQueuedStatus = AEGP_RenderItemStatus_NONE;//AEGP_RenderItemStatus_QUEUED
	A_long						rq_OutModulessN = 0;

	ERROR_THROW_AE_MOD(suites.RQItemSuite3()->AEGP_GetRQItemByIndex(itemIndex, &rq_ItemRef));
	ERROR_THROW_AE_MOD(suites.RQItemSuite3()->AEGP_GetRenderState(rq_ItemRef, &rq_ItemQueuedStatus));
	ERROR_THROW_AE_MOD(suites.RQItemSuite3()->AEGP_GetNumOutputModulesForRQItem(rq_ItemRef, &rq_OutModulessN));

    if (rq_ItemQueuedStatus == AEGP_RenderItemStatus_QUEUED && rq_OutModulessN > 0)
    {
		gfsItem = new gfsRqItem();
		if (!gfsItem) throw ErrorCodesAE::AE_ErrAlloc;

		gfsItem->indexNr = itemIndex + 1;

		ERROR_THROW_AE_MOD(suites.RQItemSuite3()->AEGP_GetCompFromRQItem(rq_ItemRef, &rq_ItemComposition));
		ERROR_THROW_AE_MOD(suites.CompSuite10()->AEGP_GetItemFromComp(rq_ItemComposition, &rq_ItemH));

        suites.ANSICallbacksSuite1()->sprintf(frameScript, "var rqItem=app.project.renderQueue.item(%d);var rqMod=rqItem.getSettings(GetSettingsFormat.STRING);var rqFps=rqMod[rqMod[\"Frame Rate\"]];\n(Math.round((rqItem.timeSpanStart)*rqFps)).toString()+\"to\"+(Math.round((rqItem.timeSpanStart+rqItem.timeSpanDuration)*rqFps)-1).toString()+\"s1\";", itemIndex + 1);
		ERROR_THROW_MOD(rbUtilities::execScript(sP, pluginId, frameScript, gfsItem->frameString, 32));

		suites.ANSICallbacksSuite1()->sprintf(frameScript, "var fpsRqMod=app.project.renderQueue.item(%d).getSettings(GetSettingsFormat.STRING);fpsRqMod[fpsRqMod[\"Frame Rate\"]].toString();", itemIndex + 1);
		ERROR_THROW_MOD(rbUtilities::execScript(sP, pluginId, frameScript, gfsItem->fps, 32));

		ERROR_THROW_AE_MOD(suites.ItemSuite8()->AEGP_GetItemDimensions(rq_ItemH, &gfsItem->width, &gfsItem->height));
		ERROR_THROW_AE_MOD(suites.ItemSuite7()->AEGP_GetItemName(rq_ItemH, gfsItem->compositioName));
		rbUtilities::leaveAllowedOnly(gfsItem->compositioName);
		//ERROR_THROW_AE_MOD(suites.ItemSuite7()->AEGP_SetItemName(rq_ItemH, gfsItem->compositioName));

		rbProj()->logg(L"QueueItemDumper", L"Parsed Render quee item nr:", std::to_wstring(itemIndex + 1).c_str());
		ERROR_THROW_AE_MOD(DumpOutputModules(rq_ItemRef, rq_OutModulessN, gfsItem, outputPath));
		AeGfsFileCreator::getInstance()->PushRenderQueueItem(gfsItem);
;
    }
	ERROR_CATCH_END_LOGGER_RETURN("QueueItem")
}

ErrorCodesAE GF_Dumper::DumpOutputModules(AEGP_RQItemRefH &rq_ItemRef, A_long outModulesNumber, gfsRqItem *parentItem,  fs::path outputPath)
{
	ERROR_CATCH_START_MOD(CallerModuleName::OutputCollectModule)
		if (!parentItem) throw NullPointerResult;

		std::string memBuff1, memBuff2;	
		AEGP_MemHandle memH1 = NULL, memH2 = NULL;
		AEGP_OutputModuleRefH rq_ItemOutModuleRef = NULL;
		AEGP_OutputTypes outType = NULL;				
		gfsRqItemOutput* outNode = NULL;

		while (outModulesNumber > 0) {
			if ((outNode = new gfsRqItemOutput()) == NULL)
				_ErrorCode = AE_ErrAlloc;			
			ERROR_AEER(suites.OutputModuleSuite4()->AEGP_GetOutputModuleByIndex(rq_ItemRef, --outModulesNumber, &rq_ItemOutModuleRef));
			ERROR_AEER(suites.OutputModuleSuite4()->AEGP_GetOutputFilePath(rq_ItemRef, rq_ItemOutModuleRef, &memH1));
#ifndef USE_BOOST
            char16_t* tempPtr = nullptr;
			ERROR_AEER(suites.MemorySuite1()->AEGP_LockMemHandle(memH1, reinterpret_cast<void**>(&tempPtr)));
			if (_ErrorCode == NoError) outNode->outputFile = tempPtr;
			ERROR_AEER(suites.MemorySuite1()->AEGP_UnlockMemHandle(memH1));
			ERROR_AEER(suites.MemorySuite1()->AEGP_FreeMemHandle(memH1));
#else
            ERROR_AEER(rbUtilities::copyMemhUTF16ToString(sP, memH1, memBuff1));
            if (_ErrorCode == NoError) outNode->outputFile = memBuff1;
#endif
			ERROR_AEER(suites.OutputModuleSuite4()->AEGP_GetExtraOutputModuleInfo(rq_ItemRef, rq_ItemOutModuleRef, &memH1, &memH2, &outNode->outFileIsSeq, &outNode->outFileIsMultiframe));
			ERROR_AEER(rbUtilities::copyMemhUTF16ToString(sP, memH1, memBuff1));
			ERROR_AEER(rbUtilities::copyMemhUTF16ToString(sP, memH2, memBuff2));

			ERROR_AEER(suites.OutputModuleSuite4()->AEGP_GetEnabledOutputs(rq_ItemRef, rq_ItemOutModuleRef, &outType));			
			if (outType & AEGP_OutputType_AUDIO) {
				ERROR_AEER(suites.OutputModuleSuite4()->AEGP_GetSoundFormatInfo(rq_ItemRef, rq_ItemOutModuleRef, &outNode->soundFormat, &outNode->outputAudioSetToUse));
				if (_ErrorCode == NoError) {
					outNode->outputAudioEnabled = true;
					if (outNode->outputAudioSetToUse > 0)
						outNode->outputAudioSetToUse = 1;
					else
						outNode->outputAudioSetToUse = 0;
				}
			}

			if (_ErrorCode == NoError) {
				outNode->indexNr = outModulesNumber+1;
				RB_STRNCPTY(outNode->outputType, memBuff1.c_str(), 46);
				RB_STRNCPTY(outNode->outputInfo, memBuff2.c_str(), 94);
				parentItem->outMods.push_back(outNode);
				outNode = nullptr;
			}
#ifndef USE_BOOST
			ERROR_THROW_AE_MOD(suites.OutputModuleSuite4()->AEGP_SetOutputFilePath(rq_ItemRef, rq_ItemOutModuleRef, (A_UTF16Char*)outputPath.u16string().c_str()));
#else
            A_UTF16Char newPath[AEGP_MAX_PATH_SIZE];
            rbUtilities::copyConvertStringLiteralIntoUTF16(outputPath.wstring().c_str(), newPath, AEGP_MAX_PATH_SIZE);
            ERROR_THROW_AE_MOD(suites.OutputModuleSuite4()->AEGP_SetOutputFilePath(rq_ItemRef, rq_ItemOutModuleRef, newPath));
#endif
			if (outModulesNumber > 1) {
				ERROR_THROW_AE_MOD(suites.RQItemSuite3()->AEGP_RemoveOutputModule(rq_ItemRef, rq_ItemOutModuleRef));
			}
			rbProj()->logg(L"QueeItemOutputDumper", L"Parsed Render item output nr:", std::to_wstring(outModulesNumber).c_str());
			_ErrorCode = NoError;
		}
	ERROR_CATCH_END_LOGGER_RETURN("OutputItem")
}

ErrorCodesAE GF_Dumper::setConteiner(AeSceneConteiner &sc)
{
	this->sc = &sc;
	return ErrorCodesAE::NoError;
}
