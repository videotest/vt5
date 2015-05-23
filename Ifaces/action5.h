#ifndef __action5_h__
#define __action5_h__

//struct __POSITION {};
//typedef __POSITION* POSITION;
typedef POSITION TPOS;

#include "defs.h"

//script plug-in must support the following interfaces:
//1. INamedObject for present it as global namespace variable in script
//2. IDispatch for executing it
//it must be registred in following section
//Action component should support IAction, INameable interface
//	also it may support IUndonable, IStoraged interface
//	also it may support IInteractive interface
//	also it may support ICmdInfo interface
//	also it may support IProtected interface
interface IAction : public IUnknown
{
	com_call AttachTarget( IUnknown *punkTarget ) = 0;
	com_call SetArgument( BSTR bstrArgName, VARIANT *pvarVal ) = 0;
	com_call SetResult( BSTR bstrArgName, VARIANT *pvarVal ) = 0;

	com_call Invoke() = 0;

	com_call GetActionState( DWORD *pdwState ) = 0;
	com_call StoreParameters( BSTR *pbstr ) = 0;

	com_call GetActionInfo(IUnknown** ppunkAI) = 0;
	com_call GetTarget( IUnknown **ppunkTarget ) = 0;
};

interface IActionArgumentHelper : public IUnknown
{
	com_call SetArgumentHelper( BSTR bstrArgName, VARIANT *pvarArg ) = 0;
	com_call GetResultHelper( BSTR bstrResName, VARIANT *pvarResult ) = 0;
};

interface IUndoneableAction : public IUnknown
{
	com_call StoreState() = 0;
	com_call Undo() = 0;
	com_call Redo() = 0;
	com_call GetTargetKey( GUID *plKey ) = 0;
//	com_call CalcMemSize( DWORD *pdwMemUsed ) = 0;
//	com_call ExtractUndoIcon( HANDLE hImageList, int *piReturnPos ) = 0;
};

enum CustomizableActionFlags
{
};

interface ICustomizableAction : public IUnknown
{
	com_call CreateSettingWindow( HWND hWnd ) = 0;
	com_call GetFlags( DWORD *pdwFlags ) = 0;
};

interface IInteractiveAction : public IUnknown
{
	com_call BeginInteractiveMode() = 0;
	com_call TerminateInteractive() = 0;

	com_call IsComplete() = 0;					//return S_OK if action is complete or S_FALSE if not
};

interface ILongOperation : public IUnknown
{
	com_call AttachNotifyWindow( HWND hWndNotifyWindow ) = 0;
	com_call GetCurrentPosition( int *pnStage, int *pnPosition, int *pnPercent ) = 0;
	com_call GetNotifyRanges( int *pnStageCount, int *pnNotifyCount ) = 0;
	com_call Abort() = 0;
};

/////////////////////////////////////////////////////////////////////////////////////

interface IActionInfo : public IUnknown
{
//arguments/results
	com_call GetArgsCount( int *piParamCount ) = 0;
	com_call GetArgInfo( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind, BSTR *pbstrDefValue ) = 0;
	com_call GetOutArgsCount( int *piParamCount ) = 0;
	com_call GetOutArgInfo( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind ) = 0;
	com_call IsFunction( BOOL *pboolAction ) = 0;
//user interface
	com_call GetCommandInfo( 
		//general information
		BSTR	*pbstrActionName,
		BSTR	*pbstrActionUserName,
		BSTR	*pbstrActionHelpString, //"Image rotation\nRotation
		BSTR	*pbstrGroupName	) = 0;
	com_call GetTBInfo(
			//toolbar information
			BSTR	*pbstrDefTBName,	//default toolbar name (as "edit operaion")
			IUnknown **ppunkBitmapHelper ) = 0;
		//menu information
	com_call GetMenuInfo(
		BSTR	*pbstrMenuNames,	//"general\\File\\Open", "main\\Tools\\Image tranphormation\\Gamma correction", ...
		DWORD	*pdwMenuFlag ) = 0;		//not used, must be zero. Will be used later, possible, 
	com_call GetHotKey( DWORD *pdwKey, BYTE *pbVirt ) = 0;
	com_call GetRuntimeInformaton( DWORD *pdwDllCode, DWORD *pdwCmdCode ) = 0;
	com_call ConstructAction( IUnknown **ppunk ) = 0;

	com_call GetTargetName( BSTR *pbstr ) = 0;
	com_call GetLocalID( int *pnID ) = 0;
	com_call SetLocalID( int nID ) = 0;
};

interface IActionInfo2 : public IActionInfo
{
	com_call IsArgumentDataObject( int iParamIdx, BOOL bInArg, BOOL *pbIs ) = 0;

	com_call GetArgInfo2( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKnid, BSTR *pbstrDefValue, int* pNumber ) = 0;
	com_call GetOutArgInfo2( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind, int* pNumber ) = 0;
	com_call GetUIUnknown( IUnknown **ppunkinterface ) = 0;	
	com_call SetActionString( BSTR	bstr ) = 0;
};

// [vanek] : use overloaded icons for actions (see CCommandManager::XMan2::AddAction) - 13.12.2004
interface IActionInfo3 : IActionInfo2
{
	com_call SetBitmapHelper( IUnknown *punkBitmapHelper ) = 0;
};


//UI possible interfaces
interface IUIDropDownMenu : public IUnknown
{
	com_call AppendMenuItems( HMENU hMenu, UINT nCommandBase ) = 0;
	com_call ExecuteCommand( UINT nCommand ) = 0;
};


//UI combo box 
interface IUIComboBox : public IUnknown
{
	com_call OnInitList( HWND hWnd ) = 0;
	com_call OnSelChange() = 0;
	com_call OnDetachWindow() = 0;
};


//ActionManager component should support IActionManager interface
//	and IDispatch interface
//IDispath interface has special design for executing action's in namespace

enum ActionFlags
{
	afEnabled					= 1 << 0,
	afChecked					= 1 << 1,
	afRequiredInvokeOnTerminate = 1 << 2,
	afTargetCompatibility		= 1 << 3,
	afLeaveModeOnChangeTarget   = 1 << 4,
	afPrivileged				= 1 << 5,
	afHasPropPage				= 1 << 6, // для скриптов в методиках - что есть форма
};

enum ActionExecuteFlags
{
	aefShowInterfaceByRequired = 0,
	aefShowInterfaceAnyway = 1,
	aefNoShowInterfaceAnyway = 2,
	aefRunning = 4,
	aefInternalExecute = 8,
	aefScriptRunning = 16,
	aefNoPreformOperation  = 32,
	aefInteractiveRunning = 64,
	aefDelayed = 128,
	aefClearDelayedList = 256,
	aefNoUndo = 512,
};


interface IActionManager : public IUnknown
{
	com_call ExecuteAction( BSTR bstrActionName, BSTR bstrExecuteParams, DWORD dwExecuteFlags ) = 0;
	com_call GetActionFlags( BSTR bstrActionName, DWORD *pdwFlags ) = 0;

	com_call GetCurrentExecuteFlags( DWORD *pdwFlags ) = 0;

	com_call SetScriptRunningFlag( BOOL bSet ) = 0;
	com_call SetInteractiveRunningFlag( BOOL bSet ) = 0;
	com_call IsScriptRunning() = 0;
	com_call TerminateInteractiveAction() = 0;

	com_call EnableManager(BOOL bEnable) = 0; //if bEnable==FALSE than prevent actions execute
	com_call ExecuteDelayedActions() = 0;
	com_call InitDelayExecuting() = 0;
	com_call GetActionInfo( UINT nDispID, IUnknown **ppunkInfo ) = 0;
	com_call AddCompleteActionToUndoList( IUnknown *punkAction ) = 0;

	com_call GetActionInfo(BSTR bstrActionName, IUnknown **ppunkInfo) = 0;
	com_call GetRunningInteractiveActionInfo(IUnknown **ppunkInfo) = 0;
	com_call ChangeInteractiveAction(IUnknown *punkInfo) = 0;
	
	com_call SetActionFlags( BSTR bstrActionName, DWORD dwState ) = 0;//override default action flags. The main purpose is setup custom states for actions VBScript
	com_call ResetActionFlags( BSTR bstrActionName ) = 0;
	com_call ResetAllActionsFlags() = 0;
	com_call GetRunningInteractiveAction(IUnknown **ppunkAction) = 0;
	com_call LockInitInteractive( BOOL bLock, BOOL *pbOldLock ) = 0;
	com_call FinalizeInteractiveAction() = 0;
};

interface IActionManager2 : public IActionManager
{
	com_call ObfuscateName(BSTR* pbstrName) = 0; // "изувечить" имя
	com_call DeobfuscateName(BSTR* pbstrName) = 0; // перевести "изувеченное" имя в нормальное; если не получается - вернуть как есть
};

interface IBitmapProvider  : public IUnknown
{
	com_call GetBitmapCount( long *piCount ) = 0;
	com_call GetBitmap( int idx, HANDLE *phBitmap ) = 0;
	com_call GetBmpIndexFromCommand( UINT nCmd, DWORD dwDllCode, long *plIndex, long *plBitmap ) = 0;
};

// [vanek] : use overloaded icons for actions - 06.12.2004
interface IAdvancedBitmapProvider  : public IBitmapProvider
{
	com_call SetBackColor( COLORREF crBack ) = 0;
	com_call GetBackColor( COLORREF *pcrBack ) = 0;
	com_call Load( ) = 0;
    com_call GetBmpIndexFromName( BSTR bstrName, long *plIndex, long *plBitmap ) = 0;
	com_call CatchActionInfo( IUnknown *punkActionInfo ) = 0;	
};

interface IActionTargetManager : public IUnknown
{
	com_call RegisterTarget( IUnknown *punkTarget, BSTR bstr ) = 0;
	com_call UnRegisterTarget( IUnknown *punkTarget ) = 0;
	com_call ActivateTarget( IUnknown *punkTarget ) = 0;
	com_call GetTarget( BSTR bstr, IUnknown **punkTarget ) = 0;
	com_call GetTargetByKey( GUID lKey, IUnknown **ppunk ) = 0;
};

interface IFilterAction : public IUnknown
{
	com_call GetFirstArgumentPosition(TPOS *pnPos) = 0;
	com_call GetNextArgument(IUnknown **ppunkDataObject, TPOS *pnPosNext) = 0;
	com_call GetArgumentInfo(TPOS lPos, BSTR *pbstrArgType, BSTR *pbstrArgName, BOOL *pbOut) = 0;
	com_call SetArgument(TPOS lPos, IUnknown *punkDataObject, BOOL bDataChanged) = 0;
	com_call InitArguments() = 0;
	com_call LockDataChanging( BOOL bLock ) = 0;
	com_call SetPreviewMode( BOOL bEnter ) = 0;
	com_call CanDeleteArgument( IUnknown *punk, BOOL *pbCan ) = 0;
};

interface IFilterAction2 : public IFilterAction
{
	com_call NeedActivateResult( IUnknown *punk, BOOL *pbCan ) = 0;
};


interface IPaintInfo : public IUnknown
{
	com_call GetInvalidRect( RECT* pRect ) = 0;
};


declare_interface( IAction, "51CE6795-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( IActionArgumentHelper, "51CE6796-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( IUndoneableAction, "51CE6797-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( ICustomizableAction, "51CE6798-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( IInteractiveAction, "51CE6799-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( ILongAction, "51CE679A-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( IActionInfo, "51CE679B-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( IActionInfo2, "75CDCE21-518D-11d3-A624-0090275995FE" )
declare_interface( IActionInfo3, "30EC3303-8CD1-4cb8-B7DB-4509EE54E173" )
declare_interface( IActionManager, "51CE679C-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( IActionManager2, "EF1FE8F1-49F5-4d34-95DF-19ECE4CE7CEB" )
declare_interface( IBitmapProvider, "51CE679D-2220-11d3-A5D6-0000B493FF1B"  )
declare_interface( IAdvancedBitmapProvider, "620A0DB9-E9EA-46f8-A185-EB5012E43B63" )
declare_interface( IActionTargetManager, "51CE679E-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( ILongOperation, "3FD04961-702D-11d3-A65A-0090275995FE" )
declare_interface( IPaintInfo, "9D7E5CA1-8310-11d3-A516-0000B493A187" )
declare_interface( IFilterAction, "4950123C-E38A-4356-B315-9DA25D1F50CF" )
declare_interface( IFilterAction2, "57C12353-81C6-4cfe-88B0-D1603F056DDF" )
declare_interface( IUIDropDownMenu, "B88F20E8-9868-43e1-84CE-35E43006AEBB" )
declare_interface( IUIComboBox, "6FEA6FBD-CDB4-4983-A8EC-EE128CF7FC64" )




#endif //__action5_h__