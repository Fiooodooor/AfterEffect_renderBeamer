//
//  gf_font_converter.cpp
//  gf_font_converter
//
//  Created by Milosz Linkiewicz on 2/28/20.
//  Copyright © 2020 GarageFarm.net. All rights reserved.
//
#define LIB_DEBUG 1
#ifdef LIB_DEBUG
#include <iostream>
#define DPRINT(DE_TEXT) std::cout << DE_TEXT << std::endl;

#else
#define DPRINT(DE_TEXT)
#endif

#include <string>
#include <vector>
#include "gf_font_converter.hpp"
#include "gf_font_converterPriv.hpp"

#ifdef __cplusplus
extern "C" {
#endif
	#include "gf_font_generate.h"
#ifdef __cplusplus
}
#endif

#if defined(_WIN32) || defined(_WIN64)
	#define FOPEN(FILEPTR,FILEPATH,FILEMODE) fopen_s(& FILEPTR, FILEPATH, FILEMODE)
	#define EXPORT	
#else
	#define FOPEN(FILEPTR,FILEPATH,FILEMODE) FILEPTR = fopen(FILEPATH, FILEMODE)
	#define EXPORT __attribute__((visibility("default")))
#endif
//#define EXPORT __attribute__((visibility("default")))

template <class T>
void endswap(T *objp)
{
    unsigned char *memp = reinterpret_cast<unsigned char*>(objp);
    std::reverse(memp, memp + sizeof(T));
}
int font_convert_start(const char* sourceFile, const char* sourcePath, const char* destinationPath, int index, FontsListS *fontsList)
{
    DPRINT("Font_Convert_Start")
    try {
        if(!fontsList) return -15;
        fontsList->structureSize = 0;
        std::string sFile(sourceFile);
        std::string sPath(sourcePath);
        std::string sDestination(destinationPath);
        return gf_font_converter::convert(sFile, sPath, sDestination, index, fontsList);
    }
    catch(...) {
        return -1;
    }
    return -2;
}

int gf_font_converter::convert(std::string sourceFile, std::string sourcePath, std::string destinationPath, int index, FontsListS *fontsList)
{
    DPRINT("Class convert function called")
    int fontsConverted = 0;
	char fontExtension[4];
    try {
        if(sourceFile.length() < 5)
            return -3;
        if(destinationPath.length() < 5)
            return -4;

        strcpy(fontExtension, &sourceFile.c_str()[sourceFile.length() - 4]);
        for (long i = 0; i < 4; i++)
			fontExtension[i] = toupper(fontExtension[i]);
        
        if (strcmp(fontExtension, ".TTC") == 0 || strcmp(fontExtension, ".OTC") == 0)
            fontsConverted = gf_font_converter::ttcToTtfConvert(sourceFile, sourcePath, destinationPath, index, fontsList);
        else
        {
            char destinationFilePath[LIB_MAXPATH_SIZE-1];
            DPRINT("single files converter start")
			sourcePath += ('/' + sourceFile);
			//destinationPath += ('/' + std::to_string(index) + '_' + sourceFile);

            fontsConverted = convert_font(sourcePath.c_str(), destinationPath.c_str(), sourceFile.c_str(), index, destinationFilePath);
            if(fontsConverted > 0) {
                fontsList->pathsTable = new FontsPathS;
                if(!fontsList->pathsTable)
                    return -5;
                strncpy(fontsList->pathsTable->fontPath, destinationFilePath, LIB_MAXPATH_SIZE);
                fontsList->structureSize = 1;
            }
        }
    }
    catch(...) {
        /* error handling */
        return -6;
    }
    return fontsConverted;
}

int32_t gf_font_converter::ttcToTtfConvert(std::string sourceFile, std::string sourcePath, std::string destinationPath, int index, FontsListS *fontsList)
{
    DPRINT("multifile converter start")
    std::vector<std::string> fontsListStack;
	char* ttcBuffer = NULL;
	uint32_t ttcSize = 0, ttcConverted = 0, ttcResult;
	FILE* ttcFilePtr = NULL;
	std::string source = sourcePath + '/' + sourceFile;

	FOPEN(ttcFilePtr, source.c_str(), "rt");

	if (ttcFilePtr == NULL)
		return -7;

	ttcBuffer = (char*)malloc(200 * sizeof(char));
	fread(ttcBuffer, sizeof(char), 199, ttcFilePtr);
	fclose(ttcFilePtr);

	if (*(int32_t*)ttcBuffer != *(int32_t*)"ttcf")
		return -8;
	else {
		ttcSize = *(uint32_t*)&ttcBuffer[0x08];
        endswap(&ttcSize);

		char subFontSource[LIB_MAXPATH_SIZE], subFontDestin[LIB_MAXPATH_SIZE], subFontResPath[LIB_MAXPATH_SIZE];
		for (uint32_t i = 0; i < ttcSize; i++)
		{
			sprintf(subFontSource, "%s(%d)", source.c_str(), i);
			sprintf(subFontDestin, "%d_%s.otf", i, sourceFile.c_str());
			ttcResult = convert_font(subFontSource,  destinationPath.c_str(), subFontDestin, index, subFontResPath);
			if (ttcResult > 0) {
				++ttcConverted;
				fontsListStack.push_back(subFontResPath);
			}
		}
	}
    fontsList->pathsTable = new FontsPathS[ttcConverted];
    if(!fontsList->pathsTable)
        return -9;
    
    fontsList->structureSize = ttcConverted;
    for(uint32_t it = 0; it < ttcSize; it++) {
        strncpy(fontsList->pathsTable[it].fontPath, fontsListStack[it].c_str(), LIB_MAXPATH_SIZE);
    }
	free(ttcBuffer);
	return ttcConverted;
}

EXPORT
#ifdef __cplusplus
extern "C" {
#endif
int __cdecl copyConvertFont(const char* sourceFile, const char* sourcePath, const char* destinationPath, int index, FontsListS *fontsList)
{
    return font_convert_start(sourceFile, sourcePath, destinationPath, index, fontsList);
}
#ifdef __cplusplus
}
#endif
// add handling of font uninitialize 

