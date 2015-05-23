// AciveColorsViewCtl.cpp : Implementation of the CAciveColorsViewCtrl ActiveX Control class.

#include "stdafx.h"
#include "AciveColorsView.h"
#include "AciveColorsViewCtl.h"
#include "AciveColorsViewPpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CAciveColorsViewCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CAciveColorsViewCtrl, COleControl)
	//{{AFX_MSG_MAP(CAciveColorsViewCtrl)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
	ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CAciveColorsViewCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CAciveColorsViewCtrl)
	DISP_PROPERTY_NOTIFY(CAciveColorsViewCtrl, "BMPFileName", m_bMPFileName, OnBMPFileNameChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY(CAciveColorsViewCtrl, "LeftBtnTransparentColor", m_leftBtnTransparentColor, OnLeftBtnTransparentColorChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CAciveColorsViewCtrl, "RightBtnTransparentColor", m_rightBtnTransparentColor, OnRightBtnTransparentColorChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CAciveColorsViewCtrl, "LeftBtnColor", m_leftBtnColor, OnLeftBtnColorChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CAciveColorsViewCtrl, "RightBtnColor", m_rightBtnColor, OnRightBtnColorChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CAciveColorsViewCtrl, "ColorTransparent", m_colorTransparent, OnColorTransparentChanged, VT_COLOR)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CAciveColorsViewCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CAciveColorsViewCtrl, COleControl)
	//{{AFX_EVENT_MAP(CAciveColorsViewCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CAciveColorsViewCtrl, 2)
	PROPPAGEID(CAciveColorsViewPropPage::guid)
	PROPPAGEID( CLSID_CColorPropPage )
END_PROPPAGEIDS(CAciveColorsViewCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CAciveColorsViewCtrl, "ACIVECOLORSVIEW.AciveColorsViewCtrl.1",
	0xc1f6a8b, 0x9db3, 0x11d3, 0xa5, 0x36, 0, 0, 0xb4, 0x93, 0xa1, 0x87)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CAciveColorsViewCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DAciveColorsView =
		{ 0xc1f6a89, 0x9db3, 0x11d3, { 0xa5, 0x36, 0, 0, 0xb4, 0x93, 0xa1, 0x87 } };
const IID BASED_CODE IID_DAciveColorsViewEvents =
		{ 0xc1f6a8a, 0x9db3, 0x11d3, { 0xa5, 0x36, 0, 0, 0xb4, 0x93, 0xa1, 0x87 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwAciveColorsViewOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CAciveColorsViewCtrl, IDS_ACIVECOLORSVIEW, _dwAciveColorsViewOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewCtrl::CAciveColorsViewCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CAciveColorsViewCtrl

BOOL CAciveColorsViewCtrl::CAciveColorsViewCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.
	if( bRegister )
		return AfxOleRegisterControlClass( 
		AfxGetInstanceHandle(), 
		m_clsid,
		m_lpszProgID,
		IDS_ACIVECOLORSVIEW, 
		IDB_ACIVECOLORSVIEW,
		afxRegApartmentThreading, 
		_dwAciveColorsViewOleMisc,
		_tlid, _wVerMajor, _wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewCtrl::CAciveColorsViewCtrl - Constructor

CAciveColorsViewCtrl::CAciveColorsViewCtrl() : 
	m_rcModifiedArea(0, 0, 0, 0), 
	m_sizeOriginBitmap(0, 0)
{
	InitializeIIDs(&IID_DAciveColorsView, &IID_DAciveColorsViewEvents);

	m_strPrevFileName = "";
	m_bMPFileName = "";
	m_hBitmap = 0;
	m_leftBtnTransparentColor = (OLE_COLOR)RGB(0, 0, 0);
	m_rightBtnTransparentColor = (OLE_COLOR)RGB(255, 255, 255);
	m_bStoredBMPInArchive = FALSE;

	// TODO: Initialize your control's instance data here.
}

void CAciveColorsViewCtrl::_FreeBMPs()
{
	if(m_hBitmap)
			::DeleteObject(m_hBitmap);
		m_hBitmap = 0;
}
/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewCtrl::~CAciveColorsViewCtrl - Destructor

CAciveColorsViewCtrl::~CAciveColorsViewCtrl()
{
	// TODO: Cleanup your control's instance data here.
	_FreeBMPs();
}


/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewCtrl::OnDraw - Drawing function

void CAciveColorsViewCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	pdc->FillRect(rcBounds, &CBrush(::GetSysColor(COLOR_BTNFACE)));
	if(m_hBitmap != 0) 
	{
		CDC memdc;
		memdc.CreateCompatibleDC( pdc );
		CBitmap *poldbmp = memdc.SelectObject( CBitmap::FromHandle(m_hBitmap));
		int nOffsetX = max(0, (rcBounds.Width() - m_sizeOriginBitmap.cx)/2);
		int nOffsetY = max(0, (rcBounds.Height() - m_sizeOriginBitmap.cy)/2);
		pdc->BitBlt( rcBounds.left+nOffsetX, rcBounds.top+nOffsetY, m_sizeOriginBitmap.cx, m_sizeOriginBitmap.cy, &memdc, 0, 0, SRCCOPY);
		memdc.SelectObject( poldbmp );
	}
	else
		DoSuperclassPaint(pdc, rcBounds);
}


/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewCtrl::DoPropExchange - Persistence support

void CAciveColorsViewCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	/*PX_Color(pPX, "LeftBtnTransparentColor", m_leftBtnTransparentColor);
	PX_Color(pPX, "RightBtnTransparentColor", m_rightBtnTransparentColor);
	PX_Color(pPX, "LeftBtnColor", m_leftBtnColor);
	PX_Color(pPX, "RightBtnColor", m_rightBtnColor);
	PX_Color(pPX, "colorTransparent", m_colorTransparent);
	PX_String(pPX, "BMPFileName", m_bMPFileName);*/
	//PX_String(pPX, "BMPFileName", m_strPrevFileName);
	
	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewCtrl::OnResetState - Reset control to default state

void CAciveColorsViewCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	_MakeBitmaps();
	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewCtrl::AboutBox - Display an "About" box to the user

void CAciveColorsViewCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_ACIVECOLORSVIEW);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CAciveColorsViewCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	//_MakeBitmaps();
	cs.lpszClass = _T("STATIC");

	return COleControl::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewCtrl::IsSubclassedControl - This is a subclassed control

BOOL CAciveColorsViewCtrl::IsSubclassedControl()
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewCtrl::OnOcmCommand - Handle command messages

LRESULT CAciveColorsViewCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.

	return 0;
}


void CAciveColorsViewCtrl::_MakeBitmaps()
{
	
	try
	{
		_FreeBMPs();		
		if(!m_DIB.IsValid())return;

		if(m_bMPFileName.IsEmpty())return;

		CClientDC dc(0);

		//m_hBitmap = 0;
		m_hBitmap = m_DIB.CreateBitmap(&dc); 

		if(!m_hBitmap) 
		{
			return;
		}


		
		BITMAP bmp;
		ZeroMemory(&bmp, sizeof(BITMAP));

		VERIFY(CBitmap::FromHandle(m_hBitmap)->GetBitmap(&bmp) != 0);

		m_sizeOriginBitmap = CSize(bmp.bmWidth, bmp.bmHeight);
		
		CDC dcColor;
		
		VERIFY(dcColor.CreateCompatibleDC(&dc) == TRUE);
		
		CBitmap* pOldBitmap = dcColor.SelectObject(CBitmap::FromHandle(m_hBitmap));
		VERIFY(pOldBitmap != 0);
		

		COLORREF	pixColor;
		COLORREF	fillTransparent = ::GetSysColor(COLOR_BTNFACE);


		m_rcModifiedArea = CRect(-1, -1, 0, 0);

		int nWidth = m_rcModifiedArea.TopLeft() == CPoint(-1, -1)?m_sizeOriginBitmap.cx:m_rcModifiedArea.Width();
		int nHeight = m_rcModifiedArea.TopLeft() == CPoint(-1, -1)?m_sizeOriginBitmap.cy:m_rcModifiedArea.Height();
		for(long i = 0; i < nWidth; i++)
		{
			for(long j = 0; j < nHeight; j++)
			{
				pixColor = dcColor.GetPixel(i, j);
				if(pixColor == TranslateColor(m_colorTransparent))
				{
					dcColor.SetPixel(i, j, fillTransparent);
				}
				else if(pixColor == TranslateColor(m_leftBtnTransparentColor))
				{
					dcColor.SetPixel(i, j, TranslateColor(m_leftBtnColor));
					/*if(m_rcModifiedArea.TopLeft() == CPoint(-1, -1))
						m_rcModifiedArea.TopLeft() = CPoint(i, j);
					m_rcModifiedArea.BottomRight() = CPoint(i, j);*/
				}
				else if(pixColor == TranslateColor(m_rightBtnTransparentColor))
				{
					dcColor.SetPixel(i, j, TranslateColor(m_rightBtnColor));
					/*if(m_rcModifiedArea.TopLeft() == CPoint(-1, -1))
						m_rcModifiedArea.TopLeft() = CPoint(i, j);
					m_rcModifiedArea.BottomRight() = CPoint(i, j);*/
				}
			}
		}
		
		m_hBitmap = (HBITMAP)*dcColor.SelectObject(pOldBitmap);
		VERIFY(m_hBitmap != 0);
		
		InvalidateControl();
	}
	catch(CException *pe)
	{
		_FreeBMPs();
		m_DIB.Free();
		pe->ReportError();
		pe->Delete();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewCtrl message handlers

void CAciveColorsViewCtrl::OnBMPFileNameChanged() 
{
	// TODO: Add notification handler code
	try
	{
		if(!m_bMPFileName.IsEmpty())// && m_strPrevFileName != m_bMPFileName)
		{
			
		//	AfxMessageBox( "OnBMPFileNameChanged() " );
			//if(m_DIB.AttachMapFile(m_bMPFileName, TRUE) == FALSE) return;

			{
			CFile file(m_bMPFileName, CFile::modeRead);
			m_DIB.Read(file);
			}

			m_strPrevFileName = m_bMPFileName;
			_MakeBitmaps();

			m_rcModifiedArea = CRect(-1, -1, 0, 0);
		}
		else
			_MakeBitmaps();
	}
	catch(CException *pe)
	{
 		m_DIB.Free();
		_MakeBitmaps();
		pe->ReportError();
		pe->Delete();
	}
		
	SetModifiedFlag();
}


void CAciveColorsViewCtrl::Serialize(CArchive& ar) 
{
	if (ar.IsStoring())
	{	
		// storing code
		ar<<m_bMPFileName;
		ar<<m_strPrevFileName;
		COLORREF color = TranslateColor(m_leftBtnTransparentColor);
		ar<<(BYTE)GetRValue(color);
		ar<<(BYTE)GetGValue(color);
		ar<<(BYTE)GetBValue(color);
		color = TranslateColor(m_rightBtnTransparentColor);
		ar<<(BYTE)GetRValue(color);
		ar<<(BYTE)GetGValue(color);
		ar<<(BYTE)GetBValue(color);
		color = TranslateColor(m_leftBtnColor);
		ar<<(BYTE)GetRValue(color);
		ar<<(BYTE)GetGValue(color);
		ar<<(BYTE)GetBValue(color);
		color = TranslateColor(m_rightBtnColor);
		ar<<(BYTE)GetRValue(color);
		ar<<(BYTE)GetGValue(color);
		ar<<(BYTE)GetBValue(color);
		color = TranslateColor(m_colorTransparent);
		ar<<(BYTE)GetRValue(color);
		ar<<(BYTE)GetGValue(color);
		ar<<(BYTE)GetBValue(color);
	}
	else
	{	
		// loading code
		ar>>m_bMPFileName;
		ar>>m_strPrevFileName;
		BYTE R, G, B;
		ar>>R;
		ar>>G;
		ar>>B;
		m_leftBtnTransparentColor = (OLE_COLOR)RGB(R, G, B);
		ar>>R;
		ar>>G;
		ar>>B;
		m_rightBtnTransparentColor = (OLE_COLOR)RGB(R, G, B);
		ar>>R;
		ar>>G;
		ar>>B;
		m_leftBtnColor = (OLE_COLOR)RGB(R, G, B);
		ar>>R;
		ar>>G;
		ar>>B;
		m_rightBtnColor = (OLE_COLOR)RGB(R, G, B);
		ar>>R;
		ar>>G;
		ar>>B;
		m_colorTransparent = (OLE_COLOR)RGB(R, G, B);
	}


	m_DIB.Serialize(ar);
	_MakeBitmaps();
	/*ar.Flush();
	if(ar.IsStoring())
	{
		if(m_DIB.IsValid() == TRUE)
		{
			//AfxMessageBox("Store BMP!");
			m_DIB.Save(*ar.GetFile());
		}
	}
	else
	{
		//AfxMessageBox("Load BMP!");
		m_DIB.Read(*ar.GetFile());
	}
	*/

	//OnBMPFileNameChanged();
}

void CAciveColorsViewCtrl::OnLeftBtnTransparentColorChanged() 
{
	// TODO: Add notification handler code
	_MakeBitmaps();
	SetModifiedFlag();
}

void CAciveColorsViewCtrl::OnRightBtnTransparentColorChanged() 
{
	// TODO: Add notification handler code
	_MakeBitmaps();
	SetModifiedFlag();
}

void CAciveColorsViewCtrl::OnLeftBtnColorChanged() 
{
	// TODO: Add notification handler code
	_MakeBitmaps();
	SetModifiedFlag();
}

void CAciveColorsViewCtrl::OnRightBtnColorChanged() 
{
	// TODO: Add notification handler code
	_MakeBitmaps();
	SetModifiedFlag();
}

void CAciveColorsViewCtrl::OnColorTransparentChanged() 
{
	// TODO: Add notification handler code
	m_rcModifiedArea = CRect(-1, -1, 0, 0);
	_MakeBitmaps();
	SetModifiedFlag();
}

