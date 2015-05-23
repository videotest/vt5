#include "stdAfx.h"
#include "DataView.h"
#include "ClassBase.h"


///////////////////////////////////////////////////////////////////////////////
// class CGridObject


CGridObject::CGridObject(IUnknown* punk, CObjectListWrpEx* pParent)
{
	m_nRow = -1;

	m_pWrpParent = pParent;
	if (CheckInterface(punk, IID_ICalcObject))
		m_sptrObj =  punk;

//	AddParams();
}

CGridObject::CGridObject(const CGridObject& obj)
{
	m_pWrpParent = obj.m_pWrpParent;
	m_sptrObj = obj.m_sptrObj;
	m_nRow = obj.m_nRow;

//	AddParams();
}


CGridObject::~CGridObject()
{
//	m_mapParams.RemoveAll();
	m_sptrObj = 0;
}

bool CGridObject::GetParam(long lKey, double & fVal)
{
	fVal = 0;
	return  SUCCEEDED(m_sptrObj->GetValue(lKey, &fVal)) ? true : false;
}

/*
bool CGridObject::GetParamByKey(long lKey, double & fVal)
{
	int npos = FindParam(lKey);
	if (npos < 0 || npos >= m_arrParams.GetSize())
		return false;

	fVal = m_arrParams[npos].fValue;

	return m_arrParams[npos].bExists ? true : false;
}

bool CGridObject::GetParamByPos(int pos, long & lKey, double & fVal)
{
	if (pos < 0 || pos >= m_arrParams.GetSize())
		return false;

	TParam param = m_arrParams[pos];
	lKey = param.lParamKey;
	fVal = param.fValue;

	return param.bExists ? true : false;
}

int CGridObject::FindParam(long lKey)
{
	if (lKey == -1)
		return -1;

	if (m_pWrpParent)
	{
		CCalcParamInfoEx* pInfo = 0;
		if (m_pWrpParent->GetParamInfoMap().Lookup(lKey, pInfo) && pInfo)
			return pInfo->GetPos();
	}
	return FindParamInArray(lKey);
}

int CGridObject::FindParamInMap(long lKey)
{
	if (lKey == -1)
		return -1;

	for (int i = 0; i < m_arrParams.GetSize(); i++)
	{
		if (m_arrParams[i].lParamKey == lKey)
			return i;
	}
	return -1;
}

// enum all params for object and add it to params array
bool CGridObject::AddParams()
{
	m_mapParams.RemoveAll();

	if (m_sptrObj == 0)
		return false;

	if (!m_pWrpParent)
		return false;

	CParamInfoExMap& mapParam = m_pWrpParent->GetParamInfoMap();

	if (mapParam.IsEmpty())
		return true;

	CCalcParamInfoEx* pInfo = 0;
	long lKey = -1;
	POSITION pos = mapParam.GetStartPosition();
	while (pos)
	{
		mapParam.GetNextAssoc(pos, lKey, pInfo);
		if (pInfo && lKey != -1)
		{
			double fValue = 0;
			TParam param;
			if (FAILED(m_sptrObj->GetValue(lKey, &fValue)))
			{	
				param.bExists = false;
			}
			else 
			{
				param.lParamKey = lKey;
				param.fValue = fValue;
				param.bExists = true;
			}	
			m_arrParams.InsertAt(pInfo->GetPos(), param);
		}
	}

	return true;
}
*/
void CGridObject::SetParent(CObjectListWrpEx* pParent)
{
//	m_arrParams.RemoveAll();
	m_pWrpParent = pParent;
//	AddParams();
}

void CGridObject::SetObject(IUnknown* punk)
{
	if (!CheckInterface(punk, IID_ICalcObject))
	{
//		m_arrParams.RemoveAll();
		m_sptrObj = 0;
		return;
	}

	if (m_sptrObj != 0 && ::GetObjectKey(m_sptrObj) == ::GetObjectKey(punk))
		return;

	m_sptrObj = punk;

//	AddParams();
}


///////////////////////////////////////////////////////////////////////////////
// class CObjectListWrpEx

CObjectListWrpEx::CObjectListWrpEx(IUnknown *punkList, bool bAddRef, LPCTSTR szName)
	:	CObjectListWrp(punkList, bAddRef)
{
	m_strMgrName = szName;
	if (!m_strMgrName.IsEmpty())
		LoadParamsInfo();
}

CObjectListWrpEx::~CObjectListWrpEx()
{
	Clear();
}


void CObjectListWrpEx::Attach(IUnknown *punkList, bool bAddRef)
{
	Clear();
	CObjectListWrp::Attach(punkList, bAddRef);
/*
	// this code will run very sloly (because objectList can contains too many objects)
	POSITION pos = GetFirstObjectPosition();
	// for all objects in this list
	bool bRet = true;
	while (pos)
	{
		IUnknown *punk = GetNextObject(pos);

		if (punk)
		{
			bRet = GetObjectParams(punk) && bRet;
			punk->Release();
		}
		else
			bRet = false;
	}
*/
	LoadParamsInfo();
}

void CObjectListWrpEx::Clear()
{
	// clear params info
	if (m_mapParam.IsEmpty())
		return;

	long lKey = -1;
	CCalcParamInfoEx *pInfo = 0;
	for (POSITION pos = m_mapParam.GetStartPosition(); pos != NULL; )
	{
		m_mapParam.GetNextAssoc(pos, lKey, pInfo);
		if (pInfo)
			delete pInfo;
	}
	m_mapParam.RemoveAll();
}


bool CObjectListWrpEx::LoadObjectParams(IUnknown* punkObj)
{
	if (!CheckInterface(punkObj, IID_ICalcObject))
		return false;

	ICalcObjectPtr sptrC = punkObj;

	// for all params
	long lPos = 0;
	sptrC->GetFirstValuePos(&lPos);
	while (lPos)
	{
		long lKey = -1;
		double fValue = 0;
		// get param key
		if (FAILED(sptrC->GetNextValue(&lPos, &lKey, &fValue)) || lKey == -1)
			continue;

		// check same key already exists
		CCalcParamInfoEx *pInfo = 0;
		if (m_mapParam.Lookup(lKey, pInfo))
			continue;
		// if not exists -> add it to param Info map
		pInfo = new CCalcParamInfoEx(lKey);
		m_mapParam.SetAt(lKey, pInfo);
	}
	// we need to fill all paramInfo from meas param group
//	return LoadParamsInfo();

	return true;
}

bool CObjectListWrpEx::CheckParamExists(long lKey)
{
	CCalcParamInfoEx* pInfo = 0;
	return m_mapParam.Lookup(lKey, pInfo) ? true : false;
}

bool CObjectListWrpEx::LoadParamsInfo()
{
	if (m_strMgrName.IsEmpty())
		return false;

	if (m_sptrParamMan == 0)
	{
		IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IParamGroupManager, m_strMgrName);

		m_sptrParamMan = punk;
		if (m_sptrParamMan == 0)
			return false;

		punk->Release();
	}

	// check manager contains groups
	int nCount = 0;
	m_sptrParamMan->GetGroupCount(&nCount);
	if (!nCount)
		return true;

	long lPos = 0;
	if (FAILED(m_sptrParamMan->GetFirstPos(&lPos)))
		return false;

	bool bRet = true;
	int nNextPos = 0;
	// for all groups
	while (lPos)
	{
		IUnknown *punkGroup = 0;
		if (FAILED(m_sptrParamMan->GetNextGroup(&lPos, &punkGroup)) || !punkGroup)
		{
			bRet = false;
			continue;
		}

		sptrIMeasParamGroup sptrGroup = punkGroup;
		punkGroup->Release();

		if (sptrGroup == 0)
		{
			bRet = false;
			continue;
		}

		// get group's params count
		long lParamCount = 0;
		if (FAILED(sptrGroup->GetParamsCount(&lParamCount)))
		{
			bRet = false;
			continue;
		}
		if (!lParamCount)
			continue;

		long lMeasUnit = 0;
		//sptrGroup->GetCalibration(0, 0, &lMeasUnit);

		long lParamPos = 0;
		if (FAILED(sptrGroup->GetFirstPos(&lParamPos)))
		{
			bRet = false;
			continue;
		}

		// for all params in group
		while (lParamPos)
		{
			long lKey = -1;
			BSTR bstrName = 0;
			BSTR bstrFormat = 0;
			DWORD dwFlag = false;

			if (SUCCEEDED(sptrGroup->GetParamInfo(lParamPos, &lKey, &bstrName, &bstrFormat, &dwFlag)) && lKey != -1)
			{
				_bstr_t bstrParamName(bstrName, false);
				_bstr_t bstrParamFormat(bstrFormat, false);

				if ((dwFlag & CCalcParamInfo::efEnable) == CCalcParamInfo::efEnable)
				{
					// check patam exists
					CCalcParamInfoEx* pInfo = 0;
					if (!m_mapParam.Lookup(lKey, pInfo))
					{
						pInfo = new CCalcParamInfoEx(lKey);
						m_mapParam.SetAt(lKey, pInfo);
					}
					ASSERT (pInfo);
					if (!pInfo)
					{
						bRet = false;
						break;
					}
			
					pInfo->SetParamName(bstrParamName);
					pInfo->SetParamFormat(bstrParamFormat);
					pInfo->SetParamKey(lKey);
					pInfo->SetParamFlag(dwFlag);

					BSTR bstrUnitName = 0;
					if (SUCCEEDED(sptrGroup->GetParamUnitName(lKey, lMeasUnit, &bstrUnitName)) && bstrUnitName)
					{
						CString strUnitName = bstrUnitName;
						::SysFreeString(bstrUnitName);
						pInfo->SetUnitName(strUnitName);
					}
					
				}
			}

			if (FAILED(sptrGroup->GetNextPos(&lParamPos)))
			{
				bRet = false;
				break;
			}
			if (!lParamPos)
				break;
		} // for all params in group
	} // for all groups

	return bRet;
}

CCalcParamInfoEx* CObjectListWrpEx::GetParamInfo(long lKey)
{
	if (lKey == -1)
		return 0;

	CCalcParamInfoEx *pInfo = 0;
	if (!m_mapParam.Lookup(lKey, pInfo))
		return 0;

	return pInfo;
}

CCalcParamInfoEx* CObjectListWrpEx::GetParamInfoByPos(int nPos)
{
	CCalcParamInfoEx *pInfo = 0;
	long lKey = -1;
	for (POSITION pos = m_mapParam.GetStartPosition(); pos != NULL; )
	{
		m_mapParam.GetNextAssoc(pos, lKey, pInfo);
		if (pInfo && pInfo->GetPos() == nPos)
			return pInfo;
	}
	return 0;
}

bool CObjectListWrpEx::SetParamInfo(long lKey, CCalcParamInfoEx* pInfo)
{
	if (lKey == -1 || !pInfo)
		return false;

	CCalcParamInfoEx* pOld = 0;

	if (!m_mapParam.Lookup(lKey, pOld))
	{
		pOld = new CCalcParamInfoEx(lKey);
		m_mapParam.SetAt(lKey, pOld);
	}
	if (!pOld)
		return false;

	*pOld = *pInfo;
/*	
	pOld->SetParamName(pInfo->GetParamName());
	pOld->SetParamFormat(pInfo->GetParamFormat());
	pOld->SetParamFlag(pInfo->GetParamFlag());
	pOld->SetParamType(pInfo->GetParamType());
	pOld->SetParamAction(pInfo->GetParamAction());
	pOld->SetPos(pInfo->GetPos());
*/
	return true;
}

bool CObjectListWrpEx::AddParam(long lKey)
{
	if (lKey == -1)
		return false;

	CCalcParamInfoEx Info;
	if (FindParamInfo(lKey, &Info))
		return SetParamInfo(lKey, &Info);
	return false;
}


bool CObjectListWrpEx::RemoveParam(long lKey)
{
	if (lKey == -1)
		return false;

	CCalcParamInfoEx *pInfo = 0;
	if (!m_mapParam.Lookup(lKey, pInfo))
		return true;

	if (pInfo)
		delete pInfo;

	m_mapParam.SetAt(lKey, 0);
	m_mapParam.RemoveKey(lKey);

	return true;
}



bool CObjectListWrpEx::FindParamInfo(long lKey, CCalcParamInfoEx * pInfo)
{
	if (m_strMgrName.IsEmpty() || lKey == -1 || !pInfo)
		return false;

	if (m_sptrParamMan == 0)
	{
		IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IParamGroupManager, m_strMgrName);

		m_sptrParamMan = punk;
		if (m_sptrParamMan == 0)
			return false;

		punk->Release();
	}

	// check manager contains groups
	int nCount = 0;
	m_sptrParamMan->GetGroupCount(&nCount);
	if (!nCount)
		return false;

	long lPos = 0;
	if (FAILED(m_sptrParamMan->GetFirstPos(&lPos)))
		return false;

	int  nNextPos = 0;
	bool bFinded = false;
	// for all groups
	while (lPos && !bFinded)
	{
		IUnknown *punkGroup = 0;
		if (FAILED(m_sptrParamMan->GetNextGroup(&lPos, &punkGroup)) || !punkGroup)
			continue;

		sptrIMeasParamGroup sptrGroup = punkGroup;
		punkGroup->Release();

		if (sptrGroup == 0)
			continue;

		// get group's params count
		long lParamCount = 0;
		if (FAILED(sptrGroup->GetParamsCount(&lParamCount)) || !lParamCount)
			continue;

		long lParamPos = 0;
		if (FAILED(sptrGroup->GetFirstPos(&lParamPos)))
			continue;

		long lMeasUnit = 0;
		//sptrGroup->GetCalibration(0, 0, &lMeasUnit);

		// for all params in group
		while (lParamPos && !bFinded)
		{
			long lParamKey = -1;
			BSTR bstrName = 0;
			BSTR bstrFormat = 0;
			DWORD dwFlag = false;

			if (SUCCEEDED(sptrGroup->GetParamInfo(lParamPos, &lParamKey, &bstrName, &bstrFormat, &dwFlag)))
			{		
				_bstr_t bstrParamName(bstrName, false);
				_bstr_t bstrParamFormat(bstrFormat, false);

				if (lKey == lParamKey && (dwFlag & CCalcParamInfo::efEnable) == CCalcParamInfo::efEnable)
				{
					pInfo->SetParamName(bstrParamName);
					pInfo->SetParamFormat(bstrParamFormat);
					pInfo->SetParamKey(lKey);
					pInfo->SetParamFlag(dwFlag);
					
					BSTR bstrUnitName = 0;
					if (SUCCEEDED(sptrGroup->GetParamUnitName(lKey, lMeasUnit, &bstrUnitName)) && bstrUnitName)
					{
						CString strUnitName = bstrUnitName;
						::SysFreeString(bstrUnitName);
						pInfo->SetUnitName(strUnitName);
					}

					bFinded = true;
					break;
				}
			}

			if (FAILED(sptrGroup->GetNextPos(&lParamPos)) || !lParamPos)
				break;

		} // for all params in group
	} // for all groups

	return bFinded;
}


bool CObjectListWrpExEx::LoadParamsInfo()
{
	if (!CheckInterface((IUnknown*)*this, IID_IMeasParamGroup2))
		return CObjectListWrpEx::LoadParamsInfo();

	IMeasParamGroup2Ptr sptrG = *this;
	if (sptrG == 0)
		return CObjectListWrpEx::LoadParamsInfo();

	long lMeasUnit = 0;
	//sptrG->GetCalibration(0, 0, &lMeasUnit);

	long lPos = 0;
	if (FAILED(sptrG->GetFirstPos(&lPos)))
		return false;

	// for all params
	bool bRet = true;
	while (lPos)
	{
		long lKey = -1;
		BSTR bstrName = 0;
		BSTR bstrFormat = 0;
		BSTR bstrAction = 0;
		DWORD dwFlag = 0;
		DWORD dwType = 0;

		// get param info
		HRESULT hr = sptrG->GetParamInfo2(lPos, &lKey, &bstrName, &bstrFormat, &dwFlag, &dwType, &bstrAction);
		_bstr_t bstrParamName(bstrName, false);
		_bstr_t bstrParamFormat(bstrFormat, false);
		_bstr_t bstrActionName(bstrAction, false);

		if (FAILED(hr) || lKey == -1 || (dwFlag & CCalcParamInfo::efEnable) != CCalcParamInfo::efEnable)
		{
			bRet = false;
		}
		else
		{
			// check patam exists & create new if need
			CCalcParamInfoEx* pInfo = 0;
			if (!m_mapParam.Lookup(lKey, pInfo))
			{
				pInfo = new CCalcParamInfoEx(lKey);
				m_mapParam.SetAt(lKey, pInfo);
			}

			if (pInfo)
			{
				// set param members	
				pInfo->SetParamName(bstrParamName);
				pInfo->SetParamFormat(bstrParamFormat);
				pInfo->SetParamKey(lKey);
				pInfo->SetParamFlag(dwFlag);
				// for manual meas param only
				if ((dwFlag & CCalcParamInfo::efManual) == CCalcParamInfo::efManual)
				{
					pInfo->SetParamType(dwType);
					pInfo->SetParamAction(bstrActionName);
				}

				BSTR bstrUnitName = 0;
				if (SUCCEEDED(sptrG->GetParamUnitName(lKey, lMeasUnit, &bstrUnitName)) && bstrUnitName)
				{
					CString strUnitName = bstrUnitName;
					::SysFreeString(bstrUnitName);
					pInfo->SetUnitName(strUnitName);
				}
			}
			else
				bRet = false;
		}

		// get next param pos
		sptrG->GetNextPos(&lPos);
	} // for all params in group

	return bRet;
}

bool CObjectListWrpExEx::FindParamInfo(long lKey, CCalcParamInfoEx * pInfo)
{
	if (!CheckInterface((IUnknown*)*this, IID_IMeasParamGroup2))
		return CObjectListWrpEx::FindParamInfo(lKey, pInfo);

	if (!FindParamInfoInner(lKey, pInfo))
		return CObjectListWrpEx::FindParamInfo(lKey, pInfo);

	return true;
}

bool CObjectListWrpExEx::FindParamInfoInner(long lKey, CCalcParamInfoEx * pInfo)
{
	if (!pInfo || lKey == -1)
		return false;

	IMeasParamGroup2Ptr sptrG = *this;
	if (sptrG == 0)
		return false;

	long lPos = 0;
	if (FAILED(sptrG->GetPosByKey(lKey, &lPos)) || !lPos)
		return false;

	long lParamKey = -1;
	BSTR bstrName = 0;
	BSTR bstrFormat = 0;
	BSTR bstrAction = 0;
	DWORD dwFlag = 0;
	DWORD dwType = 0;

	long lMeasUnit = 0;
	//sptrG->GetCalibration(0, 0, &lMeasUnit);

	// get param info
	if (FAILED(sptrG->GetParamInfo2(lPos, &lParamKey, &bstrName, &bstrFormat, &dwFlag, &dwType, &bstrAction)))
		return false;
			
	_bstr_t bstrParamName(bstrName, false);
	_bstr_t bstrParamFormat(bstrFormat, false);
	_bstr_t bstrActionName(bstrAction, false);

	// if param key is invalid -> goto next param
	if (lParamKey != lKey)
		return false;

	if ((dwFlag & CCalcParamInfo::efEnable) != CCalcParamInfo::efEnable)
		return false;

	// set param members	
	pInfo->SetParamName(bstrParamName);
	pInfo->SetParamFormat(bstrParamFormat);
	pInfo->SetParamKey(lKey);
	pInfo->SetParamFlag(dwFlag);

	// for manual meas param only
	if ((dwFlag & CCalcParamInfo::efManual) == CCalcParamInfo::efManual)
	{
		pInfo->SetParamType(dwType);
		pInfo->SetParamAction(bstrActionName);
	}

	BSTR bstrUnitName = 0;
	if (SUCCEEDED(sptrG->GetParamUnitName(lKey, lMeasUnit, &bstrUnitName)) && bstrUnitName)
	{
		CString strUnitName = bstrUnitName;
		::SysFreeString(bstrUnitName);
		pInfo->SetUnitName(strUnitName);
	}

	return true;	
}