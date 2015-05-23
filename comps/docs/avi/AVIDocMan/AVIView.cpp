#include "StdAfx.h"
#include "resource.h"
#include "AVIView.h"



#define szTypeVideo "Video"

extern CString g_sAVIToOpen;


/////////////////////////////////////////////////////////////////////////////////////////
// CAVIView

IMPLEMENT_DYNCREATE(CAVIView, CCmdTarget)

CAVIView::CAVIView()
{
	EnableAutomation();
	EnableAggregation();
	_OleLockApp( this );
	m_bPauseThenOpened = FALSE;
	m_bImageDocKeyInited = FALSE;
	m_bScheduleNext = false;

	m_sName = c_szCAVIView;
	m_sUserName.LoadString( IDS_VIDEO_VIEW );
}

CAVIView::~CAVIView()
{
	_OleUnlockApp( this );
}

void CAVIView::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.
	if( GetSafeHwnd() )
		DestroyWindow();
	delete this;
}

BEGIN_MESSAGE_MAP(CAVIView, CViewBase)
	//{{AFX_MSG_MAP(CAVIView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CAVIView, CViewBase)
	//{{AFX_DISPATCH_MAP(CAVIView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IImageView to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {898FB84A-5C80-4DD6-8D79-F9F2907E5580}
static const IID IID_IAVIViewDisp = 
{ 0x898fb84a, 0x5c80, 0x4dd6, { 0x8d, 0x79, 0xf9, 0xf2, 0x90, 0x7e, 0x55, 0x80 } };


BEGIN_INTERFACE_MAP(CAVIView, CViewBase)
	INTERFACE_PART(CAVIView, IID_IAVIViewDisp, Dispatch)
	INTERFACE_PART(CAVIView, IID_IVideoView, VideoView)
//	INTERFACE_PART(CAVIView, IID_IPreviewSite, Preview)
END_INTERFACE_MAP()


// {A669C4B0-A86F-47CB-8513-9DCFF4CBBF86}
GUARD_IMPLEMENT_OLECREATE(CAVIView, "VideoDoc.VideoView",
0xa669c4b0, 0xa86f, 0x47cb, 0x85, 0x13, 0x9d, 0xcf, 0xf4, 0xcb, 0xbf, 0x86);

IMPLEMENT_UNKNOWN(CAVIView, VideoView)


HRESULT CAVIView::XVideoView::FirstFrame()
{
	_try_nested(CAVIView, VideoView, FirstFrame)
	{
		if (pThis->m_wndAM.m_hWnd)
			pThis->m_wndAM.SetCurrentPosition(0);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAVIView::XVideoView::IsFirstFrame(BOOL *pbFirstFrame)
{
	_try_nested(CAVIView, VideoView, IsFirstFrame)
	{
		double p = pThis->m_wndAM.GetCurrentPosition();
		*pbFirstFrame = p == 0;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAVIView::XVideoView::PreviousFrame()
{
	_try_nested(CAVIView, VideoView, PreviousFrame)
	{
		if (pThis->m_wndAM.m_hWnd)
		{
			double p = pThis->m_wndAM.GetCurrentPosition();
			if (p > 0)
				pThis->m_wndAM.SetCurrentPosition(p-1);
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAVIView::XVideoView::NextFrame()
{
	_try_nested(CAVIView, VideoView, NextFrame)
	{
		if (pThis->m_wndAM.m_hWnd)
		{
			long r = pThis->m_wndAM.GetReadyState();
			if (r != 4)
				pThis->m_bScheduleNext = true;
			else
			{
				double p = pThis->m_wndAM.GetCurrentPosition();
				unsigned l = pThis->m_wndAM.AMGetLength();
				if (p < l-1)
					pThis->m_wndAM.SetCurrentPosition((double)(p+1));
			}
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAVIView::XVideoView::LastFrame()
{
	_try_nested(CAVIView, VideoView, LastFrame)
	{
		if (pThis->m_wndAM.m_hWnd)
		{
			unsigned l = pThis->m_wndAM.AMGetLength();
			pThis->m_wndAM.SetCurrentPosition(l-1);
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAVIView::XVideoView::IsLastFrame(BOOL *pbLastFrame)
{
	_try_nested(CAVIView, VideoView, IsFirstFrame)
	{
		unsigned l = pThis->m_wndAM.AMGetLength();
		double p = pThis->m_wndAM.GetCurrentPosition();
		*pbLastFrame = p >= l-1;
		return S_OK;
	}
	_catch_nested;
}


HRESULT CAVIView::XVideoView::SetImageDocKey(GUID lDocKey)
{
	_try_nested(CAVIView, VideoView, SetImageDocKey)
	{
		pThis->m_bImageDocKeyInited = TRUE;
		pThis->m_lImageDocKey = lDocKey;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAVIView::XVideoView::GetImageDocKey(GUID *plDocKey, BOOL *pbDocKeyInited)
{
	_try_nested(CAVIView, VideoView, GetImageDocKey)
	{
		*pbDocKeyInited = pThis->m_bImageDocKeyInited;
		*plDocKey = pThis->m_lImageDocKey;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CAVIView::XVideoView::Capture(IUnknown *punkImage, bool bStepRight)
{
	_try_nested(CAVIView, VideoView, Capture)
	{
		if (pThis->m_wndAM.m_hWnd)
		{
			bool bSkip = false;
			if (pThis->m_AVIFileCap.IsAviFile())
			{
				double p = pThis->m_wndAM.GetCurrentPosition();
				if (pThis->m_AVIFileCap.Capture(int(p), punkImage))
					bSkip = true;
			}
			if (!bSkip)
			{
				long l = pThis->m_wndAM.GetCurrentState();
				if (l == 0)
				{
					pThis->m_wndAM.Run();
					pThis->m_wndAM.Pause();
				}
				else if (l == 2)
					pThis->m_wndAM.Pause();
				pThis->m_wndAM.Capture(punkImage);
				if (l == 0)
					pThis->m_wndAM.Run();
			}
			if (bStepRight)
			{
				double p = pThis->m_wndAM.GetCurrentPosition();
				unsigned l = pThis->m_wndAM.AMGetLength();
				if (p < l-1)
					pThis->m_wndAM.SetCurrentPosition((double)(p+1));
			}
		}
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
// CAVIView message handlers

BEGIN_EVENTSINK_MAP(CAVIView, CViewBase)
    //{{AFX_EVENTSINK_MAP(CAVIView)
	ON_EVENT(CAVIView, ID_AMCONTROL1, 50 /* OpenComplete */, OnOpenCompleteActivemoviecontrol1, VTS_NONE)
	ON_EVENT(CAVIView, ID_AMCONTROL1, 2 /* PositionChange */, OnPositionChangeActivemoviecontrol1, VTS_R8 VTS_R8)
	ON_EVENT(CAVIView, ID_AMCONTROL1, 1 /* StateChange */, OnStateChangeActivemoviecontrol1, VTS_I4 VTS_I4)
	ON_EVENT(CAVIView, ID_AMCONTROL1, 51 /* DisplayModeChange */, OnDisplayModeChangeActivemoviecontrol1, VTS_NONE)
	ON_EVENT(CAVIView, ID_AMCONTROL1, 999 /* Error */, OnErrorActivemoviecontrol1, VTS_I2 VTS_BSTR VTS_BSTR VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void InvalidateAllChildren(HWND hwndParent)
{
	InvalidateRect(hwndParent, NULL, TRUE);
	HWND hwnd = ::GetWindow(hwndParent, GW_CHILD);
	while(hwnd)
	{
		InvalidateAllChildren(hwnd);
		hwnd = ::GetWindow(hwnd, GW_HWNDNEXT);
	}
}

int __GetVal(sptrINamedData &sptrND, LPCTSTR pszs, int nDefault)
{
	CString s(pszs);
	VARIANT var;
	::VariantInit(&var);
	sptrND->GetValue(s.AllocSysString(), &var);
	int nRet;
	if (var.vt == VT_I2)
		nRet = var.iVal;
	else if (var.vt == VT_I4)
		nRet = var.lVal;
	else
	{
		::VariantClear(&var);
		var.vt = VT_I2;
		var.iVal = nDefault;
		sptrND->SetValue(s.AllocSysString(), var);
		::VariantClear(&var);
		nRet = nDefault;
	}
	::VariantClear(&var);
	return nRet;
}

void CAVIView::OnOpenCompleteActivemoviecontrol1() 
{
//	SetTitle();
	CRect r;
	::GetClientRect(m_hWnd, &r);
	m_wndAM.SetWindowPos(NULL, r.left, r.top, r.Width(), r.Height(), /*SWP_NOACTIVATE|SWP_NOZORDER|*/SWP_NOCOPYBITS);
	SetTimer(1, 100, NULL);
}

void CAVIView::OnPositionChangeActivemoviecontrol1(double oldPosition, double newPosition) 
{
//	SetTitle();
}

void CAVIView::OnStateChangeActivemoviecontrol1(long oldState, long newState) 
{
}

void CAVIView::OnDisplayModeChangeActivemoviecontrol1() 
{
}

void CAVIView::OnErrorActivemoviecontrol1(short SCode, LPCTSTR Description, LPCTSTR Source, BOOL FAR* CancelDisplay) 
{
	if (m_bEnableErrorMsg)
	{
		CString s;
		s.Format(IDS_AMCTRL_ERROR_MESSAGE, (unsigned)SCode, Description, Source);
		int r = AfxMessageBox(s, MB_OKCANCEL|MB_ICONEXCLAMATION);
		if (r == IDCANCEL)
			m_bEnableErrorMsg = FALSE;
	}
}

void CAVIView::PostNcDestroy() 
{
	ClearList();
}

int CAVIView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CViewBase::OnCreate(lpCreateStruct) == -1)
		return -1;


	if (m_wndAM.Create(NULL, WS_CHILDWINDOW|WS_VISIBLE, CRect(0,0,0,0), this, ID_AMCONTROL1))
	{
		AttachActiveObjects();
		/*
		// Obtain file name
		CString s(szVideoObject);
		CString sFileName;
		IUnknown* punkObject = ::GetActiveObjectFromContext( GetControllingUnknown(), s );
		if (punkObject)
		{
			INamedObject2Ptr sptrNO(punkObject);
			punkObject->Release();
			BSTR bstr = NULL;
			sptrNO->GetUserName(&bstr);
			sFileName = CString(bstr);
			::SysFreeString(bstr);
		}
		// Setup file name for window and AVI file capture
		if (!sFileName.IsEmpty())
		{
			m_wndAM.SetFileName(sFileName);
			m_AVIFileCap.Open(sFileName, &m_VideoInfo);
		}
		*/
		// Read settings from shell.data
		sptrINamedData sptrND(GetAppUnknown());
		CString sSecName(_T("Video"));
		sptrND->SetupSection(sSecName.AllocSysString());
		BOOL bEnableContextMenu = __GetVal(sptrND, _T("EnableContextMenu"), FALSE);
		BOOL bShowDisplay = __GetVal(sptrND, _T("ShowDisplay"), TRUE);
		BOOL bShowControls = __GetVal(sptrND, _T("ShowControls"), TRUE);
		BOOL bShowTracker = __GetVal(sptrND, _T("ShowTracker"), TRUE);
		BOOL bEnableTracker = __GetVal(sptrND, _T("EnableTracker"), TRUE);
		BOOL bShowPositionControls = __GetVal(sptrND, _T("ShowPositionControls"), TRUE);
		BOOL bEnablePositionControls = __GetVal(sptrND, _T("EnablePositionControls"), TRUE);
		BOOL bShowSelectionControls = __GetVal(sptrND, _T("ShowSelectionControls"), TRUE);
		BOOL bEnableSelectionControls = __GetVal(sptrND, _T("EnableSelectionControls"), TRUE);
		BOOL bAllowHideDisplay = __GetVal(sptrND, _T("AllowHideDisplay"), FALSE);
		BOOL bAllowHideControls = __GetVal(sptrND, _T("AllowHideControls"), FALSE);
		int nBorderStyle = __GetVal(sptrND, _T("BorderStyle"), 1);
		int nMovieWindowSize = __GetVal(sptrND, _T("MovieWindowSize"), 0);
		int nForeColor = __GetVal(sptrND, _T("DisplayForeColor"), -1);
		int nBackColor = __GetVal(sptrND, _T("DisplayBackColor"), -1);
		BOOL bAutoStart = __GetVal(sptrND, _T("AutoStart"), FALSE);
		m_bPauseThenOpened = __GetVal(sptrND, _T("PauseThenOpened"), TRUE);
		m_bEnableErrorMsg = __GetVal(sptrND, _T("EnableErrorMessage"), TRUE);
		// Set up settings to window
		m_wndAM.SetEnableContextMenu(bEnableContextMenu);
		m_wndAM.SetDisplayMode(1);
		m_wndAM.SetShowDisplay(bShowDisplay);
		m_wndAM.SetShowControls(bShowControls);
		m_wndAM.SetShowTracker(bShowTracker);
		m_wndAM.SetEnableTracker(bEnableTracker);
		m_wndAM.SetShowPositionControls(bShowPositionControls);
		m_wndAM.SetEnablePositionControls(bEnablePositionControls);
		m_wndAM.SetShowSelectionControls(bShowSelectionControls);
		m_wndAM.SetEnableSelectionControls(bEnableSelectionControls);
		m_wndAM.SetAllowHideDisplay(bAllowHideDisplay);
		m_wndAM.SetAllowHideControls(bAllowHideControls);
		m_wndAM.SetAllowChangeDisplayMode(FALSE);
		if (nForeColor != -1)
			m_wndAM.SetDisplayForeColor(nForeColor);
		if (nBackColor != -1)
			m_wndAM.SetDisplayBackColor(nBackColor);
		m_wndAM.SetEnabled(TRUE);
		m_wndAM.SetBorderStyle(nBorderStyle);
		m_wndAM.SetMovieWindowSize(nMovieWindowSize);
		m_wndAM.SetAutoStart(bAutoStart);
	}

	return 0;
}

void CAVIView::AttachActiveObjects()
{
	if (!m_wndAM.GetControlUnknown())
		return;
	CString s(szVideoObject);
	CString sFileNameNew;
	IUnknown* punkObject = ::GetActiveObjectFromContext( GetControllingUnknown(), s );
	if (punkObject)
	{
		// Determine new file name.
		INamedObject2Ptr sptrNO(punkObject);
		BSTR bstr = NULL;
		sptrNO->GetUserName(&bstr);
		sFileNameNew = CString(bstr);
		::SysFreeString(bstr);
		// Determine old file name.
		CString sFileNameOld(m_wndAM.GetFileName());
		// Setup file name for window and AVI file capture.
		if (!sFileNameNew.IsEmpty() && sFileNameOld != sFileNameNew)
		{
			m_wndAM.SetFileName(sFileNameNew);
			m_AVIFileCap.Open(sFileNameNew, &m_VideoInfo);
			ClearList();
			punkObject->AddRef();
			m_listObjects.AddTail(punkObject);
		}
		punkObject->Release();
	}
}

void CAVIView::ClearList()
{
	POSITION pos = m_listObjects.GetHeadPosition();
	while(pos)
	{
		IUnknown *punk = m_listObjects.GetNext(pos);
		punk->Release();
	}
	m_listObjects.RemoveAll();
}

BOOL CAVIView::PreCreateWindow(CREATESTRUCT& cs) 
{
	//cs.style ^= CS_HREDRAW|CS_VREDRAW;
	return CViewBase::PreCreateWindow(cs);
}

DWORD CAVIView::GetMatchType( const char *szType )
{
//	if( !strcmp( szType, szTypeVideo ) )
	if( !strcmp( szType, szVideoObject ) )
		return mvFull;
	else
		return mvNone;
}

void CAVIView::OnSize(UINT nType, int cx, int cy) 
{
	CViewBase::OnSize(nType, cx, cy);
	CRect r;
	::GetClientRect(m_hWnd, &r);
	m_wndAM.SetWindowPos(NULL, r.left, r.top, r.Width(), r.Height(), /*SWP_NOACTIVATE|SWP_NOZORDER|*/SWP_NOCOPYBITS);
}

BOOL CAVIView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CAVIView::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent == 1 && m_wndAM.m_hWnd)
	{
		InvalidateAllChildren(m_wndAM.m_hWnd);
		KillTimer(1);
		if (m_bPauseThenOpened)
		{
			m_wndAM.Run();
			m_wndAM.Pause();
		}
		if (m_bScheduleNext)
		{
			double p = m_wndAM.GetCurrentPosition();
			unsigned l = m_wndAM.AMGetLength();
			if (p < l-1)
				m_wndAM.SetCurrentPosition((double)(p+1));
			m_bScheduleNext = false;
		}
	}
	CViewBase::OnTimer(nIDEvent);
}


POSITION CAVIView::GetFisrtVisibleObjectPosition()
{
	return m_listObjects.GetHeadPosition();
}

IUnknown *CAVIView::GetNextVisibleObject( POSITION &rPos )
{
	IUnknown *punk = m_listObjects.GetNext(rPos);
	punk->AddRef();
	return punk;
}

