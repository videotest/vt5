// BaseDialog.h: interface for the CBaseDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__BaseDlg_H__)
#define __BaseDlg_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "input.h"
#include "SmartEdit.h"

class CWindowsGroup;
class CBaseDlg : public CDialog, public IDriverDialog, public IEditSite
{
	DECLARE_DYNAMIC(CBaseDlg);
	CBaseDlg *m_pBase;
	CTypedPtrArray<CObArray,CDrvControl*> m_arrCtrls;
	bool m_bInited;
	IDriverDialogSitePtr m_sptrDDS;
	int m_idd;
	bool m_bSubdlgControls; // This dialog processes WM_COMMAND from subdialog's controls

	bool TryInitSmartEdit(CWindowsGroup& Group, VALUEINFOEX2 *pvie);
	bool TryInitCombobox(CWindowsGroup& Group, VALUEINFOEX2 *pvie);
	bool TryInitListbox(CWindowsGroup& Group, VALUEINFOEX2 *pvie);
	bool TryInitButton(CWindowsGroup& Group, VALUEINFOEX2 *pvie);
	bool TryInitStatic(CWindowsGroup& Group, VALUEINFOEX2 *pvie);

	void InitControl(CDrvControl *pCtrl, IDrvControlSite *pSite = NULL);
public:
	IInputPreview2Ptr m_sptrPrv;
	IDriver6Ptr m_sptrDrv;
	int m_nDev;
	bool m_bVideo;
	static int m_idDefault;
	static bool s_bIgnoreThumbTrack;

public:
	CBaseDlg(int idd, CWnd *pParent, CBaseDlg *pBase, IUnknown *punk = NULL);
	virtual ~CBaseDlg();

	BOOL CreatePlain(int idd, CWnd *pParent);


	void DestroyControls();
	CDrvControl *FindControl(int id);
	void RemoveControl(int id);

	virtual void InitControlsState();
	virtual void OnNewImage() {};
	virtual void OnDelayedNewImage() {};
	virtual void OnLayoutChanged() {};
	virtual void OnInitChild(CBaseDlg *pChild) {}

public:
	int GetId() {return m_idd;}
	int GetDialogType() {return DriverDialog_ChildDialog;}
	void AttachChildDialogToButton(int idButton, int idChildDlgRes) {}
	void OnResetSettings(LPCTSTR lpstrSec, LPCTSTR lpstrEntry);
	void UpdateCmdUI();
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj) {return S_FALSE;}
	STDMETHOD_(ULONG,AddRef)() {return 0;}
	STDMETHOD_(ULONG,Release)() {return 0;}


	// IEditSite
	virtual void OnChangeValue(CSmartIntEdit *pEdit, int nValue);

protected:
	bool m_bDelayedInitCtrllSite;
	bool m_bDisableAllCtrls;
	virtual bool IsOwnControl(HWND hwnd, int id) {return false;}
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBaseDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnDefault();
	virtual void OnHScroll( UINT nSBCode, UINT nPos1, CScrollBar* pScrollBar);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnDestroy();
};

class CDelayedRepaintHelper
{
protected:
	bool m_bProcessDelayedRepaint;
	DWORD m_dwTicksSent;
public:
	CDelayedRepaintHelper()
	{
		m_bProcessDelayedRepaint = false;
		m_dwTicksSent = 0;
	}
	void DelayedRepaint(CWnd *pWnd)
	{
		DWORD dwNow = GetTickCount();
		if (!m_bProcessDelayedRepaint || m_dwTicksSent < dwNow-500)
		{
			m_dwTicksSent = dwNow;
			m_bProcessDelayedRepaint = true;
			pWnd->PostMessage(WM_APP+1);
		}
	}
	void OnDelayedRepaint()
	{
		m_bProcessDelayedRepaint = false;
	}
};

#endif
