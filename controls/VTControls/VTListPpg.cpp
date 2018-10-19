// VTListPpg.cpp : Implementation of the CVTListPropPage property page class.

#include "stdafx.h"
#include "VTControls.h"
#include "VTListPpg.h"

#include "BaseColumn.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int m_bUpdNameCol=-1;
int m_bUpdNameHeader=-1;
BOOL m_bUseExtHeaderCol=false;
CUIntArray m_HeaderAction;



BOOL GetShortValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, short &value, BOOL b_useArg);
BOOL GetStringValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, CString &str);
BOOL GetCellValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, short nRow, CString &str);
BOOL GetColorValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, OLE_COLOR &color);
BOOL GetColorValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn,short nRow, OLE_COLOR &color);

BOOL SetShortValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn);
BOOL SetShortValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, short value);
BOOL SetStringValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, BSTR pStr);

BOOL SetStringValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, short nRow, BSTR pStr);
BOOL SetColorValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, OLE_COLOR color);
BOOL SetColorValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, short nRow, OLE_COLOR color);

BOOL SetFontValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, LOGFONT &lf);
BOOL GetFontValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, LOGFONT &lf);

BOOL SetStringValueForLogical(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, BSTR pStr, BOOL b_forValue);

void ReportError( CString strError, HRESULT hr );
CString GetInokeError(OLECHAR FAR* ppszNames );

IMPLEMENT_DYNCREATE(CVTListPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTListPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTListPropPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTListPropPage, "VTCONTROLS.VTListPropPage.1",
	0x120a4075, 0x992b, 0x4c00, 0xaa, 0x54, 0x7c, 0xf8, 0xe0, 0x64, 0x53, 0x76)



/////////////////////////////////////////////////////////////////////////////
// CVTListPropPage::CVTListPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTListPropPage

BOOL CVTListPropPage::CVTListPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTLIST_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTListPropPage::CVTListPropPage - Constructor

CVTListPropPage::CVTListPropPage() :
	COlePropertyPage(IDD, IDS_VTLIST_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTListPropPage)
	m_b_use_grid = FALSE;
	m_b_UseFullRowSelect = FALSE;
	m_bAutoSaveToShell = FALSE;
	m_bEnabled = FALSE;
	m_strName = _T("");
	m_bUseSystemFont = FALSE;
	m_bUseSimpleFontType = FALSE;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CVTListPropPage::DoDataExchange - Moves data between page and properties

void CVTListPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVTListPropPage)
	DDP_Check(pDX, IDC_USEGRID, m_b_use_grid, _T("UseGrid") );
	DDX_Check(pDX, IDC_USEGRID, m_b_use_grid);
	DDP_Check(pDX, IDC_USE_FULL_ROW_SELECT, m_b_UseFullRowSelect, _T("UseFullRowSelect") );
	DDX_Check(pDX, IDC_USE_FULL_ROW_SELECT, m_b_UseFullRowSelect);
	DDP_Check(pDX, IDC_CHECK_AUTO_SAVE_TO_SHELL, m_bAutoSaveToShell, _T("AutoUpdate") );
	DDX_Check(pDX, IDC_CHECK_AUTO_SAVE_TO_SHELL, m_bAutoSaveToShell);
	DDP_Check(pDX, IDC_CHECK_ENABLE, m_bEnabled, _T("Enabled") );
	DDX_Check(pDX, IDC_CHECK_ENABLE, m_bEnabled);
	DDP_Text(pDX, IDC_EDIT_NAME, m_strName, _T("Name") );
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDP_Check(pDX, IDC_USE_SYSTEM_FONT, m_bUseSystemFont, _T("UseSystemFont") );
	DDX_Check(pDX, IDC_USE_SYSTEM_FONT, m_bUseSystemFont);
	DDP_Check(pDX, IDC_SIMPLE_FONT_CHOOSER, m_bUseSimpleFontType, _T("UseSimpleFontType") );
	DDX_Check(pDX, IDC_SIMPLE_FONT_CHOOSER, m_bUseSimpleFontType);
	DDX_Check(pDX, IDC_RESIZE_COLS, m_bEnableResizeColumns);
	DDP_Check(pDX, IDC_RESIZE_COLS, m_bEnableResizeColumns, _T("EnableResizeColumns") );
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CVTListPropPage message handlers





/////////////////////////////////////////////////////////////////////////////
// CVTListPropPageCol dialog

IMPLEMENT_DYNCREATE(CVTListPropPageCol, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTListPropPageCol, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTListPropPageCol)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST, OnEndlabeleditList)
	ON_CBN_SELCHANGE(IDC_COMBO_COLUMN_TYPE, OnSelchangeComboColumnType)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_CBN_SELCHANGE(IDC_COMBO_ALIGN, OnSelchangeComboAlign)
	ON_BN_CLICKED(IDC_BUTTON_FORE_COLOR, OnButtonForeColor)
	ON_BN_CLICKED(IDC_BUTTON_BACK_COLOR, OnButtonBackColor)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_CHECK_LOG_AS_TEXT, OnCheckLogAsText)
	ON_BN_CLICKED(IDC_CHECK_USE_ICON, OnCheckUseIcon)
	ON_BN_CLICKED(IDC_BUTTON_FONT, OnButtonFont)
	ON_BN_CLICKED(IDC_BUTTON_SET_TRUE_ICON, OnButtonSetTrueIcon)
	ON_BN_CLICKED(IDC_BUTTON_SET_FALSE_ICON, OnButtonSetFalseIcon)
	ON_BN_CLICKED(IDC_CHECK_READ_ONLY, OnCheckReadOnly)
	ON_EN_CHANGE(IDC_EDIT_COL_WIDTH, OnChangeEditColWidth)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_SET_GRAY_ICON, OnButtonSetGrayIcon)
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
END_MESSAGE_MAP()

//ON_NOTIFY(TVN_SELCHANGED, IDC_LIST, OnSelchangedList)
/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

// {179FECF4-D64E-11D3-80EA-0000E8DF68C3}
IMPLEMENT_OLECREATE_EX(CVTListPropPageCol, "VTCONTROLS.VTListPropPageCol.1",
	0x179fecf4, 0xd64e, 0x11d3, 0x80, 0xea, 0x0, 0x0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// CVTListPropPageCol::CVTListPropPageColFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTListPropPageCol

BOOL CVTListPropPageCol::CVTListPropPageColFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Define string resource for page type; replace '0' below with ID.

	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTLIST_PPGEX);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTListPropPageCol::CVTListPropPageCol - Constructor

// TODO: Define string resource for page caption; replace '0' below with ID.

CVTListPropPageCol::CVTListPropPageCol() :
	COlePropertyPage(IDD, IDS_VTLIST_PPGEX_CAPTION)
{
	//{{AFX_DATA_INIT(CVTListPropPageCol)
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CVTListPropPageCol::DoDataExchange - Moves data between page and properties

void CVTListPropPageCol::DoDataExchange(CDataExchange* pDX)
{
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_DATA_MAP(CVTListPropPageCol)
	DDX_Control(pDX, IDC_EDIT_COL_WIDTH, m_colWidthCtrl);
	DDX_Control(pDX, IDC_CHECK_READ_ONLY, m_readOnlyCtrl);
	DDX_Control(pDX, IDC_CHECK_USE_ICON, m_useIcon);
	DDX_Control(pDX, IDC_CHECK_LOG_AS_TEXT, m_logFormatCtrl);
	DDX_Control(pDX, IDC_COMBO_COLUMN_TYPE, m_typeCtrl);
	DDX_Control(pDX, IDC_LIST, m_listCtrl);
	DDX_Control(pDX, IDC_COMBO_ALIGN, m_alignCtrl);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CVTListPropPageCol message handlers
							 
BOOL CVTListPropPageCol::OnInitDialog()
{
	COlePropertyPage::OnInitDialog();	
	m_bUpdNameCol=-1;
	BuildColumnList( );

	CSpinButtonCtrl* pSpin = NULL;

	pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_WIDTH );

	if( pSpin )
		pSpin->SetRange( 0, 1000 );

	if(m_bUseExtHeaderCol)
	{
		(CVTListPropPageCol::GetDlgItem(IDC_BUTTON_ADD))->EnableWindow(FALSE);
		(CVTListPropPageCol::GetDlgItem(IDC_BUTTON_DELETE))->EnableWindow(FALSE);
	}
	else
	{
		(CVTListPropPageCol::GetDlgItem(IDC_BUTTON_ADD))->EnableWindow(TRUE);
		(CVTListPropPageCol::GetDlgItem(IDC_BUTTON_DELETE))->EnableWindow(TRUE);
	}

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CVTListPropPageCol::UpdatePageAndScrollToNewItem( int nItem )
{
	if( m_listCtrl.SetItemState( nItem, LVIS_SELECTED | LVIS_FOCUSED,
			LVIS_SELECTED | LVIS_FOCUSED) )
	{
		ChangeVisual( nItem );
		m_listCtrl.EnsureVisible( nItem, TRUE );
		return TRUE;
	}
	else
		return FALSE;
}


LPDISPATCH CVTListPropPageCol::GetDispatch()
{
	ULONG Ulong;
	LPDISPATCH *lpDispatch = NULL;

	lpDispatch = GetObjectArray(&Ulong);
	
	if( !lpDispatch[0] )
	{
		ReportError( "Can't find PropertyPage dispatch interface" , 0 );
		return FALSE;	
	}

	return lpDispatch[0];
	
}

BOOL CVTListPropPageCol::BuildColumnList( )
{
	
	m_listCtrl.DeleteAllItems();
	
	CRect rc;

	m_listCtrl.GetClientRect( &rc );
	
	if(m_bUpdNameCol!=1)
	m_listCtrl.InsertColumn( 0, _T("Column"), LVCFMT_CENTER, rc.Width( ) );
	if(m_bUpdNameCol==1)
	{
		if(m_bUseExtHeaderCol)
		{
			(CVTListPropPageCol::GetDlgItem(IDC_BUTTON_ADD))->EnableWindow(FALSE);
			(CVTListPropPageCol::GetDlgItem(IDC_BUTTON_DELETE))->EnableWindow(FALSE);
		}
		else
		{
			(CVTListPropPageCol::GetDlgItem(IDC_BUTTON_ADD))->EnableWindow(TRUE);
			(CVTListPropPageCol::GetDlgItem(IDC_BUTTON_DELETE))->EnableWindow(TRUE);
		}
	}

	short nColCount = -1;
	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"GetColumnCount", -1, nColCount, FALSE) )	
		return FALSE;	
	
	
	for(short i=0;i<nColCount;i++)
	{

		CString strCaption;
		if( !GetStringValue( GetDispatch(), ((OLECHAR*)(L"GetColumnCaption")), i, strCaption) )		
			return FALSE;

		BOOL bFind=false;
		while(!bFind)
		{
			int nCF = strCaption.Find('|',0);
			if(nCF>=0)
				strCaption.Delete(0,nCF+1);
			else
				bFind=true;
		}
		m_listCtrl.InsertItem( i, strCaption );

	}


	UpdatePageAndScrollToNewItem( 0 );

	return TRUE;

}


BOOL CVTListPropPageCol::ChangeVisual( int nItem )
{
	
	int index = nItem;
	if(index < 0) return FALSE;

	short nColumnType = -1;
	if( !GetShortValue( GetDispatch(), ((OLECHAR*)(L"GetColumnType")), index, nColumnType, TRUE) )		
		return FALSE;

	m_typeCtrl.SetCurSel( nColumnType );

			   	
	short nColumnAlign = -1;
	if( !GetShortValue( GetDispatch(), ((OLECHAR*)(L"GetColumnAlign")), index, nColumnAlign, TRUE) )		
		return FALSE;

	m_alignCtrl.SetCurSel( nColumnAlign );

	
	short nShowLogicalAsText = -1;
	if( !GetShortValue( GetDispatch(), ((OLECHAR*)(L"GetDisplayLogicalAsText")), index, nShowLogicalAsText, TRUE) )		
		return FALSE;
	

	if( CBaseColumn::ConvertFromShortToEnum( nColumnType ) == TYPE_LOGICAL)
	{
		m_logFormatCtrl.EnableWindow( TRUE );
		if( (BOOL)nShowLogicalAsText )
			m_logFormatCtrl.SetCheck( TRUE );
		else
			m_logFormatCtrl.SetCheck( FALSE );		

		GetDlgItem(IDC_BUTTON_SET_TRUE_ICON)->EnableWindow( TRUE );
		GetDlgItem(IDC_BUTTON_SET_FALSE_ICON)->EnableWindow( TRUE );

	}else
	{
		m_logFormatCtrl.EnableWindow( FALSE );
		GetDlgItem(IDC_BUTTON_SET_TRUE_ICON)->EnableWindow( FALSE );
		GetDlgItem(IDC_BUTTON_SET_FALSE_ICON)->EnableWindow( FALSE );

	}

	short nColumnUseIcon = -1;
	if( !GetShortValue( GetDispatch(), ((OLECHAR*)(L"IsColumnUseIcon")), index, nColumnUseIcon, TRUE) )		
		return FALSE;
	
	if((BOOL)nColumnUseIcon)	
		m_useIcon.SetCheck( TRUE );
	else
		m_useIcon.SetCheck( FALSE );	

	
	
	if( CBaseColumn::ConvertFromShortToEnum( nColumnType ) == TYPE_SUBJECT)	
	{
		m_useIcon.EnableWindow( TRUE );		
	}
	else	
	{
		m_useIcon.EnableWindow( FALSE );
	}

	short bIsReadOnly = -1;
	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"IsColumnReadOnly", index, bIsReadOnly, TRUE) )	
		return FALSE;	

	m_readOnlyCtrl.SetCheck( (BOOL)bIsReadOnly );

	short nColumnWidth = -1;
	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"GetColumnWidth", index, nColumnWidth, TRUE) )	
		return FALSE;	


	CString strWidth;
	strWidth.Format( "%d", nColumnWidth);

	m_colWidthCtrl.SetWindowText( strWidth );
	  

	return TRUE;
}

void CVTListPropPageCol::OnChangeEditColWidth() 
{
	if( m_listCtrl.GetSafeHwnd() )
	{
		short index = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);	
		if(index < 0) return;		


		short nOldColumnWidth = -1;
		if( !GetShortValue( GetDispatch(), (OLECHAR*)L"GetColumnWidth", index, nOldColumnWidth, TRUE) )	
			return;	


		char sz_Buf[255];	

		m_colWidthCtrl.GetWindowText( sz_Buf, 255 );

		short value = ::atoi(sz_Buf);


		if( nOldColumnWidth == value) return;

		if( value > 0)
			if( !SetShortValue( GetDispatch(), (OLECHAR*)(L"SetColumnWidth"), index, value ) )
				return;
	}
}
						


void CVTListPropPageCol::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
		
	int newItem = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);
	if(newItem != -1)
	{
		ChangeVisual( newItem );
	}
	
}

void CVTListPropPageCol::OnCheckReadOnly() 
{
	short index = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;	

	short value = m_readOnlyCtrl.GetCheck( );

	if( !SetShortValue( GetDispatch(), (OLECHAR*)(L"SetColumnReadOnly"), index, value ) )
		return;
	
}



void CVTListPropPageCol::OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	short index = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;	


	if(pDispInfo->item.pszText == "" ) return;
	
	CString str = pDispInfo->item.pszText;

	if( !SetStringValue( GetDispatch(), (OLECHAR*)(L"SetColumnCaption"), index, 
		str.AllocSysString()) )
		return;

	
	m_listCtrl.SetItemText( pDispInfo->item.iItem, 0, pDispInfo->item.pszText );

	INT_PTR num=m_HeaderAction.GetCount();
	m_HeaderAction.InsertAt(num,1);
	m_bUpdNameHeader=1;
}


void CVTListPropPageCol::OnSelchangeComboColumnType() 
{	

	short index = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);	

	if(index < 0) return;	

	short value = m_typeCtrl.GetCurSel(  );
	if(value < 0) return;

	if( !SetShortValue( GetDispatch(), (OLECHAR*)(L"SetColumnType"), index, value ) )
		return;

	ChangeVisual( index );
}
			 
void CVTListPropPageCol::OnSelchangeComboAlign() 
{
	short index = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);	

	if(index < 0) return;	

	short value = m_alignCtrl.GetCurSel(  );
	if(value < 0) return;

	if( !SetShortValue( GetDispatch(), (OLECHAR*)(L"SetColumnAlign"), index, value ))	
		return;
}

void CVTListPropPageCol::OnCheckLogAsText() 
{
	short index = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);	

	if(index < 0) return;	

	short value = m_logFormatCtrl.GetCheck();

	if( !SetShortValue( GetDispatch(), (OLECHAR*)(L"SetDisplayLogicalAsText"), index, value ) )
		return;
	
}

void CVTListPropPageCol::OnButtonForeColor() 
{
	short index = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;	

	OLE_COLOR Color;
	COLORREF cr = RGB(0x00,0x00,0x00);

	if( !GetColorValue( GetDispatch(), ((OLECHAR*)(L"GetColumnForeColor")), index, Color) )		
		return;

	::OleTranslateColor(Color, NULL, &cr);

	CColorDialog dlg( cr, CC_RGBINIT | CC_ANYCOLOR | CC_FULLOPEN );
	if( dlg.DoModal() == IDOK )
	{
		if( !SetColorValue( GetDispatch(), ((OLECHAR*)(L"SetColumnForeColor")), index, (OLE_COLOR)dlg.m_cc.rgbResult) )
			return;

		CWnd* pWndStatic = NULL;
		pWndStatic = GetDlgItem( IDC_TEST_STRING );
		if( pWndStatic ) pWndStatic->Invalidate( );

	}
}

void CVTListPropPageCol::OnButtonBackColor() 
{
	short index = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;	

	OLE_COLOR Color;
	COLORREF cr = RGB(0x00,0x00,0x00);

	if( !GetColorValue( GetDispatch(), ((OLECHAR*)(L"GetColumnBackColor")), index, Color) )		
		return;

	::OleTranslateColor(Color, NULL, &cr);

	CColorDialog dlg( cr, CC_RGBINIT | CC_ANYCOLOR | CC_FULLOPEN );
	if( dlg.DoModal() == IDOK )
	{
		if( !SetColorValue( GetDispatch(), ((OLECHAR*)(L"SetColumnBackColor")), index, (OLE_COLOR)dlg.m_cc.rgbResult) )
			return;

		CWnd* pWndStatic = NULL;
		pWndStatic = GetDlgItem( IDC_TEST_STRING );
		if( pWndStatic ) pWndStatic->Invalidate( );

	}
	
}

//Add Column
void CVTListPropPageCol::OnButtonAdd() 
{
	if( !SetShortValue( GetDispatch(), ((OLECHAR*)(L"AddColumn")), -1/*NOT USE*/) )
		return;

	short nColCount = -1;
	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"GetColumnCount", -1, nColCount, FALSE) )	
		return;	


	CString strCaption;
	if( !GetStringValue( GetDispatch(), ((OLECHAR*)(L"GetColumnCaption")), nColCount-1, strCaption) )		
		return;

	m_listCtrl.InsertItem( nColCount-1, strCaption );

	UpdatePageAndScrollToNewItem( nColCount-1 );
	
	INT_PTR num=m_HeaderAction.GetCount();
	m_HeaderAction.InsertAt(num,2);
	m_bUpdNameHeader=2;
}


void CVTListPropPageCol::OnCheckUseIcon()
{
	short index = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);	

	if(index < 0) return;	

	short value = (short)m_useIcon.GetCheck( );
	if(value < 0) return;

	SetShortValue( GetDispatch(), (OLECHAR*)(L"SetColumnUseIcon"), index, value );	
	
}



//Delete Column
void CVTListPropPageCol::OnButtonDelete() 
{
	short index = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;	
	if( !SetShortValue( GetDispatch(), ((OLECHAR*)(L"DeleteColumn")), index ) )
		return;

	m_listCtrl.DeleteItem( index );

	if( !UpdatePageAndScrollToNewItem( index - 1 ) )
		UpdatePageAndScrollToNewItem( index );

	m_bUpdNameHeader=3+index;
	INT_PTR num=m_HeaderAction.GetCount();
	m_HeaderAction.InsertAt(num,m_bUpdNameHeader);
}


void CVTListPropPageCol::OnButtonFont() 
{	
	short index = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;	

	LOGFONT lf;
	ZeroMemory( &lf, sizeof(lf) );
	

	if( !GetFontValue( GetDispatch(), (OLECHAR*)L"GetColumnFont", index, lf) )	
		return ;		

	CFontDialog dlg( &lf );
	if( dlg.DoModal() == IDOK)
	{
		dlg.GetCurrentFont( &lf );
		if( !SetFontValue( GetDispatch(), ((OLECHAR*)(L"SetColumnFont")), index, lf) )		
			return;

	}

	
}

void CVTListPropPageCol::OnButtonSetTrueIcon() 
{
	short index = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;	
	
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
	dlg.m_ofn.lpstrFilter = "Ico files(*.ico)\0*.ico\0\0";

	if( dlg.DoModal( ) == IDOK )
	{
		CString str = dlg.GetPathName();
		if( !SetStringValueForLogical(GetDispatch(), ((OLECHAR*)(L"SetColumnLogicalIcon")), 
			index, str.AllocSysString(), TRUE ) )
			return;
	}
	
}

void CVTListPropPageCol::OnButtonSetFalseIcon() 
{
	short index = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;	
	
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
	dlg.m_ofn.lpstrFilter = "Ico files(*.ico)\0*.ico\0\0";

	if( dlg.DoModal( ) == IDOK )
	{
		CString str = dlg.GetPathName();
		if( !SetStringValueForLogical(GetDispatch(), ((OLECHAR*)(L"SetColumnLogicalIcon")), 
			index, str.AllocSysString(), FALSE ) )
			return;
	}
}

void CVTListPropPageCol::OnButtonSetGrayIcon() 
{
	short index = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;	
	
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
	dlg.m_ofn.lpstrFilter = "Ico files(*.ico)\0*.ico\0\0";

	if( dlg.DoModal( ) == IDOK )
	{
		CString str = dlg.GetPathName();
		if( !SetStringValueForLogical(GetDispatch(), ((OLECHAR*)(L"SetColumnLogicalIcon")), 
			index, str.AllocSysString(), 2 ) )
			return;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVTListPropPageRow dialog

IMPLEMENT_DYNCREATE(CVTListPropPageRow, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTListPropPageRow, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTListPropPageRow)
	ON_BN_CLICKED(IDC_BUTTON_ADD_ROW, OnButtonAddRow)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_ROW, OnButtonDeleteRow)
	ON_BN_CLICKED(IDC_BUTTON_SET_ICON, OnButtonSetIcon)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_ICON, OnButtonClearIcon)
	ON_BN_CLICKED(IDC_BUTTON_CUR_COLUMN_LEFT, OnButtonCurColumnLeft)
	ON_BN_CLICKED(IDC_BUTTON_CUR_COLUMN_RIGHT, OnButtonCurColumnRight)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROW, OnItemchangedListRow)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_ROW, OnEndlabeleditListRow)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	ON_EN_CHANGE(IDC_EDIT_KEY_VALUE, OnChangeEditKeyValue)		
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_FORE_COLOR, OnBnClickedButtonForeColor)
	ON_BN_CLICKED(IDC_BUTTON_BACK_COLOR, OnBnClickedButtonBackColor)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

// {7349D2AB-D786-11D3-80EE-0000E8DF68C3}
IMPLEMENT_OLECREATE_EX(CVTListPropPageRow, "VTCONTROLS.VTListPropPageRow.1",
	0x7349d2ab, 0xd786, 0x11d3, 0x80, 0xee, 0x0, 0x0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// CVTListPropPageRow::CVTListPropPageRowFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTListPropPageRow

BOOL CVTListPropPageRow::CVTListPropPageRowFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Define string resource for page type; replace '0' below with ID.

	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTLIST_PPGROW);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTListPropPageRow::CVTListPropPageRow - Constructor

// TODO: Define string resource for page caption; replace '0' below with ID.

CVTListPropPageRow::CVTListPropPageRow() :
	COlePropertyPage(IDD, IDS_VTLIST_PPGROW_CAPTION)
{
	//{{AFX_DATA_INIT(CVTListPropPageRow)		
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CVTListPropPageRow::DoDataExchange - Moves data between page and properties

void CVTListPropPageRow::DoDataExchange(CDataExchange* pDX)
{
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_DATA_MAP(CVTListPropPageRow)
	DDX_Control(pDX, IDC_LIST_ROW, m_listRowCtrl);
	DDX_Control(pDX, IDC_STATIC_CUR_COLUMN, m_curColumnCtrl);		
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CVTListPropPageRow message handlers

BOOL CVTListPropPageRow::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();
	
	BuildRowList( );
	
	ScrollToNewItem( 0 );

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}





BOOL CVTListPropPageRow::BuildRowList( )
{
	short index = m_listRowCtrl.GetNextItem(-1, LVNI_SELECTED);		

	m_listRowCtrl.DeleteAllItems( );	
	m_listRowCtrl.DeleteColumn( 0 );

	GetDlgItem(IDC_EDIT_KEY_VALUE)->SetWindowText( "" );

	CRect rc;
	m_listRowCtrl.GetClientRect( &rc );

	m_listRowCtrl.InsertColumn( 0, _T("Row"), LVCFMT_CENTER, rc.Width() );

	short nCurColumn = -1;

	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"GetCurentColumn", -1, nCurColumn, FALSE) )	
		return FALSE;		

	short nColumnType = -1;

	if( !GetShortValue( GetDispatch(), ((OLECHAR*)(L"GetColumnType")), nCurColumn, nColumnType, TRUE) )		
		return FALSE;
	 
	

	if( CBaseColumn::ConvertFromShortToEnum( nColumnType ) != TYPE_SUBJECT)	
		GetDlgItem(IDC_BUTTON_SET_ICON)->EnableWindow( FALSE );
	else
		GetDlgItem(IDC_BUTTON_SET_ICON)->EnableWindow( TRUE );

	
	short nRowCount = -1;
	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"GetRowCount", -1, nRowCount, FALSE) )	
		return FALSE;	

	short nColumn = nCurColumn;	

	CString strValue;


	for(short i=0;i<nRowCount;i++)
	{
		if( !GetCellValue( GetDispatch(), (OLECHAR*)L"GetCellAsString", nColumn, i, strValue) )
			return FALSE;

		m_listRowCtrl.InsertItem( i, strValue );
	}

	CString strColCaption;
	if( !GetStringValue( GetDispatch(), ((OLECHAR*)(L"GetColumnCaption")), nCurColumn, strColCaption) )		
			return FALSE;

	CString strColString;

	BOOL bFind=false;
	while(!bFind)
	{
		int nCF = strColCaption.Find('|',0);
		if(nCF>=0)
			strColCaption.Delete(0,nCF+1);
		else
			bFind=true;
	}

	strColString.Format( "¹%d :%s", nCurColumn + 1, strColCaption );
	
	m_curColumnCtrl.SetWindowText( strColString );


	ScrollToNewItem( index );
	

	return TRUE;
}


BOOL CVTListPropPageRow::ScrollToNewItem( int nItem )
{
	if( m_listRowCtrl.SetItemState( nItem, LVIS_SELECTED | LVIS_FOCUSED,
			LVIS_SELECTED | LVIS_FOCUSED) )
	{		
		m_listRowCtrl.EnsureVisible( nItem, TRUE );
		return TRUE;
	}
	else
		return FALSE;
}



LPDISPATCH CVTListPropPageRow::GetDispatch()
{
	ULONG Ulong;
	LPDISPATCH *lpDispatch = NULL;

	lpDispatch = GetObjectArray(&Ulong);
	
	if( !lpDispatch[0] )
	{									   
		ReportError( "Can't find PropertyPage dispatch interface" , 0);
		return FALSE;	
	}

	return lpDispatch[0];
}

	


void CVTListPropPageRow::OnButtonAddRow() 
{
	
	//short index = m_listRowCtrl.GetNextItem(-1, LVNI_SELECTED);	
	//if(index < 0) return;		
	
	if( !SetShortValue( GetDispatch(), ((OLECHAR*)(L"AddRow")), -1/*NOT USE*/) )
		return;

	short nRowCount = -1;
	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"GetRowCount", -1, nRowCount, FALSE) )	
		return;	

	BuildRowList( );

	ScrollToNewItem( nRowCount-1 );	

}

void CVTListPropPageRow::OnButtonRefresh() 
{
	BuildRowList( );	
}


void CVTListPropPageRow::OnButtonDeleteRow() 
{
	short index = m_listRowCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;		
	
	if( !SetShortValue( GetDispatch(), ((OLECHAR*)(L"DeleteRow")), index ) )
		return;

	BuildRowList( );

	if(!ScrollToNewItem( index ))
		ScrollToNewItem( index-1);
		


}

void CVTListPropPageRow::OnButtonSetIcon()
{
	short index = m_listRowCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;		

	short nCurColumn = -1;
	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"GetCurentColumn", -1, nCurColumn, FALSE) )	
		return;	


	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
	dlg.m_ofn.lpstrFilter = "Ico files(*.ico)\0*.ico\0\0";

	if( dlg.DoModal( ) == IDOK )
	{
		CString str = dlg.GetPathName();
		if( !SetStringValue(GetDispatch(), ((OLECHAR*)(L"SetIcon")), 
			nCurColumn, index, str.AllocSysString()))
			return;
	}



}


void CVTListPropPageRow::OnButtonClearIcon() 
{
	short index = m_listRowCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;		

	short nCurColumn = -1;
	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"GetCurentColumn", -1, nCurColumn, FALSE) )	
		return;	


		
	if( !SetStringValue(GetDispatch(), ((OLECHAR*)(L"SetIcon")), 
		nCurColumn, index, (OLECHAR*)L""))
		return;
//	BuildRowList( );

//	m_listCtrl.SetSel( index );
	
}

void CVTListPropPageRow::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	COlePropertyPage::OnActivate(nState, pWndOther, bMinimized);	
	
}

void CVTListPropPageRow::OnButtonCurColumnLeft() 
{
	short nCurCol = -1;
	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"CurColumnLeft", -1, nCurCol, FALSE) )	
		return;		
	
	BuildRowList( );
}

void CVTListPropPageRow::OnButtonCurColumnRight() 
{
	short nCurCol = -1;
	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"CurColumnRight", -1, nCurCol, FALSE) )	
		return;		

	BuildRowList( );

}

void CVTListPropPageRow::OnEndlabeleditListRow(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	*pResult = 0;
	
	short index = m_listRowCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;


	short nCurColumn = -1;
	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"GetCurentColumn", -1, nCurColumn, FALSE) )	
		return;	


	short nColumnType = -1;
	if( !GetShortValue( GetDispatch(), ((OLECHAR*)(L"GetColumnType")), 
		nCurColumn, nColumnType, TRUE) )		
		return;

	
	if( CBaseColumn::ConvertFromShortToEnum( nColumnType ) != TYPE_SUBJECT)
	{
		AfxMessageBox( "You can edit here only SUBJECT column type." );
		return;
	}
	


	CString str = pDispInfo->item.pszText;

	if(pDispInfo->item.pszText == "" ) return;

	if( !SetStringValue( 
			GetDispatch(), 
			(OLECHAR*)(L"SetCellValue"), 
			nCurColumn,
			index, 			
			str.AllocSysString()) )
		return;

	
	m_listRowCtrl.SetItemText( pDispInfo->item.iItem, 0, pDispInfo->item.pszText );

}


void CVTListPropPageRow::OnItemchangedListRow(NMHDR* pNMHDR, LRESULT* pResult) 
{	

	*pResult = 0;

	short index = m_listRowCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;


	short nCurColumn = -1;
	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"GetCurentColumn", -1, nCurColumn, FALSE) )	
		return;	
	
	CString strKeyValue;
	if( !GetCellValue( GetDispatch(), (OLECHAR*)L"GetKeyValue", nCurColumn, index, strKeyValue) )	
		return;	
	
	
	GetDlgItem(IDC_EDIT_KEY_VALUE)->SetWindowText( strKeyValue );
	
	
}

void CVTListPropPageRow::OnChangeEditKeyValue() 
{
	//UpdateData( TRUE );		




	short index = m_listRowCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;


	short nCurColumn = -1;
	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"GetCurentColumn", -1, nCurColumn, FALSE) )	
		return;	
	
	char sz_buf[255];
	
	GetDlgItem(IDC_EDIT_KEY_VALUE)->GetWindowText( sz_buf, 255 );

	CString str = sz_buf;

	if( !SetStringValue( GetDispatch(), (OLECHAR*)L"SetKeyValue", nCurColumn, index, str.AllocSysString() ))
		return;	
}



//////////////////////////////////////////////////////////////////////////////////////
//
//				  Helper functions
//
//
//////////////////////////////////////////////////////////////////////////////////////



BOOL GetDISPIDFromName(OLECHAR FAR* FAR* ppszNames, DISPID &id, LPDISPATCH lpDispatch)
{
	if( !lpDispatch ) return FALSE;	

	HRESULT hr = lpDispatch->GetIDsOfNames( IID_NULL, ppszNames, 1, LOCALE_USER_DEFAULT, &id );

	if( hr == S_OK) return TRUE;

	CString strFunc = *ppszNames;
	
	CString strError;	
	
	strError.Format( "Can't find function ID:%s", strFunc );
	ReportError( strError, hr );

	return FALSE;
}


BOOL GetShortValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, short &value, BOOL b_useArg)
{
	if( !lpDispatch ) return FALSE;		

	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[1];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );
	

	V_VT(&varg[0]) = VT_I2;
	V_I2(&varg[0]) = nColumn;	    


	if(b_useArg)
	{		
		dispparams.rgvarg			= varg;
		dispparams.cArgs			= 1;
		dispparams.cNamedArgs		= 0;
		
	}else
	{
		dispparams.rgvarg				= NULL;
		dispparams.rgdispidNamedArgs	= NULL;
		dispparams.cArgs				= 0;
		dispparams.cNamedArgs			= 0;
	}
	

	VARIANT vt;
	ZeroMemory( &vt, sizeof(vt) );

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					&vt,
					NULL,
					NULL
					);

	if( hr == S_OK ){
		value = vt.iVal;
		return TRUE;
	}	
	
	ReportError( GetInokeError(ppszNames), hr );


	return FALSE;	
}


BOOL GetStringValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, CString &value)
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[1];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );
	

	V_VT(&varg[0]) = VT_I2;
	V_I2(&varg[0]) = nColumn;	    


	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 1;
	dispparams.cNamedArgs		= 0;
		

	VARIANT vt={VT_EMPTY};

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					&vt,
					NULL,
					NULL
					);

	if( hr == S_OK ){		
		value = vt.bstrVal;
		return TRUE;
	}

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	
}


BOOL GetCellValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, short nRow, CString &value)
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[2];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );
	

	V_VT(&varg[1]) = VT_I2;
	V_I2(&varg[1]) = nColumn;	    
	V_VT(&varg[0]) = VT_I2;
	V_I2(&varg[0]) = nRow;	    


	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 2;
	dispparams.cNamedArgs		= 0;
		

	VARIANT vt={VT_EMPTY};

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					&vt,
					NULL,
					NULL
					);

	if( hr == S_OK ){		
		value = vt.bstrVal;
		return TRUE;
	}

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	
}



BOOL GetColorValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, OLE_COLOR &value)
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[1];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );
	

	V_VT(&varg[0]) = VT_I2;
	V_I2(&varg[0]) = nColumn;	    


	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 1;
	dispparams.cNamedArgs		= 0;
		

	VARIANT vt={VT_EMPTY};

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					&vt,
					NULL,
					NULL
					);

	if( hr == S_OK ){
		value = (DWORD)vt.lVal;
		return TRUE;
	}			

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	
}

BOOL GetColorValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, short nRow, OLE_COLOR &value)
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[2];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );
	
	V_VT(&varg[0]) = VT_I2;
	V_I2(&varg[0]) = nRow;	    

	V_VT(&varg[1]) = VT_I2;
	V_I2(&varg[1]) = nColumn;	    


	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 2;
	dispparams.cNamedArgs		= 0;
		

	VARIANT vt={VT_EMPTY};

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					&vt,
					NULL,
					NULL
					);

	if( hr == S_OK ){
		value = (DWORD)vt.lVal;
		return TRUE;
	}			

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	
}



BOOL SetShortValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, short value)
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[2];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );
	

	V_VT(&varg[0]) = VT_I2;
	V_I2(&varg[0]) = value;	    

	V_VT(&varg[1]) = VT_I2;
	V_I2(&varg[1]) = nColumn;

	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 2;
	dispparams.cNamedArgs		= 0;
	

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					NULL,
					NULL,
					NULL
					);

	if( hr == S_OK ){		
		return TRUE;
	}

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	

}

BOOL SetFontValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, LOGFONT &lf)
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[2];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );	


	V_VT(&varg[0]) = VT_VOID|VT_BYREF;
	V_BYREF(&varg[0]) = (VOID*)&lf;

	V_VT(&varg[1]) = VT_I2;
	V_I2(&varg[1]) = nColumn;	    


	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 2;
	dispparams.cNamedArgs		= 0;
	

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					NULL,
					NULL,
					NULL
					);


	ASSERT(hr == S_OK);	


	if( hr == S_OK ){		
		return TRUE;
	}

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	
}

BOOL GetFontValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, LOGFONT &lf)
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[1];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );
	

	V_VT(&varg[0]) = VT_I2;
	V_I2(&varg[0]) = nColumn;	    


	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 1;
	dispparams.cNamedArgs		= 0;
		

	VARIANT vt={VT_EMPTY};

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					&vt,
					NULL,
					NULL
					);

	if( hr == S_OK ){
		lf = *(LPLOGFONT)vt.byref;
		return TRUE;
	}			

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	
}

BOOL SetShortValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn)
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[2];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );
	

	V_VT(&varg[0]) = VT_I2;
	V_I2(&varg[0]) = nColumn;

	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 1;
	dispparams.cNamedArgs		= 0;
	

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					NULL,
					NULL,
					NULL
					);

	if( hr == S_OK ){		
		return TRUE;
	}

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	
}

BOOL SetStringValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, BSTR pStr)
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[2];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );	


	V_VT(&varg[0]) = VT_BSTR/*|VT_BYREF*/;
	V_BSTR/*REF*/(&varg[0]) = /*&*/pStr;

	V_VT(&varg[1]) = VT_I2;
	V_I2(&varg[1]) = nColumn;	    


	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 2;
	dispparams.cNamedArgs		= 0;
	

	CString str = pStr;	


	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					NULL,
					NULL,
					NULL
					);


	ASSERT(hr == S_OK);
	


	if( hr == S_OK ){		
		return TRUE;
	}

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	
}


BOOL SetStringValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, short nRow, BSTR pStr)
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[3];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );	


	V_VT(&varg[0]) = VT_BSTR/*|VT_BYREF*/;
	V_BSTR/*REF*/(&varg[0]) = /*&*/pStr;

	V_VT(&varg[1]) = VT_I2;
	V_I2(&varg[1]) = nRow;	    


	V_VT(&varg[2]) = VT_I2;
	V_I2(&varg[2]) = nColumn;	    

	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 3;
	dispparams.cNamedArgs		= 0;
	

	CString str = pStr;	


	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					NULL,
					NULL,
					NULL
					);


	ASSERT(hr == S_OK);
	


	if( hr == S_OK ){		
		return TRUE;
	}

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	
}



BOOL SetColorValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, OLE_COLOR color)
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[2];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );	


	V_VT(&varg[0]) = VT_I4;
	V_I4(&varg[0]) = color;

	V_VT(&varg[1]) = VT_I2;
	V_I2(&varg[1]) = nColumn;	    


	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 2;
	dispparams.cNamedArgs		= 0;
	

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					NULL,
					NULL,
					NULL
					);

	if( hr == S_OK ){		
		return TRUE;
	}

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	

}

BOOL SetColorValue(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, short nRow, OLE_COLOR color )
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[3];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );	


	V_VT(&varg[0]) = VT_I4;
	V_I4(&varg[0]) = color;

	V_VT(&varg[1]) = VT_I2;
	V_I2(&varg[1]) = nRow;	    

	V_VT(&varg[2]) = VT_I2;
	V_I2(&varg[2]) = nColumn;	    


	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 3;
	dispparams.cNamedArgs		= 0;
	

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					NULL,
					NULL,
					NULL
					);

	if( hr == S_OK ){		
		return TRUE;
	}

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	

}

BOOL SetStringValueForLogical(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, short nColumn, BSTR pStr, BOOL b_forValue)
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[3];

	
	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );	

	V_VT(&varg[0]) = VT_I2;
	V_BOOL(&varg[0]) = b_forValue;	    

	V_VT(&varg[1]) = VT_BSTR/*|VT_BYREF*/;
	V_BSTR/*REF*/(&varg[1]) = /*&*/pStr;

	V_VT(&varg[2]) = VT_I2;
	V_I2(&varg[2]) = nColumn;	    

	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 3;
	dispparams.cNamedArgs		= 0;
	

	CString str = pStr;	


	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					NULL,
					NULL,
					NULL
					);


	ASSERT(hr == S_OK);
	


	if( hr == S_OK ){		
		return TRUE;	
	}

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	
}


void ReportError( CString strError, HRESULT hr )
{

	CString strErrorMsg;

	strErrorMsg.Format( "Error occured.\nError description:%s\nError code:%u",
		strError, hr
		);
	
	::MessageBox( NULL, strErrorMsg, "Error" , MB_OK | MB_ICONSTOP | MB_APPLMODAL );
}

CString GetInokeError(OLECHAR FAR* ppszNames )
{
	CString strError;
	CString strFunc = ppszNames;
	strError.Format("Inoke execute failed:%s", strFunc );	
	return strError;	
}



void CVTListPropPageRow::OnBnClickedButtonForeColor()
{
	short nCurColumn = -1;

	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"GetCurentColumn", -1, nCurColumn, FALSE) )	
		return;		

	short index = m_listRowCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;	

	OLE_COLOR Color;
	COLORREF cr = 0;

	if( !GetColorValue( GetDispatch(), ((OLECHAR*)(L"GetCellForeColor")), nCurColumn, index, Color ) )		
		return;

	::OleTranslateColor( Color, NULL, &cr );

	CColorDialog dlg( cr, CC_RGBINIT | CC_ANYCOLOR | CC_FULLOPEN );
	if( dlg.DoModal() == IDOK )
	{
		if( !SetColorValue( GetDispatch(), ((OLECHAR*)(L"SetCellForeColor")), nCurColumn, index, (OLE_COLOR)dlg.m_cc.rgbResult ) )
			return;

		CWnd* pWndStatic = NULL;
		pWndStatic = GetDlgItem( IDC_TEST_STRING );
		if( pWndStatic ) pWndStatic->Invalidate( );

	}
}

void CVTListPropPageRow::OnBnClickedButtonBackColor()
{
	short nCurColumn = -1;

	if( !GetShortValue( GetDispatch(), (OLECHAR*)L"GetCurentColumn", -1, nCurColumn, FALSE) )	
		return;		

	short index = m_listRowCtrl.GetNextItem(-1, LVNI_SELECTED);	
	if(index < 0) return;	

	OLE_COLOR Color;
	COLORREF cr = RGB(0x00,0x00,0x00);

	if( !GetColorValue( GetDispatch(), ((OLECHAR*)(L"GetCellBackColor")), nCurColumn, index, Color ) )		
		return;

	::OleTranslateColor( Color, NULL, &cr );

	CColorDialog dlg( cr, CC_RGBINIT | CC_ANYCOLOR | CC_FULLOPEN );
	if( dlg.DoModal() == IDOK )
	{
		if( !SetColorValue( GetDispatch(), ((OLECHAR*)(L"SetCellBackColor")), nCurColumn, index, (OLE_COLOR)dlg.m_cc.rgbResult) )
			return;

		CWnd* pWndStatic = NULL;
		pWndStatic = GetDlgItem( IDC_TEST_STRING );
		if( pWndStatic ) pWndStatic->Invalidate( );

	}
}

///////////////////////
// CVTListPropPageHeader dialog
IMPLEMENT_DYNCREATE(CVTListPropPageHeader, COlePropertyPage)

// {EFD249DC-0807-4356-A252-BC8660D37EF8}
IMPLEMENT_OLECREATE_EX(CVTListPropPageHeader, "VTCONTROLS.VTListPropPageHeader.1",
0xefd249dc, 0x807, 0x4356, 0xa2, 0x52, 0xbc, 0x86, 0x60, 0xd3, 0x7e, 0xf8);

BOOL CVTListPropPageHeader::CVTListPropPageHeaderFactory::UpdateRegistry(BOOL bRegister)
{
	if ( bRegister )
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(), m_clsid, IDS_VTLIST_PPGHEADER );
	else
		return AfxOleUnregisterClass( m_clsid,  0 );
}

CVTListPropPageHeader::CVTListPropPageHeader()
	: COlePropertyPage(CVTListPropPageHeader::IDD, IDS_VTLIST_PPGHEADER_CAPTION )
{
	m_nCurrent = 0;
	m_nLevels = 0;
	m_bEditChangeLock = false;
}

CVTListPropPageHeader::~CVTListPropPageHeader()
{
}

void CVTListPropPageHeader::DoDataExchange(CDataExchange* pDX)
{
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_DATA_MAP(CVTListPropPageCol)
	DDX_Control(pDX, IDC_CHECK1, m_btnUseHeader);
	DDX_Control(pDX, IDC_LIST_ROW, m_lstCollumn );
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}



BEGIN_MESSAGE_MAP(CVTListPropPageHeader, COlePropertyPage)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedUseHeader)
	ON_BN_CLICKED(IDC_BUTTON_ADD_HEADER, OnBnClickedButtonAddHeader)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_HEADER, OnBnClickedButtonDeleteHeader)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN5, OnDeltaposLevels)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN6, OnDeltaposcurLevel)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN4, OnDeltaposNoOfCol)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_ROW, OnLvnEndlabeleditListRow)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROW, OnLvnItemchangedListRow)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpinWidth)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEditWidth)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEditAssign)
	ON_EN_CHANGE(IDC_EDIT4, OnEnChangeEditCLev)
	ON_EN_CHANGE(IDC_EDIT3, OnEnChangeEditLev)
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CVTListPropPageHeader::OnInitDialog()
{
	COlePropertyPage::OnInitDialog();	

	m_nDelCol=-1;

	m_bEditChangeLock = true;

	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN5 );

	if( pSpin )
		pSpin->SetRange( 0, 5 );

	pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN6 );

	if( pSpin )
		pSpin->SetRange( 0, 0 );

	pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN4 );

	if( pSpin )
	{
		pSpin->SetRange( 1, 10 );
		pSpin->SetPos( 1 );
	}

	pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 );

	if( pSpin )
	{
		pSpin->SetRange( 1, 2000 );
		pSpin->SetPos( 100 );
	}

	m_bEditChangeLock = false;

	CRect rcList;
	m_lstCollumn.GetClientRect( &rcList );

	m_lstCollumn.InsertColumn( 0, _T( "Column" ), LVCFMT_CENTER, rcList.Width( ) );

	_fill_data_from_ctrl();


	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// CVTListPropPageHeader message handlers

void CVTListPropPageHeader::EnableWindow( UINT nId, bool bState )
{
	CWnd *pwndChild = GetDlgItem( nId );

	if( pwndChild )
		pwndChild->EnableWindow( bState );

}

void CVTListPropPageHeader::OnBnClickedUseHeader()
{
	bool bEnable = true;
	if( !m_btnUseHeader.GetCheck() )
		bEnable = false;

	m_bUseExtHeaderCol=bEnable;

	EnableWindow( IDC_STATIC_LEV, bEnable ); 
	EnableWindow( IDC_STATIC_CLEV, bEnable ); 
	EnableWindow( IDC_EDIT3, bEnable ); 
	EnableWindow( IDC_EDIT4, bEnable ); 
	EnableWindow( IDC_SPIN5, bEnable ); 
	EnableWindow( IDC_SPIN6, bEnable ); 
	EnableWindow( IDC_STATIC_CNT, bEnable ); 
	EnableWindow( IDC_LIST_ROW, bEnable ); 
	EnableWindow( IDC_STATIC_NLEV, bEnable ); 
	EnableWindow( IDC_EDIT2, bEnable ); 
	EnableWindow( IDC_SPIN4, bEnable ); 
	EnableWindow( IDC_BUTTON_ADD_HEADER, bEnable ); 
	EnableWindow( IDC_BUTTON_DELETE_HEADER, bEnable ); 

	EnableWindow( IDC_STATIC_WIDTH, bEnable ); 
	EnableWindow( IDC_EDIT1, bEnable ); 
	EnableWindow( IDC_SPIN1, bEnable ); 

	if( m_nLevels==0 )
	{
		XLevelData *pLevel = new XLevelData;
		listHeaders.add_tail( pLevel );
		m_nLevels++;
		
		CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN6 );

		m_bEditChangeLock = true;

		if( pSpin )
		{
			int nPos = pSpin->GetPos();
			pSpin->SetRange( 1, m_nLevels );
			pSpin->SetPos( min( nPos, m_nLevels ) );
			m_nCurrent = pSpin->GetPos() - 1;
		}
	
		m_bEditChangeLock = false;
	
		_update_column_info();
	}
	_update_ctrl();
	
	m_bUpdNameCol=1;
}

void CVTListPropPageHeader::OnDeltaposLevels(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here

	bool bUp = ( pNMUpDown->iDelta > 0 );

	if( bUp )
	{
		if( pNMUpDown->iPos >= listHeaders.count() )
		{
			XLevelData *pLevel = new XLevelData;
			listHeaders.add_tail( pLevel );
		}
		m_nLevels++;
	}
	else
		m_nLevels--;

	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN6 );

	m_bEditChangeLock = true;

	if( pSpin )
	{
		int nPos = pSpin->GetPos();
		pSpin->SetRange( 1, m_nLevels );
		pSpin->SetPos( min( nPos, m_nLevels ) );
		m_nCurrent = pSpin->GetPos() - 1;
	}

	m_bEditChangeLock = false;

	_update_column_info();

	_update_ctrl();
	*pResult = 0;
}

void CVTListPropPageHeader::_update_column_info()
{
	m_lstCollumn.DeleteAllItems();
	int i = 0;
	for (TPOS lPos = listHeaders.head(); lPos; lPos = listHeaders.next(lPos), i++)
	{
		if( i ==  m_nCurrent && i < m_nLevels )
		{
			XLevelData *pLevelData = listHeaders.get( lPos );

			if( pLevelData )
			{
				int q = 0;
				for (TPOS lPos2 = pLevelData->listData.head(); lPos2; lPos2 = pLevelData->listData.next(lPos2), q++)
				{
					XLevelData::XData *pdata = pLevelData->listData.get( lPos2 );

					m_lstCollumn.InsertItem( q, pdata->strName );
				}
			}
		}
	}
	LRESULT lResult = 0;
	OnLvnItemchangedListRow(0, &lResult );
}

void CVTListPropPageHeader::OnDeltaposcurLevel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	bool bUp = ( pNMUpDown->iDelta > 0 );
	if( !bUp && m_nCurrent > 0 || bUp && m_nCurrent < ( m_nLevels - 1 ) )
	{
		if( bUp )
			m_nCurrent++;
		else
			m_nCurrent--;
	}
	_update_column_info();


	*pResult = 0;
}

void CVTListPropPageHeader::OnBnClickedButtonAddHeader()
{
	int i = 0;
	for( TPOS lPos = listHeaders.head(); lPos; lPos = listHeaders.next( lPos ), i++ )
	{
		if( i == m_nCurrent && i < m_nLevels  )
		{
			XLevelData *pLevelData = listHeaders.get( lPos );

			if( pLevelData )
			{
				CString strCaption;
				strCaption.Format( "%s%d", "Column", pLevelData->listData.count() + 1 );

				XLevelData::XData *pdata = new XLevelData::XData;

				pdata->strName = strCaption;
				pdata->nAssign = 1;
				pdata->nWidth = 100;

				pLevelData->listData.add_tail( pdata );

				m_lstCollumn.InsertItem( pLevelData->listData.count() + 1, pdata->strName );

				if(pLevelData->listData.count()==1)
					m_FirstColumn=true;
				else
					m_FirstColumn=false;

			}
		}
	}
	_update_ctrl();
	m_bUpdNameCol=1;
}

void CVTListPropPageHeader::OnBnClickedButtonDeleteHeader()
{
	short nIndex = m_lstCollumn.GetNextItem( -1, LVNI_SELECTED );	
	if(nIndex < 0) return;	

	int i = 0;
	for( TPOS lPos = listHeaders.head(); lPos; lPos = listHeaders.next( lPos ), i++ )
	{
		if( i == m_nCurrent && i < m_nLevels  )
		{
			XLevelData *pLevelData = listHeaders.get( lPos );

			if( pLevelData )
			{
				int q = 0;
				for( TPOS lPos2 = pLevelData->listData.head(); lPos2; lPos2 = pLevelData->listData.next( lPos2 ), q++ )
				{
					if( q == nIndex )
					{
						m_nDelCol=nIndex;
						pLevelData->listData.remove( lPos2 );
						_update_column_info();
						_update_ctrl();
						m_bUpdNameCol=1;
						m_nDelCol=-1;
						return;
					}
				}
			}
		}
	}
}

void CVTListPropPageHeader::OnLvnEndlabeleditListRow(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	short index = m_lstCollumn.GetNextItem( -1, LVNI_SELECTED );	
	if(index < 0) return;	


	if( !pDispInfo->item.pszText || pDispInfo->item.pszText == "" ) return;
	
	CString str = pDispInfo->item.pszText;

	m_lstCollumn.SetItemText( pDispInfo->item.iItem, 0, pDispInfo->item.pszText );

	XLevelData::XData *pdata = _get_item( m_lstCollumn.GetNextItem( -1, LVNI_SELECTED ) );
	if( pdata	)
		pdata->strName =str;

	_update_ctrl();
	m_bUpdNameCol=1;
}

CVTListPropPageHeader::XLevelData::XData *CVTListPropPageHeader::_get_item( int nIndex )
{
	int i = 0;
	for( TPOS lPos = listHeaders.head(); lPos; lPos = listHeaders.next( lPos ), i++ )
	{
		if( i == m_nCurrent && i < m_nLevels  )
		{
			XLevelData *pLevelData = listHeaders.get( lPos );

			if( pLevelData )
			{
				int q = 0;
				for( TPOS lPos2 = pLevelData->listData.head(); lPos2; lPos2 = pLevelData->listData.next( lPos2 ), q++ )
				{
					if( q == nIndex )
					{
						XLevelData::XData *pdata = pLevelData->listData.get( lPos2 );
						return pdata;
					}
				}
			}
		}
	}
	return 0;
}


void CVTListPropPageHeader::OnDeltaposNoOfCol(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	XLevelData::XData *pdata = _get_item( m_lstCollumn.GetNextItem( -1, LVNI_SELECTED ) );
	if( pdata )
		pdata->nAssign = pNMUpDown->iDelta + pNMUpDown->iPos;

	_update_ctrl();

	*pResult = 0;
}
void CVTListPropPageHeader::OnLvnItemchangedListRow(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	XLevelData::XData *pdata = _get_item( m_lstCollumn.GetNextItem( -1, LVNI_SELECTED ) );
	
	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN4 );

	m_bEditChangeLock = true;

	if( pSpin )
		if( pdata )
			pSpin->SetPos( pdata->nAssign );
		else
			pSpin->SetPos( 1 );

	pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 );

	if( pSpin )
		if( pdata )
			pSpin->SetPos( pdata->nWidth );
		else
			pSpin->SetPos( 100 );

	m_bEditChangeLock = false;


	*pResult = 0;
}


void CVTListPropPageHeader::_update_ctrl()
{
 	if( !_exec( GetDispatch(), ((OLECHAR*)(L"ClearExtHeaders")) ) )		
		return;

	if( !_exec_set( GetDispatch(), ((OLECHAR*)(L"SetUseExtHeader")), m_btnUseHeader.GetCheck()  ) )		
		return;
	int i = 0;
	for( TPOS lPos = listHeaders.head(); i < m_nLevels && lPos; lPos = listHeaders.next( lPos ), i++ )
	{
		XLevelData *pLevelData = listHeaders.get( lPos );

		if( pLevelData )
		{
			int q = 0;
			for( TPOS lPos2 = pLevelData->listData.head(); lPos2; lPos2 = pLevelData->listData.next( lPos2 ), q++ )
			{
				XLevelData::XData *pdata = pLevelData->listData.get( lPos2 );

				if(m_FirstColumn)
					m_nDelCol=-2;
				if( !_exec_set_data( GetDispatch(), ((OLECHAR*)(L"SetExtHeaderData")), i, q,  pdata->strName, pdata->nAssign, pdata->nWidth, m_nDelCol ) )		
				{
					return;
			}
				else
				{
					m_FirstColumn=false;
					m_nDelCol=-1;
				}
			}
		}
	}

}

void CVTListPropPageHeader::_fill_data_from_ctrl()
{
	int lUse = 0; 
	if( !_exec_get( GetDispatch(), ((OLECHAR*)(L"GetUseExtHeader")), lUse  ) )	
		return;

	m_btnUseHeader.SetCheck( lUse );
	m_bUseExtHeaderCol=lUse;

	int nCol = 0;

	if( !_exec_get( GetDispatch(), ((OLECHAR*)(L"GetExtHeadersSize")), nCol ) )		
		return;

	m_nLevels = nCol;

	for(int i = 0; i < nCol; i++ )
	{
		XLevelData *pLevel = new XLevelData;
		listHeaders.add_tail( pLevel );
	}
	{
	int i = 0;
	for (TPOS lPos = listHeaders.head(); i < m_nLevels && lPos; lPos = listHeaders.next(lPos), i++)
	{
		XLevelData *pLevelData = listHeaders.get(lPos);

		if (pLevelData)
		{
			int nCol2 = 0;

			if (!_exec_get2(GetDispatch(), ((OLECHAR*)(L"GetExtHeadersColumnSize")), i, nCol2))
				return;

			for (int t = 0; t < nCol2; t++)
			{
				XLevelData::XData *pLevel = new XLevelData::XData;
				pLevelData->listData.add_tail(pLevel);
			}
			int q = 0;
			for (TPOS lPos2 = pLevelData->listData.head(); lPos2; lPos2 = pLevelData->listData.next(lPos2), q++)
			{
				XLevelData::XData *pdata = pLevelData->listData.get(lPos2);

				if (!_exec_get_data(GetDispatch(), ((OLECHAR*)(L"GetExtHeaderName")), i, q, pdata->strName))
					return;
				if (!_exec_get_data(GetDispatch(), ((OLECHAR*)(L"GetExtHeaderAssignment")), i, q, pdata->nAssign))
					return;
				if (!_exec_get_data(GetDispatch(), ((OLECHAR*)(L"GetExtHeaderWidth")), i, q, pdata->nWidth))
					return;
			}
		}
	}}
	
	_update_column_info();
	OnBnClickedUseHeader();

	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN6 );

	m_bEditChangeLock = true;
	if( pSpin )
	{
		int nPos = pSpin->GetPos();
		pSpin->SetRange( 1, m_nLevels );
		pSpin->SetPos( min( nPos, m_nLevels ) );
		m_nCurrent = pSpin->GetPos() - 1;
	}

	pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN5 );

	if( pSpin )
		pSpin->SetPos( m_nLevels );
	m_bEditChangeLock = false;
}


LPDISPATCH CVTListPropPageHeader::GetDispatch()
{
	ULONG Ulong;
	LPDISPATCH *lpDispatch = NULL;

	lpDispatch = GetObjectArray(&Ulong);
	
	if( !lpDispatch[0] )
	{
		ReportError( "Can't find PropertyPage dispatch interface" , 0 );
		return FALSE;	
	}

	return lpDispatch[0];
	
}

BOOL CVTListPropPageHeader::_exec(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames )
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	

	ZeroMemory( &dispparams, sizeof(dispparams) );

	dispparams.rgvarg			= 0;
	dispparams.cArgs			= 0;
	dispparams.cNamedArgs		= 0;
	

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					NULL,
					NULL,
					NULL
					);

	if( hr == S_OK ){		
		return TRUE;
	}

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	
}

BOOL CVTListPropPageHeader::_exec_set(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, int nVal )
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[1];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );
	

	V_VT(&varg[0]) = VT_I2;
	V_I2(&varg[0]) = nVal;	    

	dispparams.rgvarg			= varg;
	dispparams.cArgs				= 1;
	dispparams.cNamedArgs		= 0;
	

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					NULL,
					NULL,
					NULL
					);

	if( hr == S_OK ){		
		return TRUE;
	}

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	
}

BOOL CVTListPropPageHeader::_exec_set_data(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, int nlev, int nItem, CString strname, int nAssign, int nWidth, int nDel )
{
	if( !lpDispatch ) return FALSE;		
	
	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[6];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );

	V_VT(&varg[0]) = VT_I2;
	V_I2(&varg[0]) = nDel;

	V_VT(&varg[1]) = VT_I2;
	V_I2(&varg[1]) = nWidth;

	V_VT(&varg[2]) = VT_I2;
	V_I2(&varg[2]) = nAssign;

	V_VT(&varg[3]) = VT_BSTR;
	V_BSTR(&varg[3]) = strname.AllocSysString();

	V_VT(&varg[4]) = VT_I2;
	V_I2(&varg[4]) = nItem;

	V_VT(&varg[5]) = VT_I2;
	V_I2(&varg[5]) = nlev;	    

	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 6;
	dispparams.cNamedArgs		= 0;

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					NULL,
					NULL,
					NULL
					);

	::SysFreeString( V_BSTR(&varg[3]) );

	if( hr == S_OK ){		
		return TRUE;
	}

	ReportError( GetInokeError(ppszNames), hr );

	return FALSE;	
}

BOOL CVTListPropPageHeader::_exec_get(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, int &nVal )
{
	if( !lpDispatch ) return FALSE;		

	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	

	ZeroMemory( &dispparams, sizeof(dispparams) );
	
	dispparams.rgvarg			= 0;
	dispparams.cArgs			= 0;
	dispparams.cNamedArgs		= 0;
	

	VARIANT vt;
	ZeroMemory( &vt, sizeof(vt) );

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					&vt,
					NULL,
					NULL
					);

	if( hr == S_OK ){
		nVal = vt.iVal;
		return TRUE;
	}	
	
	ReportError( GetInokeError(ppszNames), hr );


	return FALSE;	
}

BOOL CVTListPropPageHeader::_exec_get2(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, int nLev, int &nVal )
{
	if( !lpDispatch ) return FALSE;		

	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[1];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );

	V_VT(&varg[0]) = VT_I2;
	V_I2(&varg[0]) = nLev;

	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 1;
	dispparams.cNamedArgs		= 0;

	VARIANT vt;
	ZeroMemory( &vt, sizeof(vt) );

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					&vt,
					NULL,
					NULL
					);

	if( hr == S_OK ){
		nVal = vt.iVal;
		return TRUE;
	}	
	
	ReportError( GetInokeError(ppszNames), hr );


	return FALSE;	
}

BOOL CVTListPropPageHeader::_exec_get_data(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, int nlev, int nItem, CString &str )
{
	if( !lpDispatch ) return FALSE;		

	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[2];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );
	

	V_VT(&varg[0]) = VT_I2;
	V_I2(&varg[0]) = nItem;	    

	V_VT(&varg[1]) = VT_I2;
	V_I2(&varg[1]) = nlev;	    


	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 2;
	dispparams.cNamedArgs		= 0;
	

	VARIANT vt;
	ZeroMemory( &vt, sizeof(vt) );

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					&vt,
					NULL,
					NULL
					);


	if( hr == S_OK ){
		str = (char*)_bstr_t( vt.bstrVal );
		return TRUE;
	}	
	
	ReportError( GetInokeError(ppszNames), hr );


	return FALSE;	
}

BOOL CVTListPropPageHeader::_exec_get_data(LPDISPATCH lpDispatch, OLECHAR FAR* ppszNames, int nlev, int nItem, int &nVal )
{
	if( !lpDispatch ) return FALSE;		

	DISPID dispID;	
	if( !GetDISPIDFromName( &ppszNames, dispID, lpDispatch ) ) return FALSE;

	DISPPARAMS dispparams;	
    VARIANTARG varg[2];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );
	

	V_VT(&varg[0]) = VT_I2;
	V_I2(&varg[0]) = nItem;	    

	V_VT(&varg[1]) = VT_I2;
	V_I2(&varg[1]) = nlev;	    


	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 2;
	dispparams.cNamedArgs		= 0;
	

	VARIANT vt;
	ZeroMemory( &vt, sizeof(vt) );

	HRESULT hr = lpDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					&vt,
					NULL,
					NULL
					);


	if( hr == S_OK ){
		nVal = vt.iVal;
		return TRUE;
	}	
	
	ReportError( GetInokeError(ppszNames), hr );


	return FALSE;	
}
void CVTListPropPageHeader::OnDeltaposSpinWidth(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	XLevelData::XData *pdata = _get_item( m_lstCollumn.GetNextItem( -1, LVNI_SELECTED ) );
	if( pdata )
		pdata->nWidth = pNMUpDown->iDelta + pNMUpDown->iPos;

	_update_ctrl();
	*pResult = 0;
}

void CVTListPropPageHeader::OnEnChangeEditWidth()
{
	if( m_bEditChangeLock )
		return;

	if( !m_lstCollumn.m_hWnd )
		return;


	CWnd *pWnd = GetDlgItem( IDC_EDIT1 );

	CString str;
	pWnd->GetWindowText( str );

	XLevelData::XData *pdata = _get_item( m_lstCollumn.GetNextItem( -1, LVNI_SELECTED ) );
	if( pdata )
		pdata->nWidth = atoi( str );

	CSpinButtonCtrl *pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 );

	if( pSpin )
		if( pdata )
			pSpin->SetPos( pdata->nWidth );

	_update_ctrl();
}

void CVTListPropPageHeader::OnEnChangeEditAssign()
{
	if( m_bEditChangeLock )
		return;
	if( !m_lstCollumn.m_hWnd )
		return;

	CWnd *pWnd = GetDlgItem( IDC_EDIT2 );

	CString str;
	pWnd->GetWindowText( str );

	XLevelData::XData *pdata = _get_item( m_lstCollumn.GetNextItem( -1, LVNI_SELECTED ) );
	if( pdata )
		pdata->nAssign = atoi( str );

	CSpinButtonCtrl *pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN4 );

	if( pSpin )
		if( pdata )
			pSpin->SetPos( pdata->nAssign );

	_update_ctrl();
}

void CVTListPropPageHeader::OnEnChangeEditCLev()
{
	if( m_bEditChangeLock )
		return;
	if( !m_lstCollumn.m_hWnd )
		return;

	CWnd *pWnd = GetDlgItem( IDC_EDIT4 );

	CString str;
	pWnd->GetWindowText( str );

	CSpinButtonCtrl *pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN6 );

	if( pSpin )
	{
		pSpin->SetRange( 1, m_nLevels );
		pSpin->SetPos( min( atoi( str ), m_nLevels ) );
		m_nCurrent = pSpin->GetPos() - 1;
	}

	_update_ctrl();
}

void CVTListPropPageHeader::OnEnChangeEditLev()
{
	if( m_bEditChangeLock )
		return;
	if( !m_lstCollumn.m_hWnd )
		return;

	CWnd *pWnd = GetDlgItem( IDC_EDIT3 );

	CString str;
	pWnd->GetWindowText( str );

	CSpinButtonCtrl *pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN5 );

	if( pSpin )
		pSpin->SetPos( m_nLevels = atoi( str ) );

	_update_ctrl();
}

void CVTListPropPageCol::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call COlePropertyPage::OnPaint() for painting messages
	if(m_bUpdNameCol==1)
	{
		BuildColumnList();
		m_bUpdNameCol=0;
	}
}

void CVTListPropPageHeader::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call COlePropertyPage::OnPaint() for painting messages
	if(m_HeaderAction.GetCount()>0)
	{
		for(int j=0;j<m_HeaderAction.GetCount();j++)
		{
			int valueAction=m_HeaderAction.GetAt(j);
			if(valueAction>=1)
			{
				int i=0;
				for( TPOS lPos = listHeaders.head(); i < m_nLevels && lPos; lPos = listHeaders.next( lPos ), i++ )
				{
					XLevelData *pLevelData = listHeaders.get( lPos );
					if( pLevelData )
					{
						if(valueAction==2 && m_lstCollumn.GetItemCount()>0)
						{
							XLevelData::XData *pLevel = new XLevelData::XData;
							pLevelData->listData.add_tail( pLevel );
						}
						int q = 0;
						for( TPOS lPos2 = pLevelData->listData.head(); lPos2; lPos2 = pLevelData->listData.next( lPos2 ), q++ )
						{
							XLevelData::XData *pdata = pLevelData->listData.get( lPos2 );
							if( !_exec_get_data( GetDispatch(), ((OLECHAR*)(L"GetExtHeaderName")), i, q,  pdata->strName ) )
								return;
							if( !_exec_get_data( GetDispatch(), ((OLECHAR*)(L"GetExtHeaderAssignment")), i, q,  pdata->nAssign  ) )
								return;
							if( !_exec_get_data( GetDispatch(), ((OLECHAR*)(L"GetExtHeaderWidth")), i, q,  pdata->nWidth  ) )
								return;
						}
						if(valueAction>=3 && m_lstCollumn.GetItemCount()>0)
						{
							int q = 0;
							for( TPOS lPos2 = pLevelData->listData.head(); lPos2; lPos2 = pLevelData->listData.next( lPos2 ), q++ )
							{
								if( q == valueAction-3 )
								{
									pLevelData->listData.remove( lPos2 );
									break;
								}
							}
						}
					}
				}
			}
		}
		m_bUpdNameHeader=0;
		_update_column_info();
		_update_ctrl();
		m_HeaderAction.RemoveAll();
	}
}
