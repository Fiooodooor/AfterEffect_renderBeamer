
#include "GF_AEGP_LibraryLoader.h"

LibLoaderInterface::LibLoaderInterface() : libraryLoaded(false), libraryInstance(NULL), lastFunctionPointer(NULL) { }
LibLoaderInterface::~LibLoaderInterface() { if (libraryInstance) unloadTheLibrary(); }

bool LibLoaderInterface::loadLibraryFromPathW(const std::wstring path_string)
{
    utfPath = std::wstring(path_string);
    return loadLibraryFromPath(std::string(path_string.begin(), path_string.end()));
}
bool LibLoaderInterface::isLibraryLoaded()
{
    return (this->libraryLoaded && this->libraryInstance);
}
void LibLoaderInterface::unloadTheLibrary()
{
    libraryInstance = NULL;
}

PlatformLibLoader::PlatformLibLoader() : LibLoaderInterface() { }
PlatformLibLoader::~PlatformLibLoader() { }

#ifdef AE_OS_WIN
bool PlatformLibLoader::loadLibraryFromPath(const std::string path_string)
{
    try {
        uniPath = path_string;
        this->libraryInstance = (void*)LoadLibraryW(utfPath.c_str());
        if (this->libraryInstance)
            this->libraryLoaded = true;
        return this->libraryLoaded;
    }
    catch(...) {
        this->libraryInstance = NULL;
        this->libraryLoaded = true;
    }
    return false;
}
void PlatformLibLoader::unloadTheLibrary()
{
    try {
        if (isLibraryLoaded()) {
            FreeLibrary((HMODULE)libraryInstance);
            this->libraryLoaded = false;
            this->libraryInstance = NULL;
            this->lastFunctionPointer = NULL;
        }
    }
    catch (...) {
        this->libraryLoaded = false;
    }
}
bool PlatformLibLoader::loadFunctionDefinition(void** functionPt, const char* functionName)
{
    try {
        (*functionPt) = nullptr;
        this->lastFunctionPointer = NULL;
        if (isLibraryLoaded()) {
            this->lastFunctionPointer = (void*)GetProcAddress((HMODULE)this->libraryInstance, functionName);
            if(this->lastFunctionPointer) {
                (*functionPt) = this->lastFunctionPointer;
                return true;
            }
        }
    }
    catch (...) {
        this->libraryLoaded = NULL;
        return false;
    }
    return false;
}

#else

#include <dlfcn.h>

bool PlatformLibLoader::loadLibraryFromPath(const std::string path_string)
{
    try {
        uniPath = path_string;
        this->libraryInstance = (void*)dlopen(uniPath.c_str() ,RTLD_LOCAL|RTLD_LAZY);
        if (this->libraryInstance)
            this->libraryLoaded = true;
        return this->libraryLoaded;
    }
    catch (...) {
        this->libraryInstance = NULL;
        this->libraryLoaded = true;
    }
    return false;
}

void PlatformLibLoader::unloadTheLibrary()
{
    try {
        if (isLibraryLoaded()) {
            dlclose((void*)libraryInstance);
            this->libraryLoaded = false;
            this->libraryInstance = NULL;
            this->lastFunctionPointer = NULL;
        }
    }
    catch (...) {
        this->libraryLoaded = false;
    }
}

bool PlatformLibLoader::loadFunctionDefinition(void** functionPt, const char* functionName)
{
    try {
        (*functionPt) = nullptr;
        this->lastFunctionPointer = NULL;
        if (isLibraryLoaded()) {
            this->lastFunctionPointer = dlsym((void*)this->libraryInstance, functionName);
            if(this->lastFunctionPointer) {
                (*functionPt) = this->lastFunctionPointer;
                return true;
            }
        }
    }
    catch (...) {
        this->libraryLoaded = NULL;
        return false;
    }
    return false;
}

#endif
