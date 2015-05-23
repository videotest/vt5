#pragma once

inline DWORD _GetClassifierString(const char *pszSection, const char *pszEntry, const char *pszDefault, char *pszBuffer, DWORD nBufSize, const char *pszPathName)
{
	if (pszPathName)
	{
		DWORD dwRet = GetPrivateProfileString(pszSection, pszEntry, 0, pszBuffer, nBufSize, pszPathName);
		if (dwRet) return dwRet;
	}
	_variant_t varDef;
	_variant_t var = ::GetValue(GetAppUnknown(), pszSection, pszEntry, varDef);
	if (var.vt == VT_BSTR)
	{
		char *p = _com_util::ConvertBSTRToString(var.bstrVal);
		strncpy(pszBuffer, p, nBufSize);
		DWORD dwRet = strlen(p);
		delete p;
		return dwRet;
	}
	if (pszPathName)
		WritePrivateProfileString(pszSection, pszEntry, pszDefault, pszPathName);
	else
		::SetValue(GetAppUnknown(), pszSection, pszEntry, pszDefault);
	strncpy(pszBuffer, pszDefault, nBufSize);
	return strlen(pszDefault);
};

inline int _GetClassifierInt(const char *pszSection, const char *pszEntry, int nDefault, const char *pszPathName)
{
	char szData[255];
	if (pszPathName && GetPrivateProfileString(pszSection, pszEntry, 0, szData, 255, pszPathName) != 0)
		return atoi(szData);
	else
	{
		_variant_t varDef;
		_variant_t var = ::GetValue(GetAppUnknown(), pszSection, pszEntry, varDef);
		if (var.vt == VT_I2)
			return var.iVal;
		else if (var.vt == VT_I4)
			return var.lVal;
		else if (pszPathName)
		{
			_itoa(nDefault,szData,10);
			WritePrivateProfileString(pszSection, pszEntry, szData, pszPathName);
		}
		else
			::SetValue(GetAppUnknown(), pszSection, pszEntry, (long)nDefault);
		return nDefault;
	}
};

inline double _GetClassifierDouble(const char *pszSection, const char *pszEntry, double dDefault, const char *pszPathName)
{
	char szData[255];
	if (pszPathName && GetPrivateProfileString(pszSection, pszEntry, 0, szData, 255, pszPathName) != 0)
		return atof(szData);
	else
	{
		_variant_t varDef;
		_variant_t var = ::GetValue(GetAppUnknown(), pszSection, pszEntry, varDef);
		if (var.vt == VT_R4)
			return var.fltVal;
		else if (var.vt == VT_R8)
			return var.dblVal;
		else if (pszPathName)
		{
			_gcvt(dDefault,3,szData);
			WritePrivateProfileString(pszSection, pszEntry, szData, pszPathName);
		}
		else
			::SetValue(GetAppUnknown(), pszSection, pszEntry, dDefault);
		return dDefault;
	}
};

