#include "StdAfx.h"
#include "DriverWrp.h"

CDriverWrp::CDriverWrp(IUnknown *punk, bool bAddRef)
{
	if( punk )
		Attach( punk, bAddRef );
}

void CDriverWrp::Attach(IUnknown *punk, bool bAddRef)
{
	sptrIDriver2 sptrD(punk);
	sptrIDriver2::Attach(sptrD, bAddRef);
}

IUnknown *CDriverWrp::operator =(IUnknown *punk)
{
	Attach(punk);
	return punk;
}

int CDriverWrp::GetInt(int nDev, LPCTSTR sSection, LPCTSTR sEntry, int nDef, UINT nFlags)
{
	CString sSec(sSection);
	CString sEnt(sEntry);
	BSTR bstrSec = sSec.AllocSysString();
	BSTR bstrEnt = sEnt.AllocSysString();
	int nRes = nDef;
	(*this)->GetValueInt(nDev, bstrSec, bstrEnt, &nRes, nFlags);
	::SysFreeString(bstrSec);
	::SysFreeString(bstrEnt);
	return nRes;
}

void CDriverWrp::SetInt(int nDev, LPCTSTR sSection, LPCTSTR sEntry, int nValue, UINT nFlags)
{
	CString sSec(sSection);
	CString sEnt(sEntry);
	BSTR bstrSec = sSec.AllocSysString();
	BSTR bstrEnt = sEnt.AllocSysString();
	(*this)->SetValueInt(nDev, bstrSec, bstrEnt, nValue, nFlags);
	::SysFreeString(bstrSec);
	::SysFreeString(bstrEnt);
}

CString CDriverWrp::GetString(int nDev, LPCTSTR sSection, LPCTSTR sEntry, LPCTSTR sDefault, UINT nFlags)
{
	CString sSec(sSection);
	CString sEnt(sEntry);
	CString sDef(sDefault);
	BSTR bstrSec = sSec.AllocSysString();
	BSTR bstrEnt = sEnt.AllocSysString();
	BSTR bstrRes = sDef.AllocSysString();
	(*this)->GetValueString(nDev, bstrSec, bstrEnt, &bstrRes, nFlags);
	sDef = bstrRes;
	::SysFreeString(bstrSec);
	::SysFreeString(bstrEnt);
	::SysFreeString(bstrRes);
	return sDef;
}

void CDriverWrp::SetString(int nDev, LPCTSTR sSection, LPCTSTR sEntry, LPCTSTR sValue, UINT nFlags)
{
	CString sSec(sSection);
	CString sEnt(sEntry);
	CString sVal(sValue);
	BSTR bstrSec = sSec.AllocSysString();
	BSTR bstrEnt = sEnt.AllocSysString();
	BSTR bstrVal = sVal.AllocSysString();
	(*this)->SetValueString(nDev, bstrSec, bstrEnt, bstrVal, nFlags);
	::SysFreeString(bstrSec);
	::SysFreeString(bstrEnt);
	::SysFreeString(bstrVal);
}

double CDriverWrp::GetDouble(int nDev, LPCTSTR sSection, LPCTSTR sEntry, double dDef, UINT nFlags)
{
	CString sSec(sSection);
	CString sEnt(sEntry);
	BSTR bstrSec = sSec.AllocSysString();
	BSTR bstrEnt = sEnt.AllocSysString();
	double dRes = dDef;
	(*this)->GetValueDouble(nDev, bstrSec, bstrEnt, &dRes, nFlags);
	::SysFreeString(bstrSec);
	::SysFreeString(bstrEnt);
	return dRes;
}

void CDriverWrp::SetDouble(int nDev, LPCTSTR sSection, LPCTSTR sEntry, double dValue, UINT nFlags)
{
	CString sSec(sSection);
	CString sEnt(sEntry);
	BSTR bstrSec = sSec.AllocSysString();
	BSTR bstrEnt = sEnt.AllocSysString();
	(*this)->SetValueDouble(nDev, bstrSec, bstrEnt, dValue, nFlags);
	::SysFreeString(bstrSec);
	::SysFreeString(bstrEnt);
}



