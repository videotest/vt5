// ConvertorChooser.cpp : implementation file
//

#include "stdafx.h"
#include "ConvertorChooser.h"
#include "misc_utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



void CImagePane::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if ( !GetValueInt( GetAppUnknown(), "ConvertImage", "InvokeByDblClick", 1 ) )
		return;
	IUnknown* punkCC = _GetOtherComponent( GetAppUnknown(), IID_IColorConvertorManager );
	ICompManagerPtr sptrCM(punkCC);
	if( !punkCC )return;
	punkCC->Release();
	if (sptrCM == 0)return;


	int nCount;
	sptrCM->GetCount(&nCount);
	BSTR bstr;
	IColorConvertor4Ptr sptrCC;

	CMenu	menu;
	menu.CreatePopupMenu();

	IUnknown	*punk;

	CDWordArray	arr;

	for(int i = 0; i < nCount; i++)
	{
		sptrCM->GetComponentUnknownByIdx(i, &punk);
		sptrCC = punk;
		punk->Release();
		sptrCC->GetCnvName(&bstr);
		CString strName(bstr);
		::SysFreeString(bstr);

		DWORD	dwFlags = MF_STRING;
		if( !strcmp( strName, m_strCC ) )
			dwFlags |= MF_CHECKED;

		DWORD	dw;
		sptrCC->GetConvertorFlags( &dw );

		if( dw & ccfLabCompatible ) 
		{
			HICON	hIcon;
			sptrCC->GetConvertorIcon( &hIcon );

			
			HBITMAP	hBitmap = 0;

			{
				CBitmap		bitmap;
				CClientDC	dc( 0 );
				CDC			dcMem;
				bitmap.CreateCompatibleBitmap( &dc, 16, 16 );
				dcMem.CreateCompatibleDC( &dc );
				dcMem.SelectObject( &bitmap );

				dcMem.FillRect( &CRect( 0, 0, 16, 16 ), &CBrush( ::GetSysColor( COLOR_3DFACE ) ) );
				::DrawIconEx( dcMem, 0, 0, hIcon, 16, 16, 0, 0, DI_NORMAL );

				hBitmap = (HBITMAP)bitmap.Detach();
			}

			
			::DestroyIcon( hIcon );
			menu.AppendMenu( dwFlags, i+1000, strName );
			::SetMenuItemBitmaps( menu, i, MF_BYPOSITION, hBitmap, hBitmap );
			arr.Add( (DWORD)hBitmap );
		}
	}

	CRect	rect;
	GetClientRect( rect );

	ClientToScreen( &rect );

	UINT	nCmd = menu.TrackPopupMenu( TPM_BOTTOMALIGN|TPM_RETURNCMD, rect.left, rect.top, this );

	for( i = 0; i < arr.GetSize(); i++ )
		::DeleteObject( (HBITMAP)arr[i] );

	if( nCmd >= 1000 )
	{
		sptrCM->GetComponentUnknownByIdx(nCmd-1000, &punk);
		sptrCC = punk;
		punk->Release();
		sptrCC->GetCnvName(&bstr);
		CString strName(bstr);
		::SysFreeString(bstr);

		strName = "\""+strName+"\"";

		::ExecuteAction( "ConvertImage", strName );
	}
}

void CImagePane::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// vk
	if ( GetValueInt( GetAppUnknown(), "ConvertImage", "InvokeByDblClick", 1 ) )
		::ExecuteAction( "ConvertImage", 0, 0 );

	//::ExecuteAction( "ConvertImage", 0, 0 );
	// end vk
}


////////////////////////////////////////////////////////////////
//
//	CImagePane class 
//
////////////////////////////////////////////////////////////////
#define IMAGE_DIM	14
CImagePane::CImagePane()
{
	m_bProc = GetValueInt(GetAppUnknown(), "ImagePane", "Percent", FALSE);
	SetImage( 0 );

}

BEGIN_MESSAGE_MAP(CImagePane, CWnd)
	//{{AFX_MSG_MAP(CImagePane)
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CImagePane::SetImage( IUnknown *punkImage, IUnknown *punkView )
{
	m_strCC = "---";
	m_ptrCC = 0;
	m_ptrHCC = 0;
	m_strColor = "";
	m_rectImage = NORECT;

	m_ptrImage = punkImage;
	m_punkView = punkView;
	if( m_ptrImage != 0 )
	{
		CPoint	pointOffset;
		CSize	sizeImage;
		m_ptrImage->GetOffset( &pointOffset );
		m_ptrImage->GetSize( (int*)&sizeImage.cx, (int*)&sizeImage.cy );
		m_rectImage = CRect( pointOffset, sizeImage );

		IUnknown	*punkCC =  0;
		m_ptrImage->GetColorConvertor( &punkCC );
		m_ptrCC = punkCC;
		m_ptrHCC = punkCC;
		if( punkCC )punkCC->Release();

		if( m_ptrCC == 0 )
			m_ptrImage = 0;
		else
		{
			BSTR	bstrCC;
			m_ptrCC->GetCnvName( &bstrCC );
			m_strCC = bstrCC;
			::SysFreeString( bstrCC );
		}
	}
	if( GetSafeHwnd() )
		Invalidate();
}
void CImagePane::SetPosition( const POINT &point )
{
	m_strColor = "";

	if( !m_rectImage.PtInRect( point ) )
		return;

	if( m_ptrImage == 0 || m_ptrCC == 0 || m_ptrHCC == 0 )
		return;

	int	nPanes = ::GetImagePaneCount( m_ptrImage ), c;

	CPoint	pt = point;
	pt.x -= m_rectImage.left;
	pt.y -= m_rectImage.top;
	
	BSTR bstr;
	if (m_ptrHCC->GetColorString(m_ptrImage,m_punkView,pt,&bstr)==S_OK)
	{
		m_strColor = bstr;
		::SysFreeString(bstr);
	}
	else
	{
		color	*pcolor;
		CString	str;
		CString	strColor;
		color	colorH;
		for( c = 0; c < nPanes; c++ )
		{
			m_ptrImage->GetRow( pt.y, c, &pcolor );
			pcolor += pt.x;

			m_ptrHCC->GetPaneShortName( c, &bstr );
			m_ptrHCC->Convert( *pcolor, c, &colorH );
			strColor = bstr;
			::SysFreeString( bstr );

			if (m_bProc)
				str.Format( "%s:%d", (const char *)strColor, colorH*100/256 );
			else
				str.Format( "%s:%d", (const char *)strColor, colorH );
			if( m_strColor.IsEmpty() )
				m_strColor+="(";
			else
				m_strColor+=", ";
			m_strColor += str;
		}
		m_strColor+=")";
	}
	if( GetSafeHwnd() )Invalidate();
}

////////////////////////////////////////////////////////////////
void CImagePane::OnPaint() 
{
	CPaintDC	dc( this );
	CRect		rectClient;

	GetClientRect( &rectClient );

	HBRUSH	hbrush = (HBRUSH)GetParent()->SendMessage( WM_CTLCOLORSTATIC, (WPARAM)(HDC)dc, (LPARAM)GetSafeHwnd() );
	if( !hbrush )hbrush = ::GetSysColorBrush( COLOR_3DFACE );
	::FillRect( dc, rectClient, hbrush );

	rectClient.InflateRect( -1, -1 );

	if( m_ptrCC != 0 )
	{
		HICON	hIcon = 0;
		m_ptrCC->GetConvertorIcon( &hIcon );

		::DrawIconEx( dc, rectClient.left, rectClient.top, hIcon, 16, 16, 0, 0, DI_NORMAL );
		::DestroyIcon( hIcon );
	}

	CRect rectText = rectClient;
	rectText.left += 20;
	CString	strText;
	strText = m_strCC+" "+m_strColor;
	dc.SetBkMode( TRANSPARENT );

	dc.SelectObject( GetParent()->GetFont() );
	dc.DrawText( strText, rectText, DT_LEFT|DT_VCENTER|DT_SINGLELINE );
}
