#include "GF_AEGP_ErrorsHandling.h"

ErrorStringTables* PluginError::GetEnglishTable()
{
static ErrorStringTables instance = {
{
	{UnknownModule,            "\nUnknown Module: "},
	{MainModule,                "\nMain Module: "},
	{PreCheckModule,            "\nPre-Check Project Module: "},
	{PrePreparationModule,    "\nProject Preparation Module: "},
	{ProjectDumperModule,    "\nProject Dumper Module: "},
	{TreeParsingModule,        "\nItems Tree Parsing Module: "},
	{CompositionParsingModule, "\nComposition Parsing Module: "},
	{FootageParsingModule,    "\nFootage Parsing Module: "},
	{CopyFootagesModule,        "\nFoo Copy and Relink Module: "},
	{QueueCollectModule,        "\nQueue Items Module: "},
	{CopyCollectFontsModule, "\nCopy Fonts Module: "},
	{CollectEffectsModule,    "\nEffects Inf. Collecting Module: "},
	{RelinkerModule,            "\nMain Relinker Module: "},
	{HelperClassesModule,    "\nHelper Module: "},
	{LayerParsingModule,        "\nLayer Parsing Module: "},
	{LayerEffectsModule,        "\nLayer Effects Module: "},
	{OutputCollectModule,    "\nOutput Collecting Module: "},
	{RelinkerInitModule,     "\nRelinker Initializer Module: "},
	{MainCommandHookModule,  "\nMain Command Hook Module: "},
	{SceneCollectorModule,  "\nScene Collector Module: "}
}, {
	{A_Err_NONE, "\nNo error (code 0)"},
	{A_Err_GENERIC, "\nGeneric AfterEffects Error (code 1)"},
	{A_Err_STRUCT, "\nStructural AfterEffects Error (code 2)"},
	{A_Err_PARAMETER, "\nParameter AfterEffects Error (code3)"},
	{A_Err_ALLOC, "\nAllocation AfterEffects Error (code4)"},
	{A_Err_WRONG_THREAD,    "\nWrong thread AfterEffects Error (code5)"},
	{A_Err_CONST_PROJECT_MODIFICATION, "\nAfterEffects Error (code6) Attempt to write a read only copy of an AE project."},
	{7, "\nAfterEffects missing suite Error (code7)"}
}, {
	{NoError, "\nNo errors :)"},
	{NullResult, "\nWarning. Got NULL in result of action"},
	{ErrorResult, "\nWarning. Got Error string in result of action"},
	{NullPointerResult, "\nWarning. Got null pointer in result of action"},
	{UnknownError, "\nError. Unhandled error occurred, contact support."},
	{RuntimeError, "\nError. Runtime error occurred, contact support."},
	{StdException, "\nError. Std library exception occurred, contact support."},
	{AeTypeError, "\nError. After Effects internal error"},
	{UserDialogCancel, "\nError. User canceled the operation"},
	{ProjectNotOpened, "\nError. No project is opened now, nothing to send to farm."},
	{ProjectNotSaved, "\nError. Project have been modified. Save all changes before committing it to farm!"},
	{NoValidRqItems, "\nError. There are no valid render queue items in render queue."},
	{NoRqItemsQueed, "\nError. There are no render queue items with queued status."},
    {NoRqItemOutputs, "\nError. There are no valid outputs assigned for queued render items."},
    {ScriptExecuteSuccess, "\nThe script was executed successfully."},
    {ScriptExecuteError, "\nError. The script failed to execute."},
    {Parser_Err, "\nError. Parsing error. Failed to finish."},
    {Parser_Item_Err, "\nError. Parsing error. Failed to finish item."},
    {MissingAssets, "\nconfirm(\"Error. There are missing assets in the scene. Continue?\");"},
    {MissingFootage, "\nWarning, Missing footage found."},
    {DirectoryCreationErr, "\nError. Temporary directory tree creation error."},
    {GfsFileOpenError, "\nError. Could not create and open renderBeamer gfs file."},
    {BeamerGetUserName, "\nError. Could not get user data string from renderBeamer."},
    {GetLocalTempDirectory, "\nError. Could not get local temporary directory from system API."},
    {GetLocalUsersPlugin, "\nError. Could not get local user plugins directory from AE."},
    {GetLocalBeamerPath, "\nError. Could not get local valid renderBeamer path."},
    {BeamerSendTaskFailed, "\nError. Project was prepared correctly but renderBeamer send command failed."},
    {FailedToOpenWebPage, "\nError. Plugin could not open specified web page."},
    {ExecCommandFailed, "\nError. Plugin execute of commandline script returned non zero result."},
    {ExecCommandFailedToExec, "\nError. Plugin process create and script execute failed."},
    {ExecCommandFailedToRead, "\nError. Plugin execute of commandline script result could not have been read."},
    {ExecCommandFailedObjWait, "\nError. Plugin wait for finish to execute commandline script failed."},
    {AE_ErrNone, "\nNo error (code 0)"},
	{AE_ErrGeneric, "\nGeneric AfterEffects Error (code 1)"},
    {AE_ErrStruct, "\nStructural AfterEffects Error (code 2)"},
    {AE_ErrParameter, "\nParameter AfterEffects Error (code3)"},
    {AE_ErrAlloc, "\nAllocation AfterEffects Error (code4)"},
    {AE_ErrWrongThread, "\nWrong thread AfterEffects Error (code5)"},
    {AE_ErrConstProjectModification, "\nAfterEffects Error (code6) Attempt to write a read only copy of an AE project."},
    {AE_ErrMissingSuiteOther, "\nAfterEffects missing suite Error (code7)"}
	}};
    return &instance;
}
ErrorCodesAE aErrToRb(const A_Err err)
{
    switch (err)
    {
    case A_Err_NONE: return NoError;
    case A_Err_GENERIC: return AE_ErrGeneric;
    case A_Err_STRUCT: return AE_ErrStruct;
    case A_Err_PARAMETER: return AE_ErrParameter;
    case A_Err_ALLOC: return AE_ErrAlloc;
    case A_Err_WRONG_THREAD: return AE_ErrWrongThread;
    case A_Err_CONST_PROJECT_MODIFICATION: return AE_ErrConstProjectModification;
    default:
        break;
    }
    return AE_ErrMissingSuiteOther;
}
const char *PluginError::GetCallerStringA(const CallerModuleName& caller, const UserLanguage &lang_number) noexcept
{
	return GetEnglishTable()->CallerModuleString[caller].str;
}
const char *PluginError::GetErrorStringS(const CallerModuleName& caller, const A_Err &code_number, const UserLanguage &lang_number)
{
	std::string returnStr(GetEnglishTable()->CallerModuleString[caller].str);
	returnStr += GetErrorStringA(code_number, lang_number);
    char *tmp_cpy = new char[returnStr.size()];
	ASTRNCPY(tmp_cpy, returnStr.c_str(), returnStr.size())
    return tmp_cpy;
}
const char *PluginError::GetErrorStringA(const A_Err &code_number, const UserLanguage &lang_number) noexcept
{
	if (code_number >= 0 && code_number < 7)
		return GetEnglishTable()->A_Err_ErrorStrings_Eng[code_number].str;
	else
		return GetEnglishTable()->A_Err_ErrorStrings_Eng[7].str;
}

const char *PluginError::GetErrorStringS(const CallerModuleName& caller, const ErrorCodesAE &code_number, const UserLanguage &lang_number)
{
	std::string returnStr(GetEnglishTable()->CallerModuleString[caller].str);
    returnStr += GetErrorStringA(code_number, lang_number);
    char *tmpCpy = new char[returnStr.size()];
	ASTRNCPY(tmpCpy, returnStr.c_str(), returnStr.size())
    return tmpCpy;
}
const char *PluginError::GetErrorStringA(const ErrorCodesAE &code_number, const UserLanguage &lang_number) noexcept
{
	if(code_number<NumberOfElements)
		return GetEnglishTable()->GeneralErrorStrings_Eng[code_number].str;
	else
		return GetEnglishTable()->GeneralErrorStrings_Eng[UnknownError].str;
}
