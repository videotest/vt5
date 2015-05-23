#include "stdafx.h"



#define MAX_STRING_LENGTH 1024


BOOL SetRegKeyValue(
       LPTSTR pszKey,
       LPTSTR pszSubkey,
       LPTSTR pszValue)
{
  BOOL bOk = FALSE;
  LONG ec;
  HKEY hKey;
  TCHAR szKey[MAX_STRING_LENGTH];

  lstrcpy(szKey, pszKey);

  if (NULL != pszSubkey)
  {
    lstrcat(szKey, TEXT("\\"));
    lstrcat(szKey, pszSubkey);
  }

  ec = RegCreateKeyEx(
         HKEY_CLASSES_ROOT,
         szKey,
         0,
         NULL,
         REG_OPTION_NON_VOLATILE,
         KEY_ALL_ACCESS,
         NULL,
         &hKey,
         NULL);

  if (ERROR_SUCCESS == ec)
  {
    if (NULL != pszValue)
    {
      ec = RegSetValueEx(
             hKey,
             NULL,
             0,
             REG_SZ,
             (BYTE *)pszValue,
             (lstrlen(pszValue)+1)*sizeof(TCHAR));
    }
    if (ERROR_SUCCESS == ec)
      bOk = TRUE;
    RegCloseKey(hKey);
  }

  return bOk;
}


#define GUID_SIZE		128

BOOL _RegisterServer( CLSID clsid, char* pszComponentName, char* pszProgID, char* pszProgIDVer )
{
	BOOL  bOk = TRUE;
	TCHAR szID[GUID_SIZE+1];
	TCHAR szCLSID[GUID_SIZE+32];
	TCHAR szModulePath[MAX_PATH];

	// Obtain the path to this module's executable file for later use.
	GetModuleFileName(
		GetModuleHandle( NULL ),
		szModulePath,
		sizeof(szModulePath)/sizeof(TCHAR));

	/*-------------------------------------------------------------------------
	Create registry entries for the Albom4 Component.
	-------------------------------------------------------------------------*/

	OLECHAR bstrTemp[GUID_SIZE+1];	

	// Create some base key strings.
	StringFromGUID2( clsid, bstrTemp, GUID_SIZE);
	_bstr_t _bstr(bstrTemp);
	strcpy( szID, (const char*)_bstr );

	lstrcpy(szCLSID, TEXT("CLSID\\"));
	lstrcat(szCLSID, szID);


	// Create ProgID keys.
	SetRegKeyValue(
		TEXT(pszProgIDVer),
		NULL,
		TEXT(pszComponentName));
		SetRegKeyValue(
		TEXT(pszProgIDVer),
		TEXT("CLSID"),
		szID);

	// Create VersionIndependentProgID keys.
	SetRegKeyValue(
		TEXT(pszProgID),
		NULL,
		TEXT(pszComponentName));
		SetRegKeyValue(
		TEXT(pszProgID),
		TEXT("CurVer"),
		TEXT(pszProgIDVer));
		SetRegKeyValue(
		TEXT(pszProgID),
		TEXT("CLSID"),
		szID);

	// Create entries under CLSID.
	SetRegKeyValue(
		szCLSID,
		NULL,
		TEXT(pszComponentName));
		SetRegKeyValue(
		szCLSID,
		TEXT("ProgID"),
		TEXT(pszProgIDVer));
		SetRegKeyValue(
		szCLSID,
		TEXT("VersionIndependentProgID"),
		TEXT(pszProgID));
		SetRegKeyValue(
		szCLSID,
		TEXT("NotInsertable"),
		NULL);
		SetRegKeyValue(
		szCLSID,
		TEXT("LocalServer32"),
		szModulePath);


	return bOk;
}


/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   CMainWindow::UnregisterServer

  Summary:  Member function used by this server to unregister itself from
            the system Registry.

  Args:     void.

  Returns:  BOOL
              TRUE
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
BOOL _UnRegisterServer( CLSID clsid, char* pszComponentName, char* pszProgID, char* pszProgIDVer )
{
	BOOL  bOk = TRUE;
	TCHAR szID[GUID_SIZE+1];
	TCHAR szCLSID[GUID_SIZE+32];
	TCHAR szTemp[MAX_PATH+GUID_SIZE];

	/*-------------------------------------------------------------------------
	Delete registry entries for the Albom4 Component
	-------------------------------------------------------------------------*/
	//Create some base key strings.
	OLECHAR bstrTemp[GUID_SIZE+1];	
	StringFromGUID2( clsid, bstrTemp, GUID_SIZE);

	_bstr_t _bstr(bstrTemp);
	strcpy( szID, (const char*)_bstr );

	lstrcpy(szCLSID, TEXT("CLSID\\"));
	lstrcat(szCLSID, szID);

	RegDeleteKey(HKEY_CLASSES_ROOT, TEXT( (const char*)(_bstr_t(pszProgID) + _bstr_t("\\CurVer") ) ) );
	RegDeleteKey(HKEY_CLASSES_ROOT, TEXT( (const char*)(_bstr_t(pszProgID) + _bstr_t("\\CLSID") ) ) );
	RegDeleteKey(HKEY_CLASSES_ROOT, TEXT(pszProgID));

	RegDeleteKey(HKEY_CLASSES_ROOT, TEXT( (const char*)(_bstr_t(pszProgIDVer) + _bstr_t("\\CLSID") ) ) );
	RegDeleteKey(HKEY_CLASSES_ROOT, TEXT(pszProgIDVer));

	wsprintf(szTemp, TEXT("%s\\%s"), szCLSID, TEXT("ProgID"));
	RegDeleteKey(HKEY_CLASSES_ROOT, szTemp);

	wsprintf(szTemp, TEXT("%s\\%s"), szCLSID, TEXT("VersionIndependentProgID"));
	RegDeleteKey(HKEY_CLASSES_ROOT, szTemp);

	wsprintf(szTemp, TEXT("%s\\%s"), szCLSID, TEXT("NotInsertable"));
	RegDeleteKey(HKEY_CLASSES_ROOT, szTemp);

	wsprintf(szTemp, TEXT("%s\\%s"), szCLSID, TEXT("LocalServer32"));
	RegDeleteKey(HKEY_CLASSES_ROOT, szTemp);

	RegDeleteKey(HKEY_CLASSES_ROOT, szCLSID);


  return bOk;
}
