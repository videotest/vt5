#pragma once

#include "\vt5\ifaces\thumbnail.h"

// CResultPartDlg dialog
#include "ChildWnd.h"
#include "resource.h"
#include "afxwin.h"

void ExecuteContextMenu(CPoint pt);


//*****************************************************************************/
class CLayerDataItem : public CStatic
{
	IThumbnailManagerPtr m_sptrThumbMan;
	IUnknownPtr m_sptrNData;
	int m_nID;
	CWnd *m_pParent;
public:
	CLayerDataItem()
	{
		m_pParent = 0;
		m_nID = -1;
		m_sptrThumbMan = GetAppUnknown();
	}
	void SetContextParent( CWnd *pParent ) { m_pParent = pParent; }
	void SetNData( IUnknown *punk ) { m_sptrNData = punk; }
	void SetID( UINT nID ) { m_nID = nID; };
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};
//*****************************************************************************/
class CResultPartDlg : public CChildWnd, public CDialog
{
	DECLARE_DYNAMIC(CResultPartDlg)

	CWnd *m_pParent;
	CLayerDataItem m_Data;
	int m_nID;
public:
	enum { IDD = IDD_DLG_RESULT };
	CResultPartDlg(CWnd* pParent = NULL, int id = -1);   // standard constructor
	virtual ~CResultPartDlg();
	void SetNData( IUnknown *punk ) { m_Data.SetNData( punk ); }
	void Select();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedPicture2();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	void SetWindowSize(int nWidth);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};
//*****************************************************************************/
class CLayerPartDlg : public CChildWnd, public CDialog
{
	DECLARE_DYNAMIC(CLayerPartDlg)
	int m_nID;
	bool m_bSelfChange;
	CWnd *m_pParent;
	bool m_bCheched;
	CLayerDataItem m_Data;
	void RepositionWindow(int id, CRect rcBase, int nShift);
public:
	enum { IDD = IDD_DLG_LAYER };
	CLayerPartDlg(CWnd *pParent = NULL);   // standard constructor
	virtual ~CLayerPartDlg();
	void Refresh( int nID, IUnknown *punkNData );
	void ShowPane( bool bStat, bool bNoRefresh = false );
	void SetNData( IUnknown *punk ) { m_Data.SetNData( punk ); }
	void Select();
	void SetBackgroundColor(COLORREF clr);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	long _str_to_color( CString strRGBColor );

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClicked();
	void SetWindowSize(int nWidth);
	void SetColor(COLORREF clr);
	void ChangeColor(COLORREF clr);

	DECLARE_EVENTSINK_MAP()
public:
	void OnChange();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};
//*****************************************************************************/
