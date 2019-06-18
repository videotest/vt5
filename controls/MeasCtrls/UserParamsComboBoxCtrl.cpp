// UserParamsComboBoxCtrl.cpp : Implementation of the CUserParamsComboBoxCtrl ActiveX Control class.

#include "stdafx.h"
#include "MeasCtrls.h"
#include "UserParamsComboBoxCtrl.h"
#include "WorksModePropPage.h"


#include "DrawParam.h"
#include "Params.h"

#include <comdef.h>
#include <msstkppg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CUserParamsComboBoxCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CUserParamsComboBoxCtrl, COleControl)
	ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_DRAWITEM_REFLECT()
    ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CUserParamsComboBoxCtrl, COleControl)
	DISP_STOCKPROP_FONT()
	DISP_STOCKPROP_FORECOLOR()
	DISP_STOCKPROP_BACKCOLOR()
	DISP_FUNCTION_ID(CUserParamsComboBoxCtrl, "SetItemData", dispidSetItemData, SetItemData, VT_BOOL, VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(CUserParamsComboBoxCtrl, "GetItemData", dispidGetItemData, GetItemData, VT_I4, VTS_I4)
	DISP_PROPERTY_EX_ID(CUserParamsComboBoxCtrl, "ShowUserName", dispidShowUserName, GetShowUserName, SetShowUserName, VT_BOOL)
	DISP_FUNCTION_ID(CUserParamsComboBoxCtrl, "GetItemCount", dispidGetItemCount, GetItemCount, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CUserParamsComboBoxCtrl, "GetParamSection", dispidGetParamSection, GetParamSection, VT_BSTR, VTS_I4)
	DISP_PROPERTY_EX_ID(CUserParamsComboBoxCtrl, "ActiveItem", dispidActiveItem, GetActiveItem, SetActiveItem, VT_I4)
	DISP_FUNCTION_ID(CUserParamsComboBoxCtrl, "Refresh", dispidRefresh, Refresh, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX_ID(CUserParamsComboBoxCtrl, "WorksMode", dispidWorksMode, GetWorksMode, SetWorksMode, VT_I4)
	DISP_FUNCTION_ID(CUserParamsComboBoxCtrl, "SetSortUserStrings", dispidSetSortUserStrings, SetSortUserStrings, VT_EMPTY, VTS_BOOL)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CUserParamsComboBoxCtrl, COleControl)
	EVENT_CUSTOM_ID("OnActiveItemChange", eventidOnActiveItemChange, OnActiveItemChange, VTS_I4)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CUserParamsComboBoxCtrl, 3)
	PROPPAGEID(CWorksModePropPage::guid)
	PROPPAGEID(CLSID_StockColorPage)
	PROPPAGEID(CLSID_StockFontPage)
END_PROPPAGEIDS(CUserParamsComboBoxCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CUserParamsComboBoxCtrl, "USERPARAMSCOMBOB.UserParamsComboBCtrl.1",
	0x7fcc3af7, 0x8988, 0x4d01, 0xaa, 0x89, 0xd0, 0x99, 0xb4, 0x4a, 0x8, 0x76)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CUserParamsComboBoxCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DUserParamsComboBox =
		{ 0xF587A927, 0x16A6, 0x4405, { 0xB9, 0xEC, 0xFE, 0xCD, 0x95, 0x15, 0xD2, 0x7C } };
const IID BASED_CODE IID_DUserParamsComboBoxEvents =
		{ 0x51863815, 0x27E0, 0x40CA, { 0xA6, 0x7A, 0xE6, 0x3A, 0xA9, 0xCB, 0x2F, 0x97 } };



// Control type information

static const DWORD BASED_CODE _dwUserParamsComboBoxOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CUserParamsComboBoxCtrl, IDS_USERPARAMSCOMBOBOX, _dwUserParamsComboBoxOleMisc)



// CUserParamsComboBoxCtrl::CUserParamsComboBoxCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CUserParamsComboBoxCtrl

BOOL CUserParamsComboBoxCtrl::CUserParamsComboBoxCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_USERPARAMSCOMBOBOX,
			IDB_USERPARAMSCOMBOBOX,
			afxRegApartmentThreading,
			_dwUserParamsComboBoxOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CUserParamsComboBoxCtrl::CUserParamsComboBoxCtrl - Constructor

CUserParamsComboBoxCtrl::CUserParamsComboBoxCtrl()
{
	InitializeIIDs(&IID_DUserParamsComboBox, &IID_DUserParamsComboBoxEvents);
	// TODO: Initialize your control's instance data here.
	init_default( );
	//m_vbShowUserName = VARIANT_TRUE;
	m_userSort=false;
}



// CUserParamsComboBoxCtrl::~CUserParamsComboBoxCtrl - Destructor

CUserParamsComboBoxCtrl::~CUserParamsComboBoxCtrl()
{
	// TODO: Cleanup your control's instance data here.
//	FreeParamsInfo( );
}



// CUserParamsComboBoxCtrl::OnDraw - Drawing function

void CUserParamsComboBoxCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	DoSuperclassPaint(pdc, rcBounds);
}



// CUserParamsComboBoxCtrl::DoPropExchange - Persistence support

void CUserParamsComboBoxCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CUserParamsComboBoxCtrl::OnResetState - Reset control to default state

void CUserParamsComboBoxCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
	init_default( );
}



// CUserParamsComboBoxCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CUserParamsComboBoxCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("COMBOBOX");
	cs.style |= ( WS_CHILD| WS_VSCROLL | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_NOINTEGRALHEIGHT | CBS_OWNERDRAWFIXED );
	return COleControl::PreCreateWindow(cs);
}



// CUserParamsComboBoxCtrl::IsSubclassedControl - This is a subclassed control

BOOL CUserParamsComboBoxCtrl::IsSubclassedControl()
{
	return TRUE;
}



// CUserParamsComboBoxCtrl::OnOcmCommand - Handle command messages

LRESULT CUserParamsComboBoxCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.
	switch(wNotifyCode)
	{
	    case CBN_SELCHANGE: 
			{
				if( GetSafeHwnd( ) )
				{
					CComboBox *pCombo = (CComboBox *) this;
					m_lActiveItem = pCombo->GetCurSel( );
                    // fire event
					OnActiveItemChange( m_lActiveItem );    // if have smthg to select
				} 
			}
			break;
		case CBN_DROPDOWN: 
			{
				CRect	rect, rcBounds;
				GetWindowRect( &rect );
				GetClientRect( &rcBounds );
				
				{
					int dy = 0;

					CComboBox *combo = (CComboBox *)this;

					int nSz = combo->GetCount();
					int nHeight = combo->GetItemHeight( nSz > 0 ? 0 : -1 );

					if( nSz > 10 )
						nSz = 10;

					dy = nHeight * nSz;
					dy += 2 * ::GetSystemMetrics( SM_CXEDGE );

					// [vanek] SBT:257,261 - 08.06.2004
					RECT rc_combo = rcBounds;
					ClientToScreen( &rc_combo );
					RECT rc_workarea = {0};
					::SystemParametersInfo( SPI_GETWORKAREA, 0, &rc_workarea, 0);
					dy = min( max( dy, nHeight ), rc_workarea.bottom - rc_combo.bottom );
                    rect.bottom += dy;
				}

				::SetWindowPos( GetSafeHwnd(), 0, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE|SWP_NOZORDER );
			}
			break;
		case CBN_CLOSEUP: 
			COleControl::Refresh();
			break;
	}


	return 0;
}

///////////////
void	CUserParamsComboBoxCtrl::init_default()
{
	// Font
	CFontHolder &font = InternalGetFont( );
	
	FONTDESC fd = {0};
	LOGFONT lf = {0};

	fd.cbSizeofstruct	= sizeof( fd );

	HFONT hFont = (HFONT) ::GetStockObject( ANSI_VAR_FONT );
    if( hFont && ::GetObject( hFont, sizeof( lf ), &lf ) )
	{
		CString strFontName = lf.lfFaceName;
        fd.lpstrName		= strFontName.AllocSysString( ); 			
		fd.cySize.Lo		= (unsigned long)(lf.lfHeight > 0 ? lf.lfHeight : -lf.lfHeight )
								* 10000L * 72L / 96L;
		fd.cySize.Hi		= 0;
        fd.sWeight			= (SHORT)lf.lfWeight;
		fd.sCharset			= (SHORT)lf.lfCharSet;
		fd.fItalic			= (BOOL)lf.lfItalic;
		fd.fUnderline		= (BOOL)lf.lfUnderline;
		fd.fStrikethrough	= (BOOL)lf.lfStrikeOut;
    }
	else
	{
		fd.cbSizeofstruct = sizeof( FONTDESC );
		fd.lpstrName = OLESTR("Arial");
		fd.cySize.Lo = 10000;
		fd.cySize.Hi = 0;
		fd.sWeight = FW_THIN;
		fd.sCharset = DEFAULT_CHARSET;
		fd.fItalic	= FALSE;
		fd.fUnderline = FALSE;
		fd.fStrikethrough = FALSE; 
	}

    font.ReleaseFont( );
	font.InitializeFont( &fd );

	m_vbShowUserName = VARIANT_FALSE;
	m_lActiveItem = -1;
	m_lWorksMode = MIN_WORKSMODE;
}

void	CUserParamsComboBoxCtrl::load_params_section( )
{								 
	//m_ItemSectionList.clear( );
	if( !GetSafeHwnd( ) )
		return;
	
	m_mapKeyToSection.RemoveAll( );
	m_KeyList.clear( );

	CComboBox *pCombo = 0;	
	pCombo = (CComboBox*) this;
	pCombo->ResetContent( );
	
	BSTR	*pbstrSections = 0;
	long	*plKeys = 0;

	long lSize = 0;
	lSize = get_sections_by_worksmode( m_lWorksMode, &pbstrSections, &plKeys );
	if( lSize > 0 )
	{
		for( long l = 0; l < lSize; l++ )
		{
			m_KeyList.add_tail( *(plKeys + l) );
			m_mapKeyToSection.SetAt( *(plKeys + l), CString( *(pbstrSections + l) ) );
			pCombo->AddString( _T("") );
		}

		for( long l = 0; l < lSize; l++ )
		{
			if( *(pbstrSections + l) )
			{
				::SysFreeString( *(pbstrSections + l) );
				*(pbstrSections + l) = 0;
			}
		}
	}

	if( pbstrSections )
	{
		delete[] pbstrSections;
		pbstrSections = 0;
	}

	if( plKeys )
	{
		delete[] plKeys;
		plKeys = 0;
	}
	
	if(m_userSort)
		SetSortUserStrings(m_userSort);

	pCombo->SetCurSel( m_lActiveItem );
}

void	CUserParamsComboBoxCtrl::create_bk_brush( )
{
    if( m_brushBackground.GetSafeHandle() )    
		m_brushBackground.DeleteObject( );
    m_brushBackground.CreateSolidBrush( TranslateColor( GetBackColor() ) );            
}

HBRUSH	CUserParamsComboBoxCtrl::get_bk_brush( )
{
    if( !m_brushBackground.GetSafeHandle() )
		create_bk_brush( );

	return m_brushBackground;
}

// CUserParamsComboBoxCtrl message handlers
void CUserParamsComboBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	::SendMessage(GetSafeHwnd(),CB_SHOWDROPDOWN,(WPARAM)true,NULL);
}

VARIANT_BOOL CUserParamsComboBoxCtrl::SetItemData(LONG nIdx, LONG lData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	if( GetSafeHwnd( ) )
	{
		CComboBox *pCombo = (CComboBox *) this;
		return ( CB_ERR != pCombo->SetItemData( nIdx, lData ) ) ? VARIANT_TRUE : VARIANT_FALSE;
	}
	else	
		return VARIANT_FALSE;
}

LONG CUserParamsComboBoxCtrl::GetItemData(LONG nIdx)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	if( GetSafeHwnd( ) )
	{
		CComboBox *pCombo = (CComboBox *) this;
		return (long) pCombo->GetItemData( nIdx );
	}
	else
		return 0;
}

VARIANT_BOOL CUserParamsComboBoxCtrl::GetShowUserName(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
    return m_vbShowUserName;
}

void CUserParamsComboBoxCtrl::SetShowUserName(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your property handler code here
	m_vbShowUserName = newVal;							
	SetModifiedFlag();
}

void CUserParamsComboBoxCtrl::DrawItem(  LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	HFONT hFont = 0, hTempFont = 0;
	char *pOutStr = 0;

	if(lpDrawItemStruct->itemID != -1)
	{
		long lKey = m_KeyList.get( m_KeyList[ lpDrawItemStruct->itemID ] );
		CString strSection;
		if( !m_mapKeyToSection.Lookup( lKey, strSection ) )
			return;

		if( (m_vbShowUserName == VARIANT_FALSE) )
		{ // greek symbol
			long lGrSmb = ::GetValueIntWithoutSet( ::GetAppUnknown( ), strSection.GetBuffer( strSection.GetLength( ) ), sGreekSymbol, -1 );
			strSection.ReleaseBuffer( );
			if( lGrSmb != -1 )
			{
				CFontHolder& font = InternalGetFont();
				if ( !font.m_pFont )
					return;

				hFont = font.GetFontHandle();
				if( !hFont )
					return;

				LOGFONT lf = {0};
				if( !::GetObject( hFont, sizeof(LOGFONT), &lf ) )
					return;

				lf.lfCharSet = GREEK_CHARSET;
				hTempFont = ::CreateFontIndirect( &lf );
				if( !hTempFont )
					return;

				hTempFont = (HFONT) ::SelectObject( lpDrawItemStruct->hDC, hTempFont );
				pOutStr = new char[2];
				pOutStr[0] = (char)lGrSmb;
				pOutStr[1] = 0;
			}
			else
			{	// get Graphics
				CString value = GetValueStringWithoutSet( ::GetAppUnknown( ), strSection.GetBuffer( strSection.GetLength( ) ), sGraphics, "" );
				pOutStr = new char[ value.GetLength( ) + 1 ];
				char *pstemp = 0;
				pstemp = value.GetBuffer( value.GetLength( ) );
				strcpy( pOutStr, pstemp );
				value.ReleaseBuffer( );
				pstemp = 0;
			}
		}
		else
		{
			CString value = GetValueStringWithoutSet( ::GetAppUnknown( ), strSection.GetBuffer( strSection.GetLength( ) ), sUserName, "" );
			pOutStr = new char[ value.GetLength( ) + 1 ];
			char *pstemp = 0;
			pstemp = value.GetBuffer( value.GetLength( ) );
			strcpy( pOutStr, pstemp );
			value.ReleaseBuffer( );
			pstemp = 0;
		}
	}
	else
	{
		pOutStr = new char[1];
		pOutStr[0] = 0;
	}
	
	// Calculate the vertical and horizontal position. 
	TEXTMETRIC tm = {0};
    GetTextMetrics(lpDrawItemStruct->hDC, &tm); 
    int y = (lpDrawItemStruct->rcItem.bottom + lpDrawItemStruct->rcItem.top - tm.tmHeight) / 2,
		x = LOWORD(GetDialogBaseUnits()) / 4; 
 

	COLORREF	clrBack = 0, clrFore = 0;

	clrFore = SetTextColor(lpDrawItemStruct->hDC, 
                lpDrawItemStruct->itemState & ODS_SELECTED ? GetSysColor( COLOR_HIGHLIGHTTEXT ) :
				TranslateColor( GetForeColor( ) ) ); 
 
    clrBack = SetBkColor(lpDrawItemStruct->hDC, lpDrawItemStruct->itemState & ODS_SELECTED ?
                GetSysColor( COLOR_HIGHLIGHT ) : TranslateColor( GetBackColor() ) ); 

	if(m_userSort && m_tStrings.GetCount()>0 && lpDrawItemStruct->itemID != -1)
	{
		if( pOutStr )	 
		{
	        delete[] pOutStr;
			pOutStr = 0;
		}
		CString value = m_tStrings.GetAt(lpDrawItemStruct->itemID);
		pOutStr = new char[ value.GetLength( ) + 1 ];
		char *pstemp = 0;
		pstemp = value.GetBuffer( value.GetLength( ) );
		strcpy( pOutStr, pstemp );
		value.ReleaseBuffer( );
		pstemp = 0;
	}

	ExtTextOut( lpDrawItemStruct->hDC, 2 * x, y, ETO_CLIPPED | ETO_OPAQUE, &lpDrawItemStruct->rcItem,
				pOutStr, lstrlen( pOutStr ), 0 );

	if( pOutStr )	 
	{
        delete[] pOutStr;
		pOutStr = 0;
	}
	
	if( hTempFont )
	{
		::DeleteObject( ::SelectObject( lpDrawItemStruct->hDC, hTempFont ) );
		hTempFont = 0;
	}

	::SetTextColor( lpDrawItemStruct->hDC, clrBack );
	::SetTextColor( lpDrawItemStruct->hDC, clrFore );

	// If the item has the focus, draw focus rectangle. 
     if (lpDrawItemStruct->itemState & ODS_FOCUS) 
       DrawFocusRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem); 
}

HBRUSH CUserParamsComboBoxCtrl::CtlColor ( CDC* pDC, UINT nCtlColor )
{
	return get_bk_brush( );
}

HBRUSH CUserParamsComboBoxCtrl::OnCtlColor ( CDC* pDC, CWnd *pwnd, UINT nCtlColor )
{
    return get_bk_brush( );
}

int CUserParamsComboBoxCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	load_params_section( );
	return 0;
}

void CUserParamsComboBoxCtrl::OnFontChanged()
{
	// TODO: Add your specialized code here and/or call the base class

	COleControl::OnFontChanged();	 

	CFontHolder& font = InternalGetFont();

	if (font.m_pFont != NULL)
	{
		HFONT hFont = font.GetFontHandle();
		if( hFont )
		{
			CFont* pFont = CFont::FromHandle( hFont );
			if( pFont )
			{
				LOGFONT lf;
				::ZeroMemory( &lf, sizeof(LOGFONT) );
				pFont->GetLogFont( &lf );
				
				if( GetSafeHwnd() )
				{
					TEXTMETRIC tm;
					font.QueryTextMetrics( &tm );
					SendMessage( CB_SETITEMHEIGHT, 0, /*abs(lf.lfHeight )*/ tm.tmHeight + tm.tmExternalLeading + 1 );
					SendMessage( CB_SETITEMHEIGHT, -1, /*abs(lf.lfHeight)*/ tm.tmHeight + tm.tmExternalLeading + 1 );
				}
			}
		}
	} 
}

void CUserParamsComboBoxCtrl::OnBackColorChanged()
{
    COleControl::OnBackColorChanged( );
	create_bk_brush( );
}

LONG CUserParamsComboBoxCtrl::GetItemCount(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	if( !GetSafeHwnd( ) )
        return 0;

	CComboBox *pCombo = ( CComboBox* ) this;
	return pCombo->GetCount( );
}

BSTR CUserParamsComboBoxCtrl::GetParamSection(LONG nIdx)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;
	strResult.Empty( );

	if( ( nIdx >= 0 ) && ( nIdx < GetItemCount( ) ) )
	{
		long lPos = m_KeyList[ nIdx ];
		if( lPos )
			m_mapKeyToSection.Lookup( m_KeyList.get( lPos ), strResult );
	}

	return strResult.AllocSysString();
}

LONG CUserParamsComboBoxCtrl::GetActiveItem(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	return m_lActiveItem;
}

void CUserParamsComboBoxCtrl::SetActiveItem(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your property handler code here
	if( GetSafeHwnd( ) )
	{
		CComboBox *pCombo = (CComboBox *) this;
		m_lActiveItem = pCombo->SetCurSel( newVal );
	}
	else
		m_lActiveItem = -1;
    
	SetModifiedFlag();
}

void CUserParamsComboBoxCtrl::Refresh(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	load_params_section( );
}

void CUserParamsComboBoxCtrl::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		int iVer = 2;
		ar << iVer;
		ar << m_vbShowUserName;
		ar << m_lActiveItem;
		ar << m_lWorksMode;
		ar << m_userSort;
	}
	else
	{	// loading code
		int iVer = 0;
		ar >> iVer;
		ar >> m_vbShowUserName;
		ar >> m_lActiveItem;
		ar >> m_lWorksMode;

		load_params_section( );

		if(iVer>=2)
		{
			ar >> m_userSort;
			SetSortUserStrings(m_userSort);
		}
	}

	COleControl::Serialize( ar );
}

LONG CUserParamsComboBoxCtrl::GetWorksMode(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_lWorksMode;
}

void CUserParamsComboBoxCtrl::SetWorksMode(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_lWorksMode != newVal )
	{
		m_lWorksMode = newVal;
		load_params_section( );
		SetModifiedFlag();
	}
}

void CUserParamsComboBoxCtrl::SetSortUserStrings(BOOL sStr) 
{
	if(sStr && m_userSort!=sStr)
	{
		CComboBox *combo = (CComboBox*)this;
		if(combo->GetCount()>0)
		{
			int i, j, sLength;
			CString s;
			CStringArray m_bastr, temp;
			int cur=0;
			LPCTSTR NewVal;
			for(i=0;i<combo->GetCount();i++)
			{
				long lKey = m_KeyList.get( m_KeyList[ i ] );
				if( !m_mapKeyToSection.Lookup( lKey, s ) )
					return;
				s.ReleaseBuffer( );
				CString value = GetValueStringWithoutSet( ::GetAppUnknown( ), s.GetBuffer( s.GetLength( ) ), sGraphics, "" );
				value.ReleaseBuffer();
				m_bastr.InsertAt(i,value.GetBuffer(0));
			}
			m_tStrings.RemoveAll();
			for(j=0;j<m_bastr.GetCount();j++)
			{
				NewVal=m_bastr.GetAt(j);
				if(j==0)
					m_tStrings.InsertAt(j,NewVal);
				else
				{
					CStringArray temp;
					int cur=0;
	
					for(i=0;i<m_tStrings.GetCount();i++)
						temp.InsertAt(i,m_tStrings.GetAt(i));
			
					for(i=0;i<m_tStrings.GetCount();i++)
					{
						cur=strcmp(NewVal,m_tStrings.GetAt(i));
						if(cur<0)
						{
							int k;
							temp.SetAt(i,NewVal);
							for(k=i;k<m_tStrings.GetCount()-1;k++)
								temp.SetAt(k+1,m_tStrings.GetAt(k));
							temp.InsertAt(k+1,m_tStrings.GetAt(k));
							break;
						}
						else
							if(i==m_tStrings.GetCount()-1)
								temp.InsertAt(i+1,NewVal);
					}
					m_tStrings.RemoveAll();
					//combo->ResetContent();
					for(i=0;i<temp.GetCount();i++)
					{
						m_tStrings.InsertAt(i,temp.GetAt(i));
					//	combo->InsertString(i,temp.GetAt(i));
					}
				}
			}
		}
	}
	else
		if(!sStr)
		{
			CComboBox *combo = (CComboBox*)this;
			if(combo->GetCount()>0)
			{
				CString s;
				m_tStrings.RemoveAll();
				for(int i=0;i<combo->GetCount();i++)
				{
					long lKey = m_KeyList.get( m_KeyList[ i ] );
					if( !m_mapKeyToSection.Lookup( lKey, s ) )
						return;
					s.ReleaseBuffer( );
					CString value = GetValueStringWithoutSet( ::GetAppUnknown( ), s.GetBuffer( s.GetLength( ) ), sGraphics, "" );
					value.ReleaseBuffer();
					m_tStrings.InsertAt(i,value.GetBuffer(0));
				}
			}
		}
	m_userSort=sStr;
}