#if !defined(AFX_PREVIEWDLG_H__6B270EC9_8489_432A_851D_0FF8A2FC1868__INCLUDED_)
#define AFX_PREVIEWDLG_H__6B270EC9_8489_432A_851D_0FF8A2FC1868__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreviewDlg.h : header file
//

#include "input.h"
#include "BaumerStatic.h"
#include "SettingsSheet.h"

/////////////////////////////////////////////////////////////////////////////
// CPreviewDlg dialog

class CPreviewDlg : public CDialog, public IInputPreviewSite, public ISettingsSite
{
	CRect m_rcImage,m_rcOK,m_rcCancel,m_rcSettings,m_rcRTime,m_rcTotal,m_rcHelp;
	CSize m_szButton;
	int m_nGap;
	bool m_bInited;
	bool m_bBoardOK;
	bool m_bUseOverlay;
	bool m_bUseStretch;
	bool m_bOvrActive;
	BOOL m_bNaturalSize;
	CSize m_szNaturalSize;
	IInputWndOverlay *m_pOverlay;
	IInputPreview *m_pPreview; 
//	IRedrawHook *m_pRedrawHook;
	void InitLayout();
	CSize CalcSizesByCxWindow(int cxWindow);
	CSize CalcSizesByCyWindow(int cyWindow);
	void RecalcLayoutByImageSize(int cx, int cy);
	void RecalcLayoutByClientSize(int cxTotal, int cyTotal, int cxImage, int cyImage);
	void SetLayout(bool bSetTotal);
	void SetLayoutByClientSize(int cx, int cy);
	void SetLayoutByClientSize();
	void SetLayoutByImageSize();
	void ClosePreview();
public:
	bool m_bOvrMode;

	
// Construction
	CPreviewDlg(IUnknown *punk, CWnd* pParent = NULL);   // standard constructor
	~CPreviewDlg();

// Dialog Data
	//{{AFX_DATA(CPreviewDlg)
	enum { IDD = IDD_BAUMER };
	CButton	m_Help;
	CStatic	m_Time;
	CBaumerStatic	m_Image;
	CButton	m_Cancel;
	CButton	m_Settings;
	CButton	m_OK;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreviewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPreviewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSettings();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	afx_msg void OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg LRESULT OnUserMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	DECLARE_MESSAGE_MAP()

	virtual void OnChangeOvrMode(bool bOvrMode);
	virtual void OnChangeNaturalSize(bool bNaturalSize);
	com_call Invalidate();
	com_call OnChangeSize();
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj) {return S_FALSE;}
	STDMETHOD_(ULONG,AddRef)() {return 0;}
	STDMETHOD_(ULONG,Release)() {return 0;}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREVIEWDLG_H__6B270EC9_8489_432A_851D_0FF8A2FC1868__INCLUDED_)
