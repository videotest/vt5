// ExtPage.cpp : implementation file
//

#include "stdafx.h"
#include "PropertySheet.h"
#include "resource.h"
#include "ExtPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtPage property page

CExtPage::CExtPage(  ) : CPropertyPage(CExtPage::IDD)
{
	//{{AFX_DATA_INIT(CExtPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CExtPage::~CExtPage()
{
}

void CExtPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExtPage, CPropertyPage)
	//{{AFX_MSG_MAP(CExtPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtPage message handlers

BOOL CExtPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	if( m_sptrPage == 0)
		return FALSE;

	CRect rc;
	GetWindowRect(&rc);
	sptrIWindow		sptrWin(m_sptrPage);
	sptrWin->CreateWnd(GetSafeHwnd(), rc, WS_CHILD|WS_VISIBLE|DS_SETFONT, 0);
	sptrWin->GetHandle((HANDLE*)&m_hWndExt);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool CExtPage::Init( IUnknown *punkPage )
{
	m_sptrPage = punkPage;

	if( m_sptrPage == 0 )
		return false;

	//init m_template
	CDialogTemplate	dt;

	dt.Load( MAKEINTRESOURCE(IDD_DIALOG_EMPTYPAGE));
	HGLOBAL	hTempl = dt.Detach();

	SIZE size;	   

	ASSERT( m_sptrPage != 0 );

	m_sptrPage->GetSize( &size );

	((DLGTEMPLATE*)hTempl)->cx = size.cx;
	((DLGTEMPLATE*)hTempl)->cy = size.cy;

	
	{
		INamedObject2Ptr	ptrNamed( m_sptrPage );
		
		BSTR strName = NULL;
		if( ptrNamed != 0 )
			ptrNamed->GetUserName( &strName ); 
		else
			m_sptrPage->GetCaption( &strName ); 
		m_strCaption = strName;
		SysFreeString( strName );
	}

	m_psp.pszTitle = m_strCaption;
	m_psp.dwFlags |= PSP_USETITLE|PSP_DLGINDIRECT;
	m_psp.pResource = (DLGTEMPLATE*)hTempl;//&m_template;

	return true;
}

BOOL CExtPage::OnSetActive()
{
	if(m_sptrPage)
	{
		if(m_sptrPage->OnSetActive() == S_OK)
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}