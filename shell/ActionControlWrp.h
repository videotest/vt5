#ifndef __ActionControlWrp_h__
#define __ActionControlWrp_h__

class CActionControlsRegistrator
{
public:
	CActionControlsRegistrator();
	virtual ~CActionControlsRegistrator();

	void RegisterActionControl( const char *pszProgID, const char *pszActionName, bool bRegister = true );
	long GetRegistredCount();
	void GetActionControl( int nPos, CString &strProgID, CString &strActionName );
protected:
	HKEY	m_hKey;

	CStringArray	m_strControls;
	CStringArray	m_strActions;
};

class CActionControlWrp
{
public:
	CActionControlWrp();
	virtual ~CActionControlWrp();

	void Init( const char *pszProgID );

	void Create( CWnd	*pwnd );
	void DestroyWindow();

	HWND	GetSafeHwnd();
	CWnd	*GetParent();
	
	void ShowWindow( int nCmdShow );
	void MoveWindow( CRect rect );
protected:
	HWND		m_hwnd;
	IUnknownPtr	m_ptr;

};

#endif //__ActionControlWrp_h__