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


#define AEREP(SH,FN) SH.UtilitySuite6()->AEGP_ReportInfo(pluginId, FN);
#define REP(SH) AEREP(SH,theErr.what())
#define REP_UN(SH) AEREP(SH,PluginError::GetErrorStringA(ErrorCodesAE::UnknownError,GF_PLUGIN_LANGUAGE))
#define REP_AE(SH) AEREP(SH,PluginError::GetErrorStringA(theErr,GF_PLUGIN_LANGUAGE))

#define ERROR_CATCH_START ErrorCodesAE _ErrorCode = ErrorCodesAE::NoError; { try {
#define ERROR_CATCH_START_MOD(MOD) CallerModuleName _ErrorCaller = MOD; ERROR_CATCH_START
#define ERROR_CATCH_KNOWN1(SH)  catch (const PluginError& theErr) { _ErrorCode=theErr.theCode(); SH } 
#define ERROR_CATCH_KNOWN2(SH,VARERR)  catch (const std::runtime_error VARERR) { _ErrorCode=ErrorCodesAE::RuntimeError; SH } 
#define ERROR_CATCH_KNOWN3(SH,VARERR)  catch (const std::exception VARERR) { _ErrorCode=ErrorCodesAE::StdException; SH } 
#define ERROR_CATCH_KNOWN4(SH,VARERR)  catch (const A_Err VARERR) { _ErrorCode=ErrorCodesAE::AeTypeError; SH } 
#define ERROR_CATCH_UNKNOWN(SH) catch (...) { _ErrorCode=ErrorCodesAE::UnknownError; SH }
#define ERROR_CATCH_END_CONSTRUCT(SH,UN,AE,VR) } ERROR_CATCH_KNOWN1(SH) ERROR_CATCH_KNOWN2(SH,VR) ERROR_CATCH_KNOWN3(SH,VR) ERROR_CATCH_KNOWN4(AE,VR) ERROR_CATCH_UNKNOWN(UN) }

#define ERROR_CATCH_END(SH) ERROR_CATCH_END_CONSTRUCT(REP(SH),REP_UN(SH),REP_AE(SH),&theErr)
#define ERROR_CATCH_END_RETURN(SH) ERROR_CATCH_END(SH) return _ErrorCode;
#define ERROR_CATCH_END_NO_INFO ERROR_CATCH_END_CONSTRUCT(;,;,;,&)
#define ERROR_CATCH_END_NO_INFO_RETURN ERROR_CATCH_END_NO_INFO return _ErrorCode;

#define ERROR_CATCH_END_LOGGER1(OBJECT,TYPE) ERROR_CATCH_END_CONSTRUCT(GF_Dumper::rbProj()->loggErr(OBJECT,TYPE,theErr.what());,GF_Dumper::rbProj()->loggErr(OBJECT,TYPE,PluginError::GetErrorStringA(UnknownError,GF_PLUGIN_LANGUAGE));,GF_Dumper::rbProj()->loggErr(OBJECT,TYPE,PluginError::GetErrorStringA(theErr,GF_PLUGIN_LANGUAGE));,&theErr)
#define ERROR_CATCH_END_LOGGER2(OBJECT,TYPE,LOGGER) ERROR_CATCH_END_CONSTRUCT(LOGGER->loggErr(OBJECT,TYPE,theErr.what());,LOGGER->loggErr(OBJECT,TYPE,PluginError::GetErrorStringA(UnknownError,GF_PLUGIN_LANGUAGE));,LOGGER->loggErr(OBJECT,TYPE,PluginError::GetErrorStringA(theErr,GF_PLUGIN_LANGUAGE));,&theErr)
#define ERROR_CATCH_END_LOGGER(ATYPE) ERROR_CATCH_END_LOGGER1(PluginError::GetCallerStringA(_ErrorCaller,GF_PLUGIN_LANGUAGE),ATYPE)
#define ERROR_CATCH_END_LOGGER_RETURN(ATYPE) ERROR_CATCH_END_LOGGER1(PluginError::GetCallerStringA(_ErrorCaller,GF_PLUGIN_LANGUAGE),ATYPE) return _ErrorCode;

#define ERROR_CHECK(EXPR) { if(_ErrorCode == NoError) ( EXPR ); }
#define ERROR_AE(EXPR) { if(_ErrorCode == NoError) _ErrorCode=(EXPR); }
#define ERROR_AEER(EXPR) ERROR_AE(aErrToRb(EXPR))
#define ERROR_LONG_ERR(EXPR) ERROR_AE(static_cast<ErrorCodesAE>(EXPR))
#define ERROR_BOOL_ERR(EXPR) { if(_ErrorCode == NoError) _ErrorCode=(( EXPR ) == true ? NoError : ErrorResult); }
#define ERROR_RETURN(EXPR) { _ErrorCode = EXPR; if(_ErrorCode != NoError) { return _ErrorCode; } }
#define ERROR_THROW(EXPR) { ErrorCodesAE tmp = (EXPR); if(tmp != NoError) throw PluginError(tmp); }
#define ERROR_THROW_MOD(EXPR) { ErrorCodesAE tmp = (EXPR); if(tmp != NoError) throw PluginError(_ErrorCaller, tmp); }
#define ERROR_THROW_AE(EXPR) { A_Err tmp = (EXPR); if(tmp != A_Err_NONE) throw PluginError(tmp); }
#define ERROR_THROW_AE_MOD(EXPR) { A_Err tmp = (EXPR); if(tmp != A_Err_NONE) throw PluginError(_ErrorCaller, tmp); }

#define GF_PROGRESS(EXPR) { PF_Err PT_res = (EXPR); if(PT_res == PF_Interrupt_CANCEL) return ErrorCodesAE::UserDialogCancel;}
#define MAIN_PROGRESS(PROGRESS, COUNT, TOTAL) { if( _ErrorCode != NoError) { } else if( PROGRESS ) { PF_Err PT_res = suites.AppSuite6()->PF_AppProgressDialogUpdate(( PROGRESS ), ( COUNT ), ( TOTAL )); if(PT_res == PF_Interrupt_CANCEL) { suites.AppSuite6()->PF_DisposeAppProgressDialog( PROGRESS ); ( PROGRESS ) = nullptr; _ErrorCode = ErrorCodesAE::UserDialogCancel; } } else { _ErrorCode = ErrorCodesAE::AE_ErrGeneric; } }
#define MAIN_PROGRESS_THROW(EXPR) { if(pf_err_dialog_ == PF_Err_NONE) pf_err_dialog_ = (EXPR); if(pf_err_dialog_ == PF_Interrupt_CANCEL) { suites_.AppSuite6()->PF_DisposeAppProgressDialog(pf_dialog_main_); throw UserDialogCancel; } }
#define MAIN_PROGRESS_NOTHROW(EXPR, THE_ERR) { if(pf_err_dialog_ == PF_Err_NONE) pf_err_dialog_ = (EXPR); if(pf_err_dialog_ == PF_Interrupt_CANCEL) { suites_.AppSuite6()->PF_DisposeAppProgressDialog(pf_dialog_main_); ( THE_ERR ) = UserDialogCancel; } }

typedef struct {
    const long    index;
    const char    str[256];
} ErrorString;

typedef enum UserLanguage {
	UserEnglish = 0,
	UserGerman,
	UserPolish,
	LangsCount
} UserLanguage;

typedef enum CallerModuleName {
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
	ModulesCount
} CallerModuleName;

typedef enum ErrorCodesAE {
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
	NumberOfElements,
} ErrorCodesAE;
ErrorCodesAE aErrToRb(const A_Err err);

typedef struct ErrorStringTables
{
    ErrorString CallerModuleString[CallerModuleName::ModulesCount];
    ErrorString A_Err_ErrorStrings_Eng[8];
    ErrorString GeneralErrorStrings_Eng[ErrorCodesAE::NumberOfElements];
} ErrorStringTables;

class PluginError : public std::exception
{
public:
	ErrorCodesAE errCode;
	A_Err aeCode;
    const char *message_ptr;
	typedef std::exception _Mybase;

	explicit PluginError(const std::string& _Message)
		: _Mybase()
        , errCode(UnknownError), aeCode(A_Err_NONE)
        , message_ptr(_Message.c_str())
	{
	}

	explicit PluginError(const CallerModuleName& _Caller, const A_Err& _Message, const UserLanguage& lang = GF_PLUGIN_LANGUAGE)
		: _Mybase()
		, errCode(AeTypeError), aeCode(_Message)
        , message_ptr(GetErrorStringS(_Caller, _Message, lang))
    {
	}

	explicit PluginError(const A_Err& _Message, const UserLanguage& lang=GF_PLUGIN_LANGUAGE)
		: _Mybase()
		, errCode(AeTypeError), aeCode(_Message)
        , message_ptr(GetErrorStringA(_Message, lang))
    {
	}

	explicit PluginError(const CallerModuleName& _Caller, const ErrorCodesAE& _Message, const UserLanguage& lang = GF_PLUGIN_LANGUAGE)
		: _Mybase()
		, errCode(_Message), aeCode(A_Err_NONE)
        , message_ptr(GetErrorStringS(_Caller, _Message, lang))
	{
	}

	explicit PluginError(const ErrorCodesAE& _Message, const UserLanguage& lang=GF_PLUGIN_LANGUAGE)
		: _Mybase()
		, errCode(_Message), aeCode(A_Err_NONE)
        , message_ptr(GetErrorStringA(_Message, lang))
	{ 
	}
	~PluginError() = default;
    
    virtual const char *what() const noexcept { return message_ptr; }
	const ErrorCodesAE theCode() const { return errCode; }
	const A_Err theAeCode() const { return aeCode; }

	static const char *GetCallerStringA(const CallerModuleName& caller, const UserLanguage &lang_number) noexcept;
	
	static const char *GetErrorStringA(const A_Err &code_number, const UserLanguage &lang_number) noexcept;
	static const char *GetErrorStringA(const ErrorCodesAE &code_number, const UserLanguage &lang_number) noexcept;

	static const char *GetErrorStringS(const CallerModuleName& caller, const A_Err &code_number, const UserLanguage &lang_number);
	static const char *GetErrorStringS(const CallerModuleName& caller, const ErrorCodesAE &code_number, const UserLanguage &lang_number);
    
    static ErrorStringTables* GetEnglishTable();
};

#endif // !ERRORSHANDLING_H
