#if !defined(AFX_BATCHINPUTDLG_H__A5246ACC_51EB_11D6_BA8D_009027594905__INCLUDED_)
#define AFX_BATCHINPUTDLG_H__A5246ACC_51EB_11D6_BA8D_009027594905__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BatchInputDlg.h : header file
//

#include "Input.h"


struct CoordData
{
	int  nError; // < 10 - can not read byte nError, 10 - first byte not FF
	long x,y;
	DWORD Tm1Beg,Tm1End,Tm2End,Tm3End,Tm4End,Tm5End; // For debuging
	int nSeqNum;
	CString Format();
};

class CImageRecord
{
public:
	CoordData m_CoordData;
	bool m_bInit;
	DWORD m_Time;
	int m_nFrame;

	CImageRecord();
	CImageRecord(CImageRecord&);
	CImageRecord& operator=(CImageRecord &);
};

/////////////////////////////////////////////////////////////////////////////
// BatchInputDlg dialog

class BatchInputDlg : public CDialog, public CWindowImpl, public CDockWindowImpl,
	public CHelpInfoImpl, public CNamedObjectImpl
{
	DECLARE_DYNCREATE(BatchInputDlg)
	GUARD_DECLARE_OLECREATE(BatchInputDlg)
	ENABLE_MULTYINTERFACE();

	
	LONG m_nCur,m_nCur1;
	bool m_bUseMMTimer;
	int m_nBufferSize;
	int m_nTimer,m_nWinTimer;
	DWORD m_dwStart;
	bool m_bSaveImages;
	void UpdateID();
	void InputImage();
	int m_nDev;
	LPBYTE m_lpData;
	DWORD m_dwFrameSize;
	sptrIDriver6 m_Driver;
	CString m_sPath;
	IFileFilter2Ptr	m_ptrFilter;
	CArray<CImageRecord,CImageRecord&> m_Images;
	bool SaveImage(IUnknown *punkImg, LPCTSTR lpstrName);
	void SaveImages();
	void SaveSettings();
	DWORD m_dwThreadId;
	HANDLE m_hThread;
	void DoStop(bool bSaveImages);
	int m_nRealInterval;
	CString m_sPauseTxt,m_sResumeTxt;
	DWORD m_dwTriggerNotifyMode;

	enum XStage
	{
		Ready = 0,
		Capturing = 1,
		Writing = 2,
		Paused = 3,
	};
	XStage m_Stage;
	bool PrepareBuffers();
	void BeginCapture();
	void EndCapture();
	void UnprepareBuffers();
	void UpdateUIOnChangeState();
	void OnExactTimer(UINT nIDEvent);

	BOOL m_bPrevTriggerMode;
	long m_lTriggerSeqNum;
	void OnTryTrigger();
	void RestoreTriggerMode();

// Construction
public:
	BatchInputDlg();   // standard constructor
	~BatchInputDlg();

// Attributes
public:
	virtual CWnd *GetWindow();
	virtual bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );


// Dialog Data
	//{{AFX_DATA(BatchInputDlg)
	enum { IDD = IDD_BATCHINPUT_DIALOG };
	LONG	m_nFrames;
	UINT	m_nInterval;
	int		m_nIntervalType;
	BOOL    m_bUseTrigger;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BatchInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
public:

	// Generated message map functions
	//{{AFX_MSG(BatchInputDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSelchangeUnit();
	afx_msg void OnChangeEFrames();
	afx_msg void OnChangeEInterval();
	//}}AFX_MSG
	DECLARE_INTERFACE_MAP()
	BEGIN_INTERFACE_PART(DrvSite, IDriverSite)
		com_call Invalidate();
		com_call OnChangeSize();
	END_INTERFACE_PART(DrvSite);
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnUserMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetInterface(WPARAM, LPARAM){return (LRESULT)GetControllingUnknown();}
	afx_msg void OnBnClickedPause();
	afx_msg void OnBnClickedCaptureByTrigger();
	afx_msg void OnBnClickedButton1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATCHINPUTDLG_H__A5246ACC_51EB_11D6_BA8D_009027594905__INCLUDED_)
