// LookupHelperAXCtl.cpp : Implementation of the CLookupHelperAXCtrl ActiveX Control class.

#include "stdafx.h"
#include "LookupHelperAX.h"
#include "LookupHelperAXCtl.h"
#include "LookupHelperAXPpg.h"
#include "curvebox.h"
#include "math.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ScaleToRange(number)((number) = max(-100, min(100, (number))))

IMPLEMENT_DYNCREATE(CLookupHelperAXCtrl, COleControl)

BEGIN_INTERFACE_MAP(CLookupHelperAXCtrl, COleControl)
	INTERFACE_PART(CLookupHelperAXCtrl, IID_IVtActiveXCtrl, ActiveXCtrl)
	INTERFACE_PART(CLookupHelperAXCtrl, IID_IEventListener, EvList)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CLookupHelperAXCtrl, COleControl)
	//{{AFX_MSG_MAP(CLookupHelperAXCtrl)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_MESSAGE(WM_CURVECHANGE, OnCurveChange)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CLookupHelperAXCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CLookupHelperAXCtrl)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "Border", m_nBorder, OnBorderChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "ShowGrid", m_bShowGrid, OnShowGridChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "Mode", m_nMode, OnModeChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "CurveNum", m_nCurveNum, OnCurveNumChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "BrightBrightness", m_nBrightBrightness, OnBrightBrightnessChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "BrightGamma", m_nBrightGamma, OnBrightGammaChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "BrightContrast", m_nBrightContrast, OnBrightContrastChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "RedBrightness", m_nRedBrightness, OnRedBrightnessChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "GreenBrightness", m_nGreenBrightness, OnGreenBrightnessChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "BlueBrightness", m_nBlueBrightness, OnBlueBrightnessChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "RedGamma", m_nRedGamma, OnRedGammaChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "GreenGamma", m_nGreenGamma, OnGreenGammaChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "BlueGamma", m_nBlueGamma, OnBlueGammaChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "RedContrast", m_nRedContrast, OnRedContrastChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "GreenContrast", m_nGreenContrast, OnGreenContrastChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "BlueContrast", m_nBlueContrast, OnBlueContrastChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CLookupHelperAXCtrl, "ActionMode", m_bActionMode, OnActionModeChanged, VT_BOOL)
	DISP_FUNCTION(CLookupHelperAXCtrl, "ResetState", ResetState, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CLookupHelperAXCtrl, "Trace", Trace, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CLookupHelperAXCtrl, "Cancel", Cancel, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CLookupHelperAXCtrl, "Apply", Apply, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CLookupHelperAXCtrl, "LoadState", LoadState, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CLookupHelperAXCtrl, "SaveState", SaveState, VT_EMPTY, VTS_BSTR)
	DISP_STOCKPROP_BACKCOLOR()
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CLookupHelperAXCtrl, COleControl)
	//{{AFX_EVENT_MAP(CLookupHelperAXCtrl)
	EVENT_CUSTOM("UpdateExternalControls", FireUpdateExternalControls, VTS_NONE)
	EVENT_CUSTOM("OnAction", FireOnAction, VTS_NONE)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CLookupHelperAXCtrl, 2)
	PROPPAGEID(CLookupHelperAXPropPage::guid)
	PROPPAGEID(CLSID_CColorPropPage)
END_PROPPAGEIDS(CLookupHelperAXCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid
GUARD_IMPLEMENT_OLECREATE_CTRL(CLookupHelperAXCtrl, "LOOKUPHELPERAX.LookupHelperAXCtrl.1",
	0x3feac05c, 0x7050, 0x431e, 0xb1, 0x34, 0x26, 0xbb, 0xde, 0xb3, 0x2e, 0xda)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CLookupHelperAXCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs
const IID BASED_CODE IID_DLookupHelperAX =
		{ 0x5161efe8, 0xf62d, 0x4818, { 0x9b, 0x91, 0xe1, 0x76, 0xc0, 0x16, 0x4c, 0xe0 } };

const IID BASED_CODE IID_DLookupHelperAXEvents =
		{ 0x91e410aa, 0x7b21, 0x4fa1, { 0xb7, 0x37, 0x30, 0x26, 0x4, 0x99, 0xdb, 0x30 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwLookupHelperAXOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

	IMPLEMENT_OLECTLTYPE(CLookupHelperAXCtrl, IDS_LOOKUPHELPERAX, _dwLookupHelperAXOleMisc)

/////////////////////////////////////////////////////////////////////////////
// CHistAXCtrl IVtActiveXCtrl implement
IMPLEMENT_UNKNOWN(CLookupHelperAXCtrl, ActiveXCtrl);

HRESULT CLookupHelperAXCtrl::XActiveXCtrl::SetSite( IUnknown *punkVtApp, IUnknown *punkSite )
{
	METHOD_PROLOGUE_EX(CLookupHelperAXCtrl, ActiveXCtrl)

	pThis->m_sptrSite = punkSite;
	pThis->m_sptrApp = punkVtApp;	

	pThis->SetTarget();
	
	return S_OK;
}
  

HRESULT CLookupHelperAXCtrl::XActiveXCtrl::GetName( BSTR *pbstrName )
{
	METHOD_PROLOGUE_EX(CLookupHelperAXCtrl, ActiveXCtrl)	
	*pbstrName = NULL;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXCtrl::CLookupHelperAXCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CLookupHelperAXCtrl

BOOL CLookupHelperAXCtrl::CLookupHelperAXCtrlFactory::UpdateRegistry(BOOL bRegister)
{
#if defined(NOGUARD)
	if (bRegister)
	{
		return AfxOleRegisterControlClass(AfxGetInstanceHandle()
			, m_clsid, m_lpszProgID, IDS_LOOKUPHELPERAX, IDB_LOOKUPHELPERAX
			, afxRegInsertable | afxRegApartmentThreading, _dwLookupHelperAXOleMisc
			, _tlid, _wVerMajor, _wVerMinor);
	}
	else
	{
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
	}
#else
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.
	return UpdateRegistryCtrl(bRegister, AfxGetInstanceHandle(), IDS_LOOKUPHELPERAX, IDB_LOOKUPHELPERAX,
							  afxRegInsertable | afxRegApartmentThreading, _dwLookupHelperAXOleMisc,
							  _tlid, _wVerMajor, _wVerMinor);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXCtrl::CLookupHelperAXCtrl - Constructor

CLookupHelperAXCtrl::CLookupHelperAXCtrl()
{
	InitializeIIDs(&IID_DLookupHelperAX, &IID_DLookupHelperAXEvents);

	// TODO: Initialize your control's instance data here.

	m_sptrSite = 0;
	m_sptrApp = 0;
	m_pLookup = 0;
	m_nLookupSize = 0;
	m_bRegistered = false;
	m_nBorder = 5;
	m_pCurve = 0;
	m_pCurvePreview = 0;
	m_nMode = 0;
	m_bShowGrid = TRUE;
	m_sptrND = 0;
	m_nCurveNum = 1;

	m_nBrightBrightness = 0;
	m_nBrightGamma = 0;
	m_nBrightContrast = 0;
	m_nRedBrightness = 0;
	m_nGreenBrightness = 0;
	m_nBlueBrightness = 0;
	m_nRedGamma = 0;
	m_nGreenGamma = 0;
	m_nBlueGamma = 0;
	m_nRedContrast = 0;
	m_nGreenContrast = 0;
	m_nBlueContrast = 0;

	m_bCancel = false;
	m_pbyteLookupBackUp = 0;
	m_nSizeBackUp = 0;

	m_pbyteLookupStateBackUp = 0;

	m_bActionMode = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXCtrl::~CLookupHelperAXCtrl - Destructor

CLookupHelperAXCtrl::~CLookupHelperAXCtrl()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXCtrl::OnDraw - Drawing function

void CLookupHelperAXCtrl::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	// TODO: Replace the following code with your own drawing code.
	CBrush bkBrush(TranslateColor(GetBackColor()));
	pdc->FillRect(rcBounds, &bkBrush);
	//CRect rcHist(rcBounds);
	//rcHist.DeflateRect(m_nBorder, m_nBorder);

	//CBrush* pOldBrush = pdc->SelectObject(&bkBrush);

	//pdc->Rectangle(rcBounds);
	//pdc->Rectangle(rcHist);
	
	//DrawCurve(pdc, rcHist);

	//pdc->SelectObject(pOldBrush);
}


/////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXCtrl::DoPropExchange - Persistence support

void CLookupHelperAXCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	//COleControl::DoPropExchange(pPX);
	//ExchangeStockProps(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
	PX_Short(pPX, "Border", m_nBorder);
	PX_Short(pPX, "Mode", m_nMode);
	PX_Bool(pPX, "ShowGrid", m_bShowGrid);
	PX_Short(pPX, "CurveNum", m_nCurveNum);

	PX_Short(pPX, "BrightBrightness", m_nBrightBrightness);
	PX_Short(pPX, "BrightGamma", m_nBrightGamma);
	PX_Short(pPX, "BrightContrast", m_nBrightContrast);
	PX_Short(pPX, "RedBrightness", m_nRedBrightness);
	PX_Short(pPX, "GreenBrightness", m_nGreenBrightness);
	PX_Short(pPX, "BlueBrightness", m_nBlueBrightness);
	PX_Short(pPX, "RedGamma", m_nRedGamma);
	PX_Short(pPX, "GreenGamma", m_nGreenGamma);
	PX_Short(pPX, "BlueGamma", m_nBlueGamma);
	PX_Short(pPX, "RedContrast", m_nRedContrast);
	PX_Short(pPX, "GreenContrast", m_nGreenContrast);
	PX_Short(pPX, "BlueContrast", m_nBlueContrast);

	
		 
	if(pPX->IsLoading())
	{
		if(m_pCurve)
		{
			m_pCurve->SetCurveNum(m_nCurveNum);
			m_pCurve->SetMode(m_nMode);
			m_pCurve->ShowGrid(m_bShowGrid==TRUE);
		}
	}

	if( _wVerMinor >= 1  )
		PX_Bool(pPX, "ActionMode", m_bActionMode);

	
}

void CLookupHelperAXCtrl::Serialize(CArchive& ar)
{
	SerializeStockProps(ar);

	COleControl::Serialize( ar );
}

/////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXCtrl::OnResetState - Reset control to default state

void CLookupHelperAXCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
	SetBackColor(GetSysColor(COLOR_3DSHADOW));
	m_nBorder = 5;
}


/////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXCtrl message handlers

void CLookupHelperAXCtrl::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( (!strcmp( pszEvent, szAppActivateView )) || (!strcmp( pszEvent, szEventActivateObject )))
	{
		SetTarget(punkFrom);
	}
}

void CLookupHelperAXCtrl::SetTarget(IUnknown* punkActiveView)
{
	IImageViewPtr sptrV(punkActiveView);


	if(sptrV)
	{
		IUnknown* punkImg = 0;
		sptrV->GetActiveImage(&punkImg, 0);
		INamedDataPtr	sptrND(punkImg);
		if(punkImg)
			punkImg->Release();
		if(::GetObjectKey(m_sptrND) == ::GetObjectKey(sptrND))
			return;
	}
				

	m_sptrND = 0;

	if(m_sptrApp == 0) return;

	
	if(!punkActiveView)
	{
		IUnknown* punkDoc = 0;
		m_sptrApp->GetActiveDocument(&punkDoc);
		IDocumentSitePtr sptrDoc(punkDoc);
		if(punkDoc)
			punkDoc->Release();
		if(sptrDoc != 0)
		{
			IUnknown* punkView = 0;
			sptrDoc->GetActiveView(&punkView);
			sptrV = punkView;
			if(punkView)
				punkView->Release();
		}
	}
		
	
	if(sptrV == 0)
		return;

	IUnknown* punkImg = 0;
	sptrV->GetActiveImage(&punkImg, 0);
	if(punkImg)
	{
		if(!CheckInterface(punkImg, IID_INamedData))
		{
			INamedDataObject2Ptr sptrNDO(punkImg);
			if(sptrNDO != 0)
				sptrNDO->InitAttachedData();
		}
		m_sptrND = punkImg;
		punkImg->Release();
	}
	else
		return;
	if(m_sptrND != 0)
	{
		long nSize = 0;
		if(m_pbyteLookupBackUp)
		{
			delete m_pbyteLookupBackUp;
			m_nSizeBackUp = 0;
			m_pbyteLookupBackUp = 0;
		}

		byte* pbyteLookup = ::GetValuePtr(m_sptrND, (m_bActionMode?szLookupAction:szLookup), szLookupTable, &nSize);

		if(!pbyteLookup)
		{
			_SetDefaultLookupTable(m_sptrND);
			pbyteLookup = ::GetValuePtr(m_sptrND, (m_bActionMode?szLookupAction:szLookup), szLookupTable, &nSize);
		}
				
		if(nSize != 0)
		{
			m_nSizeBackUp = nSize;
			m_pbyteLookupBackUp = new byte[nSize];
			memcpy(m_pbyteLookupBackUp, pbyteLookup, nSize);
		}

		m_pCurvePreview->SetCurveBCG(0, pbyteLookup+512, pbyteLookup+256, pbyteLookup);

		if(pbyteLookup)
		{
			delete pbyteLookup;
			/*if(m_pLookup != 0)
			{
				delete m_pLookup;
				m_pLookup = 0;
			}
			m_nLookupSize = nSize;
			m_pLookup = pbyteLookup;
			*/
		}

		byte* ptr = ::GetValuePtr(m_sptrND, (m_bActionMode?szLookupAction:szLookup), szLookupState, &nSize);
		if (ptr)
		{
			ASSERT(nSize == g_nStateSize);
			m_pCurve->SetState(ptr);

			m_nBrightBrightness = ptr[g_nStateSize-1] - 100;
			m_nBrightGamma = ptr[g_nStateSize-2] - 100;
			m_nBrightContrast = ptr[g_nStateSize-3] - 100;
			m_nRedBrightness = ptr[g_nStateSize-4] - 100;
			m_nGreenBrightness = ptr[g_nStateSize-5] - 100;
			m_nBlueBrightness = ptr[g_nStateSize-6] - 100;
			m_nRedGamma = ptr[g_nStateSize-7] - 100;
			m_nGreenGamma = ptr[g_nStateSize-8] - 100;
			m_nBlueGamma = ptr[g_nStateSize-9] - 100;
			m_nRedContrast = ptr[g_nStateSize-10] - 100;
			m_nGreenContrast = ptr[g_nStateSize-11] - 100;
			m_nBlueContrast = ptr[g_nStateSize-12] - 100;
		}
		
		FireUpdateExternalControls();

		delete ptr;
	}
}

void CLookupHelperAXCtrl::OnBorderChanged() 
{
	// TODO: Add notification handler code
	InvalidateControl();
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::_SetDefaultLookupTable(IUnknown* punkDoc)
{
	byte *ptr = 0;
	m_pCurve->InitData();
	m_pCurvePreview->InitData();

	m_nBrightBrightness = 0;
	m_nBrightGamma = 0;
	m_nBrightContrast = 0;
	m_nRedBrightness = 0;
	m_nGreenBrightness = 0;
	m_nBlueBrightness = 0;
	m_nRedGamma = 0;
	m_nGreenGamma = 0;
	m_nBlueGamma = 0;
	m_nRedContrast = 0;
	m_nGreenContrast = 0;
	m_nBlueContrast = 0;

	FireUpdateExternalControls();

	if(punkDoc)
	{

		long lSize = 256*3;
		ptr = new byte[lSize];
		for(long i = 0; i < lSize/3; i++)
		{
			ptr[i] = i;
			ptr[i+256] = i;
			ptr[i+512] = i;
		}
		::SetValue(punkDoc, (m_bActionMode?szLookupAction:szLookup), szLookupTable, ptr,  lSize);
		delete ptr;
	
		//set lookup state
		ASSERT(m_pCurve != 0);
		ptr = new byte[g_nStateSize];
		m_pCurve->GetState(ptr);
		ptr[g_nStateSize-1] = m_nBrightBrightness+100;
		ptr[g_nStateSize-2] = m_nBrightGamma+100;
		ptr[g_nStateSize-3] = m_nBrightContrast+100;
		ptr[g_nStateSize-4] = m_nRedBrightness+100;
		ptr[g_nStateSize-5] = m_nGreenBrightness+100;
		ptr[g_nStateSize-6] = m_nBlueBrightness+100;
		ptr[g_nStateSize-7] = m_nRedGamma+100;
		ptr[g_nStateSize-8] = m_nGreenGamma+100;
		ptr[g_nStateSize-9] = m_nBlueGamma+100;
		ptr[g_nStateSize-10] = m_nRedContrast+100;
		ptr[g_nStateSize-11] = m_nGreenContrast+100;
		ptr[g_nStateSize-12] = m_nBlueContrast+100;
		::SetValue(punkDoc, (m_bActionMode?szLookupAction:szLookup), szLookupState, ptr,  g_nStateSize);

		delete ptr;

		if(m_bActionMode)
			FireOnAction();
	}
}

void CLookupHelperAXCtrl::OnFinalRelease() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(!m_bCancel)
	{
		Apply();
	}

	if(m_bCancel && m_sptrND != 0)
	{
		if(m_pbyteLookupStateBackUp)
			::SetValue(m_sptrND, (m_bActionMode?szLookupAction:szLookup), szLookupState, m_pbyteLookupStateBackUp,  g_nStateSize);

		::SetValue(m_sptrND, (m_bActionMode?szLookupAction:szLookup), szLookupTable, m_pbyteLookupBackUp, m_nSizeBackUp);

		if(m_bActionMode == TRUE)
			FireOnAction();
		else
			_InvalidateActiveView();

	}

	if(m_pbyteLookupStateBackUp)
	{
		delete m_pbyteLookupStateBackUp;
		m_pbyteLookupStateBackUp = 0;
	}

	if(m_pbyteLookupBackUp)
	{
		delete m_pbyteLookupBackUp;
		m_nSizeBackUp = 0;
		m_pbyteLookupBackUp = 0;	
	}
	

	if(m_bRegistered)
	{
		UnRegister();
		m_bRegistered = false;
	}
	if(m_pLookup != 0)
	{
		delete m_pLookup;
		m_pLookup = 0;
	}
		
	AddRef();
	if(m_pCurve)
	{
		if(::IsWindow(m_pCurve->m_hWnd))
			m_pCurve->DestroyWindow();
		delete m_pCurve;
		m_pCurve = 0;
	}
	AddRef();
	if(m_pCurvePreview)
	{
		if(::IsWindow(m_pCurvePreview->m_hWnd))
			m_pCurvePreview->DestroyWindow();
		delete m_pCurvePreview;
		m_pCurvePreview = 0;
	}
	COleControl::OnFinalRelease();
}

int CLookupHelperAXCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	Register();
	m_bRegistered = true;
	m_pCurve = new CCurveBox;
	m_pCurvePreview = new CCurveBox;

	m_pCurve->SetBackColor(TranslateColor(GetBackColor()));
	m_pCurvePreview->SetBackColor(TranslateColor(GetBackColor()));
	
	CRect rc = NORECT;
	GetClientRect(&rc);
	rc.DeflateRect(m_nBorder, m_nBorder);
	CRect rcCurve(rc);
	CRect rcCurvePreview(rc);
	rcCurve.right = rcCurve.left + rcCurve.Width()/2 - m_nBorder/2;
	rcCurvePreview.left = rcCurve.right + m_nBorder;
	m_pCurve->Create("", WS_CHILD|WS_VISIBLE|SS_BLACKRECT|SS_LEFT, rcCurve, this);
	m_pCurvePreview->Create("", WS_CHILD|WS_VISIBLE|SS_BLACKRECT|SS_LEFT, rcCurvePreview, this);

	m_pCurvePreview->SetBoxType(false);
	m_pCurvePreview->ShowGrid(false);
	
	//m_pCurve->SetMode(m_nMode);
	//m_pCurve->ShowGrid(m_bShowGrid==TRUE);
	return 0;
}

void CLookupHelperAXCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	COleControl::OnLButtonDown(nFlags, point);

	// TODO: Add your message handler code here and/or call default
	if(m_sptrND != 0)
	{
		CRect rcWnd;
		m_pCurve->GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);
		if (rcWnd.PtInRect(point))
		{
			CPoint pt(point);
			pt -= rcWnd.TopLeft();
			m_pCurve->OnLButtonDown(nFlags, pt);
		}
	}
}

void CLookupHelperAXCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_sptrND != 0)
	{
		CRect rcWnd;
		m_pCurve->GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);
		if (rcWnd.PtInRect(point))
		{
			CPoint pt(point);
			pt -= rcWnd.TopLeft();
			m_pCurve->OnLButtonUp(nFlags, pt);
		}	
	}
	COleControl::OnLButtonUp(nFlags, point);
}

void CLookupHelperAXCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_sptrND != 0)
	{
		CRect rcWnd;
		m_pCurve->GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);
		if (rcWnd.PtInRect(point))
		{
			CPoint pt(point);
			pt -= rcWnd.TopLeft();
			m_pCurve->OnMouseMove(nFlags, pt);
		}	
	}
	COleControl::OnMouseMove(nFlags, point);
}

void CLookupHelperAXCtrl::OnShowGridChanged() 
{
	// TODO: Add notification handler code
	if(m_pCurve)
		m_pCurve->ShowGrid(m_bShowGrid==TRUE);
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::OnModeChanged() 
{
	// TODO: Add notification handler code
	if(m_pCurve)
		m_pCurve->SetMode(m_nMode);
	SetModifiedFlag();
}

LRESULT CLookupHelperAXCtrl::OnCurveChange(WPARAM, LPARAM)
{
	int i,c;
	if(m_sptrND != 0)
	{
		//get lookup data
		byte* pCurveData = new byte[256*3];
		for(i = 0; i < 3; i++)
		{
			byte* pData = pCurveData + 256*i;
			m_pCurve->GetCurveData(i+1, pData);
		}

		byte* pBright = new byte[256];
		m_pCurve->GetCurveData(0, pBright);
		byte* pR = pCurveData;
		byte* pG = pCurveData+256;
		byte* pB = pCurveData+512;
		
		double fMul0,fAdd0,fMulB,fAddB,fMulG,fAddG,fMulR,fAddR;
		double fGamma0,fCnvMul0,fGammaB,fCnvMulB,fGammaG,fCnvMulG,fGammaR,fCnvMulR;
		BYTE baOrigBright[256];
		BYTE baOrigBlue[256];
		BYTE baOrigGreen[256];
		BYTE baOrigRed[256];
		// Get original data - for non-linear curves
		m_pCurve->GetCurveData(0, baOrigBright);
		m_pCurve->GetCurveData(1, baOrigRed);
		m_pCurve->GetCurveData(2, baOrigGreen);
		m_pCurve->GetCurveData(3, baOrigBlue);

		// Calc some useful variables for color planes
		// gray
		fMul0 = tan( ( ((double)m_nBrightContrast)+100. )/400.*PI );
		fAdd0 = m_nBrightBrightness+128.;
		fGamma0 = pow(10., m_nBrightGamma/100.);
		fCnvMul0 = 255/pow(255., 1./fGamma0);
		// blue
		fMulB = tan( ( ((double)m_nBlueContrast)+100. )/400.*PI );
		fAddB = m_nBlueBrightness+128.;
		fGammaB = pow(10., m_nBlueGamma/100.);
		fCnvMulB = 255/pow(255., 1./fGammaB);
		// green
		fMulG = tan( ( ((double)m_nGreenContrast)+100. )/400.*PI );
		fAddG = m_nGreenBrightness+128.;
		fGammaG = pow(10., m_nGreenGamma/100.);
		fCnvMulG = 255/pow(255., 1./fGammaG);
		// red
		fMulR = tan( ( ((double)m_nRedContrast)+100. )/400.*PI );
		fAddR = m_nRedBrightness+128.;
		fGammaR = pow(10., m_nRedGamma/100.);
		fCnvMulR = 255/pow(255., 1./fGammaR);
		// Init color planes
		BYTE bright = 0;
		for (i = 0; i < 256; i++)
		{
			// Gray value
			c = ((int)((i-128)*fMul0))+(int)fAdd0;
			c = min(max(0,c),255);
			if (m_nBrightGamma != 0)
				c = (BYTE)(fCnvMul0 * pow((double)c, 1./fGamma0));
			bright = baOrigBright[c];
			pBright[i] = bright;
			// Blue value
			c = ((int)((i-128)*fMulB))+(int)fAddB;
			c = min(max(0,c),255);
			if (m_nBlueGamma != 0)
				c = (BYTE)(fCnvMulB * pow((double)c, 1./fGammaB));
			c = baOrigBlue[c];
			pB[i] = max(0, min(255, (BYTE)c + bright - i));
			// Green value
			c = ((int)((i-128)*fMulG))+(int)fAddG;
			c = min(max(0,c),255);
			if (m_nGreenGamma != 0)
				c = (BYTE)(fCnvMulG * pow((double)c, 1./fGammaG));
			c = baOrigGreen[c];
			pG[i] = max(0, min(255, (BYTE)c + bright - i));
			// Red value
			c = ((int)((i-128)*fMulR))+(int)fAddR;
			c = min(max(0,c),255);
			if (m_nRedGamma != 0)
				c = (BYTE)(fCnvMulR * pow((double)c, 1./fGammaR));
			c = baOrigRed[c];
			pR[i] = max(0, min(255, (BYTE)c + bright - i));
		}

		delete pBright;

		::SetValue(m_sptrND, (m_bActionMode?szLookupAction:szLookup), szLookupTable, pCurveData, 256*3);

		m_pCurvePreview->SetCurveBCG(0, pCurveData+512, pCurveData+256, pCurveData);

		delete pCurveData;
		
		/*IImagePtr sptrI(m_sptrND);
		IUnknown *punkCC = 0;
		if(sptrI != 0)
			sptrI->GetColorConvertor(&punkCC);
		IColorConvertorLookupPtr	sptrCCL(punkCC);
		if(punkCC)
			punkCC->Release();
		if(sptrCCL != 0)
		{
			int cx = 256;
			int cy = 1; 

			sptrCCL->SetLookupTable(pCurveData, cx, cy);
		}
		*/
		if(m_bActionMode == TRUE)
			FireOnAction();
		else
			_InvalidateActiveView();
			
	}
	return 0;
}

void CLookupHelperAXCtrl::OnCurveNumChanged() 
{
	// TODO: Add notification handler code
	if(m_pCurve)
		m_pCurve->SetCurveNum(m_nCurveNum);
	if(m_pCurvePreview)
		m_pCurvePreview->SetCurveNum(m_nCurveNum);
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect rc = NORECT;
	GetClientRect(&rc);
	{
		char	sz[255];
		sprintf( sz, "m_nBorder = %d, %d %d %d %d\n", m_nBorder, rc.left, rc.top, rc.right, rc.bottom );
		OutputDebugString( sz );
	}
	rc.DeflateRect(m_nBorder, m_nBorder);
	CRect rcCurve(rc);
	CRect rcCurvePreview(rc);
	rcCurve.right = rcCurve.left + rcCurve.Width()/2 - m_nBorder/2;
	rcCurvePreview.left = rcCurve.right + m_nBorder;
	m_pCurve->MoveWindow(&rcCurve);
	m_pCurvePreview->MoveWindow(&rcCurvePreview);

}

void CLookupHelperAXCtrl::_InvalidateActiveView()
{
	IUnknown* punkDoc = 0;
	m_sptrApp->GetActiveDocument(&punkDoc);
	IDocumentSitePtr sptrDoc(punkDoc);
	IWindowPtr sptrW;
	if(punkDoc)
		punkDoc->Release();
	if(sptrDoc != 0)
	{
		IUnknown* punkView = 0;
		sptrDoc->GetActiveView(&punkView);
		sptrW = punkView;
		if(punkView)
			punkView->Release();
	}

	if(sptrW != 0)
	{
		HWND	hwnd = 0;
		sptrW->GetHandle((HANDLE*)&hwnd);
		if(hwnd)
		{
			CWnd::FromHandle(hwnd)->Invalidate();
		}
	}
}

void CLookupHelperAXCtrl::ResetState() 
{
	//backup state
	long nSize = 0;
	if(m_pbyteLookupStateBackUp)
		delete m_pbyteLookupStateBackUp;
	m_pbyteLookupStateBackUp = ::GetValuePtr(m_sptrND, (m_bActionMode?szLookupAction:szLookup), szLookupState, &nSize);
	_SetDefaultLookupTable(m_sptrND);
	m_pCurve->Invalidate();
	m_pCurvePreview->Invalidate();
	_InvalidateActiveView();
}

void CLookupHelperAXCtrl::OnBrightBrightnessChanged() 
{
	// TODO: Add notification handler code
	ScaleToRange(m_nBrightBrightness);
	SendMessage(WM_CURVECHANGE);
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::OnBrightGammaChanged() 
{
	// TODO: Add notification handler code
	ScaleToRange(m_nBrightGamma);
	SendMessage(WM_CURVECHANGE);
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::OnBrightContrastChanged() 
{
	// TODO: Add notification handler code
	ScaleToRange(m_nBrightContrast);
	m_nBrightContrast = min(99, m_nBrightContrast);
	SendMessage(WM_CURVECHANGE);
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::OnRedBrightnessChanged() 
{
	// TODO: Add notification handler code
	ScaleToRange(m_nRedBrightness);
	SendMessage(WM_CURVECHANGE);
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::OnGreenBrightnessChanged() 
{
	// TODO: Add notification handler code
	ScaleToRange(m_nGreenBrightness);
	SendMessage(WM_CURVECHANGE);
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::OnBlueBrightnessChanged() 
{
	// TODO: Add notification handler code
	ScaleToRange(m_nBlueBrightness);
	SendMessage(WM_CURVECHANGE);
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::OnRedGammaChanged() 
{
	// TODO: Add notification handler code
	ScaleToRange(m_nRedGamma);
	SendMessage(WM_CURVECHANGE);
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::OnGreenGammaChanged() 
{
	// TODO: Add notification handler code
	ScaleToRange(m_nGreenGamma);
	SendMessage(WM_CURVECHANGE);
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::OnBlueGammaChanged() 
{
	// TODO: Add notification handler code
	ScaleToRange(m_nBlueGamma);
	SendMessage(WM_CURVECHANGE);
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::OnRedContrastChanged() 
{
	// TODO: Add notification handler code
	ScaleToRange(m_nRedContrast);
	m_nRedContrast = min(99, m_nRedContrast);
	SendMessage(WM_CURVECHANGE);
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::OnGreenContrastChanged() 
{
	// TODO: Add notification handler code
	ScaleToRange(m_nGreenContrast);
	m_nGreenContrast = min(99, m_nGreenContrast);
	SendMessage(WM_CURVECHANGE);
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::OnBlueContrastChanged() 
{
	// TODO: Add notification handler code
	ScaleToRange(m_nBlueContrast);
	m_nBlueContrast = min(99, m_nBlueContrast);
	SendMessage(WM_CURVECHANGE);
	SetModifiedFlag();
}

void CLookupHelperAXCtrl::Trace(LPCTSTR szOutString) 
{
	// TODO: Add your dispatch handler code here
	TRACE("************************************************* %s\n", szOutString);
}

BOOL CLookupHelperAXCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
}

void CLookupHelperAXCtrl::Cancel() 
{
	// TODO: Add your dispatch handler code here
	m_bCancel = true;
}

void CLookupHelperAXCtrl::Apply() 
{
	// TODO: Add your dispatch handler code here

	if(m_pbyteLookupStateBackUp)
	{
		delete m_pbyteLookupStateBackUp;
		m_pbyteLookupStateBackUp = 0;
	}
	if(m_sptrND != 0)
	{
		long nSize = 0;
		if(m_pbyteLookupBackUp)
		{
			delete m_pbyteLookupBackUp;
			m_nSizeBackUp = 0;
			m_pbyteLookupBackUp = 0;
		}

		byte* pbyteLookup = ::GetValuePtr(m_sptrND, (m_bActionMode?szLookupAction:szLookup), szLookupTable, &nSize);

		if(nSize != 0)
		{
			m_nSizeBackUp = nSize;
			m_pbyteLookupBackUp = new byte[nSize];
			memcpy(m_pbyteLookupBackUp, pbyteLookup, nSize);
		}
		
		if(pbyteLookup)
			delete pbyteLookup;

		byte* ptr = new byte[g_nStateSize];
		m_pCurve->GetState(ptr);

		ptr[g_nStateSize-1] = m_nBrightBrightness+100;
		ptr[g_nStateSize-2] = m_nBrightGamma+100;
		ptr[g_nStateSize-3] = m_nBrightContrast+100;
		ptr[g_nStateSize-4] = m_nRedBrightness+100;
		ptr[g_nStateSize-5] = m_nGreenBrightness+100;
		ptr[g_nStateSize-6] = m_nBlueBrightness+100;
		ptr[g_nStateSize-7] = m_nRedGamma+100;
		ptr[g_nStateSize-8] = m_nGreenGamma+100;
		ptr[g_nStateSize-9] = m_nBlueGamma+100;
		ptr[g_nStateSize-10] = m_nRedContrast+100;
		ptr[g_nStateSize-11] = m_nGreenContrast+100;
		ptr[g_nStateSize-12] = m_nBlueContrast+100;
		
		::SetValue(m_sptrND, (m_bActionMode?szLookupAction:szLookup), szLookupState, ptr,  g_nStateSize);
		delete ptr;
	}
}

void CLookupHelperAXCtrl::OnActionModeChanged() 
{
	// TODO: Add notification handler code

	SetModifiedFlag();
}

void CLookupHelperAXCtrl::LoadState(LPCTSTR szSection) 
{
	long	nSize = 0;
	byte* ptr = ::GetValuePtr(GetAppUnknown(), szSection, szLookupState, &nSize);
	if( !ptr )
	{
		//AfxMessageBox( "Invalid entry specified" );
		return;
	}
	ASSERT(nSize == g_nStateSize);
	m_pCurve->SetState(ptr);
 
	m_nBrightBrightness = ptr[g_nStateSize-1] - 100;
	m_nBrightGamma = ptr[g_nStateSize-2] - 100;
	m_nBrightContrast = ptr[g_nStateSize-3] - 100;
	m_nRedBrightness = ptr[g_nStateSize-4] - 100;
	m_nGreenBrightness = ptr[g_nStateSize-5] - 100;
	m_nBlueBrightness = ptr[g_nStateSize-6] - 100;
	m_nRedGamma = ptr[g_nStateSize-7] - 100;
	m_nGreenGamma = ptr[g_nStateSize-8] - 100;
	m_nBlueGamma = ptr[g_nStateSize-9] - 100;
	m_nRedContrast = ptr[g_nStateSize-10] - 100;
	m_nGreenContrast = ptr[g_nStateSize-11] - 100;
	m_nBlueContrast = ptr[g_nStateSize-12] - 100;

	SendMessage(WM_CURVECHANGE);
	FireUpdateExternalControls();

	delete ptr;
}

void CLookupHelperAXCtrl::SaveState(LPCTSTR szSection) 
{
	byte* ptr = new byte[g_nStateSize];
	m_pCurve->GetState(ptr);

	ptr[g_nStateSize-1] = m_nBrightBrightness+100;
	ptr[g_nStateSize-2] = m_nBrightGamma+100;
	ptr[g_nStateSize-3] = m_nBrightContrast+100;
	ptr[g_nStateSize-4] = m_nRedBrightness+100;
	ptr[g_nStateSize-5] = m_nGreenBrightness+100;
	ptr[g_nStateSize-6] = m_nBlueBrightness+100;
	ptr[g_nStateSize-7] = m_nRedGamma+100;
	ptr[g_nStateSize-8] = m_nGreenGamma+100;
	ptr[g_nStateSize-9] = m_nBlueGamma+100;
	ptr[g_nStateSize-10] = m_nRedContrast+100;
	ptr[g_nStateSize-11] = m_nGreenContrast+100;
	ptr[g_nStateSize-12] = m_nBlueContrast+100;
	
	::SetValue(GetAppUnknown(), szSection, szLookupState, ptr,  g_nStateSize);
	delete ptr;
}
