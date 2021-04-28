#include "GF_AEGP_ErrorsHandling.h"

namespace RenderBeamer
{
    const ErrorStringTables EnglishTableString = {
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
