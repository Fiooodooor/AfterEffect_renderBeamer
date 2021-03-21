#include "GF_AEGP_ErrorsHandling.h"

ErrorStringTables* PluginError::GetEnglishTable()
{
static ErrorStringTables instance = {
{
	{UnknownModule,            "Unknown Module: "},
	{MainModule,                "Main Module: "},
	{PreCheckModule,            "Pre-Check Project Module: "},
	{PrePreparationModule,    "Project Preparation Module: "},
	{ProjectDumperModule,    "Project Dumper Module: "},
	{TreeParsingModule,        "Items Tree Parsing Module: "},
	{CompositionParsingModule, "Composition Parsing Module: "},
	{FootageParsingModule,    "Footage Parsing Module: "},
	{CopyFootagesModule,        "Foo Copy and Relink Module: "},
	{QueueCollectModule,        "Queue Items Module: "},
	{CopyCollectFontsModule, "Copy Fonts Module: "},
	{CollectEffectsModule,    "Effects Inf. Collecting Module: "},
	{RelinkerModule,            "Main Relinker Module: "},
	{HelperClassesModule,    "Helper Module: "},
	{LayerParsingModule,        "Layer Parsing Module: "},
	{LayerEffectsModule,        "Layer Effects Module: "},
	{OutputCollectModule,    "Output Collecting Module: "},
	{RelinkerInitModule,     "Relinker Initializer Module: "},
	{MainCommandHookModule,  "Main Command Hook Module: "},
	{SceneCollectorModule,  "Scene Collector Module: "}
}, {
	{A_Err_NONE, "No error (code 0)"},
	{A_Err_GENERIC, "Generic AfterEffects Error (code 1)"},
	{A_Err_STRUCT, "Structural AfterEffects Error (code 2)"},
	{A_Err_PARAMETER, "Parameter AfterEffects Error (code3)"},
	{A_Err_ALLOC, "Allocation AfterEffects Error (code4)"},
	{A_Err_WRONG_THREAD,    "Wrong thread AfterEffects Error (code5)"},
	{A_Err_CONST_PROJECT_MODIFICATION, "AfterEffects Error (code6) Attempt to write a read only copy of an AE project."},
	{7, "AfterEffects missing suite Error (code7)"}
}, {
	{NoError, ""},
	{NullResult, "Warning. Got NULL in result of action"},
	{ErrorResult, "Warning. Got Error string in result of action"},
	{NullPointerResult, "Warning. Got null pointer in result of action"},
	{UnknownError, "Error. Unhandled error occured, contact support."},
	{RuntimeError, "Error. Runtime error occured, contact support."},
	{StdException, "Error. Std library exception occured, contact support."},
	{AeTypeError, "Error. After Effects internal error"},
	{UserDialogCancel, "Error. User canceled the operation"},
	{ProjectNotOpened, "Error. No project is opened now, nothing to send to farm."},
	{ProjectNotSaved, "Error. Project have been modified. Save all changes before committing it to farm!"},
	{NoValidRqItems, "Error. There are no valid render queue items in render queue."},
	{NoRqItemsQueed, "Error. There are no render queue items with queued status."},
    {NoRqItemOutputs, "Error. There are no valid outputs assigned for queued render items."},
    {ScriptExecuteSuccess, "The script was executed successfully."},
    {ScriptExecuteError, "Error. The script failed to execute."},
    {Parser_Err, "Error. Parsing error. Failed to finish."},
    {Parser_Item_Err, "Error. Parsing error. Failed to finish item."},
    {MissingAssets, "confirm(\"Error. There are missing assets in the scene. Continue?\");"},
    {MissingFootage, "Warning, Missing footage found."},
    {DirectoryCreationErr, "Error. Temporary directory tree creation error."},
    {GfsFileOpenError, "Error. Could not create and open renderBeamer gfs file."},
    {BeamerGetUserName, "Error. Could not get user data string from renderBeamer."},
    {GetLocalTempDirectory, "Error. Could not get local temporary directory from system API."},
    {GetLocalUsersPlugin, "Error. Could not get local user plugins directory from AE."},
    {GetLocalBeamerPath, "Error. Could not get local valid renderBeamer path."},
    {BeamerSendTaskFailed, "Error. Project was prepared correctly but renderBeamer send command failed."},
    {FailedToOpenWebPage, "Error. Plugin could not open specified web page."},
    {ExecCommandFailed, "Error. Plugin execute of commandline script returned non zero result."},
    {ExecCommandFailedToExec, "Error. Plugin process create and script execute failed."},
    {ExecCommandFailedToRead, "Error. Plugin execute of commandline script result could not have been read."},
    {ExecCommandFailedObjWait, "Error. Plugin wait for finish to execute commandline script failed."},
    {AE_ErrNone, "No error (code 0)"},
	{AE_ErrGeneric, "Generic AfterEffects Error (code 1)"},
    {AE_ErrStruct, "Structural AfterEffects Error (code 2)"},
    {AE_ErrParameter, "Parameter AfterEffects Error (code3)"},
    {AE_ErrAlloc, "Allocation AfterEffects Error (code4)"},
    {AE_ErrWrongThread,    "Wrong thread AfterEffects Error (code5)"},
    {AE_ErrConstProjectModification, "AfterEffects Error (code6) Attempt to write a read only copy of an AE project."},
    {AE_ErrMissingSuiteOther, "AfterEffects missing suite Error (code7)"}
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
	if(code_number>=0 && code_number<NumberOfElements)
		return GetEnglishTable()->GeneralErrorStrings_Eng[code_number].str;
	else
		return GetEnglishTable()->GeneralErrorStrings_Eng[UnknownError].str;
}
