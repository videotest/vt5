#ifndef __registry_helper_h__
#define __registry_helper_h__

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "Helper.h"

struct RegVal
{
	DWORD dwType;
	union
	{
		DWORD	dwNumber;
		LPBYTE	pbyteData;
		char *	pszString;
	};
	union
    {
		DWORD	dwSize;
		DWORD	dwLength;
	};
};


class CRegValue : public RegVal
{
	_bstr_t bstrName;
public:
	CRegValue(const char * szName = 0);
	virtual ~CRegValue();

public:
	DWORD	GetType()						{	return dwType;	}
	void	SetType(DWORD type)				{	dwType = type;	}

	char *	GetName()						{	return bstrName;	}
	void	SetName(const char * szName)	{	bstrName = szName;	}
	
	DWORD	GetSize()						{	return dwSize;	}
	void	SetSize(DWORD size)				{	dwSize = size;	}

	DWORD	GetLength()						{	return dwLength;	}
	void	SetLength(DWORD size)			{	dwLength = size;	}
// dword
	DWORD	GetDword()						{	return dwNumber; }
	void	SetDword(DWORD dwnum = 0);
// binary
    LPBYTE	GetBinary()						{	return pbyteData;	}
	void	SetBinary(LPBYTE lpData, DWORD size);
// string
    char *	GetString()						{	return pszString;	}
	void	SetString(const char * szString, DWORD size = 0);

	LPBYTE	GetData();
	DWORD	GetDataSize();

	void SetData(DWORD dwtype, LPBYTE lpData, DWORD dwsize);

	bool IsEmpty()							{	return dwType == REG_NONE;	}
// clear
	void Clear();
};

//////////////////////////////////////////////////////////////////////////////////////////

class CRegKey
{
public:
	CRegKey();
	CRegKey(HKEY hRoot, const char * szKey);
	virtual ~CRegKey();

// Attributes
public:
	operator HKEY() const;
	HKEY	m_hKey;

	HKEY	m_hRoot;
	_bstr_t	m_bstrKey;

// Operations
public:
	bool SetValue(CRegValue & val);
	bool GetValue(CRegValue & val);

	bool Create(HKEY hRoot = 0, const char * szKey = 0);
	bool CreateSubKey(const char * szKey = 0);
	bool Open(HKEY hRoot = 0, const char * szKey = 0);
	bool Close();
	bool DeleteKey();
	bool DeleteValue(CRegValue & val);
	bool DeleteAllValues();
	bool DeleteSubKey(const char * szSubKey);
	bool DeleteAllSubKeys();
	bool EnumSubKeys(StringArray & sa);
	bool EnumValues(StringArray & sa);

	bool IsExists();
	bool IsEmpty();

	static bool IsExists(HKEY hRootKey, const char * szPath);
	static bool CheckRoot(HKEY hRoot);

};

// predefines of util helper functions
//void AddChar(_bstr_t & str, char ch);
void AddWithCheck(StringArray & sa, const char * szString);

bool RecursiveCreateBranch(HKEY hRoot, const char * szBranch);
bool GetFirstLastKey(const char * str, _bstr_t & szFirst, _bstr_t & szLast);

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





#endif //__registry_helper_h__