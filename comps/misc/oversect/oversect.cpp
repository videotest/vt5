// oversect.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "oversect.h"
#include "math.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// COversectApp

BEGIN_MESSAGE_MAP(COversectApp, CWinApp)
	//{{AFX_MSG_MAP(COversectApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COversectApp construction

COversectApp::COversectApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only COversectApp object

COversectApp theApp;

/////////////////////////////////////////////////////////////////////////////
// COversectApp initialization

BOOL COversectApp::InitInstance()
{
	VERIFY(LoadVTClass(this) == true);
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleObjectFactory::RegisterAll();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NeedComponentInfo(rclsid, riid))
		return GetComponentInfo(ppv);
	return AfxDllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll();

	CActionRegistrator	ra;
	ra.RegisterActions();

	CDataRegistrator	rd;
	rd.RegisterDataObject();

	return S_OK;
}


// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CActionRegistrator	ra;
	ra.RegisterActions( false );

	CDataRegistrator	rd;
	rd.RegisterDataObject( false );


	COleObjectFactory::UpdateRegistryAll( false );
	return S_OK;
}


// {F4F79074-4F42-4c56-8552-39ED3351EBF5}
GUARD_IMPLEMENT_OLECREATE(COverlaySection, "OverSect.OverlaySection", 
0xf4f79074, 0x4f42, 0x4c56, 0x85, 0x52, 0x39, 0xed, 0x33, 0x51, 0xeb, 0xf5);
// {C58765F7-E1AB-454b-A6AE-79DBD5B531CE}
static const GUID clsidOverlaySectInfo = 
{ 0xc58765f7, 0xe1ab, 0x454b, { 0xa6, 0xae, 0x79, 0xdb, 0xd5, 0xb5, 0x31, 0xce } };

IMPLEMENT_DYNCREATE(COverlaySection, CDataObjectBase);
DATA_OBJECT_INFO_FULL(IDS_SECTION_TYPE, COverlaySection, szTypeOverlaySection, szDrawingObjectList, clsidOverlaySectInfo, IDI_OVERLAYSECTION);

BEGIN_INTERFACE_MAP(COverlaySection, CDataObjectBase)
	INTERFACE_PART(COverlaySection, IID_IDrawObject, Draw)
	INTERFACE_PART(COverlaySection, IID_IOverlaySection, OvrSect)
	INTERFACE_PART(COverlaySection, IID_IRectObject, Rect)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(COverlaySection, OvrSect)
IMPLEMENT_UNKNOWN(COverlaySection, Rect)

COverlaySection::COverlaySection()
{
	_OleLockApp( this );
	m_point1 =m_point2 = CPoint( 0, 0 );
	m_rect.SetRectEmpty();
	m_dwDir = 0;
	m_sizeImage = CSize( 0, 0 );
}

COverlaySection::~COverlaySection()
{
	_OleUnlockApp( this );
}

HRESULT COverlaySection::XOvrSect::SetPoints( POINT point1, POINT point2 )
{
	METHOD_PROLOGUE_EX(COverlaySection, OvrSect)
	pThis->m_point1 = point1;
	pThis->m_point2 = point2;
	return S_OK;
}
HRESULT COverlaySection::XOvrSect::GetPoints( POINT *ppoint1, POINT *ppoint2 )
{
	METHOD_PROLOGUE_EX(COverlaySection, OvrSect)
	*ppoint1 = pThis->m_point1;
	*ppoint2 = pThis->m_point2;
	return S_OK;
}

HRESULT COverlaySection::XOvrSect::SetDirection( DWORD dwDir )
{
	METHOD_PROLOGUE_EX(COverlaySection, OvrSect)
	pThis->m_dwDir = dwDir;
	return S_OK;
}

HRESULT COverlaySection::XRect::HitTest( POINT	point, long *plHitTest )
{
	METHOD_PROLOGUE_EX(COverlaySection, Rect);
	CRect	rect;
	GetRect( &rect );
	*plHitTest = rect.PtInRect( point );
	return S_OK;
}
HRESULT COverlaySection::XRect::Move( POINT point )
{
	METHOD_PROLOGUE_EX(COverlaySection, Rect)
		
	CRect	rect;
	GetRect( &rect );

	rect.right = point.x + rect.Width();
	rect.bottom = point.y + rect.Height();
	rect.left = point.x;
	rect.top = point.y;

	SetRect( rect );
	return S_OK;
}


HRESULT COverlaySection::XRect::SetRect(RECT rc)
{
	METHOD_PROLOGUE_EX(COverlaySection, Rect)

	//pThis->AttachImage();

	int	dx1 = pThis->m_rect.left-pThis->m_point1.x;
	int	dx2 = pThis->m_rect.right-pThis->m_point2.x;
	int	dy1 = pThis->m_rect.top-pThis->m_point1.y;
	int	dy2 = pThis->m_rect.bottom-pThis->m_point2.y;

	if( pThis->m_dwDir == 0 )
	{
		pThis->m_point1.x+=rc.left-pThis->m_rect.left;
		pThis->m_point2.x+=rc.left-pThis->m_rect.left;

		pThis->m_point1.y+=rc.top-pThis->m_rect.top;
		pThis->m_point2.y+=rc.top-pThis->m_rect.top;
	}
	else
	if( pThis->m_dwDir == 1 )
	{
		pThis->m_point1.y+=rc.top-pThis->m_rect.top;
		pThis->m_point2.y+=rc.top-pThis->m_rect.top;
		pThis->m_point1.x = 0;
		pThis->m_point2.x = pThis->m_sizeImage.cx;
	}
	else
	if( pThis->m_dwDir == 2 )
	{
		pThis->m_point1.x+=rc.left-pThis->m_rect.left;
		pThis->m_point2.x+=rc.left-pThis->m_rect.left;	
		pThis->m_point1.y = 0;
		pThis->m_point2.y = pThis->m_sizeImage.cy;
	}

	pThis->m_rect.left = pThis->m_point1.x+dx1;
	pThis->m_rect.right = pThis->m_point2.x+dx2;
	pThis->m_rect.top = pThis->m_point1.y+dy1;
	pThis->m_rect.bottom = pThis->m_point2.y+dy2;

	return S_OK;
}

HRESULT COverlaySection::XRect::GetRect(RECT* prc)
{
	METHOD_PROLOGUE_EX(COverlaySection, Rect)
	*prc = pThis->m_rect;
	return S_OK;
}




bool COverlaySection::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	if( !CDataObjectBase::SerializeObject( ar, pparams ) )
		return false;

	long	lVersion = 0;
	GuidKey	key = GetObjectKey( m_image );
	if( ar.IsLoading() )
	{
		ar>>lVersion>>m_point1.x>>m_point1.y>>m_point2.x>>m_point2.y>>key>>(long&)m_dwDir;
		ar.Read( &m_rect, sizeof( m_rect ) );
		ar.Read( &m_sizeImage, sizeof( m_sizeImage ) );
		
		/*m_image.Attach( ::FindObjectByKeyEx( key ), false );
		if( m_image == 0 )
			m_image = ::GetActiveObjectFromDocument( m_punkNamedData, szTypeImage );

		if( m_image != 0 )
			m_image->GetSize( (int*)&m_sizeImage.cx, (int*)&m_sizeImage.cy );
		else
			m_sizeImage = CSize( 0, 0 );*/
		//AttachImage();
	}
	else
	{
		ar<<lVersion<<m_point1.x<<m_point1.y<<m_point2.x<<m_point2.y<<key<<(long)m_dwDir;
		ar.Write( &m_rect, sizeof( m_rect ) );
		ar.Write( &m_sizeImage, sizeof( m_sizeImage ) );
	}

	return true;
}

void	COverlaySection::AttachImage()
{
	//if( m_image != 0 )
	//	return;
	//m_image = 0;
	//IUnknown	*punkNewImage = 0;

	if( !CheckInterface( m_punkDrawTarget, IID_IImageView ) )
		return;

	IImageViewPtr	ptrV( m_punkDrawTarget );

	IUnknown	*punk = 0;
	ptrV->GetActiveImage( &punk, 0 );

	
	if( punk )
	{
		m_image  = punk;
		punk->Release();
	}
	else 
		return;

	m_image->GetSize( (int*)&m_sizeImage.cx, (int*)&m_sizeImage.cy );
	if( m_dwDir == 1 )
	{
		m_point1.x = 0;
		m_point2.x = m_sizeImage.cx-1;
	}
	else if( m_dwDir == 2 )
	{
		m_point1.y = 0;
		m_point2.y = m_sizeImage.cy-1;
	}
}

void COverlaySection::DoDraw( CDC &dc )
{
	AttachImage();

	CPoint	point1 = ConvertToWindow( m_punkDrawTarget, m_point1 );
	CPoint	point2 = ConvertToWindow( m_punkDrawTarget, m_point2 );

	dc.SetROP2( R2_COPYPEN );

	double	fZoom = ::GetZoom( m_punkDrawTarget );
	CPoint	pointScroll = ::GetScrollPos( m_punkDrawTarget );

	double	fAngle = ::atan2( double(m_point2.y - m_point1.y), double(m_point2.x-m_point1.x) );
	int		nMax = 100;

	CPen	pen( PS_SOLID, 0, ::GetValueColor( ::GetAppUnknown(), "\\Colors", "OverlaySection", RGB( 255, 0, 0 ) ) );
	CPen	*ppenOld = 
	dc.SelectObject( &pen );

	dc.MoveTo( point1 );
	dc.LineTo( point2 );

	int	nSteps = max( abs( m_point2.y-m_point1.y ), abs( m_point2.x-m_point1.x ) )+1;
	POINT	*ppt = new POINT[nSteps];
	double	xr, yr;

	CRect	rect = m_image.GetRect();
	for( int i = 0; i < nSteps; i++ )
	{
		double	fx = m_point1.x+1.0*(m_point2.x-m_point1.x)/nSteps*i;
		double	fy = m_point1.y+1.0*(m_point2.y-m_point1.y)/nSteps*i;

		CPoint	pointImage = CPoint( (int)fx, (int)fy );
		color	val = 0;
		if( rect.PtInRect( pointImage ) )
			val =*(m_image.GetRow( pointImage.y, 0 )+(int)pointImage.x );

		double	f = 1.0*val/colorMax*nMax;

		xr = (fx+f*cos( fAngle-3.14/2 ));
		yr = (fy+f*sin( fAngle-3.14/2 ));

		ppt[i].x = int( xr*fZoom-pointScroll.x );
		ppt[i].y = int( yr*fZoom-pointScroll.y );

	}

	dc.Polyline( ppt, nSteps );

	CPen	pen_dot( PS_DASH, 0, ::GetValueColor( ::GetAppUnknown(), "\\Colors", "OverlaySection", RGB( 255, 0, 0 ) ) );
	dc.SelectObject( &pen_dot );
	dc.SetBkMode( TRANSPARENT );
	{
		double	f = 1.0*nMax;

		xr = (m_point1.x+f*cos( fAngle-3.14/2 ));
		yr = (m_point1.y+f*sin( fAngle-3.14/2 ));
		point1.x = int( xr*fZoom-pointScroll.x );
		point1.y = int( yr*fZoom-pointScroll.y );
		xr = (m_point2.x+f*cos( fAngle-3.14/2 ));
		yr = (m_point2.y+f*sin( fAngle-3.14/2 ));
		point2.x = int( xr*fZoom-pointScroll.x );
		point2.y = int( yr*fZoom-pointScroll.y );

		dc.MoveTo( point1 );
		dc.LineTo( point2 );
	}

	delete ppt;


	dc.SelectObject( ppenOld );

	m_rect.left = min( m_point1.x, m_point2.x );
	m_rect.right = max( m_point1.x, m_point2.x )+1;
	m_rect.top = min( m_point1.y, m_point2.y );
	m_rect.bottom = max( m_point1.y, m_point2.y )+1;

	xr = (m_point1.x+nMax*cos( fAngle-3.14/2 ));
	yr = (m_point1.y+nMax*sin( fAngle-3.14/2 ));

	m_rect.left = min( (int)xr, m_rect.left );
	m_rect.right = max( (int)xr+1, m_rect.right );
	m_rect.top = min( (int)yr, m_rect.top );
	m_rect.bottom = max( (int)yr+1, m_rect.bottom );


	xr = (m_point2.x+nMax*cos( fAngle-3.14/2 ));
	yr = (m_point2.y+nMax*sin( fAngle-3.14/2 ));

	m_rect.left = min( (int)xr, m_rect.left );
	m_rect.right = max( (int)xr+1, m_rect.right );
	m_rect.top = min( (int)yr, m_rect.top );
	m_rect.bottom = max( (int)yr+1, m_rect.bottom );

}
//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionOverlaySectionFree, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionOverlaySectionVert, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionOverlaySectionHorz, CCmdTargetEx);


// olecreate 
// {67D47FCA-5DE8-4aef-AAC3-A4FA196F0C6B}
GUARD_IMPLEMENT_OLECREATE(CActionOverlaySectionFree, "OvrSect.OverlaySectionFree",
0x67d47fca, 0x5de8, 0x4aef, 0xaa, 0xc3, 0xa4, 0xfa, 0x19, 0x6f, 0xc, 0x6b);
// {6618D622-BDCC-47fb-AE2C-6BE4067625CC}
GUARD_IMPLEMENT_OLECREATE(CActionOverlaySectionVert, "OvrSect.OverlaySectionVert",
0x6618d622, 0xbdcc, 0x47fb, 0xae, 0x2c, 0x6b, 0xe4, 0x6, 0x76, 0x25, 0xcc);
// {0BD8D4BC-956A-4d5f-89AA-63F4F2B9473D}
GUARD_IMPLEMENT_OLECREATE(CActionOverlaySectionHorz, "OvrSect.OverlaySectionHorz",
0xbd8d4bc, 0x956a, 0x4d5f, 0x89, 0xaa, 0x63, 0xf4, 0xf2, 0xb9, 0x47, 0x3d);
// guidinfo 

// {549EB3DF-B432-4e1f-B7E9-A147B18C8CBA}
static const GUID guidOverlaySectionFree =
{ 0x549eb3df, 0xb432, 0x4e1f, { 0xb7, 0xe9, 0xa1, 0x47, 0xb1, 0x8c, 0x8c, 0xba } };
// {73FBF089-CF0C-49a8-AC1F-568A45235DAA}
static const GUID guidOverlaySectionVert =
{ 0x73fbf089, 0xcf0c, 0x49a8, { 0xac, 0x1f, 0x56, 0x8a, 0x45, 0x23, 0x5d, 0xaa } };
// {22BFDD43-F3DA-45fb-886A-460D3DBD68C2}
static const GUID guidOverlaySectionHorz =
{ 0x22bfdd43, 0xf3da, 0x45fb, { 0x88, 0x6a, 0x46, 0xd, 0x3d, 0xbd, 0x68, 0xc2 } };


//[ag]6. action info

ACTION_INFO_FULL(CActionOverlaySectionFree, IDS_ACTION_SECTION, -1, -1, guidOverlaySectionFree);
ACTION_INFO_FULL(CActionOverlaySectionVert, IDS_ACTION_SECTION_VERT, -1, -1, guidOverlaySectionVert);
ACTION_INFO_FULL(CActionOverlaySectionHorz, IDS_ACTION_SECTION_HORZ, -1, -1, guidOverlaySectionHorz);
//[ag]7. targets

ACTION_TARGET(CActionOverlaySectionFree, szTargetViewSite);
ACTION_TARGET(CActionOverlaySectionVert, szTargetViewSite);
ACTION_TARGET(CActionOverlaySectionHorz, szTargetViewSite);
//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionOverlaySectionFree implementation
CActionOverlaySectionFree::CActionOverlaySectionFree()
{
	m_nIDCursor = IDC_SECTION;
	m_dwDir = 0;
}

CActionOverlaySectionFree::~CActionOverlaySectionFree()
{
}

bool CActionOverlaySectionFree::Initialize()
{
	IImageViewPtr	ptrV( m_punkTarget );
	if( ptrV == 0 )return false;
	IUnknown	*punkImage = 0;
	ptrV->GetActiveImage( &punkImage, 0 );
	if( !punkImage )return false;
	IImagePtr	ptrImage( punkImage );
	punkImage->Release();

	ptrImage->GetSize( (int*)&m_sizeImage.cx, (int*)&m_sizeImage.cy );
	
	m_hcurActive = AfxGetApp()->LoadCursor( m_nIDCursor );
	
	if( !CInteractiveActionBase::Initialize() )return false;
	
	return true;
}

bool CActionOverlaySectionFree::Invoke()
{
	if( m_ptrSection == 0  )
		return false;

	IViewPtr	ptrV( m_punkTarget );
	IUnknown	*punkDoc = 0;
	ptrV->GetDocument( &punkDoc );

	ASSERT( punkDoc );
	IUnknownPtr	ptrDoc = punkDoc;
	punkDoc->Release();


	IUnknown	*punk = ::GetActiveObjectFromContext( m_punkTarget, szDrawingObjectList );
	if( !punk )
	{
		punk = ::CreateTypedObject( szDrawingObjectList );
		if( !punk  )
		{
			AfxMessageBox( IDS_ERR_CANTCREATEOBJECTLIST );
			return false;
		}
		IUnknown	*punkImage = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );
		if( punkImage )
		{
			SetBaseObject( punk, punkImage, ptrDoc );
			punkImage->Release();
		}
		m_changes.SetToDocData( ptrDoc, punk );	

	}
	{
		INamedDataObject2Ptr	ptrN( (IUnknown*)m_ptrSection );
		ptrN->SetParent( punk, 0 );
	}
	punk->Release();

	m_changes.SetToDocData( ptrDoc, m_ptrSection );

	m_lTargetKey = ::GetObjectKey( ptrDoc );
	LeaveMode();

	return true;
}

//undo/redo
bool CActionOverlaySectionFree::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionOverlaySectionFree::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

void CActionOverlaySectionFree::AjustPointLocations( bool bFirstPoint )
{
}

void CActionOverlaySectionHorz::AjustPointLocations( bool bFirstPoint )
{
	m_point1.x = 0;
	m_point2.x = m_sizeImage.cx;
	m_point1.y = m_point2.y;
}

void CActionOverlaySectionVert::AjustPointLocations( bool bFirstPoint )
{
	m_point1.y = 0;
	m_point2.y = m_sizeImage.cy;
	m_point1.x = m_point2.x;
}

void CActionOverlaySectionFree::DoDraw( CDC &dc )
{
	if( m_ptrSectionDraw != 0 )
		m_ptrSectionDraw->Paint( dc, m_rcInvalid, m_punkTarget, m_pbi, m_pdib );
}
bool CActionOverlaySectionFree::DoStartTracking( CPoint point )
{
	IUnknown	*punkDataObject = ::CreateTypedObject( szTypeOverlaySection );
	if( !punkDataObject )
	{
		AfxMessageBox( IDS_ERR_CANTCREATEOBJECTLIST );
		LeaveMode();
		return false;
	}
	m_ptrSection = punkDataObject;
	m_ptrSectionDraw = punkDataObject;
	punkDataObject->Release();

	m_ptrSection->SetDirection( m_dwDir );



	m_point1 = point;
	m_point2 = point;

	AjustPointLocations( true );
	m_ptrSection->SetPoints( m_point1, m_point2 );
	Invalidate();
	return true;
}
bool CActionOverlaySectionFree::DoTrack( CPoint point )
{
	Invalidate();
	
	m_point2 = point;
	AjustPointLocations( false );

	m_ptrSection->SetPoints( m_point1, m_point2 );

	Invalidate();

	return true;
}

bool CActionOverlaySectionFree::DoFinishTracking( CPoint point )
{
	Finalize();
	return true;
}

void CActionOverlaySectionFree::Invalidate()
{
	CWnd	*p = GetWindowFromUnknown( m_punkTarget );
	if( p->GetSafeHwnd() )p->Invalidate();
}

bool CActionOverlaySectionFree::IsAvaible()
{
	return CheckInterface( m_punkTarget, IID_IImageView );
}