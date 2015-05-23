#include "stdafx.h"
#include "common.h"
#include "meas_utils.h"


CMeasureContext::CMeasureContext(bool bCalc, IUnknown *punkObjLst)
{
	Init(bCalc,punkObjLst);
}

void CMeasureContext::Init(bool bCalc, IUnknown *punkObjLst)
{
	m_bCalc = bCalc;
	if (!m_bCalc) return;
	IUnknownPtr punk(FindComponentByName(GetAppUnknown(false), IID_IMeasureManager, szMeasurement), false);
	m_ptrManager = ICompManagerPtr(punk);
	int	nGroup;
	m_ptrManager->GetCount(&m_nGroupCount);
	if (bCalc)
	{
		if (m_ptrManager == 0)
			return;
		for(nGroup = 0; nGroup < m_nGroupCount; nGroup++ )
		{
			IUnknownPtr	ptrG;
			m_ptrManager->GetComponentUnknownByIdx(nGroup, &ptrG);
			IMeasParamGroupPtr	ptrGroup( ptrG );
			if( ptrGroup == 0 )continue;
			ptrGroup->InitializeCalculation(punkObjLst);
		}
	}
}

void CMeasureContext::Calc(IUnknown *punkCalcObj, IUnknown *punkImage)
{
	if (m_bCalc)
		::CalcObject(punkCalcObj, punkImage, false, m_ptrManager);
}

void CMeasureContext::DeInit()
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
	}
}

