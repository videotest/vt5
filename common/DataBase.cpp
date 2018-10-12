// DataBase.cpp: implementation of the CDataRegistrator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "data5.h"
#include "core5.h"
#include "DataBase.h"
#include "filebase.h"
#include "cmnbase.h"
#include "nameconsts.h"
#include "timetest.h"
#include "aliaseint.h"
#include "Factory.h"
#include "StreamEx.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CDataObjectInfoBase
BEGIN_INTERFACE_MAP(CDataObjectInfoBase, CCmdTargetEx)
	INTERFACE_PART(CDataObjectInfoBase, IID_INamedDataInfo, Info)
	INTERFACE_PART(CDataObjectInfoBase, IID_INamedDataInfo2, Info)
	INTERFACE_PART(CDataObjectInfoBase, IID_INamedObject2, Name)
END_INTERFACE_MAP();

IMPLEMENT_DYNAMIC(CDataObjectInfoBase,CCmdTargetEx);
IMPLEMENT_UNKNOWN(CDataObjectInfoBase, Info);

CDataObjectInfoBase::CDataObjectInfoBase(LPCTSTR szType, LPCTSTR szContainer)
{
	_OleLockApp( this );
	m_sName = szType;
	__m_szContainerName = szContainer;
    __m_dwContainerState  = (DWORD)(-1); // [vanek] 24.12.2003
	__m_lCounter = 1;
}

CDataObjectInfoBase::~CDataObjectInfoBase()
{
	_OleUnlockApp( this );
}

HICON CDataObjectInfoBase::__GetObjectIcon( DWORD dwFlags )
{
	HICON	h = 0;
	if( dwFlags & oif_SmallIcon )
		h = (HICON)::LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(GetDefIconID()), IMAGE_ICON, 16, 16, 0 );
	if( !h )
		h = (HICON)::LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(GetDefIconID()), IMAGE_ICON, 32, 32, 0 );

	return h;
}

const char *CDataObjectInfoBase::GetUserName()
{ 
	if( m_sUserName.IsEmpty() )
	{
		m_sUserName.LoadString( __GetTypeStringID() );

		_bstr_t	bstrName = m_sName;
		BSTR	bstrUser;

		IAliaseManPtr	ptrAliase( GetAppUnknown() );
		if( ptrAliase != 0 )
			if( ptrAliase->GetObjectString( bstrName, &bstrUser ) == S_OK )
			{
				m_sUserName = bstrUser;
				::SysFreeString( bstrUser );
			}
		if( m_sUserName.IsEmpty() )
			m_sUserName = m_sName;
	}
	return m_sUserName; 
}

// implements of INamedDataInfo::GetName
/*HRESULT CDataObjectInfoBase::XInfo::GetName(BSTR *pbstrName)
{
	_try_nested(CDataObjectInfoBase, Info, GetName)
	{
		_bstr_t	bstrName =pThis->__m_szName;

		IAliaseManPtr	ptrAliase( GetAppUnknown() );
		if( ptrAliase != 0 )
			if( ptrAliase->GetObjectString( bstrName, pbstrName ) == S_OK )
				return S_OK;
		*pbstrName = bstrName.copy();

		return S_OK;
	}
	_catch_nested;
}*/

// implements of INamedDataInfo::CreateObject
HRESULT CDataObjectInfoBase::XInfo::CreateObject(IUnknown **ppunk)
{
	METHOD_PROLOGUE_EX(CDataObjectInfoBase, Info)
	*ppunk = NULL;

	CRuntimeClass *pclass = pThis->GetDataObjectRuntime();
	CDataObjectBase *pobj = (CDataObjectBase *)pclass->CreateObject();
	IUnknown *punk = pobj->GetControllingUnknown(); //?

	pobj->m_pNamedDataInfo = this;
	pobj->m_pNamedDataInfo->AddRef();
	
	*ppunk = punk;

	return S_OK;
}

HRESULT CDataObjectInfoBase::XInfo::GetContainerType( BSTR *pbstrTypeName )
{
	METHOD_PROLOGUE_EX(CDataObjectInfoBase, Info)
	*pbstrTypeName = pThis->__m_szContainerName.AllocSysString();
	return S_OK;
}

// implements of INamedDataInfo::GetProgID
HRESULT CDataObjectInfoBase::XInfo::GetProgID(BSTR *pbstrProgID)
{
	METHOD_PROLOGUE_EX(CDataObjectInfoBase, Info)
	CString str = pThis->__GetProgID();
	*pbstrProgID = str.AllocSysString();
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
HRESULT CDataObjectInfoBase::XInfo::GetObjectIcon(HICON* phIcon, DWORD dwFlags )
{
	METHOD_PROLOGUE_EX(CDataObjectInfoBase, Info)
	*phIcon = pThis->__GetObjectIcon( dwFlags );
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// [vanek] 24.12.2003
HRESULT CDataObjectInfoBase::XInfo::GetContainerState( DWORD *pdwState )	
{
	METHOD_PROLOGUE_EX(CDataObjectInfoBase, Info)
	if( !pdwState )
		return S_FALSE;

	if( pThis->__m_dwContainerState == (DWORD)(-1) )
	{
		CString strContainerKey( _T("") );
		if( !pThis->__m_szContainerName.IsEmpty( ) )
			strContainerKey = pThis->__m_szContainerName;
		else
			strContainerKey = pThis->GetUserName( );

		pThis->__m_dwContainerState = cntsEnabled;
		if( !strContainerKey.IsEmpty( ) )
			pThis->__m_dwContainerState = (DWORD)::GetValueInt( GetAppUnknown(), szContaintersStatesSection, strContainerKey, cntsEnabled );
	}

    *pdwState = pThis->__m_dwContainerState;
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////

//CDataObjectBase
IMPLEMENT_DYNAMIC(CDataObjectBase, CCmdTargetEx);
//IMPLEMENT_UNKNOWN(CDataObjectBase, DataObj);


STDMETHODIMP_(ULONG) CDataObjectBase::XDataObj::AddRef()
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)
	return (ULONG)pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CDataObjectBase::XDataObj::Release()
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)
	return (ULONG)pThis->ExternalRelease();	  
}
STDMETHODIMP CDataObjectBase::XDataObj::QueryInterface(
	REFIID iid, LPVOID* ppvObj)
{								
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}


CDataObjectBase::CDataObjectBase()
{
	EnableAggregation();
	EnableAutomation();

	m_punkAggrNamedData = 0;
	m_pNamedDataInfo = NULL;
	m_bModified = false;
	m_punkNamedData = 0;
	m_bNotifyNamedData = true;
	m_bUsedInOtherObject = false;
	m_bHostedByOtherObject = false;

	m_pParent = 0;
	m_PosInParent = 0;
	__m_lBaseObjKey = m_key;
}

CDataObjectBase::~CDataObjectBase()
{
	ASSERT( m_pParent == 0 );
	if (m_pNamedDataInfo)
		m_pNamedDataInfo->Release();
	m_pNamedDataInfo = 0;
}

void CDataObjectBase::InitAggrNamedData()
{
	if( m_punkAggrNamedData )
		return;

	_bstr_t	bstr( szNamedDataProgID );
	CLSID	clsidNamedData;

	if( ::CLSIDFromProgID( bstr, &clsidNamedData ) )
		return;

	::CoCreateInstance( clsidNamedData, GetControllingUnknown(), CLSCTX_INPROC, 
		IID_IUnknown, (void **)&m_punkAggrNamedData );

	//23/05/2002 change to binary cos default - text format
	INamedDataPtr ptrND( GetControllingUnknown() );
	if( ptrND )
		ptrND->EnableBinaryStore( TRUE );	
}

void CDataObjectBase::OnFinalRelease()
{
	//use for make possible ussage AddRef/release pairs in deinit and destructor
	AddRef();

	_DetachChildParent();

	if( m_punkAggrNamedData )
		m_punkAggrNamedData->Release();
	m_punkAggrNamedData = 0;

	CCmdTargetEx::OnFinalRelease();
}

void CDataObjectBase::_DetachChildParent()
{
	POSITION pos = GetFirstChildPosition();
	while( pos )
	{
		sptrINamedDataObject2	sptr( GetNextChild( pos ) );
		sptr->SetParent( 0, apfNotifyNamedData|sdfCopyParentData );
	}

	ASSERT(m_listChilds.GetCount() == 0 );

	if( m_pParent != 0 )
		m_pParent->RemoveChild( GetControllingUnknown() );
	m_pParent = 0;
	m_PosInParent = 0;

}

void CDataObjectBase::Detach()	//called when parent detached
{
}
void CDataObjectBase::Attach()	//called when parent attached
{
}


// {6FB474BF-8C48-47f1-B4ED-298FBB8CDBC4}
static const IID IID_IDataObjectBaseDisp =
{ 0x6fb474bf, 0x8c48, 0x47f1, { 0xb4, 0xed, 0x29, 0x8f, 0xbb, 0x8c, 0xdb, 0xc4 } };



BEGIN_INTERFACE_MAP(CDataObjectBase, CCmdTargetEx)
	INTERFACE_PART(CDataObjectListBase, IID_IDataObjectBaseDisp, Dispatch)
	INTERFACE_PART(CDataObjectBase, IID_INamedDataObject, DataObj)
	INTERFACE_PART(CDataObjectBase, IID_INamedDataObject2, DataObj)
	INTERFACE_PART(CDataObjectBase, IID_ISerializableObject, Serialize)
	INTERFACE_PART(CDataObjectBase, IID_ITextObject, Text)
	INTERFACE_PART(CDataObjectBase, IID_INamedObject2, Name)
	INTERFACE_PART(CDataObjectBase, IID_INumeredObject, Num)
	INTERFACE_PART(CDataObjectBase, IID_INamedDataObjectProps, Props)
	INTERFACE_PART(CDataObjectBase, IID_ICompatibleObject, CompatibleObject)
	INTERFACE_PART(CDataObjectBase, IID_IPersist, Persist)
	INTERFACE_AGGREGATE(CDataObjectBase, m_punkAggrNamedData)
END_INTERFACE_MAP()



IMPLEMENT_UNKNOWN_BASE(CDataObjectBase, CompatibleObject);


BEGIN_DISPATCH_MAP(CDataObjectBase, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CDataObjectBase)
		DISP_PROPERTY_EX(CDataObjectBase, "Name", disp_GetName, disp_SetName, VT_BSTR)
		DISP_FUNCTION(CDataObjectBase, "GetType", GetType, VT_BSTR, VTS_NONE)
		DISP_FUNCTION(CDataObjectBase, "GetPrivateNamedData", disp_GetPrivateNamedData, VT_DISPATCH, VTS_NONE)
		DISP_FUNCTION(CDataObjectBase, "IsVirtual", disp_IsVirtual, VT_BOOL, VTS_NONE)
		DISP_FUNCTION(CDataObjectBase, "StoreData", disp_StoreData, VT_EMPTY, VTS_I4)
		DISP_FUNCTION(CDataObjectBase, "GetParent", disp_GetParent, VT_DISPATCH, VTS_NONE)
		DISP_FUNCTION(CDataObjectBase, "GetChildsCount", disp_GetChildsCount, VT_I4, VTS_NONE)	
		DISP_FUNCTION(CDataObjectBase, "GetFirstChildPos", disp_GetFirstChildPos, VT_EMPTY, VTS_PVARIANT)	
		DISP_FUNCTION(CDataObjectBase, "GetLastChildPos", disp_GetLastChildPos, VT_EMPTY, VTS_PVARIANT)		
		DISP_FUNCTION(CDataObjectBase, "GetNextChild", disp_GetNextChild, VT_DISPATCH, VTS_PVARIANT)
		DISP_FUNCTION(CDataObjectBase, "GetPrevChild", disp_GetPrevChild, VT_DISPATCH, VTS_PVARIANT)
		DISP_FUNCTION(CDataObjectBase, "IsEmpty", disp_IsEmpty, VT_BOOL, VTS_NONE)
		DISP_FUNCTION(CDataObjectBase, "IsPrivateNamedDataExist", IsPrivateNamedDataExist, VT_BOOL, VTS_NONE)
		DISP_FUNCTION(CDataObjectBase, "GetModifiedFlag", disp_GetModifiedFlag, VT_BOOL, VTS_NONE)
		DISP_FUNCTION(CDataObjectBase, "SetModifiedFlag", disp_SetModifiedFlag, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()
	//{{AFX_MSG_MAP(CDataObjectBase)
	//}}AFX_MSG_MAP

BSTR CDataObjectBase::disp_GetName()
{
	CString strResult(CNamedObjectImpl::GetName());
	return strResult.AllocSysString();
}

void CDataObjectBase::disp_SetName(LPCTSTR szNewName)
{
	CNamedObjectImpl::SetName(szNewName);
}

BOOL CDataObjectBase::disp_GetModifiedFlag() 
{
	BOOL bModified = FALSE;
	INamedDataObjectPtr ptrNDO( GetControllingUnknown() );
	if( ptrNDO ) ptrNDO->IsModified( &bModified );
	return bModified;
}

void CDataObjectBase::disp_SetModifiedFlag(BOOL bModified) 
{
	INamedDataObjectPtr ptrNDO( GetControllingUnknown() );
	if( ptrNDO ) ptrNDO->SetModifiedFlag( bModified );
	UpdateDataObject( m_punkNamedData, GetControllingUnknown() );
}

BSTR CDataObjectBase::GetType()
{
	BSTR	bstrType = 0;
	INamedObject2Ptr	ptrNamed( m_pNamedDataInfo );
	ptrNamed->GetName(&bstrType);
	return bstrType;
}

void CDataObjectBase::ErrorMsg(UINT nErrorID)
{
	CString strCaption;
	strCaption.LoadString(IDS_ERROR_CAPTION);

	CString strErr;
	strErr.LoadString(nErrorID);

	MessageBox(NULL, strErr, strCaption, MB_OK|MB_ICONERROR|MB_APPLMODAL);
}



HRESULT CDataObjectBase::XDataObj::InitAttachedData()
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
	pThis->InitAggrNamedData();
	return S_OK;
}
HRESULT CDataObjectBase::XDataObj::GetType(BSTR *pbstrType)
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, GetType)
	{
		if (!pbstrType)
			return E_INVALIDARG;

		INamedObject2Ptr	ptrNamed( pThis->m_pNamedDataInfo );
		return ptrNamed->GetName( pbstrType );
	}
//	_catch_nested;
}
// return interface INamedDataInfo
HRESULT CDataObjectBase::XDataObj::GetDataInfo(IUnknown **ppunk)
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, GetType)
	{
		if (!ppunk)
			return E_INVALIDARG;

		if (pThis->m_pNamedDataInfo == 0)
			return E_NOTIMPL;

		*ppunk = pThis->m_pNamedDataInfo;
		pThis->m_pNamedDataInfo->AddRef();
		
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::IsModified( BOOL *pbModified )
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)
//	_try_nested(CDataObjectBase, DataObj, IsModified)
	{
		if (!pbModified)
			return E_INVALIDARG;

		*pbModified = pThis->IsModified();
		
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::SetTypeInfo( INamedDataInfo *pinfo )
{
	//paul 12.04.2000. Reason - create object by CLSID, so object need type info, if havn't -> crash...
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)

	if( CheckInterface( pinfo, IID_INamedDataInfo) )
	{
		if( pThis->m_pNamedDataInfo )
			pThis->m_pNamedDataInfo->Release();

		pThis->m_pNamedDataInfo = pinfo;
			pThis->m_pNamedDataInfo->AddRef();
	}
		
	return S_OK;
}
HRESULT CDataObjectBase::XDataObj::SetModifiedFlag( BOOL bSet )
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, IsModified)
	{
		pThis->SetModifiedFlag( bSet == TRUE );
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::CreateChild( IUnknown **ppunk )
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, IsModified)
	{
		if (!ppunk)
			return E_INVALIDARG;

		_bstr_t	bstrObjectKind;

		bstrObjectKind = ::GetObjectKind( this );
		IUnknown *punk = ::CreateTypedObject( bstrObjectKind );
		if (!punk)
			return E_INVALIDARG;

		sptrINamedDataObject2	sptrN(punk);
		if (sptrN == 0)
		{
			punk->Release();
			return E_INVALIDARG;
		}
		sptrN->SetParent( this, apfNotifyNamedData|apfAttachParentData );
					 
		*ppunk = punk;

		return S_OK;
	}
//	_catch_nested;
}

bool CDataObjectBase::_RemoveChild(IUnknown* punkChild, POSITION pos)
{
	if(pos == 0)
		pos = _FindChild( punkChild );

	ASSERT( pos );

	if (!pos)
		return false;

	OnDeleteChild( punkChild, pos );

	m_listChilds.RemoveAt( pos );

	INamedDataObject2Ptr sptr(punkChild);
	
	if (sptr != 0)
		sptr->SetObjectPosInParent((TPOS)0);
	//punkChild->Release();

	return true;
}

void CDataObjectBase::_AddChild(IUnknown *punkChild)
{
	ASSERT( !_FindChild( punkChild ) );

	POSITION pos = m_listChilds.AddTail(punkChild);
	OnAddChild( punkChild, pos );

	INamedDataObject2Ptr sptr(punkChild);
	
	if (sptr != 0)
		sptr->SetObjectPosInParent(pos);

//	DoSetActiveChild( pos );
}


HRESULT CDataObjectBase::XDataObj::RemoveChild( IUnknown *punkChild )
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, RemoveChild)
	{
		if(!pThis->_RemoveChild(punkChild))
			return E_INVALIDARG;
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::AddChild( IUnknown *punkChild )
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, AddChild)
	{
		pThis->_AddChild(punkChild);
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::GetParent( IUnknown **ppunkParent )
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, GetParent)
	{
		if (!ppunkParent)
			return E_INVALIDARG;

		*ppunkParent = pThis->m_pParent;

		if (*ppunkParent)
			(*ppunkParent)->AddRef();

		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::SetParent( IUnknown *punkParent, DWORD dwFlags )
{
	/*bool bMoveDataToParent = (dwFlags & apfCopyToParent) != 0;
	bool bAttachParentData = (dwFlags & apfAttachParentData) != 0;*/
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, SetParent)
	{
		pThis->OnSetParent( punkParent );
		DWORD	dwThisObjectFlags = pThis->GetNamedObjectFlags();

		bool bNotifyNamedData = (dwFlags & apfNotifyNamedData) != 0 &&
								(dwThisObjectFlags & nofStoreByParent) == 0;

		if( punkParent != 0 && ( ::GetObjectKey( pThis->m_pParent ) == ::GetObjectKey( punkParent ) ) )
			return FALSE;

		if( pThis->m_pParent != 0)
		{
			pThis->ExchangeDataWithParent( dwFlags );
			pThis->m_pParent->RemoveChild( pThis->GetControllingUnknown() );
		}

		if( pThis->m_punkNamedData && bNotifyNamedData )
		{
			sptrINamedData	sptrN( pThis->m_punkNamedData );
			if (sptrN != 0)
				sptrN->NotifyContexts( ncRemoveObject, pThis->GetControllingUnknown(), 0, 0);
		}

		pThis->m_punkNamedData = 0;
		if( punkParent )
		{
			punkParent->QueryInterface( IID_INamedDataObject2, (void**)&pThis->m_pParent );

			if( pThis->m_pParent )
				pThis->m_pParent->Release();

			//AAM if( pThis->m_pParent )
			//AAM 	pThis->m_pParent->GetData( &pThis->m_punkNamedData );
		}
		else
			pThis->m_pParent = 0;

		if( pThis->m_punkNamedData && bNotifyNamedData )
		{
			sptrINamedData	sptrN( pThis->m_punkNamedData );
			if (sptrN != 0)
				sptrN->NotifyContexts( ncAddObject, pThis->GetControllingUnknown(), 0, 0);
		}

		if( pThis->m_pParent )
		{
			pThis->m_pParent->AddChild( pThis->GetControllingUnknown() );
			
			if( !pThis->ExchangeDataWithParent( dwFlags ) )
				return E_INVALIDARG;
		}

		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::StoreData( DWORD dwDirection/*StoreDataFlags*/ )
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, StoreDataToParent)
	{
		if( pThis->m_pParent == 0 )
			return S_OK;

		if( !pThis->ExchangeDataWithParent( dwDirection ) )
			return E_INVALIDARG;
		
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::GetChildsCount( long *plCount )
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, GetChildsCount)
	{
		if (!plCount)
			return E_INVALIDARG;
		*plCount = pThis->m_listChilds.GetCount();
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::GetFirstChildPosition(POSITION *plPos)
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, GetChild)
	{
		if (!plPos)
			return E_INVALIDARG;

		*plPos = pThis->GetFirstChildPosition();
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::GetNextChild(POSITION *plPos, IUnknown **ppunkChild)
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, GetNextChild)
	{
		if (!ppunkChild || !plPos)
			return E_INVALIDARG;

		*ppunkChild = pThis->GetNextChild(*plPos);

		if( *ppunkChild )
			(*ppunkChild)->AddRef();
		
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::GetLastChildPosition(POSITION *plPos)
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, GetNextChild)
	{
		if (!plPos)
			return E_INVALIDARG;

		*plPos = pThis->GetLastChildPosition();
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::GetPrevChild(POSITION *plPos, IUnknown **ppunkChild)
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, GetNextChild)
	{
		if (!ppunkChild || !plPos)
			return E_INVALIDARG;

		*ppunkChild = pThis->GetPrevChild(*plPos);

		if (*ppunkChild)
			(*ppunkChild)->AddRef();
		
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::AttachData( IUnknown *punkNamedData )
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, AttachData)
	{
		pThis->m_punkNamedData = punkNamedData;
		
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::GetObjectFlags( DWORD *pdwObjectFlags )
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, GetObjectFlags)
	{
		if (!pdwObjectFlags)
			return E_INVALIDARG;

		*pdwObjectFlags = pThis->GetNamedObjectFlags();
		
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::SetActiveChild(TPOS lPos)
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, SetActiveChild)
	{
		pThis->DoSetActiveChild(lPos);
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::GetActiveChild(TPOS *plPos)
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, GetActiveChild)
	{
		if (!plPos)
			return E_INVALIDARG;

		(*plPos) = pThis->DoGetActiveChild();
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::SetHostedFlag( BOOL bSet )
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, SetHostedFlag)
	{
		if( bSet )
			pThis->m_bHostedByOtherObject = true;
		else
			pThis->m_bHostedByOtherObject = false;

		return S_OK;
	}
//	_catch_nested;
}


HRESULT CDataObjectBase::XDataObj::SetUsedFlag( BOOL bSet )
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, SetUsedFlag)
	{
		if( bSet )
			pThis->m_bUsedInOtherObject = true;
		else
			pThis->m_bUsedInOtherObject = false;

		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::GetData( IUnknown **ppunkNamedData )
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, AttachData)
	{
		if (!ppunkNamedData)
			return E_INVALIDARG;
		
		*ppunkNamedData = pThis->m_punkNamedData;
		if(*ppunkNamedData)
			(*ppunkNamedData)->AddRef();
		
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::GetObjectPosInParent(TPOS *plPos)
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, GetObjectPosInParent)
	{
		if (!plPos)
			return E_INVALIDARG;

		*plPos = (TPOS)pThis->m_PosInParent;
		return S_OK;
	}
//	_catch_nested;
}


HRESULT CDataObjectBase::XDataObj::SetObjectPosInParent(TPOS lPos)
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, SetObjectPosInParent)
	{
		pThis->m_PosInParent = (POSITION)lPos;
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::GetChildPos(IUnknown *punkChild, TPOS *plPos)
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)									
//	_try_nested(CDataObjectBase, DataObj, SetObjectPosInParent)
	{
		if (!punkChild || !plPos)
			return E_INVALIDARG;

		POSITION pos = pThis->_FindChild(punkChild);
		if (!pos)
			return E_INVALIDARG;
		
		*plPos = (TPOS)pos;
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CDataObjectBase::XDataObj::GetBaseKey(GUID * pBaseKey)
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)
	{
		if (!pBaseKey)
			return E_INVALIDARG;

		*pBaseKey = pThis->GetBaseObjectKey();

		return S_OK;
	}
}

HRESULT CDataObjectBase::XDataObj::SetBaseKey(GUID * pBaseKey)
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)
	{
		GuidKey key(pBaseKey ? *pBaseKey : INVALID_KEY);
		return pThis->SetBaseObjectKey(key) ? S_OK : E_FAIL;
	}
}

HRESULT CDataObjectBase::XDataObj::IsBaseObject(BOOL * pbFlag)
{
	METHOD_PROLOGUE_EX(CDataObjectBase, DataObj)
	{
		if (!pbFlag)
			return E_INVALIDARG;

		*pbFlag = pThis->CanBeBaseObject();
		return  S_OK;
	}
}


HRESULT CDataObjectBase::XCompatibleObject::CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize )
{
	METHOD_PROLOGUE_EX(CDataObjectBase, CompatibleObject)
	{
		return pThis->CreateCompatibleObject( punkObjSource, pData, uDataSize );		
	}
}

HRESULT CDataObjectBase::CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize )
{
	ASSERT( FALSE );
	return E_NOTIMPL;
}




// inner functions
bool CDataObjectBase::SetBaseObjectKey(GuidKey & BaseKey)
{
	if( BaseKey == INVALID_KEY )
		::CoCreateGuid( &BaseKey );


	INamedDataPtr sptrData = m_punkNamedData;
	GuidKey PrevKey = __m_lBaseObjKey;

	
	OnSetBaseObject(BaseKey);
	__m_lBaseObjKey = BaseKey;

	if (sptrData != 0)
		sptrData->NotifyContexts(ncChangeBase, GetControllingUnknown(), 0, &PrevKey);
	
	return true;
}

void CDataObjectBase::ReplaceObjectKey(GuidKey & rKey)
{
	INamedDataPtr sptrData = m_punkNamedData;
	GuidKey PrevKey = m_key;
	
	m_key = rKey;

	if (sptrData != 0)
		sptrData->NotifyContexts(ncChangeKey, GetControllingUnknown(), 0, &PrevKey);
}


GuidKey CDataObjectBase::GetBaseObjectKey()
{
	return __m_lBaseObjKey;
}

bool CDataObjectBase::CanBeBaseObject()
{
	return false;
}

void CDataObjectBase::OnSetBaseObject(GuidKey & NewKey)
{
}

bool CDataObjectBase::IsObjectEmpty()
{
	return false;
}

POSITION CDataObjectBase::GetFirstChildPosition()
{
	return m_listChilds.GetHeadPosition();
}

POSITION CDataObjectBase::GetLastChildPosition()
{
	return m_listChilds.GetTailPosition();
}

IUnknown *CDataObjectBase::GetNextChild(POSITION& pos)
{
	return (IUnknown*)m_listChilds.GetNext( pos );
}

IUnknown *CDataObjectBase::GetPrevChild(POSITION& pos)
{
	return (IUnknown*)m_listChilds.GetPrev(pos);
}


/*/////helper - solve problem with changing object keys and setting up parents
class CLoadRuntimeHelper
{
public:
	struct XObjInfo
	{
		IUnknown	*punk;
		long		lKeyNew;
		long		lKeyOld;
	};
public:
	CLoadRuntimeHelper()
	{
		Init();
	}
	~CLoadRuntimeHelper()
	{
		Init();
	}
	void Init()
	{
		POSITION	pos = m_list.GetHeadPosition();
		while( pos )delete m_list.GetNext( pos );
		m_list.RemoveAll();
	}
	void AddAssoc( long lOldKey, long lNewKey, IUnknown *punk )
	{
		XObjInfo	*p = new XObjInfo;
		p->lKeyNew = lNewKey;
		p->lKeyOld = lOldKey;
		p->punk = punk;

		m_list.AddTail( p );
	}

	XObjInfo	*Find( long lOldKey )
	{
		POSITION	pos = m_list.GetHeadPosition();
		while( pos )
		{
			XObjInfo	*p = m_list.GetNext( pos );
			if( p->lKeyOld == lOldKey )
				return p;
		}
		return 0;
	}
protected:
	CTypedPtrList<CPtrList, XObjInfo*>	m_list;
};

CLoadRuntimeHelper	g_LoadHelper;*/

/*void InitKeyCache()
{ 
	g_LoadHelper.Init(); 
}
void ReleaseKeyCache()
{ 
	g_LoadHelper.Init(); 
}
long GetKeyFromCache( long lOldKey )
{
	if( lOldKey == -1 )return -1;
	CLoadRuntimeHelper::XObjInfo	*p = g_LoadHelper.Find( lOldKey );
	if( p )return p->lKeyNew;
	return -1;
}
IUnknown *GetObjectUsingOldKey( long lOldKey )
{
	if( lOldKey == -1 )return 0;
	CLoadRuntimeHelper::XObjInfo	*p = g_LoadHelper.Find( lOldKey );
	if( p )return p->punk;
	return 0;
}*/


bool CDataObjectBase::SerializeObject( CStreamEx& ar, SerializeParams *pparams )
{
	
	//BOOL bAggrNamedDataNeedCreate = FALSE;
	
	
	OnBeforeSerialize( ar );

	DWORD	dwMyFlags = GetNamedObjectFlags();
	bool	bStoreByParent = (dwMyFlags & nofStoreByParent)==nofStoreByParent;
	bool	bStoreByHost = (dwMyFlags & nofHasHost)==nofHasHost;

	/*
	CFile	*pfile = ar.GetFile();
	ASSERT( pfile->IsKindOf( RUNTIME_CLASS( COleStreamFile ) ) );
	COleStreamFile	*poleFile = (COleStreamFile	*)pfile;
	*/

	if( ar.IsStoring() )
	{
		ar<<4;		//version		
		
		/*
		bAggrNamedDataNeedCreate = ( m_punkAggrNamedData != NULL );
		ar<<bAggrNamedDataNeedCreate;
		*/
		
		ar<<m_key;
		ar<<m_sName;

		if( bStoreByParent && !bStoreByHost )
		{
			ar<<INVALID_KEY;
		}
		else
		{
			GuidKey lKey = ::GetObjectKey( m_pParent );
			ar<<lKey;
		}

		//serialize childs( if required )
		POSITION pos = GetFirstChildPosition();

		while( pos )
		{
			sptrINamedDataObject2	sptrChild( GetNextChild( pos ) );

			DWORD	dwFlags = 0;
			sptrChild->GetObjectFlags( &dwFlags );

			if( (dwFlags & nofStoreByParent)==nofStoreByParent &&
				(dwFlags & nofHasHost) !=nofHasHost)
			{
				ar<<(long)1;
				::StoreDataObjectToArchive( ar, sptrChild, pparams );
			}
			else
			{
				ar<<(long)0;
			}
		}
		ar<<(long)2;
		
		if( m_punkAggrNamedData )
		{
			ar<<1;
			ar.Flush();

			{

				SerializeParams	params;
				params.flags = pparams->flags;
				params.punkNamedData = (INamedData*)m_punkAggrNamedData;
				params.punkCurrentParent = 0;
	
				ISerializableObjectPtr	ptrS( m_punkAggrNamedData );
				ptrS->Store( (IStream*)ar, &params );
			}
		}
		else 
			ar<<0;

		// version 3 - serialize base objects keys
		ar << __m_lBaseObjKey;
		ar << __m_lBaseObjKey;

	}
	else
	{
		long	lDummyKey;
		long	lVer;

		//paul 26.06.2002
		_DetachChildParent();

		ar>>lVer;
		
		/*
		if( lVer >= 5 )
			ar>>bAggrNamedDataNeedCreate;					
		
		if( bAggrNamedDataNeedCreate )
			InitAggrNamedData();			
		*/	
			

		if( lVer >= 0 )
		{
			if( lVer < 4 )
			{
				ar>>lDummyKey;
				ReplaceObjectKey(GenerateNewKey());
			}
			else
			{
				GuidKey key;
				ar>>key;
				ReplaceObjectKey(key);
			}
			
			ar>>m_sName;

			GuidKey	lParentKey;

			if( lVer < 4 )
				ar>>lDummyKey;
			else
				ar>>lParentKey;

			if( lParentKey != INVALID_KEY )
			{
				IUnknown	*punkParent = 0;
				if( GetObjectKey( pparams->punkCurrentParent ) == lParentKey )
					punkParent = pparams->punkCurrentParent;
				else
				{
					IUnknown	*punk = ::GetObjectByKeyEx( pparams->punkNamedData, lParentKey );
					punkParent = punk;
					if( punk )punk->Release();
				}

				if( punkParent )
				{
					punkParent->QueryInterface( IID_INamedDataObject2, (void**)&m_pParent );

					if( m_pParent )
					{
						m_pParent->AddChild( GetControllingUnknown() );
						m_pParent->Release();
					}
				}
			}
		}

		if( lVer >= 1 )
		{
			long	lFeature = 0;

			INamedDataObject2	*punkStoreParent = pparams->punkCurrentParent;
			pparams->punkCurrentParent = &m_xDataObj;


			while( lFeature != 2 )
			{
				ar>>lFeature;

				if( lFeature == 1 )
				{
					IUnknown	*punkChild = ::LoadDataObjectFromArchive( ar, pparams );
					if( punkChild )
					{
						sptrINamedDataObject2	sptrN( punkChild );
						punkChild->Release();

						if( sptrN )
							sptrN->SetParent( GetControllingUnknown(), 0 );
					}
				}
			}

			pparams->punkCurrentParent = punkStoreParent;
		}

		if( lVer >= 2 )
		{
			long	lFeature = 0;
			ar>>lFeature;

			if( lFeature )
			{
				ar.Flush();
				InitAttachedData( GetControllingUnknown());

				SerializeParams	params;
				params.flags = pparams->flags;
				params.punkNamedData = (INamedData*)m_punkAggrNamedData;
				params.punkCurrentParent = 0;

				INamedDataPtr ptrND( m_punkAggrNamedData );
				ptrND->DeleteEntry( 0 );

				ISerializableObjectPtr	ptrS( m_punkAggrNamedData );
				ptrS->Load( (IStream*)ar, &params );

			}
		}
		if (lVer == 3)
		{
			long	lDummy;
			ar >> lDummy;
			ar >> lDummy;
		}
		else if( lVer >= 4 )
		{
			GuidKey dummyKey, baseKey;
			ar>>baseKey;
			SetBaseObjectKey(baseKey);//__m_lBaseObjKey);
			ar>>dummyKey;
		}
	}

	OnAfterSerialize( ar );

	return true;
}

POSITION CDataObjectBase::_FindChild( IUnknown *punkChild )
{
	INamedDataObject2Ptr sptr = punkChild;

	POSITION lPos = 0;
	if (sptr != 0 && SUCCEEDED(sptr->GetObjectPosInParent(&lPos)) && lPos != 0)
		return lPos;

	return m_listChilds.Find( punkChild );
	/*
	long lKey  = ::GetObjectKey( punkChild );
	POSITION pos = GetFirstChildPosition();

	while( pos )
	{
		POSITION posSave = pos;

		IUnknown *punk = GetNextChild( pos );

		if( ::GetObjectKey( punk ) == lKey )
			return posSave;
	}

	return 0;*/
}

//return the object flags
DWORD CDataObjectBase::GetNamedObjectFlags()
{
	DWORD	dwFlags = nofNormal;

	if( m_bUsedInOtherObject )
		dwFlags |= nofStoreByParent;
	if( m_bHostedByOtherObject )
		dwFlags |= nofHasHost;
	return dwFlags;
}
///////////////////////////////////////////////////////////////////////////////
// Named Data Object registrator and file name helper

CDataRegistrator::CDataRegistrator() 
	: CCompRegistrator(szPluginObject)
{
}	


void CDataRegistrator::RegisterDataObject(bool bRegister)
{	
	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();

	for (COleObjectFactory* pFactory = pModuleState->m_factoryList; pFactory != NULL; pFactory = pFactory->m_pNextFactory)
	{
		// register any non-registered, non-doctemplate factories
		if (!pFactory->IsRegistered() && pFactory->IsKindOf(RUNTIME_CLASS(CVTFactory)) && 
			((CVTFactory*)pFactory)->GetClassID() != CLSID_NULL)
		{
			if (!pFactory->Register())
				continue;

			CString	sTest = ((CVTFactory*)pFactory)->GetProgID();

			if (IsDataObject(sTest))
			{
				if (bRegister)
					RegisterComponent(sTest);
				else
					UnRegisterComponent(sTest);
			}
		}
	}


	for (CDynLinkLibrary* pDLL = pModuleState->m_libraryList; pDLL != NULL; pDLL = pDLL->m_pNextDLL)
	{
		for (COleObjectFactory *pFactory = pDLL->m_factoryList; pFactory != NULL; pFactory = pFactory->m_pNextFactory)
		{
			if (pFactory->IsKindOf(RUNTIME_CLASS(CVTFactory)))
			{
				CString	sTest = ((CVTFactory*)pFactory)->GetProgID();

				if (IsDataObject(sTest))
				{
					if (bRegister)
						RegisterComponent(sTest);
					else
						UnRegisterComponent(sTest);
				}
			}
		}
	}

//	COleObjectFactory::UpdateRegistryAll(bRegister);
}	


bool CDataRegistrator::IsDataObject(const char *szProgID)
{
	CString sProgID(NDO_NAME_PREFIX);
	CString sTest = szProgID;

	sTest = sTest.Left(sProgID.GetLength());
	return sTest == sProgID;
}

/////////////////////////////////////////////////////////////////////////////////////////
//CDataObjectListBase


BEGIN_INTERFACE_MAP(CDataObjectListBase, CDataObjectBase)
	INTERFACE_PART(CDataObjectListBase, IID_IDataObjectList, DataObjList)
END_INTERFACE_MAP();

BEGIN_DISPATCH_MAP(CDataObjectListBase, CDataObjectBase)
	//{{AFX_DISPATCH_MAP(CDataObjectListBase)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		DISP_PROPERTY_EX(CDataObjectListBase, "ActiveImage", GetActiveObject, SetActiveObject, VT_DISPATCH)
		DISP_FUNCTION(CDataObjectListBase, "GetCount", GetCount, VT_I4, VTS_NONE)
		DISP_FUNCTION(CDataObjectListBase, "GetObjectByIdx", GetObject, VT_DISPATCH, VTS_I4)
		DISP_FUNCTION(CDataObjectListBase, "AddObject", AddObject, VT_I4, VTS_DISPATCH)
		DISP_FUNCTION(CDataObjectListBase, "RemoveObjectByIdx", RemoveObject, VT_EMPTY, VTS_I4)
		DISP_FUNCTION(CDataObjectListBase, "MakeEmpty", MakeEmpty, VT_EMPTY, VTS_NONE)
		DISP_FUNCTION(CDataObjectListBase, "RemoveObjectByPos", RemoveObjectByPos, VT_EMPTY, VTS_I4)
		DISP_FUNCTION(CDataObjectListBase, "UpdateObject", disp_UpdateObject, VT_EMPTY, VTS_PVARIANT)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

long CDataObjectListBase::GetCount()
{
	return m_listChilds.GetCount();
}

LPDISPATCH CDataObjectListBase::GetActiveObject()
{
	POSITION pos = (POSITION)DoGetActiveChild();
	IUnknown* punkObj = 0;
	if(pos) punkObj = (IUnknown*)m_listChilds.GetAt(pos);
	IDispatch* pdisp = 0;
	if(punkObj)
	{
		punkObj->QueryInterface(IID_IDispatch, (void**)&pdisp);
		//punkObj->Release();
	}

	return pdisp;
}

void CDataObjectListBase::SetActiveObject(LPDISPATCH newValue)
{
	if(newValue==0)
	{ // можно передавать Nothing - снимаем выделение
		DoSetActiveChild(0);
		return;
	}

	IUnknownPtr	sptrObj(newValue);
	POSITION pos = m_listChilds.Find(sptrObj);
	if(pos == NULL)
	{
		ErrorMsg(IDS_NOTOBJECT_INLIST);
		return;
	}
	DoSetActiveChild((TPOS)pos);
}

LPDISPATCH CDataObjectListBase::GetObject(long nPos)
{
	if(!(nPos < m_listChilds.GetCount() && nPos >= 0))
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return 0;
	}
	POSITION pos = m_listChilds.FindIndex(nPos);

	if(pos)
	{
		IUnknown* punkObj = (IUnknown*)m_listChilds.GetAt(pos);
		IDispatch* pdisp = 0;
		if(punkObj)
		{
			punkObj->QueryInterface(IID_IDispatch, (void**)&pdisp);
			//punkObj->Release();
		}
		return pdisp;
	}
	else
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return 0;
	}
}

long CDataObjectListBase::AddObject(LPDISPATCH pdispObject)
{
	INamedDataObject2Ptr	ptrNamed(pdispObject);
	if(ptrNamed != 0)
	{
		ptrNamed->SetParent( GetControllingUnknown(), 0 );
//		_AddChild(sptrUnk);
		return m_listChilds.GetCount()-1;
	}
	else
	{
		ErrorMsg(IDS_INCORRECT_OBJ);
		return -1;
	}
}

void CDataObjectListBase::RemoveObject(INT_PTR nIndex)
{
	if (!(nIndex < m_listChilds.GetCount() && nIndex >= 0))
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return;
	}
	POSITION pos = m_listChilds.FindIndex(nIndex);

	//_RemoveChild((IUnknown*)m_listChilds.GetAt(pos), pos);
	INamedDataObject2Ptr	ptrNamed( (IUnknown*)m_listChilds.GetAt(pos) );
	if(ptrNamed!=0) ptrNamed->SetParent( 0, 0 );

}

void CDataObjectListBase::RemoveObjectByPos(TPOS nPos)
{
	POSITION pos = (POSITION)nPos;
	//_RemoveChild((IUnknown*)m_listChilds.GetAt(pos), pos);
	INamedDataObject2Ptr	ptrNamed( (IUnknown*)m_listChilds.GetAt(pos) );
	if(ptrNamed!=0) ptrNamed->SetParent( 0, 0 );
}

void CDataObjectListBase::MakeEmpty()
{
	POSITION pos = m_listChilds.GetHeadPosition();
	while(pos)
	{

		INamedDataObject2Ptr	ptrNamed( (IUnknown*)m_listChilds.GetNext(pos) );
		ptrNamed->SetParent( 0, 0 );
		//_RemoveChild((IUnknown*)m_listChilds.GetNext(pos), pos);
	}
}



IMPLEMENT_UNKNOWN(CDataObjectListBase, DataObjList);

HRESULT CDataObjectListBase::XDataObjList::UpdateObject( IUnknown* punkObj )
{
	_try_nested(CDataObjectListBase, DataObjList, UpdateObject)
	{
		pThis->UpdateObject(punkObj );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDataObjectListBase::XDataObjList::LockObjectUpdate( BOOL bLock )
{
	_try_nested(CDataObjectListBase, DataObjList, UpdateObject)
	{
		pThis->m_bLockUpdate = bLock==TRUE;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDataObjectListBase::XDataObjList::GetObjectUpdateLock( BOOL *pbLock )
{
	_try_nested(CDataObjectListBase, DataObjList, UpdateObject)
	{
		*pbLock = pThis->m_bLockUpdate;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDataObjectListBase::XDataObjList::MoveObject(GUID keyFrom, BOOL bDirection)
{
	_try_nested(CDataObjectListBase, DataObjList, MoveObject)
	{
		pThis->_MoveObject(keyFrom, bDirection == TRUE);
		return S_OK;
	}
	_catch_nested;
}



void CDataObjectListBase::OnAddChild( IUnknown *punkNewChild, TPOS pos )
{
	punkNewChild->AddRef();

	//m_posActiveObject = pos;
	if( m_punkNamedData && !m_bLockUpdate )
		::FireEventNotify( m_punkNamedData, szEventChangeObjectList, GetControllingUnknown(), punkNewChild, cncAdd );
}

void CDataObjectListBase::OnDeleteChild(IUnknown *punkChildToDelete, TPOS pos)
{
	if( m_posActiveObject == pos )
	{
		POSITION posTest = (POSITION)m_posActiveObject;
		m_listChilds.GetNext( posTest );
		if( !posTest )
		{
			posTest = (POSITION)m_posActiveObject;
			m_listChilds.GetPrev( posTest );
		}
		m_posActiveObject = (TPOS)posTest;

		if (m_posActiveObject)
			UpdateActiveObject();
	}

	if( m_punkNamedData && !m_bLockUpdate )
		::FireEventNotify( m_punkNamedData, szEventChangeObjectList, GetControllingUnknown(), punkChildToDelete, cncRemove );


	punkChildToDelete->Release();

}

CDataObjectListBase::CDataObjectListBase()
{
	m_bLockUpdate = false;
	m_posActiveObject = 0;
	EnableAutomation();
}

void CDataObjectListBase::OnBeforeSerialize( CStreamEx &ar )
{
	if( ar.IsLoading() )
		m_bLockUpdate = true;
}
void CDataObjectListBase::OnAfterSerialize( CStreamEx &ar )
{
	if( m_bLockUpdate )
	{
		m_bLockUpdate = false;
		::FireEventNotify( m_punkNamedData, szEventChangeObjectList, GetControllingUnknown(), 0, cncReset );
	}

	if( ar.IsLoading() )
	{
		m_posActiveObject = (TPOS)m_listChilds.GetHeadPosition();
		UpdateActiveObject();
	}
}

//static bool bInSetActiveChild = false;
bool CDataObjectListBase::DoSetActiveChild(TPOS pos)
{
	//CTimeTest time( true, "CDataObjectListBase::DoSetActiveChild" );
//	if( m_posActiveObject == pos )
//		return true;

	m_posActiveObject = (TPOS)pos;

	UpdateActiveObject();
	return true;
}

TPOS CDataObjectListBase::DoGetActiveChild()
{
	return m_posActiveObject;
}

void CDataObjectListBase::UpdateActiveObject()
{
	if( !m_bLockUpdate )
	{
		m_bLockUpdate = true;
		
		IUnknown	*punkActive = 0;
		if( m_posActiveObject )
		{
			punkActive = (IUnknown*)m_listChilds.GetAt((POSITION)m_posActiveObject);
			::FireEventNotify( m_punkNamedData, szEventChangeObjectList, GetControllingUnknown(), punkActive, cncActivate );
		}
		else
		{
			::FireEventNotify( m_punkNamedData, szEventChangeObjectList, GetControllingUnknown(), 0, cncReset );
		}

		m_bLockUpdate = false;
	}
}

void CDataObjectListBase::UpdateDeactiveObject()
{
	if( !m_bLockUpdate )
	{
		m_bLockUpdate = true;
		
		IUnknown	*punkActive = 0;
		if( m_posActiveObject )
			punkActive = (IUnknown*)m_listChilds.GetAt((POSITION)m_posActiveObject);
			::FireEventNotify( m_punkNamedData, szEventChangeObjectList, GetControllingUnknown(), punkActive, cncDeactivate );

		m_bLockUpdate = false;
	}
}


void CDataObjectListBase::UpdateObject(IUnknown* punkObj)
{
	if( !m_bLockUpdate )
	{
		if( punkObj )
			::FireEventNotify( m_punkNamedData, szEventChangeObjectList, GetControllingUnknown(), punkObj, cncChange );
		else
			::FireEventNotify( m_punkNamedData, szEventChangeObjectList, GetControllingUnknown(), 0, cncReset );

	}
}



//////////////////////////////////////////////////////////////////////
LPDISPATCH CDataObjectBase::disp_GetPrivateNamedData() 
{
	if( m_punkAggrNamedData == NULL ) // Not create yet
	{
		//try to create
		InitAggrNamedData();
	}

	if( m_punkAggrNamedData == NULL ) // oops!!
		return NULL;

	IDispatch* pdisp = NULL;	
	m_punkAggrNamedData->QueryInterface(IID_IDispatch, (void**)&pdisp);	

	return pdisp;
}

//////////////////////////////////////////////////////////////////////
BOOL CDataObjectBase::disp_IsVirtual()
{	
	DWORD dwFlags = GetNamedObjectFlags();	
	return ( dwFlags & nofStoreByParent );
}

//////////////////////////////////////////////////////////////////////
void CDataObjectBase::disp_StoreData(long dwDirection) 
{
	if( m_pParent == 0 )
		return;

	ExchangeDataWithParent( dwDirection );
}

//////////////////////////////////////////////////////////////////////
LPDISPATCH CDataObjectBase::disp_GetParent() 
{

	if( !m_pParent )
		return NULL;

	IDispatch* pdisp = NULL;
	m_pParent->QueryInterface(IID_IDispatch, (void**)&pdisp);		

	return pdisp;
}

//////////////////////////////////////////////////////////////////////
long CDataObjectBase::disp_GetChildsCount()
{
	return (long)m_listChilds.GetCount();
}

//////////////////////////////////////////////////////////////////////
void CDataObjectBase::disp_GetFirstChildPos( VARIANT FAR* pvarPos )
{
	if( pvarPos )
		*pvarPos = _variant_t( (long)0 );	

	*pvarPos = _variant_t( (long)GetFirstChildPosition() );		
}


//////////////////////////////////////////////////////////////////////
void CDataObjectBase::disp_GetLastChildPos( VARIANT FAR* pvarPos )
{
	if( pvarPos )
		*pvarPos = _variant_t( (long)0 );	

	*pvarPos = _variant_t( (long)GetLastChildPosition() );	
}


//////////////////////////////////////////////////////////////////////
LPDISPATCH CDataObjectBase::disp_GetNextChild( VARIANT FAR* pvarPos )
{
	long lPos = 0;
	IDispatch* pdisp = NULL;
	
	if( pvarPos )
	{
		lPos = pvarPos->lVal;

		IUnknown* punk = 0;
		punk = GetNextChild( (POSITION&)lPos );
				
		*pvarPos = _variant_t( (long)lPos ); 

		if( punk )
		{
			punk->QueryInterface(IID_IDispatch, (void**)&pdisp);		
			//punk->Release();
		}		
	}

	return pdisp;
}


//////////////////////////////////////////////////////////////////////
LPDISPATCH CDataObjectBase::disp_GetPrevChild( VARIANT FAR* pvarPos )
{
	long lPos = 0;
	IDispatch* pdisp = NULL;

	if( pvarPos )
	{
		lPos = pvarPos->lVal;

		IUnknown* punk = 0;
		punk = GetPrevChild((POSITION&)lPos);
		
		*pvarPos = _variant_t( (long)lPos ); 
		
		if( punk )
		{
			punk->QueryInterface(IID_IDispatch, (void**)&pdisp);		
			//punk->Release();
		}		
	}

	return pdisp;
}

//////////////////////////////////////////////////////////////////////
BOOL CDataObjectBase::disp_IsEmpty()
{
	return IsObjectEmpty();
}

BOOL CDataObjectBase::IsPrivateNamedDataExist() 
{
	return ( m_punkAggrNamedData != 0 );
}


void CDataObjectListBase::_MoveObject(GuidKey keyFrom, bool bDirection)
{
	POSITION posObj = 0, posPrev = 0, posNext = 0; 
	POSITION pos = m_listChilds.GetHeadPosition();
	while(pos)
	{
		posObj = pos;
		if(GetObjectKey((IUnknown*)m_listChilds.GetNext(pos)) == keyFrom)
		{
			posNext = pos;
			break;
		}
		posPrev = posObj;
	}
	if(bDirection)
	{
		//to head side
		if(posPrev && posObj)
		{
			IUnknown* punkObject = (IUnknown*)m_listChilds.GetAt(posObj);
			m_listChilds.RemoveAt(posObj);
			m_listChilds.InsertBefore(posPrev, punkObject);
			::FireEventNotify(m_punkNamedData, szEventChangeObjectList, GetControllingUnknown(), punkObject, cncAdd);
		}
	}
	else
	{
		//to tail side
		if(posNext && posObj)
		{
			IUnknown* punkObject = (IUnknown*)m_listChilds.GetAt(posObj);
			m_listChilds.RemoveAt(posObj);
			m_listChilds.InsertAfter(posNext, punkObject);
			::FireEventNotify(m_punkNamedData, szEventChangeObjectList, GetControllingUnknown(), punkObject, cncAdd);
		}
	}
}

void CDataObjectListBase::disp_UpdateObject(VARIANT FAR* pvarObj) 
{
	m_xDataObjList.UpdateObject( pvarObj->punkVal );
}

