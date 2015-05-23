#if !defined(AFX_VIDEODLG_H__4DDAF9C0_ACB9_11D3_A98A_FB5C2C9A634F__INCLUDED_)
#define AFX_VIDEODLG_H__4DDAF9C0_ACB9_11D3_A98A_FB5C2C9A634F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVideoDlg dialog

class CVideoDlg : public CDialog
{
// Construction
public:
	int CalcTime();
	DWORD CalcPlaybackScale();
	DWORD CalcPlaybackRate();
	CVideoDlg(CWnd* pParent = NULL);   // standard constructor
	DWORD CalcFrameDelay();
	UINT m_xMax;
	UINT m_yMax;
	void UpdateControls();


// Dialog Data
	//{{AFX_DATA(CVideoDlg)
	enum { IDD = IDD_VIDEO_CAP };
	CSliderCtrl	m_SFrameSize;
	CStatic	m_FPSMessage;
	CSliderCtrl	m_SSizeY;
	CSliderCtrl	m_SSizeX;
	CString	m_strFile;
	UINT	m_iFPS;
	UINT	m_iTime;
	int		m_nRateScale;
	UINT	m_iPlaybackFPS;
	int		m_nPlaybackRateScale;
	int		m_nCaptureMemory;
	BOOL	m_bGrayScale;
	UINT	m_nSizeX;
	UINT	m_nSizeY;
	UINT	m_nFrameSize;
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
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchangeCaptureMemory();
	afx_msg void OnDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEODLG_H__4DDAF9C0_ACB9_11D3_A98A_FB5C2C9A634F__INCLUDED_)
