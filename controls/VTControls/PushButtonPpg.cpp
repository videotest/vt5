// PushButtonPpg.cpp : Implementation of the CPushButtonPropPage property page class.

#include "stdafx.h"
#include "VTControls.h"
#include "PushButtonPpg.h"
#include "utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CPushButtonPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CPushButtonPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CPushButtonPropPage)
	ON_BN_CLICKED(IDC_BUTTON_FILEOPEN, OnButtonFileopen)
	ON_BN_CLICKED(IDC_CHECK_AUTOSIZE, OnCheckAutosize)
	ON_BN_CLICKED(IDC_CHECK_FLATBUTTON, OnCheckFlatbutton)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_SIZE, OnAutoSize)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD, OnReloadBitmap)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//	ON_BN_CLICKED(IDC_CHECKSHADOW, OnCheckshadow)

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPushButtonPropPage, "PUSHBUTTON.PushButtonPropPage.1",
	0x54a0bf87, 0x9a92, 0x11d3, 0xb1, 0xc5, 0xeb, 0x19, 0xbd, 0xba, 0xba, 0x39)


/////////////////////////////////////////////////////////////////////////////
// CPushButtonPropPage::CPushButtonPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CPushButtonPropPage

BOOL CPushButtonPropPage::CPushButtonPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_PUSHBUTTON_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CPushButtonPropPage::CPushButtonPropPage - Constructor

CPushButtonPropPage::CPushButtonPropPage() :
	COlePropertyPage(IDD, IDS_PUSHBUTTON_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CPushButtonPropPage)
	m_strFileName = _T("");
	m_nWidth = 0;
	m_nHeight = 0;
	m_bAutoSize = FALSE;
	m_bMouseInShadow = FALSE;
	m_bMouseOutShadow = FALSE;
	m_bButtonPressedShadow = FALSE;
	m_bFlatButton = FALSE;
	m_bButtonPressedPicColor = FALSE;
	m_bMouseOutPicColor = FALSE;
	m_bMouseInPicColor = FALSE;
	m_bDrawBorder = FALSE;
	m_bThreeStateButton = FALSE;
	m_nButtonDepth = 0;
	m_bDrawFocusRect = FALSE;
	m_bStretch = FALSE;
	m_bRunAction = FALSE;
	m_strActionName = _T("");
	m_bAutoRepeat = FALSE;
	m_bDrawFocusedRect = FALSE;
	m_bSemiflat = FALSE;
	m_bUseSystemFont = FALSE;
	m_bShowToolTip = FALSE;
	m_sToolTip = _T("");
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CPushButtonPropPage::DoDataExchange - Moves data between page and properties

void CPushButtonPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CPushButtonPropPage)
	DDX_Control(pDX, IDC_CHECK_STRETCH, m_checkStretch);
	DDX_Control(pDX, IDC_CHECK_DRAWBORDER, m_checkDrawBorder);
	DDX_Control(pDX, IDC_SPIN_SHADOW, m_spinShadowSize);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_editWidth);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_editHeight);
	DDX_Control(pDX, IDC_BUTTON_FILEOPEN, m_btnBrowse);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_editFileName);
	DDP_Text(pDX, IDC_EDIT_FILENAME, m_strFileName, _T("BMPFileName") );
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
	DDV_MaxChars(pDX, m_strFileName, 255);
	DDP_Text(pDX, IDC_EDIT_WIDTH, m_nWidth, _T("ButtonWidth") );
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_nWidth);
	DDV_MinMaxInt(pDX, m_nWidth, 0, 1000);
	DDP_Text(pDX, IDC_EDIT_HEIGHT, m_nHeight, _T("ButtonHeight") );
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_nHeight);
	DDV_MinMaxInt(pDX, m_nHeight, 0, 1000);
	DDP_Check(pDX, IDC_CHECK_AUTOSIZE, m_bAutoSize, _T("AutoSize") );
	DDX_Check(pDX, IDC_CHECK_AUTOSIZE, m_bAutoSize);
	DDP_Check(pDX, IDC_CHECK_MOUSEIN_SHADOW, m_bMouseInShadow, _T("MouseInShadow") );
	DDX_Check(pDX, IDC_CHECK_MOUSEIN_SHADOW, m_bMouseInShadow);
	DDP_Check(pDX, IDC_CHECK_MOUSEOUT_SHADOW, m_bMouseOutShadow, _T("MouseOutShadow") );
	DDX_Check(pDX, IDC_CHECK_MOUSEOUT_SHADOW, m_bMouseOutShadow);
	DDP_Check(pDX, IDC_CHECK_BPRESSED_SHADOW, m_bButtonPressedShadow, _T("ButtonPressedShadow") );
	DDX_Check(pDX, IDC_CHECK_BPRESSED_SHADOW, m_bButtonPressedShadow);
	DDP_Check(pDX, IDC_CHECK_FLATBUTTON, m_bFlatButton, _T("FlatButton") );
	DDX_Check(pDX, IDC_CHECK_FLATBUTTON, m_bFlatButton);
	DDP_Check(pDX, IDC_CHECK_BPRESSED_COLOR, m_bButtonPressedPicColor, _T("ButtonPressedPicColor") );
	DDX_Check(pDX, IDC_CHECK_BPRESSED_COLOR, m_bButtonPressedPicColor);
	DDP_Check(pDX, IDC_CHECK_MOUSEOUT_COLOR, m_bMouseOutPicColor, _T("MouseOutPicColor") );
	DDX_Check(pDX, IDC_CHECK_MOUSEOUT_COLOR, m_bMouseOutPicColor);
	DDP_Check(pDX, IDC_CHECK_MOUSEIN_COLOR, m_bMouseInPicColor, _T("MouseInPicColor") );
	DDX_Check(pDX, IDC_CHECK_MOUSEIN_COLOR, m_bMouseInPicColor);
	DDP_Check(pDX, IDC_CHECK_DRAWBORDER, m_bDrawBorder, _T("DrawBorder") );
	DDX_Check(pDX, IDC_CHECK_DRAWBORDER, m_bDrawBorder);
	DDP_Check(pDX, IDC_CHECKTREESTATE, m_bThreeStateButton, _T("ThreeStateButton") );
	DDX_Check(pDX, IDC_CHECKTREESTATE, m_bThreeStateButton);
	DDP_Text(pDX, IDC_EDIT_BUTTONDEPTH, m_nButtonDepth, _T("ButtonDepth") );
	DDX_Text(pDX, IDC_EDIT_BUTTONDEPTH, m_nButtonDepth);
	DDV_MinMaxInt(pDX, m_nButtonDepth, 0, 4);
	DDP_Check(pDX, IDC_CHECK_FOCUSRECT, m_bDrawFocusRect, _T("DrawFocusRect") );
	DDX_Check(pDX, IDC_CHECK_FOCUSRECT, m_bDrawFocusRect);
	DDP_Check(pDX, IDC_CHECK_STRETCH, m_bStretch, _T("Stretch") );
	DDX_Check(pDX, IDC_CHECK_STRETCH, m_bStretch);
	DDP_Check(pDX, IDC_CHECK_IS_RUN_ACTION, m_bRunAction, _T("RunAction") );
	DDX_Check(pDX, IDC_CHECK_IS_RUN_ACTION, m_bRunAction);
	DDP_Text(pDX, IDC_EDIT_ACTION_NAME, m_strActionName, _T("ActionName") );
	DDX_Text(pDX, IDC_EDIT_ACTION_NAME, m_strActionName);
	DDP_Check(pDX, IDC_AUTO_REPEAT, m_bAutoRepeat, _T("AutoRepeat") );
	DDX_Check(pDX, IDC_AUTO_REPEAT, m_bAutoRepeat);
	DDP_Check(pDX, IDC_DRAW_FOCUSED_RECT, m_bDrawFocusedRect, _T("DrawFocusedRect") );
	DDX_Check(pDX, IDC_DRAW_FOCUSED_RECT, m_bDrawFocusedRect);
	DDP_Check(pDX, IDC_SEMIFLAT, m_bSemiflat, _T("SemiFlat") );
	DDX_Check(pDX, IDC_SEMIFLAT, m_bSemiflat);
	DDP_Check(pDX, IDC_SYSTEM_FONT, m_bUseSystemFont, _T("UseSystemFont") );
	DDX_Check(pDX, IDC_SYSTEM_FONT, m_bUseSystemFont);
	DDP_Check(pDX, IDC_SHOWTOOLTIP, m_bShowToolTip, _T("ShowToolTip") );
	DDX_Check(pDX, IDC_SHOWTOOLTIP, m_bShowToolTip);
	DDP_Text(pDX, IDC_EDIT_TOOLTIP, m_sToolTip, _T("ToolTip") );
	DDX_Text(pDX, IDC_EDIT_TOOLTIP, m_sToolTip);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CPushButtonPropPage message handlers

BOOL CPushButtonPropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	//VERIFY(m_btnBrowse.AutoLoad(IDC_BUTTON_FILEOPEN, this));
	m_btnBrowse.SetIcon(LoadIcon(AfxGetInstanceHandle( ), "IDI_ICON"));

	((CButton*)GetDlgItem(IDC_BUTTON_RELOAD))->
		SetIcon(AfxGetApp()->LoadIcon( IDI_ICON_RELOAD ) );
	

	m_spinShadowSize.SetRange(0, 4);
	//OnCheckshadow();
	_OnCheckAutosize( FALSE );
	_OnCheckFlatbutton( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPushButtonPropPage::OnButtonFileopen() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
	UpdateData(TRUE);
	strcpy(dlg.m_ofn.lpstrFile, m_strFileName);
	dlg.m_ofn.lpstrFilter = "Bitmap files(*.bmp)\0*.bmp\0\0";
	if(dlg.DoModal() == IDOK)
	{
		m_strFileName = dlg.GetPathName();
		m_editFileName.SetWindowText(m_strFileName);
		UpdateData(TRUE);
	}
	
}


void CPushButtonPropPage::_OnCheckAutosize( BOOL bSave )
{
	if( bSave ) UpdateData(TRUE);

	//m_editHeight.EnableWindow(m_bAutoSize == FALSE);
	//m_editWidth.EnableWindow(m_bAutoSize == FALSE);
	//m_checkStretch.EnableWindow(m_bAutoSize == FALSE);
}


void CPushButtonPropPage::OnCheckAutosize() 
{
	_OnCheckAutosize( TRUE );	
}

void CPushButtonPropPage::OnCheckshadow() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//m_editShadowSize.EnableWindow(m_bPicShadow == TRUE);
	//m_spinShadowSize.EnableWindow(m_bPicShadow == TRUE);
}


void CPushButtonPropPage::_OnCheckFlatbutton( BOOL bSave )
{
	if( bSave ) UpdateData(TRUE);

	if(m_bFlatButton == FALSE)
	{
		m_bDrawBorder = FALSE;
	}
	m_checkDrawBorder.EnableWindow(m_bFlatButton == TRUE);
}

void CPushButtonPropPage::OnCheckFlatbutton() 
{
	_OnCheckFlatbutton( TRUE );
}


void CPushButtonPropPage::OnAutoSize() 
{
	ULONG Ulong;
	LPDISPATCH *lpDispatch	= NULL;
	LPDISPATCH pDispatch	= NULL;

	//Find object dispatch
	lpDispatch = GetObjectArray(&Ulong);	
	
	if( (!lpDispatch) || (!lpDispatch[0]) )
	{									   
		AfxMessageBox( "Can't find object dispatch interface" , 
			MB_OK | MB_ICONSTOP );
		return;
	}

	pDispatch = lpDispatch[0];

	InvokeByName( pDispatch, "SetAutoSize" );	
	UpdateData( FALSE );
}

void CPushButtonPropPage::OnReloadBitmap() 
{
	ULONG Ulong;
	LPDISPATCH *lpDispatch	= NULL;
	LPDISPATCH pDispatch	= NULL;

	//Find object dispatch
	lpDispatch = GetObjectArray(&Ulong);	
	
	if( (!lpDispatch) || (!lpDispatch[0]) )
	{									   
		AfxMessageBox( "Can't find object dispatch interface" , 
			MB_OK | MB_ICONSTOP );
		return;
	}

	pDispatch = lpDispatch[0];

	::InvokeByName( pDispatch, "ReloadBitmap" );	
	UpdateData( FALSE );
}


void CPushButtonPropPage::OnPaste() 
{
	ULONG Ulong;
	LPDISPATCH *lpDispatch	= NULL;
	LPDISPATCH pDispatch	= NULL;

	//Find object dispatch
	lpDispatch = GetObjectArray(&Ulong);	
	
	if( (!lpDispatch) || (!lpDispatch[0]) )
	{									   
		AfxMessageBox( "Can't find object dispatch interface" , 
			MB_OK | MB_ICONSTOP );
		return;
	}

	pDispatch = lpDispatch[0];

	InvokeByName( pDispatch, "PasteFromClipboard" );		
	
}

void CPushButtonPropPage::OnCopy() 
{
	ULONG Ulong;
	LPDISPATCH *lpDispatch	= NULL;
	LPDISPATCH pDispatch	= NULL;

	//Find object dispatch
	lpDispatch = GetObjectArray(&Ulong);	
	
	if( (!lpDispatch) || (!lpDispatch[0]) )
	{									   
		AfxMessageBox( "Can't find object dispatch interface" , 
			MB_OK | MB_ICONSTOP );
		return;
	}

	pDispatch = lpDispatch[0];

	InvokeByName( pDispatch, "CopyToClipboard" );		
	
}


