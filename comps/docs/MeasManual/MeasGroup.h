#if !defined(AFX_MEASGROUP_H__624E9DB7_FFBD_44D3_B643_7A7B6B08A915__INCLUDED_)
#define AFX_MEASGROUP_H__624E9DB7_FFBD_44D3_B643_7A7B6B08A915__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MeasGroup.h : header file
//

#include "Measure5.h"
#include "classbase.h"


/////////////////////////////////////////////////////////////////////////////
// CMeasGroup command target

class CManualMeasGroup : public CMeasParamGroupBase
{
	DECLARE_DYNCREATE(CManualMeasGroup)
	DECLARE_GROUP_OLECREATE(CManualMeasGroup);
public:
	CManualMeasGroup();           // protected constructor used by dynamic creation
	virtual ~CManualMeasGroup();
public:
	virtual bool CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource );
	virtual bool GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeff );


	virtual bool OnInitCalculation();
	virtual bool OnFinalizeCalculation();

	virtual bool ReloadState();

	//IPersist
	virtual void GetClassID( CLSID* pClassID );	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEASGROUP_H__624E9DB7_FFBD_44D3_B643_7A7B6B08A915__INCLUDED_)
