// HelpDlgImpl.h: interface for the CHelpDlgImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined __help_dlg_impl_h__
#define __help_dlg_impl_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "defs.h"

#define	STR_LEN	255

void dbg_assert( bool bExpression, const char *pszErrorDescription = 0 );

#ifdef _COMMON_LIB
#define stop_assert	ASSERT(FALSE)
#else
#define stop_assert	dbg_assert(false)
#endif


#ifdef _COMMON_LIB
class std_dll CHelpDlgImpl
#else
class CHelpDlgImpl
#endif
{
protected:
	HWND	m_hWndDlg;
	
	char	m_szHelpComponentName[STR_LEN];
	char	m_szHelpPageName[STR_LEN];
	char	m_szPrefix[STR_LEN];

public:
	CHelpDlgImpl();
	virtual ~CHelpDlgImpl();
	
	bool InitHelpImlp( HWND hWnd, const char* pszComponentName, const char* pszPageName, const char* pszPrefix = 0 );

	BOOL FilterHelpMsg( UINT message, WPARAM wParam, LPARAM lParam );
	BOOL IsHelpMsg( UINT message );
};

#endif // __help_dlg_impl_h__
