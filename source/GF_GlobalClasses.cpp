#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "GF_GlobalClasses.h"

#include <ctime>
#include <vector>
#include <stdarg.h>
#include "base64.h"

rbProjectClass::rbProjectClass() : logFilePathRenderBeamer{ '\0'}, logFileMode(0) {}

bool rbProjectClass::appendLogger()
{
	if (logger.is_open()) {
		logger.flush();
		logger.close();
	}

	if (logFilePathRenderBeamer[0] != '\0') {
		logger.open(logFilePathRenderBeamer, std::fstream::out | std::fstream::app);
	}
	else
		return false;

	return logger.is_open();
}

void rbProjectClass::loggErr(const char* object, const char* type, const char* message, bool nLine)
{
	if (appendLogger() == false)
		return;
	getTimeStringA();
	logger << timeStringA << "::ERROR::" << object << "::" << type << "::" << message;
	if (nLine)
		logger << std::endl;

}
void rbProjectClass::loggErr(const wchar_t* object, const wchar_t* type, const wchar_t* message, bool nLine)
{
	if (appendLogger() == false)
		return;
	getTimeString();
    logger << timeString << "::ERROR::" << rbUtilities::toUtf8(object).c_str() << "::" << rbUtilities::toUtf8(type).c_str() << "::" << rbUtilities::toUtf8(message).c_str();
	if (nLine)
		logger << std::endl;
}
void rbProjectClass::logg(const char* object, const char* type, const char* message, bool nLine)
{
	if (appendLogger() == false)
		return;
	getTimeStringA();
	logger << timeStringA << "::INFO::" << object << "::" << type << "::" << message;
	if (nLine)
		logger << std::endl;
}
void rbProjectClass::logg(const wchar_t* object, const wchar_t* type, const wchar_t* message, bool nLine)
{
	if (appendLogger() == false)
		return;
	getTimeStringA();

    logger << timeStringA << "::INFO::" << rbUtilities::toUtf8(object).c_str() << "::" << rbUtilities::toUtf8(type).c_str() << "::" << rbUtilities::toUtf8(message).c_str();
    if (nLine)
        logger << std::endl;
}
void rbProjectClass::loggA(int objectN, ...)
{
    if (appendLogger() == false)
        return;
    getTimeStringA();
    logger << timeStringA << "::INFO";
    va_list aMsg;
    va_start(aMsg, objectN);
    for(int it=0; it<objectN; it++) {
        logger << "::" << va_arg(aMsg, const char*);
    }
    logger << std::endl;
    va_end(aMsg);
}
void rbProjectClass::loggW(int objectN, ...)
{
    if (appendLogger() == false)
        return;
    getTimeStringA();
    logger << timeStringA << "::INFO";
    va_list wMsg;
    va_start(wMsg, objectN);
    for(int it=0; it<objectN; it++) {
        logger << "::" << rbUtilities::toUtf8(va_arg(wMsg, const wchar_t*)).c_str();
    }
    logger << std::endl;
    va_end(wMsg);
}

// std::fstream::app | std::fstream::out 
bool rbProjectClass::createLogger(const wchar_t* file, int mode)
{
	logFileMode = mode;
#ifndef AE_OS_MAC
	wcsncpy_s(logFilePathRenderBeamer, AEGP_MAX_PATH_SIZE, file, AEGP_MAX_PATH_SIZE - 1);//logFilePathRenderBeamer logFilePathProject
#else
	sprintf(logFilePathRenderBeamer, "%ls", file);
#endif
	try {
		logger.imbue(std::locale(RB_LOCALESTRING));
	}
	catch (...) {
		logger.imbue(std::locale("en-US"));
	}
	logger.open(logFilePathRenderBeamer, logFileMode);
	bool retVal = logger.is_open();
	if (logger.is_open())
		logger.close();
	return retVal;
}
bool rbProjectClass::openLogger()
{
	if (logger.is_open())
		logger.close();

	logger.open(logFilePathRenderBeamer, std::fstream::app | std::fstream::out);
	return logger.is_open();
}
A_Err rbUtilities::getFontFromLayer(SPBasicSuite *pb, AEGP_PluginID pluginId, A_long itemNr, A_long layerNr, A_char* font, A_char* family, A_char* location)
{
    PT_XTE_START{
        AEGP_SuiteHandler suites(pb);
        A_char tmpScript[256] = { '\0' };
        suites.ANSICallbacksSuite1()->sprintf(tmpScript, GetBeamerMaskA(BeamerMask_GetFont), itemNr, layerNr);
        PT_ETX(execScript(pb, pluginId, tmpScript, font, AEGP_MAX_PATH_SIZE));
        suites.ANSICallbacksSuite1()->sprintf(tmpScript, GetBeamerMaskA(BeamerMask_GetFontFamily), itemNr, layerNr);
        PT_ETX(execScript(pb, pluginId, tmpScript, family, AEGP_MAX_PATH_SIZE));
        suites.ANSICallbacksSuite1()->sprintf(tmpScript, GetBeamerMaskA(BeamerMask_GetFontPath), itemNr, layerNr);
        PT_ETX(execScript(pb, pluginId, tmpScript, location, AEGP_MAX_PATH_SIZE));
    } PT_XTE_CATCH_RETURN_ERR;
}

ErrorCodesAE rbUtilities::execScript(SPBasicSuite *pb, AEGP_PluginID pluginId, const A_char *inScript, A_char *outAchar, A_long maxLength)
{
	ERROR_CATCH_START
		AEGP_SuiteHandler suites(pb);
        AEGP_MemHandle result = NULL;
        AEGP_MemHandle error = NULL;
        A_Err errRes = suites.UtilitySuite6()->AEGP_ExecuteScript(pluginId, inScript, FALSE, &result, &error);
        if (errRes == A_Err_NONE) {
            copyMemhandleToAChar(pb, result, outAchar, maxLength);
            PT_ETX(suites.MemorySuite1()->AEGP_FreeMemHandle(error));
            return ErrorCodesAE::NoError;
        }
        else {
            copyMemhandleToAChar(pb, error, outAchar, maxLength);
            PT_ETX(suites.MemorySuite1()->AEGP_FreeMemHandle(result));
            return ErrorCodesAE::ScriptExecuteError;
        }
	ERROR_CATCH_END_NO_INFO_RETURN
}

A_Err rbUtilities::copyMemhandleToAChar(SPBasicSuite *pb, AEGP_MemHandle& inputString, A_char* destination, A_long maxLength)
{
    AEGP_SuiteHandler suites(pb);
    PT_XTE_START{
        A_char* tempBuffor;

        PT_ETX(suites.MemorySuite1()->AEGP_LockMemHandle(inputString, reinterpret_cast<void**>(&tempBuffor)));
		if (!destination)
			_err = A_Err_ALLOC;
		else if (!tempBuffor)
			destination[0] = '\0';
		else {
		#ifdef AE_OS_MAC
			strncpy(reinterpret_cast<char*>(destination), tempBuffor, maxLength - 1);
		#else
			strncpy_s(reinterpret_cast<char*>(destination), maxLength, tempBuffor, maxLength - 1);
		#endif
			destination[maxLength - 1] = '\0';
		}
        PT_ETX(suites.MemorySuite1()->AEGP_UnlockMemHandle(inputString));
        PT_ETX(suites.MemorySuite1()->AEGP_FreeMemHandle(inputString));
    } PT_XTE_CATCH_RETURN_ERR;
}

size_t rbUtilities::utf16Length(A_UTF16Char* in)
{
	size_t i = 0;
	A_UTF16Char itChar;
	if (in) {
		do {
			itChar = in[i++];
		} while (itChar != 0);
		return i;
	}
	return 0;
}

A_Err rbUtilities::copyMemhUTF16ToPath(SPBasicSuite *pb, AEGP_MemHandle& inputString, fs::path &resPath)
{
	A_Err err = A_Err_NONE;
    #ifndef USE_BOOST
	AEGP_SuiteHandler suites(pb);
	char16_t* res16B = NULL;
	ERR(suites.MemorySuite1()->AEGP_LockMemHandle(inputString, reinterpret_cast<void**>(&res16B)));	
	if (err == A_Err_NONE) {
		resPath = res16B;
	}
	ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(inputString));
	ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(inputString));
    #else
    std::wstring result;
    err = rbUtilities::copyMemhUTF16ToString(pb, inputString, result);
    if (err == A_Err_NONE) {
       resPath = result;
    }
    #endif
	return err;
}
A_Err rbUtilities::copyMemhUTF16ToString(SPBasicSuite *pb, AEGP_MemHandle& inputString, std::string &resString)
{
    A_Err err = A_Err_NONE;
    std::wstring resStringW;
    err = rbUtilities::copyMemhUTF16ToString(pb, inputString, resStringW);
    if (!err) {
		if (resStringW.empty())
			resString = "";
        else 
			resString = std::string(resStringW.begin(), resStringW.end());
    }
    return err;
}

A_Err rbUtilities::copyMemhUTF16ToString(SPBasicSuite *pb, AEGP_MemHandle& inputString, std::wstring &resString)
{
    A_Err err = A_Err_NONE;
    AEGP_SuiteHandler suites(pb);
	try {
		A_UTF16Char* res16B = NULL;
		resString = L"";
		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(inputString, reinterpret_cast<void**>(&res16B)));
		if (!err) {
			size_t length = rbUtilities::utf16Length(res16B) - 1;
			if (length > 0)
			{
#ifdef AE_OS_MAC
				CFRange range = { 0, AEGP_MAX_PATH_SIZE };
				range.length = length;
				wchar_t *destination = new wchar_t[length + 1];

				CFStringRef inputStringCFSR = CFStringCreateWithBytes(kCFAllocatorDefault, reinterpret_cast<const UInt8 *>(res16B), length * sizeof(A_UTF16Char), kCFStringEncodingUTF16LE, FALSE);
				CFStringGetBytes(inputStringCFSR, range, kCFStringEncodingUTF32LE, 0, FALSE, reinterpret_cast<UInt8 *>(destination), length * (sizeof(kCFStringEncodingUTF32LE)), NULL);
				CFRelease(inputStringCFSR);

				destination[length] = 0;
				std::wstring ws(destination);

				resString = std::wstring(ws.begin(), ws.end());
				delete[] destination;
#else
				std::wstring ws(reinterpret_cast<wchar_t*>(res16B));
				resString = std::wstring(ws.begin(), ws.end());
#endif
			}
		}
        ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(inputString));
        ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(inputString));
    }
    catch (...) {
        return A_Err_GENERIC;
    }
    return err;
}

// Function to convert and copy string literals to A_UTF16Char.
// On Win: Pass the input directly to the output
// On Mac: All conversion happens through the CFString format
void rbUtilities::toUTF16(const wchar_t* inputString, A_UTF16Char* destination, A_long maxLength)
{
    copyConvertStringLiteralIntoUTF16(inputString, destination, maxLength);
}
void rbUtilities::copyConvertStringLiteralIntoUTF16(const wchar_t* inputString, A_UTF16Char* destination, A_long maxLength)
{
#ifdef AE_OS_MAC
    size_t length = wcslen(inputString);
    CFRange    range = { 0, AEGP_MAX_PATH_SIZE };
    range.length = length;
    CFStringRef inputStringCFSR = CFStringCreateWithBytes(kCFAllocatorDefault, reinterpret_cast<const UInt8 *>(inputString), length * sizeof(wchar_t), kCFStringEncodingUTF32LE, FALSE);
    CFStringGetBytes(inputStringCFSR, range, kCFStringEncodingUTF16, 0, FALSE, reinterpret_cast<UInt8 *>(destination), length * (sizeof(A_UTF16Char)), NULL);
    destination[length] = 0; // Set NULL-terminator, since CFString calls don't set it
    CFRelease(inputStringCFSR);
#elif defined AE_OS_WIN
    wcsncpy_s(reinterpret_cast<wchar_t*>(destination), maxLength, inputString, maxLength - 1);
#endif
}

void rbUtilities::pathStringFixIllegal(fs::path &path, bool dissalowed, bool cut_extension)
{
    unsigned long long length;
	fs::path extension;
	fs::path tempPath = path;
	if (tempPath.has_extension()) {
		extension = tempPath.extension();
		tempPath = tempPath.replace_extension();
	}
    
    length = TSTRLEN(tempPath.TSTRING().c_str()) + 1;
    TCHAR *tmp_str = new TCHAR[length];

    if (tmp_str)
    {
        TSTRNCPY(tmp_str, tempPath.TSTRING().c_str(), length - 1);

        if(dissalowed) {
            replaceDisallowedOnly(tmp_str);
        }
        else {
            leaveAllowedOnly(tmp_str);
        }
#ifndef USE_BOOST
        path.assign(tmp_str);
#else
        path.assign(tmp_str, tmp_str + length);
#endif
		if (!extension.empty() && !cut_extension) {
			path.replace_extension(extension);
		}
        delete[] tmp_str;
    }
}

void rbUtilities::replaceDisallowedOnly(wchar_t *sourceStr)
{
    unsigned long long charNrPos = 0;
    wchar_t *analizedString = sourceStr;
    wchar_t *mask = DISALLOWED_CHARACTERSW;

    while (analizedString && *analizedString)
    {
        charNrPos = wcscspn(analizedString, mask);
        analizedString += charNrPos;
        if (analizedString[0] == '\0')
            break;
        else
            analizedString[0] = '_';
    }
}

void rbUtilities::replaceDisallowedOnly(A_char *sourceStr)
{
    unsigned long long charNrPos = 0;
    A_char *analizedString = sourceStr;
    A_char *mask = DISALLOWED_CHARACTERSA;

    while (analizedString && *analizedString)
    {
        charNrPos = strcspn(analizedString, mask);
        analizedString += charNrPos;
        if (analizedString[0] == '\0')
            break;
        else
            analizedString[0] = '_';
    }
}

void rbUtilities::leaveAllowedOnly(wchar_t *sourceStr)
{
    unsigned long long charNrPos = 0;
    wchar_t *analizedString = sourceStr;
    wchar_t *mask = ALLOWED_CHARACTERSW;

    while (analizedString && *analizedString)
    {
        charNrPos = wcsspn(analizedString, mask);
        analizedString += charNrPos;
        if (analizedString[0] == '\0')
            break;
        else
            analizedString[0] = '_';
    }
}
void rbUtilities::leaveAllowedOnly(A_char *sourceStr)
{
    unsigned long long charNrPos = 0;
    A_char *analizedString = sourceStr;
    A_char *mask = ALLOWED_CHARACTERSA;

    while (analizedString && *analizedString)
    {
        charNrPos = strspn(analizedString, mask);
        analizedString += charNrPos;
        if (analizedString[0] == '\0')
            break;
        else
            analizedString[0] = '_';
    }
}

void rbUtilities::getTimeString(char *buff, A_long buffSize, bool path)
{
	time_t currentTime;
	tm localTime;
	time(&currentTime);
#ifdef AE_OS_MAC
	localTime = *localtime(&currentTime);
#else
	localtime_s(&localTime, &currentTime);
#endif
	if (path == false) {
		strftime(buff, buffSize, "[%Y_%m_%d-%H:%M:%S]", &localTime);
	}
	else {
		strftime(buff, buffSize, "%Y%m%d-%H%M%S", &localTime);
		//Fail safe fixes to code.Logger fixes and minor other changes.
	}
}
void rbUtilities::getTimeString(wchar_t *buff, A_long buffSize, bool path)
{
    time_t currentTime;
    tm localTime;
    time(&currentTime);
    //localtime_s(&localTime, &currentTime);
#ifdef AE_OS_MAC
    localTime = *localtime(&currentTime);
#else
    localtime_s(&localTime, &currentTime);
#endif
	if (path == false) {
		wcsftime(buff, buffSize, L"[%Y_%m_%d-%H:%M:%S]", &localTime);
	}
	else {
		wcsftime(buff, buffSize, L"%Y%m%d-%H%M%S", &localTime);
		//Fail safe fixes to code.Logger fixes and minor other changes.
	}

}

void rbUtilities::getEnvVariable(const char *variable, char *buffer, unsigned long long size)
{
    char* tmpbuffer = nullptr;
#ifdef AE_OS_WIN
    size_t bufferSize = 0;
    if (_dupenv_s(&tmpbuffer, &bufferSize, variable) == 0 && buffer != nullptr)
    {
        strcpy_s(buffer, size, tmpbuffer);
        free(tmpbuffer);
    }

#else
    tmpbuffer = getenv(variable);
    if (tmpbuffer != NULL)
        strncpy(buffer, tmpbuffer, size);
    else
        buffer[0] = '\0';
#endif
}

ErrorCodesAE rbUtilities::execBeamerCmd(SPBasicSuite *pb, BeamerMasks mask, wchar_t *buffor, A_long size, beamerParamsStruct bps)
{
	ERROR_CATCH_START
	switch (mask)
	{
	case BeamerMask_GetUser:
		RB_SWPRINTF(bps.beamerExecScript, 2048, GetBeamerMaskW(BeamerMask_GetUser), bps.beamerScript, bps.beamerTmpFile.generic_wstring().c_str());
		break;
	case BeamerMask_GetTemp:
		RB_SWPRINTF(bps.beamerExecScript, 2048, GetBeamerMaskW(BeamerMask_GetTemp), bps.beamerScript, bps.beamerTmpFile.generic_wstring().c_str());
		break;
	case BeamerMask_CheckScene:
		RB_SWPRINTF(bps.beamerExecScript, 2048, GetBeamerMaskW(BeamerMask_CheckScene), bps.beamerScript, bps.bp.projectCheckVersion.generic_wstring().c_str(), bps.rmtUser, bps.beamerTmpFile.generic_wstring().c_str());
		break;
	case BeamerMask_SendTask:
		RB_SWPRINTF(bps.beamerExecScript, 2048, GetBeamerMaskW(BeamerMask_SendTask), bps.beamerScript, bps.bp.relinkedSceneRoot.generic_wstring().c_str(), bps.bp.remoteProjectPath.generic_wstring().c_str(), bps.beamerTmpFile.generic_wstring().c_str());
		break;
	case BeamerMask_SendTaskEncoded:
		RB_SWPRINTF(bps.beamerExecScript, 2048, GetBeamerMaskW(BeamerMask_SendTaskEncoded), bps.beamerScript, base64_encode( FS_U8STRING(bps.bp.relinkedSceneRoot)).c_str(), bps.bp.remoteProjectPath.generic_wstring().c_str(), bps.beamerTmpFile.generic_wstring().c_str());
		break;
	default:
		return ErrorResult;
	}
	execCmd(pb, bps.beamerExecScript, buffor, size, bps.beamerTmpFile.generic_wstring().c_str());
	if (size > 0) {
		if (buffor == nullptr) {
			_ErrorCode = ErrorCodesAE::NullPointerResult;
		}
		else if (buffor[0] == '\0') {
			_ErrorCode = ErrorCodesAE::ErrorResult;
		}
		else if (wcscmp(buffor, L"[NULL]") == 0) {
			_ErrorCode = ErrorCodesAE::NullResult;
		}
		else if (wcsncmp(buffor, L"ERROR", 5) == 0) {
			_ErrorCode = ErrorCodesAE::ErrorResult;
		}
	}
	ERROR_CATCH_END_NO_INFO
	return _ErrorCode;
}
void rbUtilities::execCmd(SPBasicSuite *pb, wchar_t* theTmpCmd, wchar_t *bufforW, A_long bufforSize, const wchar_t *outFileW)
{
    AEGP_SuiteHandler suites(pb);
#ifdef AE_OS_WIN
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	HANDLE fileH = CreateFileW(outFileW,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		&sa,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	
    bool bProcessEnded = false;
    STARTUPINFOW si = { sizeof(STARTUPINFOW) };
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
	si.hStdInput = NULL;
	si.hStdError = fileH;
	si.hStdOutput = fileH;

    PROCESS_INFORMATION pi = { 0 };
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcessW(NULL, (LPWSTR)theTmpCmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        while (!bProcessEnded) { bProcessEnded = WaitForSingleObject(pi.hProcess, 300) == WAIT_OBJECT_0; }
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

#else
    size_t wlen = wcslen(theTmpCmd);
    char* ctmp = new char[wlen + 1];
    sprintf(ctmp, "%ls", theTmpCmd);

    system(ctmp);
    delete[] ctmp;
#endif
    if (bufforW != nullptr && outFileW != nullptr)
    {
        std::wfstream tempStr;
#ifndef USE_BOOST
        tempStr.open(outFileW, std::fstream::in);
#else
        char filename[AEGP_MAX_PATH_SIZE];
        sprintf(filename, "%ls", outFileW);
        tempStr.open(filename, std::fstream::in);
#endif
        if (tempStr.is_open())
        {
            tempStr.getline(bufforW, bufforSize);
            tempStr.close();
        }
		bufforW[bufforSize-1] = '\0';
    }
}

ErrorCodesAE rbUtilities::getVersionString(A_char* buff, long buff_size)
{
	ERROR_CATCH_START
		const std::string versionString = GetStringPtr(StrID_Name);
		//versionString += std::string("renderBeamer for After Effects v.") + std::string(GF_PLUGIN_VERSION_MAJOR);
		ASTRNCPY(buff, versionString.c_str(), buff_size);
	ERROR_CATCH_END_NO_INFO_RETURN
}

ErrorCodesAE rbUtilities::getCpuInfoString(SPBasicSuite *pb, wchar_t *buffer, A_long buffer_length, const wchar_t *outFileName)
{
	ERROR_CATCH_START
#ifdef AE_OS_WIN
	wchar_t pathBuffer[MAX_PATH];
	wchar_t regParams[512];
	GetWindowsDirectoryW(pathBuffer, MAX_PATH);
	swprintf(regParams, 512, L"%ls\\system32\\reg.exe EXPORT \"HKLM\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0\" \"%ls\" /y /reg:64", pathBuffer, outFileName);
	rbUtilities::execCmd(pb, regParams, NULL, 0, NULL);

	std::wifstream myfile(outFileName, std::ios::binary | std::wfstream::in);
	std::wstring line, subLine=L"";

	if (myfile.is_open()) 
	{
		myfile.imbue(std::locale(myfile.getloc(), new std::codecvt_utf16<wchar_t, 0xffff, std::consume_header>));
		std::wstring keyString = L"\"ProcessorNameString\"=\"";
		for (size_t poz = 0; getline(myfile, line);) {
			if ((poz = line.find(keyString)) != std::wstring::npos) {
				subLine = line.substr(poz + keyString.length(), line.length() - poz - keyString.length() - 2);
				break;
			}
		}
		myfile.close();
	}
	swprintf(buffer, buffer_length, L"%ls", subLine.c_str());
#else
	wchar_t theCmd[1024];
	RB_SWPRINTF(theCmd, 1024, L"/usr/sbin/sysctl -n \"machdep.cpu.brand_string\" > \"%ls\"", outFileName);
	rbUtilities::execCmd(pb, theCmd, buffer, buffer_length, outFileName);
#endif
	ERROR_CATCH_END_NO_INFO_RETURN
}

ErrorCodesAE rbUtilities::encodeUrlString(const wchar_t *url, wchar_t *buffor, long buffor_size)
{
	std::wstring returnUrl = L"";
    
	wchar_t c, bufHex[10];
	long ic, len = static_cast<long>(wcslen(url));

	for (long i = 0; i < len; i++) 
	{
		c = url[i];
		ic = c;
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') returnUrl += c;
		else {
			swprintf(bufHex, 10, L"%X", c);
			if (ic < 16)
				returnUrl += L"%0";
			else
				returnUrl += L"%";
			returnUrl += bufHex;
		}
	}
	WSTRNCPY(buffor, returnUrl.c_str(), buffor_size);
	return NoError;
}
ErrorCodesAE rbUtilities::openCostCalculator(SPBasicSuite *pb)
{
	ERROR_CATCH_START
    
    fs::path data_tmp =  fs::temp_directory_path();
    data_tmp += "beamerExchange.txt";
		A_long frames = 250;
	#ifdef GF_PLUGIN_BUILD_GARAGEFARM
		wchar_t theUrl[] = L"https://garagefarm.net/cost-calculator/";
	#else
		wchar_t theUrl[] = L"https://copernicuscomputing.com/pl/estimate/";
	#endif // GF_PLUGIN_BUILD_GARAGEFARM
    wchar_t theTempProcChar[512] = L"\0";
    wchar_t procEncoded[512] = L"\0";
    
	if (rbUtilities::getCpuInfoString(pb, theTempProcChar, 512, data_tmp.wstring().c_str()) != ErrorCodesAE::NoError)
        return ErrorCodesAE::ErrorResult;
    
    if(rbUtilities::encodeUrlString(theTempProcChar, procEncoded, 512) != ErrorCodesAE::NoError)
        return ErrorCodesAE::ErrorResult;
        //procEncoded[0] = '\0';
    #ifdef AE_OS_WIN
		wchar_t theTempCmd[1024];
		RB_SWPRINTF(theTempCmd, 1024, L"%ls?cpu=%ls&frames=%d", theUrl, procEncoded,  frames);
		if (32 >= reinterpret_cast<ULONG_PTR>(ShellExecuteW(NULL, L"open", theTempCmd, NULL, NULL, SW_SHOWNORMAL))) {
			throw PluginError(ErrorCodesAE::FailedToOpenWebPage);
		}
#else
		char theTempCmd[1024];
		RB_SPRINTF(theTempCmd, "open \"%ls?cpu=%ls&frames=%d\"", theUrl, procEncoded, frames);
		system(theTempCmd);
#endif
	ERROR_CATCH_END_NO_INFO_RETURN
}
std::string rbUtilities::toUtf8(const wchar_t* stringToConvert)
{
    size_t retval = 0;
    rsize_t dstsz = 0;
    std::string newbuffer;
    RB_DEFINELOCALE(utf_locale);
	try {
		RB_NEWLOCALE(utf_locale, RB_LOCALESTRING);
	}
	catch(...) {
		RB_NEWLOCALE(utf_locale, "en-US");
	}
    RB_WCSTOMBS_L(retval, nullptr, 0, stringToConvert, 0, utf_locale);
    if(retval != 0 && retval != (size_t)-1)
    {
        dstsz = retval;
        newbuffer.resize(dstsz);
        RB_WCSTOMBS_L(retval, const_cast<char*>(newbuffer.c_str()), dstsz+1, stringToConvert, dstsz, utf_locale);
    }
    RB_FREELOCALE(utf_locale);
    return newbuffer;
}
