#if !defined(AFX_SPERMMEASGROUP_H__2CD90AB0_6DB1_4C22_B755_667500F2D3F2__INCLUDED_)
#define AFX_SPERMMEASGROUP_H__2CD90AB0_6DB1_4C22_B755_667500F2D3F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Named2Darrays.h"

class CSpermMeasGroup : 
	public CMeasParamGroupBase
{
	DECLARE_DYNCREATE(CSpermMeasGroup)
	DECLARE_GROUP_OLECREATE(CSpermMeasGroup);
public:
	CSpermMeasGroup();           // protected constructor used by dynamic creation
	virtual ~CSpermMeasGroup();
public:
	virtual bool CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource );
	virtual bool GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeff );
	virtual bool LoadCreateParam();
	virtual bool OnInitCalculation();

	//IPersist
	virtual void GetClassID( CLSID* pClassID );	

private:
    int m_nPointsAverage;
	int m_nStepAverage; // параметры для получения сглаженной кривой
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPERMMEASGROUP_H__2CD90AB0_6DB1_4C22_B755_667500F2D3F2__INCLUDED_)
