#if !defined(__Debug_H__)
#define __Debug_H__

class CProgramException : public CException
{
	int m_nError;
public:
	CProgramException(int nError, BOOL bAutoDelete) : CException(bAutoDelete)
	{
		m_nError = nError;
	}
	virtual BOOL GetErrorMessage( LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL );
};

#define DBG_ASSERT(b) if (!b) __dbg_assert_failed(__FILE__, __LINE__)

void __dbg_assert_failed(TCHAR *pszFileName, int nLine);// throw CProgramException;

void dprintf(char * szFormat, ...);
void eprintf(char * szFormat, ...);
void eprintf(int nFormat, ...);

#endif
