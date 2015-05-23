#ifndef __script_int_h__
#define __script_int_h__

interface IScriptEngine : public IUnknown
{
	com_call GetActiveScript( IUnknown** pi_as ) = 0;
	com_call GetActiveScriptParse( IUnknown** pi_asp ) = 0;	
};

#define DEBUGGER_APP_NAME	"VT5 Clobal Script"
#define DEBUGGER_FORM_NAME	"VT5 Forms"
#define DEBUGGER_SECTION	"\\Debugger"
#define DEBUGGER_ENABLE		"EnableDebugger"
#define DEBUGGER_PRESENT	"Debugger Present In System"


interface IDebugHost : public IUnknown
{
	com_call Init( BSTR bstr_app_name ) = 0;
	com_call OnRunScript( BSTR bstrScript, BSTR bstrFileName, BSTR bstrTitle, IActiveScript *pIActiveScript, DWORD* pdw_context ) = 0;	
	com_call CloseScript( DWORD dw_context ) = 0;
	com_call ShowDebugger( DWORD dw_context ) = 0;
	com_call BreakIntoDebuger( ) = 0;
};

declare_interface( IScriptEngine, "9F6B6D79-FFE3-479c-815D-D1947F616F58" );
declare_interface( IDebugHost, "75AAA7AB-F3EE-41c4-87CC-7334A039B739" );



#endif//__script_int_h__
