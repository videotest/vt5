#include "stdafx.h"
#include "Cromos.h"
#include "Normalize.h"
#include "math.h"
#include "ParamsKeys.h"
#include "woolz.h"
#include "Settings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IUnknownPtr find_child_by_interface(IUnknownPtr sptrParent, const GUID guid);
IUnknownPtr find_woolz_in_list(IUnknownPtr sptrObjectsList)
{
	long pos = 0;
	sptrINamedDataObject2 sptrN(sptrObjectsList);
	sptrN->GetFirstChildPosition((long*)&pos);
	while (pos)
	{
		IUnknownPtr sptr;
		sptrN->GetNextChild((long*)&pos, &sptr);
		if (bool(sptr))
		{
			ICalcObjectPtr sptrObj((IUnknown *)sptr);
			IUnknownPtr sptrW = find_child_by_interface(sptr, IID_IWoolzObject);
			bool b = sptrW;
			if (b)
				return sptrW;
		}
	}
	return NULL;
}


double FindMediana(double *pdFirstObject, int nAmountObjects)
{
	int i,ii,nEnd;
	double nTmp=0,dMediana;
	for(ii=0;ii<nAmountObjects;ii++)
	for(i=1;i<nAmountObjects;i++)
	{
		if(pdFirstObject[i]>pdFirstObject[i-1]) 
		{
			nTmp=pdFirstObject[i-1];
			pdFirstObject[i-1]=pdFirstObject[i];
			pdFirstObject[i]=nTmp;
		}
	}
	for(i=0;i<nAmountObjects;i++) if(pdFirstObject[i]==-1) break;
	nEnd=i;
	if (nEnd&1)
		dMediana = pdFirstObject[nEnd/2];
	else
		dMediana = (pdFirstObject[nEnd/2]+pdFirstObject[nEnd/2-1])/2;
	return dMediana;
}

class CNormBase : public INormalize
{
protected:
	int m_nKey;
	int m_nNewKey;
	int m_nCount;
	double *m_pdValues;
public:
	CNormBase(int nKey, int nNewKey, int nCount, double *pdValues)
	{
		m_nKey = nKey;
		m_nNewKey = nNewKey;
		m_nCount = nCount;
		m_pdValues = pdValues;
	}
	void OnGetValue(int nObj, ICalcObjectPtr &sptrCalcObj)
	{
		m_pdValues[nObj] = 0.;
		sptrCalcObj->GetValue(m_nKey, &m_pdValues[nObj]);	
	}
	virtual double Normalize(double fValue) {return fValue;}
	void OnSetValue(int nObj, ICalcObjectPtr &sptrCalcObj)
	{
		double fValue;
		sptrCalcObj->GetValue(m_nKey, &fValue);	
		sptrCalcObj->SetValue(m_nNewKey, Normalize(fValue));
	}
};

class CNormMedian : public CNormBase
{
//	double m_dMediana;
	double a1,b1;
	int a,b;
public:
	CNormMedian(int nKey, int nNewKey, int nCount, double *pdValues) : CNormBase(nKey,nNewKey,nCount,pdValues) {}
	void AfterGetValues()
	{
		double dMediana = FindMediana(m_pdValues, m_nCount);
		if (g_FloatArith == 1)
		{
			a1 = 1. / dMediana;
			b1 = -1;
		}
		else
		{
			a = 500 * 256 / dMediana;
			b = -500 * 256;
		}
	}
	double Normalize(double fValue)
	{
		if (g_FloatArith == 1)
			return a1*fValue+b1;
		else
			return (a*int(fValue)+b)/256;
	}
};

class CNormMean : public CNormBase
{
	double a1,b1;
	int a,b;
public:
	CNormMean(int nKey, int nNewKey, int nCount, double *pdValues) : CNormBase(nKey,nNewKey,nCount,pdValues) {}
	void AfterGetValues()
	{
		double dSum = 0.,dSum2 = 0.;
		for (int i = 0; i < m_nCount; i++)
		{
			dSum += m_pdValues[i];
			dSum2 += m_pdValues[i]*m_pdValues[i];
		}
		double dAver = dSum/m_nCount;
		double dStdDev = sqrt((dSum2-m_nCount*dAver*dAver)/(m_nCount-1));
		if (g_FloatArith == 1)
		{
			if (dStdDev != 0)
				a1 = 1. / dStdDev;
			else
				a1 = 0.;
			b1 = -dAver*a1;
		}
		else
		{
			if (dStdDev != 0)
				a = 100 * 256 / dStdDev;
			else
				a = 100;
			b = -dAver*a;
		}
	}
	double Normalize(double fValue)
	{
		if (g_FloatArith == 1)
			return a1*fValue+b1;
		else
			return (a*int(fValue)+b)/256;
	}
};

class CNormFixed : public CNormBase
{
	double a,b;
public:
	CNormFixed(int nKey, int nNewKey, int nCount, double *pdValues, double a, double b) : CNormBase(nKey,nNewKey,nCount,pdValues)
	{
		this->a = a;
		this->b = b;
	}
	double Normalize(double fValue) {return a*fValue+b;}
};

class CNormADL : public CNormBase
{
protected:
	double m_dAreaSum,m_dMassSum,a;
	double m_dMedianaArea;
	double m_dMedianaLenght;
	double *m_pdLength;
	double *m_pdMasses;
	double *m_pdDens;
public:
	CNormADL(int nCount, double *pdValues) : CNormBase(KEY_DENSITY,KEY_NORMA_DENSITY,nCount,pdValues)
	{
		m_pdLength = new double[nCount];
		m_pdMasses = new double[nCount];
		m_pdDens = new double[nCount];
	}
	~CNormADL()
	{
		delete m_pdLength;
		delete m_pdMasses;
		delete m_pdDens;
	}
	void Init()
	{
		m_dAreaSum = m_dMassSum = 0.;
	};
	void OnGetValue(int nObj, ICalcObjectPtr &sptrCalcObj)
	{
		sptrCalcObj->GetValue(KEY_NEW_AREA, &m_pdValues[nObj]);
		sptrCalcObj->GetValue(KEY_MASS, &m_pdMasses[nObj]);
		sptrCalcObj->GetValue(KEY_DENSITY, &m_pdDens[nObj]);
		sptrCalcObj->GetValue(KEY_LENGHT, &m_pdLength[nObj]);
		m_dAreaSum += m_pdValues[nObj];
		m_dMassSum += m_pdMasses[nObj];
	}
	void AfterGetValues()
	{
//		double dMeanDens = m_dDensSum/m_dAreaSum;
		if (g_FloatArith == 1)
			a = m_dAreaSum;
		else
			a = 100.*m_dAreaSum;
		if (m_dMassSum>0.) a /= m_dMassSum;
		double *p = new double[m_nCount];
		memcpy(p,m_pdValues,m_nCount*sizeof(double));
		m_dMedianaArea = FindMediana(p,m_nCount);
		memcpy(p,m_pdLength,m_nCount*sizeof(double));
		m_dMedianaLenght = FindMediana(p,m_nCount);
		delete p;
	}
	void OnSetValue(int nObj, ICalcObjectPtr &sptrCalcObj)
	{
		if (g_FloatArith == 1)
		{
			double dNormaArea = m_pdValues[nObj]/m_dMedianaArea;
			double dNormaLenght = m_pdLength[nObj]/m_dMedianaLenght;
			dNormaLenght = (dNormaLenght+dNormaArea-2)/3;
			dNormaArea -= 1.;
			double dNormaSize = dNormaLenght;
			sptrCalcObj->SetValue(KEY_NORMA_AREA, dNormaArea);
			sptrCalcObj->SetValue(KEY_NORMA_DENSITY, a*m_pdDens[nObj]);
			sptrCalcObj->SetValue(KEY_NORMA_LENGHT, dNormaLenght);
			sptrCalcObj->SetValue(KEY_NORMA_SIZE, dNormaSize);
		}
		else
		{
			int dNormaArea = (int(500*256/m_dMedianaArea))*int(m_pdValues[nObj])/256;
			int alen = int(50*256/m_dMedianaLenght);
			int dNormaLenght = alen*int(m_pdLength[nObj])/256;
			dNormaLenght = 10*dNormaLenght+dNormaArea;
			dNormaArea -= 500;
			dNormaLenght = (dNormaLenght-1000)/3;
			double dNormaSize = dNormaLenght;
			sptrCalcObj->SetValue(KEY_NORMA_AREA, (double)dNormaArea);
			sptrCalcObj->SetValue(KEY_NORMA_DENSITY, (int(a*256)*int(m_pdDens[nObj]))/256);
			sptrCalcObj->SetValue(KEY_NORMA_LENGHT, (double)dNormaLenght);
			sptrCalcObj->SetValue(KEY_NORMA_SIZE, dNormaSize);
		}
	}
};

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionActionCalcNrmPar, CCmdTargetEx);

#ifdef _DEBUG
//[ag]2. olecreate debug

// {CE1FC9E2-8598-11d4-8336-AA082F165473}
GUARD_IMPLEMENT_OLECREATE(CActionActionCalcNrmPar, "CalcNormalParametrs.ActionCalcNrmParD",
0xce1fc9e2, 0x8598, 0x11d4, 0x83, 0x36, 0xaa, 0x8, 0x2f, 0x16, 0x54, 0x73);

//[ag]3. guidinfo debug

// {CE1FC9E0-8598-11d4-8336-AA082F165473}
static const GUID guidActionCalcNrmPar =
{ 0xce1fc9e0, 0x8598, 0x11d4, { 0x83, 0x36, 0xaa, 0x8, 0x2f, 0x16, 0x54, 0x73 } };

#else
//[ag]4. olecreate release

// {CE1FC9E3-8598-11d4-8336-AA082F165473}
GUARD_IMPLEMENT_OLECREATE(CActionActionCalcNrmPar, "CalcNormalParametrs.ActionCalcNrmPar",
0xce1fc9e3, 0x8598, 0x11d4, 0x83, 0x36, 0xaa, 0x8, 0x2f, 0x16, 0x54, 0x73);

//[ag]5. guidinfo release

// {CE1FC9E1-8598-11d4-8336-AA082F165473}
static const GUID guidActionCalcNrmPar =
{ 0xce1fc9e1, 0x8598, 0x11d4, { 0x83, 0x36, 0xaa, 0x8, 0x2f, 0x16, 0x54, 0x73 } };

#endif //_DEBUG


//[ag]6. action info

ACTION_INFO_FULL(CActionActionCalcNrmPar,IDS_ACTION_CALCNRMPARAM, -1, -1, guidActionCalcNrmPar);

//[ag]7. targets

ACTION_TARGET(CActionActionCalcNrmPar,"anydoc");

//[ag]8. arguments


//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionActionCalcNrmPar implementation
CActionActionCalcNrmPar::CActionActionCalcNrmPar()
{
}

CActionActionCalcNrmPar::~CActionActionCalcNrmPar()
{
}

void CActionActionCalcNrmPar::NormVals(CObjectListWrp &list, int nCount, INormalize *pNorm, bool bDestroy)
{
	int i;
	POSITION sPos;
	IUnknown *punk;
	pNorm->Init();
	// Get values from objects list
	sPos = list.GetFirstObjectPosition();	
	for(i=0;i<nCount;i++)
	{
		double pfValue=-1;
		punk = list.GetNextObject(sPos);
		ICalcObjectPtr	sptrCalcObj = punk;
		if (punk) punk->Release();
		pNorm->OnGetValue(i,sptrCalcObj);
	}
	pNorm->AfterGetValues();
	// Set up new values
	sPos = list.GetFirstObjectPosition();	
	for(i=0;i<nCount;i++)
	{
		double pfValue=-1;
		punk = list.GetNextObject(sPos);
		ICalcObjectPtr	sptrCalcObj = punk;
		if (punk) punk->Release();
		pNorm->OnSetValue(i,sptrCalcObj);
	}
	pNorm->Deinit();
	if (bDestroy) delete pNorm;
}

int GetFeatValue(ICalcObjectPtr &sptrCalcObj, long lParamKey)
{
	double dValue = 0.;
	sptrCalcObj->GetValue(lParamKey, &dValue);
	return int(dValue);
}

extern int g_CompareData;

bool CActionActionCalcNrmPar::Invoke()
{
	long nCount;
	double* pdFirstObject;	
	if (!m_punkTarget)
		return false;
	bool bDone = false;
	IUnknown *punk = GetActiveList();
	if (!punk) return false;
	IUnknownPtr ptrList(punk);
	punk->Release();
	if (g_CalcMethod == 6 || g_CompareData)
	{
		IUnknownPtr sptrWoolz = find_woolz_in_list(ptrList);
		bool b = sptrWoolz;
		if (b)
		{
			IWoolzObjectPtr wo((IUnknown *)sptrWoolz);
			wo->NormalizeParameters(ptrList);
			if (g_CalcMethod == 6)
				bDone = true;
		}
	}
	if (!bDone)
	{
		CObjectListWrp list;
		list.Attach(GetActiveList(), false);
		nCount=list.GetCount();
		IDataObjectListPtr sptrList = list;
		pdFirstObject = new double[nCount];
		if(nCount==0) return false;
		NormVals(list, nCount, new CNormADL(nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormFixed(KEY_AREACI, KEY_NORMA_AREACI, nCount, pdFirstObject, 10, -250), true);
		NormVals(list, nCount, new CNormMedian(KEY_NEW_PERIMETR, KEY_NORMA_PERIMETR, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_WDD1, KEY_NORMA_WDD1, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_WDD2, KEY_NORMA_WDD2, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_WDD2P, KEY_NORMA_WDD2P, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_WDD3, KEY_NORMA_WDD3, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_WDD4, KEY_NORMA_WDD4, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_WDD5, KEY_NORMA_WDD5, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_WDD6, KEY_NORMA_WDD6, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_MWDD1, KEY_NORMA_MWDD1, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_MWDD2, KEY_NORMA_MWDD2, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_MWDD2P, KEY_NORMA_MWDD2P, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_MWDD3, KEY_NORMA_MWDD3, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_MWDD4, KEY_NORMA_MWDD4, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_MWDD5, KEY_NORMA_MWDD5, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_MWDD6, KEY_NORMA_MWDD6, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_GWDD1, KEY_NORMA_GWDD1, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_GWDD2, KEY_NORMA_GWDD2, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_GWDD2P, KEY_NORMA_GWDD2P, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_GWDD3, KEY_NORMA_GWDD3, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_GWDD4, KEY_NORMA_GWDD4, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_GWDD5, KEY_NORMA_GWDD5, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_GWDD6, KEY_NORMA_GWDD6, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_CVDD, KEY_NORMA_CVDD, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_NSSD, KEY_NORMA_NSSD, nCount, pdFirstObject), true);
		NormVals(list, nCount, new CNormMean(KEY_MDRA, KEY_NORMA_MDRA, nCount, pdFirstObject), true);
		delete pdFirstObject;
		if (g_CompareData)
		{
			POSITION sPos;
			IUnknown *punk;
			// Get values from objects list
			sPos = list.GetFirstObjectPosition();	
			for(int i = 0; i < nCount; i++)
			{
				double pfValue=-1;
				punk = list.GetNextObject(sPos);
				ICalcObjectPtr	sptrCO = punk;
				if (punk) punk->Release();
				int nFeat[30];
				nFeat[0] = GetFeatValue(sptrCO,KEY_NORMA_AREA);
				nFeat[1] = GetFeatValue(sptrCO,KEY_NORMA_LENGHT);
				nFeat[2] = GetFeatValue(sptrCO,KEY_NORMA_DENSITY);
				nFeat[3] = GetFeatValue(sptrCO,KEY_NORMA_AREACI);
				nFeat[4] = GetFeatValue(sptrCO,KEY_AREACI_DENSCI);
				nFeat[5] = GetFeatValue(sptrCO,KEY_NORMA_CVDD);
				nFeat[6] = GetFeatValue(sptrCO,KEY_NORMA_NSSD);
				nFeat[7] = GetFeatValue(sptrCO,KEY_NORMA_MDRA);
				nFeat[8] = GetFeatValue(sptrCO,KEY_NORMA_WDD1);
				nFeat[9] = GetFeatValue(sptrCO,KEY_NORMA_WDD2);
				nFeat[10] = GetFeatValue(sptrCO,KEY_NORMA_WDD3);
				nFeat[11] = GetFeatValue(sptrCO,KEY_NORMA_WDD4);
				nFeat[12] = GetFeatValue(sptrCO,KEY_NORMA_WDD5);
				nFeat[13] = GetFeatValue(sptrCO,KEY_NORMA_WDD6);
				nFeat[14] = GetFeatValue(sptrCO,KEY_NORMA_MWDD1);
				nFeat[15] = GetFeatValue(sptrCO,KEY_NORMA_MWDD2);
				nFeat[16] = GetFeatValue(sptrCO,KEY_NORMA_MWDD3);
				nFeat[17] = GetFeatValue(sptrCO,KEY_NORMA_MWDD4);
				nFeat[18] = GetFeatValue(sptrCO,KEY_NORMA_MWDD5);
				nFeat[19] = GetFeatValue(sptrCO,KEY_NORMA_MWDD6);
				nFeat[20] = GetFeatValue(sptrCO,KEY_NORMA_GWDD1);
				nFeat[21] = GetFeatValue(sptrCO,KEY_NORMA_GWDD2);
				nFeat[22] = GetFeatValue(sptrCO,KEY_NORMA_GWDD3);
				nFeat[23] = GetFeatValue(sptrCO,KEY_NORMA_GWDD4);
				nFeat[24] = GetFeatValue(sptrCO,KEY_NORMA_GWDD5);
				nFeat[25] = GetFeatValue(sptrCO,KEY_NORMA_GWDD6);
				nFeat[26] = GetFeatValue(sptrCO,KEY_LENCI);
				nFeat[27] = GetFeatValue(sptrCO,KEY_NORMA_PERIMETR);
				IUnknownPtr s111 = find_child_by_interface(sptrCO, IID_IWoolzObject);
				bool b = s111;
				if (b)
				{
					IWoolzObjectPtr wo((IUnknown *)s111);
					CHROMOFEATURES cf;
					cf.nObj = i;
					cf.nFeatsNum = 28;
					cf.pnFeats = nFeat;
					wo->Compare(COMPARE_NFEATURES, cf.nFeatsNum, &cf);
				}
			}
		}
	}
	// Now all OK. Redraw document.
	IDataObjectListPtr list(ptrList);
	if(list)
	{
		BOOL bLocked;
		list->GetObjectUpdateLock( &bLocked );
		if(bLocked) return true;
	}
	INamedDataObject2Ptr NDO(ptrList);
	IUnknownPtr ptrNC;
	HRESULT hr = NDO->GetData(&ptrNC);
	if (SUCCEEDED(hr))
	{
		INotifyControllerPtr sptrNC(ptrNC);
		long l = cncReset;
		if (sptrNC != 0)
			sptrNC->FireEvent(_bstr_t(szEventChangeObjectList), NULL, NDO, &l, 0);
	}
	return true;
}

//extended UI
bool CActionActionCalcNrmPar::IsAvaible()
{
	return true;
}

bool CActionActionCalcNrmPar::IsChecked()
{
	return false;
}

IUnknown	*CActionActionCalcNrmPar::GetActiveList()
{
	if( CheckInterface( m_punkTarget, IID_IDocument ) )
		return ::GetActiveObjectFromDocument( m_punkTarget, szTypeObjectList );
	else
		return ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
}

