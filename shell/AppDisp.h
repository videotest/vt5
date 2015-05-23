#if !defined(AFX_APPDISP_H__73C48036_D40D_4C1D_A530_89A1451E1A1B__INCLUDED_)
#define AFX_APPDISP_H__73C48036_D40D_4C1D_A530_89A1451E1A1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AppDisp.h : header file
//
#include "\vt5\common\measure_misc.h"

// for select folder
static int CALLBACK	BrowseCallbackProc (HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
BOOL GetFolder (LPCTSTR szTitle, LPTSTR szPath, LPCTSTR szRoot, HWND hWndOwner);
//


/////////////////////////////////////////////////////////////////////////////
// CAppDisp command target

class CAppDisp :	public CCmdTarget, 
					public CNamedObjectImpl
{
	ENABLE_MULTYINTERFACE();
	DECLARE_DYNCREATE(CAppDisp)
	GUARD_DECLARE_OLECREATE(CAppDisp)

	CAppDisp();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppDisp)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CAppDisp();

	// Generated message map functions
	//{{AFX_MSG(CAppDisp)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAppDisp)
	afx_msg BOOL ProcessMessage(long lFlags);
	afx_msg BSTR DoOpenFileDialog( LPCTSTR strFileName, LPCTSTR strTemplate );
	afx_msg BSTR DoSaveFileDialog( LPCTSTR strFileName, LPCTSTR strTemplate );
	afx_msg LPDISPATCH GetGlobalScript(LPCTSTR psz_name);
	afx_msg void debug_break();
	afx_msg BOOL DoWritePrivateProfileString( LPCTSTR bstrFile, LPCTSTR bstrSection, LPCTSTR bstrKey, LPCTSTR bstrValue );
	afx_msg BSTR DoGetPrivateProfileString( LPCTSTR bstrFile, LPCTSTR bstrSection, LPCTSTR bstrKey, LPCTSTR bstrValue);
	afx_msg double VtDbl( const VARIANT FAR& Value );
	afx_msg BSTR VtStr( const VARIANT FAR& Value, LPCTSTR bstrFormat );
	afx_msg	BSTR DoSelectFolderDialog( LPCTSTR strTitle, LPCTSTR strRootFolder, LPCTSTR strCurrFolder );
	afx_msg double GetDisplayPixelPerM();
	afx_msg void OpenHelp( LPCTSTR strCHMFileName, LPCTSTR strHTML );
	afx_msg long GetVMSize();
	afx_msg long GetGDICount();
	afx_msg void ShellExecute( LPCTSTR szCommand );
	// [vanek] SBT:888 - 26.05.2004
	afx_msg void ReloadScripts();	
	afx_msg double GetMeasureParamCoef( long lCode, const VARIANT FAR& diNamedData );	
	afx_msg BSTR GetMeasureParamUnit( long lCode, const VARIANT FAR& diNamedData  );	
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	map_meas_params m_mapMeasParam;
public:
	void UpdateUnits(void);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPDISP_H__73C48036_D40D_4C1D_A530_89A1451E1A1B__INCLUDED_)
