//********************************************************************************
//* TransparentWindow.CPP
//*
//* A transparent window class.
//*
//* Based on the idea of Jason Wylie
//*
//* (C) 1998 by Franz Polzer
//*
//* Visit me at:	stud1.tuwien.ac.at/~e9225140
//* Write to me:	e9225140@student.tuwien.ac.at
//********************************************************************************

#include "stdafx.h"
#include "TransparentWnd.h"
#include "resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//********************************************************************************
//* Constructor
//********************************************************************************

CTransparentWnd::CTransparentWnd()
{
}


//********************************************************************************
//* Destructor
//********************************************************************************

CTransparentWnd::~CTransparentWnd()
{
}


BEGIN_MESSAGE_MAP(CTransparentWnd, CWnd)
	//{{AFX_MSG_MAP(CTransparentWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//********************************************************************************
//* CreateTransparent()
//*
//* Creates the main application window transparent
//********************************************************************************

void CTransparentWnd::CreateTransparent(LPCTSTR pTitle, RECT &rect, unsigned short BitmapID)
{
	CreateEx( WS_EX_TOPMOST|WS_EX_TOOLWINDOW,
						AfxRegisterWndClass(CS_DBLCLKS),
						pTitle,
						WS_POPUP | WS_SYSMENU,
						rect,
						NULL,
						NULL,
						0 );
	SetupRegion(GetWindowDC(), BitmapID);
}


//********************************************************************************
//* SetupRegion()
//*
//* Set the Window Region for transparancy outside the mask region
//********************************************************************************

void CTransparentWnd::SetupRegion(CDC *pDC, unsigned short BitmapID)
{
	CDC					memDC;
	CBitmap*		pOldMemBmp = NULL;
	COLORREF		col;
	CRect				cRect;
	int					x, y;
	CRgn				wndRgn, rgnTemp;

	GetWindowRect(&cRect);

	m_bitmap.DeleteObject();
	m_bitmap.LoadBitmap( BitmapID );

	CSize	size( 100, 100 );
	BITMAP	bm;
	m_bitmap.GetBitmap( &bm );
	cRect.right = cRect.left+bm.bmWidth;
	cRect.bottom = cRect.top+bm.bmHeight;
	memDC.CreateCompatibleDC(pDC);
	pOldMemBmp = memDC.SelectObject( &m_bitmap );

	//BOOL bPrevVisible = ShowWindow( SW_HIDE );
	SetWindowRgn( 0, TRUE );

	wndRgn.DeleteObject();
	wndRgn.CreateRectRgn(0, 0, cRect.Width(), cRect.Height());
	for(x=0; x<=cRect.Width(); x++)
	{
		for(y=0; y<=cRect.Height(); y++)
		{
			col = memDC.GetPixel(x, y);
			if(col == RGB( 255, 255, 255))
			{
				rgnTemp.CreateRectRgn(x, y, x+1, y+1);
				wndRgn.CombineRgn(&wndRgn, &rgnTemp, RGN_XOR);
				rgnTemp.DeleteObject();	
			}
		}
	}
	if (pOldMemBmp) memDC.SelectObject(pOldMemBmp);
	SetWindowRgn((HRGN)wndRgn, TRUE);
	MoveWindow( cRect );

	//if( bPrevVisible )
	//	ShowWindow( SW_SHOW );
	Invalidate();
	UpdateWindow();
}


//********************************************************************************
//* CTransparentWnd message handlers
//********************************************************************************


BOOL CTransparentWnd::OnEraseBkgnd(CDC* pDC) 
{
	CRect	rect;
	GetWindowRect(&rect);

	CDC memDC;
	CBitmap*		pOldMemBmp = NULL;

	memDC.CreateCompatibleDC(pDC);
	pOldMemBmp = memDC.SelectObject(&m_bitmap);
	//pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
	BITMAP	bm;
	m_bitmap.GetBitmap( &bm );
	pDC->StretchBlt( 0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY );

	if (pOldMemBmp) memDC.SelectObject( pOldMemBmp );
	
	return TRUE;
}

void CTransparentWnd::OnPaint()
{
	CPaintDC dc(this);

	// Add your drawing code here!
}

