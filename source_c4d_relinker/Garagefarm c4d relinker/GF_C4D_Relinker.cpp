#include "pch.h"

#include "GF_C4D_Relinker.hpp"
#include "GF_C4D_RelinkerPriv.hpp"
#include <stdexcept>

#define WIN_LOWER(THESTR) ((OS_CASE == 0) ? THESTR.ToLower() : THESTR)

using namespace cineware;

void GetWriterInfo(Int32 &id, String &appname)
{
    id = static_cast<Int32>(2462845682);
    appname = "GarageFarm.NET textures relinker";
}

namespace cinewareRelinker
{

    c4dAssetsCollector::c4dAssetsCollector() : relink(false), using_tex(false), c4dDoc(NULL), mat(NULL), files()
    { }
    void c4dAssetsCollector::setRelink(bool value) {
        relink = value;
    }
    void c4dAssetsCollector::setUseTexFolder(bool value) {
        using_tex = value;
    }

    bool c4dAssetsCollector::pushBack(FileNodeCineware* node)
    {
        for (auto it : files)
        {
            if (it->file.Compare(node->file) == 0) {
                node->relinkedPath = it->relinkedPath;
                node->relinkedFile = it->relinkedFile;
                return false;
            }
        }
        files.push_back(node);
        return true;
    }

    void c4dAssetsCollector::collectAssets(cineware::BaseShader *shader)
    {
        cineware::BaseShader *sh = shader;
        while (sh)
        {
            if (sh->GetType() == Xlayer)
            {
                cineware::iBlendDataType* d = (cineware::iBlendDataType*)sh->GetDataInstance()->GetData(cineware::SLA_LAYER_BLEND).GetCustomDataType(CUSTOMDATA_BLEND_LIST);
                if (d)
                {
                    cineware::LayerShaderLayer *lsl = (cineware::LayerShaderLayer*)(d->m_BlendLayers.GetObject(0));
                    while (lsl)
                    {
                        if (lsl->GetType() == cineware::TypeFolder)
                        {
                            cineware::LayerShaderLayer *subLsl = (cineware::LayerShaderLayer*)((cineware::BlendFolder*)lsl)->m_Children.GetObject(0);
                            while (subLsl)
                            {
                                if (subLsl->GetType() == cineware::TypeShader)
                                    collectAssets((cineware::BaseShader*)((cineware::BlendShader*)subLsl)->m_pLink->GetLink());
                                subLsl = subLsl->GetNext();
                            }
                        }
                        else if (lsl->GetType() == cineware::TypeShader)
                            collectAssets((cineware::BaseShader*)((cineware::BlendShader*)lsl)->m_pLink->GetLink());
                        lsl = lsl->GetNext();
                    }
                }
            }
            else if (sh->GetType() == Xbitmap) {
                FileNodeCineware *node = new FileNodeCineware();

                if (!node)
                    continue;
                if (!sh->GetFileName().IsPopulated()) {
                    delete node;
                    continue;
                }
                node->file = WIN_LOWER(sh->GetFileName().GetString());
                node->isUrl = sh->GetFileName().IsBrowserUrl();
                bool unique = this->pushBack(node);

                if (!node->isUrl)
                {
                    if (relink && unique) {
                        cineware::Filename tmp;
                        if (!using_tex)
                            tmp.SetDirectory(newPathPrefixM);
                        tmp.SetFile(cineware::String::IntToString(static_cast<cineware::Int32>(files.size())) += sh->GetFileName().GetFile().GetString());
                        sh->SetFileName(tmp);
                        node->relinkedPath = WIN_LOWER(tmp.GetString());
                        node->relinkedFile = WIN_LOWER(tmp.GetFile().GetString());
                    }
                    else if (relink && !unique) {
                        sh->SetFileName(node->relinkedPath);
                    }
                }
                /*
                add support for online assets
                */
            }
            else if (sh->GetType() == Xvariation)
            {
                auto v_data = dynamic_cast<cineware::VariationShaderData*>(sh->GetNodeData());
                if (v_data)
                {
                    for (cineware::Int32 t = 0; t < v_data->GetTextureCount(); t++)
                        collectAssets(v_data->GetTextureLayer(t)._shader);
                }
                else
                    collectAssets(sh->GetDown());
            }
            sh = sh->GetNext();
        }
    }

    errorCodes c4dAssetsCollector::loadScene(const cineware::String &loadFile, const cineware::String &saveFile, const cineware::String &relinkPath)
    {
        try {
            projectPathM = loadFile;

            if (relink)
            {
                newPathPrefix = relinkPath;
                newPathPrefixM = newPathPrefix;
                newProjectPathM = saveFile;
            }

            c4dDoc = LoadDocument(projectPathM, SCENEFILTER_OBJECTS | SCENEFILTER_MATERIALS);

            if (c4dDoc) {
                mat = c4dDoc->GetFirstMaterial();
                while (mat)
                {
                    mat->GetFirstShader();
                    collectAssets(mat->GetFirstShader());
                    mat = mat->GetNext();
                }

                if (relink) {
                    if (!SaveDocument(c4dDoc, newProjectPathM, cineware::SAVEDOCUMENTFLAGS_0)) {
                        cineware::BaseDocument::Free(c4dDoc);
                        return err_SaveFileError;
                    }
                }

                cineware::BaseDocument::Free(c4dDoc);
                return err_NoError;
            }
            return err_LoadFileError;
            
        }
        catch (...) {
            return err_UnknownLoadSceneException;
        }
    }

errorCodes c4dAssetsCollector::getAssetsList(c4dStruct* in_data, FileNode** out_data)
        {
       cineware::String loadFile, saveFile, relinkPath;
       try {
           c4dAssetsCollector theCollector;
           if (!in_data)
               return err_InputPointerError;
    
           in_data->errorCode = cinewareRelinker::err_NoError;
                       
            loadFile.SetCString(in_data->loadFile);
            saveFile.SetCString(in_data->saveFile);
            relinkPath.SetCString(in_data->relinkPath);
       
            if(loadFile.GetLength() < 1)
                return err_LoadPathEmpty;
       
            if (saveFile.GetLength() > 1)
                theCollector.setRelink(true);
       
            if (relinkPath.GetLength() < 1)
                theCollector.setUseTexFolder(true);
       
            in_data->errorCode = theCollector.loadScene(loadFile, saveFile, relinkPath);
            in_data->stackSize = theCollector.files.size();
       
            if (in_data->stackSize <= 0)
                return err_NoError;
       
            *out_data = new FileNode[theCollector.files.size()];
       
            for (size_t i = 0; i < theCollector.files.size(); i++)
            {
                theCollector.files.at(i)->file.GetCString((*out_data)[i].file, LIB_C4D_MAXPATH);
                theCollector.files.at(i)->relinkedPath.GetCString((*out_data)[i].relinkedPath, LIB_C4D_MAXPATH);
                theCollector.files.at(i)->relinkedFile.GetCString((*out_data)[i].relinkedFile, LIB_C4D_MAXPATH);
                (*out_data)[i].isUrl = theCollector.files.at(i)->isUrl;
            }
                   
            }
            catch (...) {
                if (in_data)
                    in_data->errorCode = cinewareRelinker::err_UnknownException;
                return err_UnknownException;
            }
            return err_NoError;
           }
       }

int cppWrapperFunction(c4dStruct* in_data, FileNode** out_data)
{
    int ret = cinewareRelinker::c4dAssetsCollector::getAssetsList(in_data, out_data);
    return ret;
}

#ifdef __cplusplus
extern "C" {
#endif
#ifdef OS_WIN
    __declspec(dllexport) int getAssetsListAndRelink(c4dStruct* in_data, FileNode** out_data)
#else
    EXPORT
    int getAssetsListAndRelink(c4dStruct* in_data, FileNode** out_data)
#endif
    {
        return cppWrapperFunction(in_data, out_data);
    }

#ifdef __cplusplus
}
#endif


/*
	cineware::BaseContainer* bas;
	BaseObject* obj = c4dDoc->GetFirstObject();
	GeData dat;

	while (obj)
	{
		if (obj->GetType() == Oxref)
		{
			bas = obj->GetDataInstance();
			if (bas)
			{
				if (bas->GetParameter(CK_MORPH_LINK, dat))
				{
					if (dat.GetType() == DA_FILENAME)
					{
						printf("\n\t filename: %s", dat.GetFilename().GetString().GetCStringCopy());
						cineware::Filename tmp = dat.GetFilename();
						tmp.SetFile("first_dependency.c4d");
						dat.SetFilename(tmp);
						bas->SetParameter(CK_MORPH_LINK, dat);
					}
				}
			}
			//obj->G
		}
		obj = obj->GetNext();
	}

*/