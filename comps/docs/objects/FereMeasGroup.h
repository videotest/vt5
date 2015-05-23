#if !defined(AFX_FEREMEASGROUP_H__2CD90AB0_6DB1_4C22_B755_667500F2D3F2__INCLUDED_)
#define AFX_FEREMEASGROUP_H__2CD90AB0_6DB1_4C22_B755_667500F2D3F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FereMeasGroup.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CFereMeasGroup command target

class CFereMeasGroup : public CMeasParamGroupBase
{
	DECLARE_DYNCREATE(CFereMeasGroup)
	DECLARE_GROUP_OLECREATE(CFereMeasGroup);
public:
	CFereMeasGroup();           // protected constructor used by dynamic creation
	virtual ~CFereMeasGroup();
public:
	virtual bool CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource );
	virtual bool GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeff );
	virtual bool LoadCreateParam();

	//IPersist
	virtual void GetClassID( CLSID* pClassID );	

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FEREMEASGROUP_H__2CD90AB0_6DB1_4C22_B755_667500F2D3F2__INCLUDED_)
