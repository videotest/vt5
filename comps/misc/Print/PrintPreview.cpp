// PrintPreview.cpp : implementation file
//

#include "stdafx.h"
#include "print.h"
#include "PrintPreview.h"
#include "PageSetup.h"
#include "PrintProgress.h"
#include "PrinterSetup.h"

#include "wndmisc5.h"

#include "afxpriv.h"

HRESULT _invoke_func( IDispatch *pDisp, BSTR bstrFuncName, VARIANT* pargs, int nArgsCount, VARIANT* pvarResult )
{
		CString szFuncName( bstrFuncName );

		if( szFuncName.IsEmpty() )
			return S_FALSE;

		CString str = szFuncName;
		BSTR bstr = str.AllocSysString( );
		DISPID id = -1;
		HRESULT hr = pDisp->GetIDsOfNames( IID_NULL, &bstr, 1, LOCALE_USER_DEFAULT, &id );
		if( hr != S_OK )
		{
			::SysFreeString( bstr );	bstr = 0;
			return S_FALSE;
		}
		
		::SysFreeString( bstr );	bstr = 0;

		DISPPARAMS dispparams;	    

		ZeroMemory( &dispparams, sizeof(dispparams) );	
		
		dispparams.rgvarg			= pargs;
		dispparams.cArgs			= nArgsCount;
		dispparams.cNamedArgs		= 0;
			
		VARIANT vt;
		ZeroMemory( &vt, sizeof(vt) );

		hr = pDisp->Invoke( 
						id, 
						IID_NULL, 
						LOCALE_SYSTEM_DEFAULT,
						DISPATCH_METHOD,
						&dispparams,
						&vt,
						NULL,
						NULL
						);

		if( pvarResult )
			*pvarResult = _variant_t( vt );

		return S_OK;
}

void CEvRouter::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	m_ppv->OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );
}

BEGIN_INTERFACE_MAP(CEvRouter, CCmdTarget)
	INTERFACE_PART(CEvRouter, IID_IEventListener, EvList)
END_INTERFACE_MAP()



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ToolBarID				WM_USER+381
#define ToolBarButtonsIDBase	/*WM_USER+*/382
#define MaxButtonsCount			50

#define TOOLBAR_HEIGHT	30
#define CONTEXT_WIDTH	200


/////////////////////////////////////////////////////////////////////////////
// CPrintPreview dialog

//ViewAX ProgID

#define szContextAXProgID	"CONTEXTAX.ContextAXCtrl.1"


#import "stdole2.tlb" rename_namespace("OcxSpace") exclude("GUID", "DISPPARAMS", "EXCEPINFO", \
	"OLE_XPOS_PIXELS", "OLE_YPOS_PIXELS", "OLE_XSIZE_PIXELS", "OLE_YSIZE_PIXELS", "OLE_XPOS_HIMETRIC", \
	"OLE_YPOS_HIMETRIC", "OLE_XSIZE_HIMETRIC", "OLE_YSIZE_HIMETRIC", "OLE_XPOS_CONTAINER", \
	"OLE_YPOS_CONTAINER", "OLE_XSIZE_CONTAINER", "OLE_YSIZE_CONTAINER", "OLE_HANDLE", "OLE_OPTEXCLUSIVE", \
	"OLE_CANCELBOOL", "OLE_ENABLEDEFAULTBOOL", "FONTSIZE", "OLE_COLOR", \
	"IUnknown", "IDispatch", "IEnumVARIANT", "IFont", "IPicture")

//ViewAX button referense

#import <ContextAX.dll> rename_namespace("OcxSpace") 

using namespace OcxSpace; 



IMPLEMENT_DYNCREATE(CPrintPreview, CDialog)

BEGIN_INTERFACE_MAP(CPrintPreview, CDialog)
	INTERFACE_PART(CPrintPreview, IID_IScrollZoomSite, ScrollSite)		
	INTERFACE_PART(CPrintPreview, IID_IScrollZoomSite2, ScrollSite)		
	INTERFACE_PART(CPrintPreview, IID_INumeredObject, Num)	
	//INTERFACE_PART(CPrintPreview, IID_IEventListener, EvList)
	INTERFACE_PART(CPrintPreview, IID_INamedObject2, Name)
	INTERFACE_PART(CPrintPreview, IID_IRootedObject, Rooted)	
	INTERFACE_AGGREGATE(CPrintPreview, m_punkView)	
	INTERFACE_AGGREGATE(CPrintPreview, m_punkContext)	
END_INTERFACE_MAP()


#define MAX_ZOOM	11

int zoom[] = 
{
	5,
	10,
	20,
	30,
	50,
	70,
	90,
	100,
	150,
	200,
	400
};





CPrintPreview::CPrintPreview(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintPreview::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintPreview)
	m_nTemplate = -1;
	//}}AFX_DATA_INIT
	m_punkView = m_punkContext = NULL;
	
	EnableAggregation();		
	m_bInitOK = false;
	m_ptOffset = CPoint( 0 ,0 );	

	m_strAutoRptActiveView.LoadString( IDS_AUTO_RPT_ACTIVE_VIEW );
	m_strAutoRptOneObjectPerPage.LoadString( IDS_AUTO_RPT_ONE_OBJECT_PER_PAGE );
	m_strAutoRptVerticalAsIs.LoadString( IDS_AUTO_RPT_VERTICAL_AS_IS );
	m_strAutoRptGallery.LoadString( IDS_AUTO_RPT_GALLERY );
	m_strAutoRptFromSplitter.LoadString( IDS_AUTO_RPT_FROM_SPLITTER );

	m_bitmaps.SetSize( 0, 100 );	
	m_pEvRouter = new CEvRouter( this );

	m_bLockCheckButton = false;

	m_ntoolbar_height = TOOLBAR_HEIGHT;
}


CPrintPreview::~CPrintPreview()
{

	IUnknown* punkAXPP = GetAXPropertyPage();
	if( punkAXPP )
	{
		INotifyControllerPtr sptrNC( punkAXPP );
		if( sptrNC )
			sptrNC->UnRegisterEventListener( NULL, m_pEvRouter->GetControllingUnknown() );
		
		punkAXPP->Release();
	}
	


	IApplicationPtr	ptrApp( GetAppUnknown() );
	IUnknown	*punkTM = 0;
	ptrApp->GetTargetManager( &punkTM );
	if( punkTM )
	{
		IActionTargetManagerPtr	targetMan( punkTM );
		punkTM->Release();

		//_bstr_t	bstrTarget = szTargetViewSite;
		targetMan->UnRegisterTarget( GetControllingUnknown() ); /*,bstrTarget*/ 
		//targetMan->ActivateTarget( GetControllingUnknown() );
	}

	DeInit();

	for( int i = 0; i < m_bitmaps.GetSize(); i++ )
	{
		HBITMAP	h = (HBITMAP)m_bitmaps[i];
		::DeleteObject( h );
	}

	m_bitmaps.RemoveAll();
	m_pEvRouter->GetControllingUnknown()->Release();
}



void CPrintPreview::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintPreview)
	DDX_Control(pDX, IDCANCEL, m_WndCancel);
	DDX_Control(pDX, IDOK, m_wndOK);
	DDX_Control(pDX, IDC_START, m_wndStart);
	DDX_Control(pDX, IDC_PRINT_WND, m_previewWnd);
	DDX_Control(pDX, IDC_ZOOM, m_zoomWnd);
	DDX_Control(pDX, IDC_SCRL_VERT, m_VertScrlWnd);
	DDX_Control(pDX, IDC_SCRL_HORZ, m_HorzScrlWnd);
	DDX_CBIndex(pDX, IDC_TEMPLATE_NAME, m_nTemplate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintPreview, CDialog)
	//{{AFX_MSG_MAP(CPrintPreview)
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_PRINT_SETUP, OnPrintSetup)
	ON_BN_CLICKED(IDC_PRINT_TO_FILE, OnPrintToFile)
	ON_BN_CLICKED(IDC_PAGE_SETUP, OnPageSetup)
	ON_BN_CLICKED(IDC_PRINT, OnPrint)
	ON_CBN_EDITCHANGE(IDC_ZOOM, OnEditchangeZoom)
	ON_WM_SIZE()	
	ON_CBN_SELENDOK(IDC_ZOOM, OnSelendokZoom)
	ON_CBN_SELCHANGE(IDC_TEMPLATE_NAME, OnSelchangeTemplateName)	
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTN_PAGE_COLOR, OnBtnPageColor)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_TEMPLATE_SETUP, OnTemplateSetup)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ToolBarButtonsIDBase, (ToolBarButtonsIDBase + MaxButtonsCount), OnCommandToolBar)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_WININICHANGE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SIZING()
	ON_MESSAGE(WM_PRINT_PROGRESS, OnPrintProgress)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintPreview message handlers

CScrollBar* CPrintPreview::GetScrollBarCtrl( int sbCode )
{	   	
	if (sbCode == SB_HORZ)
		return &m_HorzScrlWnd;//Temp;//m_HorzScrlWnd;
	else if (sbCode == SB_VERT)
		return &m_VertScrlWnd;//Temp;//m_VertScrlWnd;
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
CWnd * CPrintPreview::GetWindow()
{
	return (CWnd*)::GetWindowFromUnknown( m_punkView );
}
				   
BOOL CPrintPreview::OnInitDialog() 
{
	
	CDialog::OnInitDialog();

//	CHelpDlgImpl::InitHelpImlp( GetSafeHwnd(), "Print", "PrintPreview" );

	//is read only?
	bool bread_only = is_read_only();
	if( bread_only )
		m_ntoolbar_height = 0;

	//Need 
	if( ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW, PRINT_SHOWTEMPLATESETUPBTN, 1 ) != 1L )
	{
		CWnd* pwnd = GetDlgItem( IDC_TEMPLATE_SETUP );
		if( pwnd )
			pwnd->ShowWindow( SW_HIDE );
	}	

	TerminateInteractive();

	IUnknown* punkAXPP = GetAXPropertyPage();
	if( punkAXPP )
	{
		INotifyControllerPtr sptrNC( punkAXPP );
		if( sptrNC )
			sptrNC->RegisterEventListener( NULL, m_pEvRouter->GetControllingUnknown() );
		
		punkAXPP->Release();
	}
	
	
	if( !is_read_only() )
		CreateToolbar();

	FillTemplateCombo();
	

	m_bInitOK = false;

	if( m_ptrTargetDocument == NULL )
		return TRUE;


	IDocumentSitePtr ptrDS( m_ptrTargetDocument );
	if( ptrDS )
	{
		IUnknown* punkV = 0;
		ptrDS->GetActiveView( &punkV );

//		m_ptrOldAciveView = punkV;
		m_guidOldActiveView = GetObjectKey(punkV);
		if( punkV )
			punkV->Release();	punkV = 0;

	}


	///Get settings from documet
	{
		CReportConstructor rptConstructor( m_ptrTargetDocument );
		m_templateSource		= rptConstructor.GetTemplateSource();
		m_strTemplateName		= rptConstructor.GetTemplateName();
		m_autoTemplateAlghoritm	= rptConstructor.GetAutoTemplateAlghoritm();
	}
	

	if( !Init() )
		return TRUE;	


	m_bInitOK = true;	

	OnInitScrollSite();//SetNewScrollPos();	

	CRect rcDesktop;
	CWnd* pDesktop = GetDesktopWindow();
	pDesktop->GetWindowRect( &rcDesktop );

	int nX, nY, nWidth, nHeight;

	nX = GetValueInt( GetAppUnknown(), PRINT_PREVIEW, "Dlg placement x", 
		rcDesktop.Width() / 4 );
	nY = GetValueInt( GetAppUnknown(), PRINT_PREVIEW, "Dlg placement y", 
		rcDesktop.Height() / 4 );

	nWidth = GetValueInt( GetAppUnknown(), PRINT_PREVIEW, "Dlg placement width", 
		rcDesktop.Width() / 2 );

	nHeight = GetValueInt( GetAppUnknown(), PRINT_PREVIEW, "Dlg placement height", 
		rcDesktop.Height() / 2 );

	MoveWindow( nX, nY, nWidth, nHeight, FALSE );


	m_zoomWnd.ResetContent();
	
	for( int i=0;i<MAX_ZOOM;i++)
	{
		CString str;
		str.Format("%d", zoom[i] );
		str += "%";
		m_zoomWnd.AddString( str );
	}


	CString strFormat;
	strFormat.LoadString( IDS_FIT_TO_PAGE );
	m_zoomWnd.AddString( strFormat );
	strFormat.LoadString( IDS_FIT_TO_PAGE_WIDTH );
	m_zoomWnd.AddString( strFormat );



	int nCurSel = ::GetValueInt( GetAppUnknown(), PRINT_PREVIEW, "LastZoomIndex", MAX_ZOOM );
	m_zoomWnd.SetCurSel( nCurSel ); //Fit To page

	sptrIReportView sptrView( m_punkView );
	if( sptrView )
	{
		ISOK( sptrView->FitToPage() );
	}	


	IApplicationPtr	ptrApp( GetAppUnknown() );
	IUnknown	*punkTM = 0;
	ptrApp->GetTargetManager( &punkTM );
	if( punkTM )
	{
		IActionTargetManagerPtr	targetMan( punkTM );
		punkTM->Release();

		_bstr_t	bstrTarget = szTargetViewSite;
		targetMan->RegisterTarget( GetControllingUnknown(), bstrTarget );
		targetMan->ActivateTarget( GetControllingUnknown() );
	}


	CRect rc = CRect(0,0,0,0);		
	
	m_ctrlContextAX.CreateControl( _bstr_t(szContextAXProgID), NULL, 
					WS_CHILD | WS_VISIBLE | WS_TABSTOP, rc, this, IDC_CONTEXT_AX );

	if( m_ctrlContextAX.GetSafeHwnd() )
		m_ctrlContextAX.RedrawWindow( NULL, NULL, RDW_FRAME|RDW_INVALIDATE|RDW_INTERNALPAINT );
	
	
	
	_DContextAXPtr ptrCV( m_ctrlContextAX.GetControlUnknown() );
	if( ptrCV )
	{			
		ptrCV->ReadOnly = true;
	}	

	OnSelendokZoom();	
	
	if (GetValueInt( GetAppUnknown(), PRINT_PREVIEW, "DisableTemplateSelection", FALSE))
	{
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_TEMPLATE_NAME), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_TEMPLATE_NAME), FALSE);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPrintPreview::ProcessMainFrameAccel( MSG* pMsg )
{
	IUnknown	*punkMW = NULL;
	punkMW = _GetOtherComponent( GetAppUnknown(), IID_IMainWindow );

	ITranslatorWndPtr	ptrTW(punkMW);
	if( punkMW )
		punkMW->Release();

	BOOL bRes = FALSE;
	if( ptrTW != 0 )
		ptrTW->TranslateAccelerator( pMsg, &bRes );

	return bRes;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CPrintPreview::PreTranslateMessage(MSG* pMsg)
{

	//return CWnd::PreTranslateMessage( pMsg );


	IApplicationPtr	ptrApp( GetAppUnknown() );
	IUnknown	*punkAM = 0;
	ptrApp->GetActionManager( &punkAM );	
	if( punkAM )
	{
		LRESULT lRes;
		IMsgListenerPtr	ptrMsgL( punkAM );
		if( ptrMsgL )
		{
			ptrMsgL->OnMessage( pMsg, &lRes );
		}
		punkAM->Release();	
	}

	//if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE )
	//	ASSERT( FALSE );

	
	if( pMsg->message == WM_LBUTTONDOWN )
	{
		CWnd* pWndView = NULL;
		pWndView = ::GetWindowFromUnknown( m_punkView );
		if( pWndView && pWndView->GetSafeHwnd() )
		{	
			CRect rc;
			pWndView->GetWindowRect( &rc );
			CPoint pt( LOWORD(pMsg->lParam), HIWORD(pMsg->lParam) );
			ClientToScreen( &pt );
			
			if( rc.PtInRect( pt ) )
				pWndView->SetFocus();					
		}
	}
	
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{		
		if( m_punkView )
		{
			CWnd* pWndFocus = CWnd::GetFocus();
			CWnd* pWndView = ::GetWindowFromUnknown( m_punkView ); 

			CWnd* pWndPreviewOwner = GetDlgItem(IDC_PRINT_WND);

			if( pWndFocus && pWndFocus->GetSafeHwnd() && pWndView && pWndView->GetSafeHwnd() )
			{
				bool bFoundFocusInView = false;
				if( pWndView->GetSafeHwnd() == pWndFocus->GetSafeHwnd() )
					bFoundFocusInView = TRUE;
				else
				if( pWndPreviewOwner->GetSafeHwnd() == pWndFocus->GetSafeHwnd() )
					bFoundFocusInView = TRUE;
				else
				{
					CWnd* pChild = pWndView->GetWindow( GW_CHILD );
					while( pChild )
					{
						if( pChild->GetSafeHwnd() == pWndFocus->GetSafeHwnd() )
							bFoundFocusInView = TRUE;

						pChild = pChild->GetNextWindow();
					}
				}


				if( bFoundFocusInView )
					if( ProcessMainFrameAccel( pMsg ) )
						return TRUE;		
			}

		}
	}

	return CWnd/*CDialog*/::PreTranslateMessage( pMsg );

}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CDialog::OnLButtonDown(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////
bool CPrintPreview::Init()
{
	DeInit();

	m_bInitOK = false;


	if( !CreatePreviewObject() )
		return false;

	if( !CreateAggregates() )
		return false;


	m_bInitOK = true;

	return true;
}

static IUnknownPtr FindViewByGuid(IDocumentSite *pds, GuidKey guidOrig)
{
	IUnknownPtr punkView1;
	TPOS pos;
	pds->GetFirstViewPosition(&pos);
	while(pos)
	{
		IUnknownPtr punk;
		pds->GetNextView(&punk, &pos);
		if (punk != 0)
		{
			punkView1 = punk;
			GuidKey guid = GetObjectKey(punk);
			if (guidOrig == guid)
				return punk;
		}
	}
	return punkView1;
}


/////////////////////////////////////////////////////////////////////////////
bool CPrintPreview::DeInit()
{	
	//m_strTemplateName.Empty();

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


	sptrIDocumentSite	sptrDocSite( m_ptrTargetDocument );
	if( sptrDocSite == NULL )
		return FALSE;

//	sptrDocSite->SetActiveView( m_ptrOldAciveView );
//	m_ptrOldAciveView = NULL;
	IUnknownPtr punkPrevActive = FindViewByGuid(sptrDocSite, m_guidOldActiveView);
	sptrDocSite->SetActiveView(punkPrevActive);
	m_guidOldActiveView = GUID_NULL;

	IUnknown	*punkV = 0;
	sptrDocSite->GetActiveView( &punkV );

	if( punkV )
	{
		sptrIReportView	sptrV(punkV);
		punkV->Release();
		if( sptrV )
		{			
			ISOK( sptrV->DisconnectNotifyController( FALSE ) );
		}
	}	

	KillPreviewFromActiveDocument();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
bool CPrintPreview::CreatePreviewObject()
{
	m_nTemplate = -1;
	UpdateData( FALSE );

	

	if( m_ptrTargetDocument == NULL )
		return false;

	CReportConstructor rptConstructor( m_ptrTargetDocument );
	rptConstructor.SetTemplateSource( m_templateSource );
	rptConstructor.SetTemplateName( m_strTemplateName );
	rptConstructor.SetAutoTemplateAlghoritm( m_autoTemplateAlghoritm );


	m_strTemplateName = rptConstructor.GetTemplateName();


	IUnknown* punkReport = NULL;
	punkReport = rptConstructor.GenerateReport();
	if( punkReport == NULL )
	{
		return false;
	}


	IUnknown* punkDocumentReportCopyFrom = NULL;
	punkDocumentReportCopyFrom = rptConstructor.GetDocumentReportCopyFrom();

	m_ptrDocReportCopyFrom = NULL;

	if( punkDocumentReportCopyFrom )
	{
		m_ptrDocReportCopyFrom = punkDocumentReportCopyFrom;
		punkDocumentReportCopyFrom->Release();
	}	

	//Set name
	INamedObject2Ptr sptrNO2( punkReport );
	if( sptrNO2 )	
		sptrNO2->SetName( _bstr_t((LPCSTR)g_szPrintPreviewObject) );
	

	::SetValue( m_ptrTargetDocument, 0, 0, _variant_t( (IUnknown*)punkReport ) );

/*
	sptrIDataContext sptrDC( m_ptrTargetDocument );
	if( sptrDC )
		sptrDC->SetActiveObject( _bstr_t(szTypeReportForm), punkReport );
		*/


	punkReport->Release();

	m_nTemplate = GetTemplateNameIndex();
	UpdateData( FALSE );
	


	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CPrintPreview::CreateAggregates()	
{
	if( m_ptrTargetDocument == NULL)		
		return false;

	sptrIDocumentSite	sptrDocSite( m_ptrTargetDocument );
	if( sptrDocSite == NULL )
		return false;



	//Get Print preview object
	IUnknownPtr ptrReport = NULL; 
	
	{
		IUnknown* punkReport = NULL;
		punkReport = ::GetObjectByName( m_ptrTargetDocument, g_szPrintPreviewObject, szTypeReportForm );
		if( punkReport == NULL )
			return false;
		
		ptrReport = punkReport;
		punkReport->Release();
		if( ptrReport == NULL )
			return false;
	}	



	HRESULT hr;	
	CLSID	clsid;
	CString strProgID;

	//Create view
	strProgID = szPrintViewProgID;		
	if( ::CLSIDFromProgID( _bstr_t(strProgID), &clsid ) )
		return FALSE;
	
	hr = CoCreateInstance( clsid, GetControllingUnknown(), 
							CLSCTX_INPROC_SERVER,IID_IUnknown, (LPVOID*)&m_punkView);
	if( hr != S_OK )
	{
		m_punkView = NULL;
		return false;		
	}	

	sptrIView sptrView( m_punkView );			
	if( sptrView == NULL )
		return false;

	sptrView->Init( m_ptrTargetDocument, GetControllingUnknown() );	


	//Create context;
	strProgID = szContextProgID; 	
	if( ::CLSIDFromProgID( _bstr_t(strProgID), &clsid ) )
		return false;
	
	hr = CoCreateInstance( clsid, GetControllingUnknown(), 
							CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&m_punkContext );
	
	if(hr != S_OK)
	{
		m_punkContext = NULL;
		return false;
	}

	//attach document
	{
		sptrIDataContext	sptrContext(m_punkContext);
		if( sptrContext == NULL )
			return false;
		
		if (sptrContext->AttachData(m_ptrTargetDocument) != S_OK)
			return false;
	}	


	//Set report active 
	{
		sptrIDataContext	sptrContext(m_punkContext);		
		if( sptrContext == NULL )
			return false;

		ISOK( sptrContext->SetActiveObject( _bstr_t(szTypeReportForm), ptrReport, aofActivateDepended ) );
	}


	sptrIScrollZoomSite	pstrS( GetControllingUnknown() );	

	sptrIWindow sptrWin( m_punkView );		

	CRect rcPreview;
	CRect rcTemp;

	m_previewWnd.GetWindowRect( &rcTemp );

	rcPreview = CRect(0,0,rcTemp.Width(), rcTemp.Height() );
	

	if (sptrWin->CreateWnd( /*m_previewWnd.*/m_hWnd, rcPreview,
			WS_CHILD|WS_VISIBLE|WS_TABSTOP, 777) != S_OK)
	{
		return false;
	}	
	

	CWnd* wndView = GetWindowFromUnknown(sptrWin);
	wndView->SendMessage(SW_SHOW);
	wndView->SendMessage(0x0364); 
	wndView->SendMessage(WM_SIZE);	

	
	{
		IUnknown	*punkV = 0;
		sptrDocSite->GetActiveView( &punkV );

		if( punkV )
		{
//			m_ptrOldAciveView = punkV;
			m_guidOldActiveView = GetObjectKey(punkV);

			sptrIReportView	sptrV(punkV);
			punkV->Release();
			if( sptrV )
			{			
				ISOK( sptrV->DisconnectNotifyController( TRUE ) );
			}
		}	
	}

	
	sptrIReportView sptrReportView( m_punkView );
	if( sptrReportView )
	{	 
		ISOK(sptrReportView->SetTargetDocument( m_ptrTargetDocument ));
		ISOK(sptrReportView->SetReportSourceDocument( m_ptrDocReportCopyFrom ));

		DWORD dw_flags = 0;
		if( is_read_only() )
			dw_flags = 1;

		ISOK(sptrReportView->EnterPreviewMode( dw_flags ));

		ISOK( sptrDocSite->SetActiveView( sptrReportView ) );
	}

	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CPrintPreview::KillPreviewFromActiveDocument()
{	
	if( m_ptrTargetDocument == NULL )
		return false;


	
	IUnknown* punkPreview = NULL;
	punkPreview = ::GetObjectByName( m_ptrTargetDocument, g_szPrintPreviewObject, szTypeReportForm );
	if( punkPreview != NULL )
	{
		::DeleteObject( m_ptrTargetDocument, punkPreview );
		punkPreview->Release();
	}
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnOK() 
{

	if( m_bInitOK ) 
	{
		sptrIReportView sptrView( m_punkView );
		if( sptrView )		
			ISOK(sptrView->LeavePreviewMode());			
		
	}
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnDestroy() 
{	

	UpdateData( TRUE );
	
	if( m_ptrTargetDocument != NULL )
	{		

		::SetValue( m_ptrTargetDocument, 
						REPORT_TEMPLATE_SECTION, REPORT_TEMPLATE_SOURCE, (long)m_templateSource );
		::SetValue( m_ptrTargetDocument, 
						REPORT_TEMPLATE_SECTION, REPORT_EXIST_TEMPLATE_NAME, m_strTemplateName );

		::SetValue( m_ptrTargetDocument, 
						REPORT_TEMPLATE_SECTION, REPORT_AUTO_TEMPLATE_ALGHORITM, (long)m_autoTemplateAlghoritm );
	}


	CRect rcWnd;
	GetWindowRect( &rcWnd );
	

	SetValue( GetAppUnknown(), PRINT_PREVIEW, "Dlg placement x", 
		(long)rcWnd.left );
	SetValue( GetAppUnknown(), PRINT_PREVIEW, "Dlg placement y", 
		(long)rcWnd.top );

	SetValue( GetAppUnknown(), PRINT_PREVIEW, "Dlg placement width", 
		(long)rcWnd.Width() );

	SetValue( GetAppUnknown(), PRINT_PREVIEW, "Dlg placement height", 
		(long)rcWnd.Height() );

	//ISOK( sptrV->DisconnectNotifyController( FALSE ) );


	::SetValue( GetAppUnknown(), PRINT_PREVIEW, "LastZoomIndex", (long)m_zoomWnd.GetCurSel() );	

	CDialog::OnDestroy();	
}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollZoomSiteImpl::OnScroll( SB_HORZ, nSBCode, nPos );
	//CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollZoomSiteImpl::OnScroll( SB_VERT, nSBCode, nPos );
	//CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::RecalcLayout()
{

	if( !(IsWindow(m_wndStart.GetSafeHwnd()) &&
		IsWindow(m_previewWnd.GetSafeHwnd()) &&
		IsWindow(m_VertScrlWnd.GetSafeHwnd()) &&
		IsWindow(m_HorzScrlWnd.GetSafeHwnd()) &&
		IsWindow(m_wndOK.GetSafeHwnd()) &&
		IsWindow(m_WndCancel.GetSafeHwnd())
		))
		return;

	int nContextAXWidth = CONTEXT_WIDTH;


	if( m_ctrlContextAX.GetSafeHwnd() )
	{
		nContextAXWidth = GetValueInt( GetAppUnknown(), PRINT_PREVIEW, "ContextAXWidth", CONTEXT_WIDTH );
		if( nContextAXWidth < 0 )
			nContextAXWidth = CONTEXT_WIDTH;
	}

	if( ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW, "ShowContextAX", 0 ) != 1 )
		nContextAXWidth = 1;
	

	int nRightAlign, nBottomAlign;
	nRightAlign = nBottomAlign = 10;

	CRect rcClient;
	GetClientRect( &rcClient );

	//buttons;
	CRect rcBtn;

	m_wndOK.GetWindowRect( &rcBtn );

	CRect rcNewBtnOK, rcNewBtnCancel, rcNewBtnHelp;

	rcNewBtnHelp.right  = rcClient.right - nRightAlign;
	rcNewBtnHelp.bottom = rcClient.bottom - nBottomAlign;
	rcNewBtnHelp.left = rcNewBtnHelp.right  - rcBtn.Width();
	rcNewBtnHelp.top  = rcNewBtnHelp.bottom - rcBtn.Height();

	rcNewBtnCancel = rcNewBtnHelp;

	rcNewBtnCancel.left  = rcNewBtnHelp.left - rcBtn.Width() - nRightAlign;
	rcNewBtnCancel.right = rcNewBtnCancel.left + rcBtn.Width();

	rcNewBtnOK = rcNewBtnCancel;

	rcNewBtnOK.left   = rcNewBtnCancel.left - rcBtn.Width() - nRightAlign;
	rcNewBtnOK.right = rcNewBtnOK.left + rcBtn.Width();


	m_wndOK.MoveWindow( rcNewBtnOK.left, rcNewBtnOK.top, 
					rcNewBtnOK.Width(), rcNewBtnOK.Height() );

	m_WndCancel.MoveWindow( rcNewBtnCancel.left, rcNewBtnCancel.top, 
					rcNewBtnCancel.Width(), rcNewBtnCancel.Height() );

	CWnd	*p = GetDlgItem( IDHELP );
	if( p )p->MoveWindow( rcNewBtnHelp.left, rcNewBtnHelp.top, 
					rcNewBtnHelp.Width(), rcNewBtnHelp.Height() );


	m_wndOK.Invalidate();
	m_WndCancel.Invalidate();
	if( p )p->Invalidate();

	//Start point
	CRect rcWndStart;
	m_wndStart.GetWindowRect( &rcWndStart );
	ScreenToClient( &rcWndStart );

	//scrools
	CRect rcScrlHorz, rcScrlVert;
	CRect rcNewScrlHorz, rcNewScrlVert;

	m_HorzScrlWnd.GetWindowRect( &rcScrlHorz );
	m_VertScrlWnd.GetWindowRect( &rcScrlVert );

	rcNewScrlHorz.right  = rcClient.right - nRightAlign - rcScrlVert.Width();
	rcNewScrlHorz.bottom = rcNewBtnOK.top - nBottomAlign;
	rcNewScrlHorz.left = rcWndStart.left;
	rcNewScrlHorz.top  = rcNewScrlHorz.bottom - rcScrlHorz.Height();

	rcNewScrlHorz.right		-= nContextAXWidth;
	rcNewScrlHorz.top		-= m_ntoolbar_height + 10;
	rcNewScrlHorz.bottom	-= m_ntoolbar_height + 10;

	
	CRect rcToolbar;
	rcToolbar = rcNewScrlHorz;
	rcToolbar.top		+= m_ntoolbar_height;
	rcToolbar.bottom	+= rcToolbar.top + rcToolbar.top;

	rcToolbar.right = rcNewBtnOK.left - 10;

	if( m_toolbarAXCtrl.GetSafeHwnd() )
		m_toolbarAXCtrl.MoveWindow( rcToolbar );

	
	rcNewScrlVert.right  = rcClient.right - nRightAlign;
	rcNewScrlVert.bottom = rcNewScrlHorz.top;
	rcNewScrlVert.left = rcNewScrlVert.right - rcScrlVert.Width();
	rcNewScrlVert.top  = rcWndStart.top;

	
	rcNewScrlVert.left		-= nContextAXWidth;	
	rcNewScrlVert.right		-= nContextAXWidth;		

	
	
	if( m_ctrlContextAX.GetSafeHwnd() )
	{
		CRect rcContext;
		rcContext.left		= rcNewScrlVert.right + 10;
		rcContext.right		= rcContext.left + nContextAXWidth;
		rcContext.top		= rcNewScrlVert.top   + 10;
		rcContext.bottom	= rcNewScrlVert.bottom;		

		m_ctrlContextAX.MoveWindow( &rcContext );

		m_ctrlContextAX.Invalidate();
		m_ctrlContextAX.UpdateWindow();

	}	
	
	
	
	CRect rcNewPreview;	
	rcNewPreview = rcWndStart;

	rcNewPreview.right  = rcNewPreview.left + rcNewScrlVert.left - rcWndStart.left - 10;
	rcNewPreview.bottom = rcNewPreview.top +  rcNewScrlHorz.top  - rcWndStart.top - 10;

	/*
	m_previewWnd.MoveWindow( rcNewPreview.left, rcNewPreview.top, 
					rcNewPreview.Width(), rcNewPreview.Height() );
					*/

	CWnd* previewChild = NULL;
	previewChild = ::GetWindowFromUnknown( m_punkView );//m_previewWnd.GetWindow( GW_CHILD );
	if( previewChild != NULL )
	{	
		previewChild->MoveWindow( &rcNewPreview );
		::SendMessage( previewChild->GetSafeHwnd(), WM_SIZE, 0, MAKEWPARAM( rcNewPreview.Width(), rcNewPreview.Height() ) );
					
	}
	
	m_HorzScrlWnd.MoveWindow( rcNewScrlHorz.left, rcNewScrlHorz.top, 
					rcNewScrlHorz.Width(), rcNewScrlHorz.Height() );

	m_VertScrlWnd.MoveWindow( rcNewScrlVert.left, rcNewScrlVert.top, 
					rcNewScrlVert.Width(), rcNewScrlVert.Height() );


}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	CScrollZoomSiteImpl::OnSize();

	RecalcLayout();

	if( IsWindow( m_zoomWnd ) )
	{
		int nCurSel = m_zoomWnd.GetCurSel();

		sptrIReportView sptrView( m_punkView );
		if( sptrView == NULL )
			return;
		
		if( nCurSel == MAX_ZOOM )
			ISOK( sptrView->FitToPage() );		

		if( nCurSel == MAX_ZOOM+1 )
			ISOK( sptrView->FitToPageWidth() );		
	}
	
}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnPrintSetup() 
{	
	g_prnSetup.ChooseAnother( this );
	g_prnSetup.Write();

	IReportViewPtr reportView( m_punkView );		
	if( reportView != NULL )
		ISOK( reportView->OnPrinterSettingsChange() );

}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnPageSetup() 
{
	if( !m_bInitOK )
		return;

	if( m_ptrTargetDocument == NULL )
		return;

	IUnknown	*punkForm = ::GetObjectByName( m_ptrTargetDocument, g_szPrintPreviewObject, szTypeReportForm );
	if( !punkForm )
		return;

	sptrIReportForm form( punkForm );
	punkForm->Release();

	if( form == NULL )
		return;

	int nOrientation;
	CSize size;
	CRect rect;

	CPageSetup dlg;

	form->GetPaperSize( &size );
	form->GetPaperField( &rect );
	form->GetPaperOrientation( &nOrientation );
	


	dlg.SetData( FROM_VTPIXEL_TO_DOUBLE(size.cx), FROM_VTPIXEL_TO_DOUBLE(size.cy),
					FROM_VTPIXEL_TO_DOUBLE(rect.left), FROM_VTPIXEL_TO_DOUBLE(rect.top), 
					FROM_VTPIXEL_TO_DOUBLE(rect.right), FROM_VTPIXEL_TO_DOUBLE(rect.bottom),
					nOrientation
					);
	

	if( dlg.DoModal() == IDOK )
	{
		form->SetPaperSize( CSize(
			FROM_DOUBLE_TO_VTPIXEL(dlg.m_fWidth), 
			FROM_DOUBLE_TO_VTPIXEL(dlg.m_fHeight)
			) );
		form->SetPaperField( CRect(
			FROM_DOUBLE_TO_VTPIXEL(dlg.m_fLeft), 
			FROM_DOUBLE_TO_VTPIXEL(dlg.m_fTop), 
			FROM_DOUBLE_TO_VTPIXEL(dlg.m_fRight), 
			FROM_DOUBLE_TO_VTPIXEL(dlg.m_fBottom)
			) );
		form->SetPaperOrientation( dlg.m_nOrientation );		

		sptrIReportView	sptrV(m_punkView);	
		if( sptrV != NULL )
		{
			ISOK( sptrV->OnPrinterSettingsChange() );
			//sptrV->UpdateView();
		}

	}

	
	
}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::TerminateInteractive()
{
	IActionManagerPtr ptrAM;
	IUnknown	*punk = NULL;
	punk = _GetOtherComponent( GetAppUnknown(false), IID_IActionManager );

	ptrAM = punk;

	if( punk )
		punk->Release();
	
	if( ptrAM )
		ptrAM->TerminateInteractiveAction();

}


/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnBtnPageColor() 
{
	if( !m_punkView )
		return;

	IHwndHookPtr ptrHook( ::GetAppUnknown() );
	if( ptrHook == 0 )
		return;
			   
	try{
	
	ptrHook->EnterHook( GetSafeHwnd() );

	::ExecuteAction( "RunOptionsPrint" );


	CWnd* preview = ::GetWindowFromUnknown( m_punkView );
	if( preview )
	{
		preview->Invalidate();
		preview->UpdateWindow();
	}

	}
	catch(...)
	{

	}

	ptrHook->LeaveHook( );

	/*
	sptrIDocumentSite	sptrDocSite( m_ptrTargetDocument );
	if( sptrDocSite == NULL )
		return;

	sptrDocSite->SetActiveView( m_punkView );	


	sptrIReportView	sptrV(m_punkView);	
	if( sptrV != NULL )
		sptrV->UpdateView();	
		*/


	SetFocus();
	
}


/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnPrint() 
{	
	if( !m_punkView )
		return;


	if( m_ptrTargetDocument == NULL )
		return;


	IUnknown	*punkReport = ::GetObjectByName( m_ptrTargetDocument, g_szPrintPreviewObject, szTypeReportForm );

	if( punkReport == NULL )
		return;


	sptrIDocumentSite	sptrDocSite( m_ptrTargetDocument );
	if( sptrDocSite == NULL )
		return;



	sptrIReportView	sptrV(m_punkView);
	if( sptrV )
		ISOK( sptrV->DisconnectNotifyController( TRUE ) );
	
//		sptrDocSite->SetActiveView( m_ptrOldAciveView );
	IUnknownPtr punkOldActiveView = FindViewByGuid(sptrDocSite, m_guidOldActiveView);
	sptrDocSite->SetActiveView(punkOldActiveView);

	CPrintProgress dlg( this );
	dlg.SetReportForm( punkReport );
	dlg.SetActiveDocument( m_ptrTargetDocument );
	dlg.DoModal( );	

	sptrDocSite->SetActiveView( m_punkView );

	if( sptrV )
	{		
		ISOK( sptrV->DisconnectNotifyController( FALSE ) );

		CWnd* pWnd = GetWindowFromUnknown( sptrV );
		if( pWnd && pWnd->GetSafeHwnd() )
		{
			CRect rc;
			pWnd->GetWindowRect( &rc );
			::SendMessage( pWnd->GetSafeHwnd(), WM_SIZE, 0, MAKEWPARAM( rc.Width(), rc.Height() ) );
		}
	}


	sptrDocSite->SetActiveView( m_punkView );

	punkReport->Release();	


}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnEditchangeZoom() 
{
	sptrIScrollZoomSite	pstrS( GetControllingUnknown() );		
	CWnd* pWnd = NULL;
	pWnd = GetDlgItem( IDC_ZOOM );
	if( pWnd && pstrS != NULL )
	{
		double fZoom;
		int nValue;
		CString strText;
		pWnd->GetWindowText( strText );
		nValue = atoi( strText );
		fZoom = (double)nValue /100.0;
		ISOK( pstrS->SetZoom( fZoom ) );
	}
	
	
}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnSelendokZoom() 
{	
	int nCurSel = m_zoomWnd.GetCurSel();

	if( nCurSel >= 0 && nCurSel < MAX_ZOOM ) 
	{
		sptrIScrollZoomSite	pstrS( GetControllingUnknown() );		
		if( pstrS != NULL )
		{
			double fZoom;			
			fZoom = (double)zoom[nCurSel]/100.0;
			ISOK( pstrS->SetZoom( fZoom ) );			
		}


	}
	else
	{
		sptrIReportView sptrView( m_punkView );
		if( sptrView == NULL )
			return;
		
		if( nCurSel == MAX_ZOOM )
			ISOK( sptrView->FitToPage() );		

		if( nCurSel == MAX_ZOOM+1 )
			ISOK( sptrView->FitToPageWidth() );		

	}

	RecalcLayout();	
		
}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnSelchangeTemplateName() 
{	

	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_TEMPLATE_NAME);
	if( !pCombo || !pCombo->GetSafeHwnd() )
		return;

	UpdateData( TRUE );
	if( m_nTemplate == -1 )
		return;

	CString strTempl;
	pCombo->GetLBText( m_nTemplate, strTempl );
	if( strTempl.IsEmpty() )
		return;

	m_templateSource = tsAutoTemplate;

	//store margins
	{		
		CString strSection;
		if( m_autoTemplateAlghoritm == ataActiveView )				strSection = ACTIVE_VIEW_MARGINS;
		else if( m_autoTemplateAlghoritm == ataOneObjectPerPage )	strSection = ONE_OBJECT_PER_PAGE_MARGINS;
		else if( m_autoTemplateAlghoritm == ataVerticalAsIs )		strSection = VERTICAL_AS_IS_MARGINS;
		else if( m_autoTemplateAlghoritm == ataGallery )			strSection = GALLERY_MARGINS;
		else if( m_autoTemplateAlghoritm == ataFromSplitter )		strSection = FROM_SPLITTER_MARGINS;

		if( !strSection.IsEmpty( ) )
		{
			
			IUnknown* punkReport = NULL;
			punkReport = ::GetObjectByName( m_ptrTargetDocument, g_szPrintPreviewObject, szTypeReportForm );
			if( punkReport )
			{
				IReportFormPtr ptrReport( punkReport );
				punkReport->Release();	punkReport = 0;

				if( ptrReport )
				{
					CRect rc;
					ptrReport->GetPaperField( &rc );
					::SetGlobalPaperMargins( strSection, rc );
				}
			}
		}
	}

	if( strTempl == m_strAutoRptActiveView )
		m_autoTemplateAlghoritm = ataActiveView;
	else
	if( strTempl == m_strAutoRptOneObjectPerPage )
		m_autoTemplateAlghoritm = ataOneObjectPerPage;
	else
	if( strTempl == m_strAutoRptVerticalAsIs )
		m_autoTemplateAlghoritm = ataVerticalAsIs;
	else
	if( strTempl == m_strAutoRptGallery )
		m_autoTemplateAlghoritm = ataGallery;
	else
	if( strTempl == m_strAutoRptFromSplitter )
		m_autoTemplateAlghoritm = ataFromSplitter;
	else
	{
		m_templateSource = tsUseExist;
		m_strTemplateName = strTempl;
	}

	Init();
	
	RecalcLayout();

	CScrollZoomSiteImpl::OnSize();

	if( IsWindow( m_zoomWnd ) )
	{
		int nCurSel = m_zoomWnd.GetCurSel();
		if( nCurSel == MAX_ZOOM || nCurSel == MAX_ZOOM + 1 )
			OnSelendokZoom();
	}

}

/////////////////////////////////////////////////////////////////////////////
int CPrintPreview::GetTemplateNameIndex()
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_TEMPLATE_NAME);
	if( !pCombo || !pCombo->GetSafeHwnd() )
		return -1;


	//Use exist
	if( m_templateSource != tsAutoTemplate )
	{
		return pCombo->FindString( 0, m_strTemplateName );		
	}
	else	
	{
		if( m_autoTemplateAlghoritm == ataActiveView )
			return pCombo->FindString( 0, m_strAutoRptActiveView );
		else
		if( m_autoTemplateAlghoritm == ataOneObjectPerPage )
			return pCombo->FindString( 0, m_strAutoRptOneObjectPerPage );
		else
		if( m_autoTemplateAlghoritm == ataVerticalAsIs )
			return pCombo->FindString( 0, m_strAutoRptVerticalAsIs );
		else
		if( m_autoTemplateAlghoritm == ataGallery )
			return pCombo->FindString( 0, m_strAutoRptGallery );
		else
		if( m_autoTemplateAlghoritm == ataFromSplitter )
			return pCombo->FindString( 0, m_strAutoRptFromSplitter );
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::FillTemplateCombo()
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_TEMPLATE_NAME);
	if( !pCombo || !pCombo->GetSafeHwnd() )
		return;

	pCombo->ResetContent();


	if( ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW_TEMPL, PRINT_SHOWACTIVEVIEWTEMPL, 1 ) == 1L )
		pCombo->AddString( m_strAutoRptActiveView );

	if( ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW_TEMPL, PRINT_SHOWONEELEMENTTEMPL, 1 ) == 1L )
		pCombo->AddString( m_strAutoRptOneObjectPerPage );

	if( ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW_TEMPL, PRINT_SHOWVERTICALTEMPL, 1 ) == 1L )
		pCombo->AddString( m_strAutoRptVerticalAsIs );

	if( ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW_TEMPL, PRINT_SHOWGALLERYTEMPL, 1 ) == 1L )
		pCombo->AddString( m_strAutoRptGallery );

	if( ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW_TEMPL, PRINT_SHOWFROMSPLITTER, 1 ) == 1L )
		pCombo->AddString( m_strAutoRptFromSplitter );

	bool buse_other_doc_templ = ( ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW_TEMPL, PRINT_OTHER_DOC_TEMPL, 1 ) == 1L );

	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA != NULL )
	{
		LONG_PTR	lPosTemplate = 0;
		
		sptrA->GetFirstDocTemplPosition( &lPosTemplate );
		
		while( lPosTemplate )
		{
			LONG_PTR	lPosDoc = 0;

			sptrA->GetFirstDocPosition( lPosTemplate, &lPosDoc );

			while( lPosDoc )
			{
				IUnknown *punkDoc = 0;
				sptrA->GetNextDoc( lPosTemplate, &lPosDoc, &punkDoc );

				sptrIDocumentSite sptrDocSite( punkDoc );
				punkDoc->Release();	punkDoc = 0;

				if( !buse_other_doc_templ && ::GetObjectKey(sptrDocSite) != ::GetObjectKey(m_ptrTargetDocument) )
					continue;

				if( sptrDocSite != NULL )
				{
					//Get template from this documet
					IUnknown	*punkV = 0;
					sptrDocSite->GetActiveView( &punkV );
					sptrIView	sptrV( punkV );
					sptrIDataContext2	sptrC( punkV );

					if( punkV )
						punkV->Release();
					else
						continue;

					if( sptrV == NULL || sptrC == NULL )
						continue;

					long nCount = -1;	

					_bstr_t bstrType(szTypeReportForm);
					sptrC->GetChildrenCount( bstrType, NULL, &nCount );

					if (nCount)
					{
						long lPos = 0;
						sptrC->GetFirstChildPos(bstrType, 0, &lPos);
						while (lPos)
						{
							IUnknownPtr sptrObj;
							if (FAILED(sptrC->GetNextChild(bstrType, 0, &lPos, &sptrObj)) || sptrObj == 0)
								continue;

							CString str = ::GetObjectName(sptrObj);
							pCombo->AddString( str );
						}
					}
				}				
			}
			sptrA->GetNextDocTempl( &lPosTemplate, 0, 0 );
		} 
	}


	if( ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW_TEMPL, PRINT_RPT_TEMPL, 1 ) == 1L )
	{
		//Fill file based report
		CString strDir = GetReportDir();
		CString strMask = strDir + "*.rpt";

		CFileFind ff;

		BOOL bFound = ff.FindFile( strMask );

		while( bFound )
		{
			bFound = ff.FindNextFile();				
			pCombo->AddString( ff.GetFileName() );			
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::CreateToolbar()
{	
	CRect rcTool = NORECT;
	m_toolbarAXCtrl.Create(WS_CHILD|WS_VISIBLE|CCS_NORESIZE|TBSTYLE_TOOLTIPS, rcTool, this, ToolBarID);
	
	long nEntriesCount = 0;
	sptrINamedData spShellData( GetAppUnknown() );
	if( spShellData != NULL )
	{		
		spShellData->SetupSection( _bstr_t("ReportEditor") );
		spShellData->GetEntriesCount( &nEntriesCount );				
	}

	if( spShellData )
	{								  				
		int nCounter = 0;
		for(int i = 0; i < nEntriesCount; i++)
		{
			BSTR bstrEntryName;
			spShellData->GetEntryName( i, &bstrEntryName );
			CString strEntry = bstrEntryName;
			::SysFreeString( bstrEntryName );
			if( strEntry.Find( "Control_" ) != -1 )			
			{
				CString srtProgID = ::GetValueString(GetAppUnknown(), "ReportEditor", strEntry, "" );
				if( !srtProgID.IsEmpty() )
				{
					_AddCtrlButton( srtProgID, nCounter );
					nCounter++;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::_AddCtrlButton(CString strProgID, int nCodeID )
{

	HBITMAP hBitmap = _GetBitmapFromProgID(strProgID);
	if(hBitmap == 0)
		hBitmap = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_NOTSPECIFIED));
	if( hBitmap )
	{
		m_toolbarAXCtrl.AddBitmap(1, CBitmap::FromHandle(hBitmap));
		m_bitmaps.Add( (void*)hBitmap );
		//::DeleteObject( hBitmap );
	}
	m_arrProgIDs.Add(strProgID);
	TBBUTTON tbb = {0};
	tbb.iBitmap = nCodeID;
	tbb.idCommand = ToolBarButtonsIDBase + nCodeID;
	tbb.fsState = TBSTATE_ENABLED;	
	tbb.fsStyle = TBSTYLE_CHECKGROUP;
	m_toolbarAXCtrl.AddButtons(1, &tbb);
}

/////////////////////////////////////////////////////////////////////////////
HBITMAP CPrintPreview::_GetBitmapFromProgID(CString strProgID)
{
	//return  0;
	_try(CAXInsertPropPage, Invoke)
	{
		HBITMAP hBitmap = 0;
		CLSID clsid;
		::CLSIDFromProgID(_bstr_t(strProgID), &clsid);
	
		BSTR	bstr = 0;
		CString strCLSID;
		if(!::StringFromCLSID(clsid, &bstr))
		{
			strCLSID = bstr;
			CoTaskMemFree(bstr);
		
	
			LONG lSize ;
			LONG	 regResult = ERROR_SUCCESS;
			HKEY hKey;
			char szBuffer[MAX_PATH*2] ;
		
	
			regResult = ::RegOpenKey(HKEY_CLASSES_ROOT, CString("CLSID\\")+strCLSID, &hKey);
			if (regResult != ERROR_SUCCESS) return 0;
				lSize = sizeof(szBuffer) ; 
			regResult = ::RegQueryValue(hKey, "ToolboxBitmap32", szBuffer, &lSize) ;
			if (regResult == ERROR_SUCCESS)
			{
				CString strPathAndResNum(szBuffer, (int)lSize) ;
				// Let's pretend we're using Basic.
				int indexComma = strPathAndResNum.Find(',') ; 
				CString strPath = strPathAndResNum.Left(indexComma) ;
				CString strResNum = strPathAndResNum.Mid(indexComma+1) ;	
				WORD wResNum = atoi(strResNum) ;

				HINSTANCE hInstCtl  = ::LoadLibrary(strPath) ;
				if (hInstCtl != NULL)
				{
					// The .OCX file might not actually exist.
					hBitmap = ::LoadBitmap(hInstCtl, MAKEINTRESOURCE(wResNum)) ;
					::FreeLibrary(hInstCtl) ;
				}
			}
			::RegCloseKey(hKey); 
		}
		return hBitmap;
	}
	_catch;
	return  0;
}

/////////////////////////////////////////////////////////////////////////////
IUnknown* CPrintPreview::GetAXPropertyPage()
{
	IUnknown	*punkPropertySheet = ::GetPropertySheet();
	IAXInsertPropPagePtr ptrAXPropPage;


	if( punkPropertySheet )
	{
		IPropertySheetPtr	spPSheet( punkPropertySheet );
		punkPropertySheet->Release();

		int nPagesCount = 0;
		spPSheet->GetPagesCount( &nPagesCount );
		if( spPSheet )
		{
			for( long nPageNo = nPagesCount-1; nPageNo >-1; nPageNo-- )
			{
				if( ptrAXPropPage != NULL )
					continue;
				IUnknown *punkPage = NULL;
				spPSheet->GetPage( nPageNo, &punkPage );
				if( punkPage == NULL )
					continue;
				IUnknownPtr ptrPage = punkPage;
				punkPage->Release();

				if( !CheckInterface(ptrPage, IID_IAXInsertPropPage ) )
					continue;

				ptrAXPropPage = ptrPage;
			}		
		}
	}

	if( ptrAXPropPage == NULL )
		return FALSE;

	ptrAXPropPage->AddRef();
	return ptrAXPropPage;
}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::UnCheckCurrentButton()
{
	if( !m_toolbarAXCtrl.GetSafeHwnd() )
		return;

	for( int i=0;i<m_toolbarAXCtrl.GetButtonCount();i++ )
	{
		TBBUTTON Button;
		::ZeroMemory( &Button, sizeof(Button) );
		if( m_toolbarAXCtrl.GetButton( i, &Button ) )
		{
			m_toolbarAXCtrl.CheckButton( Button.idCommand, FALSE );
		}
	}


}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnCommandToolBar(UINT nID)
{
	int nCurButton = nID - ToolBarButtonsIDBase;
	if( nCurButton < 0 || nCurButton >= m_arrProgIDs.GetSize() )
	{
		ASSERT( FALSE );
		return;
	}

	m_bLockCheckButton = true;
	TerminateInteractive();
	m_bLockCheckButton = false;

	CString strProgID = m_arrProgIDs[nCurButton];
	CString strActionParams;
	strActionParams.Format( "\"%s\"", strProgID );
	::ExecuteAction( "AxInsertControl", strActionParams );
}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreview::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp(pszEvent, szEventAXInsertControlDone) )
	{
		if( !m_bLockCheckButton )
			UnCheckCurrentButton();		
	}
	else
	if( !strcmp(pszEvent, "AXInsertControlInit") )
	{
		//UnCheckCurrentButton();		
	}

}


void CPrintPreview::OnPaint() 
{
	CPaintDC dc(this);
	CRect rc;
	GetClientRect( &rc );
	dc.FillRect( &rc, &CBrush( ::GetSysColor( COLOR_BTNFACE ) ) );
	/*
	if( m_punkView )
	{
		CRect rcView;
		CRect rcPreview;
		CWnd* preview = ::GetWindowFromUnknown( m_punkView );
		if( preview )
		{
			CRect rcPreview;
			preview->GetWindowRect( &rcPreview );
			ScreenToClient( &rcPreview );

			COLORREF clrBack = ::GetValueColor( ::GetAppUnknown(), "PageColor", "Background", 
						::GetSysColor(COLOR_BTNSHADOW) );
			
			dc.FillRect( &rcPreview, &CBrush(clrBack) );
		}
	}
	*/
	

}



/////////////////////////////////////////////////////////////////////////////
LRESULT CPrintPreview::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{	
/*	if( CHelpDlgImpl::IsHelpMsg( message ) )
	{
		return CHelpDlgImpl::FilterHelpMsg( message, wParam, lParam );
	}*/
	return CDialog::WindowProc(message, wParam, lParam);
}

void CPrintPreview::OnTemplateSetup() 
{

	IHwndHookPtr ptrHook( ::GetAppUnknown() );
	if( ptrHook == 0 )
		return;
	
	bool bRes = false;
	try{
	
	ptrHook->EnterHook( GetSafeHwnd() );

	bRes = RunAutoTemplateSetup();	

	}
	catch(...)
	{

	}

	ptrHook->LeaveHook( );

	SetFocus();

	if( bRes )
		OnSelchangeTemplateName();
	
}

/////////////////////////////////////////////////////////////////////////////
bool CPrintPreview::RunAutoTemplateSetup()
{

	ICompManagerPtr	ptrM( ::GetAppUnknown() );

	if( ptrM == 0 )
		return false;

	int	lCount = 0;
	ptrM->GetCount( &lCount );

	IUnknownPtr ptrFormManager;

	for( int i=0;i<lCount;i++ )
	{
		if( ptrFormManager != 0 )
			continue;

		IUnknown *punk = 0;
		if( S_OK != ptrM->GetComponentUnknownByIdx( i, &punk ) )
			continue;		 

		INamedObject2Ptr ptrNDO( punk );
		if( punk )
			punk->Release();	punk = 0;

		if( ptrNDO == 0 )
			continue;

		BSTR bstrName = 0;
		ptrNDO->GetName( &bstrName );

		CString strName = bstrName;
		::SysFreeString( bstrName );	bstrName = 0;

		if( strName == "FormManager" )
			ptrFormManager = ptrNDO;
	}

	if( ptrFormManager == 0 )
		return false;


	IDispatch* pdisp = 0;
	if( S_OK != ptrFormManager->QueryInterface( IID_IDispatch, (void**)&pdisp ) )
		return false;

	if( pdisp == 0 )
		return false;

	IDispatchPtr ptrDisp = pdisp;
	pdisp->Release();	pdisp = 0;


	CString srtFuncName = "ExecuteModal";	
	BSTR bstr = srtFuncName.AllocSysString();
	DISPID dispID = -1;
	HRESULT hr = ptrDisp->GetIDsOfNames( IID_NULL, &bstr, 1, LOCALE_USER_DEFAULT, &dispID );

	::SysFreeString( bstr );	bstr = 0;

	if( hr != S_OK )
		return false;


	DISPPARAMS dispparams;	
    VARIANTARG varg[1];

	ZeroMemory( &dispparams, sizeof(dispparams) );
	ZeroMemory( &varg, sizeof(varg) );	

	_bstr_t bstrFormName = "PrintAutoTemplateSetup";

	V_VT(&varg[0]) = VT_BSTR;
	V_BSTR(&varg[0]) = bstrFormName;

	dispparams.rgvarg			= varg;
	dispparams.cArgs			= 1;
	dispparams.cNamedArgs		= 0;

	_variant_t varRes;

	

	hr = ptrDisp->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					&varRes,
					NULL,
					NULL
					);

	if( hr != S_OK )
		return false;

	return (varRes.lVal == IDOK );
}


void CPrintPreview::OnHelp() 
{
	HelpDisplay( "PrintPreview" );
}

void CPrintPreview::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
/*	if( lpwndpos->cx < 400 )
		lpwndpos->cx = 400;
	if( lpwndpos->cy < 300 )
		lpwndpos->cy = 300;
*/
	__super::OnWindowPosChanging(lpwndpos);
}


void CPrintPreview::OnSizing(UINT fwSide, LPRECT pRect)
{
	int cx = pRect->right - pRect->left;
	int cy = pRect->bottom - pRect->top;

	if( cx < 400 )
	{
		if( fwSide == WMSZ_TOPRIGHT || fwSide == WMSZ_RIGHT || fwSide == WMSZ_BOTTOMRIGHT )
			pRect->right = pRect->left + 400;
		else if( fwSide == WMSZ_TOPLEFT || fwSide == WMSZ_LEFT || fwSide == WMSZ_BOTTOMLEFT )
			pRect->left = pRect->right - 400;
	}
	
	if( cy < 300 )
	{
		if( fwSide == WMSZ_BOTTOMLEFT || fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMRIGHT )
			pRect->bottom = pRect->top + 300;
		else if( fwSide == WMSZ_TOPLEFT || fwSide == WMSZ_TOP || fwSide == WMSZ_TOPRIGHT )
			pRect->top = pRect->bottom - 300;
	}
		


	__super::OnSizing(fwSide, pRect);

	// TODO: Add your message handler code here
}

BOOL CPrintPreview::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	{
		NMHDR* pnmhdr = (NMHDR*)lParam;
		if( pnmhdr->code == TTN_GETDISPINFO )
		{
			//lParam
			LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
			CString str_tooltip = "";
			int nidx = wParam - ToolBarButtonsIDBase;

			if( nidx >= 0 && nidx < m_arrProgIDs.GetSize() )
			{
				//Get Name from ProgId
				CString strProgID = m_arrProgIDs[nidx];
				CString strName;
				CLSID clsid;
				::CLSIDFromProgID(_bstr_t(strProgID), &clsid);
				BSTR	bstr = 0;
				CString strCLSID;
				if(!::StringFromCLSID(clsid, &bstr))
				{
					strCLSID = bstr;
					CoTaskMemFree(bstr);
				
					LONG lSize ;
					LONG	 regResult = ERROR_SUCCESS;
					HKEY hKey;
					char szBuffer[MAX_PATH*2] ;
					
				
					regResult = ::RegOpenKey(HKEY_CLASSES_ROOT, CString("CLSID\\")+strCLSID, &hKey);
					if (regResult == ERROR_SUCCESS)
					{
						lSize = sizeof(szBuffer) ; 
						regResult = ::RegQueryValue(hKey, NULL, szBuffer, &lSize) ;
						if (regResult == ERROR_SUCCESS)
						{
							str_tooltip = CString(szBuffer, (int)lSize) ;
						}
					}
				}
			}
			strcpy( lpttt->szText, str_tooltip );

			*pResult = 1L;
			return TRUE;			
		}
	}
	return __super::OnNotify(wParam, lParam, pResult);
}

bool CPrintPreview::is_read_only()
{
	return ( ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW, PRINT_PREVIEW_READ_ONLY, 0 ) == 1L );
}

void CPrintPreview::OnPrintToFile()
{
	CString strFilter;
	strFilter.LoadString( IDS_FILTER_SAVE );

	{
		IDispatchPtr sptrDisp;
		sptrDisp.CreateInstance( "Shell.App", 0, CLSCTX_INPROC_HANDLER );

	//	CString name = sptrDisp.DoSaveFileDialog( "", strFilter );

		_variant_t varFlName;
		_variant_t varFilter = _bstr_t( strFilter );

		VARIANT varArgs[2] = { 0 };
		varArgs[1] = varFlName;
		varArgs[0] = varFilter;

		VARIANT varRes={VT_EMPTY}; 
		varRes.bstrVal = 0;
		_invoke_func( sptrDisp, _bstr_t( "DoSaveFileDialog" ), varArgs, 2, &varRes );

		if( varRes.bstrVal == 0 || _bstr_t( varRes.bstrVal ).length() == 0)
			return;
		else
			m_strFileName = varRes.bstrVal;
											  
		int nPos = -1, nPos2 = -1;

		while( ( nPos2 = m_strFileName.Find( "." , nPos + 1 ) ) != -1 )
		  nPos = nPos2;

		if( !nPos )
			nPos = nPos2;

		if( nPos != -1 )
			m_strFileName = m_strFileName.Left( nPos );
		sptrDisp = 0;
	}


	CClientDC dc( this );

	IReportView2Ptr sptrView = m_punkView;
	CRect rcClient;


	CDC dcImage;
	dcImage.CreateCompatibleDC( &dc );

	sptrView->GetPageRect( dcImage, &rcClient );
	m_rcClientPrint = rcClient;

	BITMAPINFOHEADER	bmih = {0};

	bmih.biBitCount = 24;	
	bmih.biWidth	= rcClient.Width();
	bmih.biHeight	= rcClient.Height();
	bmih.biSize		= sizeof( bmih );
	bmih.biPlanes	= 1;

	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection( dcImage, (BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );
	CBitmap *psrcBitmap = CBitmap::FromHandle( hDIBSection );

	CBitmap* pOldBitmap = dcImage.SelectObject( psrcBitmap );

	m_pBities = pdibBits;
	m_bmih = bmih;

 	m_nMultiPage = 0;

	m_pDC = &dcImage;
	dcImage.FillRect( &rcClient, &CBrush( RGB( 255, 255, 255 ) ) );

	if( sptrView )
		sptrView->Draw( dcImage );

	if( m_nMultiPage == 1 )
	{
		IUnknown *punkI = CreateTypedObject( "Image" );
		CImageWrp wrpI( punkI );
		punkI ->Release();

		wrpI.AttachDIBBits( &bmih, pdibBits );

		IFileFilter2Ptr	ptrFileFilter;
		
		ptrFileFilter.CreateInstance( "FileFilters2.JPGFilter" );

		if( ptrFileFilter == 0 )
			return;	

		IFileDataObjectPtr	ptrFile = ptrFileFilter;
		if( ptrFile == 0 )
			return;	

		ptrFileFilter->AttachNamedData( GetAppUnknown(), 0 );
		ptrFileFilter->AttachNamedObject( wrpI );

		if( ptrFile->SaveFile( _bstr_t( m_strFileName + ".jpg" ) ) != S_OK )
			return;

		ptrFileFilter->AttachNamedData( 0, 0 );
		ptrFileFilter->AttachNamedObject( 0 );
	}
	else
	{
		IUnknown *punkI = CreateTypedObject( "Image" );
		CImageWrp wrpI( punkI );
		punkI ->Release();

		wrpI.AttachDIBBits( &m_bmih, m_pBities );

		IFileFilter2Ptr	ptrFileFilter;
		
		ptrFileFilter.CreateInstance( "FileFilters2.JPGFilter" );

		if( ptrFileFilter == 0 )
			return;	

		IFileDataObjectPtr	ptrFile = ptrFileFilter;
		if( ptrFile == 0 )
			return;	

		ptrFileFilter->AttachNamedData( GetAppUnknown(), 0 );
		ptrFileFilter->AttachNamedObject( wrpI );

		CString strFileName;

		strFileName.Format( "_%d", m_nMultiPage  );

		strFileName = m_strFileName + strFileName + ".jpg";
		if( ptrFile->SaveFile( _bstr_t( strFileName ) ) != S_OK )
			return;

		ptrFileFilter->AttachNamedData( 0, 0 );
		ptrFileFilter->AttachNamedObject( 0 );

		m_pDC->FillRect( &m_rcClientPrint, &CBrush( RGB( 255, 255, 255 ) ) );
	}

	::DeleteObject( hDIBSection );
}

LRESULT CPrintPreview::OnPrintProgress(WPARAM wParam, LPARAM lParam)
{
	m_nMultiPage++;

	if( m_nMultiPage > 1 )
	{
		IUnknown *punkI = CreateTypedObject( "Image" );
		CImageWrp wrpI( punkI );
		punkI ->Release();

		wrpI.AttachDIBBits( &m_bmih, m_pBities );

		IFileFilter2Ptr	ptrFileFilter;
		
		ptrFileFilter.CreateInstance( "FileFilters2.JPGFilter" );

		if( ptrFileFilter == 0 )
			return 0;	

		IFileDataObjectPtr	ptrFile = ptrFileFilter;
		if( ptrFile == 0 )
			return 0;	

		ptrFileFilter->AttachNamedData( GetAppUnknown(), 0 );
		ptrFileFilter->AttachNamedObject( wrpI );

		CString strFileName;

		strFileName.Format( "_%d", m_nMultiPage - 1  );

		strFileName = m_strFileName + strFileName + ".jpg";
		if( ptrFile->SaveFile( _bstr_t( strFileName ) ) != S_OK )
			return 0;

		ptrFileFilter->AttachNamedData( 0, 0 );
		ptrFileFilter->AttachNamedObject( 0 );

		m_pDC->FillRect( &m_rcClientPrint, &CBrush( RGB( 255, 255, 255 ) ) );
	}

	return 1L;

}