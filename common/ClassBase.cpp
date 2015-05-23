#include "stdafx.h"
#include "ClassBase.h"
#include "nameconsts.h"
#include "Utils.h"
#include "Core5.h"
#include "params.h"
#include "PropBagImpl.h"
#include "\vt5\awin\misc_dbg.h"

class time_test2
{
public:
	time_test2( const _char *psz=0, bool report=true )
	{
		m_psz = _tcsdup( psz );
		m_dw = 0;
		m_dw_last = ::GetTickCount();
		m_report = report;
		m_calls = 0;
	}
	~time_test2()
	{
		if( m_report )
			report();
		if( m_psz )
			::free( m_psz );
	}

	void start()
	{
		m_dw -= GetTickCount();
		m_calls++;
	}

	void stop()
	{
		DWORD dw = GetTickCount();
		m_dw += dw;
		if(dw-m_dw_last>5000)
		{ // каждые 5 секунд - промежуточный итог
			m_dw_last = dw;
			report();
		}
	}

	void report()
	{
		DWORD	dw = m_dw;

		if( m_psz )_trace_file( sz_profiler_log, _T("[%s:%d] %d"), m_psz, m_calls, dw );
		else _trace_file( sz_profiler_log, _T("[unknown:%d] %d"), m_calls, dw );
	}

public:
	DWORD	m_dw;
	DWORD	m_dw_last;
	bool	m_report;
	_char	*m_psz;
	int		m_calls;
};

class time_test2_trigger
{
public:
	time_test2_trigger(time_test2* p) : m_p(p)
	{
		m_p->start();
	};

	~time_test2_trigger()
	{
		m_p->stop();
	};

	time_test2* m_p;
};


// string params for CParamGroupManBase 
static const TCHAR szGroupCount[]		= "GroupCount";
static const TCHAR szGroupProgID[]		= "GroupProgID";
static const TCHAR szGroupNeedLoad[]	= "GroupNeedLoad";
static const TCHAR szGroupLoadParam[]	= "GroupLoadParam";
static const TCHAR szGroupHasPermit[]	= "GroupHasPermit";

static void InitMask(DWORD *pdwMask, int nMaskDwords)
{
	for (int i = 0; i < nMaskDwords; i++)
		pdwMask[i] = 0;
};

static void SetMask(DWORD *pdwMask, int nMaskDwords, int nBit, bool bSet)
{
	int nDword = nBit/32;
	if (nDword > nMaskDwords) return;
	int nShift = nBit%32;
	if (bSet)
		pdwMask[nDword] |= 1<<nShift;
	else
		pdwMask[nDword] &= ~(1<<nShift);
};

static void ParseMask(DWORD *pdwMask, int nMaskDwords, LPCTSTR lpstrMask)
{
	InitMask(pdwMask, nMaskDwords);
	LPCTSTR p = lpstrMask;
	if (!p) return;
	while (1)
	{
		if (*p == 0)
			break;
		int nNum = _ttoi(p);
		int n = _tcscspn(p, _T(",-"));
		if (n == -1)
		{
			SetMask(pdwMask, nMaskDwords, n, true);
			break;
		}
		if (p[n] == _T('-'))
		{
			p = &p[n+1];
			if (*p == 0)
				break;
			int nNum2 = _ttoi(p);
			for (int i = nNum; i <= nNum2; i++)
				SetMask(pdwMask, nMaskDwords, i, true);
		}
		else
			SetMask(pdwMask, nMaskDwords, nNum, true);
		p = _tcschr(p, _T(','));
		if (p == NULL)
			break;
		p++;
	}
}

static void LoadMask(DWORD *pdwMask, int nMaskDwords, LPCTSTR lpSec, LPCTSTR lpEntry)
{
	CString s = ::GetValueString( GetAppUnknown(), lpSec, lpEntry, NULL);
	ParseMask(pdwMask, nMaskDwords, s);
}

CCalcObjValue& CCalcObjValue::operator = (const CCalcObjValue& rval)
{
	m_fValue = rval.m_fValue;
	m_lKey = rval.m_lKey;
	POSITION pos = rval.m_AdditionalValues.GetHeadPosition();
	m_AdditionalValues.RemoveAll();
	while (pos)
	{
		CParamValueEx value = rval.m_AdditionalValues.GetNext(pos);
		m_AdditionalValues.AddTail(value);
	}
	return *this;
}

bool CCalcObjValue::GetValueEx(struct ParameterAdditionalInfo *pAddInfo, double *pfValue)
{
	POSITION pos = m_AdditionalValues.GetHeadPosition();
	while (pos)
	{
		CParamValueEx &value = m_AdditionalValues.GetNext(pos);
		if (value.byPane == pAddInfo->byPane && value.byPhase == pAddInfo->byPhase)
		{
			*pfValue = value.dValue;
			return true;
		}
	}
	*pfValue = 0;
	return false;
}

bool CCalcObjValue::SetValueEx(struct ParameterAdditionalInfo *pAddInfo, double fValue)
{
	POSITION pos = m_AdditionalValues.GetHeadPosition();
	while (pos)
	{
		CParamValueEx &value = m_AdditionalValues.GetNext(pos);
		if (value.byPane == pAddInfo->byPane && value.byPhase == pAddInfo->byPhase)
		{
			value.dValue = fValue;
			return true;
		}
	}
	CParamValueEx value;
	value.byPane = pAddInfo->byPane;
	value.byPhase = pAddInfo->byPhase;
	value.dValue = fValue;
	m_AdditionalValues.AddTail(value);
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
// CCalcObjectImpl

IMPLEMENT_UNKNOWN_BASE(CCalcObjectImpl, CalcObj);


CCalcObjectImpl::CCalcObjectImpl()
{
	m_ValueList.RemoveAll();
}

CCalcObjectImpl::~CCalcObjectImpl()
{
	m_ValueList.RemoveAll();
}

HRESULT CCalcObjectImpl::XCalcObj::GetValue(long lParamKey, double * pfValue)
{
	METHOD_PROLOGUE_BASE(CCalcObjectImpl, CalcObj)
//	_try_nested_base(CCalcObjectImpl, CalcObj, GetValue)
	{
		if (lParamKey == -1)
			return E_INVALIDARG;
		return pThis->GetValue(lParamKey, pfValue) ? S_OK : E_INVALIDARG;
	}
//	_catch_nested;
}

HRESULT CCalcObjectImpl::XCalcObj::SetValue(long lParamKey, double fValue)
{
	METHOD_PROLOGUE_BASE(CCalcObjectImpl, CalcObj)
//	_try_nested_base(CCalcObjectImpl, CalcObj, SetValue)
	{
		if (lParamKey == -1)
			return E_INVALIDARG;
		return pThis->SetValue(lParamKey, fValue) ? S_OK : E_INVALIDARG;
	}
//	_catch_nested;
}

HRESULT CCalcObjectImpl::XCalcObj::ClearValues()
{
	METHOD_PROLOGUE_BASE(CCalcObjectImpl, CalcObj)
//	_try_nested_base(CCalcObjectImpl, CalcObj, ClearValues)
	{
		return pThis->ClearValues() ? S_OK : E_FAIL;
	}
//	_catch_nested;
}

HRESULT CCalcObjectImpl::XCalcObj::GetFirstValuePos(long * plPos)
{
	METHOD_PROLOGUE_BASE(CCalcObjectImpl, CalcObj)
//	_try_nested_base(CCalcObjectImpl, CalcObj, GetFirstValuePos)
	{
		if (!plPos)
			return E_INVALIDARG;
		return pThis->GetFirstValuePos(plPos) ? S_OK : E_FAIL;
	}
//	_catch_nested;
}

HRESULT CCalcObjectImpl::XCalcObj::GetNextValue(long * plPos, long * plKey, double * pfValue)
{
	METHOD_PROLOGUE_BASE(CCalcObjectImpl, CalcObj)
//	_try_nested_base(CCalcObjectImpl, CalcObj, GetNextValue)
	{
		if (!plPos)
			return E_INVALIDARG;
		return pThis->GetNextValue(plPos, plKey, pfValue) ? S_OK : E_INVALIDARG;
	}
//	_catch_nested;
}

HRESULT CCalcObjectImpl::XCalcObj::GetValuePos(long lKey, long *plPos)
{
	METHOD_PROLOGUE_BASE(CCalcObjectImpl, CalcObj)
//	_try_nested_base(CCalcObjectImpl, CalcObj, GetValuePos)
	{
		if (!plPos)
			return E_INVALIDARG;
		return pThis->GetValuePos(lKey, plPos) ? S_OK : E_INVALIDARG;
	}
//	_catch_nested;
}

HRESULT CCalcObjectImpl::XCalcObj::RemoveValueByPos(long lPos)
{
	METHOD_PROLOGUE_BASE(CCalcObjectImpl, CalcObj)
//	_try_nested_base(CCalcObjectImpl, CalcObj, RemoveValueByPos)
	{
		return pThis->RemoveValueByPos(lPos) ? S_OK : E_INVALIDARG;
	}
//	_catch_nested;
}

HRESULT CCalcObjectImpl::XCalcObj::RemoveValue(long lParamKey)
{
	METHOD_PROLOGUE_BASE(CCalcObjectImpl, CalcObj)
//	_try_nested_base(CCalcObjectImpl, CalcObj, RemoveValueByKey)
	{
		return pThis->RemoveValue(lParamKey) ? S_OK : E_INVALIDARG;
	}
//	_catch_nested;
}


HRESULT CCalcObjectImpl::XCalcObj::GetValueEx(long lParamKey, struct ParameterAdditionalInfo *pAddInfo,
	double *pfValue)
{
	METHOD_PROLOGUE_BASE(CCalcObjectImpl, CalcObj)
	{
		if (lParamKey == -1)
			return E_INVALIDARG;
		return pThis->GetValueEx(lParamKey, pAddInfo, pfValue) ? S_OK : E_INVALIDARG;
	}
}

HRESULT CCalcObjectImpl::XCalcObj::SetValueEx(long lParamKey, struct ParameterAdditionalInfo *pAddInfo,
	double fValue)
{
	METHOD_PROLOGUE_BASE(CCalcObjectImpl, CalcObj)
	{
		if (lParamKey == -1)
			return E_INVALIDARG;
		return pThis->SetValueEx(lParamKey, pAddInfo, fValue) ? S_OK : E_INVALIDARG;
	}
}


HRESULT CCalcObjectImpl::XCalcObj::GetObjectClass(GUID *plClassKey)
{
	METHOD_PROLOGUE_BASE(CCalcObjectImpl, CalcObj)
//	_try_nested_base(CCalcObjectImpl, CalcObj, GetObjectClass)
	{
		if (!plClassKey)
			return E_INVALIDARG;
		// get key from CClassObject
		return pThis->GetClassKey(plClassKey) ? S_OK : E_INVALIDARG;
	}
//	_catch_nested;
}

HRESULT CCalcObjectImpl::XCalcObj::SetObjectClass(GUID lClassKey)
{
	METHOD_PROLOGUE_BASE(CCalcObjectImpl, CalcObj)
//	_try_nested_base(CCalcObjectImpl, CalcObj, SetObjectClass)
	{
		return pThis->SetClassKey(lClassKey) ? S_OK : E_INVALIDARG;
	}
//	_catch_nested;
}

// inner function implementation

bool CCalcObjectImpl::GetValuePos(long lKey, long * plPos)
{
	if (lKey == -1 || !plPos)
		return false;

	*plPos = 0;
	if (m_ValueList.IsEmpty())
		return false;

	POSITION pos = m_ValueList.GetHeadPosition();
	while (pos)
	{	
		POSITION PrevPos = pos;
		CCalcObjValue & value = m_ValueList.GetNext(pos);
		if (value.GetKey() == lKey)
		{
			*plPos = (long)PrevPos;
			return true;
		}
	}
	return false;
}

bool CCalcObjectImpl::RemoveValueByPos(long lPos)
{
	if (m_ValueList.IsEmpty())
		return false;

	if (!lPos)
		return false;

	m_ValueList.RemoveAt((POSITION)lPos);
	return true;
}

bool CCalcObjectImpl::RemoveValue(long lParamKey)
{
	if (lParamKey == -1)
		return false;

	if (m_ValueList.IsEmpty())
		return false;

	POSITION pos = m_ValueList.GetHeadPosition();
	POSITION RemovePos = 0;
	while (pos)
	{	
		POSITION PrevPos = pos;
		CCalcObjValue & value = m_ValueList.GetNext(pos);
		if (value.GetKey() == lParamKey)
		{
			RemovePos = PrevPos;
			break;
		}
	}
	if (!RemovePos)
		return false;

	m_ValueList.RemoveAt(RemovePos);
	return true;
}


bool CCalcObjectImpl::GetNextValue(long * plPos, long * plKey, double * pfValue)
{
	if (!plPos)
		return false;

	POSITION pos = (POSITION)*plPos;
	if (plKey)
		*plKey = -1; 

	if (pfValue)
		*pfValue = 0;
	
	if (!pos)
		return false;
	
	CCalcObjValue & value = m_ValueList.GetNext(pos);

	*plPos = (long)pos;	
	
	if (plKey)
		*plKey = value.GetKey();
	if (pfValue)
		*pfValue = value.GetValue();
	
	return true;	
}

bool CCalcObjectImpl::GetFirstValuePos(long * plPos)
{	
	if (!plPos)
		return false;

	*plPos = 0;
	if (m_ValueList.IsEmpty())
		return true;

	POSITION pos = m_ValueList.GetHeadPosition();
	*plPos = (long)pos;	
	
	return true;	
}

time_test2 ttSetValue("SetValue");

bool CCalcObjectImpl::SetValue(long lParamKey, double fValue)
{	
	//time_test2_trigger tt(&ttSetValue);

	POSITION pos = m_ValueList.GetHeadPosition();
	while (pos)
	{	
		CCalcObjValue & value = m_ValueList.GetNext(pos);
		if (value.GetKey() == lParamKey)
		{
			value.SetValue(fValue);
			return true;
		}
	}
	// 
	m_ValueList.AddTail(CCalcObjValue(lParamKey, fValue));
	return true;
}


time_test2 ttGetValue("GetValue");

bool CCalcObjectImpl::GetValue(long lParamKey, double * pfValue)
{
	//time_test2_trigger tt(&ttGetValue);

	if (!pfValue)
		return false;

	*pfValue = 0; 
	if (m_ValueList.IsEmpty())
		return false;

	POSITION pos = m_ValueList.GetHeadPosition();
	while (pos)
	{	
		CCalcObjValue & value = m_ValueList.GetNext(pos);
		if (value.GetKey() == lParamKey)
		{
			*pfValue = value.GetValue();
			return true;
		}
	}

	return false;
}

bool CCalcObjectImpl::SetValueEx(long lParamKey, struct ParameterAdditionalInfo *pAddInfo,
	double fValue)
{
	POSITION pos = m_ValueList.GetHeadPosition();
	while (pos)
	{	
		CCalcObjValue & value = m_ValueList.GetNext(pos);
		if (value.GetKey() == lParamKey)
		{
			value.SetValueEx(pAddInfo, fValue);
			return true;
		}
	}
	CCalcObjValue value(lParamKey, 0.);
	value.SetValueEx(pAddInfo,fValue);
	m_ValueList.AddTail(value);
	return true;
}

bool CCalcObjectImpl::GetValueEx(long lParamKey, struct ParameterAdditionalInfo *pAddInfo, double *pfValue)
{	
	if (!pfValue)
		return false;

	*pfValue = 0; 
	if (m_ValueList.IsEmpty())
		return false;

	POSITION pos = m_ValueList.GetHeadPosition();
	while (pos)
	{	
		CCalcObjValue & value = m_ValueList.GetNext(pos);
		if (value.GetKey() == lParamKey)
			return value.GetValueEx(pAddInfo, pfValue);;
	}

	return false;
}

bool CCalcObjectImpl::ClearValues()
{	
	if (m_ValueList.IsEmpty())
		return true;

	POSITION pos = m_ValueList.GetTailPosition();

	while (pos)
	{	
		POSITION posNext = pos;
		CCalcObjValue & value = m_ValueList.GetPrev(pos);
		long lkey = value.GetKey( );
		
		// 25.04.2006 build 103 [AAM] переправил малехо, чтоб удобней было исключения вписывать
		if(lkey==KEY_CLASS) continue;
		if( lkey >= CONST_PARAMS_FIRST_KEY && lkey <= CONST_PARAMS_LAST_KEY)
			continue; // ~[vanek] : const params
		if( lkey >= MANUAL_PARAMS_FIRST_KEY && lkey <= MANUAL_PARAMS_LAST_KEY)
			continue; // AAM: manual params - BT 5127, 5146

		// value.Clear();	  Max [sbt:203]
		m_ValueList.RemoveAt( posNext );
	}

	return true;	
}

bool CCalcObjectImpl::Serialize(CStreamEx & ar)
{
	if (ar.IsStoring())
	{
		long lVersion = 2;
		ar << lVersion;
		ar << m_lClassKey;
		int nCount = m_ValueList.GetCount();
		ar << nCount;

		for (POSITION pos = m_ValueList.GetHeadPosition(); pos != NULL; )
		{
			CCalcObjValue & val = m_ValueList.GetNext(pos);
			ar << val.GetKey();
			ar << val.GetValue();
		}

		INamedPropBagSerPtr bag(Unknown());
		byte bBagStored = 1;
		if(bag==0) bBagStored = 0;
		ar << bBagStored;
		IStream* ps(ar);
		if(bBagStored) bag->Store(ps);
	}
	else
	{
		long lVersion = 1;
		ar >> lVersion;
		ar >> m_lClassKey;
		int nCount = 0;
		ar >> nCount;

		m_ValueList.RemoveAll();
		for (int i = 0; i < nCount; i++)
		{
			long lKey = -1;
			ar >> lKey;
			double fVal = 0;
			ar >> fVal;
			m_ValueList.AddTail(CCalcObjValue(lKey, fVal));
		}
		ASSERT (nCount == m_ValueList.GetCount());
		if(lVersion>=2)
		{
			byte bBagStored = 0;
			ar >> bBagStored;
			if(bBagStored)
			{
				INamedPropBagSerPtr bag(Unknown());
				ASSERT(bag!=0);
				if(bag==0) return false;
				IStream* ps(ar);
				bag->Load(ps);
			}
		}
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CMeasParamGroupImpl

IMPLEMENT_UNKNOWN_BASE(CMeasParamGroupImpl, Group);

CMeasParamGroupImpl::CMeasParamGroupImpl()
{
	m_dwPanesMask = 0;
}

CMeasParamGroupImpl::~CMeasParamGroupImpl()
{
	POSITION	pos = m_listParamDescr.GetHeadPosition();
	while( pos )
	{
		ParameterDescriptor	*pdescr = (ParameterDescriptor	*)m_listParamDescr.GetNext( pos );
		if( pdescr->bstrName )::SysFreeString( pdescr->bstrName );
		if( pdescr->bstrUserName )::SysFreeString( pdescr->bstrUserName );
		if( pdescr->bstrDefFormat )::SysFreeString( pdescr->bstrDefFormat );
		delete pdescr;
	}
	m_listParamDescr.RemoveAll();
	m_mapKeyToPosition.RemoveAll();
}

bool CMeasParamGroupImpl::ParamIsEnabledByKey( long lKey )
{
	POSITION	pos = _find( lKey );
	ASSERT( pos );
	if(!pos) return false;
	ParameterDescriptor*	pdescr = (ParameterDescriptor*)m_listParamDescr.GetNext( pos );
	return pdescr->lEnabled != 0;
}

void CMeasParamGroupImpl::DefineParameter( long lKey, const char *pszName, const char *pszDefFormat )
{
	ParameterDescriptor	*pdescr = new ParameterDescriptor;
	::ZeroMemory( pdescr, sizeof(ParameterDescriptor) );
	pdescr->cbSize = sizeof(ParameterDescriptor);
	pdescr->bstrName = _bstr_t(pszName).copy();
	pdescr->bstrUserName = _bstr_t(pszName).copy();
	pdescr->bstrDefFormat = _bstr_t(pszDefFormat).copy();
	pdescr->lKey = lKey;
	pdescr->lEnabled = 0;
	pdescr->pos = (LONG_PTR)m_listParamDescr.AddTail( pdescr );
	m_mapKeyToPosition[lKey] = (POSITION)pdescr->pos;
	CString	sMainSection = "\\measurement\\parameters\\";
	CString sParamsSection = sMainSection+pszName;
	::SetValue( ::GetAppUnknown(), sParamsSection, "Key", lKey ); 
}

POSITION	CMeasParamGroupImpl::_find( long	lKey )
{
	POSITION	pos = m_mapKeyToPosition[lKey];
	//ASSERT(pos);
	return pos;
}


HRESULT CMeasParamGroupImpl::XGroup::CalcValues (IUnknown *punkCalcObject, IUnknown *punkSource )
{
	METHOD_PROLOGUE_BASE(CMeasParamGroupImpl, Group);
	return pThis->CalcValues( punkCalcObject, punkSource )?S_OK:S_FALSE;
}
HRESULT CMeasParamGroupImpl::XGroup::GetParamsCount ( long * plCount )
{
	METHOD_PROLOGUE_BASE(CMeasParamGroupImpl, Group);
	*plCount = pThis->m_listParamDescr.GetCount();
	return S_OK;
}
HRESULT CMeasParamGroupImpl::XGroup::GetFirstPos ( LONG_PTR *plpos )
{
	METHOD_PROLOGUE_BASE(CMeasParamGroupImpl, Group);
	*plpos = (LONG_PTR)pThis->m_listParamDescr.GetHeadPosition();
	return S_OK;
}
HRESULT CMeasParamGroupImpl::XGroup::GetNextParam(LONG_PTR *plPos, ParameterDescriptor **ppDescriptior )
{
	METHOD_PROLOGUE_BASE(CMeasParamGroupImpl, Group);
	ParameterDescriptor*	pdescr = (ParameterDescriptor*)pThis->m_listParamDescr.GetNext( (POSITION&)*plPos );
	if( ppDescriptior )*ppDescriptior = pdescr;
	return S_OK;
}
HRESULT CMeasParamGroupImpl::XGroup::GetPosByKey ( long lKey, LONG_PTR *plpos )
{
	METHOD_PROLOGUE_BASE(CMeasParamGroupImpl, Group);
	*plpos = (LONG_PTR)pThis->_find(lKey);
	return S_OK;
}

HRESULT CMeasParamGroupImpl::XGroup::InitializeCalculation ( IUnknown * punkContainer )
{
	METHOD_PROLOGUE_BASE(CMeasParamGroupImpl, Group);

	pThis->m_ptrContainer = punkContainer;
	if( pThis->m_ptrContainer == 0 )return S_FALSE;

	pThis->OnInitCalculation();

	LONG_PTR	lposP = 0;
	GetFirstPos( &lposP );

	while( lposP )
	{
		ParameterDescriptor	*pdescr;

		GetNextParam( &lposP, &pdescr );
		if( pdescr->lEnabled )
			pThis->m_ptrContainer->DefineParameter( pdescr->lKey, etUndefined, this, 0 );
	}

	return S_OK;
}

HRESULT CMeasParamGroupImpl::XGroup::GetUnit( long lType, BSTR *pbstr, double *pfCoeffToUnits )
{
	METHOD_PROLOGUE_BASE(CMeasParamGroupImpl, Group);

	_bstr_t	bstr;
	double	f = 1;

	pThis->GetUnit( -1, lType, bstr, f );

	if( pfCoeffToUnits )*pfCoeffToUnits = f;
	if( pbstr )*pbstr = bstr.copy();


	return S_OK;
}

HRESULT CMeasParamGroupImpl::XGroup::FinalizeCalculation()
{
	METHOD_PROLOGUE_BASE(CMeasParamGroupImpl, Group);

	pThis->OnFinalizeCalculation();

	pThis->m_ptrContainer = 0;
	return S_OK;
}

bool CMeasParamGroupImpl::ReloadState()
{
	m_dwPanesMask = 0;
	POSITION	pos = m_listParamDescr.GetHeadPosition();
	while( pos )
	{
		_bstr_t	bstrUnit;

		ParameterDescriptor	*pdescr = (ParameterDescriptor	*)m_listParamDescr.GetNext( pos );
		GetUnit( pdescr->lKey, etUndefined, bstrUnit, pdescr->fCoeffToUnits );

		if( pdescr->bstrUnit )
			::SysFreeString( pdescr->bstrUnit );	pdescr->bstrUnit = 0;
		pdescr->bstrUnit = bstrUnit.copy();

		CString	strSection = "\\measurement\\parameters\\";
		strSection+=pdescr->bstrName;

		pdescr->lEnabled = ::GetValueInt( GetAppUnknown(), strSection, "Enable", pdescr->lEnabled );

		CString	strUserName = pdescr->bstrUserName;
		CString	strDefFormat = pdescr->bstrDefFormat;
		if( pdescr->bstrUserName )
			::SysFreeString( pdescr->bstrUserName );	pdescr->bstrUserName = 0;
		if( pdescr->bstrDefFormat )
			::SysFreeString( pdescr->bstrDefFormat );	pdescr->bstrDefFormat = 0;

		pdescr->bstrUserName = ::GetValueString( GetAppUnknown(), strSection, "UserName", strUserName ).AllocSysString();
		pdescr->bstrDefFormat = ::GetValueString( GetAppUnknown(), strSection, "Format", strDefFormat ).AllocSysString();
		pdescr->lTableOrder = ::GetValueInt( GetAppUnknown(), strSection, "TableOrder", pdescr->lTableOrder );
		if (pdescr->lEnabled)
		{
			LoadMask(&pdescr->dwPanesMask, 1, strSection, "UsedPanes");
			m_dwPanesMask |= pdescr->dwPanesMask;
			LoadMask(pdescr->adwPhasesMask, 4, strSection, "UsedPhases");
		}
		else
		{
			InitMask(&pdescr->dwPanesMask, 1);
			InitMask(pdescr->adwPhasesMask, 4);
		}
	}

	return true;
}



/////////////////////////////////////////////////////////////////////////////
// CMeasParamGroupBase

IMPLEMENT_DYNAMIC(CMeasParamGroupBase, CCmdTargetEx);

BEGIN_INTERFACE_MAP(CMeasParamGroupBase, CCmdTargetEx)
	INTERFACE_PART(CMeasParamGroupBase, IID_IMeasParamGroup,	Group)
	INTERFACE_PART(CMeasParamGroupBase, IID_INamedObject2,		Name)
	INTERFACE_PART(CMeasParamGroupBase, IID_INumeredObject,		Num)
	INTERFACE_PART(CMeasParamGroupBase, IID_IInitializeObjectVT,	Init)
	INTERFACE_PART(CMeasParamGroupBase, IID_IPersist,			Persist)
	INTERFACE_PART(CMeasParamGroupBase, IID_IEventListener,		EvList)
END_INTERFACE_MAP()



CMeasParamGroupBase::CMeasParamGroupBase()
{
	m_sName = "General";

	Register( GetAppUnknown() );
}

CMeasParamGroupBase::~CMeasParamGroupBase()
{
	UnRegister( GetAppUnknown() );
}

bool CMeasParamGroupBase::Init()
{
	LoadCreateParam();
	ReloadState();
	return CInitializeObjectImpl::Init();
}

void CMeasParamGroupBase::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szEventNewSettings ) )
		ReloadState();
}

HRESULT CMeasParamGroupBase::GetPriority(long *plPriority)
{
	*plPriority = 0;
	return S_OK;
}