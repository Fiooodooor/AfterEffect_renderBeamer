
#include "GF_AEGP_Relinker.h"
#include "GF_AEGP_Dumper.h"
namespace RenderBeamer {

GF_AEGP_Relinker::GF_AEGP_Relinker(SPBasicSuite *basicSuite, AEGP_PluginID pI)
    : sP(basicSuite)
    , suites(sP)
    , pluginId(pI)
	, c4d_interface(nullptr)
	, fonts_interface(nullptr)
	, out(nullptr)
	, projectSavePath()
    , bps(nullptr)
{
}
GF_AEGP_Relinker::~GF_AEGP_Relinker()
{
	delete c4d_interface;
	delete fonts_interface;
}

ErrorCodesAE GF_AEGP_Relinker::RelinkerInitialize(beamerParamsStruct *tmpBps, A_Boolean batchRelink)
{
	ERROR_CATCH_START
		setPathsStruct(tmpBps);	
		if (batchRelink == TRUE)
		{
			if (tmpBps->bp.relinkedSavePath.has_extension())
				tmpBps->bp.relinkedSavePath.replace_extension(".aepx");
			else
				tmpBps->bp.relinkedSavePath += ".aepx";
			
		}
		rbUtilities::toUTF16(tmpBps->bp.relinkedSavePath.wstring().c_str(), projectSavePath, AEGP_MAX_PATH_SIZE);
		

		c4d_interface = new PlatformLibLoader();
        if(c4d_interface && fs::exists(bps->c4d_LibPath)) {
            if(!c4d_interface->loadLibraryFromPathW(bps->c4d_LibPath))
                GF_Dumper::rbProj()->loggErr("RelinkerInitialize", "CinemaRelinker", "c4d_interface::loadLibraryFromPath error");
        }
    
		fonts_interface = new PlatformLibLoader();
        if (fonts_interface && fs::exists(bps->fontLibPath)) {
			if(!fonts_interface->loadLibraryFromPathW(bps->fontLibPath))
                GF_Dumper::rbProj()->loggErr("RelinkerInitialize", "FontLibrary", "fonts_interface::loadLibraryFromPath error");
        } 
	ERROR_CATCH_END_RETURN(suites)
}
ErrorCodesAE GF_AEGP_Relinker::unloadFontLibrary()
{
	ERROR_CATCH_START
	if(fonts_interface)
	{
		delete fonts_interface;
		fonts_interface = nullptr;
	}
	ERROR_CATCH_END_NO_INFO_RETURN
}
A_Err GF_AEGP_Relinker::RelinkProject(AEGP_ProjectH projectH)
{
    PT_XTE_START{
        PT_ETX(suites.ProjSuite6()->AEGP_SaveProjectToPath(projectH, projectSavePath))
    } PT_XTE_CATCH_RETURN_ERR
}

ErrorCodesAE GF_AEGP_Relinker::newRelinkFootage(AeFootageNode *node)
{
	ERROR_CATCH_START
	FS_ERROR_CODE(err)
	AEGP_MemHandle memPathH;
	AEGP_FootageInterp newInterpret;

	if (!node->isFooFile())
		return NoError;
	
	if (node->copyOf == -1)
	{
		node->pathRelinked = bps->bp.footageMainOutput.string() + std::to_string(node->getItemId()) + '_' + node->path.filename().string();

		if (node->isFooSequence())
		{			
			A_long TmpSeqUID = node->getItemId();
            std::string TmpSeqUIDString = std::to_string(TmpSeqUID);
			
			for (A_long it = node->getNrFiles() - 1; it >= 0; --it)
			{
				ERROR_THROW_AE(suites.FootageSuite5()->AEGP_GetFootagePath(node->getFooH(), it, AEGP_FOOTAGE_MAIN_FILE_INDEX, &memPathH))
				ERROR_THROW_AE(rbUtilities::copyMemhUTF16ToPath(sP, memPathH, node->path))
				FS_REPLACE_FILENAME(node, TmpSeqUID)
				GFCopyFile(TmpSeqUIDString, node->path, node->pathRelinked, true);
			}
		}
		else if (node->path.has_extension() && node->path.extension().compare(fs::path(".c4d")) == 0)
		{
			if (GFCopy_C4D_File(c4d_interface, node->path, node->pathRelinked, bps->bp.remoteFootagePath, std::to_string(node->getItemId())) != NoError) {
				GFCopyFile(node->getItemIdString(), node->path, node->pathRelinked, true);
			}
		}
		else
			GFCopyFile(node->getItemIdString(),node->path, node->pathRelinked, true);
	}
	else {
		node->pathRelinked = node->copyFootageNode->pathRelinked;
	}

	if (fs::exists(node->pathRelinked, err))
	{
		rbUtilities::copyConvertStringLiteralIntoUTF16(node->pathRelinked.wstring().c_str(), node->relinkedPath, AEGP_MAX_PATH_SIZE);                
        ERROR_THROW_AE(suites.FootageSuite5()->AEGP_NewFootage(pluginId, node->relinkedPath, &node->layerKey, &node->seqImpOptions, AEGP_InterpretationStyle_NO_DIALOG_NO_GUESS, NULL, &node->newFooH))
		ERROR_THROW_AE(suites.FootageSuite5()->AEGP_ReplaceItemMainFootage(node->newFooH, node->getItemH()))
		ERROR_THROW_AE(suites.FootageSuite5()->AEGP_SetFootageInterpretation(node->getItemH(), FALSE, &node->interpret))
		ERROR_THROW_AE(suites.FootageSuite5()->AEGP_GetFootageInterpretation(node->getItemH(), FALSE, &newInterpret))
		newInterpret.native_fpsF = node->interpret.native_fpsF;
		newInterpret.conform_fpsF = node->interpret.conform_fpsF;
		newInterpret.pd = node->interpret.pd;
		newInterpret.al = node->interpret.al;
		ERROR_THROW_AE(suites.FootageSuite5()->AEGP_SetFootageInterpretation(node->getItemH(), FALSE, &newInterpret))
	}
	else
		return ErrorResult;
	ERROR_CATCH_END_NO_INFO_RETURN
}


bool GF_AEGP_Relinker::GFCopyFile(const std::string &UID, fs::path oldFootagePath, fs::path &tmpNewFootagePath, bool forceNoRename, bool forceNoSymlinks)
{
    bool success = false;
    FS_ERROR_CODE(err)
    FS_ERROR_CODE(err1)
		
    if (fs::exists(oldFootagePath, err))
    {
        if (!forceNoRename)
        {
            fs::path tmpFilename = tmpNewFootagePath.parent_path().string() + SEP + UID + '_' + tmpNewFootagePath.filename().string();
			tmpNewFootagePath = tmpFilename;
        }
        if (!forceNoSymlinks)
        {
            if (fs::is_symlink(oldFootagePath)) {
                fs::copy_symlink(oldFootagePath, tmpNewFootagePath, err);
            }
            else {
                fs::create_symlink(oldFootagePath, tmpNewFootagePath, err);
            }
            if (err.value() == 0)
                success = true;
        }
        if (success == false)
        {
            while (fs::is_symlink(oldFootagePath, err) && err.value() == 0) {
                if (fs::exists(fs::read_symlink(oldFootagePath, err), err1) && err.value() == 0 && err1.value() == 0)
                    oldFootagePath = fs::read_symlink(oldFootagePath, err1);
                if (err1.value() != 0)
                    break;
            }
            if (err1.value() == 0 && fs::exists(oldFootagePath, err) && err.value() == 0)
                fs::copy_file(oldFootagePath, tmpNewFootagePath, FS_COPY_OPTIONS, err);
        }
    }
    if (err.value() == 0)
        return true;
    else
        return false;
}
ErrorCodesAE GF_AEGP_Relinker::CopyFont(AeFontNode *node, A_long id, std::vector<std::string> &fontsList)
{
	ERROR_CATCH_START
	node->path = node->getLocation();
	if (fs::exists(node->path))
	{
		node->pathRelinked = bps->bp.fontsMainOutput;
		node->pathRelinked /= node->path.filename();
        if (!fonts_interface || !fonts_interface->isLibraryLoaded())
        {
			if (!node->pathRelinked.has_extension())
				node->pathRelinked += ".ttf";
			if (GFCopyFile(std::to_string(id), node->path, node->pathRelinked, false, true)) {
				fontsList.push_back(FS_U8STRING(node->pathRelinked.filename()).c_str());
				GF_Dumper::rbProj()->logg("CopyFont", FS_U8STRING(node->path).c_str(), FS_U8STRING(node->pathRelinked).c_str());
			}
			else {
				GF_Dumper::rbProj()->loggErr("CopyFont", FS_U8STRING(node->path).c_str(), FS_U8STRING(node->pathRelinked).c_str());
			}
        }
		else if (CopyConvertFontLib(fonts_interface, node->path, node->pathRelinked, id, fontsList) != NoError)
        {
            GF_Dumper::rbProj()->logg("FontLibrary", "CopyFont", "Font copy using library failed! Trying to copy manualy.");
            GFCopyFile(std::to_string(id), node->path, node->pathRelinked, false, true);
            fontsList.push_back(FS_U8STRING(node->pathRelinked.filename()).c_str());
		}
	}
	ERROR_CATCH_END_NO_INFO_RETURN
}


ErrorCodesAE GF_AEGP_Relinker::CopyConvertFontLib(PlatformLibLoader* libIt, fs::path &nodePath, fs::path &nodeRelinked, A_long id, std::vector<std::string> &fontsList)
{
	ERROR_CATCH_START
    int converted = 0;
    std::string pluginPath(bps->pluginPath.begin(), bps->pluginPath.end());
 
    if (libIt && libIt->isLibraryLoaded())
    {
        copyConvertFont_creator theConverter;
        if (libIt->loadFunctionDefinition((void**)(&theConverter), "copyConvertFont"))
        {
            GF_Dumper::rbProj()->logg("FontLibrary", "LoadFunction", "copyConvertFont function load success");
			char srcFile[LIB_MAXPATH_SIZE] = { '\0' }, srcPath[LIB_MAXPATH_SIZE] = { '\0' }, dstPath[LIB_MAXPATH_SIZE] = { '\0' };
            auto *fntList = new FontsListS;
            if(fntList)
            {
				fntList->pathsTable = nullptr;
                int ind = static_cast<int>(id);
				RB_STRNCPTY(srcFile, FS_U8STRING(nodePath.filename()).c_str(), LIB_MAXPATH_SIZE);
				RB_STRNCPTY(srcPath, FS_U8STRING(nodePath.parent_path()).c_str(), LIB_MAXPATH_SIZE);
				RB_STRNCPTY(dstPath, FS_U8STRING(nodeRelinked.parent_path()).c_str(), LIB_MAXPATH_SIZE);
                GF_Dumper::rbProj()->logg("FontLibrary", "RelinkTo", dstPath);
                
                converted = theConverter(srcFile, srcPath, dstPath, ind, fntList);
                if(fntList)
                {
                    if(converted > 0 && fntList->pathsTable)
                    {
                        for(int it=0; it < fntList->structureSize; it++)
                        {
                            fs::path fontFile(fntList->pathsTable[it].fontPath);
                            fontsList.push_back(FS_U8STRING(fontFile.filename()) );
                        }
						_ErrorCode = NoError;
                    }                    
                }
                GF_Dumper::rbProj()->logg("FontLibrary", "Converted", std::to_string(converted).c_str());
            }
			else { 
				_ErrorCode = AE_ErrAlloc; 
			}
        	if(fntList)
        	{
        		if(fntList->pathsTable)
					delete [] fntList->pathsTable;
				delete fntList;
        	}
        }
    }
	if (converted <= 0) {
		_ErrorCode = ErrorResult;
	}
	ERROR_CATCH_END_NO_INFO_RETURN
}

ErrorCodesAE GF_AEGP_Relinker::GFCopy_C4D_File(PlatformLibLoader* libIt, const fs::path &oldFootagePath, const fs::path &tmpNewFootagePath, const fs::path &remoteFootagePath, const std::string &id)
{
    ERROR_CATCH_START
	bool success = false;
	FS_ERROR_CODE(err)
	if (fs::exists(oldFootagePath, err) && err.value() == 0)
	{		
		if (libIt && libIt->isLibraryLoaded())
		{
			GF_Dumper::rbProj()->logg("C4D_Asset", "Lib", "Library file loaded. Function symbol importing.");
			_f_getAssetsListAndRelink getAndRelinkFunction;
			if(libIt->loadFunctionDefinition((void**)&getAndRelinkFunction, "getAssetsListAndRelink"))
			{
				auto* dataStruct = new c4dStruct({ 0, cinewareRelinker::err_NoError, {'\0'}, {'\0'}, {'\0'} });
				FileNode* dataStack = nullptr;

				const fs::path c4d_relinkdir = tmpNewFootagePath.parent_path().string() + SEP + "cinema_" + id + SEP;
				const fs::path c4d_relinkRemotePath = remoteFootagePath.string() + SEP + "cinema_" + id + SEP;

				fs::create_directories(c4d_relinkdir, err);

				RB_STRNCPTY(dataStruct->loadFile, oldFootagePath.lexically_normal().string().c_str(), LIB_C4D_MAXPATH);
				RB_STRNCPTY(dataStruct->saveFile, tmpNewFootagePath.lexically_normal().string().c_str(), LIB_C4D_MAXPATH);
				RB_STRNCPTY(dataStruct->relinkPath, c4d_relinkRemotePath.lexically_normal().string().c_str(), LIB_C4D_MAXPATH);

				GF_Dumper::rbProj()->logg("C4D_Asset", "Lib", "Function symbol imported success. Calling library for relink start.");
				GF_Dumper::rbProj()->logg("C4D_Asset", "FileLoad", dataStruct->loadFile);
				GF_Dumper::rbProj()->logg("C4D_Asset", "FileSave", dataStruct->saveFile);
				GF_Dumper::rbProj()->logg("C4D_Asset", "LocalRelDir", c4d_relinkdir.string().c_str());
				GF_Dumper::rbProj()->logg("C4D_Asset", "RemoteRelDir", dataStruct->relinkPath);

				int ret = getAndRelinkFunction(dataStruct, &dataStack);
                if(ret != 0) {
					GF_Dumper::rbProj()->loggErr("C4D_Asset", "FileLoad", ("There were an error while relinking c4d file! Code: " + std::to_string(ret)).c_str());
                }
				else if (dataStruct->errorCode == cinewareRelinker::err_NoError)
				{                    
					fs::path dataStackSourceFile;
                    
					GF_Dumper::rbProj()->logg("C4D_Asset", "FileLoad", ("C4D file relinked ok. Returned assets for copy: " + std::to_string(dataStruct->stackSize)).c_str());
					
					for (size_t i=0; i < dataStruct->stackSize; i++)
					{
						if (dataStack[i].isUrl == false) 
						{
							fs::path tmpDataStackSourceFile = fs::path(dataStack[i].file);
							if (tmpDataStackSourceFile.is_relative())
							{
								if (fs::exists(oldFootagePath.parent_path().string() + SEP + tmpDataStackSourceFile.string()))
									dataStackSourceFile = oldFootagePath.parent_path().string() + SEP + tmpDataStackSourceFile.string();
								else
									dataStackSourceFile = oldFootagePath.parent_path().string() + SEP + "tex" + SEP + tmpDataStackSourceFile.string();
							}
							else
								dataStackSourceFile = tmpDataStackSourceFile;

							fs::path dataStackRelinkedFile = c4d_relinkdir.string() + SEP + dataStack[i].relinkedFile;
                            bool cpyRes = GFCopyFile(id, dataStackSourceFile, dataStackRelinkedFile, true);
							GF_Dumper::rbProj()->logg("C4D_Asset", (cpyRes == true ? "OK::FROM" : "Failed::FROM"), dataStackSourceFile.lexically_normal().string().c_str());
							GF_Dumper::rbProj()->logg("C4D_Asset", (cpyRes == true ? "OK::DEST" : "Failed::DEST"), dataStackRelinkedFile.lexically_normal().string().c_str());
						}
                        else {
							GF_Dumper::rbProj()->loggErr("C4D_Asset", "URL Asset", dataStack[i].file);
                        }
					}
					success = true;
				}
                else {
					GF_Dumper::rbProj()->loggErr("C4D_Asset", "Relinker", ("Error code inside the return structure. Code: " + std::to_string(dataStruct->errorCode)).c_str());
                }
				delete dataStruct;
			}
			else { // function was not loaded
				GF_Dumper::rbProj()->loggErr(L"C4D_Asset", L"LibSymbols", (std::wstring(C4D_LIB_NAME)).c_str());
			}
		}
		else { // dll was not loaded
			GF_Dumper::rbProj()->loggErr(L"C4D_Asset", L"LibLoad", (std::wstring(C4D_LIB_NAME)).c_str());
		}
	}
	else { // file does not exists
		GF_Dumper::rbProj()->loggErr("C4D_Asset", "FileNotFound", oldFootagePath.string().c_str());
	}
	if(success)
        _ErrorCode = NoError;
    else
        _ErrorCode = ErrorResult;
    ERROR_CATCH_END_NO_INFO_RETURN
}

A_Err GF_AEGP_Relinker::RelinkQueueModuleItem(AEGP_RQItemRefH &rq_ItemRef, A_long moduleIndex)
{
    PT_XTE_START{

    } PT_XTE_CATCH_RETURN_ERR
}
void GF_AEGP_Relinker::setPathsStruct(beamerParamsStruct *beamerParamsS)
{
    //    FS_ERROR_CODE(err)
    bps = beamerParamsS;

    //    fs::path tmpSymlink = bps->bp.relGfsFile.replace_extension("lnk");
    //    fs::create_symlink(bps->bp.relGfsFile, tmpSymlink, err);

    //    if (err.value() == 0)
    //    {
    //        useSymlinksFlag = true;
    //        fs::remove(tmpSymlink, err);
    //    }
    //    else
    
}
PlatformLibLoader *GF_AEGP_Relinker::GetC4dLibloader()
{
	return c4d_interface;
}

} // namespace RenderBeamer
