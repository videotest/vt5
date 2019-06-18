// ClassObject.cpp : implementation file
//

#include "stdafx.h"
#include "Classes.h"
#include "ClassObject.h"
#include "Utils.h"


/////////////////////////////////////////////////////////////////////////////
// CClassObject

/////////////////////////////////////////////////////////////////////////////
// CClassObject message handlers

IMPLEMENT_DYNCREATE(CClassObjectList, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CClassObject, CCmdTargetEx);

BEGIN_INTERFACE_MAP(CClassObject, CDataObjectBase)
	INTERFACE_PART(CClassObject, IID_IClassObject,	ClassObj)
	INTERFACE_PART(CClassObject, IID_ICalcObject,	CalcObj)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CClassObject, ClassObj);


// {EB9E965A-2D32-4388-B7AE-492D88D8A898}
static const GUID clsidClassObjectList = 
{ 0xeb9e965a, 0x2d32, 0x4388, { 0xb7, 0xae, 0x49, 0x2d, 0x88, 0xd8, 0xa8, 0x98 } };
// {341D4AC6-4325-4033-BE68-6E6BAEA245F5}
static const GUID clsidClassObject = 
{ 0x341d4ac6, 0x4325, 0x4033, { 0xbe, 0x68, 0x6e, 0x6b, 0xae, 0xa2, 0x45, 0xf5 } };
// {8E87F6D5-4EFD-4c3b-8D03-158F1F858224}
GUARD_IMPLEMENT_OLECREATE(CClassObjectList, "Classes.ClassObjectList", 
0x8e87f6d5, 0x4efd, 0x4c3b, 0x8d, 0x3, 0x15, 0x8f, 0x1f, 0x85, 0x82, 0x24);
// {3F2A607E-8DCC-456c-AC77-D8F78A2F1B20}
GUARD_IMPLEMENT_OLECREATE(CClassObject, "Classes.ClassObject", 
0x3f2a607e, 0x8dcc, 0x456c, 0xac, 0x77, 0xd8, 0xf7, 0x8a, 0x2f, 0x1b, 0x20);


DATA_OBJECT_INFO(IDS_CLASSLIST_TYPE, CClassObjectList, CClassObjectList::c_szType, clsidClassObjectList, IDI_THUMBNAIL_ICON );
DATA_OBJECT_INFO_FULL(IDS_CLASS_TYPE, CClassObject, CClassObject::c_szType, CClassObjectList::c_szType, clsidClassObject, IDI_THUMBNAIL_ICON );


const char *CClassObject::c_szType = szTypeClass;
const char *CClassObjectList::c_szType = szTypeClassList;

/////////////////////////////////////////////////////////////////////////////////////////
//native interface implementation

//	com_call SetName( BSTR bstrName );


HRESULT CClassObject::XClassObj::GetColor( DWORD *pdwColor )
{
	_try_nested(CClassObject, ClassObj, GetColor)
	{
		*pdwColor = pThis->GetColor();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CClassObject::XClassObj::SetColor( DWORD dwColor )
{
	_try_nested(CClassObject, ClassObj, SetColor)
	{
		pThis->SetColor((COLORREF)dwColor);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CClassObject::XClassObj::SetParamLimits(long lParamKey, double fLo, double fHi)
{
	METHOD_PROLOGUE(CClassObject, ClassObj)
	pThis->_SetParamLimits(lParamKey, fLo, fHi);
	return S_OK;
}

HRESULT CClassObject::XClassObj::GetParamLimits(long lParamKey, double* pfLo, double* pfHi)
{
	METHOD_PROLOGUE(CClassObject, ClassObj)
	pThis->_GetParamLimits(lParamKey, pfLo, pfHi);
	return S_OK;
}


HRESULT CClassObject::XClassObj::RemoveParamLimits(long lParamKey)
{
	METHOD_PROLOGUE(CClassObject, ClassObj)
	pThis->_RemoveParamLimits(lParamKey);
	return S_OK;
}
	
HRESULT CClassObject::XClassObj::EmptyAllParamsLimits()
{
	METHOD_PROLOGUE(CClassObject, ClassObj)
	pThis->_EmptyAllParamsLimits();
	return S_OK;
}
	
HRESULT CClassObject::XClassObj::GetFirstParamLimitsPos(long* plPos)
{
	METHOD_PROLOGUE(CClassObject, ClassObj)
	if(plPos)
		*plPos = pThis->_GetFirstParamLimitsPos();
	return S_OK;
}
	
HRESULT CClassObject::XClassObj::GetNextParamLimits(long* plPos, long* plParamKey, double* pfLo, double* pfHi)
{
	METHOD_PROLOGUE(CClassObject, ClassObj)
	pThis->_GetNextParamLimits(plPos, plParamKey, pfLo, pfHi);
	return S_OK;
}
	
	

/////////////////////////////////////////////////////////////////////////////////////////
//virtuals
DWORD CClassObject::GetNamedObjectFlags()
{
	return nofNormal|nofStoreByParent;
}


/////////////////////////////////////////////////////////////////////////////////////////
//virtuals

bool CClassObject::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	if (!CDataObjectBase::SerializeObject(ar, pparams))
		return false;

	if (!CCalcObjectImpl::Serialize(ar))
		return false;

	if (ar.IsStoring())
	{
		long	lVersion = 2;

		ar << lVersion;	//
		DWORD dwColor = (DWORD)m_cColor;
		ar << dwColor;

		ar<<(int)m_mapParamLimits.GetCount();
		POSITION pos = m_mapParamLimits.GetStartPosition();
		long nParam = 0;
		while(pos)
		{
			ParamLimitsInfo* pInfo = 0;
			m_mapParamLimits.GetNextAssoc(pos, nParam, pInfo);

			ar<<nParam;

			if(pInfo)
			{
				ar<<pInfo->fLo;
				ar<<pInfo->fHi;
			}
			else
			{
				ar<<0.;
				ar<<0.;
			}
		}

	}
	else
	{
		long	lVersion = 2;
		ar >> lVersion;
		DWORD dwColor = 0;
		ar >> dwColor;
		m_cColor = (COLORREF)dwColor;

		if(lVersion > 1)
		{
			_EmptyAllParamsLimits();
			long nCount = 0;
			ar>>nCount;
			long nParam = 0;
			double fLo = 0., fHi = 0.;
			for(long i = 0; i < nCount; i++)
			{

				ar>>nParam;
				ar>>fLo;
				ar>>fHi;
			
				ParamLimitsInfo* pInfo = new ParamLimitsInfo(fLo, fHi);
				m_mapParamLimits.SetAt(nParam, pInfo);
			}
		}

	}

	return true;
}

CClassObject::CClassObject()
{
	m_cColor = GetNewClassObjectColor();
}

CClassObject::~CClassObject()
{
	_EmptyAllParamsLimits();
}	

DWORD CClassObject::GetColor()
{
	return (DWORD)m_cColor;
}

void CClassObject::SetColor(COLORREF cColor)
{
	m_cColor = cColor;
}

	

void CClassObject::_SetParamLimits(long lParamKey, double fLo, double fHi)
{
	_RemoveParamLimits(lParamKey);

	ParamLimitsInfo* pInfo = new ParamLimitsInfo(fLo, fHi);
	
	m_mapParamLimits.SetAt(lParamKey, pInfo);
}

void CClassObject::_GetParamLimits(long lParamKey, double* pfLo, double* pfHi)
{
	ParamLimitsInfo* pInfo = 0;
	if(m_mapParamLimits.Lookup(lParamKey, pInfo) == TRUE)
	{
		if(pInfo)
		{
			if(pfLo) *pfLo = pInfo->fLo;
			if(pfHi) *pfHi = pInfo->fHi;
		}
	}
}

void CClassObject::_RemoveParamLimits(long lParamKey)
{
	ParamLimitsInfo* pInfo = 0;
	if(m_mapParamLimits.Lookup(lParamKey, pInfo) == TRUE)
	{
		if(pInfo) delete pInfo;
		m_mapParamLimits.RemoveKey(lParamKey);
	}
}

void CClassObject::_EmptyAllParamsLimits()
{
	POSITION pos = m_mapParamLimits.GetStartPosition();
	long nParam = 0;
	while(pos)
	{
		ParamLimitsInfo* pInfo = 0;
		m_mapParamLimits.GetNextAssoc(pos, nParam, pInfo);
		if(pInfo)
			delete pInfo;
	}
	m_mapParamLimits.RemoveAll();
}

long CClassObject::_GetFirstParamLimitsPos()
{
	return (long)m_mapParamLimits.GetStartPosition();
}

void CClassObject::_GetNextParamLimits(long* plPos, long* plParamKey, double* pfLo, double* pfHi)
{
	if(plPos && *plPos && plParamKey)
	{
		ParamLimitsInfo* pInfo = 0;
		POSITION pos = (POSITION)*plPos;
		m_mapParamLimits.GetNextAssoc(pos, *plParamKey, pInfo);
		*plPos = (long)pos;
		if(pInfo)
		{
			if(pfLo) *pfLo = pInfo->fLo;
			if(pfHi) *pfHi = pInfo->fHi;
		}
	}
}


bool CClassObject::GetClassKey(GuidKey * pClassKey) 
{	
	if (!pClassKey)
		return false;
	*pClassKey = ::GetObjectKey(GetControllingUnknown());
	return true;	
}

bool CClassObject::SetClassKey(GuidKey ClassKey)
{
	GuidKey Key = ::GetObjectKey(GetControllingUnknown());
	return (Key == ClassKey);	
}

static const COLORREF s_colorFaceArray[] = 
{
	RGB(255, 128, 128),
	RGB(128, 255, 128),
	RGB(128, 128, 255),
	RGB(255, 128, 255),
	RGB(255, 255, 128),
	RGB(128, 255, 255),
	RGB(255, 255, 255),
	RGB(0,   255, 0  ),
	RGB(0,   0,   255),
	RGB(255, 0,   255),
	RGB(255, 255, 0  ),
	RGB(255, 0,   255),
	RGB(128, 0,   0  ),
	RGB(0,   128, 0  ),
	RGB(255, 0,   128),
	RGB(255, 0,   128)
};

COLORREF GetNewClassObjectColor(int nIndex)
{
	static int nCount;
	if (nIndex == -1)
	{
		nIndex = nCount % (sizeof(s_colorFaceArray)/sizeof(COLORREF));
		nCount++;
	}
	
	return s_colorFaceArray[nIndex % (sizeof(s_colorFaceArray)/sizeof(COLORREF))];
}

BEGIN_DISPATCH_MAP(CClassObject, CDataObjectBase)
	//{{AFX_DISPATCH_MAP(CClassObject)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		DISP_PROPERTY_EX(CClassObject, "Color", GetColorDisp, SetColorDisp, VT_COLOR)
		DISP_FUNCTION(CClassObject, "GetValue", GetValueDisp, VT_R8, VTS_I4)
		DISP_FUNCTION(CClassObject, "SetValue", SetValueDisp, VT_EMPTY, VTS_I4 VTS_R8)
	//}}AFX_DISPATCH_MAP
//		DISP_PROPERTY_EX(CClassObject, "Key", GetKey, SetKey, VT_I4)
END_DISPATCH_MAP()

OLE_COLOR CClassObject::GetColorDisp()
{
	return (OLE_COLOR)GetColor();
}

void CClassObject::SetColorDisp(OLE_COLOR color)
{
	SetColor((DWORD)color);
}

double CClassObject::GetValueDisp(long nKey)
{
	double fValue = 0;
	GetValue(nKey, &fValue);
	return fValue;
}

void CClassObject::SetValueDisp(long nKey, double fValue)
{
	SetValue(nKey, fValue);
}

/*
long CClassObject::GetKey()
{
	long lClassKey = 0;
	GetClassKey(&lClassKey);
	return lClassKey;
}

void CClassObject::SetKey(long nKey)
{
	SetClassKey(nKey);
}
*/