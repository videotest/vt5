// FilterPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "FilterPropPage.h"

#include "FilterCondition.h"

#include "PropPage.h"

/////////////////////////////////////////////////////////////////////////////
// CFilterListBox
/////////////////////////////////////////////////////////////////////////////
CFilterListBox::CFilterListBox()
{
	m_nLastActiveSelection = -1;

	LOGFONT lf;
	::ZeroMemory( &lf, sizeof(lf) );

	CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) );

	if( pFont )
	{
		pFont->GetLogFont( &lf );

		lf.lfWeight = FW_NORMAL;
		m_fontRegular.CreateFontIndirect( &lf );

		lf.lfWeight	= FW_BOLD;
		m_fontBold.CreateFontIndirect( &lf );
	}	


	//Create reserved word array
	CString strReservedWords;
	strReservedWords.LoadString( IDS_RESERVED_WORD_ARRAY );	

	int nReservLen = strReservedWords.GetLength();

	CString strCurrent;
	
	strCurrent.Empty();

	for( int i=0;i<nReservLen;i++ )
	{	
		if( strReservedWords[i] != '\n' )
		{
			strCurrent += strReservedWords[i];
		}
		else
		{
			m_arReservList.Add( strCurrent );
			strCurrent.Empty();
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
CFilterListBox::~CFilterListBox()
{
	m_arReservList.RemoveAll( );
}

/////////////////////////////////////////////////////////////////////////////
void CFilterListBox::StoreIndex()
{
	if( !GetSafeHwnd() ) 
		return;

	m_nLastActiveSelection = GetCurSel();	
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterListBox::IsValidIndex( int nIndex )
{
	if( !GetSafeHwnd() ) 
		return false;

	int nCount = GetCount();

	if( nIndex >= 0 && nIndex < nCount )
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////
void CFilterListBox::SetToLastSelection()
{
	if( !GetSafeHwnd() ) 
		return;

	int nSel = GetCurSel();

	if( IsValidIndex( nSel ) )
		return;

	if( IsValidIndex( m_nLastActiveSelection ) )
	{
		SetCurSel( m_nLastActiveSelection );
	}
	else
	{
		if( IsValidIndex( m_nLastActiveSelection - 1 ) )
		{
			m_nLastActiveSelection--;
			SetCurSel( m_nLastActiveSelection );
		}
		else
		if( IsValidIndex( m_nLastActiveSelection + 1 ) )
		{
			m_nLastActiveSelection++;
			SetCurSel( m_nLastActiveSelection + 1 );
		}
		else
		{
			m_nLastActiveSelection = 0;
			SetCurSel( 0 );
		}
	}

}


BEGIN_MESSAGE_MAP(CFilterListBox, CListBox)
	//{{AFX_MSG_MAP(CFilterListBox)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CFilterListBox::DrawItem( LPDRAWITEMSTRUCT lpDIS )
{
	if( !lpDIS )
		return;

	CDC* pDC = CDC::FromHandle( lpDIS->hDC );
	if( !pDC )
		return;

	COLORREF clNormalText, clNormalTextBk;
	COLORREF clSelectedText, clSelectedTextBk;


	clNormalText		= ::GetSysColor( COLOR_WINDOWTEXT );
	clNormalTextBk		= ::GetSysColor( COLOR_WINDOW );

	clSelectedText		= ::GetSysColor( COLOR_HIGHLIGHTTEXT );
	clSelectedTextBk	= ::GetSysColor( COLOR_HIGHLIGHT ); 

	bool bDrawFocused = false;

	CString strText;
	if( lpDIS->itemID >= 0 && lpDIS->itemID < GetCount() )
		GetText( lpDIS->itemID, strText );

	CRect rcBounds	= lpDIS->rcItem;
	
	CRect rcFocused	= rcBounds;
	CRect rcFill	= rcBounds;
	CRect rcText	= rcBounds;
	rcText.DeflateRect( 1, 1, 1, 1 );

	COLORREF clText, clBk;
	
	clText	= clNormalText;
	clBk	= clNormalTextBk;

	int nItemState = lpDIS->itemState;


	if( lpDIS->itemState & ODS_SELECTED )
	{
		clText	= clSelectedText;
		clBk	= clSelectedTextBk;
		bDrawFocused = true;
	}	

	if( lpDIS->itemState & ODS_FOCUS )
	{
		bDrawFocused = true;
	}

	//Draw background
	pDC->FillRect( &rcFill, &CBrush(clBk) );

	//Draw focused rect
	//if( bDrawFocused )
		//pDC->DrawFocusRect( rcFocused );


	//Draw text
	int nBkMode = pDC->SetBkMode(TRANSPARENT);
	
	COLORREF clOldText	= pDC->SetTextColor( clText );

	DrawText( pDC, strText, rcText );	
	
	pDC->SetTextColor( clOldText );
	
	pDC->SetBkMode( nBkMode );


}

/////////////////////////////////////////////////////////////////////////////
void CFilterListBox::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	if( !lpMeasureItemStruct )
		return;

	{
		CClientDC dc(this);
		TEXTMETRIC tm;
		CFont* pOldFont = dc.SelectObject( &m_fontBold );
		dc.GetTextMetrics( &tm );					
		dc.SelectObject( pOldFont );

		lpMeasureItemStruct->itemHeight = tm.tmHeight;


		if( lpMeasureItemStruct->itemID != -1 )
		{
			CString strItem;
			GetText( lpMeasureItemStruct->itemID, strItem );
			lpMeasureItemStruct->itemWidth = GetStringWidth( strItem );
		}
	}	
}

/////////////////////////////////////////////////////////////////////////////
int CFilterListBox::GetStringWidth( CString strText )
{
	int nResult = 0;
	CArray<CString, CString> arStr;
	GetStringArray( strText, arStr );
	for( int i=0;i<arStr.GetSize();i++ )
	{
		nResult += GetWordWidth( arStr[i] );
	}
	arStr.RemoveAll();

	return nResult;
}

/////////////////////////////////////////////////////////////////////////////
int CFilterListBox::GetWordWidth( CString strWord )
{
	CClientDC dc(this);
	CRect rcText = NORECT;
	CFont* pOldFont = NULL;

	if( IsBoldWord( strWord ) )
		pOldFont = dc.SelectObject( &m_fontBold );
	else
		pOldFont = dc.SelectObject( &m_fontRegular );

	dc.DrawText( strWord, &rcText, DT_CALCRECT | DT_LEFT );	
	
	dc.SelectObject( pOldFont );		

	return rcText.Width() + 1;		
}

/////////////////////////////////////////////////////////////////////////////
void CFilterListBox::DrawText( CDC* pDC, CString strText, CRect rcText )
{
	
	CArray<CString, CString> arStr;
	GetStringArray( strText, arStr );
	int nCurX = 0;

	for( int i=0;i<arStr.GetSize();i++ )
	{
		CString str = arStr[i];
		CFont* pOldFont = NULL;
		
		if( IsBoldWord( arStr[i] ) )
			pOldFont = pDC->SelectObject( &m_fontBold );
		else
			pOldFont = pDC->SelectObject( &m_fontRegular );
		
		int nWidth = GetWordWidth( arStr[i] );				

		CRect rcOut = CRect( nCurX, rcText.top, nCurX + nWidth, rcText.bottom );

		pDC->DrawText( arStr[i], &rcOut, DT_LEFT | DT_SINGLELINE | DT_VCENTER );
		
		nCurX += nWidth;

		pDC->SelectObject( pOldFont );		
	}

	arStr.RemoveAll();
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterListBox::GetStringArray( CString strSource, CArray<CString, CString>& arStr )
{
	int nLen = strSource.GetLength( );
	CString strCurString;
	strCurString.Empty();
	
	for( int i=0;i<nLen;i++ )
	{
		if( strSource[i] != ' ' )
		{
			strCurString += strSource[i];
		}// = ""
		else
		{
			if( strCurString.IsEmpty() )			
			{
				arStr.Add( " " );
			}
			else
			{
				arStr.Add( strCurString );
				strCurString.Empty();
				if( i != nLen - 1 )
					arStr.Add( " " );
			}
		}
	}

	if( !strCurString.IsEmpty() )
		arStr.Add( strCurString );
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterListBox::IsBoldWord( CString strWord )
{	
	bool bFound = false;

	for( int i=0;i<m_arReservList.GetSize();i++ )
	{
		if( m_arReservList[i] == strWord )
			bFound = true;
	}		

	return !bFound;	
}

/////////////////////////////////////////////////////////////////////////////
// CFilterPropPage
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CFilterPropPage, CDialog);



BEGIN_INTERFACE_MAP(CFilterPropPage, CPropertyPageBase)
	INTERFACE_PART(CFilterPropPage, IID_IDBFilterPage, DBFilterPage)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CFilterPropPage, DBFilterPage);



// {AF27A49D-6A12-45f0-9AED-5225855E4AD9}
GUARD_IMPLEMENT_OLECREATE(CFilterPropPage, szFilterPropPageProgID, 
0xaf27a49d, 0x6a12, 0x45f0, 0x9a, 0xed, 0x52, 0x25, 0x85, 0x5e, 0x4a, 0xd9);


CFilterPropPage::CFilterPropPage()
	: CPropertyPageBase( CFilterPropPage::IDD )
{
	_OleLockApp( this );
	m_sName = c_szCFilterPropPage;
	m_sUserName.LoadString( IDS_FILTER_PROPERTY_PAGE );
	//{{AFX_DATA_INIT(CFilterPropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CFilterPropPage::~CFilterPropPage()
{
	_OleUnlockApp( this );
}


void CFilterPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterPropPage)
	DDX_Control(pDX, IDC_CONDITION, m_ctrlContext);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilterPropPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CFilterPropPage)	
	ON_LBN_SELCHANGE(IDC_CONDITION, OnSelchangeCondition)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_LEFT_BRACKET, OnLeftBracket)
	ON_BN_CLICKED(IDC_RIGHT_BRACKET, OnRightBracket)
	ON_BN_CLICKED(IDC_RESET_BRACKETS, OnResetBrackets)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterPropPage message handlers

BOOL CFilterPropPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	BuildAppearance();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterPropPage::BuildAppearance()
{	
	if( !::IsWindow( m_ctrlContext.GetSafeHwnd() ) ) 
		return false;		

	SetVisibleButton();

	m_ctrlContext.StoreIndex();

	m_ctrlContext.ResetContent();

	IDBaseFilterHolderPtr ptrFH = GetActiveFilterHolder();

	if( ptrFH == NULL )
		return false;

	BSTR bstrFilter = NULL;
	if( S_OK != ptrFH->GetActiveFilter( &bstrFilter ) )
		return false;

	_bstr_t _bstrFilter( bstrFilter );

	if( bstrFilter )
		::SysFreeString( bstrFilter );

	BSTR bstrHeader = NULL;	
	if( S_OK != ptrFH->GetFilterForOrganizer( _bstrFilter, &bstrHeader) )
		return false;

	
	//m_ctrlContext.LockWindowUpdate();
	
	m_ctrlContext.AddString( (LPCTSTR)CString(bstrHeader) );


	if( bstrHeader )
		::SysFreeString( bstrHeader );

	int nCount = -1;
	ptrFH->GetFilterConditionCount( _bstrFilter, &nCount );
	for( int i=0;i<nCount;i++ )
	{
		BSTR bstrCond = NULL;
		if( S_OK != ptrFH->GetFilterConditionForOrganizer( 
			_bstrFilter, i, &bstrCond) )
			continue;

		m_ctrlContext.AddString( CString(bstrCond) );		

		if( bstrCond )
			::SysFreeString( bstrCond );		
	}				 		


	m_ctrlContext.SelectString( 0, m_strLastActiveString );
	

	m_ctrlContext.SetToLastSelection();	


	SetVisibleButton();

	//m_ctrlContext.UnlockWindowUpdate();

	SetListoxAutoExtent( &m_ctrlContext );	

	return true;
}

/////////////////////////////////////////////////////////////////////////////
IDBaseFilterHolderPtr CFilterPropPage::GetActiveFilterHolder()
{
	
	
	sptrIApplication sptrApp( ::GetAppUnknown() );
	if( sptrApp == NULL )
		return NULL;

	IUnknown* punkDoc = NULL;
	sptrApp->GetActiveDocument( &punkDoc );
	if( punkDoc == NULL )
		return NULL;

	IDBaseFilterHolderPtr ptr( punkDoc );		
	punkDoc->Release( ); 

	return ptr;
}

/////////////////////////////////////////////////////////////////////////////
void CFilterPropPage::SetVisibleButton()
{

	if( !::IsWindow( m_ctrlContext.GetSafeHwnd() ) ) 
		return;

	bool bFilterAvailable = IsFilterAvailable();
	int nCondIndex = GetActiveConditionIndex();

	if( !bFilterAvailable )
	{			
		GetDlgItem(IDC_NEW)->EnableWindow( FALSE );
		GetDlgItem(IDC_EDIT)->EnableWindow( FALSE );
		GetDlgItem(IDC_COPY)->EnableWindow( FALSE );
		GetDlgItem(IDC_REMOVE)->EnableWindow( FALSE );
		GetDlgItem(IDC_LEFT_BRACKET)->EnableWindow( FALSE );
		GetDlgItem(IDC_RIGHT_BRACKET)->EnableWindow( FALSE );
		GetDlgItem(IDC_RESET_BRACKETS)->EnableWindow( FALSE );
	}
	else
	{
		GetDlgItem(IDC_NEW)->EnableWindow( TRUE );

		if( nCondIndex != -1 )
		{
			GetDlgItem(IDC_EDIT)->EnableWindow( TRUE );
			GetDlgItem(IDC_COPY)->EnableWindow( TRUE );
			GetDlgItem(IDC_REMOVE)->EnableWindow( TRUE );
			GetDlgItem(IDC_LEFT_BRACKET)->EnableWindow( TRUE );
			GetDlgItem(IDC_RIGHT_BRACKET)->EnableWindow( TRUE );
			GetDlgItem(IDC_RESET_BRACKETS)->EnableWindow( TRUE );
		}
		else
		{
			GetDlgItem(IDC_EDIT)->EnableWindow( FALSE );
			GetDlgItem(IDC_COPY)->EnableWindow( FALSE );
			GetDlgItem(IDC_REMOVE)->EnableWindow( FALSE );
			GetDlgItem(IDC_LEFT_BRACKET)->EnableWindow( FALSE );
			GetDlgItem(IDC_RIGHT_BRACKET)->EnableWindow( FALSE );
			GetDlgItem(IDC_RESET_BRACKETS)->EnableWindow( FALSE );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterPropPage::IsFilterAvailable()
{
		
	if( !::IsWindow( m_ctrlContext.GetSafeHwnd() ) ) 
		return false;

	IDBaseFilterHolderPtr ptrFH = GetActiveFilterHolder();	
	if( ptrFH == NULL )
		return false;

	BSTR bstrFilter = NULL;
	if( S_OK != ptrFH->GetActiveFilter( &bstrFilter ) )
		return false;

	if( bstrFilter )
		::SysFreeString( bstrFilter );

	return true;

}


/////////////////////////////////////////////////////////////////////////////
int CFilterPropPage::GetActiveConditionIndex()
{
	if( !::IsWindow( m_ctrlContext.GetSafeHwnd() ) ) 
		return -1;

	int nItemCount = m_ctrlContext.GetCount();
	if( nItemCount < 2 )
		return -1;

	int nCurSel = m_ctrlContext.GetCurSel();
	
	if( nCurSel == -1 )
		return -1;

	int nCondIndex = nCurSel - 1;

	IDBaseFilterHolderPtr ptrFH = GetActiveFilterHolder();	
	if( ptrFH == NULL )
		return false;

	BSTR bstrFilter = NULL;
	if( S_OK != ptrFH->GetActiveFilter( &bstrFilter ) )
		return -1;

	_bstr_t _bstrFilter( bstrFilter );

	if( bstrFilter )
		::SysFreeString( bstrFilter );


	int nCondCount = -1;
	if( S_OK != ptrFH->GetFilterConditionCount( _bstrFilter, &nCondCount ) )
		return -1;

	if( nCondIndex < 0 || nCondIndex >= nCondCount )
		return -1;

	return nCondIndex;	
}


/////////////////////////////////////////////////////////////////////////////
void CFilterPropPage::OnSelchangeCondition() 
{	
	m_ctrlContext.StoreIndex();

	SetVisibleButton();
	if( m_ctrlContext.GetCurSel() != -1 )
		m_ctrlContext.GetText( m_ctrlContext.GetCurSel(), m_strLastActiveString );
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterPropPage::OnNew() 
{

	//SetVisibleButton();

	IDBaseFilterHolderPtr ptrFH = GetActiveFilterHolder();	
	if( ptrFH == NULL )
		return;

	BSTR bstrFilter = NULL;
	if( S_OK != ptrFH->GetActiveFilter( &bstrFilter ) )
		return;


	CFilterConditionDlg dlg;	
	dlg.SetFilterHolder( ptrFH );
	if( dlg.DoModal() == IDOK )
	{				

		ptrFH->AddFilterCondition( bstrFilter, 
									_bstr_t((LPCSTR)dlg.m_strTable), 
									_bstr_t((LPCSTR)dlg.m_strField), 
									_bstr_t((LPCSTR)dlg.m_strCondition), 
									_bstr_t((LPCSTR)dlg.m_strValue), 
									_bstr_t((LPCSTR)dlg.m_strValueTo), 
									!dlg.m_bOrUnion, 
									0, 0
									);

		
	}

	if( bstrFilter )
		::SysFreeString( bstrFilter );

	::SetModifiedFlagToDoc( ptrFH );	

	SetListoxAutoExtent( &m_ctrlContext );

}

/////////////////////////////////////////////////////////////////////////////
void CFilterPropPage::OnEdit() 
{
	IDBaseFilterHolderPtr ptrFH = GetActiveFilterHolder();	
	if( ptrFH == NULL )
		return;

	int nFilterIndex = GetActiveConditionIndex();
	if( nFilterIndex < 0 )
		return;


	BSTR bstrFilter = NULL;
	if( S_OK != ptrFH->GetActiveFilter( &bstrFilter ) )
		return;

	BSTR bstrTable, bstrField, bstrCondition, bstrValue, bstrValueTo;
	BOOL bAndUnion;
	int nLeftBracket, nRightBracket;

	if( S_OK != ptrFH->GetFilterCondition(bstrFilter, nFilterIndex,
						&bstrTable, &bstrField, 
						&bstrCondition, 
						&bstrValue, &bstrValueTo,
						&bAndUnion,
						&nLeftBracket, &nRightBracket					
						) )
	{
		if( bstrFilter )
			::SysFreeString( bstrFilter );
		return;
	}


	CFilterConditionDlg dlg;	
	dlg.SetFilterHolder( ptrFH );
	
	dlg.m_strTable	= bstrTable;
	dlg.m_strField	= bstrField;

	dlg.m_strValue		= bstrValue;
	dlg.m_strValueTo	= bstrValueTo;

	dlg.m_strCondition	= bstrCondition;
	
	dlg.m_bOrUnion		= !bAndUnion;


	if( bstrTable )
		::SysFreeString( bstrTable );

	if( bstrField )
		::SysFreeString( bstrField );

	if( bstrCondition )
		::SysFreeString( bstrCondition );

	if( bstrValue )
		::SysFreeString( bstrValue );

	if( bstrValueTo )
		::SysFreeString( bstrValueTo );

	if( dlg.DoModal() == IDOK )
	{

		ptrFH->EditFilterCondition( bstrFilter, nFilterIndex,
									_bstr_t((LPCSTR)dlg.m_strTable), 
									_bstr_t((LPCSTR)dlg.m_strField), 
									_bstr_t((LPCSTR)dlg.m_strCondition), 
									_bstr_t((LPCSTR)dlg.m_strValue), 
									_bstr_t((LPCSTR)dlg.m_strValueTo), 
									!dlg.m_bOrUnion, 
									nLeftBracket, nRightBracket
									);

		::SetModifiedFlagToDoc( ptrFH );
		
	}

	if( bstrFilter )
		::SysFreeString( bstrFilter );

	SetListoxAutoExtent( &m_ctrlContext );
}

/////////////////////////////////////////////////////////////////////////////
void CFilterPropPage::OnCopy() 
{
	IDBaseFilterHolderPtr ptrFH = GetActiveFilterHolder();	
	if( ptrFH == NULL )
		return;

	int nFilterIndex = GetActiveConditionIndex();
	if( nFilterIndex < 0 )
		return;


	BSTR bstrFilter = NULL;
	if( S_OK != ptrFH->GetActiveFilter( &bstrFilter ) )
		return;

	BSTR bstrTable, bstrField, bstrCondition, bstrValue, bstrValueTo;
	BOOL bAndUnion;
	int nLeftBracket, nRightBracket;

	if( S_OK != ptrFH->GetFilterCondition(bstrFilter, nFilterIndex,
						&bstrTable, &bstrField, 
						&bstrCondition, 
						&bstrValue, &bstrValueTo,
						&bAndUnion,
						&nLeftBracket, &nRightBracket					
						) )
	{
		if( bstrFilter )
			::SysFreeString( bstrFilter );
		return;
	}

	ptrFH->AddFilterCondition( bstrFilter,
								bstrTable, 
								bstrField, 
								bstrCondition, 
								bstrValue, 
								bstrValueTo, 
								bAndUnion, 
								nLeftBracket, nRightBracket
					  			);		
	

	if( bstrTable )
		::SysFreeString( bstrTable );

	if( bstrField )
		::SysFreeString( bstrField );

	if( bstrCondition )
		::SysFreeString( bstrCondition );

	if( bstrValue )
		::SysFreeString( bstrValue );

	if( bstrValueTo )
		::SysFreeString( bstrValueTo );


	if( bstrFilter )
		::SysFreeString( bstrFilter );

	::SetModifiedFlagToDoc( ptrFH );
}

/////////////////////////////////////////////////////////////////////////////
void CFilterPropPage::OnRemove() 
{
	IDBaseFilterHolderPtr ptrFH = GetActiveFilterHolder();	
	if( ptrFH == NULL )
		return;

	int nFilterIndex = GetActiveConditionIndex();
	if( nFilterIndex < 0 )
		return;


	BSTR bstrFilter = NULL;
	if( S_OK != ptrFH->GetActiveFilter( &bstrFilter ) )
		return;

	ptrFH->DeleteFilterCondition( bstrFilter, nFilterIndex );

	if( bstrFilter )
		::SysFreeString( bstrFilter );	

	::SetModifiedFlagToDoc( ptrFH );	

	SetListoxAutoExtent( &m_ctrlContext );
}

/////////////////////////////////////////////////////////////////////////////
void CFilterPropPage::OnLeftBracket() 
{
	IDBaseFilterHolderPtr ptrFH = GetActiveFilterHolder();	
	if( ptrFH == NULL )
		return;

	int nFilterIndex = GetActiveConditionIndex();
	if( nFilterIndex < 0 )
		return;


	BSTR bstrFilter = NULL;
	if( S_OK != ptrFH->GetActiveFilter( &bstrFilter ) )
		return;

	BSTR bstrTable, bstrField, bstrCondition, bstrValue, bstrValueTo;
	BOOL bAndUnion;
	int nLeftBracket, nRightBracket;

	if( S_OK != ptrFH->GetFilterCondition(bstrFilter, nFilterIndex,
						&bstrTable, &bstrField, 
						&bstrCondition, 
						&bstrValue, &bstrValueTo,
						&bAndUnion,
						&nLeftBracket, &nRightBracket					
						) )
	{
		if( bstrFilter )
			::SysFreeString( bstrFilter );

		return;
	}


	ptrFH->EditFilterCondition( bstrFilter, nFilterIndex,
								bstrTable, 
								bstrField, 
								bstrCondition, 
								bstrValue, 
								bstrValueTo, 
								bAndUnion, 
								nLeftBracket + 1, nRightBracket
								);		
	

	if( bstrTable )
		::SysFreeString( bstrTable );

	if( bstrField )
		::SysFreeString( bstrField );

	if( bstrCondition )
		::SysFreeString( bstrCondition );

	if( bstrValue )
		::SysFreeString( bstrValue );

	if( bstrValueTo )
		::SysFreeString( bstrValueTo );

	if( bstrFilter )
		::SysFreeString( bstrFilter );

	::SetModifiedFlagToDoc( ptrFH );	

	SetListoxAutoExtent( &m_ctrlContext );
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterPropPage::OnRightBracket() 
{
	IDBaseFilterHolderPtr ptrFH = GetActiveFilterHolder();	
	if( ptrFH == NULL )
		return;

	int nFilterIndex = GetActiveConditionIndex();
	if( nFilterIndex < 0 )
		return;


	BSTR bstrFilter = NULL;
	if( S_OK != ptrFH->GetActiveFilter( &bstrFilter ) )
		return;

	BSTR bstrTable, bstrField, bstrCondition, bstrValue, bstrValueTo;
	BOOL bAndUnion;
	int nLeftBracket, nRightBracket;

	if( S_OK != ptrFH->GetFilterCondition(bstrFilter, nFilterIndex,
						&bstrTable, &bstrField, 
						&bstrCondition, 
						&bstrValue, &bstrValueTo,
						&bAndUnion,
						&nLeftBracket, &nRightBracket					
						) )
	{
		if( bstrFilter )
			::SysFreeString( bstrFilter );

		return;
	}

	ptrFH->EditFilterCondition( bstrFilter, nFilterIndex,
								bstrTable, 
								bstrField, 
								bstrCondition, 
								bstrValue, 
								bstrValueTo, 
								bAndUnion, 
								nLeftBracket, nRightBracket + 1
								);
	

	if( bstrTable )
		::SysFreeString( bstrTable );

	if( bstrField )
		::SysFreeString( bstrField );

	if( bstrCondition )
		::SysFreeString( bstrCondition );

	if( bstrValue )
		::SysFreeString( bstrValue );

	if( bstrValueTo )
		::SysFreeString( bstrValueTo );

	if( bstrFilter )
		::SysFreeString( bstrFilter );

	::SetModifiedFlagToDoc( ptrFH );	

	SetListoxAutoExtent( &m_ctrlContext );
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterPropPage::OnResetBrackets() 
{
	IDBaseFilterHolderPtr ptrFH = GetActiveFilterHolder();	
	if( ptrFH == NULL )
		return;

	int nFilterIndex = GetActiveConditionIndex();
	if( nFilterIndex < 0 )
		return;

	BSTR bstrFilter = NULL;
	if( S_OK != ptrFH->GetActiveFilter( &bstrFilter ) )
		return;

	int nCondCount = -1;
	ptrFH->GetFilterConditionCount( bstrFilter, &nCondCount );


	for( int i=0;i<nCondCount;i++ )
	{
		BSTR bstrTable, bstrField, bstrCondition, bstrValue, bstrValueTo;
		BOOL bAndUnion;
		int nLeftBracket, nRightBracket;

		if( S_OK != ptrFH->GetFilterCondition(bstrFilter, nFilterIndex,
							&bstrTable, &bstrField, 
							&bstrCondition, 
							&bstrValue, &bstrValueTo,
							&bAndUnion,
							&nLeftBracket, &nRightBracket					
							) )
		{			
			continue;
		}



		ptrFH->EditFilterCondition( bstrFilter, nFilterIndex,
									bstrTable, 
									bstrField, 
									bstrCondition, 
									bstrValue, 
									bstrValueTo, 
									bAndUnion, 
									0, 0
									);		
		

		if( bstrTable )
			::SysFreeString( bstrTable );

		if( bstrField )
			::SysFreeString( bstrField );

		if( bstrCondition )
			::SysFreeString( bstrCondition );

		if( bstrValue )
			::SysFreeString( bstrValue );

		if( bstrValueTo )
			::SysFreeString( bstrValueTo );
	}


	if( bstrFilter )
		::SysFreeString( bstrFilter );

	::SetModifiedFlagToDoc( ptrFH );	

	SetListoxAutoExtent( &m_ctrlContext );
	
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CFilterPropPage::XDBFilterPage::BuildAppearance( )
{
	_try_nested(CFilterPropPage, DBFilterPage, BuildAppearance )
	{
		pThis->BuildAppearance();				
		return S_OK;
	}
	_catch_nested;
}

