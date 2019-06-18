#include "stdafx.h"
#include "RegHelp.h"
#include "Helper.h"
#include "stringEx.h"


//////////////////////////////////////////////////////////////////////
// Utility functions

bool RecursiveCreateBranch(HKEY hRoot, const char * szBranch)
{
	if (!hRoot)
		return false;
	
	if (!szBranch || !lstrlen(szBranch))
		return true;

	CRegKey reg;
	
	// if branch exists simply return true
	if (reg.Open(hRoot, szBranch))
		return true;
	
	_bstr_t bstrKey;
	_bstr_t bstrSubKey;
	
	if (!GetFirstLastKey(szBranch, bstrKey, bstrSubKey))
		return false;

	if (bstrKey.length())
	{
		if (!RecursiveCreateBranch(hRoot, bstrKey))
			return false;

		if (!reg.Open(hRoot, bstrKey))
			return false;
	}

	return reg.CreateSubKey(bstrSubKey);
}

bool GetFirstLastKey(const char * str, _bstr_t & bstrFirst, _bstr_t & bstrLast)
{
	bstrFirst = "";
	bstrLast = "";

	long lLen = str ? lstrlen(str) : 0;
	if (!lLen)
		return true;

	char * szSrc = StrDupNew(str);
	if (!szSrc)
		return false;

	// remove last '\\', if exists
	if (szSrc[lLen - 1] == '\\')
		szSrc[lLen - 1] = 0;

	// find last '\\'
	char * pdest = strrchr(szSrc, '\\');
	// if found
	if (pdest)
	{
		*pdest = 0;
		bstrLast = pdest + 1;
		bstrFirst = szSrc;
	}
	else
		bstrLast = szSrc;

	delete [] szSrc;

	return true;
}


void AddChar(_bstr_t & str, char ch)
{
	char add[2] = { 0, 0 };
	add[0] = ch;

	char * ptr = str;
	if (str.length() > 0 && ptr[str.length() - 1] != ch)
		str += add;
}


void AddWithCheck(StringArray & sa, const char * szString)
{
	if (!szString || !strlen(szString))
		return;

	for (int i = 0; i < sa.GetSize(); i++)
	{
		if (!lstrcmp(sa[i], szString))
			return;
	}
	sa.Add(szString);
}

/////////////////////////////////////////////////////////////////////////////////////////
// class CRegValue

CRegValue::CRegValue(const char * szName)
{	
	if( szName )
		SetName(szName);	
	
	dwType = 0; 
	dwNumber = 0;
	pszString = 0;
    pbyteData = 0;
    dwSize = 0;
	dwLength = 0;
}

CRegValue::~CRegValue()
{	
	Clear();	
}

// dword
void CRegValue::SetDword(DWORD dwnum)
{
	Clear();

	dwNumber = dwnum;
	SetType(REG_DWORD);
}

// binary
void CRegValue::SetBinary(LPBYTE lpData, DWORD size)
{
	Clear();
	SetType(REG_BINARY);
		
	if (!lpData || !size)
		return;

	pbyteData = new BYTE [size];
	if (!pbyteData)
		return;

	memcpy(pbyteData, lpData, sizeof (BYTE) * size);

	SetSize(size);
}

// string
void CRegValue::SetString(const char * szString, DWORD size)
{
	Clear();
	SetType(REG_SZ);
		
	if (!szString)
	{
		SetLength(0);
		return;
	}

	if (!size)
		size = strlen(szString) + 1; // NULL-terminated simbol

	pszString = new TCHAR [size];
	if (!pszString)
		return;

	memcpy(pszString, szString, sizeof(char)*size);
	SetLength(size);
}

// clear
void CRegValue::Clear()
{
	switch (GetType())
	{
	case REG_DWORD: //	REG_DWORD_LITTLE_ENDIAN:
	case REG_DWORD_BIG_ENDIAN:
		{
			dwNumber = 0;
		}
		break;

	case REG_BINARY:
		{
			if (pbyteData)
				delete pbyteData, pbyteData = 0;
			SetSize(0);
		}
		break;

	case REG_MULTI_SZ:
	case REG_EXPAND_SZ:
	case REG_SZ:
		{
			if (pszString)
				delete [] pszString, pszString = 0;
			SetLength(0);
		}
		break;

	case REG_NONE:
		break;
	}	// switch
	SetType(REG_NONE);
}

LPBYTE CRegValue::GetData()
{
	LPBYTE lpByte = 0;

	switch (GetType())
	{
	case REG_DWORD:
		lpByte = (LPBYTE)&dwNumber;
		break;

	case REG_BINARY:
		lpByte = pbyteData;
		break;

	case REG_SZ:
		lpByte = (LPBYTE)pszString;
		break;
	}
	return lpByte;
}

DWORD CRegValue::GetDataSize()
{
	DWORD dwRet = 0;

	switch (GetType())
	{
	case REG_DWORD:
		dwRet = sizeof(DWORD);
		break;

	case REG_BINARY:
		dwRet = GetSize();
		break;

	case REG_SZ:
		dwRet = GetLength();
		break;
	}
	return dwRet;
}

void CRegValue::SetData(DWORD dwtype, LPBYTE lpData, DWORD dwsize)
{
	switch (dwtype)
	{
	case REG_DWORD: // == 	REG_DWORD_LITTLE_ENDIAN:
	case REG_DWORD_BIG_ENDIAN:
		{
			SetDword((DWORD)*lpData);
		}
		break;

	case REG_BINARY:
		{
			SetBinary(lpData, dwsize);
		}
		break;

	case REG_MULTI_SZ:
	case REG_EXPAND_SZ:
	case REG_SZ:
		{
			SetString((const char *)lpData, dwsize);
		}
		break;
	}	// switch
}


//#define CheckRet(a) ((if (a) return false;))


/////////////////////////////////////////////////////////////////////////////////////////
// class CRegKey

CRegKey::CRegKey()
{
	m_hKey = NULL;
	m_hRoot = NULL;
//	m_bstrKey = "";
}

CRegKey::CRegKey(HKEY hRoot, const char * szKey)
{
	m_hKey = NULL;
	m_hRoot = CheckRoot(hRoot) ? hRoot : NULL;
	m_bstrKey = szKey ? szKey : "";
}

CRegKey::~CRegKey()
{
	Close();
}


CRegKey::operator HKEY() const
{
	return m_hKey;
}

bool CRegKey::SetValue(CRegValue & val)
{
	if (!m_hKey && !Create())
		return false;

	return (ERROR_SUCCESS == RegSetValueEx(m_hKey, val.GetName(), 0, val.GetType(), val.GetData(), val.GetDataSize()));
}

bool CRegKey::GetValue(CRegValue & val)
{
	if (!m_hKey && !Open())
		return false;

	DWORD dwType = 0;
	DWORD cbData = 0;
	// get info for this key
	if (ERROR_SUCCESS != RegQueryValueEx(m_hKey, val.GetName(), 0, &dwType, 0, &cbData))
		return false;
 
	// allocate needed buffer
	LPBYTE	lpData = new BYTE [cbData]; // address of buffer for value data
	if (!lpData)
		return false;
	
	if (ERROR_SUCCESS != RegQueryValueEx(m_hKey, val.GetName(), 0, &dwType, lpData, &cbData))
	{
		// free allocated buffer
		delete [] lpData;
		return false;
	}

	// fill wrapper
	val.SetData(dwType, lpData, cbData);
	// free allocated buffer
	delete [] lpData;

	return true;
}

bool CRegKey::Create(HKEY hRoot, const char * szKey)
{
	if (Open(hRoot, szKey))
		return true;

	DWORD dwDisp = 0;
	if (ERROR_SUCCESS == RegCreateKeyEx(m_hRoot, m_bstrKey, 0, 0, 0 , KEY_ALL_ACCESS, NULL, &m_hKey, &dwDisp))
		return true;

	if (!RecursiveCreateBranch(m_hRoot, m_bstrKey))
		return false;

	return Open();
}

bool CRegKey::Open(HKEY hRoot, const char * szKey)
{
	Close();

	if (CheckRoot(hRoot))
		m_hRoot = hRoot;

	m_bstrKey = szKey ? szKey : "";
	if (!m_hRoot)
		return false;

	return (ERROR_SUCCESS == RegOpenKeyEx(m_hRoot, m_bstrKey, 0, KEY_ALL_ACCESS, &m_hKey));
}

bool CRegKey::CreateSubKey(const char * szKey)
{
	if (!szKey || !strlen(szKey))
		return false;

	if (!m_hKey && !Open())
		return false;

	HKEY hKey = 0;
	DWORD dwDisp = 0;

	if (ERROR_SUCCESS == RegCreateKeyEx(m_hKey, szKey, 0, 0, 0 , KEY_ALL_ACCESS, NULL, &hKey, &dwDisp))
	{
		RegCloseKey(hKey);
		return true;
	}
	return false;
}


bool CRegKey::Close()
{
	if (!m_hKey)
		return true;

	bool bRet = (ERROR_SUCCESS == RegCloseKey(m_hKey));
	if (bRet)
		m_hKey = 0;

	return bRet;
}

bool CRegKey::DeleteKey()
{
	// get last subkey
	_bstr_t bstrFirst;
	_bstr_t bstrLast;

	if (!GetFirstLastKey(m_bstrKey, bstrFirst, bstrLast))
		return false;

	// open subkey
	if (!Open(m_hRoot, bstrFirst))
		return false;

	if (!DeleteSubKey(bstrLast))
		return false;

	return true;
}

bool CRegKey::EnumSubKeys(StringArray & sa)
{
	if (!m_hKey && !Open())
		return false;

	DWORD dwNumSubKeys = 0;
	DWORD cbMaxSubKeyLen = 0;

	// get info for this key
	if (ERROR_SUCCESS != RegQueryInfoKey(m_hKey, 0, 0, 0, &dwNumSubKeys, &cbMaxSubKeyLen, 0, 0, 0, 0, 0, 0))
		return false;
 
	FILETIME ft;
	DWORD	cbSubKeyName = cbMaxSubKeyLen + 1;
	char *	szName = new char [cbSubKeyName];
	if (!szName)
		return false;

	LONG lRet = 0;
	// for all subkeys
	for (int i = 0; i < (int)dwNumSubKeys && lRet != ERROR_NO_MORE_ITEMS; i++)
	{
		//get subkeys and process it
		lRet = RegEnumKeyEx(m_hKey, (DWORD)i, szName, &cbSubKeyName, 0, 0, 0, &ft);

		if (lRet != ERROR_SUCCESS)
			continue;
		AddWithCheck(sa, szName);
	}
	delete [] szName;

	return true;
}

bool CRegKey::EnumValues(StringArray & sa)
{
	if (!m_hKey && !Open())
		return false;

	DWORD cValues = 0;
	DWORD cbMaxValueNameLen = 0;
	DWORD cbMaxValueLen = 0;

	// get info for this key
	if (ERROR_SUCCESS != RegQueryInfoKey(m_hKey, 0, 0, 0, 0,0, 0, &cValues, 
										 &cbMaxValueNameLen, &cbMaxValueLen, 0, 0))
		return false; 

	DWORD	cbValueName = cbMaxValueNameLen + 1;
	char *	szName = new char [cbValueName];
	if (!szName)
		return false;
//	DWORD	dwType = 0;
	LONG lRet = 0;	
	// for all values 
	for (int i = 0; i < (int)cValues && ERROR_NO_MORE_ITEMS != lRet; i++)
	{
		// get value
		lRet = RegEnumValue(m_hKey, (DWORD)i, szName, &cbValueName, 0, 0/*&dwType*/, 0, 0);
		if (lRet != ERROR_SUCCESS)
			continue;

		AddWithCheck(sa, szName);
	}
	
	delete [] szName;

	return true;
}



bool CRegKey::DeleteValue(CRegValue & val)
{
	if (!m_hKey && !Open())
		return false;

	return (ERROR_SUCCESS == RegDeleteValue(m_hKey, val.GetName()));
}


bool CRegKey::DeleteAllValues()
{
	StringArray sa;
	if (!EnumValues(sa))
		return false;

	bool bRet = true;
	for (int i = 0; i < sa.GetSize(); i++)
	{
		if (!DeleteValue(CRegValue(sa[i])))
			bRet = false;
	}

	return bRet;
}

bool CRegKey::DeleteSubKey(const char * szSubKey)
{
	_bstr_t bstr(szSubKey);
	if (!bstr.length())
		return true;

	if (!m_hKey && !Open())
		return false;

	_bstr_t bstrSubKey = m_bstrKey;
	AddChar(bstrSubKey, '\\');
	bstrSubKey += bstr;

	CRegKey reg;
	if (!reg.Open(m_hRoot, bstrSubKey))
		return true;

	bool bRet = reg.DeleteAllValues();
	bRet = reg.DeleteAllSubKeys() && bRet;
	reg.Close();

	return (ERROR_SUCCESS == RegDeleteKey(m_hKey, szSubKey)) && bRet;
}

bool CRegKey::DeleteAllSubKeys()
{
	StringArray sa;
	if (!EnumSubKeys(sa))
		return false;

	bool bRet = true;
	for (int i = 0; i < sa.GetSize(); i++)
	{
		if (!DeleteSubKey(sa[i]))
			bRet = false;
	}

	return bRet;
}

bool CRegKey::IsExists()
{
	if (m_hKey)
		return true;

	if (!m_hRoot)
		return false;

	HKEY hKey = NULL;
	if (ERROR_SUCCESS != RegOpenKeyEx(m_hRoot, m_bstrKey, 0, KEY_READ, &hKey) || !hKey)
		return false;

	RegCloseKey(hKey);
	return true;
}

bool CRegKey::IsExists(HKEY hRootKey, const char * szPath)
{
	if (!hRootKey || !szPath)
		return false;

	HKEY hKey = NULL;
	if (ERROR_SUCCESS != RegOpenKeyEx(hRootKey, szPath, 0, KEY_READ, &hKey) || !hKey)
		return false;

	RegCloseKey(hKey);
	return true;
}
bool CRegKey::IsEmpty()
{
	// if not key is not can not be opened it's means key wasn't be created
	if (!m_hKey && !Open())
		return true;

	DWORD dwNumSubKeys = 0;
	DWORD dwNumValues = 0;

	// get info for this key
	if (ERROR_SUCCESS != RegQueryInfoKey(m_hKey, 0, 0, 0, &dwNumSubKeys, 0, 0, &dwNumValues, 0, 0, 0, 0))
		return false;

	// has subkeys || has values
	if (dwNumSubKeys || dwNumValues)
		return false;

	return true;
}


bool CRegKey::CheckRoot(HKEY hRoot)
{
	if (hRoot == HKEY_CLASSES_ROOT)
		return true;
	if (hRoot == HKEY_CURRENT_USER)
		return true;
	if (hRoot == HKEY_LOCAL_MACHINE)
		return true;
	if (hRoot == HKEY_USERS)
		return true;
	if (hRoot == HKEY_CURRENT_CONFIG)
		return true;

	return false;
}

