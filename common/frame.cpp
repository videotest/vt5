#include "stdafx.h"
#include "math.h"
#include "afxpriv.h"

#include "frame.h"
#include "TimeTest.h"
#include "image5.h"
#include "docviewbase.h"
#include "object5.h"

#include "StreamEx.h"

IMPLEMENT_DYNCREATE(CFrame, CCmdTargetEx);

/////////////////////////////////////////////////////////////////////////////////////////
//CResizingFrame
//resizing/rotating frame
CResizingFrame::CResizingFrame( CRect rect, double fZoomX, double fZoomY, double fAngle )
{
	m_rect = rect;
	m_fZoomX = fZoomX;
	m_fZoomY = fZoomY;
	m_fAngle = fAngle;

	m_dwFlags = fcfNone;
	m_pointDelta = CPoint( 0, 0 );
	m_bRotate = false;
	m_bDragDropActive = false;

	ZeroMemory( &m_hcursors[0], sizeof( m_hcursors ) );
	m_punkView = 0;
	m_hwnd = 0;
}

CResizingFrame::~CResizingFrame()
{
}

void CResizingFrame::Init( IUnknown *punk, HWND hwnd )
{
	m_punkView = punk;
	m_hwnd = hwnd;
}

void CResizingFrame::SetFlags( DWORD dwFlags )
{
	m_dwFlags = dwFlags;

	ReLoadCursors();
}

#pragma optimize("", off)

void CResizingFrame::Draw(CDC& dc, CRect, BITMAPINFOHEADER *, byte *)
{
	if( m_dwFlags == fcfNone || m_rect == NORECT )
		return;

	if( m_bDragDropActive )
		return;

	dc.SelectStockObject( WHITE_BRUSH );
	dc.SelectStockObject( BLACK_PEN );

	double	fAngle = 0;
	CPoint	point;

	if( m_dwFlags & fcfResize )
	{
		point = ::ConvertToWindow( m_punkView, CalcPointLocation( Left, fAngle ) );
		_DrawPoint( dc, point, fAngle+A180 );
		point = ::ConvertToWindow( m_punkView, CalcPointLocation( Top, fAngle ) );
		_DrawPoint( dc, point, fAngle+A180 );
		point = ::ConvertToWindow( m_punkView, CalcPointLocation( Right, fAngle ) );
		_DrawPoint( dc, point, fAngle+A180 );
		point = ::ConvertToWindow( m_punkView, CalcPointLocation( Bottom, fAngle ) );
		_DrawPoint( dc, point, fAngle+A180 );
	}

	if( m_dwFlags & fcfRotate || m_dwFlags & fcfResize )
	{
		point = ::ConvertToWindow( m_punkView, CalcPointLocation( TopLeft, fAngle ) );
		_DrawPoint( dc, point, fAngle+A90 );
		point = ::ConvertToWindow( m_punkView, CalcPointLocation( BottomLeft, fAngle ) );
		_DrawPoint( dc, point, fAngle+A90 );
		point = ::ConvertToWindow( m_punkView, CalcPointLocation( TopRight, fAngle ) );
		_DrawPoint( dc, point, fAngle+A90 );
		point = ::ConvertToWindow( m_punkView, CalcPointLocation( BottomRight, fAngle ) );
		_DrawPoint( dc, point, fAngle+A90 );
	} 
}

//redraw the frame. Unknown is view
void CResizingFrame::Redraw( bool bOnlyPoints, bool bUpdate )
{
	
	if( !m_hwnd )
		return;
	if(m_rect == NORECT)
		return;

	CSize	size( 0, 0 );
	double	fDummy;
	CPoint	point;
	CRect	rect_all = NORECT;

	for( int i = Left; i <= BottomLeft; i++ )
	{
		point = CalcPointLocation( (PointLocation)i, fDummy );

		if( point == CPoint( -1, -1 ) )
			continue; 

		point = ::ConvertToWindow( m_punkView, point );
		CRect	rect( point, size );
		rect.InflateRect( 10, 10 );

		if( rect_all == NORECT )
			rect_all = rect;
		else
			rect_all.UnionRect( &rect, &rect_all );

		if( bOnlyPoints )
			::InvalidateRect( m_hwnd, rect, true );
	}

	if( !bOnlyPoints )
		::InvalidateRect( m_hwnd, rect_all, true );

	if( bUpdate )
		::UpdateWindow( m_hwnd );
}

CPoint CResizingFrame::CalcPointLocation( PointLocation location, double &fAngle )
{
//calc the real rect size
	CPoint	pointCenter = m_rect.CenterPoint();
	CRect	rect;
	rect.left = int( pointCenter.x - m_rect.Width()*m_fZoomX/2+.5);
	rect.right = int( pointCenter.x + m_rect.Width()*m_fZoomX/2+.5);
	rect.top = int( pointCenter.y - m_rect.Height()*m_fZoomY/2+.5);
	rect.bottom = int( pointCenter.y + m_rect.Height()*m_fZoomY/2+.5);
	//get the angle

	fAngle = 0;

//	if( rect.left >= rect.right || rect.top >= rect.bottom )
//		return CPoint( -1, -1 );

	fAngle = ::atan2( (double)rect.Height(), (double)rect.Width() );
	double	dx = rect.Width()/2;
	double	dy = rect.Height()/2;
	double	delta = ::sqrt( dx*dx+dy*dy );

	CPoint	pointWork;

	if( location == Left )
	{
		fAngle = m_fAngle+A180;
		delta = rect.Width()/2;
		pointWork = CPoint( m_rectCurrent.left, m_rectCurrent.CenterPoint().y );
	}
	else
	if( location == Bottom )
	{
		fAngle = m_fAngle+A90;
		delta = rect.Height()/2;
		pointWork = CPoint( m_rectCurrent.CenterPoint().x, m_rectCurrent.bottom );
	}
	else
	if( location == Right )
	{
		fAngle = m_fAngle;
		delta = rect.Width()/2;
		pointWork = CPoint( m_rectCurrent.right, m_rectCurrent.CenterPoint().y );
	}
	else
	if( location == Top )
	{
		fAngle = m_fAngle+A270;
		delta = rect.Height()/2;
		pointWork = CPoint( m_rectCurrent.CenterPoint().x, m_rectCurrent.top );
	}
	else
	if( location == TopRight )
	{
		fAngle = m_fAngle-fAngle;
		pointWork = CPoint( m_rectCurrent.right, m_rectCurrent.top );
	}
	else
	if( location == BottomRight )
	{
		fAngle = m_fAngle+fAngle;
		pointWork = m_rectCurrent.BottomRight();
	}
	else
	if( location == TopLeft )
	{
		fAngle = m_fAngle+fAngle+A180;
		pointWork = m_rectCurrent.TopLeft();
		
	}
	else
	if( location == BottomLeft )
	{
		fAngle = m_fAngle-fAngle+A180;
		pointWork = CPoint( m_rectCurrent.left, m_rectCurrent.bottom );
	}
	else
		ASSERT( FALSE );

	if( m_bRotate )
	{
		pointWork.x = int( pointCenter.x+delta*::cos( fAngle )+.5 );
		pointWork.y = int( pointCenter.y+delta*::sin( fAngle )+.5 );
	}
	
	if( ( m_dwFlags & fcfRotate ) == 0 )
	{
		if( location == TopRight ||location == TopLeft||
			location == BottomLeft ||location == BottomRight )
			fAngle += A90;
	}
	return pointWork;

}

void CResizingFrame::SetRect( CRect const rectSrc )
{
	m_rect = rectSrc;
	m_rectCurrent = rectSrc;
	m_fZoomX = 1;
	m_fZoomY = 1;
	m_fAngle = 0;
	m_bRotate = false;

}

void CResizingFrame::_DrawPoint( CDC &dc, CPoint point, double a )
{
	dc.SelectStockObject( WHITE_BRUSH );
	//dc.SelectStockObject( NULL_BRUSH );

	int	dx=0,	dxTrW = 7,	dyTrH = 5;

	CPoint	points[10];

	points[0].x = -dx;
	points[0].y = 0;
	points[1].x = -dx;
	points[1].y = -dyTrH;
	points[2].x = -dx-dxTrW;
	points[2].y = 0;
	points[3].x = -dx;
	points[3].y = dyTrH;
	points[4].x = -dx;
	points[4].y = 0;

	points[5].x = dx;
	points[5].y = 0;
	points[6].x = dx;
	points[6].y = -dyTrH;
	points[7].x = dx+dxTrW;
	points[7].y = 0;
	points[8].x = dx;
	points[8].y = dyTrH;
	points[9].x = dx;
	points[9].y = 0;

	while( a > A180 )a-=2*PI;
	while( a < -A180 )a+=2*PI;

	for( int i = 0; i < 10; i++ )
	{
		CPoint	pointNew;
		pointNew.x = int( point.x+points[i].x*::cos( a )+points[i].y*::sin( a )+.5);
		pointNew.y = int( point.y+points[i].x*::sin( a )-points[i].y*::cos( a )+.5);
		points[i] = pointNew;
	}

	dc.Polygon( points, 10 );
}

CResizingFrame::PointLocation 
	CResizingFrame::HitTest( CPoint point )
{
	if( m_dwFlags == fcfNone || m_rect == NORECT )
		return None;

	double	fDummy;
	CPoint	point1;

	double	fRadius = 10000000000;
	int		nMaxDistance = 15;

	PointLocation	location = None;

	for( int i = Left; i <= BottomLeft; i++ )
	{
		point1 = CalcPointLocation( (PointLocation)i, fDummy );

		double	f = ::sqrt( (double)(point.x-point1.x)*(point.x-point1.x)+
					(point.y-point1.y)*(point.y-point1.y) );

		if( f < 10 )
		{
			if( location == None )
			{
				location = (PointLocation)i;
				fRadius = f;
			}
			else
			{
				if( fRadius > f )
				{
					location = (PointLocation)i;
					fRadius = f;
				}
			}
		}
	}

	return location;
}

void CResizingFrame::RecalcChanges( CPoint point, CResizingFrame::PointLocation location )
{
	if( !m_bRotate )
	{
		if( location == Left )
			m_rectCurrent.left = point.x;
		else if( location == Right )
			m_rectCurrent.right = point.x;
		else if( location == Top )
			m_rectCurrent.top = point.y;
		else if( location == Bottom )
			m_rectCurrent.bottom = point.y;
		else
		if( m_dwFlags & fcfRotate )
			m_bRotate = true;
		else  if( location == TopLeft )
		{
			m_rectCurrent.left = point.x;
			m_rectCurrent.top = point.y;
		}
		else if( location == BottomLeft )
		{
			m_rectCurrent.left = point.x;
			m_rectCurrent.bottom = point.y;
		}
		else if( location == BottomRight )
		{
			m_rectCurrent.right = point.x;
			m_rectCurrent.bottom = point.y;
		}
		else if( location == TopRight )
		{
			m_rectCurrent.right = point.x;
			m_rectCurrent.top = point.y;
		}

		if( !m_bRotate )
		{
			if( m_rect.Width() != 0 )
			{
				double	dx = m_rect.Width();
				m_fZoomX = m_rectCurrent.Width()/dx;
			}
			if( m_rect.Height() != 0 )
			{
				double	dy = m_rect.Height();
				m_fZoomY = m_rectCurrent.Height()/dy;
			}
		}
	}

	if( !m_bRotate )
		return;

	CPoint	ptCenter = m_rect.CenterPoint();

	if( location == Left || location == Right )
	{
		double	fDistance = abs(int( (point.x-ptCenter.x)*cos( m_fAngle )+(point.y-ptCenter.y)*sin( m_fAngle ) ) );
		double	dx = m_rect.Width()/2/**cos( m_fAngle )*/;
		m_fZoomX = fDistance/dx;
	}
	else if( location == Top || location == Bottom )
	{
		double	fDistance = abs(int( (point.x-ptCenter.x)*sin( m_fAngle )-(point.y-ptCenter.y)*cos( m_fAngle ) ) );
		double	dy = m_rect.Height()/2/**cos( m_fAngle )*/;
		m_fZoomY = fDistance/dy;
	}
	else if( location == TopLeft || location == TopRight ||
		location == BottomLeft || location == BottomRight )
	{
		double	fAngle = ::atan2( m_rect.Height()*m_fZoomY, m_rect.Width()*m_fZoomX );
		
		double	fAngleNew = ::atan2( (double)m_rect.CenterPoint().y-point.y, 
									(double)m_rect.CenterPoint().x-point.x );

		
		if( location == TopLeft )
		{
			fAngle = fAngle;
		}
		else
		if( location == BottomLeft )
		{
			fAngle = -fAngle;
		}
		else
		if( location == BottomRight )
		{
			fAngle = fAngle+A180;
		}
		else
		if( location == TopRight )
		{
			fAngle = -fAngle+A180;
		}

		m_fAngle = fAngleNew - fAngle;
		while( m_fAngle < 0 )m_fAngle+=A180*2;
		while( m_fAngle > A180*2 )m_fAngle-=A180*2;
	}

}

CRect	CResizingFrame::CalcBoundsRect()
{
	double	dummy;
	CPoint	point = CalcPointLocation( Left, dummy );
	CRect	rect = CRect( point.x, point.y, point.x, point.y );

	for( int loc = Left; loc <= BottomLeft; loc++ )
	{
		point = CalcPointLocation( (PointLocation)loc, dummy );
		rect.left = min( rect.left, point.x );
		rect.right = max( rect.right, point.x );
		rect.top = min( rect.top, point.y );
		rect.bottom = max( rect.bottom, point.y );
	}
	return rect;
}

void CResizingFrame::ReLoadCursors()
{
	if( !m_dwFlags )
		return;
	
	if( m_dwFlags & fcfResize )
	{
		m_hcursors[Left] = AfxGetApp()->LoadStandardCursor( IDC_SIZEWE );
		m_hcursors[Top] = AfxGetApp()->LoadStandardCursor( IDC_SIZENS );
		m_hcursors[Right] = AfxGetApp()->LoadStandardCursor( IDC_SIZEWE );
		m_hcursors[Bottom] = AfxGetApp()->LoadStandardCursor( IDC_SIZENS );
	}
	if( m_dwFlags & fcfRotate )
	{
		m_hcursors[TopLeft] = AfxGetApp()->LoadStandardCursor( IDC_SIZENESW );
		m_hcursors[TopRight] = AfxGetApp()->LoadStandardCursor( IDC_SIZENWSE );
		m_hcursors[BottomLeft] = AfxGetApp()->LoadStandardCursor( IDC_SIZENWSE );
		m_hcursors[BottomRight] = AfxGetApp()->LoadStandardCursor( IDC_SIZENESW );
	}
	else
	{
		m_hcursors[TopLeft] = AfxGetApp()->LoadStandardCursor( IDC_SIZENWSE  );
		m_hcursors[TopRight] = AfxGetApp()->LoadStandardCursor( IDC_SIZENESW );
		m_hcursors[BottomLeft] = AfxGetApp()->LoadStandardCursor( IDC_SIZENESW );
		m_hcursors[BottomRight] = AfxGetApp()->LoadStandardCursor( IDC_SIZENWSE    );
	}
}

bool CResizingFrame::DoSetCursor( CPoint pt, PointLocation loc )
{
	if( loc ==  None )
		loc = HitTest( pt );

	if( loc ==  None )
		return false;

	if( !m_dwFlags )
		return false;

	if( !m_bRotate )
	{
		SetCursor( m_hcursors[loc] );
	}
	else
	{
		double fAngle = GetAngle();

		while( fAngle >= PI*2 )fAngle-=PI*2;
		while( fAngle < 0 )fAngle+=PI*2;

		{
			fAngle+=PI/8;

			int	n = int( fAngle/(PI/4) );
			int	nIndex = loc+n;
			
			if( nIndex >= 8 )
				nIndex -= 8;
			ASSERT( nIndex >=0 && nIndex < 8 );

			SetCursor( m_hcursors[nIndex] );
		}
	}
	return true;
}

#pragma optimize("", on)



/////////////////////////////////////////////////////////////////////////////////////////
//CFrame - multiselection objects frame 
BEGIN_INTERFACE_MAP(CFrame, CCmdTargetEx)
	INTERFACE_PART(CFrame, IID_IMultiSelection, Multi)
	INTERFACE_PART(CFrame, IID_ISerializableObject, Serialize)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CFrame, Multi);

HRESULT CFrame::XMulti::SelectObject(IUnknown* punkObj, BOOL bAddToExist)
{
	_try_nested(CFrame, Multi, SelectObject)
	{
		pThis->SelectObject(punkObj, bAddToExist==TRUE);
		return S_OK;
	}
	_catch_nested;
}


HRESULT CFrame::XMulti::UnselectObject(IUnknown* punkObj)
{
	_try_nested(CFrame, Multi, UnselectObject)
	{
		pThis->UnselectObject(punkObj);
		return S_OK;
	}
	_catch_nested;
}


HRESULT CFrame::XMulti::GetObjectsCount(DWORD* pnCount)
{
	_try_nested(CFrame, Multi, GetObjectsCount)
	{
		if(pnCount)
			*pnCount = pThis->GetObjectsCount();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFrame::XMulti::GetObjectByIdx(DWORD nIdx, IUnknown** ppunkObj)
{
	_try_nested(CFrame, Multi, GetObjectByIdx)
	{
		if(!ppunkObj)
			return E_INVALIDARG;
		*ppunkObj = pThis->GetObjectByIdx(nIdx);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFrame::XMulti::GetFlags(DWORD* pnFlags)
{
	_try_nested(CFrame, Multi, GetFlags)
	{
		if(pnFlags)*pnFlags = pThis->m_dwFlags;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFrame::XMulti::SetFlags(DWORD nFlags)
{
	_try_nested(CFrame, Multi, SetFlags)
	{
		pThis->m_dwFlags = nFlags;
		pThis->Redraw();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFrame::XMulti::GetFrameOffset(POINT* pptOffset)
{
	_try_nested(CFrame, Multi, GetFrameOffset)
	{
		pThis->GetFrameOffset((CPoint)*pptOffset);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFrame::XMulti::SetFrameOffset(POINT ptOffset)
{
	_try_nested(CFrame, Multi, SetFrameOffset)
	{
		pThis->SetFrameOffset(ptOffset);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFrame::XMulti::GetObjectOffsetInFrame(IUnknown* punkObject, POINT* pptOffset)
{
	_try_nested(CFrame, Multi, GetObjectOffsetInFrame)
	{
		pThis->GetObjectOffsetInFrame(punkObject, (CPoint)*pptOffset);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFrame::XMulti::RedrawFrame()
{
	_try_nested(CFrame, Multi, RedrawFrame)
	{
		pThis->Redraw();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFrame::XMulti::EmptyFrame()
{
	_try_nested(CFrame, Multi, EmptyFrame)
	{
		pThis->EmptyFrame();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFrame::XMulti::SyncObjectsRect()
{
	_try_nested(CFrame, Multi, SyncObjectsRect)
	{
		pThis->_CalculateFrameBoundsRect();
		return S_OK;
	}
	_catch_nested;
}


CFrame::CFrame()
{
	m_dwFlags = fcfNone;
	m_rcBounds = NORECT;
	m_nID = time(NULL)%10000;
	m_locationCurrent = None;
	m_bLockUpdate = false;
	m_bNotifyDeselect = false;
	m_pView = 0;
//	m_iTrackDeltaPos = 1;
}
		
CFrame::~CFrame()
{
	_ClearSelectedObjectsArray();
}

void CFrame::_ClearSelectedObjectsArray()
{
	for( int i = 0; i < GetObjectsCount(); i++ )
		m_ptrSelectedObjects[i]->Release();

	m_ptrSelectedObjects.RemoveAll();
	m_ptrSelectedObjectsKeys.RemoveAll();
	m_arrObjectsOffsets.RemoveAll();
	SetRect( NORECT );
}

//static bool bInSelectObject = false;
void CFrame::SelectObject(IUnknown* punkObject, bool bAddToExist)
{
	if (!punkObject)
	{
		EmptyFrame();		
		return;
	}
	
	if (!bAddToExist)
	{
		if( CheckObjectInSelection( punkObject ) && 
			GetObjectsCount() == 1 )
			return;

		EmptyFrame();
	}
	else
	{
		if (CheckObjectInSelection(punkObject))
		{
			Redraw();
			if (!m_pView->IsInNotify())
				::NotifyObjectSelect(punkObject, m_pView->GetControllingUnknown(), true);
//			bInSelectObject = false;
			return;
		}
	}

	_select( punkObject );
	
	_CalculateFrameBoundsRect();

// notify parent list
	if (!m_pView->IsInNotify())
		::NotifyObjectSelect(punkObject, m_pView->GetControllingUnknown(), true);
}

bool CFrame::UnselectObject(IUnknown* punkObject)
{
//	CTimeTest tt(true, "CFrame::UnselectObject");
	if (!CheckObjectInSelection(punkObject))
		return false;

	for (int i = 0; i < GetObjectsCount(); i++)
	{
		if (GetObjectKey(m_ptrSelectedObjects[i]) == GetObjectKey(punkObject))
		{
			_deselect( i );
			_CalculateFrameBoundsRect();
			if (m_ptrSelectedObjects.GetSize() > 0)
			{
				if (!m_pView->IsInNotify())
					::NotifyObjectSelect(m_ptrSelectedObjects[0], m_pView->GetControllingUnknown(), true);
			}

			return true;
		}
	}
	return false;
}

bool CFrame::CheckObjectInSelection(IUnknown* punkObject)
{
	GuidKey lTestKey = GetObjectKey(punkObject);
	for (int i = 0; i < GetObjectsCount(); i++)
	{
		if (lTestKey == m_ptrSelectedObjectsKeys[i] )
			return true;
	}
	return false;
}

bool CFrame::CheckObjectInSelection( GuidKey lTestKey )
{
	for (int i = 0; i < GetObjectsCount(); i++)
	{
		if (lTestKey == m_ptrSelectedObjectsKeys[i] )
			return true;
	}
	return false;
}

int  CFrame::GetObjectsCount()
{
	return m_ptrSelectedObjects.GetSize();
}

IUnknown*  CFrame::GetObjectByIdx(int nIdx)
{
	if(nIdx >= 0 && nIdx < GetObjectsCount())
	{
		IUnknown* punkObj = m_ptrSelectedObjects[nIdx];
		punkObj->AddRef();
		return punkObj;
	}
	return 0;
}

void CFrame::Init(CViewBase* pView, bool bNotifyDeselect)
{
	m_bNotifyDeselect = bNotifyDeselect;
	m_pView = pView;

	if( m_pView )
		m_dataSource.Init(m_pView, this);

	CResizingFrame::Init( pView->GetControllingUnknown(), pView->GetSafeHwnd() );
}

		
bool CFrame::DoEnterMode(CPoint point )
{
	//WARNING - point in screen coords
	CPoint	pointClient = ::ConvertToClient( m_pView->GetControllingUnknown(),  point );


	if( CResizingFrame::HitTest( pointClient ) != CResizingFrame::None )
		return CMouseImpl::DoEnterMode( point );
	m_bDragDropActive = true;
	Redraw();
	bool b = m_dataSource.DoEnterMode(point);
	m_bDragDropActive = false;
	return b;
}

IUnknown* CFrame::GetTarget()
{
	if( !m_pView )
		return 0;
	return m_pView->GetControllingUnknown();
}

bool CFrame::CanStartTracking( CPoint pt )
{
	if( CResizingFrame::HitTest( pt ) != CResizingFrame::None )
		return true;
	return m_pView->CanStartTracking( pt );
}

bool CFrame::DoFinishTracking( CPoint pt )
{
	if( !m_pView )
		return false;
	if(!m_bDragDropActive)
	{
		double	fAngle = GetAngle();
		double	fZoomX = GetZoomX();
		double	fZoomY = GetZoomY();

		IUnknown	*punk = GetActiveObject();
		if( !punk )
			return false;

		Redraw( false );

		m_pView->DoResizeRotateObject( punk, /*GetRect()fZoomX, fZoomY, fAngle*/this );

		_CalculateFrameBoundsRect();
		
		punk->Release();
	}

	
	m_locationCurrent = None;
	//return m_pView->DoFinishTracking( pt );
	return false;
}


bool CFrame::DoStartTracking( CPoint pt )
{
	if( !m_pView )
		return false;

	if( !m_dwFlags )
		return false;
	if(!m_bDragDropActive)
	{
		m_locationCurrent = HitTest( pt );
	
		if( m_locationCurrent == CFrame::None )
			return false;

		Redraw( false );
		return true;
	}

	//return m_pView->DoStartTracking( pt );
	return false;
}	

bool CFrame::DoTrack( CPoint pt )
{
	if( !m_pView )
		return false;
	if(!m_bDragDropActive)
	{
		Redraw( false );
		RecalcChanges( pt, m_locationCurrent );
		Redraw( false );
		m_pView->UpdateWindow();
	}
	//return m_pView->DoTrack( pt );
	return false;
}

void CFrame::_ManageObjectSelections(IUnknown* punk)
{
//	CTimeTest tt(true, "CFrame::_ManageObjectSelections");
	bool bGroup = ((short)GetKeyState(VK_CONTROL) < 0);
	if(bGroup)
	{
		if(!UnselectObject(punk))
		{
			ISmartSelectorPtr ptrSmartSelector = punk;
			if(ptrSmartSelector)
			{
				if(ptrSmartSelector->SmartSelect(true, Unknown()) == S_OK)
					return; // если обработали - то все.
			}
			SelectObject(punk, bGroup);
		}
		else if(GetObjectsCount() < 1)
		{
			ISmartSelectorPtr ptrSmartSelector = punk;
			if(ptrSmartSelector)
			{
				if(ptrSmartSelector->SmartSelect(true, Unknown()) == S_OK)
					return; // если обработали - то все.
			}
			SelectObject(punk, bGroup);
		}
		
	}
	else
	{	// AAM: обработка 'умных' объектов - которые имеют свое мнение о том, кого можно селектить
		ISmartSelectorPtr ptrSmartSelector = punk;
		if(ptrSmartSelector)
		{
			if(ptrSmartSelector->SmartSelect(true, Unknown()) == S_OK)
				return; // если обработали - то все.
		}
		SelectObject(punk, bGroup);
	}
}

bool CFrame::DoRButtonDown( CPoint pt )
{
	bool	bClickOnSelectedObject = false;
	for( int i = 0; i < GetObjectsCount(); i++ )
	{
		if( m_pView->PtInObject( m_ptrSelectedObjects[i], pt ) )
			bClickOnSelectedObject = true;
	}
	if( bClickOnSelectedObject )
		return false;

	IUnknown* punk = 0;
	punk = m_pView->GetObjectByPoint(pt);
	
	if(punk != 0)
	{
		_ManageObjectSelections(punk);
		punk->Release();
	}
	else
	{
		Redraw();
		if (m_ptrSelectedObjects.GetSize() && !m_pView->IsInNotify())
			::NotifyObjectSelect(m_ptrSelectedObjects[0], false);

		_ClearSelectedObjectsArray();
	}
	return false;
}

static bool bAddByButtonUp = false;
bool CFrame::DoLButtonDown( CPoint pt )
{
	if( CResizingFrame::HitTest( pt ) != CResizingFrame::None )
		return false;

	bool	bClickOnSelectedObject = false;
	for( int i = 0; i < GetObjectsCount(); i++ )
	{
		if( m_pView->PtInObject( m_ptrSelectedObjects[i], pt ) )
			bClickOnSelectedObject = true;
	}

	bAddByButtonUp = true;
	if( bClickOnSelectedObject )
		return false;

	IUnknown* punk = 0;
	punk = m_pView->GetObjectByPoint(pt);
	
	
	if(punk != 0)
	{
		if(CheckObjectInSelection(punk))
		{
			punk->Release();
		}
		else
		{
			bAddByButtonUp = false;
			_ManageObjectSelections(punk);
			punk->Release();
		}
	}
	else
	{
		bAddByButtonUp = false;

		Redraw();
		if (m_ptrSelectedObjects.GetSize() && !m_pView->IsInNotify())
			::NotifyObjectSelect(m_ptrSelectedObjects[0], false);

		_ClearSelectedObjectsArray();

		//m_pView->UpdateWindow();
	}
	
	return false;
}

bool CFrame::DoLButtonUp( CPoint pt )
{
	if(bAddByButtonUp)	
	{
		IUnknown* punk = 0;
		punk = m_pView->GetObjectByPoint(pt);
		if(punk != 0)
		{
			_ManageObjectSelections(punk);
			punk->Release();
		}
	}
	bAddByButtonUp = false;

	return false;
}

void CFrame::Draw(CDC& dc, CRect rectPaint, BITMAPINFOHEADER *pbih, byte *pdibBits )
{
	if( !m_pView )
		return;
	IUnknown *punkObject = 0;
	int nNumObjs = GetObjectsCount();
	for(int i = 0; i < nNumObjs; i++)
	{
		CRect rc = NORECT;
		punkObject = m_ptrSelectedObjects[i];
		rc = m_pView->GetObjectRect(punkObject);
		m_pView->DoDrawFrameObjects(punkObject, dc, rectPaint, pbih, pdibBits, (i==0)?odsActive:odsSelected );
		//punkObject->Release();
	}
	CResizingFrame::Draw( dc, rectPaint, pbih, pdibBits );
}

void CFrame::_CalculateFrameBoundsRect()
{
	int nNumObjs = GetObjectsCount();
	m_rcBounds = NORECT;
	for(int i = 0; i < nNumObjs; i++)
	{
		CRect rc = NORECT;
		rc = m_pView->GetObjectRect(m_ptrSelectedObjects[i]);
		m_rcBounds.UnionRect(&m_rcBounds, &rc);	
	}

	m_arrObjectsOffsets.RemoveAll();
	m_arrObjectsOffsets.SetSize(nNumObjs);
	SetRect(NORECT);
	for(i = 0; i < nNumObjs; i++)
	{
		CRect rc = NORECT;
		rc = m_pView->GetObjectRect(m_ptrSelectedObjects[i]);

		if( i == 0 && nNumObjs == 1)
		{
			//RedrawFrame(m_pView->GetControllingUnknown());
			SetRect( rc );
			//RedrawFrame(m_pView->GetControllingUnknown());
		}


		CPoint pt(0, 0);
		pt = rc.TopLeft() - m_rcBounds.TopLeft();
		ASSERT(pt.x >=0 && pt.y >=0);
		m_arrObjectsOffsets.SetAt(i, MAKELONG(pt.x, pt.y));
	}
}

void CFrame::GetFrameOffset(CPoint& ptOffset)
{
	ptOffset = m_rcBounds.TopLeft();
}

void CFrame::SetFrameOffset(CPoint ptOffset)
{
	CSize szOffset = ptOffset - m_rcBounds.TopLeft();
	CRect rcOld = m_rcBounds;
	m_rcBounds.OffsetRect(szOffset);
	int nObjects = GetObjectsCount();
	//RedrawFrame(m_pView->GetControllingUnknown());
	SetRect(NORECT);
	for(int i = 0; i < nObjects; i++)
	{	
		DWORD dwOffset = m_arrObjectsOffsets.GetAt(i);
		CPoint ptBase = rcOld.TopLeft();
		CPoint ptObjOffset(LOWORD(dwOffset), HIWORD(dwOffset));
		ptBase.Offset(ptObjOffset);
		ptBase.Offset(szOffset);
		m_pView->SetObjectOffset( m_ptrSelectedObjects[i], ptBase );

		if( i == 0 && nObjects == 1)
		{
			SetRect( m_pView->GetObjectRect( m_ptrSelectedObjects[i] ) );
		}
	}
	//RedrawFrame(m_pView->GetControllingUnknown());

}

void CFrame::GetObjectOffsetInFrame(IUnknown* punkObject, CPoint& ptOffset)
{
	for(int i = 0; i < GetObjectsCount(); i++)
	{
		if( GetObjectKey(m_ptrSelectedObjects[i]) == GetObjectKey(punkObject))
		{
			DWORD dwOffset = m_arrObjectsOffsets.GetAt(i);
			ptOffset = CPoint(HIWORD(dwOffset), LOWORD(dwOffset));
			return;		
		}
	}
}

bool CFrame::SerializeObject( CStreamEx& ar, SerializeParams *pparams )
{
	if(ar.IsStoring())
	{
		long nObjectCount = GetObjectsCount();

		ar<<nObjectCount;
		ar<<m_rcBounds.left;
		ar<<m_rcBounds.top;
		ar<<m_rcBounds.right;
		ar<<m_rcBounds.bottom;
		IUnknown* punkDataObject = 0;
		CString	strObjectType = "";

		for(int i = 0; i < nObjectCount; i++)		
		{
			ar<<m_arrObjectsOffsets[i];
			punkDataObject = GetObjectByIdx(i);
			strObjectType = ::GetObjectKind( punkDataObject );
			ar<<strObjectType;
			punkDataObject->Release();
			punkDataObject = 0;
		}

		ar.Flush();

		//CArchiveStream	stream( &ar );

		for(i = 0; i < nObjectCount; i++)
		{
			punkDataObject = GetObjectByIdx(i);
			sptrINamedDataObject2	sptrN( punkDataObject );
			DWORD	dwFlags = 0;
			sptrN->GetObjectFlags( &dwFlags );
			bool bVirtual  = ( dwFlags & nofHasData ) == 0;
			//if( bVirtual )
			//	sptrN->StoreData( sdfCopyParentData );

			IUnknown	*punkParentObject = 0;
			sptrN->GetParent( &punkParentObject );

			BOOL	bParentLock;

			if( CheckInterface( punkParentObject, IID_IDataObjectList ) )
			{
				IDataObjectListPtr	ptrP( punkParentObject );
				ptrP->GetObjectUpdateLock( &bParentLock );
				ptrP->LockObjectUpdate( true );
			}

			if( bVirtual )
				sptrN->SetParent( 0, sdfCopyParentData );
			else
				sptrN->SetParent( 0, apfNone );

			sptrISerializableObject	sptrS( punkDataObject );
			sptrS->Store( (IStream*)ar, pparams );

			if( bVirtual )
				sptrN->SetParent( punkParentObject, apfAttachParentData );
			else
				sptrN->SetParent( punkParentObject, apfNone );
			if( punkParentObject )
				punkParentObject->Release();

			if( CheckInterface( punkParentObject, IID_IDataObjectList ) )
			{
				IDataObjectListPtr	ptrP( punkParentObject );
				ptrP->LockObjectUpdate( bParentLock );
			}

			//if( bVirtual )
			//	sptrN->StoreData( apfAttachParentData );

			punkDataObject->Release();
			punkDataObject = 0;
		}
		ar.Flush();
	}
	else
	{
		EmptyFrame();

		int nObjectCount = 0;
		ar>>nObjectCount;
		ar>>m_rcBounds.left;
		ar>>m_rcBounds.top;
		ar>>m_rcBounds.right;
		ar>>m_rcBounds.bottom;
		m_arrObjectsOffsets.SetSize(nObjectCount);
		CStringArray strarrTypes;
		strarrTypes.SetSize(nObjectCount);
		for(int i = 0; i < nObjectCount; i++)		
		{
			ar>>m_arrObjectsOffsets[i];
			ar>>strarrTypes[i];
		}

		ar.Flush();
		//CArchiveStream	stream( &ar );
		for(i = 0; i < nObjectCount; i++)		
		{
			IUnknown	*punkO = ::CreateTypedObject( strarrTypes[i] );
			if( !punkO )return false;
			CString	strOldName = ::GetObjectName( punkO );
			{
				sptrISerializableObject	sptrS( punkO );
				sptrS->Load( (IStream*)ar, pparams );
				//::SetObjectName( punkO, 0 );
				INumeredObjectPtr	sptrN( punkO );
				sptrN->GenerateNewKey( 0 );
			}
			//punkO->AddRef();
			m_ptrSelectedObjects.Add( punkO );
			GuidKey lkey = ::GetObjectKey( punkO );
			m_ptrSelectedObjectsKeys.Add( lkey );
		}
	}
	return true;
}

bool CFrame::LockUpdate( bool bLock )
{
	bool	bOldLock = m_bLockUpdate;
	m_bLockUpdate = bLock;
	return bOldLock;
}

void CFrame::Redraw( bool bOnlyPoints, bool bUpdate )
{
	if( m_bLockUpdate )
		return;
	if(!m_pView)
		return;

	if( !bOnlyPoints )
	{
		for(int i = 0; i < GetObjectsCount(); i++)
		{
			IUnknown* punkObj = m_ptrSelectedObjects[i];
			CRect rc = m_pView->GetObjectRect(punkObj);
			rc = ::ConvertToWindow( m_pView->GetControllingUnknown(), rc );
			rc.InflateRect( 1, 1 );
			if( m_pView->GetSafeHwnd() )
				m_pView->InvalidateRect(rc);
		}
	}
	CResizingFrame::Redraw( bOnlyPoints, bUpdate );
}

void CFrame::EmptyFrame()
{
	//RedrawFrame(m_pView->GetControllingUnknown());
	Redraw();
	SetRect(NORECT);
	
	if( GetObjectsCount() )
		for( int i = GetObjectsCount()-1; i >= 0; i-- )
			_deselect( i );

	m_arrObjectsOffsets.RemoveAll();
	m_rcBounds = NORECT;
	Redraw();
//	m_pView->UpdateWindow();
}

IUnknown *CFrame::GetActiveObject()
{
	if( !GetObjectsCount() )
		return 0;
	return GetObjectByIdx( 0 );
}

bool CFrame::DoSetCursor( CPoint pt )
{
	return CResizingFrame::DoSetCursor( pt, m_locationCurrent );
}

void CFrame::OnBeginDragDrop()
{
	m_bDragDropActive = true;
	m_pView->OnBeginDragDrop();
}

void CFrame::OnEndDragDrop()
{
	m_bDragDropActive = false;
	m_pView->OnEndDragDrop();
}


//remove all objects with specified parent
void CFrame::UnselectObjectsWithSpecifiedParent( IUnknown *punkParent )
{
	if( !punkParent )return;

	Redraw();


	int	c = 0;
	GuidKey nParentKey = ::GetObjectKey( punkParent );

	for (int i = GetObjectsCount()-1; i >=0 ; i-- )
	{
		GuidKey nObjectParentKey = ::GetParentKey( m_ptrSelectedObjects[i] );
		if( nParentKey == nObjectParentKey )
		{
			_deselect( i );
			continue;
		}
		GuidKey nObjectKey = ::GetObjectKey( m_ptrSelectedObjects[i] );
		if( nParentKey == nObjectKey )
		{
			_deselect( i );
		}
	}

	_CalculateFrameBoundsRect();
	Redraw();

	if (m_ptrSelectedObjects.GetSize() > 0)
	{
		if (!m_pView->IsInNotify())
			::NotifyObjectSelect(m_ptrSelectedObjects[0], m_pView->GetControllingUnknown(), true);
	}
}

void CFrame::_select( IUnknown *punk )
{
	punk->AddRef();

	IUnknown	*punkOld = m_ptrSelectedObjects.GetSize()?m_ptrSelectedObjects[0]:0;
	m_ptrSelectedObjects.InsertAt(0, punk);
	GuidKey key = ::GetObjectKey( punk );
	m_ptrSelectedObjectsKeys.InsertAt(0, key );


	bool	bOld = m_pView->LockNotification( true );
	INotifyObjectPtr	ptrNotify( punk );
	if( ptrNotify != 0 )ptrNotify->NotifySelect( true );
	m_pView->LockNotification( bOld );

	CRect	rect;
	{
		IScrollZoomSitePtr	ptr( m_pView->GetControllingUnknown() );
		CRect	rect = m_pView->GetObjectRect( punk );
		if( rect != NORECT )
		{
			if( ptr != 0 )ptr->EnsureVisible( rect );

			if( m_pView && m_pView->GetSafeHwnd() )
				m_pView->InvalidateRect( ConvertToWindow( m_pView->GetControllingUnknown(), rect ) );
		}
	}
	if( punkOld && m_pView->GetSafeHwnd() )
		m_pView->InvalidateRect( ConvertToWindow( m_pView->GetControllingUnknown(), m_pView->GetObjectRect( punkOld ) ) );
}
void CFrame::_deselect( int idx )
{
	IUnknown	*punk = m_ptrSelectedObjects[idx];
	IUnknown	*punkOld = (idx == 0 && m_ptrSelectedObjects.GetSize()>1)?
							m_ptrSelectedObjects[1]:0;

	if(m_bNotifyDeselect)m_pView->OnDeselect( punk );
	INotifyObjectPtr	ptrNotify( punk );
	bool	bOld = m_pView->LockNotification( true );
	if( ptrNotify != 0 )ptrNotify->NotifySelect( false );
	m_pView->LockNotification( bOld );

	m_ptrSelectedObjects.RemoveAt(idx);
	m_ptrSelectedObjectsKeys.RemoveAt(idx);
	m_arrObjectsOffsets.RemoveAt(idx);

	if( m_pView && m_pView->GetSafeHwnd() )
		m_pView->InvalidateRect( ConvertToWindow( m_pView->GetControllingUnknown(), m_pView->GetObjectRect( punk ) ) );

	if( punkOld && m_pView && m_pView->GetSafeHwnd() )
		m_pView->InvalidateRect( ConvertToWindow( m_pView->GetControllingUnknown(), m_pView->GetObjectRect( punkOld ) ) );

	punk->Release();
}