#if !defined(__VideoDlg_H__)
#define __VideoDlg_H__

#include "NewDocStdProfile.h"
#include "input.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CVideoDlg dialog



class CVideoDlg : public CDialog
{
// Construction
	CStdProfileManager *m_pProf;
	IDriverPtr m_sptrDrv;
public:
	void ReadSettings();
	int CalcTime();
	DWORD CalcPlaybackScale();
	DWORD CalcPlaybackRate();
	CVideoDlg(CStdProfileManager *pProf, IUnknown *punkDrv, CWnd* pParent = NULL);   // standard constructor
	DWORD CalcFrameDelay();

// Dialog Data
	//{{AFX_DATA(CVideoDlg)
	enum { IDD = IDD_VIDEO_CAP };
	CString	m_strFile;
	UINT	m_iFPS;
	UINT	m_iTime;
	int		m_nRateScale;
	UINT	m_iPlaybackFPS;
	int		m_nPlaybackRateScale;
	UINT	m_nFileSize;
	BOOL	m_bAllocFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVideoDlg)
	afx_msg void OnBrowse();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeFps();
	afx_msg void OnSelchangeCaptureRateScale();
	afx_msg void OnHelp();
	afx_msg void OnCompression();
	afx_msg void OnAllocFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
