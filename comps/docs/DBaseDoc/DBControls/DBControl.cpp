// DBControl.cpp : implementation file
//

#include "stdafx.h"
#include "DBControls.h"
#include "DBControl.h"


/////////////////////////////////////////////////////////////////////////////
// CDBControlImpl

IMPLEMENT_UNKNOWN_BASE(CDBControlImpl, DBControl);
IMPLEMENT_UNKNOWN_BASE(CDBControlImpl, DBaseListener);

CDBControlImpl::CDBControlImpl()
{
	InitDefaults();
	m_bCanRecieveNotify = true;
	
	m_BlankViewMode = bvmNoInfo;
	m_ReportViewMode = vmNoInfo;

	LOGFONT lf;
	::ZeroMemory( &lf, sizeof(lf) );
	CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) );	
	if( pFont )
	{
		pFont->GetLogFont( &lf );
		lf.lfCharSet = 	DEFAULT_CHARSET;
		//strcpy( lf.lfFaceName, "Arial" );
		m_Font.CreatePointFontIndirect( &lf );
	}
}

CDBControlImpl::~CDBControlImpl()
{
	 UnRegisterDB();
}

/////////////////////////////////////////////////////////////////////////////
void CDBControlImpl::InitDefaults()
{
	m_strTableName.Empty();
	m_strFieldName.Empty();
	m_bEnableEnumeration	=	FALSE;
	m_bAutoLabel			=	TRUE;
	m_nLabelPosition		=	0;
	m_bReadOnly				=	FALSE;

	m_bWasRegister = false;

	m_punkControl = NULL;
}

/////////////////////////////////////////////////////////////////////////////
void CDBControlImpl::RegisterDB()
{
	GetDBaseDocument( m_spDBaseDocument );	

	if( m_bWasRegister )
		return;

	if( m_spDBaseDocument )
	{		
 		m_punkControl = Unknown();
		if( m_punkControl == NULL )
			return;

		m_bWasRegister = true;

		m_spDBaseDocument->RegisterDBaseListener( m_punkControl );		


	}
}

/////////////////////////////////////////////////////////////////////////////
void CDBControlImpl::UnRegisterDB()
{
	if( m_bWasRegister && m_spDBaseDocument != NULL )
		m_spDBaseDocument->UnRegisterDBaseListener( m_punkControl );		
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBControlImpl::IDBControl_Serialize( CArchive &ar )
{
	long nVer = 3;
	try
	{
		if( ar.IsStoring() )	
		{
			ar << nVer;
			ar << m_strTableName;
			ar << m_strFieldName;
			ar << m_bEnableEnumeration;
			ar << m_bAutoLabel;
			ar << m_nLabelPosition;
			ar << m_bReadOnly;
        }
		else
		{
			ar >> nVer;
			ar >> m_strTableName;
			ar >> m_strFieldName;
			ar >> m_bEnableEnumeration;
			ar >> m_bAutoLabel;
			if( nVer >= 2 )
				ar >> m_nLabelPosition;
			if( nVer >= 3 )
				ar >> m_bReadOnly;

			m_strTableName.MakeLower();
			m_strFieldName.MakeLower();


			IDBControl_BuildAppearanceAfterLoad();
		}


	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;

}

CIntIniValue g_FrameMode("DBControls", "Frame", 4);
CIntIniValue g_ShowFocused("DBControls", "ShowFocus", TRUE);
void CDBControlImpl::DrawBorder( CDC* pdc, CRect rcBorder, bool bFocused )
{
	if ( (int)g_FrameMode > 0)
	{
		UINT nFlags = g_FrameMode == 1 ? BF_RECT|BF_SOFT : 
						g_FrameMode == 2 ? BF_RECT|BF_MONO :
						g_FrameMode == 3 ? BF_RECT|BF_FLAT :
						BF_RECT;
		if( bFocused && g_ShowFocused )
			pdc->DrawEdge( &rcBorder, EDGE_SUNKEN, nFlags );
		else
			pdc->DrawEdge( &rcBorder, EDGE_RAISED, nFlags );
	}
}


sptrISelectQuery CDBControlImpl::GetActiveQuery()
{
	if( m_spDBaseDocument == NULL )
		return NULL;

	IUnknown* pUnkQuery = NULL;
	m_spDBaseDocument->GetActiveQuery( &pUnkQuery );
	if( pUnkQuery == NULL )
		return NULL;
	
	sptrISelectQuery spQuery( pUnkQuery );
	pUnkQuery->Release();

	return spQuery;
}

/////////////////////////////////////////////////////////////////////////////
CString CDBControlImpl::IDBControl_GetTableName( )
{
	return m_strTableName;
}

/////////////////////////////////////////////////////////////////////////////
void CDBControlImpl::IDBControl_SetTableName( CString strTableName )
{
	m_strTableName = strTableName;
	IDBControl_OnSetTableName();
}

/////////////////////////////////////////////////////////////////////////////
CString CDBControlImpl::IDBControl_GetFieldName( )
{
	return m_strFieldName;
}

/////////////////////////////////////////////////////////////////////////////
void CDBControlImpl::IDBControl_SetFieldName( CString strFieldName )
{
	m_strFieldName = strFieldName;
	IDBControl_OnSetFieldName();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBControlImpl::IDBControl_GetEnableEnumeration( )
{
	return m_bEnableEnumeration;
}

/////////////////////////////////////////////////////////////////////////////
void CDBControlImpl::IDBControl_SetEnableEnumeration( BOOL bEnable )
{
	m_bEnableEnumeration = bEnable;
	IDBControl_OnSetEnableEnumeration();

	RefreshAppearance();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBControlImpl::IDBControl_GetAutoLabel( )
{
	return m_bAutoLabel;
}

/////////////////////////////////////////////////////////////////////////////
void CDBControlImpl::IDBControl_SetAutoLabel( BOOL bEnable )
{
	m_bAutoLabel = bEnable;
	IDBControl_OnSetAutoLabel();

	RefreshAppearance();
}

/////////////////////////////////////////////////////////////////////////////
short CDBControlImpl::IDBControl_GetLabelPosition( )
{
	return m_nLabelPosition;
}

/////////////////////////////////////////////////////////////////////////////
void CDBControlImpl::IDBControl_SetLabelPosition( short nLabelPosition )
{
	m_nLabelPosition = nLabelPosition;
	IDBControl_OnSetLabelPosition();
	
	RefreshAppearance();
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::GetTableName( BSTR* pbstrTableName )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)
	*pbstrTableName = pThis->IDBControl_GetTableName().AllocSysString();
	return S_OK;	
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::SetTableName( BSTR   bstrTableName )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)
	pThis->IDBControl_SetTableName( bstrTableName );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::GetFieldName( BSTR* pbstrFieldName )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)
	*pbstrFieldName = pThis->IDBControl_GetFieldName().AllocSysString();
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::SetFieldName( BSTR   bstrFieldName )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)
	pThis->IDBControl_SetFieldName( bstrFieldName );
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::GetEnableEnumeration( BOOL* pbEnable )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)
	*pbEnable = pThis->IDBControl_GetEnableEnumeration();
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::SetEnableEnumeration( BOOL   bEnable )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)
	pThis->IDBControl_SetEnableEnumeration( bEnable );	
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::GetAutoLabel( BOOL* pbAutoLabel )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)
	*pbAutoLabel = pThis->IDBControl_GetAutoLabel();
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::SetAutoLabel( BOOL   bAutoLabel )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)
	pThis->IDBControl_SetAutoLabel( bAutoLabel );	
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::GetValue( tagVARIANT *pvar )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)
	pThis->IDBControl_GetValue( pvar );	
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::SetValue( const tagVARIANT var )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)
	pThis->IDBControl_SetValue( var );	
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::GetLabelPosition( short* pbLabelPosition )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)
	*pbLabelPosition = pThis->IDBControl_GetLabelPosition();
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::SetLabelPosition( short  bLabelPosition )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)
	pThis->IDBControl_SetLabelPosition( bLabelPosition );	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::GetDBaseDocument( IUnknown** ppunkDBaseDocument )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)
	if( pThis->m_spDBaseDocument )
	{
		*ppunkDBaseDocument = pThis->m_spDBaseDocument;
		pThis->m_spDBaseDocument->AddRef();	
	}
	else
	{
		*ppunkDBaseDocument = NULL;
		//ASSERT(FALSE);
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::ChangeValue( const tagVARIANT var )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)	
	pThis->IDBControl_ChangeValue( var );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::SetBlankViewMode( short oldBlankViewMode, short newBlankMode )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)	
	
	pThis->m_BlankViewMode = (BlankViewMode)newBlankMode;
	pThis->IDBControl_OnChangeBlankViewMode( (BlankViewMode)oldBlankViewMode, (BlankViewMode)newBlankMode );

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::SetReportViewMode( short oldReportViewMode, short newReportMode )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)	
	
	pThis->m_ReportViewMode = (ViewMode)newReportMode;
	pThis->IDBControl_OnChangeReportViewMode( (ViewMode)oldReportViewMode, (ViewMode)newReportMode );

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::GetReadOnly( BOOL* pbReadOnly )
{
    METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)	
	
	if( !pbReadOnly )
		return E_INVALIDARG;

	*pbReadOnly = pThis->IDBControl2_GetReadOnly( );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBControl::SetReadOnly( BOOL	bReadOnly )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBControl)	
    
	pThis->IDBControl2_SetReadOnly( bReadOnly );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBControlImpl::XDBaseListener::OnNotify( BSTR bstrEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, tagVARIANT var )
{
	METHOD_PROLOGUE_BASE(CDBControlImpl, DBaseListener)	
	_variant_t data(var);
	CString strEvent( bstrEvent );
	pThis->OnDBaseNotify( strEvent, punkObject, punkDBaseDocument, bstrTableName, bstrFieldName, data );

	if( !strcmp( strEvent, szDBaseEventActiveFieldLost) || !strcmp( strEvent, szDBaseEventActiveFieldSet ) ) 
	{
		int nField = pThis->GetFieldIndexInQuery();
		if( nField != -1 && var.lVal == nField )
		{
			pThis->_RepaintCtrl();
		}	
	}


	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
CString CDBControlImpl::GetCaption()
{
	if( m_spDBaseDocument )
	{
		IDBaseStructPtr ptrDBS( m_spDBaseDocument );
		if( ptrDBS == 0 )
			return "";

		BSTR bstrCaption = 0;
		ptrDBS->GetFieldInfo( 
						_bstr_t( (LPCTSTR)m_strTableName ), 
						_bstr_t( (LPCTSTR)m_strFieldName ), 
						&bstrCaption, 0, 0, 0, 0, 0, 0, 0
			);
		CString strCaption;		

		if( bstrCaption )
		{
			strCaption = bstrCaption;
			if( bstrCaption )
				::SysFreeString( bstrCaption );
		}

//		strCaption += ": ";
		strCaption += "  ";
		return strCaption;
	}

	return "";
}

void CDBControlImpl::IDBControl_OnChangeBlankViewMode( BlankViewMode oldBlankViewMode, BlankViewMode newBlankMode )
{
	sptrISelectQuery spSelectQuery = GetActiveQuery();

	if( spSelectQuery == NULL )
	{
		SetEnableWindow( false );		
	}
	else
	{
		SetEnableWindow( true );		
	}

	EnterViewMode( newBlankMode );
}

void CDBControlImpl::IDBControl_OnChangeReportViewMode( ViewMode oldReportViewMode, ViewMode newReportMode )
{

}

BOOL CDBControlImpl::IDBControl2_GetReadOnly( )
{
	return m_bReadOnly;
}

void CDBControlImpl::IDBControl2_SetReadOnly( BOOL bReadOnly )
{
    m_bReadOnly = bReadOnly;
	IDBControl2_OnSetReadOnly();

	RefreshAppearance();    
}

int  CDBControlImpl::GetFieldIndexInQuery()
{
	if( m_spDBaseDocument )
	{
		sptrIQueryObject ptrQ = GetActiveQuery();
		if( ptrQ == 0 )
			return false;

		int nFieldsCount = -1;
		ptrQ->GetFieldsCount( &nFieldsCount );

		int nField = -1;

		for( int i=0;i<nFieldsCount;i++ )
		{
			BSTR bstrTable = 0;
			BSTR bstrField = 0;
			if( S_OK != ptrQ->GetField( i, 0, &bstrTable, &bstrField ) )
				continue;

			CString strTable, strField;
			strTable = bstrTable;
			strField = bstrField;

			if( bstrTable )
				::SysFreeString( bstrTable );	bstrTable = 0;

			if( bstrField )
				::SysFreeString( bstrField );	bstrField = 0;

			if( IDBControl_GetTableName() == strTable && IDBControl_GetFieldName() == strField )
				return i;
		}
	}

	return -1;

}

	
bool CDBControlImpl::IsActiveDBaseField()
{
	if( m_spDBaseDocument )
	{
		int nField = GetFieldIndexInQuery();
		int nActiveField = -1;			

		sptrIQueryObject ptrQ = GetActiveQuery();
		if( ptrQ == 0 )
			return false;

		ptrQ->GetActiveField( &nActiveField );

		if( nActiveField == nField && nField != -1 )
				return true;		
	}

	return false;
}

void CDBControlImpl::SetAciveDBaseField()
{
	if( m_spDBaseDocument )
	{
		int nField = GetFieldIndexInQuery();

		sptrIQueryObject ptrQ = GetActiveQuery();
		if( ptrQ == 0 )
			return;

		if( nField != -1 )
			ptrQ->SetActiveField( nField );
	}
}


/*

sptrIBlankView CDBControlImpl::GetBlankView( CWnd* pCtrlWnd )
{
	if( !( pCtrlWnd && ::IsWindow( pCtrlWnd->GetSafeHwnd() ) ) )	
		return NULL;

	CWnd* pParent = pCtrlWnd->GetParent();

	if( !( pParent && ::IsWindow( pParent->GetSafeHwnd() ) ) )
		return NULL;

	sptrIApplication spApp( ::GetAppUnknown() );
	IUnknown* pUnkActiveDoc;
	spApp->GetActiveDocument( &pUnkActiveDoc );

	sptrIDocumentSite spDocSite( pUnkActiveDoc );
	pUnkActiveDoc->Release();

	LPOS lPos = 0;
	spDocSite->GetFirstViewPosition( &lPos );

	while( lPos )
	{			
		IUnknown	*punkView = 0;
		spDocSite->GetNextView( &punkView, &lPos );
		
		CWnd* pWnd = GetWindowFromUnknown( punkView );
		if( pWnd->GetSafeHwnd() == pParent->GetSafeHwnd() )
		{
			ASSERT( FALSE );
		}
		punkView->Release();
	}

	return pParent->GetControllingUnknown();
}
*/

void operator<<(CArchive &ar, FONTDESC &X)
{
	CString strName = X.lpstrName;

    ar << X.cbSizeofstruct;
	ar << strName;

    ar << X.cySize.Hi;
    ar << X.cySize.Lo;

    ar << X.sWeight;
    ar << X.sCharset;
    ar << X.fItalic;
    ar << X.fUnderline;
    ar << X.fStrikethrough;

} 

void operator>>(CArchive &ar, FONTDESC &X)
{
	CString strName;

    ar >> X.cbSizeofstruct;
	ar >> strName;
	X.lpstrName = strName.AllocSysString();

    ar >> X.cySize.Hi;
    ar >> X.cySize.Lo;

    ar >> X.sWeight;
    ar >> X.sCharset;
    ar >> X.fItalic;
    ar >> X.fUnderline;
    ar >> X.fStrikethrough;

}

void CDBControlImpl::init_default_font( FONTDESC* pdescr, const char* psz_def_section )
{
	if( !pdescr )	return;

	const char* psz_sect = psz_def_section ? psz_def_section : "\\DBControls\\Font";
	IUnknown* punk_app = ::GetAppUnknown();
	m_bstr_def_font = ::GetValueString( punk_app, psz_sect, "Face", "Arial" );

	pdescr->cbSizeofstruct	= sizeof( FONTDESC );
    pdescr->lpstrName		= m_bstr_def_font;
    pdescr->cySize.Lo		= 10000 * ::GetValueInt( punk_app, psz_sect, "Size", 12 );
	pdescr->cySize.Hi		= 0;
	pdescr->sCharset		= DEFAULT_CHARSET;
    pdescr->sWeight			= (short)::GetValueInt( punk_app, psz_sect, "Weight", FW_THIN );
    pdescr->fItalic			= ::GetValueInt( punk_app, psz_sect, "Italic", FALSE );
    pdescr->fUnderline		= ::GetValueInt( punk_app, psz_sect, "Underline", FALSE );
    pdescr->fStrikethrough	= ::GetValueInt( punk_app, psz_sect, "Strikethrough", FALSE );

}