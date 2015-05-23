#ifndef __scriptdocint_h__
#define __scriptdocint_h__

#include "activdbg.h"

interface IProvideDebuggerIDE : public IUnknown
{
	com_call GetApplicationDebugger(IApplicationDebugger** ppad) = 0;
	com_call ToggleBreakpoint() = 0;
	com_call SetBreakpointsToEngine(DWORD dwContext) = 0;
};

interface IScriptDataObject : public IUnknown
{
	com_call InitNew() = 0;
	com_call ReadFile( BSTR bstr ) = 0;
	com_call WriteFile( BSTR bstr ) = 0;

	com_call GetText( BSTR *pbstr ) = 0;
	com_call GetFullText( BSTR *pbstr ) = 0;
	com_call GetLinesCount( long *pnCount ) = 0;
	com_call GetLine( long index, BSTR *pbstrText ) = 0;

	com_call InsertString( BSTR bstr, long index ) = 0;
	com_call DeleteString( long index ) = 0;

	com_call GetPrivateDataPtr( DWORD *pdwDataPtr ) = 0;
	com_call RegisterScriptAction() = 0;
};

interface IScriptDebugger : public IUnknown
{
	com_call SetBreakOnStart() = 0;
	com_call ConnectDebugger() = 0;
	com_call SetScriptText(BSTR bstrScriptText, DWORD* pdwContext) = 0;
	com_call Reset() = 0;
	com_call StartDebugger() = 0;
};



declare_interface( IScriptDataObject, "D363CEB1-23F3-11d3-A5D8-0000B493FF1B" );
declare_interface( IProvideDebuggerIDE, "B09F91C0-7DCD-4223-986A-0FE838A6CD22");
declare_interface( IScriptDebugger, "B5960925-006B-4e81-962C-8624B1AC66B9");


#endif //__scriptdocint_h__