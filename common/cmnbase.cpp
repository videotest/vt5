#include "stdafx.h"
#include "cmnbase.h"
#include "core5.h"
#include "nameconsts.h"

#include "guardint.h"

//extern IUnknown *g_punkApp;	//application Unknown (thin reference)

IMPLEMENT_UNKNOWN_BASE(CNamedObjectImpl, Name);
IMPLEMENT_UNKNOWN_BASE(CNumeredObjectImpl, Num);
IMPLEMENT_UNKNOWN_BASE(CCompManagerImpl, CompMan);
IMPLEMENT_UNKNOWN_BASE(CCompManagerImpl, CompReg);
IMPLEMENT_UNKNOWN_BASE(CRootedObjectImpl, Rooted);
IMPLEMENT_UNKNOWN_BASE(CEventListenerImpl, EvList);
IMPLEMENT_UNKNOWN_BASE(CPersistImpl, Persist);

HRESULT CNamedObjectImpl::XName::GetName( BSTR *pbstr )
{
	METHOD_PROLOGUE_BASE(CNamedObjectImpl, Name);
	{
		CString	s = pThis->GetName();
		*pbstr = s.AllocSysString();
		return S_OK;
	}
}

HRESULT CNamedObjectImpl::XName::GetUserName( BSTR *pbstr )
{
	METHOD_PROLOGUE_BASE(CNamedObjectImpl, Name);
	{
		CString	s = pThis->GetUserName();
		*pbstr = s.AllocSysString();
		return S_OK;
	}
}

HRESULT CNamedObjectImpl::XName::SetName( BSTR bstr )
{
	METHOD_PROLOGUE_BASE(CNamedObjectImpl, Name);
	{
		CString	s;
		
		if( bstr ) s = bstr;
		pThis->SetName( s );

		pThis->OnChangeName();

		return S_OK;
	}
}

HRESULT CNamedObjectImpl::XName::SetUserName( BSTR bstr )
{
	METHOD_PROLOGUE_BASE(CNamedObjectImpl, Name);
	{
		CString	s;
		
		if( bstr ) s = bstr;
		pThis->SetUserName( s );

		pThis->OnChangeName();

		return S_OK;
	}
}

HRESULT CNamedObjectImpl::XName::GenerateUniqueName( BSTR bstrBase )
{
	METHOD_PROLOGUE_BASE(CNamedObjectImpl, Name);
	{
		static long	lCount = 1;

		CString	s;

		if( bstrBase )
			s = bstrBase;

		if( s.IsEmpty() )
			s = "Name";


		CString	str;
		str.Format( "%s%d", (LPCSTR)s, ++lCount );
		pThis->SetName( str );
		pThis->SetUserName( str );

		return S_OK;
	}
}

///////////////////////////////CNumeredObjectImpl 
long CNumeredObjectImpl::s_nCounter = 1;
CNumeredObjectImpl::CNumeredObjectImpl()
{
	GenerateKey();
	m_nCreatedNum = s_nCounter++;
}

CNumeredObjectImpl::~CNumeredObjectImpl()
{
}

void CNumeredObjectImpl::GenerateKey()
{
	::CoCreateGuid( (GUID*)&m_key );
}

GuidKey CNumeredObjectImpl::GenerateNewKey()
{
	GuidKey key;
	::CoCreateGuid((GUID*)&key);
	return key;
}

void CNumeredObjectImpl::ReplaceObjectKey(GuidKey & rKey)
{
	if (m_key != rKey)
		m_key = rKey;
}

HRESULT CNumeredObjectImpl::XNum::GetCreateNum( long *plNum )
{
	METHOD_PROLOGUE_BASE(CNumeredObjectImpl, Num)
	*plNum = pThis->m_nCreatedNum;
	return S_OK;
}

HRESULT CNumeredObjectImpl::XNum::AssignKey( GUID key )
{
	METHOD_PROLOGUE_BASE(CNumeredObjectImpl, Num)
	{
		GuidKey NewKey(key);
		pThis->ReplaceObjectKey(NewKey);
	}
	return S_OK;
}

HRESULT CNumeredObjectImpl::XNum::GetKey( GUID *plKey )
{
	METHOD_PROLOGUE_BASE(CNumeredObjectImpl, Num)
	if (!plKey)
		return E_INVALIDARG;
	*plKey = pThis->m_key;
	return S_OK;
}

HRESULT CNumeredObjectImpl::XNum::GenerateNewKey()
{
	METHOD_PROLOGUE_BASE(CNumeredObjectImpl, Num)
	pThis->GenerateKey();
	return S_OK;
}

HRESULT CNumeredObjectImpl::XNum::GenerateNewKey( GUID * pKey )
{
	METHOD_PROLOGUE_BASE(CNumeredObjectImpl, Num)
	{
		pThis->GenerateKey();		
		if( pKey )
			*pKey = pThis->m_key;

		return S_OK;
	}
}

///////////////////////////////CCompManagerImpl
HRESULT CCompManagerImpl::XCompMan::GetCount( int *piCount )
{
	METHOD_PROLOGUE_BASE(CCompManagerImpl, CompMan);
	{
		*piCount = pThis->GetComponentCount();
		return S_OK;
	}
}

HRESULT CCompManagerImpl::XCompMan::LoadComponents()
{
	METHOD_PROLOGUE_BASE(CCompManagerImpl, CompMan);
	{
		pThis->CCompManager::Init();
		return S_OK;
	}
}

HRESULT CCompManagerImpl::XCompMan::FreeComponents()
{
	METHOD_PROLOGUE_BASE(CCompManagerImpl, CompMan);
	{
		pThis->CCompManager::FreeComponents();
		return S_OK;
	}
}

HRESULT CCompManagerImpl::XCompMan::GetComponentUnknownByIdx( int idx, IUnknown **ppunk )
{
	METHOD_PROLOGUE_BASE(CCompManagerImpl, CompMan);
	{
		*ppunk = pThis->GetComponent( idx );
		return S_OK;
	}
}

HRESULT CCompManagerImpl::XCompMan::GetComponentUnknownByIID( IID iid, IUnknown **ppunk )
{
	METHOD_PROLOGUE_BASE(CCompManagerImpl, CompMan);
	{
		*ppunk = pThis->GetComponent( iid );
		return S_OK;
	}
}

HRESULT CCompManagerImpl::XCompMan::GetComponentUnknownByCLSID( CLSID clsid, IUnknown **ppunk )
{
	METHOD_PROLOGUE_BASE(CCompManagerImpl, CompMan);
	{
		*ppunk = pThis->GetComponent( clsid );
		return S_OK;
	}
}

HRESULT CCompManagerImpl::XCompMan::AddComponent( IUnknown *punk, int *piIdx )
{
	METHOD_PROLOGUE_BASE(CCompManagerImpl, CompMan);
	{
		int	idx = pThis->AddComponent( punk );
		if( piIdx )
			*piIdx = idx;

		return S_OK;
	}
}


HRESULT CCompManagerImpl::XCompMan::RemoveComponent( int idx )
{
	METHOD_PROLOGUE_BASE(CCompManagerImpl, CompMan);
	{
		pThis->RemoveComponent( idx );
		return S_OK;
	}
}


HRESULT CCompManagerImpl::XCompReg::GetCount( int *piCount )
{
	METHOD_PROLOGUE_BASE(CCompManagerImpl, CompReg);
	{
		*piCount = pThis->GetRegistredComponentCount();
		return S_OK;
	}
}

HRESULT CCompManagerImpl::XCompReg::GetCompInfo( int idx, BSTR *pbstr )
{
	METHOD_PROLOGUE_BASE(CCompManagerImpl, CompReg);
	{
		_bstr_t	bstr( pThis->CCompManager::GetComponentName( idx ) );
		*pbstr = bstr.copy();
		return S_OK;
	}
}

///////////////////////////////CRootedObjectImpl
CRootedObjectImpl::CRootedObjectImpl()
{
	m_punkParent = 0;
}

CRootedObjectImpl::~CRootedObjectImpl()
{
	if( m_punkParent ) 
		m_punkParent->Release();
}

IUnknown *CRootedObjectImpl::GetParent()
{
	if( m_punkParent )
		m_punkParent->AddRef();

	return m_punkParent;
}

void CRootedObjectImpl::SetParent( IUnknown *punkParent )
{
	if( punkParent )
		punkParent->AddRef();
	if( m_punkParent )
		m_punkParent->Release();

	m_punkParent = punkParent;

	OnChangeParent();
}

HRESULT CRootedObjectImpl::XRooted::AttachParent( IUnknown *punkParent )
{
	METHOD_PROLOGUE_BASE(CRootedObjectImpl, Rooted);
	{
		pThis->SetParent( punkParent );

		return S_OK;
	}
}

HRESULT CRootedObjectImpl::XRooted::GetParent( IUnknown **ppunkParent )
{
	METHOD_PROLOGUE_BASE(CRootedObjectImpl, Rooted);
	{
		*ppunkParent = pThis->m_punkParent;

		if( *ppunkParent )
			(*ppunkParent)->AddRef();

		return S_OK;
	}
}

//event listener

CEventListenerImpl::CEventListenerImpl()
{
	m_nRegCounter = 0;
	m_bInNotify = false;
	m_bLockNotification = false;
}
CEventListenerImpl::~CEventListenerImpl()
{
//	TRACE( "CEventListenerImpl::~CEventListenerImpl()\n" );
	if( m_nRegCounter != 0 )
	{
//		ASSERT( FALSE );
	}
}

bool CEventListenerImpl::LockNotification( bool bLock )
{
	bool	bOld = m_bLockNotification;
	m_bLockNotification = bLock;
	return bOld;
}


bool CEventListenerImpl::IsInNotify()
{
	return m_bInNotify;
}

void CEventListenerImpl::Register( IUnknown *punkCtrl, const char *pszEvString )
{
	pszEvString;	//not used

	if( !punkCtrl )
		punkCtrl = GetAppUnknown(false);

	INotifyControllerPtr	sp(punkCtrl);
	sp->RegisterEventListener( 0, Unknown() );

	//m_arrRegistredCtrls.AddTail( punkCtrl );
	m_nRegCounter++;
}

void CEventListenerImpl::UnRegister( IUnknown *punkCtrl, const char *pszEvString )
{
	//if( !m_arrRegistredCtrls.Find( punkCtrl ) )
	//	return;

	if( !m_nRegCounter )
		return;

	m_nRegCounter--;

	pszEvString;	//not used

	if( !punkCtrl )
		punkCtrl = GetAppUnknown(false);

	INotifyControllerPtr	sp(punkCtrl);
	if(sp != 0)
		sp->UnRegisterEventListener( 0, Unknown() );
}

bool CEventListenerImpl::IsRegistered(IUnknown * punkCtrl, const char * pszEvString)
{
	pszEvString;	//not used
	if (!m_nRegCounter)
		return false;

	if (!punkCtrl)
		punkCtrl = GetAppUnknown(false);

	INotifyControllerPtr	sp = punkCtrl;
	long bFlag = false;
	if (sp)
		sp->IsRegisteredEventListener(0, Unknown(), &bFlag);

	return bFlag == TRUE;
}

HRESULT CEventListenerImpl::XEvList::Notify( BSTR szEventDesc, IUnknown *pHint, IUnknown *pFrom, void *pdata, long cbSize )
{
	METHOD_PROLOGUE_BASE(CEventListenerImpl, EvList)
	{
		if( pThis->m_bLockNotification )
			return S_OK;
		pThis->m_bInNotify = true;

		CString	str( szEventDesc );

		pThis->OnNotify( str, pHint, pFrom, pdata, cbSize );

		pThis->m_bInNotify = false;

		return S_OK;
	}
}

HRESULT CEventListenerImpl::XEvList::IsInNotify(long  *pbFlag)
{
	METHOD_PROLOGUE_BASE(CEventListenerImpl, EvList)
	{
		if (!pbFlag)
			return E_INVALIDARG;
		*pbFlag = pThis->m_bInNotify ? TRUE : FALSE;
		return S_OK;
	}
}


BEGIN_INTERFACE_MAP(CDispatchWrapperBase, CCmdTargetEx)
	INTERFACE_PART(CDispatchWrapperBase, IID_IDispatch, Dispatch)
	INTERFACE_PART(CDispatchWrapperBase, IID_INamedObject2, Name)
	INTERFACE_PART(CDispatchWrapperBase, IID_IRootedObject, Rooted)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CDispatchWrapperBase, Dispatch);

//returns dual interface
IDispatch *CDispatchWrapperBase::GetDispatchInterface()
{
	if( !m_punkParent )
		return FALSE;

	if( !CheckInterface( m_punkParent, GetIID() ) )
		return FALSE;

	IDispatch *pdisp = 0;

	m_punkParent->QueryInterface( GetIID(), (void **)&pdisp );
											 
	return pdisp;
}

HRESULT CDispatchWrapperBase::XDispatch::GetTypeInfoCount( 
    UINT __RPC_FAR *pctinfo)
{
	_try_nested_base(CDispatchWrapperBase, Dispatch, GetTypeInfo)
	{
		IDispatch *pdisp = pThis->GetDispatchInterface();
		if( !pdisp )
			return E_NOTIMPL;
		HRESULT h = pdisp->GetTypeInfoCount( pctinfo );

		pdisp->Release();

		return h;
	}
	_catch_nested;
}

HRESULT CDispatchWrapperBase::XDispatch::GetTypeInfo( 
    UINT iTInfo,
    LCID lcid,
    ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo)
{
	_try_nested_base(CDispatchWrapperBase, Dispatch, GetTypeInfo)
	{
		IDispatch *pdisp = pThis->GetDispatchInterface();
		if( !pdisp )
			return E_NOTIMPL;
		HRESULT h = pdisp->GetTypeInfo( iTInfo, lcid, ppTInfo );
		
		pdisp->Release();

		return h;
	}
	_catch_nested;
}

HRESULT CDispatchWrapperBase::XDispatch::GetIDsOfNames( 
    REFIID riid,
    LPOLESTR __RPC_FAR *rgszNames,
    UINT cNames,
    LCID lcid,
    DISPID __RPC_FAR *rgDispId)
{
	_try_nested_base(CDispatchWrapperBase, Dispatch, GetIDsOfNames)
	{
		IDispatch *pdisp = pThis->GetDispatchInterface();
		if( !pdisp )
			return E_NOTIMPL;
		HRESULT h = pdisp->GetIDsOfNames( riid, rgszNames, cNames, lcid, rgDispId );
		
		pdisp->Release();

		return h;
	}
	_catch_nested;
}

HRESULT CDispatchWrapperBase::XDispatch::Invoke( 
    DISPID dispIdMember,
    REFIID riid,
    LCID lcid,
    WORD wFlags,
    DISPPARAMS __RPC_FAR *pDispParams,
    VARIANT __RPC_FAR *pVarResult,
    EXCEPINFO __RPC_FAR *pExcepInfo,
    UINT __RPC_FAR *puArgErr)
{
	_try_nested_base(CDispatchWrapperBase, Dispatch, Invoke)
	{
		IDispatch *pdisp = pThis->GetDispatchInterface();
		if( !pdisp )
			return E_NOTIMPL;
		HRESULT h = pdisp->Invoke( dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr );
		
		pdisp->Release();

		return h;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////////////////
//CLongOperationImpl
IMPLEMENT_DYNAMIC(CTerminateException, CException);
IMPLEMENT_UNKNOWN_BASE(CLongOperationImpl, Long)


int	g_nNotify = 1;

CLongOperationImpl::CLongOperationImpl()
{
	m_nStagesCount = 0;
	m_nCurrentStage = 0;
	m_nNotifyCount = 0;
	m_nCurrentNotifyPos = 0;
	m_nPercrent = 0;
	m_hwndNotify = 0;
	m_bNeedToTerminate  = false;

}

CLongOperationImpl::~CLongOperationImpl()
{
}

bool CLongOperationImpl::StartNotification( int nCYCount, int nStageCount, int nNotifyFreq )
{
	g_nNotify = ::GetValueInt(  ::GetAppUnknown(), "\\General", "EnableFilterNotify", g_nNotify );

	m_bNeedToTerminate  = false;
	if( !m_hwndNotify )
	{
		sptrIApplication	sptrA( GetAppUnknown() );
		sptrA->GetMainWindowHandle( &m_hwndNotify );
	}

	m_nNotifyCount = nCYCount;
	m_nStagesCount = nStageCount;
	m_nNotifyFreq = nNotifyFreq;

	ASSERT(m_nStagesCount>=1);
	ASSERT(m_nNotifyCount>=0);
	
	if( m_nNotifyFreq == -1 )
		m_nNotifyFreq = 30;//m_nNotifyCount/20;

	m_nCurrentStage = 0;
	m_nCurrentNotifyPos = 0;
	m_nPercrent = 0;

	::SendMessage( m_hwndNotify, WM_NOTIFYLONGOPERATION, loaStart, (LPARAM)Unknown()) ;

	return true;
}

bool CLongOperationImpl::Notify( int nPos )
{
	m_nCurrentNotifyPos = nPos;

	ASSERT( m_nCurrentNotifyPos < m_nNotifyCount );

	m_nPercrent  = 100*(m_nCurrentStage*m_nNotifyCount+m_nCurrentNotifyPos)/max( m_nStagesCount*m_nNotifyCount, 1);

	if( !g_nNotify )
		return true;

	if( (nPos % m_nNotifyFreq) == 0 )
		::SendMessage( m_hwndNotify, WM_NOTIFYLONGOPERATION, loaSetPos, (LPARAM)Unknown());

	if( m_bNeedToTerminate )
	{
		::SendMessage( m_hwndNotify, WM_NOTIFYLONGOPERATION, loaTerminate, (LPARAM)Unknown());
		//TRACE0("TERMINATE EXCEPTION GO!");
		throw new CTerminateException();
	}
	return !m_bNeedToTerminate;
}

bool CLongOperationImpl::NextNotificationStage()
{
	m_nCurrentNotifyPos = 0;
	m_nCurrentStage++;

	m_nCurrentStage = min( m_nCurrentStage, m_nStagesCount-1 );

	m_nPercrent = 100*(m_nCurrentStage*m_nNotifyCount+m_nCurrentNotifyPos)/max( m_nStagesCount*m_nNotifyCount, 1);

	::SendMessage( m_hwndNotify, WM_NOTIFYLONGOPERATION, loaSetPos, (LPARAM)Unknown());

	return true;
}

bool CLongOperationImpl::FinishNotification()
{
	m_nCurrentNotifyPos = m_nNotifyCount-1;
	m_nCurrentStage = m_nStagesCount-1;
	m_nPercrent = 100;

	::SendMessage( m_hwndNotify, WM_NOTIFYLONGOPERATION, loaComplete, (LPARAM)Unknown());

	return true;
}

bool CLongOperationImpl::TerminateNotification()
{
	::SendMessage( m_hwndNotify, WM_NOTIFYLONGOPERATION, loaTerminate, (LPARAM)(IUnknown*)this);
	return true;
}

void CLongOperationImpl::_Abort()
{
	::SendMessage( m_hwndNotify, WM_NOTIFYLONGOPERATION, loaTerminate, (LPARAM)Unknown());
	TRACE0("TERMINATE EXCEPTION GO!\n");
	throw new CTerminateException();
}

HRESULT CLongOperationImpl::XLong::AttachNotifyWindow( HWND hWndNotifyWindow )
{
	METHOD_PROLOGUE_BASE(CLongOperationImpl, Long);
	{
		pThis->m_hwndNotify = hWndNotifyWindow;
		return S_OK;
	}
}

HRESULT CLongOperationImpl::XLong::GetCurrentPosition( int *pnStage, int *pnPosition, int *pnPercent )
{
	METHOD_PROLOGUE_BASE(CLongOperationImpl, Long);
	{
		if( pnStage )
			*pnStage = pThis->m_nCurrentStage;
		if( pnPosition )
			*pnPosition = pThis->m_nCurrentNotifyPos;
		if( pnPercent )
			*pnPercent = pThis->m_nPercrent;

		return S_OK;
	}
}

HRESULT CLongOperationImpl::XLong::GetNotifyRanges( int *pnStageCount, int *pnNotifyCount )
{
	METHOD_PROLOGUE_BASE(CLongOperationImpl, Long);
	{
		if( pnStageCount )
			*pnStageCount = pThis->m_nStagesCount;
		if( pnNotifyCount )
			*pnNotifyCount = pThis->m_nNotifyCount;
		return S_OK;
	}
}

HRESULT CLongOperationImpl::XLong::Abort()
{
	METHOD_PROLOGUE_BASE(CLongOperationImpl, Long);
	{
		pThis->m_bNeedToTerminate = true;
		//pThis->_Abort();
		return S_OK;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//COpenFileDialogImpl
IMPLEMENT_UNKNOWN_BASE(COpenFileDialogImpl, OpenFile)

COpenFileDialogImpl::COpenFileDialogImpl()
{
	m_strSection  = "\\FileOpen";
}

COpenFileDialogImpl::~COpenFileDialogImpl()
{
}

void COpenFileDialogImpl::GetDlgWindowHandle( HWND *phWnd )
{
	phWnd = 0;
}

HRESULT COpenFileDialogImpl::XOpenFile::Execute( DWORD dwFlags )
{
	_try_nested_base(COpenFileDialogImpl, OpenFile, Execute)
	{
		if (pThis->Execute(dwFlags) == IDOK)
			return S_OK;
		return S_FALSE;
	}
	_catch_nested;
}

HRESULT COpenFileDialogImpl::XOpenFile::SetOFNStruct( OPENFILENAME *pofn )
{
	_try_nested_base(COpenFileDialogImpl, OpenFile, SetOFNStruct)
	{
		LPOPENFILENAME lpofn = 0;
		pThis->GetOFN(lpofn);
		if (lpofn && pofn)
			memcpy( lpofn, pofn, min(lpofn->lStructSize, pofn->lStructSize) );

		return S_OK;
	}
	_catch_nested;
}


HRESULT COpenFileDialogImpl::XOpenFile::GetPathName( BSTR * pbstrFileName )
{
	_try_nested_base(COpenFileDialogImpl, OpenFile, GetPathName)
	{
		LPOPENFILENAME lpofn = 0;
		pThis->GetOFN(lpofn);
		CString strFileName;
		if (lpofn)
			strFileName = lpofn->lpstrFile;
		*pbstrFileName = strFileName.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT COpenFileDialogImpl::XOpenFile::GetFilesCount( int *pnCount )
{
	_try_nested_base(COpenFileDialogImpl, OpenFile, GetFilesCount)
	{
		*pnCount = pThis->m_nFileCount;
		return S_OK;
	}
	_catch_nested;
}

HRESULT COpenFileDialogImpl::XOpenFile::GetFileName( int nPos, BSTR *pbstrFileName )
{
	_try_nested_base(COpenFileDialogImpl, OpenFile, GetFileName)
	{
		LPOPENFILENAME lpofn = 0;
		pThis->GetOFN(lpofn);
		CString strFileName;
		if (lpofn)
		{
			if (nPos == 0)
				strFileName = lpofn->lpstrFileTitle;
		}
		*pbstrFileName = strFileName.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT COpenFileDialogImpl::XOpenFile::SaveState( IUnknown *punkDD )
{
	_try_nested_base(COpenFileDialogImpl, OpenFile, SaveState)
	{
		if (pThis->SaveState(punkDD))
			return S_OK;
		else
			return S_FALSE;
	}
	_catch_nested;
}

HRESULT COpenFileDialogImpl::XOpenFile::RestoreState( IUnknown *punkDD )
{
	_try_nested_base(COpenFileDialogImpl, OpenFile, RestoreState)
	{
		if (pThis->LoadState(punkDD))
			return S_OK;
		else
			return S_FALSE;
	}
	_catch_nested;
}


HRESULT COpenFileDialogImpl::XOpenFile::GetDlgWindowHandle( HWND *phWnd )
{
	_try_nested_base(COpenFileDialogImpl, OpenFile, GetDlgWindowHandle)
	{
		pThis->GetDlgWindowHandle( phWnd );
			return S_OK;
	}
	_catch_nested;
}

HRESULT COpenFileDialogImpl::XOpenFile::SetDefaultSection( BSTR bstr )
{
	_try_nested_base(COpenFileDialogImpl, OpenFile, GetDlgWindowHandle)
	{
		if( bstr )
			pThis->m_strSection = bstr;
		else
			pThis->m_strSection = "\\FileOpen";
		return S_OK;
	}
	_catch_nested;
}


//////////////////////////////////////////////////////////////////////////////////////
//IPersist implementation
HRESULT CPersistImpl::XPersist::GetClassID( CLSID *pClassID )
{
	METHOD_PROLOGUE_BASE(CPersistImpl, Persist)
	{
		if (!pClassID)
			return E_POINTER;

		GuidKey guidInner;
		pThis->GetClassID(&guidInner);

		IVTApplicationPtr sptrA = GetAppUnknown(false);
		sptrA->GetEntry(1, (DWORD*)&guidInner, (DWORD*)pClassID, 0);

		return S_OK;
	}
}


IMPLEMENT_UNKNOWN_BASE(CInitializeObjectImpl, Init)

HRESULT CInitializeObjectImpl::XInit::Initialize()
{
	METHOD_PROLOGUE_BASE(CInitializeObjectImpl, Init)
	return pThis->Init()?S_OK:S_FALSE;
}
