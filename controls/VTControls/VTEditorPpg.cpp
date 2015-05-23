// VTEditorPpg.cpp : Implementation of the CVTEditorPropPage property page class.

#include "stdafx.h"
#include "VTControls.h"
#include "VTEditorPpg.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTEditorPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTEditorPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTEditorPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTEditorPropPage, "VTCONTROLS.VTEditorPropPage.1",
	0x4d7f0929, 0x87a1, 0x4c46, 0xa3, 0x86, 0xe5, 0x75, 0xa2, 0xb9, 0x25, 0xac)


/////////////////////////////////////////////////////////////////////////////
// CVTEditorPropPage::CVTEditorPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTEditorPropPage

BOOL CVTEditorPropPage::CVTEditorPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTEDITOR_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTEditorPropPage::CVTEditorPropPage - Constructor

CVTEditorPropPage::CVTEditorPropPage() :
	COlePropertyPage(IDD, IDS_VTEDITOR_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTEditorPropPage)
	m_strName = _T("");
	m_strKey = _T("");
	m_bAutoUpdate = FALSE;
	m_bUseSystemFont = FALSE;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CVTEditorPropPage::DoDataExchange - Moves data between page and properties

void CVTEditorPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVTEditorPropPage)
	DDP_Text(pDX, IDC_EDIT_NAME, m_strName, _T("Name") );
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDP_Text(pDX, IDC_EDIT_KEY_VALUE, m_strKey, _T("Key") );
	DDX_Text(pDX, IDC_EDIT_KEY_VALUE, m_strKey);
	DDP_Check(pDX, IDC_CHECK_AUTO_UPDATE, m_bAutoUpdate, _T("AutoUpdate") );
	DDX_Check(pDX, IDC_CHECK_AUTO_UPDATE, m_bAutoUpdate);
	DDP_Check(pDX, IDC_USE_SYSTEM_FONT, m_bUseSystemFont, _T("UseSystemFont") );
	DDX_Check(pDX, IDC_USE_SYSTEM_FONT, m_bUseSystemFont);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CVTEditorPropPage message handlers
/////////////////////////////////////////////////////////////////////////////
// CVTEditorPropPageExt dialog

IMPLEMENT_DYNCREATE(CVTEditorPropPageExt, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTEditorPropPageExt, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTEditorPropPageExt)
	ON_BN_CLICKED(IDC_CHECK_CAPTION_ENABLE, OnCheckCaptionEnable)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_VALIDATION, OnCheckEnableValidation)
	ON_CBN_SELCHANGE(IDC_COMBO_DATA_TYPE, OnSelchangeComboDataType)
	ON_CBN_SELCHANGE(IDC_COMBO_CAPTION_ALIGN, OnSelchangeComboCaptionAlign)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_CAPTION_AUTO_WIDTH, OnCheckEnableCaptionAutoWidth)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_SIZE, OnSetAutoSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

// {0ED43342-FAB2-47F7-82A8-169DB741960C}
IMPLEMENT_OLECREATE_EX(CVTEditorPropPageExt, "VTCONTROLS.VTEditorPropPageExt.1",
	0xed43342, 0xfab2, 0x47f7, 0x82, 0xa8, 0x16, 0x9d, 0xb7, 0x41, 0x96, 0xc)


/////////////////////////////////////////////////////////////////////////////
// CVTEditorPropPageExt::CVTEditorPropPageExtFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTEditorPropPageExt

BOOL CVTEditorPropPageExt::CVTEditorPropPageExtFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Define string resource for page type; replace '0' below with ID.

	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTEDITOR_PPG_EXT);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTEditorPropPageExt::CVTEditorPropPageExt - Constructor

// TODO: Define string resource for page caption; replace '0' below with ID.

CVTEditorPropPageExt::CVTEditorPropPageExt() :
	COlePropertyPage(IDD, IDS_VTEDITOR_PPG_EXT_CAPTION)
{
	//{{AFX_DATA_INIT(CVTEditorPropPageExt)
	m_nCaptionAlign = -1;
	m_nDataType = -1;
	m_bCaptionEnable = FALSE;
	m_bSpinEnable = FALSE;
	m_bValidationEnable = FALSE;
	m_strCaptionText = _T("");
	m_nEditFieldWidth = 0;
	m_fMaxValue = 0.0f;
	m_fMinValue = 0.0f;
	m_bCaptionAutoWidth = FALSE;
	m_bAlignClient = FALSE;
	m_bMultiLine = FALSE;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CVTEditorPropPageExt::DoDataExchange - Moves data between page and properties

void CVTEditorPropPageExt::DoDataExchange(CDataExchange* pDX)
{
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_DATA_MAP(CVTEditorPropPageExt)
	DDP_CBIndex(pDX, IDC_COMBO_CAPTION_ALIGN, m_nCaptionAlign, _T("CaptionPosition") );
	DDX_CBIndex(pDX, IDC_COMBO_CAPTION_ALIGN, m_nCaptionAlign);
	DDP_CBIndex(pDX, IDC_COMBO_DATA_TYPE, m_nDataType, _T("DataType") );
	DDX_CBIndex(pDX, IDC_COMBO_DATA_TYPE, m_nDataType);
	DDP_Check(pDX, IDC_CHECK_CAPTION_ENABLE, m_bCaptionEnable, _T("CaptionEnable") );
	DDX_Check(pDX, IDC_CHECK_CAPTION_ENABLE, m_bCaptionEnable);
	DDP_Check(pDX, IDC_CHECK_ENABLE_SPIN, m_bSpinEnable, _T("SpinEnable") );
	DDX_Check(pDX, IDC_CHECK_ENABLE_SPIN, m_bSpinEnable);
	DDP_Check(pDX, IDC_CHECK_ENABLE_VALIDATION, m_bValidationEnable, _T("ValidationEnable") );
	DDX_Check(pDX, IDC_CHECK_ENABLE_VALIDATION, m_bValidationEnable);
	DDP_Text(pDX, IDC_EDIT_CAPTION_TEXT, m_strCaptionText, _T("CaptionText") );
	DDX_Text(pDX, IDC_EDIT_CAPTION_TEXT, m_strCaptionText);
	DDP_Text(pDX, IDC_EDIT_FIELD_WIDTH, m_nEditFieldWidth, _T("CaptionFieldWidth") );
	DDX_Text(pDX, IDC_EDIT_FIELD_WIDTH, m_nEditFieldWidth);
	DDP_Text(pDX, IDC_EDIT_MAX_VALUE, m_fMaxValue, _T("MaxValue") );
	DDX_Text(pDX, IDC_EDIT_MAX_VALUE, m_fMaxValue);
	DDP_Text(pDX, IDC_EDIT_MIN_VALUE, m_fMinValue, _T("MinValue") );
	DDX_Text(pDX, IDC_EDIT_MIN_VALUE, m_fMinValue);
	DDP_Check(pDX, IDC_CHECK_ENABLE_CAPTION_AUTO_WIDTH, m_bCaptionAutoWidth, _T("CaptionAutoWidth") );
	DDX_Check(pDX, IDC_CHECK_ENABLE_CAPTION_AUTO_WIDTH, m_bCaptionAutoWidth);
	DDP_Check(pDX, IDC_CHECK_ALIGN_CLIENT, m_bAlignClient, _T("AlignClient") );
	DDX_Check(pDX, IDC_CHECK_ALIGN_CLIENT, m_bAlignClient);
	DDP_Check(pDX, IDC_CHECK_MULTI_LINE, m_bMultiLine, _T("MultiLine") );
	DDX_Check(pDX, IDC_CHECK_MULTI_LINE, m_bMultiLine);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}
	  

/////////////////////////////////////////////////////////////////////////////
// CVTEditorPropPageExt message handlers

BOOL CVTEditorPropPageExt::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();


	CComboBox* pCombo = NULL;
	pCombo = (CComboBox*)GetDlgItem( IDC_COMBO_CAPTION_ALIGN );

	if( pCombo )
	{
		pCombo->AddString( _T("Align Up") );
		pCombo->AddString( _T("Align Left") );
	}

	pCombo = NULL;
	pCombo = (CComboBox*)GetDlgItem( IDC_COMBO_DATA_TYPE );

	if( pCombo )
	{
		pCombo->AddString( _T("String") );
		pCombo->AddString( _T("Integer") );
		pCombo->AddString( _T("Float") );
	}

	
	SetControlStateOnCangeCaptionEnable( FALSE );
	SetControlStateOnCangeEnableValidation( FALSE );
	SetControlStateOnCangeComboDataType( FALSE );
	SetControlStateOnChangeComboCaptionAlign( FALSE );
	SetControlStateEnableCaptionAutoWidth( FALSE );

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_EDIT_WIDTH))->SetRange( 0, 1000 );
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_MIN))->SetRange( -1000000, 1000000 );
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_MAX))->SetRange( -1000000, 1000000 );

	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}				

void CVTEditorPropPageExt::OnCheckCaptionEnable() 
{
	SetControlStateOnCangeCaptionEnable( TRUE ) ;
}



void CVTEditorPropPageExt::OnCheckEnableValidation() 
{
	SetControlStateOnCangeEnableValidation( TRUE ) ;
}


void CVTEditorPropPageExt::OnSelchangeComboDataType() 
{
	SetControlStateOnCangeComboDataType( TRUE ) ;
}

void CVTEditorPropPageExt::OnSelchangeComboCaptionAlign() 
{
	SetControlStateOnChangeComboCaptionAlign( TRUE ) ;
	
}

void CVTEditorPropPageExt::OnCheckEnableCaptionAutoWidth() 
{
	SetControlStateEnableCaptionAutoWidth( TRUE );	
}



void CVTEditorPropPageExt::SetControlStateOnCangeCaptionEnable( BOOL bSaveData )
{	
	if( bSaveData ) UpdateData( TRUE );
	
	if( m_bCaptionEnable )
	{
		GetDlgItem( IDC_COMBO_CAPTION_ALIGN )->EnableWindow( TRUE );
		GetDlgItem( IDC_EDIT_CAPTION_TEXT )->EnableWindow( TRUE );		
	}
	else
	{
		GetDlgItem( IDC_COMBO_CAPTION_ALIGN )->EnableWindow( FALSE );
		GetDlgItem( IDC_EDIT_CAPTION_TEXT )->EnableWindow( FALSE );		
	}

}

void CVTEditorPropPageExt::SetControlStateOnCangeEnableValidation( BOOL bSaveData )
{
	if( bSaveData ) UpdateData( TRUE );
	if( m_bValidationEnable )
	{
		GetDlgItem( IDC_EDIT_MIN_VALUE )->EnableWindow( TRUE );
		GetDlgItem( IDC_EDIT_MAX_VALUE )->EnableWindow( TRUE );
		GetDlgItem( IDC_SPIN_MIN )->EnableWindow( TRUE );
		GetDlgItem( IDC_SPIN_MAX )->EnableWindow( TRUE );			   	   
	}
	else
	{
		GetDlgItem( IDC_EDIT_MIN_VALUE )->EnableWindow( FALSE );
		GetDlgItem( IDC_EDIT_MAX_VALUE )->EnableWindow( FALSE );
		GetDlgItem( IDC_SPIN_MIN )->EnableWindow( FALSE );
		GetDlgItem( IDC_SPIN_MAX )->EnableWindow( FALSE );

	}

}

void CVTEditorPropPageExt::SetControlStateOnCangeComboDataType( BOOL bSaveData )
{
	if( bSaveData ) UpdateData( TRUE );

	if( m_nDataType == 0 ) // String
	{
		GetDlgItem( IDC_EDIT_MIN_VALUE )->EnableWindow( FALSE );
		GetDlgItem( IDC_EDIT_MAX_VALUE )->EnableWindow( FALSE );
		GetDlgItem( IDC_SPIN_MIN )->EnableWindow( FALSE );
		GetDlgItem( IDC_SPIN_MAX )->EnableWindow( FALSE );			   	  
		GetDlgItem( IDC_CHECK_ENABLE_VALIDATION )->EnableWindow( FALSE );		
	}
	else
	{
		GetDlgItem( IDC_EDIT_MIN_VALUE )->EnableWindow( TRUE );
		GetDlgItem( IDC_EDIT_MAX_VALUE )->EnableWindow( TRUE );
		GetDlgItem( IDC_SPIN_MIN )->EnableWindow( TRUE );
		GetDlgItem( IDC_SPIN_MAX )->EnableWindow( TRUE );			   	  
		GetDlgItem( IDC_CHECK_ENABLE_VALIDATION )->EnableWindow( TRUE );
	}

}

void CVTEditorPropPageExt::SetControlStateOnChangeComboCaptionAlign( BOOL bSaveData )
{
	if( bSaveData ) UpdateData( TRUE );
	if( m_nCaptionAlign == 1 )
	{
		//GetDlgItem( IDC_EDIT_FIELD_WIDTH )->EnableWindow( TRUE );
		//GetDlgItem( IDC_SPIN_EDIT_WIDTH )->EnableWindow( TRUE );
	}
	else
	{
		//GetDlgItem( IDC_EDIT_FIELD_WIDTH )->EnableWindow( FALSE );
		//GetDlgItem( IDC_SPIN_EDIT_WIDTH )->EnableWindow( FALSE );

	}
}


void CVTEditorPropPageExt::SetControlStateEnableCaptionAutoWidth( BOOL bSaveData )
{
	if( bSaveData ) UpdateData( TRUE );
	if( m_bCaptionAutoWidth )
	{
		GetDlgItem( IDC_EDIT_FIELD_WIDTH )->EnableWindow( FALSE );
		GetDlgItem( IDC_SPIN_EDIT_WIDTH )->EnableWindow( FALSE );
	}
	else
	{
		GetDlgItem( IDC_EDIT_FIELD_WIDTH )->EnableWindow( TRUE );
		GetDlgItem( IDC_SPIN_EDIT_WIDTH )->EnableWindow( TRUE );

	}

	//SetControlStateOnChangeComboCaptionAlign( TRUE );
}

void CVTEditorPropPageExt::OnSetAutoSize() 
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

	UpdateData( TRUE );
	InvokeByName( pDispatch, "SetAutoSize" );	
	//InvokeByName( pDispatch, "SetAutoSize" );	
	UpdateData( FALSE );//Set field width
}
