#pragma once
#include "\vt5\awin\win_base.h"
#include "\vt5\awin\misc_list.h"

#define WM_DISABLEMODAL     0x036C  // lResult = 0, disable during modal state

#ifdef _UNICODE
class CToolInfo : public tagTOOLINFOW
#else
class CToolInfo : public tagTOOLINFOA
#endif
{
public:
	TCHAR szText[256];
};

typedef struct tagAFX_OLDTOOLINFO {
	UINT cbSize;
	UINT uFlags;
	HWND hwnd;
	UINT uId;
	RECT rect;
	HINSTANCE hinst;
	LPTSTR lpszText;
} AFX_OLDTOOLINFO;

class CToolTip32 : public win_impl
{
	//CToolBarCtrl
	_list_t<int> m_listTips;
	BOOL m_bIsActive;
public:
	BOOL IsActive() { return m_bIsActive; }

	CToolTip32();
	~CToolTip32();

	bool CreateTooltip( HWND hParent, DWORD style = 0);
// Attributes
	void GetText(LPSTR, HWND, UINT_PTR nIDTool = 0);
	BOOL GetToolInfo(::CToolInfo& ToolInfo, HWND, UINT_PTR nIDTool = 0);
	void SetToolRect(HWND, UINT_PTR nIDTool, LPCRECT lpRect);

	BOOL AddTool(HWND, LPCTSTR lpszText = LPSTR_TEXTCALLBACK,
		LPCRECT lpRectTool = NULL, UINT_PTR nIDTool = 0);

	void DeleteAll( HWND );


	void DelTool(HWND, UINT_PTR nIDTool = 0);

	BOOL HitTest(HWND, POINT pt, LPTOOLINFO lpToolInfo);
	void UpdateTipText(LPCTSTR lpszText, HWND, UINT_PTR nIDTool = 0);

	void FillInToolInfo(TOOLINFO& ti, HWND, UINT_PTR nIDTool);

	inline void SetDelayTime(UINT nDelay)
		{ ::SendMessage( handle(), TTM_SETDELAYTIME, 0, nDelay); }
	inline int GetDelayTime(DWORD dwDuration)
		{ return (int) ::SendMessage( handle(), TTM_GETDELAYTIME, dwDuration, 0L); }
	inline void GetMargin(LPRECT lprc)
		{ ::SendMessage( handle(), TTM_GETMARGIN, 0, (LPARAM)lprc); }
	inline int GetMaxTipWidth()
		{ return (int) ::SendMessage( handle(), TTM_GETMAXTIPWIDTH, 0, 0L); }
	inline COLORREF GetTipBkColor()
		{ return (COLORREF) ::SendMessage( handle(), TTM_GETTIPBKCOLOR, 0, 0L); }
	inline COLORREF GetTipTextColor()
		{ return (COLORREF) ::SendMessage( handle(), TTM_GETTIPTEXTCOLOR, 0, 0L); }
	inline void Pop()
		{ ::SendMessage( handle(), TTM_POP, 0, 0L); }
	inline void SetDelayTime(DWORD dwDuration, int iTime)
		{ ::SendMessage( handle(), TTM_SETDELAYTIME, dwDuration, MAKELPARAM(iTime, 0)); }
	inline void SetMargin(LPRECT lprc)
		{ ::SendMessage( handle(), TTM_SETMARGIN, 0, (LPARAM)lprc); }
	inline int SetMaxTipWidth(int iWidth)
		{ return (int) ::SendMessage( handle(), TTM_SETMAXTIPWIDTH, 0, iWidth); }
	inline void SetTipBkColor(COLORREF clr)
		{ ::SendMessage( handle(), TTM_SETTIPBKCOLOR, clr, 0L); }
	inline void SetTipTextColor(COLORREF clr)
		{ ::SendMessage( handle(), TTM_SETTIPTEXTCOLOR, clr, 0L); }
	inline void Update()
		{ ::SendMessage( handle(), TTM_UPDATE, 0, 0L); }

	#if _WIN32_IE >= 0x0500
	inline BOOL AdjustRect(LPRECT lprc, BOOL bLarger /*= TRUE*/)
		{ return (BOOL)::SendMessage( handle(), TTM_ADJUSTRECT, bLarger, (LPARAM)lprc); }
	inline BOOL SetTitle(UINT uIcon, LPCTSTR lpstrTitle)
		{ return (BOOL)::SendMessage( handle(), TTM_SETTITLE, uIcon, (LPARAM)lpstrTitle); }
	#endif

	inline void Activate(BOOL bActivate)
		{ ::SendMessage( handle(), TTM_ACTIVATE, bActivate, 0L); }
	inline void SetToolInfo(LPTOOLINFO lpToolInfo)
		{ ::SendMessage( handle(), TTM_SETTOOLINFO, 0, (LPARAM)lpToolInfo); }
	inline void RelayEvent(LPMSG lpMsg)
		{ ::SendMessage( handle(), TTM_RELAYEVENT, 0, (LPARAM)lpMsg); }
	inline int GetToolCount()
		{ return (int) ::SendMessage( handle(), TTM_GETTOOLCOUNT, 0, 0L); }

	LRESULT OnDisableModal(WPARAM, LPARAM);
	LRESULT OnWindowFromPoint(WPARAM, LPARAM);
	void OnEnable(BOOL bEnable);

	long	handle_message( UINT m, WPARAM w, LPARAM l )
	{
		if( m == WM_DISABLEMODAL )
			return OnDisableModal( w, l );
		if( m == TTM_WINDOWFROMPOINT )
			return OnWindowFromPoint( w, l );
		if( m == WM_ENABLE )
		{
			OnEnable( (BOOL)w );
			return 0L;	  
		}
		if( m == TTM_ACTIVATE )
		{
			m_bIsActive = ( (BOOL)w );
			return 0L;
		}

		return __super::handle_message( m, w, l );
	}
};