// ViewAXSizePropPage.cpp : implementation file
//

#include "stdafx.h"
#include "viewax.h"
#include "ViewAXSizePropPage.h"
#include "axint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewAXSizePropPage dialog

IMPLEMENT_DYNCREATE(CViewAXSizePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CViewAXSizePropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CViewAXSizePropPage)
	ON_CBN_SELCHANGE(IDC_TRANSFORMATION, OnSelchangeTransformation)
	ON_BN_CLICKED(IDC_USE_OBJECT_DPI, OnUseObjectDpi)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid
// {BBD8C522-8967-4c25-8F4E-091270DE2069}
GUARD_IMPLEMENT_OLECREATE_CTRL(CViewAXSizePropPage, "VIEWAX.ViewAXSizePropPage.1",
0xbbd8c522, 0x8967, 0x4c25, 0x8f, 0x4e, 0x9, 0x12, 0x70, 0xde, 0x20, 0x69);



/////////////////////////////////////////////////////////////////////////////
// CViewAXSizePropPage::CViewAXSizePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CViewAXSizePropPage

BOOL CViewAXSizePropPage::CViewAXSizePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Define string resource for page type; replace '0' below with ID.
	return UpdateRegistryPage(bRegister, AfxGetInstanceHandle(), IDS_VIEWAX_PPG);
}


/////////////////////////////////////////////////////////////////////////////
// CViewAXSizePropPage::CViewAXSizePropPage - Constructor

// TODO: Define string resource for page caption; replace '0' below with ID.

CViewAXSizePropPage::CViewAXSizePropPage() :
	COlePropertyPage(IDD, IDS_VIEWAX_SIZE_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CViewAXSizePropPage)
	m_nTransformation = -1;
	m_fDPI = 0;
	m_fZoom = 0.0;
	m_bAutoBuild = FALSE;
	m_bUseObjectDPI = FALSE;
	m_fObjectDPI = 0.0;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CViewAXSizePropPage::DoDataExchange - Moves data between page and properties

void CViewAXSizePropPage::DoDataExchange(CDataExchange* pDX)
{
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_DATA_MAP(CViewAXSizePropPage)
	DDP_CBIndex(pDX, IDC_TRANSFORMATION, m_nTransformation, _T("ObjectTransformation") );
	DDX_CBIndex(pDX, IDC_TRANSFORMATION, m_nTransformation);
	DDP_Text(pDX, IDC_EDIT_DPI, m_fDPI, _T("DPI") );
	DDX_Text(pDX, IDC_EDIT_DPI, m_fDPI);
	DDP_Text(pDX, IDC_EDIT_ZOOM, m_fZoom, _T("Zoom") );
	DDX_Text(pDX, IDC_EDIT_ZOOM, m_fZoom);
	DDP_Check(pDX, IDC_AUTO_BUILD, m_bAutoBuild, _T("AutoBuild") );
	DDX_Check(pDX, IDC_AUTO_BUILD, m_bAutoBuild);
	DDP_Check(pDX, IDC_USE_OBJECT_DPI, m_bUseObjectDPI, _T("UseObjectDPI") );
	DDX_Check(pDX, IDC_USE_OBJECT_DPI, m_bUseObjectDPI);
	DDP_Text(pDX, IDC_OBJECT_DPI, m_fObjectDPI, _T("ObjectDPI") );
	DDX_Text(pDX, IDC_OBJECT_DPI, m_fObjectDPI);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CViewAXSizePropPage message handlers

BOOL CViewAXSizePropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();
	

	SetOnOf( FALSE );

	int nLower, nUpper;
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1))->GetRange( nLower, nUpper );
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1))->SetRange( nUpper*100, nLower*100 );
	
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN2))->GetRange( nLower, nUpper );
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN2))->SetRange( nUpper*100, nLower*100 );


	CString str;
	str.Format( "%.2f", m_fObjectDPI );


	CWnd* pWnd = GetDlgItem(IDC_USE_OBJECT_DPI);
	if( pWnd )
	{
		CString strCaption;
		pWnd->GetWindowText( strCaption );

		strCaption += ":";
		strCaption += str;		

		pWnd->SetWindowText( strCaption );
	}
	HideUnusedControls();
	return TRUE;	              
}
void CViewAXSizePropPage::HideUnusedControls()
{
	CComboBox* cmb =(CComboBox*)GetDlgItem(IDC_TRANSFORMATION);
	if(::GetValueInt(GetAppUnknown(), "ViewAX", "EnableControlScale", 0)==0)
	{
		CComboBox* cmb =(CComboBox*)GetDlgItem(IDC_TRANSFORMATION);
		int n = cmb->GetCount();
		cmb->DeleteString(n-1);
		GetDlgItem(IDC_STATIC_MS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_DPI)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_ZOOM)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SPIN1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SPIN2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_USE_OBJECT_DPI)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_DPI)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_ZOOM)->ShowWindow(SW_HIDE);
	}

}
/////////////////////////////////////////////////////////////////////////////
void CViewAXSizePropPage::OnSelchangeTransformation() 
{	
	SetOnOf();	
}

/////////////////////////////////////////////////////////////////////////////
void CViewAXSizePropPage::SetOnOf( BOOL bUpdate )
{
	if( 0 != ::GetValueInt( ::GetAppUnknown(), "\\ViewAX", "FixedSizeForApportionment", 1 ) );
	{
		// получим вьюху...
		sptrIViewCtrl sptrV( GetViewAXControl() );
		if( sptrV == NULL )
			return;

		_bstr_t bstrProgId;
		sptrV->GetViewProgID(bstrProgId.GetAddress());

		if( bstrProgId == _bstr_t("Apportionment.AView") )
		{ // если Apportionment - запретить все нах, иначе подумаем
			GetDlgItem(IDC_TRANSFORMATION)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_DPI)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_ZOOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_SPIN1)->EnableWindow(FALSE);
			GetDlgItem(IDC_SPIN2)->EnableWindow(FALSE);
			GetDlgItem(IDC_USE_OBJECT_DPI)->EnableWindow(FALSE);
			GetDlgItem(IDC_AUTO_BUILD)->EnableWindow(FALSE);
			GetDlgItem(IDC_USE_OBJECT_DPI)->EnableWindow(FALSE);
			GetDlgItem(IDC_OBJECT_DPI)->EnableWindow(FALSE);

			m_nTransformation = otStretch;
			SetPropText("ObjectTransformation", m_nTransformation);
			m_fDPI = 96;
			SetPropText("DPI", m_fDPI);
			m_fZoom = 1.0;
			SetPropText("Zoom", m_fZoom);
			//// m_bAutoBuild ;
			m_bUseObjectDPI = FALSE;
			SetPropCheck("UseObjectDPI", m_bUseObjectDPI);
			////m_fObjectDPI;
			UpdateData(FALSE);

			return;
		}
	}

	int nIndex = -1;

	if( bUpdate )
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_TRANSFORMATION);
		if( pCombo )
		{
			nIndex = pCombo->GetCurSel();
		}
	}
	else
	{
		nIndex = m_nTransformation;
	}

	GetDlgItem(IDC_TRANSFORMATION)->EnableWindow(TRUE);
	GetDlgItem(IDC_AUTO_BUILD)->EnableWindow(TRUE);
	GetDlgItem(IDC_USE_OBJECT_DPI)->EnableWindow(TRUE);
	GetDlgItem(IDC_OBJECT_DPI)->EnableWindow(TRUE);

	if( (ObjectTransformation)nIndex == otStretch )
	{
		if( m_bUseObjectDPI )	
		{
			GetDlgItem(IDC_EDIT_DPI)->EnableWindow(FALSE);	
			GetDlgItem(IDC_SPIN1)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_EDIT_DPI)->EnableWindow(TRUE);	
			GetDlgItem(IDC_SPIN1)->EnableWindow(TRUE);
		}

		GetDlgItem(IDC_EDIT_ZOOM)->EnableWindow(TRUE);

		GetDlgItem(IDC_SPIN2)->EnableWindow(TRUE);
		GetDlgItem(IDC_USE_OBJECT_DPI)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_DPI)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ZOOM)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN1)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN2)->EnableWindow(FALSE);
		GetDlgItem(IDC_USE_OBJECT_DPI)->EnableWindow(FALSE);
	}
}

void CViewAXSizePropPage::OnUseObjectDpi() 
{	
	UpdateData( TRUE );

	if( m_bUseObjectDPI )	
	{
		GetDlgItem(IDC_EDIT_DPI)->EnableWindow(FALSE);	
		GetDlgItem(IDC_SPIN1)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_DPI)->EnableWindow(TRUE);	
		GetDlgItem(IDC_SPIN1)->EnableWindow(TRUE);
	}	
}

/////////////////////////////////////////////////////////////////////////////
sptrIViewCtrl CViewAXSizePropPage::GetViewAXControl()
{
	ULONG nControls = 0;
	LPDISPATCH FAR* ppDisp = GetObjectArray(&nControls);
	
	if( ppDisp[0] == NULL )
		return NULL;

	sptrIViewCtrl sptrV( ppDisp[0] );
	if( sptrV == NULL )
		return NULL;

	return sptrV;
}

BOOL CViewAXSizePropPage::OnEditProperty(DISPID)
{
	return FALSE;
}

BOOL CViewAXSizePropPage::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	return __super::OnNotify( wParam, lParam, pResult );
}

BOOL CViewAXSizePropPage::OnChildNotify( UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	SetOnOf(); // довольно-таки кривой вариант - сидим хрен знает на каком сообщении вместо нужного...
	// но оно приходит при переключении на нашу страничку - так что сойдет.
	return __super::OnChildNotify( message, wParam, lParam, pResult );
}
