//
//  Garagefarm_c4d_relinker.hpp
//  Garagefarm c4d relinker
//
//  Created by Milosz Linkiewicz on 12/05/2021r.
//  Copyright © 2021 garageFarm.net. All rights reserved.
//
#ifndef Garagefarm_c4d_relinker_
#define Garagefarm_c4d_relinker_

#if defined(_WIN64) || defined(_WIN32)
    #define OS_WIN 1
    #define OS_CASE 0
#elif __APPLE__
    #define OS_MACOS 1
    #define OS_CASE 1
    #define EXPORT __attribute__((visibility("default")))
#endif

#define LIB_C4D_MAXPATH 1040 // Macos 4 bit for wchar_t (4xchar) [unicode]

#ifdef OS_MACOS
#pragma GCC visibility push(default)
#endif

typedef struct tagFileNode {
    bool isUrl;
    char file[LIB_C4D_MAXPATH];
    char relinkedPath[LIB_C4D_MAXPATH];
    char relinkedFile[LIB_C4D_MAXPATH];
} FileNode;

typedef struct {
    unsigned long long stackSize;
    int errorCode;
    char loadFile[LIB_C4D_MAXPATH];
    char saveFile[LIB_C4D_MAXPATH];
    char relinkPath[LIB_C4D_MAXPATH];
} c4dStruct;

namespace cinewareRelinker
{
    typedef enum errorCodes {
		err_OPEN = -1,
		err_CLOSE = -2,
		err_READ = -3,
		err_WRITE = -4,
		err_SEEK = -5,
		err_INVALID = -6,
		err_OUTOFMEMORY = -7,
		err_USERBREAK = -8,
		err_WRONG_VALUE = -100,
		err_CHUNK_NUMBER = -102,
		err_VALUE_NO_CHUNK = -103,
		err_FILE_END = -104,
		err_UNKNOWN_VALUE = -105,
        err_NoError = 0,
        err_LoadPathEmpty,
    	err_LoadFileDoNotExist,
    	err_LoadFileIdentifyError,
        err_LoadFileError,
    	err_LoadFileAllocation,
        err_SaveFileError,
        err_InputStackError,
        err_InputPointerError,
        err_UnknownError,
        err_UnknownException,
        err_UnknownLoadSceneException
    } errorCodes;
}

typedef int( *_f_getAssetsListAndRelink)(c4dStruct* in_data, FileNode** out_data);

extern "C" {
#ifdef OS_WIN
    __declspec(dllexport) int getAssetsListAndRelink(c4dStruct* in_data, FileNode** out_data);
#else
    EXPORT
        int getAssetsListAndRelink(c4dStruct* in_data, FileNode** out_data);
#endif
}

#ifdef OS_MACOS
#pragma GCC visibility pop
#endif

#endif
