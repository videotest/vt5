#include "stdafx.h"
#include "registry.h"
#include "Utils.h"
#include "GuardInt.h"

//registry key
const char *szCountEntry = "Count"; 
const char *szProgIDEntry = "Component"; 
const char *szProtectionEntry = "Code"; 
const char *szRegistryKey = 

#ifdef _DEBUG
"software\\VideoTest\\vt5\\add-ins(debug)";
#else
"software\\VideoTest\\vt5\\add-ins";
#endif //_DEBUG


const char *szMenuRegistryKey = 
#ifdef _DEBUG
"software\\VideoTest\\vt5\\add-ins(debug)\\menus";
#else
"software\\VideoTest\\vt5\\add-ins\\menus";
#endif //_DEBUG


CCompRegistrator::CCompRegistrator( const char *szSection )
{
	m_hKey = 0;
	SetSectionName( szSection );
}

CCompRegistrator::~CCompRegistrator()
{
	DeInit();
}

void CCompRegistrator::SetSectionName( const char *szNewSection )
{
	DeInit();

	if( szNewSection )
	{
		CString strProgRegKey(szRegistryKey);

		//[paul]31.10.2001, cos GetRegProgKey get section by export module function, but need use interface
		/*
		long lSize = 0;
		GetRegProgKey(0, &lSize);
		if (lSize)
		{
			GetRegProgKey(strProgRegKey.GetBuffer(lSize + 1), &lSize);
			strProgRegKey.ReleaseBuffer();
		}
		*/

		IUnknown* punkAppUnknown = 0;
		typedef  IUnknown* (*PGET_APP_UNKNOWN)(bool bAddRef /*= false*/);
		HMODULE hMod = GetModuleHandle(0);
		if (hMod)
		{
			PGET_APP_UNKNOWN pGetAppUnknown = (PGET_APP_UNKNOWN)GetProcAddress(hMod, "GuardGetAppUnknown"); 
			if (pGetAppUnknown)
				punkAppUnknown = pGetAppUnknown(false);
		}

		if (!punkAppUnknown)
		{
		#ifdef _DEBUG
			HINSTANCE hDll_Common = GetModuleHandle("common_d.dll");
		#else
			HINSTANCE hDll_Common = GetModuleHandle("common.dll");
		#endif
			if (hDll_Common)
			{
				PGET_APP_UNKNOWN pGetAppUnknown = (PGET_APP_UNKNOWN)GetProcAddress(hDll_Common, "GetAppUnknown"); 
				if (pGetAppUnknown)
					punkAppUnknown = pGetAppUnknown(false);
			}
		}

		IVTApplicationPtr sptrA = punkAppUnknown;
		if (sptrA != 0)
		{
			BSTR bstr = 0;
			sptrA->GetRegKey(&bstr);
			strProgRegKey = bstr;
			::SysFreeString( bstr );	bstr = 0;
		}
			
		m_sSectionName = strProgRegKey + "\\" + szNewSection;
		Init();
	}
}

bool CCompRegistrator::RegisterComponent( const char *szProgID, long iProtectionIndex )
{
	CString	sProgID(szProgID);
/*	for( int idx = 0; idx < GetRegistredComponentCount(); idx++ )
	{
		CString	sCompName = GetComponentName( idx );

		if( sProgID == sCompName )
			break;
	}


	CString	s;
	s.Format( "%s%d", szProgIDEntry, idx );

	if( !SUCCEEDED( RegSetValueEx( m_hKey, s, 0, REG_SZ, (byte *)(const char *)sProgID, sProgID.GetLength() ) ) )
		return false;

	s.Format( "%s%d", szProtectionEntry, idx );

	if( !SUCCEEDED( RegSetValueEx( m_hKey, s, 0, REG_DWORD, (byte *)&iProtectionIndex, sizeof(iProtectionIndex) ) ) )
		return false;

	m_lCount = max( idx+1, m_lCount );

	if( !SUCCEEDED( RegSetValueEx( m_hKey, szCountEntry, 0, REG_DWORD, (byte *)&m_lCount, sizeof(m_lCount) ) ) )
		return false;

	return true;*/

	DWORD	dwCompFlag = 0;
	return (::RegSetValueEx(m_hKey, sProgID, 0, REG_DWORD, (byte*)&dwCompFlag, 4) == ERROR_SUCCESS);
}

bool CCompRegistrator::UnRegisterComponent( const char *szProgID )
{
	CString	sProgID(szProgID);

	/*for( int idx = 0; idx < GetRegistredComponentCount(); idx++ )
	{
		CString	sCompName = GetComponentName( idx );

		if( sProgID == sCompName )
			break;
	}

	if( idx == m_lCount )
		return true;

	int	idxSet = idx;

	CString	s;

	s.Format( "%s%d", szProgIDEntry, idxSet );
	RegDeleteValue( m_hKey, s );
	s.Format( "%s%d", szProtectionEntry, idxSet );
	RegDeleteValue( m_hKey, s );

	
	for( ; idx < GetRegistredComponentCount(); idx++ )
	{
		

		CString	sCompID = GetComponentName( idx );
		long	lProt = GetComponentProtIdx( idx );

		if( sCompID.IsEmpty() )
			continue;

		s.Format( "%s%d", szProgIDEntry, idxSet );
	
		if( RegSetValueEx( m_hKey, s, 0, REG_SZ, (byte *)(const char *)sCompID, sCompID.GetLength() ) )
			continue;

		s.Format( "%s%d", szProtectionEntry, idxSet );

		if( RegSetValueEx( m_hKey, s, 0, REG_DWORD, (byte *)&lProt, sizeof( lProt ) ) )
			continue;

		idxSet++;
	}

	m_lCount = idxSet;

	if( !SUCCEEDED( RegSetValueEx( m_hKey, szCountEntry, 0, REG_DWORD, (byte *)&m_lCount, sizeof(m_lCount) ) ) )
		return false;

	return true;*/
	DWORD	dwCompFlag = 0;
	return (::RegDeleteValue(m_hKey, sProgID) == ERROR_SUCCESS);
}

long CCompRegistrator::GetRegistredComponentCount()
{
	DWORD	dwSize = 255;
	char	sz[255];
	int	nCount = 0;
	while (!::RegEnumValue(m_hKey, nCount, sz, &dwSize, 0, 0, 0, 0))
	{
		dwSize = 255;
		nCount++;
	}
	return nCount;
}

CString CCompRegistrator::GetComponentName( int idx )
{
	DWORD	dwSize = 255;
	char	sz[255];
	::RegEnumValue( m_hKey, idx, sz, &dwSize, 0, 0, 0, 0 );

	return sz;

/*	CString	s;
	s.Format( "%s%d", szProgIDEntry, idx );

	char szComponentName[255];
	long	iProtIndex = -1;

	DWORD	dwReadKind = REG_SZ;
	DWORD	dwDataSize = 255;

	if( RegQueryValueEx( m_hKey, s, 0, &dwReadKind, (byte *)szComponentName, &dwDataSize ) )
		return "";

	if( dwReadKind != REG_SZ )
		return "";

	return szComponentName;*/
}

long CCompRegistrator::GetComponentProtIdx( int idx )
{
	DWORD	dwSize = 255;
	long	nData = 0;
	::RegEnumValue( m_hKey, idx, 0, 0, 0, 0, (byte*)&nData, &dwSize );

	return nData;
	/*CString s;

	s.Format( "%s%d", szProtectionEntry, idx );

	long	iProtIndex = -1;
	DWORD	dwDataSize = sizeof( iProtIndex );
	DWORD	dwReadKind = REG_DWORD;
	
	if( RegQueryValueEx( m_hKey, s, 0, &dwReadKind, (byte *)&iProtIndex, &dwDataSize ) )
		return -1;

	if( dwReadKind != REG_DWORD )
		return -1;

	return iProtIndex;*/
}

void CCompRegistrator::Init()
{
	if( m_sSectionName.IsEmpty() )
		return;


	{
		CVTRegKey reg;
		VERIFY(reg.Create(HKEY_LOCAL_MACHINE, m_sSectionName));
	}


	VERIFY(::RegOpenKey(HKEY_LOCAL_MACHINE, m_sSectionName, &m_hKey) == ERROR_SUCCESS);


		

	DWORD	dwKind = REG_DWORD;
	//DWORD	dwDataSize = sizeof( m_lCount );
//	DWORD	dwKind = 0;
//	DWORD	dwDataSize = 0;
							
/*	if( ::RegQueryValueEx( m_hKey, szCountEntry, 0, &dwKind, (byte *)&m_lCount, &dwDataSize ) )
	{
		TRACE( "Counter failed\n" );
		return;
	}*/
}

void CCompRegistrator::DeInit()
{
	if( m_hKey )
		::RegCloseKey( m_hKey );

	//m_lCount = 0;
	m_hKey = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Utility functions

bool RecursiveCreateBranch(HKEY hRoot, LPCTSTR szBranch)
{
	CString strBranch(szBranch);
	CString strKey(szBranch);
	CString strSubKey;

	CVTRegKey reg;
	// if branch exists simply return true
	if (reg.Open(hRoot, strBranch))
		return true;
	
	GetFirstLastKey(strBranch, strKey, strSubKey);

	if (!RecursiveCreateBranch(hRoot, strKey))
		return false;

	if (!reg.Open(hRoot, strKey))
		return false;

	return reg.CreateSubKey(strSubKey);
}

bool GetFirstLastKey(const CString & str, CString & strFirst, CString & strLast)
{
	CString strSrc = str;
	strLast.Empty();
	strFirst.Empty();

	if (strSrc.IsEmpty())
		return true;

	if (strSrc[strSrc.GetLength() - 1] == _T('\\'))
		strSrc.Delete(strSrc.GetLength() - 1, 1);

	int Last = strSrc.ReverseFind(_T('\\'));
	if (Last != -1)
	{
		strLast = strSrc.Right(strSrc.GetLength() - Last - 1);
		strFirst = strSrc.Left(Last);
	}
	else
	{
		strLast = strSrc;
		strFirst.Empty();
	}
	return true;
}

void AddChar(CString & str, TCHAR ch)
{
	if (str.GetLength() && str[str.GetLength() - 1] != ch)
		str += ch;
}

void AddWithCheck(CStringArray & sa, LPCTSTR szString)
{
	CString str(szString);
	if (str.IsEmpty())
		return;

	for (int i = 0; i < sa.GetSize(); i++)
	{
		if (sa[i] == str)
			return;
	}
	sa.Add(str);
}

/////////////////////////////////////////////////////////////////////////////////////////
// class CVTRegValue

CVTRegValue::CVTRegValue(LPCTSTR szName)
{	
	m_strName = szName ? szName : _T("") ;	
	dwType = 0; 
	dwNumber = 0;
	pszString = 0;
    pbyteData = 0;
    dwSize = 0;
	dwLength = 0;
}

CVTRegValue::~CVTRegValue()
{	
	Clear();	
}

// dword
void CVTRegValue::SetDword(DWORD dwnum)
{
	Clear();

	dwNumber = dwnum;
	SetType(REG_DWORD);
}

// binary
void CVTRegValue::SetBinary(LPBYTE lpData, DWORD size)
{
	Clear();
	SetType(REG_BINARY);
		
	if (!lpData)
		return;

	if (!size)
		return;

	pbyteData = new BYTE [size];
	if (!pbyteData)
		return;

	memcpy(pbyteData, lpData, sizeof (BYTE) * size);

	SetSize(size);
}

// string
void CVTRegValue::SetString(LPCTSTR szString, DWORD size)
{
	Clear();
	SetType(REG_SZ);
		
	if (!szString)
	{
//		pszString = 0;
		SetLength(0);
		return;
	}

	if (!size)
		size = lstrlen(szString) + 1; // NULL-terminated simbol

	pszString = new TCHAR [size];
	if (!pszString)
		return;

	lstrcpy(pszString, szString);

	SetLength(size);
}

// clear
void CVTRegValue::Clear()
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
				delete pszString, pszString = 0;
			SetLength(0);
		}
		break;

	case REG_NONE:
		break;
	}	// switch
	SetType(REG_NONE);
}

LPBYTE CVTRegValue::GetData()
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

DWORD CVTRegValue::GetSIZE()
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

void CVTRegValue::SetData(DWORD dwtype, LPBYTE lpData, DWORD dwsize)
{
//	Clear();

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
			SetString((LPCTSTR)lpData, dwsize);
		}
		break;
	}	// switch
}

/*
bool CVTRegValue::ToString(CString & str)
{
	str.Empty();
	if (GetType() == REG_NONE)
		return true;

	// add name and '='
	str = '\"';
	str += m_strName + "\"=";

	bool bRet = true;
	CString strTemp;

	if (GetType() == REG_SZ)
	{
		CString strVal = GetString();
		bRet = StrValToString(strVal, strTemp);
	}
	else if (GetType() == REG_DWORD)
	{
		// add type
		str += REG_FILE_DWORD;
		str += ':';
		DWORD dwTemp = GetDword();
		bRet = DwordToString(dwTemp, strTemp);

	}
	else if (GetType() == REG_BINARY)
	{
		// add type
		str += REG_FILE_BINARY;
		str += ':';
		bRet = BinaryToString(GetBinary(), GetSize(), strTemp);
	}
	// add value to string
	str += strTemp;

	return bRet;
}

bool CVTRegValue::FromString(CString & str)
{
	Clear();
//	SetName("");

	if (str.IsEmpty())
		return true;

	CString strTemp, strName;
	// get name
	if (str[0] != _T('\"'))
		return false;

	int pos = str.Find(_T('\"'), 1);
	if (pos == -1)
		return false;

	while (str[pos - 1] == _T('\\'))
	{
		pos = str.Find(_T('\"'), pos);
		if (pos == -1)
			return false;
	}

	strTemp = str.Mid(0, pos + 1);
	if (!StringToStrVal(strName, strTemp))
		return false;

	SetName(strName);

	// then check string is valid
	if (pos + 2 >= str.GetLength() - 1)
		return false;

	if (str[pos + 1] != _T('='))
		return false;

	/// get value
	bool bRet = true;

	if (str[pos + 2] == _T('\"')) // type is string 
	{
		strTemp = str.Right(str.GetLength() - pos - 2);
		CString strVal;

		if (!StringToStrVal(strVal, strTemp))
			return false;

		SetString(strVal);
	}
	else
	{
		int last = str.Find(':');
		if (last == -1) // empty
			return true;
		
		// get type
		CString strType = str.Mid(pos + 2, last - pos - 2);
		// get string contains value
		strTemp = str.Right(str.GetLength() - last - 1);

		if (0 == strType.CompareNoCase(REG_FILE_DWORD))// type is dword
		{
			DWORD dwTemp = 0;
			if (!StringToDword(dwTemp, strTemp))
				return false;

			SetDword(dwTemp);
		}
		else if (0 == strType.CompareNoCase(REG_FILE_BINARY)) // type is binary
		{
			int len = GetBinaryLenFromString(strTemp);
			LPBYTE lpData = 0;
			if (len)
			{
				LPBYTE lpData = new BYTE [len];
				if (!lpData)
					return false;

				int retLen = 0;
				if (StringToBinary(lpData, len, retLen, strTemp))
				{
					SetBinary(lpData, retLen);
				}
				delete [] lpData;
			}
			else
				SetBinary(0, 0);
		}
		else if (0 == strType.CompareNoCase(REG_FILE_HEX2)) // string in hex 
		{
			int len = GetBinaryLenFromString(strTemp);
			LPBYTE lpData = 0;
			if (len)
			{
				LPBYTE lpData = new BYTE [len];
				if (!lpData)
					return false;

				int retLen = 0;
				if (StringToBinary(lpData, len, retLen, strTemp))
				{
					SetString((LPCTSTR)lpData);
				}
				delete [] lpData;
			}
			else
				SetString(0);
		}
		else
			return false;

	}
	return true;
}
*/

//#define CheckRet(a) ((if (a) return false;))


/////////////////////////////////////////////////////////////////////////////////////////
// class CVTRegKey

CVTRegKey::CVTRegKey()
{
	m_hKey = NULL;
	m_hRoot = NULL;
	m_strKey = _T("");
}

CVTRegKey::CVTRegKey(HKEY hRoot, LPCTSTR szKey)
{
	m_hKey = NULL;
	m_hRoot = CheckRoot(hRoot) ? hRoot : NULL;
	m_strKey = szKey;
}

CVTRegKey::~CVTRegKey()
{
	Close();
}


CVTRegKey::operator HKEY() const
{
	return m_hKey;
}

bool CVTRegKey::SetValue(CVTRegValue & val)
{
	CVTRegValue old;
	old.SetName(val.GetName());

	if (!GetValue(old))
		old.Clear();

	if (!m_hKey && !Create())
		return false;

	bool bRet = (ERROR_SUCCESS == RegSetValueEx(m_hKey, val.GetName(), 0, val.GetType(), 
												val.GetData(), val.GetSIZE()));
	return bRet;
}

bool CVTRegKey::GetValue(CVTRegValue & val)
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
		return false;

	// fill wrapper
	val.SetData(dwType, lpData, cbData);
	// free allocated buffer
	delete [] lpData;

	return true;
}

bool CVTRegKey::Create(HKEY hRoot, LPCTSTR szKey)
{
	if (Open(hRoot, szKey))
		return true;

	DWORD dwDisp = 0;
	if (ERROR_SUCCESS == RegCreateKeyEx(m_hRoot, m_strKey, 0, 0, 0 , KEY_ALL_ACCESS, NULL, &m_hKey, &dwDisp))
		return true;

	if (!RecursiveCreateBranch(m_hRoot, m_strKey))
		return false;

	return Open();
}

bool CVTRegKey::Open(HKEY hRoot, LPCTSTR szKey)
{
	Close();
	if (hRoot && CheckRoot(hRoot))
		m_hRoot = hRoot;

	if (szKey)
		m_strKey = szKey;

	if (!m_hRoot)
		return false;

	//if(m_hRoot==HKEY_CURRENT_USER || m_hRoot==HKEY_CLASSES_ROOT)
	//	return (ERROR_SUCCESS == RegOpenKeyEx(m_hRoot, m_strKey, 0, KEY_READ, &m_hKey));
	//else
		return (ERROR_SUCCESS == RegOpenKeyEx(m_hRoot, m_strKey, 0, KEY_ALL_ACCESS, &m_hKey));

}

bool CVTRegKey::CreateSubKey(LPCTSTR szKey)
{
	CString strSubKey(szKey);
	if (strSubKey.IsEmpty())
		return false;

	if (!m_hKey && !Open())
		return false;

	HKEY hKey = 0;
	DWORD dwDisp = 0;

	if (ERROR_SUCCESS == RegCreateKeyEx(m_hKey, strSubKey, 0, 0, 0 , KEY_ALL_ACCESS, NULL, &hKey, &dwDisp))
	{
		RegCloseKey(hKey);
		return true;
	}
	return false;
}


bool CVTRegKey::Close()
{
	if (!m_hKey)
		return true;
	bool bRet = (ERROR_SUCCESS == RegCloseKey(m_hKey));
	if (bRet)
		m_hKey = 0;
	return bRet;
}

bool CVTRegKey::DeleteKey()
{
	// get last subkey
	CString strFirst, strLast;
	if (!GetFirstLastKey(m_strKey, strFirst, strLast))
		return false;

	// open subkey
	if (!Open(m_hRoot, strFirst))
		return false;

	return DeleteSubKey(strLast);
}

bool CVTRegKey::EnumSubKeys(CStringArray & sa)
{
	if (!m_hKey && !Open())
		return false;

	DWORD dwNumSubKeys = 0;
	DWORD cbMaxSubKeyLen = 0;

	// get info for this key
	if (ERROR_SUCCESS != RegQueryInfoKey(m_hKey, 0, 0, 0, &dwNumSubKeys,
										 &cbMaxSubKeyLen, 0, 0, 0, 0, 0, 0))
		return false;
 
	CString strSubKeyName;
	DWORD	cbSubKeyName = 0;
	LONG	lRet = 0;

	// for all subkeys
	for (int i = 0; i < (int)dwNumSubKeys && lRet != ERROR_NO_MORE_ITEMS; i++)
	{
		FILETIME ft;
		LPTSTR szName = strSubKeyName.GetBuffer(cbMaxSubKeyLen + 1);
		cbSubKeyName = cbMaxSubKeyLen + 1;

		//get subkeys and process it
		LONG lRet = RegEnumKeyEx(m_hKey, (DWORD)i, szName, &cbSubKeyName, 0, 0, 0, &ft);

		strSubKeyName.ReleaseBuffer();

		if (lRet != ERROR_SUCCESS)
			continue;

		AddWithCheck(sa, strSubKeyName);
	}
	return true;
}

bool CVTRegKey::EnumValues(CStringArray & sa)
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

	CString strValueName;
	DWORD	cbValueName = 0;
//	DWORD	dwType = 0;
	
	LONG lRet = 0;
	// for all values 
	for (int i = 0; i < (int)cValues && ERROR_NO_MORE_ITEMS != lRet; i++)
	{
		LPTSTR szName = strValueName.GetBuffer(cbMaxValueNameLen + 1);
		// get value
		cbValueName = cbMaxValueNameLen + 1;
		lRet = RegEnumValue(m_hKey, (DWORD)i, szName, &cbValueName, 0, 0/*&dwType*/, 0, 0);

		strValueName.ReleaseBuffer();

		if (lRet != ERROR_SUCCESS)
			continue;

		AddWithCheck(sa,  strValueName);
	}
	return true;
}



bool CVTRegKey::DeleteValue(CVTRegValue & val)
{
	if (!m_hKey && !Create())
		return false;

	return (ERROR_SUCCESS == RegDeleteValue(m_hKey, (LPTSTR)val.GetName()));
}


bool CVTRegKey::DeleteAllValues()
{
	CStringArray sa;
	if (!EnumValues(sa))
		return false;

	if (!m_hKey && !Open())
		return false;

	bool bRet = true;

	for (int i = 0; i  < sa.GetSize(); i++)
	{
		// fill wrapper
		CVTRegValue val(sa[i]);
		bRet = DeleteValue(val) && bRet;
	}	
	return bRet;
}

bool CVTRegKey::DeleteSubKey(LPCTSTR szSubKey)
{
	CString str(szSubKey);
	if (str.IsEmpty())
		return true;

	if (!m_hKey && !Open())
		return false;

	CVTRegKey reg;
	CString strSubKey = m_strKey;
	AddChar(strSubKey, '\\');
	strSubKey += str;

	if (!reg.Open(m_hRoot, strSubKey))
		return true;

	bool bRet = reg.DeleteAllValues();
	bRet = reg.DeleteAllSubKeys() && bRet;
	reg.Close();

	return (ERROR_SUCCESS == RegDeleteKey(m_hKey, szSubKey)) && bRet;
}

bool CVTRegKey::DeleteAllSubKeys()
{
	if (!m_hKey && !Open())
		return false;

	CStringArray sa;
	bool bRet = true;
	if (!EnumSubKeys(sa))
		bRet = false;

	for (int i = 0; i < sa.GetSize(); i++)
	{
		bRet = DeleteSubKey(sa[i]) && bRet;
	}

	return bRet;
}

bool CVTRegKey::IsExists()
{
	if (m_hKey)
		return true;

	if (!m_hRoot)
		return false;

	HKEY hKey = NULL;
	LONG lRes = RegOpenKeyEx(m_hRoot, m_strKey, 0, KEY_READ, &hKey);
	if (!hKey)
		return false;

	RegCloseKey(hKey);

	return true;
}

bool CVTRegKey::IsExists(HKEY hRootKey, LPCTSTR szPath)
{
	if (!hRootKey || !szPath)
		return false;

	HKEY hKey = NULL;
	LONG lRes = RegOpenKeyEx(hRootKey, szPath, 0, KEY_READ, &hKey);
	if (!hKey || lRes != ERROR_SUCCESS)
		return false;

	RegCloseKey(hKey);

	return true;
}
bool CVTRegKey::IsEmpty()
{
	// if not key is not can not be opened it's means key wasn't be created
	if (!m_hKey && !Open())
		return true;

	DWORD dwNumSubKeys = 0;
	DWORD dwNumValues = 0;

	// get info for this key
	if (ERROR_SUCCESS != RegQueryInfoKey(m_hKey, 0, 0, 0, &dwNumSubKeys, 0, 0, &dwNumValues, 0, 0, 0, 0))
		return false;

	// has subkeys
	if (dwNumSubKeys)
		return false;
	// has values
	if (dwNumValues)
		return false;

	Close();
	return true;
}


bool CVTRegKey::CheckRoot(HKEY hRoot)
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

/*
bool CVTRegKey::ToString(CString & str)
{
	str.Empty();

	CString strTemp;
	if (m_hRoot == HKEY_CLASSES_ROOT)
		str = REG_FILE_KEY_CLASSES_ROOT;

	else if (m_hRoot == HKEY_CURRENT_USER)
		str = REG_FILE_KEY_CURRENT_USER;

	else if (m_hRoot == HKEY_LOCAL_MACHINE)
		str = REG_FILE_KEY_LOCAL_MACHINE;

	else if (m_hRoot == HKEY_USERS)
		str = REG_FILE_KEY_USERS;

	else if (m_hRoot == HKEY_CURRENT_CONFIG)
		str = REG_FILE_KEY_CURRENT_CONFIG;

	else
		return false;

	str += _T('\\');
	str += m_strKey;

	return true;
}

bool CVTRegKey::FromString(CString & str)
{
	Close();

	m_hRoot = 0;
	m_strKey.Empty();

	if (str.IsEmpty())
		return false;

	int pos = str.Find('\\');
	CString strRoot;

	if (pos == -1)
	{
		strRoot = str;
	}
	else
	{
		strRoot = str.Left(pos);
		m_strKey = str.Right(str.GetLength() - pos - 1);
	}

	if (0 == strRoot.CompareNoCase(REG_FILE_KEY_CLASSES_ROOT))
	{
		m_hRoot = HKEY_CLASSES_ROOT;
	}
	else if (0 == strRoot.CompareNoCase(REG_FILE_KEY_CURRENT_USER))
	{
		m_hRoot = HKEY_CURRENT_USER;
	}
	else if (0 == strRoot.CompareNoCase(REG_FILE_KEY_LOCAL_MACHINE))
	{
		m_hRoot = HKEY_LOCAL_MACHINE;
	}
	else if (0 == strRoot.CompareNoCase(REG_FILE_KEY_USERS))
	{
		m_hRoot = HKEY_USERS;
	}
	else if (0 == strRoot.CompareNoCase(REG_FILE_KEY_CURRENT_CONFIG))
	{
		m_hRoot = HKEY_CURRENT_CONFIG;
	}
	else 
		return false;

	return true;
}
*/


/*
/////////////////////////////////////////////////////////////////////////////////////////
// class CVTRegFile

CVTRegFile::CVTRegFile(LPCTSTR szFileName)
{
	m_strFileName = szFileName;
}

CVTRegFile::~CVTRegFile()
{
	if (m_File.m_hFile != (UINT)NULL && m_File.m_hFile != (UINT)INVALID_HANDLE_VALUE)
		m_File.Close();
}


// save registry branch to array 
bool CVTRegFile::SaveRegistry(HKEY hKey, LPCTSTR szSubKey)
{
	CString strKey(szSubKey);

	if (m_arr.GetSize())
		m_arr.RemoveAll();

	if (hKey == 0 || strKey.IsEmpty())
		return false;

	return ::ProcessRegistry(hKey, strKey, m_arr);
}

// load registry from array
bool CVTRegFile::LoadRegistry()
{
	if (!m_arr.GetSize())
		return true;

	return ::ProcessArray(m_arr);
}


// create reg file and save string array to it
bool CVTRegFile::SaveFile(LPCTSTR szFileName)
{
	CString strName(szFileName ? szFileName : m_strFileName);
	bool bRet = true;

	try
	{
		if (m_File.m_hFile != (UINT)NULL && m_File.m_hFile != (UINT)INVALID_HANDLE_VALUE)
			m_File.Close();

		if (!strName.IsEmpty() && strName != m_strFileName)
			m_strFileName = strName;


		CFileException fe;
		if (!m_File.Open(m_strFileName, CFile::modeCreate|CFile::modeReadWrite|CFile::typeText, &fe))
			AfxThrowFileException(fe.m_cause, fe.m_lOsError, fe.m_strFileName);
		
		// save reg file signature
		CString strFileSig(REG_FILE_SIG);
		::WriteString(m_File, strFileSig);
		::WriteString(m_File, CString(""));

		if (m_arr.GetSize())
		{
			bRet = ::SaveArray(m_File, m_arr);
		}

		m_File.Close();
	}
	catch(...)
	{
		if (m_File.m_hFile != (UINT)NULL && m_File.m_hFile != (UINT)INVALID_HANDLE_VALUE)
			m_File.Close();

		return false;
	}
	return bRet;
}

// read reg.file and load string aray from it
bool CVTRegFile::LoadFile(LPCTSTR szFileName)
{
	CString strName(szFileName ? szFileName : m_strFileName);
	bool bRet = true;

	try
	{
		if (m_File.m_hFile != (UINT)NULL && m_File.m_hFile != (UINT)INVALID_HANDLE_VALUE)
			m_File.Close();

		if (!strName.IsEmpty() && strName != m_strFileName)
			m_strFileName = strName;

		CFileException fe;
		if (!m_File.Open(m_strFileName, CFile::modeRead|CFile::shareDenyWrite|CFile::typeText, &fe))
			AfxThrowFileException(fe.m_cause, fe.m_lOsError, fe.m_strFileName);

		m_arr.RemoveAll();

		CString str, strFileSig(REG_FILE_SIG);
		m_File.ReadString(str);
		
		if (str != strFileSig)
		{
			m_File.Close();
			return false;
		}

		bRet = ::LoadArray(m_File, m_arr);

		m_File.Close();
	}
	catch(...)
	{
		if (m_File.m_hFile != (UINT)NULL && m_File.m_hFile != (UINT)INVALID_HANDLE_VALUE)
			m_File.Close();

		return false;
	}

	return bRet;
}

// check reg file signature
bool CVTRegFile::Init()
{
	try
	{
		if (m_File.m_hFile != (UINT)NULL && m_File.m_hFile != (UINT)INVALID_HANDLE_VALUE)
			m_File.Close();

		if (m_strFileName.IsEmpty())
			return false;
			
		CFileException fe;
		if (!m_File.Open(m_strFileName, CFile::modeCreate|CFile::modeReadWrite|CFile::typeText, &fe))
			AfxThrowFileException(fe.m_cause, fe.m_lOsError, fe.m_strFileName);

		CString str;
		m_File.ReadString(str);
		
		CString strFileSig(REG_FILE_SIG);

		m_File.Close();

		if (str != strFileSig)
			return false;
	}
	catch(...)
	{
		if (m_File.m_hFile != (UINT)NULL && m_File.m_hFile != (UINT)INVALID_HANDLE_VALUE)
			m_File.Close();

		return false;
	}
	return true;
}
*/


/*
/////////////////////////////////////////////////////////////////////////////////////////
// util serialization functions

bool DwordToString(DWORD & dwVal, CString & str)
{
	str.Empty();
	str.Format("%08x", dwVal);
	return true;
}

bool StringToDword(DWORD & dwVal, CString & str)
{
	dwVal = (DWORD)((str.IsEmpty()) ? 0 : atol(str));
	return true;
}

bool BinaryToString(LPBYTE lpbVal, DWORD size, CString & str)
{
	if (size && !AfxIsValidAddress(lpbVal, size, false))
		return false;

	str.Empty();
	CString strNum;
	
	for (int i = 0; i < (int)size; i++)
	{
		strNum.Format("%2x", lpbVal[i]);
		str += strNum;

		if (i != (int)size - 1)
			str += _T(',');
	}
	return true;
}

bool StringToBinary(LPBYTE lpbVal, int nSize, int & nRetSize, CString & str)
{
	if (!nSize)
		return false;

	if (!AfxIsValidAddress(lpbVal, nSize, true))
		return false;

	ZeroMemory(lpbVal, sizeof(BYTE) * nSize);
	nRetSize = 0;

	if (str.IsEmpty())
		return true;

	int prev = 0;
	int pos = str.Find(_T(','));

	while (pos != -1)
	{
		int nval = atoi(str.Mid(prev, pos - prev));
		lpbVal[nRetSize++] = (BYTE)nval;
		
		if (nRetSize == nSize)
			return true;

		prev = pos + 1;
		pos = str.Find(_T(','), pos);
	}

	if (nRetSize == nSize)
		return true;

	// last Value
	int nval = atoi(str.Mid(prev));
	lpbVal[nRetSize++] = (BYTE)nval;

	return true;
}

int GetBinaryLenFromString(CString & str)
{
	if (str.IsEmpty())
		return 0;

	int nCount = 0;
	int pos = str.Find(_T(','));

	while (pos != -1)
	{
		nCount++;
		pos = str.Find(_T(','), pos);
	}
	return ++nCount;
}

bool StrValToString(CString & strString, CString & str)
{
	str.Empty();
	
	CString strTemp;
	if (strString.IsEmpty())
	{
		str = _T("\"\"");
		return true;
	}

	strTemp = _T("\""); // first '\"'

	for (int i = 0; i < strString.GetLength(); i++)
	{
		TCHAR ch = strString[i];
		// add additional '\\'
		if (ch == _T('\\') || ch == _T('\"') || ch == _T('\''))
			strTemp += _T('\\');

		strTemp += ch;
	}

	strTemp += _T("\""); // last '\"'
	str = strTemp;

	return true;
}

bool StringToStrVal(CString & strString, CString & str)
{
	CString strTemp;
	if (str.IsEmpty())
		return false;

	strString.Empty();
	// skip first & last simbols ( it's '\"' )
	for (int i = 1; i < str.GetLength() - 1; i++)
	{
		TCHAR ch = str[i];
		// skip additional '\\'
		if (ch == _T('\\') && (str[i + 1]  == _T('\\') || str[i + 1] == _T('\"') || str[i + 1] == _T('\'')))
			continue;

		strTemp += ch;
	}
	
	strString = strTemp; 
	return true;
}


// check string contains last simbol == '\\' 
bool NeedNextLine(CString & str)
{
	if (str.GetLength() && str[str.GetLength() - 1] == _T('\\'))
		return true;
	return false;
}

// read string from file w/ continues read if it's neccessary
bool ReadString(CStdioFile & File, CString & str)
{
	try
	{
		str.Empty();
		if (File.m_hFile == (UINT)NULL || File.m_hFile == (UINT)INVALID_HANDLE_VALUE)
			return false;

		CString strRead;
		if (!File.ReadString(strRead))
			return false;

		while (NeedNextLine(strRead))
		{
			strRead = strRead.Left(strRead.GetLength() - 1);

			str += strRead;
			if (!File.ReadString(strRead))
				return false;
		}
		str += strRead;
	}
	catch(...)
	{
		return false;
	}
	return true;
}

// write string to file //w/ continues write if it's neccessary
bool WriteString(CStdioFile & File, CString & str)
{
	try
	{
		if (File.m_hFile == (UINT)NULL || File.m_hFile == (UINT)INVALID_HANDLE_VALUE)
			return false;

		str += _T("\n");
		File.WriteString(str);
	}
	catch(...)
	{
		return false;
	}
	return true;

}

bool GetKeyFromString(HKEY & hRoot, CString & strKey, CString & str)
{
	hRoot = 0;
	strKey.Empty();

	if (str.IsEmpty())
		return false;

	if (str[0] != '[' && str[str.GetLength() - 1] != ']')
		return false;

	str = str.Mid(1, str.GetLength() - 2);
	
	int pos = str.Find('\\');
	if (pos == -1)
		return false;
	
	CString strRoot = str.Left(pos);
	strKey = str.Right(str.GetLength() - pos - 1);

	if (0 == strRoot.CompareNoCase(REG_FILE_KEY_CLASSES_ROOT))
	{
		hRoot = HKEY_CLASSES_ROOT;
	}
	else if (0 == strRoot.CompareNoCase(REG_FILE_KEY_CURRENT_USER))
	{
		hRoot = HKEY_CURRENT_USER;
	}
	else if (0 == strRoot.CompareNoCase(REG_FILE_KEY_LOCAL_MACHINE))
	{
		hRoot = HKEY_LOCAL_MACHINE;
	}
	else if (0 == strRoot.CompareNoCase(REG_FILE_KEY_USERS))
	{
		hRoot = HKEY_USERS;
	}
	else if (0 == strRoot.CompareNoCase(REG_FILE_KEY_CURRENT_CONFIG))
	{
		hRoot = HKEY_CURRENT_CONFIG;
	}
	else 
		return false;

	return true;
}

bool GetStringFromKey(HKEY hRoot, CString & strKey, CString & str)
{
	str.Empty();

	CString strTemp;
	if (hRoot == HKEY_CLASSES_ROOT)
		strTemp = REG_FILE_KEY_CLASSES_ROOT;

	else if (hRoot == HKEY_CURRENT_USER)
		strTemp = REG_FILE_KEY_CURRENT_USER;

	else if (hRoot == HKEY_LOCAL_MACHINE)
		strTemp = REG_FILE_KEY_LOCAL_MACHINE;

	else if (hRoot == HKEY_USERS)
		strTemp = REG_FILE_KEY_USERS;

	else if (hRoot == HKEY_CURRENT_CONFIG)
		strTemp = REG_FILE_KEY_CURRENT_CONFIG;

	else
		return false;

	strTemp += _T('\\');
	strTemp += strKey;

	str = _T('[');
	str += strTemp;
	str += _T(']');

	return true;
}


bool IsSection(CString & str)
{
	if (!str.IsEmpty() && str[0] == _T('[') && str[str.GetLength() - 1] == _T(']'))
		return true;

	return false;
}

bool IsComment(CString & str)
{
	if (str.IsEmpty())
		return true;

	if (str[0] == _T(';'))
		return true;

	CString strSig(REG_FILE_SIG);
	if (str == strSig)
		return true;

	return false;
}


bool LoadArray(CStdioFile & File, CStringArray & sa)
{
	try
	{
		if (File.m_hFile == (UINT)NULL || File.m_hFile == (UINT)INVALID_HANDLE_VALUE)
			return false;

		sa.RemoveAll();

		CString	s;

		while (1)
		{
			if (!ReadString(File, s))
				break;

//			if (!IsComment(s))
				sa.Add(s);
		}
	}
	catch(...)
	{
		return false;
	}
	return true;
}

bool SaveArray(CStdioFile & File, CStringArray & sa)
{
	try
	{

		if (File.m_hFile == (UINT)NULL || File.m_hFile == (UINT)INVALID_HANDLE_VALUE)
			return false;

		for (int i = 0; i < sa.GetSize(); i++)
		{
			if (!WriteString(File, sa[i]))
				return false;
		}
	}
	catch(...)
	{
		return false;
	}
	return true;
}


bool ProcessArray(CStringArray & sa)
{
	if (!sa.GetSize())
		return true;

	bool bRet = true;

	HKEY hRoot = 0, hCurKey = 0;
	CString strKey;
	
	for (int i = 0; i < sa.GetSize(); i++)
	{
		// if string is section
		if (IsSection(sa[i]))
		{
			CString str = sa[i];
			// get root hkey and key string
			if (!GetKeyFromString(hRoot, strKey, str))
				continue;

			// Create this key and set as default for createing values
			if (hCurKey)
			{
				RegCloseKey(hCurKey);
				hCurKey = 0;
			}

			if (!RecursiveCreateBranch(hRoot, strKey))
			{
				bRet = false;
				return false;
			}

			DWORD dwDisp = 0;
			if (ERROR_SUCCESS != RegCreateKeyEx(hRoot, strKey, 0, 0, 
												REG_OPTION_BACKUP_RESTORE, 
												KEY_ALL_ACCESS, 0, 
												&hCurKey, &dwDisp))
			{
				bRet = false;
			}
		}
		else if (IsComment(sa[i]) || sa[i].IsEmpty()) // skip comments and empty strings
			continue;
		else // it must be reg value
		{
			
			// add values for default key if it's exists
			if (!hCurKey)
			{
				bRet = false;
				continue;
			}
			
			// load RegValue from string
			CVTRegValue Value;

			CString str = sa[i];
			if (!Value.FromString(str))
			{
				bRet = false;
				continue;
			}
			// create value
			if (ERROR_SUCCESS != RegSetValueEx(hCurKey, Value.GetName(), 0, Value.GetType(), Value.GetData(), Value.GetSIZE()))
			{
				bRet = false;
			}
		}
	}
	if (hCurKey)
	{
		RegCloseKey(hCurKey);
		hCurKey = 0;
	}

	return bRet;
}


bool ProcessRegistry(HKEY hRoot, LPCTSTR szSubKey, CStringArray & sa)
{
	bool	bRet = true;
	CString strKey(szSubKey);

	HKEY	hKey = 0;
	DWORD	dwDisp = 0;
	CString strTemp;

	if (hRoot == 0 || strKey.IsEmpty())
		return false;

	if (ERROR_SUCCESS != RegCreateKeyEx(hRoot, strKey, 0, 0, 0,
//										REG_OPTION_BACKUP_RESTORE, 
										KEY_ALL_ACCESS|KEY_QUERY_VALUE, 0, 
										&hKey, &dwDisp))
	{
//		ReportError(GetLastError());
		return false;
	}

	// convert key to string
	if (!::GetStringFromKey(hRoot, strKey, strTemp))
	{
		RegCloseKey(hKey);
		return false;
	}
	// add empty string before key
	sa.Add(CString(_T("")));
	// add key to array
	sa.Add(strTemp);

	DWORD dwNumSubKeys = 0;
	DWORD cbMaxSubKeyLen = 0;
	DWORD cValues = 0;
	DWORD cbMaxValueNameLen = 0;
	DWORD cbMaxValueLen = 0;
	// get info for this key
	if (ERROR_SUCCESS != RegQueryInfoKey(hKey, 0, 0, 0, &dwNumSubKeys,
										 &cbMaxSubKeyLen, 0, &cValues, 
										 &cbMaxValueNameLen, &cbMaxValueLen, 0, 0))
	{
		bRet = false;
	}
 

	CString strValueName;
	LPBYTE	lpData = new BYTE [cbMaxValueLen]; // address of buffer for value data
	DWORD	cbValueName = 0;
	DWORD	dwType = 0;
	DWORD   cbData = 0;

	if (!lpData)
	{
		RegCloseKey(hKey);
		return false;
	}
	
	LONG lRet = 0;
	// for all values 
	for (int i = 0; i < (int)cValues && ERROR_NO_MORE_ITEMS != lRet; i++)
	{
		LPTSTR szName = strValueName.GetBuffer(cbMaxValueNameLen + 1);
		// get value
		cbValueName = cbMaxValueNameLen + 1;
		cbData = cbMaxValueLen;
		lRet = RegEnumValue(hKey, (DWORD)i, szName, &cbValueName, 0, &dwType, lpData, &cbData);

		strValueName.ReleaseBuffer();

		if (lRet != ERROR_SUCCESS)// && lRet != ERROR_NO_MORE_ITEMS && lRet != ERROR_MORE_DATA)
		{
//			ReportError(lRet);
			bRet = false;
			continue;
		}
 
		// fill wrapper
		CVTRegValue val;
		val.SetName(strValueName);
		val.SetData(dwType, lpData, cbData);

		// convert value to string
		if (!val.ToString(strTemp))
		{
			bRet = false;
			continue;
		}

		// and add it to string array
		sa.Add(strTemp);
	}
 
	delete [] lpData;
	CString strSubKeyName;
	DWORD	cbSubKeyName = 0;

	lRet = 0;
	// for all subkeys
	for (i = 0; i < (int)dwNumSubKeys && lRet != ERROR_NO_MORE_ITEMS; i++)
	{
		FILETIME ft;
		LPTSTR szName = strSubKeyName.GetBuffer(cbMaxSubKeyLen + 1);
		cbSubKeyName = cbMaxSubKeyLen + 1;

		//get subkeys and process it
		LONG lRet = RegEnumKeyEx(hKey, (DWORD)i, szName, &cbSubKeyName, 0, 0, 0, &ft);

		strSubKeyName.ReleaseBuffer();

		if (lRet != ERROR_SUCCESS)// && lRet != ERROR_NO_MORE_ITEMS && lRet != ERROR_MORE_DATA)
		{
//			ReportError(lRet);
			bRet = false;
			continue;
		}
		strTemp = strKey + _T('\\');
		strTemp += strSubKeyName;
		bRet = ProcessRegistry(hRoot, strTemp, sa) && bRet;
	}
	
	RegCloseKey(hKey);

	return bRet;
}
*/

