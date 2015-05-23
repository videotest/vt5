#include "StdAfx.h"
//#include "Common.h"
#include "Input.h"
#include "VT5Profile.h"
#if !defined(COMMON1)
#include <ComDef.h>
#include "misc_utils.h"
#include "data5.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CVT5ProfileManager::CVT5ProfileManager(LPCTSTR lpDrvName, bool bSingleProfile) : CStdProfileManager(bSingleProfile),
	m_sDriverName(lpDrvName)
{
}

bool CVT5ProfileManager::IsVT5Profile()
{
	IUnknown *punk = GetAppUnknown();
	return punk!=NULL;
}

VARIANT CVT5ProfileManager::_GetProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, bool bIgnoreMethodic)
{
	sptrINamedData sptrND(GetAppUnknown());
	CString sSecName(_T("Input"));
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		BSTR bstr;
		sptrDM->GetSectionName(&bstr);
		sSecName = bstr;
		::SysFreeString(bstr);
	}
	VARIANT var;
	::VariantInit(&var);
	sSecName += CString("\\")+m_sDriverName;
	sSecName += CString("\\")+SectionName(lpSection,bIgnoreMethodic);
	CString sKeyName(lpEntry);
	sptrND->SetupSection(sSecName.AllocSysString());
	sptrND->GetValue(sKeyName.AllocSysString(), &var);
	return var;
}

VARIANT CVT5ProfileManager::GetDefaultProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry)
{
	sptrINamedData sptrND(GetAppUnknown());
	CString sSecName(_T("Input"));
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		BSTR bstr;
		sptrDM->GetSectionName(&bstr);
		sSecName = bstr;
		::SysFreeString(bstr);
	}
	VARIANT var;
	::VariantInit(&var);
	sSecName += CString("\\")+m_sDriverName;
//	if (lpSection)
		sSecName += CString("\\")+lpSection;
	sSecName += "-Default";
	CString sKeyName(lpEntry);
	sptrND->SetupSection(sSecName.AllocSysString());
	sptrND->GetValue(sKeyName.AllocSysString(), &var);
	return var;
}

VARIANT CVT5ProfileManager::GetProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, bool bIgnoreMethodic)
{
	VARIANT var = _GetProfileValue(lpSection, lpEntry, bIgnoreMethodic);
	if (var.vt == VT_EMPTY)
		var = GetDefaultProfileValue(lpSection, lpEntry);
	return var;
}

void CVT5ProfileManager::WriteProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, VARIANT var, bool bIgnoreMethodic)
{
	sptrINamedData sptrND(GetAppUnknown());
	CString sSecName(_T("Input"));
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		BSTR bstr;
		sptrDM->GetSectionName(&bstr);
		sSecName = bstr;
		::SysFreeString(bstr);
	}
	sSecName += CString("\\")+m_sDriverName;
//	if (lpSection)
		sSecName += CString("\\")+SectionName(lpSection,bIgnoreMethodic);
	CString sKeyName(lpEntry);
	sptrND->SetupSection(sSecName.AllocSysString());
	sptrND->SetValue(sKeyName.AllocSysString(), var);
}

int CVT5ProfileManager::_GetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bIgnoreMethodic)
{
	if (IsVT5Profile())
	{
		VARIANT var = _GetProfileValue(lpSection, lpEntry);
		int nRet = nDef;
		if (var.vt == VT_I2)
			nRet = var.iVal;
		else if (var.vt == VT_I4)
			nRet = var.lVal;
		::VariantClear(&var);
		return nRet;
	}
	else
		return CStdProfileManager::_GetProfileInt(lpSection, lpEntry, nDef, bIgnoreMethodic);
}

double CVT5ProfileManager::_GetProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed, bool bIgnoreMethodic)
{
	if (IsVT5Profile())
	{
		VARIANT var = _GetProfileValue(lpSection, lpEntry);
		double dRet = dDef;
		if (var.vt == VT_R4)
			dRet = var.fltVal;
		else if (var.vt == VT_R8)
			dRet = var.dblVal;
		if (pbDefValueUsed)
			*pbDefValueUsed = var.vt!=VT_R4&&var.vt!=VT_R8;
		::VariantClear(&var);
		return dRet;
	}
	else
		return CStdProfileManager::_GetProfileDouble(lpSection, lpEntry, dDef, pbDefValueUsed, bIgnoreMethodic);
}

CString CVT5ProfileManager::_GetProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault, bool bIgnoreMethodic)
{
	if (IsVT5Profile())
	{
		VARIANT var = _GetProfileValue(lpSection, lpEntry);
		CString sRet(lpDefault);
		if (var.vt == VT_BSTR)
			sRet = var.bstrVal;
		::VariantClear(&var);
		return sRet;
	}
	else
		return CStdProfileManager::_GetProfileString(lpSection, lpEntry, lpDefault, bIgnoreMethodic);
}

int CVT5ProfileManager::GetDefaultProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef)
{
	if (IsVT5Profile())
	{
		VARIANT var = GetDefaultProfileValue(lpSection, lpEntry);
		int nRet = nDef;
		if (var.vt == VT_I2)
			nRet = var.iVal;
		else if (var.vt == VT_I4)
			nRet = var.lVal;
		::VariantClear(&var);
		return nRet;
	}
	else
		return CStdProfileManager::GetDefaultProfileInt(lpSection, lpEntry, nDef);
}

double CVT5ProfileManager::GetDefaultProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed)
{
	if (IsVT5Profile())
	{
		VARIANT var = GetDefaultProfileValue(lpSection, lpEntry);
		double dRet = dDef;
		if (var.vt == VT_R4)
			dRet = var.fltVal;
		else if (var.vt == VT_R8)
			dRet = var.dblVal;
		if (pbDefValueUsed)
			*pbDefValueUsed = var.vt!=VT_R4&&var.vt!=VT_R8;
		::VariantClear(&var);
		return dRet;
	}
	else
		return CStdProfileManager::GetDefaultProfileDouble(lpSection, lpEntry, dDef, pbDefValueUsed);
}

CString CVT5ProfileManager::GetDefaultProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault)
{
	if (IsVT5Profile())
	{
		VARIANT var = GetDefaultProfileValue(lpSection, lpEntry);
		CString sRet(lpDefault);
		if (var.vt == VT_BSTR)
			sRet = var.bstrVal;
		::VariantClear(&var);
		return sRet;
	}
	else
		return CStdProfileManager::GetDefaultProfileString(lpSection, lpEntry, lpDefault);
}

void CVT5ProfileManager::WriteProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nValue, bool bIgnoreMethodic)
{
	if (IsVT5Profile())
	{
		VARIANT var;
		var.vt = VT_I4;
		var.lVal = nValue;
		WriteProfileValue(lpSection, lpEntry, var);
	}
	else
		CStdProfileManager::WriteProfileInt(lpSection, lpEntry, nValue, bIgnoreMethodic);
}

void CVT5ProfileManager::WriteProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dValue, bool bIgnoreMethodic)
{
	if (IsVT5Profile())
	{
		VARIANT var;
		var.vt = VT_R8;
		var.dblVal = dValue;
		WriteProfileValue(lpSection, lpEntry, var);
	}
	else
		CStdProfileManager::WriteProfileDouble(lpSection, lpEntry, dValue, bIgnoreMethodic);
}

void CVT5ProfileManager::WriteProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpValue, bool bIgnoreMethodic)
{
	if (IsVT5Profile())
	{
		CString s(lpValue);
		VARIANT var;
		var.vt = VT_BSTR;
		var.bstrVal = s.AllocSysString();
		WriteProfileValue(lpSection, lpEntry, var);
	}
	else
		CStdProfileManager::WriteProfileString(lpSection, lpEntry, lpValue, bIgnoreMethodic);
}

static void CopySection(CString sDriverName, CString sSec)
{
	sptrINamedData sptrND(GetAppUnknown());
	CString sSecName(_T("Input"));
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		BSTR bstr;
		sptrDM->GetSectionName(&bstr);
		sSecName = bstr;
		::SysFreeString(bstr);
	}
	sSecName += CString("\\")+sDriverName;
	if (!sSec.IsEmpty())
		sSecName += CString("\\")+sSec;
	CString sSecDefault(sSecName);
	sSecDefault += "-Default";
	sptrND->SetupSection(sSecDefault.AllocSysString());
	long nCount;
	HRESULT hr = sptrND->GetEntriesCount(&nCount);
	if (SUCCEEDED(hr))
	{
		for (int i = 0; i < nCount; i++)
		{
			BSTR bstr;
			sptrND->SetupSection(sSecDefault.AllocSysString());
			sptrND->GetEntryName(i, &bstr);
			VARIANT var;
			VariantInit(&var);
			sptrND->GetValue(bstr, &var);
			sptrND->SetupSection(sSecName.AllocSysString());
			sptrND->SetValue(bstr, var);
		}
	}
}

void CVT5ProfileManager::InitSettings()
{
}

void CVT5ProfileManager::ResetSettings()
{
	CopySection(m_sDriverName, _T("Settings"));
	CopySection(m_sDriverName, _T("Source"));
	CopySection(m_sDriverName, _T("Format"));
	CopySection(m_sDriverName, _T("AVI"));
}


