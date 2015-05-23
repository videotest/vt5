// CarioAnalizeView.cpp : implementation file
//

#include "stdafx.h"
#include "chromosex.h"
#include "CarioAnalizeView.h"

#include "consts.h"
#include "menuconst.h"
#include "MenuRegistrator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "wnd_misc.h"

/////////////////////////////////////////////////////////////////////////////
// CCarioAnalizeView

IMPLEMENT_DYNCREATE(CCarioAnalizeView, CCarioView)

CCarioAnalizeView::CCarioAnalizeView()
{
	EnableAutomation();
	EnableAggregation();
	
	AfxOleLockApp();

	m_sName = szCarioAnalizeView;
	m_sUserName.LoadString( IDS_CARIO_ANALIZE_VIEW_NAME );

	m_carioViewMode		= cvmCellar;

	//m_pframeRuntime = 0;
	
}

CCarioAnalizeView::~CCarioAnalizeView()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();
}

void CCarioAnalizeView::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	if( GetSafeHwnd() )
		DestroyWindow();

	delete this;
}


BEGIN_MESSAGE_MAP(CCarioAnalizeView, CCarioView)
	//{{AFX_MSG_MAP(CCarioAnalizeView)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CCarioAnalizeView, CCarioView)
	//{{AFX_DISPATCH_MAP(CCarioAnalizeView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ICarioAnalizeView to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {FEE7B82E-2704-46C8-83C2-ADAE33B3FD30}
static const IID IID_ICarioAnalizeView =
{ 0xfee7b82e, 0x2704, 0x46c8, { 0x83, 0xc2, 0xad, 0xae, 0x33, 0xb3, 0xfd, 0x30 } };

BEGIN_INTERFACE_MAP(CCarioAnalizeView, CCarioView)
	INTERFACE_PART(CCarioAnalizeView, IID_ICarioAnalizeView, Dispatch)
END_INTERFACE_MAP()

// {D17CF4FD-F5EF-4788-840C-56DF29022D11}
GUARD_IMPLEMENT_OLECREATE(CCarioAnalizeView, szCarioAnalizeViewProgID, 0xd17cf4fd, 0xf5ef, 0x4788, 0x84, 0xc, 0x56, 0xdf, 0x29, 0x2, 0x2d, 0x11)

/////////////////////////////////////////////////////////////////////////////
// CCarioAnalizeView message handlers

int CCarioAnalizeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCarioView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CCarioAnalizeView::OnActivateView( bool bActivate, IUnknown *punkOtherView )
{
	if( !bActivate )
		statusShowObjectCountPage( false );
	else
		if( ::GetValueInt( ::GetAppUnknown(), OBJECT_COUNT_PANE_SECTION, "KaryoAnalizeView", 0L ) )
			statusShowObjectCountPage( bActivate );
}

LRESULT CCarioAnalizeView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{	
	if( message == UINT( WM_USER + 101 ) )
		return 1;
	else if( message == UINT( WM_USER + 102 ) )
	{
		if( (UINT)lParam != sizeof( POINT ) )
			return 0;

		POINT *pt = (POINT *)wParam;
		
		if( pt )
		{
			int l = -1, c = -1, ch = -1;
			
			POINT lpt = *pt;
			ScreenToClient( &lpt );
			CPoint ptObj = convert_from_window_to_object( CPoint( lpt ) );
			hit_test_area( ptObj, &l, &c, &ch );
			if( l != -1 && c != -1 && ch != -1 )
				return ch + 1;
		}
		return 0;
	}
	else if( message == WM_USER + 103 )
	{
		IUnknown *punkDoc = 0;
		IViewPtr sptrView = GetControllingUnknown();
		sptrView->GetDocument( &punkDoc );

		IDocumentSitePtr sptrDoc = punkDoc;
		if( punkDoc )
			punkDoc->Release();
		punkDoc = 0;

		if( sptrDoc != 0 )
			sptrDoc->SetActiveView( sptrView );

		CWnd *pWnd = ::GetWindowFromUnknown( sptrView );
		
		if( pWnd )
		{
			CWnd *pParWnd = pWnd->GetParent();

			if( pParWnd )
				pParWnd->SetFocus();
		}
		return 1;
	}


	return __super::WindowProc(message, wParam, lParam);
}
