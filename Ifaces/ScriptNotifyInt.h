#ifndef __scriptnotifyint_h__
#define __scriptnotifyint_h__

interface IScriptSite : public IUnknown
{
	com_call RegisterScript( IUnknown *punk, DWORD dwFlags ) = 0;
	com_call UnregisterScript( IUnknown *punk, DWORD dwFlags  ) = 0;
	com_call UnregisterAll() = 0;
	com_call Invoke( BSTR bstrFuncName, VARIANT* pargs, int nArgsCount, VARIANT* pvarResult, DWORD dwFlags  ) = 0;

	com_call GetFirstPos(TPOS *dwPos) = 0;
	com_call GetNextPos(TPOS *dwPos, IUnknown **punk, DWORD *dwFlags) = 0;
};

interface IScriptNotifyListner : public IUnknown
{
	com_call ScriptNotify( BSTR bstrNotifyDesc, VARIANT varValue ) = 0;
};

declare_interface( IScriptSite, "C5266575-AAEA-410f-82E2-A2DF8D7A3BA1" );
declare_interface( IScriptNotifyListner, "E1CC7C84-B80D-4784-9E9C-B199C95DFF8C" );

enum ScriptSiteFlags
{
	fwFormScript = 1 << 0,
	fwAppScript  = 1 << 1
};

enum NotifyLevel
{
	nfAction = 1,
	nfDocument = 2,
	nfCurrentDocOnly = 4
} ;


#endif //__scriptnotifyint_h__