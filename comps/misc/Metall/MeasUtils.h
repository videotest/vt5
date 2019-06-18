#if !defined(__MeasUtils_H__)
#define __MeasUtils_H__

#include "debug.h"

inline IUnknownPtr FindComponentByName(IUnknown *punkThis, const IID iid, _bstr_t strName)
{
	_bstr_t strCompName;
	if (CheckInterface(punkThis, iid))
	{
		strCompName = GetName(punkThis);
		if (strCompName == strName)
			return punkThis;
	}
	if (CheckInterface(punkThis, IID_ICompManager))
	{
		ICompManagerPtr	sptr(punkThis);
		int nCount = 0;
		sptr->GetCount(&nCount);
		for (int i = 0; i < nCount; i++)
		{
			IUnknownPtr punk;
			sptr->GetComponentUnknownByIdx(i, &punk);
			if (punk != 0)
			{
				if (CheckInterface(punk, iid))
				{
					strCompName = ::GetName(punk);
					if (strCompName == strName)
						return punk;
				}
			}
		}
	}
	IUnknownPtr punk;
	if (CheckInterface(punkThis, IID_IRootedObject))
	{
		IRootedObjectPtr	sptrR(punkThis);
		IUnknownPtr punkParent;
		sptrR->GetParent(&punkParent);
		punk = FindComponentByName(punkParent, iid, strName);
	}
	return punk;
}


class CMeasureContext
{
	bool m_bCalc;
	ICompManagerPtr	m_ptrManager;
	int m_nGroupCount;
	MTracePointsArray(m_TracePoints);
public:
	CMeasureContext(bool bCalc, IUnknown *punkObjLst)
	{
		Init(bCalc,punkObjLst);
	}
	void Init(bool bCalc, IUnknown *punkObjLst)
	{
		m_bCalc = bCalc;
		m_nGroupCount = 0;
		if (!m_bCalc) return;
		IUnknownPtr punk(FindComponentByName(GetAppUnknown(), IID_IMeasureManager, szMeasurement));
		m_ptrManager = ICompManagerPtr(punk);
		int	nGroup;
		m_ptrManager->GetCount(&m_nGroupCount);
		if (bCalc)
		{
			if (m_ptrManager == 0)
				return;
			InitTracePointsArray(m_TracePoints,"Measure",m_nGroupCount);
			for(nGroup = 0; nGroup < m_nGroupCount; nGroup++ )
			{
				IUnknownPtr	ptrG;
				m_ptrManager->GetComponentUnknownByIdx(nGroup, &ptrG);
				IMeasParamGroupPtr	ptrGroup( ptrG );
				if( ptrGroup == 0 )continue;
				ptrGroup->InitializeCalculation(punkObjLst);
				TPA_InitName(m_TracePoints,"Measure",nGroup,ptrGroup);
			}
		}
	}
	void Calc(IUnknown *punkCalcObj, IUnknown *punkImage)
	{
		if (m_bCalc)
		{
			int	nCount;
			m_ptrManager->GetCount(&nCount);
			for (int idx = 0; idx < nCount; idx++)
			{
				StartTracePointInArray(m_TracePoints,idx);
				IUnknownPtr punk;
				m_ptrManager->GetComponentUnknownByIdx(idx, &punk);
				IMeasParamGroupPtr	ptrG(punk);
				if (ptrG == 0) continue;
				ptrG->CalcValues(punkCalcObj, punkImage);
			}
		}
		StopTracePoint(m_TracePoints);
	}
	void DeInit()
	{
		if (m_bCalc)
		{
			for (int nGroup = 0; nGroup < m_nGroupCount; nGroup++)
			{
				IUnknownPtr	ptrG;
				m_ptrManager->GetComponentUnknownByIdx(nGroup, &ptrG);
				IMeasParamGroupPtr	ptrGroup(ptrG);
				if (ptrGroup == 0) continue;
				ptrGroup->FinalizeCalculation();
			}
			m_bCalc = false;
			MsgBoxTracePoint(m_TracePoints);
		}
	}
	ICompManagerPtr GetMeasMgr() {return m_ptrManager;}
	int GetMeasGroupCount() {return m_nGroupCount;}
};

inline IUnknownPtr find_child_by_interface(IUnknownPtr sptrParent, const GUID guid)
{
	INamedDataObject2Ptr sptrNDOParent(sptrParent);
	long lPos = 0;
	sptrNDOParent->GetFirstChildPosition(&lPos);
	while (lPos)
	{
		IUnknownPtr sptrChild;
		sptrNDOParent->GetNextChild(&lPos, &sptrChild);
		if (bool(sptrChild))
		{
			if (::CheckInterface(sptrChild, guid))
				return sptrChild;
		}
	}
	return NULL;
}

#endif