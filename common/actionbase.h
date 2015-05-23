#ifndef __actionbase_h__
#define __actionbase_h__

#include "cmnbase.h"
#include "registry.h"
#include "core5.h"

#include "action5.h"
#include "wndbase.h"
#include "helpbase.h"
#include "objbase_.h"
#include "mousebase.h"
#include "Factory.h"

#ifndef __AFXTEMPL_H__
#pragma message( "Warning: You should include afxtempl.h to stdafx.h to avoid this message" )
#include "afxtempl.h"
#endif //__AFXTEMPL_H__

struct	ArgumentInfo;

//helper class for store information about ToolBar
//!!!todo: Add support for external bitmaps loading
//!!!todo: possible it requires AddBitmapData member for VB scripting - may be in
//!!!todo: support "large" and "small" bitmap
//derived class
class CBitmapProvider : public CCmdTargetEx,
						public CNumeredObjectImpl
{
public:
	long	m_iCounter;	//counter of loaded bitmaps
	CSize	m_ImageSize;
	CPtrArray	m_arrLookedInstances;

	struct CmdInfo
	{
		UINT	nCmd;
		DWORD	dwDllCode;
		long	lBmpNo;
		long	lPictNo;
	};
protected:
	//DECLARE_DYNCREATE(CBitmapProvider);
	//it will never been created by CoCreateInstance
	//DECLARE_OLECREATE(CBitmapProvider);	
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();
public:
	CBitmapProvider();
	~CBitmapProvider();

	//do nothing
	virtual void Init();		
	//free all resources
	virtual void DeInit();
	void EnsureInstanceWasLooked( HINSTANCE hInstance );
public:
	virtual long GetBitmapCount();
	virtual HANDLE GetBitmap( int idx );
	virtual CmdInfo *GetCmdInfo( UINT nCmd, DWORD dwDllCode );
protected:
	BEGIN_INTERFACE_PART_EXPORT(Bmps, IBitmapProvider)
		com_call GetBitmapCount( long *piCount );
		com_call GetBitmap( int idx, HANDLE *phBitmap );
		com_call GetBmpIndexFromCommand( UINT nCmd, DWORD dwDllCode, long *plIndex, long *plBitmap );
	END_INTERFACE_PART(Bmps)

	CPtrArray	m_arrBmps;
	CPtrArray	m_arrCmdInfos;
};

class CActionInfoWrp;
class CActionBase;
//the main purpose of this class is implementation low-level service for
//easy making action classes
class std_dll CActionDlgHelper : public CCmdTargetEx
{
	DECLARE_INTERFACE_MAP();
protected:
	CActionBase	*m_pa;
public:
	CActionDlgHelper( CActionBase *paction );
	virtual ~CActionDlgHelper();

	BEGIN_INTERFACE_PART_EXPORT(Dlg, ICustomizableAction)
		com_call CreateSettingWindow( HWND hWnd );
		com_call GetFlags( DWORD *pdwFlags );
	END_INTERFACE_PART(Dlg)
};

class std_dll CActionUndoHelper : public CCmdTargetEx
{
	DECLARE_INTERFACE_MAP();
protected:
	CActionBase	*m_pa;
public:
	CActionUndoHelper( CActionBase *paction );
	virtual ~CActionUndoHelper();

	BEGIN_INTERFACE_PART_EXPORT(Undo, IUndoneableAction)
		com_call StoreState();
		com_call Undo();
		com_call Redo();
		com_call GetTargetKey( GUID *plKey );
	END_INTERFACE_PART(Undo)
};


class std_dll CActionBase : public CCmdTargetEx,
			public CNumeredObjectImpl,
			public CRootedObjectImpl			

{
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();
public:
	IActionInfo2	*m_pActionInfo;
	IUnknown		*m_punkTarget;
	GuidKey			m_lTargetKey;
protected:
	IUnknown		*m_punkDlgHelper;
	IUnknown		*m_punkUndoHelper;
	DWORD			m_dwExecuteSettings;
	CString			m_strErrorString;
	CModifiedUndoRecord		m_modified;
	
public:
	CActionBase();
	virtual ~CActionBase();

	BEGIN_INTERFACE_PART_EXPORT(Action, IAction)
		com_call Invoke();
		com_call AttachTarget( IUnknown *punkTarget );
		com_call GetActionState( DWORD *pdwState );
		com_call SetArgument( BSTR bstrArgName, VARIANT *pvarVal );
		com_call SetResult( BSTR bstrArgName, VARIANT *pvarVal );
		com_call StoreParameters( BSTR *pbstr );
		com_call GetActionInfo(IUnknown** ppunkAI);
		com_call GetTarget( IUnknown **ppunkTarget );
	END_INTERFACE_PART(Action)
	BEGIN_INTERFACE_PART_EXPORT(Args, IActionArgumentHelper)
		com_call SetArgumentHelper( BSTR bstrArgName, VARIANT *pvarArg );
		com_call GetResultHelper( BSTR bstrResName, VARIANT *pvarResult );
	END_INTERFACE_PART(Args)
public:
	virtual bool CanInvoke();
	virtual bool Invoke();
//customizable actions
	virtual bool ExecuteSettings( CWnd *pwndParent ){return false;};
//undoneable action
	virtual bool DoUndo(){return false;};
	virtual bool DoRedo(){return false;};
	virtual bool StoreState(){return false;};

//undoneable helpers
	virtual bool PrepareUndoRedo();
	virtual bool UnPrepareUndoRedo();

	virtual void OnAttachTarget( IUnknown *punkTarget, bool bAttach ){};

	virtual CString GetTextParams();
	virtual CString GetName();

	virtual void Init();

	virtual bool HasSettings(){return false;}
	virtual bool HasUndo(){return false;}
//action state flags

	virtual DWORD GetActionState()
	{	
		DWORD	dw = 0;
		if( IsAvaible() )dw |= afEnabled;
		if( IsChecked() )dw |= afChecked;
		if( IsRequiredInvokeOnTerminate() )dw |= afRequiredInvokeOnTerminate;
		if( IsCompatibleTarget() )dw |= afTargetCompatibility;
		return dw;
	}
	virtual bool IsAvaible(){return true;};
	virtual bool IsChecked(){return false;};
	virtual bool IsRequiredInvokeOnTerminate(){return false;}
	virtual bool IsCompatibleTarget(){return true;}
public:
	IApplication *GetApplication();
	IActionManager *GetActionManager();
public:
	virtual void MakeModified();
	virtual void SetModified( bool bSet );	//set if bSet == true, reset if false
	virtual void StoreCurrentExecuteParams(){};
protected:
	VARIANT GetArgument( const char *szArgName );
	CString GetArgumentString( const char *szArgName );
	long GetArgumentInt( const char *szArgName );
	double GetArgumentDouble( const char *szArgName );
	/*CString GetBoolArgument( const char *szArgName );
	CString GetDoubleArgument( const char *szArgName );*/

	void SetArgument( const char *szArgName, const VARIANT var );
	void SetResult( const char *szArgName, VARIANT *pvarRes );
	
	CMap<CString, LPCTSTR, COleVariant, COleVariant&>m_args;

	bool ForceShowDialog();
	bool ForceNotShowDialog();

	void SetError( UINT	nIDString )
	{	m_strErrorString.LoadString( nIDString );	}
};

#define ENABLE_SETTINGS()	virtual bool HasSettings(){return true;}
#define ENABLE_UNDO()		virtual bool HasUndo(){return true;}

class std_dll CActionInfoBase : 
		public CCmdTargetEx,
		public CRootedObjectImpl,
		public CNumeredObjectImpl,
		public CHelpInfoImpl
				
{
	
	DECLARE_DYNAMIC(CActionInfoBase);
	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();
protected:
	CString	m_strName,
			m_strUserName,
			m_strMenuNames,
			m_strGroupName,
			m_strHelpStringTooltip,
			m_strToolBarName;

	bool	m_bInitialized;
	bool	m_bShouldReleaseBmpProvider;

	UINT	m_nToolBarResourceID;
	UINT	m_nMenuResourceID;
	int		m_nLocalID;

	// [vanek] : support IActionInfo3 interface - 13.12.2004
	IUnknownPtr	m_spunkBmpHelper;

	virtual void Init();
	virtual CRuntimeClass *GetActionRuntime() = 0;
	virtual UINT GetCommand() = 0;
	virtual HINSTANCE GetHinstance() = 0;
	virtual const char *GetTarget() = 0;
	virtual CRuntimeClass *GetUIRuntime() = 0;

	virtual CString GetDefMenuString();
	virtual CString GetDefToolBarString();

	virtual ArgumentInfo	*GetArgumentInfos() = 0;
	const char *GetHelpTopic()	{return m_strName;}


	int GetArgCount( bool bIn );
	void LoadString( const char *psz, bool bAliase );
	ArgumentInfo *GetArgInfo( int idx, bool bIn, int* pNumber = 0 );
public:
	CActionInfoBase();
	~CActionInfoBase();
public:
	static CBitmapProvider *s_pBmps;

	BEGIN_INTERFACE_PART_EXPORT(Info, IActionInfo3)
		com_call IsArgumentDataObject( int iParamIdx, BOOL bInArg, BOOL *pbIs );
		com_call GetArgsCount( int *piParamCount );
		com_call GetArgInfo( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKnid, BSTR *pbstrDefValue );
		com_call GetOutArgsCount( int *piParamCount );
		com_call GetOutArgInfo( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind );
		com_call IsFunction( BOOL *pboolAction );
	//user interface
		com_call GetCommandInfo( 
			//general information
			BSTR	*pbstrActionName,
			BSTR	*pbstrActionUserName,
			BSTR	*pbstrActionHelpString, //"Image rotation\nRotation
			BSTR	*pbstrGroupName	);
		com_call GetTBInfo(
			//toolbar information
			BSTR	*pbstrDefTBName,	//default toolbar name (as "edit operaion")
			IUnknown **ppunkBitmapHelper );
		com_call GetHotKey( DWORD *pdwKey, BYTE *pbVirt );
			//menu information
		com_call GetRuntimeInformaton( DWORD *pdwDllCode, DWORD *pdwCmdCode );
		com_call GetMenuInfo(
			BSTR	*pbstrMenus,		//"general", "image doc", ... if action avaible in several menus, it should 
			DWORD	*pdwMenuFlag );		//not used, must be zero. Will be used later, possible, 
										//for special sub-menu (such as "Recent files")
		com_call ConstructAction( IUnknown **ppunk );
		com_call GetTargetName( BSTR *pbstr );

		com_call GetArgInfo2( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKnid, BSTR *pbstrDefValue, int* pNumber );
		com_call GetOutArgInfo2( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind, int* pNumber );
		com_call GetUIUnknown( IUnknown **ppunkinterface );
		com_call GetLocalID( int *pnID );
		com_call SetLocalID( int nID );
		com_call SetActionString( BSTR	bstr );
		//  [vanek]:  IActionInfo3 - 13.12.2004
		com_call SetBitmapHelper( IUnknown *punkBitmapHelper );
	END_INTERFACE_PART(Info)
};


//macros for auto generation ActionInfo component

#define ACTION_INFO_FULL(class_name, idd, iddMenu, iddToolBar, clsid)									\
class __ActionInfo##class_name : public CActionInfoBase													\
{																										\
	DECLARE_DYNCREATE(__ActionInfo##class_name);														\
	GUARD_DECLARE_OLECREATE(__ActionInfo##class_name);													\
																										\
	virtual HINSTANCE GetHinstance()																	\
	{		return AfxGetApp()->m_hInstance;/*InstanceHandle();*/	}									\
	virtual CRuntimeClass *GetActionRuntime()															\
	{	return RUNTIME_CLASS( class_name );	}															\
	virtual CRuntimeClass *GetUIRuntime()																\
	{	return s_pclassUI;	}																			\
	virtual UINT GetCommand()																			\
	{		return idd;	}																				\
	static CString	GetProgID()																			\
	{																									\
		static CString	s;																				\
		s = CActionRegistrator::_generate_progid( #class_name ); 										\
		return s;																						\
	}																									\
	virtual CString GetDefMenuString()																	\
	{																									\
		UINT	nID = iddMenu; 																			\
		if( nID == -1 )	return CActionInfoBase::GetDefMenuString();										\
		CString	s;	s.LoadString( nID );	return s;													\
	}																									\
	virtual CString GetDefToolBarString()																\
	{																									\
		UINT	nID = iddToolBar; 																		\
		if( nID == -1 )	return CActionInfoBase::GetDefToolBarString();									\
		CString	s;	s.LoadString( nID );	return s;													\
	}																									\
	virtual const char *GetTarget()																		\
	{	return s_strTarget;	}																			\
	virtual ArgumentInfo	*GetArgumentInfos()															\
	{	return s_pargs;	}																				\
	static ArgumentInfo	*s_pargs;																		\
	static CString	s_strTarget;																		\
	static CRuntimeClass *s_pclassUI;																	\
};																										\
ArgumentInfo	*__ActionInfo##class_name::s_pargs = 0;													\
CString	__ActionInfo##class_name::s_strTarget = szTargetApplication;									\
CRuntimeClass	*__ActionInfo##class_name::s_pclassUI = 0;												\
IMPLEMENT_DYNCREATE(__ActionInfo##class_name, CCmdTargetEx)												\
GUARD_IMPLEMENT_OLECREATE_SHORT(__ActionInfo##class_name, __ActionInfo##class_name::GetProgID(), clsid)

#define ACTION_INFO(class_name, idd, clsid)	ACTION_INFO_FULL(class_name, idd, -1, -1, clsid)

class std_dll CActionRegistrator:public CCompRegistrator
{
public:
	CActionRegistrator();

	void RegisterActions( bool bRegister = true );
public:
	static CString _generate_progid( const char *sz )
	{	return CString( "VideoTesT5Action." )+sz;	}

	bool IsAction( const char *szProgID );
};


//interactive action implementation
class std_dll CInteractiveActionBase : public CActionBase,
							public CMsgListenerImpl,
							public CDrawObjectImpl,
							public CMouseImpl,
							public CEventListenerImpl
{
public:
	enum ActionState
	{
		asInactive,
		asActive,
		asReady,
		asTerminate
	};
protected:
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();
	DECLARE_MESSAGE_MAP();
protected:
	CInteractiveActionBase();
	virtual ~CInteractiveActionBase();

	
//interface implementation helper
	virtual bool _Init();
	virtual void _DeInit();

	virtual void _Draw( CDC *pdc = 0 );
	BEGIN_INTERFACE_PART_EXPORT(IntAction, IInteractiveAction)
		com_call BeginInteractiveMode();
		com_call TerminateInteractive();
		com_call IsComplete();
	END_INTERFACE_PART(IntAction);

protected:
	virtual void OnAttachTarget( IUnknown *punkTarget, bool bAttach );

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
	{	return CMouseImpl::OnWndMsg( message, wParam, lParam, pResult );	}

	virtual IUnknown *GetTargetWindow();
	virtual IUnknown *GetTarget()	{return m_punkTarget;}
	virtual CRect	GetInvalidateRect();

public:
	//return the current action state
	ActionState	GetState()	const		{return m_state;}
	virtual bool CanInvoke();

	//this virtual is called when window should set cursor
	virtual bool DoSetCursor( CPoint point );
	//this virtual is called whan settings on action property page is changed
	virtual bool DoUpdateSettings();
	virtual bool DoTerminate();		//this virtual called when user terminate the action
									//return true if target window should be repainted
	virtual bool DoChar( UINT nChar );
	virtual bool DoLButtonDblClick( CPoint pt )					{return DoLButtonDown( pt );};
	virtual void DoActivateObjects( IUnknown *punkObject )		{}

	virtual bool Initialize();
	virtual void Finalize();
	virtual void Terminate();
	virtual void LeaveMode();
	virtual void _MouseEvent( MouseEvent me, CPoint point );

	void EnterInteractiveMode();
	void LeaveInteractiveMode();

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	//add the name of property page as NC
	void AddPropertyPageDescription( const char *szPageName );
	void AddPropertyPageDescription( UINT nID );	//resource-based version
	void AddPropertyPageDescriptionByProgID( const char *szPageProgID );
	void UpdatePropertyPages();

	//get the properties pages count
	virtual long GetPropertyPagesCount()					{return (long)m_pages.GetSize();};
	//get the properties pages title name
	virtual CString GetPropertyPageName( long nPage )		{return m_pages[nPage];};
protected:
	ActionState		m_state;				//current action state
	bool			m_bStoreInteractiveFlag;
	HWND			m_hwndTarget;
//overrided
	HCURSOR				m_hcurActive;
	CStringArray		m_pages;
	CStringArray		m_pagesProgID;
	bool				m_bOldEnable;
};


#endif //__actionbase_h__
