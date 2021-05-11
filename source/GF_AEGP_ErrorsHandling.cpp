#include "GF_AEGP_ErrorsHandling.h"

namespace RenderBeamer
{
    const ErrorStringTables EnglishTableString = {
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
            {NoError, "No errors :)"},
            {NullResult, "Warning. Got NULL in result of action"},
            {ErrorResult, "Warning. Got Error string in result of action"},
            {NullPointerResult, "Warning. Got null pointer in result of action"},
            {UnknownError, "Error. Unhandled error occurred, contact support."},
            {RuntimeError, "Error. Runtime error occurred, contact support."},
            {StdException, "Error. Std library exception occurred, contact support."},
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
            {AE_ErrWrongThread, "Wrong thread AfterEffects Error (code5)"},
            {AE_ErrConstProjectModification, "AfterEffects Error (code6) Attempt to write a read only copy of an AE project."},
            {AE_ErrMissingSuiteOther, "AfterEffects missing suite Error (code7)"}
        }
    };
    ErrorCodesAE AeErrToErrorCodeAE(const A_Err &err) noexcept
    {
        if(err <= A_Err_NONE) return NoError;
        if(err == A_Err_MISSING_SUITE) return AE_ErrMissingSuiteOther;
        long code = err + AE_ErrNone;
        return (code >= _ErrorCodesCount ? AE_ErrGeneric : code );
    }
    const char *AeGetErrorString(std::string &out_buffer, ErrorCodesAE _err_msg_id, UserLanguage use_language, CallerModuleName _caller) noexcept
    {
        out_buffer.reserve(560);
        out_buffer = "";
        if(use_language == UserEnglish)
        {
            if(_caller > UnknownModule && _caller < _CallerModulesCount)
                out_buffer += EnglishTableString.CallerModuleString[_caller].str;
            if(_err_msg_id > NoError && _err_msg_id < _ErrorCodesCount)
                out_buffer += EnglishTableString.GeneralErrorStrings[_err_msg_id].str;
        }
        return out_buffer.c_str();
    }
    const char *AeGetCallerString(CallerModuleName _caller, UserLanguage use_language) noexcept
    {
        if(_caller <= UnknownModule || _caller >= _CallerModulesCount)
            _caller = UnknownModule;
        if(use_language == UserEnglish)
            return EnglishTableString.CallerModuleString[_caller].str;
        return EnglishTableString.CallerModuleString[_caller].str;
    }
    void PluginError::SetErrorString() noexcept
    {
        error_string_combined.reserve(560);
        error_string_combined = " RenderBeamer throwed ";
        if(errCodesLanguage == UserEnglish)
        {
            if(errCallerModuleModule > UnknownModule && errCallerModuleModule < _CallerModulesCount)
                error_string_combined += EnglishTableString.CallerModuleString[errCallerModuleModule].str;
            if(errCode > NoError && errCode < _ErrorCodesCount)
                error_string_combined += EnglishTableString.GeneralErrorStrings[errCode].str;
        }
    }

} // namespace RenderBeamer
