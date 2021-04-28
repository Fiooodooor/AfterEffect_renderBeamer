#pragma once
#ifndef GF_GLOBALTYPES_H
#define GF_GLOBALTYPES_H

#include "AEConfig.h"
#include "entry.h"

#define GF_PLUGIN_VERSION_MAJOR "19"

#define GF_PLUGIN_LANGUAGE UserEnglish
#define GF_PLUGIN_BUILD_GARAGEFARM 1    // If not, the copernicus build will be compiled
#define ALLOWED_CHARACTERSA  "_-0123456789ABCDEFGHIJKLMNOPQRSTUWVXYZabcdefghijklmnopqrstuwvxyz/\\[]:!"
#define ALLOWED_CHARACTERSW L"_-0123456789ABCDEFGHIJKLMNOPQRSTUWVXYZabcdefghijklmnopqrstuwvxyz/\\[]:!"
#define DISALLOWED_CHARACTERSA  " ~`!@#$%^&*()+=[]{};:'\"\\/|,<>/?."
#define DISALLOWED_CHARACTERSW L" ~`!@#$%^&*()+=[]{};:'\"\\/|,<>/?."
#define MAIN_BEAMER_TEMP L"beamer_temp_"
#define MAIN_PROJECT_DIR1 L"data"
#define MAIN_FOOTAGE_DIR1 L"footage"
#define MAIN_FOONT_DIR1 L"fonts"
#define MAIN_LOGS_DIR1 L"logs"

#include <cstdio>
#include <cwchar>
#include <fstream>
#include <mutex>
#include <string>

#ifdef AE_OS_WIN
#define BEAMER_SCRIPT L"renderbeamer\\renderbeamer.bat"
//#define BEAMER_SCRIPT L"renderbeamer\\renderbeamer.exe"

#define FONT_LIB_NAME L"\\libgf_font_converter.dll"
#define C4D_LIB_NAME L"\\C4dRelinkerLibrary.dll"
#define ENV_HOME_DIR "USERPROFILE"
#define SEP '\\'
#include <Windows.h>
#define RB_DUPENV(...) _dupenv_s(__VA_ARGS__)
#define RB_SPRINTF(...) sprintf_s(__VA_ARGS__)
#define RB_SWPRINTF(buff, ...) swprintf_s(buff, __VA_ARGS__)
#define RB_STRNCPTY(...) strncpy_s(__VA_ARGS__)
#define ASTRNCPY(ST1,ST2,N) strncpy_s(ST1,N+1,ST2,N);
#define WSTRNCPY(ST1,ST2,N) wcsncpy_s(ST1,N+1,ST2,N);
#define RB_LOCALESTRING "en-US.utf-8" // .utf8
#define RB_DEFINELOCALE(TLOCNAME) _locale_t TLOCNAME;
#define RB_NEWLOCALE(TLOCNAME,TLOCSTRING) TLOCNAME = _create_locale(LC_ALL, TLOCSTRING);
#define RB_FREELOCALE(TLOCNAME) _free_locale(TLOCNAME);
#define RB_WCSTOMBS_L(bts_converted,dst,dst_bytes,src,max_bytes,locale) if(_wcstombs_s_l(&bts_converted,dst,dst_bytes,src,max_bytes,locale)==0) bts_converted-=1;
#define TCHAR wchar_t
#define TCHARPT wchar_t*
#define TSTRLEN(...) wcslen(__VA_ARGS__)
#define TSTRNCPY(ST1,ST2,N) wcsncpy_s(ST1,N+1,ST2,N);
#define TSTRING wstring

#elif defined AE_OS_MAC
#define BEAMER_SCRIPT L"renderbeamer/renderbeamer.sh"
#define FONT_LIB_NAME L"/renderBeamer.plugin/Contents/Resources/libgf_font_converter.dylib"
#define C4D_LIB_NAME L"/renderBeamer.plugin/Contents/Resources/libC4dRelinkerLibrary.dylib"
#define ENV_HOME_DIR "HOME"
#define SEP '/'
#define RB_DUPENV(...) getenv(__VA_ARGS__)
#define RB_SPRINTF(...) snprintf(__VA_ARGS__)
#define RB_SWPRINTF(buff, ...) swprintf(buff, __VA_ARGS__)
#define RB_STRNCPTY(...) strncpy(__VA_ARGS__)
#define ASTRNCPY(ST1,ST2,N) strncpy(ST1,ST2,N);
#define WSTRNCPY(ST1,ST2,N) wcsncpy(ST1,ST2,N);
#define RB_LOCALESTRING "en_US.UTF-8"
#define RB_DEFINELOCALE(TLOCNAME) locale_t TLOCNAME;
#define RB_NEWLOCALE(TLOCNAME,TLOCSTRING) TLOCNAME = newlocale(LC_ALL_MASK, TLOCSTRING ,NULL);
#define RB_FREELOCALE(TLOCNAME) freelocale(TLOCNAME);
#define RB_WCSTOMBS_L(bts_converted,dst,dst_bytes,src,max_bytes,locale) bts_converted=wcstombs_l(dst,src,max_bytes,locale);
#define TCHAR char
#define TCHARPT char*
#define TSTRLEN(...) strlen(__VA_ARGS__)
#define TSTRNCPY(...) strncpy(__VA_ARGS__);
#define TSTRING string

#define USE_BOOST 1
#endif

#ifndef USE_BOOST
#include <filesystem>
namespace fs = std::filesystem;
#define FS_COPY_DEFINE(NAME) fs::copy_options NAME;
#define FS_COPY_OPTIONS fs::copy_options::overwrite_existing
#define FS_ERROR_CODE(NAME) std::error_code NAME;
#define FS_ERROR_ASSIGN(NAME,NR) NAME.assign(NR, std::generic_category());
#define FS_REPLACE_FILENAME(MY_PTH, MY_ID) (MY_PTH)->pathRelinked.replace_filename(std::to_string(MY_ID) + '_' + (MY_PTH)->path.filename().string());
#define FS_U8STRING(MY_PTH) MY_PTH.u8string()
#define WOPEN(FILEH,FILEP,PARAMS) FILEH.open((FILEP).wstring().c_str(), PARAMS)
#define FS_TYPE_UNKNOWN fs::file_type::unknown
#define FS_TYPE_NONE fs::file_type::none
#else
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#define FS_COPY_DEFINE(NAME) fs::copy_option NAME;
#define FS_COPY_OPTIONS fs::copy_option::overwrite_if_exists
#define FS_ERROR_CODE(NAME) boost::system::error_code NAME;
#define FS_ERROR_ASSIGN(NAME,NR) NAME.assign(NR, boost::system::generic_category());
#define FS_REPLACE_FILENAME(MY_PTH, MY_ID) MY_PTH->pathRelinked.remove_filename(); MY_PTH->pathRelinked.append(std::to_string(MY_ID) + '_' + MY_PTH->path.filename().string());
#define FS_U8STRING(MY_PTH) MY_PTH.string()
#define WOPEN(FILEH,FILEP,PARAMS) FILEH.open(FILEP.string().c_str(), PARAMS)
#define FS_TYPE_UNKNOWN fs::file_type::type_unknown
#define FS_TYPE_NONE fs::file_type::status_error
#endif

#include "AE_GeneralPlug.h"
#include "AE_Macros.h"

#include "AE_GeneralPlugPanels.h"
#include "AEGP_SuiteHandler.h"
#include "GF_AEGP_ErrorsHandling.h"
#include "AE_GeneralPlugPanels.h"
#include "SuiteHelper.h"
namespace RenderBeamer {
typedef enum StrIDType {
    StrID_NONE=0,
    StrID_Name,
	StrID_MenuBatch,
    StrID_MenuCost,
    StrID_AboutMenu,
    StrID_Description,
    StrID_LoadPluginError,
    StrID_AboutDialogText,
	StrID_CollectSmartWarning,
	StrID_OutputFormatPNGWarning,
	StrID_ProjectSent,
    StrID_NUMTYPES
} StrIDType;

typedef enum BeamerMasks {
    BeamerMask_GetUser,
    BeamerMask_GetTemp,
    BeamerMask_SendTask,
    BeamerMask_CheckScene,
    BeamerMask_GetFont,
    BeamerMask_GetFontFamily,
    BeamerMask_GetFontPath,
	BeamerMask_SendTaskEncoded,
	BeamerMask_SendLogFile,
	BeamerMask_GetLocalPort,
    _BeamerMask_ItemsN
} BeamerMasks;

typedef struct {
    A_long    index;
    A_char        str[256];
} TableString;

typedef struct {
    fs::path originalProject;
    fs::path relinkedSavePath;
    fs::path relProjPath;
    fs::path tempLogPath;
    fs::path tempLogFile;
    fs::path relGfsFile;
    fs::path relinkedSceneRoot;
    fs::path footageMainOutput;
    fs::path fontsMainOutput;
    fs::path logsMainOutput;
    fs::path rqMainOutput;
    fs::path remoteProjectPath;
    fs::path remote_renders_path;
    fs::path remoteFootagePath;
	fs::path remoteFontsPath;
    fs::path projectRootCorrect;
    fs::path projectFilenameCorrect;    
    fs::path tempPrefix;
    fs::path tempSufix;
} beamerPaths;
/*
#ifdef AE_OS_MAC
    typedef struct {
        A_char              versionStr[32];
        A_char              userPath[AEGP_MAX_PATH_SIZE];
        fs::path            beamerTmpFile;
        std::string         projectPath;
        std::string         pluginPath;
        std::string         fontLibPath;
        std::string         c4d_LibPath;
        A_char              beamerTmpPath[AEGP_MAX_PATH_SIZE];
        A_char              beamerScript[AEGP_MAX_PATH_SIZE];
        A_char              beamerExecScript[2048];
        A_char              rmtUser[16];
        A_char              timeString[20];
        A_char              beamerVersionFilename[AEGP_MAX_PATH_SIZE];
        A_long              colectedItems;
        A_long              currentItem;
        beamerPaths         bp;
    } beamerParamsStruct;
#else
 */
    typedef struct {
		std::mutex			m;
        A_char              versionStr[32];       
        fs::path            beamerTmpFile;
        std::wstring        projectPath;
        std::wstring        pluginPath;
        std::wstring        fontLibPath;
        std::wstring        c4d_LibPath;
		A_UTF16Char			original_project[AEGP_MAX_PATH_SIZE];
        wchar_t             beamerTmpPath[AEGP_MAX_PATH_SIZE];
        fs::path            beamerScript;
        A_char              beamerExecScript[2048];
        wchar_t             rmtUser[16];
		wchar_t				socketPort[16];
		long				socketPort_long;
        wchar_t             timeString[20];
        A_char              timeStringA[20];
        wchar_t             beamerVersionFilename[AEGP_MAX_PATH_SIZE];
        A_long              colectedItems;
        A_long              currentItem;
        beamerPaths         bp;
    } beamerParamsStruct;
//#endif

} // namespace RenderBeamer
#endif
