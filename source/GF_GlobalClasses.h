#pragma once
#ifndef GF_GLOBAL_CLASSES
#define GF_GLOBAL_CLASSES

#include "GF_GlobalTypes.h"
#include "GF_AEGP_Strings.h"
#include <vector>
class rbFontNode;

class rbUtilities
{
public:
    static ErrorCodesAE execScript(SPBasicSuite *pb, AEGP_PluginID pluginId, const A_char *inScript, A_char *outAchar, A_long maxLength);
    static A_Err copyMemhandleToAChar(SPBasicSuite *pb, AEGP_MemHandle& inputString, A_char* destination, A_long maxLength);

    static size_t utf16Length(A_UTF16Char* in);
	static A_Err copyMemhUTF16ToPath(SPBasicSuite *pb, AEGP_MemHandle& inputString, fs::path &resPath);
    static A_Err copyMemhUTF16ToString(SPBasicSuite *pb, AEGP_MemHandle& inputString, std::string &resString);
    static A_Err copyMemhUTF16ToString(SPBasicSuite *pb, AEGP_MemHandle& inputString, std::wstring &resString);

    static void toUTF16(const wchar_t* inputString, A_UTF16Char* destination, A_long maxLength);
    static void copyConvertStringLiteralIntoUTF16(const wchar_t* inputString, A_UTF16Char* destination, A_long maxLength);

	static A_Err getFontFromLayer(SPBasicSuite *pb, AEGP_PluginID pluginId, A_long itemNr, A_long layerNr, A_char* font, A_char* family, A_char* location);

    static void pathStringFixIllegal(fs::path &path, bool dissalowed=true, bool cut_extension=false);
    static void replaceDisallowedOnly(wchar_t *sourceStr);
    static void replaceDisallowedOnly(A_char *sourceStr);
    static void leaveAllowedOnly(A_char *sourceStr);
    static void leaveAllowedOnly(wchar_t *sourceStr);
    
	static void getTimeString(char *buff, A_long buffSize, bool path =false);
    static void getTimeString(wchar_t *buff, A_long buffSize, bool path=false);
    static void getEnvVariable(const char *variable, char *buffer, unsigned long long size);

    static ErrorCodesAE execBeamerCmd(SPBasicSuite *pb, BeamerMasks mask, wchar_t *buffor, A_long size, beamerParamsStruct bps);
    static void execCmd(SPBasicSuite *pb, wchar_t* theTmpCmd, wchar_t *buffor, A_long size, const wchar_t *outFileName);
	
	static ErrorCodesAE getVersionString(A_char* buff, long buff_size);
	static ErrorCodesAE getCpuInfoString(SPBasicSuite *pb, wchar_t *buffer, A_long buffer_length, const wchar_t *outFileName);
    static ErrorCodesAE encodeUrlString(const wchar_t *url, wchar_t *buffor, long buffor_size);
	static ErrorCodesAE openCostCalculator(SPBasicSuite *pb);
    static std::string toUtf8(const wchar_t* stringToConvert);
};


class rbProjectClass
{
public:
    rbProjectClass();

    bool createLogger(const wchar_t* file, int mode);
    bool openLogger();
	bool appendLogger();
   

	void logg(const char* object, const char* type, const char* message, bool nLine = true);
    void logg(const wchar_t* object, const wchar_t* type, const wchar_t* message, bool nLine = true);
	void loggErr(const char* object, const char* type, const char* message, bool nLine = true);
	void loggErr(const wchar_t* object, const wchar_t* type, const wchar_t* message, bool nLine = true);
    
    void loggA(int objectN, ...);
    void loggW(int objectN, ...);

	std::wfstream logger;

private:
	TCHAR logFilePathRenderBeamer[AEGP_MAX_PATH_SIZE];
	int logFileMode;
    void getTimeString() { rbUtilities::getTimeString(timeString, 25); };
    void getTimeStringA() { rbUtilities::getTimeString(timeStringA, 25); };
    wchar_t timeString[25];
	char timeStringA[25];


};

#endif
