#ifndef __ShellProgress_h__
#define __ShellProgress_h__

class CShellProgress : public CWnd
{
public:
	CShellProgress();

	DECLARE_MESSAGE_MAP()
	//{{AFX_MSG(CShellProgress)
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	//afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnSetText( WPARAM, LPARAM );
public:
	void SetPercent( int nNewPercent );
	void LockUpdateText( bool bLock = true );

protected:
	int		m_nCurPersent;
	int		m_nPrevPersent;
	bool	m_bLockUpdate;
	CString	m_strLockedText;
	
};

#endif //__ShellProgress_h__