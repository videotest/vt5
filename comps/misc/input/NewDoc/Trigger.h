#if !defined(__Trigger_H__)
#define __Trigger_H__

#include "input.h"

class CActionDriverDlg : public CActionBase
{
	DECLARE_DYNCREATE(CActionDriverDlg)
	GUARD_DECLARE_OLECREATE(CActionDriverDlg)

public:
	CActionDriverDlg();
	virtual ~CActionDriverDlg();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};


class CActionTriggerMode : public CActionBase
{
	DECLARE_DYNCREATE(CActionTriggerMode)
	GUARD_DECLARE_OLECREATE(CActionTriggerMode)

public:
	CActionTriggerMode();
	virtual ~CActionTriggerMode();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionSoftwareTrigger: public CActionBase
{
	DECLARE_DYNCREATE(CActionSoftwareTrigger)
	GUARD_DECLARE_OLECREATE(CActionSoftwareTrigger)

public:
	CActionSoftwareTrigger();
	virtual ~CActionSoftwareTrigger();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};


class CActionTriggerSequence: public CActionBase
{
	DECLARE_DYNCREATE(CActionTriggerSequence)
	GUARD_DECLARE_OLECREATE(CActionTriggerSequence)

public:
	CActionTriggerSequence();
	virtual ~CActionTriggerSequence();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};





#endif
/////////////////////////////////////////////////////////////////////////////
// CTriggerSeqDlg dialog

class CTriggerSeqDlg : public CDialog, public IDriverSite
{
	CPtrArray m_arrFrames;
	CRITICAL_SECTION m_cs;
	IDriver3Ptr m_ptrDrv;
	int m_nDev;
	DWORD m_dwTicksEnd;
	int m_nImage;
	int m_nMaxImages;
	void SaveImages();
// Construction
public:
	CTriggerSeqDlg(IDriver3 *ptrDrv, int nDev, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTriggerSeqDlg)
	enum { IDD = IDD_TRIGGER_SEQUENCE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTriggerSeqDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTriggerSeqDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	// For IDriverSite
	virtual ULONG __stdcall AddRef() {return 1;}
	virtual ULONG __stdcall Release() {return 1;}
	com_call QueryInterface(const IID &iid, void **pret) {return E_NOINTERFACE;}
	com_call Invalidate();
	com_call OnChangeSize() {return S_OK;}
};
