
#pragma once
#ifndef GF_AEGP_LIBRARYLOADER_H
#define GF_AEGP_LIBRARYLOADER_H

#include "GF_GlobalTypes.h"

class LibLoaderInterface
{
public:
    LibLoaderInterface();
    ~LibLoaderInterface();

    bool loadLibraryFromPathW(const std::wstring path_string);
    virtual bool loadLibraryFromPath(const std::string path_string) = 0;
    
    bool isLibraryLoaded();
    
    virtual void unloadTheLibrary();

protected:
   // virtual void* functionLibAddress(const char* functionName) = 0;
    bool libraryLoaded;
    void* libraryInstance;
    void* lastFunctionPointer;
    std::string uniPath;
    std::wstring utfPath;
};

class PlatformLibLoader : public LibLoaderInterface
{
public:
    PlatformLibLoader();
    ~PlatformLibLoader();

    virtual bool loadLibraryFromPath(const std::string path_string) override;
    virtual void unloadTheLibrary() override;
    
    bool loadFunctionDefinition(void** functionPt, const char* functionName);
    
};

#endif
