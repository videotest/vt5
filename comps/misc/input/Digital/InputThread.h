#if !defined(AFX_INPUTTHREAD_H__C3C82C61_811F_11D1_A291_00002148EA4B__INCLUDED_)
#define AFX_INPUTTHREAD_H__C3C82C61_811F_11D1_A291_00002148EA4B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InputThread.h : header file
//


#define WM_GRABDONE	(WM_USER+777)
/////////////////////////////////////////////////////////////////////////////
// CInputThread thread

class CInputThread : public CWinThread
{
	DECLARE_DYNAMIC(CInputThread)

	BITMAPINFOHEADER	*m_pbi;
	HANDLE				m_hwnd;
	HANDLE				m_hEventDone;
	HANDLE				m_hEventComplete;
public:
	HANDLE				m_hEventStopper;
	CCriticalSection	m_csBuffAccess;

public:
	CInputThread(
		BITMAPINFOHEADER	*pbi,
		HANDLE				hwnd,
		HANDLE				hEventDone,
		HANDLE				hEventComplete	);           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CInputThread();

	// Generated message map functions
	//{{AFX_MSG(CInputThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	void BeginAccess(){m_csBuffAccess.Lock();}
	void EndAccess(){m_csBuffAccess.Unlock();}

	void PaintImage( HDC =0 );

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTTHREAD_H__C3C82C61_811F_11D1_A291_00002148EA4B__INCLUDED_)
