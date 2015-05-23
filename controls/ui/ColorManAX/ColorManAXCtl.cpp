// ColorManAXCtl.cpp : Implementation of the CColorManAXCtrl ActiveX Control class.

#include "stdafx.h"
#include "ColorManAX.h"
#include "ColorManAXCtl.h"
#include "ColorManAXPpg.h"
#include "FramesPropPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define HistColorBound	65536
#define InitHiLoValue	HistColorBound/2

#define IDcombo		478

#define IDedit1		479
#define IDedit2		480

#define IDspin1		481
#define IDspin2		482

#define IDradio1	483
#define IDradio2	484
#define IDradio3	485
#define IDstatic	486



#define nIconX		15 
#define nIconY		15

#define nHorzInterval 10
#define nVertInterval 20



IMPLEMENT_DYNCREATE(CColorManAXCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CColorManAXCtrl, COleControl)
	//{{AFX_MSG_MAP(CColorManAXCtrl)
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_CBN_CLOSEUP(IDcombo, OnCloseCB)
	ON_CONTROL_RANGE(BN_CLICKED, IDradio1, IDradio3, OnRadioChange)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CColorManAXCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CColorManAXCtrl)
	DISP_PROPERTY_NOTIFY(CColorManAXCtrl, "InnerSunken", m_innerSunken, OnInnerSunkenChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CColorManAXCtrl, "OuterRaised", m_outerRaised, OnOuterRaisedChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CColorManAXCtrl, "OuterSunken", m_outerSunken, OnOuterSunkenChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CColorManAXCtrl, "InnerRaised", m_innerRaised, OnInnerRaisedChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CColorManAXCtrl, "HiRange", m_hiRange, OnHiRangeChanged, VT_I4)
	DISP_PROPERTY_NOTIFY(CColorManAXCtrl, "LoRange", m_loRange, OnLoRangeChanged, VT_I4)
	DISP_PROPERTY_NOTIFY(CColorManAXCtrl, "AddInterval", m_addInterval, OnAddIntervalChanged, VT_BOOL)
	DISP_FUNCTION(CColorManAXCtrl, "IsPaneCycled", IsPaneCycled, VT_BOOL, VTS_I2)
	DISP_FUNCTION(CColorManAXCtrl, "ResetIntervalsSet", ResetIntervalsSet, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CColorManAXCtrl, "UndoChangeIntervals", UndoChangeIntervals, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CColorManAXCtrl, "IsUndoAvail", IsUndoAvail, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CColorManAXCtrl, "AddIntervalsSet", AddIntervalsSet, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CColorManAXCtrl, "RemoveIntervalsSet", RemoveIntervalsSet, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CColorManAXCtrl, "SetActiveIntervalsSet", SetActiveIntervalsSet, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CColorManAXCtrl, "GetLoRangeOfPane", GetLoRangeOfPane, VT_I4, VTS_I2 VTS_I2)
	DISP_FUNCTION(CColorManAXCtrl, "GetHiRangeOfPane", GetHiRangeOfPane, VT_I4, VTS_I2 VTS_I2)
	DISP_FUNCTION(CColorManAXCtrl, "GetPanesCount", GetPanesCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CColorManAXCtrl, "Fake", Fake, VT_EMPTY, VTS_BSTR)
	DISP_STOCKPROP_FONT()
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


BEGIN_INTERFACE_MAP(CColorManAXCtrl, COleControl)
	INTERFACE_PART(CColorManAXCtrl, IID_IVtActiveXCtrl, ActiveXCtrl)
	INTERFACE_PART(CColorManAXCtrl, IID_IEventListener, EvList)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CColorManAXCtrl, COleControl)
	//{{AFX_EVENT_MAP(CColorManAXCtrl)
	EVENT_CUSTOM("OnPaneChange", FireOnPaneChange, VTS_I2)
	EVENT_CUSTOM("OnChangeHiRange", FireOnChangeHiRange, VTS_I4)
	EVENT_CUSTOM("OnChangeLoRange", FireOnChangeLoRange, VTS_I4)
	EVENT_CUSTOM("OnManageColorInterval", FireOnManageColorInterval, VTS_NONE)
	EVENT_CUSTOM("OnChangeUndoState", FireOnChangeUndoState, VTS_NONE)
	EVENT_CUSTOM("OnColorModelChange", FireOnColorModelChange, VTS_NONE)
	EVENT_CUSTOM("OnChangeWorkingImage", FireOnChangeWorkingImage, VTS_DISPATCH)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CColorManAXCtrl, 2)
	PROPPAGEID(CColorManAXPropPage::guid)
	PROPPAGEID(CFramesPropPage::guid)
	//PROPPAGEID(CLSID_CFontPropPage)	
END_PROPPAGEIDS(CColorManAXCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid
// {3871368D-87AC-4f8e-AE3D-2FC2A3DB60A8}
GUARD_IMPLEMENT_OLECREATE_CTRL(CColorManAXCtrl, "COLORMANAX.ColorManAXCtrl.1",
	0x3871368d, 0x87ac, 0x4f8e, 0xae, 0x3d, 0x2f, 0xc2, 0xa3, 0xdb, 0x60, 0xa8);



/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CColorManAXCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DColorManAX =
		{ 0x5db73bb4, 0xf590, 0x11d3, { 0xa0, 0xbc, 0, 0, 0xb4, 0x93, 0xa1, 0x87 } };
const IID BASED_CODE IID_DColorManAXEvents =
		{ 0x5db73bb5, 0xf590, 0x11d3, { 0xa0, 0xbc, 0, 0, 0xb4, 0x93, 0xa1, 0x87 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwColorManAXOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

	IMPLEMENT_OLECTLTYPE(CColorManAXCtrl, IDS_COLORMANAX, _dwColorManAXOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CColorManAXCtrl::CColorManAXCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CColorManAXCtrl

BOOL CColorManAXCtrl::CColorManAXCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.
	return UpdateRegistryCtrl(bRegister, AfxGetInstanceHandle(), IDS_COLORMANAX, IDB_COLORMANAX,
							  afxRegApartmentThreading, _dwColorManAXOleMisc,
							  _tlid, _wVerMajor, _wVerMinor);

}


/////////////////////////////////////////////////////////////////////////////
// CColorManAXCtrl::CColorManAXCtrl - Constructor

CColorManAXCtrl::CColorManAXCtrl()
{
	InitializeIIDs(&IID_DColorManAX, &IID_DColorManAXEvents);

	// TODO: Initialize your control's instance data here.
	m_sptrSite = 0;
	m_sptrApp = GetAppUnknown();
	m_pcbColorModel = 0;

	m_innerRaised = TRUE;
	m_innerSunken = FALSE;
	m_outerRaised = FALSE;
	m_outerSunken = TRUE;

	m_pEdit1 = 0;
	m_pEdit2 = 0;

	m_pSpin1 = 0;
	m_pSpin2 = 0;

	m_pRadio1 = 0;
	m_pRadio2 = 0;
	m_pRadio3 = 0;

	m_pStaticWorkImage = 0;

	m_nLastCBSel = 0;
	m_nLastRadioSel = 0;

	m_hiRange = InitHiLoValue;
	m_loRange = InitHiLoValue;
	m_nCurIntervalsSet = -1;
	m_sptrWorkingImage = 0;
	m_nInfiniteImageIdx = -1;
}


/////////////////////////////////////////////////////////////////////////////
// CColorManAXCtrl::~CColorManAXCtrl - Destructor

CColorManAXCtrl::~CColorManAXCtrl()
{
	// TODO: Cleanup your control's instance data here.
	if(m_pcbColorModel)
	{
		delete m_pcbColorModel;
		m_pcbColorModel = 0;
	}
	if(m_pEdit1)
	{
		delete m_pEdit1;
		m_pEdit1 = 0;
	}
	if(m_pEdit2)
	{
		delete m_pEdit2;
		m_pEdit2 = 0;
	}
	if(m_pSpin1)
	{
		delete m_pSpin1;
		m_pSpin1 = 0;
	}
	if(m_pSpin2)
	{
		delete m_pSpin2;
		m_pSpin2 = 0;
	}
	if(m_pRadio1)
	{
		delete m_pRadio1;
		m_pRadio1 = 0;
	}
	if(m_pRadio2)
	{
		delete m_pRadio2;
		m_pRadio2 = 0;
	}
	if(m_pRadio3)
	{
		delete m_pRadio3;
		m_pRadio3 = 0;
	}
	if(m_pStaticWorkImage)
	{
		delete m_pStaticWorkImage;
		m_pStaticWorkImage = 0;
	}
	

	m_imgCC.DeleteImageList();	

	/*
	POSITION pos = m_listCCIntervalsUndo.GetHeadPosition();
	IntervalUndo* pIntervalUndo;
	while(pos)
	{
		pIntervalUndo = m_listCCIntervalsUndo.GetNext(pos);
		if(pIntervalUndo)
		{
			delete pIntervalUndo->pnLoRanges;
			delete pIntervalUndo->pnHiRanges;
			delete pIntervalUndo;
			pIntervalUndo = 0;
		}
	}
	
	m_listCCIntervalsUndo.RemoveAll();
	*/


	int numModels = m_arrCCInfo.GetSize();
	int nISs = m_arrIntervals.GetUpperBound();
	for(int j = 0; j <= nISs; j++ )
	{
		IntervalsSet* pIS = m_arrIntervals[j];
		for(int i = 0; i < numModels; i++)
		{
			delete pIS->ppnLoRanges[i];
			delete pIS->ppnHiRanges[i];
		}
		delete pIS->ppnLoRanges;
		delete pIS->ppnHiRanges;

		POSITION pos = pIS->listCCIntervalsUndo.GetHeadPosition();
		IntervalUndo* pIntervalUndo = 0;
		while(pos)
		{
			pIntervalUndo = pIS->listCCIntervalsUndo.GetNext(pos);
			if(pIntervalUndo)
			{
				delete pIntervalUndo->pnLoRanges;
				delete pIntervalUndo->pnHiRanges;
				delete pIntervalUndo;
				pIntervalUndo = 0;
			}
		}
		pIS->listCCIntervalsUndo.RemoveAll();

		delete pIS;
		pIS = 0;

	}
	m_arrIntervals.RemoveAll();

	_KillCCArray();
}


/////////////////////////////////////////////////////////////////////////////
// CColorManAXCtrl::OnDraw - Drawing function

void CColorManAXCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	// TODO: Replace the following code with your own drawing code.
	pdc->FillRect(rcBounds, &CBrush(::GetSysColor(COLOR_BTNFACE)));

	//draw edge
	UINT	edgeType = 0;
	if(m_innerRaised)
		edgeType |= BDR_RAISEDINNER;
	if(m_innerSunken)
		edgeType |= BDR_SUNKENINNER;
	if(m_outerRaised)
		edgeType |= BDR_RAISEDOUTER;
	if(m_outerSunken)
		edgeType |= BDR_SUNKENOUTER;
	if(edgeType != 0)
	{
		pdc->DrawEdge(CRect(1,1,rcBounds.Width()-2,rcBounds.Height()-2), edgeType, BF_RECT);
	}

}


/////////////////////////////////////////////////////////////////////////////
// CColorManAXCtrl::DoPropExchange - Persistence support

void CColorManAXCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CColorManAXCtrl::OnResetState - Reset control to default state

void CColorManAXCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}



/////////////////////////////////////////////////////////////////////////////
// CColorManAXCtrl IVtActiveXCtrl implement
IMPLEMENT_UNKNOWN(CColorManAXCtrl, ActiveXCtrl);

HRESULT CColorManAXCtrl::XActiveXCtrl::SetSite( IUnknown *punkVtApp, IUnknown *punkSite )
{
	METHOD_PROLOGUE_EX(CColorManAXCtrl, ActiveXCtrl)

	pThis->m_sptrSite = punkSite;
	pThis->m_sptrApp = punkVtApp;	

	pThis->_InitCCcombo();
	pThis->_ManageEnabled();
	//pThis->_LoadState();

	return S_OK;
}
  

HRESULT CColorManAXCtrl::XActiveXCtrl::GetName( BSTR *pbstrName )
{
	METHOD_PROLOGUE_EX(CColorManAXCtrl, ActiveXCtrl)	
	*pbstrName = NULL;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CColorManAXCtrl message handlers

void CColorManAXCtrl::OnInnerSunkenChanged() 
{
	// TODO: Add notification handler code
	SetModifiedFlag();
}

void CColorManAXCtrl::OnOuterRaisedChanged() 
{
	// TODO: Add notification handler code
	SetModifiedFlag();
}

void CColorManAXCtrl::OnOuterSunkenChanged() 
{
	// TODO: Add notification handler code
	SetModifiedFlag();
}

void CColorManAXCtrl::OnInnerRaisedChanged() 
{
	// TODO: Add notification handler code
	SetModifiedFlag();
}

void CColorManAXCtrl::Serialize(CArchive& ar) 
{
	if (ar.IsStoring())
	{	// storing code
		ar<<(BYTE)m_innerRaised;
		ar<<(BYTE)m_innerSunken;
		ar<<(BYTE)m_outerRaised;
		ar<<(BYTE)m_outerSunken;
	}
	else
	{	// loading code
		BYTE byte;
		ar>>byte;
		m_innerRaised = (BOOL)byte;
		ar>>byte;
		m_innerSunken = (BOOL)byte;
		ar>>byte;
		m_outerRaised = (BOOL)byte;
		ar>>byte;
		m_outerSunken = (BOOL)byte;
	}

	SerializeStockProps(ar);
}

BOOL CColorManAXCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	return COleControl::PreCreateWindow(cs);
}

int CColorManAXCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rc = NORECT;
	GetWindowRect(&rc);
	int nControlWidth = rc.Width();

	// TODO: Add your specialized creation code here
	
	int nWidthCB = min(nControlWidth, 170);
	m_pcbColorModel = new CComboBoxEx();
	CRect rcCB(nVertInterval, nHorzInterval, nVertInterval+nWidthCB, nHorzInterval+400);
	m_pcbColorModel->Create(WS_CHILD|CBS_DROPDOWNLIST|WS_VISIBLE|WS_TABSTOP, rcCB, this, IDcombo);

	int nCEditWidth = (rcCB.Width() - nVertInterval)/2;
	int nCEditHeight = m_pcbColorModel->GetItemHeight(-1)+3;

	m_pEdit1 = new CEdit();
	m_pEdit2 = new CEdit();
	CRect rcEdit1(rcCB.left, nCEditHeight+3*nHorzInterval, rcCB.left+nCEditWidth, nCEditHeight+3*nHorzInterval+nCEditHeight);
	CRect rcEdit2(rcCB.left+nVertInterval+nCEditWidth, nCEditHeight+3*nHorzInterval, rcCB.left+nVertInterval+2*nCEditWidth, nCEditHeight+3*nHorzInterval+nCEditHeight);

	m_pSpin1 = new CSpinButtonCtrl();
	m_pSpin2 = new CSpinButtonCtrl();

	
	DWORD dwStyle = WS_CHILDWINDOW|WS_VISIBLE/*|WS_BORDER*/|WS_TABSTOP|ES_LEFT|ES_AUTOHSCROLL|ES_NUMBER;
	DWORD dwExStyle = WS_EX_CLIENTEDGE;
	CString strInitEdit;
	strInitEdit.Format("%li", InitHiLoValue);
	((CWnd*)m_pEdit1)->CreateEx(dwExStyle, "Edit", strInitEdit, dwStyle, rcEdit1, this, IDedit1);

	((CWnd*)m_pEdit2)->CreateEx(dwExStyle, "Edit", strInitEdit, dwStyle, rcEdit2, this, IDedit2);

	
	m_pSpin1->Create(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT|UDS_ALIGNRIGHT|UDS_ARROWKEYS, NORECT, this, IDspin1);
	m_pSpin2->Create(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT|UDS_ALIGNRIGHT|UDS_ARROWKEYS, NORECT, this, IDspin2);
	m_pSpin1->SetBuddy((CWnd*)m_pEdit1);
	m_pSpin2->SetBuddy((CWnd*)m_pEdit2);

	m_pSpin1->SetRange32(0, HistColorBound-1);
	m_pSpin2->SetRange32(0, HistColorBound-1);


	m_pStaticWorkImage = new CStatic();
	CRect rcStaticWorkImage(rcCB.left, nCEditHeight+4*nHorzInterval+nCEditHeight, rcCB.left + 2*nCEditWidth, nCEditHeight+4*nHorzInterval+2*nCEditHeight);
	m_pStaticWorkImage->Create("Working image", WS_CHILD|WS_VISIBLE|SS_LEFT, rcStaticWorkImage, this, IDstatic);

	m_pRadio1 = new CButton();
	m_pRadio2 = new CButton();
	m_pRadio3 = new CButton();

	DWORD dwRadioStyle = WS_CHILD/*|WS_VISIBLE*/|BS_AUTORADIOBUTTON;
	int nRadioLeft = nWidthCB + 2*nVertInterval;
	int nRadioWidth = nControlWidth - nRadioLeft - nVertInterval - 1;
	//nRadioWidth = max(nRadioWidth, 85);

	m_pcbColorModel->GetWindowRect(&rcCB);
	m_pEdit1->GetWindowRect(&rcEdit1);
	int nRadioHeight = (rcEdit1.bottom - rcCB.top - 2*nHorzInterval)/3;

	CRect rcRadio1(nRadioLeft, 1*nHorzInterval+0*nRadioHeight, nRadioLeft+nRadioWidth, 1*nHorzInterval+1*nRadioHeight);
	CRect rcRadio2(nRadioLeft, 2*nHorzInterval+1*nRadioHeight, nRadioLeft+nRadioWidth, 2*nHorzInterval+2*nRadioHeight);
	CRect rcRadio3(nRadioLeft, 3*nHorzInterval+2*nRadioHeight, nRadioLeft+nRadioWidth, 3*nHorzInterval+3*nRadioHeight);

	m_pRadio1->Create("P1", dwRadioStyle|WS_GROUP, rcRadio1, this, IDradio1);
	m_pRadio2->Create("P2", dwRadioStyle, rcRadio2, this, IDradio2);
	m_pRadio3->Create("P3", dwRadioStyle, rcRadio3, this, IDradio3);

	m_pRadio1->SetCheck(1);
	FireOnPaneChange(0);

	Register();


	//_Layout();

	return 0;
}

void CColorManAXCtrl::_InitCCcombo()
{
	CFont* pFont = CFont::FromHandle(InternalGetFont().GetFontHandle());
	m_pEdit1->SetFont(pFont);
	m_pEdit2->SetFont(pFont);
	m_pRadio1->SetFont(pFont);
	m_pRadio2->SetFont(pFont);
	m_pRadio3->SetFont(pFont);

	if(m_sptrApp == 0 || m_pcbColorModel == 0 || m_pcbColorModel->m_hWnd == 0)
		return;

	m_imgCC.Create(nIconX, nIconY, ILC_COLOR, 0, 1);
	m_imgCC.SetBkColor(::GetSysColor(COLOR_WINDOW));
	m_pcbColorModel->SetImageList(&m_imgCC);

	IUnknown* punk = 0;
	punk = _GetOtherComponent(m_sptrApp, IID_IColorConvertorManager);
	ICompManagerPtr sptrCM(punk);
	if(punk)
		punk->Release();
	if(sptrCM != 0)
	{
		int numCvtrs = 0;
		sptrCM->GetCount(&numCvtrs);

		m_arrConvertedImages.SetSize(numCvtrs);

		BSTR bstr = 0;
		sptrIColorConvertor4 sptrCC;
		COMBOBOXEXITEM cbItem;
		int idx = 0;
		for(int i = 0; i < numCvtrs; i++)
		{
			sptrCM->GetComponentUnknownByIdx(i, &punk);
			sptrCC = punk;
			punk->Release();
			sptrCC->GetCnvName(&bstr);
			CString strName(bstr);
			::SysFreeString(bstr);
			HICON hIcon = 0;
			sptrCC->GetConvertorIcon( &hIcon );
			m_imgCC.Add(hIcon);
			cbItem.mask = CBEIF_TEXT|CBEIF_DI_SETITEM|CBEIF_IMAGE|CBEIF_SELECTEDIMAGE;
			cbItem.pszText = (LPTSTR)(LPCTSTR)strName;
			cbItem.iImage = idx;
			cbItem.iSelectedImage = idx;
			cbItem.iItem = idx;
			idx++;
			m_pcbColorModel->InsertItem(&cbItem);	


			int nPanes = 0;
			sptrCC->GetColorPanesDefCount(&nPanes);

			CCInfo* pCCInfo = new CCInfo;
			pCCInfo->nPanes = nPanes;
			if(nPanes > 0)
			{
				pCCInfo->pszPanesCaptions = new BSTR[nPanes];
				pCCInfo->pdwFlags = new DWORD[nPanes];
				pCCInfo->pdwPanesColors =  new DWORD[nPanes];
				for(int i = 0; i < nPanes; i++)
				{
					BSTR bstrName = 0;
					sptrCC->GetPaneName(i, &bstrName);
					pCCInfo->pszPanesCaptions[i] = bstrName;
					DWORD dwFlags = pfOrdinary;
					sptrCC->GetPaneFlags(i, &dwFlags);
					pCCInfo->pdwFlags[i] = dwFlags;
					DWORD dwColor;
					sptrCC->GetPaneColor(i, &dwColor); 
					pCCInfo->pdwPanesColors[i] = dwColor;
					//::SysFreeString(bstrName);
				}
			}
			else
			{
				pCCInfo->pszPanesCaptions = 0;
				pCCInfo->pdwFlags = 0;
			}
			m_arrCCInfo.Add(pCCInfo);
		}
	}

	_AddIntervalsSet();
	ASSERT(m_nCurIntervalsSet == 0);

	_SetEditors(InitHiLoValue, InitHiLoValue);
}

void CColorManAXCtrl::_ManageEnabled()
{
	//SetEnabled(FALSE);

	if( m_sptrWorkingImage != 0 )
		return;

	if(m_sptrApp != 0)
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
			IImageViewPtr sptrView(punkView);
			if(punkView)
				punkView->Release();
			if(sptrView != 0)
			{
				IUnknown* punkImage = 0;
				sptrView->GetActiveImage(&punkImage, 0);
				IImagePtr	sptrImage(punkImage);

				if(m_sptrWorkingImage == 0)
				{
					m_sptrWorkingImage = punkImage;

					_FireChangeWorkingImage();
				}

				if(punkImage)
					punkImage->Release();


				if(m_sptrWorkingImage == sptrImage)
				{
					//SetEnabled(TRUE);

					m_nInfiniteImageIdx = _GetCnvNumberInCB(sptrImage);

					if(m_nInfiniteImageIdx >= 0)
					{
						m_arrConvertedImages[m_nInfiniteImageIdx] = sptrImage;
						_SetupInterval(m_nInfiniteImageIdx);
					}
				}
			}
		}
	}
}

void CColorManAXCtrl::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( (!strcmp( pszEvent, szAppActivateView )) || (!strcmp( pszEvent, szEventActivateObject )))
	{
		_ManageEnabled();
	}
	else if(!strcmp(pszEvent, szEventManageColorInterval))
	{
		_ManageColorInterval(punkFrom);
	}
}

void CColorManAXCtrl::OnFinalRelease() 
{
	// TODO: Add your specialized code here and/or call the base class
	//_StoreState();

	if( m_sptrApp != 0 )
	{
		CObjectListUndoRecord changes;
		IUnknown* punkDoc = 0;
		m_sptrApp->GetActiveDocument(&punkDoc);
		::SetValue(punkDoc, "ExtractPhases", "WorkImage", "");

		for(int i = 0; i < m_arrConvertedImages.GetSize(); i++)
		{
			if(i != m_nInfiniteImageIdx)
				changes.RemoveFromDocData(punkDoc, m_arrConvertedImages[i]);
			m_arrConvertedImages[i] = 0;
		}
		m_arrConvertedImages.RemoveAll();
		if(punkDoc)
			punkDoc->Release();

		UnRegister();
	}
	COleControl::OnFinalRelease();
	//delete this;
}

void CColorManAXCtrl::_Layout()
{
	HDWP hdwp = ::BeginDeferWindowPos(10);
	DWORD	dwFlags = SWP_NOACTIVATE|SWP_NOZORDER;
	
	CRect rcCB(nVertInterval, nHorzInterval, nVertInterval+100, nHorzInterval+400);	
	//CRect rcEdit1
	//CRect rcEdit2
	//CRect rcRadio1
	//CRect rcRadio2
	//CRect rcRadio3

	hdwp = ::DeferWindowPos(hdwp, *m_pcbColorModel, HWND_NOTOPMOST, rcCB.left, rcCB.top, rcCB.Width(), rcCB.Height(), dwFlags);

	::EndDeferWindowPos( hdwp );
}

static bool bFirstUpdate = true;
BOOL CColorManAXCtrl::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(HIWORD(wParam) == EN_UPDATE)	
	{
		//validate edit data
		if(!bFirstUpdate)
		{
			return -1;
		}
		CString	strText;
		char chDigSeparator[1];
		::GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_STHOUSAND, chDigSeparator, 1);

		long nNumber2 = 65535;
		long nNumber1 = 0;
		if(m_pEdit1->GetSafeHwnd()) 
		{
			m_pEdit1->GetWindowText(strText);
			strText.Remove(chDigSeparator[0]);
			nNumber1 = atol(strText);
		}
		if(m_pEdit2->GetSafeHwnd())
		{
			m_pEdit2->GetWindowText(strText);
			strText.Remove(chDigSeparator[0]);
			nNumber2 = atol(strText);
		}

		if(m_pEdit1->GetSafeHwnd() == (HWND)lParam)
		{
			if(nNumber1 >= HistColorBound)
			{
				nNumber1 = HistColorBound-1;
				MessageBeep(MB_OK);
			}
			if(nNumber1 > nNumber2)
			{
				nNumber1 = nNumber2;
			}
			
			bFirstUpdate = false;
			_ChangeCurLo(nNumber1);
			bFirstUpdate = true;
		}
		else if(m_pEdit2->GetSafeHwnd() == (HWND)lParam)
		{
			if(nNumber2 >= HistColorBound)
			{
				nNumber2 = HistColorBound-1;
				MessageBeep(MB_OK);
			}
			if(nNumber1 > nNumber2)
			{
				nNumber2 = nNumber1;
			}
			bFirstUpdate = false;
			_ChangeCurHi(nNumber2);
			bFirstUpdate = true;
		}
	}
	
	return COleControl::OnCommand(wParam, lParam);
}

void CColorManAXCtrl::_LoadState()
{
	if(m_sptrApp == 0) return;

	/*//_KillCCArray();

	CString strEntry;
	long nVal = 0;
	CString strText;
	int nIdx = 0;

	for(int i = 0; i < m_arrCCIntervals.GetSize(); i++)
	{
		for(int j = 0; j < m_arrCCIntervals[i]->nPanes; j++)
		{
			CString strSubEntry;
			strSubEntry.Format("Pane_%d_", j);
			//CString strPaneName;
			//strPaneName.Format("P%d", j);
			//m_arrCCIntervals[i]->pstrPanesCaptions[j] = ::GetValueString(m_sptrApp, "ColorManAX", strEntry+strSubEntry+"HiRange", strPaneName);
			m_arrCCIntervals[i]->pnLoRanges[j] = (WORD)::GetValueInt(m_sptrApp, "ColorManAX", strEntry+strSubEntry+"LoRange", 0);
			m_arrCCIntervals[i]->pnHiRanges[j] = (WORD)::GetValueInt(m_sptrApp, "ColorManAX", strEntry+strSubEntry+"HiRange", 0);
		}
	}

	/*while(true)
	{
		strEntry.Format("ColorModel%d_", nIdx);
		
		int Panes = ::GetValueInt(m_sptrApp, "ColorManAX", strEntry+"Panes", -1);
		if(Panes < 0)
			break;

		CCIntervals* pCCIntervalsInfo = new CCIntervals;

		pCCIntervalsInfo->nPanes = nPanes;
		if(nPanes > 0)
		{
			pCCIntervalsInfo->pnLoRanges = new BYTE[nPanes];
			pCCIntervalsInfo->pnHiRanges = new BYTE[nPanes];
		}
		else
		{
			pCCIntervalsInfo->pnLoRanges = 0;
			pCCIntervalsInfo->pnHiRanges = 0;
		}
		
		for(int i = 0; i < Panes; i++)
		{
			CString strSubEntry;
			strSubEntry.Format("Pane_%d_", i);
			pCCIntervalsInfo->pnLoRanges[i] = ::GetValueInt(m_sptrApp, "ColorManAX", strEntry+strSubEntry+"LoRange", 0);
			pCCIntervalsInfo->pnHiRanges[i] = ::GetValueInt(m_sptrApp, "ColorManAX", strEntry+strSubEntry+"HiRange", 0);
		}
		m_arrCCIntervals.Add(pCCIntervalsInfo);
		
		nIdx++;
	}
	*/
}

void CColorManAXCtrl::_StoreState()
{
	if(m_sptrApp == 0) return;

	/*//_SetPaneInterval(0, -1, false);

	int nEntries = m_arrCCIntervals.GetSize();
	CString strEntry;
	for(int i = 0; i < nEntries; i++)
	{
		strEntry.Format("ColorModel%d_", i);
		//::SetValue(m_sptrApp, "ColorManAX", strEntry+"Panes", _variant_t(m_arrCCIntervals[i]->nPanes));
		for(int j = 0; j < m_arrCCIntervals[i]->nPanes; j++)
		{
			CString strSubEntry;
			strSubEntry.Format("Pane_%d_", j);
			//::SetValue(m_sptrApp, "ColorManAX", strEntry+strSubEntry+"Name", _variant_t(m_arrCCIntervals[i]->pstrPanesCaptions[j]));
			::SetValue(m_sptrApp, "ColorManAX", strEntry+strSubEntry+"LoRange", _variant_t((long)m_arrCCIntervals[i]->pnLoRanges[j]));
			::SetValue(m_sptrApp, "ColorManAX", strEntry+strSubEntry+"HiRange", _variant_t((long)m_arrCCIntervals[i]->pnHiRanges[j]));
		}
		//::SetValue(m_sptrApp, "ColorManAX", strEntry+"ActivePane", _variant_t(_GetActiveRadio()));
	}
	*/
}

int CColorManAXCtrl::_GetActiveRadio()
{
	int nRet = -1;

	if(m_pRadio1->GetCheck() == 1)
		nRet = 0;
	else if(m_pRadio2->GetCheck() == 1)
		nRet = 1;
	else if(m_pRadio3->GetCheck() == 1)
		nRet = 2;

	return nRet;
}

void CColorManAXCtrl::_KillCCArray()
{
	for(int i = 0; i < m_arrCCInfo.GetSize(); i++)
	{
		for(int j = 0; j < m_arrCCInfo[i]->nPanes; j++)
		{
			::SysFreeString(m_arrCCInfo[i]->pszPanesCaptions[j]);
		}
		
		if(m_arrCCInfo[i]->pszPanesCaptions)
			delete m_arrCCInfo[i]->pszPanesCaptions;
		if(m_arrCCInfo[i]->pdwFlags)
			delete m_arrCCInfo[i]->pdwFlags;
		if(m_arrCCInfo[i]->pdwPanesColors)
			delete m_arrCCInfo[i]->pdwPanesColors;
	
		delete m_arrCCInfo[i];
	}
	m_arrCCInfo.RemoveAll();
}

int CColorManAXCtrl::_GetCnvNumberInCB(IUnknown* punkImage)
{
	IImagePtr	sptrImage(punkImage);
	if(sptrImage == 0)
		return - 1;

	IUnknown* punkCC = 0;
	sptrImage->GetColorConvertor(&punkCC);
	IColorConvertor2Ptr sptrCC(punkCC);
	if(punkCC)
		punkCC->Release();
	if(sptrCC != 0 && m_pcbColorModel != 0)
	{
		BSTR bstrName = 0;
		sptrCC->GetCnvName(&bstrName);
		CString strName(bstrName);
		::SysFreeString(bstrName);

		int nCBcount = m_pcbColorModel->GetCount();
		CString strItem;
		for(int i = 0; i < nCBcount; i++)
		{
			m_pcbColorModel->GetLBText(i, strItem);
			if(strItem == strName)
			{
				return i;
				break;
			}
		}	

	}
	return - 1;
}

void CColorManAXCtrl::OnCloseCB()
{
	int nModel = m_pcbColorModel->GetCurSel();

	if(m_nLastCBSel == nModel)
		return;

	/*CString strObjectName;
	IUnknown* punkDoc = 0;
	m_sptrApp->GetActiveDocument(&punkDoc);
	IDocumentSitePtr sptrDoc(punkDoc);
	if(punkDoc)
		punkDoc->Release();
	if(sptrDoc != 0)
	{
		IUnknown* punkView = 0;
		sptrDoc->GetActiveView(&punkView);
		IImageViewPtr sptrView(punkView);
		if(punkView)
			punkView->Release();
		if(sptrView != 0)
		{
			IUnknown* punkImage = 0;
			sptrView->GetActiveImage(&punkImage, 0);
			if(punkImage)
			{
				strObjectName = GetObjectName(punkImage);
				punkImage->Release();
			}
		}
	}*/
	

	IImagePtr	sptrImgAlreadyCoverted = m_arrConvertedImages[nModel];
	if(sptrImgAlreadyCoverted == 0)
	{

		CString	strModelName = "";
		m_pcbColorModel->GetLBText(nModel, strModelName);
		//::ExecuteAction(_T("ConvertImage"), "\""+strModelName+"\""+","+"\""+strObjectName+"\""+","+"\""+strObjectName+"\"");

		if(m_sptrWorkingImage == 0)
			return;
		//CString strWorkImageName(::GetObjectName(m_sptrWorkingImage)
		CString strInfiniteImageName(::GetObjectName(m_arrConvertedImages[m_nInfiniteImageIdx]));
	
		CString strOutImageName = "Converted_";
		strOutImageName += strModelName;

		::ExecuteAction(_T("ConvertImage"), "\""+strModelName+"\""+","+"\"0\""+","+"\""+strInfiniteImageName+"\""+","+"\""+strOutImageName+"\"");


		//set working image with name = strOutImageName
		IUnknown* punkDoc = 0;
		m_sptrApp->GetActiveDocument(&punkDoc);
		IUnknown* punkObj = ::GetObjectByName(punkDoc, strOutImageName);
		m_sptrWorkingImage = punkObj;
		if(punkDoc)
			punkDoc->Release();
		if(punkObj)
			punkObj->Release();


		int nNum = _GetCnvNumberInCB(m_sptrWorkingImage);
		ASSERT(nNum >= 0);
		m_arrConvertedImages[nNum] = m_sptrWorkingImage;
	}
	else
		m_sptrWorkingImage = sptrImgAlreadyCoverted;


	_FireChangeWorkingImage();

	_SetupInterval(nModel);

	FireOnColorModelChange();
}

void CColorManAXCtrl::_SetupInterval(int nModel)
{
	int nCBcount = m_pcbColorModel->GetCount();
	if(nModel >= nCBcount || nModel < 0) return;

	_SetPaneInterval(m_nLastRadioSel, nModel);
	
	if(m_pcbColorModel->GetCurSel() != nModel)
		m_pcbColorModel->SetCurSel(nModel);
	//m_nLastCBSel = nModel;


	//set radio captions by cur CC
	int nPanes = m_arrCCInfo[m_nLastCBSel]->nPanes;
	if(nPanes == 1)
	{
		if(!m_pRadio1->IsWindowVisible())
			m_pRadio1->ShowWindow(SW_SHOW);
		m_pRadio1->SetWindowText(CString(m_arrCCInfo[m_nLastCBSel]->pszPanesCaptions[0]));
		if(m_pRadio2->IsWindowVisible())
			m_pRadio2->ShowWindow(SW_HIDE);
		if(m_pRadio3->IsWindowVisible())
			m_pRadio3->ShowWindow(SW_HIDE);
	}
	else if(nPanes == 3)
	{
		if(!m_pRadio1->IsWindowVisible())
			m_pRadio1->ShowWindow(SW_SHOW);
		m_pRadio1->SetWindowText(CString(m_arrCCInfo[m_nLastCBSel]->pszPanesCaptions[0]));
		if(!m_pRadio2->IsWindowVisible())
			m_pRadio2->ShowWindow(SW_SHOW);
		m_pRadio2->SetWindowText(CString(m_arrCCInfo[m_nLastCBSel]->pszPanesCaptions[1]));
		if(!m_pRadio3->IsWindowVisible())
			m_pRadio3->ShowWindow(SW_SHOW);
		m_pRadio3->SetWindowText(CString(m_arrCCInfo[m_nLastCBSel]->pszPanesCaptions[2]));
	}
	else
	{
		if(!m_pRadio1->IsWindowVisible())
			m_pRadio1->ShowWindow(SW_HIDE);
		if(m_pRadio2->IsWindowVisible())
			m_pRadio2->ShowWindow(SW_HIDE);
		if(m_pRadio3->IsWindowVisible())
			m_pRadio3->ShowWindow(SW_HIDE);
	}

	if(m_nLastRadioSel < 0 || _GetActiveRadio() >= nPanes)
	{
		m_nLastRadioSel = 0;
		m_pRadio2->SetCheck(0);
		m_pRadio3->SetCheck(0);

		if(m_pRadio1->IsWindowVisible())
			m_pRadio1->SetCheck(1);

		FireOnPaneChange(0);
	}
}

void CColorManAXCtrl::_SetPaneInterval(int nPane, int nModel, bool bSetText)
{
	if(nModel < 0)
		nModel = m_nLastCBSel;

	IntervalsSet* pIS = m_arrIntervals[m_nCurIntervalsSet];
	/*
	if(m_nLastCBSel >= 0)
	{
		//store prev state
		if(m_arrCCInfo[m_nLastCBSel]->nPanes > m_nLastRadioSel)
		{
			CString strText;
			m_pEdit1->GetWindowText(strText);
			pIS->ppnLoRanges[m_nLastCBSel][m_nLastRadioSel] = atoi(strText);
			m_pEdit2->GetWindowText(strText);
			pIS->ppnHiRanges[m_nLastCBSel][m_nLastRadioSel] = atoi(strText);
		}
	}
	*/

	nPane = min(m_arrCCInfo[nModel]->nPanes-1, nPane);
	m_nLastRadioSel = nPane;
	m_nLastCBSel = nModel;

	if(bSetText)
	{
		_SetEditors(pIS->ppnLoRanges[m_nLastCBSel][nPane], pIS->ppnHiRanges[m_nLastCBSel][nPane]);	
	}

	FireOnPaneChange(nPane);
}

void CColorManAXCtrl::_SetEditors(WORD nLo, WORD nHi)
{
	if(m_pEdit2->m_hWnd == 0 || m_pEdit1->m_hWnd == 0) return;
	CString strText;
	if(nLo > m_hiRange)
	{
		_ChangeCurHi(nHi);
		_ChangeCurLo(nLo);
	}
	else
	{
		_ChangeCurLo(nLo);
		_ChangeCurHi(nHi);
	}
}

void CColorManAXCtrl::OnRadioChange(UINT nID)
{
	int nPane = _GetActiveRadio();
	if(nPane >= 0)
	{
		_SetPaneInterval(nPane);
	}

}

void CColorManAXCtrl::_ChangeCurLo(WORD nLo)
{
	m_loRange = nLo;
	CString strText;
	strText.Format("%li", nLo);
	m_pEdit1->SetWindowText(strText);
	if(m_nCurIntervalsSet >= 0)
	{
		IntervalsSet* pIS = m_arrIntervals[m_nCurIntervalsSet];
		pIS->ppnLoRanges[m_nLastCBSel][m_nLastRadioSel] = nLo;
	}

	FireOnChangeLoRange(nLo);
}

void CColorManAXCtrl::_ChangeCurHi(WORD nHi)
{
	m_hiRange = nHi;
	CString strText;
	strText.Format("%li", nHi);
	m_pEdit2->SetWindowText(strText);
	if(m_nCurIntervalsSet >= 0)
	{
		IntervalsSet* pIS = m_arrIntervals[m_nCurIntervalsSet];
		pIS->ppnHiRanges[m_nLastCBSel][m_nLastRadioSel] = nHi;
	}

	FireOnChangeHiRange(nHi);
}

void CColorManAXCtrl::OnHiRangeChanged() 
{
	// TODO: Add notification handler code
	_ChangeCurHi(m_hiRange);
	SetModifiedFlag();
}

void CColorManAXCtrl::OnLoRangeChanged() 
{
	// TODO: Add notification handler code
	_ChangeCurLo(m_loRange);
	SetModifiedFlag();
}

void CColorManAXCtrl::_AdjustLimits(WORD wordVal, WORD &iLow, WORD &iHigh, bool bCycled)
{
	//_try( CColorManAXCtrl, _AdjustLimits)

//1. currently limits are empty (equals)
	if( iLow == iHigh )
	{
		iLow = wordVal;
		iHigh  = wordVal+1;
		return;
	}
//2. case if limits no-cycled
	if( !bCycled )
	{
		if( m_addInterval )
		{
			iLow = min( iLow, wordVal );
			iHigh = max( iHigh, wordVal+1 );
		}
		else
		{
			if( iLow <= wordVal && iHigh > wordVal )
			{
				if( ::abs( iHigh - wordVal ) > ::abs( iLow - wordVal ) )
					iLow = min( iHigh, wordVal+1 );
				else
					iHigh = max( iLow, wordVal );
			}

		}
	}
//3. Limits are cycled
	else
	{
		if( m_addInterval )
		{
//1.	return if no expansion required
			if( iLow < iHigh && iLow <= wordVal && iHigh > wordVal  )
				return;
			if( iLow > iHigh && ( wordVal >= iLow  || wordVal < iHigh ) )
				return;

			iHigh--;


			if( iLow <= iHigh )	//normal limits order
			{
				if( wordVal < iLow )
				{
					if( iLow - wordVal <= wordVal-iHigh+HistColorBound )	//decrease iLow
						iLow = wordVal;
					else										//swap iLow and iHigh
					{
						iHigh = wordVal;
					}
				}

				if( wordVal > iHigh )
				{
					if( wordVal - iHigh <= iLow-wordVal+HistColorBound )
						iHigh = wordVal;
					else
					{
						iLow = wordVal;
					}
				}
			}
			else
			{
				if( wordVal - iHigh < iLow - wordVal )
					iHigh = wordVal;
				else
					iLow = wordVal;
			}


			iHigh++;
		}
		else
		{
			//1.	return if no ajustement required
			if( iLow < iHigh && ( iLow > wordVal || iHigh <= wordVal ) )
				return;
			if( iLow > iHigh && ( wordVal < iLow  && wordVal >= iHigh ) )
				return;

			iHigh--;


			if( iLow <= iHigh )	//normal limits order
			{
				if( iHigh - wordVal < wordVal - iLow )
					iHigh = wordVal-1;
				else
					iLow = wordVal+1;
			}
			else
			{
				if( wordVal < iHigh )
				{
					if( wordVal - iHigh <= iLow-wordVal+HistColorBound )	//decrese iHigh
						iHigh = wordVal-1;
					else										//swap iLow and iHigh
						iLow = wordVal+1;
				}

				if( wordVal > iLow )
				{
					if( wordVal - iLow <= iHigh-wordVal+HistColorBound )	//increase iLow
						iLow = wordVal+1;
					else										//swap iLow and iHigh
						iHigh = wordVal-1;
				}
			}

			iHigh++;
		}	
	}
	//_catch;
}

void CColorManAXCtrl::_ManageColorInterval(IUnknown* punkProvideColors)
{
	IProvideColorsPtr sptrProvideColors(punkProvideColors);
	if(sptrProvideColors == 0) return;

	FireOnManageColorInterval();

	long nCount = 0;
	DWORD* pdwColors = 0;
	sptrProvideColors->GetColorsInfo(&nCount, &pdwColors);
	int nCurModel = m_pcbColorModel->GetCurSel();
	if(nCurModel < 0) return;

	IntervalsSet* pIS = m_arrIntervals[m_nCurIntervalsSet];

	//for Undo
	_CopyToUndo(nCurModel);
	

	int nPanes = m_arrCCInfo[nCurModel]->nPanes;
	int nIdx = nCount;
	int nActiveRadio = _GetActiveRadio();
	ASSERT(nActiveRadio >= 0);
	while(nIdx > 0)
	{
		for(int i = 0; i < nPanes; i++)
		{
			if(m_addInterval || (i == nActiveRadio))
			{
				_AdjustLimits(pdwColors[i + (nCount-nIdx)], pIS->ppnLoRanges[nCurModel][i], pIS->ppnHiRanges[nCurModel][i], m_arrCCInfo[nCurModel]->pdwFlags[i]&pfCycled);
			}
		}
		nIdx -= nPanes;
	}
	m_nLastCBSel = -1;
	_SetPaneInterval(m_nLastRadioSel, nCurModel, true);
}

void CColorManAXCtrl::OnAddIntervalChanged() 
{
	// TODO: Add notification handler code
	SetModifiedFlag();
}

void CColorManAXCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	//if(nSBCode)	
	COleControl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CColorManAXCtrl::IsPaneCycled(short nPane) 
{
	// TODO: Add your dispatch handler code here
	BOOL bCycled = FALSE;
	if(nPane >= 0 && nPane < m_arrCCInfo[m_nLastCBSel]->nPanes)
	{
		if(m_arrCCInfo[m_nLastCBSel]->pdwFlags[nPane] & pfCycled) 
			bCycled = TRUE;
	}
	return bCycled;
}

void CColorManAXCtrl::ResetIntervalsSet(short nIS) 
{
	// TODO: Add your dispatch handler code here
	if(nIS < 0 ) nIS = m_nCurIntervalsSet;
	IntervalsSet* pIS = m_arrIntervals[nIS];

	int numModels = m_arrCCInfo.GetSize();
	for(int i = 0; i < numModels; i++)
	{
		for(int j = 0; j < m_arrCCInfo[i]->nPanes; j++)
		{
			pIS->ppnHiRanges[i][j] = pIS->ppnLoRanges[i][j] = InitHiLoValue;
		}
	}
	if(nIS == m_nCurIntervalsSet)
		_SetEditors(InitHiLoValue, InitHiLoValue);
}

void CColorManAXCtrl::_CopyToUndo(int nModel)
{
	IntervalUndo* pIntervalUndo = new IntervalUndo;

	int nPanes = m_arrCCInfo[nModel]->nPanes;

	pIntervalUndo->pnHiRanges = new WORD[nPanes];
	pIntervalUndo->pnLoRanges = new WORD[nPanes];
	pIntervalUndo->nModel = nModel;

	IntervalsSet* pIS = m_arrIntervals[m_nCurIntervalsSet];

	memcpy(pIntervalUndo->pnHiRanges, pIS->ppnHiRanges[nModel], sizeof(WORD)*nPanes);
	memcpy(pIntervalUndo->pnLoRanges, pIS->ppnLoRanges[nModel], sizeof(WORD)*nPanes);
	
	pIS->listCCIntervalsUndo.AddHead(pIntervalUndo);

	if(pIS->listCCIntervalsUndo.GetCount() == 1)
		FireOnChangeUndoState();
}


bool CColorManAXCtrl::_IsUndoAvail()
{
	if(m_nCurIntervalsSet < 0) return false;
	IntervalsSet* pIS = m_arrIntervals[m_nCurIntervalsSet];
	return pIS->listCCIntervalsUndo.GetCount() > 0;
}

void CColorManAXCtrl::UndoChangeIntervals() 
{
	// TODO: Add your dispatch handler code here
	if(!_IsUndoAvail()) return;

	IntervalsSet* pIS = m_arrIntervals[m_nCurIntervalsSet];

	IntervalUndo* pIntervalUndo = pIS->listCCIntervalsUndo.RemoveHead();

	int nUndoModel = -1;
	if(pIntervalUndo)
	{
		nUndoModel = pIntervalUndo->nModel;
		int nPanes = m_arrCCInfo[nUndoModel]->nPanes;

		memcpy(pIS->ppnHiRanges[nUndoModel], pIntervalUndo->pnHiRanges, sizeof(WORD)*nPanes);
		memcpy(pIS->ppnLoRanges[nUndoModel], pIntervalUndo->pnLoRanges, sizeof(WORD)*nPanes);

		delete pIntervalUndo->pnLoRanges;
		delete pIntervalUndo->pnHiRanges;
		delete pIntervalUndo;
		pIntervalUndo = 0;
	}

	if(nUndoModel >= 0)
	{
		int nCurModel = m_pcbColorModel->GetCurSel();
		if(nCurModel == nUndoModel)
		{
			//need update
			_SetEditors(pIS->ppnLoRanges[nCurModel][m_nLastRadioSel], pIS->ppnHiRanges[nCurModel][m_nLastRadioSel]);
		}
	}

  
	if(!_IsUndoAvail())
		FireOnChangeUndoState();
}

BOOL CColorManAXCtrl::IsUndoAvail() 
{
	// TODO: Add your dispatch handler code here
	return _IsUndoAvail();
}

int CColorManAXCtrl::_AddIntervalsSet()
{
	IntervalsSet* pIS = new IntervalsSet;
	int numModels = m_arrCCInfo.GetSize();
	ASSERT(numModels > 0);

	pIS->ppnLoRanges = new WORD*[numModels];
	pIS->ppnHiRanges = new WORD*[numModels];
	for(int i = 0; i < numModels; i++)
	{
		int nModelPanes = m_arrCCInfo[i]->nPanes;
		pIS->ppnHiRanges[i] = new WORD[nModelPanes];
		pIS->ppnLoRanges[i] = new WORD[nModelPanes];
		
	}

	m_arrIntervals.Add(pIS);
	//m_nCurIntervalsSet = m_arrIntervals.GetUpperBound();
	if(m_nCurIntervalsSet < 0) m_nCurIntervalsSet = 0;

	ResetIntervalsSet(m_arrIntervals.GetUpperBound());

	return m_nCurIntervalsSet;
}

int CColorManAXCtrl::_GetIntervalsSetsCount()
{
	return m_arrIntervals.GetSize();
}

void CColorManAXCtrl::_DeleteIntervalsSet(int nIS)
{
	if(m_arrIntervals.GetSize() <= 1) return; //don't let del last intervals set

	if(!(nIS >= 0 && nIS < _GetIntervalsSetsCount()))
	{
		return;
	}
	int numModels = m_arrCCInfo.GetSize();
	IntervalsSet* pIS = m_arrIntervals[nIS];
	for(int i = 0; i < numModels; i++)
	{
		delete pIS->ppnLoRanges[i];
		delete pIS->ppnHiRanges[i];
	}
	delete pIS->ppnLoRanges;
	delete pIS->ppnHiRanges;
	delete pIS;
	pIS->ppnLoRanges = 0;
	pIS->ppnHiRanges = 0;
	pIS = 0;
	m_arrIntervals.RemoveAt(nIS);

	m_nCurIntervalsSet = min(m_nCurIntervalsSet, m_arrIntervals.GetUpperBound());
	_SetPaneInterval(m_nLastRadioSel);
}

void CColorManAXCtrl::AddIntervalsSet() 
{
	// TODO: Add your dispatch handler code here
	_AddIntervalsSet();
}

void CColorManAXCtrl::RemoveIntervalsSet(short nIntervalsSet) 
{
	// TODO: Add your dispatch handler code here
	_DeleteIntervalsSet(nIntervalsSet);
}

void CColorManAXCtrl::SetActiveIntervalsSet(short nIntervalsSet) 
{
	// TODO: Add your dispatch handler code here
	if(nIntervalsSet >= 0 && nIntervalsSet < _GetIntervalsSetsCount())
	{
		//_SetPaneInterval(m_nLastRadioSel, -1, false);
		m_nCurIntervalsSet = nIntervalsSet;
		_SetPaneInterval(m_nLastRadioSel);

		FireOnChangeUndoState();
	}
}

HBRUSH CColorManAXCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = COleControl::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if (nCtlColor == CTLCOLOR_BTN)
    {
		/*int nCurModel = m_pcbColorModel->GetCurSel();
		int nPanes = m_arrCCInfo[nCurModel]->nPanes;
		if(pWnd->GetSafeHwnd() == m_pRadio1->GetSafeHwnd())
		{
			SetTextColor(pDC->GetSafeHdc(), m_arrCCInfo[nCurModel]->pdwPanesColors[0]);
		}
		else if(pWnd->GetSafeHwnd() == m_pRadio2->GetSafeHwnd())
		{
			SetTextColor(pDC->GetSafeHdc(), m_arrCCInfo[nCurModel]->pdwPanesColors[1]);
		}
		else if(pWnd->GetSafeHwnd() == m_pRadio3->GetSafeHwnd())
		{
			SetTextColor(pDC->GetSafeHdc(), m_arrCCInfo[nCurModel]->pdwPanesColors[2]);
		}
		*/
    }
      
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

long CColorManAXCtrl::GetHiRangeOfPane(short nClass, short nPane) 
{
	// TODO: Add your dispatch handler code here
	long nRange = 0;
	if(nClass >= 0 && nClass < m_arrIntervals.GetSize())
	{
		IntervalsSet* pIS = m_arrIntervals[nClass];
		if(nPane >= 0 && nPane < m_arrCCInfo[m_nLastCBSel]->nPanes)
			nRange = pIS->ppnHiRanges[m_nLastCBSel][nPane];
	}
	return nRange;
}

long CColorManAXCtrl::GetLoRangeOfPane(short nClass,  short nPane) 
{
	// TODO: Add your dispatch handler code here
	long nRange = 0;
	if(nClass >= 0 && nClass < m_arrIntervals.GetSize())
	{
		IntervalsSet* pIS = m_arrIntervals[nClass];
		if(nPane >= 0 && nPane < m_arrCCInfo[m_nLastCBSel]->nPanes)
			nRange = pIS->ppnLoRanges[m_nLastCBSel][nPane];
	}
	return nRange;
}

long CColorManAXCtrl::GetPanesCount() 
{
	// TODO: Add your dispatch handler code here
	return m_arrCCInfo[m_nLastCBSel]->nPanes;
}

void CColorManAXCtrl::Fake(LPCTSTR szComment) 
{
	// TODO: Add your dispatch handler code here
	CString str(szComment);
}

void CColorManAXCtrl::_FireChangeWorkingImage()
{
	CString strObj = ::GetObjectName(m_sptrWorkingImage);
	m_pStaticWorkImage->SetWindowText(strObj);

	IDispatch* pdisp = 0;
	m_sptrWorkingImage->QueryInterface(IID_IDispatch, (void**)&pdisp);
	ASSERT(pdisp != 0);

	FireOnChangeWorkingImage(pdisp);
	
	if(pdisp)
		pdisp->Release();

	IUnknown* punkDoc = 0;
	m_sptrApp->GetActiveDocument(&punkDoc);
	::SetValue(punkDoc, "ExtractPhases", "WorkImage", strObj);
	if(punkDoc)
		punkDoc->Release();
}
