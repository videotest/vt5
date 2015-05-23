#pragma once

#include "BaseDlg.h"
#include "PreviewStatic.h"
#include "HistoDlg.h"
#include "Reposition.h"
#include "input.h"
#include "VideoHook.h"

struct CPreviewIds
{
	CHistoIds m_HistoIds;
	int m_idStaticPeriod,m_idImage;
	int m_idFirstButton,m_idLastButton;
	int m_idStaticVideoStage, m_idProgressVideoStage;
	int m_idHistoCheckBox;
	int m_idFPS,m_idPeriod;
};

struct COptionsPageInfo
{
	int idCmd;
	int idDialog;
};

class CPreviewPixDlg : public CBaseDlg, public IInputPreviewSite, public CVideoHookImpl,
	public CRepositionManager, public CDelayedRepaintHelper
{
	bool m_bInitOk;
	bool m_bPreview;
	bool m_bVideo,m_bCaptureVideo;
	int m_nDlgSize;
	DWORD m_dwFlags;
	CPreviewIds m_PreviewIds;
	CBaseDlg *m_pDialog;
	void AddDialog(CBaseDlg *p, int idd);
	void SetChoice(int id);
	void InitDlgSize();
	int  m_idd;
	bool CanBeZoomed();

	CArray<COptionsPageInfo,COptionsPageInfo&> m_arrPages;
public:
	CPreviewPixDlg(int idd, CPreviewIds &PreviewIds, IUnknown *punk, int nDevice, DWORD dwFlags, CWnd* pParent = NULL);
	~CPreviewPixDlg(void);

	enum PreviewDialogFlags
	{
		RightCtrls = 1,
		ForAvi = 2,
		ForSettings = 4,
	};
	CStatic	m_StaticPeriod;
	CPreviewStatic	m_Image;
	CHistoDlg m_HistoDlg;

	void ReserveSpaceForSubdialog(int id);

	int GetDialogType() {return DriverDialog_PreviewDialog;}
	void AttachChildDialogToButton(int idButton, int idChildDlgRes);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	virtual void OnLayoutChanged();
	virtual void OnInitChild(CBaseDlg *pChild);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual void OnCancel();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDelayedRepaint(WPARAM wParam, LPARAM lParam);

	bool IsOwnControl(HWND hwnd, int id);

	// For IInputPreviewSite
	com_call Invalidate();
	com_call OnChangeSize();
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj) {return S_FALSE;}
	STDMETHOD_(ULONG,AddRef)() {return 0;}
	STDMETHOD_(ULONG,Release)() {return 0;}

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
