#ifndef __cmnbase_h__
#define __cmnbase_h__

#include "registry.h"
#include "defs.h"
#include "utils.h"
#include "misc5.h"
#include "action5.h"
#include "Classes5.h"
#include "fileopenint.h"
#include "compmanager.h"
#include "cmdtargex.h"
#include <afxTempl.h>


class std_dll CNamedObjectImpl : public CImplBase
{
protected:
	CString	m_sName;
	CString	m_sUserName;

protected:
	CNamedObjectImpl(){}

	virtual void OnChangeName(){};
	virtual void SetName( const char *sz ){ m_sName = sz; }
	const char *GetName(){ return m_sName; }
	virtual void SetUserName( const char *sz ){ m_sUserName = sz; }
	virtual const char *GetUserName(){ return m_sUserName.IsEmpty()?m_sName:m_sUserName; }
	bool IsNameEmpty(){return m_sName.IsEmpty() == TRUE;}
	bool IsUserNameEmpty(){return m_sName.IsEmpty() == TRUE;}


	BEGIN_INTERFACE_PART_EXPORT(Name, INamedObject2)
		com_call GetName( BSTR *pbstr );
		com_call GetUserName( BSTR *pbstr );
		com_call SetName( BSTR bstr );
		com_call SetUserName( BSTR bstr );
		com_call GenerateUniqueName( BSTR bstrBase );
	END_INTERFACE_PART(Name)
};

//purpose of this interface is support unique number.
class std_dll CNumeredObjectImpl : public CImplBase
{				
protected:
	GuidKey	m_key;
	long	m_nCreatedNum;
	static long s_nCounter;
protected:
	CNumeredObjectImpl();
	virtual ~CNumeredObjectImpl();

	void GenerateKey();
	GuidKey GenerateNewKey();
	virtual void ReplaceObjectKey(GuidKey & rKey);

protected:	
	BEGIN_INTERFACE_PART_EXPORT(Num, INumeredObject)
		com_call AssignKey( GUID Key );
		com_call GetKey( GUID *pKey );
		com_call GenerateNewKey();
		com_call GetCreateNum( long *plNum );
		com_call GenerateNewKey( GUID * pKey );
	END_INTERFACE_PART(Num)
};

class std_dll CRootedObjectImpl : public CImplBase
{
public:
	CRootedObjectImpl();
	virtual ~CRootedObjectImpl();
public:	//operators
	IUnknown *GetParent();
	void SetParent( IUnknown *punk );
public:	//overrides
	virtual void OnChangeParent(){};
protected:
	IUnknown	*m_punkParent;

	BEGIN_INTERFACE_PART_EXPORT(Rooted, IRootedObject)
		com_call AttachParent( IUnknown *punkParent );
		com_call GetParent( IUnknown **ppunkParent );
	END_INTERFACE_PART(Rooted)
};


//component manager base implementation
class std_dll CCompManagerImpl : public CImplBase, 
		public CCompManager,
		public CCompRegistrator
{
protected:
	virtual void DeInit()
	{
		CCompManager::DeInit();
		CCompRegistrator::DeInit();
	}

	BEGIN_INTERFACE_PART_EXPORT(CompMan, ICompManager)
		com_call GetCount( int *piCount );
		com_call LoadComponents();
		com_call FreeComponents();
		com_call GetComponentUnknownByIdx( int idx, IUnknown **ppunk );
		com_call GetComponentUnknownByIID( IID iid, IUnknown **ppunk );
		com_call GetComponentUnknownByCLSID( CLSID clsid, IUnknown **ppunk );
		com_call AddComponent( IUnknown *punk, int *piIdx );
		com_call RemoveComponent( int idx );
	END_INTERFACE_PART(CompMan)
	BEGIN_INTERFACE_PART_EXPORT(CompReg, ICompRegistrator)
		com_call GetCount( int *piCount );
		com_call GetCompInfo( int idx, BSTR *pbstr );
	END_INTERFACE_PART(CompReg)
};


//implements IEventListener class
class std_dll CEventListenerImpl : public CImplBase
{
public:
	CEventListenerImpl();
	virtual ~CEventListenerImpl();
public:
	void Register( IUnknown *punkCtrl = 0, const char *pszEvString = 0 );
	void UnRegister( IUnknown *punkCtrl = 0, const char *pszEvString = 0 );
	bool IsInNotify();
	bool IsRegistered(IUnknown * punkCtrl = 0, const char * pszEvString = 0);
	bool	LockNotification( bool bLock );


protected:
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize ) = 0;

	BEGIN_INTERFACE_PART_EXPORT(EvList, IEventListener)
		com_call Notify( BSTR szEventDesc, IUnknown *pHint, IUnknown *pFrom, void *pdata, long cbSize );
		com_call IsInNotify(long *pbFlag);
	END_INTERFACE_PART(EvList)
protected:
	CPtrArray	m_arrRegistredCtrls;
	int			m_nRegCounter;
	bool		m_bInNotify;
	bool		m_bLockNotification;
};


//purpose of this class is implementation IDispatch interface using object with dual interface
//dual interface  is got from IRootedObject interface
class std_dll CDispatchWrapperBase : public CCmdTargetEx, 
							public CNamedObjectImpl,
							public CRootedObjectImpl
{
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();
protected:
	virtual const IID GetIID() = 0;						//this function should return IID for dual interface
	virtual IDispatch *GetDispatchInterface();			//returns dual interface

	BEGIN_INTERFACE_PART_EXPORT(Dispatch, IDispatch)
        com_call GetTypeInfoCount( 
             UINT __RPC_FAR *pctinfo);
        
        com_call GetTypeInfo( 
            UINT iTInfo,
            LCID lcid,
             ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        com_call GetIDsOfNames( 
            REFIID riid,
            LPOLESTR __RPC_FAR *rgszNames,
            UINT cNames,
            LCID lcid,
            DISPID __RPC_FAR *rgDispId);
        
        com_call Invoke( 
            DISPID dispIdMember,
            REFIID riid,
            LCID lcid,
            WORD wFlags,
            DISPPARAMS __RPC_FAR *pDispParams,
             VARIANT __RPC_FAR *pVarResult,
            EXCEPINFO __RPC_FAR *pExcepInfo,
            UINT __RPC_FAR *puArgErr);
	END_INTERFACE_PART(Dispatch);
};

class std_dll CLongOperationImpl : public CImplBase
{
//notification functions
public:
	CLongOperationImpl();
	virtual ~CLongOperationImpl();
public:
	virtual bool StartNotification( int nCYCount, int nStageCount = 1, int nNotifyFreq = 10 );
	virtual bool Notify( int nPos );
	virtual bool NextNotificationStage();
	virtual bool FinishNotification();
	virtual bool TerminateNotification();

	void	_Abort();
protected:
	BEGIN_INTERFACE_PART_EXPORT(Long, ILongOperation)
		com_call AttachNotifyWindow( HWND hWndNotifyWindow );
		com_call GetCurrentPosition( int *pnStage, int *pnPosition, int *pnPercent );
		com_call GetNotifyRanges( int *pnStageCount, int *pnNotifyCount );
		com_call Abort();
	END_INTERFACE_PART(Long)
protected:
	int		m_nStagesCount;
	int		m_nCurrentStage;
	int		m_nNotifyCount;
	int		m_nCurrentNotifyPos;
	int		m_nPercrent;
	HWND	m_hwndNotify;
	long	m_nNotifyFreq;

	bool	m_bNeedToTerminate;
};

class std_dll CTerminateException : public CException
{
	DECLARE_DYNAMIC(CTerminateException);
};

class std_dll COpenFileDialogImpl : public CImplBase
{
public:
	COpenFileDialogImpl();
	virtual ~COpenFileDialogImpl();

	CString m_strExtension;

	virtual int Execute(DWORD dwFlags){return 0;};
	virtual void GetOFN(LPOPENFILENAME& lpofn){};
	virtual bool SaveState(IUnknown *punkDD){return false;};
	virtual bool LoadState(IUnknown *punkDD){return false;};
	virtual void GetDlgWindowHandle( HWND *phWnd );

protected:
	BEGIN_INTERFACE_PART_EXPORT(OpenFile, IOpenFileDialog)
		com_call Execute( DWORD dwFlags );
		com_call SetOFNStruct( OPENFILENAME *pofn );
		com_call GetPathName( BSTR * pbstrFileName );
		com_call GetFilesCount( int *pnCount );
		com_call GetFileName( int nPos, BSTR *pbstrFileName );
		com_call SaveState( IUnknown *punkDD );
		com_call RestoreState( IUnknown *punkDD );
		com_call GetDlgWindowHandle( HWND *phWnd );
		com_call SetDefaultSection( BSTR bstr );
	END_INTERFACE_PART(OpenFile)

protected:
	CString	m_strSection;
	int m_nFileCount;
};


class std_dll CPersistImpl : public CImplBase
{
protected:
	BEGIN_INTERFACE_PART_EXPORT(Persist, IPersist)
		com_call GetClassID(CLSID *pClassID ); 
	END_INTERFACE_PART(Persist)
public:
	virtual void GetClassID( CLSID* pClassID ) = 0;	
};

class std_dll CInitializeObjectImpl : public CImplBase
{
public:
	CInitializeObjectImpl()
	{m_bInitialized = false;}
	BEGIN_INTERFACE_PART_EXPORT(Init, IInitializeObjectVT)
		com_call Initialize();
	END_INTERFACE_PART(Init);

	virtual bool Init()	{m_bInitialized = true;return true;};
protected:
	bool	m_bInitialized;
};





#endif //__cmnbase_h__