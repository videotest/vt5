// TemplateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "print.h"
#include "TemplateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg dialog


CTemplateDlg::CTemplateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplateDlg)
	m_nTemplateSource = 0;
	m_nAlghoritm = 0;
	m_strTemplateObjectName = _T("");
	m_strTemplateFileName = _T("");
	m_nNewTemplateTarget = 0;
	m_strTemplateName = _T("");
	//}}AFX_DATA_INIT
}


void CTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateDlg)
	DDX_Radio(pDX, IDC_TENPLATE_SOURCE, m_nTemplateSource);
	DDX_Radio(pDX, IDC_ALGHORITM, m_nAlghoritm);
	DDX_Text(pDX, IDC_TEMPLATE_NAME_IN_DOCUMENT, m_strTemplateObjectName);
	DDX_Text(pDX, IDC_TEMPLATE_NAME_IN_FILE, m_strTemplateFileName);
	DDX_Radio(pDX, IDC_GENERATE, m_nNewTemplateTarget);
	DDX_CBString(pDX, IDC_EXIST_TEMPLATE_NAME, m_strTemplateName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateDlg, CDialog)
	//{{AFX_MSG_MAP(CTemplateDlg)
	ON_BN_CLICKED(IDC_TENPLATE_SOURCE, OnTenplateSource)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_GENERATE, OnGenerate)
	ON_BN_CLICKED(IDC_GENERATE2, OnGenerate2)
	ON_BN_CLICKED(IDC_GENERATE3, OnGenerate3)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CTemplateDlg::SetDocSite( IUnknown* punkDS )
{
	m_sptrDocSite = punkDS;
}

/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg message handlers
BOOL CTemplateDlg::OnInitDialog() 
{

	CDialog::OnInitDialog();
	
	((CButton*)GetDlgItem(IDC_LOAD_BTN))->SetIcon(LoadIcon(AfxGetInstanceHandle( ), "IDI_ICON"));

	FillExistTemplates();	

	GetTemplateSettingsFromDocument();	

	UpdateData( FALSE );

	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_EXIST_TEMPLATE_NAME);
	if( pCombo && pCombo->GetSafeHwnd() )
	{
		pCombo->SetCurSel( pCombo->FindStringExact( 0, m_strTemplateName ) );				
	}
		

	

	SetWindowsEnable( false );	
	
	return TRUE;	              
}

/////////////////////////////////////////////////////////////////////////////
void CTemplateDlg::FillExistTemplates()
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_EXIST_TEMPLATE_NAME);

	if( !pCombo || !pCombo->GetSafeHwnd() )
		return;


	pCombo->ResetContent( );

	//Add *.rpt

	//Fill file based report
	CString strDir = ::GetReportDir();
	CString strMask = strDir + "*.rpt";

	CFileFind ff;

	BOOL bFound = ff.FindFile( strMask );

	while( bFound )
	{
		bFound = ff.FindNextFile();				
		pCombo->AddString( ff.GetFileName() );			
	}



	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA != NULL )
	{			

		long	lPosTemplate = 0;
		
		sptrA->GetFirstDocTemplPosition( &lPosTemplate );
		
		while( lPosTemplate )
		{
			long	lPosDoc = 0;

			sptrA->GetFirstDocPosition( lPosTemplate, &lPosDoc );

			while( lPosDoc )
			{
				IUnknown *punkDoc = 0;
				sptrA->GetNextDoc( lPosTemplate, &lPosDoc, &punkDoc );

				if( punkDoc == NULL )
					return;
				

				sptrIDataContext2 spDC( punkDoc );
				punkDoc->Release();

				if( spDC != NULL )
				{
					int nCount = -1;	

					_bstr_t bstrType(szTypeReportForm);

					long lPos = NULL;
					
					spDC->GetFirstObjectPos( bstrType, &lPos );
					while( lPos )
					{
						IUnknown* punkObject = NULL;
						spDC->GetNextObject( bstrType, &lPos, &punkObject );

						CString str = ::GetObjectName( punkObject );

						if( punkObject )
							punkObject->Release();

						pCombo->AddString( str );							
						
					}
				}
			}

			sptrA->GetNextDocTempl( &lPosTemplate, 0, 0 );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
void CTemplateDlg::GetTemplateSettingsFromDocument()
{
	
	if( m_sptrDocSite == NULL )
		return;

	m_nTemplateSource = ::GetValueInt( m_sptrDocSite, 
								REPORT_TEMPLATE_SECTION, REPORT_TEMPLATE_SOURCE, tsAutoTemplate );

	
	m_strTemplateName = ::GetValueString( m_sptrDocSite, 
								REPORT_TEMPLATE_SECTION, REPORT_EXIST_TEMPLATE_NAME, "" );

	m_nAlghoritm = ::GetValueInt( m_sptrDocSite, 
								REPORT_TEMPLATE_SECTION, REPORT_AUTO_TEMPLATE_ALGHORITM, ataActiveView );

	m_nNewTemplateTarget = ::GetValueInt( m_sptrDocSite, 
								REPORT_TEMPLATE_SECTION, REPORT_NEW_TEMPLATE_TARGET, tgtNoGenerate );
	m_strTemplateFileName = ::GetValueString( m_sptrDocSite, 
								REPORT_TEMPLATE_SECTION, REPORT_NEW_TEMPLATE_FILENAME, "" );
	m_strTemplateObjectName = ::GetValueString( m_sptrDocSite, 
								REPORT_TEMPLATE_SECTION, REPORT_NEW_TEMPLATE_OBJECTNAME, "" );
	
}

/////////////////////////////////////////////////////////////////////////////
void CTemplateDlg::SetTemplateSettingsToDocument()
{
	UpdateData( TRUE );
	
	if( m_sptrDocSite == NULL )
		return;

	::SetValue( m_sptrDocSite, 
					REPORT_TEMPLATE_SECTION, REPORT_TEMPLATE_SOURCE, (long)m_nTemplateSource );
	::SetValue( m_sptrDocSite, 
					REPORT_TEMPLATE_SECTION, REPORT_EXIST_TEMPLATE_NAME, m_strTemplateName );

	::SetValue( m_sptrDocSite, 
					REPORT_TEMPLATE_SECTION, REPORT_AUTO_TEMPLATE_ALGHORITM, (long)m_nAlghoritm );

	::SetValue( m_sptrDocSite, 
					REPORT_TEMPLATE_SECTION, REPORT_NEW_TEMPLATE_TARGET, (long)m_nNewTemplateTarget );
	::SetValue( m_sptrDocSite, 
					REPORT_TEMPLATE_SECTION, REPORT_NEW_TEMPLATE_FILENAME, m_strTemplateFileName );
	::SetValue( m_sptrDocSite, 
					REPORT_TEMPLATE_SECTION, REPORT_NEW_TEMPLATE_OBJECTNAME, m_strTemplateObjectName );
	
}


/////////////////////////////////////////////////////////////////////////////
void CTemplateDlg::ON_Wnd( UINT nIDD )
{
	CWnd* pWnd = GetDlgItem( nIDD );
	if( pWnd && pWnd->GetSafeHwnd() )
		pWnd->EnableWindow( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
void CTemplateDlg::OFF_Wnd( UINT nIDD )
{
	CWnd* pWnd = GetDlgItem( nIDD );
	if( pWnd && pWnd->GetSafeHwnd() )
		pWnd->EnableWindow( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
void CTemplateDlg::SetWindowsEnable( bool bUpdateData )
{
	if( bUpdateData )
		UpdateData( TRUE );
	
	if( m_nTemplateSource == tsUseExist )
	{
		OFF_Wnd(IDC_ALGHORITM);
		OFF_Wnd(IDC_ALGHORITM2);
		OFF_Wnd(IDC_ALGHORITM3);
		OFF_Wnd(IDC_ALGHORITM4);
		OFF_Wnd(IDC_ALGHORITM5);
		OFF_Wnd(IDC_GENERATE);
		OFF_Wnd(IDC_GENERATE2);
		OFF_Wnd(IDC_GENERATE3);
		OFF_Wnd(IDC_TEMPLATE_NAME_IN_DOCUMENT);
		OFF_Wnd(IDC_TEMPLATE_NAME_IN_FILE);
		OFF_Wnd(IDC_GENERATE_BTN);
		OFF_Wnd(IDC_LOAD_BTN);
		
		ON_Wnd(IDC_EXIST_TEMPLATE_NAME);
	}
	else
	{
		OFF_Wnd(IDC_EXIST_TEMPLATE_NAME);

		ON_Wnd(IDC_ALGHORITM);
		ON_Wnd(IDC_ALGHORITM2);
		ON_Wnd(IDC_ALGHORITM3);
		ON_Wnd(IDC_ALGHORITM4);
		ON_Wnd(IDC_ALGHORITM5);

		ON_Wnd(IDC_GENERATE);
		ON_Wnd(IDC_GENERATE2);
		ON_Wnd(IDC_GENERATE3);

		if( m_nNewTemplateTarget == tgtNoGenerate )
		{
			OFF_Wnd(IDC_TEMPLATE_NAME_IN_DOCUMENT);
			OFF_Wnd(IDC_TEMPLATE_NAME_IN_FILE);
			OFF_Wnd(IDC_GENERATE_BTN);
			OFF_Wnd(IDC_LOAD_BTN);
		}
		else
		{
			ON_Wnd(IDC_GENERATE_BTN);

			if( m_nNewTemplateTarget == tgtSaveInDocument )
			{
				ON_Wnd(IDC_TEMPLATE_NAME_IN_DOCUMENT);
				OFF_Wnd(IDC_TEMPLATE_NAME_IN_FILE);				
				OFF_Wnd(IDC_LOAD_BTN);
			}
			else
			if( m_nNewTemplateTarget == tgtSaveToFile )
			{
				OFF_Wnd(IDC_TEMPLATE_NAME_IN_DOCUMENT);
				ON_Wnd(IDC_TEMPLATE_NAME_IN_FILE);				
				ON_Wnd(IDC_LOAD_BTN);
			}
		}
	}
	
}

/////////////////////////////////////////////////////////////////////////////
void CTemplateDlg::OnTenplateSource() 
{
	SetWindowsEnable();	
}

/////////////////////////////////////////////////////////////////////////////
void CTemplateDlg::OnRadio3() 
{
	SetWindowsEnable();	
}

/////////////////////////////////////////////////////////////////////////////
void CTemplateDlg::OnGenerate() 
{
	SetWindowsEnable();
}

/////////////////////////////////////////////////////////////////////////////
void CTemplateDlg::OnGenerate2() 
{
	SetWindowsEnable();
}

/////////////////////////////////////////////////////////////////////////////
void CTemplateDlg::OnGenerate3() 
{
	SetWindowsEnable();
}

/////////////////////////////////////////////////////////////////////////////
void CTemplateDlg::OnOK() 
{
	SetTemplateSettingsToDocument();	
	CDialog::OnOK();
}


void CTemplateDlg::OnHelp() 
{
	HelpDisplay( "TemplateDlg" );
}
