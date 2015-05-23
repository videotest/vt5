// DBDateTimeCtl.cpp : Implementation of the CDBObject ActiveX Control class.

#include "stdafx.h"
#include "DBControls.h"
#include "DBObject.h"
#include "DBObjectPpg.h"
#include "DBCommonPropPage.h"

#include "\vt5\awin\profiler.h"

#include "thumbnail.h"

#include "EditCtrl.h"

#include "obj_types.h"
#include "ScriptNotifyInt.h"
#include <PropBag.h>

							   
IMPLEMENT_DYNCREATE(CDBObject, COleControl)

BEGIN_INTERFACE_MAP(CDBObject, COleControl)
	INTERFACE_PART(CDBObject, IID_IDBControl, DBControl)	
	INTERFACE_PART(CDBObject, IID_IDBControl2, DBControl)	
	INTERFACE_PART(CDBObject, IID_IDBaseListener, DBaseListener)	
	INTERFACE_PART(CDBObject, IID_IViewCtrl, ViewCtrl )	
	INTERFACE_PART(CDBObject, IID_IPrintView, PrintView)	
	INTERFACE_PART(CDBObject, IID_IVtActiveXCtrl, VtActiveXCtrl)	
	INTERFACE_PART(CDBObject, IID_IVtActiveXCtrl2, VtActiveXCtrl2)		
	INTERFACE_PART(CDBObject, IID_IDBObjectControl, DBObjectControl)		
	INTERFACE_PART(CDBObject, IID_IVTPrintCtrl, PrintCtrl)
	INTERFACE_PART(CDBObject, IID_IAXCtrlDataSite, AXCtrlDataSite)
	INTERFACE_PART(CDBObject, IID_IEventListener, EvList)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CDBObject, ViewCtrl);
IMPLEMENT_UNKNOWN(CDBObject, PrintView);
IMPLEMENT_UNKNOWN(CDBObject, VtActiveXCtrl);
IMPLEMENT_UNKNOWN(CDBObject, VtActiveXCtrl2);
IMPLEMENT_UNKNOWN(CDBObject, DBObjectControl);
IMPLEMENT_AX_DATA_SITE(CDBObject)

CORRECT_MFC_OnSetObjectRects(CDBObject);
IMPLEMENT_UNKNOWN(CDBObject, PrintCtrl)
/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CDBObject, COleControl)
	//{{AFX_MSG_MAP(CDBObject)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_KILLFOCUS()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CDBObject, COleControl)
	//{{AFX_DISPATCH_MAP(CDBObject)
	DISP_PROPERTY_EX(CDBObject, "AutoLabel", GetAutoLabel, SetAutoLabel, VT_BOOL)
	DISP_PROPERTY_EX(CDBObject, "EnableEnumeration", GetEnableEnumeration, SetEnableEnumeration, VT_BOOL)
	DISP_PROPERTY_EX(CDBObject, "TableName", GetTableName, SetTableName, VT_BSTR)
	DISP_PROPERTY_EX(CDBObject, "FieldName", GetFieldName, SetFieldName, VT_BSTR)
	DISP_PROPERTY_EX(CDBObject, "LabelPosition", GetLabelPosition, SetLabelPosition, VT_I2)
	DISP_PROPERTY_EX(CDBObject, "ViewType", GetViewType, SetViewType, VT_BSTR)
	DISP_PROPERTY_EX(CDBObject, "AutoViewType", GetAutoViewType, SetAutoViewType, VT_BOOL)
	//DISP_PROPERTY_EX(CDBObject, "ReadOnly", GetReadOnly, SetReadOnly, VT_BOOL)
	DISP_FUNCTION(CDBObject, "GetPropertyAXStatus", GetPropertyAXStatus, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CDBObject, "SetPropertyAXStatus", SetPropertyAXStatus, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CDBObject, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CDBObject, COleControl)
	//{{AFX_EVENT_MAP(CDBObject)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
/*BEGIN_PROPPAGEIDS(CDBObject, 2)
	PROPPAGEID(CDBCommonPropPage::guid)
	PROPPAGEID(CDBObjectPropertyPage::guid)	
END_PROPPAGEIDS(CDBObject)
*/
static CLSID pages[2];

LPCLSID CDBObject::GetPropPageIDs( ULONG& cPropPages )
{
	cPropPages = 0;
	pages[cPropPages++]=CDBCommonPropPage::guid;
	if( ::GetValueInt( ::GetAppUnknown(), "\\DBControls", "UseObjectPane", 1 ) != 0 )
		pages[cPropPages++]=CDBObjectPropertyPage::guid;
	m_PropertyAXStatus=true;

	return pages;
}

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CDBObject, DBVTObjectProgID,
	0x31bd1b24, 0x7b8d, 0x4017, 0xb5, 0xbf, 0xeb, 0xb8, 0x5f, 0x96, 0x12, 0xe3)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CDBObject, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs
// {6F09781B-09DA-4e83-907A-F21140AD7F7C}

extern const IID BASED_CODE IID_DDBObject =
		{ 0x6f09781b, 0x9da, 0x4e83, { 0x90, 0x7a, 0xf2, 0x11, 0x40, 0xad, 0x7f, 0x7c } };
const IID BASED_CODE IID_DDBObjectEvents =
	{ 0xed332c9, 0xcc5f, 0x4bd2, { 0xae, 0xc6, 0xc4, 0xc9, 0x95, 0x68, 0xae, 0x1d } };

/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwDBObjectOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CDBObject, IDS_DBOBJECT, _dwDBObjectOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CDBObject::CDBObjectFactory::UpdateRegistry -
// Adds or removes system registry entries for CDBObject

BOOL CDBObject::CDBObjectFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_DBOBJECT,
			IDB_DBOBJECT,
			afxRegApartmentThreading,
			_dwDBObjectOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CDBObject::CDBObject - Constructor

CDBObject::CDBObject()   : m_fontHolder( &m_xFontNotification )
, m_ViewSubType(0)
{
	InitializeIIDs(&IID_DDBObject, &IID_DDBObjectEvents);
	CDBControlImpl::InitDefaults();	

	m_bAutoLabel = TRUE;
	m_nLabelPosition = 1;//top
	m_bAutoViewType = TRUE;	

	init_default_font( &m_fontDesc );

	m_fontHolder.ReleaseFont();
	m_fontHolder.InitializeFont( &m_fontDesc );	

	//added by akm 13_12_k5
	m_PropertyAXStatus=false;
}


/////////////////////////////////////////////////////////////////////////////
// CDBObject::~CDBObject - Destructor

CDBObject::~CDBObject()
{
}


#define szViewAXProgID  "VIEWAX.ViewAXCtrl.1"

#define VIEWAX_CTRL	100

/////////////////////////////////////////////////////////////////////////////
int CDBObject::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rectClient;
	GetClientRect( &rectClient );
	
	RegisterDB();

	
	CRect rc = CRect(0,0,0,0);		
	BOOL result = m_ctrlAXView.CreateControl(	_bstr_t(szViewAXProgID), NULL,
												WS_CHILD|WS_VISIBLE|WS_TABSTOP,
												rc, this, VIEWAX_CTRL );
	ASSERT(result);

	
	Register( m_spDBaseDocument );

	//comment by paul 13.02.2003
	//SetViewName();	
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::Resize()
{

	if( !::IsWindow( m_ctrlAXView.GetSafeHwnd() ) )
		return;		

	CRect rcClient;	
	GetClientRect( &rcClient );



	if( GetReportViewMode() != vmNoInfo )
	{
		m_ctrlAXView.MoveWindow( &rcClient );
		return;
	}

	CRect rcView;

	rcView.left	= rcClient.right;
	rcView.top	= rcClient.bottom;

	IScrollZoomSitePtr ptrSZS( m_ctrlAXView.GetControlUnknown() );
	if( ptrSZS )
	{
		SIZE size;
		ptrSZS->GetClientSize( &size );

		rcView.right	= rcView.left + size.cx;
		rcView.bottom	= rcView.top + size.cy;

		m_ctrlAXView.MoveWindow( &rcView );
	}
	

	/*
	rcClient.DeflateRect( 3, 3, 3, 3 );


	CSize sizeLeftOffset = CSize(0,0);	

	CRect rcEdit;

	rcEdit = rcClient;

	rcEdit.left += sizeLeftOffset.cx;	
	rcEdit.top += sizeLeftOffset.cy;
	
	

	if( GetReportViewMode() == vmNoInfo )		
	{
		//rcEdit = NORECT;	
		rcEdit += CPoint( rcClient.right, rcClient.bottom );
	}
	
	
	m_ctrlAXView.MoveWindow( &rcEdit );			
	*/
	
}


/////////////////////////////////////////////////////////////////////////////
LRESULT CDBObject::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	if( message == WM_SET_CONTROL_TEXT ) 
	{
		CString strText = (char*)lParam;		
		_variant_t var_t(strText);
		if( var_t.vt == VT_DATE )
		{
			try{
				//m_ctrlAXView.SetValue( var_t );					
			}
			catch(...){}
		}
		return TRUE;
	}
	
	return COleControl::DefWindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::OnSetFocus(CWnd* pOldWnd) 
{
	COleControl::OnSetFocus(pOldWnd);			
	
	SetAciveDBaseField();	
	return;

}

void CDBObject::OnRButtonDown(UINT nFlags, CPoint point) 
{
	COleControl::OnRButtonDown(nFlags, point);

	SetAciveDBaseField();
}


void CDBObject::OnKillFocus(CWnd* pNewWnd) 
{
	COleControl::OnKillFocus(pNewWnd);

	return;
	
	CRect rcClient;
	GetClientRect( &rcClient );
	DrawBorder( GetDC(), rcClient, false );
}


/////////////////////////////////////////////////////////////////////////////
void CDBObject::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);		
	Resize();
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::IDBControl_BuildAppearanceAfterLoad( )
{
	SetViewName();	
}


/*void CDBObject::DrawBorder( CDC* pdc, CRect rcBorder, bool bFocused )
{
	if( bFocused )
		pdc->DrawEdge( &rcBorder, EDGE_SUNKEN, BF_RECT );
	else
		pdc->DrawEdge( &rcBorder, EDGE_RAISED, BF_RECT );
}*/

void CDBObject::InitDrawControlMode()
{
	INamedObject2Ptr sptrNO2(m_ctrlAXView.GetControlUnknown());
	if (sptrNO2 != 0)
	{
		_bstr_t bstrName;
		sptrNO2->GetName(bstrName.GetAddress());
		if (bstrName.length() > 0)
		{
			CString sSection(_T("\\Database\\Blank\\"));
			sSection += (LPCTSTR)bstrName;
			int nZoomingMode = ::GetValueInt(::GetAppUnknown(), sSection, "ZoomingMode", 0);
			if (nZoomingMode >= dcmAlreadyZoom && nZoomingMode <= dcmMaximal)
			{
				m_DrawControlMode.dcm = (DrawControlMode)nZoomingMode;
				m_DrawControlMode.nLimitProc = GetValueInt(GetAppUnknown(), sSection, "LimitProcent");
				TCHAR szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME],szExt[_MAX_EXT];
				GetModuleFileName(AfxGetInstanceHandle(), szPath, _MAX_PATH);
				_tsplitpath(szPath, szDrive, szDir, szFName, szExt);
				_tmakepath(szPath, szDrive, szDir, 0, 0);
				CString sPicPath = ::GetValueString(::GetAppUnknown(), "\\Paths", "DBBlankPictures", szPath);
				CString sCurPath = ::GetValueString(::GetAppUnknown(), sSection, "Picture", NULL);
				if (!sCurPath.IsEmpty())
				{
					_tsplitpath(sCurPath, szDrive, szDir, szFName, szExt);
					if (szFName[0] && szExt[0])
					{
						_tsplitpath(sPicPath, szDrive[0]?0:szDrive, szDir[0]?0:szDir, 0, 0);
						_tmakepath(szPath, szDrive, szDir, szFName, szExt);
						if (m_DrawControlMode.dcm == dcmPicture)
						{
							m_DrawControlMode.sPicturePath = szPath;
							if (m_DrawControlMode.bmp)
								::DeleteObject(m_DrawControlMode.bmp);
							m_DrawControlMode.bmp = (HBITMAP)LoadImage(NULL, m_DrawControlMode.sPicturePath,
								IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
						}
					}
				}
				m_DrawControlMode.nPictureZoomMode = ::GetValueInt(GetAppUnknown(), sSection, "ZoomPictureMode", COLORONCOLOR);
				if (m_DrawControlMode.nPictureZoomMode < BLACKONWHITE || m_DrawControlMode.nPictureZoomMode > HALFTONE )
					m_DrawControlMode.nPictureZoomMode = COLORONCOLOR;
				m_DrawControlMode.bZoomPicture = ::GetValueInt(GetAppUnknown(), sSection, "ZoomPicture", FALSE);
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDBObject::OnDraw - Drawing function
void CDBObject::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	CRect rcClient = rcBounds;
	//GetClientRect( &rcClient );

	CRect rcFill = rcClient;
	rcFill.InflateRect( 0, 0, 1, 1 );

	pdc->FillRect( rcFill, &CBrush::CBrush( ::GetSysColor(COLOR_3DFACE)));	

	DrawBorder( pdc, rcFill, IsActiveDBaseField() );


	if( m_spDBaseDocument == 0 )
		return;
	
	BlankViewMode blankViewMode	= GetBlankViewMode();
	ViewMode ReportViewMode		= GetReportViewMode();

	if( ReportViewMode == vmDesign /*|| BlankViewMode == bvmDesign */)
	{
		CString strOut = m_strTableName + CString(".") + m_strFieldName;
		if( m_strTableName.IsEmpty() )
			strOut = "Table empty";

		if( m_strFieldName.IsEmpty() )
			strOut = "Field empty";

		COLORREF oldColor = pdc->SetBkColor( ::GetSysColor(COLOR_3DFACE) );
		CFont* pOldFont = pdc->SelectObject( &m_Font );
		CRect rc_text = rcClient;
		rc_text.DeflateRect( 3, 3, 0, 0 );
		pdc->DrawText( GetCaption(), &rc_text, DT_LEFT );
		//pdc->TextOut( rcClient.left + 3, rcClient.top + 3 ,strOut );	
		pdc->SelectObject( pOldFont );
		pdc->SetBkColor( ::GetSysColor(oldColor) );
		return;
	}

	
		
	CRect rcImage = rcClient;
	
	rcImage.DeflateRect( 5, 5, 5, 5 );

	/*
	CString strObjectName;
	strObjectName.Format( "%s.%s", (LPCSTR)IDBControl_GetTableName(), (LPCSTR)IDBControl_GetFieldName() );

	IUnknown* punkObject = ::GetObjectByName( m_spDBaseDocument, strObjectName, 0 );
	if( punkObject == NULL )
		return;

	IUnknownPtr ptrObject = punkObject;
	punkObject->Release();

	if( ptrObject == NULL )
		return;


	sptrIThumbnailManager spThumbMan( GetAppUnknown() );
	if( spThumbMan == NULL )
		return;


	IUnknown* punkRenderObject = NULL;

	spThumbMan->GetRenderObject( ptrObject, &punkRenderObject );
	if( punkRenderObject == NULL )
		return;


	sptrIRenderObject spRenderObject( punkRenderObject );
	punkRenderObject->Release();

	if( spRenderObject == NULL ) 
		return;
		*/

	CSize sizeLeftOffset = CSize(0,0);
	CSize sizeRightOffset = CSize(0,0);

	if( m_bAutoLabel )
	{
		CClientDC dc( this );
		CString srtCaption = GetCaption();
		CRect rcText;
		GetClientRect(rcText);
		CFont* pOldFont = dc.SelectObject( &m_Font );
		dc.DrawText( srtCaption, rcText, DT_CALCRECT );
		dc.SelectObject( pOldFont );

		if( m_nLabelPosition == 0 )
			sizeLeftOffset.cx = rcText.Width();
		else
			sizeLeftOffset.cy = rcText.Height();
	}

	rcImage.left	+= sizeLeftOffset.cx;
	rcImage.top		+= sizeLeftOffset.cy;


	/*
	BYTE* pbi = NULL;

	short nSupported;
	spRenderObject->GenerateThumbnail( ptrObject, 24, 0, 0, 
				CSize(rcImage.Width(), rcImage.Height() ), 
				&nSupported, &pbi );

	if( pbi )
	{
		::ThumbnailDraw( pbi, pdc, 
				CRect(	rcImage.left, rcImage.top,
						rcImage.right, rcImage.bottom
						), 0
					);
		::ThumbnailFree( pbi );
		pbi = NULL;
	}
	*/

	if( m_bAutoLabel )
	{		

		COLORREF oldColor = pdc->SetBkColor( ::GetSysColor(COLOR_3DFACE) );
		CFont* pOldFont = pdc->SelectObject( &m_Font );
		CRect rc_text = rcClient;
		rc_text.DeflateRect( 3, 3, 0, 0 );
		pdc->DrawText( GetCaption(), &rc_text, DT_LEFT );
		//pdc->TextOut( rcClient.left + 3, rcClient.top + 3 , GetCaption() );	
		pdc->SelectObject( pOldFont );
		pdc->SetBkColor( ::GetSysColor(oldColor) );

	}
	

	IPrintViewPtr ptrView( m_ctrlAXView.GetControlUnknown() );
	if( ptrView )
	{
		int nout_width = rcImage.Width();
		int nout_height = rcImage.Height();

		int nWidth=nout_width, nUserPosX = 0;
		int nHeight=nout_height, nUserPosY = 0;

		BOOL bContinue = FALSE;

		ptrView->GetPrintWidth( 1000000, &nWidth, &bContinue, 0, &nUserPosX );
		ptrView->GetPrintHeight( 1000000, &nHeight, &bContinue, 0, &nUserPosY );

		CSize sizeTarget, sizeThumbnail;

		sizeTarget.cx = nWidth;
		sizeTarget.cy = nHeight;

		sizeThumbnail.cx = nout_width;
		sizeThumbnail.cy = nout_height;

		DrawControlMode dcm = m_DrawControlMode.dcm;
		if (dcm != dcmAlreadyZoom && m_DrawControlMode.nLimitProc > 0 &&
			nout_width >=  m_DrawControlMode.nLimitProc*nWidth/100 &&
			nout_height >= 70*m_DrawControlMode.nLimitProc/100)
			dcm = dcmAlreadyZoom;
		if (dcm == dcmPicture && m_DrawControlMode.bmp == NULL)
			dcm = dcmAlreadyZoom;

		if (dcm == dcmAlreadyZoom)
		{
			CSize size_res = ThumbnailGetRatioSize( sizeThumbnail, sizeTarget );		
			rcImage.left	= rcImage.left + nout_width / 2 - size_res.cx / 2;
			rcImage.top		= rcImage.top + nout_height / 2 - size_res.cy / 2;
			rcImage.right	= rcImage.left + size_res.cx;
			rcImage.bottom	= rcImage.top + size_res.cy;
			ptrView->Print( pdc->GetSafeHdc(), rcImage, 0, 0, nUserPosX, nUserPosY, TRUE, 1 );
		}
		else if (dcm == dcmTopLeft)
		{
#if 1
			int nSourceDX,nSourceDY;
			CRect rcTarget(rcImage.left,rcImage.top,0,0);
			if (nout_width > 70*nWidth/100)
			{ // image fully fit horizontally into control rectangle
				nSourceDX = nUserPosX;
				rcTarget.right = rcTarget.left+70*nWidth/100;
			}
			else
			{ // draw only left part of view
				nSourceDX = 100*nout_width/70;
				rcTarget.right = rcTarget.left+nout_width;
			}
			if (nout_height > 70*nHeight/100)
			{ // image fully fit vertically into control rectangle
				nSourceDY = nUserPosY;
				rcTarget.bottom = rcTarget.top+70*nHeight/100;
			}
			else
			{ // draw only upper part of view
				nSourceDY = 100*nout_height/70;
				rcTarget.bottom = rcTarget.top+nout_height;
			}
			ptrView->Print( pdc->GetSafeHdc(), rcTarget, 0, 0, nSourceDX, nSourceDY, TRUE, 1 );
#else
			CRect rcTarget(rcImage.left,rcImage.top,rcImage.left+70*nWidth/100,rcImage.top+70*nHeight/100);
			ptrView->Print( pdc->GetSafeHdc(), rcTarget, 0, 0, nUserPosX, nUserPosY, TRUE, 1 );
#endif
		}
		else if (dcm == dcmPicture)
		{
			BITMAP bm;
			::GetObject(m_DrawControlMode.bmp, sizeof(bm), &bm);
			if (m_DrawControlMode.bZoomPicture || bm.bmWidth > rcImage.Width() || bm.bmHeight > rcImage.Height())
			{
				CSize size_res = ThumbnailGetRatioSize( sizeThumbnail, CSize(bm.bmWidth,bm.bmHeight) );
				rcImage.left	= rcImage.left + nout_width / 2 - size_res.cx / 2;
				rcImage.top		= rcImage.top + nout_height / 2 - size_res.cy / 2;
				rcImage.right	= rcImage.left + size_res.cx;
				rcImage.bottom	= rcImage.top + size_res.cy;
			}
			else
			{
				rcImage.left	= rcImage.left + nout_width / 2 - bm.bmWidth / 2;
				rcImage.top		= rcImage.top + nout_height / 2 - bm.bmHeight / 2;
				rcImage.right	= rcImage.left + bm.bmWidth;
				rcImage.bottom	= rcImage.top + bm.bmHeight;
			}
			HDC	hdcM = ::CreateCompatibleDC( pdc->m_hDC );
			::SelectObject( hdcM, m_DrawControlMode.bmp );
			int nPrevMode = ::SetStretchBltMode( pdc->m_hDC, m_DrawControlMode.nPictureZoomMode );
			::StretchBlt( pdc->m_hDC, rcImage.left, rcImage.top, rcImage.Width(), rcImage.Height(), hdcM,
				0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY );
			::SetStretchBltMode( pdc->m_hDC, nPrevMode);
			::DeleteDC( hdcM );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDBObject::DoPropExchange - Persistence support
void CDBObject::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CDBObject::OnResetState - Reset control to default state

void CDBObject::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	//CDBControlImpl::InitDefaults();	
}


/////////////////////////////////////////////////////////////////////////////
// CDBObject::AboutBox - Display an "About" box to the user

void CDBObject::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_DBDATETIME);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////

void CDBObject::OnPropertyChange()
{
	SetModifiedFlag( TRUE );
	if( ::IsWindow( GetSafeHwnd() ) )
		InvalidateControl( );
}



/////////////////////////////////////////////////////////////////////////////
// Redeclaration class used in MFC - copy from <ctlimpl.h>
/////////////////////////////////////////////////////////////////////////////
// CArchivePropExchange - for persistence in an archive.

class CArchivePropExchange : public CPropExchange
{
// Constructors
public:
	CArchivePropExchange(CArchive& ar);

// Operations
	virtual BOOL ExchangeProp(LPCTSTR pszPropName, VARTYPE vtProp,
				void* pvProp, const void* pvDefault = NULL);
	virtual BOOL ExchangeBlobProp(LPCTSTR pszPropName, HGLOBAL* phBlob,
				HGLOBAL hBlobDefault = NULL);
	virtual BOOL ExchangeFontProp(LPCTSTR pszPropName, CFontHolder& font,
				const FONTDESC* pFontDesc, LPFONTDISP pFontDispAmbient);
	virtual BOOL ExchangePersistentProp(LPCTSTR pszPropName,
				LPUNKNOWN* ppUnk, REFIID iid, LPUNKNOWN pUnkDefault);

// Implementation
protected:
	CArchive& m_ar;
};



void CDBObject::Serialize(CArchive& ar)
{
	// A.M. fix GPF then pPropNotifySink==NULL (on DBBlank change control property an press undo).
//	COleControl::Serialize(ar);	
	CArchivePropExchange px(ar);
	DoPropExchange(&px);
	if (ar.IsLoading())
	{
		POSITION pos = m_xPropConnPt.GetStartPosition();
		LPPROPERTYNOTIFYSINK pPropNotifySink;
		while (pos != NULL)
		{
			pPropNotifySink =
				(LPPROPERTYNOTIFYSINK)m_xPropConnPt.GetNextConnection(pos);
			if (pPropNotifySink)
				pPropNotifySink->OnChanged(DISPID_UNKNOWN);
		}
		InvalidateControl();
	}


	long nVersion = 4;
	if( ar.IsStoring() )
	{
		ar << nVersion;
		ar << m_strViewProgID;
		ar << m_bAutoViewType;
		ar << m_fontDesc;
	}
	else
	{
		ar >> nVersion;
		if( nVersion >= 2 )
			ar >> m_strViewProgID;
		
		if( nVersion >= 3 )
			ar >> m_bAutoViewType;

		if( nVersion >= 4 )
		{	
			ar >> m_fontDesc;
			m_fontHolder.ReleaseFont();
			m_fontHolder.InitializeFont( &m_fontDesc );	
		}
	}
	
	CDBControlImpl::IDBControl_Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::IDBControl_OnSetTableName(  )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::IDBControl_OnSetFieldName(  )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::IDBControl_OnSetEnableEnumeration( )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::IDBControl_OnSetAutoLabel( )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
// [vanek] BT2000: 3510 - 26.01.2004
void CDBObject::IDBControl2_OnSetReadOnly( )
{
	OnPropertyChange( );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBObject::GetAutoLabel() 
{	
	return IDBControl_GetAutoLabel( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::SetAutoLabel(BOOL bNewValue) 
{
	IDBControl_SetAutoLabel( bNewValue );	
	//m_ctrlDropDownEdit.SetAutoLabel( CDBControlImpl::m_bAutoLabel == TRUE );	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBObject::GetEnableEnumeration() 
{	
	return IDBControl_GetEnableEnumeration( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::SetEnableEnumeration(BOOL bNewValue) 
{	
	IDBControl_SetEnableEnumeration( bNewValue );
	//m_ctrlDropDownEdit.SetEnumeration( CDBControlImpl::m_bEnableEnumeration == TRUE );
}

/////////////////////////////////////////////////////////////////////////////
BSTR CDBObject::GetTableName() 
{
	CString strResult = IDBControl_GetTableName();	
	return strResult.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::SetTableName(LPCTSTR lpszNewValue) 
{	
	IDBControl_SetTableName( lpszNewValue );	
}

/////////////////////////////////////////////////////////////////////////////
BSTR CDBObject::GetFieldName() 
{
	CString strResult = IDBControl_GetFieldName( );
	return strResult.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::SetFieldName(LPCTSTR lpszNewValue) 
{	
	IDBControl_SetFieldName( lpszNewValue );
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::IDBControl_GetValue( tagVARIANT *pvar )
{
	CComVariant var(m_ViewSubType);
	*pvar = var;
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::IDBControl_SetValue( const tagVARIANT var )
{
	if(VT_UI4==var.vt){
		m_ViewSubType=var.lVal;
		if(m_ViewSubType>0)
		{
			if(IsWindow(m_ctrlAXView))
			{
				if(INamedPropBagPtr pNamedPropBag=m_ctrlAXView.GetControlUnknown())
				{
					HRESULT hr=pNamedPropBag->SetProperty(CComBSTR(L"Views"),CComVariant(m_ViewSubType));
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
short CDBObject::GetLabelPosition() 
{	
	return IDBControl_GetLabelPosition( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::SetLabelPosition(short nNewValue) 
{	
	IDBControl_SetLabelPosition( nNewValue );
	//m_ctrlDropDownEdit.SetLabelPosition( IDBControl_GetLabelPosition() );
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::IDBControl_OnSetLabelPosition( )
{
	OnPropertyChange();
}


/////////////////////////////////////////////////////////////////////////////
void CDBObject::SetEnableWindow( bool bEnable )
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	EnableWindow( bEnable == true );
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::EnterViewMode( BlankViewMode newBlankMode )
{
	if( !::IsWindow( m_ctrlAXView ) )
		return;
	
	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::IDBControl_ChangeValue( const tagVARIANT var )
{
	
}

void CDBObject::GetValue(sptrIDBaseDocument spDBaseDoc)
{	
	if( spDBaseDoc == NULL )
	{
		spDBaseDoc = m_spDBaseDocument;
		if( spDBaseDoc == NULL )
			return;
	}

	SetViewName();		
	
	
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var )
{	

	if( !m_bCanRecieveNotify )
		return;
	if( punkDBaseDocument == NULL )
		return;

	sptrIDBaseDocument spDBaseDoc( punkDBaseDocument );
	if( spDBaseDoc == NULL )
		return;


	// A.M. fix. BT 4554
	bool bPreviewView = false;
	IDocumentSitePtr sptrDS(punkDBaseDocument);
	if (sptrDS != 0)
	{
		IUnknownPtr punkView;
		sptrDS->GetActiveView(&punkView);
		IReportView2Ptr sptrRV2(punkView);
		if (sptrRV2 != 0)
			bPreviewView = true;
	}

	CString strEvent = pszEvent;
	if( strEvent == szDBaseEventAfterQueryOpen ||
		strEvent == szDBaseEventBeforQueryClose ||
		strEvent == szDBaseEventAfterNavigation && !bPreviewView||
		strEvent == szDBaseEventAfterInsertRecord ||
		strEvent == szDBaseEventAfterDeleteRecord ||
		(strEvent == szDBaseEventFieldChange &&
		bstrTableName == IDBControl_GetTableName() &&
		bstrFieldName == IDBControl_GetFieldName()
		)
		)
	{
		SetViewName();
		Invalidate();
	}


	if( !strcmp( pszEvent, szDBaseEventCaptionChange ) && 
		IDBControl_GetTableName() == bstrTableName &&
		IDBControl_GetFieldName() == bstrFieldName
		)				
	{
		Resize();
		Invalidate();
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBObject::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::SetViewName()
{
	m_ar_objects.RemoveAll();

	if( !::IsWindow( (m_ctrlAXView.GetSafeHwnd() ) ) )
		return;

	
	//if( GetReportViewMode() == vmDesign || GetReportViewMode() == vmNoInfo )
	//	return;
		

	IUnknown* pUnkCtrl = m_ctrlAXView.GetControlUnknown();
	if( pUnkCtrl == NULL )
		return;
	IVtActiveXCtrl2Ptr	sptrVTC( pUnkCtrl );//pwnd->GetControlSite()->m_pObject);

	IViewCtrlPtr ptrViewCtrl( pUnkCtrl );
	if( ptrViewCtrl )
	{	
		ptrViewCtrl->DestroyView();
		ptrViewCtrl->SetAutoBuild( FALSE );
		ptrViewCtrl->SetUseActiveView( FALSE );
		ptrViewCtrl->SetViewAutoAssigned( m_bAutoViewType );
		ptrViewCtrl->SetObjectTransformation( (int)otGrowTwoSide );		
		ptrViewCtrl->SetViewProgID( _bstr_t( (LPCSTR)m_strViewProgID ) );

		if( m_strViewProgID.IsEmpty() )
			ptrViewCtrl->SetViewAutoAssigned( TRUE );			


		CString strObject;
		strObject.Format( "%s.%s", 
			(LPCTSTR)IDBControl_GetTableName(), (LPCTSTR)IDBControl_GetFieldName() );		

		long nPos = 1;
		while( nPos )
		{
			ptrViewCtrl->GetFirstObjectPosition( &nPos );
			if( nPos )
				ptrViewCtrl->RemoveAt( nPos );
		}
		
		IUnknown* punkObject = ::FindObjectByName( m_spDBaseDocument, strObject );
		if( punkObject )
		{	
			GUID guidBaseObject = ::GetObjectKey( punkObject );

			CString strObjectType = ::GetObjectKind( punkObject );			
			
			ptrViewCtrl->InsertAfter( 0, _bstr_t( (LPCTSTR)strObject ), FALSE, 
										_bstr_t( (LPCTSTR)strObjectType ) );

			m_ar_objects.Add( strObject );

			IDataContext2Ptr ptrDC( m_spDBaseDocument );
			if( ptrDC )
			{
				long ltype_count = 0;
				ptrDC->GetObjectTypeCount( &ltype_count );
				for( long i=0;i<ltype_count;i++ )
				{
					BSTR bstr = 0;
					ptrDC->GetObjectTypeName( i, &bstr );

					_bstr_t bstrType = bstr;

					if( bstr )
						::SysFreeString( bstr );	bstr = 0;

					long lpos = 0;
					ptrDC->GetFirstObjectPos( bstrType, &lpos );
					while( lpos )
					{
						IUnknown* punk = 0;
						ptrDC->GetNextObject( bstrType, &lpos, &punk );
						if( !punk ) continue;

						GUID guid_db_parent = INVALID_KEY;
						BOOL bdb_object = FALSE;
						BOOL bempty = FALSE;
						m_spDBaseDocument->IsDBaseObject( punk, &bdb_object, &guid_db_parent, &bempty );

						if( ::GetObjectKey( punk ) != guidBaseObject &&
							guid_db_parent == guidBaseObject &&
							!bempty && bdb_object
							)
						{
							CString strChildName = ::GetObjectName( punk );
							CString strChildType = ::GetObjectKind( punk );

							ptrViewCtrl->InsertAfter( 0, _bstr_t( (LPCTSTR)strChildName ), FALSE, 
										_bstr_t( (LPCTSTR)strChildType ) );

							m_ar_objects.Add( strChildName );
						}

						punk->Release();	punk = 0;
					}
				}				
			}	


			Resize();

			if(m_ViewSubType>0){
				if(INamedPropBagPtr pPropBag=ptrViewCtrl)
				{
					pPropBag->SetProperty(CComBSTR(L"Views"), CComVariant(m_ViewSubType));
				}
			}

			ptrViewCtrl->Build( 0 );
			punkObject->Release();
		}

 		Resize();		
	}

	FireCtrlPropChange( GetControllingUnknown() );
	
	pUnkCtrl = 0;	

	// A.M. fix. SBT1406. Appointment works approperiately only when object transformation is stretch
	CString sViewName = ::GetObjectName(m_ctrlAXView.GetControlUnknown());
	if( sViewName == _T("Apportionment") && ptrViewCtrl != 0)
		ptrViewCtrl->SetObjectTransformation( (int)otStretch );		
	InitDrawControlMode();
}

/////////////////////////////////////////////////////////////////////////////
BSTR CDBObject::GetViewType() 
{
	CString strResult = GetViewNameByProgID( m_strViewProgID, true );
	return strResult.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::SetViewType(LPCTSTR lpszNewValue) 
{	
	CString sRes=GetObjTypeByID(m_strViewProgID);
	BSTR oldObjectDB=sRes.AllocSysString();
	m_strViewProgID = GetViewProgIDByName( lpszNewValue );
	if(m_strViewProgID=="" && oldObjectDB)
	{
		int count=0;
		int res=-1;
		BOOL mFind=false;
		int OnOffView=0;
		CString s;
	
		CStringArray arViewProgID;
		GetAvailableViewProgID( arViewProgID );
		for( int i=0;i<arViewProgID.GetSize();i++ )
		{
			try
			{
				BSTR bstr = arViewProgID[i].AllocSysString();
				BSTR bstr_old=bstr;
				IViewPtr ptrV( bstr );

				if( bstr )
					::SysFreeString( bstr );	bstr = 0;

				DWORD dwMatch = mvNone; 
				ptrV->GetMatchType( oldObjectDB, &dwMatch );
				if( !( dwMatch & mvFull ) )
					continue;
				
				CString SOne,STwo,SThree;
				SOne.Format("DBViewFilter\\%s",sRes);
				STwo=(CString)(bstr_old);
				mFind=false;
				count=0;
				res=-1;
				OnOffView=0;
				while(!mFind)
				{
					count=STwo.Find('.',res+1);
					if(count>=0)
						res=count;
					else
						mFind=true;
				}
				if(mFind)
				{
					STwo.Delete(0,res+1);
					SThree.Format("Name%s",STwo);
				}
				s = ::GetValueString(::GetAppUnknown(), SOne, SThree, "");
				if( !s.IsEmpty() )
				{
					if(s==lpszNewValue)
						m_strViewProgID=(CString)(bstr_old);
				}
			}
			catch(...)
			{

			}
		}
	}
	if(oldObjectDB)
		::SysFreeString(oldObjectDB);
	oldObjectDB=0;
	SetModifiedFlag();
}


IUnknown* CDBObject::GetViewAXControlUnknown()
{
	if( !::IsWindow( m_ctrlAXView.GetSafeHwnd() ) )
		return NULL;

	return m_ctrlAXView.GetControlUnknown();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBObject::GetAutoViewType()
{
	return m_bAutoViewType;
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::SetAutoViewType(BOOL bNewValue)
{
	m_bAutoViewType = bNewValue;
	SetModifiedFlag();
}


/////////////////////////////////////////////////////////////////////////////
//
//
//	IViewCtrl Interface pool
//
//
/////////////////////////////////////////////////////////////////////////////
#define START_VIEWAX_IMPL											\
	IUnknown* pUnkViewAXCtrl = pThis->GetViewAXControlUnknown();	\
	if( pUnkViewAXCtrl == NULL )									\
		return S_FALSE;												\
																	\
	sptrIViewCtrl spViewCtrl( pUnkViewAXCtrl );						\
	if( spViewCtrl == NULL )										\
		return S_FALSE;												\


#define END_VIEWAX_IMPL				\
	pUnkViewAXCtrl = 0;				\

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::GetApp(IUnknown** ppunkApp)
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->GetApp( ppunkApp );
	END_VIEWAX_IMPL	
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::GetDoc(IUnknown** ppunkDoc)
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->GetDoc( ppunkDoc );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::GetAutoBuild( BOOL* pbAutoBuild )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->GetAutoBuild( pbAutoBuild );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::SetAutoBuild( BOOL bAutoBuild )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->SetAutoBuild( bAutoBuild );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::Build( BOOL* pbSucceded )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL
	pThis->SetViewName();
	pThis->Resize();
	HRESULT hr = spViewCtrl->Build( pbSucceded );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::GetDPI( double* pfDPI )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->GetDPI( pfDPI );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::SetDPI( double fDPI )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->SetDPI( fDPI );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::GetZoom( double* pfZoom )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->GetZoom( pfZoom );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::SetZoom( double fZoom )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->SetZoom( fZoom );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::GetObjectTransformation( short* pnObjectTransformation )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->GetObjectTransformation( pnObjectTransformation );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::SetObjectTransformation( short nObjectTransformation )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->SetObjectTransformation( nObjectTransformation );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::GetUseActiveView( BOOL* pbUseActiveView )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	*pbUseActiveView = FALSE;
	//HRESULT hr = spViewCtrl->GetUseActiveView( pbUseActiveView );
	END_VIEWAX_IMPL	
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::SetUseActiveView( BOOL bUseActiveView )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->SetUseActiveView( bUseActiveView );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::GetViewAutoAssigned( BOOL* pbViewAutoAssigned )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	*pbViewAutoAssigned = TRUE;

	//HRESULT hr = spViewCtrl->GetViewAutoAssigned( pbViewAutoAssigned );
	END_VIEWAX_IMPL	
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::SetViewAutoAssigned( BOOL bViewAutoAssigned )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->SetViewAutoAssigned( bViewAutoAssigned );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::GetViewProgID( BSTR* pbstrProgID )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->GetViewProgID( pbstrProgID );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::SetViewProgID( BSTR bstrProgID )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->SetViewProgID( bstrProgID );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::GetFirstObjectPosition( long* plPos )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->GetFirstObjectPosition( plPos );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::GetNextObject( BSTR* pbstrObjectName, BOOL* pbActiveObject, 
								BSTR* bstrObjectType, long *plPos )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->GetNextObject( pbstrObjectName, pbActiveObject, 
								bstrObjectType, plPos );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::InsertAfter( long lPos,  
								BSTR bstrObjectName, BOOL bActiveObject, 
								BSTR bstrObjectType )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->InsertAfter( lPos,  
							bstrObjectName, bActiveObject, 
							bstrObjectType );
	END_VIEWAX_IMPL	
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::EditAt( long lPos,  
								BSTR bstrObjectName, BOOL bActiveObject, 
								BSTR bstrObjectType )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->EditAt( lPos,  
						bstrObjectName, bActiveObject, 
						bstrObjectType );
	END_VIEWAX_IMPL
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::RemoveAt( long lPos )
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->RemoveAt( lPos );
	END_VIEWAX_IMPL
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XViewCtrl::DestroyView()
{
	METHOD_PROLOGUE_EX(CDBObject, ViewCtrl)	
	START_VIEWAX_IMPL	
	HRESULT hr = spViewCtrl->DestroyView();
	END_VIEWAX_IMPL
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//	IPrintView Interface pool
//
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XPrintView::GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX )
{
	METHOD_PROLOGUE_EX(CDBObject, PrintView)

	*pnNewUserPosX = 0;

	*pbContinue = FALSE;

	IUnknown* pUnkViewAXCtrl = pThis->GetViewAXControlUnknown();
	if( pUnkViewAXCtrl == NULL )
		return S_FALSE;

	sptrIPrintView spPrintView( pUnkViewAXCtrl );
	if( spPrintView == NULL )
		return S_FALSE;

	spPrintView->GetPrintWidth( nMaxWidth, pnReturnWidth, pbContinue, nUserPosX, pnNewUserPosX );

	pUnkViewAXCtrl = 0;

	return S_OK;
}
HRESULT CDBObject::XPrintView::GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY )
{
	METHOD_PROLOGUE_EX(CDBObject, PrintView)

	*pnNewUserPosY = 0;

	*pbContinue = FALSE;

	IUnknown* pUnkViewAXCtrl = pThis->GetViewAXControlUnknown();
	if( pUnkViewAXCtrl == NULL )
		return S_FALSE;

	sptrIPrintView spPrintView( pUnkViewAXCtrl );
	if( spPrintView == NULL )
		return S_FALSE;

	spPrintView->GetPrintHeight( nMaxHeight, pnReturnHeight, pbContinue, nUserPosY, pnNewUserPosY );

	pUnkViewAXCtrl = 0;

	return S_OK;
}

CSize CDBObject::GetLabelSize( CDC* pdc )
{
	CSize sizeLeftOffset = CSize(0,0);	

	CClientDC dc( this );
	pdc = &dc;

	if( !m_bAutoLabel )
		return CSize( 0, 0 );

	{		
		CString srtCaption = GetCaption();
		CRect rcText;
		CFont* pOldFont = pdc->SelectObject( &m_Font );
		pdc->DrawText( srtCaption, rcText, DT_CALCRECT );
		pdc->SelectObject( pOldFont );

		if( m_nLabelPosition == 0 )
			sizeLeftOffset.cx = rcText.Width();
		else
			sizeLeftOffset.cy = rcText.Height();
	}

	return sizeLeftOffset;
}

HRESULT CDBObject::XPrintView::Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags )
{
	METHOD_PROLOGUE_EX(CDBObject, PrintView)

	IUnknown* pUnkViewAXCtrl = pThis->GetViewAXControlUnknown();
	if( pUnkViewAXCtrl == NULL )
		return S_FALSE;

	sptrIPrintView spPrintView( pUnkViewAXCtrl );
	if( spPrintView == NULL )
		return S_FALSE;

	/*
	if( nUserPosX == 0 && nUserPosY == 0 )
	{
		CSize sizeLabel = pThis->GetLabelSize();
		CRect rc = ;
	}
	*/

	spPrintView->Print( hdc, rectTarget, nUserPosX, nUserPosY, nUserPosDX, nUserPosDY, bUseScrollZoom==TRUE, dwFlags );

	pUnkViewAXCtrl = 0;

	return S_OK;
}
					 

/////////////////////////////////////////////////////////////////////////////
//
//
//	 IVtActiveXCtrl Interface pool
//
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XVtActiveXCtrl::SetSite( IUnknown *punkVtApp, IUnknown *punkSite )
{
	METHOD_PROLOGUE_EX(CDBObject, VtActiveXCtrl)

	IUnknown* pUnkViewAXCtrl = pThis->GetViewAXControlUnknown();
	if( pUnkViewAXCtrl == NULL )
		return S_FALSE;

	sptrIVtActiveXCtrl spVtActiveXCtrl( pUnkViewAXCtrl );
	if( spVtActiveXCtrl == NULL )
		return S_FALSE;

	spVtActiveXCtrl->SetSite( punkVtApp, punkSite );

	pUnkViewAXCtrl = 0;

	return S_OK;
}
HRESULT CDBObject::XVtActiveXCtrl::GetName( BSTR *pbstrName )
{
	METHOD_PROLOGUE_EX(CDBObject, VtActiveXCtrl)

	IUnknown* pUnkViewAXCtrl = pThis->GetViewAXControlUnknown();
	if( pUnkViewAXCtrl == NULL )
		return S_FALSE;

	sptrIVtActiveXCtrl spVtActiveXCtrl( pUnkViewAXCtrl );
	if( spVtActiveXCtrl == NULL )
		return S_FALSE;

	spVtActiveXCtrl->GetName( pbstrName );

	pUnkViewAXCtrl = 0;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//	 IVtActiveXCtrl2 Interface pool
//
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XVtActiveXCtrl2::SetSite( IUnknown *punkVtApp, IUnknown *punkSite )
{
	METHOD_PROLOGUE_EX(CDBObject, VtActiveXCtrl2)

	IUnknown* pUnkViewAXCtrl = pThis->GetViewAXControlUnknown();
	if( pUnkViewAXCtrl == NULL )
		return S_FALSE;

	sptrIVtActiveXCtrl2 spVtActiveXCtrl2( pUnkViewAXCtrl );
	if( spVtActiveXCtrl2 == NULL )
		return S_FALSE;

	spVtActiveXCtrl2->SetSite( punkVtApp, punkSite );

	pUnkViewAXCtrl = 0;

	return S_OK;
}
HRESULT CDBObject::XVtActiveXCtrl2::GetName( BSTR *pbstrName )
{
	METHOD_PROLOGUE_EX(CDBObject, VtActiveXCtrl2)

	IUnknown* pUnkViewAXCtrl = pThis->GetViewAXControlUnknown();
	if( pUnkViewAXCtrl == NULL )
		return S_FALSE;

	sptrIVtActiveXCtrl2 spVtActiveXCtrl2( pUnkViewAXCtrl );
	if( spVtActiveXCtrl2 == NULL )
		return S_FALSE;

	spVtActiveXCtrl2->GetName( pbstrName );

	pUnkViewAXCtrl = 0;

	return S_OK;
}
HRESULT CDBObject::XVtActiveXCtrl2::SetDoc( IUnknown *punkDoc )
{
	METHOD_PROLOGUE_EX(CDBObject, VtActiveXCtrl2)

	IUnknown* pUnkViewAXCtrl = pThis->GetViewAXControlUnknown();
	if( pUnkViewAXCtrl == NULL )
		return S_FALSE;

	sptrIVtActiveXCtrl2 spVtActiveXCtrl2( pUnkViewAXCtrl );
	if( spVtActiveXCtrl2 == NULL )
		return S_FALSE;

	spVtActiveXCtrl2->SetDoc( punkDoc );

	pUnkViewAXCtrl = 0;

	return S_OK;
}

HRESULT CDBObject::XVtActiveXCtrl2::SetApp( IUnknown *punkVtApp )
{
	METHOD_PROLOGUE_EX(CDBObject, VtActiveXCtrl2)

	IUnknown* pUnkViewAXCtrl = pThis->GetViewAXControlUnknown();
	if( pUnkViewAXCtrl == NULL )
		return S_FALSE;

	sptrIVtActiveXCtrl2 spVtActiveXCtrl2( pUnkViewAXCtrl );
	if( spVtActiveXCtrl2 == NULL )
		return S_FALSE;

	spVtActiveXCtrl2->SetApp( punkVtApp );

	pUnkViewAXCtrl = 0;

	return S_OK;
}

BOOL CDBObject::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{	
	return COleControl::OnNotify(wParam, lParam, pResult);
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XPrintCtrl::Draw( HDC hDC, SIZE size )
{
	METHOD_PROLOGUE_BASE(CDBObject, PrintCtrl)
	
	CDC* pdc = CDC::FromHandle( hDC );
	CRect rcBounds = CRect( 0, 0, size.cx, size.cy );

	CRect rcFill = rcBounds;
	CRect rcFill_bk = rcBounds;
	//CRect rcFill3 = rcBounds;
	rcFill.right -= 100;
	rcFill.top += 100;
	rcFill_bk.left += 100;
	rcFill_bk.bottom -= 100;

	pdc->FillRect( rcFill_bk, &CBrush::CBrush( ::GetSysColor(COLOR_3DSHADOW)));
	pdc->FillRect( rcFill, &CBrush::CBrush( ::GetSysColor(COLOR_3DFACE)));	

	if( pThis->m_bAutoLabel )
	{		
		LOGFONT lf = {0};
		HFONT hFont = pThis->m_fontHolder.GetFontHandle( );
		CFont* pFont = CFont::FromHandle( hFont );
		pFont->GetLogFont( &lf );
		double fAspect = 1.0 / 72.0 * 25.2 * 10;
	
		IFontPtr fontPtr( pThis->m_fontHolder.m_pFont );

		if( fontPtr != 0 )
		{
			CY		sizeCY;
			fontPtr->get_Size( &sizeCY );
			lf.lfHeight = (int)(((double)sizeCY.Lo)/(1000.0) * fAspect);
		}	

		CFont fontToPrint;			

		fontToPrint.CreateFont( 
			lf.lfHeight, 0, 
			0, 0, 
			lf.lfWeight, lf.lfItalic, 
			lf.lfUnderline, lf.lfStrikeOut, 
			lf.lfCharSet,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,DEFAULT_PITCH,						
			lf.lfFaceName 
			);
	
		CFont* pOldFont = pdc->SelectObject( &fontToPrint );
		CRect rc_text = rcBounds;
		rc_text.left = 100;
		rc_text.bottom = rcBounds.top + 100;
		rc_text.top = rcBounds.bottom;
		CorrectHimetricsRect( &rc_text, &rcBounds );
		pdc->SetBkMode( TRANSPARENT );
		pdc->DrawText( pThis->GetCaption(), &rc_text, DT_LEFT );
		pdc->SelectObject( pOldFont );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XDBObjectControl::SetViewType( BSTR bstrViewName )
{
	METHOD_PROLOGUE_BASE(CDBObject, DBObjectControl)
	
	pThis->SetViewType( _bstr_t(bstrViewName) );

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XDBObjectControl::GetViewType( BSTR* pbstrViewName )
{
	METHOD_PROLOGUE_BASE(CDBObject, DBObjectControl)

	if( !pbstrViewName )
		return E_POINTER;

	*pbstrViewName = pThis->GetViewType();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XDBObjectControl::SetAutoViewType( BOOL bAutoViewType )
{
	METHOD_PROLOGUE_BASE(CDBObject, DBObjectControl)

	pThis->SetAutoViewType( bAutoViewType );
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XDBObjectControl::GetAutoViewType( BOOL* pbAutoViewType )
{
	METHOD_PROLOGUE_BASE(CDBObject, DBObjectControl)
	
	if( !pbAutoViewType )
		return E_POINTER;

	*pbAutoViewType = pThis->GetAutoViewType();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBObject::XDBObjectControl::Build()
{
	METHOD_PROLOGUE_BASE(CDBObject, DBObjectControl)

	pThis->SetViewName();
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CDBObject::OnLButtonDblClk(UINT nFlags, CPoint point) 
{		
	COleControl::OnLButtonDblClk(nFlags, point);

	_bstr_t bstrViewName;
	{
		INamedObject2Ptr sptrNO2(m_ctrlAXView.GetControlUnknown());
		if (sptrNO2 != 0)
			sptrNO2->GetName(bstrViewName.GetAddress());
	}

	IScriptSitePtr	sptrSS(::GetAppUnknown());
	if (sptrSS != 0)
	{
		_bstr_t bstrEvent("DBBlank_OnLButtonDblClk");
		_variant_t var(bstrViewName);
		sptrSS->Invoke(bstrEvent, &var, 1, 0, fwAppScript);
	}

	ChangeViewType();	
}

/////////////////////////////////////////////////////////////////////////////
bool CDBObject::ChangeViewType()
{
	IViewCtrlPtr ptrV( GetViewAXControlUnknown() );
	if( ptrV == 0 )		return false;

	BSTR bstr = 0;
	ptrV->GetViewProgID( &bstr );
	if( bstr == 0 )		return false;

	_bstr_t bstr_prog_id = bstr;
	::SysFreeString( bstr );	bstr = 0;

	IApplicationPtr ptrApp( ::GetAppUnknown() );
	if( ptrApp == 0 )	return false;

	IUnknown* punk = 0;
	ptrApp->GetActiveDocument( &punk );

	if( !punk )			return false;

	IDocumentSitePtr ptrDS( punk );
	punk->Release();	punk = 0;

	if( ptrDS == 0 )	return false;

	ptrDS->GetActiveView( &punk );
	if( !punk )			return false;


	IViewSitePtr ptrVS( punk );
	punk->Release();	punk = 0;


	ptrVS->GetFrameWindow( &punk );
	if( !punk )			return false;

	sptrIFrameWindow	sptrF( punk );
	punk->Release();	punk = 0;


	sptrF->GetSplitter( &punk );
	if( !punk )			return false;

	sptrISplitterWindow	sptrS( punk );
	punk->Release();	punk = 0;	
		
	int	nRow, nCol;
	nRow = nCol = 0;

	sptrS->GetActivePane( &nRow, &nCol );
	sptrS->ChangeViewType( nRow, nCol, bstr_prog_id );

	sptrS->GetViewRowCol( nRow, nCol, &punk );
	if( !punk )			return false;

	IDataContext2Ptr	ptrContext( punk );
	punk->Release();	punk = 0;

	if( ptrContext == 0 )
		return false;

	CString strObject;
	strObject.Format( "%s.%s", 
		(LPCTSTR)IDBControl_GetTableName(), (LPCTSTR)IDBControl_GetFieldName() );		

	for( int i=0; i<m_ar_objects.GetSize(); i++ )
	{
		IUnknown* punkObj = ::GetObjectByName( ptrDS, m_ar_objects[i] );
		if( punkObj )
		{
			ptrContext->SetObjectSelect( punkObj, TRUE );
			punkObj->Release();
		}
	}
	

	return true;
}
void CDBObject::OnDestroy()
{
	UnRegister( m_spDBaseDocument );
	__super::OnDestroy();
}

void CDBObject::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szEventChangeObject ) || !strcmp( pszEvent, szEventChangeObjectList ) )
	{
		if( GetObjectName( punkFrom ) == get_object_name() )
		{
			Invalidate();
		}
	}
}

CString	CDBObject::get_object_name()
{
	CString strObject;
	strObject.Format( "%s.%s", 
		(LPCTSTR)IDBControl_GetTableName(), (LPCTSTR)IDBControl_GetFieldName() );		
	return strObject;
}

//added by akm 22_11_k5
BOOL CDBObject::GetPropertyAXStatus() 
{
	return m_PropertyAXStatus;
}

void CDBObject::SetPropertyAXStatus(BOOL val) 
{		
	m_PropertyAXStatus=val;
}