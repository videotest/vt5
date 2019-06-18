// VTList.cpp : Implementation of the CVTList ActiveX Control class.

#include "stdafx.h"
#include "VTControls.h"
#include "VTList.h"
#include "VTListPpg.h"
#include "FramesPropPage.h"

#include "utils.h"
#include <msstkppg.h>
#include "VTListPropPageHeader.h"

//#define STRICT				//Определение строгой проверки типов
#include <windows.h>		//Файл определений функций windows
#include <windowsx.h>		//Файл макроопределений windows
#include <commctrl.h>		//Функции работы со стандартными окнами windows

#pragma comment(lib, "comctl32.lib")
LRESULT CALLBACK HeaderExWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void HeaderEx_Title(HWND hwnd);
HWND APIENTRY CreateToolTip(HWND hWndParent); 
void APIENTRY FillInToolInfo(TOOLINFO* ti, HWND hWnd, UINT nIDTool = 0); 
BOOL APIENTRY AddTool(HWND hTip, HWND hWnd, RECT* pr = NULL, UINT nIDTool = 0, LPCTSTR szText = NULL); 
void APIENTRY UpdateTipText(HWND hTip, HWND hWnd, UINT nIDTool = 0, LPCTSTR lpszText = NULL);
void APIENTRY GetTipText(HWND hTip, HWND hWnd, UINT nIDTool, LPSTR szText);
void APIENTRY EnableToolTip(HWND hTip, BOOL activate);
BOOL APIENTRY DestroyToolTip(HWND hWnd);

/*
	Функция облегчения задания ListView'у навороченного хедера
 */
WNDPROC InitListViewEx(HWND hWndLV);

/*
	Функция задания окошку SysHeader32 навороченного стиля
 */
WNDPROC InitHeaderEx(HWND hWndHDR);
int cyHeader=0;
int levelHeader=0;
BOOL b_createToolTip=false;
BOOL bExtHead=false;
CToolTipCtrl m_tooltip;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

inline CString _change_chars( CString str, CString strA, CString strB )
{
	CString strRet;

	long lPrev = 0;
	long lPos = str.Find( strA, lPrev );

	while( lPos != -1 )
	{
		strRet += str.Mid( lPrev, lPos - lPrev ); 
		strRet +=  strB;
		lPos += strA.GetLength();
		lPrev = lPos;

		lPos = str.Find( strA, lPrev );
	}

	if( lPos == -1 )
		strRet += str.Right( str.GetLength() - lPrev );

	return strRet;
}

/////////////////////////////////////////////////////////////////////////////
// CListEditView




IMPLEMENT_SERIAL(CVTList, COleControl, 1)

#ifndef FOR_HOME_WORK
BEGIN_INTERFACE_MAP(CVTList, COleControl)
	INTERFACE_PART(CVTList, IID_IVtActiveXCtrl, ActiveXCtrl)
	INTERFACE_PART(CVTList, IID_IVtActiveXCtrl2, ActiveXCtrl)
	INTERFACE_PART(CVTList, IID_IVtActiveXCtrl3, ActiveXCtrl)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CVTList, ActiveXCtrl)
#endif

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTList, COleControl)
	//{{AFX_MSG_MAP(CVTList)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_DESTROY()

	//---------  For tolltip -------------
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTList, COleControl)
	//{{AFX_DISPATCH_MAP(CVTList)
	DISP_PROPERTY_NOTIFY(CVTList, "UseGrid", m_useGrid, OnUseGridChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CVTList, "UseFullRowSelect", m_useFullRowSelect, OnUseFullRowSelectChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CVTList, "AutoUpdate", m_AutoUpdate, OnAutoUpdateChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CVTList, "Enabled", m_enabled, OnEnabledChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CVTList, "Name", m_strName, OnNameChanged, VT_BSTR)
	DISP_PROPERTY_EX(CVTList, "InnerRaised", GetInnerRaised, SetInnerRaised, VT_BOOL)
	DISP_PROPERTY_EX(CVTList, "InnerSunken", GetInnerSunken, SetInnerSunken, VT_BOOL)
	DISP_PROPERTY_EX(CVTList, "OuterRaised", GetOuterRaised, SetOuterRaised, VT_BOOL)
	DISP_PROPERTY_EX(CVTList, "OuterSunken", GetOuterSunken, SetOuterSunken, VT_BOOL)
	DISP_FUNCTION(CVTList, "GetColumnBackColor", GetColumnBackColor, VT_COLOR, VTS_I2)
	DISP_FUNCTION(CVTList, "GetColumnForeColor", GetColumnForeColor, VT_COLOR, VTS_I2)
	DISP_FUNCTION(CVTList, "GetColumnCaption", GetColumnCaption, VT_BSTR, VTS_I2)
	DISP_FUNCTION(CVTList, "GetColumnType", GetColumnType, VT_I2, VTS_I2)
	DISP_FUNCTION(CVTList, "GetColumnAlign", GetColumnAlign, VT_I2, VTS_I2)
	DISP_FUNCTION(CVTList, "GetColumnCount", GetColumnCount, VT_I2, VTS_NONE)
	DISP_FUNCTION(CVTList, "GetColumnWidth", GetColumnWidth, VT_I2, VTS_I2)
	DISP_FUNCTION(CVTList, "GetColumnFont", GetColumnFont, VT_VARIANT, VTS_I2)
	DISP_FUNCTION(CVTList, "SetColumnAlign", SetColumnAlign, VT_EMPTY, VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "SetColumnBackColor", SetColumnBackColor, VT_EMPTY, VTS_I2 VTS_COLOR)
	DISP_FUNCTION(CVTList, "SetColumnForeColor", SetColumnForeColor, VT_EMPTY, VTS_I2 VTS_COLOR)
	DISP_FUNCTION(CVTList, "SetColumnType", SetColumnType, VT_EMPTY, VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "SetColumnWidth", SetColumnWidth, VT_EMPTY, VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "SetColumnCaption", SetColumnCaption, VT_EMPTY, VTS_I2 VTS_BSTR)
	DISP_FUNCTION(CVTList, "SetColumnFont", SetColumnFont, VT_EMPTY, VTS_I2 VTS_VARIANT)
	DISP_FUNCTION(CVTList, "AddColumn", AddColumn, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CVTList, "DeleteColumn", DeleteColumn, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CVTList, "GetDisplayLogicalAsText", GetDisplayLogicalAsText, VT_I2, VTS_I2)
	DISP_FUNCTION(CVTList, "SetDisplayLogicalAsText", SetDisplayLogicalAsText, VT_EMPTY, VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "GetCurentColumn", GetCurentColumn, VT_I2, VTS_NONE)
	DISP_FUNCTION(CVTList, "GetRowCount", GetRowCount, VT_I2, VTS_NONE)
	DISP_FUNCTION(CVTList, "IsColumnUseIcon", IsColumnUseIcon, VT_I2, VTS_I2)
	DISP_FUNCTION(CVTList, "SetColumnUseIcon", SetColumnUseIcon, VT_EMPTY, VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "GetCellAsString", GetCellAsString, VT_BSTR, VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "DeleteRow", DeleteRow, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CVTList, "AddRow", AddRow, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CVTList, "SetIcon", SetIcon, VT_EMPTY, VTS_I2 VTS_I2 VTS_BSTR)
	DISP_FUNCTION(CVTList, "SetColumnLogicalIcon", SetColumnLogicalIcon, VT_EMPTY, VTS_I2 VTS_BSTR VTS_I2)
	DISP_FUNCTION(CVTList, "CurColumnLeft", CurColumnLeft, VT_I2, VTS_NONE)
	DISP_FUNCTION(CVTList, "CurColumnRight", CurColumnRight, VT_I2, VTS_NONE)
	DISP_FUNCTION(CVTList, "SetColumnReadOnly", SetColumnReadOnly, VT_EMPTY, VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "IsColumnReadOnly", IsColumnReadOnly, VT_I2, VTS_I2)
	DISP_FUNCTION(CVTList, "SetCellValue", SetCellValue, VT_EMPTY, VTS_I2 VTS_I2 VTS_BSTR)
	DISP_FUNCTION(CVTList, "GetKeyValue", GetKeyValue, VT_BSTR, VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "SetKeyValue", SetKeyValue, VT_EMPTY, VTS_I2 VTS_I2 VTS_BSTR)
	DISP_FUNCTION(CVTList, "GetCellKey", GetCellKey, VT_BSTR, VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "GetCellAsSubject", GetCellAsSubject, VT_BSTR, VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "GetCellAsColor", GetCellAsColor, VT_COLOR, VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "GetCellAsLogical", GetCellAsLogical, VT_I2, VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "GetCellAsValue", GetCellAsValue, VT_BSTR, VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "StoreToData", StoreToData, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CVTList, "GetCurentRow", GetCurentRow, VT_I2, VTS_NONE)
	DISP_FUNCTION(CVTList, "SetCurentRow", SetCurentRow, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CVTList, "SetCurentCol", SetCurentCol, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CVTList, "RestoreFromData", RestoreFromData, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX(CVTList, "Disabled", GetDisabled, SetDisabled, VT_BOOL)
	DISP_PROPERTY_EX(CVTList, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_FUNCTION(CVTList, "SetFocus", SetFocus, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX(CVTList, "EditBackColor", GetEditBackColor, SetEditBackColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTList, "EditTextColor", GetEditTextColor, SetEditTextColor, VT_COLOR)
  DISP_FUNCTION(CVTList, "ForceEditCell", ForceEditCell, VT_EMPTY, VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "SetCellAsLogical", SetCellAsLogical, VT_EMPTY, VTS_I2 VTS_I2 VTS_I2)	
	DISP_PROPERTY_EX(CVTList, "ListBkColor", GetListBkColor, SetListBkColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTList, "UseSystemFont", GetUseSystemFont, SetUseSystemFont, VT_BOOL)
	DISP_FUNCTION(CVTList, "SetCellValueColor", SetCellValueColor, VT_EMPTY, VTS_I2 VTS_I2 VTS_COLOR)
	DISP_FUNCTION(CVTList, "SetCellValueColorRGB", SetCellValueColorRGB, VT_EMPTY, VTS_I2 VTS_I2 VTS_BSTR)
	DISP_FUNCTION(CVTList, "GetCellAsColorRGB", GetCellAsColorRGB, VT_BSTR, VTS_I2 VTS_I2 )
	DISP_FUNCTION(CVTList, "ClearCombo", ClearCombo, VT_EMPTY, VTS_I2 VTS_I2 )
	DISP_FUNCTION(CVTList, "AddComboString", AddComboString, VT_BOOL, VTS_I2 VTS_I2 VTS_BSTR )
	DISP_FUNCTION(CVTList, "RemoveComboItem", RemoveComboItem, VT_BOOL, VTS_I2 VTS_I2 VTS_I2 )
	DISP_PROPERTY_EX(CVTList, "UseSimpleFontType", GetUseSimpleFontType, SetUseSimpleFontType, VT_BOOL)
	DISP_PROPERTY_EX(CVTList, "ActiveCellBackColor", GetActiveCellBackColor, SetActiveCellBackColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTList, "ActiveCellTextColor", GetActiveCellTextColor, SetActiveCellTextColor, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CVTList, "EnableResizeColumns", m_bEnableResizeColumns, OnEnableResizeColumnsChanged, VT_BOOL)
	DISP_FUNCTION(CVTList, "GetCellBackColor", GetCellBackColor, VT_COLOR, VTS_I2 VTS_I2 )
	DISP_FUNCTION(CVTList, "GetCellForeColor", GetCellForeColor, VT_COLOR, VTS_I2 VTS_I2 )
	DISP_FUNCTION(CVTList, "SetCellBackColor", SetCellBackColor, VT_EMPTY, VTS_I2 VTS_I2 VTS_COLOR )
	DISP_FUNCTION(CVTList, "SetCellForeColor", SetCellForeColor, VT_EMPTY, VTS_I2 VTS_I2 VTS_COLOR )

	DISP_FUNCTION(CVTList, "ClearExtHeaders", ClearExtHeaders, VT_EMPTY, VTS_NONE )
	DISP_FUNCTION(CVTList, "SetUseExtHeader", SetUseExtHeader, VT_EMPTY, VTS_I2 )
	DISP_FUNCTION(CVTList, "GetUseExtHeader", GetUseExtHeader, VT_I2, VTS_NONE )
	DISP_FUNCTION(CVTList, "GetExtHeadersSize", GetExtHeadersSize, VT_I2, VTS_NONE )
	DISP_FUNCTION(CVTList, "SetExtHeaderData", SetExtHeaderData, VT_EMPTY, VTS_I2 VTS_I2 VTS_BSTR VTS_I2 VTS_I2 VTS_I2)
	DISP_FUNCTION(CVTList, "GetExtHeaderName", GetExtHeaderName, VT_BSTR, VTS_I2 VTS_I2 )
	DISP_FUNCTION(CVTList, "GetExtHeaderAssignment", GetExtHeaderAssignment, VT_I4, VTS_I2 VTS_I2 )
	DISP_FUNCTION(CVTList, "GetExtHeadersColumnSize", GetExtHeadersColumnSize, VT_I4, VTS_I2 )
	DISP_FUNCTION(CVTList, "GetExtHeaderWidth", GetExtHeaderWidth, VT_I2, VTS_I2 VTS_I2 )

	DISP_FUNCTION(CVTList, "SetHideHeader", SetHideHeader, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CVTList, "SetHideBorder", SetHideBorder, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CVTList, "GetHideHeader", GetHideHeader, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CVTList, "GetHideBorder", GetHideBorder, VT_BOOL, VTS_NONE)

	DISP_FUNCTION(CVTList, "SetCellTypeShading", SetCellTypeShading, VT_EMPTY, VTS_I2 VTS_I2 VTS_I2 )
	DISP_FUNCTION(CVTList, "GetCellTypeShading", GetCellTypeShading, VT_I2, VTS_I2 VTS_I2 )
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CVTList, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()
/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTList, COleControl)
	//{{AFX_EVENT_MAP(CVTList)
	EVENT_CUSTOM("CurentColumnChange", FireCurentColumnChange, VTS_I2)
	EVENT_CUSTOM("CurentRowChange", FireCurentRowChange, VTS_I2)
	EVENT_CUSTOM("DataChange", FireDataChange, VTS_I2  VTS_I2)
	EVENT_CUSTOM("OnDeleteRow", FireOnDeleteRow, VTS_I2)
	EVENT_CUSTOM("OnDblClick", FireDblClick, VTS_I2  VTS_I2)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages
/*
// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CVTList, 6)
	PROPPAGEID(CVTListPropPage::guid)
	PROPPAGEID(CVTListPropPageCol::guid)
	PROPPAGEID(CVTListPropPageRow::guid)	
	if( ::GetValueInt( ::GetAppUnknown(), "\\VTListControl", "UseHeaderPane", 0 ) != 0 )
	{
		PROPPAGEID(CVTListPropPageHeader::guid )	
	}
	PROPPAGEID(CFramesPropPage::guid)
	PROPPAGEID(CLSID_StockColorPage)
END_PROPPAGEIDS(CVTList)
*/


LPCLSID CVTList::GetPropPageIDs( ULONG& cPropPages )
{
	static CLSID pages[6];
	cPropPages = 0;

	pages[cPropPages++] = CVTListPropPage::guid;
	pages[cPropPages++] = CVTListPropPageCol::guid;
	pages[cPropPages++] = CVTListPropPageRow::guid;
	
	if( _GetValueInt( ::GetAppUnknown(), "\\VTListControl", "UseHeaderPane", 0 ) != 0 )
		pages[cPropPages++] = CVTListPropPageHeader::guid;

	pages[cPropPages++] = CFramesPropPage::guid;
	pages[cPropPages++] = CLSID_StockColorPage;
	return pages;
}
/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTList, "VTCONTROLS.VTList.1",
	0xad7c61b8, 0x1356, 0x432f, 0xbe, 0xba, 0x1f, 0x94, 0x62, 0xa, 0x9f, 0x3d)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTList, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DVTList =
		{ 0x7850253b, 0x44e2, 0x44c5, { 0xa7, 0x28, 0x37, 0xda, 0xac, 0x3, 0x1f, 0x2a } };
const IID BASED_CODE IID_DVTListEvents =
		{ 0xbd593ddd, 0x9b97, 0x4717, { 0xa7, 0x7a, 0x40, 0x2f, 0xc, 0x4d, 0x22, 0x3a } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTListOleMisc =

	/*
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_ACTSLIKEBUTTON;
	*/
	

	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE|	
	OLEMISC_RECOMPOSEONRESIZE;
	
	
	
	

IMPLEMENT_OLECTLTYPE(CVTList, IDS_VTLIST, _dwVTListOleMisc)




/////////////////////////////////////////////////////////////////////////////
// CVTList::CVTListFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTList

BOOL CVTList::CVTListFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTLIST,
			IDB_VTLIST,
			afxRegApartmentThreading,
			_dwVTListOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


static int nReference = 0;

/////////////////////////////////////////////////////////////////////////////
// CVTList::CVTList - Constructor

CVTList::CVTList()
{
#ifndef FOR_HOME_WORK
	m_ptrSite = m_ptrApp = NULL;
#endif

	InitializeIIDs(&IID_DVTList, &IID_DVTListEvents);		

	// TODO: Initialize your control's instance data here.
	m_bUseExtHeader = false;
	m_bLock = false;
	nReference++;
	m_listDraw=false;

	m_HideHeader=false;
	m_HideBorder=false;
	flagHideHeader=false;
	m_zeroColumnWidth=0;

	cyHeader=0;
	b_createToolTip=false;
	bExtHead=false;
	levelHeader=0;
}


/////////////////////////////////////////////////////////////////////////////
// CVTList::~CVTList - Destructor

CVTList::~CVTList()
{
	// TODO: Cleanup your control's instance data here.
	//if( m_AutoUpdate )
	//	StoreToData();

	nReference--;
	CString strDebug;
	strDebug.Format( "Reference count:%d\n", nReference );
	//TRACE( strDebug );
}

static int font_size(int nHeight)
{
//	HDC dc = ::GetDC( 0 );
//	int nNewHeight =  -MulDiv( nHeight, 72, GetDeviceCaps( dc, LOGPIXELSY )) ;
//	::ReleaseDC( 0, dc );
//	return nNewHeight;
	return abs( nHeight );
}

static int font_height(int nHeight)
{
//	HDC dc = ::GetDC( 0 );
//	int nNewHeight =  -MulDiv( nHeight, GetDeviceCaps( dc, LOGPIXELSY ), 72 ) ;
//	::ReleaseDC( 0, dc );
//	return nNewHeight;
	return nHeight > 0 ? -nHeight : nHeight;
}

/////////////////////////////////////////////////////////////////////////////
// CVTList::OnDraw - Drawing function

void CVTList::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	CRect rc;

	GetClientRect( &rc );

	pdc->FillRect( rc, &(CBrush( ::GetSysColor(COLOR_BTNFACE) )) );

	UINT	edgeType = 0;
	if(m_bInnerRaised)
		edgeType |= BDR_RAISEDINNER;
	if(m_bInnerSunken)
		edgeType |= BDR_SUNKENINNER;
	if(m_bOuterRaised)
		edgeType |= BDR_RAISEDOUTER;
	if(m_bOuterSunken)
		edgeType |= BDR_SUNKENOUTER;
	if( edgeType != 0)
	{
		pdc->DrawEdge( rc, edgeType, BF_RECT);

		rc.left		+= VT_BORDER_WIDTH;
		rc.top		+=VT_BORDER_WIDTH;
		rc.right	-=VT_BORDER_WIDTH;
		rc.bottom	-=VT_BORDER_WIDTH;

	}
	else
	{

		HPEN hPen = ::CreatePen( PS_SOLID, 1, GetSysColor( /*COLOR_BTNTEXT*/COLOR_BTNFACE ) );
		HPEN hold = (HPEN)::SelectObject( pdc->m_hDC, hPen );
		HBRUSH holdB = (HBRUSH)::SelectObject( pdc->m_hDC, GetStockObject( NULL_BRUSH ) );

		::Rectangle( pdc->m_hDC, rc.left, rc.top, rc.right, rc.bottom );

		::SelectObject( pdc->m_hDC, hold );
		::SelectObject( pdc->m_hDC, holdB );
		
		::DeleteObject( hPen );
	}

	if(flagHideHeader!=m_HideHeader)
	{
		flagHideHeader=m_HideHeader;
		m_listCtrl.GetClientRect(rc);
		CRect rect;
		GetClientRect(rect);
		if(m_HideHeader)
		{
			int heightCtrl;
			if((rc.top-cyHeader+rc.Height()+2*cyHeader)>rect.bottom)
				heightCtrl=rect.bottom-(rc.top-cyHeader);
			else
				heightCtrl=rc.Height()+2*cyHeader;
			m_listCtrl.MoveWindow(rc.left,rc.top-cyHeader,m_widthCtrlList/*rc.Width()+2*/,heightCtrl);
		}
		else
			m_listCtrl.MoveWindow(0,0,m_widthCtrlList/*rc.Width()+2*/,rect.Height());
	}
	
	if(b_createToolTip)
		m_listCtrl.Invalidate();
	/*if( m_listCtrl.GetSafeHwnd() )
	{
		m_listCtrl.RedrawWindow( NULL, NULL, 
			RDW_FRAME|RDW_INVALIDATE|RDW_INTERNALPAINT );	

		CHeaderCtrl* pHeader= NULL;
		pHeader= m_listCtrl.GetHeaderCtrl();
		if( pHeader && pHeader->GetSafeHwnd() )
			pHeader->RedrawWindow( NULL, NULL, 
				RDW_FRAME|RDW_INVALIDATE|RDW_INTERNALPAINT );	

		for( long lPos = m_lstExtHeaderCtls.head(); lPos; lPos = m_lstExtHeaderCtls.next( lPos ) )
		{
			CListHeaderCtrl *pCtrl = m_lstExtHeaderCtls.get( lPos );
			
			pCtrl->RedrawWindow( NULL, NULL, RDW_FRAME|RDW_INVALIDATE|RDW_INTERNALPAINT );	
			
			
		}

	}

	HPEN hPen = CreatePen( PS_SOLID, 1, RGB(0,0,0) );
	HPEN hold = (HPEN)SelectObject( pdc->m_hDC, hPen );
	HBRUSH holdB = (HBRUSH)SelectObject( pdc->m_hDC, GetStockObject( NULL_BRUSH ) );

	::Rectangle( pdc->m_hDC, rc.left, rc.top, rc.right, rc.bottom );

	SelectObject( pdc->m_hDC, hold );
	SelectObject( pdc->m_hDC, holdB );
	DeleteObject( hPen );*/
}


/////////////////////////////////////////////////////////////////////////////
// CVTList::DoPropExchange - Persistence support

void CVTList::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	//COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

	PX_Bool( pPX, "UseGrid", m_useGrid, TRUE );	
	PX_Bool( pPX, "UseFullRowSelect", m_useFullRowSelect, TRUE );	


	PX_Bool( pPX, "AutoUpdate", m_AutoUpdate, FALSE );		
	PX_Bool( pPX, "Enabled", m_bEnabled, TRUE );		
	PX_String( pPX, "Name", m_strName, "VList" );		

	//PX_Bool( pPX, "HideHeader", flagHideHeader, FALSE );	
	//PX_Bool( pPX, "HideBorder", m_HideBorder, FALSE );

	//If serialize after object was created
	OnUseFullRowSelectChanged();
	OnUseGridChanged();
	//OnHideHeaderChanged();
	//OnHideBorderChanged();
		
}


/////////////////////////////////////////////////////////////////////////////
// CVTList::OnResetState - Reset control to default state

void CVTList::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange	

	m_useGrid			= TRUE;
	m_useFullRowSelect	= TRUE;


	m_AutoUpdate		= FALSE;
	m_bEnabled			= TRUE;
	m_strName			= "VList";


	//flagHideHeader=FALSE;
	//m_HideBorder=FALSE;

	m_bInnerRaised = m_bInnerSunken = 
		m_bOuterRaised = m_bOuterSunken = FALSE;


	m_listCtrl.ResetDefaults();

	m_bUseSystemFont = TRUE;
	m_bUseSimpleFontType = FALSE;

	// [vanek] BT2000: 3542 - 26.12.2003
	m_bEnableResizeColumns = TRUE;
	
}

void CVTList::Serialize(CArchive& ar)
{
	try
	{
		long	nVersion = 10;
		COleControl::Serialize( ar );

		if( ar.IsStoring( ) )
		{		
			ar<<nVersion;
			m_listCtrl.Serialize( ar );

			ar<<m_useGrid;
			ar<<m_useFullRowSelect;

			ar<<m_AutoUpdate;
			ar<<m_bEnabled;
			ar<<m_strName;
			ar<<m_bInnerRaised;
			ar<<m_bInnerSunken;
			ar<<m_bOuterRaised;
			ar<<m_bOuterSunken;

			ar<<m_bUseSystemFont;
			ar<<m_bUseSimpleFontType;

			// [vanek] BT2000: 3542 - 26.12.2003
			ar<<m_bEnableResizeColumns;

			ar << m_bUseExtHeader;
			ar << m_listHeaders.count();

			for( long lPos = m_listHeaders.head(), q = 0; lPos; lPos = m_listHeaders.next( lPos ), q++ )
			{
				typeHD *pLevel = m_listHeaders.get( lPos );

				if( pLevel )
				{
					ar << pLevel->count();

					for( long lPos = pLevel->head(), q = 0; lPos; lPos = pLevel->next( lPos ), q++ )
					{
						XHeaderData *pData = pLevel->get( lPos );

						ar << pData->strName;
						ar << pData->nAssignment;
						ar << pData->lWidth;
					}
				}
			}
			ar << m_HideHeader;
			ar << m_HideBorder;
		}else	
		{
			ar>>nVersion;
			m_listCtrl.Serialize( ar );

			if( nVersion >=4 )
			{
				ar>>m_useGrid;
				ar>>m_useFullRowSelect;

				ar>>m_AutoUpdate;
				ar>>m_bEnabled;
				ar>>m_strName;
			}

			if( nVersion >=5 )
			{
				ar>>m_bInnerRaised;
				ar>>m_bInnerSunken;
				ar>>m_bOuterRaised;
				ar>>m_bOuterSunken;
			}

			if( nVersion >= 6 )
			{
				ar>>m_bUseSystemFont;
				m_listCtrl.ReMeasureItem();
			}

			if( nVersion >= 7 )
				ar>>m_bUseSimpleFontType;
			
			// [vanek] BT2000: 3542 - 26.12.2003
			if( nVersion >= 8 )
				ar>>m_bEnableResizeColumns;

			if( nVersion >= 9 )
			{
				ar >> m_bUseExtHeader;

				long lCount = 0;
				ar >> lCount;

				for( int i = 0; i < lCount; i++ )
				{
					typeHD *item = new typeHD;
					m_listHeaders.add_tail( item );
				}

				for( long lPos = m_listHeaders.head(); lPos; lPos = m_listHeaders.next( lPos ) )
				{
					typeHD *pLevel = m_listHeaders.get( lPos );

					if( pLevel )
					{
						long lCount2 = 0;
						ar >> lCount2;

						for( int i = 0; i < lCount2; i++ )
						{
							XHeaderData *pData = new XHeaderData;
							pLevel->add_tail( pData );
						}

						for( long lPos = pLevel->head(); lPos; lPos = pLevel->next( lPos ) )
						{
							XHeaderData *pData = pLevel->get( lPos );

							ar >> pData->strName;
							ar >> pData->nAssignment;
							ar >> pData->lWidth;
						}
					}
				}
				
				{
					long lCount = m_listCtrl.GetHeaderCtrl()->GetItemCount();
					for( int i = 0; i < lCount; i++ )
					{
						XHeaderData *pData = new XHeaderData;
						m_listHeadersMain.add_tail( pData );
					}
				}

				_create_header_ctrls();
			}
			if( nVersion >= 10 )
			{
				ar >> m_HideHeader;
				ar >> m_HideBorder;
				SetHideHeader(m_HideHeader);
				SetHideBorder(m_HideBorder);
			}
		}		
				
		OnUseFullRowSelectChanged();
		OnUseGridChanged();		
		SendWM_SIZE();
		
		

		InvalidateControl();

		if(m_listCtrl.GetSafeHwnd())
		{
			m_listCtrl.Invalidate();
			m_listCtrl.RedrawWindow( NULL, NULL, 
				RDW_FRAME|RDW_INVALIDATE|RDW_INTERNALPAINT );
			
		}
		
		

	}	
	catch(CException *pe)
	{
		pe->ReportError();
		pe->Delete();
		OnResetState();
		AfxMessageBox("Bad Load status. Reset to default.");
	}


}

/////////////////////////////////////////////////////////////////////////////
// CVTList::AboutBox - Display an "About" box to the user

void CVTList::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_VTLIST);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CVTList::PreCreateWindow - Modify parameters for CreateWindowEx
int CVTList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{


	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_listCtrl.Create( this );
	
	//m_listCtrl.ModifyStyle(WS_HSCROLL,NULL);
	//m_listCtrl.ModifyStyle(WS_VSCROLL,NULL);

	CRect rect;
	GetClientRect(rect);
	m_widthCtrlList=rect.Width();
	//CHeaderCtrl *pCtrl = m_listCtrl.GetHeaderCtrl();
	//pCtrl->ModifyStyle( HDS_BUTTONS, HDS_FLAT );
	
	//m_hideHeader=false;
	
	// Инициализация доп.свойств header'а
	hwndLV=m_listCtrl.m_hWnd;
	wpOrigEditProc=InitListViewEx(hwndLV);
	// Для отображения всех наворотов
	ListView_SetExtendedListViewStyle(hwndLV,WS_EX_STATICEDGE |	LVS_EX_INFOTIP | LVS_EX_LABELTIP |
										/*LVS_EX_HEADERDRAGDROP |*/	LVS_EX_FULLROWSELECT);

	m_listCtrl.m_headerCtrl.EnableToolTips(TRUE);
	CToolTipCtrl* pToolTip = m_listCtrl.GetToolTips();
	/*pToolTip->SendMessage( TTM_SETMAXTIPWIDTH, 0,			 SHRT_MAX);
	pToolTip->SendMessage( TTM_SETDELAYTIME,   TTDT_AUTOPOP, SHRT_MAX);
	pToolTip->SendMessage( TTM_SETDELAYTIME,   TTDT_INITIAL, 200);
	pToolTip->SendMessage( TTM_SETDELAYTIME,   TTDT_RESHOW,  200);*/
    pToolTip->SetWindowPos(&wndTopMost, 0, 0, 0, 0, 0);
	ListView_SetToolTips(hwndLV,pToolTip->m_hWnd);
	//CToolInfo tinf;
	//pToolTip->GetToolInfo(tinf,FromHandle(m_listCtrl.m_hWnd),0);
	//m_listCtrl.BuildControl( );

	//OnEnabledChanged();

	return 0;
}


void CVTList::OnSize(UINT nType, int cx, int cy) 
{
 	COleControl::OnSize(nType, cx, cy);

	BOOL bHaveEdge = (m_bInnerRaised || m_bInnerSunken
					||m_bOuterRaised || m_bOuterSunken);

	long lTop = 0;

	//if(bHaveEdge)
	if(m_HideBorder || bHaveEdge)
		m_listCtrl.ModifyStyle( WS_BORDER, 0 );
	else
		m_listCtrl.ModifyStyle( 0, WS_BORDER );
	/*else
		m_listCtrl.ModifyStyle( 0, WS_BORDER );*/

	/*if( m_bUseExtHeader )
	{

		for( long lPos = m_lstExtHeaderCtls.head(); lPos; lPos = m_lstExtHeaderCtls.next( lPos ) )
	{
			CListHeaderCtrl *pCtrl = m_lstExtHeaderCtls.get( lPos );
			int i=0;
			
		}

		/*RECT rcHeader = {0};
		m_listCtrl.GetHeaderCtrl()->GetClientRect( &rcHeader );

 		lTop = ( m_lstExtHeaderCtls.count() - 1 ) * ( rcHeader.bottom - rcHeader.top );
 		for( long lPos = m_lstExtHeaderCtls.head(); lPos; lPos = m_lstExtHeaderCtls.next( lPos ) )
		{
			CListHeaderCtrl *pCtrl = m_lstExtHeaderCtls.get( lPos );

			if( pCtrl->GetSafeHwnd() )
			{
       			pCtrl->MoveWindow(  (bHaveEdge ? (VT_BORDER_WIDTH + 2) : 1),	
									(bHaveEdge ? (VT_BORDER_WIDTH + lTop) :( lTop + 2 )), 
									(bHaveEdge ? (cx-2*VT_BORDER_WIDTH - 4): (cx - 2) ), 
									rcHeader.bottom - rcHeader.top );
				
				lTop -= rcHeader.bottom - rcHeader.top;
			}
			
		}
		
 		lTop = m_lstExtHeaderCtls.count() * ( rcHeader.bottom - rcHeader.top );
		m_listCtrl.MoveWindow( 
							(bHaveEdge ? (VT_BORDER_WIDTH + 2+1) : 1),
							(bHaveEdge ? (VT_BORDER_WIDTH + lTop) : (lTop + 1)),
							(bHaveEdge ? (cx-2*VT_BORDER_WIDTH - 4-1): (cx-2-1) ),
							(bHaveEdge ? (cy - lTop -2*VT_BORDER_WIDTH): (cy - lTop - 2) )
							);    
	*/
	//}
	//else
	//{
		m_listCtrl.MoveWindow( 
							(bHaveEdge ? VT_BORDER_WIDTH : 0),
							(bHaveEdge ? VT_BORDER_WIDTH : 0),
							(bHaveEdge ? (cx-2*VT_BORDER_WIDTH): cx),
							(bHaveEdge ? (cy-2*VT_BORDER_WIDTH): cy)
							);  
	//}


	CString strSize;
	strSize.Format( "\nList:cx=%d, cy=%d", cx, cy );
	OutputDebugString( strSize );
}

/////////////////////////////////////////////////////////////////////////////
// CVTList::OnOcmCommand - Handle command messages

LRESULT CVTList::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CVTList message handlers




/////////////////////////////////////////////////////////////////////////////
//
//
//	Set def. values row & col count to 0, destroy values array, etc.
//
//
/////////////////////////////////////////////////////////////////////////////
void CVTList::SetDefaultValues()
{
	
}

void CVTList::UpdateControl( BOOL b_repaint )
{
	SetModifiedFlag();

	if( b_repaint )
		if(m_listCtrl.GetSafeHwnd())
			m_listCtrl.Invalidate( );	
}


void CVTList::OnUseGridChanged() 
{		
	
	if( !m_listCtrl.GetSafeHwnd() ) return;

	DWORD dwExStyle = m_listCtrl.GetExtendedStyle( );

	if( UseGrid() )
		dwExStyle |= LVS_EX_GRIDLINES;
	else
		dwExStyle &= ~LVS_EX_GRIDLINES;

	m_listCtrl.SetExtendedStyle( dwExStyle );

	UpdateControl();
}

void CVTList::OnUseFullRowSelectChanged() 
{		
	UpdateControl();
}

DWORD CVTList::GetControlFlags() 
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// The control will not be redrawn when making the transition
	// between the active and inactivate state.
	dwFlags |= noFlickerActivate;

	// The control can optimize its OnDraw method, by not restoring
	// the original GDI objects in the device context.
	dwFlags |= canOptimizeDraw;
	//return dwFlags;
	
	return COleControl::GetControlFlags();
}



OLE_COLOR CVTList::GetColumnBackColor(short nColumn) 
{
	return m_listCtrl.GetColumnBackColor( nColumn );
}

OLE_COLOR CVTList::GetColumnForeColor(short nColumn) 
{
	return m_listCtrl.GetColumnForeColor( nColumn );
}

BSTR CVTList::GetColumnCaption(short nColumn) 
{
	CString strResult = m_listCtrl.GetColumnCaption( nColumn );

	return strResult.AllocSysString();
}			   


short CVTList::GetColumnType(short nColumn) 
{	
	return (short)m_listCtrl._GetColumnType( nColumn );
}

short CVTList::GetColumnAlign(short nColumn) 
{
	return (short)m_listCtrl.GetColumnAlign( nColumn );
}

short CVTList::GetColumnWidth(short nColumn) 
{
	return (short)m_listCtrl.GetColumnWidth( nColumn );
}




void CVTList::SetColumnAlign(short nColumn, short nAlign) 
{
	m_listCtrl.SetColumnAlign( nColumn, nAlign);
	UpdateControl();
}

void CVTList::SetColumnBackColor(short nColumn, OLE_COLOR color) 
{
	// TODO: Add your dispatch handler code here
	m_listCtrl.SetColumnBackColor( nColumn, color );
	UpdateControl();

}

void CVTList::SetColumnForeColor(short nColumn, OLE_COLOR color) 
{	
	m_listCtrl.SetColumnForeColor( nColumn, color);
	UpdateControl();
}

void CVTList::SetColumnType(short nColumn, short nType) 
{
	m_listCtrl.SetColumnType( nColumn, nType);
	UpdateControl();
}

void CVTList::SetColumnWidth(short nColumn, short nWidth) 
{
	m_listCtrl.SetColumnWidth( nColumn, nWidth);
	UpdateControl();
}

void CVTList::SetColumnCaption(short nColumn, LPCTSTR pstrCaption) 
{	
	CString str = pstrCaption;	
	if(m_sNameColumn.GetCount()>0 && m_sNameColumn.GetCount()-1>=nColumn)
	{
		CString strCaption, s;
		s=strCaption=m_sNameColumn.GetAt(nColumn);
		BOOL bFind=false;
		BOOL bWasFind=false;
		int nc=0,i=0;
		while(!bFind)
		{
			int nCF = strCaption.Find('|',0);
			if(nCF>=0)
			{
				nc+=nCF+i;
				strCaption.Delete(0,nCF+1);
				bWasFind=true;
				i++;
			}
			else
				bFind=true;
		}
		if(bWasFind)
		{
			s.Delete(nc+1,1000);
			s+=str;
			m_sNameColumn.SetAt(nColumn,s);
			if(m_bUseExtHeader)
				m_listCtrl.SetColumnCaption( nColumn, s );
			else
	m_listCtrl.SetColumnCaption( nColumn, str );
		}
		else
		{
			m_sNameColumn.SetAt(nColumn,str);
			m_listCtrl.SetColumnCaption( nColumn, str );
		}
			for( long lPos = m_listHeaders.head(), q = 0; lPos; lPos = m_listHeaders.next( lPos ), q++ )
			{
				if( q == 0 )
				{
					typeHD *pLevel = m_listHeaders.get( lPos );
		
					if( pLevel )
					{
						for( int i = pLevel->count(); i < nColumn + 1; i++ )
						{
							XHeaderData *pData = new XHeaderData;
							pLevel->add_tail( pData );
						}
		
						for( long lPos2 = pLevel->head(), w = 0; lPos2; lPos2 = pLevel->next( lPos2 ), w++ )
						{
							if( w == nColumn )
							{
								XHeaderData *pData = pLevel->get( lPos2 );
								CString strName=m_sNameColumn.GetAt(nColumn);
								pData->strName = _change_chars( /*pbstrName*/strName, "\\n", "\r\n" );
								//pData->nAssignment = nAssignment;
								//pData->lWidth = nWidth;
		
								break;
							}
						}
					}
					break;
				}
			}
		
	}
	else
		m_listCtrl.SetColumnCaption( nColumn, str );
	
	UpdateControl();
}


void CVTList::AddColumn(short nColumn) 
{
	if(m_sNameColumn.GetCount()>0)
		SetExtHeaderData( 0, m_listCtrl.GetColCount(), "Value", 0, 100, -1);
	else
	m_listCtrl._AddColumn();	
	UpdateControl();
}

void CVTList::DeleteColumn(short nColumn) 
{
	if(m_sNameColumn.GetCount()>0)
	{
		CString s;
		s=m_sNameColumn.GetAt(0);
		SetExtHeaderData( 0, 0, s, 0, m_zeroColumnWidth, nColumn);
	}
	else
	m_listCtrl._DeleteColumn( nColumn );
	UpdateControl();
}

short CVTList::GetDisplayLogicalAsText(short nColumn) 
{
	return (short)(m_listCtrl.GetDisplayLogicalAsText( nColumn ));
}

void CVTList::SetDisplayLogicalAsText(short nColumn, short nValue) 
{
	UpdateControl();
	m_listCtrl.SetDisplayLogicalAsText( nColumn, BOOL(nValue) );	
}



short CVTList::IsColumnUseIcon(short nColumn) 
{
	return (short)m_listCtrl.IsColumnUseIcon( nColumn );
}

void CVTList::SetColumnUseIcon(short nColumn, short nValue) 
{
	m_listCtrl.SetColumnUseIcon( nColumn, (BOOL) nValue);
	UpdateControl();
}

BSTR CVTList::GetCellAsString(short nColumn, short nRow) 
{
	CString strResult;

	datatype_e datatype = CBaseColumn::ConvertFromShortToEnum( GetColumnType( nColumn ) );

	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);

	if( pCell )
	{
		strResult = pCell->GetAsString( datatype ); 
	}

	return strResult.AllocSysString();
}
									   
									
void CVTList::DeleteRow(short nRow) 
{	
	m_listCtrl.DeleteRow(nRow);
	nRow = min(nRow, m_listCtrl.GetRowCount()-1);
	FireOnDeleteRow( nRow );
	UpdateControl();
}

void CVTList::AddRow(short nRow) 
{	
	m_listCtrl.AddRow( nRow );
	UpdateControl();
}

void CVTList::SetIcon(short nColumn, short nRow, LPCTSTR strFileName) 
{
	CString str = strFileName;	
	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);
	if( pCell )
	{
		pCell->SetIconFileName(str);
	}	
	UpdateControl();

}

VARIANT CVTList::GetColumnFont(short nColumn) 
{
	VARIANT vaResult;
	VariantInit(&vaResult);

	LPLOGFONT lpLF = m_listCtrl.GetFont( nColumn );	
		
	vaResult.byref = (PVOID)lpLF;

	return vaResult;
}


void CVTList::SetColumnFont(short nColumn, const VARIANT FAR& pVariant) 
{

	LPVOID pVoid = pVariant.byref;

	LPLOGFONT lpLF = (LPLOGFONT)pVoid;

	m_listCtrl.SetFont( nColumn , lpLF );

	UpdateControl();

}

void CVTList::SetColumnLogicalIcon(short nColumn, LPCTSTR pbStrFileName, short b_forValue) 
{
	CString strFileName = pbStrFileName;
	m_listCtrl.SetColumnLogicalIcon( nColumn, strFileName, b_forValue );
	UpdateControl();
}

short CVTList::CurColumnLeft() 
{
	UpdateControl();
	return m_listCtrl.CurColumnMove( -1 );
}

short CVTList::CurColumnRight() 
{
	UpdateControl();
	return m_listCtrl.CurColumnMove( 1 );	
}



void CVTList::SetColumnReadOnly(short nColumn, short nValue) 
{
	m_listCtrl.SetColumnReadOnly( nColumn, (BOOL)nValue);
	UpdateControl();
}

short CVTList::IsColumnReadOnly(short nColumn) 
{
	return (short)m_listCtrl.IsColumnReadOnly( nColumn );	
}

					   
void CVTList::SetCellValue(short nColumn, short nRow, LPCTSTR pbStrValue) 
{	
	CString str = pbStrValue;
	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);
	if( pCell )
	{
		pCell->SetSubject( str );
		pCell->SetValue( str );
	}	
	UpdateControl();
}

void CVTList::SetCellValueColor(short nColumn, short nRow, OLE_COLOR color) 
{	
	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);
	if( pCell )
	{
		pCell->SetColor( color );
	}

	UpdateControl();
}

void CVTList::SetCellValueColorRGB(short nColumn, short nRow, LPCTSTR strRGBColor) 
{	
	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);
	if( pCell )
	{
		OLE_COLOR color = (OLE_COLOR)RGB(255,255,255);
		CString str = strRGBColor;
		CString strR = "", strG = "", strB = "";

		if( str.Find("(") != -1 && str.Find(")") != -1 )
		{
			int brCount = 0;
			for( int i = 1; i < str.GetLength() - 1 ;i++)
			{
				char ch = str.GetAt( i );
				if( ch != ',' )
				{
					switch( brCount )
					{
					case 0:	strR += ch; 
							break;
					case 1:	strG += ch; 
							break;
					case 2:	strB += ch; 
							break;
					}
				}
				else
					brCount++;
			}
			
			if( !strR.IsEmpty() && !strG.IsEmpty() && !strB.IsEmpty() )
				color = (OLE_COLOR)RGB( atoi(strR), atoi(strG), atoi(strB) );
		}

		pCell->SetColor( color );
	}
	UpdateControl();
}

BSTR CVTList::GetCellAsColorRGB(short nColumn, short nRow) 
{	
	OLE_COLOR color = GetCellAsColor( nColumn, nRow );
	COLORREF Color = (COLORREF)color;

	CString str;
	str.Format( "(%d,%d,%d)", GetRValue(Color), GetGValue(Color), GetBValue(Color) );

	return str.AllocSysString();
}



BSTR CVTList::GetKeyValue(short nColumn, short nRow) 
{
	CString strResult;

	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);

	if( pCell )
	{
		strResult = pCell->GetKeyValue( ); 
	}

	return strResult.AllocSysString();
}

void CVTList::SetKeyValue(short nColumn, short nRow, LPCTSTR pbStrKey) 
{
	CString str = pbStrKey;
	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);
	if( pCell )
	{
		pCell->SetKeyValue( str );
	}	
	
	UpdateControl( FALSE );
}

void CVTList::OnAutoUpdateChanged() 
{
	// TODO: Add notification handler code

	SetModifiedFlag();
}

BSTR CVTList::GetCellKey(short nColumn, short nRow) 
{
	CString strResult;
	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);
	if( pCell )
	{
		strResult = pCell->GetKeyValue( );
	}

	return strResult.AllocSysString();
}

BSTR CVTList::GetCellAsSubject(short nColumn, short nRow) 
{
	CString strResult;
	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);
	if( pCell )
	{
		strResult = pCell->GetAsSubject( );
	}

	return strResult.AllocSysString();
}

OLE_COLOR CVTList::GetCellAsColor(short nColumn, short nRow) 
{
	OLE_COLOR clColor = (OLE_COLOR) RGB(0,0,0);
	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);
	if( pCell )
	{
		clColor = pCell->GetAsColor( );	
	}
	return clColor;
}

short CVTList::GetCellAsLogical(short nColumn, short nRow) 
{
	BOOL b_result = FALSE;;
	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);
	if( pCell )
	{
		b_result = pCell->GetAsLogical( );
	}	

	return b_result;
}

short CVTList::GetCellTypeShading(short nColumn, short nRow) 
{
	short typeShading = -1;
	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);
	if( pCell )
	{
		typeShading = pCell->GetShadingType();	
	}
	return typeShading;
}

void CVTList::SetCellTypeShading(short nColumn, short nRow, short typeShading)
{	
	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);
	if( pCell )
	{
		if(typeShading > 15 || typeShading < -1)
			typeShading=-1;
		pCell->SetShadingType( typeShading );
		SetModifiedFlag();
		UpdateControl();
	}		
	
}

void CVTList::SetCellAsLogical(short nColumn, short nRow, short bValue)
{	
	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);
	if( pCell )
	{
		pCell->SetLogical( bValue );
		SetModifiedFlag();
		UpdateControl();
	}		
	
}

BSTR CVTList::GetCellAsValue(short nColumn, short nRow) 
{
	CString strResult;
	
	CBaseCell* pCell = m_listCtrl.GetCell(nRow, nColumn);
	if( pCell )
	{
		strResult = pCell->GetAsValue( );
	}

	return strResult.AllocSysString();
}


void CVTList::OnEnabledChanged() 
{
	m_listCtrl.EnableWindow( m_bEnabled );	
		
	SetModifiedFlag();
}

void CVTList::OnNameChanged() 
{
	SetModifiedFlag();
}




BOOL CVTList::GetInnerRaised() 
{		
	return m_bInnerRaised;
}

void CVTList::SetInnerRaised(BOOL bNewValue) 
{
	m_bInnerRaised = bNewValue;
	SetModifiedFlag();
	SendWM_SIZE();
}

BOOL CVTList::GetInnerSunken() 
{
	return m_bInnerSunken;
}

void CVTList::SetInnerSunken(BOOL bNewValue) 
{
	m_bInnerSunken = bNewValue;
	SetModifiedFlag();
	SendWM_SIZE();
}

BOOL CVTList::GetOuterRaised() 
{
	return m_bOuterRaised;
}

void CVTList::SetOuterRaised(BOOL bNewValue) 
{
	m_bOuterRaised = bNewValue;
	SetModifiedFlag();
	SendWM_SIZE();
}

BOOL CVTList::GetOuterSunken() 
{
	return m_bOuterSunken;
}

void CVTList::SetOuterSunken(BOOL bNewValue) 
{
	m_bOuterSunken = bNewValue;
	SetModifiedFlag();
	SendWM_SIZE();
}

void CVTList::SendWM_SIZE()
{
	if( m_hWnd )
	{
		CRect rc;
		GetWindowRect( &rc );

		
		WINDOWPOS wp;
		wp.hwnd = m_hWnd;
		wp.cx = rc.Width();
		wp.cy = rc.Height();
		wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
		SendMessage( WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp );
	}
}


#ifndef FOR_HOME_WORK
HRESULT CVTList::XActiveXCtrl::SetSite( IUnknown *punkVtApp, IUnknown *punkSite )
{
	METHOD_PROLOGUE_EX(CVTList, ActiveXCtrl)

	pThis->m_ptrSite = punkSite;
	pThis->m_ptrApp = punkVtApp;

	pThis->RestoreFromShellData();

	return S_OK;
}
  

HRESULT CVTList::XActiveXCtrl::GetName( BSTR *pbstrName )
{
	METHOD_PROLOGUE_EX(CVTList, ActiveXCtrl)	
	*pbstrName = pThis->m_strName.AllocSysString();
	return S_OK;
}

HRESULT CVTList::XActiveXCtrl::SetDoc( IUnknown *punkDoc )
{
	METHOD_PROLOGUE_EX(CVTList, ActiveXCtrl)	
	return S_OK;
}

HRESULT CVTList::XActiveXCtrl::SetApp( IUnknown *punkVtApp )
{
	METHOD_PROLOGUE_EX(CVTList, ActiveXCtrl)	
	return S_OK;
}



HRESULT CVTList::XActiveXCtrl::OnOK( )
{
	METHOD_PROLOGUE_EX(CVTList, ActiveXCtrl)

	if( pThis->m_AutoUpdate )
		pThis->StoreToData();

	return S_OK;
}

HRESULT CVTList::XActiveXCtrl::OnApply( )
{
	METHOD_PROLOGUE_EX(CVTList, ActiveXCtrl)

	OnOK();

	return S_OK;
}

HRESULT CVTList::XActiveXCtrl::OnCancel( )
{
	METHOD_PROLOGUE_EX(CVTList, ActiveXCtrl)

	return S_OK;
}


#endif

BOOL CVTList::RestoreFromShellData()
{

#ifndef FOR_HOME_WORK

	//if( !m_AutoUpdate ) return FALSE;

	if( m_ptrApp == NULL ) return FALSE;
	m_lVersion = _GetValueInt( m_ptrApp, "\\VTListControl", "ControlVersion", 1 );
		
	
	CString strValue;
	COLORREF crColor;


	for( int iCol=0;iCol<m_listCtrl.GetColumnCount();iCol++)
	{
		if( m_listCtrl.IsValidColumnIndex( iCol ) )
		{			

			for( int iRow=0;iRow<m_listCtrl.GetRowCount();iRow++)
			{
				CBaseCell* pCell = m_listCtrl.GetCell( iRow, iCol );
				if( pCell )
				{

					CString strKeyValue = pCell->GetKeyValue( );

					CString strSection;
					CString strEntry;

					if( ::GetSectionEntry( strKeyValue, strSection, strEntry ) )
					{
						datatype_e datatype = m_listCtrl.GetColumnType( iCol );
						switch( datatype )
						{
							case TYPE_SUBJECT:
								 strValue = _GetValueString( m_ptrApp, 
															strSection, 
															strEntry
															);
								 pCell->SetSubject( strValue );
								 
								 break;

							case TYPE_LOGICAL:
								{
								 int nValue = _GetValueInt( m_ptrApp, 
															strSection, 
															strEntry
															);
								if( nValue )
									pCell->SetLogical( TRUE );
								else
									pCell->SetLogical( FALSE );

								  break;
								}

							case TYPE_COLOR:
								crColor = _GetValueColor( m_ptrApp, 
															strSection, 
															strEntry,
															::GetSysColor( COLOR_WINDOW )
															);

								pCell->SetColor( crColor );
								
								break;
							
							case TYPE_SHADING:
								crColor = _GetValueColor( m_ptrApp, 
															strSection, 
															strEntry,
															::GetSysColor( COLOR_WINDOW )
															);

								pCell->SetColor( crColor );
								
								break;

							case TYPE_FONT:
							{
								LOGFONT  lf;
								memcpy( &lf, pCell->GetAsFont(), sizeof(LOGFONT) );								 								 
								if( m_lVersion == 1 )
								{
									strValue = _GetValueString( m_ptrApp, 
																strSection, 
																strEntry
																);
									
									strcpy( lf.lfFaceName, strValue );

									{
										strEntry = strEntry.Left( strEntry.GetLength() - 1 );

										lf.lfHeight         = _GetValueInt( m_ptrApp, strSection, strEntry + "_Height\\", 11 );
										lf.lfWeight         = _GetValueInt( m_ptrApp, strSection, strEntry + "_Weight\\", 0  );
										lf.lfItalic         = (BOOL)_GetValueInt( m_ptrApp, strSection, strEntry + "_Italic\\", 0  );
										lf.lfUnderline      = (BOOL)_GetValueInt( m_ptrApp, strSection, strEntry + "_Underline\\", 0  );
										lf.lfStrikeOut      = (BOOL)_GetValueInt( m_ptrApp, strSection, strEntry + "_StrikeOut\\", 0  );
										lf.lfCharSet        = (BYTE)_GetValueInt( m_ptrApp, strSection, strEntry + "_CharSet\\", 1  );
										lf.lfPitchAndFamily = (BYTE)_GetValueInt( m_ptrApp, strSection, strEntry + "_PitchAndFamily\\", 0  );
									}

									CString strPath = _GetValueString( m_ptrApp, strSection, strEntry + "_Location\\", "" );
									if( !strPath.IsEmpty() )
									{
										if( strPath[ strPath.GetLength() - 1] == '\\' )
										strPath = strPath.Left( strPath.GetLength() - 1 );

										CString strName;
										for( int i = strPath.GetLength() - 1; i >= 0; i-- )
										{
											if( strPath[i] != '\\' )
												strName.Insert( 0, strPath[i] );
											else
												break;
										}

										if( !strName.IsEmpty() )
										{
											strPath = strPath.Left( strPath.GetLength() - strName.GetLength() );
																													  
											lf.lfHeight = font_height( _GetValueInt( m_ptrApp, strPath, strName + "_Height\\", 12 ) );
											lf.lfWeight = _GetValueInt( m_ptrApp, strPath, strName + "_Bold\\",  0) == 1 ? FW_BOLD : FW_NORMAL;

											lf.lfUnderline = (BYTE)_GetValueInt( m_ptrApp, strPath, strName + "_Underline\\", 0 );

											lf.lfItalic = (BYTE)_GetValueInt( m_ptrApp, strPath, strName + "_Italic\\"  );

											strValue = _GetValueString( m_ptrApp, strPath, strName + "_Name\\", "Arial" );
											strcpy( lf.lfFaceName, strValue );
										}
									}
								}
								else if( m_lVersion == 2 )
								{
									strcpy( lf.lfFaceName, _GetValueString( m_ptrApp, strSection + "\\" + strEntry, "Name", 0 ) );
									pCell->m_nPointFontSize = _GetValueInt( m_ptrApp, strSection + "\\" + strEntry, "Size", 0 ) * 10;
									lf.lfItalic = _GetValueInt( m_ptrApp, strSection + "\\" + strEntry, "Italic", 0 ) == 1 ? 255 : 0;
									lf.lfUnderline = _GetValueInt( m_ptrApp, strSection + "\\" + strEntry, "Underline", 0 );
									lf.lfStrikeOut = _GetValueInt( m_ptrApp, strSection + "\\" + strEntry, "StrikeOut", 0 );
									lf.lfWeight = _GetValueInt( m_ptrApp, strSection + "\\" + strEntry, "Bold", 0 ) == 1 ? FW_BOLD : FW_NORMAL;
									pCell->m_crFontColor = _GetValueColor( m_ptrApp, strSection + "\\" + strEntry, "TextColor", pCell->m_crFontColor );
								}

								 pCell->SetFont( lf );					 

								  break;
							}
							case TYPE_VALUE:
							case TYPE_VALUE_COMBO:
								 strValue = _GetValueString( m_ptrApp, 
															strSection, 
															strEntry
															);
								 pCell->SetValue( strValue );

							default:
								 break;
						}
						

					}	

				}
			}
		}

	}		

	UpdateControl();
#endif
	return TRUE;
}

BOOL CVTList::WriteToShellData(CBaseCell* pCell, datatype_e datatype )
{
#ifndef FOR_HOME_WORK

	if( !pCell ) return FALSE;

	//if( !m_AutoUpdate ) return FALSE;
		
	if( m_ptrApp )
	{		
		CString strKeyValue = pCell->GetKeyValue( );

		CString strSection;
		CString strEntry;

		if( !::GetSectionEntry( strKeyValue, strSection, strEntry ) )
			return FALSE;
		

		 switch( datatype )
		 {
			 case TYPE_SUBJECT:
				  _SetValue( m_ptrApp, strSection, strEntry, 
								pCell->GetAsSubject() );
				  break;

			 case TYPE_LOGICAL:
				  _SetValueInt( m_ptrApp, strSection, strEntry, (long)pCell->GetAsLogical() );
				  break;

			 case TYPE_COLOR:
				  _SetValueColor( m_ptrApp, strSection, strEntry, 
								pCell->GetAsColor( ) );
				  break;

			 case TYPE_SHADING:
				  _SetValueColor( m_ptrApp, strSection, strEntry, 
								pCell->GetAsColor( ) );
				  break;

			 case TYPE_FONT:
				  {
					  LOGFONT *fnt = pCell->GetAsFont();

					  if( fnt )
					  {
						  if( m_lVersion == 1 )
						  {
							_SetValue( m_ptrApp, strSection, strEntry, 
											pCell->GetAsFontString() );
							strEntry = strEntry.Left( strEntry.GetLength() - 1 );
							_SetValueInt( m_ptrApp, strSection, strEntry + "_Height\\",
											fnt->lfHeight );
							_SetValueInt( m_ptrApp, strSection, strEntry + "_Weight\\",
											fnt->lfWeight );
							_SetValueInt( m_ptrApp, strSection, strEntry + "_Italic\\",
											fnt->lfItalic );
							_SetValueInt( m_ptrApp, strSection, strEntry + "_Underline\\",
											fnt->lfUnderline );
							_SetValueInt( m_ptrApp, strSection, strEntry + "_StrikeOut\\",
											fnt->lfStrikeOut );
							_SetValueInt( m_ptrApp, strSection, strEntry + "_CharSet\\",
											fnt->lfCharSet );
							_SetValueInt( m_ptrApp, strSection, strEntry + "_PitchAndFamily\\",
											fnt->lfPitchAndFamily );
						  }
						  else if( m_lVersion == 2 )
						  {
							_SetValue( m_ptrApp, strSection + "\\" + strEntry, "Name", pCell->GetAsFontString() );
							_SetValueInt( m_ptrApp, strSection + "\\" + strEntry, "Size",pCell->m_nPointFontSize / 10 );
							_SetValueInt( m_ptrApp, strSection + "\\" + strEntry, "Italic", fnt->lfItalic != 0 ? 1: 0 );
							_SetValueInt( m_ptrApp, strSection + "\\" + strEntry, "Underline", fnt->lfUnderline != 0 ? 1 : 0 );
							_SetValueInt( m_ptrApp, strSection + "\\" + strEntry, "StrikeOut", fnt->lfStrikeOut != 0 ? 1 : 0 );
							_SetValueInt( m_ptrApp, strSection + "\\" + strEntry, "Bold", fnt->lfWeight > FW_NORMAL ? 1 : 0 );
							_SetValueColor( m_ptrApp, strSection + "\\" + strEntry, "TextColor", pCell->m_crFontColor );
						  }
					  }

					if( m_lVersion == 1 )
					{
					  CString strPath = _GetValueString( m_ptrApp, strSection, strEntry + "_Location\\", "" );
					  if( !strPath.IsEmpty() )
					  {
						if( strPath[ strPath.GetLength() - 1] == '\\' )
						  strPath = strPath.Left( strPath.GetLength() - 1 );

						CString strName;
						for( int i = strPath.GetLength() - 1; i >= 0; i-- )
						{
							if( strPath[i] != '\\' )
								strName.Insert( 0, strPath[i] );
							else
								break;
						}

						

						if( !strName.IsEmpty() )
						{
							strPath = strPath.Left( strPath.GetLength() - strName.GetLength() );

							_SetValueInt( m_ptrApp, strPath, strName + "_Height\\", font_size( fnt->lfHeight ) );
			

							_SetValueInt( m_ptrApp, strPath, strName + "_Bold\\",
										fnt->lfWeight == FW_BOLD);

							_SetValueInt( m_ptrApp, strPath, strName + "_Underline\\",
										fnt->lfUnderline );

							_SetValueInt( m_ptrApp, strPath, strName + "_Italic\\",
										fnt->lfItalic );

						  _SetValueInt( m_ptrApp, strPath, strName + "_StrikeOut\\",
										fnt->lfStrikeOut );


							_SetValue( m_ptrApp, strPath, strName + "_Name\\",
										pCell->GetAsFontString() );
						}
					  }
					}
				  }
				  break;

			 case TYPE_VALUE:
			 case TYPE_VALUE_COMBO:
				  _SetValue( m_ptrApp, strSection, strEntry, 
								pCell->GetAsValue( ) );
				  break;

			 default:
				 return FALSE;
		 }


	}

	else
	{
		return FALSE;
	}
#endif
	return TRUE;
}





void CVTList::StoreToData() 
{
#ifndef FOR_HOME_WORK

	if( m_ptrApp == NULL ) return;

	for( int iCol=0;iCol<m_listCtrl.GetColumnCount();iCol++)
	{
		if( m_listCtrl.IsValidColumnIndex( iCol ) )
		{			

			for( int iRow=0;iRow<m_listCtrl.GetRowCount();iRow++)
			{
				CBaseCell* pCell = m_listCtrl.GetCell( iRow, iCol );
				if( pCell )
				{

					CString strKeyValue = pCell->GetKeyValue( );

					CString strSection;
					CString strEntry;

					if( ::GetSectionEntry( strKeyValue, strSection, strEntry ) )
					{
						datatype_e datatype = m_listCtrl.GetColumnType( iCol );

						WriteToShellData(pCell, datatype );
					}
				}
			}
		}
	}
#endif
}



void CVTList::GetClientRect(LPRECT lpRect)
{
	if(GetSafeHwnd() == NULL)
	{		
		int nWidth, nHeight;
		GetControlSize( &nWidth, &nHeight );

		lpRect->left = lpRect->top = 0;		
		lpRect->right = nWidth;
		lpRect->bottom = nHeight;
	}
	else
		COleControl::GetClientRect(lpRect);

}


short CVTList::GetColumnCount() 
{
	return (short)m_listCtrl.GetColumnCount();
}

short CVTList::GetRowCount() 
{
	return (short)m_listCtrl.GetRowCount();
}


short CVTList::GetCurentColumn() 
{	
	return m_listCtrl.GetCurentColumn();
}

short CVTList::GetCurentRow() 
{	
	return m_listCtrl.GetCurentRow();
}

void CVTList::SetCurentRow(short nRow) 
{	
	m_listCtrl.SetCurentRow( nRow );
}

void CVTList::SetCurentCol(short nCol) 
{	
	m_listCtrl.SetCurentCol( nCol );
}

void CVTList::RestoreFromData() 
{	
	RestoreFromShellData();
}

/////////////////////////////////////////////////////////////////////////////////////
// function for visible & enable control status & SetFocus
/////////////////////////////////////////////////////////////////////////////////////
BOOL CVTList::GetDisabled()
{
	return !::_IsWindowEnable( this );
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTList::SetDisabled(BOOL bNewValue)
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	EnableWindow( !bNewValue );

	m_listCtrl.EnableWindow( !bNewValue );

	CHeaderCtrl* pHeader = m_listCtrl.GetHeaderCtrl();
	if( pHeader )
		pHeader->EnableWindow( !bNewValue );

	for( long lPos = m_lstExtHeaderCtls.head(); lPos; lPos = m_lstExtHeaderCtls.next( lPos ) )
	{
		CListHeaderCtrl *pCtrl = m_lstExtHeaderCtls.get( lPos );
		pCtrl->EnableWindow( !bNewValue );	
	}
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CVTList::GetVisible()
{
	return ::_IsWindowVisible( this );
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTList::SetVisible(BOOL bNewValue)
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	if( !bNewValue )
		ShowWindow( SW_HIDE );
	else
		ShowWindow( SW_SHOWNA );


	if( ::IsWindow( m_listCtrl.GetSafeHwnd() ) )
	{
		if( !bNewValue )
			m_listCtrl.ShowWindow( SW_HIDE );
		else
			m_listCtrl.ShowWindow( SW_SHOWNA );
	}

	


}

/////////////////////////////////////////////////////////////////////////////////////
void CVTList::SetFocus()
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;
	
	CWnd::SetFocus( );
}

/////////////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTList::GetEditBackColor()
{
	return m_listCtrl.GetEditBackColor();
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTList::SetEditBackColor(OLE_COLOR nNewValue)
{
	m_listCtrl.SetEditBackColor( nNewValue );
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTList::GetEditTextColor()
{
	return m_listCtrl.GetEditTextColor();
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTList::SetEditTextColor(OLE_COLOR nNewValue)
{
	m_listCtrl.SetEditTextColor( nNewValue );
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTList::ForceEditCell( short nColumn, short nRow )
{
	m_listCtrl.ForceEditCell( nColumn, nRow  );
}

/////////////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTList::GetListBkColor()
{
	return m_listCtrl.GetListBkColor();
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTList::SetListBkColor(OLE_COLOR nNewValue)
{
	m_listCtrl.SetListBkColor( nNewValue );
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTList::GetActiveCellBackColor()
{
	return m_listCtrl.GetActiveCellBackColor( );
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTList::SetActiveCellBackColor(OLE_COLOR nNewValue)
{
	m_listCtrl.SetActiveCellBackColor( nNewValue );
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTList::GetActiveCellTextColor()
{
	return m_listCtrl.GetActiveCellTextColor( );
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTList::SetActiveCellTextColor(OLE_COLOR nNewValue)
{
	m_listCtrl.SetActiveCellTextColor( nNewValue );
	SetModifiedFlag();
}	

/////////////////////////////////////////////////////////////////////////////////////
BOOL CVTList::GetUseSystemFont() 
{
	return m_bUseSystemFont;
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTList::SetUseSystemFont(BOOL bNewValue)
{
	m_bUseSystemFont = bNewValue;
	SetModifiedFlag();	

	m_listCtrl.ReMeasureItem();

	UpdateControl();	
}

void CVTList::ClearCombo( short nColumn, short nRow )
{
	CBaseCell *pCell = m_listCtrl.GetCell( nRow, nColumn );
	if( pCell )
		pCell->ClearCombo();
}

BOOL CVTList::AddComboString( short nColumn, short nRow, LPCTSTR pbStrValue )
{
	CBaseCell *pCell = m_listCtrl.GetCell( nRow, nColumn );
	if( pCell )
	{
		pCell->AddComboValue( pbStrValue );
		return TRUE;
	}
	return FALSE;
}

BOOL CVTList::RemoveComboItem( short nColumn, short nRow, short nIndex )
{
	CBaseCell *pCell = m_listCtrl.GetCell( nRow, nColumn );
	if( pCell )
		return pCell->RemoveComboValue( nIndex );
	return FALSE;
}


BOOL CVTList::GetUseSimpleFontType()
{
	return m_bUseSimpleFontType;
}

void CVTList::SetUseSimpleFontType(BOOL bNewValue)
{
	m_bUseSimpleFontType = bNewValue;
	SetModifiedFlag();
}

void CVTList::OnEnableResizeColumnsChanged()
{
	UpdateControl( );
}

OLE_COLOR CVTList::GetCellBackColor(short nColumn, short nRow )
{
	return m_listCtrl.GetCellBackColor( nColumn, nRow );
}
OLE_COLOR CVTList::GetCellForeColor(short nColumn, short nRow )
{
	return m_listCtrl.GetCellForeColor( nColumn, nRow );
}

void CVTList::SetCellBackColor(short nColumn, short nRow, OLE_COLOR color)
{
	m_listCtrl.SetCellBackColor( nColumn, nRow, color );
	UpdateControl();
}

void CVTList::SetCellForeColor(short nColumn, short nRow, OLE_COLOR color)
{
	m_listCtrl.SetCellForeColor( nColumn, nRow, color );
	UpdateControl();
}

void CVTList::ClearExtHeaders()
{
	m_listHeaders.clear();
	UpdateControl();
}

void CVTList::SetUseExtHeader( short bUse )
{
	m_bUseExtHeader = bool( bUse != 0 );
	UpdateControl();
	bExtHead=m_bUseExtHeader;

	m_listHeadersMain.clear();

	{
		long lCount = m_listCtrl.GetHeaderCtrl()->GetItemCount();
		for( int i = 0; i < lCount; i++ )
		{
			XHeaderData *pData = new XHeaderData;
			m_listHeadersMain.add_tail( pData );
		}
	}

	_create_header_ctrls();
	
	SetModifiedFlag();
	SendWM_SIZE();
}

long CVTList::GetUseExtHeader()
{
	return m_bUseExtHeader;
}

long CVTList::GetExtHeadersSize()
{
	return m_listHeaders.count();
}

void CVTList::SetExtHeaderData( short nLev, short nItem, LPCTSTR pbstrName, short nAssignment, short nWidth, short nDel )
{
	if(nItem==0)
		m_zeroColumnWidth=nWidth;
	if(nItem==0 && m_sNameColumn.GetCount()>0)
		m_sNameColumn.RemoveAll();

	if (nItem==0 && nDel==-2)
	{
		CHeaderCtrl *hed = m_listCtrl.GetHeaderCtrl();
		int cc = hed->GetItemCount();
		for(int i=cc-1;i>0;i--)
			m_listCtrl._DeleteColumn(i);
		hed->ShowWindow(TRUE);
	}
	if(nItem==0 && nDel>=0)
	{
		m_listCtrl._DeleteColumn(nDel);
		CHeaderCtrl *hed = m_listCtrl.GetHeaderCtrl();
		hed->ShowWindow(TRUE);
	}

	CString strCaption=pbstrName;
	BOOL bWasFind=false;
	if(!m_bUseExtHeader)
{
		BOOL bFind=false;
		while(!bFind)
		{
			int nCF = strCaption.Find('|',0);
			if(nCF>=0)
			{
				strCaption.Delete(0,nCF+1);
				bWasFind=true;
			}
			else
				bFind=true;
		}
	}
	else
	{
		int nCF = strCaption.Find('|',0);
		if(nCF>=0)
			bWasFind=true;
	}

	m_sNameColumn.Add(pbstrName);
	pbstrName=(LPCTSTR)strCaption;
	if((m_listCtrl.GetColumnCount()-1)>=nItem)
	{
		m_listCtrl.SetColumnCaption(nItem,pbstrName);
	}
	else
	{
/*		LV_COLUMN lvc; 
		lvc.iSubItem = nItem+1;
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cx = nWidth; 
		lvc.pszText = (LPSTR)pbstrName;

		ListView_InsertColumn(hwndLV, lvc.iSubItem, &lvc);*/

//		m_listCtrl.InsertColumn(lvc.iSubItem, &lvc);
		//AddColumn(nItem);
		m_listCtrl._AddColumn();
		m_listCtrl.SetColumnCaption(nItem,pbstrName);
	}
	short curWidth=m_listCtrl.GetColumnWidth(nItem);
	if(bWasFind || curWidth!=nWidth)
	{
		m_listCtrl.SetColumnWidth(nItem,nWidth+1);
		m_listCtrl.SetColumnWidth(nItem,nWidth);
	}
	//m_listCtrl.UpdateData();
	
	
	for( int i = m_listHeaders.count(); i < nLev + 1; i++ )
	{
		typeHD *item = new typeHD;
		m_listHeaders.add_tail( item );
	}

	for( long lPos = m_listHeaders.head(), q = 0; lPos; lPos = m_listHeaders.next( lPos ), q++ )
	{
		if( q == nLev )
		{
			typeHD *pLevel = m_listHeaders.get( lPos );

			if( pLevel )
			{
				for( int i = pLevel->count(); i < nItem + 1; i++ )
				{
					XHeaderData *pData = new XHeaderData;
					pLevel->add_tail( pData );
				}

				for( long lPos2 = pLevel->head(), w = 0; lPos2; lPos2 = pLevel->next( lPos2 ), w++ )
				{
					if( w == nItem )
					{
						XHeaderData *pData = pLevel->get( lPos2 );
						CString strName=m_sNameColumn.GetAt(nItem);
						pData->strName = _change_chars( /*pbstrName*/strName, "\\n", "\r\n" );
						pData->nAssignment = nAssignment;
						pData->lWidth = nWidth;

						break;
					}
				}
			}
			break;
		}
	}

	UpdateControl();
}

BSTR CVTList::GetExtHeaderName( short nLev, short nItem )
{
	for( long lPos = m_listHeaders.head(), q = 0; lPos; lPos = m_listHeaders.next( lPos ), q++ )
	{
		if( q == nLev )
		{
			typeHD *pLevel = m_listHeaders.get( lPos );

			if( pLevel )
			{
				for( long lPos2 = pLevel->head(), w = 0; lPos2; lPos2 = pLevel->next( lPos2 ), w++ )
				{
					if( w == nItem )
					{
						XHeaderData *pData = pLevel->get( lPos2 );
						return _change_chars( pData->strName, "\r\n", "\\n" ) .AllocSysString();
					}
				}
			}
		}
	}

	return 0;
}

long CVTList::GetExtHeaderAssignment( short nLev, short nItem )
{
	for( long lPos = m_listHeaders.head(), q = 0; lPos; lPos = m_listHeaders.next( lPos ), q++ )
	{
		if( q == nLev )
		{
			typeHD *pLevel = m_listHeaders.get( lPos );

			if( pLevel )
			{
				for( long lPos2 = pLevel->head(), w = 0; lPos2; lPos2 = pLevel->next( lPos2 ), w++ )
				{
					if( w == nItem )
					{
						XHeaderData *pData = pLevel->get( lPos2 );
						return pData->nAssignment;
					}
				}
			}
		}
	}

	return 0;
}

long  CVTList::GetExtHeadersColumnSize( short nLev )
{
	for( long lPos = m_listHeaders.head(), q = 0; lPos; lPos = m_listHeaders.next( lPos ), q++ )
	{
		if( q == nLev )
		{
			typeHD *pLevel = m_listHeaders.get( lPos );

			if( pLevel )
				return pLevel->count();
		}
	}
	return 0;
}

long CVTList::GetExtHeaderWidth( short nLev, short nItem )
{
	for( long lPos = m_listHeaders.head(), q = 0; lPos; lPos = m_listHeaders.next( lPos ), q++ )
	{
		if( q == nLev )
		{
			typeHD *pLevel = m_listHeaders.get( lPos );

			if( pLevel )
			{
				for( long lPos2 = pLevel->head(), w = 0; lPos2; lPos2 = pLevel->next( lPos2 ), w++ )
				{
					if( w == nItem )
					{
						XHeaderData *pData = pLevel->get( lPos2 );
						return pData->lWidth;
					}
				}
			}
		}
	}

	return 0;
}

void CVTList::_create_header_ctrls()
{
 	bool btemp = m_bEnableResizeColumns;
	m_bEnableResizeColumns = true;
 	m_listCtrl.m_headerCtrl.m_pMan = &m_Man;
	m_lstExtHeaderCtls.clear();
	m_Man.ClearItems();

	if( !m_bUseExtHeader)
	{
		m_bEnableResizeColumns = btemp;
		return;
	}

	/*for( long lPos = m_listHeaders.head(); lPos; lPos = m_listHeaders.next( lPos ) )
	{
		typeHD *pLevel = m_listHeaders.get( lPos );

		if( pLevel )
		{
			CListHeaderCtrl *pCtrl = new CListHeaderCtrl;
			m_lstExtHeaderCtls.add_tail( pCtrl );

			pCtrl->m_pMan = &m_Man;
			pCtrl->m_plist	 = &m_listCtrl;

			RECT rcRect = {0 ,0, 10, 10 };
			pCtrl->Create( WS_CHILDWINDOW | WS_VISIBLE | HDS_HORZ | HDS_FLAT | HDS_FULLDRAG, rcRect, this, lPos ); 
			pCtrl->SetFont( m_listCtrl.GetHeaderCtrl()->GetFont() );

			m_Man.AddItem( pCtrl );

			for( long lPos2 = pLevel->head(), q = 0; lPos2; lPos2 = pLevel->next( lPos2 ), q++ )
			{
				XHeaderData *pData = pLevel->get( lPos2 );

				HDITEM item;

				item.mask	= HDI_TEXT | HDI_WIDTH | HDI_FORMAT;
				item.cxy		= pData->lWidth;
				item.fmt		= HDF_STRING | HDF_CENTER;

				item.pszText = pData->strName.GetBuffer(0);

				pCtrl->InsertItem( q, &item );
			}
		}
	}*/

	m_listCtrl.GetHeaderCtrl()->SendMessage( WM_USER + 0x9876, 0 , 0 );
	//_create_assignment_map();
	m_bEnableResizeColumns = btemp;
}

void CVTList::_create_assignment_map()
{
	/*CHeaderCtrl *pCtrl = m_listCtrl.GetHeaderCtrl();

	for( long lPos = m_listHeaders.head(), lPosM = m_lstExtHeaderCtls.head(); lPos; lPos = m_listHeaders.next( lPos ), lPosM = m_lstExtHeaderCtls.next( lPosM ) )
	{
		typeHD *pLevel = m_listHeaders.get( lPos );

		if( pLevel )
		{
			int nCounter = 0;
			for( long lPos2 = pLevel->head(); lPos2; lPos2 = pLevel->next( lPos2 ) )
			{
				XHeaderData *pData = pLevel->get( lPos2 );

				pData->pHeaderLow		= pCtrl;
				pData->nFrom				= nCounter;
				nCounter				  += pData->nAssignment;
			}
		}

		pCtrl = m_lstExtHeaderCtls.get( lPosM );
	}

 	for( long lPos = m_listHeaders.head(), lPosM = m_lstExtHeaderCtls.head(); lPos; lPos = m_listHeaders.next( lPos ), lPosM = m_lstExtHeaderCtls.next( lPosM ) )
	{
		typeHD *pLevel = m_listHeaders.get( lPos );
		if( pLevel )
		{
 			for( long lPos2 = pLevel->head(), q = 0; lPos2; lPos2 = pLevel->next( lPos2 ), q++ )
			{
				XHeaderData *pData = pLevel->get( lPos2 );

				if( pData->nAssignment == 1 )
				{
					HDITEM hItem = {0};

					CHeaderCtrl *pCtrl = m_lstExtHeaderCtls.get( lPosM );
					
					hItem.mask |= HDI_LPARAM | HDI_FORMAT | HDI_WIDTH;
					pCtrl->GetItem(  q, &hItem );
					
					hItem.lParam = LEFT_BORDER  |  TOP_BORDER | RIGHT_BORDER ;

					pCtrl->SetItem( q, &hItem );

				}
				else
				{
					HDITEM hItem = {0};

					CHeaderCtrl *pCtrl = m_lstExtHeaderCtls.get( lPosM );
					
					hItem.mask |= HDI_LPARAM | HDI_WIDTH;
					pCtrl->GetItem(  q, &hItem );
					
					hItem.lParam =  LEFT_BORDER | BOTTOM_BORDER | RIGHT_BORDER| TOP_BORDER;

					pCtrl->SetItem( q, &hItem );
				}

 				{
					if( pData->nAssignment == 1 )
					{
						HDITEM hItem = {0};

						CHeaderCtrl *pCtrl = m_lstExtHeaderCtls.get( lPosM );
						
						hItem.mask |= HDI_LPARAM | HDI_FORMAT | HDI_WIDTH;
						pData->pHeaderLow->GetItem( pData->nFrom, &hItem );
						
						if( hItem.lParam )
							hItem.lParam &= ~TOP_BORDER;
						else
							hItem.lParam =  LEFT_BORDER  |  BOTTOM_BORDER | RIGHT_BORDER;

						pData->pHeaderLow->SetItem( pData->nFrom, &hItem );
					}
					else
					{
						for( int zz = 0; zz < pData->nAssignment; zz++ )
						{
							HDITEM hItem = {0};
							hItem.mask |= HDI_LPARAM | HDI_WIDTH;
							pData->pHeaderLow->GetItem( pData->nFrom + zz, &hItem );
							
							if( lPos == m_listHeaders.head() || !hItem.lParam )
								hItem.lParam =  LEFT_BORDER | BOTTOM_BORDER | RIGHT_BORDER| TOP_BORDER;

							pData->pHeaderLow->SetItem( pData->nFrom + zz, &hItem );
						}
					}
				}
			}
		}
	}  
	_calc_header_groups( 0, -1, 0, -1, "", -1 );*/
}

int CVTList::_calc_header_groups( CHeaderCtrl *pCtr, int nitem, int nIndex, int nMaxCount, CString strMainName, int nParentAssignment )
{
	/*CHeaderCtrl *pCtrlMain = m_listCtrl.GetHeaderCtrl();

	if( pCtr == pCtrlMain )
	{
 		for( long lPos2 = m_listHeadersMain.head(), q = 0; lPos2; lPos2 = m_listHeadersMain.next( lPos2 ), q++ )
		{
			if( nitem == -1 || q == nitem )
			{
				XHeaderData *pData = m_listHeadersMain.get( lPos2 );

				pData->strForienName = strMainName.IsEmpty() ? pData->strName : strMainName;

				if( nMaxCount > pData->nGrCount )
				{
					pData->nGrCount = nMaxCount;
					pData->nGrIndex =  nIndex;
				}
			}
		}

		if( nParentAssignment == 1 )
			return 1;
		return 0;
	}

	for( long lPos = m_listHeaders.tail(), lPosM = m_lstExtHeaderCtls.tail(); lPos; lPos = m_listHeaders.prev( lPos ), lPosM = m_lstExtHeaderCtls.prev( lPosM ) )
	{
		CHeaderCtrl *pCtrlCur = m_lstExtHeaderCtls.get( lPosM );

		if( !pCtr || pCtrlCur == pCtr )
		{
			typeHD *pLevel = m_listHeaders.get( lPos );
			if( pLevel )
			{
 				for( long lPos2 = pLevel->head(), q = 0; lPos2; lPos2 = pLevel->next( lPos2 ), q++ )
				{
					if( nitem == -1 || q == nitem )
					{
						XHeaderData *pData = pLevel->get( lPos2 );

						if( pData->nAssignment == 1 || nParentAssignment == 1 )
						{
							int iCol = 0;
							
							if( nMaxCount == -1 )
								iCol = 1 + _calc_header_groups( pData->pHeaderLow, pData->nFrom, nIndex + 1, -1, "", pData->nAssignment );
							pData->strForienName = strMainName.IsEmpty() ? pData->strName : strMainName;
							_calc_header_groups( pData->pHeaderLow, pData->nFrom, nIndex + 1, max( nMaxCount, iCol ),  pData->strForienName, pData->nAssignment );

							if( pData->nGrCount <  max( nMaxCount, iCol ) )
							{
								pData->nGrCount = max( nMaxCount, iCol );
								pData->nGrIndex =  nIndex;
							}

							if( nitem != -1 )
								return iCol;
						}

						if( nitem != -1 )
							return 1;
					}
				}
			}
		}
	}*/
	return 0;
}

void CVTList::_apply_header_resize( CHeaderCtrl *pCtrl, int iItem, int nWidth, bool bAsgnHeader, bool InreaseChW, bool InternalCall, bool MainCall )
{
	/*if( !m_listHeaders.count() || !m_lstExtHeaderCtls.count() )
		return;

 	CHeaderCtrl *pCtrlM = m_listCtrl.GetHeaderCtrl();

  	if( pCtrlM == pCtrl  )
	{
		if( bAsgnHeader )
		{
			if( !m_bLock )
			{
				LVCOLUMN hCol = {0};

				hCol.mask |= LVCF_WIDTH;
				m_listCtrl.GetColumn( iItem, &hCol );

				hCol.cx += nWidth;

				m_listCtrl.SetColumn( iItem, &hCol );
				m_listCtrl.InvalidateRect( 0, TRUE );
			}
		}
		else
		{
			typeHD *pLevel = m_listHeaders.get( m_listHeaders.head() );
			if( pLevel )
			{
				for( long lPos2 = pLevel->head(), z = 0; lPos2; lPos2 = pLevel->next( lPos2 ), z++ )
				{
 					XHeaderData *pData = pLevel->get( lPos2 );
					if( iItem >= pData->nFrom && iItem < ( pData->nFrom + pData->nAssignment ) )
					{
						int nw = 0;
						for( int zz = 0; zz < pData->nAssignment; zz++ )
						{
							LVCOLUMN hCol = {0};

							hCol.mask |= LVCF_WIDTH;
							m_listCtrl.GetColumn( pData->nFrom + zz, &hCol );

							nw += hCol.cx;
						}
						CHeaderCtrl  *pCtrlH = m_lstExtHeaderCtls.get( m_lstExtHeaderCtls.head() );
						
						m_bLock = true;
						_apply_header_resize( pCtrlH, z, nw, false, false, true, false );
						m_bLock = false;
						return;
					}
				}
			}
		}

		return;
	}

	for( long lPos = m_listHeaders.head(), lPosM = m_lstExtHeaderCtls.head(); lPos; lPos = m_listHeaders.next( lPos ), lPosM = m_lstExtHeaderCtls.next( lPosM ) )
	{
		CHeaderCtrl  *pCtrlH = m_lstExtHeaderCtls.get( lPosM );

		if( pCtrlH == pCtrl )
		{
			typeHD *pLevel = m_listHeaders.get( lPos );
			if( pLevel )
			{
				if( !InternalCall || lPos == m_listHeaders.tail() )
				{
					for( long lPos2 = pLevel->head(), q = 0; lPos2; lPos2 = pLevel->next( lPos2 ), q++ )
					{
						if( q == iItem )
						{
 							XHeaderData *pData = pLevel->get( lPos2 );

							int nW = nWidth - pData->lWidth;

							if( bAsgnHeader )
							{
								nW = pData->lWidth;
								pData->lWidth = pData->lWidth + nWidth;

								nW = nWidth;
								

								HDITEM hItem = {0};
								
								hItem.mask |= HDI_WIDTH;
								hItem.cxy = pData->lWidth;

								pCtrl->SetItem( iItem, &hItem );
							}
							else
							{
								pData->lWidth = nWidth;

								HDITEM hItem = {0};
								
								hItem.mask |= HDI_WIDTH;
								hItem.cxy = pData->lWidth;

								pCtrl->SetItem( iItem, &hItem );
							}


							_apply_header_resize( pData->pHeaderLow, pData->nFrom + pData->nAssignment - 1, InreaseChW ? nW : 0, true, InreaseChW, false, false );
							return;
						}
					}
				}
				else
				{
					typeHD *pLevelUp = m_listHeaders.get( m_listHeaders.next( lPos ) );
					if( pLevelUp )
					{
						for( long lPos2 = pLevelUp->head(), q = 0; lPos2; lPos2 = pLevelUp->next( lPos2 ), q++ )
						{
 							XHeaderData *pData = pLevelUp->get( lPos2 );

							if( iItem >= pData->nFrom && iItem < ( pData->nFrom + pData->nAssignment ) )
							{
								if( MainCall ) 
									_apply_header_resize( pCtrl, iItem, nWidth, bAsgnHeader, InreaseChW, false, false );

								for( long lPos2 = pLevel->head(), b = 0; lPos2; lPos2 = pLevel->next( lPos2 ), b++ )
								{
									if( b == iItem )
									{
 										XHeaderData *pData = pLevel->get( lPos2 );

										pData->lWidth = nWidth;

										HDITEM hItem = {0};
										
										hItem.mask |= HDI_WIDTH;
										hItem.cxy = pData->lWidth;

										pCtrl->SetItem( iItem, &hItem );
										break;
									}
								}

								int nw = 0;
								for( int zz = 0; zz < pData->nAssignment; zz++ )
								{
									HDITEM hItem = {0};
									
									hItem.mask |= HDI_WIDTH;
									hItem.cxy = pData->lWidth;

									pCtrl->GetItem(  pData->nFrom + zz, &hItem );

									nw += hItem.cxy;
								}

								CHeaderCtrl  *pCtrlH = m_lstExtHeaderCtls.get( m_lstExtHeaderCtls.next( lPosM ) );
								_apply_header_resize( pCtrlH, q, nw, false, false, true, false );
								return;
							}
						}
					}
				}
			}
		}
	}*/
}

BOOL CVTList::_can_be_resizable( CHeaderCtrl *pCtrl, int iItem, int nDir )
{
 	/*CHeaderCtrl *pCtrlM = m_listCtrl.GetHeaderCtrl();

 	if( pCtrlM != pCtrl )
	{
		for( long lPos = m_listHeaders.head(), lPosM = m_lstExtHeaderCtls.head(); lPos; lPos = m_listHeaders.next( lPos ), lPosM = m_lstExtHeaderCtls.next( lPosM ) )
		{
			CHeaderCtrl  *pCtrlH = m_lstExtHeaderCtls.get( lPosM );

			if( pCtrlH == pCtrl )
			{
				typeHD *pLevel = m_listHeaders.get( lPos );
				if( pLevel )
				{
					for( long lPos2 = pLevel->head(), q = 0; lPos2; lPos2 = pLevel->next( lPos2 ), q++ )
					{
						if( q == iItem )
						{
							XHeaderData *pData = pLevel->get( lPos2 );
							CHeaderCtrl  *pCtrlLow = pData->pHeaderLow;

							int nw = 0;

							if( !_can_be_resizable( pCtrlLow, pData->nFrom + pData->nAssignment - 1, nDir ) )
								return false;

							HDITEM hItem = {0};
							
							hItem.mask |= HDI_WIDTH;
							hItem.cxy = pData->lWidth;

							pCtrl->GetItem(  iItem, &hItem );

							if( hItem.cxy - nDir < 2 )
								return false;

							return true;
						}
					}
				}
			}
		}
	}
	else
	{
		HDITEM hItem = {0};
		
		hItem.mask |= HDI_WIDTH;
		pCtrl->GetItem(  iItem, &hItem );

		if( hItem.cxy - nDir < 2 )
			return false;
	}
	return true;*/
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CListHeaderCtrl message handlers

LRESULT CVTList::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
 	if( message == WM_USER + 0x9876 )
		return LRESULT( m_bUseExtHeader ); 

/*	if( message == ( WM_USER + 0x4554 ) )
		_apply_header_resize( (CHeaderCtrl *)(CHeaderCtrl::FromHandle( (HWND)lParam ) ), HIWORD( wParam ), LOWORD( wParam ), false, true, true, true );
	else if( message == ( WM_USER + 0x5445 ) )
 		return _can_be_resizable( (CHeaderCtrl *)(CHeaderCtrl::FromHandle( (HWND)lParam ) ), HIWORD( wParam ), LOWORD( wParam ) );

	else */if( message == ( WM_USER + 0x7887 ) )
	{
		HEADER_DRAW_PROP *pProp = (HEADER_DRAW_PROP *)lParam;
		
		CHeaderCtrl *pCtrlM = (CHeaderCtrl *)CHeaderCtrl::FromHandle( pProp->hHdr );

		if( pCtrlM == m_listCtrl.GetHeaderCtrl() )
		{
			for( long lPos2 = m_listHeadersMain.head(), q = 0; lPos2; lPos2 = m_listHeadersMain.next( lPos2 ), q++ )
			{
				XHeaderData *pData = m_listHeadersMain.get( lPos2 );

				if( pProp->iItem == q )
				{
					pProp->iGrCount = pData->nGrCount;
					pProp->iGrIndex = pData->nGrIndex;
					strcpy( pProp->szText, pData->strForienName );
					return 1;
				}
			}
		}

		for( long lPos = m_listHeaders.tail(), lPosM = m_lstExtHeaderCtls.tail(); lPos; lPos = m_listHeaders.prev( lPos ), lPosM = m_lstExtHeaderCtls.prev( lPosM ) )
		{
			if( pCtrlM == m_lstExtHeaderCtls.get( lPosM ) )
			{
				typeHD *pLevel = m_listHeaders.get( lPos );
				if( pLevel )
				{
					for( long lPos2 = pLevel->head(), q = 0; lPos2; lPos2 = pLevel->next( lPos2 ), q++ )
					{
						XHeaderData *pData = pLevel->get( lPos2 );

						if( pProp->iItem == q )
						{
							pProp->iGrCount = pData->nGrCount;
							pProp->iGrIndex = pData->nGrIndex;
							strcpy( pProp->szText, pData->strForienName );
							return 1;
						}
					}
				}
			}
		}
	}
/*	else if(message==WM_PAINT)
	{
		if( m_listCtrl.GetSafeHwnd() )
		{
			for( long lPos = m_lstExtHeaderCtls.head(); lPos; lPos = m_lstExtHeaderCtls.next( lPos ) )
			{
				CListHeaderCtrl *pCtrl = m_lstExtHeaderCtls.get( lPos );
				CDC *hdc = pCtrl->GetWindowDC();
				
				RECT rcHeader = {0};
				pCtrl->GetClientRect(&rcHeader);
				int iScroll;
				iScroll=m_listCtrl.GetScrollPos(SB_HORZ);
				HPEN hPen = ::CreatePen( PS_SOLID, 1, RGB(0,0,0) );
				HPEN hold = (HPEN)::SelectObject( hdc->m_hDC, hPen );
				hdc->MoveTo(iScroll-1,rcHeader.top);
				hdc->LineTo(iScroll-1,rcHeader.bottom);
				::SelectObject( hdc->m_hDC, hold );
				::DeleteObject( hPen );
				ReleaseDC(hdc); 
				pCtrl->UpdateWindow();
			}
		}
	}*/

//		return	HeaderExWndProc(m_listCtrl.m_hWnd, message, wParam, lParam);
	return COleControl::WindowProc(message, wParam, lParam);
}











/*
	Функция облегчения задания ListView'у навороченного хедера
 */
WNDPROC InitListViewEx(HWND hWndLV)
{
	WNDPROC ret = NULL;
	char out[200];

// Типа проверить класс контрола, чтоб не получить лажи
// в случае криворукости программера
	GetClassName(hWndLV,out,sizeof(out));
	if(!lstrcmpi(out,WC_LISTVIEW))
	{
	// Найдем хедер и выполним переопределения для него
		ret = InitHeaderEx(ListView_GetHeader(hWndLV));
	}
	return ret;
}

/*
	Функция задания окошку SysHeader32 навороченного стиля
 */
WNDPROC InitHeaderEx(HWND hWndHdr)
{
	WNDPROC ret = NULL;
	char out[200];

// Типа проверить класс контрола, чтоб не получить лажи
// в случае криворукости программера
	GetClassName(hWndHdr,out,sizeof(out));
	if(!lstrcmpi(out,WC_HEADER))
	{
		// Подменим WndProc окна на свой
		ret=(WNDPROC)SetWindowLong(hWndHdr,GWLP_WNDPROC,(LONG)HeaderExWndProc);
		//ret = (BOOL)SetWindowLong(hWndHdr,GWL_WNDPROC,(LONG)HeaderExWndProc);
	}
	return ret;
}


/*
    А эта ф-ция написана для создания градаций
	У хедера есть наворот - когда цветов больше 256,
	рисует "выпуклый" заголовок
 */
DWORD OffsetColor(DWORD color,BYTE offset,BOOL dir)
{
	DWORD ncolor = color;
	if(!dir)
	{
		if(GetRValue(ncolor)>offset) ncolor-=offset;				else ncolor&=0xffffff00;
		if(GetGValue(ncolor)>offset) ncolor-=(DWORD)(offset<<8);	else ncolor&=0xffff00ff;
		if(GetBValue(ncolor)>offset) ncolor-=(DWORD)(offset<<16);	else ncolor&=0xff00ffff;
	}
	else
	{
		if((GetRValue(ncolor)+offset)<0x100) ncolor+=offset;				else ncolor|=0x000000ff;
		if((GetGValue(ncolor)+offset)<0x100) ncolor+=(DWORD)(offset<<8);	else ncolor|=0x0000ff00;
		if((GetBValue(ncolor)+offset)<0x100) ncolor+=(DWORD)(offset<<16);	else ncolor|=0x00ff0000;
	}
	return ncolor;
}


/*
	Структурка для быстрого отображения заголовков
*/
typedef struct HEADEREXSHOWDATA
{
	HEADEREXSHOWDATA*	next;
	int		width;
	char	szName[500];
} *LPHEADEREXSHOWDATA;

/*
	Функция расчета высоты заголовка
	Для чего просто считает количество переводов строки и
	умножает на высоту буквы "W" - стандартно для MSDN
	Ну и добавляет высоту рамки, которой будет окружен
	элемент заголовка
*/
int HeaderExTitleHeight(HWND hwnd)
{
	int titleheight = 0;
	char out[500];
	HDITEM hdi;
	hdi.mask = HDI_TEXT;
	hdi.pszText = out;
	hdi.cchTextMax = sizeof(out);

	SIZE sz;
	HDC dc = GetDC(hwnd);
	HFONT hfnt = GetWindowFont(GetParent(hwnd));
	HFONT ofnt = SelectFont(dc,hfnt);
	GetTextExtentPoint32(dc,"W",1,&sz);
	SelectFont(dc,ofnt);
	ReleaseDC(hwnd, dc);

	int cnt = Header_GetItemCount(hwnd);
	if(bExtHead)
	{
		//CString s;
		//titleheight=1;
		for(int i=0;i<cnt;i++)
		{
			Header_GetItem(hwnd, i, &hdi);
			//s=hdi.pszText;
			int tmp=0; char *a=out;
			while(a) { tmp++; a=strchr(a+1,'|'); }
			if(titleheight<tmp) titleheight=tmp;
			/*BOOL flag=false;
			int count=0;
			int res=-1;
			while(!flag)
			{
				count=s.Find('|',res+1);
				if(count>=0)
				{
					res=count;
					titleheight++;
				}
				else
					flag=true;
			}*/
		}
	}
	else
		titleheight=1;
	sz.cy+=2;//*GetSystemMetrics(SM_CXDLGFRAME)+2;
	levelHeader=titleheight;
	titleheight*=sz.cy;
	titleheight-=2;
	cyHeader=titleheight+4;
	return titleheight;
}
/*
	Эта ф-ция аналог DrawFrameControl, только заполняет прямоугольник
	градацией, если имеем более 256 цветов
	Спросите, почему не воспользоваться стандартными ф-циями?
	Дык это должно работать и на Вин95 (по заданию проекта, для которого
	весь этот сыр-бор мутился)
 */
BOOL DrawFrameControlEx(HDC hdc, LPRECT lprc, UINT uType, UINT uState)
{
	BOOL ret = FALSE;
	if(uType==DFC_BUTTON && GetDeviceCaps(hdc,BITSPIXEL)>=16)
	{
		int offset = 32;
		DWORD cl[2] = { OffsetColor(GetSysColor(COLOR_BTNFACE),offset/2,TRUE), OffsetColor(GetSysColor(COLOR_BTNFACE),offset/2,FALSE) };
		if((uState&DFCS_PUSHED) == DFCS_PUSHED)
		{
			DWORD cll = cl[0];
			cl[0] = cl[1];
			cl[1] = cll;
		}
		int r1 = GetRValue(cl[0]);
		int g1 = GetGValue(cl[0]);
		int b1 = GetBValue(cl[0]);
		int r2 = GetRValue(cl[1]);
		int g2 = GetGValue(cl[1]);
		int b2 = GetBValue(cl[1]);
		HPEN pen = CreatePen(PS_SOLID, 1, cl[0]);
		HPEN opn = SelectPen(hdc, pen);
		for(int y=0;y<=lprc->bottom-lprc->top;y++)
		{
			/*DWORD cll = RGB(
				r1+(r2-r1)*y/(lprc->bottom-lprc->top),
				g1+(g2-g1)*y/(lprc->bottom-lprc->top),
				b1+(b2-b1)*y/(lprc->bottom-lprc->top));*/
		// Видимо это старый кусок кода, т.к. я тут
		// юзал GetNearestColor для создания пера или
		// пропуска его создания, которая имеется и
		// в Вин95.
		//   Оставил так, т.к. ни где не глючило, да и
		//   ресурсы высвобождаются своевремено
			DWORD cll=GetSysColor(COLOR_BTNFACE);
			HPEN tmp = CreatePen(PS_SOLID, 1, cll);
			SelectPen(hdc, tmp);
			DeletePen(pen);
			pen = tmp;
			MoveToEx(hdc, lprc->left, lprc->top+y, NULL);
			LineTo(hdc, lprc->right, lprc->top+y);
		}
		SelectPen(hdc, opn);
		DeletePen(pen);
		//DrawEdge(hdc, lprc, ((uState&DFCS_PUSHED) == DFCS_PUSHED) ? BDR_SUNKENOUTER:EDGE_RAISED, BF_RECT);
		DrawEdge(hdc, lprc, BDR_RAISEDINNER, BF_RECT);
	}
	else
	{
	// А так рисуем, если комп-гавно, а работать нуна!
	// Или, вдруг, просим рисовать не кнопку.
	// Я же отдаю все это вам, а что попросите рисовать,
	// не в курсах
		ret = DrawFrameControl(hdc, lprc, uType, uState);
	}

	return ret;
}


/*
	Эта функция рассчитывает положение текста в рамке
	точно в центре, ну и, конечно же, именно там и
	рисует.
	Вопрос: Почему не воспользоваться флажком DT_VCENTER?
	Ответ:  Дык, он не пашет с DT_WORDBREAK!
 */
BOOL HeaderExVCenter(HDC dc, char* out, RECT* r)
{
	RECT rr,r1;
	CopyRect(&r1,r);
	CopyRect(&rr,r);
	if(((r1.right-1)>r1.left)&&((r1.bottom-7)>r1.top))
	{
		DrawFrameControlEx(dc,&r1,DFC_BUTTON,DFCS_BUTTONPUSH);
		DrawText(dc,out,-1,&r1,DT_WORDBREAK|DT_CENTER|DT_CALCRECT);
		int dist = ((rr.bottom-rr.top)-(r1.bottom-r1.top))/2;
		r1.top+=dist; r1.bottom+=dist; r1.right = rr.right;
		if(dist<0) CopyRect(&r1,&rr);
		DrawText(dc,out,-1,&r1,DT_WORDBREAK|DT_CENTER);
	}
	return TRUE;
}

/*
	Эта функция моя гордость или наоборот.
	Комментировать выбор странных входных переменных, типа
		RECT **r, char** out
	не буду, типа нужно вникнуть в ее алгоритм, а это не
	тривиально. Да и проблемы у мя с выражениями :)
	Скажу одно - функция рисует все шапочки за один проход.
	Судите сами о ее сложности по этому высказыванию...
 */
void HeaderEx_SetTit(HDC dc, RECT **r, char** out, LPHEADEREXSHOWDATA* l, int pos, int maxstr)
{
	RECT r1,r2;
	char tmp[1000];
	while(*l)
	{
		SetRect(&r1,(*r)->right,(*r)->top,(*r)->right+(*l)->width,(*r)->bottom);
		char *a=(*out);
		for(int i=0;i<=pos;i++)
		{
			a=strchr(a+1,'|');
			if(!a) 
				{ a=(*out)+lstrlen(*out); break; }
		}
		int alen=a-(*out);
		if(*a=='|' /*&& !strncmp((*out),(*l)->szName,alen)*/)
		{
			int rl=(*r)->left;
			char* b=a=(*out);
			for(int i=0;i<pos;i++)
			{
				a=strchr(a+1,'|');
				if(!a) { a=(*out)+lstrlen(*out); break; }
			}
			int aa = strchr(a+1,'|')-a;
			//if(aa+1!=lstrlen(*out))
				while(!strncmp(b,(*out),alen)) 
					HeaderEx_SetTit(dc, r, out, l, pos+1, maxstr);
			SetRect(&r2,rl,(*r)->top+((*r)->bottom-(*r)->top)*pos/maxstr,(*r)->left,(*r)->top+((*r)->bottom-(*r)->top)*(pos+1)/maxstr);
			lstrcpyn(tmp,a+(*a=='|' ? 1 : 0),aa+(*a=='|' ? 0 : 1));
			HeaderExVCenter(dc,tmp,&r2);
			if(pos) return;
			continue;
		}
		a=(*out);
		for(int i=0;i<pos;i++)
		{
			a=strchr(a+1,'|');
			if(!a) { a=(*out)+lstrlen(*out); break; }
			a++;
		}
		SetRect(&r2,(*r)->left,(*r)->top+((*r)->bottom-(*r)->top)*pos/maxstr,(*r)->right,(*r)->bottom);
		HeaderExVCenter(dc,a,&r2);
		CopyRect(*r,&r1);
		if(strncmp((*out),(*l)->szName,a-(*out))) break;
		if(*l) { (*out)=(*l)->szName; (*l)=(*l)->next; }
	}
	if(*l) { (*out)=(*l)->szName; (*l)=(*l)->next; }
	SetRect(&r1,(*r)->right,(*r)->top,2000,(*r)->bottom);
	HeaderExVCenter(dc,"",&r1);
}


/*
	А эта ф-ция готовит данные для крутик-функции
	Ни чо замечательного в ней нет :о(
 */
void HeaderEx_Title(HWND hwnd)
{
	int titleheight = 0;
	RECT r, rw, rc, *r1=&r; char *out="•";
	GetWindowRect(hwnd,&r);
	GetWindowRect(hwnd,&rw);
	GetClientRect(hwnd,&rc);
	titleheight = rw.bottom-rw.top;
	HDC dcc = GetDC(hwnd);
	HDC dc = CreateCompatibleDC(dcc);
	HBITMAP bmp = CreateCompatibleBitmap(dcc,r.right-r.left,titleheight);
	HBITMAP oldbmp = SelectBitmap(dc, bmp);
//	HDC dc = GetDC(hwnd);
	SetBkMode(dc,TRANSPARENT);
	HFONT oldfont = SelectFont(dc,GetWindowFont(GetParent(hwnd)));
	int maxstr=0;
	int cnt = Header_GetItemCount(hwnd);
	if(cnt)
	{
		int* ar = new int[cnt];
		Header_GetOrderArray(hwnd, cnt, ar);
		LPHEADEREXSHOWDATA ldata = new HEADEREXSHOWDATA[cnt+1];
		for(int i=0;i<cnt;i++)
		{
			memset(&ldata[i],0,sizeof(ldata[i]));
			HDITEM hdi;
			hdi.mask = HDI_WIDTH|HDI_TEXT;
			hdi.cxy = 0;
			hdi.pszText = ldata[i].szName;
			hdi.cchTextMax = sizeof(ldata[i].szName);
			Header_GetItem(hwnd, ar[i], &hdi);
			ldata[i].width = hdi.cxy;
			ldata[i].next = &ldata[i+1];
		}
		memset(&ldata[cnt],0,sizeof(ldata[cnt]));
		lstrcpy(ldata[cnt].szName,"          ");
		for(LPHEADEREXSHOWDATA l = ldata;l!=NULL;l=l->next)
		{
			int tmp=0; char *a=l->szName;
			while(a) { tmp++; a=strchr(a+1,'|'); }
			if(maxstr<tmp) maxstr=tmp;
		}

		SetRect(&r,0,0,0,titleheight);
		l = ldata;
		HeaderEx_SetTit(dc, &r1, &out, &l, 0, maxstr);

		delete [] ldata;
		delete [] ar;
	}
	else
	{
		SetRect(&r,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top);
		DrawFrameControlEx(dc,&r,DFC_BUTTON,DFCS_BUTTONPUSH);
	}
	SelectObject(dc,oldfont);
//	ReleaseDC(hwnd, dc);
	SetRect(&r,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top);
	BitBlt(dcc,r.left,r.top,r.right,r.bottom, dc,0,0,SRCCOPY);
	SelectBitmap(dc, oldbmp);
	DeleteBitmap(bmp);
	DeleteDC(dc);
	ReleaseDC(hwnd, dcc);
}


LRESULT CALLBACK HeaderExWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND  hTip = NULL;
	static char* buf[]={"Edit Box"};

	switch(msg)
	{
		case WM_MOUSEMOVE:
			{
				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam); 
				if(!b_createToolTip)
				{
					b_createToolTip=true;
					HWND hwndParent = GetParent(hwnd);
					hTip = CreateToolTip(hwndParent);
					BOOL res = AddTool(hTip,hwnd,NULL,0,buf[0]); 
					EnableToolTip(hTip,TRUE);
				}
				if(b_createToolTip)
				{
					int cnt = Header_GetItemCount(hwnd);
					if(cnt)
					{
						CString s;
						int* ar = new int[cnt];
						Header_GetOrderArray(hwnd, cnt, ar);
						LPHEADEREXSHOWDATA ldata = new HEADEREXSHOWDATA[cnt+1];
						for(int i=0;i<cnt;i++)
						{
							memset(&ldata[i],0,sizeof(ldata[i]));
							HDITEM hdi;
							hdi.mask = HDI_WIDTH|HDI_TEXT;
							hdi.cxy = 0;
							hdi.pszText = ldata[i].szName;
							hdi.cchTextMax = sizeof(ldata[i].szName);
							Header_GetItem(hwnd, ar[i], &hdi);
							ldata[i].width = hdi.cxy;
							ldata[i].next = &ldata[i+1];
						}
						if(cnt>1)
						{
							int sum_width=0;
							int curXHeader=0;
							int i=0;
							for(;i<cnt;i++)
							{
								if(xPos>sum_width && xPos<=sum_width+ldata[i].width)
								{
									curXHeader=i;
									break;
								}
								sum_width+=ldata[i].width;
							}	
							if(i==cnt)
								s="";
							else
							{
								int curYHeader=(int)(yPos*levelHeader)/cyHeader;
								s=ldata[curXHeader].szName;
								int nCF;
								for(int i=0;i<=curYHeader;i++)
								{
									nCF = s.Find('|',0);
									if(nCF>=0)
									{
										if(i!=curYHeader)
											s.Delete(0,nCF+1);
										else
											s.Delete(nCF,1000);
									}
								}
							}
						}
						else
						{
							s=ldata[0].szName;
							if(xPos>ldata[0].width)
								s="";
						}
						delete [] ldata;
						delete [] ar;
						UpdateTipText(hTip,hwnd,0,s);
					}
				}
				//SendMessage(hwnd,WM_MOUSEMOVE,FALSE,0);
				//CallWindowProc((WNDPROC)GetClassLong(hwnd,GCL_WNDPROC), hwnd, msg, wParam, lParam);
				/*HWND hwndParent = GetParent(hwnd);
				if(hwndParent)
					SendMessage(hwndParent, WM_MOUSEMOVE, NULL, lParam);
				else
					return TRUE;*/
				//SendMessage(hwnd,WM_MOUSEMOVE,TRUE,0);
				//return;
			}
			break;
	// Обработаем стандартное сообщение хедера для подсчета клиентской
	// области предка. Если этого не сделать, то ни хедер не увеличится
	// в высоту, ни родитель не отступит от верха...
	// В общем хорошая ситуация случится.
		case HDM_LAYOUT:
		{
			HDLAYOUT* lphl = (HDLAYOUT*)lParam;
			CallWindowProc((WNDPROC)GetClassLong(hwnd,GCLP_WNDPROC), hwnd, msg, wParam, lParam);
			if(lphl->pwpos && lphl->pwpos->cy)
			{
				lphl->prc->top = HeaderExTitleHeight(hwnd)+5;
				lphl->pwpos->cy = HeaderExTitleHeight(hwnd)+4;
			}
		}
		return TRUE;
	// Тут мы перерисовываем сам хедер, правда, говорим перерисоваться
	// самому хедеру, а потом рисуем сами, да еще и запрещаем отображение
	// перерисовки стандартному способу.
	//   Не утверждаю, что это лучший вариант и почему отказался от стандартного
	//   BeginPaint-EndPaint, под какой-то версией Виндов что-то глючило...
	//   А может и нет...
		case WM_PAINT:
			SendMessage(hwnd, WM_SETREDRAW, FALSE, 0);
			CallWindowProc((WNDPROC)GetClassLong(hwnd,GCLP_WNDPROC), hwnd, msg, wParam, lParam);
			SendMessage(hwnd, WM_SETREDRAW, TRUE, 0);
			HeaderEx_Title(hwnd);
		return 0;
	}
// А вот так мы и вызываем стандартные обработчики окна
// "Если добавляете свои навороты, то только после меня" :)
// Это делается для вынесения этой примочки в длл, без
// ущерба для здоровья при создании кода и поиске глюков
	return CallWindowProc((WNDPROC)GetClassLong(hwnd,GCLP_WNDPROC), hwnd, msg, wParam, lParam);
}

void CVTList::SetHideHeader(BOOL val) 
{		
	m_HideHeader=val;
	UpdateControl();
	Invalidate();
}

void CVTList::SetHideBorder(BOOL val) 
{		
	m_HideBorder=val;
	CRect rect, rc;
	GetClientRect(rect);
	m_listCtrl.GetClientRect(rc);
	if(m_HideBorder)
	{
		m_listCtrl.ModifyStyle( WS_BORDER, 0 );
		if(m_HideHeader)
		{
			int heightCtrl;
			if((rc.top-cyHeader+rc.Height()+2*cyHeader)>rect.bottom)
				heightCtrl=rect.bottom-(rc.top-cyHeader);
			else
				heightCtrl=rc.Height()+2*cyHeader;
			m_listCtrl.MoveWindow(rc.left-1,rc.top-cyHeader-1,m_widthCtrlList+1,heightCtrl+1);
		}
		else
		{
			rect.left-=1;
			rect.top-=1;
			rect.bottom+=1;
			rect.right+=1;
			m_listCtrl.MoveWindow(rect);
		}
	}
	else
	{
		m_listCtrl.ModifyStyle( 0, WS_BORDER );
		if(m_HideHeader)
		{
			int heightCtrl;
			if((rc.top-cyHeader+rc.Height()+2*cyHeader)>rect.bottom)
				heightCtrl=rect.bottom-(rc.top-cyHeader);
			else
				heightCtrl=rc.Height()+2*cyHeader;
			m_listCtrl.MoveWindow(rc.left+1,rc.top-cyHeader+1,m_widthCtrlList-1,heightCtrl-1);
		}
		else
		{
			rect.left+=1;
			rect.top+=1;
			rect.bottom-=1;
			rect.right-=1;
			m_listCtrl.MoveWindow(rect);
		}
	}
	UpdateControl();
	
	m_listCtrl.RedrawWindow( NULL, NULL, RDW_FRAME|RDW_INVALIDATE|RDW_INTERNALPAINT );	
}

BOOL CVTList::GetHideHeader() 
{
	return m_HideHeader;
}

BOOL CVTList::GetHideBorder() 
{
	return m_HideBorder;
}
void CVTList::OnDestroy()
{
	COleControl::OnDestroy();

	// TODO: Add your message handler code here
	SetWindowLong(ListView_GetHeader(hwndLV),GWLP_WNDPROC,(LONG)wpOrigEditProc);
}

BOOL CVTList::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText=" ";

	_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));

	*pResult = 0;
	return TRUE;
}


//-------------------------------------------------------------
HWND APIENTRY CreateToolTip(HWND hWndParent)
{
    InitCommonControls();
    HWND hTip = CreateWindowEx(
        0,TOOLTIPS_CLASS,
        0,0,0,0,0,0,
        hWndParent,0,0,0);
    return hTip;
}

//-------------------------------------------------------------
void APIENTRY FillInToolInfo(TOOLINFO* ti, HWND hWnd, UINT nIDTool)
{
    ZeroMemory(ti,sizeof(TOOLINFO));
    ti->cbSize = sizeof(TOOLINFO);
    if(!nIDTool)
    {
        ti->hwnd   = GetParent(hWnd);
        ti->uFlags = TTF_IDISHWND;
        ti->uId    = (UINT)hWnd;
    }
    else
    {
        ti->hwnd   = hWnd;
        ti->uFlags = 0;
        ti->uId    = nIDTool;
    }
}

//-------------------------------------------------------------
BOOL APIENTRY AddTool(HWND hTip, HWND hWnd, RECT* pr, UINT nIDTool, LPCTSTR szText)
{
    TOOLINFO ti;
    RECT     r = {0,0,0,0};

    FillInToolInfo(&ti, hWnd, nIDTool);
    ti.hinst  = (HINSTANCE)GetModuleHandle(NULL);
    ti.uFlags |= TTF_SUBCLASS | TTF_TRANSPARENT;
    ti.lpszText = LPSTR( szText ? szText : LPSTR_TEXTCALLBACK );
    if(!(ti.uFlags & TTF_IDISHWND))
    {
        if(!pr)
        {
            pr = &r;
            GetClientRect(hWnd, pr);
        }
        memcpy(&ti.rect, pr, sizeof(RECT));
    }
    BOOL res = SendMessage(hTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
    return res;
}

//-------------------------------------------------------------
void APIENTRY UpdateTipText(HWND hTip, HWND hWnd, UINT nIDTool, LPCTSTR lpszText)
{
    TOOLINFO ti;
    FillInToolInfo(&ti, hWnd, nIDTool);
    ti.lpszText = LPSTR( lpszText ? lpszText : LPSTR_TEXTCALLBACK );
    SendMessage(hTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
}

//-------------------------------------------------------------
void APIENTRY GetTipText(HWND hTip, HWND hWnd, UINT nIDTool, LPSTR szText)
{
    TOOLINFO ti;
    if(!szText)
        return;
    *szText = 0;
    FillInToolInfo(&ti, hWnd, nIDTool);
    ti.lpszText = szText;
    SendMessage(hTip, TTM_GETTEXT, 0, (LPARAM)&ti);
}

//-------------------------------------------------------------
void APIENTRY EnableToolTip(HWND hTip, BOOL activate)
{
    SendMessage(hTip, TTM_ACTIVATE, activate, 0);
}

BOOL APIENTRY DestroyToolTip(HWND hWnd)
{
	BOOL res = DestroyWindow(hWnd);
	return res;
}