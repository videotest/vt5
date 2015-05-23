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

// KeyHelper.cpp: implementation of the CKeyHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KeyHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKeyHelper::CKeyHelper(LPACCEL lpAccel) :
	m_lpAccel (lpAccel)
{
}
//*******************************************************************
CKeyHelper::CKeyHelper() :
	m_lpAccel (NULL)
{
}
//*******************************************************************
CKeyHelper::~CKeyHelper()
{
}
//*******************************************************************
void CKeyHelper::Format (CString& str) const
{
	str.Empty ();

	if (m_lpAccel == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	if (m_lpAccel->fVirt & FCONTROL)
	{
		AddVirtKeyStr (str, VK_CONTROL);
	}

	if (m_lpAccel->fVirt & FSHIFT)
	{
		AddVirtKeyStr (str, VK_SHIFT);
	}

	if (m_lpAccel->fVirt & FALT)
	{
		AddVirtKeyStr (str, VK_MENU);
	}

	if (m_lpAccel->fVirt & FVIRTKEY)
	{
		AddVirtKeyStr (str, m_lpAccel->key, TRUE);
	}
	else
	{
		str += (char) m_lpAccel->key;
	}
}
//******************************************************************
void CKeyHelper::AddVirtKeyStr (CString& str, UINT uiVirtKey, BOOL bLast) const
{
	//
	// This file was modified by Sven Ritter
	//

	#define BUFFER_LEN 50
	TCHAR szBuffer [BUFFER_LEN + 1];
	
	UINT nScanCode = ::MapVirtualKeyEx (uiVirtKey, 0, 
		::GetKeyboardLayout (0)) <<16 | 0x1;
	
	if (uiVirtKey >= VK_PRIOR && uiVirtKey <= VK_HELP)
	{
		nScanCode |= 0x01000000;
	}
	
	::GetKeyNameText (nScanCode, szBuffer, BUFFER_LEN);
	
	CString strKey(szBuffer);
	strKey.MakeLower();
	
	//--------------------------------------
	// The first letter should be uppercase:
	//--------------------------------------
	for (int nCount = 0; nCount < strKey.GetLength(); nCount++)
	{
		TCHAR c = strKey[nCount];
		if (IsCharLower (c))
		{
//			CharUpper (&c);
			c = (TCHAR) toupper (c); // Convert single character JY 4-Dec-99
			strKey.SetAt (nCount, c);
			break;
		}
	}

	str += strKey;
	
	if (!bLast)
	{
		str += '+';
	}
}

