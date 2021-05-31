#pragma once
#ifndef ERRORSHANDLING_H
#define ERRORSHANDLING_H
#include "GF_GlobalTypes.h"
#include <exception>

#define PT_XTE_START {A_Err _err = A_Err_NONE; try {

#define PT_CATCH_PART1		}	catch (A_Err _tmp_err) { _err = _tmp_err; } \
								catch(std::bad_alloc&) { _err = A_Err_ALLOC; }
#define PT_CATCH_PART2		if (_err) {
#define PT_XTE_CATCH		PT_CATCH_PART1 PT_CATCH_PART2
#define PT_ENDTRY			} if (_err) throw ((long) _err); }

#define PT_NO_PROPAGATE		_err = A_Err_NONE
#define PT_XTE_ENDTRY(ERR)	ERR = _err; PT_NO_PROPAGATE; PT_ENDTRY

#define PT_XTE(EXPR) PT_XTE_START { EXPR; } PT_XTE_CATCH { ; } PT_XTE_ENDTRY(err)

#define PT_XTE_CATCH_RETURN_ERR		PT_XTE_CATCH A_Err retErr; retErr = _err; PT_NO_PROPAGATE; return retErr; }};	return A_Err_NONE;

#define PT_ETX(EXPR) { A_Err _res = (EXPR); if(_res != A_Err_NONE) throw _res;}
#define PT_ERR(EXPR) { if (!err) {	err = (EXPR);	}	}


#define AEREP(SH,FN) (SH).UtilitySuite6()->AEGP_ReportInfo(pluginId, (FN));
#define REP(SH) AEREP(SH,theErr.what())
#define REP_UN(SH) AEREP(SH,AeGetErrorString(catch_buffer_, UnknownError, GF_PLUGIN_LANGUAGE))
#define REP_AE(SH) AEREP(SH,AeGetErrorString(catch_buffer_, theErr, GF_PLUGIN_LANGUAGE))

#define ERROR_CATCH_START ErrorCodesAE _ErrorCode = NoError; { try {
#define ERROR_CATCH_START_MOD(MOD) CallerModuleName _ErrorCaller = MOD; ERROR_CATCH_START
#define ERROR_CATCH_KNOWN1(SH)  catch (PluginError & theErr) { std::string catch_buffer_; _ErrorCode=theErr.theCode(); SH }
#define ERROR_CATCH_KNOWN2(SH,VAR_ERR)  catch (std::runtime_error (VAR_ERR)) { std::string catch_buffer_; _ErrorCode=RuntimeError; SH }
#define ERROR_CATCH_KNOWN3(SH,VAR_ERR)  catch (std::exception (VAR_ERR)) { std::string catch_buffer_; _ErrorCode=StdException; SH }
#define ERROR_CATCH_KNOWN4(SH,VAR_ERR)  catch (A_Err (VAR_ERR)) { std::string catch_buffer_; _ErrorCode=AeTypeError; SH }
#define ERROR_CATCH_UNKNOWN(SH) catch (...) { std::string catch_buffer_; _ErrorCode=UnknownError; SH }
#define ERROR_CATCH_END_CONSTRUCT(SH,UN,AE,VR) \
        } \
            catch (const PluginError & theErr) { std::string catch_buffer_; _ErrorCode=theErr.theCode(); SH } \
            ERROR_CATCH_KNOWN2(SH,VR) \
            ERROR_CATCH_KNOWN3(SH,VR) \
            ERROR_CATCH_KNOWN4(AE,VR) \
            ERROR_CATCH_UNKNOWN(UN) }

#define ERROR_CATCH_START2 std::string catch_buffer_(256, 0); ErrorCodesAE _ErrorCode = NoError; { try {
#define ERROR_CATCH_END_CONSTRUCT2 \
        }   catch (PluginError & theErr)		{ _ErrorCode=theErr.theCode(); catch_buffer_ = theErr.what(); }	\
            catch (std::runtime_error & theErr) { _ErrorCode=RuntimeError; catch_buffer_ = theErr.what(); }		\
            catch (std::bad_alloc & theErr)		{ _ErrorCode=AE_ErrAlloc; catch_buffer_ = theErr.what(); }		\
            catch (std::exception &theErr)		{ _ErrorCode=StdException; catch_buffer_ = theErr.what(); }		\
            catch (A_Err &theErr)				{ _ErrorCode=AeTypeError; catch_buffer_ = AeGetErrorString(catch_buffer_, theErr, GF_PLUGIN_LANGUAGE); }			\
            catch (...)							{ _ErrorCode=UnknownError; catch_buffer_ = AeGetErrorString(catch_buffer_, UnknownError, GF_PLUGIN_LANGUAGE); }		\
		}

#define ERROR_CATCH_END(SH) ERROR_CATCH_END_CONSTRUCT(REP(SH),REP_UN(SH),REP_AE(SH),&theErr)
#define ERROR_CATCH_END_RETURN(SH) ERROR_CATCH_END(SH) return _ErrorCode;
#define ERROR_CATCH_END_NO_INFO ERROR_CATCH_END_CONSTRUCT(;,;,;,&)
#define ERROR_CATCH_END_NO_INFO_RETURN ERROR_CATCH_END_NO_INFO return _ErrorCode;

#define ERROR_CATCH_END_LOGGER2(OBJECT,TYPE,LOGGER) ERROR_CATCH_END_CONSTRUCT(LOGGER->loggErr(OBJECT,TYPE,theErr.what());,LOGGER->loggErr(OBJECT,TYPE,AeGetErrorString(catch_buffer_, UnknownError, GF_PLUGIN_LANGUAGE));,LOGGER->loggErr(OBJECT,TYPE,AeGetErrorString(catch_buffer_, theErr, GF_PLUGIN_LANGUAGE));,&theErr)
#define ERROR_CATCH_END_LOGGER1(OBJECT,TYPE) ERROR_CATCH_END_LOGGER2(OBJECT,TYPE,GF_Dumper::rbProj())
#define ERROR_CATCH_END_LOGGER(ATYPE) ERROR_CATCH_END_LOGGER1(AeGetCallerString(_ErrorCaller,GF_PLUGIN_LANGUAGE),ATYPE)
#define ERROR_CATCH_END_LOGGER_RETURN(ATYPE) ERROR_CATCH_END_LOGGER1(AeGetCallerString(_ErrorCaller,GF_PLUGIN_LANGUAGE),ATYPE) return _ErrorCode;

#define ERROR_CHECK(EXPR) { if(_ErrorCode == NoError) ( EXPR ); }
#define ERROR_AE(EXPR) { if(_ErrorCode == NoError) _ErrorCode=(EXPR); }
#define ERROR_AEER(EXPR) ERROR_AE(AeErrToErrorCodeAE(EXPR))
#define ERROR_LONG_ERR(EXPR) ERROR_AE(static_cast<ErrorCodesAE>(EXPR))
#define ERROR_BOOL_ERR(EXPR) { if(_ErrorCode == NoError) _ErrorCode=(( EXPR ) == true ? NoError : ErrorResult); }
#define ERROR_RETURN(EXPR) { _ErrorCode = EXPR; if(_ErrorCode != NoError) { return _ErrorCode; } }
#define ERROR_THROW_2(EXPR) { if(_ErrorCode == NoError) _ErrorCode = (EXPR); if(_ErrorCode != NoError) { throw PluginError(GF_PLUGIN_LANGUAGE, _ErrorCode); } }
#define ERROR_RETURN_VOID(EXPR) { if(_ErrorCode == NoError) _ErrorCode = ( EXPR ); if(_ErrorCode != NoError) { return; } }
#define ERROR_THROW(EXPR) { ErrorCodesAE tmp = (EXPR); if(tmp != NoError) throw PluginError(GF_PLUGIN_LANGUAGE, tmp); }
#define ERROR_THROW_MOD(EXPR) { ErrorCodesAE tmp = (EXPR); if(tmp != NoError) throw PluginError(GF_PLUGIN_LANGUAGE, tmp, _ErrorCaller); }
#define ERROR_THROW_AE(EXPR) { A_Err tmp = (EXPR); if(tmp != A_Err_NONE) throw PluginError(GF_PLUGIN_LANGUAGE, AeErrToErrorCodeAE(tmp)); }
#define ERROR_THROW_AE_MOD(EXPR) { A_Err tmp = (EXPR); if(tmp != A_Err_NONE) throw PluginError(GF_PLUGIN_LANGUAGE, AeErrToErrorCodeAE(tmp), _ErrorCaller); }

#define GF_PROGRESS(EXPR) { PF_Err PT_res = (EXPR); if(PT_res == PF_Interrupt_CANCEL) return UserDialogCancel;}
#define MAIN_PROGRESS(PROGRESS, COUNT, TOTAL) { if( _ErrorCode != NoError) { } else if( PROGRESS ) { PF_Err PT_res = suites.AppSuite6()->PF_AppProgressDialogUpdate(( PROGRESS ), ( COUNT ), ( TOTAL )); if(PT_res == PF_Interrupt_CANCEL) { suites.AppSuite6()->PF_DisposeAppProgressDialog( PROGRESS ); ( PROGRESS ) = nullptr; _ErrorCode = UserDialogCancel; } } else { _ErrorCode = AE_ErrGeneric; } }
#define MAIN_PROGRESS_THROW(PROGRESS, COUNT, TOTAL) { MAIN_PROGRESS(PROGRESS, COUNT, TOTAL) if( _ErrorCode != NoError) { throw PluginError(GF_PLUGIN_LANGUAGE, _ErrorCode); } }
#define MAIN_PROGRESS_RETURN(PROGRESS, COUNT, TOTAL) { MAIN_PROGRESS(PROGRESS, COUNT, TOTAL) if( _ErrorCode == UserDialogCancel) { return UserDialogCancel; } }

#define MAIN_PROGRESS_NOTHROW(EXPR, THE_ERR) { if(pf_err_dialog_ == PF_Err_NONE) pf_err_dialog_ = (EXPR); if(pf_err_dialog_ == PF_Interrupt_CANCEL) { suites_.AppSuite6()->PF_DisposeAppProgressDialog(pf_dialog_main_); ( THE_ERR ) = UserDialogCancel; } }

namespace RenderBeamer
{
    typedef struct {
        const long    index;
        const char    str[256];
    } ErrorString;

    typedef enum {
        UserEnglish = 0,
        UserGerman,
        UserPolish,
        LangsCount
    } UserLanguage;

    enum {
        UnknownModule = 0,
        MainModule,
        PreCheckModule,
        PrePreparationModule,
        ProjectDumperModule,
        TreeParsingModule,
        CompositionParsingModule,
        FootageParsingModule,
        CopyFootagesModule,
        QueueCollectModule,
        CopyCollectFontsModule,
        CollectEffectsModule,
        RelinkerModule,
        HelperClassesModule,
        LayerParsingModule,
        LayerEffectsModule,
        OutputCollectModule,
        RelinkerInitModule,
        MainCommandHookModule,
        SceneCollectorModule,
        _CallerModulesCount
    };
    typedef long CallerModuleName;
    
    enum {
        NoError=0,
        NullResult,
        ErrorResult,
        NullPointerResult,
        UnknownError,
        RuntimeError,
        StdException,
        AeTypeError,
        UserDialogCancel,
        ProjectNotOpened,
        ProjectNotSaved,
        NoValidRqItems,
        NoRqItemsQueed,
        NoRqItemOutputs,
        ScriptExecuteSuccess,
        ScriptExecuteError,
        Parser_Err,
        Parser_Item_Err,
        MissingAssets,
        MissingFootage,
        DirectoryCreationErr,
        GfsFileOpenError,
        BeamerGetUserName,
        GetLocalTempDirectory,
        GetLocalUsersPlugin,
        GetLocalBeamerPath,
        BeamerSendTaskFailed,
        FailedToOpenWebPage,
        ExecCommandFailed,
        ExecCommandFailedToExec,
        ExecCommandFailedToRead,
        ExecCommandFailedObjWait,
        AE_ErrNone,
        AE_ErrGeneric,
        AE_ErrStruct,
        AE_ErrParameter,
        AE_ErrAlloc,
        AE_ErrWrongThread,
        AE_ErrConstProjectModification,
        AE_ErrMissingSuiteOther,
        _ErrorCodesCount
    };
    typedef long ErrorCodesAE;

    typedef struct ErrorStringTables
    {
        ErrorString CallerModuleString[_CallerModulesCount];
        ErrorString A_Err_ErrorStrings_Eng[8];
        ErrorString GeneralErrorStrings[_ErrorCodesCount];
    } ErrorStringTables;
    
    ErrorCodesAE AeErrToErrorCodeAE(const A_Err &err) noexcept;

    const char *AeGetErrorString(std::string &out_buffer, ErrorCodesAE _err_msg_id=NoError, UserLanguage use_language = GF_PLUGIN_LANGUAGE, CallerModuleName _Caller=UnknownModule) noexcept;
    const char *AeGetCallerString(CallerModuleName _Caller=UnknownModule, UserLanguage use_language = GF_PLUGIN_LANGUAGE) noexcept;

    class PluginError : public std::exception
    {
    public:
        explicit PluginError(std::string _string_message) noexcept
            : _Mybase(), error_string_combined(std::move(_string_message))
        {
        }
        
        explicit PluginError(UserLanguage use_language, ErrorCodesAE _err_msg_id, CallerModuleName _Caller=UnknownModule) noexcept
            : _Mybase()
            , errCallerModuleModule(_Caller), errCode(_err_msg_id), errCodesLanguage(use_language)
        {
            SetErrorString();
        }
        
        explicit PluginError(CallerModuleName _Caller, ErrorCodesAE _err_msg_id, UserLanguage use_language=UserEnglish) noexcept
            : _Mybase()
            , errCallerModuleModule(_Caller), errCode(_err_msg_id), errCodesLanguage(use_language)
        {
            SetErrorString();
        }
        
        ~PluginError() override = default;
        
        const char *what() const noexcept override { return error_string_combined.c_str(); }
        [[nodiscard]] ErrorCodesAE theCode() const { return errCode; }
        
    private:
        typedef std::exception _Mybase;
        void SetErrorString() noexcept;
                
        CallerModuleName errCallerModuleModule=0;
		ErrorCodesAE errCode = 0;
        UserLanguage errCodesLanguage=( GF_PLUGIN_LANGUAGE );
        std::string error_string_combined{};
    };
} // namespace
#endif // !ERRORSHANDLING_H
