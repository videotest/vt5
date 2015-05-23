#ifndef __core5_h__
#define __core5_h__

#include "defs.h"
/*#include "activscp.h"


typedef _com_IIID<IActiveScript, &IID_IActiveScript>	IIID_IActiveScript;
typedef _com_IIID<IActiveScriptParse, &IID_IActiveScriptParse>	IIID_IActiveScriptParse;
typedef _com_ptr_t<IIID_IActiveScript>	sptrIActiveScript;
typedef _com_ptr_t<IIID_IActiveScriptParse>	sptrIActiveScriptParse;
*/

//this interfac e
interface IApplication : public IUnknown
{
	com_call GetActionManager( IUnknown **punk ) = 0;
	com_call GetCommandManager( IUnknown **punk ) = 0;
	com_call GetActiveScript( IUnknown **punk ) = 0;
	com_call GetTargetManager( IUnknown **punk ) = 0;

	com_call GetMatchDocTemplate( BSTR bstrFileName, IUnknown **punk ) = 0;

	com_call GetFirstDocTemplPosition( LONG_PTR *plPos ) = 0;
	com_call GetNextDocTempl( LONG_PTR *plPos, BSTR *pbstrName, IUnknown **punk ) = 0;
			//temporely IUnknow doesn/t used

	com_call GetFirstDocPosition( LONG_PTR lPosTemplate, LONG_PTR *plPosDoc ) = 0;
	com_call GetNextDoc( LONG_PTR lPosTemplate, LONG_PTR *plPosDoc, IUnknown **punkDoc ) = 0;

	com_call GetActiveDocument( IUnknown **plDoc ) = 0;
	com_call ProcessMessage() = 0;
	com_call GetMainWindowHandle( HWND *phWnd ) = 0;

	com_call GetStatusBar( IUnknown **punk ) = 0;
	com_call SetNestedMessageLoop( BOOL bSet, BOOL *pbOld ) = 0;
	com_call IsNestedMessageLoop() = 0;
	com_call IdleUpdate() = 0;
};

interface ICommandManager : public IUnknown
{
	com_call ExecuteContextMenu( BSTR bstrTargetName, POINT point ) = 0;
	com_call AddAction( IUnknown *punkActionInfo ) = 0;
	com_call RemoveAction( IUnknown *punkActionInfo ) = 0;
};

interface ICommandManager2 : public ICommandManager
{
	com_call RegisterMenuCreator( IUnknown *punk ) = 0;
	com_call UnRegisterMenuCreator( IUnknown *punk ) = 0;

	com_call GetActionName( BSTR *pbstrActionName, int nID ) = 0;
	com_call GetActionID( BSTR bstrActionName, int *pnID ) = 0;

	com_call ExecuteContextMenu2( BSTR bstrTargetName, POINT point, DWORD dwFlag ) = 0;
	com_call ExecuteContextMenu3( BSTR bstrTargetName, POINT point, DWORD dwFlag, UINT *puCmd ) = 0;

};

interface IOperationManager : public IUnknown
{
	com_call RegisterPermanent( IUnknown *punk ) = 0;
	com_call UnRegisterPermanent( IUnknown *punk ) = 0;
};

interface IOperationManager2 : public IOperationManager
{
	// 0 - clear terminate flag
	// 1 - set terminate flag
	// 2 - set terminate flag with autoreset (terminates only one action, set flag to 0 after termination).
	com_call TerminateCurrentOperation(long lTerminateFlag) = 0;
};


interface IMenuInitializer : public IUnknown
{
	com_call OnInitPopup( BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu ) = 0;
};

interface IMenuInitializer2 : public IMenuInitializer
{
	com_call UpdateActionState( BSTR bstrAction, UINT id, DWORD *pdwFlags ) = 0;
};


/*struct __declspec(uuid( "51CE6793-2220-11d3-A5D6-0000B493FF1B" )) IApplication;
typedef _com_IIID<IApplication, &__uuidof(IApplication)>	iiid_IApplication;
typedef _com_ptr_t<iiid_IApplication>	sptrIApplication;
const IID IID_IApplication	= __uuidof( IApplication );*/

declare_interface( IApplication, "51CE6793-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( ICommandManager, "51CE6794-2220-11d3-A5D6-0000B493FF1B")
declare_interface( ICommandManager2, "05816FD1-4F26-11d3-A61F-0090275995FE" )
declare_interface( IMenuInitializer, "05816FD2-4F26-11d3-A61F-0090275995FE" )
declare_interface( IOperationManager, "FBF0D76B-F3E0-4d9b-8903-1A2AB4A89209" )
declare_interface( IOperationManager2, "759868A0-9F4C-4144-8344-F104C994ACE7" )
declare_interface( IMenuInitializer2, "76041E95-0622-489a-8E51-E2785F543BC3" )


#endif //__core5_h__