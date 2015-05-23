#ifndef __registry_h__
#define __registry_h__

#include "defs.h"

class std_dll CCompRegistrator 
{
protected:
	CString	m_sSectionName;
public:
	HKEY	m_hKey;
public:
	CCompRegistrator( const char *szSection = 0 );
	~CCompRegistrator();

	void SetSectionName( const char *szNewSection );
public:
	bool RegisterComponent( const char *szProgID, long iProtectionIndex = -1 );
	bool UnRegisterComponent( const char *szProgID );
public:
	long GetRegistredComponentCount();
	CString GetComponentName( int idx );
	long GetComponentProtIdx( int idx );
protected:
	void Init();
	void DeInit();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct std_dll RegVal
{
	DWORD dwType;
	union
	{
		DWORD dwNumber;
		LPBYTE pbyteData;
		LPTSTR pszString;
	};
	union
    {
		DWORD dwSize;
		DWORD dwLength;
	};
};


class std_dll CVTRegValue : public RegVal
{
	CString m_strName;
public:
	CVTRegValue(LPCTSTR szName = _T(""));
	~CVTRegValue();


public:
	DWORD GetType()					{	return dwType;	}
	void SetType(DWORD type)		{	dwType = type;	}

	LPCTSTR GetName()				{	return m_strName;	}
	void	SetName(LPCTSTR szName)	{	m_strName = szName;	}
	
	DWORD	GetSize()				{	return dwSize;	}
	void	SetSize(DWORD size)		{	dwSize = size;	}

	DWORD	GetLength()				{	return dwLength;	}
	void	SetLength(DWORD size)	{	dwLength = size;	}
// dword
	DWORD	GetDword()				{	return dwNumber; }
	void	SetDword(DWORD dwnum = 0);
// binary
    LPBYTE GetBinary()				{	return pbyteData;	}
	void SetBinary(LPBYTE lpData, DWORD size);
// string
    LPTSTR GetString()				{	return pszString;	}
	void SetString(LPCTSTR szString, DWORD size = 0);

	LPBYTE GetData();
	DWORD  GetSIZE();

	void SetData(DWORD dwtype, LPBYTE lpData, DWORD dwsize);

	bool IsEmpty()					{	return dwType == REG_NONE;	}
// clear
	void Clear();

//	bool ToString(CString & str);
//	bool FromString(CString & str);
};

//////////////////////////////////////////////////////////////////////////////////////////

class std_dll CVTRegKey
{
public:
	CVTRegKey();
	CVTRegKey(HKEY hRoot, LPCTSTR szKey);
	~CVTRegKey();

// Attributes
public:
	operator HKEY() const;
	HKEY	m_hKey;

	HKEY	m_hRoot;
	CString m_strKey;

// Operations
public:
	bool SetValue(CVTRegValue & val);
	bool GetValue(CVTRegValue & val);

	bool Create(HKEY hRoot = 0, LPCTSTR szKey = 0);
	bool CreateSubKey(LPCTSTR szKey = 0);
	bool Open(HKEY hRoot = 0, LPCTSTR szKey = 0);
	bool Close();
	bool DeleteKey();
	bool DeleteValue(CVTRegValue & val);
	bool DeleteAllValues();
	bool DeleteSubKey(LPCTSTR szSubKey);
	bool DeleteAllSubKeys();
	bool EnumSubKeys(CStringArray & sa);
	bool EnumValues(CStringArray & sa);

	bool IsExists();
	bool IsEmpty();
//	bool ToString(CString & str);
//	bool FromString(CString & str);

	static bool IsExists(HKEY hRootKey, LPCTSTR szPath);
	static bool CheckRoot(HKEY hRoot);

};

// predefines of util helper functions
std_dll void AddChar(CString & str, TCHAR ch);
std_dll void AddWithCheck(CStringArray & sa, LPCTSTR szString);

std_dll bool RecursiveCreateBranch(HKEY hRoot, LPCTSTR szBranch);
std_dll bool GetFirstLastKey(const CString & str, CString & strFirst, CString & strLast);

#define REG_FILE_SIG	"REGEDIT4"
#define REG_FILE_DWORD	"dword"
#define REG_FILE_BINARY "hex"
#define REG_FILE_HEX2	"hex(2)"
#define REG_FILE_STRING ""


#define REG_FILE_KEY_CLASSES_ROOT	"HKEY_CLASSES_ROOT"
#define REG_FILE_KEY_CURRENT_USER	"HKEY_CURRENT_USER"
#define REG_FILE_KEY_LOCAL_MACHINE	"HKEY_LOCAL_MACHINE"
#define REG_FILE_KEY_USERS			"HKEY_USERS"
#define REG_FILE_KEY_CURRENT_CONFIG "HKEY_CURRENT_CONFIG"



/*
class std_dll CVTRegFile  
{
	CString			m_strFileName;
	CStringArray	m_arr;
	CStdioFile		m_File;

public:
	CVTRegFile(LPCTSTR szFileName = 0);
	virtual ~CVTRegFile();

	bool SaveRegistry(HKEY hKey, LPCTSTR szSubKey);
	bool LoadRegistry();

	void	SetFileName(LPCTSTR szFileName)	{	m_strFileName = szFileName;	}
	LPCTSTR GetFileName(LPCTSTR)			{	return m_strFileName;	}

	CStringArray&	GetArray()				{	return m_arr;	}

	CStdioFile&		GetFile()				{	return m_File;	}

	bool SaveFile(LPCTSTR szFileName = 0);
	bool LoadFile(LPCTSTR szFileName = 0);

	bool Init();

};

bool DwordToString(DWORD & dwVal, CString & str);
bool StringToDword(DWORD & dwVal, CString & str);

bool BinaryToString(LPBYTE lpbVal, DWORD size, CString & str);
bool StringToBinary(LPBYTE lpbVal, int nSize, int & nRetSize, CString & str);
int  GetBinaryLenFromString(CString & str);

bool StrValToString(CString & strString, CString & str);
bool StringToStrVal(CString & strString, CString & str);

bool GetStringFromKey(HKEY hRoot, CString & strKey, CString & str);
bool GetKeyFromString(HKEY & hRoot, CString & strKey, CString & str);
bool IsSection(CString & str);
bool IsComment(CString & str);


bool NeedNextLine(CString & str);
bool ReadString(CStdioFile & File, CString & str);
bool WriteString(CStdioFile & File, CString & str);

bool SaveArray(CStdioFile & File, CStringArray & sa);
bool LoadArray(CStdioFile & File, CStringArray & sa);

bool ProcessArray(CStringArray & sa);
bool ProcessRegistry(HKEY hRoot, LPCTSTR szSubKey, CStringArray & sa);
*/

/*	rtBinary	= REG_BINARY,	// Binary data in any form. 
	rtDword		= REG_DWORD,	// A 32-bit number. 
	rtEndianL	= REG_DWORD_LITTLE_ENDIAN,	// A 32-bit number in little-endian format. 
											// This is equivalent to REG_DWORD.
											// In little-endian format, a multi-byte value is stored 
											// in memory from the lowest byte (the "little end") to 
											// the highest byte. For example, the value 0x12345678 
											// is stored as (0x78 0x56 0x34 0x12) in little-endian format. 
											// Windows NT, Windows 95, and Windows 98 are designed to run 
											// on little-endian computer architectures. A user may connect 
											// to computers that have big-endian architectures, such as 
											// some UNIX systems. 
	rtEndianB	= REG_DWORD_BIG_ENDIAN,		// A 32-bit number in big-endian format. In big-endian format, 
											// a multi-byte value is stored in memory from the highest byte 
											// (the "big end") to the lowest byte. For example, 
											// the value 0x12345678 is stored as (0x12 0x34 0x56 0x78) 
											// in big-endian format.
	rtExpStr	= REG_EXPAND_SZ,	// A null-terminated string that contains unexpanded 
									// references to environment variables (for example, "%PATH%"). 
									// It will be a Unicode or ANSI string depending on whether you use 
									// the Unicode or ANSI functions. To expand the environment variable 
									// references, use theExpandEnvironmentStrings function. 
	rtLink		= REG_LINK,		// A Unicode symbolic link. 
	rtMultiStr	= REG_MULTI_SZ,	// An array of null-terminated strings, terminated by two null characters. 
	rtNone		= REG_NONE,		// No defined value type. 
	rtResList	= REG_RESOURCE_LIST,	// A device-driver resource list. 
	rtStr		= REG_SZ		// A null-terminated string. It will be a Unicode or ANSI string 
								// depending on whether you use the Unicode or ANSI functions. 
*/


#endif //__registry_h__