//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************

/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998 by Shane Martin
// All rights reserved
//
// Distribute freely, except: don't remove my name from the source or
// documentation (don't take credit for my work), mark your changes (don't
// get me blamed for your possible bugs), don't alter or remove this
// notice.
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc., and
// I'll try to keep a version up to date.  I can be reached as follows:
//    shane.kim@kaiserslautern.netsurf.de
/////////////////////////////////////////////////////////////////////////////

// last revised: 24 Apr 98
// Registry.cpp : implementation file
//
// Description:
// CBCGRegistry is a wrapper for the Windows Registry API.  It allows
//  easy modification of the Registry with easy to remember terms like
//  Read, Write, Open, and Close.

#include "stdafx.h"
#include "BCGRegistry.h"

#define READ_ONLY_KEYS	(KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_NOTIFY)

CBCGRegistry::CBCGRegistry(BOOL bAdmin, BOOL bReadOnly) :
	m_bReadOnly (bReadOnly)
{
	m_hKey = bAdmin ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
}

CBCGRegistry::~CBCGRegistry()
{
	Close();
}

BOOL CBCGRegistry::VerifyKey (LPCTSTR pszPath)
{
	ASSERT (m_hKey);

	CString strPath = pszPath;
	int iPathLen = strPath.GetLength ();
	if (iPathLen > 0 && strPath [iPathLen - 1] == _T('\\'))
	{
		strPath = strPath.Left (iPathLen - 1);
	}

	LONG ReturnValue = RegOpenKeyEx (m_hKey, strPath, 0L,
		m_bReadOnly ? READ_ONLY_KEYS : KEY_ALL_ACCESS, &m_hKey);
	
	m_Info.lMessage = ReturnValue;
	m_Info.dwSize = 0L;
	m_Info.dwType = 0L;

	if(ReturnValue == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

BOOL CBCGRegistry::VerifyValue (LPCTSTR pszValue)
{
	ASSERT(m_hKey);
	LONG lReturn = RegQueryValueEx(m_hKey, pszValue, NULL,
		NULL, NULL, NULL);

	m_Info.lMessage = lReturn;
	m_Info.dwSize = 0L;
	m_Info.dwType = 0L;

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL CBCGRegistry::CreateKey (LPCTSTR pszPath)
{
	if (m_bReadOnly)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	ASSERT (pszPath != NULL);
	HKEY hKeySave = m_hKey;

	CString strPath = pszPath;
	int iPathLen = strPath.GetLength ();
	if (iPathLen > 0 && strPath [iPathLen - 1] == _T('\\'))
	{
		strPath = strPath.Left (iPathLen - 1);
	}

	DWORD dw;

	LONG ReturnValue = RegCreateKeyEx (m_hKey, strPath, 0L, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, 
		&m_hKey, &dw);

	m_Info.lMessage = ReturnValue;
	m_Info.dwSize = 0L;
	m_Info.dwType = 0L;

	if(ReturnValue == ERROR_SUCCESS)
		return TRUE;

	TRACE(_T("Can't create registry key: %s\n"), strPath);
	m_hKey = hKeySave;
	return FALSE;
}

BOOL CBCGRegistry::Open (LPCTSTR pszPath)
{
	ASSERT (pszPath != NULL);

	HKEY hKeySave = m_hKey;
	m_sPath = pszPath;

	CString strPath = pszPath;
	int iPathLen = strPath.GetLength ();
	if (iPathLen > 0 && strPath [iPathLen - 1] == _T('\\'))
	{
		strPath = strPath.Left (iPathLen - 1);
	}

	LONG ReturnValue = RegOpenKeyEx (m_hKey, strPath, 0L,
		m_bReadOnly ? READ_ONLY_KEYS : KEY_ALL_ACCESS, &m_hKey);

	m_Info.lMessage = ReturnValue;
	m_Info.dwSize = 0L;
	m_Info.dwType = 0L;

	if(ReturnValue == ERROR_SUCCESS)
		return TRUE;

	m_hKey = hKeySave;
	return FALSE;
}

void CBCGRegistry::Close()
{
	if (m_hKey)
	{
		RegCloseKey (m_hKey);
		m_hKey = NULL;
	}
}

BOOL CBCGRegistry::Write (LPCTSTR pszKey, int iVal)
{
	if (m_bReadOnly)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	DWORD dwValue;

	ASSERT(m_hKey);
	ASSERT(pszKey);
	
	dwValue = (DWORD)iVal;
	LONG ReturnValue = RegSetValueEx (m_hKey, pszKey, 0L, REG_DWORD,
		(CONST BYTE*) &dwValue, sizeof(DWORD));

	m_Info.lMessage = ReturnValue;
	m_Info.dwSize = sizeof(DWORD);
	m_Info.dwType = REG_DWORD;

	if(ReturnValue == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

BOOL CBCGRegistry::Write (LPCTSTR pszKey, DWORD dwVal)
{
	if (m_bReadOnly)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	ASSERT(m_hKey);
	ASSERT(pszKey);
	return RegSetValueEx (m_hKey, pszKey, 0L, REG_DWORD,
		(CONST BYTE*) &dwVal, sizeof(DWORD));
}

BOOL CBCGRegistry::Write (LPCTSTR pszKey, LPCTSTR pszData)
{
	if (m_bReadOnly)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	ASSERT(m_hKey);
	ASSERT(pszKey);
	ASSERT(pszData);
	ASSERT(AfxIsValidAddress(pszData, _tcslen(pszData), FALSE));

	LONG ReturnValue = RegSetValueEx (m_hKey, pszKey, 0L, REG_SZ,
		(CONST BYTE*) pszData, _tcslen(pszData) + 1);

	m_Info.lMessage = ReturnValue;
	m_Info.dwSize = _tcslen(pszData) + 1;
	m_Info.dwType = REG_SZ;

	if(ReturnValue == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

BOOL CBCGRegistry::Write (LPCTSTR pszKey, CStringList& scStringList)
{
	if (m_bReadOnly)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	CMemFile file(byData, iMaxChars, 16);
	CArchive ar(&file, CArchive::store);
	ASSERT(scStringList.IsSerializable());
	scStringList.Serialize(ar);
	ar.Close();
	const DWORD dwLen = file.GetLength();
	ASSERT(dwLen < iMaxChars);
	LONG lReturn = RegSetValueEx(m_hKey, pszKey, 0, REG_BINARY,
		file.Detach(), dwLen);
	
	m_Info.lMessage = lReturn;
	m_Info.dwSize = dwLen;
	m_Info.dwType = REG_BINARY;

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

BOOL CBCGRegistry::Write (LPCTSTR pszKey, CObList& list)
{
	if (m_bReadOnly)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	BOOL bRes = FALSE;
	try
	{
		CMemFile file;

		{
			CArchive ar (&file, CArchive::store);

			list.Serialize (ar);
			ar.Flush ();
		}

		DWORD dwDataSize = file.GetLength ();
		LPBYTE lpbData = file.Detach ();

		if (lpbData == NULL)
		{
			return FALSE;
		}

		bRes = Write (pszKey, lpbData, (UINT) dwDataSize);
		free (lpbData);
	}
	catch (CMemoryException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("Memory exception in CBCGRegistry::Write ()!\n"));
		return FALSE;
	}

	return bRes;
}

BOOL CBCGRegistry::Write (LPCTSTR pszKey, CObject& obj)
{
	if (m_bReadOnly)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	BOOL bRes = FALSE;
	try
	{
		CMemFile file;

		{
			CArchive ar (&file, CArchive::store);

			obj.Serialize (ar);
			ar.Flush ();
		}

		DWORD dwDataSize = file.GetLength ();
		LPBYTE lpbData = file.Detach ();

		if (lpbData == NULL)
		{
			return FALSE;
		}

		bRes = Write (pszKey, lpbData, (UINT) dwDataSize);
		free (lpbData);
	}
	catch (CMemoryException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("Memory exception in CBCGRegistry::Write ()!\n"));
		return FALSE;
	}

	return bRes;
}

BOOL CBCGRegistry::Write (LPCTSTR pszKey, CByteArray& bcArray)
{
	if (m_bReadOnly)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	CMemFile file(byData, iMaxChars, 16);
	CArchive ar(&file, CArchive::store);
	ASSERT(bcArray.IsSerializable());
	bcArray.Serialize(ar);
	ar.Close();
	const DWORD dwLen = file.GetLength();
	ASSERT(dwLen < iMaxChars);
	LONG lReturn = RegSetValueEx(m_hKey, pszKey, 0, REG_BINARY,
		file.Detach(), dwLen);
	
	m_Info.lMessage = lReturn;
	m_Info.dwSize = dwLen;
	m_Info.dwType = REG_BINARY;

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

BOOL CBCGRegistry::Write (LPCTSTR pszKey, CDWordArray& dwcArray)
{
	ASSERT (m_hKey != NULL);
	ASSERT (pszKey != NULL);

	if (m_bReadOnly)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	BOOL bRes = FALSE;
	try
	{
		CMemFile file;

		{
			CArchive ar (&file, CArchive::store);
			dwcArray.Serialize(ar);
			ar.Flush ();
		}

		DWORD dwDataSize = file.GetLength ();
		LPBYTE lpbData = file.Detach ();

		if (lpbData == NULL)
		{
			return FALSE;
		}

		bRes = Write (pszKey, lpbData, (UINT) dwDataSize);
		free (lpbData);
	}
	catch (CMemoryException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("Memory exception in CBCGRegistry::Write ()!\n"));
		return FALSE;
	}

	return bRes;
/*
	const int iMaxChars = 4096;
	LPBYTE lpData = new BYTE [iMaxChars];
	ASSERT (lpData != NULL);

	CMemFile file (lpData, iMaxChars, 16);
	CArchive ar(&file, CArchive::store);
	ASSERT(dwcArray.IsSerializable());

	dwcArray.Serialize(ar);
	ar.Close();
	const DWORD dwLen = file.GetLength();
	ASSERT(dwLen < iMaxChars);
	LONG lReturn = RegSetValueEx(m_hKey, pszKey, 0, REG_BINARY,
		file.Detach(), dwLen);
	
	m_Info.lMessage = lReturn;
	m_Info.dwSize = dwLen;
	m_Info.dwType = REG_BINARY;

	delete lpData;
	return (lReturn == ERROR_SUCCESS);*/
}

BOOL CBCGRegistry::Write (LPCTSTR pszKey, CWordArray& wcArray)
{
	if (m_bReadOnly)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	BOOL bRes = FALSE;
	try
	{
		CMemFile file;

		{
			CArchive ar (&file, CArchive::store);

			ar << wcArray.GetSize ();
			for (int i = 0; i < wcArray.GetSize (); i ++)
			{
				ar << wcArray [i];
			}

			ar.Flush ();
		}

		DWORD dwDataSize = file.GetLength ();
		LPBYTE lpbData = file.Detach ();

		if (lpbData == NULL)
		{
			return FALSE;
		}

		bRes = Write (pszKey, lpbData, (UINT) dwDataSize);
		free (lpbData);
	}
	catch (CMemoryException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("Memory exception in CBCGRegistry::Write ()!\n"));
		return FALSE;
	}

	return bRes;
}

BOOL CBCGRegistry::Write (LPCTSTR pszKey, CStringArray& scArray)
{
	if (m_bReadOnly)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	CMemFile file(byData, iMaxChars, 16);
	CArchive ar(&file, CArchive::store);
	ASSERT(scArray.IsSerializable());
	scArray.Serialize(ar);
	ar.Close();
	const DWORD dwLen = file.GetLength();
	ASSERT(dwLen < iMaxChars);
	LONG lReturn = RegSetValueEx(m_hKey, pszKey, 0, REG_BINARY,
		file.Detach(), dwLen);
	
	m_Info.lMessage = lReturn;
	m_Info.dwSize = dwLen;
	m_Info.dwType = REG_BINARY;

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

BOOL CBCGRegistry::Write(LPCTSTR pszKey, const CRect& rect)
{
	if (m_bReadOnly)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	BOOL bRes = FALSE;
	try
	{
		CMemFile file;

		{
			CArchive ar (&file, CArchive::store);

			ar << rect;
			ar.Flush ();
		}

		DWORD dwDataSize = file.GetLength ();
		LPBYTE lpbData = file.Detach ();

		if (lpbData == NULL)
		{
			return FALSE;
		}

		bRes = Write (pszKey, lpbData, (UINT) dwDataSize);
		free (lpbData);
	}
	catch (CMemoryException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("Memory exception in CBCGRegistry::Write ()!\n"));
		return FALSE;
	}

	return bRes;
}

BOOL CBCGRegistry::Write(LPCTSTR pszKey, LPPOINT& lpPoint)
{
	if (m_bReadOnly)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 20;
	CDWordArray dwcArray;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	dwcArray.SetSize(5);
	dwcArray.SetAt(0, lpPoint->x);
	dwcArray.SetAt(1, lpPoint->y);

	CMemFile file(byData, iMaxChars, 16);
	CArchive ar(&file, CArchive::store);
	ASSERT(dwcArray.IsSerializable());
	dwcArray.Serialize(ar);
	ar.Close();
	const DWORD dwLen = file.GetLength();
	ASSERT(dwLen < iMaxChars);
	LONG lReturn = RegSetValueEx(m_hKey, pszKey, 0, REG_BINARY,
		file.Detach(), dwLen);
	
	m_Info.lMessage = lReturn;
	m_Info.dwSize = dwLen;
	m_Info.dwType = REG_POINT;

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

BOOL CBCGRegistry::Write (LPCTSTR pszKey, LPBYTE pData, UINT nBytes)
{
	if (m_bReadOnly)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	ASSERT (m_hKey != NULL);
	ASSERT (pszKey != NULL);
	ASSERT (pData != NULL);
	ASSERT (AfxIsValidAddress (pData, nBytes, FALSE));

	LONG lResult = ::RegSetValueEx (m_hKey, pszKey, NULL, REG_BINARY,
									pData, nBytes);

	m_Info.lMessage = lResult;
	m_Info.dwSize = nBytes;
	m_Info.dwType = REG_BINARY;

	return (lResult == ERROR_SUCCESS);
}

BOOL CBCGRegistry::Read(LPCTSTR pszKey, int& iVal)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);

	DWORD dwType;
	DWORD dwSize = sizeof (DWORD);
	DWORD dwDest;

	LONG lReturn = RegQueryValueEx (m_hKey, (LPTSTR) pszKey, NULL,
		&dwType, (BYTE *) &dwDest, &dwSize);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwSize;

	if(lReturn == ERROR_SUCCESS)
	{
		iVal = (int)dwDest;
		return TRUE;
	}

	return FALSE;
}

BOOL CBCGRegistry::Read (LPCTSTR pszKey, DWORD& dwVal)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);

	DWORD dwType;
	DWORD dwSize = sizeof (DWORD);
	DWORD dwDest;

	LONG lReturn = RegQueryValueEx (m_hKey, (LPTSTR) pszKey, NULL, 
		&dwType, (BYTE *) &dwDest, &dwSize);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwSize;

	if(lReturn == ERROR_SUCCESS)
	{
		dwVal = dwDest;
		return TRUE;
	}

	return FALSE;
}

BOOL CBCGRegistry::Read (LPCTSTR pszKey, CString& sVal)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);

	DWORD dwType;
	DWORD dwSize = 200;
	TCHAR  szString[255];

	LONG lReturn = RegQueryValueEx (m_hKey, (LPTSTR) pszKey, NULL,
		&dwType, (BYTE *) szString, &dwSize);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwSize;

	if(lReturn == ERROR_SUCCESS)
	{
		sVal = szString;
		return TRUE;
	}

	return FALSE;
}

BOOL CBCGRegistry::Read (LPCTSTR pszKey, CStringList& scStringList)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	DWORD dwType;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	LONG lReturn = RegQueryValueEx(m_hKey, pszKey, NULL, &dwType,
		byData, &dwData);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwData;

	if(lReturn == ERROR_SUCCESS && dwType == REG_BINARY)
	{
		ASSERT(dwData < iMaxChars);
		CMemFile file(byData, dwData);
		CArchive ar(&file, CArchive::load);
		ar.m_bForceFlat = FALSE;
		ASSERT(ar.IsLoading());
		ASSERT(scStringList.IsSerializable());
		scStringList.RemoveAll();
		scStringList.Serialize(ar);
		ar.Close();
		file.Close();
	}

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL CBCGRegistry::Read (LPCTSTR pszKey, CByteArray& bcArray)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	int OldSize = bcArray.GetSize();
	DWORD dwType;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	LONG lReturn = RegQueryValueEx(m_hKey, pszKey, NULL, &dwType,
		byData, &dwData);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwData;

	if(lReturn == ERROR_SUCCESS && dwType == REG_BINARY)
	{
		ASSERT(dwData < iMaxChars);
		CMemFile file(byData, dwData);
		CArchive ar(&file, CArchive::load);
		ar.m_bForceFlat = FALSE;
		ASSERT(ar.IsLoading());
		ASSERT(bcArray.IsSerializable());
		bcArray.RemoveAll();
		bcArray.SetSize(10);
		bcArray.Serialize(ar);
		bcArray.SetSize(OldSize);
		ar.Close();
		file.Close();
	}

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL CBCGRegistry::Read (LPCTSTR pszKey, CDWordArray& dwcArray)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
//	int OldSize = dwcArray.GetSize();
	DWORD dwType;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	LONG lReturn = RegQueryValueEx(m_hKey, pszKey, NULL, &dwType,
		byData, &dwData);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwData;

	if(lReturn == ERROR_SUCCESS && dwType == REG_BINARY)
	{
		ASSERT(dwData < iMaxChars);
		CMemFile file(byData, dwData);
		CArchive ar(&file, CArchive::load);
		ar.m_bForceFlat = FALSE;
		ASSERT(ar.IsLoading());
		ASSERT(dwcArray.IsSerializable());
		dwcArray.RemoveAll();
//		dwcArray.SetSize(10);
		dwcArray.SetSize(0);
		dwcArray.Serialize(ar);
//		dwcArray.SetSize(OldSize);
		ar.Close();
		file.Close();
	}

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL CBCGRegistry::Read (LPCTSTR pszKey, CWordArray& wcArray)
{
	wcArray.SetSize (0);

	BOOL	bSucess = FALSE;
	BYTE*	pData = NULL;
	UINT	uDataSize;

	if (!Read (pszKey, &pData, &uDataSize))
	{
		ASSERT (pData == NULL);
		return FALSE;
	}

	ASSERT (pData != NULL);

	try
	{
		CMemFile file (pData, uDataSize);
		CArchive ar (&file, CArchive::load);

		int iSize;
		ar >> iSize;

		wcArray.SetSize (iSize);
		for (int i = 0; i < iSize; i ++)
		{
			ar >> wcArray [i];
		}

		bSucess = TRUE;
	}
	catch (CMemoryException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("Memory exception in CBCGRegistry::Read ()!\n"));
	}
	catch (CArchiveException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("CArchiveException exception in CBCGRegistry::Read ()!\n"));
	}

	delete pData;
	return bSucess;
}

BOOL CBCGRegistry::Read (LPCTSTR pszKey, CStringArray& scArray)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	int OldSize = scArray.GetSize();
	DWORD dwType;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	LONG lReturn = RegQueryValueEx(m_hKey, pszKey, NULL, &dwType,
		byData, &dwData);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwData;

	if(lReturn == ERROR_SUCCESS && dwType == REG_BINARY)
	{
		ASSERT(dwData < iMaxChars);
		CMemFile file(byData, dwData);
		CArchive ar(&file, CArchive::load);
		ar.m_bForceFlat = FALSE;
		ASSERT(ar.IsLoading());
		ASSERT(scArray.IsSerializable());
		scArray.RemoveAll();
		scArray.SetSize(10);
		scArray.Serialize(ar);
		scArray.SetSize(OldSize);
		ar.Close();
		file.Close();
	}

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL CBCGRegistry::Read(LPCTSTR pszKey, CRect& rect)
{
	BOOL	bSucess = FALSE;
	BYTE*	pData = NULL;
	UINT	uDataSize;

	if (!Read (pszKey, &pData, &uDataSize))
	{
		ASSERT (pData == NULL);
		return FALSE;
	}

	ASSERT (pData != NULL);

	try
	{
		CMemFile file (pData, uDataSize);
		CArchive ar (&file, CArchive::load);

		ar >> rect;
		bSucess = TRUE;
	}
	catch (CMemoryException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("Memory exception in CBCGRegistry::Read ()!\n"));
	}
	catch (CArchiveException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("CArchiveException exception in CBCGRegistry::Read ()!\n"));
	}

	delete pData;
	return bSucess;
}

BOOL CBCGRegistry::Read(LPCTSTR pszKey, LPPOINT& lpPoint)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 20;
	CDWordArray dwcArray;
	DWORD dwType;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	LONG lReturn = RegQueryValueEx(m_hKey, pszKey, NULL, &dwType,
		byData, &dwData);

	if(lReturn == ERROR_SUCCESS && dwType == REG_BINARY)
	{
		ASSERT(dwData < iMaxChars);
		CMemFile file(byData, dwData);
		CArchive ar(&file, CArchive::load);
		ar.m_bForceFlat = FALSE;
		ASSERT(ar.IsLoading());
		ASSERT(dwcArray.IsSerializable());
		dwcArray.RemoveAll();
		dwcArray.SetSize(5);
		dwcArray.Serialize(ar);
		ar.Close();
		file.Close();
		lpPoint->x = dwcArray.GetAt(0);
		lpPoint->y = dwcArray.GetAt(1);
	}

	m_Info.lMessage = lReturn;
	m_Info.dwType = REG_POINT;
	m_Info.dwSize = sizeof(POINT);

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL CBCGRegistry::Read (LPCTSTR pszKey, BYTE** ppData, UINT* pBytes)
{
	ASSERT (m_hKey != NULL);
	ASSERT (pszKey != NULL);
	ASSERT(ppData != NULL);
	ASSERT(pBytes != NULL);
	*ppData = NULL;
	*pBytes = 0;

	DWORD dwType, dwCount;
	LONG lResult = ::RegQueryValueEx (m_hKey, pszKey, NULL, &dwType,
		NULL, &dwCount);

	if (lResult == ERROR_SUCCESS)
	{
		*pBytes = dwCount;
		ASSERT (dwType == REG_BINARY);

		*ppData = new BYTE [*pBytes];

		lResult = ::RegQueryValueEx (m_hKey, pszKey, NULL, &dwType,
			*ppData, &dwCount);

		if (lResult == ERROR_SUCCESS)
		{
			ASSERT (dwType == REG_BINARY);
		}
		else
		{
			delete [] *ppData;
			*ppData = NULL;
		}
	}

	m_Info.lMessage = lResult;
	m_Info.dwType = REG_BINARY;
	m_Info.dwSize = *pBytes;

	return (lResult == ERROR_SUCCESS);
}

BOOL CBCGRegistry::Read (LPCTSTR pszKey, CObList& list)
{
	BOOL	bSucess = FALSE;
	BYTE*	pData = NULL;
	UINT	uDataSize;

	while (!list.IsEmpty ())
	{
		delete list.RemoveHead ();
	}

	if (!Read (pszKey, &pData, &uDataSize))
	{
		ASSERT (pData == NULL);
		return FALSE;
	}

	ASSERT (pData != NULL);

	try
	{
		CMemFile file (pData, uDataSize);
		CArchive ar (&file, CArchive::load);

		list.Serialize (ar);
		bSucess = TRUE;
	}
	catch (CMemoryException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("Memory exception in CBCGRegistry::Read ()!\n"));
	}
	catch (CArchiveException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("CArchiveException exception in CBCGRegistry::Read ()!\n"));
	}

	delete pData;
	return bSucess;
}

BOOL CBCGRegistry::Read (LPCTSTR pszKey, CObject& obj)
{
	BOOL	bSucess = FALSE;
	BYTE*	pData = NULL;
	UINT	uDataSize;

	if (!Read (pszKey, &pData, &uDataSize))
	{
		ASSERT (pData == NULL);
		return FALSE;
	}

	ASSERT (pData != NULL);

	try
	{
		CMemFile file (pData, uDataSize);
		CArchive ar (&file, CArchive::load);

		obj.Serialize (ar);
		bSucess = TRUE;
	}
	catch (CMemoryException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("Memory exception in CBCGRegistry::Read ()!\n"));
	}
	catch (CArchiveException* pEx)
	{
		pEx->Delete ();
		TRACE(_T("CArchiveException exception in CBCGRegistry::Read ()!\n"));
	}

	delete pData;
	return bSucess;
}

BOOL CBCGRegistry::DeleteValue (LPCTSTR pszValue)
{
	ASSERT(m_hKey);
	LONG lReturn = RegDeleteValue(m_hKey, pszValue);


	m_Info.lMessage = lReturn;
	m_Info.dwType = 0L;
	m_Info.dwSize = 0L;

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL CBCGRegistry::DeleteKey (LPCTSTR pszPath, BOOL bAdmin)
{
	//--------------------------
	// Enhanced by Erwin Tratar
	//--------------------------
	if (m_bReadOnly)
	{
		return FALSE;
	}
	
	ASSERT (pszPath != NULL);
	
	CString strPath = pszPath;

	int iPathLen = strPath.GetLength ();
	if (iPathLen > 0 && strPath [iPathLen - 1] == _T('\\'))
	{
		strPath = strPath.Left (iPathLen - 1);
	}
	
	// open the key
	HKEY hSubKey;
	LONG lReturn = ::RegOpenKeyEx (bAdmin ? HKEY_LOCAL_MACHINE :
	HKEY_CURRENT_USER,
		strPath, 0L, KEY_ALL_ACCESS, &hSubKey);
	
	if(lReturn != ERROR_SUCCESS)
		return FALSE;
	
	// first, delete all subkeys (else it won't work on NT!)
	for( DWORD dwSubKeys = 1; dwSubKeys > 0; )
	{
		dwSubKeys = 0;
		
		// first get an info about this subkey ...
		DWORD dwSubKeyLen;
		if( ::RegQueryInfoKey( hSubKey, 0,0,0, &dwSubKeys, &dwSubKeyLen,
			0,0,0,0,0,0) != ERROR_SUCCESS)
		{
			::RegCloseKey(hSubKey);
			return FALSE;
		}
		
		if( dwSubKeys > 0 )
		{
			// call DeleteKey() recursivly
			LPTSTR szSubKeyName = new TCHAR[dwSubKeyLen + 1];
			
			if( ::RegEnumKey( hSubKey, 0, szSubKeyName, dwSubKeyLen+1) !=
				ERROR_SUCCESS
				|| ! DeleteKey( strPath + "\\" + szSubKeyName, bAdmin ) )
			{
				delete szSubKeyName;
				::RegCloseKey(hSubKey);
				return FALSE;
			}
			
			delete szSubKeyName;
		}
	}
	::RegCloseKey(hSubKey);
	
	// finally delete the whole key
	lReturn = ::RegDeleteKey (bAdmin ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER,
		strPath);
	m_Info.lMessage = lReturn;
	m_Info.dwType = 0L;
	m_Info.dwSize = 0L;
	
	if(lReturn == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

BOOL CBCGRegistry::ReadSubKeys(CStringArray& SubKeys)
{
	BOOL result = TRUE;
	DWORD rc = ERROR_SUCCESS;
	TCHAR szSubKey[ 1024  ] = _T("\0");
	DWORD length = sizeof( szSubKey );

	ASSERT(m_hKey);

	int index = 0;
	rc = RegEnumKeyEx(m_hKey, index, szSubKey, &length, NULL, NULL, NULL, NULL);

	if( rc == ERROR_NO_MORE_ITEMS) {
		result = false;
	}
	else while(rc == ERROR_SUCCESS) {
		SubKeys.Add( szSubKey );		
		length = sizeof( szSubKey );
		index++;
		rc = RegEnumKeyEx(m_hKey, index, szSubKey, &length, NULL, NULL, NULL, NULL);		
		
	} // while
	return( result );
}


BOOL CBCGRegistry::ReadKeyValues(CStringArray &Values)
{
	DWORD rc = ERROR_SUCCESS;
	BOOL result = FALSE;
	TCHAR szValue[ 1024 ];
	DWORD length = sizeof( szValue );
	int index = 0;

	ASSERT(m_hKey);

	rc = RegEnumValue(m_hKey, index, szValue, &length, NULL, NULL, NULL, NULL);
	if( rc == ERROR_NO_MORE_ITEMS) {
	  result = FALSE;
	}
	else while( rc == ERROR_SUCCESS )  {
		result = TRUE;
		Values.Add( szValue );
		length = sizeof( szValue );
		index++;
		rc = RegEnumValue(m_hKey, index, szValue, &length, NULL, NULL, NULL, NULL);		
	}
	return( result );

}
