#if !defined(AFX_IMAGEAREAMEASGROUP_H__7E57DE4D_B762_4E34_88D2_D08585919851__INCLUDED_)
#define AFX_IMAGEAREAMEASGROUP_H__7E57DE4D_B762_4E34_88D2_D08585919851__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageAreaMeasGroup.h : header file
//

#include "ClassBase.h"

/////////////////////////////////////////////////////////////////////////////
// CImageAreaMeasGroup command target

class CImageAreaMeasGroup : public CMeasParamGroupBase
{
	DECLARE_DYNCREATE(CImageAreaMeasGroup)
	DECLARE_GROUP_OLECREATE(CImageAreaMeasGroup);
//	ENABLE_MULTYINTERFACE();				//

	void CalcValuesByPanes(ICalcObject2Ptr ptrCalc, CImageWrp &imageCalc);
public:
	CImageAreaMeasGroup();           // protected constructor used by dynamic creation
	virtual ~CImageAreaMeasGroup();

// Operations
public:
	virtual bool CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource );
	virtual bool GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeffToUnits );
	virtual bool LoadCreateParam();
	virtual bool OnInitCalculation();

protected:
	void MeasurePerimeter(Contour *p, double &fP, bool	bOuter);

private:
	bool	m_bUseSmooth;
	int		m_nSmoothLevel;

	//IPersist
	virtual void GetClassID( CLSID* pClassID );	

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEAREAMEASGROUP_H__7E57DE4D_B762_4E34_88D2_D08585919851__INCLUDED_)
