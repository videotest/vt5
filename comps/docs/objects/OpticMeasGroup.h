#if !defined(AFX_OPTICMEASGROUP_H__6F4F6870_885A_477F_8CCE_1E18617BC1EE__INCLUDED_)
#define AFX_OPTICMEASGROUP_H__6F4F6870_885A_477F_8CCE_1E18617BC1EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpticMeasGroup.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// COpticMeasGroup command target

class COpticMeasGroup : public CMeasParamGroupBase
{
	DECLARE_DYNCREATE(COpticMeasGroup)
	DECLARE_GROUP_OLECREATE(COpticMeasGroup)

	COpticMeasGroup();           // protected constructor used by dynamic creation
	~COpticMeasGroup();

public:
	virtual bool CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource );
	virtual bool GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeff );
	virtual bool LoadCreateParam();
private:
	IOpticMeasureManPtr		m_sptrOpticMan;

	//IPersist
	virtual void GetClassID( CLSID* pClassID );	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTICMEASGROUP_H__6F4F6870_885A_477F_8CCE_1E18617BC1EE__INCLUDED_)
