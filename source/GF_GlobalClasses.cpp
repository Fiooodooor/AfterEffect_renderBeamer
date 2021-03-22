#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "GF_GlobalClasses.h"

#include <ctime>
#include <vector>
#include <cstdarg>
#include "base64.h"
#include "GF_AEGP_Dumper.h"

rbProjectClass::rbProjectClass() : logFilePathRenderBeamer{ '\0'}, logFileMode(0) {}
rbProjectClass::~rbProjectClass()
{
	try
	{
		if(logger.is_open())
		{
			logger.flush();
			logger.close();
		}
	}
	catch(...)
	{
	}
}

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
        PT_ETX(execScript(pb, pluginId, tmpScript, font, AEGP_MAX_PATH_SIZE))
        suites.ANSICallbacksSuite1()->sprintf(tmpScript, GetBeamerMaskA(BeamerMask_GetFontFamily), itemNr, layerNr);
        PT_ETX(execScript(pb, pluginId, tmpScript, family, AEGP_MAX_PATH_SIZE))
        suites.ANSICallbacksSuite1()->sprintf(tmpScript, GetBeamerMaskA(BeamerMask_GetFontPath), itemNr, layerNr);
        PT_ETX(execScript(pb, pluginId, tmpScript, location, AEGP_MAX_PATH_SIZE))
    } PT_XTE_CATCH_RETURN_ERR
}

ErrorCodesAE rbUtilities::execScript(SPBasicSuite *pb, AEGP_PluginID pluginId, const A_char *inScript, A_char *outAchar, A_long maxLength)
{
	ERROR_CATCH_START
		AEGP_SuiteHandler suites(pb);
        AEGP_MemHandle result = nullptr, error = nullptr;
		ERROR_AEER(suites.UtilitySuite6()->AEGP_ExecuteScript(pluginId, inScript, FALSE, &result, &error))
        if (_ErrorCode == NoError) {
			ERROR_AEER(copyMemhandleToAChar(pb, result, outAchar, maxLength))
			suites.MemorySuite1()->AEGP_FreeMemHandle(error);
        }		
        else {
			copyMemhandleToAChar(pb, error, outAchar, maxLength);
			suites.MemorySuite1()->AEGP_FreeMemHandle(result);
			_ErrorCode = ScriptExecuteError;
        }
	ERROR_CATCH_END_NO_INFO_RETURN
}

A_Err rbUtilities::copyMemhandleToAChar(SPBasicSuite *pb, AEGP_MemHandle& inputString, A_char* destination, A_long maxLength)
{
    AEGP_SuiteHandler suites(pb);
    PT_XTE_START{
        A_char* tempBuffor;

        PT_ETX(suites.MemorySuite1()->AEGP_LockMemHandle(inputString, reinterpret_cast<void**>(&tempBuffor)))
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
        PT_ETX(suites.MemorySuite1()->AEGP_UnlockMemHandle(inputString))
        PT_ETX(suites.MemorySuite1()->AEGP_FreeMemHandle(inputString))
    } PT_XTE_CATCH_RETURN_ERR
}

size_t rbUtilities::utf16Length(A_UTF16Char* in)
{
	size_t i = 0;
	if (in) {
		while(in[i] != 0) {
			++i;
		}
		return i;
	}
	return 0;
}

A_Err rbUtilities::copyMemhUTF16ToPath(SPBasicSuite *pb, AEGP_MemHandle& inputString, fs::path &resPath)
{
	A_Err err = A_Err_NONE;
    #ifndef USE_BOOST
	AEGP_SuiteHandler suites(pb);
	char16_t* res16B = nullptr;
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
    fs::path tempPath = path.c_str();
	if (tempPath.has_extension()) {
        extension = tempPath.extension().c_str();
		tempPath.replace_extension();
	}
    
    length = TSTRLEN(tempPath.TSTRING().c_str()) + 1;
    TCHAR *tmp_str = new TCHAR[length];

    if (tmp_str)
    {
        TSTRNCPY(tmp_str, tempPath.TSTRING().c_str(), length - 1)

        if(dissalowed) {
            replaceDisallowedOnly(tmp_str);
        }
        else {
            leaveAllowedOnly(tmp_str);
        }
#ifndef USE_BOOST
        path.assign(tmp_str);
#else
        path.assign(tmp_str, tmp_str + length - 1);
#endif
		if (!extension.empty() && !cut_extension) {
            path += extension.c_str();
		}
        delete[] tmp_str;
    }
}

void rbUtilities::replaceDisallowedOnly(wchar_t *sourceStr)
{
    auto *source_str_ptr = sourceStr;
    wchar_t *mask = DISALLOWED_CHARACTERSW;

    while (source_str_ptr && *source_str_ptr)
    {
		source_str_ptr += wcscspn(source_str_ptr, mask);
        if (source_str_ptr[0] == '\0')
            break;
		source_str_ptr[0] = '_';
    }
}

void rbUtilities::replaceDisallowedOnly(A_char *sourceStr)
{
    auto *source_str_ptr = sourceStr;
    A_char *mask = DISALLOWED_CHARACTERSA;

    while (source_str_ptr && *source_str_ptr)
    {
		source_str_ptr += strcspn(source_str_ptr, mask);
        if (source_str_ptr[0] == '\0')
            break;
		source_str_ptr[0] = '_';
    }
}

void rbUtilities::leaveAllowedOnly(wchar_t *sourceStr)
{
    auto *source_str_ptr = sourceStr;
    wchar_t *mask = ALLOWED_CHARACTERSW;

    while (source_str_ptr && *source_str_ptr)
    {
		source_str_ptr += wcsspn(source_str_ptr, mask);
        if (source_str_ptr[0] == '\0')
            break;
		source_str_ptr[0] = '_';
    }
}
void rbUtilities::leaveAllowedOnly(A_char *sourceStr)
{
    auto *source_str_ptr = sourceStr;
    A_char *mask = ALLOWED_CHARACTERSA;

    while (source_str_ptr && *source_str_ptr)
    {
		source_str_ptr += strspn(source_str_ptr, mask);
        if (source_str_ptr[0] == '\0')
            break;
		source_str_ptr[0] = '_';
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
    char* tmp_buffer = nullptr;
#ifdef AE_OS_WIN
    size_t bufferSize = 0;
    if (_dupenv_s(&tmp_buffer, &bufferSize, variable) == 0 && buffer != nullptr)
    {
        strcpy_s(buffer, size, tmp_buffer);
        free(tmp_buffer);
    }

#else
	tmp_buffer = getenv(variable);
    if (tmp_buffer != NULL)
        strncpy(buffer, tmp_buffer, size);
    else
        buffer[0] = '\0';
#endif
}

ErrorCodesAE rbUtilities::execBeamerCmd(beamerParamsStruct bps, BeamerMasks mask, wchar_t *buffer, A_long buffer_size)
{
	ERROR_CATCH_START
	switch (mask)
	{
	case BeamerMask_GetUser:
        RB_SPRINTF(bps.beamerExecScript, 2048, GetBeamerMaskA(BeamerMask_GetUser), bps.beamerTmpFile.lexically_normal().wstring().c_str());
		break;
	case BeamerMask_GetTemp:
		RB_SPRINTF(bps.beamerExecScript, 2048, GetBeamerMaskA(BeamerMask_GetTemp), bps.beamerTmpFile.lexically_normal().wstring().c_str());
		break;
	case BeamerMask_CheckScene:
		RB_SPRINTF(bps.beamerExecScript, 2048, GetBeamerMaskA(BeamerMask_CheckScene), bps.bp.remoteProjectPath.lexically_normal().wstring().c_str(), bps.rmtUser, bps.beamerTmpFile.lexically_normal().wstring().c_str());
		break;
	case BeamerMask_SendTask:
		RB_SPRINTF(bps.beamerExecScript, 2048, GetBeamerMaskA(BeamerMask_SendTask), bps.bp.relinkedSceneRoot.lexically_normal().wstring().c_str(), bps.bp.remoteProjectPath.lexically_normal().wstring().c_str(), bps.beamerTmpFile.lexically_normal().wstring().c_str());
		break;
	case BeamerMask_SendTaskEncoded:
		RB_SPRINTF(bps.beamerExecScript, 2048, GetBeamerMaskA(BeamerMask_SendTaskEncoded), base64_encode( FS_U8STRING(bps.bp.relinkedSceneRoot.lexically_normal())).c_str(), bps.bp.remoteProjectPath.lexically_normal().wstring().c_str(), bps.beamerTmpFile.lexically_normal().wstring().c_str());
		break;
	default:
		return ErrorResult;
	}
	_ErrorCode = exec_cmd(fs::path(bps.beamerScript), std::string(bps.beamerExecScript), bps.beamerTmpFile.lexically_normal(), buffer, buffer_size);
	
	if (_ErrorCode == NoError)
	{
		if(buffer_size == 0) {
			_ErrorCode = NoError;
		}
		else if (buffer == nullptr) {
			_ErrorCode = NullPointerResult;
		}
		else if (buffer[0] == '\0') {
			_ErrorCode = ErrorResult;
		}
		else if (wcscmp(buffer, L"[NULL]") == 0) {
			_ErrorCode = NullResult;
		}
		else if (wcsncmp(buffer, L"ERROR", 5) == 0) {
			_ErrorCode = ErrorResult;
		}
	}
	ERROR_CATCH_END_NO_INFO
	return _ErrorCode;
}

ErrorCodesAE rbUtilities::exec_cmd(fs::path const &app, std::string const &args, fs::path const &out_file, wchar_t *buffer_w, unsigned long buffer_size)
{
	ErrorCodesAE _ErrorCode = NoError;
	GF_Dumper::rbProj()->logg("HelperClass", "ExecCmd::Args", args.c_str());
#ifdef AE_OS_WIN
	_ErrorCode = win_exec_cmd(app, args, out_file, buffer_w, buffer_size);
#else
	_ErrorCode = mac_exec_cmd(app, args, out_file, buffer_w, buffer_size);
#endif
	GF_Dumper::rbProj()->logg("HelperClass", "ExecCmd::Result", std::to_string(static_cast<int>(_ErrorCode)).c_str());
	if(_ErrorCode == NoError && buffer_size > 0 && buffer_w)
		GF_Dumper::rbProj()->logg(L"HelperClass", L"ExecCmd::ResultBuffer", buffer_w);
	return _ErrorCode;
}

ErrorCodesAE rbUtilities::win_exec_cmd(fs::path const &app, std::string const &args, fs::path const &out_file, wchar_t *buffer_w, unsigned long buffer_size)
{
	ERROR_CATCH_START_MOD(HelperClassesModule)
#ifdef AE_OS_WIN
	if (app.empty() || args.empty())
		return ErrorResult;
	std::wstring cmd;
	DWORD return_code = 0;
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	cmd.reserve(MAX_PATH);
	cmd = L"\"" + app.filename().wstring() + L"\" " + std::wstring(args.cbegin(), args.cend());

	if (!CreateProcessW(app.wstring().c_str(), &cmd[0], nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
		throw PluginError(ExecCommandFailedToExec);

	const DWORD fn_signaled_state = WaitForSingleObject(pi.hProcess, INFINITE);
	if (!GetExitCodeProcess(pi.hProcess, &return_code))
		return_code = 0;
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if (fn_signaled_state != WAIT_OBJECT_0)
		throw PluginError(ExecCommandFailedObjWait);
	if (return_code != 0)
		throw PluginError(ExecCommandFailed);
	_ErrorCode = read_file_buffer_w(out_file, buffer_w, buffer_size);
	
#endif
	ERROR_CATCH_END_LOGGER_RETURN("WinExecCmd")
}
ErrorCodesAE rbUtilities::mac_exec_cmd(fs::path const &app, std::string const &args, fs::path const &out_file, wchar_t *buffer_w, unsigned long buffer_size)
{
	ERROR_CATCH_START_MOD(HelperClassesModule)
#ifndef AE_OS_WIN
	if (app.empty() || args.empty())
		return ErrorResult;

	std::string cmd = app.string() + " " + args;

	if (!std::system(nullptr))
		throw PluginError(ExecCommandFailedToExec);

	const int exit_status = std::system(cmd.c_str());
	if (exit_status == -1)
		throw PluginError(ExecCommandFailedToExec);
	if (exit_status != 0)
		throw PluginError(ExecCommandFailed);

	_ErrorCode = read_file_buffer_w(out_file, buffer_w, buffer_size);
	
#endif
	ERROR_CATCH_END_LOGGER_RETURN("MacExecCmd")
}
ErrorCodesAE rbUtilities::read_file_buffer_w(fs::path const &out_file, wchar_t *buffer_w, unsigned long buffer_size)
{
	ERROR_CATCH_START_MOD(HelperClassesModule)
		if (buffer_w && buffer_size > 0 && !out_file.empty())
		{
			std::wfstream tmp_stream(out_file.wstring().c_str(), std::wfstream::in);
			if (!tmp_stream.is_open())
				throw PluginError(ExecCommandFailedToRead);

			tmp_stream.getline(buffer_w, buffer_size);
			buffer_w[buffer_size - 1] = '\0';
			if (tmp_stream.gcount() < 1 || tmp_stream.rdstate() == std::ios_base::failbit || tmp_stream.rdstate() == std::ios_base::badbit)
				throw PluginError(ExecCommandFailedToRead);
		}
	ERROR_CATCH_END_LOGGER_RETURN("ReadFileBuffer")
}
ErrorCodesAE rbUtilities::getVersionString(A_char* buff, long buff_size)
{
	ERROR_CATCH_START
		const std::string versionString = GetStringPtr(StrID_Name);		
		ASTRNCPY(buff, versionString.c_str(), buff_size)
	ERROR_CATCH_END_NO_INFO_RETURN
}

ErrorCodesAE rbUtilities::getCpuInfoString(SPBasicSuite *pb, wchar_t *buffer, A_long buffer_length, const wchar_t *outFileName)
{
	ERROR_CATCH_START
#ifdef AE_OS_WIN
	wchar_t pathBuffer[MAX_PATH];
	char regParams[512];
	GetWindowsDirectoryW(pathBuffer, MAX_PATH);
	fs::path reg_exe = pathBuffer;
	reg_exe /= "system32";
	reg_exe /= "reg.exe";
	RB_SPRINTF(regParams, "EXPORT \"HKLM\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0\" \"%ls\" /y /reg:64", outFileName);
	win_exec_cmd(reg_exe, regParams, fs::path(), nullptr, 0);

	std::wifstream myfile(outFileName, std::ios::binary | std::wfstream::in);
	std::wstring line, subLine = L"";

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
	fs::path out_path = fs::path(outFileName);
	std::string args = std::string("-n \"machdep.cpu.brand_string\" > \"") + out_path.string() + std::string("\"");
	mac_exec_cmd("/usr/sbin/sysctl", args, out_path, buffer, buffer_length);
#endif
	
	ERROR_CATCH_END_NO_INFO_RETURN
}

ErrorCodesAE rbUtilities::encodeUrlString(const wchar_t *url, wchar_t *buffer, long buffer_size)
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
	WSTRNCPY(buffer, returnUrl.c_str(), buffer_size)
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
    
	if (getCpuInfoString(pb, theTempProcChar, 512, data_tmp.wstring().c_str()) != ErrorCodesAE::NoError)
        return ErrorResult;
    
    if(encodeUrlString(theTempProcChar, procEncoded, 512) != ErrorCodesAE::NoError)
        return ErrorResult;
        //procEncoded[0] = '\0';
#ifdef AE_OS_WIN
		wchar_t theTempCmd[1024];
		RB_SWPRINTF(theTempCmd, 1024, L"%ls?cpu=%ls&frames=%d", theUrl, procEncoded,  frames);
		if (32 >= reinterpret_cast<ULONG_PTR>(ShellExecuteW(nullptr, L"open", theTempCmd, nullptr, nullptr, SW_SHOWNORMAL))) {
			throw PluginError(FailedToOpenWebPage);
		}
#else
		char theTempCmd[1024];
		RB_SPRINTF(theTempCmd, 1024, "open \"%ls?cpu=%ls&frames=%d\"", theUrl, procEncoded, frames);
		system(theTempCmd);
#endif
	ERROR_CATCH_END_NO_INFO_RETURN
}
// in windows change to WideCharToMultiByte with
// CP_UTF8 - for output to files, beamer etc.
// CP_OEMCP - for string literals for AE.
// 
// in macOs change to use locale with
// .UTF-8 for output files, beamer etc.
// GetApplicationTextEncoding()  - for string literals for AE
std::string rbUtilities::toUtf8(const wchar_t* stringToConvert)
{
	std::string new_buffer;
	ERROR_CATCH_START
	size_t retval = 0;
	rsize_t dstsz;

	_locale_t utf_locale = _create_locale(LC_ALL, RB_LOCALESTRING);
	
	// RB_WCSTOMBS_L(bts_converted,dst,dst_bytes,src,max_bytes,locale) bts_converted=wcstombs_l(dst,src,max_bytes,locale)
	// 
	// RB_WCSTOMBS_L(bts_converted,dst,dst_bytes,src,max_bytes,locale) if(_wcstombs_s_l(&bts_converted,dst,dst_bytes,src,max_bytes,locale)==0) bts_converted-=1;
	RB_WCSTOMBS_L(retval, nullptr, 0, stringToConvert, 0, utf_locale)
		if (retval != 0 && retval != static_cast<std::size_t>(-1))
		{
			dstsz = retval;
			new_buffer.resize(dstsz);
			RB_WCSTOMBS_L(retval, const_cast<char*>(new_buffer.c_str()), dstsz + 1, stringToConvert, dstsz, utf_locale)
		}
	
	_free_locale(utf_locale);	
	ERROR_CATCH_END_NO_INFO
	return new_buffer;
}
