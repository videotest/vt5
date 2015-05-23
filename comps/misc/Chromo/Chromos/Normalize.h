#ifndef __CalcNormParam_h__
#define __CalcNormParam_h__

#include "ClassBase.h"
#include "math.h"
#include "ChromParam.h"

class INormalize
{
public:
	virtual ~INormalize() {};
	virtual void Init() {};
	virtual void OnGetValue(int nObj, ICalcObjectPtr &sptrCalcObj) {};
	virtual void AfterGetValues() {};
	virtual void OnSetValue(int nObj, ICalcObjectPtr &sptrCalcObj) {};
	virtual void Deinit() {};
};


//[ag]1. classes

class CActionActionCalcNrmPar : public CActionBase
{
	DECLARE_DYNCREATE(CActionActionCalcNrmPar)
	GUARD_DECLARE_OLECREATE(CActionActionCalcNrmPar)

	void NormVals(CObjectListWrp &list, int nCount, INormalize *pNorm, bool bDestroy);

public:
	CActionActionCalcNrmPar();
	virtual ~CActionActionCalcNrmPar();
	IUnknown* GetActiveList();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
protected:

	void GetDensity(IUnknown* punk,CChromParam*);
	void GetShape(IUnknown* punk,CChromParam*);
	double  FindMediana(int,double*);
	double  FindAverDark(int,double*);
	double  FindHistDark(int,double*);
};


#endif //__CalcNormParam_h__
