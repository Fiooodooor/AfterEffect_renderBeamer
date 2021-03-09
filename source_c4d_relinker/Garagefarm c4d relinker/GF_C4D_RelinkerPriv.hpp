//
//  Garagefarm_c4d_relinkerPriv.hpp
//  Garagefarm c4d relinker
//
//  Created by CzornyCzfanyCzop on 3/23/20.
//  Copyright © 2020 garageFarm.net. All rights reserved.
//

/* The classes below are not exported */
#ifndef Garagefarm_c4d_relinkerpriv_
#define Garagefarm_c4d_relinkerpriv_


#include "default_alien_overloads.h"
#include "c4d.h"
#include <c4d_string.h>
#include <vector>
#include <string>


int cppWrapperFunction(void *data);

namespace cinewareRelinker
{
    typedef struct {
        cineware::String file;
        cineware::String relinkedPath;
        cineware::String relinkedFile;
        bool isUrl;
    } FileNodeCineware;

    typedef std::vector<FileNodeCineware*> FilesStack;

    class c4dAssetsCollector
    {
    public:
        FilesStack files;
        c4dAssetsCollector();
        static errorCodes getAssetsList(c4dStruct* in_data, FileNode** out_data);

        void setRelink(bool value);
        void setUseTexFolder(bool value);

    private:
        cineware::Bool relink, using_tex;
        cineware::BaseDocument* c4dDoc;
        cineware::BaseMaterial* mat;

        cineware::String newPathPrefix;
        cineware::String newPathPrefixTex;

        cineware::Filename projectPathM;
        cineware::Filename newProjectPathM;
        cineware::Filename newPathPrefixM;

        bool pushBack(FileNodeCineware* node);
        void collectAssets(cineware::BaseShader *shader);
        errorCodes loadScene(const cineware::String &loadFile, const cineware::String &saveFile, const cineware::String &relinkPath);

    };
}

#endif