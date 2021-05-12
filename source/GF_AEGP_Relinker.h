#pragma once
#ifndef GF_AEGP_RELINKER_H
#define GF_AEGP_RELINKER_H

#include "GF_GlobalTypes.h"
#include "GF_GlobalClasses.h"
#include "NodeObjects/AeConteiner.h"
#include "GF_AEGP_LibraryLoader.h"

#include "GF_LIBS_C4D_Relinker.h"
#include "GF_LIBS_FontConverter.h"

namespace RenderBeamer {

class GF_AEGP_Relinker
{
public:
	GF_AEGP_Relinker(SPBasicSuite *basicSuite, AEGP_PluginID pluginId);
	~GF_AEGP_Relinker();
	
	ErrorCodesAE RelinkerInitialize(beamerParamsStruct *tmpBps, A_Boolean batchRelink = FALSE);

	A_Err RelinkProject(AEGP_ProjectH projectH);

	ErrorCodesAE CopyFont(AeFontNode *node, A_long id, std::vector<std::string> &fontsList);

    ErrorCodesAE CopyConvertFontLib(PlatformLibLoader* libIt, fs::path &nodePath, fs::path &nodeRelinked, A_long id, std::vector<std::string> &fontsList);
	static bool GFCopyFile(const std::string &UID, fs::path oldFootagePath, fs::path &tmpNewFootagePath, bool forceNoRename = false, bool forceNoSymlinks = false);
	static ErrorCodesAE GFCopy_C4D_File(PlatformLibLoader* libIt, const fs::path &oldFootagePath, const fs::path &tmpNewFootagePath, const fs::path &remoteFootagePath, const std::string  &id);

	void setPathsStruct(beamerParamsStruct *beamerParamsS);
	PlatformLibLoader *GetC4dLibloader();
	ErrorCodesAE unloadFontLibrary();
	
private:
	SPBasicSuite *sP;
	AEGP_SuiteHandler suites;
	AEGP_PluginID pluginId;
	PlatformLibLoader *c4d_interface;
	PlatformLibLoader *fonts_interface;

	FILE *out;

	fs::path newProjectPath;
	A_UTF16Char projectSavePath[AEGP_MAX_PATH_SIZE];

	beamerParamsStruct *bps;
};

} // namespace RenderBeamer
#endif
