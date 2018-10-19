#include "stdafx.h"
#include "printview.h"
#include "resource.h"
#include "Print.h"
#include "PrinterSetup.h"
#include "TemplateSetup.h"
#include <afxpriv2.h>
#include "ax_ctrl_misc.h"
#include "\vt5\awin\profiler.h"

#include "..\\..\\..\\common2\\misc_calibr.h"

#include "dbase.h"
#include "scriptnotifyint.h"


STDAPI_(HDC) OleStdCreateDC(DVTARGETDEVICE FAR* ptd)
{
	HDC hdc=NULL;
	LPDEVNAMES lpDevNames;
	LPDEVMODE lpDevMode;
	LPTSTR lpszDriverName;
	LPTSTR lpszDeviceName;
	LPTSTR lpszPortName;

	if (ptd == NULL) {
		hdc = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
		goto errReturn;
	}

	lpDevNames = (LPDEVNAMES) ptd; // offset for size field

	if (ptd->tdExtDevmodeOffset == 0) {
		lpDevMode = NULL;
	}else{
		lpDevMode  = (LPDEVMODE) ((LPTSTR)ptd + ptd->tdExtDevmodeOffset);
	}

	lpszDriverName = (LPTSTR) lpDevNames + ptd->tdDriverNameOffset;
	lpszDeviceName = (LPTSTR) lpDevNames + ptd->tdDeviceNameOffset;
	lpszPortName   = (LPTSTR) lpDevNames + ptd->tdPortNameOffset;

	hdc = CreateDC(lpszDriverName, lpszDeviceName, lpszPortName, lpDevMode);

errReturn:
	return hdc;
}



/////////////////////////////////////////////////////////////////////////////
// CControlInfo
CControlInfo::CControlInfo()
{
	m_pCtrlData		= NULL;	
	
	m_rectOnPage	= NORECT;
	m_bFirstEntry	= FALSE;
	
	m_ctrlSize		= CSize(0,0);
	m_ptOffset		= CPoint(0,0);


	m_sizeViewCtrl = 
	m_sizeUserOffset = 
	m_sizeUserOffsetDX = CSize(0,0);

}

CControlInfo::~CControlInfo()
{

}


/////////////////////////////////////////////////////////////////////////////
// CPage
CPage::CPage()
{
	m_nPageCol = -1;
	m_nPageRow = -1;

	m_pageSize = CSize( 0, 0 );
	m_pageAlign = m_pageField = NORECT;
	
	m_ptClient = CPoint(0,0);
	
	m_nMainPage = -1;
}



CPage::~CPage()
{
	for( int i=0;i<m_ctrl.GetSize();i++ )
	{
		delete m_ctrl[i];
	}
	
	m_ctrl.RemoveAll();

}

void CPage::SetMainPage( int nMainPage )
{
	ASSERT( nMainPage>=0 );
	m_nMainPage = nMainPage;
}

int CPage::GetMainPage( )
{
	ASSERT( m_nMainPage>=0 );
	return m_nMainPage;
}

BOOL CPage::IsMainPage()
{
	return ( (m_nPageCol == m_nPageRow && m_nPageRow == 0 )?TRUE:FALSE );
}

void CPage::AddCtrl( IUnknown* pCtrlData,  
					CRect rcOnPageRect, CSize ctrlSize, CPoint ptOffset,
					BOOL bFirstEntry, CSize sizeViewCtrl,
					CSize sizeUserOffset,
					CSize sizeUserOffsetDX
					)
{
	
	CControlInfo* pCI = new CControlInfo;

	if( !CheckInterface( pCtrlData,  IID_IReportCtrl ) )
		ASSERT( FALSE );

	pCI->m_pCtrlData	= pCtrlData;		
	pCI->m_bFirstEntry	= bFirstEntry;
	
	pCI->m_rectOnPage	= rcOnPageRect;
	
	
	pCI->m_ctrlSize		= ctrlSize;
	ASSERT( pCI->m_ctrlSize.cx >= 0 && pCI->m_ctrlSize.cy >= 0 );
	pCI->m_ptOffset		= ptOffset;

	

	pCI->m_sizeViewCtrl	= sizeViewCtrl;
	ASSERT( pCI->m_sizeViewCtrl.cx >= 0 && pCI->m_sizeViewCtrl.cy >= 0 );

	pCI->m_sizeUserOffset	= sizeUserOffset;

	pCI->m_sizeUserOffsetDX = sizeUserOffsetDX;

	
	m_ctrl.Add( pCI );
	
}

void CPage::RemoveCtrl( IUnknown* pCtrlData )
{
	for(int i=0;i<m_ctrl.GetSize();i++)
	{
		if( m_ctrl[i]->m_pCtrlData == pCtrlData )
		{
			delete m_ctrl[i];
			m_ctrl.RemoveAt(i);
		}
	}
}


CPoint CPage::GetClientStartPoint(){
	return m_ptClient;
}

void CPage::SetClientStartPoint( CPoint pt ){
	m_ptClient = pt;
}


void CPage::SetPaperSize ( CSize size ){
	m_pageSize = size;
}

void CPage::SetPaperAlign( CRect rect ){
	m_pageAlign = rect;
}

void CPage::SetPaperField( CRect rect ){
	m_pageField = rect;
}


CRect CPage::GetRectPage ( )
{
	CRect pageRect = CRect( 0, 0, m_pageSize.cx, m_pageSize.cy );

	pageRect += m_ptClient;

	return pageRect;
}

CRect CPage::GetRectAlign( )
{
	CRect pageRect = GetRectPage( );

	CRect alignRect;
	alignRect.left	= pageRect.left + m_pageAlign.left;
	alignRect.top	= pageRect.top  + m_pageAlign.top;
	alignRect.right		= pageRect.right - m_pageAlign.right;
	alignRect.bottom	= pageRect.bottom - m_pageAlign.bottom;

	return alignRect;	
}

CRect CPage::GetRectField( )
{
	
	CRect pageRect = GetRectPage( );
	CRect fieldRect;

	fieldRect.left	= pageRect.left + m_pageField.left;
	fieldRect.top	= pageRect.top  + m_pageField.top;
	fieldRect.right		= pageRect.right - m_pageField.right;
	fieldRect.bottom	= pageRect.bottom - m_pageField.bottom;

	return fieldRect;	
}



void CPage::GetPageColRow(int& nCol, int& nRow)
{
	ASSERT( m_nPageCol >=0 && m_nPageRow >= 0 );
	nCol = m_nPageCol;
	nRow = m_nPageRow;
}

void CPage::SetPageColRow(int nCol, int nRow)
{
	ASSERT( nCol >=0 && nRow >= 0 );
	m_nPageCol = nCol;
	m_nPageRow = nRow;
}

PointPlace CPage::GetPointPlace( CPoint point )
{	
	CRect rectPage	= GetRectPage ( );
	CRect rectAlign	= GetRectAlign( );
	CRect rectField	= GetRectField( );

	if( !rectPage.PtInRect(point) )
		return ppOutside;

	if( rectField.PtInRect(point) )
		return ppWork;

	if( rectAlign.PtInRect(point) )
		return ppField;

	if( rectPage.PtInRect(point) )
		return ppAlign;

	return ppOutside;

}


BOOL CPage::IsEmptyPage()
{
	if( m_ctrl.GetSize() > 0 )
		return FALSE;
	else
		return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPrintView window
BEGIN_DISPATCH_MAP(CPrintView, CViewBase)
	//{{AFX_DISPATCH_MAP(CCarioView)
	DISP_FUNCTION(CPrintView, "GetFirstCtrlPos", GetFirstCtrlPos, VT_I4, VTS_NONE)
	DISP_FUNCTION(CPrintView, "GetNextCtrlPos", GetNextCtrlPos, VT_I4, VTS_I4)
	DISP_FUNCTION(CPrintView, "GetCtrlDispatch", GetCtrlDispatch, VT_DISPATCH, VTS_I4)
	DISP_FUNCTION(CPrintView, "GetCtrlProgID", GetCtrlProgID, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CPrintView, "GetSelectedPos", GetSelectedPos, VT_I4, VTS_NONE)
	DISP_FUNCTION(CPrintView, "StoreCtrlData", StoreCtrlData, VT_EMPTY, VTS_I4)	
	DISP_FUNCTION(CPrintView, "SetSelectedPos", SetSelectedPos, VT_EMPTY, VTS_I4)	
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ICCarioView to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {9A6C0E2D-DF20-4a7c-A308-4638BB501EE4}
static const GUID IID_IPrintViewDisp = 
{ 0x9a6c0e2d, 0xdf20, 0x4a7c, { 0xa3, 0x8, 0x46, 0x38, 0xbb, 0x50, 0x1e, 0xe4 } };

GUARD_IMPLEMENT_OLECREATE(CPrintView, "Print.PrintView", 
0x9a943ea9, 0xb6f6, 0x464b, 0xb4, 0x4d, 0x9b, 0x8d, 0x44, 0x71, 0xc2, 0x31);

IMPLEMENT_DYNCREATE(CPrintView, CAxBaseView);

BEGIN_INTERFACE_MAP(CPrintView, CAxBaseView)
	INTERFACE_PART(CPrintView, IID_IPrintViewDisp, Dispatch)	
	INTERFACE_PART(CPrintView, IID_IReportView, ReportVw)	
	INTERFACE_PART(CPrintView, IID_IReportView2, ReportVw)
	INTERFACE_PART(CPrintView, IID_IReportPageView, PageVw)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CPrintView, ReportVw);
IMPLEMENT_UNKNOWN(CPrintView, PageVw);


GUID CPrintView::get_view_dispiid( )
{
	return IID_IPrintViewDisp;
}


BEGIN_MESSAGE_MAP(CPrintView, CAxBaseView)
	//{{AFX_MSG_MAP(CPrintView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int	nViewCount = 0;

////////////////////////////////////////////////////////////////
CPrintView::CPrintView()
{
	m_bQuasiPrint = false;
	nViewCount++;
	_OleLockApp( this );
	
	EnableAggregation();
	EnableAutomation();

	//PAge count
	m_nPageCountHorz = 1;	
	m_nPageCountVert = 0;
	
	m_pointFormOffset = CPoint(0,0);
	TRACE( "CPrintView::CPrintView()\n" );	
	
	m_bIsLoadingProcess = FALSE;

	viewMode = vmDesign;

	m_ScrollOffset = CSize(0, 0);

	m_bDisconnectNotifyContrl = FALSE;

	m_punkReportSourceDoc = NULL;
	
	m_sName = c_szCPrintView;
	m_sUserName.LoadString(IDS_PRINTVIEW_NAME);

	m_bAttachActiveObject = false;
	
	m_bUseAnimationDraw = false;//( ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW, "AnimationDraw", 0 ) == 1 );

	m_bShowBorderAroundCtrl = false;

	m_bReadOnly	= false;

	m_npage_align = 1000;

	m_clrBorderAroundCtrl =
	m_clrBack =
	m_clrAlign =
	m_clrField =
	m_clrMainWork =
	m_clrAdditionalWork =
	m_clrFreeWork = RGB( 0, 0, 0 );

	m_pt_scroll		= CPoint( 0, 0 );
	m_fzoom			= 0;

	m_lpaper_width_px = m_lpaper_height_px = 0;
	m_lpaper_width_mm = m_lpaper_height_mm = 0;
	m_lpaper_offset_x_px = m_lpaper_offset_y_px = 0;
}

////////////////////////////////////////////////////////////////
CPrintView::~CPrintView()
{
	nViewCount--;
	DeleteContext();
	TRACE( "CPrintView::~CPrintView()\n" );	

	_OleUnlockApp( this );
}


////////////////////////////////////////////////////////////////
BOOL CPrintView::BuildContext()
{
	return TRUE;	
}

////////////////////////////////////////////////////////////////
BOOL CPrintView::DeleteContext()
{
	for( int i=0;i<m_page.GetSize();i++ )
		delete m_page[i];

	m_page.RemoveAll();

	return TRUE;	
}

////////////////////////////////////////////////////////////////
void CPrintView::AttachActiveObjects()
{

	m_bAttachActiveObject = true;

	CAxBaseView::AttachActiveObjects();

	if( ::IsWindow( GetSafeHwnd() ) )
	{
		if( viewMode == vmPreview )
		{
			//EnterDesignMode();

			viewMode = vmDesign; 
			Update();

			EnterPreviewMode();
		}
		else
		{
			Update( NULL, NULL );
		}
	}

	m_bAttachActiveObject = false;

	return;
}

//////////	//////////////////////////////////////////////////////
int CPrintView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAxBaseView::OnCreate(lpCreateStruct) == -1)
		return -1;

	load_settings();

	sptrIScrollZoomSite	pstrS( GetControllingUnknown() );

	if( pstrS )
		pstrS->SetAutoScrollMode( TRUE );	

	//Register( ::GetAppUnknown() );

	return 0;
}

////////////////////////////////////////////////////////////////
void CPrintView::OnDestroy() 
{
	//UnRegister( ::GetAppUnknown() );

	CAxBaseView::OnDestroy();
}


////////////////////////////////////////////////////////////////
void CPrintView::load_settings()
{
	m_bShowBorderAroundCtrl = 
		( 1L == ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW, "ShowBorderAroundCtrl", 1L ) );

	m_clrBorderAroundCtrl = 
		( 1L == ::GetValueColor( ::GetAppUnknown(), PRINT_PREVIEW, "BorderAroundCtrlColor", RGB(0,0,0) ) );

	m_npage_align = ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW, "BorderSize", 1000 );

	if( m_npage_align < 0 )
		m_npage_align = 1000;


	m_clrBack		= ::GetValueColor( ::GetAppUnknown(), "\\PageColor", "Background", 
							::GetSysColor(COLOR_BTNSHADOW) );

	COLORREF clrDefColor	= ::GetSysColor(COLOR_WINDOW);
	m_clrAlign			= ::GetValueColor( ::GetAppUnknown(), "\\PageColor", "Align", clrDefColor );
	m_clrField			= ::GetValueColor( ::GetAppUnknown(), "\\PageColor", "Field", clrDefColor );

	m_clrMainWork		= ::GetValueColor( ::GetAppUnknown(), "\\PageColor", "MainWork", clrDefColor );
	m_clrAdditionalWork = ::GetValueColor( ::GetAppUnknown(), "\\PageColor", "AddWork", clrDefColor );

	m_clrFreeWork		= ::GetValueColor( ::GetAppUnknown(), "\\PageColor", "FreeWork", clrDefColor );

	
}

////////////////////////////////////////////////////////////////
//Set new view scroll size
void CPrintView::SetNewScrollSize()
{
	sptrIScrollZoomSite	sptrScroll( GetControllingUnknown() );
	if( sptrScroll == NULL)
		return;

	sptrIReportForm	sptrForm( m_sptrControls );
	if( sptrForm == NULL )
	{
		CClientDC ClientDC(this);
		CSize Size;
		draw_report_empty(&ClientDC, &Size);
		sptrScroll->SetClientSize(Size);
		Invalidate( );
		return;
	}
	
	CSize paperSize;
	sptrForm->GetPaperSize( &paperSize );	

	paperSize = ConvertFromVTPixelToMM( paperSize );


	CSize oldSize;
	sptrScroll->GetClientSize( &oldSize );


	CSize sizeNew( 	(m_nPageCountHorz + 1) * ConvertFromVTPixelToMM(m_npage_align)
						+ m_nPageCountHorz * paperSize.cx, 
					(m_nPageCountVert + 1) * ConvertFromVTPixelToMM(m_npage_align)
						+ m_nPageCountVert * paperSize.cy
					);

	/*
	CRect rcClient;
	GetClientRect( &rcClient );
	rcClient = ConvertToClient( &rcClient);
	if( sizeNew.cx < rcClient.Width() )
		sizeNew.cx = rcClient.Width();
		*/

	


	if( oldSize != sizeNew )
	{
		sptrScroll->SetClientSize( sizeNew );		
		Invalidate( );
		TRACE("Set new size\n");
	}

}

////////////////////////////////////////////////////////////////
void CPrintView::OnSize(UINT nType, int cx, int cy) 
{
	OnZoomChange();
	Invalidate();

	CAxBaseView::OnSize(nType, cx, cy);	
}

///////////////////////////////////////////////////
void CPrintView::RecalcColRowCount(IUnknown *punkControlDelete)
{
	
	sptrIScrollZoomSite	sptrScroll( GetControllingUnknown() );
	if( sptrScroll == NULL)
		return;

	CRect rcClient;
	GetClientRect( &rcClient );

	rcClient = ConvertFromMMToVTPixel( rcClient );

	double fZoom;
	sptrScroll->GetZoom( &fZoom );
	
	if( fZoom < 0.0 )
		fZoom = 1.0;

	int nMaxCol, nMaxRow;
	nMaxCol = nMaxRow = 0;

	nMaxCol = long( ( rcClient.Width() / fZoom ) / ( GetPanePageRect(0,0).Width() + m_npage_align) );

	if(nMaxCol > m_page.GetSize() )
		nMaxCol = m_page.GetSize();

	if( nMaxCol  < 1)
		nMaxCol = 1;


	if( IsSimpleEditMode() && viewMode == vmDesign )
		nMaxCol = 1;
	

	//nMaxCol = m_nPageCountHorz;
	nMaxRow = m_page.GetSize() / nMaxCol;
	if( (m_page.GetSize() % nMaxCol) != 0)
		nMaxRow++;


	//if( m_nPageCountHorz != nMaxCol || m_nPageCountVert != nMaxRow )
	//{
		m_nPageCountHorz = nMaxCol;
		m_nPageCountVert = nMaxRow;
		SetNewScrollSize();		
	//}
	

}


///////////////////////////////////////////////////
//
//	Page position
//
//	

///////////////////////////////////////////////////
CRect CPrintView::GetPanePageRect ( int nCol, int nRow )
{
	CRect pageRect = NORECT;

	sptrIReportForm	sptrForm( m_sptrControls );
	if( sptrForm == NULL )
		return pageRect;
	
	CSize paperSize;
	sptrForm->GetPaperSize( &paperSize );	


	pageRect.left	= nCol * paperSize.cx + nCol;
	pageRect.top	= nRow * paperSize.cy + nRow;
	pageRect.right	= pageRect.left + paperSize.cx;
	pageRect.bottom	= pageRect.top  + paperSize.cy;	

	return pageRect;
}

///////////////////////////////////////////////////
CRect CPrintView::GetPaneAlignRect( int nCol, int nRow )
{
	CRect alignRect = NORECT;

	sptrIReportForm	sptrForm( m_sptrControls );
	if( sptrForm == NULL )
		return alignRect;

	CRect paperAlign;
	sptrForm->GetPaperAlign( &paperAlign );	

	CRect pageRect = GetPanePageRect ( nCol, nRow );

	alignRect.left	= pageRect.left + paperAlign.left;
	alignRect.top	= pageRect.top  + paperAlign.top;
	alignRect.right		= pageRect.right - paperAlign.right;
	alignRect.bottom	= pageRect.bottom - paperAlign.bottom;

	return alignRect;
}

///////////////////////////////////////////////////
CRect CPrintView::GetPaneFieldRect( int nCol, int nRow )
{
	CRect fieldRect = NORECT;

	sptrIReportForm	sptrForm( m_sptrControls );
	if( sptrForm == NULL )
		return fieldRect;

	CRect paperField;
	sptrForm->GetPaperField( &paperField );	

	CRect pageRect = GetPanePageRect ( nCol, nRow );

	fieldRect.left	= pageRect.left + paperField.left;
	fieldRect.top	= pageRect.top  + paperField.top;
	fieldRect.right		= pageRect.right - paperField.right;
	fieldRect.bottom	= pageRect.bottom - paperField.bottom;

	return fieldRect;
}	

///////////////////////////////////////////////////
//Find near page col & row by point
void CPrintView::GetPaneColRow( CPoint point, int& nCol, int& nRow )
{
	CRect pageRect = GetPanePageRect( 0, 0 );
	int nPageWidth = pageRect.Width();
	int nPageHeight = pageRect.Height();	

	nCol = point.x / (nPageWidth);
	nRow = point.y / (nPageHeight);
}

///////////////////////////////////////////////////
bool CPrintView::IsRectIntersect( CRect rc1, CRect rc2 )
{
	CRect rc;

	BOOL bRes = ::IntersectRect( &rc, &rc1, &rc2 );

	return bRes == TRUE;
}

///////////////////////////////////////////////////
void CPrintView::VerifyCtrlPos( IUnknown* punkAdd )
{

	CRect rcPage  =	GetPanePageRect( 0, 0 );
	CRect rcAlign =	GetPaneAlignRect( 0, 0 );
	CRect rcField =	GetPaneFieldRect( 0, 0 );			
	
	CRect rcCtrl;

	POSITION	pos = m_sptrControls.GetFirstObjectPosition();

	//at first test need inc fields ?
	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );		

		sptrIReportCtrl sptrCtrl( punk );
		punk->Release();

		if( sptrCtrl == NULL )
			continue;

		if( IsViewCtrl(sptrCtrl) )
			continue;


		if( m_bIsLoadingProcess || ::GetObjectKey( punkAdd ) != ::GetObjectKey( sptrCtrl ) )
			continue;


		sptrCtrl->GetDesignPosition( &rcCtrl );
		
		PointPlace pp = GetPointPlace( CPoint(rcCtrl.left, rcCtrl.top) );
		
		CSize size = CSize( rcCtrl.Width(), rcCtrl.Height() );
		CPoint ptDesign = CPoint( rcCtrl.left, rcCtrl.top );			

		PointPlace pp1 = GetPointPlace( ptDesign );
		PointPlace pp2 = GetPointPlace( CPoint( ptDesign.x + size.cx, ptDesign.y ) );
		PointPlace pp3 = GetPointPlace( CPoint( ptDesign.x + size.cx, ptDesign.y  + size.cy ) );
		PointPlace pp4 = GetPointPlace( CPoint( ptDesign.x			 , ptDesign.y ) );

		if( !(pp1 == ppField || pp2 == ppField || pp3 == ppField || pp4 == ppField ) )
			continue;


		BOOL bNeedIncFields = FALSE;
		int nNewLeft, nNewTop, nNewRight, nNewBottom;
		nNewLeft = nNewTop = nNewRight = nNewBottom = 0;

		

		//Left up 
		if( (ptDesign.x < rcField.left) &&
			(ptDesign.x + size.cx > rcField.left) &&			
			IsRectIntersect( rcCtrl, rcField )
			

			)
		{
			bNeedIncFields = TRUE;
			nNewLeft = rcField.left + size.cx - (rcField.left - ptDesign.x);
		}
	

		if( (ptDesign.y < rcField.top) &&
			(ptDesign.y + size.cy > rcField.top)  &&			
			IsRectIntersect( rcCtrl, rcField ) )
		{
			bNeedIncFields = TRUE;
			nNewTop = rcField.top + size.cy - (rcField.top - ptDesign.y);
		}
						  
		if( (ptDesign.y + size.cy > rcField.bottom) &&
			(ptDesign.y < rcField.bottom)  &&			
			IsRectIntersect( rcCtrl, rcField ) )
		{
			bNeedIncFields = TRUE;
			nNewBottom = rcPage.bottom - ptDesign.y;
		}

		if( (ptDesign.x + size.cx > rcField.right) &&
			(ptDesign.x < rcField.right)  &&			
			IsRectIntersect( rcCtrl, rcField ) )
		{
			bNeedIncFields = TRUE;
			nNewRight = rcPage.right - ptDesign.x;
		}										

		CString strMessage;
		strMessage.LoadString( IDS_NEED_CHANGE_MARGINS );

		if( bNeedIncFields && AfxMessageBox( strMessage, MB_YESNO | MB_ICONQUESTION) == IDYES )
		{
			CRect paperField;
			sptrIReportForm	sptrForm( m_sptrControls );
			if( sptrForm != NULL )
			{								
				sptrForm->GetPaperField( &paperField );	
				
				if( nNewLeft )
					paperField.left		= nNewLeft;

				if( nNewTop )
					paperField.top		= nNewTop;

				if( nNewRight )
					paperField.right	= nNewRight;

				if( nNewBottom )
					paperField.bottom	= nNewBottom;				
				
				CSize size;
				sptrForm->GetPaperSize( &size );

				CRect rcAlign;
				sptrForm->GetPaperAlign( &rcAlign );

				paperField.left = max( paperField.left, rcAlign.left );
				paperField.top = max( paperField.top, rcAlign.top );
				paperField.right = max( paperField.right, rcAlign.right );
				paperField.bottom = max( paperField.bottom, rcAlign.bottom );
				
				if( paperField.left + paperField.right > size.cx - rcAlign.left - rcAlign.right )
				{
					paperField.left  = rcAlign.left;
					paperField.right = rcAlign.right;
				}

				if( paperField.top + paperField.bottom > size.cy - rcAlign.top - rcAlign.bottom )
				{
					paperField.top  = rcAlign.top;
					paperField.bottom = rcAlign.bottom;
				}

				sptrForm->SetPaperField( paperField );	
			}

		}

	}


	// A.M. fix - BT 3520. Решено не переносить контролы, а просто не рисовать те,
	// которые не помещаются. Это будет в BuildPages
	// BT 4295. Если контрол вылезает за поля, но часть его туда помещается, попробуем
	// все-таки его уменьшить.
	pos = m_sptrControls.GetFirstObjectPosition();

	rcPage  =	GetPanePageRect( 0, 0 );
	rcAlign =	GetPaneAlignRect( 0, 0 );
	rcField =	GetPaneFieldRect( 0, 0 );			

	
	
	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );		

		sptrIReportCtrl sptrCtrl( punk );
		if( sptrCtrl != NULL )
		{	
			
			sptrCtrl->GetDesignPosition( &rcCtrl );


			PointPlace pp = GetPointPlace( CPoint(rcCtrl.left, rcCtrl.top) );

			CSize size = CSize( rcCtrl.Width(), rcCtrl.Height() );
			CPoint ptDesign = CPoint( rcCtrl.left, rcCtrl.top );			


			BOOL bViewAX = IsViewCtrl( sptrCtrl );

			//leave ctrl pos Or only correct start point for ViewAX 
			if( (pp == ppOutside || pp == ppAlign) || ( pp != ppWork && bViewAX ) )
			{
				ptDesign = rcCtrl.TopLeft();

				if( rcCtrl.left < rcField.left && rcCtrl.right > rcField.left)
					rcCtrl.left = rcField.left;

				if( rcCtrl.top < rcField.top && rcCtrl.bottom > rcField.top)
					rcCtrl.bottom = rcField.top;

				if( rcCtrl.left < rcField.right && rcCtrl.right > rcField.right)
					rcCtrl.right = rcField.right;

				if( rcCtrl.top < rcField.bottom && rcCtrl.bottom > rcField.bottom)
					rcCtrl.bottom = rcField.bottom;

				size = rcCtrl.Size();
				ptDesign = rcCtrl.TopLeft();
				pp = ppWork;
			}

			

			if( pp == ppField )// Only correct size if need
			{
				int nMaxWidth  = rcAlign.Width() - (ptDesign.x - rcAlign.left);
				int nMaxHeight = rcAlign.Height() - (ptDesign.y - rcAlign.top);

				if( size.cx > nMaxWidth )
					size.cx = nMaxWidth;
				
				if( size.cy > nMaxHeight )
					size.cy = nMaxHeight;

			}


			if( size.cx <= 0 )
				size.cx = 500;

			if( size.cy <= 0 )
				size.cy = 500;

			pp = GetPointPlace( ptDesign );

			if( IsSimpleEditMode() && viewMode == vmDesign) // we need to place ctrl's whithout page transfer
			{
				if( pp == ppWork )
				{
					if( ptDesign.x + size.cx > rcField.right )
					{
						if( size.cx > rcField.Width() )
						{
							size.cx = rcField.Width();
							ptDesign.x = rcField.left;
						}
						else
						{
							ptDesign.x = rcField.right - size.cx;
						}
					}

					if( ptDesign.y + size.cy > rcField.bottom )
					{
						if( size.cy > rcField.Height() )
						{
							size.cy = rcField.Height();
							ptDesign.y = rcField.top;
						}
						else
						{
							ptDesign.y = rcField.bottom - size.cy;
						}
					}						
						
				}
			}

			sptrCtrl->SetDesignPosition( 
						CRect(
						ptDesign.x, ptDesign.y,
						ptDesign.x+size.cx, ptDesign.y + size.cy
						) );
		}

		punk->Release();
	}

}

bool CPrintView::IsControlInside( IUnknown *punkCtrl, CRect rcCtrl)
{
	PointPlace pp1 = GetPointPlace( CPoint(rcCtrl.left, rcCtrl.top) );
	/*PointPlace pp2 = GetPointPlace( CPoint(rcCtrl.left, rcCtrl.bottom) );*/
	/*PointPlace pp3 = GetPointPlace( CPoint(rcCtrl.right, rcCtrl.top) );*/
	PointPlace pp4 = GetPointPlace( CPoint(rcCtrl.right, rcCtrl.bottom) );

	// [vanek] : теперь viewax можно вытаскивать на поля BT2000:4082 - 15.11.2004
	/*BOOL bViewAX = IsViewCtrl(punkCtrl);
    if (bViewAX)
		return pp1 == ppWork && pp2 == ppWork && pp3 == ppWork && pp4 == ppWork;
	else*/
		return (pp1==ppWork||pp1==ppField)/*&&(pp2==ppWork||pp2==ppField)&&
			(pp3==ppWork||pp3==ppField)*/&&(pp4==ppWork||pp4==ppField);
}

bool CPrintView::IsObjectCanBeSelected(IUnknown *punkObj)
{
	GuidKey guid = ::GetObjectKey(punkObj);
	for (int i = 0; i < m_page.GetSize(); i++)
	{
		CPage *pPage = m_page.GetAt(i);
		for (int j = 0; j < pPage->m_ctrl.GetSize(); j++)
		{
			CControlInfo* pInfo = pPage->m_ctrl.GetAt(j);
			GuidKey guidCur = ::GetObjectKey(pInfo->m_pCtrlData);
			if (guid == guidCur)
				return true;
		}
	}
	return false;
}


double CPrintView::GetDisplayPixelPMM()
{
	static double fPixelPerMM = 0;
	if( fPixelPerMM == 0 )
	{
		DisplayCalibration dc;
		fPixelPerMM = dc.GetPixelPerMM();
	}

	return fPixelPerMM;
}

	

CSize CPrintView::ConvertFromHimetricToDisplay( CSize size)
{
	size.cx = ConvertFromVTPixelToMM( size.cx );
	size.cy = ConvertFromVTPixelToMM( size.cy );

	CSize newSize;

	newSize.cx = size.cx;//* GetDisplayHorzPixelPMM(); // fPaperWidthMM * fPaperWidthPx;
	newSize.cy = size.cy;//* GetDisplayVertPixelPMM(); // fPaperHeightMM * fPaperHeightPx;

	return newSize;

}

int CPrintView::ConvertFromMMToVTPixel( int nMM )
{
	return ((int) ( ((double)nMM) * 100.0 / GetDisplayPixelPMM() ) );
}
int CPrintView::ConvertFromVTPixelToMM( int nVTpixel)
{
	double fPPM=GetDisplayPixelPMM();
	return ((int) ( ((double)nVTpixel) * fPPM / 100.0 +.5 ) );	
}


///////////////////////////////////////////////////
static bool m_bRemoveControl = false;
void CPrintView::BuildPages( IUnknown* punkDelete )
{
	VerifyPageContext();

	for( int i=0;i<m_page.GetSize();i++ )
	{		
		delete m_page[i];
	}

	m_page.RemoveAll();

	

	VerifyPageContext();

	CSize paperSize;
	CRect paperAlign, paperField;

	sptrIReportForm	sptrForm( m_sptrControls );
	if( sptrForm == NULL )
		return;
		
	sptrForm->GetPaperSize( &paperSize );	
	sptrForm->GetPaperAlign( &paperAlign );	
	sptrForm->GetPaperField( &paperField );	


	int nControls = m_sptrControls.GetCount();
	POSITION	pos = m_sptrControls.GetFirstObjectPosition();
	
	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );
		if( punkDelete != punk )
		{
			sptrIReportCtrl pCtrl( punk );
			CRect rectCtrl;

			if( viewMode == vmDesign)
				pCtrl->GetDesignPosition( rectCtrl );
			else
				pCtrl->GetPreviewPosition( rectCtrl );

			if ((nControls > 1 && !m_bRemoveControl || nControls > 2 && m_bRemoveControl) && !IsControlInside(pCtrl, rectCtrl))
			{
				// [vanek] : исключение - ViewAX + Apportionment view (до тех пор,пока не выработается концепция
				// того, как это должно быть :)) - 15.11.2004
				_bstr_t bstr_view_progid;
				IViewCtrlPtr sptr_viewax;
				{
					POSITION	posCtrl = _Find( punk );
					if( posCtrl )
					{				
						ControlData *pdata = m_controlInfos.GetAt( posCtrl );
						if( pdata && pdata->ptrCtrl != NULL )
							sptr_viewax = pdata->ptrCtrl;						
					}
				}

				if( sptr_viewax != 0 )          
					sptr_viewax->GetViewProgID( bstr_view_progid.GetAddress() );

				if( bstr_view_progid != _bstr_t(_T("Apportionment.AView")) )
				{
					if (m_pframe->CheckObjectInSelection(punk))
						m_pframe->UnselectObject(punk);
					punk->Release();			
					continue;
				}
			}


			int nCol, nRow;
			int nMainPage;

			if( viewMode == vmDesign)
				pCtrl->GetDesignColRow( &nCol, &nRow );
			else
				pCtrl->GetPreviewColRow( &nCol, &nRow );

			pCtrl->GetDesignOwnerPage( &nMainPage );


			if( nCol < 0) nCol = 0;
			if( nRow < 0) nRow = 0;
			
			
			
			CRect rectPage = GetPanePageRect( nCol, nRow );

			int nOccupedCol, nOccupedRow;
			CRect* pRect;
			CSize* pSizeViewCtrl;
			CSize* pUserOffset;
			CSize* pUserOffsetDX;

			
			GenerateCtrlRect( punk, CPoint(rectCtrl.left, rectCtrl.top), 
								CSize(rectCtrl.Width(), rectCtrl.Height()),
								pRect, nOccupedCol, nOccupedRow,
								pSizeViewCtrl, pUserOffset, pUserOffsetDX, viewMode );		

			int nOffsetX, nOffsetY;			

			int index;

			nOffsetX = 0;
			for(int iCol=0;iCol<nOccupedCol;iCol++ )
			{
				nOffsetY = 0;
				for(int iRow=0;iRow<nOccupedRow;iRow++ )
				{
					CPage* page=NULL;

					BOOL bCreateNewPage = FALSE;
					page = FindPage( nMainPage, nCol + iCol, nRow + iRow );

					if(page == NULL)
					{
						page = new CPage;
						bCreateNewPage = TRUE;
					}

					page->SetMainPage( nMainPage );
					page->SetPageColRow( nCol + iCol, nRow + iRow );
					page->SetPaperSize( paperSize );
					page->SetPaperAlign( paperAlign );
					page->SetPaperField( paperField );

					index = iRow*nOccupedCol + iCol;


					CSize ctrlSize   = CSize(rectCtrl.Width(), rectCtrl.Height());
					CSize ctrlOffset = CSize( nOffsetX, nOffsetY );

					page->AddCtrl( punk,
								pRect[index], 
								ctrlSize,
								ctrlOffset,
								index == 0 ? TRUE : FALSE,
								pSizeViewCtrl[index],
								pUserOffset[index],
								pUserOffsetDX[index]								
								);

					if( bCreateNewPage)
						m_page.Add(page);
					
					nOffsetY += pRect[index].Height();
				}

				nOffsetX += pRect[index].Width();

			}

			
			delete []pRect;
			delete []pSizeViewCtrl;
			delete []pUserOffset;
			delete []pUserOffsetDX;


		}
		punk->Release();

	}
	
	if( viewMode == vmDesign )
	{
		
		int nNewMainPage = FindFreeMainPage( punkDelete );

		CPage* pNewPage = new CPage;

		pNewPage->SetMainPage( nNewMainPage );
		pNewPage->SetPageColRow( 0, 0 );
		pNewPage->SetPaperSize( paperSize );
		pNewPage->SetPaperAlign( paperAlign );
		pNewPage->SetPaperField( paperField );
		m_page.Add( pNewPage );

		VerifyPageContext();
	}


}


CPage* CPrintView::FindPage(  int nMainPage, int nCol, int nRow )
{
	for( int i=0;i<m_page.GetSize();i++ )
	{
		int nNewCol, nNewRow;		
		m_page[i]->GetPageColRow( nNewCol, nNewRow );
		int nNewMainPage = m_page[i]->GetMainPage();

		if( nNewMainPage == nMainPage && nNewCol == nCol &&  nNewRow == nRow )
			return m_page[i];
	}

	return NULL;
}


///////////////////////////////////////////////////
//Find free col row on pane
int CPrintView::FindFreeMainPage( IUnknown* punkDelete )
{
	
	int nMaxMainPage = -1;
	
	POSITION	pos = m_sptrControls.GetFirstObjectPosition();
	
	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );
		if( punk != punkDelete )
		{
			sptrIReportCtrl pCtrl( punk );

			int nMainPage;
			pCtrl->GetDesignOwnerPage( &nMainPage );			
			nMaxMainPage = max( nMaxMainPage, nMainPage );			
		}

		punk->Release();

	}

	return ++nMaxMainPage;

}

int ComparePage( CPage* pPage1, CPage* pPage2 )
{	

	int nCol1, nRow1;
	int nMainPage1 = pPage1->GetMainPage();
	pPage1->GetPageColRow( nCol1, nRow1 );
	
	int nCol2, nRow2;
	int nMainPage2 = pPage2->GetMainPage();
	pPage2->GetPageColRow( nCol2, nRow2 );

	if( nMainPage1 > nMainPage2 )
		return 1;

	if( nMainPage1 < nMainPage2 )
		return -1;

	if( nRow1 > nRow2 )
		return 1;

	if( nRow1 < nRow2 )
		return -1;

	if( nCol1 > nCol2 )
		return 1;

	if( nCol1 < nCol2 )
		return -1;
	
	return 0;
}


void CPrintView::SortPages()
{	
	VerifyPageContext();
	
	if( m_page.GetSize() < 2 )
		return;

	for( int i=0;i<(m_page.GetSize()-1);i++)
	{
		CPage* pPage1 = m_page[i];
		int nCol1, nRow1;
		int nMainPage1 = pPage1->GetMainPage();
		pPage1->GetPageColRow( nCol1, nRow1 );

		int	nCurIdx = i;
		for(int ii=i+1;ii<m_page.GetSize();ii++)
		{	
			CPage* pPage2 =  m_page[ii];
			int nCol2, nRow2;
			int nMainPage2 = pPage2->GetMainPage();
			pPage2->GetPageColRow( nCol2, nRow2 );

			if( ComparePage( m_page[nCurIdx], pPage2) == 1 ) 
			{								
				nCurIdx = ii;
			}
			
		}
		m_page[ i]		= m_page[nCurIdx];
		m_page[nCurIdx] = pPage1;		
	}


	VerifyPageContext();
}



void CPrintView::VerifyPageContext()
{  

	m_page.AssertValid();

	CPage* pPage;
	for(int i=0;i<m_page.GetSize();i++)
	{
		pPage = m_page[i];


		for( int j = 0; j<m_page.GetSize(); j++ )
		{
			if( pPage == m_page[j] )
			{
				ASSERT( j == i );
			}
		}

		ASSERT_POINTER(pPage, CPage);		

		int nCol, nRow;		
		pPage->GetPageColRow( nCol, nRow );
		
//		ASSERT( nCol >= 0 && nRow >= 0);		

	}
	
	
}


///////////////////////////////////////////////////
//Set new client start point for all pages
void CPrintView::SetPagesPos()
{
	if( viewMode == vmPrinting )
		return;

	CRect rcPage = GetPanePageRect(0,0);

	int nPageWidth  = rcPage.Width();
	int nPageHeight = rcPage.Height();
	//

	int nAddCenter = 0;
	if( IsSimpleEditMode() && viewMode == vmDesign )
	{
		//paul 12.03.2003
		nAddCenter = 0;	
		/*
		CRect rcClient;
		GetClientRect( &rcClient );

		rcClient = ConvertToClient( &rcClient);
		rcClient = ConvertFromMMToVTPixel( rcClient );

		nAddCenter = ( rcClient.Width() - rcPage.Width() - m_npage_align ) / 2;		
		if( nAddCenter < 0)
			nAddCenter = 0;
			*/
	}	
	

	int nX, nY;
	nX = nY = m_npage_align;	
	//m_nPageCountHorz
	int nCurHorzCol = 0;
	int nCurVertCol = 0;
	for( int i=0;i<m_page.GetSize();i++ )
	{
		nX = m_npage_align + nCurHorzCol * (nPageWidth  + m_npage_align) + nAddCenter;
		nY = m_npage_align + nCurVertCol * (nPageHeight + m_npage_align);

		m_page[i]->SetClientStartPoint( CPoint(nX, nY) );

		nCurHorzCol++;	
		if(nCurHorzCol >= m_nPageCountHorz)
		{
			nCurHorzCol = 0;
			nCurVertCol++;
		}
	}
}

void CPrintView::SyncAXRect()
{
	POSITION	pos = m_sptrControls.GetFirstObjectPosition();
	
	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );
		sptrIActiveXCtrl pCtrl( punk );
		sptrIReportCtrl	sptrReportCtrl( punk );

		for( int i=0;i<m_page.GetSize();i++ )
		{
			for( int ii=0;ii<m_page[i]->m_ctrl.GetSize();ii++ )
			{
				CControlInfo* pCI = m_page[i]->m_ctrl[ii];
				if( pCI->m_pCtrlData == punk && pCI->m_bFirstEntry )
				{					
					CRect rcPage = m_page[i]->GetRectPage( );
					CRect rcAX = CRect( 
							rcPage.left +  pCI->m_rectOnPage.left,
							rcPage.top  +  pCI->m_rectOnPage.top, 
							rcPage.left +  pCI->m_rectOnPage.left + pCI->m_rectOnPage.Width(),
							rcPage.top  +  pCI->m_rectOnPage.top  + pCI->m_rectOnPage.Height()
							);

					CRect rcAX1 = ConvertFromVTPixelToMM( rcAX );					
					pCtrl->SetRect(rcAX1);
					sptrReportCtrl->SetPrevFormPosition( rcAX1 );					
				}
			}
		}

		punk->Release();
	}


//	MoveCtlWindows();

}


PointPlace CPrintView::GetPointPlace( CPoint Point )
{
	CRect rectPage	= GetPanePageRect ( 0,0 );
	CRect rectAlign	= GetPaneAlignRect( 0,0 );
	CRect rectField	= GetPaneFieldRect( 0,0 );

	if( !rectPage.PtInRect(Point) )
		return ppOutside;

	if( rectField.PtInRect(Point) )
		return ppWork;

	if( rectAlign.PtInRect(Point) )
		return ppField;

	if( rectPage.PtInRect(Point) )
		return ppAlign;

	return ppOutside;
}

void CPrintView::GenerateCtrlRect(IUnknown* punk,//control
								  IN CPoint ctrlOffset,//top-left corner of control
								  CSize ctrlSize,//control size
								  OUT CRect*& pRect,//out
								  OUT int& nOccupedCol,//control column
								  OUT int&nOccupedRow,//control row
                                  OUT CSize*& pViewCtrlSize,//
								  OUT CSize*& pUserOffset,//
								  OUT CSize*& pUserOffsetDX,//
								  ViewMode mode /* = vmDesign or vmPreview*/)
{
	CArray <int, int> width;
	CArray <int, int> height;

	CArray <int, int> widthView;
	CArray <int, int> heightView;

	CArray <int, int> userX;
	CArray <int, int> userY;

	CArray <int, int> userDX;
	CArray <int, int> userDY;

	if( GetPointPlace( ctrlOffset) != ppWork )
	{
		nOccupedCol = nOccupedRow = 1;
		pRect = new CRect[1];
		pRect[0] = CRect( ctrlOffset.x, ctrlOffset.y, 
							ctrlOffset.x + ctrlSize.cx, ctrlOffset.y + ctrlSize.cy );

		pViewCtrlSize = new CSize[ 1 ];
		pUserOffset = new CSize[ 1 ];
		pUserOffsetDX = new CSize[ 1 ];

		pViewCtrlSize[0] = ConvertFromVTPixelToMM( CSize(pRect[0].Width(), pRect[0].Height()) );
		
		ASSERT( pViewCtrlSize[0].cx >= 0 && pViewCtrlSize[0].cy >= 0 );
	
		pUserOffset[0] = CSize(0,0);
		pUserOffsetDX[0] = CSize(0,0);

		return;
	}

	bool bViewCtrl = false;

	IUnknown* punkCtrl = NULL;
	POSITION	posCtrl = _Find( punk );
	if( posCtrl )
	{				
		ControlData *pdata = m_controlInfos.GetAt( posCtrl );
		if( pdata && pdata->ptrCtrl != NULL )
		{
			 punkCtrl = pdata->ptrCtrl;
		}
	}


	if( (punkCtrl!=NULL) && CheckInterface( punkCtrl, IID_IViewCtrl) && mode != vmDesign )
	{
		if( CheckInterface( punkCtrl, IID_IPrintView) )
		{
			bViewCtrl = true;
		}
	}

	int nCtrlWidth  = ctrlSize.cx;
	int nCtrlHeight = ctrlSize.cy;

	CRect rectCtrlPage  = GetPanePageRect( 0, 0 );
	// A. M. fix, BT 3520
	CRect rcFieldPage   = bViewCtrl? GetPaneFieldRect( 0, 0 ): GetPaneAlignRect( 0, 0 );

	int nFieldWidth  = rcFieldPage.Width();
	int nFieldHeight = rcFieldPage.Height();

	
	int nFirstWidth  = rcFieldPage.right - ctrlOffset.x;
	int nFirstHeight = rcFieldPage.bottom  - ctrlOffset.y;		

	int nFirstX  = ctrlOffset.x - rectCtrlPage.left;
	int nFirstY  = ctrlOffset.y  - rectCtrlPage.top;		

	ASSERT( nFirstWidth > 0 && nFirstHeight > 0 );

	if( bViewCtrl )
	{
		BOOL bContinue = TRUE;
		int nUserPos = 0;
		int nTemp = 0;

		sptrIPrintView sptrViewCtrl( punkCtrl );
		
		int nReturnWidth = ConvertFromVTPixelToMM( ctrlSize.cx );
		int index = 0;

		while( bContinue )
		{
			int nCurWidth;
			if( index == 0 )
				nCurWidth = ConvertFromVTPixelToMM(nFirstWidth);
			else
				nCurWidth = ConvertFromVTPixelToMM(nFieldWidth);

			
			ISOK( sptrViewCtrl->GetPrintWidth( nCurWidth, &nReturnWidth, 
				&bContinue, 
				nUserPos, &nTemp ) );


			int nRealWidth = 0;

			if( !bContinue )
			{
				nRealWidth = ConvertFromMMToVTPixel( nReturnWidth );
			}
			else
			{
				if( index == 0 )
					nRealWidth = nFirstWidth;				
				else
					nRealWidth = nFieldWidth;
			}

								   
			if( nReturnWidth <= 0 && index == 0 )
				nReturnWidth = ConvertFromVTPixelToMM( ctrlSize.cx );

			if( nRealWidth <= 0 && index == 0 )
				nRealWidth = ctrlSize.cx;


			widthView.Add( nReturnWidth );
			width.Add( nRealWidth );
			userX.Add( nUserPos );
			userDX.Add( nTemp - nUserPos );
			index++;

			nUserPos = nTemp;
		}


		bContinue = TRUE;
		nUserPos = 0;
		nTemp = 0;
		
		int nReturnHeight = ConvertFromVTPixelToMM( ctrlSize.cy );
		index = 0;

		while( bContinue )
		{
			int nCurHeight;
			if( index == 0 )
				nCurHeight = ConvertFromVTPixelToMM( nFirstHeight );
			else
				nCurHeight = ConvertFromVTPixelToMM( nFieldHeight );

			
			ISOK( sptrViewCtrl->GetPrintHeight( nCurHeight, &nReturnHeight, 
				&bContinue, 
				nUserPos, &nTemp ) );
						

			int nRealHeight = 0;

			if( !bContinue )
			{
				nRealHeight = ConvertFromMMToVTPixel( nReturnHeight );
			}
			else
			{
				if( index == 0 )
					nRealHeight = nFirstHeight;				
				else
					nRealHeight = nFieldHeight;
			}


			if( nReturnHeight <= 0 && index == 0 )
				nReturnHeight = ConvertFromVTPixelToMM( ctrlSize.cy );

			if( nRealHeight <= 0 && index == 0 )
				nRealHeight = ctrlSize.cy;


			heightView.Add( nReturnHeight );
			height.Add( nRealHeight );
			userY.Add( nUserPos );			
			userDY.Add( nTemp - nUserPos );
			index++;

			nUserPos = nTemp;
		}


	}
	else
	{
		// Width operation
		if( nFirstWidth >= nCtrlWidth )
			nFirstWidth = nCtrlWidth;

		nCtrlWidth -= nFirstWidth;
		width.Add( nFirstWidth );
		widthView.Add( ConvertFromVTPixelToMM(nFirstWidth) );			
		userX.Add( 0 );
		userDX.Add( 0 );


		while( nCtrlWidth >0 )
		{
			int add;
			if( nFieldWidth >= nCtrlWidth )
				add = nCtrlWidth;
			else
				add = nFieldWidth;

			width.Add( add );

			widthView.Add( ConvertFromVTPixelToMM(add) );			
			userX.Add( 0 );
			userDX.Add( 0 );

			
			nCtrlWidth -= add;
		}
	
		
	
		// Height operation
		if( nFirstHeight >= nCtrlHeight )
			nFirstHeight = nCtrlHeight;

		nCtrlHeight -= nFirstHeight;
		height.Add( nFirstHeight );
		heightView.Add( ConvertFromVTPixelToMM(nFirstHeight) );			
		userY.Add( 0 );
		userDY.Add( 0 );



		while( nCtrlHeight >0 )
		{
			int add;
			if( nFieldHeight >= nCtrlHeight )
				add = nCtrlHeight;
			else
				add = nFieldHeight;

			height.Add( add );		

			heightView.Add( ConvertFromVTPixelToMM(add) );			
			userY.Add( 0 );
			userDY.Add( 0 );

			nCtrlHeight -= add;
		}

	}


	nOccupedCol = width.GetSize();//nRealWidthCount;
	nOccupedRow = height.GetSize();//nRealHeightCount;

	ASSERT( nOccupedRow*nOccupedCol > 0 );

	pRect = new CRect[ nOccupedCol*nOccupedRow ];


	pViewCtrlSize = new CSize[ nOccupedCol*nOccupedRow ];
	pUserOffset = new CSize[ nOccupedCol*nOccupedRow ];
	pUserOffsetDX = new CSize[ nOccupedCol*nOccupedRow ];

	for(int iCol=0;iCol<nOccupedCol;iCol++)
	{
		for(int iRow=0;iRow<nOccupedRow;iRow++)
		{
			int index = iCol + iRow * nOccupedCol;

			if( iCol == 0 )
				pRect[index].left = nFirstX;			
			else
				pRect[index].left = rcFieldPage.left - rectCtrlPage.left;

			if( iRow == 0 )
				pRect[index].top  = nFirstY;
			else
				pRect[index].top  = rcFieldPage.top  - rectCtrlPage.top;

				
			pRect[index].right  = pRect[index].left + width[iCol];
			pRect[index].bottom = pRect[index].top  + height[iRow];

			ASSERT( width[iCol] > 0 && height[iRow] > 0 );
			

			if( widthView[iCol] <= 0 )
			{
				widthView[iCol] = ConvertFromVTPixelToMM( width[iCol] );
				ASSERT( FALSE );
			}

			if( heightView[iRow] <= 0 )
			{
				heightView[iRow] = ConvertFromVTPixelToMM( height[iRow] );
				ASSERT( FALSE );
			}

			pViewCtrlSize[index].cx = widthView[iCol];
			pViewCtrlSize[index].cy = heightView[iRow];


			pUserOffset[index].cx = userX[iCol];
			pUserOffset[index].cy = userY[iRow];

			pUserOffsetDX[index].cx = userDX[iCol];
			pUserOffsetDX[index].cy = userDY[iRow];


		}
	}


	width.RemoveAll();
	height.RemoveAll();
	widthView.RemoveAll();
	heightView.RemoveAll();

	userX.RemoveAll();
	userY.RemoveAll();

	userDX.RemoveAll();
	userDY.RemoveAll();

}

BOOL CPrintView::IsViewCtrl( IUnknown* punk)
{
	IUnknown* punkCtrl = NULL;
	POSITION	posCtrl = _Find( punk );
	if( posCtrl )
	{				
		ControlData *pdata = m_controlInfos.GetAt( posCtrl );
		if( pdata && pdata->ptrCtrl != NULL )
		{
			 punkCtrl = pdata->ptrCtrl;
		}
	}


	if( (punkCtrl!=NULL) && CheckInterface( punkCtrl, IID_IViewCtrl) )
	{
		if( CheckInterface( punkCtrl, IID_IPrintView) )
		{
			return TRUE;
		}
	}

	return FALSE;

}

///////////////////////////////////////////////////
struct _control{
	IUnknown	*punk;
	int nCol;
	int nRow;
	int nMainPage;
};
///////////////////////////////////////////////////
// Kill hole between main pages & hole bettwen slave col & row
void CPrintView::DefragPane( IUnknown* punkDelete )
{
	

	CArray <_control*, _control*> controls;
	int i;

	POSITION	pos = m_sptrControls.GetFirstObjectPosition();
	
	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );
		if( punk != punkDelete )
		{
			sptrIReportCtrl pCtrl( punk );

			int nMainPage;
			int nCol, nRow;
			pCtrl->GetDesignOwnerPage( &nMainPage );

			if( viewMode == vmDesign)
				pCtrl->GetDesignColRow( &nCol, &nRow );
			else
				pCtrl->GetPreviewColRow( &nCol, &nRow );

			

			_control* pControl = new _control;
			pControl->nCol		= nCol;
			pControl->nRow		= nRow;
			pControl->nMainPage = nMainPage;
			pControl->punk		= punk;

			controls.Add( pControl );
		}

		punk->Release();
	}




	if( controls.IsEmpty())
		return;
	
	//2. Kill hole between main pages
    
	/*int nMin = controls[0]->nMainPage;
	for( i=1;i<controls.GetSize();i++)
		nMin = min( nMin, controls[i]->nMainPage );*/



	
	
	for( i=0;i<controls.GetSize()-1;i++ )	
	{
		int	nCurIdx = i;
		for( int ii=i+1;ii<(controls.GetSize());ii++ )
		{					
			if( controls[nCurIdx]->nMainPage > controls[ii]->nMainPage )			
				nCurIdx = ii;			
		}
		_control* pTemp = controls[i];
		controls[i]  = controls[nCurIdx];
		controls[nCurIdx] = pTemp;

	}

	for( i=0;i<controls.GetSize();i++ )
		controls[i]->nMainPage -=controls[0]->nMainPage ;


	int nCur = controls[0]->nMainPage;
	
	for( i=0;i<controls.GetSize();i++ )
	{		
		int nVal = controls[i]->nMainPage;
		controls[i]->nMainPage = nCur;
		if( (i+1) < controls.GetSize() )
			if( nVal != controls[i+1]->nMainPage )
				nCur++;
	}
	  
	


	//Store 
	pos = m_sptrControls.GetFirstObjectPosition();
	
	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );
		sptrIReportCtrl pCtrl( punk );

		for( i=0;i<controls.GetSize();i++)
		{
			_control* pControl = controls[i];
			if( punk == pControl->punk)
			{
				pCtrl->SetDesignOwnerPage( pControl->nMainPage );
				if( viewMode == vmDesign)
					pCtrl->SetDesignColRow( pControl->nCol, pControl->nRow );
				else
					pCtrl->SetPreviewColRow( pControl->nCol, pControl->nRow );

			}
		}

		punk->Release();
	}


	//Delete
	for( i=0;i<controls.GetSize();i++)
	{
		delete controls[i];
	}
	

}

///////////////////////////////////////////////////
//
//	Control position
//
//	

///////////////////////////////////////////////////
//Fire when control pos change
void CPrintView::OnCtrlPosChange(IUnknown *punkControl)
{	
	if( m_bAttachActiveObject )
		return;

	CorrectCtrlPos( punkControl );
	RecalcColRowCount();
	//GeneratePages();
}

///////////////////////////////////////////////////
//Correct control position
void CPrintView::CorrectCtrlPos(IUnknown *punkControl, BOOL bForceExit)
{

	SetReportViewModeToDBControls();

	CRect rcCtrl;	
	IActiveXCtrlPtr	ptrAX( punkControl );
	sptrIReportCtrl	sptrReportCtrl( punkControl );

	rcCtrl = GetObjectRect( punkControl );	
	rcCtrl.DeflateRect( 5, 5 );

	rcCtrl = ConvertFromMMToVTPixel( rcCtrl );

	CRect rectCtrl;
	CRect rcCtrlPrev;


	DWORD dwFlag;
	sptrReportCtrl->GetFlag( &dwFlag );	

	BOOL bManualDrop = TRUE;

	if( m_bIsLoadingProcess || dwFlag == (DWORD)1 )
		bManualDrop = FALSE;

	sptrReportCtrl->SetFlag( 0 );


	bool bNeedUpdateDesignRect=false;

	//Ups
	if( viewMode != vmDesign ) 
	{
		if( bManualDrop )
		{

			BOOL bWidthChange, bHeightChange;			
			bWidthChange = bHeightChange = TRUE;

			POSITION	pos = _Find( punkControl );

			if( pos )
			{			
				ControlData *pdata = m_controlInfos.GetAt( pos );
				if( pdata && pdata->ptrCtrl != NULL )
				{
					if( pdata->ptrCtrl )
					{
						sptrIViewCtrl sptrVC( pdata->ptrCtrl );
						if( sptrVC )
						{
							short trans = 0;							
							
							ISOK( sptrVC->GetObjectTransformation( &trans ) );
							
							ObjectTransformation ot = (ObjectTransformation)trans;
							
							//bWidthChange = bHeightChange = FALSE;
							
							if( ot == otGrowTwoSide )
								bWidthChange = bHeightChange = TRUE;
							else
							if( ot == otGrowHorz )							
								bWidthChange = FALSE;
							else
							if( ot == otGrowVert )							
								bHeightChange = FALSE;
							else//otStretch
								bWidthChange = bHeightChange = FALSE;
						}
					}
				}
			}


			CRect rectDesign;
			CRect rectNewDesign;

			CRect rectOnForm;
			CRect rectOnFormPrev;

			sptrReportCtrl->GetDesignPosition( &rectDesign );
			sptrReportCtrl->GetPrevFormPosition( &rectOnFormPrev );
			

			rectOnFormPrev = ConvertFromMMToVTPixel( rectOnFormPrev );
			
			rectOnForm = rcCtrl;
			

			rectNewDesign.left = rectDesign.left + (rectOnForm.left - rectOnFormPrev.left);
			rectNewDesign.top  = rectDesign.top  + (rectOnForm.top  - rectOnFormPrev.top);


			int nWidth  = rectDesign.Width()  + rectOnForm.Width() - rectOnFormPrev.Width();
			int nHeight = rectDesign.Height() + rectOnForm.Height() - rectOnFormPrev.Height();

			if( !bWidthChange )			
				nWidth = rectDesign.Width();
			
			if( !bHeightChange )			
				nHeight = rectDesign.Height();

			rectNewDesign.right  = rectNewDesign.left + nWidth;
			rectNewDesign.bottom = rectNewDesign.top  + nHeight;

			sptrReportCtrl->SetDesignPosition( rectNewDesign );
			//sptrReportCtrl->SetPreviewPosition( rectNewDesign );
			bNeedUpdateDesignRect = true;

			CString strTrace;
			strTrace.Format( "\ndX=%d, dY=%d, dW=%d, dH=%d, ", 
							rectNewDesign.left - rectDesign.left,
							rectNewDesign.top  - rectDesign.top,
							rectNewDesign.Width() - rectDesign.Width(),
							rectNewDesign.Height()  - rectDesign.Height()
							);

			TRACE( strTrace );


			int nRow, nCol, nMainPage;
			nRow = nCol = nMainPage = 0;

			sptrReportCtrl->GetDesignColRow( &nRow, &nCol );
			sptrReportCtrl->GetDesignOwnerPage( &nMainPage );

			if( nRow < 0 || nCol < 0 || nMainPage < 0 )
			{
				int nPageNum = -1;
				CPoint startPoint = CPoint(rcCtrl.left, rcCtrl.top);
				
				for(int i=0;i<m_page.GetSize();i++)
				{				
					if( m_page[i]->GetRectPage( ).PtInRect(startPoint) )
					{
						nPageNum = i;
					}
				}

				int nPageCol, nPageRow, nPageMainPage;
				nPageCol = nPageRow = nPageMainPage = 0;

				if( nPageNum == -1 && m_page.GetSize() > 0  )
					nPageNum = 0;
				

				if( nPageNum != -1 )
				{
					nPageMainPage = m_page[nPageNum]->GetMainPage();
					m_page[nPageNum]->GetPageColRow( nPageCol, nPageRow );				

					CPoint ptDesign = FromClientToPane( CPoint(rcCtrl.left, rcCtrl.top),
													m_page[nPageNum]);
					CSize sizeDesign( rectNewDesign.Width(), rectNewDesign.Height() );

					sptrReportCtrl->SetDesignPosition( CRect(
										ptDesign.x, ptDesign.y,
										ptDesign.x+sizeDesign.cx, ptDesign.y + sizeDesign.cy
										) );


				}
				else
				{
					ASSERT( FALSE );
				}

				sptrReportCtrl->SetDesignColRow( nPageCol, nPageRow );
				sptrReportCtrl->SetDesignOwnerPage( nPageMainPage );

			}

		}

		if( bForceExit )
			return;

		if(bNeedUpdateDesignRect)
		{
			CRect rectDesign, rectOnForm;
			sptrReportCtrl->GetDesignPosition( &rectDesign );
			ptrAX->GetRect( &rectOnForm );
			rectOnForm = ConvertFromMMToVTPixel( rectOnForm );

			rectDesign.right = rectDesign.left + rectOnForm.Width();
			rectDesign.bottom = rectDesign.top + rectOnForm.Height();
			sptrReportCtrl->SetDesignPosition( rectDesign );
		}
	
		VerifyCtrlPos();				
		MoveCtlWindows();
		InitViewCtrlData();							
		CalcCtrlPreviewPos();		
		Update( NULL, punkControl );


		return;		
	}



	
	sptrReportCtrl->GetDesignPosition( &rectCtrl );
	
	sptrReportCtrl->GetPrevFormPosition( &rcCtrlPrev );
	rcCtrlPrev = ConvertFromMMToVTPixel( rcCtrlPrev );
	
	int nDesignCol, nDesignRow;



	if( bManualDrop ) //After Insert or drag'n'drop
	{
		CPoint startPoint = CPoint(rcCtrl.left, rcCtrl.top);				
		int nPageNum = -1;
		
		for(int i=0;i<m_page.GetSize();i++)
		{
			
			if( m_page[i]->GetRectPage( ).PtInRect(startPoint) )
			{
				nPageNum = i;
			}
		}
		
		
		if( nPageNum > -1 )//Page was found
		{

			CPoint ptDesign = FromClientToPane( CPoint(rcCtrl.left, rcCtrl.top),
													m_page[nPageNum]);
			CSize size;

			if( rectCtrl == NORECT ) //First insert
			{
				size.cx = rcCtrl.Width();
				size.cy = rcCtrl.Height();
			}
			else
			{
				size.cx = rectCtrl.Width();
				size.cy = rectCtrl.Height();

				size.cx += rcCtrl.Width() - rcCtrlPrev.Width();
				size.cy += rcCtrl.Height() - rcCtrlPrev.Height();

			}

			m_page[nPageNum]->GetPageColRow( nDesignCol, nDesignRow );

			sptrReportCtrl->SetDesignPosition( 
						CRect(
						ptDesign.x, ptDesign.y,
						ptDesign.x+size.cx, ptDesign.y + size.cy
						) );

			sptrReportCtrl->SetDesignColRow( nDesignCol, nDesignRow );

			int nMainPage = m_page[nPageNum]->GetMainPage();
			sptrReportCtrl->SetDesignOwnerPage( nMainPage );

		}
		else // Not find page
		{							
			CRect fieldRect = GetPaneFieldRect( 0, 0 );
			sptrReportCtrl->SetDesignPosition( 
						CRect(
						fieldRect.left, fieldRect.top,
						fieldRect.left + 
						(rcCtrl.Width()<=0 ? 10 : rcCtrl.Width()),
						fieldRect.top + 
						(rcCtrl.Height()<=0 ? 10 : rcCtrl.Height())
						) );
			sptrReportCtrl->SetDesignOwnerPage(0);
			sptrReportCtrl->SetDesignColRow( 0, 0 );
		}
	}
	

	if( bForceExit )
		return;

	Update( NULL, punkControl );



}

void CPrintView::InitViewCtrlData()
{
	POSITION posData = m_sptrControls.GetFirstObjectPosition();
	
	while( posData )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( posData );

		POSITION	pos = _Find( punk );
		if( pos )
		{				
			ControlData *pdata = m_controlInfos.GetAt( pos );
			if( pdata && pdata->ptrCtrl != NULL )
			{
				IUnknown* punkCtrl = pdata->ptrCtrl;
				
				sptrIVtActiveXCtrl2 sptrActiveX2( punkCtrl );
				if( sptrActiveX2 )
				{	
					ISOK( sptrActiveX2->SetDoc( m_ptrTargetDoc ));					
					ISOK( sptrActiveX2->SetSite( GetAppUnknown(), GetControllingUnknown() ));

					sptrIViewCtrl sptrVC( punkCtrl );
					if( sptrVC )
					{
						BOOL bResult = FALSE;
						sptrVC->Build( &bResult );
						//ASSERT( bResult );
					}
				}
			}
		}

		punk->Release();
	}

}

void CPrintView::MoveCtlWindows()
{

	POSITION posData = m_sptrControls.GetFirstObjectPosition();
	
	while( posData )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( posData );

		POSITION	pos = _Find( punk );
		if( pos )
		{				
			ControlData *pdata = m_controlInfos.GetAt( pos );
			if( pdata && pdata->ptrCtrl != NULL )
			{
				IUnknown* punkCtrl = pdata->ptrCtrl;

				sptrIReportCtrl	sptrReportCtrl( punk );
				if( sptrReportCtrl )
				{
					CRect rcWnd = NORECT;
					
					//if( viewMode == vmDesign )
						sptrReportCtrl->GetDesignPosition(rcWnd);
					//else
					//	sptrReportCtrl->GetPreviewPosition(rcWnd);

					if( pdata->pwnd )
					{						
						rcWnd = ConvertFromVTPixelToMM( rcWnd );
						((CWnd*)pdata->pwnd)->MoveWindow( &rcWnd );
					}
					
				}
			}
		}

		punk->Release();
	}

}


void CPrintView::Update( IUnknown* punkDelete, IUnknown* punkAdd )
{	
	//If loading or after margins and fields change
	VerifyCtrlPos( punkAdd );

	DefragPane( punkDelete );
	BuildPages( punkDelete );
	RecalcColRowCount( punkDelete );
	SortPages();
	SetPagesPos();
	SyncAXRect();	

	AddNewFieldControls();

	Invalidate();

}

//Copy ctrls from 1 page to all other which in fields area
void CPrintView::AddNewFieldControls()
{
	if( viewMode == vmDesign )
		return;
	
	if( m_page.GetSize() < 2 )
		return;



	CPage* pFirstPage = m_page[0];

	for( int iCtrl=0;iCtrl<pFirstPage->m_ctrl.GetSize();iCtrl++)
	{
		CControlInfo* pCI = pFirstPage->m_ctrl[iCtrl];
		if( pFirstPage->GetPointPlace( pFirstPage->GetClientStartPoint() + 
					CPoint( pCI->m_rectOnPage.left,
					pCI->m_rectOnPage.top)) == ppField )
		{

			for( int i=1;i<m_page.GetSize();i++)
			{
				CPage* pPageTarget = m_page[i];				
				pPageTarget->AddCtrl( pCI->m_pCtrlData, pCI->m_rectOnPage, 
					pCI->m_ctrlSize, pCI->m_ptOffset, pCI->m_bFirstEntry,
					pCI->m_sizeViewCtrl, pCI->m_sizeUserOffset,
					pCI->m_sizeUserOffsetDX);
			}			
		}
	}


}


CPoint CPrintView::FromClientToPane( CPoint point, CPage* page)
{
	CPoint pt;
	ASSERT( page != NULL );
	if( page != NULL )
	{
		CRect rcClientPage = page->GetRectPage( );	
		pt = CPoint( point.x - rcClientPage.left, 
					 point.y - rcClientPage.top	);
	}	

	return pt;
	
}



void CPrintView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( m_bDisconnectNotifyContrl )
		return;

	if( !strcmp( pszEvent, szAxCtrlPropChange ) )
	{
		if( !punkHit )			return;
		if( !_Find( punkHit ) )	return;

		InvalidateRect( 0, true );
	}

	CAxBaseView::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );

	if( !strcmp( pszEvent, szEventChangeObject ) )
	{
		

		if( GetObjectKey( punkFrom ) == GetObjectKey( m_sptrControls ) )
		{				
			int nObjects = m_pframe->GetObjectsCount();
			for( int i=0;i<nObjects;i++)
			{

				IUnknown* punkObj = m_pframe->GetObjectByIdx(i);
				if( punkObj != NULL )
				{
					CorrectCtrlPos( punkObj, i!=nObjects-1 );
					punkObj->Release();
				}			

			}

			//Update( );
			//RecalcColRowCount();
		}
		else
		{
			
			//Object change
			if( punkFrom && viewMode == vmPreview ) // if ViewAx need rebuil it
			{
				viewMode = vmDesign; 
				Update();
				EnterPreviewMode();
			}
				/*
				CString strObjectName = ::GetObjectName( punkFrom );
				if( !strObjectName.IsEmpty() )//find 
				{

					POSITION posData = m_sptrControls.GetFirstObjectPosition();
					
					while( posData )
					{
						IUnknown	*punk = m_sptrControls.GetNextObject( posData );

						POSITION	pos = _Find( punk );
						if( pos )
						{				
							ControlData *pdata = m_controlInfos.GetAt( pos );
							bool bFound = false;
							if( pdata && pdata->ptrCtrl != NULL )
							{									
								IViewCtrlPtr ptrV( pdata->ptrCtrl );
								if( ptrV )
								{
									LPOS lPos = 0;
									ptrV->GetFirstObjectPosition( &lPos );
									while( lPos )
									{
										BSTR bstrObjectName, bstrObjectType;
										BOOL bActiveObject;
										bstrObjectName = bstrObjectType = 0;

										ptrV->GetNextObject( &bstrObjectName, &bActiveObject, &bstrObjectType, &lPos );

										::SysFreeString( bstrObjectName );
										::SysFreeString( bstrObjectType );

										if( CString( bstrObjectName ) == strObjectName )
											bFound = true;
									}
								}

								if( bFound )
								{
									BOOL bResult;
									ptrV->Build( &bResult );
								}
							}
						}
						punk->Release();
					}
				}					
			}

			Invalidate();
			*/
			
		}

	}
	else if( viewMode == vmDesign && m_page.GetSize() < 1 && !strcmp( pszEvent, szActivateView ) )
	{
		BuildPages();
	}
	else if( !strcmp(pszEvent, szEventNewSettings) )
	{
		load_settings();
		Invalidate( 0 );
	}
	/*
	else if( !strcmp( pszEvent, szEventScrollPosition ) && 
		( ::GetObjectKey( GetControllingUnknown() ) == ::GetObjectKey(punkFrom) ) )
	{
		//if( m_pframe )
		//	m_pframe->Redraw( true, true );

		//Invalidate();
	}
	*/
}


void CPrintView::OnZoomChange()
{
	RecalcColRowCount( );	
	SetPagesPos();
	SyncAXRect();

}

BOOL CPrintView::OnEraseBkgnd(CDC* pDC) 
{
	/*
	if( m_bUseAnimationDraw && viewMode == vmPreview  )
	{
		CRect rcUpdate;
		GetUpdateRect( &rcUpdate );

		CDC dcImage;
		dcImage.CreateCompatibleDC( pDC );

		CBitmap Bitmap;
		Bitmap.CreateCompatibleBitmap( pDC, rcUpdate.Width() + 1, rcUpdate.Height() + 1 );
		CBitmap* pOldBitmap = dcImage.SelectObject( &Bitmap );

		COLORREF clrBack = ::GetValueColor( ::GetAppUnknown(), "PageColor", "Background", 
						::GetSysColor(COLOR_BTNSHADOW) );

		dcImage.FillRect( &rcUpdate, &CBrush(clrBack) );

		pDC->BitBlt( rcUpdate.left, rcUpdate.top, rcUpdate.Width() + 1, rcUpdate.Height() + 1,
					&dcImage, rcUpdate.left, rcUpdate.top, SRCCOPY );

		dcImage.SelectObject( pOldBitmap );

	}
	*/

	return TRUE;
	
}

#define NEW_PRINT

void CPrintView::OnPaint()
{
	//TIME_TEST( "CPrintView::OnPaint()" )
	
//paul
#ifdef NEW_PRINT
	on_paint();
#else
	CRect rcUpdate;
	GetUpdateRect( &rcUpdate );
	CPaintDC dc( this );
	_draw( &dc, rcUpdate );
#endif
}

void CPrintView::_draw( CDC *pdc, CRect rcUpdate )
{
	//m_bUseAnimationDraw = false;

	CTimeTest time(true, "Printing...:");
	time.m_bEnableFileOutput = false;		

	double fZoom = ::GetZoom( GetControllingUnknown() );


	if( fZoom > 1 )
		rcUpdate.InflateRect( long(fZoom), long(fZoom), long(fZoom), long(fZoom) );

	CRect rcClient;
	GetClientRect( &rcClient );

	CDC* pDC = 0;
	if( m_bUseAnimationDraw && viewMode == vmPreview  )
		pDC = pdc;


	CDC dcImage;
	dcImage.CreateCompatibleDC( pdc );

	CBitmap Bitmap;
	Bitmap.CreateCompatibleBitmap( pdc, rcClient.Width() + 1, rcClient.Height() + 1 );
	CBitmap* pOldBitmap = dcImage.SelectObject( &Bitmap );
	

	/*
	if( pDC != 0 )
		dc.FillRect( &rcUpdate, &CBrush(clrBack) );		
		*/
	
	dcImage.FillRect( &rcClient, &CBrush(m_clrBack) );
	

	if( pDC == 0 )
		DrawEmptyPages( &dcImage );

	DrawPagesContext( &dcImage, FALSE, pDC, &rcUpdate );

	if( m_page.GetSize() < 1 )
	{
		CString strEmptyPage;
		strEmptyPage.LoadString( IDS_REPORT_EMPTY );
		
		int nMode = dcImage.SetBkMode( TRANSPARENT );
		dcImage.TextOut( 10, 10, strEmptyPage );
		dcImage.SetBkMode( nMode );

	}

	if( pDC == 0 )
		pdc->BitBlt( rcClient.left, rcClient.top, rcClient.Width() + 1, rcClient.Height() + 1,
					&dcImage, 0, 0, SRCCOPY );

	dcImage.SelectObject( pOldBitmap );
		

	ProcessDrawing( *pdc, rcClient, 0, 0 );	

	m_pframe->Draw( *pdc, NORECT, 0, 0 );
	if( m_pframe->m_bDragDropActive )
		m_pframeDrag->Draw( *pdc, NORECT, 0, 0 );
	
}


void CPrintView::DrawRulers(CDC* pDC)
{

}

void CPrintView::DrawEmptyPage( CDC* pDC, int nNum )
{
	if( nNum < 0  || nNum >= m_page.GetSize() )
	{
		ASSERT( FALSE );
		return;
	}

	int i = nNum;

	BOOL bAdditionalPage = FALSE;
	BOOL bLastPage = FALSE;

	CPage* pPage = m_page[i];


	if( !m_page[i]->IsMainPage()) 
		bAdditionalPage = TRUE;

	if( i == m_page.GetSize()-1 )
		bLastPage = TRUE;


	CRect rcPage = m_page[i]->GetRectPage( );			

	CRect rcAlign, rcField;
	
	rcAlign = m_page[i]->GetRectAlign( );
	rcField = m_page[i]->GetRectField( );

	rcPage  = ConvertFromVTPixelToMM( rcPage );
	rcAlign = ConvertFromVTPixelToMM( rcAlign );
	rcField = ConvertFromVTPixelToMM( rcField );


	rcPage  = ConvertToWindow( rcPage );
	rcAlign = ConvertToWindow( rcAlign );
	rcField = ConvertToWindow( rcField );
	
	//Shadow
	rcPage += CPoint(2,2);
	pDC->FillRect( rcPage, &CBrush(::GetSysColor(COLOR_WINDOWTEXT)) ); 					
	rcPage -= CPoint(2,2);
	
	//Align
	pDC->FillRect( rcPage, &CBrush(m_clrAlign) ); 			
	
	//Field
	pDC->FillRect( rcAlign, &CBrush(m_clrField) ); 		
	
	//Work
	COLORREF color;
	if( bLastPage && viewMode == vmDesign )
	{
		color = m_clrFreeWork;
	}
	else
	{
		if( bAdditionalPage )
			color = m_clrAdditionalWork;
		else
			color = m_clrMainWork;
	}
	pDC->FillRect( rcField, &CBrush(color) );
	
	
	pDC->FrameRect( &rcAlign, &CBrush(::GetSysColor(COLOR_WINDOWTEXT)) );
	pDC->FrameRect( &rcField, &CBrush(::GetSysColor(COLOR_WINDOWTEXT)) );
	
	//Border
	pDC->FrameRect( &rcPage, &CBrush(::GetSysColor(COLOR_WINDOWTEXT)) );

}

void CPrintView::DrawEmptyPages(CDC* pDC)
{	
	for(int i=0;i<m_page.GetSize();i++)
		DrawEmptyPage( pDC, i );
}



BOOL CPrintView::IsVisiblePage( int nPageNum, CRect* pRCUpdate )
{
	if( viewMode == vmPrinting )
		return TRUE;


	if( nPageNum > -1 && nPageNum < m_page.GetSize() )
	{
		CRect rcClient;
		if( pRCUpdate )
			rcClient= *pRCUpdate;
		else
			GetClientRect( &rcClient );
		
		rcClient = ConvertToClient( rcClient );
		rcClient = ConvertFromMMToVTPixel( rcClient );

		CPage* pPage = m_page[nPageNum];
		
		CRect rcPage = pPage->GetRectPage();

		CRect rcIntersect;
		if( rcIntersect.IntersectRect( &rcPage, &rcClient ) )
			return TRUE;
		else
			return FALSE;
		

	}
	else
		return FALSE;
}

//cache view
////////////////////////////////////////////////////////////////////////////
void CPrintView::update_zoom_scroll()
{
	m_pt_scroll	= ::GetScrollPos( GetControllingUnknown() );
	m_fzoom		= ::GetZoom( GetControllingUnknown() );
}

////////////////////////////////////////////////////////////////////////////
CPoint CPrintView::get_scroll_pos()
{
	return m_pt_scroll;
}

////////////////////////////////////////////////////////////////////////////
double CPrintView::get_zoom()
{
	return m_fzoom;
}

//cache printer
////////////////////////////////////////////////////////////////////////////
void CPrintView::update_printer( CDC* pdc )
{
	if( !pdc )	return;

	m_lpaper_width_px		= pdc->GetDeviceCaps(HORZRES);
	m_lpaper_height_px		= pdc->GetDeviceCaps(VERTRES);

	m_lpaper_width_mm		= pdc->GetDeviceCaps(HORZSIZE);
	m_lpaper_height_mm		= pdc->GetDeviceCaps(VERTSIZE);

	m_lpaper_offset_x_px	= pdc->GetDeviceCaps( PHYSICALOFFSETX );
	m_lpaper_offset_y_px	= pdc->GetDeviceCaps( PHYSICALOFFSETY );
}

////////////////////////////////////////////////////////////////////////////
CRect CPrintView::convert_from_vt_to_prn( CRect rc_vt )
{
	CPoint pt0 = convert_from_vt_to_prn( CPoint( rc_vt.left, rc_vt.top ) );
	CPoint pt1 = convert_from_vt_to_prn( CPoint( rc_vt.right, rc_vt.bottom ) );
	CRect rc_prn( pt0, pt1 );
	return rc_prn;
}

////////////////////////////////////////////////////////////////////////////
CPoint CPrintView::convert_from_vt_to_prn( CPoint pt_vt )
{
	if( !m_lpaper_width_px || !m_lpaper_height_px ||
		!m_lpaper_width_mm || !m_lpaper_height_mm )
		return pt_vt;

	//convert to MM
	double fx = ((double)pt_vt.x) / 100.0;
	double fy = ((double)pt_vt.y) / 100.0;

	//convert to printr units
	fx = fx * (double)m_lpaper_width_px / (double)m_lpaper_width_mm;
	fy = fy * (double)m_lpaper_height_px / (double)m_lpaper_height_mm;

	//offset (cos non-printable area)
	fx -= m_lpaper_offset_x_px;
	fy -= m_lpaper_offset_y_px;

	return CPoint( long(fx), long(fy) );
}

//new drawing
////////////////////////////////////////////////////////////////////////////
void CPrintView::on_paint( )
{
	CRect rc_paint;
	GetUpdateRect( &rc_paint );

	update_zoom_scroll();

	CPoint pt_scroll	= get_scroll_pos();
	double fzoom		= get_zoom();

	CPaintDC	dc_paint( this );
	CDC			dc_memory;

	dc_memory.CreateCompatibleDC( &dc_paint );

	int nimage_width	= rc_paint.Width( )  + 1;
	int nimage_height	= rc_paint.Height( )  + 1;

	if( nimage_width < 1 || nimage_height < 1 )
		return;	

	BITMAPINFOHEADER	bmih;
	ZeroMemory( &bmih, sizeof( bmih ) );

	bmih.biBitCount = 24;	
	bmih.biWidth	= nimage_width;
	bmih.biHeight	= nimage_height;
	bmih.biSize		= sizeof( bmih );
	bmih.biPlanes	= 1;

	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection( dc_paint, (BITMAPINFO*)&bmih, DIB_RGB_COLORS, (void**)&pdibBits, 0, 0 );
	CBitmap *psrcBitmap = CBitmap::FromHandle( hDIBSection );

	CBitmap *poldBmp = dc_memory.SelectObject( psrcBitmap );

	dc_memory.SetMapMode( MM_TEXT );	

 	dc_memory.SetViewportOrg( -rc_paint.left, -rc_paint.top );
 	dc_memory.SetBrushOrg( -( rc_paint.left +  + pt_scroll.x ) % 8, -( rc_paint.top + pt_scroll.y ) % 8 );
	
	{
		//fill background
		dc_memory.FillRect( &rc_paint, &CBrush( m_clrBack ) );
		
		//process drawing
 		do_draw( &dc_memory, rc_paint );
	}

	dc_memory.SetViewportOrg( 0, 0 );

	//paint to paint DC
	::SetDIBitsToDevice( dc_paint, rc_paint.left, rc_paint.top, nimage_width, nimage_height,
		0, 0, 0, nimage_height, pdibBits, (BITMAPINFO*)&bmih, DIB_RGB_COLORS );

	ProcessDrawing( dc_paint, rc_paint, 0, 0 );	

	m_pframe->Draw( dc_paint, NORECT, 0, 0 );
	if( m_pframe->m_bDragDropActive )
		m_pframeDrag->Draw( dc_paint, NORECT, 0, 0 );

	//prepare to delete objects
	dc_memory.SelectObject( poldBmp );

	if( psrcBitmap )
		psrcBitmap->DeleteObject();	psrcBitmap = 0;
}

void CPrintView::draw_report_empty( CDC* pdc, CSize *pCalc )
{
	HFONT hDefGui = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	if (GetObject(hDefGui,sizeof(lf),&lf) != sizeof(lf))
	{
		lf.lfHeight = 10;
		strcpy(lf.lfFaceName,"Arial");
	}
	else
	{
		lf.lfHeight *= 4;
		lf.lfWidth *= 4;
	}
	lf.lfHeight = int(lf.lfHeight*m_fzoom);
	if (lf.lfWidth != 0) lf.lfWidth = int(lf.lfWidth*m_fzoom);
	CFont font;
	CFont *pOldFont = NULL;
	if (font.CreateFontIndirect(&lf))
		pOldFont = pdc->SelectObject(&font);
	CString strEmptyPage;
	if( m_sptrControls.GetFirstObjectPosition() != 0 )
		strEmptyPage.LoadString( IDS_REPORT_PAGE_EMPTY );
	else
		strEmptyPage.LoadString( IDS_REPORT_EMPTY );

	int nMode = pdc->SetBkMode( TRANSPARENT );
	if (pCalc)
	{
		CRect rc(0,0,0,0);
		pdc->DrawText(strEmptyPage, rc, DT_SINGLELINE|DT_CALCRECT);
		*pCalc = CSize(rc.Width()+80, rc.Height()+80);
	}
	else
		pdc->TextOut(int(40*m_fzoom)-m_pt_scroll.x, int(40*m_fzoom)-m_pt_scroll.y, strEmptyPage);
	pdc->SetBkMode( nMode );
	if (pOldFont != NULL)
		pdc->SelectObject(pOldFont);
}

////////////////////////////////////////////////////////////////////////////
void CPrintView::do_draw( CDC* pdc, CRect& rc_update )
{
	if( !pdc )	return;

	for( int idx_page=0;idx_page<m_page.GetSize();idx_page++ )
	{						   
		CPage* ppage	= m_page[idx_page];		
		CRect rc_page	= ppage->GetRectPage();
		if( !is_rect_visible( rc_page, rc_update ) )
			continue;

		draw_empty_page( pdc, idx_page, rc_update );		
		draw_page_content( pdc, idx_page, rc_update, false );
	}
	if( m_page.GetSize() < 1 )
		draw_report_empty(pdc);
}

////////////////////////////////////////////////////////////////////////////
void CPrintView::do_print( CDC* pdc )
{
	if( !pdc )	return;

	update_printer( pdc );

	for( int idx_page=0;idx_page<m_page.GetSize();idx_page++ )
	{						   
		CPage* ppage	= m_page[idx_page];		

		//notify print progress dialog
		CWnd* pwnd = CWnd::GetParent();
		if( pwnd && pwnd->GetSafeHwnd() )
		{
			if( pwnd->SendMessage( WM_PRINT_PROGRESS, (WPARAM)idx_page, (LPARAM)m_page.GetSize() ) == 0L )
			{
				CWnd* pwnd_dlg = pwnd->GetParent();
				if( pwnd_dlg && pwnd_dlg->GetSafeHwnd() )
				{
					pwnd_dlg->SendMessage( WM_PRINT_PROGRESS, (WPARAM)idx_page, (LPARAM)m_page.GetSize() );
					pwnd_dlg->UpdateWindow();
				}
			}
		}

		int r = ::StartPage( pdc->m_hDC );
		draw_page_content( pdc, idx_page, NORECT, true );
		EndPage( pdc->m_hDC );
	}
}

////////////////////////////////////////////////////////////////////////////
void CPrintView::draw_page_content( CDC* pdc, int idx_page, CRect& rc_update, bool bprint )
{
	if( !pdc || idx_page < 0 || idx_page >= m_page.GetSize() )	return;

	CPage* ppage = m_page[idx_page];

	for( int idx_ctrl=0;idx_ctrl<ppage->m_ctrl.GetSize();idx_ctrl++)
	{
		CControlInfo* pci = ppage->m_ctrl[idx_ctrl];
		if( !pci->m_pCtrlData )		continue;

		POSITION pos_ctrl = _Find( pci->m_pCtrlData );
		if( !pos_ctrl )				continue;

		ControlData* pdata = m_controlInfos.GetAt( pos_ctrl );
		if( pdata->ptrCtrl == 0 )	continue;

		CRect rc_vt = pci->m_rectOnPage;
		if( !bprint )
			rc_vt += ppage->m_ptClient;

		//CRect rc_wnd = convert_from_vt_to_window( rc_vt );
		//pdc->FrameRect( &rc_wnd, &CBrush(RGB(255,0,0)) );
		
		if( !bprint && !is_rect_visible( rc_vt, rc_update ) )
			continue;

		CRect rc_wnd = ( !bprint ?	convert_from_vt_to_window( rc_vt ) : 
									convert_from_vt_to_prn( rc_vt ) );

		if( !rc_wnd.Width() || !rc_wnd.Height() )
			continue;

		/*CRect rc_clip = rc_wnd;
		CRgn rgn;
		rgn.CreateRectRgn( 0, 0, rc_clip.Width() + 1, rc_clip.Height() + 1 );
		pdc->SelectClipRgn( &rgn );
		*/

		if( !bprint )
			draw_border( pdc, rc_wnd );
		
		CSize	window_ext, viewport_ext, window_ext_old, viewport_ext_old;
		CPoint	viewport_org, viewport_org_old;
		window_ext = viewport_ext = window_ext_old = viewport_ext_old = CSize( 0, 0 );
		viewport_org = viewport_org_old = CPoint( 0, 0 );

		int nsave = pdc->SaveDC( );
		
		int nold_mode = pdc->SetMapMode( MM_ANISOTROPIC );

		IPrintViewPtr ptr_view_ax = pdata->ptrCtrl;
		IDBControlPtr ptr_db = pdata->ptrCtrl;
		
		if( ptr_view_ax != 0 && !( ptr_db != 0 && viewMode == vmDesign ) )//Native view
		{
			CRect rc_item = rc_wnd;
			rc_item.OffsetRect( -rc_update.left, -rc_update.top );
			
			//need native rect, cos viewport ignore
			window_ext			= CSize( rc_item.Width(), rc_item.Height() );
			viewport_org		= CPoint( 0, 0 );
			viewport_ext		= CSize( rc_wnd.Width(), rc_wnd.Height() );

			window_ext_old		= pdc->SetWindowExt( window_ext );
			viewport_ext_old	= pdc->SetViewportExt( viewport_ext );
			viewport_org_old	= pdc->SetViewportOrg( viewport_org );

			if( pci->m_sizeUserOffsetDX.cx != 0 && 
				pci->m_sizeUserOffsetDX.cy != 0 )
			{
				ptr_view_ax->Print( pdc->GetSafeHdc(), rc_item,
					pci->m_sizeUserOffset.cx, pci->m_sizeUserOffset.cy,
					pci->m_sizeUserOffsetDX.cx, pci->m_sizeUserOffsetDX.cy, 
					TRUE,  ( m_bQuasiPrint ? 1 : !bprint )
					);
			}
			/*else
			{	
				pdc->FillRect( &rc_item, &CBrush( RGB( 240, 240, 240 ) ) );
				CPen pen(PS_SOLID, 1, RGB(0,0,0));
				CPen* pOldPen = pdc->SelectObject( &pen );
				pdc->Rectangle( &rc_item );
				pdc->DrawText( "Not Init", &rc_item, DT_LEFT );
				pdc->SelectObject( pOldPen );				
			}*/
			
		}
		else//VTDesign controls
		{
			IVTPrintCtrlPtr ptr_print_ctrl = pdata->ptrCtrl;
			if( ptr_print_ctrl )
			{
				//himetrics :(
				window_ext		= CSize( rc_vt.Width(), -rc_vt.Height() );

				viewport_ext	= CSize( rc_wnd.Width(), rc_wnd.Height() );
				viewport_org	= CPoint( rc_vt.left, rc_vt.top );
				
				viewport_org	= ( !bprint ?	convert_from_vt_to_window( viewport_org ) : 
												convert_from_vt_to_prn( viewport_org ) );
				viewport_org.x -= rc_update.left;
				viewport_org.y -= rc_update.top;
				viewport_org.y += rc_wnd.Height();

				window_ext_old		= pdc->SetWindowExt( window_ext );					
				viewport_ext_old	= pdc->SetViewportExt( viewport_ext );
				viewport_org_old	= pdc->SetViewportOrg( viewport_org );

				ptr_print_ctrl->SetPageInfo( idx_page, m_page.GetSize() );
				ptr_print_ctrl->Draw( pdc->m_hDC, CSize( rc_vt.Width(), rc_vt.Height() ) );
			}
			else//Simple OCX
			{
				//normal offsets
				window_ext		= CSize( rc_vt.Width(), rc_vt.Height() );
				viewport_ext	= CSize( rc_wnd.Width(), rc_wnd.Height() );
				viewport_org	= CPoint( rc_vt.left, rc_vt.top );

				viewport_org = ( !bprint ?	convert_from_vt_to_window( viewport_org ) : 
												convert_from_vt_to_prn( viewport_org ) );
				viewport_org.x -= rc_update.left;
				viewport_org.y -= rc_update.top;
				
				window_ext_old		= pdc->SetWindowExt( window_ext );					
				viewport_ext_old	= pdc->SetViewportExt( viewport_ext );
				viewport_org_old	= pdc->SetViewportOrg( viewport_org );


				IViewObjectPtr	ptr_ocx( pdata->ptrCtrl );
				if( ptr_ocx != 0 )
				{
					DVASPECTINFO	info = {0};
					info.cb = sizeof( info );
					info.dwFlags = DVASPECTINFOFLAG_CANOPTIMIZE;
					CRect rc_item = CRect( 0, 0, rc_vt.Width() + 1, rc_vt.Height() + 1 );
					RECTL* prect = (RECTL*)&rc_item;
					ptr_ocx->Draw( DVASPECT_CONTENT, -1, 
										0, 0, pdc->m_hAttribDC, pdc->m_hDC, 
										prect, prect, NULL, 0 );
				}
			}
		}
		pdc->SetWindowExt( window_ext_old );
		pdc->SetViewportExt( viewport_ext_old );
		pdc->SetViewportOrg( viewport_org_old );
		pdc->SetMapMode( nold_mode );

		//pdc->SelectClipRgn( 0, RGN_COPY );

		pdc->RestoreDC( nsave );
	}
	
	if( bprint && GetStealthKeysNumber() && StealthIndexIsAvailable( 30 ) )
	{
		CString str;

 		str.LoadString( GetModuleHandle( "..\\shell.exe" ), 102 );

		LOGFONT lf = {0};
		strcpy( lf.lfFaceName, "Times New Roman" );
		lf.lfHeight = -20;
		lf.lfWeight = 400;
		lf.lfItalic = 1;
		lf.lfUnderline = 0;
 		lf.lfStrikeOut = 0;
		lf.lfCharSet = DEFAULT_CHARSET;

		HFONT hFont = ::CreateFontIndirect( &lf );
		HFONT hOldFont = (HFONT)::SelectObject( pdc->m_hDC , hFont );

 		CRect _rcPageOnScreen = ppage->GetRectField( );
		_rcPageOnScreen = ConvertFromVTPixelToMM( _rcPageOnScreen );

		RECT _rc = { 0 };

   		::DrawText( pdc->m_hDC , str, str.GetLength(), &_rc, DT_LEFT | DT_TOP | DT_NOCLIP | DT_CALCRECT );

 		int n = ( _rcPageOnScreen.bottom - _rcPageOnScreen.top ) / ( _rc.bottom - _rc.top );

		_rc = ConvertToWindow( _rc );

 		double fScale = double( _rcPageOnScreen.right - _rcPageOnScreen.left ) / ( _rc.right - _rc.left );

		lf.lfHeight *= fScale;

		::SelectObject( pdc->m_hDC , hOldFont );
		::DeleteObject( hFont );

		hFont = ::CreateFontIndirect( &lf );
		hOldFont = (HFONT)::SelectObject( pdc->m_hDC , hFont );

		::SetBkMode( pdc->m_hDC, TRANSPARENT );

   		::DrawText( pdc->m_hDC , str, str.GetLength(), &_rc, DT_LEFT | DT_TOP | DT_NOCLIP | DT_CALCRECT );

 		_rcPageOnScreen = ConvertToWindow( _rcPageOnScreen );

		::SetTextColor( pdc->m_hDC, RGB( 64, 64, 64 ) );
		
		int dy = ( _rc.bottom - _rc.top );
		int dx = ( _rc.right - _rc.left );

		lf.lfHeight *= 0.5;

		::SelectObject( pdc->m_hDC , hOldFont );
		::DeleteObject( hFont );

		hFont = ::CreateFontIndirect( &lf );
		hOldFont = (HFONT)::SelectObject( pdc->m_hDC , hFont );


 		for( int j = 0; j < n; j += 3 )
		{
 			RECT rc = { _rcPageOnScreen.left, _rcPageOnScreen.top + dy * j, _rcPageOnScreen.left + dx, _rcPageOnScreen.top + dy * ( j + 1 ) };

			::DrawText( pdc->m_hDC , str, str.GetLength(), &rc, DT_CENTER | DT_VCENTER | DT_NOCLIP );
		}


		::SelectObject( pdc->m_hDC , hOldFont );
		::DeleteObject( hFont );

	}
}

////////////////////////////////////////////////////////////////////////////
void CPrintView::draw_empty_page( CDC* pdc, int idx_page, CRect& rc_update )
{
	if( !pdc || idx_page < 0 || idx_page >= m_page.GetSize() )	return;

	CPage* ppage = m_page[idx_page];
	bool blast = ( idx_page == m_page.GetSize() - 1 );

	//Is page additioonal?
	bool badditional = false;
	if( !ppage->IsMainPage()) 
		badditional = true;

	CRect rc_page	= convert_from_vt_to_window( ppage->GetRectPage( ) );
	CRect rc_align	= convert_from_vt_to_window( ppage->GetRectAlign( ) );
	CRect rc_field	= convert_from_vt_to_window( ppage->GetRectField( ) );

	//Shadow
	rc_page += CPoint(2,2);
	pdc->FillRect( rc_page, &CBrush(::GetSysColor(COLOR_WINDOWTEXT)) ); 					
	rc_page -= CPoint(2,2);
	
	//Align
	pdc->FillRect( rc_page, &CBrush(m_clrAlign) ); 			
	
	//Field
	pdc->FillRect( rc_align, &CBrush(m_clrField) ); 		
	
	//Work
	COLORREF color;
	if( blast && viewMode == vmDesign )
	{
		color = m_clrFreeWork;
	}
	else
	{
		if( badditional )
			color = m_clrAdditionalWork;
		else
			color = m_clrMainWork;
	}
	pdc->FillRect( rc_field, &CBrush(color) );
	
	
	pdc->FrameRect( &rc_align, &CBrush(::GetSysColor(COLOR_WINDOWTEXT)) );
	pdc->FrameRect( &rc_field, &CBrush(::GetSysColor(COLOR_WINDOWTEXT)) );
	
	//Border
	pdc->FrameRect( &rc_page, &CBrush(::GetSysColor(COLOR_WINDOWTEXT)) );
}

////////////////////////////////////////////////////////////////////////////
bool CPrintView::is_rect_visible( CRect& rc_vt, CRect& rc_update )
{
	CRect rc = rc_update;
	rc.InflateRect( 1, 1, 1, 1 );
	rc = convert_from_window_to_vt( rc );

	CRect rcIntersect;
	if( rcIntersect.IntersectRect( &rc, &rc_vt ) )
		return TRUE;
	else
		return FALSE;
}

////////////////////////////////////////////////////////////////////////////
CRect CPrintView::convert_from_vt_to_window( CRect rc_vt )
{
	CPoint pt0 = convert_from_vt_to_window( CPoint( rc_vt.left, rc_vt.top ) );
	CPoint pt1 = convert_from_vt_to_window( CPoint( rc_vt.right, rc_vt.bottom ) );
	CRect rc( pt0, pt1 );
	return rc;
}

////////////////////////////////////////////////////////////////////////////
CPoint CPrintView::convert_from_vt_to_window( CPoint pt_vt )
{
	//CPoint pt = ConvertFromVTPixelToMM( pt_vt );
	//pt = ConvertToWindow( pt );	

	double fppm	= GetDisplayPixelPMM();
	double fx	= (double)pt_vt.x * fppm / 100.;
	double fy	= (double)pt_vt.y * fppm / 100.;

	fx *= m_fzoom;
	fy *= m_fzoom;

	CPoint pt( (long)fx - m_pt_scroll.x, (long)fy - m_pt_scroll.y );	
	return pt;
}

////////////////////////////////////////////////////////////////////////////
CRect CPrintView::convert_from_window_to_vt( CRect rc_wnd )
{
	CPoint pt0 = convert_from_window_to_vt( CPoint( rc_wnd.left, rc_wnd.top ) );
	CPoint pt1 = convert_from_window_to_vt( CPoint( rc_wnd.right, rc_wnd.bottom ) );
	CRect rc( pt0, pt1 );
	return rc;
}

////////////////////////////////////////////////////////////////////////////
CPoint CPrintView::convert_from_window_to_vt( CPoint pt_wnd )
{
	CPoint pt = ConvertToClient( pt_wnd );
	/*CPoint pt;
	pt.x = int( (pt_wnd.x + m_pt_scroll.x) / m_fzoom );
	pt.y = int( (pt_wnd.y + m_pt_scroll.y) / m_fzoom );
	*/
	pt = ConvertFromMMToVTPixel( pt );
	return pt;
}

//EOF new drawing
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
void CPrintView::SetReportViewModeToDBControls()
{
	POSITION	pos = m_sptrControls.GetFirstObjectPosition();
	
	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );
		POSITION	posCtrl = _Find( punk );
		ControlData *pdata = NULL;

		if( posCtrl )		
			pdata = m_controlInfos.GetAt( posCtrl );

		if( pdata && pdata->ptrCtrl != NULL )
		{
			IDBControlPtr	ptrDBControl( pdata->ptrCtrl );			
			if( ptrDBControl )
				ptrDBControl->SetReportViewMode( viewMode, viewMode );

		}

		punk->Release( );
	}

}


////////////////////////////////////////////////////////////////////////////
void CPrintView::DrawPagesContext(CDC* pdcTarget, BOOL bPrint, CDC* pDCSrc, CRect* pRCUpdate )
{

	CDC* pDC = pdcTarget;
	

	//SetReportViewModeToDBControls();

struct MetaData{
	IUnknownPtr	pCtrlData;	
	IUnknownPtr	pCtrl;	
	HMETAFILE		hMeta;
	CSize			sizeMeta;
	HENHMETAFILE	hEnhMeta;
	BOOL			bEnhMetafile;
	CSize			sizeDraw;
};

	int nMetaCount = m_sptrControls.GetCount();

	if( nMetaCount < 1 )
		return;

	
	MetaData* pMeta = new MetaData[ nMetaCount ];
	
	for( int i=0;i<nMetaCount;i++)	
	{
		pMeta[i].hMeta		= NULL;
		pMeta[i].hEnhMeta	= NULL;
		pMeta[i].sizeMeta	= CSize(0,0);
		pMeta[i].bEnhMetafile = FALSE;		
		pMeta[i].sizeDraw   = CSize(0,0);
	}
	


	int nIndex = -1;

	try{


	POSITION pos = m_sptrControls.GetFirstObjectPosition();
	
	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );
		nIndex++;

		POSITION	posCtrl = _Find( punk );
		ControlData *pdata = NULL;

		if( posCtrl )		
			pdata = m_controlInfos.GetAt( posCtrl );


		sptrIReportCtrl sptrCtrl( punk );	 //Add 1.09.2000 for DBControl support
		if( posCtrl && sptrCtrl != NULL && ( (viewMode == vmDesign) || !IsViewCtrl(punk) ))
		{		

			CRect rectCtrlHIMETRICS;
			
			if( viewMode == vmDesign)
				sptrCtrl->GetDesignPosition( rectCtrlHIMETRICS );
			else
				sptrCtrl->GetPreviewPosition( rectCtrlHIMETRICS );

			HDC hAttribDC;

			CMetaFileDC mfDC;

			CSize sizeMeta;

			sizeMeta.cx = rectCtrlHIMETRICS.Width();
			sizeMeta.cy = rectCtrlHIMETRICS.Height();

			if( sizeMeta.cx <= 0 )
				sizeMeta.cx = 100;

			if( sizeMeta.cy <= 0 )
				sizeMeta.cy = 100;

			sizeMeta = ConvertFromHimetricToDisplay( sizeMeta );

			pMeta[nIndex].pCtrl = 	pdata->ptrCtrl;

			if( (pdata->ptrCtrl != NULL) && CheckInterface( pdata->ptrCtrl, IID_IVTPrintCtrl ) )
			{

				pMeta[nIndex].bEnhMetafile = TRUE;		
		
				VERIFY(mfDC.CreateEnhanced( NULL, NULL/*"d:\\1.emf"*/,
					&CRect( 0, 0, rectCtrlHIMETRICS.Width(), rectCtrlHIMETRICS.Height() ), NULL ));


				
				mfDC.SetMapMode( MM_HIMETRIC );
				mfDC.SetWindowExt( rectCtrlHIMETRICS.Width(), rectCtrlHIMETRICS.Height() );
				mfDC.SetViewportExt( sizeMeta.cx, sizeMeta.cy );
				mfDC.SetWindowOrg( 0, rectCtrlHIMETRICS.Height() );
				
				
			}
			else
			{
				pMeta[nIndex].bEnhMetafile = FALSE;
				
				VERIFY(mfDC.Create( ) );
				hAttribDC = ::OleStdCreateDC( NULL );
				mfDC.SetAttribDC( hAttribDC );				
			}



			IViewObjectPtr	ptrDrawObject( pdata->ptrCtrl );			

			if( ptrDrawObject != NULL )
			{

				DVASPECTINFO	info;
				ZeroMemory( &info, sizeof(info) );
				info.cb = sizeof( info );
				info.dwFlags = DVASPECTINFOFLAG_CANOPTIMIZE;

				CRect rcPaint = CRect( 0, 0, sizeMeta.cx, sizeMeta.cy  );

				RECTL	*prect = (RECTL*)&rcPaint;			


				if( !pMeta[nIndex].bEnhMetafile )
				{
					//int nSave = mfDC.SaveDC();
					HRESULT hr = ptrDrawObject->Draw( DVASPECT_CONTENT, -1, 
										NULL, NULL, mfDC.m_hAttribDC, mfDC.m_hDC, 
										prect, prect, NULL, 0 );

					if( hr ) TRACE( "Drawing problem\n" );							

					mfDC.SetAttribDC( NULL );
					DeleteDC( hAttribDC );						
					pMeta[nIndex].hMeta		= mfDC.Close();

				}
				else
				{
					sptrIVTPrintCtrl pVTPrnCtrl( pdata->ptrCtrl );						
					

					if( pVTPrnCtrl != NULL )
					{
						//pVTPrnCtrl->SetPageInfo( );
						CRect rcPaint = CRect( 0, 0, sizeMeta.cx, sizeMeta.cy  );

						/*
						if( !bPrint && m_bShowBorderAroundCtrl )
							draw_border( mfDC.m_hDC, 
								CRect( 0, 0, rectCtrlHIMETRICS.Width(), rectCtrlHIMETRICS.Height() ) );
						*/

						pVTPrnCtrl->Draw( mfDC.m_hDC, //rcPaint, 
								CSize( rectCtrlHIMETRICS.Width(), rectCtrlHIMETRICS.Height() ) );
					}
					
					pMeta[nIndex].hEnhMeta		= mfDC.CloseEnhanced();
					pMeta[nIndex].sizeDraw		= 
						CSize( rectCtrlHIMETRICS.Width(), rectCtrlHIMETRICS.Height() );

					
				}

				
				pMeta[nIndex].sizeMeta	= sizeMeta;					
				pMeta[nIndex].pCtrlData	= punk;
				
			}
		}

		punk->Release();
	}



	for( int iPage=0;iPage<m_page.GetSize();iPage++)
	{
		if( m_bQuasiPrint || IsVisiblePage( iPage, pRCUpdate ) )
		{
			CPage* pPage = m_page[iPage];


			if( pDCSrc != NULL )//Anim draw
				DrawEmptyPage( pDC, iPage );

			if( bPrint )
			{
				CWnd* pWnd;
				pWnd = CWnd::GetParent();

				if( pWnd && pWnd->GetSafeHwnd() )
				{
					if( pWnd->SendMessage( WM_PRINT_PROGRESS, (WPARAM)iPage, (LPARAM)m_page.GetSize() ) == 0L )
					{
						CWnd* pWndDlg = pWnd->GetParent();
						if( pWndDlg && pWndDlg->GetSafeHwnd() )
						{
							pWndDlg->SendMessage( WM_PRINT_PROGRESS, (WPARAM)iPage, (LPARAM)m_page.GetSize() );
						}				
					}
				}

			}


			if( bPrint )
			{					
				::StartPage( pDC->m_hDC );
			}


			
			for( int iCtrl=0;iCtrl<pPage->m_ctrl.GetSize();iCtrl++)
			{
				CControlInfo* pCI = pPage->m_ctrl[iCtrl];
													 //Add 1.09.2000 for DBControl support
			  if( !IsViewCtrl( pCI->m_pCtrlData ) || (viewMode == vmDesign) )
			  {


				for( int iMeta=0;iMeta<nMetaCount;iMeta++)
				{				

					
					if( CheckInterface(pMeta[iMeta].pCtrl,  IID_IVTPrintCtrl) ) // we must rebuild metafile
					{
						sptrIVTPrintCtrl pVTPrnCtrl( pMeta[iMeta].pCtrl );
						if( pVTPrnCtrl )
						{
							BOOL bNeedUpdate = FALSE;
							ISOK( pVTPrnCtrl->NeedUpdate( &bNeedUpdate ) );
							if( bNeedUpdate )
							{
								::DeleteMetaFile( pMeta[iMeta].hMeta );
								::DeleteEnhMetaFile( pMeta[iMeta].hEnhMeta );

								CMetaFileDC mfDC;
								VERIFY(mfDC.CreateEnhanced( NULL, NULL,
									&CRect( 0, 0, pMeta[iMeta].sizeDraw.cx, 
									pMeta[iMeta].sizeDraw.cy ), NULL ));

								
								mfDC.SetMapMode( MM_HIMETRIC );
								mfDC.SetWindowExt( pMeta[iMeta].sizeDraw.cx, pMeta[iMeta].sizeDraw.cy );
								mfDC.SetViewportExt(pMeta[iMeta].sizeMeta.cx, pMeta[iMeta].sizeMeta.cy );
								mfDC.SetWindowOrg( 0, pMeta[iMeta].sizeDraw.cy );

								sptrIVTPrintCtrl pVTPrnCtrl( pMeta[iMeta].pCtrl );						

								ISOK( pVTPrnCtrl->SetPageInfo( iPage, m_page.GetSize() ) );
								
								/*
								if( !bPrint && m_bShowBorderAroundCtrl )
								{
									draw_border( mfDC.m_hDC, 
										CRect( 0, 0, pMeta[iMeta].sizeDraw.cx, pMeta[iMeta].sizeDraw.cy ) );
								}
								*/

								pVTPrnCtrl->Draw( mfDC.m_hDC, //rcPaint, 
										CSize( pMeta[iMeta].sizeDraw.cx, pMeta[iMeta].sizeDraw.cy ) );

								HENHMETAFILE hMF = mfDC.CloseEnhanced();
								pMeta[iMeta].hEnhMeta		= hMF;

							}
						}
						

					}


					if( pMeta[iMeta].pCtrlData == pCI->m_pCtrlData )
					{
						
						int nSave = pDC->SaveDC();					

						int nMode = pDC->SetMapMode( MM_ANISOTROPIC );

						//In millimeters
						CSize sizeWindowExt;

						//In device unit
						CSize sizeViewPortExt;

						CPoint ptWindowOrg;
						CPoint ptViewPortOrg;


						/////////////////////////////////////
						sizeWindowExt = CSize( pMeta[iMeta].sizeMeta.cx,
													pMeta[iMeta].sizeMeta.cy 
													);					

						if( bPrint )
						{
							//sizeWindowExt.cx /= GetDisplayPixelPMM();
							//sizeWindowExt.cy /= GetDisplayPixelPMM();
						}
						/////////////////////////////////////
						if( !bPrint )
						{
							sizeViewPortExt = sizeWindowExt;
							sizeViewPortExt = ConvertToWindow( sizeWindowExt );
							//@@@@@@@@@@
							//sizeViewPortExt.cx *= GetDisplayPixelPMM();
							//sizeViewPortExt.cy *= GetDisplayPixelPMM();						
							
						}
						else
						{
							double fPaperWidthPx   = (double)pDC->GetDeviceCaps(HORZRES);
							double fPaperHeightPx  = (double)pDC->GetDeviceCaps(VERTRES);

							double fPaperWidthMM   = (double)pDC->GetDeviceCaps(HORZSIZE) * GetDisplayPixelPMM();
							double fPaperHeightMM  = (double)pDC->GetDeviceCaps(VERTSIZE) * GetDisplayPixelPMM();

							
							sizeViewPortExt.cx = long( ((double)sizeWindowExt.cx) / 
													fPaperWidthMM * fPaperWidthPx );
							sizeViewPortExt.cy = long( ((double)sizeWindowExt.cy) / 
													fPaperHeightMM * fPaperHeightPx );
						}

						/////////////////////////////////////
						if( !bPrint )
						{
							ptWindowOrg.x = pCI->m_rectOnPage.left + pPage->GetClientStartPoint().x
									- pCI->m_ptOffset.x;
							ptWindowOrg.y = pCI->m_rectOnPage.top + pPage->GetClientStartPoint().y
									- pCI->m_ptOffset.y;

							ptWindowOrg = ConvertFromVTPixelToMM( ptWindowOrg );						
							ptViewPortOrg =  ConvertToWindow( ptWindowOrg );
						}
						else
						{

							ptWindowOrg.x = pCI->m_rectOnPage.left 
									- pCI->m_ptOffset.x -m_rectPrinterMagins.left * 100;
							ptWindowOrg.y = pCI->m_rectOnPage.top 
									- pCI->m_ptOffset.y-m_rectPrinterMagins.top * 100;

							ptWindowOrg = ConvertFromVTPixelToMM( ptWindowOrg );						



							double fPaperWidthPx   = (double)pDC->GetDeviceCaps(HORZRES);
							double fPaperHeightPx  = (double)pDC->GetDeviceCaps(VERTRES);

							double fPaperWidthMM   = (double)pDC->GetDeviceCaps(HORZSIZE) * GetDisplayPixelPMM();
							double fPaperHeightMM  = (double)pDC->GetDeviceCaps(VERTSIZE) * GetDisplayPixelPMM();

							ptViewPortOrg = CPoint(
								long( double(fPaperWidthPx)/fPaperWidthMM
								//((double)sizeViewPortExt.cx) / ((double)sizeWindowExt.cx)
								* ptWindowOrg.x ), 
								//((double)sizeViewPortExt.cy) / ((double)sizeWindowExt.cy)
								long( double(fPaperHeightPx)/fPaperHeightMM
								* ptWindowOrg.y  )
								);

								ptViewPortOrg.x -= long( m_rectPrinterMagins.left *
										fPaperWidthPx / fPaperWidthMM / GetDisplayPixelPMM() );
								ptViewPortOrg.y -= long( m_rectPrinterMagins.top *
										fPaperHeightPx / fPaperHeightMM / GetDisplayPixelPMM() );


						}

						pDC->SetWindowExt( sizeWindowExt );					
						pDC->SetViewportExt( sizeViewPortExt );
						pDC->SetViewportOrg( ptViewPortOrg );
						

						//In mm   
						CRect rcPrint;

						//if( !bPrint )
						{	

							rcPrint.left = pCI->m_ptOffset.x;
							rcPrint.top  = pCI->m_ptOffset.y;

							rcPrint.right   = rcPrint.left + pCI->m_rectOnPage.Width();
							rcPrint.bottom  = rcPrint.top  + pCI->m_rectOnPage.Height();
							
							rcPrint = ConvertFromVTPixelToMM( rcPrint );
							//@@@@@@@@@@
							//rcPrint.left /= GetDisplayPixelPMM(); 
							//rcPrint.top /= GetDisplayPixelPMM();
							//rcPrint.right /= GetDisplayPixelPMM();
							//rcPrint.bottom /= GetDisplayPixelPMM();
							
						}

						
						CRgn rgnPrint;							
						
						pDC->BeginPath();
						pDC->MoveTo( rcPrint.left, rcPrint.top );
						pDC->LineTo( rcPrint.right, rcPrint.top );
						pDC->LineTo( rcPrint.right, rcPrint.bottom );
						pDC->LineTo( rcPrint.left, rcPrint.bottom );
						pDC->LineTo( rcPrint.left, rcPrint.top );
						pDC->EndPath();
						rgnPrint.CreateFromPath( pDC );


						pDC->SelectClipRgn( &rgnPrint );							

						if( !bPrint && m_bShowBorderAroundCtrl )
							draw_border( pDC, rcPrint );


						if( pMeta[iMeta].bEnhMetafile )
						{
							pDC->PlayMetaFile( pMeta[iMeta].hEnhMeta, 
									CRect( 0, 0, sizeWindowExt.cx, sizeWindowExt.cy ) );
							/*
							CClientDC dc(0);
							dc.PlayMetaFile( pMeta[iMeta].hEnhMeta, 
									CRect( 0, 0, sizeWindowExt.cx, sizeWindowExt.cy ) );
									*/


						}
						else
							pDC->PlayMetaFile( pMeta[iMeta].hMeta );



						pDC->SelectClipRgn( NULL );
						
						rgnPrint.DeleteObject();
						
						
						pDC->SetMapMode( nMode );

						pDC->RestoreDC( nSave );

					}
				}
			  }
			  else
			  {
			  		int nSaveDC = pDC->SaveDC();

					POSITION	posCtrl = _Find( pCI->m_pCtrlData );
					ControlData *pdata = NULL;

					if( posCtrl )		
						pdata = m_controlInfos.GetAt( posCtrl );

					if( pdata && pdata->ptrCtrl != NULL )
					{
						sptrIPrintView viewCtrl( pdata->ptrCtrl );
						if( viewCtrl )
						{
							if( !bPrint )
							{
								/*
								CRect rcUpdate;
								GetClientRect( &rcUpdate );
								
								rcUpdate = ConvertToClient( rcUpdate );
								rcUpdate = ConvertFromMMToVTPixel( rcUpdate );

								
								CRect rcIntersect;
								if( rcIntersect.IntersectRect( &pCI->m_rectOnPage, &rcUpdate ) )
								*/
								{									

									CPoint ptStart = pCI->m_rectOnPage.TopLeft() +
										pPage->GetClientStartPoint();

									ptStart = ConvertFromVTPixelToMM( ptStart );

									CSize sizeCtrl = pCI->m_sizeViewCtrl;

									CRect rcTarget = CRect( ptStart.x, ptStart.y,
											ptStart.x + sizeCtrl.cx, ptStart.y + sizeCtrl.cy
											);

									rcTarget = ConvertToWindow( rcTarget );	

									if( !bPrint && m_bShowBorderAroundCtrl )
										draw_border( pDC, rcTarget );

									 
									if( pCI->m_sizeUserOffsetDX.cx != 0 && 
										pCI->m_sizeUserOffsetDX.cy != 0 )
									{
										/*
										if( !bPrint && m_bShowBorderAroundCtrl )
											draw_border( pDC->GetSafeHdc(), rcTarget );
										*/

										viewCtrl->Print( pDC->GetSafeHdc(), rcTarget, 
											pCI->m_sizeUserOffset.cx, pCI->m_sizeUserOffset.cy,
											pCI->m_sizeUserOffsetDX.cx, pCI->m_sizeUserOffsetDX.cy, TRUE,  ( m_bQuasiPrint ? 1 : !bPrint )
											);
									}
									else
									{	
										pDC->FillRect( &rcTarget, &CBrush( RGB( 240, 240, 240 ) ) );
										CPen pen(PS_SOLID, 1, RGB(0,0,0));
										CPen* pOldPen = pDC->SelectObject( &pen );
										pDC->Rectangle( &rcTarget );
										pDC->DrawText( "Not Init", &rcTarget, DT_LEFT );
										pDC->SelectObject( pOldPen );
									}
								
								}

								   
							}
							else
							{								
									
									//int nMode = pDC->SetMapMode( MM_ANISOTROPIC );

									double fPaperWidthPx   = (double)pDC->GetDeviceCaps(HORZRES);
									double fPaperHeightPx  = (double)pDC->GetDeviceCaps(VERTRES);

									double fPaperWidthMM   = 
										(double)(pDC->GetDeviceCaps(HORZSIZE) ) * GetDisplayPixelPMM();
									double fPaperHeightMM  = 
										(double)(pDC->GetDeviceCaps(VERTSIZE) ) * GetDisplayPixelPMM();

									CSize sizeWindowExt = CSize( long(fPaperWidthMM), long(fPaperHeightMM) );

									CSize sizeViewPortExt = CSize( long(fPaperWidthPx), long(fPaperHeightPx) );

									CPoint ptWindowOrg = CPoint( 
										-m_rectPrinterMagins.left * 100,
										-m_rectPrinterMagins.top * 100 );

									ptWindowOrg  = ConvertFromVTPixelToMM( ptWindowOrg );

									CPoint ptViewPortOrg;

									ptViewPortOrg.x = long( ptWindowOrg.x / fPaperWidthMM * fPaperWidthPx );
									ptViewPortOrg.y = long( ptWindowOrg.y / fPaperHeightMM * fPaperHeightPx );

									
									//pDC->SetWindowExt( sizeWindowExt );					
									//pDC->SetViewportExt( sizeViewPortExt );
									//pDC->SetViewportOrg( ptViewPortOrg );

  
									CRect rcTarget;
									rcTarget.left = ConvertFromVTPixelToMM (pCI->m_rectOnPage.left );
									rcTarget.top  = ConvertFromVTPixelToMM (pCI->m_rectOnPage.top );
									rcTarget.right  = rcTarget.left + pCI->m_sizeViewCtrl.cx;
									rcTarget.bottom = rcTarget.top  + pCI->m_sizeViewCtrl.cy;

									
									rcTarget.left *= ( fPaperWidthPx/fPaperWidthMM );
									rcTarget.top  *= ( fPaperHeightPx/fPaperHeightMM );
									rcTarget.right  *= ( fPaperWidthPx/fPaperWidthMM );
									rcTarget.bottom *= ( fPaperHeightPx/fPaperHeightMM );

									rcTarget.left  += long( ptViewPortOrg.x );
									rcTarget.right += long( ptViewPortOrg.x );

									rcTarget.top    += long( ptViewPortOrg.y );
									rcTarget.bottom += long( ptViewPortOrg.y );
									

									/*CPen pen(PS_SOLID, 1, RGB(0,0,0));
									pDC->SelectObject( &pen );
									pDC->Rectangle( &rcTarget );
									*/

									if( pCI->m_sizeUserOffsetDX.cx != 0 && 
										pCI->m_sizeUserOffsetDX.cy != 0 )
									{
										viewCtrl->Print( pDC->GetSafeHdc(), rcTarget, 
											pCI->m_sizeUserOffset.cx, pCI->m_sizeUserOffset.cy,
											pCI->m_sizeUserOffsetDX.cx, pCI->m_sizeUserOffsetDX.cy, TRUE, ( m_bQuasiPrint ? 1 : !bPrint )
											);
									}
									else
									{
										pDC->FillRect( &rcTarget, &CBrush( RGB( 240, 240, 240 ) ) );
										CPen pen(PS_SOLID, 1, RGB(0,0,0));
										CPen* pOldPen = pDC->SelectObject( &pen );
										//pDC->Rectangle( &rcTarget );
										//pDC->DrawText( "Not Init", &rcTarget, DT_LEFT );
										pDC->SelectObject( pOldPen );

									}

									//pDC->SetMapMode( nMode );								
									
							}
						}
					}					  					  

					pDC->RestoreDC( nSaveDC );
			  }
				
			}


			if( pDCSrc != NULL )//Anim draw
			{
				CRect _rcPageOnScreen = pPage->GetRectPage( );
				_rcPageOnScreen = ConvertFromVTPixelToMM( _rcPageOnScreen );
				_rcPageOnScreen = ConvertToWindow( _rcPageOnScreen );

				pDCSrc->BitBlt( _rcPageOnScreen.left, _rcPageOnScreen.top, 
								_rcPageOnScreen.Width() + 1, _rcPageOnScreen.Height() + 1,
								pDC,
								 _rcPageOnScreen.left, _rcPageOnScreen.top, SRCCOPY );

			 if( bPrint && GetStealthKeysNumber() && StealthIndexIsAvailable( 30 ) )
			{
				CString str;

 				str.LoadString( GetModuleHandle( "..\\shell.exe" ), 102 );

				LOGFONT lf = {0};
				strcpy( lf.lfFaceName, "Times New Roman" );
				lf.lfHeight = -20;
				lf.lfWeight = 400;
				lf.lfItalic = 1;
				lf.lfUnderline = 0;
 				lf.lfStrikeOut = 0;
				lf.lfCharSet = DEFAULT_CHARSET;

				HFONT hFont = ::CreateFontIndirect( &lf );
				HFONT hOldFont = (HFONT)::SelectObject( pDC->m_hDC , hFont );

 				CRect _rcPageOnScreen = pPage->GetRectField( );
				_rcPageOnScreen = ConvertFromVTPixelToMM( _rcPageOnScreen );

				RECT _rc = { 0 };

   				::DrawText( pDC->m_hDC , str, str.GetLength(), &_rc, DT_LEFT | DT_TOP | DT_NOCLIP | DT_CALCRECT );

 				int n = ( _rcPageOnScreen.bottom - _rcPageOnScreen.top ) / ( _rc.bottom - _rc.top );

				_rc = ConvertToWindow( _rc );

 				double fScale = double( _rcPageOnScreen.right - _rcPageOnScreen.left ) / ( _rc.right - _rc.left );

				lf.lfHeight *= fScale;

				::SelectObject( pDC->m_hDC , hOldFont );
				::DeleteObject( hFont );

				hFont = ::CreateFontIndirect( &lf );
				hOldFont = (HFONT)::SelectObject( pDC->m_hDC , hFont );

				::SetBkMode( pDC->m_hDC, TRANSPARENT );

   				::DrawText( pDC->m_hDC , str, str.GetLength(), &_rc, DT_LEFT | DT_TOP | DT_NOCLIP | DT_CALCRECT );

 				_rcPageOnScreen = ConvertToWindow( _rcPageOnScreen );

				::SetTextColor( pDC->m_hDC, RGB( 64, 64, 64 ) );
				
				int dy = ( _rc.bottom - _rc.top );
				int dx = ( _rc.right - _rc.left );

				lf.lfHeight *= 0.5;

				::SelectObject( pDC->m_hDC , hOldFont );
				::DeleteObject( hFont );

				hFont = ::CreateFontIndirect( &lf );
				hOldFont = (HFONT)::SelectObject( pDC->m_hDC , hFont );


 				for( int j = 0; j < n; j += 3 )
				{
 					RECT rc = { _rcPageOnScreen.left, _rcPageOnScreen.top + dy * j, _rcPageOnScreen.left + dx, _rcPageOnScreen.top + dy * ( j + 1 ) };

					::DrawText( pDC->m_hDC , str, str.GetLength(), &rc, DT_CENTER | DT_VCENTER | DT_NOCLIP );
				}


				::SelectObject( pDC->m_hDC , hOldFont );
				::DeleteObject( hFont );

			}

			}


			if( bPrint )
			{	
				EndPage( pDC->m_hDC );				
			}

		}
	}


	}
	catch(...)
	{
		TRACE("\nPrint error...");
	}


	for( i=0;i<nMetaCount;i++)	
	{
		if( pMeta[i].hMeta )
			::DeleteMetaFile( pMeta[i].hMeta );

		if( pMeta[i].hEnhMeta )
			::DeleteEnhMetaFile( pMeta[i].hEnhMeta );
	}


	delete []pMeta;

		
				 
}


void CPrintView::draw_border( CDC* pdc, CRect rc_border )
{
	if( m_bReadOnly )
		return;

	rc_border.DeflateRect( 0, 0, 1, 1 );

	CPen pen( PS_DOT, 0, m_clrBorderAroundCtrl );
	CPen* pOldPen = pdc->SelectObject( &pen );
	
	pdc->MoveTo( rc_border.left, rc_border.top );
	pdc->LineTo( rc_border.right, rc_border.top );
	pdc->LineTo( rc_border.right, rc_border.bottom );
	pdc->LineTo( rc_border.left, rc_border.bottom );
	pdc->LineTo( rc_border.left, rc_border.top );

	//pdc->Rectangle( &rc_border );	

	pdc->SelectObject( pOldPen );
}




CPoint CPrintView::ConvertFromMMToVTPixel( CPoint point)
{
	return CPoint( ConvertFromMMToVTPixel(point.x), 
					ConvertFromMMToVTPixel(point.y) );
}

CRect CPrintView::ConvertFromMMToVTPixel( CRect rect)
{
	return CRect( ConvertFromMMToVTPixel(CPoint(rect.left, rect.top)),
				  ConvertFromMMToVTPixel(CPoint(rect.right, rect.bottom))
				  );
}

CPoint CPrintView::ConvertFromVTPixelToMM( CPoint point)
{
	return CPoint( ConvertFromVTPixelToMM(point.x), 
					ConvertFromVTPixelToMM(point.y) );
}

CRect CPrintView::ConvertFromVTPixelToMM( CRect rect)
{
	return CRect( ConvertFromVTPixelToMM(CPoint(rect.left, rect.top)),
				  ConvertFromVTPixelToMM(CPoint(rect.right, rect.bottom))
				  );

}

void CPrintView::_AddAllControls()
{
	m_bIsLoadingProcess = TRUE;
	
	//TIME_TEST( "CPrintView::_AddAllControls()" );

	BuildContext();
	CAxBaseView::_AddAllControls();
	m_bIsLoadingProcess = FALSE;
	SetReportViewModeToDBControls();
	
	OnPrinterSettingsChange( false );	

}

void CPrintView::_RemoveAllControls()
{
	CAxBaseView::_RemoveAllControls();
	DeleteContext();
}

CRect CPrintView::GetObjectRect( IUnknown *punkObject )
{
	if( !CheckInterface( punkObject, IID_IActiveXCtrl ) )
		return NORECT;

	CRect	rect;
	sptrIActiveXCtrl	sptrControl( punkObject );
	sptrControl->GetRect( &rect );
	rect += Offset();
	rect.InflateRect( 5, 5 );

	return rect;
	
}


void CPrintView::SetObjectOffset( IUnknown *punkObject, CPoint pointOffest )
{

	CPoint ptCursor;
	::GetCursorPos( &ptCursor );
	
	ScreenToClient( &ptCursor );
	ptCursor = ConvertToClient( ptCursor );
	ptCursor = ConvertFromMMToVTPixel( ptCursor );
	

	CRect rcCtrl = GetObjectRect( punkObject );	
	rcCtrl.DeflateRect( 5, 5 );
	rcCtrl = ConvertFromMMToVTPixel( rcCtrl );

	CPoint ptOffset = ConvertFromMMToVTPixel( pointOffest );

	int nPageNum = -1;

	for(int i=0;i<m_page.GetSize();i++)	
	{
		if( m_page[i]->GetRectPage( ).PtInRect(ptCursor) )
		{
			nPageNum = i;
			//nPrevPage = nPageNum;
		}
	}
	/*		
	if( nPageNum == -1 )
		nPageNum = nPrevPage;
		*/

	int nDelta = 100;
	if( nPageNum > -1 && nPageNum < m_page.GetSize() )//Page was found
	{
		CRect rcPage  = m_page[nPageNum]->GetRectPage();
		CRect rcAlign = m_page[nPageNum]->GetRectAlign();
		CRect rcField = m_page[nPageNum]->GetRectField();

		CPoint ptCursorPane = FromClientToPane( ptCursor, m_page[nPageNum] );
		/*
		//Update pane
		{
			int nXDelta, nYDelta;
			nYDelta = nXDelta = 0;

			CRect rcCtrl = NORECT;
			IReportCtrlPtr ptrCtrl( punkObject );
			if( ptrCtrl )
				ptrCtrl->GetDesignPosition( &rcCtrl );

			if( g_rcCtrl = NORECT )
			{
				g_rcCtrl = rcCtrl;
			}

			nXDelta = V2D( g_rcCtrl.left ) - V2D(rcCtrl.left);// + V2D(ptOffset.x);
			nYDelta = V2D( g_rcCtrl.top  ) - V2D(rcCtrl.top);//  + V2D(ptOffset.y);

			CString strRes;
			strRes.Format( "x:%s%d, y:%s%d",
							nXDelta < 0 ? (const char*)"" : (const char*)"+", nXDelta,
							nYDelta < 0 ? (const char*)"" : (const char*)"+", nYDelta
							);
			
			UpdateCtrlPane( strRes );

		}
		*/

		PointPlace pp = GetPointPlace( ptCursorPane );
		if( pp != ppWork )
		{

			if( ptOffset.x < rcAlign.left - nDelta )
				ptOffset.x = rcAlign.left - nDelta;

			if( ptOffset.y < rcAlign.top - nDelta )
				ptOffset.y = rcAlign.top - nDelta;

			if( ptOffset.x + rcCtrl.Width() + 2 * nDelta >= rcAlign.right)
				ptOffset.x = rcAlign.right - rcCtrl.Width() - 2 * nDelta;			

			if( ptOffset.y + rcCtrl.Height() + 2 * nDelta  >= rcAlign.bottom + nDelta )			
				ptOffset.y = rcAlign.bottom - rcCtrl.Height() - 2 * nDelta;


		}			

		if( pp == ppWork )
		{
			if( ptOffset.x < rcField.left - nDelta )
				ptOffset.x = rcField.left - nDelta;

			if( ptOffset.y < rcField.top - nDelta )
				ptOffset.y = rcField.top - nDelta;

			if( ptOffset.x + rcCtrl.Width() + nDelta >= rcField.right)
				ptOffset.x = rcField.right - rcCtrl.Width() - 2 * nDelta;			

			if( ptOffset.y + rcCtrl.Height() + nDelta  >= rcField.bottom )			
				ptOffset.y = rcField.bottom - rcCtrl.Height() - 2 * nDelta;
		}
	}

		
	pointOffest = ConvertFromVTPixelToMM( ptOffset );


	CAxBaseView::SetObjectOffset( punkObject, pointOffest );

}
/*
bool CPrintView::DoDrag( CPoint point )
{

	CPoint pt = ConvertToClient( point );
		
	int nPageNum = -1;

	pt = ConvertFromMMToVTPixel( pt );
	
	
	for(int i=0;i<m_page.GetSize();i++)
	{		
		if( m_page[i]->GetRectPage( ).PtInRect( pt ) )
		{
			nPageNum = i;
		}
	}


	if( nPageNum > -1 )//Page was found
	{
		CPoint ptDesign = FromClientToPane( pt,	m_page[nPageNum] );
		PointPlace pp = GetPointPlace( ptDesign );
		if( pp == ppField )
			TRACE("\nField");
		else
			TRACE("\nWork");
		
	}


	bool bResult = CAxBaseView::DoDrag( point );
	CString str;
	str.Format( "%d, %d\n", pt.x, pt.y );
	TRACE( str );
	return bResult;
	
}
*/


BOOL CPrintView::PreTranslateMessage(MSG* pMsg) 
{
	return CAxBaseView::PreTranslateMessage(pMsg);
}



void CPrintView::_AddControl( IUnknown *punkControl )
{	
	CAxBaseView::_AddControl( punkControl );

	POSITION	pos = _Find( punkControl );

	if( !pos )
		return;

	ControlData *pdata = m_controlInfos.GetAt( pos );

	if( pdata )
	{
		CWnd* pWnd = (CWnd*)pdata->pwnd;
		if( pWnd )
		{
			//pWnd->MoveWindow( CRect( 0, 0, 300, 300 ) );
			//::ShowWindow( pWnd->GetSafeHwnd(), SW_HIDE );

	
			{
			IVtActiveXCtrlPtr	sptrVTC( pdata->ptrCtrl );
			if( sptrVTC )
				sptrVTC->SetSite( GetAppUnknown(), GetControllingUnknown() );
			}
			
			IVtActiveXCtrl2Ptr	sptrVTC(pdata->ptrCtrl);//pwnd->GetControlSite()->m_pObject);
			if(sptrVTC != 0)
				sptrVTC->SetApp(GetAppUnknown());

			IAXCtrlDataSitePtr ptr_ds( pdata->ptrCtrl );
			if( ptr_ds )
				ptr_ds->SetData( punkControl );
		}

	}

	OnCtrlPosChange( punkControl );

}

void CPrintView::_RemoveControl( IUnknown *punkControl )
{
	for(int i=0;i<m_page.GetSize();i++)
		m_page[i]->RemoveCtrl( punkControl );	

	CAxBaseView::_RemoveControl( punkControl );

	if( IsRemoveCtrlInProgress() )
		return;

	if( viewMode == vmPreview ) 
		CalcCtrlPreviewPos();		


	m_bRemoveControl = true;
	Update( punkControl );	
	m_bRemoveControl = false;
}

void  CPrintView::_UpdateLayout( bool bRepositionControls )
{
	CAxBaseView::_UpdateLayout( bRepositionControls );	
	RecalcColRowCount();
	//SetNewScrollSize();
}

///////////////////////////////////////////////////
void CPrintView::OnPrinterSettingsChange( bool bCorrectCtrlPos )
{	

	CRect rcPrnMargins = g_prnSetup.GetPrinterMargins( m_sptrControls );

	m_rectPrinterMagins = rcPrnMargins;

	sptrIReportForm	sptrForm( m_sptrControls );
	if( sptrForm == NULL )
		return;

	CRect paperAlignNew = CRect( 
				FROM_DOUBLE_TO_VTPIXEL(rcPrnMargins.left),
				FROM_DOUBLE_TO_VTPIXEL(rcPrnMargins.top),
				FROM_DOUBLE_TO_VTPIXEL(rcPrnMargins.right),
				FROM_DOUBLE_TO_VTPIXEL(rcPrnMargins.bottom)
				);

	CRect paperAlign;
	sptrForm->GetPaperAlign( &paperAlign );	

	CRect paperField;
	sptrForm->GetPaperField( &paperField );	

	BOOL bNeedCorrectField = FALSE;

	if( paperAlign != paperAlignNew )
	{

		BOOL bNeedAsk = FALSE;
		//Verify new aligns
		POSITION	pos = m_sptrControls.GetFirstObjectPosition();

	
		while( pos )
		{
			IUnknown	*punk = m_sptrControls.GetNextObject( pos );		
			if( !punk )
				continue;

			sptrIReportCtrl sptrCtrl( punk );
			punk->Release();	punk = 0;

			if( sptrCtrl != NULL )
			{	
				CRect rcCtrl;
				sptrCtrl->GetDesignPosition( &rcCtrl );
				PointPlace pp = GetPointPlace( CPoint(rcCtrl.left, rcCtrl.top) );
				if( pp == ppAlign || pp == ppOutside )
					bNeedAsk = TRUE;
			}
		}


		CString strMessage;
		strMessage.LoadString( IDS_MARGINS_CHANGE );

		if( bNeedAsk && !::GetValueInt(::GetAppUnknown(), "\\Print", "AutoSaveMargins", FALSE))
		{
			if( AfxMessageBox( strMessage, MB_YESNO | MB_ICONQUESTION) != IDYES )
				return;
		}
		{	
			if( paperField.left < paperAlignNew.left || 
				paperField.top < paperAlignNew.top || 
				paperField.right > paperAlignNew.right || 
				paperField.bottom > paperAlignNew.bottom 
				)
			{
				bNeedCorrectField = TRUE;
			}

			CRect  NewPaperField = CRect( 
				FROM_DOUBLE_TO_VTPIXEL(rcPrnMargins.left + 10),
				FROM_DOUBLE_TO_VTPIXEL(rcPrnMargins.top + 10),
				FROM_DOUBLE_TO_VTPIXEL(rcPrnMargins.right + 10),
				FROM_DOUBLE_TO_VTPIXEL(rcPrnMargins.bottom + 10)
				);

			sptrForm->SetPaperField( NewPaperField );	

			sptrForm->SetPaperAlign( paperAlignNew );	
			//CalcCtrlPreviewPos();
			//Update( );	
		}
	}

	
	if( bCorrectCtrlPos && GetSafeHwnd() )
	{
		POSITION posData = m_sptrControls.GetFirstObjectPosition();
		
		while( posData )
		{
			IUnknown	*punk = m_sptrControls.GetNextObject( posData );
			if( !punk )
				continue;

			CorrectCtrlPos( punk, posData != 0 );

			punk->Release();	punk = 0;
		}		
	}	

}


bool CPrintView::OnDocumentPrint( bool bpreview )
{
	IScriptSitePtr	ptr_ss = ::GetAppUnknown();
	if( ptr_ss == 0 )
		return false;

	_bstr_t bstr_func = "PrintManager_OnPrint";
	_variant_t args;
	IDispatch* pi_view = 0;
	GetControllingUnknown()->QueryInterface( IID_IPrintViewDisp, (void**)&pi_view );
	if( pi_view == 0 )
		return false;

	IDispatchPtr ptr_data( m_sptrControls );

	args = pi_view;
	
	
	ptr_ss->Invoke( bstr_func, &args, 1, 0, fwAppScript );
	pi_view->Release();	pi_view = 0;
	
	return true;
}

void CPrintView::InitCtrlMode( int nOldMode, int nNewMode )
{

	POSITION posData = m_sptrControls.GetFirstObjectPosition();
	
	while( posData )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( posData );

		POSITION	pos = _Find( punk );
		if( pos )
		{				
			ControlData *pdata = m_controlInfos.GetAt( pos );
			if( pdata && pdata->ptrCtrl != NULL )
			{
				IUnknown* punkCtrl = pdata->ptrCtrl;

				sptrIDBControl	sptrCtrl( punkCtrl );
				
				if( sptrCtrl )
				{
					sptrCtrl->SetReportViewMode( nOldMode, nNewMode );
				}
			}
		}
		punk->Release();
	}

}

///////////////////////////////////////////////////
void CPrintView::EnterDesignMode()
{
	sptrIReportForm	sptrForm( m_sptrControls );
	if( sptrForm != NULL )
		ISOK( sptrForm->SetMode( (int) vmDesign ) );	
		
	InitCtrlMode( viewMode, vmDesign );
 
	viewMode = vmDesign; 

	DestroyViewCtrlContext();
	Update();
}

///////////////////////////////////////////////////
void CPrintView::DestroyViewCtrlContext()
{
	//Refresh ViewAX controls
	POSITION	pos = m_sptrControls.GetFirstObjectPosition();
	
	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );
		POSITION	posCtrl = _Find( punk );
		ControlData *pdata = NULL;

		if( posCtrl )		
			pdata = m_controlInfos.GetAt( posCtrl );

		if( pdata && pdata->ptrCtrl != NULL )
		{
			IViewCtrlPtr	ptrViewCtrl( pdata->ptrCtrl );			
			if( ptrViewCtrl )
			{
				BOOL bAutoBuild = FALSE;					
				ptrViewCtrl->GetAutoBuild( &bAutoBuild );
				if( !bAutoBuild )
					ptrViewCtrl->DestroyView();
			}
		}
		punk->Release( );
	}

}

///////////////////////////////////////////////////
void CPrintView::EnterPreviewMode()
{

	sptrIReportForm	sptrForm( m_sptrControls );
	if( sptrForm != NULL )
		ISOK( sptrForm->SetMode( (int) vmPreview ) );	

	InitCtrlMode( viewMode, vmPreview );

	viewMode = vmPreview; 

	InitViewCtrlData();	

	CalcCtrlPreviewPos();

	Update();
}

void CPrintView::LeavePreviewMode()
{
	WriteToTargetTemplate();
}
///////////////////////////////////////////////////
void CPrintView::EnterPrintMode()
{
	sptrIReportForm	sptrForm( m_sptrControls );
	if( sptrForm != NULL )
		ISOK( sptrForm->SetMode( (int) vmPrinting ) );	


	InitCtrlMode( viewMode, vmPrinting );

	viewMode = vmPrinting;

	InitViewCtrlData();	
	CalcCtrlPreviewPos();
	Update();
	PrintPageContext();
}

///////////////////////////////////////////////////
void CPrintView::UpdateView()
{
	CalcCtrlPreviewPos();
	Update();
}

///////////////////////////////////////////////////
void CPrintView::FitToPage()
{
	if( m_page.GetSize() < 1 )
		return;

	CRect rcClient;
	GetClientRect( &rcClient );
	

	int nWndWidthMM = rcClient.Width();
	int nWndHeightMM = rcClient.Height();


	CRect rcPage = m_page[0]->GetRectPage();	
	rcPage = ConvertFromVTPixelToMM( rcPage );

	int nPageWidthMM = rcPage.Width();
	int nPageHeightMM = rcPage.Height();


	double fZoomX = ((double)nPageWidthMM) / ((double)nWndWidthMM);
	double fZoomY = ((double)nPageHeightMM) / ((double)nWndHeightMM);		

	sptrIScrollZoomSite	pstrS( GetControllingUnknown() );
	if( pstrS == NULL )
		return;

	pstrS->SetZoom( min(1.0/fZoomX, 1.0/fZoomY) * 0.9 );

	OnZoomChange();

	ScroolToVisiblePage();

}

///////////////////////////////////////////////////
void CPrintView::FitToPageWidth()
{
	if( m_page.GetSize() < 1 )
		return;

	CRect rcClient;
	GetClientRect( &rcClient );
	

	int nWndWidthMM = rcClient.Width();	

	CRect rcPage = m_page[0]->GetRectPage();	
	rcPage = ConvertFromVTPixelToMM( rcPage );

	int nPageWidthMM = rcPage.Width();


	double fZoomX = ((double)nPageWidthMM) / ((double)nWndWidthMM);

	sptrIScrollZoomSite	pstrS( GetControllingUnknown() );
	if( pstrS == NULL )
		return;

	pstrS->SetZoom( 1.0/fZoomX * 0.9 );

	OnZoomChange();

	ScroolToVisiblePage();
}

void CPrintView::ScroolToVisiblePage()
{
	int nVisiblePage = -1;
	bool bFind = false;
	for( int i=0;i<m_page.GetSize();i++ )
	{
		if( !bFind && IsVisiblePage(i, NULL ) )
		{
			bFind = true;
			nVisiblePage = i;
		}
	}

	if( nVisiblePage != -1 )
	{
		CRect rcPage = m_page[nVisiblePage]->GetRectPage();
		rcPage = ConvertFromVTPixelToMM( rcPage );

		sptrIScrollZoomSite	pstrS( GetControllingUnknown() );
		if( pstrS == NULL )
			return;

		CPoint pt;
		double fZoom;
		
		ISOK(pstrS->GetScrollPos( &pt ) );
		ISOK(pstrS->GetZoom( &fZoom ) );
		ISOK(pstrS->SetScrollPos( CPoint( long(rcPage.left*fZoom), long(rcPage.top*fZoom)) ) );

	}

}

///////////////////////////////////////////////////
void CPrintView::ScrollToPage( int nPageNum, CRect rcTarget )
{
	if( nPageNum < 0 || nPageNum >= m_page.GetSize() )
		return;
	sptrIScrollZoomSite	pstrS( GetControllingUnknown() );
	if( pstrS == NULL )
		return;

	CRect rcPage = m_page[nPageNum]->GetRectPage();


	rcPage  = ConvertFromVTPixelToMM( rcPage );

		
	double fZoomX = ((double)rcTarget.Width()) / ((double)rcPage.Width());
	double fZoomY = ((double)rcTarget.Height()) / ((double)rcPage.Height());

	double fZoom = 	min( fZoomX, fZoomY ) * 0.85;

	pstrS->SetZoom( fZoom );

	OnZoomChange();

	CRect rcPrintWnd = CRect( 0, 0, 
						long( ((double)rcTarget.Width()) / (fZoom ) ), 
						long( ((double)rcTarget.Height()) / (fZoom) ) );

	CPoint ptOffset = CPoint( 
								(rcPrintWnd.Width() - rcPage.Width())/2, 
								(rcPrintWnd.Height() - rcPage.Height())/2
								);

	ptOffset = ConvertFromMMToVTPixel(ptOffset);
	//Move window to upper-left	
	m_page[nPageNum]->SetClientStartPoint( ptOffset );	

	for( int i=0;i<m_page.GetSize();i++)
	{
		if( i != nPageNum )
			m_page[i]->SetClientStartPoint( CPoint(rcPage.Width()*100, rcPage.Height()*100) );	
	}

	Invalidate( TRUE );

	pstrS->SetScrollPos( CPoint(0,0) );
}


/*===============================*/
/* The Abort Procudure           */
/* ==============================*/
BOOL CALLBACK AbortProc( HDC hDC, int Error )
{
	/*
	_AFX_WIN_STATE* pWinState = _afxWinState;
	MSG msg;
	while (!pWinState->m_bUserAbort &&
		::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		if (!AfxGetThread()->PumpMessage())
			return FALSE;   // terminate if WM_QUIT received
	}
	return !pWinState->m_bUserAbort;
	*/

	/*
	MSG   msg;
	while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	return TRUE;
	*/

	return TRUE;
}
   
///////////////////////////////////////////////////
BOOL CPrintView::PrintPageContext() 
{
	HDC hPrinterDC = NULL;
	hPrinterDC = g_prnSetup.GetHDC( m_sptrControls );

	CDC printDC;

	
	if( hPrinterDC )
	{	
		CString strTitle = "VT5(R)";

		{
			IUnknown* punk = GetDocument( );
			if( punk ) 
			{
				IDocumentSitePtr ptrDS( punk );
				punk->Release();	punk = 0;
				if( ptrDS )
				{
					BSTR bstr = 0;
					ptrDS->GetPathName( &bstr );

					CString strFileName = bstr;
					strTitle += " - ";
					strTitle += strFileName;

					::SysFreeString( bstr );	bstr = 0;
				}

			}
		}



		

		DOCINFO m_docInfo;
		m_docInfo.cbSize = sizeof(DOCINFO);
		m_docInfo.lpszDocName  = strTitle;
		m_docInfo.lpszOutput   = NULL;
		m_docInfo.lpszDatatype = NULL;
		m_docInfo.fwType	   = 0;
		

		if( SetAbortProc( hPrinterDC, AbortProc ) == SP_ERROR )
        {
			AfxMessageBox( "Error setting up AbortProc", MB_APPLMODAL | MB_OK );
			return FALSE;
        }
   

		printDC.Attach( hPrinterDC );		
		//printDC.StartDoc( &m_docInfo );

		int nJob = StartDoc( hPrinterDC, &m_docInfo );
		if (nJob <= 0)
			AfxMessageBox(IDS_PRINTER_ERROR, MB_OK|MB_ICONEXCLAMATION);
#ifdef NEW_PRINT
		do_print( &printDC );
#else
		DrawPagesContext( &printDC, TRUE );
#endif
		/*
		/////////////
		printDC.StartPage( );

		printDC.MoveTo( 100, 100 );
		printDC.LineTo( 1000, 1000 );

		printDC.EndPage( );
		/////////////
		*/

		EndDoc( hPrinterDC );
		
		printDC.Detach();

		SetAbortProc( hPrinterDC, 0 );

		DeleteDC( hPrinterDC );	

		

		return TRUE;

	}
	else
	{
		CWnd* pWnd;
		pWnd = CWnd::GetParent();
		if( pWnd && pWnd->GetSafeHwnd() )
		{
			CWnd* pWndDlg = pWnd->GetParent();
			if( pWndDlg && pWndDlg->GetSafeHwnd() )
			{
				pWndDlg->SendMessage( WM_PRINT_PROGRESS, (WPARAM)0, (LPARAM)1 );
			}				
		}

		AfxMessageBox( IDS_WARNING_CANT_CREATEP_RN_DC, MB_ICONSTOP | MB_OK );		

	}

	return FALSE;
}


void CPrintView::SetTargetDocument(IUnknown* punkTargDoc)
{
	m_ptrTargetDoc = punkTargDoc;
}


BOOL CPrintView::IsSimpleEditMode()
{
	sptrIReportForm	sptrForm( m_sptrControls );
	if( sptrForm == NULL )
		return FALSE;

	BOOL bSimpleEditMode;
	ISOK( sptrForm->GetSimpleMode(&bSimpleEditMode) );

	return bSimpleEditMode;
}


struct CtrlInfo{
	IUnknown* pCtrl;
	IUnknown* pCtrlData;
	CRect rcDesign;
	CRect rcPreview;
	int nDesignCol;
	int nDesignRow;
	int nPreviewCol;
	int nPreviewRow;
	BOOL bScale;
	CRect rcAbsDesign;
	CRect rcAbsPreview;
	int nMainPage;

};

///////////////////////////////////////////////////
void CPrintView::CalcCtrlPreviewPos( )
{

	int nPageWidth  = GetPanePageRect(0,0).Width();
	int nPageHeight = GetPanePageRect(0,0).Height();

	int nLeftField, nRightField, nTopField, nBottomField;
	
	CRect rectPage, rectField;
	
	rectPage  = GetPanePageRect(0,0);
	rectField = GetPaneFieldRect(0,0);


	nLeftField   = rectField.left - rectPage.left;
	nRightField  = rectPage.right - rectField.right;

	nTopField    = rectField.top   - rectPage.top;
	nBottomField = rectPage.bottom - rectField.bottom;

	CArray <CtrlInfo*, CtrlInfo*> m_ctrlInfo;

	POSITION	pos = m_sptrControls.GetFirstObjectPosition();
	
	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );

		POSITION	posCtrl = _Find( punk );
		punk->Release();

		if( !posCtrl )
			continue;

		ControlData *pdata = m_controlInfos.GetAt( posCtrl );		

		BOOL bScale = FALSE;

		if(CheckInterface( pdata->ptrCtrl, IID_IViewCtrl) && 
			CheckInterface( pdata->ptrCtrl, IID_IPrintView))
		{
			IViewCtrlPtr ptrVAX( pdata->ptrCtrl );
			if( ptrVAX )
			{
				short trans;
				ptrVAX->GetObjectTransformation( &trans );
				if( (ObjectTransformation)trans == otStretch )
				{
					bScale = TRUE;
				}				
			}			
		}

		sptrIReportCtrl sptrReportCtrl( punk );

		if( sptrReportCtrl )
		{
			//Adding to ctrlInfo
			CtrlInfo* pCtrlInfo = new CtrlInfo;

			pCtrlInfo->pCtrl = pdata->ptrCtrl;		
			pCtrlInfo->pCtrlData = punk;

			sptrReportCtrl->GetDesignPosition( &pCtrlInfo->rcDesign );
			sptrReportCtrl->GetDesignColRow( &pCtrlInfo->nDesignCol, &pCtrlInfo->nDesignRow );

			sptrReportCtrl->GetDesignOwnerPage( &pCtrlInfo->nMainPage );

			pCtrlInfo->rcPreview	= pCtrlInfo->rcDesign;
			pCtrlInfo->nPreviewCol	= pCtrlInfo->nDesignCol;
			pCtrlInfo->nPreviewRow	= pCtrlInfo->nDesignRow;
			pCtrlInfo->bScale		= bScale;


			


			//Only for ctrl in work area
			CPoint point;
			
			point.x = pCtrlInfo->rcDesign.left + pCtrlInfo->nDesignCol * nPageWidth - 
					(pCtrlInfo->nDesignCol + 1) * nLeftField - 
					pCtrlInfo->nDesignCol * nRightField;

			point.y = pCtrlInfo->rcDesign.top + pCtrlInfo->nDesignRow * nPageHeight - 
					(pCtrlInfo->nDesignRow + 1) * nTopField - 
					pCtrlInfo->nDesignRow * nBottomField;
			
			if( GetPointPlace( 
					CPoint(pCtrlInfo->rcDesign.left, pCtrlInfo->rcDesign.top))
					== ppWork )
			{
				pCtrlInfo->rcAbsDesign	= CRect( point.x , point.y,
											point.x + pCtrlInfo->rcDesign.Width(),
											point.y + pCtrlInfo->rcDesign.Height()
											);
			}
			else
				pCtrlInfo->rcAbsDesign = NORECT;

			pCtrlInfo->rcAbsPreview = pCtrlInfo->rcAbsDesign;


			m_ctrlInfo.Add( pCtrlInfo );

		}
		
	}

	
	int nMaxCtrlXpos = 0;
	for( int i=0;i<(m_ctrlInfo.GetSize());i++)
	{
		nMaxCtrlXpos = max( nMaxCtrlXpos, m_ctrlInfo[i]->rcAbsDesign.left);
	}
	
	nMaxCtrlXpos++;

	//Sort!!!
	

	for( i=0;i<(m_ctrlInfo.GetSize()-1);i++)
	{
		int nAbsX1, nAbsY1;		
		CtrlInfo* pCI1 = m_ctrlInfo[i];
		nAbsX1 = pCI1->rcAbsDesign.left;
		nAbsY1 = pCI1->rcAbsDesign.top;


		int	nCurVal = nAbsY1*nMaxCtrlXpos + nAbsX1;
		int	nCurIdx = i;

		for(int ii=i+1;ii<m_ctrlInfo.GetSize();ii++)
		{	
			int nAbsX2, nAbsY2;		
			CtrlInfo* pCI2 = m_ctrlInfo[ii];

			nAbsX2 = pCI2->rcAbsDesign.left;
			nAbsY2 = pCI2->rcAbsDesign.top;

			int	nTestVal = nAbsY2*nMaxCtrlXpos + nAbsX2;				

			if( nTestVal < nCurVal )
			{
				nCurVal = nTestVal;
				nCurIdx = ii;
			}
		}

		m_ctrlInfo[i]		= m_ctrlInfo[nCurIdx];
		m_ctrlInfo[nCurIdx] = pCI1;
	}
	
	//Sort done



	/*
	
	//4 test purpouse only
	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	TRACE( "After sort\n" );			
	for( i=0;i<(m_ctrlInfo.GetSize());i++)
	{
		CString strDebug;
		CtrlInfo* pCI1 = m_ctrlInfo[i];
		strDebug.Format( "%d %d\n", pCI1->rcAbsPreview.left, pCI1->rcAbsPreview.top  );
		TRACE( strDebug );			
	}	

	CtrlInfo* pCITest0 = NULL;
	if( m_ctrlInfo.GetSize() > 0 )
		pCITest0 = m_ctrlInfo[0];

	CtrlInfo* pCITest1 = NULL;
	if( m_ctrlInfo.GetSize() > 1 )
		pCITest1 = m_ctrlInfo[1];

	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	*/
  	

	//Now correct size(if possible)
	for( i=0;i<m_ctrlInfo.GetSize();i++ )
	{	
		CtrlInfo* pCI1 = m_ctrlInfo[i];

		//Only in work area
		if( GetPointPlace( CPoint(pCI1->rcDesign.left, pCI1->rcDesign.top)) == ppWork )
		{

			if( pCI1->bScale )
			{
				int nDeltaWidth  = 0;
				int nDeltaHeight = 0;
				int nNewWidth  = pCI1->rcAbsDesign.Width()  + nDeltaWidth;
				int nNewHeight = pCI1->rcAbsDesign.Height() + nDeltaHeight;

				int nOccupedCol, nOccupedRow;
				CRect* pRect;
				CSize* pViewCtrlSize;
				CSize* pUserOffset;
				CSize* pUserOffsetDX;

				CPoint point;
				int nCol, nRow;

				nCol = pCI1->rcAbsPreview.left / rectField.Width();
				nRow = pCI1->rcAbsPreview.top  / rectField.Height();

				point.x = pCI1->rcAbsPreview.left - rectField.Width() * nCol//DeltaX on page
							+ nLeftField;

				point.y = pCI1->rcAbsPreview.top - rectField.Height() * nRow//DeltaX on page
							+ nTopField;					

				
				GenerateCtrlRect( pCI1->pCtrlData, point, 
						CSize( nNewWidth, nNewHeight ),
						pRect, nOccupedCol, nOccupedRow,
						pViewCtrlSize,
						pUserOffset,
						pUserOffsetDX,
						vmPreview );

				
				int nRealWidth, nRealHeight;
				nRealWidth = nRealHeight = 0;
				nNewWidth = nNewHeight = 0;

				for( int ii=0;ii<nOccupedCol;ii++)
				{
					nRealWidth += pRect[ii].Width();
					nNewWidth  += pViewCtrlSize[ii].cx;
				}

				nNewWidth = ConvertFromMMToVTPixel( nNewWidth );
					
				for( ii=0;ii<nOccupedRow;ii++)
				{
					int index = nOccupedCol * ii; 
					nRealHeight += pRect[index].Height();
					nNewHeight  += pViewCtrlSize[index].cy;
				}			

				nNewHeight = ConvertFromMMToVTPixel( nNewHeight );
				
				CRect rcNewPreview = CRect( 
								pCI1->rcAbsPreview.left,
								pCI1->rcAbsPreview.top,
								pCI1->rcAbsPreview.left + nNewWidth,
								pCI1->rcAbsPreview.top  + nNewHeight
								);

				CRect rcRealPreview = CRect( 
								pCI1->rcAbsPreview.left,
								pCI1->rcAbsPreview.top,
								pCI1->rcAbsPreview.left + nRealWidth,
								pCI1->rcAbsPreview.top  + nRealHeight
								);

				delete []pRect;
				delete []pViewCtrlSize;
				delete []pUserOffset;
				delete []pUserOffsetDX;


				
				for( int j=0;j<m_ctrlInfo.GetSize();j++ )
				{					
					CtrlInfo* pCI2 = m_ctrlInfo[j];

					if( j != i && pCI1->nMainPage == pCI2->nMainPage )
					{						

						//Vertical ctrl move down
						//if( pCI2->rcAbsPreview.top > pCI1->rcAbsPreview.bottom )
						if( pCI2->rcAbsDesign.top > pCI1->rcAbsDesign.bottom )
						{
							/*
							if( (pCI2->rcAbsPreview.left > rcRealPreview.left && 
								pCI2->rcAbsPreview.left < rcRealPreview.right) ||
								(pCI2->rcAbsPreview.right > rcRealPreview.left && 
								pCI2->rcAbsPreview.right < rcRealPreview.right) )
								*/
							{
								int nOldSpace = pCI2->rcAbsPreview.top - pCI1->rcAbsPreview.bottom;
								int nNewSpace = pCI2->rcAbsPreview.top - rcNewPreview.bottom;
								int nDelta = nOldSpace - nNewSpace;
								
								pCI2->rcAbsPreview.top		+= nDelta;
								pCI2->rcAbsPreview.bottom	+= nDelta;

							}
						}

						//Horizontal ctrl move right
						//if( pCI2->rcAbsPreview.left > pCI1->rcAbsPreview.right )
						if( pCI2->rcAbsDesign.left > pCI1->rcAbsDesign.right )
						{
							if( (pCI2->rcAbsPreview.top > rcRealPreview.top && 
								pCI2->rcAbsPreview.top < rcRealPreview.bottom) ||
								(pCI2->rcAbsPreview.bottom > rcRealPreview.top && 
								pCI2->rcAbsPreview.bottom < rcRealPreview.bottom) )
							{
								int nOldSpace = pCI2->rcAbsPreview.left - pCI1->rcAbsPreview.right;
								int nNewSpace = pCI2->rcAbsPreview.left - rcNewPreview.right;
								int nDelta = nOldSpace - nNewSpace;
								
								pCI2->rcAbsPreview.left		+= nDelta;
								pCI2->rcAbsPreview.right	+= nDelta;
							}
						}

					}
				}

				pCI1->rcAbsPreview = rcNewPreview;
			}
		}

	}
	
	//Convert from rcAbsPreview-> rcPreview, nPreviewCol, nPreviewRow
	for( i=0;i<m_ctrlInfo.GetSize();i++ )
	{	
		CtrlInfo* pCI = m_ctrlInfo[i];
		if( GetPointPlace( CPoint(pCI->rcDesign.left, pCI->rcDesign.top)) == ppWork )
		{
			int nCol, nRow;

			nCol = pCI->rcAbsPreview.left / rectField.Width();
			nRow = pCI->rcAbsPreview.top  / rectField.Height();

			CPoint point;

			point.x = pCI->rcAbsPreview.left - rectField.Width() * nCol//DeltaX on page
							+ nLeftField;

			point.y = pCI->rcAbsPreview.top - rectField.Height() * nRow//DeltaX on page
							+ nTopField;

			pCI->nPreviewCol = nCol;
			pCI->nPreviewRow  = nRow;

			pCI->rcPreview = CRect( point.x, point.y,
					point.x + pCI->rcAbsPreview.Width(), point.y + pCI->rcAbsPreview.Height()
					);
		}
	}

	//now write to objectList

	pos = m_sptrControls.GetFirstObjectPosition();
	
	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );

		for( i=0;i<m_ctrlInfo.GetSize();i++ )
		{	
			CtrlInfo* pCI = m_ctrlInfo[i];
			if( punk == pCI->pCtrlData )
			{
				sptrIReportCtrl sptrReportCtrl( punk );
				if( sptrReportCtrl )
				{
					sptrReportCtrl->SetPreviewColRow( pCI->nPreviewCol, pCI->nPreviewRow );
					sptrReportCtrl->SetPreviewPosition( pCI->rcPreview );
				}
			}
		}

		punk->Release();
	}



	for( i=0;i<m_ctrlInfo.GetSize();i++ )
		delete m_ctrlInfo[i];
	
	m_ctrlInfo.RemoveAll();
	
}


CPoint CPrintView::PaneConvertOffset( CPoint ptOffset )
{
	CPoint pt = ptOffset;
	pt = ConvertFromMMToVTPixel( pt );	
	
	return CPoint( long(V2D(pt.x)), (pt.y) );
}

CRect  CPrintView::PaneConvertObjectRect( IUnknown* punkObject )
{
	if( CheckInterface( punkObject, IID_IReportCtrl ) )
	{
		CRect	rc;
		sptrIReportCtrl	sptrControl( punkObject );
		sptrControl->GetDesignPosition( &rc );

		rc = CRect( long(V2D(rc.left)), long(V2D(rc.top)), long(V2D(rc.right)), long(V2D(rc.bottom)) );
		return rc;
	}

	return NORECT;
}


void CPrintView::WriteToTargetTemplate()
{
	if( viewMode != vmPrinting )
	{
		if( m_ptrTargetDoc != NULL )
		{
			CString strMessage;
			strMessage.LoadString( IDS_SAVE_PREVIEW );
			if( AfxMessageBox( strMessage, MB_YESNO | MB_ICONQUESTION) == IDYES )
			{			
				CString strTemplateName	= _GetValueString( m_ptrTargetDoc, REPORT_TEMPLATE_SECTION, 
										"TemplateName", "_none777" );

				if( strTemplateName == "_none777" )
					return;

				TemplateSource1 ts;
				ts = (TemplateSource1)_GetValueInt( m_ptrTargetDoc, REPORT_TEMPLATE_SECTION, 
										"nSourceTemplate",  (int)tsFile );	

				IUnknownPtr thisReportPtr = NULL;
				thisReportPtr = m_sptrControls;
				

				if( thisReportPtr != NULL )
				{

					if( ts == tsDocument && m_punkReportSourceDoc != NULL )
					{
						{	//Delete old report
							IUnknown* punkOldReport;
							punkOldReport = ::GetObjectByName( m_punkReportSourceDoc, strTemplateName, szTypeReportForm );
							if( punkOldReport != NULL )
							{
								::DeleteObject( m_punkReportSourceDoc, punkOldReport );
								punkOldReport->Release();
							}
						}
						//Create new 
						IUnknown* punkNewReport = ::CloneObject( thisReportPtr );
						if( punkNewReport != NULL )
						{
							_bstr_t bstrName( strTemplateName );
							INamedObject2Ptr ptrNO( punkNewReport );
							if( ptrNO )
							{
								ptrNO->SetName( bstrName );
								ptrNO->SetUserName( bstrName );
							}							

							//Cos Context change 28.08.2000
							//INumeredObjectPtr	ptrN( punkNewReport );
							//ptrN->GenerateNewKey();

							
							::SetValue( m_punkReportSourceDoc, 0, 0, _variant_t( punkNewReport ) );

							punkNewReport->Release();

						}						
					}

					if( ts == tsFile )
					{
						CString strFileName = GetReportDir() + strTemplateName + ".rpt";
						
						IUnknown* punkNewReport = NULL;

						try
						{
							punkNewReport = ::CloneObject( thisReportPtr );
							if( punkNewReport != NULL )
							{
								_bstr_t bstrName( strTemplateName );
								INamedObject2Ptr ptrNO( punkNewReport );
								if( ptrNO )
								{
									ptrNO->SetName( bstrName );
									ptrNO->SetUserName( bstrName );
								}								

							}						

							CFile	file( strFileName, CFile::modeCreate|CFile::modeWrite );
							sptrISerializableObject	sptrO( punkNewReport );

							SerializeParams	params;
							ZeroMemory( &params, sizeof( params ) );

							{
								CArchive		ar( &file, CArchive::store );
								CArchiveStream	ars( &ar );
								sptrO->Store( &ars, &params );
							}
						}
						catch( CFileException* ex)
						{
							ex->ReportError();
							ex->Delete();							
						}

						if( punkNewReport != NULL) 
							punkNewReport->Release(); 

					}
				}

			}
		}
	}

}

DWORD CPrintView::GetMatchType( const char *szType )
{
	if( !strcmp(szType, szArgumenReportCtrl )  )
		return mvFull;
	if( !strcmp(szType, szArgumenReportForm )  )
		return mvFull;
	return mvNone;
}


HRESULT CPrintView::XReportVw::GetMode( int* pnMode )
{
	_try_nested(CPrintView, ReportVw, GetMode)
	{
		*pnMode = (int)pThis->viewMode;
		return S_OK;
	}
	_catch_nested;
}


HRESULT CPrintView::XReportVw::EnterPreviewMode( DWORD dwFlags )
{
	_try_nested(CPrintView, ReportVw, EnterPreviewMode)
	{	
		if( dwFlags )
		{
			pThis->m_bReadOnly = true;
			if( pThis->m_pframe )
			{	
				pThis->m_pframe->EmptyFrame();
			}
		}
		else
			pThis->m_bReadOnly = false;

		pThis->OnDocumentPrint( true );
		pThis->EnterPreviewMode();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPrintView::XReportVw::EnterDesignMode()
{
	_try_nested(CPrintView, ReportVw, EnterDesignMode)
	{
		pThis->m_bReadOnly = false;
		pThis->EnterDesignMode();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPrintView::XReportVw::EnterPrintMode()
{
	_try_nested(CPrintView, ReportVw, EnterPrintMode)
	{		
		pThis->m_bReadOnly = true;
		pThis->OnDocumentPrint( false );
		pThis->EnterPrintMode();
		return S_OK;
	}
	_catch_nested;
}
HRESULT CPrintView::XReportVw::UpdateView()
{
	_try_nested(CPrintView, ReportVw, UpdateView)
	{		
		pThis->UpdateView();
		return S_OK;
	}
	_catch_nested;
}


HRESULT CPrintView::XReportVw::FitToPage()
{
	_try_nested(CPrintView, ReportVw, FitToPage)
	{		
		pThis->FitToPage();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPrintView::XReportVw::FitToPageWidth()
{
	_try_nested(CPrintView, ReportVw, FitToPageWidth)
	{		
		pThis->FitToPageWidth();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPrintView::XReportVw::ScrollToPage( int nPageNum, RECT rcTarget)
{
	_try_nested(CPrintView, ReportVw, ScrollToPage)
	{		
		pThis->ScrollToPage( nPageNum, rcTarget );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPrintView::XReportVw::GetPageCount( int* pnPageCount )
{
	_try_nested(CPrintView, ReportVw, GetPageCount)
	{		
		*pnPageCount = pThis->m_page.GetSize( );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPrintView::XReportVw::SetTargetDocument( IUnknown* punkDoc )
{
	_try_nested(CPrintView, ReportVw, SetTargetDocument)
	{		
		pThis->SetTargetDocument( punkDoc );
		return S_OK;
	}
	_catch_nested;
}

		
HRESULT CPrintView::XReportVw::LeavePreviewMode( )
{
	_try_nested(CPrintView, ReportVw, LeavePreviewMode)
	{		
		pThis->LeavePreviewMode( );
		return S_OK;
	}
	_catch_nested;
}
		 
HRESULT CPrintView::XReportVw::OnPrinterSettingsChange( )
{
	_try_nested(CPrintView, ReportVw, OnPrinterSettingsChange)
	{		
		pThis->OnPrinterSettingsChange( true );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPrintView::XReportVw::Draw( HDC hdc )
{
	_try_nested(CPrintView, ReportVw, Draw)
	{		
		CDC *pDC = CDC::FromHandle( hdc );
		pThis->m_bQuasiPrint = true;
		pThis->DrawPagesContext( pDC, TRUE );
		pThis->m_bQuasiPrint = false;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPrintView::XReportVw::GetPageRect( HDC hDC, RECT *pRect )
{
	_try_nested(CPrintView, ReportVw, GetPageRect )
	{		
		if( !pRect )
			return S_FALSE;

		CDC *pDC = CDC::FromHandle(hDC);

		CRect rcPage = pThis->m_page[0]->GetRectPage( );			

		rcPage  = pThis->ConvertFromVTPixelToMM( rcPage );
		rcPage.OffsetRect( -rcPage.TopLeft() );

		double fPaperWidthPx   = (double)pDC->GetDeviceCaps(HORZRES);
		double fPaperHeightPx  = (double)pDC->GetDeviceCaps(VERTRES);

		double fPaperWidthMM   = 
			(double)(pDC->GetDeviceCaps(HORZSIZE) ) * pThis->GetDisplayPixelPMM();
		double fPaperHeightMM  = 
			(double)(pDC->GetDeviceCaps(VERTSIZE) ) * pThis->GetDisplayPixelPMM();

		rcPage.right = long( rcPage.right * fPaperWidthPx / fPaperWidthMM );
		rcPage.bottom = long( rcPage.bottom * fPaperHeightPx / fPaperHeightMM );

		*pRect = rcPage;

		return S_OK;
	}
	_catch_nested;
}


HRESULT CPrintView::XReportVw::DisconnectNotifyController( BOOL bDisconnect )
{
	_try_nested(CPrintView, ReportVw, DisconnectNotifyController)
	{		
		pThis->m_bDisconnectNotifyContrl = bDisconnect;
		return S_OK;
	}
	_catch_nested;
}


HRESULT CPrintView::XReportVw::SetReportSourceDocument( IUnknown* punkDoc )
{
	_try_nested(CPrintView, ReportVw, SetReportSourceDocument)
	{		
		pThis->m_punkReportSourceDoc = punkDoc;
		return S_OK;
	}
	_catch_nested;
}



void CPrintView::OnRButtonDown(UINT nFlags, CPoint point) 
{	
	CAxBaseView::OnRButtonDown(nFlags, point);
	
	/*
	if( ::GetAsyncKeyState(VK_CONTROL) )
	{
		IReportViewPtr ptrV( GetControllingUnknown() );
		if( ptrV )
			ptrV->EnterPreviewMode( );	
	}
	*/
}

void CPrintView::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CAxBaseView::OnLButtonDown(nFlags, point);
	SetFocus();	
}

void CPrintView::OnKillFocus(CWnd *pWndPrev)
{
	__super::OnKillFocus(pWndPrev);
//	if (!pWndPrev->IsChild(this) && m_bAttachActiveObject == false)
//		pWndPrev = NULL;
}

void CPrintView::OnSetFocus(CWnd *pWndPrev)
{
	__super::OnSetFocus(pWndPrev);
}


BOOL CPrintView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if( m_bReadOnly )
		return CWnd::OnWndMsg( message, wParam, lParam, pResult );

	return CAxBaseView::OnWndMsg( message, wParam, lParam, pResult );

}


CPoint CPrintView::GetScrollPos( )
{
	return ::GetScrollPos( GetControllingUnknown() );
}

//coordinate mapping - screen to client
CRect CPrintView::ConvertToClient( CRect rc)
{	
	return ::ConvertToClient( GetControllingUnknown(), rc );
}

CPoint CPrintView::ConvertToClient( CPoint pt)
{
	return ::ConvertToClient( GetControllingUnknown(), pt);
}

CSize CPrintView::ConvertToClient( CSize size)
{
	return ::ConvertToClient( GetControllingUnknown(), size );
}

//coordinate mapping - client to screen
CRect CPrintView::ConvertToWindow( CRect rc)
{
	return ::ConvertToWindow( GetControllingUnknown(), rc );
}

CPoint CPrintView::ConvertToWindow( CPoint pt)
{
	return ::ConvertToWindow( GetControllingUnknown(), pt );
}

CSize CPrintView::ConvertToWindow( CSize size)
{
	return ::ConvertToWindow( GetControllingUnknown(), size );
}

//dispatch functions
////////////////////////////////////////////////////////////////////////////
long CPrintView::GetFirstCtrlPos( )
{
	if( m_sptrControls == 0 )
		return 0;

	POSITION pos = m_sptrControls.GetFirstObjectPosition();
	return (long)pos;
}

////////////////////////////////////////////////////////////////////////////
long CPrintView::GetNextCtrlPos( LPOS lPos )
{
	if( m_sptrControls == 0 )
		return 0;

	if( !lPos )	return 0;

	POSITION pos = (POSITION)lPos;
	IUnknown* punk = m_sptrControls.GetNextObject( pos );
	if( punk )	
		punk->Release();	punk = 0;	

	return (long)pos;
}

////////////////////////////////////////////////////////////////////////////
LPDISPATCH CPrintView::GetCtrlDispatch( LPOS lPos )
{
	if( m_sptrControls == 0 )
		return 0;

	POSITION pos = (POSITION)lPos;
	IUnknown* punk = m_sptrControls.GetNextObject( pos );
	if( !punk )
		return 0;

	POSITION	pos_ctrl = _Find( punk );
	punk->Release();	punk = 0;

	if( pos_ctrl == 0 )
		return 0;

	ControlData* pdata = m_controlInfos.GetAt( pos_ctrl );
	if( pdata->ptrCtrl == 0 )
		return 0;

	IDispatchPtr ptr_disp = pdata->ptrCtrl;
	if( ptr_disp == 0 )
		return 0;

	ptr_disp->AddRef();

	return ptr_disp;

}	


////////////////////////////////////////////////////////////////////////////
BSTR CPrintView::GetCtrlProgID( LPOS lPos )
{
	if( m_sptrControls == 0 )
		return 0;

	if( !lPos )	return 0;	

	POSITION pos = (POSITION)lPos;
	IUnknown* punk = m_sptrControls.GetNextObject( pos );
	if( !punk )
		return 0;

	sptrIActiveXCtrl	ptr_ax( punk );
	punk->Release();	punk = 0;

	if( ptr_ax == 0 )
		return 0;

	BSTR	bstrProgID = 0;
	ptr_ax->GetProgID( &bstrProgID );

	return bstrProgID;
}

////////////////////////////////////////////////////////////////////////////
long CPrintView::GetSelectedPos( )
{
	if( m_sptrControls == 0 )
		return 0;

	if( !m_pframe )
		return 0;

	IUnknown* punk = m_pframe->GetObjectByIdx( 0 );
	if( punk == 0 )
		return 0;

	GUID guid_test = ::GetObjectKey( punk );
	punk->Release();	punk = 0;

	POSITION pos = m_sptrControls.GetFirstObjectPosition();
	while( pos )
	{
		POSITION pos_save = pos;
		punk = m_sptrControls.GetNextObject( pos );
		if( ::GetObjectKey( punk ) == guid_test )
		{
			punk->Release();
			return (long)pos_save;
		}
		punk->Release();	punk = 0;
	}	

	return 0;
}

////////////////////////////////////////////////////////////////////////////
void CPrintView::SetSelectedPos( LPOS lPos )
{
	if( m_sptrControls == 0 )
		return;
	if( !m_pframe )
		return;
	POSITION pos = (POSITION)lPos;
	IUnknown *punk = m_sptrControls.GetNextObject(pos);
	if( punk )
	{
		m_pframe->SelectObject(punk, false);
		punk->Release();
	}
}


////////////////////////////////////////////////////////////////////////////
void CPrintView::StoreCtrlData( LPOS lPos )
{
	if( m_sptrControls == 0 )	return;

	POSITION pos = (POSITION)lPos;
	IUnknown* punk = m_sptrControls.GetNextObject( pos );
	if( !punk )					return;

	POSITION	pos_ctrl = _Find( punk );
	punk->Release();	punk = 0;

	if( pos_ctrl == 0 )			return;

	ControlData* pdata = m_controlInfos.GetAt( pos_ctrl );
	if( pdata->ptrCtrl == 0 )	return;

	::StoreContainerToDataObject( pdata->punkCtrlData, pdata->ptrCtrl );
}
////////////////////////////////////////////////////////////////////////////
HRESULT CPrintView::XPageVw::GetZoom(float* zoom)
{
	_try_nested(CPrintView, PageVw, GetZoom)
	{
		*zoom = pThis->m_fzoom;
		return S_OK;
	}_catch_nested;
}

HRESULT CPrintView::XPageVw::DrawPage(HDC hDC, int idx, float zoom)
{
	_try_nested(CPrintView, PageVw, DrawPage)
	{		
		CDC *pDC = CDC::FromHandle( hDC );

		
		if( !pDC || idx < 0 || idx >= pThis->m_page.GetSize() || pThis->viewMode!=vmPreview)	return E_INVALIDARG;
		
		float tempZ =pThis->m_fzoom; 

		pThis->m_fzoom=zoom; 


		CPage* ppage = pThis->m_page[idx];

		CSize	window_ext, viewport_ext, window_ext_old, viewport_ext_old;
		CPoint	viewport_org, viewport_org_old;
		window_ext = viewport_ext = window_ext_old = viewport_ext_old = CSize( 0, 0 );
		viewport_org = viewport_org_old = CPoint( 0, 0 );



		for( int idx_ctrl=0;idx_ctrl<ppage->m_ctrl.GetSize();idx_ctrl++)
		{
			int oldDC = pDC->SaveDC();
			int nold_mode = pDC->SetMapMode( MM_ANISOTROPIC );	


			CControlInfo* pci = ppage->m_ctrl[idx_ctrl];
			if( !pci->m_pCtrlData )		continue;

			POSITION pos_ctrl = pThis->_Find( pci->m_pCtrlData );
			if( !pos_ctrl )				continue;

			ControlData* pdata = pThis->m_controlInfos.GetAt( pos_ctrl );
			if( pdata->ptrCtrl == 0 )	continue;
			CRect rc_vt = pci->m_rectOnPage;
			//rc_vt.left *=zoom;
			//rc_vt.right *=zoom;
			//rc_vt.top *=zoom;
			//rc_vt.bottom *=zoom;


			CRect rc_wnd =pThis->convert_from_vt_to_window( rc_vt ); 

			IPrintViewPtr ptr_view_ax = pdata->ptrCtrl;
			IDBControlPtr ptr_db = pdata->ptrCtrl;

			/*pDC->SetViewportOrg(rc_wnd.left,rc_wnd.top);*/
			if( ptr_view_ax != 0 && !( ptr_db != 0 && pThis->viewMode == vmDesign ) )//Native view
			{
				rc_wnd.OffsetRect(pThis->m_pt_scroll);
				CRect rc_item = rc_wnd;
				
				window_ext			= CSize( rc_item.Width(), rc_item.Height() );
				viewport_org		=CPoint( 0, 0 );
				viewport_ext		= CSize( rc_wnd.Width(), rc_wnd.Height() );

				window_ext_old		= pDC->SetWindowExt( window_ext );
				viewport_ext_old	= pDC->SetViewportExt( viewport_ext );
				viewport_org_old	= pDC->SetViewportOrg( viewport_org );
				

				if( pci->m_sizeUserOffsetDX.cx != 0 && 
					pci->m_sizeUserOffsetDX.cy != 0 )
				{
					
					ptr_view_ax->Print( pDC->GetSafeHdc(), rc_wnd,
						pci->m_sizeUserOffset.cx, pci->m_sizeUserOffset.cy,
						pci->m_sizeUserOffsetDX.cx, pci->m_sizeUserOffsetDX.cy, 
						TRUE,  1 );

				}
				else
				{	
					pDC->FillRect( &rc_item, &CBrush( RGB( 240, 240, 240 ) ) );
					CPen pen(PS_SOLID, 1, RGB(0,0,0));
					CPen* pOldPen = pDC->SelectObject( &pen );
					pDC->Rectangle( &rc_item );
					pDC->DrawText( "Not Init", &rc_item, DT_LEFT );
					pDC->SelectObject( pOldPen );				
				}

			}
			else//VTDesign controls
			{
				IVTPrintCtrlPtr ptr_print_ctrl = pdata->ptrCtrl;
				if( ptr_print_ctrl )
				{
					//himetrics :(
					window_ext		= CSize( rc_vt.Width(), -rc_vt.Height() );

					viewport_ext	= CSize( rc_wnd.Width(), rc_wnd.Height() );
					viewport_org	= CPoint( rc_vt.left, rc_vt.top );

					viewport_org	= /*pThis->convert_from_vt_to_window( viewport_org )*/
						CPoint(rc_wnd.left, rc_wnd.top);
					viewport_org.Offset( pThis->m_pt_scroll);

					viewport_org.y += rc_wnd.Height();

					window_ext_old		= pDC->SetWindowExt( window_ext );					
					viewport_ext_old	= pDC->SetViewportExt( viewport_ext );
					viewport_org_old	= pDC->SetViewportOrg( viewport_org );

					ptr_print_ctrl->SetPageInfo( idx, pThis->m_page.GetSize() );
					ptr_print_ctrl->Draw( pDC->m_hDC, CSize( rc_vt.Width(), rc_vt.Height() ) );


				}
				else//Simple OCX
				{
					window_ext		= CSize( rc_vt.Width(), rc_vt.Height() );
					viewport_ext	= CSize( rc_wnd.Width(), rc_wnd.Height() );
					viewport_org	= CPoint( rc_vt.left, rc_vt.top );

					viewport_org = pThis->convert_from_vt_to_window( viewport_org ) ;


					window_ext_old		= pDC->SetWindowExt( window_ext );					
					viewport_ext_old	= pDC->SetViewportExt( viewport_ext );
					viewport_org_old	= pDC->SetViewportOrg( viewport_org );

					IViewObjectPtr	ptr_ocx( pdata->ptrCtrl );
					if( ptr_ocx != 0 )
					{
						DVASPECTINFO	info = {0};
						info.cb = sizeof( info );
						info.dwFlags = DVASPECTINFOFLAG_CANOPTIMIZE;
						CRect rc_item = CRect( 0, 0, rc_vt.Width() + 1, rc_vt.Height() + 1 );
						RECTL* prect = (RECTL*)&rc_item;
						ptr_ocx->Draw( DVASPECT_CONTENT, -1, 
							0, 0, pDC->m_hAttribDC, pDC->m_hDC, 
							prect, prect, NULL, 0 );
					}
				}
			}
			pDC->SetWindowExt( window_ext_old );
			pDC->SetViewportExt( viewport_ext_old );
			pDC->SetViewportOrg( viewport_org_old );
			pDC->SetMapMode( nold_mode );

			pDC->RestoreDC(oldDC);		

		}
		pThis->m_fzoom =tempZ;

	}
	_catch_nested;

}