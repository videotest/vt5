#if !defined(AFX_CLASSOBJECT_H__EEEAFDC7_9EAD_4D95_BFBE_88146C8632ED__INCLUDED_)
#define AFX_CLASSOBJECT_H__EEEAFDC7_9EAD_4D95_BFBE_88146C8632ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClassObject.h : header file
//
#include "CmnBase.h"
#include "DataBase.h"
#include "ClassBase.h"

struct ParamLimitsInfo
{
	ParamLimitsInfo(double fL, double fH) : fLo(fL), fHi(fH){};
	double fLo;
	double fHi;
};


/////////////////////////////////////////////////////////////////////////////
// CClassObject command target
class CClassObject : public CDataObjectBase,
					 public	CCalcObjectImpl
{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(CClassObject);
	GUARD_DECLARE_OLECREATE(CClassObject);
	ENABLE_MULTYINTERFACE();					//

// Constructors
public:
	CClassObject();
	~CClassObject();

// Operations
public:
	BEGIN_INTERFACE_PART(ClassObj, IClassObject)
		com_call GetColor( DWORD *pdwColor );
		com_call SetColor( DWORD dwColor );

		com_call SetParamLimits(long lParamKey, double fLo, double fHi);
		com_call GetParamLimits(long lParamKey, double* pfLo, double* pfHi);
		com_call RemoveParamLimits(long lParamKey); 
		com_call EmptyAllParamsLimits(); 
		com_call GetFirstParamLimitsPos(long* plPos);
		com_call GetNextParamLimits(long* plPos, long* plParamKey, double* pfLo, double* pfHi);
	END_INTERFACE_PART(ClassObj)

//overrided virtuals
	virtual DWORD GetNamedObjectFlags();
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );


	virtual bool GetClassKey(GuidKey * pClassKey);
	virtual bool SetClassKey(GuidKey ClassKey);


	DWORD	GetColor();
	void	SetColor(COLORREF cColor);



	void _SetParamLimits(long lParamKey, double fLo, double fHi);
	void _GetParamLimits(long lParamKey, double* pfLo, double* pfHi);
	void _RemoveParamLimits(long lParamKey); 
	void _EmptyAllParamsLimits(); 
	long _GetFirstParamLimitsPos();
	void _GetNextParamLimits(long* plPos, long* plParamKey, double* pfLo, double* pfHi);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassObject)
	//}}AFX_VIRTUAL

// Implementation
protected:
	COLORREF m_cColor;
	// Generated message map functions
	//{{AFX_MSG(CClassObject)
	//}}AFX_MSG

	DECLARE_INTERFACE_MAP();

	DECLARE_DISPATCH_MAP();
	//class wizard support
	//{{AFX_DISPATCH(CMeasureObject)
		afx_msg OLE_COLOR GetColorDisp();
		afx_msg void SetColorDisp(OLE_COLOR color);
		afx_msg double GetValueDisp(long nKey);
		afx_msg void SetValueDisp(long nKey, double fValue);
	//}}AFX_DISPATCH
//		afx_msg void SetKey(long nKey);
//		afx_msg long GetKey();

public:
	static const char *c_szType;

	CMap<long, long, ParamLimitsInfo*, ParamLimitsInfo*&> m_mapParamLimits;
};

class CClassObjectList : public CDataObjectListBase
{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(CClassObjectList);
	GUARD_DECLARE_OLECREATE(CClassObjectList);
public:
	CClassObjectList(){};
	~CClassObjectList()
	{
		TRACE( "CClassObjectList::~CClassObjectList\n" );
	}

public:
	static const char *c_szType;
};


COLORREF GetNewClassObjectColor(int nIndex = -1);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLASSOBJECT_H__EEEAFDC7_9EAD_4D95_BFBE_88146C8632ED__INCLUDED_)
