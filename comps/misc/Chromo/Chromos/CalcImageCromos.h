// CalcImageCromos.h: interface for the CCalcImageCromos class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALCIMAGECROMOS_H__4C0593C0_7E7E_11D4_8336_CF3C18CD2675__INCLUDED_)
#define AFX_CALCIMAGECROMOS_H__4C0593C0_7E7E_11D4_8336_CF3C18CD2675__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ClassBase.h"
#include "math.h"

class CCalcImageCromos : public CMeasParamGroupBase  
{
	DECLARE_DYNCREATE(CCalcImageCromos)
	DECLARE_GROUP_OLECREATE(CCalcImageCromos);

public:
	CCalcImageCromos();
	virtual bool CalcValues(IUnknown *punkCalcObject, IUnknown *p);
	virtual ~CCalcImageCromos();
	virtual bool LoadCreateParam();
//	bool Init();
	virtual bool GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeff );
protected:
	virtual bool OnFinalizeCalculation();
	virtual bool GetParamUnitName(long lParamKey, long lMeasUnit, CString & strUnitName);
	virtual bool SetParamUnitName(long lParamKey, long lMeasUnit, CString & strUnitName);
private:
	virtual void GetClassID( CLSID* pClassID );	
};


#endif // !defined(AFX_CALCIMAGECROMOS_H__4C0593C0_7E7E_11D4_8336_CF3C18CD2675__INCLUDED_)
