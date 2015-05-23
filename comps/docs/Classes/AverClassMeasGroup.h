#if !defined(AFX_AVERCLASSMEASGROUP_H__3AFD9B6D_A232_41C3_99E6_18D2B6F01CFD__INCLUDED_)
#define AFX_AVERCLASSMEASGROUP_H__3AFD9B6D_A232_41C3_99E6_18D2B6F01CFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AverClassMeasGroup.h : header file
//

#include "ClassBase.h"
#include "Factory.h"

/////////////////////////////////////////////////////////////////////////////
// CAverClassMeasGroup command target

class CAverClassMeasGroup : public CMeasParamGroupBase
{
	DECLARE_DYNCREATE(CAverClassMeasGroup)
	GUARD_DECLARE_OLECREATE_PROGID(CAverClassMeasGroup)
//	ENABLE_MULTYINTERFACE();				//

public:
	CAverClassMeasGroup();           // protected constructor used by dynamic creation
	virtual ~CAverClassMeasGroup();

// Operations
public:
	virtual bool CalcValues( IUnknown *punkCalcObject, IUnknown *punkOther );
	virtual bool GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeff );
	virtual bool LoadCreateParam();

	//IPersist
	virtual void GetClassID( CLSID* pClassID );	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AVERCLASSMEASGROUP_H__3AFD9B6D_A232_41C3_99E6_18D2B6F01CFD__INCLUDED_)
