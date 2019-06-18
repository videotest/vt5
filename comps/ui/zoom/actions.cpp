#include "stdafx.h"
#include "actions.h"
#include "resource.h"
#include "math.h"

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionFit2Image, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFitDoc2Screen, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionZoom, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionViewZoom11, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionViewZoom41, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionViewZoom31, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionViewZoom21, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionViewZoom12, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionViewZoom13, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionViewZoom14, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionViewZoomOut, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionViewZoomIn, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionViewZoomFit, CCmdTargetEx);

// olecreate 

// {FFD0ED70-78C0-46e6-9709-F09A5365AC9D}
GUARD_IMPLEMENT_OLECREATE(CActionFit2Image, "zoom.Fit2Image",
0xffd0ed70, 0x78c0, 0x46e6, 0x97, 0x9, 0xf0, 0x9a, 0x53, 0x65, 0xac, 0x9d);
// {D9344029-F5A1-48f6-9B25-808A4C18AA05}
GUARD_IMPLEMENT_OLECREATE(CActionFitDoc2Screen, "zoom.FitDoc2Screen",
0xd9344029, 0xf5a1, 0x48f6, 0x9b, 0x25, 0x80, 0x8a, 0x4c, 0x18, 0xaa, 0x5);
// {44340807-4A33-11d3-A619-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionZoom, "Zoom.Zoom",
0x44340807, 0x4a33, 0x11d3, 0xa6, 0x19, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {91DDA55B-4678-11d3-A614-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionViewZoom11, "Zoom.ViewZoom11",
0x91dda55b, 0x4678, 0x11d3, 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {91DDA557-4678-11d3-A614-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionViewZoom41, "Zoom.ViewZoom41",
0x91dda557, 0x4678, 0x11d3, 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {91DDA553-4678-11d3-A614-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionViewZoom31, "Zoom.ViewZoom31",
0x91dda553, 0x4678, 0x11d3, 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {91DDA54F-4678-11d3-A614-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionViewZoom21, "Zoom.ViewZoom21",
0x91dda54f, 0x4678, 0x11d3, 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {91DDA54B-4678-11d3-A614-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionViewZoom12, "Zoom.ViewZoom12",
0x91dda54b, 0x4678, 0x11d3, 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {91DDA547-4678-11d3-A614-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionViewZoom13, "Zoom.ViewZoom13",
0x91dda547, 0x4678, 0x11d3, 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {91DDA543-4678-11d3-A614-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionViewZoom14, "Zoom.ViewZoom14",
0x91dda543, 0x4678, 0x11d3, 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {7E72DC37-4661-11d3-A614-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionViewZoomOut, "Zoom.ViewZoomOut",
0x7e72dc37, 0x4661, 0x11d3, 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {7E72DC33-4661-11d3-A614-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionViewZoomIn, "Zoom.ViewZoomIn",
0x7e72dc33, 0x4661, 0x11d3, 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {D070386B-8FD9-4732-A27E-5110210FB5AA}
GUARD_IMPLEMENT_OLECREATE(CActionViewZoomFit, "Zoom.ViewZoomFit", 
0xd070386b, 0x8fd9, 0x4732, 0xa2, 0x7e, 0x51, 0x10, 0x21, 0xf, 0xb5, 0xaa);

// guidinfo 

// {FE6E5727-D771-402d-8390-60D3CCA446B6}
static const GUID guidFit2Image =
{ 0xfe6e5727, 0xd771, 0x402d, { 0x83, 0x90, 0x60, 0xd3, 0xcc, 0xa4, 0x46, 0xb6 } };
// {16555B0F-7D80-49dc-9E25-9AF2B6FD876A}
static const GUID guidFitDoc2Screen =
{ 0x16555b0f, 0x7d80, 0x49dc, { 0x9e, 0x25, 0x9a, 0xf2, 0xb6, 0xfd, 0x87, 0x6a } };
// {44340805-4A33-11d3-A619-0090275995FE}
static const GUID guidZoom =
{ 0x44340805, 0x4a33, 0x11d3, { 0xa6, 0x19, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {91DDA559-4678-11d3-A614-0090275995FE}
static const GUID guidViewZoom11 =
{ 0x91dda559, 0x4678, 0x11d3, { 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {91DDA555-4678-11d3-A614-0090275995FE}
static const GUID guidViewZoom41 =
{ 0x91dda555, 0x4678, 0x11d3, { 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {91DDA551-4678-11d3-A614-0090275995FE}
static const GUID guidViewZoom31 =
{ 0x91dda551, 0x4678, 0x11d3, { 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {91DDA54D-4678-11d3-A614-0090275995FE}
static const GUID guidViewZoom21 =
{ 0x91dda54d, 0x4678, 0x11d3, { 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {91DDA549-4678-11d3-A614-0090275995FE}
static const GUID guidViewZoom12 =
{ 0x91dda549, 0x4678, 0x11d3, { 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {91DDA545-4678-11d3-A614-0090275995FE}
static const GUID guidViewZoom13 =
{ 0x91dda545, 0x4678, 0x11d3, { 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {91DDA541-4678-11d3-A614-0090275995FE}
static const GUID guidViewZoom14 =
{ 0x91dda541, 0x4678, 0x11d3, { 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {7E72DC35-4661-11d3-A614-0090275995FE}
static const GUID guidViewZoomOut =
{ 0x7e72dc35, 0x4661, 0x11d3, { 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {7E72DC31-4661-11d3-A614-0090275995FE}
static const GUID guidViewZoomIn =
{ 0x7e72dc31, 0x4661, 0x11d3, { 0xa6, 0x14, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {DF5F9661-AC1E-49ed-9DCE-F154F5711DA6}
static const GUID guidViewZoomFit = 
{ 0xdf5f9661, 0xac1e, 0x49ed, { 0x9d, 0xce, 0xf1, 0x54, 0xf5, 0x71, 0x1d, 0xa6 } };


//[ag]6. action info

ACTION_INFO_FULL(CActionFit2Image, ID_ACTION_FIT_2IMAGE, IDS_MENU_VIEW_ZOOM, IDS_TB_VIEW, guidFit2Image);
ACTION_INFO_FULL(CActionFitDoc2Screen, ID_ACTION_FIT_DOC2SCR, IDS_MENU_VIEW_ZOOM, IDS_TB_VIEW, guidFitDoc2Screen);
ACTION_INFO_FULL(CActionZoom, ID_ACTION_ZOOM, IDS_MENU_VIEW_ZOOM, IDS_TB_VIEW, guidZoom);
ACTION_INFO_FULL(CActionViewZoom11, ID_ACTION_ZOOM_1_1, IDS_MENU_VIEW_ZOOM, -1, guidViewZoom11);
ACTION_INFO_FULL(CActionViewZoom41, ID_ACTION_ZOOM_1_4, IDS_MENU_VIEW_ZOOM, -1, guidViewZoom41);
ACTION_INFO_FULL(CActionViewZoom31, ID_ACTION_ZOOM_1_3, IDS_MENU_VIEW_ZOOM, -1, guidViewZoom31);
ACTION_INFO_FULL(CActionViewZoom21, ID_ACTION_ZOOM_1_2, IDS_MENU_VIEW_ZOOM, -1, guidViewZoom21);
ACTION_INFO_FULL(CActionViewZoom12, ID_ACTION_ZOOM_2_1, IDS_MENU_VIEW_ZOOM, -1, guidViewZoom12);
ACTION_INFO_FULL(CActionViewZoom13, ID_ACTION_ZOOM_3_1, IDS_MENU_VIEW_ZOOM, -1, guidViewZoom13);
ACTION_INFO_FULL(CActionViewZoom14, ID_ACTION_ZOOM_4_1, IDS_MENU_VIEW_ZOOM, -1, guidViewZoom14);
ACTION_INFO_FULL(CActionViewZoomOut, ID_ACTION_ZOOM_OUT, IDS_MENU_VIEW, IDS_TB_VIEW, guidViewZoomOut);
ACTION_INFO_FULL(CActionViewZoomIn, ID_ACTION_ZOOM_IN, IDS_MENU_VIEW, IDS_TB_VIEW, guidViewZoomIn);
ACTION_INFO_FULL(CActionViewZoomFit, ID_ACTION_ZOOM_FIT, IDS_MENU_VIEW, IDS_TB_VIEW, guidViewZoomFit);

//[ag]7. targets

ACTION_TARGET(CActionFit2Image, szTargetViewSite);
ACTION_TARGET(CActionFitDoc2Screen, szTargetAnydoc);
ACTION_TARGET(CActionViewZoom11, szTargetViewSite);
ACTION_TARGET(CActionViewZoom41, szTargetViewSite);
ACTION_TARGET(CActionViewZoom31, szTargetViewSite);
ACTION_TARGET(CActionViewZoom21, szTargetViewSite);
ACTION_TARGET(CActionViewZoom12, szTargetViewSite);
ACTION_TARGET(CActionViewZoom13, szTargetViewSite);
ACTION_TARGET(CActionViewZoom14, szTargetViewSite);
ACTION_TARGET(CActionViewZoomOut, szTargetViewSite);
ACTION_TARGET(CActionViewZoomIn, szTargetViewSite);
ACTION_TARGET(CActionZoom, szTargetViewSite);
ACTION_TARGET(CActionViewZoomFit, szTargetViewSite);

//[ag]8. arguments
ACTION_ARG_LIST(CActionZoom)
	ARG_DOUBLE(_T("Zoom"), 0)
END_ACTION_ARG_LIST()
ACTION_ARG_LIST(CActionFitDoc2Screen)
	ARG_DOUBLE(_T("MaxZoom"), 100.0)
END_ACTION_ARG_LIST()


//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionFit2Image implementation
CActionFit2Image::CActionFit2Image()
{
}

CActionFit2Image::~CActionFit2Image()
{
}

bool CActionFit2Image::Invoke()
{
	IViewSitePtr	sptrV(m_punkTarget);
	if(sptrV == 0)
		return false;
	IUnknown* punkFrame = 0;
	sptrV->GetFrameWindow(&punkFrame);
	IFrameWindowPtr	sptrFrame(punkFrame);
	if(punkFrame)
		punkFrame->Release();
	if(sptrFrame == 0)
		return false;

	sptrFrame->Restore();

	IUnknown* punkSplitter = 0;
	sptrFrame->GetSplitter(&punkSplitter);
	ISplitterWindowPtr	sptrSplitter(punkSplitter);
	if(punkSplitter)
		punkSplitter->Release();
	if(sptrSplitter == 0)
		return false;

	sptrSplitter->OptimizeSplitter();

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionFitDoc2Screen implementation
CActionFitDoc2Screen::CActionFitDoc2Screen()
{
}

CActionFitDoc2Screen::~CActionFitDoc2Screen()
{
}

bool CActionFitDoc2Screen::Invoke()
{
	//try maximize active doc frame
	IDocumentSitePtr sptrDoc(m_punkTarget);
	if(sptrDoc == 0)
		return false;
	IUnknown* punkView = 0;
	sptrDoc->GetActiveView(&punkView);
	IViewSitePtr	sptrV(punkView);
	if(punkView)
		punkView->Release();
	if(sptrV == 0)
		return false;
	IUnknown* punkFrame = 0;
	sptrV->GetFrameWindow(&punkFrame);
	IFrameWindowPtr	sptrFrame(punkFrame);
	if(punkFrame)
		punkFrame->Release();
	if(sptrFrame == 0)
		return false;
	sptrFrame->Maximize();

	CZoomUndoable zoom; 
	if( !zoom.IsAvaible( sptrV ) )
		return true;

	double fMaxZoom = GetArgumentDouble( _T("MaxZoom") );

	//set zooms
	long lPos = 0;
	sptrDoc->GetFirstViewPosition(&lPos);
	while(lPos)
	{
		sptrDoc->GetNextView(&punkView, &lPos);
		IScrollZoomSitePtr	sptrSite(punkView);
		IWindowPtr	sptrWnd(punkView);
		if(punkView)
			punkView->Release();
		if(sptrWnd == 0)
			continue;
		if(sptrSite == 0)
			continue;
		HWND hWnd = 0;
		sptrWnd->GetHandle((HANDLE*)&hWnd);
		if(hWnd == 0)
			continue;
		CRect rcClient = NORECT;
		::GetClientRect(hWnd, &rcClient);

		CSize sizeClient(0, 0);
		sptrSite->GetClientSize(&sizeClient);

		double fZoom = 1;
		double fZoomX = rcClient.Width()/((double)sizeClient.cx+5);
		double fZoomY = rcClient.Height()/((double)sizeClient.cy+5);
		fZoom = fZoomX > fZoomY ? fZoomY : fZoomX;
		fZoom = min(fZoom, fMaxZoom);
		sptrSite->SetZoom(fZoom);
	}

	return true;
}

double Zoom( double f )
{
	if( f >= 1 )
		return min( f, 16 );
	f -= 2;
	f = -f;
	return max( 1/f, 1./16 );
}

double ZoomBack( double f )
{
	if( f >= 1 )
		return min( f, 16 );

	f = -1./f;
	f += 2;

	return max( -14, f );
}

void AjustZoom( IUnknown *punkView )
{
	IViewSitePtr	sptrV(punkView);
	if(sptrV == 0)
		return;
	IUnknown* punkFrame = 0;
	sptrV->GetFrameWindow(&punkFrame);
	IFrameWindowPtr	sptrFrame(punkFrame);
	if(punkFrame)
		punkFrame->Release();
	if(sptrFrame == 0)
		return;
	
	IUnknown* punkSplitter = 0;
	sptrFrame->GetSplitter(&punkSplitter);
	ISplitterWindowPtr	sptrSplitter(punkSplitter);
	if(punkSplitter)
		punkSplitter->Release();
	if(sptrSplitter == 0)
		return;

	sptrSplitter->OptimizeSplitter();
}
//////////////////////////////////////////////////////////////////////
//CActionViewZoom11 implementation
bool CActionViewZoom11::Invoke()
{
	sptrIScrollZoomSite	sptrSite( m_punkTarget );
	sptrIScrollZoomSite2	sptrSite2( m_punkTarget );
	if( sptrSite2 )	sptrSite2->SetFitDoc(FALSE);

	CPoint	pointScroll;
	sptrSite->GetScrollPos( &pointScroll );
	
	CRect	rectClient;
	CWnd	*pwnd = GetWindowFromUnknown( sptrSite );
	pwnd->GetClientRect( rectClient );

	CPoint	pointCenterClient = ::ConvertToClient( m_punkTarget, rectClient.CenterPoint() );

	double	fNewZoom = GetZoom();
	sptrSite->SetZoom( fNewZoom );

	CPoint	pointCenterClientNew = ::ConvertToClient( m_punkTarget, rectClient.CenterPoint() );
	CSize	size = ::ConvertToWindow( m_punkTarget, pointCenterClientNew - pointCenterClient );

	sptrSite->GetScrollPos( &pointScroll );

	pointScroll.x -= size.cx;
	pointScroll.y -= size.cy;

	sptrSite->SetScrollPos( pointScroll );

	//paul 17.04.2003 to avoid ShellFrame Resize
	//AjustZoom( sptrSite );

	return true;
}

bool CActionViewZoom11::IsAvaible()
{ 
	CZoomUndoable zoom; 
	if( !zoom.IsAvaible( m_punkTarget ) ) return false;
	
	sptrIScrollZoomSite	sptrSite( m_punkTarget );
	double	fZoom = 1;
	sptrSite->GetZoom( &fZoom );
	if(fabs(GetZoom()-fZoom)<0.001) return false;

	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionViewZoomOut implementation
CActionViewZoomOut::CActionViewZoomOut()
{
}

CActionViewZoomOut::~CActionViewZoomOut()
{
}

double CActionViewZoomOut::GetZoom()
{
	sptrIScrollZoomSite	sptrSite( m_punkTarget );

	double	fZoom = 1;
	sptrSite->GetZoom( &fZoom );

	double	f = ZoomBack( fZoom );
	return Zoom( f-1 );
}

//////////////////////////////////////////////////////////////////////
//CActionViewZoomIn implementation
CActionViewZoomIn::CActionViewZoomIn()
{
	
}

CActionViewZoomIn::~CActionViewZoomIn()
{
}

double CActionViewZoomIn::GetZoom()
{
	sptrIScrollZoomSite	sptrSite( m_punkTarget );

	double	fZoom = 1;
	sptrSite->GetZoom( &fZoom );

	double	f = ZoomBack( fZoom );
	return Zoom( f+1 );
}


//////////////////////////////////////////////////////////////////////
//CActionZoom implementation
CActionZoom::CActionZoom()
{
	m_bUseFrame = false;
	m_fZoom = 1;
	m_fAspect = 1;
	m_ptScrollPos = CPoint( 0, 0 );
	m_hcurZoomIn = AfxGetApp()->LoadCursor( IDC_ZOOMIN );
	m_hcurZoomOut = AfxGetApp()->LoadCursor( IDC_ZOOMOUT );
	m_rect = CRect( 0, 0, 0, 0 );
}

CActionZoom::~CActionZoom()
{
}

bool CActionZoom::Invoke()
{
	sptrIScrollZoomSite2	sptrSite2( m_punkTarget );
	if( sptrSite2 )	sptrSite2->SetFitDoc(FALSE);

	sptrIScrollZoomSite	sptrSite( m_punkTarget );
	if( m_fZoom>0 )
	{
		if( sptrSite )	sptrSite->SetZoom( m_fZoom );
		if( sptrSite )	sptrSite->SetScrollPos( m_ptScrollPos );
	}

	//paul 17.04.2003 to avoid ShellFrame Resize
	//AjustZoom( m_punkTarget );

	return true;
}


void CActionZoom::DoDraw( CDC &dc )
{
	CRect	rectDraw = ::ConvertToWindow( m_punkTarget, m_rect );
	dc.Rectangle( rectDraw );
}

void CActionZoom::_CorrectRect()
{
	int	x, y;
	if( m_rect.Height() > 0 )		y = int( m_rect.top+abs( m_rect.Width() )*m_fAspect +.5);
	else							y = int( m_rect.top-abs( m_rect.Width() )*m_fAspect +.5);
	if( m_rect.Width() > 0 )		x = int( m_rect.left+abs( m_rect.Height() )/m_fAspect +.5);
	else							x = int( m_rect.left-abs( m_rect.Height() )/m_fAspect +.5);

	if( x > m_rect.right == m_rect.Width() > 0 )
		m_rect.right = x;
	else
		m_rect.bottom = y;


}

bool CActionZoom::DoStartTracking( CPoint pt )
{
	m_rect.right = m_rect.left = pt.x;
	m_rect.bottom = m_rect.top = pt.y;


	CRect	rcClient;
	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	pwnd->GetClientRect( rcClient );

	if( rcClient.Width() != 0 )
		m_fAspect = double( rcClient.Height() )/double( rcClient.Width() );
	_Draw();

	return true;
}

bool CActionZoom::DoTrack( CPoint pt )
{
	_Draw();
	
	m_rect.right = pt.x;
	m_rect.bottom = pt.y;
	_CorrectRect();

	_Draw();

	return true;
}

bool CActionZoom::DoFinishTracking( CPoint pt )
{
	_Draw();
	m_rect.right = pt.x;
	m_rect.bottom = pt.y;
	_CorrectRect();


	CRect	rcClient;
	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	pwnd->GetClientRect( rcClient );

	m_rect.NormalizeRect();

	m_fZoom = rcClient.Width()/max( m_rect.Width(), 1. );
	m_fZoom = max( 1./16, min( 16, m_fZoom ) );

	m_rect.NormalizeRect();
	m_ptScrollPos = CPoint( int( m_rect.left*m_fZoom+.5 ),
									int( m_rect.top*m_fZoom+.5 ) );

	m_bUseFrame = true;

	Finalize();

	return true;
}
bool CActionZoom::DoRButtonUp( CPoint pt )
{
	m_fZoom = 1;
	Finalize();

	return true;
}

bool IsKeyPressed( UINT vk )
{
	return ::GetAsyncKeyState( vk ) != 0;
}

bool CActionZoom::DoLButtonUp( CPoint pt )
{
	if( m_bUseFrame )return true;

	CRect	rcClient;
	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	pwnd->GetClientRect( rcClient );

	sptrIScrollZoomSite	sptrSite( m_punkTarget );
	sptrSite->GetZoom( &m_fZoom );

	if( IsKeyPressed( VK_SHIFT ) )
		m_fZoom = Zoom( ZoomBack( m_fZoom ) - 1 );
	else
		m_fZoom = Zoom( ZoomBack( m_fZoom ) + 1 );

	m_fZoom = max( 1./16, min( 16, m_fZoom ) );
	m_ptScrollPos = CPoint( int( pt.x*m_fZoom-rcClient.CenterPoint().x+.5),
							int( pt.y*m_fZoom-rcClient.CenterPoint().y+.5) );
	Finalize();

	return true;
}

bool CActionZoom::Initialize()
{
	BOOL	bAuto = TRUE;

	IScrollZoomSitePtr	ptrSZ( m_punkTarget );
	if( ptrSZ == 0 )
		return false;
	
	ptrSZ->GetAutoScrollMode( &bAuto );

	if( !bAuto )
	{
		LeaveMode();
		Terminate();
		return false;
	}

	m_fZoom = GetArgumentDouble( _T("Zoom") );

	if( !CInteractiveActionBase::Initialize() )return false;

	if( m_fZoom > 0 )
	{
		m_fZoom = max( 1./16, min( 16, m_fZoom ) );
		Finalize();
		return false;
	}
	else if( m_fZoom == -1 )
	{ // особый случай: воспринимаем -1 как сигнал включить FitDoc
		Finalize();
		return false;
	}

	

	
	IWindowPtr ptr_wnd( m_punkTarget );
	if( ptr_wnd )
	{
		CPoint pt_cursor;
		::GetCursorPos( &pt_cursor );	
		HANDLE hwnd = 0;
		ptr_wnd->GetHandle( &hwnd );
		CRect rc_wnd;
		::GetWindowRect( (HWND)hwnd, &rc_wnd );
		if( rc_wnd.PtInRect( pt_cursor ) )
			::SetCursor( m_hcurZoomIn );		
	}
	

	return true;
	
}
void CActionZoom::Finalize()
{
	CInteractiveActionBase::Finalize();
	SetCursor( ::LoadCursor( 0,  IDC_ARROW ) );
	SetArgument( _T("Zoom"), _variant_t( m_fZoom ) );
}

bool CActionZoom::DoSetCursor( CPoint point )
{
	if( IsKeyPressed( VK_SHIFT ) && !m_bInTrackMode )
		m_hcurActive = m_hcurZoomOut;
	else
		m_hcurActive = m_hcurZoomIn;

	return CInteractiveActionBase::DoSetCursor( point );
}

bool CZoomUndoable::IsAvaible( IUnknown *punkTarget )
{
	if( !punkTarget )
		return false;

	IWindowPtr ptrWnd = punkTarget;

	if( ptrWnd == 0 )
		return false;

	DWORD dwFlags = 0;
	ptrWnd->GetWindowFlags( &dwFlags );
	if( dwFlags & wfZoomSupport )
		return true;

	return false;
}


//////////////////////////////////////////////////////////////////////
//CActionViewZoomFit implementation
bool CActionViewZoomFit::Invoke()
{
	sptrIScrollZoomSite2	sptrSite2( m_punkTarget );
	if( sptrSite2 )	sptrSite2->SetFitDoc(TRUE);

	return true;
}

bool CActionViewZoomFit::IsAvaible()
{ 
	CZoomUndoable zoom; 
	if( !zoom.IsAvaible( m_punkTarget ) ) return false;
	
	sptrIScrollZoomSite2	sptrSite2( m_punkTarget );
	if( sptrSite2 == 0 ) return false;

	return true;
}

bool CActionViewZoomFit::IsChecked()
{ 
	sptrIScrollZoomSite2	sptrSite2( m_punkTarget );

	BOOL bChecked = FALSE;
	if( sptrSite2 ) sptrSite2->GetFitDoc(&bChecked);

	return bChecked;
}
