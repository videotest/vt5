// PrintProgress.cpp : implementation file
//

#include "stdafx.h"
#include "print.h"
#include "PrintProgress.h"
#include "TemplateSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintProgress dialog

IMPLEMENT_DYNCREATE(CPrintProgress, CDialog)

BEGIN_INTERFACE_MAP(CPrintProgress, CDialog)
	INTERFACE_PART(CPrintProgress, IID_IScrollZoomSite, ScrollSite)		
	INTERFACE_PART(CPrintProgress, IID_IScrollZoomSite2, ScrollSite)		
	INTERFACE_PART(CPrintProgress, IID_INamedObject2, Name)	
	INTERFACE_AGGREGATE(CPrintProgress, m_punkView)	
	INTERFACE_AGGREGATE(CPrintProgress, m_punkContext)
END_INTERFACE_MAP()

//IMPLEMENT_UNKNOWN(CPrintProgress, ScrollSite)

CPrintProgress::CPrintProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintProgress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintProgress)
	//}}AFX_DATA_INIT
	m_punkView = /*m_punkData = */m_punkContext = NULL;
	m_pReportForm = NULL;
	EnableAggregation();		

	m_bInitOK = FALSE;
	
}

CPrintProgress::~CPrintProgress()
{

	{
		sptrIWindow sptrWin(m_punkView);	
		if( sptrWin )
			sptrWin->DestroyWindow();
	}

	if( m_punkView )
	{
		m_punkView->Release();
		m_punkView = 0;
	}

	{
		sptrIDataContext	sptrDC( m_punkContext );
		if( sptrDC != NULL )	
			sptrDC->AttachData(0);	
	}

	if( m_punkContext )
	{
		m_punkContext->Release();	
		m_punkContext = 0;
	}


}

void CPrintProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintProgress)
	DDX_Control(pDX, IDC_STATUS, m_statusWnd);
	DDX_Control(pDX, IDC_PROGRESS, m_progressWnd);
	DDX_Control(pDX, IDC_PREVIEW, m_previewWnd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintProgress, CDialog)
	//{{AFX_MSG_MAP(CPrintProgress)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_PRINT_PROGRESS, OnPrintProgress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintProgress message handlers
CScrollBar* CPrintProgress::GetScrollBarCtrl( int sbCode )
{
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
CWnd * CPrintProgress::GetWindow()
{
	return (CWnd*)this;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPrintProgress::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_bInitOK = FALSE;

	if( !InitActiveDocument() )			
		return TRUE;	

	if( !InitTemplate() )			
		return TRUE;
	

	m_bInitOK = TRUE;

	ShowWindow( SW_SHOW );
	UpdateWindow();

	return TRUE;	            
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPrintProgress::InitTemplate()
{
	if( m_pReportForm == NULL )
		return FALSE;
	
	HRESULT hr;
	BSTR bstr;	

	
	CLSID	clsid;
	CString strProgID;

	


	//Create view
	strProgID = szPrintViewProgID;	
	bstr = strProgID.AllocSysString();
	if( ::CLSIDFromProgID( bstr, &clsid ) )
		return FALSE;
	::SysFreeString( bstr );
	hr = CoCreateInstance(clsid,
       GetControllingUnknown(), CLSCTX_INPROC_SERVER,
       IID_IUnknown, (LPVOID*)&m_punkView);

	if( hr != S_OK)
	{
		m_punkView = NULL;
		return FALSE;		
	}
	
	sptrIView	sptrV( GetControllingUnknown() );
	sptrV->Init( m_punkDocument, GetControllingUnknown() );
	


	strProgID = szContextProgID; 
	bstr = strProgID.AllocSysString();
	if( ::CLSIDFromProgID( bstr, &clsid ) )
		return FALSE;
	::SysFreeString( bstr );
	hr = CoCreateInstance(clsid,
       GetControllingUnknown(), CLSCTX_INPROC_SERVER,
       IID_IUnknown, (LPVOID*)&m_punkContext);
	
	if(hr != S_OK)
		return FALSE;


	{
		sptrIDataContext	sptrContext(m_punkContext);
		ASSERT(sptrContext != 0);
		if (sptrContext->AttachData(m_punkDocument) != S_OK)
			return FALSE;
	}




	sptrIDataContext	sptrContext(m_punkContext);		
	if( sptrContext != NULL )
			ISOK( sptrContext->SetActiveObject( 0, m_pReportForm, aofActivateDepended) );	
	 
	

	sptrIScrollZoomSite	pstrS( GetControllingUnknown() );

	sptrIWindow sptrWin(m_punkView);
	sptrWin->SetWindowFlags(wfPlaceInDialog);

	

	CRect rcPreview;
	CRect rcTemp;

	m_previewWnd.GetWindowRect( &rcTemp );

	rcPreview = CRect(0,0,rcTemp.Width(), rcTemp.Height() );	

	if (sptrWin->CreateWnd( m_previewWnd.m_hWnd, rcPreview,
			WS_CHILD|WS_VISIBLE, WM_USER+120) != S_OK)
	{
		return FALSE;
	}	


	



	CWnd* wndView = GetWindowFromUnknown(sptrWin);
	wndView->SendMessage(SW_SHOW);
	wndView->SendMessage(0x0364); 
	wndView->SendMessage(WM_SIZE);

	wndView->EnableWindow( FALSE );



	sptrIReportView sptrView( m_punkView );
	if( sptrView )
	{
		ISOK(sptrView->SetTargetDocument( m_punkDocument ));
		ISOK(sptrView->EnterPrintMode( ));
	}
		
	return TRUE;
}



BOOL CPrintProgress::InitActiveDocument()
{
	return TRUE;
}



void CPrintProgress::OnDestroy() 
{
	if( m_bInitOK ) 
	{
		//KillPreviewFromActiveDocument();
	}
	CDialog::OnDestroy();		
}


LRESULT CPrintProgress::OnPrintProgress(WPARAM wParam, LPARAM lParam)
{

	int nCurPage = (int)wParam;
	int nPageCount = (int)lParam;


	if( nCurPage == 0 )	
		m_progressWnd.SetRange( 0, nPageCount-1 );
	
	m_progressWnd.SetPos( nCurPage );

	CString strStatus;
	strStatus.Format("Printing page %d of %d", nCurPage+1, nPageCount );

	m_statusWnd.SetWindowText( strStatus );	

	CRect rcPreview;
	CRect rcTemp;

	m_previewWnd.GetWindowRect( &rcTemp );

	rcPreview = CRect(0,0,rcTemp.Width(), rcTemp.Height() );	
	
	sptrIReportView sptrView( m_punkView );

	if( sptrView != NULL )
		sptrView->ScrollToPage( nCurPage, rcPreview );	


	if( nCurPage == nPageCount-1 )
		OnOK();		

	return 1L;

}





