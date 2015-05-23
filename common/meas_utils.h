#pragma once

class std_dll CMeasureContext
{
	bool m_bCalc;
	ICompManagerPtr	m_ptrManager;
	int m_nGroupCount;
public:
	CMeasureContext(bool bCalc, IUnknown *punkObjLst);
	void Init(bool bCalc, IUnknown *punkObjLst);
	void Calc(IUnknown *punkCalcObj, IUnknown *punkImage);
	void DeInit();
};
