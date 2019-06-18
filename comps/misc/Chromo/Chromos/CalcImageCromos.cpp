// CalcImageCromos.cpp: implementation of the CCalcImageCromos class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "objbase.h"
#include "comdef.h"
#include "Cromos.h"
#include "ChromParam.h"
#include "ChildObjects.h"
#include "CalcImageCromos.h"
#include "units_const.h"
#include "math.h"
#include "resource.h"
#include "ParamsKeys.h"
#include "Woolz.h"
#include "Settings.h"
#include "Debug.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int nObj = 0;
int g_CompareData = 0;

static void InvertImage(CImageWrp &pImgWrp1)
{
	int nWidth = pImgWrp1.GetWidth();
	int nHeight = pImgWrp1.GetHeight();
	int nPanes = pImgWrp1.GetColorsCount();
	for (int i = 0; i < nHeight; i++)
	{
		for (int k = 0; k < nPanes; k++)
		{
			color *prow = pImgWrp1.GetRow(i,k);
			for (int j = 0; j < nWidth; j++)
			{
				byte by = prow[j]>>8;
				prow[j] = (0xFF-by)<<8;
			}
		}
	}
}

class _Inversion
{
public:
	BOOL m_b;
	CImageWrp m_pImgWrp;
	_Inversion(BOOL b, CImageWrp &pImgWrp)
	{
		m_b = b;
		m_pImgWrp = pImgWrp;
		if (m_b) InvertImage(m_pImgWrp);
	}
	~_Inversion()
	{
		if (m_b) InvertImage(m_pImgWrp);
	}
};

IUnknownPtr find_child_by_interface(IUnknownPtr sptrParent, const GUID guid)
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


IChromosomePtr CreateChildObjects(IUnknown *punkCalcObject)
{
	IChromosomePtr sptr;
	// Check whether the object already exists
	IUnknownPtr ptrObj = punkCalcObject;
	IUnknownPtr ptrFound = find_child_by_interface(ptrObj, IID_IChromosome);
	bool bFound = ptrFound;
	if (bFound)
	{
		sptr = ptrFound;
		return sptr;
	}
	// Create child objects and add it to chromosome
//	CChromosome *pChromos = new CChromosome;
//	IUnknown *punk1 = pChromos->GetControllingUnknown();
	IUnknown *punk1 = CreateTypedObject("ChromosomeAxisAndCentromere");
	if( !punk1 )return 0;
	sptr =  punk1;
	sptrINamedDataObject2 sptrNO1 = punk1;
	punk1->Release();
	sptrNO1->SetParent(punkCalcObject, 0);
	return sptr;
};

bool DoCalcValues(IUnknown *punkCalcObject, IChromosome * /*pChromos*/)
{
	InitSettings();
	// Initialize interfaces
	IUnknownPtr s111;
	ICalcObjectPtr sptrCO = punkCalcObject;
	IMeasureObjectPtr sptrM = punkCalcObject;
	INamedDataObject2Ptr sptrNO = punkCalcObject;
	CChromParam Param;
	// For debug purposes - use Piper's libraries 
	bool bCalcDone = false;
	if (g_CalcMethod == 6 || g_CompareData)
	{
		s111 = find_child_by_interface(punkCalcObject, IID_IWoolzObject);
		bool b = s111;
		if (b)
		{
			IWoolzObjectPtr wo((IUnknown *)s111);
			if (g_CompareData)
			{
				Param.bOk = true;
				Param.wo = wo;
			}
			IChromosomePtr sptrChromos = CreateChildObjects(punkCalcObject);
			wo->CalculateParameters(punkCalcObject, 0, NULL, sptrChromos);
			if (g_CalcMethod == 6)
				bCalcDone = true;
		}
	}
	// Calculate axis and parameters
	if (!bCalcDone)
	{
		IChromosomePtr sptrChromos = CreateChildObjects(punkCalcObject);
		if( sptrChromos == 0 )return 0;
		if (!Param.Init(sptrM,sptrChromos))
		{
			sptrINamedDataObject2 sptrNO1(sptrChromos);
			sptrNO1->SetParent(NULL, 0);
			return false;
		}
		// Perform calculation itself
		_Inversion Inv(g_BlackOnWhite, Param.pImgWrp);
		Param.CalcParams1(); // Parameters group 1: needed only ImgWrp
		Param.CalcObjAngle(); // Calculate object angle
		if (Param.pIntvl == 0 || Param.pIntvl->nrows < 5)
			return false;
		if (!Param.CalcShortAxis()) // Calculate "short" axis
		{
			if (g_BlackOnWhite) sptrChromos->SetRotatedImage(NULL);
			return false;
		}
		Param.CalcLongAxis(); // Calculate "long" axis using short
		Param.CalcProfiles(); // Calculate profiles using "long" axis
		Param.CalcParams2(); // Parameter group 2 (lenght, cvdd, etc.): needed axis and profiles
		Param.CalcCentromere(); // Calculate polarity and centromere
		Param.CalcParams3(); // Parameters group 3: needed polarity
		Param.CalcParams4(); // Parameters group 4: needed centromere location
		// Set up parameters
#if 0
		int nFeat[30];
		Param.GetParamVector(nFeat);
		for (int i = 0; i < 28; i++)
			dprintf("%d ", nFeat[i]);
		dprintf("\n");
		sptrCO->SetValue(KEY_NEW_AREA,(double)nFeat[0]);
		sptrCO->SetValue(KEY_LENGHT,(double)nFeat[1]);
		sptrCO->SetValue(KEY_DENSITY,(double)nFeat[2]);
		sptrCO->SetValue(KEY_AREACI,(double)nFeat[3]);
		sptrCO->SetValue(KEY_AREACI_DENSCI,(double)nFeat[4]);
		sptrCO->SetValue(KEY_CVDD,(double)nFeat[5]);
		sptrCO->SetValue(KEY_NSSD,(double)nFeat[6]);
		sptrCO->SetValue(KEY_MDRA,(double)nFeat[7]);
		sptrCO->SetValue(KEY_WDD1,(double)nFeat[8]);
		sptrCO->SetValue(KEY_WDD2,(double)nFeat[9]);
		sptrCO->SetValue(KEY_WDD2P,0.);
		sptrCO->SetValue(KEY_WDD3,(double)nFeat[10]);
		sptrCO->SetValue(KEY_WDD4,(double)nFeat[11]);
		sptrCO->SetValue(KEY_WDD5,(double)nFeat[12]);
		sptrCO->SetValue(KEY_WDD6,(double)nFeat[13]);
		sptrCO->SetValue(KEY_MWDD1,(double)nFeat[14]);
		sptrCO->SetValue(KEY_MWDD2,(double)nFeat[15]);
		sptrCO->SetValue(KEY_MWDD2P,0.);
		sptrCO->SetValue(KEY_MWDD3,(double)nFeat[16]);
		sptrCO->SetValue(KEY_MWDD4,(double)nFeat[17]);
		sptrCO->SetValue(KEY_MWDD5,(double)nFeat[18]);
		sptrCO->SetValue(KEY_MWDD6,(double)nFeat[19]);
		sptrCO->SetValue(KEY_GWDD1,(double)nFeat[20]);
		sptrCO->SetValue(KEY_GWDD2,(double)nFeat[21]);
		sptrCO->SetValue(KEY_GWDD2P,0.);
		sptrCO->SetValue(KEY_GWDD3,(double)nFeat[22]);
		sptrCO->SetValue(KEY_GWDD4,(double)nFeat[23]);
		sptrCO->SetValue(KEY_GWDD5,(double)nFeat[24]);
		sptrCO->SetValue(KEY_GWDD6,(double)nFeat[25]);
		sptrCO->SetValue(KEY_LENCI,(double)nFeat[26]);
		sptrCO->SetValue(KEY_NEW_PERIMETR,(double)nFeat[27]);
		sptrCO->SetValue(KEY_MASS,Param.dDensity);
		sptrCO->SetValue(KEY_POLARITY,Param.dLenghtCI>50?1.:0.);
		if (Param.bOk)
			Param.bOk = Param.wo->Compare(COMPARE_FEATURES, 28, nFeat) == S_OK;
#else
		sptrCO->SetValue(KEY_NEW_AREA,Param.dArea);
		sptrCO->SetValue(KEY_LENGHT,Param.dLenght);
		sptrCO->SetValue(KEY_DENSITY,int(Param.dDensity/Param.dArea));
		double cia=Param.dAreaCI,cid=Param.dDensCI,cil=Param.dLenghtCI,kci,dPolar;
		if (g_FloatArith==1)
		{
			if (cia>0.5)
			{
				cia = 1.-cia;
				cil = 1.-cil;
				cid = 1.-cid;
			}
			kci = cia==0?1:cid/cia;
			dPolar = Param.dLenghtCI>0.5?1.:0.;
		}
		else
		{
			if (cia>50)
			{
				cia = 100-cia;
				cil = 100-cil;
				cid = 100-cid;
			}
			if (cia < 1)
			{
				cia = 1;
				cid = 1;
			}
			kci = 100*int(cid)/int(cia);
			dPolar = Param.dLenghtCI>50?1.:0.;
		}
		sptrCO->SetValue(KEY_AREACI,cia);
		sptrCO->SetValue(KEY_AREACI_DENSCI,kci);
		sptrCO->SetValue(KEY_LENCI,cil);
		sptrCO->SetValue(KEY_CVDD,Param.cvdd);
		sptrCO->SetValue(KEY_NSSD,Param.nssd);
		sptrCO->SetValue(KEY_MDRA,Param.mdra);
		sptrCO->SetValue(KEY_WDD1,Param.wdd1);
		sptrCO->SetValue(KEY_WDD2,Param.wdd2);
		sptrCO->SetValue(KEY_WDD2P,0.);
		sptrCO->SetValue(KEY_WDD3,Param.wdd3);
		sptrCO->SetValue(KEY_WDD4,Param.wdd4);
		sptrCO->SetValue(KEY_WDD5,Param.wdd5);
		sptrCO->SetValue(KEY_WDD6,Param.wdd6);
		sptrCO->SetValue(KEY_MWDD1,Param.mwdd1);
		sptrCO->SetValue(KEY_MWDD2,Param.mwdd2);
		sptrCO->SetValue(KEY_MWDD2P,0.);
		sptrCO->SetValue(KEY_MWDD3,Param.mwdd3);
		sptrCO->SetValue(KEY_MWDD4,Param.mwdd4);
		sptrCO->SetValue(KEY_MWDD5,Param.mwdd5);
		sptrCO->SetValue(KEY_MWDD6,Param.mwdd6);
		sptrCO->SetValue(KEY_GWDD1,Param.gwdd1);
		sptrCO->SetValue(KEY_GWDD2,Param.gwdd2);
		sptrCO->SetValue(KEY_GWDD2P,0.);
		sptrCO->SetValue(KEY_GWDD3,Param.gwdd3);
		sptrCO->SetValue(KEY_GWDD4,Param.gwdd4);
		sptrCO->SetValue(KEY_GWDD5,Param.gwdd5);
		sptrCO->SetValue(KEY_GWDD6,Param.gwdd6);
		sptrCO->SetValue(KEY_NEW_PERIMETR,Param.dPerim);
		sptrCO->SetValue(KEY_MASS,Param.dDensity);
		sptrCO->SetValue(KEY_POLARITY,dPolar);
#endif
		if (g_BlackOnWhite) sptrChromos->SetRotatedImage(NULL);
	}
	return true;
}


IMPLEMENT_DYNCREATE(CCalcImageCromos, CMeasParamGroupBase)

// {CABEBB66-7E83-11d4-8336-CF3C18CD2675}
static const GUID clsidCalcImageCromos = 
{ 0xcabebb66, 0x7e83, 0x11d4, { 0x83, 0x36, 0xcf, 0x3c, 0x18, 0xcd, 0x26, 0x75 } };

// {CABEBB64-7E83-11d4-8336-CF3C18CD2675}
IMPLEMENT_GROUP_OLECREATE(CCalcImageCromos,"Chromos.CCalcImageCromos", 
0xcabebb64, 0x7e83, 0x11d4, 0x83, 0x36, 0xcf, 0x3c, 0x18, 0xcd, 0x26, 0x75);




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCalcImageCromos::CCalcImageCromos()
{
	Init();
	m_sName = "Cromos";
}

CCalcImageCromos::~CCalcImageCromos()
{

}





bool CCalcImageCromos::CalcValues(IUnknown *punkCalcObject, IUnknown *p)
{
	//dprintf("Object N %d\n", ++nObj);
	//TRACE("Object N %d\n", nObj);
	_try(CCalcImageCromos,CalcValues)
	{
		// Check for valid object
		if (!::CheckInterface(punkCalcObject, IID_ICalcObject))
		{
			dprintf("No ICalcObject interface\n");
			return false;
		}
		// punk must be CMeasureObject
		if (!::CheckInterface(punkCalcObject, IID_IMeasureObject))
		{
			dprintf("No IMeasureObject interface\n");
			return false;
		}
		// punk must be CMeasureObject
		if (!::CheckInterface(punkCalcObject, IID_INamedDataObject2))
		{
			dprintf("No INamedDataObject2 interface\n");
			return false;
		}
		if (GetValueInt(GetAppUnknown(), "\\Chromos", "Disable", 0))
			return false;

		if (!DoCalcValues(punkCalcObject, NULL))
			return false;
	}
	_catch
	{
		dprintf("Exception thrown\n");
		return false;
	}
	return true;
}


bool CCalcImageCromos::SetParamUnitName(long lParamKey, long lMeasUnit, CString & strUnitName)
{
	return false;
}

//расчет длины строки названия идет по этой спецификации столбца .. вывод в строку 1
bool CCalcImageCromos::GetParamUnitName(long lParamKey, long lMeasUnit, CString & strUnitName)
{
	strUnitName.Empty();
	strUnitName = (lMeasUnit == 0) ? ::GetValueString(GetAppUnknown(false), szCalibration, szCurrentUnitName, "pixel") : _T("Ed");
	return strUnitName.IsEmpty() ? false : true;
	/*	switch (lParamKey)
	{
	
	case KEY_NEW_AREA:
		strUnitName = (lMeasUnit == 0) ? _T("Pixel") : _T("Ed");
		break;

	case KEY_NEW_PERIMETR:
		strUnitName = (lMeasUnit == 0) ? _T("Pixel") : _T("Ed");
		break;

	case KEY_CONVEX_PERIMETR:
		strUnitName = (lMeasUnit == 0) ? _T("Pixel") : _T("Ed");
		break;

	case KEY_LENGHT:
		strUnitName = (lMeasUnit == 0) ? _T("Pixel") : _T("Ed");
		break;

	case KEY_CENTR:
		strUnitName = (lMeasUnit == 0) ? _T("Pixel") : _T("Ed");
		break;
	
	case KEY_NORMA_LENGHT:
		strUnitName = (lMeasUnit == 0) ? _T("Pixel") : _T("Ed");
		break;}
*/
	
}


bool CCalcImageCromos::LoadCreateParam()
{
	DefineParameter(KEY_NEW_AREA,	"Area",	"%0.3f");
	DefineParameter(KEY_NORMA_AREA,"Normalized area",	"%0.3f");
	DefineParameter(KEY_DENSITY,	"Optic density",	"%0.3f");
	DefineParameter(KEY_NORMA_DENSITY,	"Normalized optic density",	"%0.3f");
	DefineParameter(KEY_NEW_PERIMETR,	"Perimeter",	"%0.3f");
	DefineParameter(KEY_NORMA_PERIMETR,	"Normalized perimeter",	"%0.3f");
	DefineParameter(KEY_LENGHT,	"Lenght",	"%0.3f");
	DefineParameter(KEY_NORMA_LENGHT,"Normalized lenght","%0.3f");
	DefineParameter(KEY_NORMA_SIZE,	"Size",	"%0.3f");
//	DefineParameter(KEY_CONVEX_PERIMETR,	"Perimetr Convex Contour",	"%0.3f");
	DefineParameter(KEY_WDD1,         "WDD1", "%0.3f");
	DefineParameter(KEY_NORMA_WDD1,   "Normalized WDD1",	"%0.3f");
	DefineParameter(KEY_WDD2,         "WDD2", "%0.3f");
	DefineParameter(KEY_NORMA_WDD2,   "Normalized WDD2", "%0.3f");
	DefineParameter(KEY_WDD2P,        "WDD2P","%0.3f");
	DefineParameter(KEY_NORMA_WDD2P,  "Normalized WDD2P","%0.3f");
	DefineParameter(KEY_WDD3,         "WDD3", "%0.3f");
	DefineParameter(KEY_NORMA_WDD3,   "Normalized WDD3", "%0.3f");
	DefineParameter(KEY_WDD4,         "WDD4", "%0.3f");
	DefineParameter(KEY_NORMA_WDD4,   "Normalized WDD4", "%0.3f");
	DefineParameter(KEY_WDD5,         "WDD5", "%0.3f");
	DefineParameter(KEY_NORMA_WDD5,   "Normalized WDD5", "%0.3f");
	DefineParameter(KEY_WDD6,         "WDD6", "%0.3f");
	DefineParameter(KEY_NORMA_WDD6,   "Normalized WDD6", "%0.3f");
	DefineParameter(KEY_MWDD1,        "MWDD1", "%0.3f");
	DefineParameter(KEY_NORMA_MWDD1,  "Normalized MWDD1", "%0.3f");
	DefineParameter(KEY_MWDD2,        "MWDD2", "%0.3f");
	DefineParameter(KEY_NORMA_MWDD2,  "Normalized MWDD2", "%0.3f");
	DefineParameter(KEY_MWDD2P,       "MWDD2P", "%0.3f");
	DefineParameter(KEY_NORMA_MWDD2P, "Normalized MWDD2P", "%0.3f");
	DefineParameter(KEY_MWDD3,        "MWDD3", "%0.3f");
	DefineParameter(KEY_NORMA_MWDD3,  "Normalized MWDD3", "%0.3f");
	DefineParameter(KEY_MWDD4,        "MWDD4", "%0.3f");
	DefineParameter(KEY_NORMA_MWDD4,  "Normalized MWDD4", "%0.3f");
	DefineParameter(KEY_MWDD5,        "MWDD5", "%0.3f");
	DefineParameter(KEY_NORMA_MWDD5,  "Normalized MWDD5", "%0.3f");
	DefineParameter(KEY_MWDD6,        "MWDD6", "%0.3f");
	DefineParameter(KEY_NORMA_MWDD6,  "Normalized MWDD6", "%0.3f");
	DefineParameter(KEY_GWDD1,        "GWDD1", "%0.3f");
	DefineParameter(KEY_NORMA_GWDD1,  "Normalized GWDD1", "%0.3f");
	DefineParameter(KEY_GWDD2,        "GWDD2", "%0.3f");
	DefineParameter(KEY_NORMA_GWDD2,  "Normalized GWDD2", "%0.3f");
	DefineParameter(KEY_GWDD2P,       "GWDD2P", "%0.3f");
	DefineParameter(KEY_NORMA_GWDD2P, "Normalized GWDD2P", "%0.3f");
	DefineParameter(KEY_GWDD3,        "GWDD3", "%0.3f");
	DefineParameter(KEY_NORMA_GWDD3,  "Normalized GWDD3", "%0.3f");
	DefineParameter(KEY_GWDD4,        "GWDD4", "%0.3f");
	DefineParameter(KEY_NORMA_GWDD4,  "Normalized GWDD4", "%0.3f");
	DefineParameter(KEY_GWDD5,        "GWDD5", "%0.3f");
	DefineParameter(KEY_NORMA_GWDD5,  "Normalized GWDD5", "%0.3f");
	DefineParameter(KEY_GWDD6,        "GWDD6", "%0.3f");
	DefineParameter(KEY_NORMA_GWDD6,  "Normalized GWDD6", "%0.3f");
	DefineParameter(KEY_LENCI,	"Lenght centromere index",	"%0.3f");
	DefineParameter(KEY_AREACI, "Area centromere index",	"%0.3f");
	DefineParameter(KEY_DENSCI, "Density centromere index",	"%0.3f");
	DefineParameter(KEY_CVDD, "CVDD",	"%0.3f");
	DefineParameter(KEY_NORMA_CVDD, "Normalized CVDD",	"%0.3f");
	DefineParameter(KEY_NSSD, "NSSD",	"%0.3f");
	DefineParameter(KEY_NORMA_NSSD, "Normalized NSSD",	"%0.3f");
	DefineParameter(KEY_MDRA, "MDRA",	"%0.3f");
	DefineParameter(KEY_NORMA_MDRA, "Normalized MDRA",	"%0.3f");
	DefineParameter(KEY_AREACI_DENSCI, "AreaCI2DensCI",	"%0.3f");
	DefineParameter(KEY_NORMA_AREACI_DENSCI, "Normalized AreaCI2DensCI",	"%0.3f");
	DefineParameter(KEY_POLARITY, "Polarity",	"%0.3f");
	return true;
}

bool CCalcImageCromos::GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fMeterPerUnit )
{
	switch (lKey)
	{
	case KEY_NEW_AREA:
	{
		CString	strUnitName, str;
		str = GetValueString(GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits);
		strUnitName.Format("%s*%s", (const char*)str, (const char*)str);
		bstrType = strUnitName;
		break;
	}
	case KEY_NEW_PERIMETR:
	case KEY_LENGHT:
	{
		bstrType = GetValueString(GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits);
		break;
	}
	default:
		if (lKey >= KEY_NEW_AREA && lKey <= KEY_CHROMO_LAST || lKey >= KEY_BASE_CHROMOS && lKey < KEY_BASE_MULTIOBJ)
		{
			bstrType = GetValueString(GetAppUnknown(), _T("\\Units\\Chromos"), _T("Units"), _T("Unit"));
			break;
		}
		else
			return false;
	}
	fMeterPerUnit = 1.;
	return true;
}

void DoExecute(LPCTSTR pActionName, LPCTSTR pParams)
{
	IUnknown *punkAM = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
	sptrIActionManager	pActionMan(punkAM);
	punkAM->Release();
	CString sActionName(pActionName);
	CString sParams(pParams);
	BSTR bstrActionName = sActionName.AllocSysString();
	BSTR bstrParams = sParams.AllocSysString();
	pActionMan->ExecuteAction(bstrActionName, bstrParams, 0);
	::SysFreeString(bstrActionName);
	::SysFreeString(bstrParams);
};


bool CCalcImageCromos::OnFinalizeCalculation()
{
	if (GetValueInt(GetAppUnknown(), "Chromos", "AutoNormalize", 1))
		DoExecute(_T("NormalizeParameters"), NULL);
	DeinitSettings();
	INamedDataObject2Ptr NDO(m_ptrContainer);
	IUnknown *punk = NULL;
	HRESULT hr = NDO->GetData(&punk);
	if (SUCCEEDED(hr) && punk)
	{
		long l = cncReset;
		::FireEvent(punk, szEventChangeObjectList, m_ptrContainer, NULL, &l, 0);
		punk->Release();
	}
#if defined(_DEBUG)
	nObj = 0;
#endif
	return CMeasParamGroupBase::OnFinalizeCalculation();
}


/////////////////////////////////////////////////////////////////////////////
//IPersist
void CCalcImageCromos::GetClassID( CLSID* pClassID )
{
	memcpy( pClassID, &guid, sizeof(CLSID) );
}





