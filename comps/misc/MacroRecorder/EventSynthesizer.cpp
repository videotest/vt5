#include "stdafx.h"
#include "resource.h"
#include "EventSynthesizer.h"
#include "macro_utils.h"
#include <math.h>
#include <Mmsystem.h>
#include "misc_utils.h"
#include "core5.h"

#include "MacroHelper.h"

static BOOL s_bEditComplete = FALSE;

/*class CTransparentWindow
{
public:
	HWND m_hWnd;
	virtual LRESULT OnWndProc(UINT msg, WPARAM wParam, LPARAM lParam)
	{
	}

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_NCCREATE)
		{
			LPCREATESTRUCT lpCreateStr = (LPCREATESTRUCT)lParam;
		}
		CTransparentWindow *p = GetWindowLong(hWnd, 0);
		return p->OnWndProc(msg, wParam, lParam);
	}
};*/

class CTransparentWindow : public win_impl
{
public:
	_rect m_rc,m_rcOrig;
	BOOL m_bEditComplete;
	int m_nCompleteCode;
	int m_nTimer;
	_point m_ptStartDrag;
	_rect  m_rcStartDrag;
	bool m_bDrag;


	CTransparentWindow(RECT rc)
	{
		memcpy(&m_rc,&rc,sizeof(rc));
		m_rcOrig = m_rc;
		m_bEditComplete = FALSE;
		m_nCompleteCode = -1;
		m_nTimer = 0;
		m_bDrag = false;
	}
protected:
	void RedrawRect()
	{
		HDC hDC = GetDC(m_hwnd);
		HPEN hPen = ::CreatePen(PS_SOLID,5,RGB(127,127,127));
//		HPEN hPen2 = ::CreatePen(PS_SOLID,1,RGB(127,127,127));
		HPEN hPenOld = (HPEN)::SelectObject(hDC, hPen);
		int nRop = SetROP2(hDC, R2_XORPEN);
		::MoveToEx(hDC,m_rc.left,m_rc.top,NULL);
		::LineTo(hDC, m_rc.right, m_rc.top);
		::LineTo(hDC, m_rc.right, m_rc.bottom);
		::LineTo(hDC, m_rc.left, m_rc.bottom);
		::LineTo(hDC, m_rc.left, m_rc.top);
/*		::SelectObject(hDC, hPen2);
		::MoveToEx(hDC,m_rc.left+1,m_rc.top+1,NULL);
		::LineTo(hDC, m_rc.right-1, m_rc.top+1);
		::LineTo(hDC, m_rc.right-1, m_rc.bottom-1);
		::LineTo(hDC, m_rc.left+1, m_rc.bottom-1);
		::LineTo(hDC, m_rc.left+1, m_rc.top+1);*/
		SetROP2(hDC, nRop);
		SelectObject(hDC, hPenOld);
//		DeleteObject(hPen2);
		DeleteObject(hPen);
		ReleaseDC(m_hwnd, hDC);
	}

	void SetNewRect(_rect rc)
	{
		RedrawRect();
		m_rc = rc;
		RedrawRect();
	}

	void MoveRect(int dx, int dy)
	{
		SetNewRect(_rect(m_rc.left+dx,m_rc.top+dy,m_rc.right+dx,m_rc.bottom+dy));
	}

protected:
	virtual long on_create( CREATESTRUCT *pcs )
	{
		long l = __super::on_create(pcs);
		m_nTimer = SetTimer(handle(), 1, 100, NULL);
		SetCapture(handle());
		return l;
	}
	virtual long on_mousemove(const _point &point)
	{
		if (m_bDrag)
		{
			RedrawRect();
			int dx = point.x-m_ptStartDrag.x;
			int dy = point.y-m_ptStartDrag.y;
			m_rc.left = m_rcStartDrag.left+dx;
			m_rc.right = m_rcStartDrag.right+dx;
			m_rc.top = m_rcStartDrag.top+dy;
			m_rc.bottom = m_rcStartDrag.bottom+dy;
			RedrawRect();
		}
		return 0;
	}
	virtual long on_lbuttondown( const _point &point )
	{
		m_ptStartDrag = point;
		m_rcStartDrag = m_rc;
		m_bDrag = true;
		return 0;
	}
	virtual long on_lbuttonup( const _point &point )
	{
		m_bDrag = false;
		return 0;
	}
	virtual long on_rbuttonup( const _point &point )
	{
		m_bEditComplete = true;
		m_nCompleteCode = 0;
		return 0;
	}
	virtual long on_keydown( long nVirtKey )
	{
		switch (nVirtKey)
		{
		case VK_ESCAPE:
			m_bEditComplete = true;
			m_nCompleteCode = 0;
			break;
		case VK_RETURN:
			m_bEditComplete = true;
			m_nCompleteCode = 1;
			break;
		case VK_UP:
			MoveRect(0,-1);
			break;
		case VK_DOWN:
			MoveRect(0,1);
			break;
		case VK_LEFT:
			MoveRect(-1,0);
			break;
		case VK_RIGHT:
			MoveRect(1,0);
			break;
		case VK_HOME:
		case VK_BACK:
			SetNewRect(m_rcOrig);
			break;
		}
		return __super::on_keydown(nVirtKey);
	}
	virtual long on_erasebkgnd(HDC)
	{
		return 1;
	}
	virtual long on_timer(ulong lEvent)
	{
		RedrawRect();
		KillTimer(handle(), m_nTimer);
		return 0;
	}
	virtual long on_activate(unsigned code, HWND hwnd)
	{
		if (code == WA_INACTIVE)
		{
			DWORD dwProcessId;
			GetWindowThreadProcessId(hwnd,&dwProcessId);
			DWORD dwCurrentProcessId = (DWORD)GetCurrentProcessId();
			if (dwProcessId != dwCurrentProcessId)
			{
				m_bEditComplete = true;
				if (m_nCompleteCode == -1) m_nCompleteCode = 0;
			}
		}
		return __super::on_activate(code, hwnd);
	}
};

bool DoEditRectangle(CEditedRectangleInfo &RInfo)
{
/*	static bool bClassRegistered = false;
	if (!bClassRegistered)
	{
		WNDCLASS WndClass;
		memset(&WndClass, 0, sizeof(WndClass));
		WndClass.hInstance = app::get_instance()->handle();
		WndClass.lpfnWndProc = EditRectanglesProc;
		WndClass.lpszClassName = "EditRectangleClass";
		WndClass.cbWndExtra = sizeof(CTransparentWindow*);
		RegisterClass(&WndClass);
		bClassRegistered = true;
	}*/
	int cxFullScreen = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int cyFullScreen = ::GetSystemMetrics(SM_CYFULLSCREEN);
	s_bEditComplete = FALSE;
	CTransparentWindow WndRects(RInfo.rcOld);
	IApplicationPtr sptrApp(GetAppUnknown());
	HWND hWndParent = NULL;
	sptrApp->GetMainWindowHandle(&hWndParent);
	if (WndRects.create_ex(WS_VISIBLE|WS_POPUP, _rect(0,0,cxFullScreen,cyFullScreen), 0,
		hWndParent, 0, 0, WS_EX_TRANSPARENT/*|WS_EX_TOPMOST*/))
//	HWND hWnd = CreateWindowEx(WS_EX_TOPMOST, "EditRectangleClass", NULL, WS_VISIBLE|WS_POPUP,
//		0, 0, cxFullScreen, cyFullScreen, NULL, NULL, app::get_instance()->handle(), NULL);
//	if (hWnd != NULL)
	{
		MSG msg;
		BOOL bRet;
		while (!WndRects.m_bEditComplete && (bRet = GetMessage(&msg, 0/*WndRects.handle()*/, 0, 0)) != 0)
		{ 
			if (bRet == -1)
				break;
			else
			{
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			}
		}
		DestroyWindow(WndRects.handle());
		if (WndRects.m_nCompleteCode == 1)
		{
			RInfo.rcNew = WndRects.m_rc;
			return true;
		}

	}
	return false;
}

bool CompareClassNames(const char *pszClsName1, const char *pszClsName2)
{
	if (_tcsncmp(pszClsName1,_T("Afx:"),_tcslen(_T("Afx:"))) == 0 &&
		_tcsncmp(pszClsName2,_T("Afx:"),_tcslen(_T("Afx:"))) == 0 )
		return true;
	if (_tcsncmp(pszClsName1,_T("BCGToolbar:"),_tcslen(_T("BCGToolbar:"))) == 0 &&
		_tcsncmp(pszClsName2,_T("BCGToolbar:"),_tcslen(_T("BCGToolbar:"))) == 0)
		return true;
	int r = _tcscmp(pszClsName1, pszClsName2);
	return r==0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// Class CEventSynthesizer
//
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
CEventSynthesizer::CEventSynthesizer(void)
{
	m_dwdelay = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
CEventSynthesizer::~CEventSynthesizer(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL	CEventSynthesizer::FireEvent( WorkConditions  *pcurr_cond, BOOL bplay_sound )
{
	if( bplay_sound )
	{
		ULONG ulresource_sound = GetSoundResourceID( );
		if( ulresource_sound != (ULONG)(-1) )
			::PlaySound( MAKEINTRESOURCE(ulresource_sound), module::hinst(), SND_RESOURCE | SND_ASYNC );
	}

	return DoFireEvent( pcurr_cond );
}

///////////////////////////////////////////////////////////////////////////////////////////////
// return: 0 - no, 1 - yes, 2 - ltimeout has elapsed
int	CEventSynthesizer::CanFireEvent( WorkConditions  *pcurr_cond, DWORD dwStartTime, long ltimeout )
{
	if( !pcurr_cond )
		return 0;

	if( DoCanFireEvent( *pcurr_cond ) )
		return 1;

	DWORD dwcurr_time = __GetTickCount( );
	if( (ULONG)(dwcurr_time - dwStartTime) >= ULONG(ltimeout) )			  
		return 2;

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// Class CMessageEventSynthesizer
//
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
CMessageEventSynthesizer::CMessageEventSynthesizer(void)
{
	m_message = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
CMessageEventSynthesizer::~CMessageEventSynthesizer(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CMessageEventSynthesizer::CanFireEvent( WorkConditions  *pcurr_cond, DWORD dwStartTime, long ltimeout )
{
	if( !pcurr_cond )
		return 0;

	// doing delay
	if( (ULONG)(__GetTickCount() - dwStartTime) >= (ULONG)(m_dwdelay) )                        
		return __super::CanFireEvent( pcurr_cond, dwStartTime + m_dwdelay, ltimeout );    
	else 
		return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// Class CMouseSynthesizer
//
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
CMouseSynthesizer::CMouseSynthesizer(void)
{
	m_point.x = m_point.y = 0;
    m_message = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
CMouseSynthesizer::~CMouseSynthesizer(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////
void CMouseSynthesizer::SetPoint( POINT pt )
{
	m_point = pt;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void CMouseSynthesizer::SetPoint( int nX, int nY )
{
	POINT pt = {nX, nY};
    SetPoint( pt );
}

///////////////////////////////////////////////////////////////////////////////////////////////
POINT CMouseSynthesizer::GetPoint( )
{
	return m_point;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CMouseSynthesizer::FireEvent( WorkConditions  *pcurr_cond, BOOL bplay_sound )
{
	if( is_dblclick( ) )
	{
		// [vanek] BT2000:4010 не надо имиитировать нажатие кнопок мыши на клавиатуре! - 04.10.2004
		UINT	uimouseup = 0,
				uimousedown = 0;

		if( m_message == WM_LBUTTONDBLCLK )
		{
			uimouseup = WM_LBUTTONUP;
			uimousedown = WM_LBUTTONDOWN;			
		}
		else if( m_message == WM_RBUTTONDBLCLK )
		{
        	uimouseup = WM_RBUTTONUP;
			uimousedown = WM_RBUTTONDOWN;			
		}

		CMouseSynthesizer mouse_synth_down;
        CMouseSynthesizer mouse_synth_up;

		// down
		mouse_synth_down.SetMessage( uimousedown );
		mouse_synth_down.SetDelay( 0 );
		mouse_synth_down.SetPoint( m_point );

		// up
		mouse_synth_up.SetMessage( uimouseup );
		mouse_synth_up.SetDelay( 0 );
		mouse_synth_up.SetPoint( m_point );

		UINT uidbl_click_time = 0;
		uidbl_click_time = ::GetDoubleClickTime( );
		
		// [vanek] : prepare double-click - 13.08.2004
		::Sleep( uidbl_click_time + 10 );

		// first click
		mouse_synth_down.FireEvent( pcurr_cond, bplay_sound );
        mouse_synth_up.FireEvent( pcurr_cond, bplay_sound );

		// [vanek] : sleeping between the first and second click of a double-click - 13.08.2004
        ::Sleep( uidbl_click_time / 2 );

		// [vanek]: restore point to click - 13.08.2004
		mouse_synth_down.SetPoint( m_point ); 
		mouse_synth_up.SetPoint( m_point );

		// second click
		mouse_synth_down.FireEvent( pcurr_cond, bplay_sound );
		mouse_synth_up.FireEvent( pcurr_cond, bplay_sound );

		return TRUE;
	}
	else
		return __super::FireEvent( pcurr_cond, bplay_sound );
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CMouseSynthesizer::DoCanFireEvent( WorkConditions curr_cond )
{
	if( m_message < WM_MOUSEFIRST || m_message > WM_MOUSELAST )
		return FALSE;

	HWND hwnd = 0;
	_point pt(m_point);
	pt.x += curr_cond.rcWnd.left;
	pt.y += curr_cond.rcWnd.top;
	hwnd = ::WindowFromPoint(pt);

	TCHAR _stClassName[MAX_PATH];
	::GetClassName( hwnd, _stClassName, sizeof( _stClassName ) );

	bool b = CompareClassNames(_stClassName, curr_cond.strWndClass);
	if (!b)
		OutputDebugString("CMouseSynthesizer::DoCanFireEvent failed\n");

	return b;
}

BOOL CMouseSynthesizer::DoFireEvent( WorkConditions  *pcurr_cond )
{
	DWORD	dwflags = MOUSEEVENTF_ABSOLUTE;
	BOOL	bcorrect_cursor_pos = FALSE;

	switch( m_message )
	{
	case WM_MOUSEMOVE:
		dwflags |= MOUSEEVENTF_MOVE;
		break;

	case WM_LBUTTONDOWN:
		dwflags |= MOUSEEVENTF_LEFTDOWN;
		bcorrect_cursor_pos = TRUE;
		break;

	case WM_LBUTTONUP:
		dwflags |= MOUSEEVENTF_LEFTUP;
		break;

	case WM_RBUTTONDOWN:
		dwflags |= MOUSEEVENTF_RIGHTDOWN;
		bcorrect_cursor_pos = TRUE;
		break;

	case WM_RBUTTONUP:
		dwflags |= MOUSEEVENTF_RIGHTUP;
		break;

	case WM_MBUTTONDOWN:
		dwflags |= MOUSEEVENTF_MIDDLEDOWN;
		bcorrect_cursor_pos = TRUE;
		break;

	case WM_MBUTTONUP:
		dwflags |= MOUSEEVENTF_MIDDLEUP;
		break;
	}

	m_point.x += pcurr_cond->rcWnd.left;
	m_point.y += pcurr_cond->rcWnd.top;
	POINT pt_norm = m_point;
	Normalize( &pt_norm );

	if( bcorrect_cursor_pos )
		CorrectCursorPos( m_point );

    ::mouse_event( dwflags, pt_norm.x, pt_norm.y, 0, 0); 
	return TRUE;
}

ULONG CMouseSynthesizer::GetSoundResourceID( )
{
    switch( m_message )
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		return IDR_MOUSE_DOWN;
		
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		return IDR_MOUSE_UP;

	default:
		return __super::GetSoundResourceID();
    }	    
}


///////////////////////////////////////////////////////////////////////////////////////////////
//
// Class CKeybdSynthesizer
//
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
CKeybdSynthesizer::CKeybdSynthesizer(void)
{
	m_lVk = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
CKeybdSynthesizer::~CKeybdSynthesizer(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////
void CKeybdSynthesizer::SetKey( long lVk, BOOL bExtended /*= FALSE*/)
{
    m_lVk = lVk;
	m_bextended_key = bExtended;
}

///////////////////////////////////////////////////////////////////////////////////////////////
long CKeybdSynthesizer::GetKey(void)
{
    return m_lVk;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CKeybdSynthesizer::IsExtendedKey( )
{
    return m_bextended_key;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CKeybdSynthesizer::DoCanFireEvent( WorkConditions curr_cond )
{
	if( m_message < WM_KEYFIRST || m_message > WM_KEYLAST )
		return FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CKeybdSynthesizer::DoFireEvent( WorkConditions  *pcurr_cond )
{
	DWORD dwFlags = 0;  
    BYTE byteScanCode = 0;

	if( m_message == WM_KEYUP || m_message == WM_SYSKEYUP )
		dwFlags |= KEYEVENTF_KEYUP;

	if( IsExtendedKey( ) )
	{
		dwFlags |= KEYEVENTF_EXTENDEDKEY;
		byteScanCode = ::MapVirtualKey( m_lVk, 0);
	}

	::keybd_event( (BYTE)(m_lVk), byteScanCode, dwFlags, 0 );
	return TRUE;
}

ULONG CKeybdSynthesizer::GetSoundResourceID( )
{
	if( m_lVk != VK_SHIFT && m_lVk != VK_MENU && m_lVk != VK_CONTROL )
	{
		switch( m_message )
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			return IDR_KEYBRD_DOWN;

		/*case WM_KEYUP:
		case WM_SYSKEYUP:
			return IDR_KEYBRD_UP;		*/
		}	
	}

	return __super::GetSoundResourceID();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// Class CConditionsSynthesizer
//
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
CConditionsSynthesizer::CConditionsSynthesizer(void)
{
	m_stcond.init( );
	m_pMacroHelper = 0;
	m_pScriptGenerator = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
CConditionsSynthesizer::~CConditionsSynthesizer(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////
void	CConditionsSynthesizer::SetConditions( WorkConditions stnew )
{
	m_stcond = stnew;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL	CConditionsSynthesizer::GetConditions( WorkConditions *pcond )
{
	if( !pcond )
		return FALSE;

	*pcond = m_stcond;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void	CConditionsSynthesizer::SetHelper( CMacroHelper	*pMH )
{
    if( !pMH  )
		return;

	m_pMacroHelper = pMH;
}

///////////////////////////////////////////////////////////////////////////////////////////////
CMacroHelper *CConditionsSynthesizer::GetHelper(void)
{
    return m_pMacroHelper;
}

void CConditionsSynthesizer::SetScriptGenerator(CCmdScriptGenerator *pScriptGenerator)
{
	m_pScriptGenerator = pScriptGenerator;
}


BOOL CConditionsSynthesizer::DoCanFireEvent( WorkConditions curr_cond )
{
	return (m_pMacroHelper != 0 && m_stcond.bGoodData);
}

BOOL CConditionsSynthesizer::DoFireEvent( WorkConditions  *pcurr_cond )
{
    if( !m_pMacroHelper || !m_stcond.bGoodData )
		return FALSE;

	m_pMacroHelper->SetCurrCond( m_stcond );
	if (m_pScriptGenerator)
	{
		char szBuff[256];
		wnsprintf(szBuff, 256, "MacroHelper.SetEventsWindow2 \"%s\",%d,%d,%d,%d,%d",
			(const char *)m_stcond.strWndClass, m_stcond.rcWnd.left, m_stcond.rcWnd.top,
			m_stcond.rcWnd.width(), m_stcond.rcWnd.height(), (long)m_stcond.hWndSaved);
		m_pScriptGenerator->m_bstrCmd = szBuff;
	}

	return TRUE;
}

