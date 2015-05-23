// FormPage.cpp : implementation file
//

#include "stdafx.h"
#include "axform.h"
#include "FormPage.h"


/////////////////////////////////////////////////////////////////////////////
// CFormPage dialog


CFormPage::CFormPage( const char *pszTitle )
	: CFormDialog()
{
	//{{AFX_DATA_INIT(CFormPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sName = pszTitle;
}


void CFormPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFormPage, CDialog)
	//{{AFX_MSG_MAP(CFormPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CFormPage, CFormDialog)
	INTERFACE_PART(CFormPage, IID_INamedObject2, Name)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFormPage message handlers

BOOL CFormPage::OnInitDialog() 
{
	CFormDialog::OnInitDialog();
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CFormPage::OnDestroy() 
{
	m_site.m_bLockFireEvent = true;
	m_site.DestroyWindow();
	CFormDialog::OnDestroy();
	//m_site.FireOnHide();
	m_site.m_bLockFireEvent = false;
}

void CFormPage::OnFinalRelease() 
{
	if( IsWindow( GetSafeHwnd() ) )
		DestroyWindow();

	delete this;
}

void CFormPage::SetForm( IUnknown *punkForm )
{	
	m_site.SetForm( punkForm );	

	HWND	hwnd;
	IApplication	*pApp = GetAppUnknown();
	pApp->GetMainWindowHandle( &hwnd );
	::SendMessage( hwnd, WM_CAPTIONCHANGE, 0, (LPARAM)(const char*)m_site.m_strTitle );

}


void CFormPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);
}
