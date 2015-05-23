// PrintFieldAXPpg.cpp : Implementation of the CPrintFieldAXPropPage property page class.

#include "stdafx.h"
#include "PrintFieldAX.h"
#include "PrintFieldAXPpg.h"
#include "PrintFieldAXCtl.h"
#include "ax_ctrl_misc.h"

#include "TextCtrlDlg.h"
#include "NumberCtrlDlg.h"
#include "DatePropDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CPrintFieldAXPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CPrintFieldAXPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CPrintFieldAXPropPage)
	ON_BN_CLICKED(IDC_AUTO_WIDTH, OnWidthChange)
	ON_BN_CLICKED(IDC_FIXED_WIDTH, OnWidthChange)
	ON_BN_CLICKED(IDC_PROPERTIES, OnProperties)
	ON_CBN_CLOSEUP(IDC_COMBO_FROM, OnCloseupCombo)
	ON_BN_CLICKED(IDC_CHECK_USEKEY, OnCheckUseKey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPrintFieldAXPropPage, "PRINTFIELDAX.PrintFieldAXPropPage.1",
	0x50abb1b7, 0xc1cc, 0x44d4, 0x9d, 0x10, 0xd7, 0x1e, 0xb6, 0xba, 0x56, 0x30)


/////////////////////////////////////////////////////////////////////////////
// CPrintFieldAXPropPage::CPrintFieldAXPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CPrintFieldAXPropPage

BOOL CPrintFieldAXPropPage::CPrintFieldAXPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_PRINTFIELDAX_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CPrintFieldAXPropPage::CPrintFieldAXPropPage - Constructor

CPrintFieldAXPropPage::CPrintFieldAXPropPage() :
	COlePropertyPage(IDD, IDS_PRINTFIELDAX_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CPrintFieldAXPropPage)
	//}}AFX_DATA_INIT
	ZeroMemory( &m_params, sizeof( m_params ) );
}


/////////////////////////////////////////////////////////////////////////////
// CPrintFieldAXPropPage::DoDataExchange - Moves data between page and properties

void CPrintFieldAXPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CPrintFieldAXPropPage)
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_spinWidth);
	DDX_Control(pDX, IDC_STYLE, m_comboStyles);
	DDX_Control(pDX, IDC_GROUP, m_comboGroups);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);

	if( pDX->m_bSaveAndValidate )
		OnOK();

	if( pDX->m_bSaveAndValidate )
		FirePropExchange( this );
}


/////////////////////////////////////////////////////////////////////////////
// CPrintFieldAXPropPage message handlers

IUnknown *CPrintFieldAXPropPage::GetAXInstance()
{
	unsigned long lCount = 0;
	IDispatch **ppdisp = GetObjectArray( &lCount );

	if( lCount == 0 )return 0;
	return ppdisp[0];
}

BOOL CPrintFieldAXPropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();
	m_bUseKeyToRead = false;

	// [vanek] BT2000: 3693
    long lCount = ::GetEntriesCount( ::GetAppUnknown(), "\\PrintFieldAX\\Groups" );
	for( long lPos = 0; lPos < lCount; lPos++ )  
        m_comboGroups.AddString( ::GetEntryName( ::GetAppUnknown(), "\\PrintFieldAX\\Groups", lPos ) );        
	
	IPrintFieldPtr	ptrPrintField( GetAXInstance() );
	IDispatchPtr	ptrDispatch = ptrPrintField;

	m_spinWidth.SetRange( 0, 100 );
	m_spinWidth.SetBuddy( GetDlgItem( IDC_WIDTH ) );

	if( ptrPrintField != 0 )
	{
		ptrPrintField->GetParams( &m_params );

		unsigned	type = m_params.flags & PFF_TYPE_MASKS;
		if (type == PFF_TYPE_TEXT && (m_params.flags&PFF_TEXT_STYLE_INITBYDATE) == PFF_TEXT_STYLE_INITBYDATE)
			type = PFF_TYPE_DATE;

		m_comboStyles.SetCurSel( type );

		{
			BSTR	bstrCaption = 0;
			_com_dispatch_propget( ptrDispatch, CPrintFieldAXCtrl::dispidCaption, VT_BSTR, &bstrCaption );
			_bstr_t	bstr_caption = bstrCaption;
			::SysFreeString( bstrCaption );
			SetDlgItemText( IDC_CAPTION, bstr_caption );
		}

		{
			BSTR	bstrGroup  = 0;
			_com_dispatch_propget( ptrDispatch, CPrintFieldAXCtrl::dispidGroup, VT_BSTR, &bstrGroup );
			_bstr_t	bstr_group = bstrGroup;
			::SysFreeString( bstrGroup );
			SetDlgItemText( IDC_GROUP, bstr_group );
		}

		BOOL	bAutoWidth = false;
		long	lWidth = 50;

//		_com_dispatch_propget( ptrDispatch, CPrintFieldAXCtrl::dispidCaptioAutoWidth, VT_I4, &bAutoWidth );
		_com_dispatch_propget( ptrDispatch, CPrintFieldAXCtrl::dispidCaptionWidth, VT_I4, &lWidth );

		long lAlignment = 0;
		_com_dispatch_propget( ptrDispatch, CPrintFieldAXCtrl::dispidCaptionAlignment, VT_I4, &lAlignment );

		CString	str;
		str.Format( "%d", lWidth );
		SetDlgItemText( IDC_WIDTH, str );

//		CheckRadioButton( IDC_AUTO_WIDTH, IDC_FIXED_WIDTH, bAutoWidth?IDC_AUTO_WIDTH:IDC_FIXED_WIDTH );

		((CButton *)GetDlgItem( IDC_ALIGN_LEFT ))->SetCheck( 0 );
		((CButton *)GetDlgItem( IDC_ALIGN_CENTER ))->SetCheck( 0 );
		((CButton *)GetDlgItem( IDC_ALIGN_RIGHT ))->SetCheck( 0 );
		((CButton *)GetDlgItem( IDC_FIXED_WIDTH ))->SetCheck( 0 );

		if( lAlignment == 0 )
			((CButton *)GetDlgItem( IDC_ALIGN_LEFT ))->SetCheck( 1 );
		else if( lAlignment == 1 )
			((CButton *)GetDlgItem( IDC_ALIGN_CENTER ))->SetCheck( 1 );
		else if( lAlignment == 2 )
			((CButton *)GetDlgItem( IDC_ALIGN_RIGHT ))->SetCheck( 1 );
		else if( lAlignment == -1 )
			((CButton *)GetDlgItem( IDC_FIXED_WIDTH ))->SetCheck( 1 );

		CComboBox *pBox = (CComboBox *)GetDlgItem( IDC_COMBO_FROM );

		if( pBox )
		{
			pBox->Clear();
			INamedDataPtr sptrData = ::GetAppUnknown();

			sptrData->SetupSection( _bstr_t( "\\Values" ) );

			long lCount = 0;
			sptrData->GetEntriesCount( &lCount );

			for( int i = 0; i < lCount; i++ )
			{
				BSTR bstrName = 0;
				sptrData->GetEntryName( i, &bstrName );

				if( bstrName )
				{
					int _n = 0;
					if( ( _n = pBox->FindString( 0, CString( bstrName ) ) ) == -1 )
						pBox->AddString( CString( bstrName ) );
					else
					{
						CString str;
						pBox->GetLBText( _n, str );

						if( str != CString( bstrName ) )
							pBox->AddString( CString( bstrName ) );
					}

					::SysFreeString( bstrName );
				}
			}
		}

		{
			m_strKeyToRead.Empty();
			BSTR	bstr  = 0;
			_com_dispatch_propget( ptrDispatch, CPrintFieldAXCtrl::dispidKeyToRead, VT_BSTR, &bstr );
			m_strKeyToRead = bstr;
			::SysFreeString( bstr );
		}

		if( !m_strKeyToRead.IsEmpty() && pBox )
			pBox->SelectString( 0, m_strKeyToRead );

		m_bUseKeyToRead = false;
		_com_dispatch_propget( ptrDispatch, CPrintFieldAXCtrl::dispidUseKeyToRead, VT_BOOL, &m_bUseKeyToRead );

//		m_bUseKeyToRead *= m_bUseKeyToRead;

		pBox->EnableWindow( m_bUseKeyToRead );
		CButton *pBtn = (CButton *)GetDlgItem( IDC_CHECK_USEKEY );
		pBtn->SetCheck( m_bUseKeyToRead );

		_update_controls();
	}

	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrintFieldAXPropPage::_update_controls()
{
	GetDlgItem( IDC_WIDTH )->EnableWindow( IsDlgButtonChecked( IDC_FIXED_WIDTH ) );
}

void CPrintFieldAXPropPage::OnWidthChange() 
{
	_update_controls();
}

void CPrintFieldAXPropPage::OnProperties() 
{
	int	type = m_comboStyles.GetCurSel();
	if( type == 0 )
	{
		CNumberCtrlDlg	dlg( this, &m_params );
		dlg.DoModal();
	} 
	else if( type == 1 )
	{
		CTextCtrlDlg	dlg( this, &m_params );
		dlg.DoModal();
	}
	else if( type == 2 )
	{
		CDatePropDlg dlg( this, &m_params );
		dlg.DoModal();
	}
}

void CPrintFieldAXPropPage::OnOK()
{
	char	sz[200];

	GetDlgItemText( IDC_CAPTION, sz, sizeof( sz ) );
	_bstr_t	bstr_caption = sz;
	GetDlgItemText( IDC_GROUP, sz, sizeof( sz ) );
	_bstr_t	bstr_group = sz;

	// [vanek] BT2000: 3693
	if( strlen( sz ) )
		::SetValue( GetAppUnknown(), "\\PrintFieldAX\\Groups", sz, (long)(1) );
	
	IPrintFieldPtr	ptrPrintField( GetAXInstance() );
	IDispatchPtr	ptrDispatch = ptrPrintField;

	if( ptrPrintField != 0 )
	{
		m_params.flags &= ~PFF_TYPE_MASKS;

		m_params.flags |= m_comboStyles.GetCurSel();
		if( (m_params.flags & PFF_TYPE_MASKS)==PFF_TYPE_NUMERIC )
			m_params.flags &= ~PFF_TEXT_STYLE_MASK;

		ptrPrintField->SetParams( &m_params );


		BOOL	bAutoWidth = IsDlgButtonChecked( IDC_AUTO_WIDTH );
		long	lWidth = 50;
		GetDlgItemText( IDC_WIDTH, sz, sizeof( sz ) );
		sscanf( sz, "%d", &lWidth );

		_com_dispatch_propput( ptrDispatch, CPrintFieldAXCtrl::dispidCaption, VT_BSTR, (BSTR)bstr_caption );
		_com_dispatch_propput( ptrDispatch, CPrintFieldAXCtrl::dispidGroup, VT_BSTR, (BSTR)bstr_group );
//		_com_dispatch_propput( ptrDispatch, CPrintFieldAXCtrl::dispidCaptioAutoWidth, VT_I4, bAutoWidth );
		_com_dispatch_propput( ptrDispatch, CPrintFieldAXCtrl::dispidCaptionWidth, VT_I4, lWidth );
		_com_dispatch_propput( ptrDispatch, CPrintFieldAXCtrl::dispidKeyToRead, VT_BSTR, (BSTR)_bstr_t( m_strKeyToRead ) );
		_com_dispatch_propput( ptrDispatch, CPrintFieldAXCtrl::dispidUseKeyToRead, VT_BOOL, m_bUseKeyToRead );

		long lAlignment = ((CButton *)GetDlgItem( IDC_ALIGN_LEFT ))->GetCheck();
		
		if( lAlignment )
			_com_dispatch_propput( ptrDispatch, CPrintFieldAXCtrl::dispidCaptionAlignment, VT_I4, 0 );
		else
		{
			lAlignment = ((CButton *)GetDlgItem( IDC_ALIGN_CENTER ))->GetCheck();
			if( lAlignment )
				_com_dispatch_propput( ptrDispatch, CPrintFieldAXCtrl::dispidCaptionAlignment, VT_I4, 1 );
			else
			{
				lAlignment = ((CButton *)GetDlgItem( IDC_ALIGN_RIGHT ))->GetCheck();
				if( lAlignment )
					_com_dispatch_propput( ptrDispatch, CPrintFieldAXCtrl::dispidCaptionAlignment, VT_I4, 2 );
				else
					_com_dispatch_propput( ptrDispatch, CPrintFieldAXCtrl::dispidCaptionAlignment, VT_I4, -1 );
			}
		}
	}
}

void CPrintFieldAXPropPage::OnCloseupCombo() 
{
	CComboBox *pBox = (CComboBox *)GetDlgItem( IDC_COMBO_FROM );

	int nSel = pBox->GetCurSel();

	if( nSel != -1 )
		pBox->GetLBText( nSel, m_strKeyToRead );
}

void CPrintFieldAXPropPage::OnCheckUseKey() 
{
	CButton *pBtn = (CButton *)GetDlgItem( IDC_CHECK_USEKEY );

	m_bUseKeyToRead = BOOL( pBtn->GetCheck() != 0 );

	CComboBox *pBox = (CComboBox *)GetDlgItem( IDC_COMBO_FROM );
	pBox->EnableWindow( m_bUseKeyToRead );
}
