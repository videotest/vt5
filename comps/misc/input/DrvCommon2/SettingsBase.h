#pragma once

#include "input.h"
#include "BaseDlg.h"
#include "HistoDlg.h"

//sergey 03/10/06
//#include "Reposition.h"
//#include "PreviewStatic.h"
//end



struct CSettingsIds
{
	CHistoIds m_HistoIds;
	int m_idTab,m_idPeriod;
	int m_idFPSStr,m_idPeriodStr,m_idHistoCheckBox;
};

class CTabPane
{
public:
	CString m_sName;
	int m_idd;
};

// CSettingsDialog dialog

class CSettingsBase : public CBaseDlg, public IInputPreviewSite, public CDelayedRepaintHelper//,public CRepositionManager//sergey 03/10/06-public CRepositionManager
{
	//sergey 03/10/06
	//CPreviewStatic	m_Image;
	//end
	DECLARE_DYNAMIC(CSettingsBase)

	CSize m_szButton;
	IInputPreview2Ptr m_sptrPrv;
//	IDriverDialogSitePtr m_sptrDDS;
	IDriver3Ptr m_sptrDrv;
	int m_nDev;
	CSettingsIds m_Ids;
	CArray<CTabPane,CTabPane&> m_arrPanes;
	bool m_bUseHelp;

	CBaseDlg *m_pDialog;
	void AddDialog(CBaseDlg *p, int idd);

public:
	
	CSettingsBase(IUnknown *punk, int nDevice, int idd, CSettingsIds Ids, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsBase();

	void AddPane(int idName, int idDlg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	bool IsOwnControl(HWND hwnd, int id);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	// For IInputPreviewSite
	com_call Invalidate();
	com_call OnChangeSize();
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj) {return S_FALSE;}
	STDMETHOD_(ULONG,AddRef)() {return 0;}
	STDMETHOD_(ULONG,Release)() {return 0;}
public:
	CTabCtrl m_Tab;
	virtual BOOL OnInitDialog();
	CHistoDlg m_HistoDlg;
	CStatic m_StaticPeriod;
	CButton m_Ok;
	CButton m_Help;
	void ShowHistogram(BOOL bShow);
protected:
	virtual void OnOK();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnDestroy();
	afx_msg LRESULT OnDelayedRepaint(WPARAM wParam, LPARAM lParam);
	virtual void OnInitChild(CBaseDlg *pChild);
  
	//sergey 02/10/06
	/*afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);*/

	//afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	//end
};
