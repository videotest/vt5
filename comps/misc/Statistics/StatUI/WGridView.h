// WGridView.h : interface of the CWGridView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
namespace ViewSpace{

struct CLVItem:LVITEM
{
	struct Ex{
		COLORREF crBkColor,crForeColor;
	}ex;
	CString sText;
	CLVItem(){
		LVITEM lvi={0};
		LVITEM& lv=*this;
		lv=lvi;
	}
	void SetForeColor(const COLORREF ForeColor){
		ex.crForeColor=ForeColor;
	}
	void SetBkColor(const COLORREF BkColor){
		ex.crBkColor=BkColor;
	}
	const COLORREF GetForeColor(){
		return ex.crForeColor;
	}
	const COLORREF GetBkColor(){
		return ex.crBkColor;
	}
};

class CHdrWnd:public CWindowImpl<CHdrWnd>
{
//		MSG_WM_PAINT(OnPaint)
	BEGIN_MSG_MAP(CHdrWnd)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	END_MSG_MAP()
	LRESULT OnPaint(HDC hdc);
public:
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		SetMsgHandled(FALSE);
		return 0;
	}
};

#define WM_NOTYFYREFLECT	(WM_USER+1002)

class CWGridView : public CWindowImpl<CWGridView, CListViewCtrl>
								 , public CCustomDraw<CWGridView>
{
	typedef CWindowImpl<CWGridView, CListViewCtrl> CMyWin;
	bool m_bPaint;
	BOOL ComputeLastColWidth(int iItem);
	BOOL SetEmptyColWidth(int& colSwap);
	HFONT	m_hfHeaderFont;
public:
	DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

	BEGIN_MSG_MAP(CWGridView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_HSCROLL(OnHScroll)
		MSG_WM_VSCROLL(OnVScroll)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
		MSG_WM_SIZE(OnSize)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
		NOTIFY_HANDLER_EX(0, HDN_ITEMCHANGINGA, OnItemChanging) 
		NOTIFY_HANDLER_EX(0, HDN_ITEMCHANGINGW, OnItemChanging) 
		NOTIFY_HANDLER_EX(0, HDN_ITEMCHANGEDA, OnItemChanged) 
		NOTIFY_HANDLER_EX(0, HDN_ITEMCHANGEDW, OnItemChanged) 
		MESSAGE_HANDLER(SBM_SETSCROLLINFO, OnSetScrollInfo)
		CHAIN_MSG_MAP_ALT(CCustomDraw<CWGridView>, 1)
    DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
//		NOTIFY_HANDLER_EX(0, HDN_TRACKA, OnTrack) 
//		NOTIFY_HANDLER_EX(0, HDN_TRACKW, OnTrack) 
//		NOTIFY_HANDLER_EX(0, HDN_ENDTRACKA, OnTrack) 
//		NOTIFY_HANDLER_EX(0, HDN_ENDTRACKW, OnTrack) 
//    REFLECT_NOTIFICATIONS()

	CHdrWnd m_hdr;
	LRESULT OnCreate(LPCREATESTRUCT pCS);
	LRESULT OnPaint(HDC hdc);
	LRESULT OnSize(UINT resizeType, CSize size);
	LRESULT OnWindowPosChanging(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnItemChanging(LPNMHDR pnmh);
	LRESULT OnItemChanged(LPNMHDR pnmh);
	LRESULT OnTrack(LPNMHDR pnmh);
	LRESULT OnNotify(int idCtrl, LPNMHDR pnmh);
	LRESULT OnHScroll(int sCode, short scrollPos, HWND hWnd);
	LRESULT OnVScroll(int sCode, short scrollPos, HWND hWnd);
	LRESULT OnSetScrollInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(HDC hDC)
	{
//		SetMsgHandled(FALSE));
		return 0;
	};
	LRESULT OnMouseWheel(UINT fwKeys, short zDelta, CPoint pnt);
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	CWGridView():m_bPaint(true), m_colEmpty(-1)
		, bBlockChanged(false)
		, m_wEmpty(0)
		, m_bSetColumnWidth(false)
	{}
	BOOL PreTranslateMessage(MSG* pMsg);
	BOOL Header_SetFont( LOGFONT *plf );
	int GetColumnCount();
	void DeleteAllColumns(void);

	BOOL EnsureVisible(int nItem, BOOL bPartialOK);

	BOOL BeginUpdate(){
		return LockWindowUpdate(TRUE);
	}
	void EndUpdate(){
		LockWindowUpdate(FALSE);
//		m_bPaint=true;
//		InvalidateRect(NULL,FALSE);
//		UpdateWindow();
	}

	BOOL Scroll(int dx, int dy);

public:

  DWORD OnPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
  {
      // Запрашиваем уведомления NM_CUSTOMDRAW для каждого элемента списка.
      return CDRF_NOTIFYITEMDRAW;
  }

  DWORD OnItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
  {
      // Нам нужны поля, специфичные для ListView.
      LPNMLVCUSTOMDRAW pLVCD = (LPNMLVCUSTOMDRAW)lpNMCustomDraw;

      if((lpNMCustomDraw->dwItemSpec & 0x01) != 0)
      {
          // Для нечётных элементов: рисуем белым по чёрному.
          pLVCD->clrText = RGB(255,255,255);
          pLVCD->clrTextBk = RGB(0,0,0);
      }
      else
      {
          // Для чётных элементов: рисуем красным по серому.
          pLVCD->clrText = RGB(255,0,0);
          pLVCD->clrTextBk = RGB(200,200,200);
      }

      return CDRF_NEWFONT;
  }

	int StateScrollInfo(int nType);

private:
	int m_colEmpty;
public:
	int InsertColumn(int iCol, LPLVCOLUMN pLVColumn);
	bool bBlockChanged;
private:
	int m_wEmpty;
	int m_colLastSwap;
public:
	BOOL SetItemState(int nItem, UINT nState, UINT nStateMaske);
	BOOL SetColumnWidth(int nCol, int cx);
private:
	bool m_bSetColumnWidth;
};
}