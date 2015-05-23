// AXCtrlBase.cpp: implementation of the CAXCtrlBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AXCtrlBase.h"


int	g_nCreatedControlsCount = 0;


IMPLEMENT_DYNAMIC(CAXCtrlBase, CDataObjectBase);

//////////////////////////////////////////////////////////////////////
// class CAXCtrlBase
//////////////////////////////////////////////////////////////////////

// {29F9C5D0-F409-4fb6-A98A-4BE6D88F4C4F}
static const IID IID_IAxCtrlDisp = 
{ 0x29f9c5d0, 0xf409, 0x4fb6, { 0xa9, 0x8a, 0x4b, 0xe6, 0xd8, 0x8f, 0x4c, 0x4f } };

BEGIN_INTERFACE_MAP(CAXCtrlBase, CDataObjectBase)
	INTERFACE_PART(CAXCtrlBase, IID_IActiveXCtrl, Ctrl)
	INTERFACE_PART(CAXCtrlBase, IID_IViewSubType, ViewSubType)
	INTERFACE_PART(CAXCtrlBase, IID_IAxCtrlDisp, Dispatch)	
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CAXCtrlBase, Ctrl)

BEGIN_DISPATCH_MAP(CAXCtrlBase, CDataObjectBase)
	//{{AFX_DISPATCH_MAP(CAXCtrlBase)
	DISP_FUNCTION(CAXCtrlBase, "StoreCtrlData", StoreCtrlData, VT_EMPTY, VTS_DISPATCH)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()




CAXCtrlBase::CAXCtrlBase()
: m_ViewSubType(0)
{
	m_rect = CRect( 100, 100, 300, 250 );
	m_nID = 0;
	m_dwStyle = WS_CHILD|WS_VISIBLE;
	m_dwMemSize = 0;
	m_pObjectData = 0;

	m_strControlName.Format( "Control%d", g_nCreatedControlsCount++ );
}

CAXCtrlBase::~CAXCtrlBase()
{
	if( m_pObjectData )
		delete m_pObjectData;
}

HRESULT CAXCtrlBase::XCtrl::SetProgID( BSTR bstrProgID )
{
	_try_nested(CAXCtrlBase, Ctrl, SetProgID)
	{
		pThis->m_strProgID = bstrProgID;
		return S_OK;
	}
	_catch_nested;
}
HRESULT CAXCtrlBase::XCtrl::GetProgID( BSTR *pbstrProgID )
{
	_try_nested(CAXCtrlBase, Ctrl, GetProgID)
	{
		*pbstrProgID = pThis->m_strProgID.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXCtrlBase::XCtrl::GetRect( RECT *prect )
{
	_try_nested(CAXCtrlBase, Ctrl, GetRect)
	{
		prect->left = pThis->m_rect.left;
		prect->top = pThis->m_rect.top;
		prect->right = pThis->m_rect.right;
		prect->bottom = pThis->m_rect.bottom;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXCtrlBase::XCtrl::SetRect( RECT rect )
{
	_try_nested(CAXCtrlBase, Ctrl, SetRect)
	{
		pThis->m_rect = rect;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXCtrlBase::XCtrl::GetStyle( DWORD *pdwStyle )
{
	_try_nested(CAXCtrlBase, Ctrl, GetStyle)
	{
		*pdwStyle = pThis->m_dwStyle;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXCtrlBase::XCtrl::SetStyle( DWORD dwStyle )
{
	_try_nested(CAXCtrlBase, Ctrl, SetStyle)
	{
		pThis->m_dwStyle = dwStyle;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXCtrlBase::XCtrl::GetControlID( UINT *pnID )
{
	_try_nested(CAXCtrlBase, Ctrl, GetControlID)
	{
		*pnID = pThis->m_nID;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXCtrlBase::XCtrl::SetControlID( UINT nID )
{
	_try_nested(CAXCtrlBase, Ctrl, SetControlID)
	{
		pThis->m_nID = nID;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXCtrlBase::XCtrl::GetObjectData( BYTE **ppbyte, DWORD *pdwMemSize )
{
	_try_nested(CAXCtrlBase, Ctrl, GetObjectData)
	{
		if( pdwMemSize )
			*pdwMemSize = pThis->m_dwMemSize;
		if( ppbyte )
			if( pThis->m_dwMemSize )
				memcpy( *ppbyte, pThis->m_pObjectData, pThis->m_dwMemSize );
			else
				*ppbyte = 0;
		return S_OK;
	}
	_catch_nested;
}
HRESULT CAXCtrlBase::XCtrl::SetObjectData( BYTE *pbyte, DWORD dwMemSize )
{
	_try_nested(CAXCtrlBase, Ctrl, SetObjectData)
	{
		if( pThis->m_pObjectData )
			delete pThis->m_pObjectData;
		pThis->m_pObjectData = 0;
		pThis->m_dwMemSize = dwMemSize;

		if( dwMemSize )
		{
			pThis->m_pObjectData = new byte[dwMemSize];
			memcpy( pThis->m_pObjectData, pbyte, dwMemSize );
		}

		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXCtrlBase::XCtrl::GetName( BSTR	*pbstrName )
{
	_try_nested(CAXCtrlBase, Ctrl, GetName)
	{
		*pbstrName = pThis->m_strControlName.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXCtrlBase::XCtrl::SetName( BSTR	bstrName )
{
	_try_nested(CAXCtrlBase, Ctrl, SetName)
	{
		pThis->m_strControlName = bstrName;
		return S_OK;
	}
	_catch_nested;
}


DWORD CAXCtrlBase::GetNamedObjectFlags()
{
	return nofNormal|nofStoreByParent;
}

bool CAXCtrlBase::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	long	lVersion = 3;

	if( ar.IsStoring() )
	{
		ar<<lVersion;
		ar<<m_strProgID<<m_rect.left<<m_rect.top<<m_rect.right<<m_rect.bottom<<
			(long)m_nID<<(long)m_dwStyle;
		ar<<m_dwMemSize;
		if( m_dwMemSize )
			ar.Write( m_pObjectData, m_dwMemSize );
		ar<<m_strControlName;

	}
	else
	{
		if( m_pObjectData )
			delete m_pObjectData;
		m_pObjectData = 0;

		ar>>lVersion;
		ar>>m_strProgID>>m_rect.left>>m_rect.top>>m_rect.right>>m_rect.bottom>>
			(long&)m_nID>>(long&)m_dwStyle;

		if( lVersion >= 2 )
		{
			ar>>m_dwMemSize;
			if( m_dwMemSize )
			{
				m_pObjectData = new byte[m_dwMemSize];
				ar.Read( m_pObjectData, m_dwMemSize );
			}
		}

		if( lVersion >= 3 )
		{
			ar>>m_strControlName;
		}
		else
			m_strControlName = ::GetObjectName( GetControllingUnknown() );
	}

	CDataObjectBase::SerializeObject( ar, pparams );

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CAXCtrlBase::XCtrl::IsUniqueName( BSTR	bstrName, BOOL* pbUniqueName )
{
	_try_nested(CAXCtrlBase, Ctrl, IsUniqueName)
	{
		*pbUniqueName = (pThis->IsUniqueName( bstrName ) == TRUE );
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CAXCtrlBase::XCtrl::GenerateUniqueName( BSTR bstrOfferName, BSTR* pbstrNewName )
{
	_try_nested(CAXCtrlBase, Ctrl, GenerateUniqueName)
	{		
		*pbstrNewName = pThis->GenerateUniqueName( bstrOfferName ).AllocSysString();
		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
bool CAXCtrlBase::IsUniqueName( CString strName )
{

	bool bUniqueName = true;
	sptrIActiveXCtrl spThisAX( GetControllingUnknown() );

	if( m_pParent )
	{		

		CObjectListWrp	list( m_pParent );

		POSITION	pos = list.GetFirstObjectPosition();

		while( pos )
		{
			IUnknown *punk = NULL;
			punk = list.GetNextObject( pos );
			if( punk )
			{
				sptrIActiveXCtrl sptrAX( punk );
				punk->Release();
				if( (sptrAX != NULL) && (sptrAX != spThisAX) )
				{
					BSTR bstrName;

					sptrAX->GetName( &bstrName );
					if( CString(bstrName) == strName )
						bUniqueName = false;

					::SysFreeString( bstrName );

				}
			}
			
		}


	}
	return bUniqueName;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
CString CAXCtrlBase::GenerateUniqueName( CString strOfferName )
{
	if( IsUniqueName(strOfferName) )
		return strOfferName;

	CString strUniqueName;
	CArray<CString, CString> arrName;

	sptrIActiveXCtrl spThisAX( GetControllingUnknown() );

	if( m_pParent )
	{
		CObjectListWrp	list( m_pParent );

		POSITION	pos = list.GetFirstObjectPosition();

		while( pos )
		{
			IUnknown *punk = NULL;
			punk = list.GetNextObject( pos );
			if( punk )
			{
				sptrIActiveXCtrl sptrAX( punk );
				punk->Release();
				
				if( (sptrAX != NULL) && (spThisAX != sptrAX) )
				{
					BSTR bstrName;
					sptrAX->GetName( &bstrName );
					arrName.Add( CString(bstrName) );
					::SysFreeString( bstrName );
				}
			}			
		}
	}

	CString strDefault = "Control";
	CString strBase;

	for( int i=0;i<strOfferName.GetLength();i++ )
	{
		if( !(strOfferName[i] >= '0' && strOfferName[i] <= '9') )
		{
			strBase += strOfferName[i];
		}
		
	}

	if( strBase.IsEmpty() )
		strBase = strDefault;

	int nCurCount = 0;
	bool bEnough = false;
	bool bFind = false;
	

	while( !bEnough )
	{
		strUniqueName.Format( "%s%d", strBase, ++nCurCount );

		bFind = false;
		for( i=0;i<arrName.GetSize();i++)
			if( arrName[i] == strUniqueName )
				bFind = true;
		
		if( !bFind )
			bEnough = true;

		if( nCurCount == 10000 )
			bEnough = true;
	}

	arrName.RemoveAll();
	
	return strUniqueName;
}


//////////////////////////////////////////////////////////////////////
void CAXCtrlBase::StoreCtrlData( LPDISPATCH lpCtrlDisp )
{
	if( !lpCtrlDisp )	return;

	::StoreContainerToDataObject( GetControllingUnknown(), lpCtrlDisp ); //@@@@@@@@@@@@@@@@@@@@@@@@	
}

//////////////////////////////////////////////////////////////////////
// class CAXFormBase
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CAXFormBase, CDataObjectBase);

BEGIN_INTERFACE_MAP(CAXFormBase, CDataObjectListBase)
	INTERFACE_PART(CAXFormBase, IID_IActiveXForm, Form)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CAXFormBase, Form)


/////////////////////////////////////////////////////////////////////////////////////////////////////
CAXFormBase::CAXFormBase()
{	
	m_size = CSize( 400, 300 );

	IUnknown	*punk = ::CreateTypedObject( szArgumentTypeScript );
	m_sptrScript = punk;
	if( punk )
	{
		m_sptrScript->SetHostedFlag( true );
		punk->Release();
	}
	GuidKey	key = GetObjectKey( punk );
}

CAXFormBase::~CAXFormBase()
{
}


HRESULT CAXFormBase::XForm::SetTitle( BSTR bstr )
{
	_try_nested( CAXFormBase, Form, SetTitle)
	{
		pThis->m_strTitle = bstr;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXFormBase::XForm::GetTitle( BSTR *pbstr )
{
	_try_nested( CAXFormBase, Form, GetTitle)
	{
		*pbstr = pThis->m_strTitle.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXFormBase::XForm::SetSize( SIZE size )
{
	_try_nested( CAXFormBase, Form, SetSize)
	{
		pThis->m_size = size;

		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXFormBase::XForm::GetSize( SIZE *psize )
{
	_try_nested( CAXFormBase, Form, GetSize)
	{
		*psize = pThis->m_size;

		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXFormBase::XForm::AutoUpdateSize()
{
	_try_nested( CAXFormBase, Form, AutoUpdateSize )
	{
		CObjectListWrp	list( this );

		POSITION	pos = list.GetFirstObjectPosition();

		while( pos )
		{
			IUnknown *punk = list.GetNextObject( pos );
			sptrIActiveXCtrl sptrAX( punk );
			punk->Release();

			CRect	rect;
			sptrAX->GetRect( &rect );

			pThis->m_size.cx = max( pThis->m_size.cx, rect.right+10 );
			pThis->m_size.cy = max( pThis->m_size.cy, rect.bottom+10 );
		}

		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXFormBase::XForm::GetScript( IUnknown **ppunkScript )
{
	_try_nested( CAXFormBase, Form, AutoUpdateSize )
	{

		*ppunkScript = pThis->m_sptrScript;

		if( *ppunkScript )
			(*ppunkScript)->AddRef();

		return S_OK;
	}
	_catch_nested;
}

// Implement IViewSubType interface

IMPLEMENT_UNKNOWN(CAXCtrlBase, ViewSubType)

HRESULT CAXCtrlBase::XViewSubType::GetViewSubType( unsigned long* pViewSubType )
{
	_try_nested(CAXCtrlBase, ViewSubType, SetViewSubType )
	{	
		*pViewSubType=pThis->m_ViewSubType;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXCtrlBase::XViewSubType::SetViewSubType( unsigned long ViewSubType )
{
	_try_nested(CAXCtrlBase, ViewSubType, SetViewSubType )
	{	
		pThis->m_ViewSubType=ViewSubType;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXFormBase::XForm::SetScript( IUnknown *punkScript )
{
	_try_nested( CAXFormBase, Form, AutoUpdateSize )
	{
		pThis->m_sptrScript = punkScript;
		return S_OK;
	}
	_catch_nested;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////////////////////////////////////////////////////////////////////////////

bool CAXFormBase::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	CDataObjectListBase::SerializeObject( ar, pparams );

	long	lVersion = 2;

	if( ar.IsStoring() )
	{
		ar<<lVersion;
		ar<<m_strTitle<<m_size.cx<<m_size.cy;

		::StoreDataObjectToArchive( ar, m_sptrScript, pparams );
		if( m_sptrScript != 0 )
			m_sptrScript->SetModifiedFlag( false );
	}
	else
	{
		ar>>lVersion;
		ar>>m_strTitle>>m_size.cx>>m_size.cy;

		if( lVersion >= 2 )
		{
			IUnknown *punk = ::LoadDataObjectFromArchive( ar, pparams );
			m_sptrScript = punk;

			if( punk )
			{
				m_sptrScript->SetHostedFlag( true );
				m_sptrScript->SetModifiedFlag( false );
				punk->Release();
			}
		}
	}
	return true;
}

bool CAXFormBase::CanBeBaseObject()
{
	return true;
}

bool CAXFormBase::IsModified()
{
	BOOL	bIs = false;
	m_sptrScript->IsModified( &bIs );
	return bIs == TRUE;
}

void CAXFormBase::SetModifiedFlag( bool bSet )
{
	m_sptrScript->SetModifiedFlag( bSet );
}
