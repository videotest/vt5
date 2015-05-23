#if !defined(__Library_H__)
#define __Library_H__

class CLibraryException : public CException
{
	CString m_sName;
public:
	CLibraryException(const char *pszName) { m_sName = pszName; }
	int ReportError( UINT nType = MB_OK, UINT nMessageID = 0 ); 
	DECLARE_DYNAMIC(CLibraryException);
};


class CFuncNotFoundException : public CException
{
	CString m_sLibName;
	CString m_sFuncName;
public:
	CFuncNotFoundException(const char *pszLibName, const char *pszFuncName)
	{
		m_sLibName = pszLibName;
		m_sFuncName = pszFuncName;
	}
	int ReportError( UINT nType = MB_OK, UINT nMessageID = 0 ); 
	DECLARE_DYNAMIC(CFuncNotFoundException);
};

// Class CLibrary needed for safe freeing of libraries.
class CLibrary : public CObject
{
	HMODULE m_hMod;
public:
	CString m_sLibName;
	// Constructor and destructors
	CLibrary();
	CLibrary( const char *pszLibName );
	~CLibrary();
	// Operations
	BOOL LoadLibrary( const char *pszLibName );
	BOOL FreeLibrary();
	// attributes
	BOOL IsLoaded();
	operator HMODULE();
	FARPROC GetProcAddress( const char *pszFuncName );
};


#endif