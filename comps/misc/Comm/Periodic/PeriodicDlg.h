#if !defined(AFX_PERIODICDLG_H__165C8AA3_C152_4378_A67E_1024A2C544D2__INCLUDED_)
#define AFX_PERIODICDLG_H__165C8AA3_C152_4378_A67E_1024A2C544D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PeriodicDlg.h : header file
//

#include "Input.h"
#include "InfoField.h"

class CDeviceRecord
{
public:
	IDriver3Ptr m_sptrDrv;
	int m_nDevice;
	DWORD m_dwFrameSize;
	CDeviceRecord();
	CDeviceRecord(CDeviceRecord &s);
	CDeviceRecord& operator=(CDeviceRecord &s);
};

class CDevices : public CArray<CDeviceRecord,CDeviceRecord&>
{
public:
	bool Init();
};


/////////////////////////////////////////////////////////////////////////////
// CPeriodicDlg dialog

class CPeriodicDlg : public CDialog, public CWindowImpl, public CDockWindowImpl,
	public CNamedObjectImpl, public CEventListenerImpl, public CDrawObjectImpl
{
	DECLARE_DYNCREATE(CPeriodicDlg)
	GUARD_DECLARE_OLECREATE(CPeriodicDlg)
	ENABLE_MULTYINTERFACE();

	int m_nTimerCap;
	int m_nTimerComm;
	int m_nCur;
	CDevices m_Devices;
//	int m_nComPeriod;
	CommData m_CommData;
	LPBYTE m_lpBuffer;
	UINT m_nAllFramesSize; // Size of all frames + CommData
	COLORREF m_clrText;
	void InputImages();
	void OpenImages();
//	void ReadCommData();
// Construction
public:
	CPeriodicDlg(CWnd* pParent = NULL);   // standard constructor
	~CPeriodicDlg();

// Dialog Data
	//{{AFX_DATA(CPeriodicDlg)
	enum { IDD = IDD_PERIODIC_DIALOG };
	UINT	m_nFrames;
	UINT	m_nInterval;
	int		m_nType;
	//}}AFX_DATA
	virtual CWnd *GetWindow();
	virtual bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPeriodicDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	virtual void DoDraw( CDC &dc );

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPeriodicDlg)
	afx_msg void OnChangeEFrames();
	afx_msg void OnChangeEInterval();
	afx_msg void OnTime();
	afx_msg void OnStart();
	afx_msg void OnStop1();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBoth();
	afx_msg void OnSignal();
	//}}AFX_MSG
	DECLARE_INTERFACE_MAP()
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERIODICDLG_H__165C8AA3_C152_4378_A67E_1024A2C544D2__INCLUDED_)
