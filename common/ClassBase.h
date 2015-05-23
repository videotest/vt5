#ifndef __CLASSBASE_H__
#define __CLASSBASE_H__


#include "defs.h"
#include "utils.h"
#include "Classes5.h"
#include "cmdtargex.h"
#include "CmnBase.h"
#include "DataBase.h"
#include "Factory.h"
#include "image5.h"

#include <afxTempl.h>
#include "StreamEx.h"

struct CParamValueEx : public ParameterAdditionalInfo
{
	double dValue;
};

class std_dll CCalcObjValue
{
	double	m_fValue;
	long	m_lKey;
	CList<CParamValueEx,CParamValueEx&> m_AdditionalValues;

public:
	CCalcObjValue(long lKey = -1, double fValue = 0)
		:	m_lKey(lKey),
			m_fValue(fValue)
	{
	}
	CCalcObjValue(const CCalcObjValue & rval) 
	{	*this = rval;	}
	void Clear() 
	{
		m_fValue = 0;
		m_AdditionalValues.RemoveAll();
	}
	double	GetValue()
	{	return m_fValue;	}
	void SetValue(double fValue)
	{	m_fValue = fValue;	}

	long	GetKey()
	{	return m_lKey;	}
	void	SetKey(long	lKey)
	{	m_lKey = lKey;	}

	bool GetValueEx(struct ParameterAdditionalInfo *pAddInfo, double *pfValue);
	bool SetValueEx(struct ParameterAdditionalInfo *pAddInfo, double fValue);

	CCalcObjValue& operator = (const CCalcObjValue& rval);

};

typedef CList <CCalcObjValue, CCalcObjValue &> CCalcValueList;

class std_dll CCalcObjectImpl : public CImplBase
{
public:
	CCalcObjectImpl();
	virtual ~CCalcObjectImpl();

	virtual bool GetClassKey(GUID * plClassKey) 
	{	*plClassKey = m_lClassKey; return true;	}
	virtual bool SetClassKey(GUID lClassKey)
	{	m_lClassKey = lClassKey; return true;	}

	virtual bool GetNextValue(long * plPos, long * plKey, double * pfValue);
	virtual bool GetFirstValuePos(long * plPos);
	virtual bool GetValuePos(long lKey, long * plPos);
	virtual bool RemoveValueByPos(long lPos);

	virtual bool SetValue(long lParamKey, double fValue);
	virtual bool GetValue(long lParamKey, double * pfValue);
	virtual bool RemoveValue(long lParamKey);

	virtual bool GetValueEx(long lParamKey, struct ParameterAdditionalInfo *pAddInfo, double *pfValue);
	virtual bool SetValueEx(long lParamKey, struct ParameterAdditionalInfo *pAddInfo, double fValue);


	virtual bool ClearValues();
	virtual bool Serialize(CStreamEx & ar);

//	virtual POSITION FindByKey(long lParamKey);

protected:
	CCalcValueList	m_ValueList;
	GuidKey			m_lClassKey;

	BEGIN_INTERFACE_PART_EXPORT(CalcObj, ICalcObject2)
		// ICalcObject
		com_call GetValue(long lParamKey, double * pfValue);
		com_call SetValue(long lParamKey, double fValue);
		com_call RemoveValue(long lParamKey);
		com_call ClearValues();
		com_call GetFirstValuePos(long * plPos);
		com_call GetNextValue(long * plPos, long * plKey, double * pfValue);
		com_call GetValuePos(long lKey, long *plPos);
		com_call RemoveValueByPos(long lPos);
		com_call GetObjectClass(GUID* plClassKey);
		com_call SetObjectClass(GUID lClassKey);
		// ICalcObject2
		com_call GetValueEx(long lParamKey, struct ParameterAdditionalInfo *pAddInfo, double *pfValue);
		com_call SetValueEx(long lParamKey, struct ParameterAdditionalInfo *pAddInfo, double fValue);
	END_INTERFACE_PART(CalcObj)
};


//typedef CArray <CCalcParamInfo*, CCalcParamInfo*> CParamInfoArray;
/*template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CMapEx : public CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>
{
public:
	CMapEx(int nBlockSize = 10) : CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>(nBlockSize) {};

	// Lookup
	BOOL GetAt(ARG_KEY key, VALUE& rValue, POSITION &rPos) const;
};

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetAt(ARG_KEY key, VALUE& rValue, POSITION &rPos) const
{
	rPos = NULL;
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	rPos = (POSITION)pAssoc;
	return TRUE;
} */


//typedef CMap <long, long, POSITION, POSITION>		CParamInfoMap;
//typedef CList <CCalcParamInfo*, CCalcParamInfo*>	CParamInfoList;


/////////////////////////////////////////////////////////////////////////////
// CMeasParamGroupBase  & CMeasParamGroupImpl command target


class std_dll CMeasParamGroupImpl :	public CImplBase
{
public:
	//typedef LONG_PTR TPOS;
	CMeasParamGroupImpl();
	virtual ~CMeasParamGroupImpl();
public:
	BEGIN_INTERFACE_PART_EXPORT(Group, IMeasParamGroup)
 		com_call CalcValues(IUnknown *punkCalcObject, IUnknown *punkSource );
		com_call GetParamsCount(long *plCount);
		com_call GetFirstPos(LONG_PTR *plPos);
		com_call GetNextParam(LONG_PTR *plPos, struct ParameterDescriptor **ppDescriptior );
		com_call GetPosByKey(long lKey, LONG_PTR *plPos);
		com_call InitializeCalculation( IUnknown *punkContainer );
		com_call FinalizeCalculation();
		com_call GetUnit( long lType, BSTR *pbstr, double *pfCoeffToUnits );
	END_INTERFACE_PART(Group)
protected:
	virtual bool CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource ) = 0;
	virtual bool GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeff ) = 0;
	virtual bool LoadCreateParam()			{return true;}
	virtual bool ReloadState();
	virtual bool OnInitCalculation()		{return true;}
	virtual bool OnFinalizeCalculation()	{return true;}

	void DefineParameter( long lKey, const char *pszName, const char *pszDefFormat );
	bool ParamIsEnabledByKey( long lKey );
	POSITION	_find( long	lkey );

protected:
	CPtrList									m_listParamDescr;
	CMap<long, long&, POSITION, POSITION&>		m_mapKeyToPosition;
	DWORD                                       m_dwPanesMask;


	ICalcObjectContainerPtr	m_ptrContainer;
};


class std_dll CMeasParamGroupBase :	public CCmdTargetEx,
									public CMeasParamGroupImpl,
									public CNamedObjectImpl,
									public CRootedObjectImpl,
									public CNumeredObjectImpl,
									public CInitializeObjectImpl,
									public CPersistImpl,
									public CEventListenerImpl,
									public IPriority

{
	DECLARE_DYNAMIC(CMeasParamGroupBase)
	ENABLE_MULTYINTERFACE();
public:
	CMeasParamGroupBase();           // protected constructor used by dynamic creation
	virtual ~CMeasParamGroupBase();

	virtual LPUNKNOWN GetInterfaceHook(const void* p)
	{
		if (*(IID*)p == IID_IPriority)
			return (IPriority*)this;
		else return __super::GetInterfaceHook(p);
	}

	virtual bool Init();
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	// IPriority
	com_call GetPriority(long *plPriority);

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeasParamGroupBase)
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMeasParamGroupBase)
	//}}AFX_MSG

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CMeasParamGroupBase)
	//}}AFX_DISPATCH
	DECLARE_INTERFACE_MAP()
};

#define DECLARE_GROUP_OLECREATE(class_name) \
	GUARD_DECLARE_OLECREATE_PROGID(class_name)

#define IMPLEMENT_GROUP_OLECREATE(class_name, external_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	GUARD_IMPLEMENT_OLECREATE_PROGID(class_name, external_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) 



/////////////////////////////////////////////////////////////////////////////
// CParamGroupManBase 
/*

базовый класс для манеджера групп параметров.
умеет загружать параметры, сохранять их,
реагироавть на изменение Target'a (сохранять дони загружать другие группы)
хранит список разрешенных и запрещенных групп, умеет запрщать и разрешать их.

производные классы имеют более узкую направленность
и базируются на различных секциях regestry

*/


/*class std_dll  CParamGroupManBase : public CCmdTargetEx,
								   public CCompManagerImpl,
								   public CNamedObjectImpl,
								   public CRootedObjectImpl,
								   public CNumeredObjectImpl
{
	DECLARE_DYNCREATE(CParamGroupManBase)
	DECLARE_INTERFACE_MAP()
	ENABLE_MULTYINTERFACE();					//

public:
	CParamGroupManBase();
	virtual ~CParamGroupManBase();

// Operations
public:

	// override from CCompManagerImpl
	virtual void DeInit(); // default deinitialization
	virtual bool Init();// default initialization

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParamGroupManBase)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// override from CCompManager
	virtual void OnAddComponent(int idx);
	virtual void OnRemoveComponent(int idx);
	
	// need overrite in derived classes for group w/IMeasParamGroup2
	virtual LPCTSTR GetGroupLoadParamString()
	{	return NULL;	}
	// load adn save state for manager (inner functions)
	// load groups and permits( and create user defined groups and load its state if need)
	virtual bool LoadState(IUnknown* punkData);
	// load groups and permits( and save user defined groups and save its state if need)
	virtual bool SaveState(IUnknown* punkData);

	void LoadStateInner();
	void SaveStateInner();

	// find group in CCompManager
	int		FindGroup(IUnknown* punkGroup);

	// inner function for work w/ permits
	POSITION	FindPermit(int nGroupIndex);
	POSITION	InsertPermit(int nGroupIndex);
	bool		RemovePermit(int nGroupIndex);

	int			GetPermit(POSITION Pos);
	POSITION	GetFirstPermit();
	int			GetNextPermit(POSITION &rPos);
	int			GetPermitCount();

	void		RemoveAllPermit();

	
	BEGIN_INTERFACE_PART_EXPORT(GroupMan, IParamGroupManager)
		com_call ExcludeGroup(long *plPos);
		com_call ExcludeGroup(IUnknown* punkGroup);
		com_call IncludeGroup(IUnknown* punkGroup, long *plPos);

		com_call GetFirstPos(LONG_PTR *plPos);
		com_call GetNextGroup(long *LONG_PTR, IUnknown **ppunk);
		com_call GetGroupPos(IUnknown* punkGroup, LONG_PTR *plPos); //???
		com_call GetGroupCount(int *pnCount);

		com_call SaveState(IUnknown* punkNamedData);
		com_call LoadState(IUnknown* punkNamedData);

		com_call SetCalcImageToGroups(IUnknown* punkImage);
	END_INTERFACE_PART(GroupMan);

protected:
	CList <int, int> m_listPermit; // array of indexes of permission group
};*/

/*
#define PNC_ADD		0
#define PNC_REMOVE	0
#define PNC_CHANGE	0

// notify param data
typedef struct tParamNotifyData
{
	UINT uCode;			// operation code
	long lParamKey;		// param key (ID)
};

typedef struct tParamNotifyData TParamNotifyData;


TParamNotifyData * CreatePNData();
void DeletePNData(TParamNotifyData * );
*/

#endif// __CLASSBASE_H__