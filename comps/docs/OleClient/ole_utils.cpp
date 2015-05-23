#include "stdafx.h"
#include "ole_utils.h"
#include "misc_utils.h"





IOleFramePtr GetMainFrame()
{
	IUnknown	*punkMF = NULL;
	punkMF = _GetOtherComponent( GetAppUnknown(), IID_IMainWindow );

	IOleFramePtr	ptr( punkMF );
	if( punkMF )
		punkMF->Release();

	return ptr;

}


// extracts icon resource ID and path name from registry "file.exe,35" format
void GetIconInfo(LPCTSTR lpszRegInfo, LPTSTR lpszImagePath,
	UINT& nIndex)
{
	LPTSTR pstrTarget = lpszImagePath;
	LPCTSTR pstrSource = lpszRegInfo;
	while (*pstrSource != ',' && *pstrSource != '\0')
	{
		*pstrTarget = *pstrSource;
		pstrTarget = _tcsinc(pstrTarget);
		pstrSource = _tcsinc(pstrSource);
	}
	*pstrTarget = '\0';

	// extract the index
	if (*pstrSource != '\0')
	{
		LPTSTR pstrIndex = _tcsinc(pstrSource);
		nIndex = (UINT) _ttol(pstrIndex);
	}
	else
		nIndex = 0;
}


HICON GetIconFromRegistry( HMODULE hInst, CLSID& clsid )
{
	HICON hIcon = NULL;
	HRESULT hr;
	OLECHAR *szCLSID;
	DWORD dwType = 0;
	TCHAR szName[MAX_PATH+1];
	TCHAR szPathName[MAX_PATH+1];
	HKEY hkeyObj;
	HKEY hkeyDefIcon;
	HKEY hkeyCLSID;
	UINT nIndex;

	hr = ::StringFromCLSID(clsid, &szCLSID);
	if (!SUCCEEDED(hr))
		return NULL;	

	// first, try for the real icon
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, _T("clsid"), 0, KEY_READ, &hkeyCLSID) == ERROR_SUCCESS)
	{
		if (RegOpenKeyEx(hkeyCLSID, _bstr_t(szCLSID), 0, KEY_READ, &hkeyObj) == ERROR_SUCCESS)
		{
			if (RegOpenKeyEx(hkeyObj, _T("DefaultIcon"), 0, KEY_READ, &hkeyDefIcon) == ERROR_SUCCESS)
			{
				DWORD dwCount;
				dwCount = sizeof(szName);
				if (RegQueryValueEx(hkeyDefIcon, NULL, NULL, &dwType, (BYTE*) szName, &dwCount) == ERROR_SUCCESS)
				{
					GetIconInfo(szName, szPathName, nIndex);
					// Load the icon
					hIcon = ::ExtractIcon( hInst, szPathName, nIndex);

					// ExtractIcon() failure case means NULL return
					if (int(hIcon) == 1)
						hIcon = NULL;
				}
				RegCloseKey(hkeyDefIcon);
			}
			RegCloseKey(hkeyObj);
		}
		RegCloseKey(hkeyCLSID);
	}

	// if we didn't get the real icon, try the default icon
	if (hIcon == NULL)
	{
		if (RegOpenKeyEx(HKEY_CLASSES_ROOT, _T("DocShortcut"), 0, KEY_READ,&hkeyObj) == ERROR_SUCCESS)
		{
			if (RegOpenKeyEx(hkeyObj, _T("DefaultIcon"), 0, KEY_READ, &hkeyDefIcon) == ERROR_SUCCESS)
			{
				DWORD dwCount;
				dwCount = sizeof(szName);
				if (RegQueryValueEx(hkeyDefIcon, NULL, NULL, &dwType, (BYTE*) szName, &dwCount) == ERROR_SUCCESS)
				{
					GetIconInfo(szName, szPathName, nIndex);

					// Load the icon
					hIcon = ::ExtractIcon( hInst, szPathName, nIndex);

					// ExtractIcon() failure case means NULL return
					if (int(hIcon) == 1)
						hIcon = NULL;
				}
				RegCloseKey(hkeyDefIcon);
			}
			RegCloseKey(hkeyObj);
		}
	}

	::CoTaskMemFree(szCLSID);
	return hIcon;
}
