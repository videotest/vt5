#include "stdafx.h"
#include "thactions.h"
#include "resource.h"
#include "binary.h"
#include "\vt5\common2\misc_templ.h"
#include "..\common2\class_utils.h"
#include "meas_utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionMeasureBin, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMeasurePhases, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMeasurePhasesLight, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionThresholdingGrayBin, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionExtractPhase, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAddRemoveColorInterval, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAddRemoveColor, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionThresholdingGray, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionThresholdingGrayBinAuto, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionThresGrayMute, CCmdTargetEx);
// olecreate 

// {40C38235-129D-4763-B65B-00E950495903}
GUARD_IMPLEMENT_OLECREATE(CActionMeasureBin, "FiltersMain.MeasureBin",
0x40c38235, 0x129d, 0x4763, 0xb6, 0x5b, 0x0, 0xe9, 0x50, 0x49, 0x59, 0x3);
// {D0683E09-7D9C-4a07-A8C7-007B1C2EEA9B}
GUARD_IMPLEMENT_OLECREATE(CActionMeasurePhases, "FiltersMain.MeasurePhases", 
0xd0683e09, 0x7d9c, 0x4a07, 0xa8, 0xc7, 0x0, 0x7b, 0x1c, 0x2e, 0xea, 0x9b);
// {1B78521B-7703-42fa-9386-867F90DDFFCD}
GUARD_IMPLEMENT_OLECREATE(CActionMeasurePhasesLight, "FiltersMain.MeasurePhasesLight", 
0x1b78521b, 0x7703, 0x42fa, 0x93, 0x86, 0x86, 0x7f, 0x90, 0xdd, 0xff, 0xcd);
// {18BD2141-646D-4fa1-BBA0-6BF9C10C5D09}
GUARD_IMPLEMENT_OLECREATE(CActionThresholdingGrayBin, "FiltersMain.ThresholdingGrayBin",
0x18bd2141, 0x646d, 0x4fa1, 0xbb, 0xa0, 0x6b, 0xf9, 0xc1, 0xc, 0x5d, 0x9);
// {BEE5653D-91EB-4a13-B63A-E075DAAFC1C0}
GUARD_IMPLEMENT_OLECREATE(CActionThresholdingGrayBinAuto, "FiltersMain.ThresholdingGrayBinAuto",
0xbee5653d, 0x91eb, 0x4a13, 0xb6, 0x3a, 0xe0, 0x75, 0xda, 0xaf, 0xc1, 0xc0);
// {F76CAD23-03F6-11d4-A0C7-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionExtractPhase, "filtersmain.ExtractPhase",
0xf76cad23, 0x3f6, 0x11d4, 0xa0, 0xc7, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {2A594473-FFD2-11d3-A0C6-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionAddRemoveColorInterval, "filtersmain.AddRemoveColorInterval",
0x2a594473, 0xffd2, 0x11d3, 0xa0, 0xc6, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {897E3CD3-FE70-11d3-A0C5-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionAddRemoveColor, "filtersmain.AddColor",
0x897e3cd3, 0xfe70, 0x11d3, 0xa0, 0xc5, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {C111C380-2686-4589-A603-78A43EDFE579}
GUARD_IMPLEMENT_OLECREATE(CActionThresholdingGray, "FiltersMain.ThresholdingGray",
0xc111c380, 0x2686, 0x4589, 0xa6, 0x3, 0x78, 0xa4, 0x3e, 0xdf, 0xe5, 0x79);
// {6291A3B8-46C9-45f3-83D5-F19D336471AA}
GUARD_IMPLEMENT_OLECREATE(CActionThresGrayMute, "FiltersMain.ThresholdingGrayMute", 
0x6291a3b8, 0x46c9, 0x45f3,  0x83, 0xd5, 0xf1, 0x9d, 0x33, 0x64, 0x71, 0xaa );


// guidinfo 

// {D628F3C3-DF50-461d-AE07-D8F1D6452F06}
static const GUID guidMeasureBin =
{ 0xd628f3c3, 0xdf50, 0x461d, { 0xae, 0x7, 0xd8, 0xf1, 0xd6, 0x45, 0x2f, 0x6 } };
// {B35108F3-2B05-4b79-BB27-71354CD84D0B}
static const GUID guidMeasurePhases = 
{ 0xb35108f3, 0x2b05, 0x4b79, { 0xbb, 0x27, 0x71, 0x35, 0x4c, 0xd8, 0x4d, 0xb } };
// {EF1ADDCE-F04D-4bac-8BB7-ED560DC09243}
static const GUID guidMeasurePhasesLight = 
{ 0xef1addce, 0xf04d, 0x4bac, { 0x8b, 0xb7, 0xed, 0x56, 0xd, 0xc0, 0x92, 0x43 } };
// {450A9A3F-9388-454d-99B0-22D8C5F9B05A}
static const GUID guidThresholdingGrayBin =
{ 0x450a9a3f, 0x9388, 0x454d, { 0x99, 0xb0, 0x22, 0xd8, 0xc5, 0xf9, 0xb0, 0x5a } };
// {063364B4-6A79-4012-B56E-711F41B52A00}
static const GUID guidThresholdingGrayBinAuto = 
{ 0x63364b4, 0x6a79, 0x4012, { 0xb5, 0x6e, 0x71, 0x1f, 0x41, 0xb5, 0x2a, 0x0 } };

// {F76CAD21-03F6-11d4-A0C7-0000B493A187}
static const GUID guidExtractPhase =
{ 0xf76cad21, 0x3f6, 0x11d4, { 0xa0, 0xc7, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {2A594471-FFD2-11d3-A0C6-0000B493A187}
static const GUID guidAddRemoveColorInterval =
{ 0x2a594471, 0xffd2, 0x11d3, { 0xa0, 0xc6, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {897E3CD1-FE70-11d3-A0C5-0000B493A187}
static const GUID guidAddRemoveColor =
{ 0x897e3cd1, 0xfe70, 0x11d3, { 0xa0, 0xc5, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {054FE8E6-3079-4129-92AA-56913C0D31A0}
static const GUID guidThresholdingGray =
{ 0x54fe8e6, 0x3079, 0x4129, { 0x92, 0xaa, 0x56, 0x91, 0x3c, 0xd, 0x31, 0xa0 } };
// {3B9BDDAC-7192-4cae-B820-341D6C016824}
static const GUID  guidThresholdingGrayMute= 
{ 0x3b9bddac, 0x7192, 0x4cae, { 0xb8, 0x20, 0x34, 0x1d, 0x6c, 0x1, 0x68, 0x24 } };



//[ag]6. action info

ACTION_INFO_FULL(CActionMeasureBin, IDS_ACTION_MEASURE_BIN, IDS_MENU_FILTERS, IDS_TB_MAIN, guidMeasureBin);
ACTION_INFO_FULL(CActionMeasurePhases, IDS_ACTION_MEASURE_PHASES, IDS_MENU_FILTERS, IDS_TB_MAIN, guidMeasurePhases);
ACTION_INFO_FULL(CActionMeasurePhasesLight, IDS_ACTION_MEASURE_PHASES_LIGHT, IDS_MENU_FILTERS, IDS_TB_MAIN, guidMeasurePhasesLight);
ACTION_INFO_FULL(CActionThresholdingGrayBin, IDS_ACTION_GRAY_THRESHOLDING_BIN, IDS_MENU_FILTERS, IDS_TB_MAIN, guidThresholdingGrayBin);
ACTION_INFO_FULL(CActionExtractPhase, IDS_ACTION_EXTRACT_PHASE, IDS_MENU_FILTERS, IDS_TB_MAIN, guidExtractPhase);
ACTION_INFO_FULL(CActionAddRemoveColorInterval, IDS_ACTION_ADD_COLORINTERVAL, IDS_MENU_FILTERS, IDS_TB_MAIN, guidAddRemoveColorInterval);
ACTION_INFO_FULL(CActionAddRemoveColor, IDS_ACTION_ADD_COLOR, IDS_MENU_FILTERS, IDS_TB_MAIN, guidAddRemoveColor);
ACTION_INFO_FULL(CActionThresholdingGray, IDS_ACTION_GRAY_THRESHOLDING, IDS_MENU_FILTERS, IDS_TB_MAIN, guidThresholdingGray);
ACTION_INFO_FULL(CActionThresholdingGrayBinAuto, IDS_ACTION_GRAY_THRESHOLDING_BIN_AUTO, IDS_MENU_FILTERS, IDS_TB_MAIN, guidThresholdingGrayBinAuto);
ACTION_INFO_FULL(CActionThresGrayMute, IDS_ACTION_GRAY_THRESHOLDING_MUTE, IDS_MENU_FILTERS, IDS_TB_MAIN, guidThresholdingGrayMute);
//[ag]7. targets

ACTION_TARGET(CActionMeasureBin, szTargetAnydoc);
ACTION_TARGET(CActionMeasurePhases, szTargetAnydoc);
ACTION_TARGET(CActionMeasurePhasesLight, szTargetAnydoc);
ACTION_TARGET(CActionThresholdingGrayBin, szTargetAnydoc);
ACTION_TARGET(CActionExtractPhase, szTargetAnydoc);
ACTION_TARGET(CActionAddRemoveColorInterval, szTargetViewSite);
ACTION_TARGET(CActionAddRemoveColor, szTargetViewSite);
ACTION_TARGET(CActionThresholdingGray, szTargetAnydoc);
ACTION_TARGET(CActionThresholdingGrayBinAuto, szTargetAnydoc);
ACTION_TARGET(CActionThresGrayMute, szTargetAnydoc);
//[ag]8. arguments

ACTION_ARG_LIST(CActionExtractPhase)
	ARG_INT(_T("Class"), -1)
	ARG_IMAGE( _T("Image") )
	RES_OBJECT( _T("ObjectList") )		//it isn't image, but image in preview mode
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionAddRemoveColor)
	ARG_INT(_T("X"), -1)
	ARG_INT(_T("Y"), -1)
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionThresholdingGray)
	ARG_INT(_T("Low"), 96 )
	ARG_INT(_T("High"), 192 )
	ARG_IMAGE( _T("Image") )
	RES_OBJECT( _T("ObjectList") )		//it isn't image, but image in preview mode
	ARG_INT(_T("PaneNum"), -1 )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionThresholdingGrayBin)
	ARG_INT(_T("Low"), 96 )
	ARG_INT(_T("High"), 192 )
	ARG_IMAGE( _T("Image") )
	RES_BINIMAGE( _T("ThresholdingGray") )
	ARG_INT(_T("PaneNum"), -1 )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionThresholdingGrayBinAuto)
	ARG_BOOL(_T("LightOnDark"), 0 )
	ARG_INT(_T("PaneNum"), -1 )
	ARG_IMAGE( _T("Image") )
	RES_BINIMAGE( _T("ThresholdingGrayAuto") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionThresGrayMute)
	ARG_BOOL(_T("LightOnDark"), 0 )
	ARG_INT(_T("PaneNum"), -1 )
	ARG_IMAGE( _T("Image") )
END_ACTION_ARG_LIST();

char* szMeasureBin = "MeasureBin";
ACTION_ARG_LIST(CActionMeasureBin)
	ARG_IMAGE( _T("BaseImage") )
	ARG_BINIMAGE( _T("BinImage") )
	ARG_INT(_T("Calc"), 1 )
	RES_OBJECT( _T(szMeasureBin) )
	ARG_INT(_T("Phase"), -1 )
	ARG_INT(_T("CopyPhases"), 0 )  // переносить ли фазы из Binary в маски объектов
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionMeasurePhases)
	ARG_IMAGE( _T("BaseImage") )
	ARG_BINIMAGE( _T("BinImage") )
	ARG_INT(_T("Calc"), 1 )
	RES_OBJECT( _T(szMeasureBin) )
	ARG_INT(_T("Phase"), -1 )
	ARG_INT(_T("CopyPhases"), 0 )
	ARG_STRING(_T("ClassifierName"), _T("") )
END_ACTION_ARG_LIST();


ACTION_ARG_LIST(CActionMeasurePhasesLight)
	ARG_IMAGE( _T("BaseImage") )
	ARG_BINIMAGE( _T("BinImage") )
	ARG_INT(_T("Calc"), 1 )
	RES_OBJECT( _T(szMeasureBin) )
	ARG_INT(_T("Phase"), -1 )
	ARG_INT(_T("CopyPhases"), 1 )
	ARG_STRING(_T("ClassifierName"), _T("") )
END_ACTION_ARG_LIST();

//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionMeasureBin implementation
CActionMeasureBin::CActionMeasureBin()
{
}

CActionMeasureBin::~CActionMeasureBin()
{
}

static bool GetSelRect(IUnknown *punkImage, IUnknown *punkBase, CRect rcBinOnImage, CRect &rcMeasOnBin, CImageWrp &frmimage)
{
	rcMeasOnBin = CRect(CPoint(0,0), rcBinOnImage.Size());;
	IUnknownPtr punkParent;
	INamedDataObject2Ptr ptrN(punkImage);
	ptrN->GetParent(&punkParent);
	if (punkParent == 0)
		return false; // No selection
/*	if (GetObjectKey(punkImage) == GetObjectKey(punkBase))
	{
		frmimage = punkImage;
		return true; // Binary created on frame, measurement on whole binary image and use it as frame
	}*/
	CImageWrp base(punkImage);
	CRect rcFrmOnImage(base.GetRect());
	rcMeasOnBin.left = max(rcFrmOnImage.left, rcBinOnImage.left)-rcBinOnImage.left;
	rcMeasOnBin.top = max(rcFrmOnImage.top, rcBinOnImage.top)-rcBinOnImage.top;
	rcMeasOnBin.right = min(rcFrmOnImage.right, rcBinOnImage.right)-rcBinOnImage.left;
	rcMeasOnBin.bottom = min(rcFrmOnImage.bottom, rcBinOnImage.bottom)-rcBinOnImage.top;
	frmimage = punkImage;
	return true;
}

bool CActionMeasureBin::InvokeFilter()
{
	CImageWrp defimage(GetDataArgument());
	CImageWrp baseimage(defimage);
	IUnknownPtr punkParent;
	INamedDataObject2Ptr ptrN(defimage);
	ptrN->GetParent(&punkParent);
	if (punkParent != 0)
		baseimage = punkParent;
	CBinImageWrp binary(GetDataArgument("BinImage"));
	/*INamedDataObject2Ptr sptrNDO2Img(binary);
	GuidKey guid;
	sptrNDO2Img->GetBaseKey(&guid);
	IUnknownPtr punkBase(GetObjectByKey(m_punkTarget, guid), false);
	if (punkBase == 0 || !CheckInterface(punkBase, IID_IImage4))
		return false;
	CImageWrp baseimage(punkBase);*/
	CObjectListWrp objects(GetDataResult());
	bool bCalc = GetArgumentInt("Calc") == 1;
	bool bUseLight = ::GetValueInt(GetAppUnknown(), "\\measurement", "UseLightObjects", 1) == 1;
	int nPhase = GetArgumentInt("Phase");
	bool bCopyPhases = GetArgumentInt("CopyPhases")!=0;

	/* переименование листа, если light
	if(bUseLight)
	{
		INamedObject2Ptr sptrNO(objects);
		if(sptrNO != 0)
		{
			BSTR bstr = 0;
			sptrNO->GetName(&bstr);
			CString strName(bstr);
			::SysFreeString(bstr);
			if(strName.Left(strlen(szMeasureBin)) == szMeasureBin)
			{
				strName = strName.Right(strName.GetLength() - strlen(szMeasureBin));
				strName.Insert(0, CString(szMeasureBin)+" light");
			}
			else
			{
				strName += " light";
			}

			sptrNO->SetName(_bstr_t(strName));
		}
	}
	*/

	IDataObjectListPtr sptrList = objects;

	long cx = binary.GetWidth();
	long cy = binary.GetHeight();
	CPoint ptOffset(0,0);
	binary->GetOffset(&ptOffset);
	CRect rcBound(ptOffset, CSize(cx, cy));

//	CPoint ptOffsetImg = baseimage.GetOffset();
	CRect rcMeas;
	CImageWrp frmimage;
	bool bUseFrame = GetSelRect(GetDataArgument(), baseimage, rcBound, rcMeas, frmimage);

	CWalkFillInfo* pwfi = new CWalkFillInfo(rcBound);


	int nRowSize = 0;
	
	byte* pbin = 0;

	StartNotification(cy);

	if (nPhase >= 0)
	{
		byte byPhase = 0x80+nPhase;
		for(long y = rcMeas.top; y < rcMeas.bottom; y++ )
		{
			binary->GetRow(&pbin, y, FALSE);
			byte *pMask = bUseFrame?frmimage.GetRowMask(y-rcMeas.top):NULL;
			byte* pmaskdest = pwfi->GetRowMask(y)+rcMeas.left;
			for(long x = rcMeas.left; x < rcMeas.right; x++, pmaskdest++, pMask++)
				*pmaskdest = pbin[x]==byPhase&&(!bUseFrame||*pMask>0)?255:0;
			Notify( y );
		}
	}
	else
	{
		for(long y = rcMeas.top; y < rcMeas.bottom; y++ )
		{
			binary->GetRow(&pbin, y, FALSE);
			byte *pMask = bUseFrame?frmimage.GetRowMask(y-rcMeas.top):NULL;
			byte* pmaskdest = pwfi->GetRowMask(y)+rcMeas.left;
			for(long x = rcMeas.left; x < rcMeas.right; x++, pmaskdest++,pMask++)
			{
				if(pbin[x]!= 0&&(!bUseFrame||*pMask>0))
					*pmaskdest = 255;
				else
					*pmaskdest = 0;
			}
			Notify( y );
		}
	}

	IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IMeasureManager, szMeasurement);
	ICompManagerPtr	ptrManager( punk );
	if( punk )punk->Release();

	int	nGroup, nGroupCount;
	ptrManager->GetCount( &nGroupCount );

	if( bCalc )
	{
		if( ptrManager == 0 )
			return false;

		for( nGroup = 0; nGroup < nGroupCount; nGroup++ )
		{
			IUnknownPtr	ptrG = 0;
			ptrManager->GetComponentUnknownByIdx( nGroup, &ptrG );
			IMeasParamGroupPtr	ptrGroup( ptrG );
			if( ptrGroup == 0 )continue;
			ptrGroup->InitializeCalculation( objects );
		}
	}
	bool bSetParams = true;
	CListLockUpdate lock(objects);
	CPoint	pointStart = CPoint( -1, -1 );
	IUnknown* punkImage = 0;
	while( pwfi->InitContours(true, pointStart, &pointStart, NORECT, baseimage, &punkImage) )
	{
		CImageWrp	image(punkImage, false);

		if(bCopyPhases)
		{
			int cx2=image.GetWidth();
			int cy2=image.GetHeight();
			CPoint ptOffset2=image.GetOffset();
			CRect rcBound2(ptOffset2, CSize(cx2, cy2));
			rcBound2 &= rcBound;
			for(int y=rcBound2.top; y<rcBound2.bottom; y++)
			{
				byte * psrc;
				binary->GetRow(&psrc, y-ptOffset.y, false);
				psrc -= ptOffset.x;
				byte * pdst = image.GetRowMask(y-ptOffset2.y)-ptOffset2.x;
				for(int x=rcBound2.left; x<rcBound2.right; x++)
				{
					byte b=psrc[x];
					if(pdst[x]>=128) pdst[x] = (b>=128) ? b : 128+b;
				}
			}
		}

		for( int nCtr = 0; nCtr < pwfi->GetContoursCount(); nCtr++ )
		{
			Contour	*pc = pwfi->GetContour( nCtr );
			image->AddContour( pc );
		}

		pwfi->DetachContours();

		ICalcObjectPtr sptrCalc;

		if(bUseLight)
		{
			IUnknown *punkObj = ::CreateTypedObject(szTypeLightObject);
			sptrCalc = punkObj;
			if(punkObj)punkObj->Release();
		}
		else
		{
			CObjectWrp	object(::CreateTypedObject(szTypeObject), false);
			object.SetImage(image);
			sptrCalc = object;
		}
		
		INamedDataObject2Ptr ptrObject(sptrCalc);
		if(ptrObject != 0)ptrObject->SetParent(objects, 0);


		if(bCalc)
		{
			::CalcObject( sptrCalc, image, false, ptrManager );
			sptrList->UpdateObject(sptrCalc);
		}
	}

	if( bCalc )
	{
		for( nGroup = 0; nGroup < nGroupCount; nGroup++ )
		{
			IUnknownPtr	ptrG = 0;
			ptrManager->GetComponentUnknownByIdx( nGroup, &ptrG );
			IMeasParamGroupPtr	ptrGroup( ptrG );
			if( ptrGroup == 0 )continue;
			ptrGroup->FinalizeCalculation();
		}
	}

	delete pwfi;

	FinishNotification();
	
	return true;
}

/*class CMeasureContext
{
	bool m_bCalc;
	ICompManagerPtr	m_ptrManager;
	int m_nGroupCount;
public:
	CMeasureContext(bool bCalc, IUnknown *punkObjLst)
	{
		Init(bCalc,punkObjLst);
	}
	void Init(bool bCalc, IUnknown *punkObjLst)
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
	void Calc(IUnknown *punkCalcObj, IUnknown *punkImage)
	{
		if (m_bCalc)
			::CalcObject(punkCalcObj, punkImage, false, m_ptrManager);
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
		}
	}
};*/

//////////////////////////////////////////////////////////////////////
//CActionMeasurePhases implementation
CActionMeasurePhases::CActionMeasurePhases()
{
}

CActionMeasurePhases::~CActionMeasurePhases()
{
}

bool CActionMeasurePhases::InvokeFilter()
{
	CImageWrp defimage(GetDataArgument());
	CImageWrp baseimage(defimage);
	IUnknownPtr punkParent;
	INamedDataObject2Ptr ptrN(defimage);
	ptrN->GetParent(&punkParent);
	if (punkParent != 0)
		baseimage = punkParent;
	CBinImageWrp binary(GetDataArgument("BinImage"));
	/*INamedDataObject2Ptr sptrNDO2Img(binary);
	GuidKey guid;
	sptrNDO2Img->GetBaseKey(&guid);
	IUnknownPtr punkBase(GetObjectByKey(m_punkTarget, guid), false);
	if (punkBase == 0 || !CheckInterface(punkBase, IID_IImage4))
		return false;
	CImageWrp baseimage(punkBase);*/
	CObjectListWrp objects(GetDataResult());
	bool bCalc = GetArgumentInt("Calc") == 1;
	bool bUseLight = ::GetValueInt(GetAppUnknown(), "\\measurement", "UseLightObjects", 1) == 1;
	int nUsedPhase = GetArgumentInt("Phase");
	bool bCopyPhases = GetArgumentInt("CopyPhases")!=0;
	int  nMinSize = ::GetValueInt(GetAppUnknown(), "\\measurement", "MinObjectSize", 10);
	CString sClassifierName = GetArgumentString("ClassifierName");
	long cx = binary.GetWidth();
	long cy = binary.GetHeight();
	CPoint ptOffset(0,0);
	binary->GetOffset(&ptOffset);
	CRect rcBound(ptOffset, CSize(cx, cy));
	CRect rcMeas;
	CImageWrp frmimage;
	bool bUseFrame = GetSelRect(GetDataArgument(), baseimage, rcBound, rcMeas, frmimage);
	// Prepare object list name
	IDataObjectListPtr sptrList = objects;
	// Initialize calculation (if needed).
	IUnknownPtr punk(FindComponentByName(GetAppUnknown(false), IID_IMeasureManager, szMeasurement), false);
	ICompManagerPtr	ptrManager( punk );
	int	nGroup, nGroupCount;
	ptrManager->GetCount( &nGroupCount );
	if( bCalc )
	{
		if ( ptrManager == 0)
			return false;
		for( nGroup = 0; nGroup < nGroupCount; nGroup++ )
		{
			IUnknownPtr	ptrG = 0;
			ptrManager->GetComponentUnknownByIdx( nGroup, &ptrG );
			IMeasParamGroupPtr	ptrGroup( ptrG );
			if( ptrGroup == 0 )continue;
			ptrGroup->InitializeCalculation( objects );
		}
	}
	// Process phases
	CWalkFillInfo* pwfi = new CWalkFillInfo(rcBound);
	int nPhases = ::GetValueInt(::GetAppUnknown(), "Phase", "PhasesCount");
	StartNotification(nPhases*cy);
	for (int nPhase = 0; nPhase < nPhases; nPhase ++)
	{
		int nRowSize = 0;
		byte* pbin = 0;
		byte byPhase = 0x80+nPhase;
		for(long y = rcMeas.top; y < rcMeas.bottom; y++ )
		{
			binary->GetRow(&pbin, y, FALSE);
			byte *pMask = bUseFrame?frmimage.GetRowMask(y-rcMeas.top):NULL;
			byte* pmaskdest = pwfi->GetRowMask(y)+rcMeas.left;
			for(long x = rcMeas.left; x < rcMeas.right; x++, pmaskdest++, pMask++)
				*pmaskdest = pbin[x]==byPhase&&(!bUseFrame||*pMask>0)?255:0;
		}
		bool bSetParams = true;
		CListLockUpdate lock(objects);
		CPoint	pointStart = CPoint( -1, -1 );
		IUnknownPtr punkImage;
		while( pwfi->InitContours(true, pointStart, &pointStart, NORECT, baseimage, &punkImage) )
		{
			Notify(nPhase*cy+max(pointStart.y-rcBound.top,0));
			CImageWrp	image(punkImage);
			int cx2=image.GetWidth();
			int cy2=image.GetHeight();
			if (cx2 < nMinSize && cy2 < nMinSize)
				continue;
			if(bCopyPhases)
			{
				CPoint ptOffset2=image.GetOffset();
				CRect rcBound2(ptOffset2, CSize(cx2, cy2));
				rcBound2 &= rcBound;
				for(int y=rcBound2.top; y<rcBound2.bottom; y++)
				{
					byte * psrc;
					binary->GetRow(&psrc, y-ptOffset.y, false);
					psrc -= ptOffset.x;
					byte *pdst = image.GetRowMask(y-ptOffset2.y)-ptOffset2.x;
					for(int x=rcBound2.left; x<rcBound2.right; x++)
					{
						byte b=psrc[x];
						if(pdst[x]>=128) pdst[x] = (b>=128) ? b : 128+b;
					}
				}
			}
			for( int nCtr = 0; nCtr < pwfi->GetContoursCount(); nCtr++ )
			{
				Contour	*pc = pwfi->GetContour( nCtr );
				image->AddContour( pc );
			}
			pwfi->DetachContours();
			ICalcObjectPtr sptrCalc;
			if(bUseLight)
			{
				IUnknownPtr punkObj(::CreateTypedObject(szTypeLightObject), false);
				sptrCalc = punkObj;
			}
			else
			{
				CObjectWrp	object(::CreateTypedObject(szTypeObject), false);
				object.SetImage(image);
				sptrCalc = object;
			}
			INamedDataObject2Ptr ptrObject(sptrCalc);
			if(ptrObject != 0)ptrObject->SetParent(objects, 0);
			if(bCalc)
			{
				::CalcObject( sptrCalc, image, false, ptrManager );
				sptrList->UpdateObject(sptrCalc);
			}
			set_object_class(sptrCalc, nPhase, sClassifierName.GetBuffer());
			sClassifierName.ReleaseBuffer();
		}
	}
	delete pwfi;
	if( bCalc )
	{
		for( nGroup = 0; nGroup < nGroupCount; nGroup++ )
		{
			IUnknownPtr	ptrG = 0;
			ptrManager->GetComponentUnknownByIdx( nGroup, &ptrG );
			IMeasParamGroupPtr	ptrGroup( ptrG );
			if( ptrGroup == 0 )continue;
			ptrGroup->FinalizeCalculation();
		}
	}
	FinishNotification();
	
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionMeasurePhasesLight implementation
CActionMeasurePhasesLight::CActionMeasurePhasesLight()
{
}

CActionMeasurePhasesLight::~CActionMeasurePhasesLight()
{
}

#include "\vt5\awin\trace.h"
bool CActionMeasurePhasesLight::InvokeFilter()
{
	// Prepare initial data
	CImageWrp defimage(GetDataArgument());
	CImageWrp baseimage(defimage);
	IUnknownPtr punkParent;
	INamedDataObject2Ptr ptrN(defimage);
	ptrN->GetParent(&punkParent);
	if (punkParent != 0)
		baseimage = punkParent;
	CBinImageWrp binary(GetDataArgument("BinImage"));
	CObjectListWrp objects(GetDataResult());
	int nCalc = GetArgumentInt("Calc");
	bool bUseLight = ::GetValueInt(GetAppUnknown(), "\\measurement", "UseLightObjects", 1) == 1;
	int nUsedPhase = GetArgumentInt("Phase");
	bool bCopyPhases = GetArgumentInt("CopyPhases")!=0;
	int  nMinSize = ::GetValueInt(GetAppUnknown(), "\\measurement", "MinObjectSize", 10);
	CString sClassifierName = GetArgumentString("ClassifierName");
	long cx = binary.GetWidth();
	long cy = binary.GetHeight();
	CPoint ptOffset(0,0);
	binary->GetOffset(&ptOffset);
	CRect rcBound(ptOffset, CSize(cx, cy));
	CRect rcMeas;
	CImageWrp frmimage;
	bool bUseFrame = GetSelRect(GetDataArgument(), baseimage, rcBound, rcMeas, frmimage);
	IDataObjectListPtr sptrList = objects;
	CMeasureContext MeasCtx(nCalc>0, objects);
	ICalibrPtr	ptrCalibr = baseimage;
	double fCalibr2;
	if (ptrCalibr != 0 && nCalc == 2)
	{
		ptrCalibr->GetCalibration(&fCalibr2, 0);
		fCalibr2 = fCalibr2*fCalibr2;
	}
	// Process phases
	BOOL bContours = ::GetValueInt(::GetAppUnknown(), "\\MeasurePhasesLight", "MakeContours", FALSE);
	int nPhases = ::GetValueInt(::GetAppUnknown(), "Phase", "PhasesCount");
	StartNotification(nPhases*cy);
	for (int nPhase = 0; nPhase < nPhases; nPhase ++)
	{
		CRect rect(rcBound.left+rcMeas.left,rcBound.top+rcMeas.top,rcBound.left+rcMeas.right,
			rcBound.top+rcMeas.bottom);
		// Make image for object by phase
		CImageWrp imgObj(baseimage.CreateVImage(rect), FALSE);
		imgObj.InitRowMasks();
		byte* pbin = 0;
		byte byPhase = 0x80+nPhase;
		int nPoints = 0;
		for(long y = rcMeas.top; y < rcMeas.bottom; y++ )
		{
			binary->GetRow(&pbin, y, FALSE);
			byte *pMask = bUseFrame?frmimage.GetRowMask(y-rcMeas.top):NULL;
			byte* pmaskdest = imgObj.GetRowMask(y-rcMeas.top);
			for(long x = rcMeas.left; x < rcMeas.right; x++)
			{
				if (pbin[x] == byPhase &&(!bUseFrame||pMask[x-rcMeas.left]>0))
				{
					pmaskdest[x-rcMeas.left] = pbin[x];
					nPoints++;
				}
				else
					pmaskdest[x-rcMeas.left] = 0;
			}
		}
		if (nPoints == 0)
			continue;
		CPoint ptOffset(rcMeas.left+rcBound.left, rcMeas.top+rcBound.top);
		imgObj.SetOffset(ptOffset);
		if (bContours)
		{
			CWalkFillInfo* pwfi = new CWalkFillInfo(rcBound);
			byte byPhase = 0x80+nPhase;
			for(long y = rcMeas.top; y < rcMeas.bottom; y++ )
			{
				binary->GetRow(&pbin, y, FALSE);
				byte *pMask = bUseFrame?frmimage.GetRowMask(y-rcMeas.top):NULL;
				byte* pmaskdest = pwfi->GetRowMask(y)+rcMeas.left;
				for(long x = rcMeas.left; x < rcMeas.right; x++, pmaskdest++, pMask++)
					*pmaskdest = pbin[x]==byPhase&&(!bUseFrame||*pMask>0)?255:0;
			}
			CPoint	pointStart = CPoint( -1, -1 );
			IUnknownPtr punkImage;
			while( pwfi->InitContours(true, pointStart, &pointStart, NORECT, baseimage, &punkImage) )
			{
				CImageWrp	image(punkImage);
				int cx2=image.GetWidth();
				int cy2=image.GetHeight();
				for( int nCtr = 0; nCtr < pwfi->GetContoursCount(); nCtr++ )
				{
					Contour	*pc = pwfi->GetContour( nCtr );
					ContourCompact(pc);
					imgObj->AddContour( pc );
				}
				CPoint ptOffset2 = image.GetOffset();
				CRect rectO(max(rect.left,ptOffset2.x),max(rect.top,ptOffset2.y),
					min(rect.right,ptOffset2.x+cx2),min(rect.bottom,ptOffset2.y+cy2));
				for (int y = rectO.top; y < rectO.bottom; y++)
				{
					byte *pRowMaskSrc = image.GetRowMask(y-ptOffset2.y);
					byte *pRowMaskDst = imgObj.GetRowMask(y-rect.top);
					for (int x = rectO.left; x < rectO.right; x++)
					{
						if (pRowMaskSrc[x-ptOffset2.x] == 1 && pRowMaskDst[x-rect.left] == 0)
							pRowMaskDst[x-rect.left] = 1;

					}
				}
				pwfi->DetachContours();
			}
			delete pwfi;
		}
		CListLockUpdate lock(objects);
		ICalcObjectPtr sptrCalc;
		if(bUseLight)
		{
			IUnknownPtr punkObj(::CreateTypedObject(szTypeLightObject), false);
			sptrCalc = punkObj;
		}
		else
		{
			CObjectWrp	object(::CreateTypedObject(szTypeObject), false);
			object.SetImage(imgObj);
			sptrCalc = object;
		}
		INamedDataObject2Ptr ptrObject(sptrCalc);
		if(ptrObject != 0)ptrObject->SetParent(objects, 0);
		if (nCalc == 1)
			MeasCtx.Calc(sptrCalc,imgObj);
		else if (nCalc == 2)
			sptrCalc->SetValue(KEY_AREA, (double)nPoints*fCalibr2);
		set_object_class(sptrCalc, nPhase, sClassifierName.GetBuffer());
		sClassifierName.ReleaseBuffer();
		Notify(nPhase*cy);
	}
	MeasCtx.DeInit();
	FinishNotification();
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionThresholdingGrayBin implementation

static bool MakeBinary(CImageWrp &image, CBinImageWrp &binimage, CPoint &ptOffset)
{
	INamedDataObject2Ptr sptrNDO2Image(image);
	IUnknownPtr punkParent;
	sptrNDO2Image->GetParent(&punkParent);
	CImageWrp imageParent(punkParent);
	if (imageParent == 0)
	{
		long	cx = image.GetWidth(), cy = image.GetHeight();
		if( cy == 0 || cx == 0 )
			return false;
		if( !binimage.CreateNew(cx, cy) )
			return false;
		ptOffset = CPoint(0,0);
	}
	else
	{
		long	cx = imageParent.GetWidth(), cy = imageParent.GetHeight();
		if( cy == 0 || cx == 0 )
			return false;
		if( !binimage.CreateNew(cx, cy) )
			return false;
		ptOffset = image.GetOffset();
	}
	return true;
}


CActionThresholdingGrayBin::CActionThresholdingGrayBin()
{
}

CActionThresholdingGrayBin::~CActionThresholdingGrayBin()
{
}

bool CActionThresholdingGrayBin::InvokeFilter()
{
	int	nLow = GetArgumentInt( "Low" );
	int	nHigh = GetArgumentInt( "High" );
	int	nPane = GetArgumentInt( "PaneNum" );

	CImageWrp	image( GetDataArgument() );
	CBinImageWrp	binimage( GetDataResult() );

	long	cx = image.GetWidth(), cy = image.GetHeight();
	
	if( cy == 0 )
		return false;

	IUnknown	*punkCC = 0;
	image->GetColorConvertor( &punkCC );
	if( !punkCC )
		return false;
	IColorConvertorPtr	ptrC( punkCC );
	punkCC->Release();


    /*CPoint	point = image.GetOffset();
	if( !binimage.CreateNew(cx, cy) )
		return false;
	binimage->SetOffset( point, true );*/
	CPoint ptOffs;
	if (!MakeBinary(image, binimage, ptOffs))
		return false;

	long	colors = image.GetColorsCount();
	
	StartNotification( cy );


	_ptr_t2<color*>	ptr_colors;
	_ptr_t2<byte>	ptr_byte;

	color** ppcolors = ptr_colors.alloc( colors );
	byte* pbuff = ptr_byte.alloc( cx+4 );;

	int nBinaryRowSize = 0;
		
	for( long y = 0; y < cy; y++ )
	{
		if(nPane != -1)
		{
			color *pcolor = image.GetRow( y, nPane );
			for(int x=0; x<cx; x++) pbuff[x] = pcolor[x] >> 8;
		}
		else
		{
			for( long c = 0; c  < colors; c++ )
				ppcolors[c]= image.GetRow( y, c );
			ptrC->ConvertImageToDIB( ppcolors, pbuff, cx, false );
		}

		byte* pmasksource = image.GetRowMask( y );
		byte* binptr = 0;
		binimage->GetRow(&binptr, y+ptOffs.y, FALSE);
		binptr += ptOffs.x;
		byte* psource = pbuff;
		
		for( long x = 0; x < cx; x++, pmasksource++, psource++ )
		{
			byte	byteVal = 0;
			if( *pmasksource )
			{
				byteVal = (*psource >= nLow && *psource < nHigh)?1:0;
			}
			if(byteVal)
				//binptr[x/8] |= 0x80>>( x % 8 );
        binptr[x] = 0xFF;
			/*else
			{
				byte b = ~(0x80>>( x % 8 ));
				binptr[x/8] &= b;
			}*/
		}

		Notify( y );
	}

	FinishNotification();


	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionExtractPhase implementation
const char* szSectionName = "ExtractPhases";

CActionExtractPhase::CActionExtractPhase()
{
	m_ppcolors = 0;
	m_pdwLoRanges = 0;
	m_pdwHiRanges = 0;
}

CActionExtractPhase::~CActionExtractPhase()
{
	if(m_ppcolors)
		delete	m_ppcolors;
	if(m_pdwLoRanges)
		delete m_pdwLoRanges;
	if(m_pdwHiRanges)
		delete m_pdwHiRanges;
}

COLORREF CActionExtractPhase::GetFillColor()
{
	COLORREF crFill = RGB(0,0,0);
	if(m_nClass >= 0)
	{
		CString strEntry;
		strEntry.Format("Class%dColor", m_nClass);
		crFill = (COLORREF)::GetValueInt(GetAppUnknown(), szSectionName, strEntry, RGB(0,0,0));
	}
	return crFill;
}

bool CActionExtractPhase::MakeBinaryImage()
{
	m_nClass = GetArgumentInt( "Class" );

	CString strEntry;
	strEntry.Format("Class%dPanesCount", m_nClass);
	int nPanes = ::GetValueInt(GetAppUnknown(), szSectionName, strEntry, 0);

	if(nPanes > 0)
	{
		m_pdwLoRanges = new DWORD[nPanes];
		m_pdwHiRanges = new DWORD[nPanes];
	}

	for(int i = 0; i < nPanes; i++)
	{
		strEntry.Format("Class%dPane%d_Lo", m_nClass, i);
		m_pdwLoRanges[i] = ::GetValueInt(GetAppUnknown(), szSectionName, strEntry, 0);
		strEntry.Format("Class%dPane%d_Hi", m_nClass, i);
		m_pdwHiRanges[i] = ::GetValueInt(GetAppUnknown(), szSectionName, strEntry, 0);
	}
	
	//get the color convertor
	IUnknown	*punkCC = 0;
	m_image->GetColorConvertor( &punkCC );

	if( !punkCC )
		return false;
	IColorConvertorPtr	ptrC( punkCC );
	punkCC->Release();

	//byte	*psource;
	byte	*pmasksource, *pmaskdest;

	int	colors = m_image.GetColorsCount();

	ASSERT(colors == nPanes);

	//alloc memory
	m_ppcolors = new color*[colors];

	int	x, y, c;
	int	cx = m_image.GetWidth();
	int	cy = m_image.GetHeight();

	
	for( y = 0; y < cy; y++ )
	{
		for( c = 0; c <	colors; c++ )
			m_ppcolors[c]= m_image.GetRow( y, c );

		pmasksource = m_image.GetRowMask( y );
		pmaskdest = m_pwfi->GetRowMask( y+m_rect.top )+m_rect.left;

		for( x = 0; x < cx; x++, pmasksource++, pmaskdest++)
		{
			if( *pmasksource )
			{
				//byte	byteVal = ((*psource >= nLoB && *psource < nHiB)&&(*(psource+1) >= nLoG && *(psource+1) < nHiG)&&(*(psource+2) >= nLoR && *(psource+2) < nHiR))?255:0;
				//*pmaskdest = byteVal;
				//psource += 3;
				bool bInRange = true;
				for(int i = 0; i < nPanes; i++)
				{
					bInRange = bInRange && (*(m_ppcolors[i]+x) >= m_pdwLoRanges[i] && *(m_ppcolors[i]+x) < m_pdwHiRanges[i]);

					if(!bInRange) break;
				}

				*pmaskdest = bInRange?255:0;
			}
			else
			{
				*pmaskdest = 0;
			}
		}
		Notify( y );
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionAddRemoveColorInterval implementation
BEGIN_INTERFACE_MAP(CActionAddRemoveColorInterval, CInteractiveActionBase)
	INTERFACE_PART(CActionAddRemoveColorInterval, IID_IProvideColors, Colors)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////////////////
//ITextPage interface implementation
IMPLEMENT_UNKNOWN(CActionAddRemoveColorInterval, Colors);
HRESULT CActionAddRemoveColorInterval::XColors::GetColorsInfo(long* pnCount, DWORD** ppdwColors)
{
	_try_nested(CActionAddRemoveColorInterval, Colors, GetColorsInfo)
	{
		pThis->GetColorsInfo(pnCount, ppdwColors);
		return S_OK;
	}
	_catch_nested;
}

void CActionAddRemoveColorInterval::GetColorsInfo(long* pnCount, DWORD** ppdwColors)
{
	if(pnCount > 0)
	{
		*pnCount = m_nCount;
		*ppdwColors = m_pColors;
	}
}


CActionAddRemoveColorInterval::CActionAddRemoveColorInterval()
{
	m_pcontour = 0;
	m_bTracking = false;
}

CActionAddRemoveColorInterval::~CActionAddRemoveColorInterval()
{
	::ContourDelete( m_pcontour );
}

bool CActionAddRemoveColorInterval::ExecuteSettings( CWnd *pwndParent )
{
	return true;
}

bool CActionAddRemoveColorInterval::Invoke()
{
	CRect	rect = ::ContourCalcRect( m_pcontour );

	CImageWrp	image;

	sptrINamedDataObject2	sptrN;
	IUnknown	*punkImage = 0;

	IViewPtr sptrV(m_punkTarget);
	IUnknown *punkDoc = 0;
	sptrV->GetDocument(&punkDoc);
	IDocumentPtr sptrDoc(punkDoc);
	if(punkDoc)
		punkDoc->Release();

	
	CString strImageName = ::GetValueString(sptrDoc, "ExtractPhases", "WorkImage", "");

	if(!strImageName.IsEmpty())
	{
		punkImage = ::GetObjectByName(sptrDoc, strImageName, szTypeImage);
	}
	
	if(!punkImage)
		punkImage = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );

	if( !punkImage )
		return false;

	sptrN = punkImage;
	IUnknown	*punkParent = 0;
	sptrN->GetParent( &punkParent );

	if( punkParent )
	{
		punkImage->Release();
		image.Attach( punkParent, false );	
	}
	else
		image.Attach( punkImage, false );	
	

	int nPanes = image.GetColorsCount();

	CWalkFillInfo	wfi( rect );
	CDC	*pDC = wfi.GetDC();
	::ContourDraw( *pDC, m_pcontour, 0, cdfClosed|cdfFill );

	color** pColor = new color*[nPanes];

	long nPointInContour = 0;

	m_nCount = 0;
	m_pColors = 0;

	const int BodyMask = 1<<7;

	DWORD* pColors = new DWORD[rect.Width()*rect.Height()*nPanes];

	for(int i = rect.top; i <= rect.bottom; i++)
	{
		for(int k = 0; k < nPanes; k++)
		{
			pColor[k] = image.GetRow(i, k);	//image row
		}

		for(int j = rect.left; j <= rect.right; j++)
		{
			BYTE pixel = wfi.GetPixel(j, i);
			if(pixel & BodyMask)
			{
				nPointInContour++;
				/*DWORD* pColors = m_pColors;
				m_pColors = new DWORD[nPointInContour*nPanes];
				if(pColors)
				{
					memcpy(m_pColors, pColors, sizeof(DWORD)*m_nCount);
					delete pColors;
					pColors = 0;
				}
				m_nCount = nPointInContour*nPanes;
				for(k = 0; k < nPanes; k++)
				{
					m_pColors[m_nCount - nPanes + k] = *(pColor[k]+j);
				}
				*/
				m_nCount = nPointInContour*nPanes;
				for(k = 0; k < nPanes; k++)
				{
					pColors[m_nCount - nPanes + k] = *(pColor[k]+j);
				}
			}
		}
	}

	if(m_nCount > 0)
	{
		m_pColors = new DWORD[m_nCount];
		memcpy(m_pColors, pColors, sizeof(DWORD)*m_nCount);
	}
	
	if(pColors)
		delete pColors;

	if(pColor)
		delete pColor;

	::FireEvent(GetAppUnknown(), szEventManageColorInterval, GetControllingUnknown());
		
	if(m_pColors)
	{
		delete m_pColors;
		m_pColors = 0;
	}
	m_nCount = 0;

	return true;
}

//extended UI
bool CActionAddRemoveColorInterval::IsAvaible()
{
	return CheckInterface( m_punkTarget, IID_IImageView );
}

bool CActionAddRemoveColorInterval::IsChecked()
{
	return false;
}

bool CActionAddRemoveColorInterval::Initialize()
{
	if( !CInteractiveActionBase::Initialize() )return false;
	m_hcurActive = AfxGetApp()->LoadCursor( IDC_ADDREMOVE_INTERVAL );
	m_pcontour = ::ContourCreate();
	return true;
}

bool CActionAddRemoveColorInterval::DoStartTracking( CPoint pt )
{
	m_bTracking = true;
	::ContourAddPoint( m_pcontour, pt.x, pt.y );
	_Draw();
	return true;
}

bool CActionAddRemoveColorInterval::DoTrack( CPoint pt )
{
	ASSERT(m_pcontour);
	ASSERT(m_pcontour->nContourSize > 0);

	ContourPoint	cpt = m_pcontour->ppoints[m_pcontour->nContourSize-1];

	if( cpt.x  == pt.x && cpt.y == pt.y )
		return true;

	_Draw();
	::ContourAddPoint( m_pcontour, pt.x, pt.y );
	_Draw();

	return true;
}

bool CActionAddRemoveColorInterval::DoFinishTracking( CPoint pt )
{
	ASSERT(m_pcontour);
	ASSERT(m_pcontour->nContourSize > 0);

	ContourPoint	cpt = m_pcontour->ppoints[m_pcontour->nContourSize-1];

	_Draw();
	if( cpt.x != pt.x && cpt.y != pt.y )
	{
		::ContourAddPoint( m_pcontour, pt.x, pt.y );
	}

	m_bTracking = false;

	int x = 0;
	int y = 0;
	::ContourGetPoint( m_pcontour, 0, x, y );
	::ContourAddPoint( m_pcontour, x, y );
	_Draw();

	Sleep(300);
	_Draw();

	Finalize();
	return true;
}

void CActionAddRemoveColorInterval::DoDraw( CDC &dc )
{
	::ContourDraw( dc, m_pcontour, m_punkTarget, 0 );
}


//////////////////////////////////////////////////////////////////////
//CActionAddRemoveColor implementation
BEGIN_INTERFACE_MAP(CActionAddRemoveColor, CInteractiveActionBase)
	INTERFACE_PART(CActionAddRemoveColor, IID_IProvideColors, Colors)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////////////////
//ITextPage interface implementation
IMPLEMENT_UNKNOWN(CActionAddRemoveColor, Colors);
HRESULT CActionAddRemoveColor::XColors::GetColorsInfo(long* pnCount, DWORD** ppdwColors)
{
	_try_nested(CActionAddRemoveColor, Colors, GetColorsInfo)
	{
		pThis->GetColorsInfo(pnCount, ppdwColors);
		return S_OK;
	}
	_catch_nested;
}


CActionAddRemoveColor::CActionAddRemoveColor()
{
	m_nX = -1;
	m_nY = -1;
	m_bInit = false;
	m_nCount = 0;
	m_pColors = 0;
}

CActionAddRemoveColor::~CActionAddRemoveColor()
{
}

void CActionAddRemoveColor::GetColorsInfo(long* pnCount, DWORD** ppdwColors)
{
	if(pnCount > 0)
	{
		*pnCount = m_nCount;
		*ppdwColors = m_pColors;
	}
}

bool CActionAddRemoveColor::Invoke()
{
	_try(CActionAddRemoveColor, Invoke)
	{
		if(m_nX < 0 || m_nY < 0) return false;
		sptrIDataContext sptrDC(m_punkTarget);
		if(sptrDC == 0)
			return false;
		IUnknown* punkImage = 0;

		IViewPtr sptrV(m_punkTarget);
		IUnknown *punkDoc = 0;
		sptrV->GetDocument(&punkDoc);
		IDocumentPtr sptrDoc(punkDoc);
		if(punkDoc)
			punkDoc->Release();

		CString strImageName = ::GetValueString(sptrDoc, "ExtractPhases", "WorkImage", "");

		if(!strImageName.IsEmpty())
		{
			
			punkImage = ::GetObjectByName(sptrDoc, strImageName, szTypeImage);
		}
		
		if(!punkImage)
			sptrDC->GetActiveObject( _bstr_t(szArgumentTypeImage), &punkImage );

		if( !punkImage )
			return false;
		
		CImageWrp image(punkImage);

		punkImage->Release();

		CSize imgOffset = image.GetOffset();
		int	nColorsCount = image.GetColorsCount();
		IUnknown *punkCC;
		image->GetColorConvertor( &punkCC );
		sptrIColorConvertor sptrCC(punkCC);
		punkCC->Release();
		if(sptrCC == 0)
			return false;

		CRect	rc = image.GetRect();
		if(rc.PtInRect(CPoint(m_nX, m_nY)) == FALSE) 
		{
			return false;
		}

		m_nX -= imgOffset.cx;
		m_nY -= imgOffset.cy;

		
		m_nCount = nColorsCount;
		m_pColors = new DWORD[nColorsCount];

		for( int nColor = 0; nColor < nColorsCount; nColor++ )
		{
			m_pColors[nColor] = *(image.GetRow( m_nY, nColor ) + m_nX);
		}
		
		SetArgument( _T("X"), _variant_t( (long)m_nX ) );
		SetArgument( _T("Y"), _variant_t( (long)m_nY ) );

		::FireEvent(GetAppUnknown(), szEventManageColorInterval, GetControllingUnknown());
		
		if(m_pColors)
		{
			delete m_pColors;
			m_pColors = 0;
		}
		m_nCount = 0;

		return true;
	}
	_catch;

	return false;
}

bool CActionAddRemoveColor::ExecuteSettings( CWnd *pwndParent )
{
	m_nX = GetArgumentInt( _T("X") );
	m_nY = GetArgumentInt( _T("Y") );
	return true;
}


//extended UI
bool CActionAddRemoveColor::IsAvaible()
{
	return CheckInterface( m_punkTarget, IID_IImageView );
}

bool CActionAddRemoveColor::IsChecked()
{
	return false;
}

bool CActionAddRemoveColor::DoLButtonDown( CPoint pt )
{
	m_bInit = true;
	m_nX = pt.x;
	m_nY = pt.y;
	return false;
}

bool CActionAddRemoveColor::DoLButtonUp( CPoint pt )
{
	m_nX = pt.x;
	m_nY = pt.y;
	if(m_bInit)
	{
		Finalize();
	}
	m_bInit = false;
	return false;
}

bool CActionAddRemoveColor::Initialize()
{
	if( !CInteractiveActionBase::Initialize() )return false;
	m_hcurActive = AfxGetApp()->LoadCursor(IDC_ADD_COLOR);
	if(m_nX >= 0 && m_nY >= 0)
	{
		Finalize();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionThresholdingGray implementation
CActionThresholdingGray::CActionThresholdingGray()
{
	m_pbuff = 0;
	m_ppcolors = 0;
}

CActionThresholdingGray::~CActionThresholdingGray()
{
	delete m_pbuff;
	delete m_ppcolors;
}

bool CActionThresholdingGray::MakeBinaryImage()
{
	int	nLow = GetArgumentInt( "Low" );
	int	nHigh = GetArgumentInt( "High" );
	int	nPane = GetArgumentInt( "PaneNum" );
	m_nPane = nPane;

	//get the color convertor
	IUnknown	*punkCC = 0;
	m_image->GetColorConvertor( &punkCC );

	if( !punkCC )
		return false;
	IColorConvertor6Ptr	ptrC( punkCC );
	punkCC->Release();

	byte	*psource;
	byte	*pmasksource, *pmaskdest;

	int	colors = m_image.GetColorsCount();

	//alloc memory
	m_pbuff = new byte[m_rect.Width()+4];
	m_ppcolors = new color*[colors];

	int	x, y, c;
	int	cx = m_image.GetWidth();
	int	cy = m_image.GetHeight();

	
	for( y = 0; y < cy; y++ )
	{
		for( c = 0; c <	colors; c++ )
			m_ppcolors[c]= m_image.GetRow( y, c );
		if(nPane >= 0)
		{
			color *pcolor = m_image.GetRow( y, nPane );
			for(int x=0; x<cx; x++) m_pbuff[x] = pcolor[x] >> 8;
		}
		else
		{
			//convert row to gray
			IDocumentSitePtr sptrDS(m_punkTarget);
			IUnknownPtr punkView;
			sptrDS->GetActiveView(&punkView);
			ptrC->ConvertImageToDIB2( m_ppcolors, m_pbuff, m_rect.Width(), false, nPane,
				m_image, punkView);
		}

		psource = m_pbuff;
		pmasksource = m_image.GetRowMask( y );
		pmaskdest = m_pwfi->GetRowMask( y );

		for( x = 0; x < cx; x++, pmasksource++, pmaskdest++, psource++ )
		{
			if( *pmasksource )
			{
				byte	byteVal = (*psource >= nLow && *psource < nHigh)?255:0;
				*pmaskdest = byteVal;
			}
			else
			{
				*pmaskdest = 0;
			}
		}
		Notify( y );
	}

	return true;
}



///////////////////////////////////////////////////////////////////////////////////////////////
//CActionThresholdingGrayBinAuto implementation	///////////////
using namespace gray_histo;

CActionThresholdingGrayBinAuto::CActionThresholdingGrayBinAuto()
{}

CActionThresholdingGrayBinAuto::~CActionThresholdingGrayBinAuto()
{}


bool CActionThresholdingGrayBinAuto::InvokeFilter()
{
	/*
	ARG_BOOL(_T("LightOnDark"), 1 )
	ARG_INT(_T("PaneNum"), -1 ) 
	ARG_IMAGE( _T("Image") )
	RES_BINIMAGE( _T("ThresholdingGrayAuto") )                                            
	*/

	BOOL bLoD = GetArgumentInt("LightOnDark");
	int nPane = GetArgumentInt("PaneNum");

	CImageWrp	image( GetDataArgument() );
	
	CBinImageWrp	binimage( GetDataResult() );

	long	cx = image.GetWidth(), cy = image.GetHeight();
	if( cy == 0 ) return false;

	CPoint offset;
	if (!MakeBinary(image, binimage, offset)) return false;

	IUnknown	*punkCC = 0;
	image->GetColorConvertor( &punkCC );
	if( !punkCC )
		return false;
	IColorConvertorPtr	ptrC( punkCC );
	punkCC->Release();

	long	colors = image.GetColorsCount();

	_ptr_t2<color*>	ptr_colors;
	_ptr_t2<byte>	ptr_byte;


	StartNotification( cy );



	color** ppcolors = ptr_colors.alloc( colors );
	byte* pbuff = ptr_byte.alloc( cx+4 );
	
	int nBinaryRowSize = 0;

	CHistogramm histo;
	histo.Create("GrayThresholdingBinAuto",bLoD);
//Now converting to gray and fill out histogramm
	for( long y = 0; y < cy; y++ )
	{
		if(nPane != -1)
		{
			color *pcolor = image.GetRow( y, nPane );
			for(int x=0; x<cx; x++) pbuff[x] = pcolor[x] >> 8;
		}
		else
		{
			for( long c = 0; c  < colors; c++ )
				ppcolors[c]= image.GetRow( y, c );
			ptrC->ConvertImageToDIB( ppcolors, pbuff, cx, false );
		}

		byte* pmasksource = image.GetRowMask( y );
		histo.AddData(pbuff, pmasksource, cx);
	
		byte* binptr = 0;
		binimage->GetRow(&binptr, y+offset.y, FALSE);
		
		binptr += offset.x;
		byte* psource = pbuff;


		for( long x = 0; x < cx; x++, pmasksource++, psource++ )
		{
			//byte	byteVal = 0;
			//if( *pmasksource )
			//{
			//	byteVal = (*psource >= nLow && *psource < nHigh)?1:0;
			//}
			//if(byteVal) binptr[x] = 0xFF;
			if( *pmasksource )
			{
				binptr[x]=*psource;
			}
		}
	Notify(y/2);
	}
	long nLow=0, nHigh=255;

	histo.SmoothData(1);
	histo.Equalize();	

	//if(!histo.FindMaxMins()) return false;
	//if(bLoD) nLow = histo.GetNextLimit();
	//else nHigh = histo.GetNextLimit();




	if(bLoD) nLow = histo.CallAlgorithm();
	else nHigh = histo.CallAlgorithm();

	IUnknown* pUnk = GetAppUnknown();
	::SetValue( pUnk, "GrayThresholdingBinAuto\\Result", "Low", nLow);
	::SetValue( pUnk, "GrayThresholdingBinAuto\\Result", "High", nHigh);
	//now fill binimage with 0/1
	for( long y = 0; y < cy; y++ )
	{
		byte* pmasksource = image.GetRowMask( y );

		byte* binptr = 0;
		binimage->GetRow(&binptr, y+offset.y, FALSE);

		binptr += offset.x;
		byte* psource = binptr;
		for( long x = 0; x < cx; x++, pmasksource++, psource++ )
		{

			byte	byteVal = 0;
			if( *pmasksource )
			{
				byteVal = (*psource >= nLow && *psource <= nHigh)? 0xff : 0;
				
			}
			binptr[x]=byteVal;
		}

		Notify( (cy+y)/2 );
	}

	FinishNotification();
	return true;
}


bool CHistogramm::Create(TCHAR* tcsSectName, BOOL useLightObjects)
{
	 
	IUnknown* pUnc = GetAppUnknown();
	Algorithm = ::GetValueInt(pUnc, tcsSectName, "Algorithm", 0);
	fUseFract    = ::GetValueDouble(pUnc, tcsSectName, "UseFract", 0.99);
	if(fUseFract>=1 || fUseFract<=0) fUseFract =0.99;
	currentLimit=0;
	histo.RemoveAll();
	histo.SetSize(256);
	ZeroMemory(histo.GetData(),256*sizeof(ULONG));
	this->useLightObjects = useLightObjects;
	//limits.RemoveAll();
	//maximums.RemoveAll();
	dataCount = 256;
	pixCount = 0;
	nFrom=nTo=-1;


#ifdef _DEBUG
	bCreated = TRUE;
#endif
	return true;
}
int CHistogramm::CallAlgorithm()
{

	ASSERT(bCreated);
	int ret;
	switch(Algorithm)
	{
	case 1:
		ret = Enthropy();//
		break;
	case 2:
		ret = OtsuThreshold();
		break;
	case 0:
	default:
		ret = IterativeThreshold();
		break;
	}
	return ret;
}
void CHistogramm::AddData(byte* imageRow, byte* rowMask, UINT cx)
{
	ASSERT(bCreated);
	for(UINT i =0; i<cx; i++, imageRow++, rowMask++ )
	{
		if (*rowMask) 
		{
			histo[*imageRow]++;
			pixCount++;
		}

	}
	
}

void CHistogramm::SmoothData(/*CArray<ULONG>& dst,*/ int nMask)
{
	ASSERT(bCreated);
	UINT nLen = histo.GetSize();
	CArray<ULONG> src;
	src.Copy(histo);

	for (int i = 0; i < nLen; i++)
	{
		long nSum = 0;
		int n1,n2;
		if (i < nLen - nMask)
		{
			n1 = max(0, i - nMask);
			n2 = n1 + 2 * nMask;
		}
		else // i >= nLen - nMask
		{
			n2 = nLen - 1;
			n1 = n2 - 2 * nMask;
		}
		int nSiz = n2 - n1 + 1;
		for (int j = n1; j <= n2; j++)
			nSum += src[j];
		histo[i] = nSum/nSiz;
	}
}

void CHistogramm::CalculatePixCount()
{
	ASSERT(bCreated);
	if(nFrom==nTo) nFrom=0, nTo = 255;
	pixCount = 0;
	for (UINT i=nFrom;i<=nTo;i++ )
		pixCount += histo[i];

}


/*UINT CHistogramm::GetNextLimit()
{
	ASSERT(bCreated);
	if (!limits.GetSize()) return 0;
	if (currentLimit==limits.GetSize()) { currentLimit=0; return 0; }
	currentLimit++;
	return limits.GetAt(currentLimit).pos;
	
}*/


double CHistogramm::mean(int beg, int end, double freq)
{
	double mean = 0;
	for(int i = beg; i<end;i++)
	{
		mean+=i*histo[i];
	}
	mean /= freq*pixCount;//mean
	return mean;
}

double CHistogramm::freq(int beg, int end)
{
	double f = 0;

	for(int i = beg; i<end;i++)
	{
		f+=histo[i];
	}
	f/=pixCount;
	return f;
}

double CHistogramm::variance(int beg, int end, double mean)
{
	double var = 0;
	for(int i = beg; i<end; i++)
	{
		int t = i-mean;
		var+= (t*t)*histo[i];
	}
	var/=pixCount;//variance
	return var;
}


int CHistogramm::OtsuThreshold()
{
	if(nFrom==nTo) nFrom = 0, nTo = 255;
	double criteria[256] ={0.0};
	double totalVar = variance(0,256,mean(0,256,freq(0,256)));
	for (int i=nFrom;i<=nTo;i++)
	{
	double w0 = freq(0,i);
	double w1 = freq(i,256);
	double m0 = mean(0,i,w0);
	double m1 = mean(i,256,w1);
	double m = m1-m0;

	criteria[i] = w0*w1*m*m;
	criteria[i]/=totalVar;
	}
	int ret;
	double maxC = criteria[0];
	for(i=1;i<256;i++)
	{
		if(maxC < criteria[i])
		{
			maxC = criteria[i];
			ret = i;
		}
	}
	return ret;
}
int CHistogramm::CalculateLocalPixCount(int begin,int end)
{
	int ret = 0;
	for(int i=begin;i<end;i++)
	{
		ret+=histo[i];
	}
	return ret;
}
int CHistogramm::Enthropy()
{
	double sums[256] ={0};
		double sumsB;
		double sumsW ;
	for (int j=0;j<256;j++)
	{
		int pixCountB = CalculateLocalPixCount(0,j);
		int pixCountW = CalculateLocalPixCount(j,256);

		sumsB = 0,sumsW = 0;
		for(int i = 0;i<j;i++)
		{
			if (!histo[i]) continue;
			double p = histo[i]/(double)pixCountB;
			sumsB+=p*log(p);
		}
		for(i=j;i<256;i++)
		{
			if (!histo[i]) continue;
			double p = histo[i]/(double)pixCountW;
			sumsW+=p*log(p);
		}
		sums[j] = -sumsB-sumsW;
	}
	double thres = sums[0];
	int ret = 0;
	for(j=0;j<256;j++)
	{
		if(thres<sums[j])
		{
			thres = sums[j];
			ret = j;
		}
	}
	return ret;

}


int CHistogramm::IterativeThreshold()
{	
	ASSERT(bCreated);
	
	int thresNew =(nTo+nFrom)/2, thresOld;

	do{	
		thresOld = thresNew;
		ULONG levLeft = 0,levRight = 0;
		ULONG pixLeft = 0, pixRight = 0;
		for(int i = 0; i<thresOld; i++)
		{
			levLeft+=i*histo[i];
			pixLeft+=histo[i];
		}
		for(int j=thresOld; j<256; j++)
		{
			levRight+=j*histo[j];
			pixRight+=histo[j];
		}
		levRight /= pixRight;
		levLeft /= pixLeft;
		thresNew = (levRight + levLeft)/2;


	}while(thresNew != thresOld);

	return thresNew;

}

//bool CHistogramm::FindMaxMins()
//{
//	ASSERT(bCreated);
//	SmoothData(1);
//	CalculatePixCount();
//	Equalize();
//	if (nTo == nFrom)
//		return false;
//	UINT nAreaSize = (int)((nTo-nFrom)*fLocalFract); // Size of area to find local maximum.
//	UINT nAreaRad  = nAreaSize / 2;//seek radius 
//
//	for (int i = nFrom; i <= nTo; i++)
//	{
//		int n1 = max(0,i-nAreaRad);
//		int n2 = min(i+nAreaRad,dataCount-1);
//		ULONG nValue = histo[i];
//
//		BOOL bMax = TRUE;
//
//		// Check for flat pick
//		register int n1f,n2f;// first points (to left and right)near 'i' which differs in value from 'i' 
//		for (n1f = i; n1f >= n1; n1f--)//to the left... 
//			if (histo[n1f] != nValue) break;
//				
//		if (n1f < n1 && n1 != 0) //not using a break => flat
//			bMax = FALSE;
//
//		if (bMax)
//		{
//			for (n2f = i; n2f <= n2; n2f++)//and to the right
//				if (histo[n2f] != nValue)
//					break;
//			if (n2f > n2 && n2 != dataCount - 1)
//				bMax = FALSE;
//		}
//
//		// Check for maximum.
//		if (bMax)
//		{
//			for (register int n = n1; n <= n1f; n++)//again to the left of the area...
//			{
//				if (histo[n] >= nValue && n != i)//skip itself (i)
//				{
//					bMax = FALSE;
//					break;
//				}
//			}
//		}
//		if (bMax)
//		{
//			for (int n = n2f; n <= n2; n++)//and to the right
//			{
//				if (histo[n] >= nValue && n != i)
//				{
//					bMax = FALSE;
//					break;
//				}
//			}
//		}
//		if (bMax)//bingo - Local maximum.
//		{
//			ASSERT(i >= 0);
//			ASSERT(i < dataCount);
//			HPEAK peak = {i, histo[i]};
//			maximums.Add(peak);
//			i += nAreaRad; //skip already learned area
//		}
//	}
//
//	// Now n local maximums found. Find the n-1 minimums on gaps
//	// between this points. Yet another two points - 0 and m_Histo.m_nDataNum-1.
//	// Special cases : 1. No local maximums found. Prpbably, the
//	// histogram have the uniform type. In this case binarization may not be
//	// performed.
//	// 2. 1 local maximum found. In this case use empiric rule -
//	// select objects as points, lying above or below the points
//	// which separates the side of histogram pf 18% points.
//	int maxCount = maximums.GetSize();
//	if (maxCount <= 0) //uniform/empty or error
//		return false;
//	if (maxCount == 1)// Special case 2.
//	{
//		
//		int nLim = -1, nFind, nPtNum;
//		UINT nPick = maximums.GetAt(0).pos;
//		nFind = (int)(fBackFract *  pixCount);//crop by 18%
//		nPtNum = pixCount;
//		if (nTo - nPick > nPick - nFrom)	//maximum near left
//		{
//
//			for (i = dataCount - 1; i >= 0; i--)
//			{
//				nPtNum -= histo[i];
//				if (nPtNum <= nFind)
//				{
//					nLim = i;
//					break;
//				}
//			}
//		}
//		else								//maximum near right
//		{
//
//			for (ULONG i = 0; i < dataCount; i++)
//			{
//				nPtNum -= histo[i];
//				if (nPtNum <= nFind)
//				{
//					nLim = i;
//					break;
//				}
//			}
//		}
//		ASSERT(nLim != -1);
//
//		HPEAK peak ;
//		peak.pos = 0;
//		peak.value = histo[0];
//		limits.Add(peak);
//
//		peak.pos = nLim;
//		peak.value = histo[nLim];
//		limits.Add(peak);
//
//		peak.pos = dataCount-1;
//		peak.value = histo[dataCount-1];
//		limits.Add(peak);
//		return true;
//	}
//
//	// Generic case.
//	HPEAK peak ;
//	peak.pos = 0;
//	peak.value = histo[0];
//	limits.Add(peak);
//
//	int sizeM = maximums.GetSize();
//	int begin = 0;
//	if(!useLightObjects) //we need only most light or dark peak and its neighbour 
//	{
//		begin = sizeM - 2;
//	}
//
//	{
//		int nFrom = maximums.GetAt(begin).pos;
//		int nTo   = maximums.GetAt(begin+1).pos;
//		/*if (nFrom < nTo)*/
//		{
//			for (int i = nFrom; i < nTo; i++)
//			{
//				if (histo[i] > histo[nFrom])	//finding first point with value bigger then
//					break;						// value(nFrom)
//			}
//			ASSERT(i > nFrom);
//			nTo = i + 1;
//		}
//		//else if (nTo < nFrom)    sims to be a nonsence 
//		//{
//		//	for (int i = nTo; i > nFrom; i--)
//		//	{
//		//		if (m_Histo.m_pData[i] > m_Histo.m_pData[nTo])
//		//			break;
//		//	}
//		//	ASSERT(i < nTo);
//		//	nFrom = i - 1;
//		//}
//		double fVal1 = maximums.GetAt(begin).value;
//		double fVal2 = maximums.GetAt(begin+1).value;
//		ASSERT(nTo > nFrom);
//		int nCenter = ( nFrom + nTo ) / 2; 
//		int nArea = ( nTo - nFrom ) / 2;
//		int nFndPos = -1;
//		// Find the minimum on this area.
//		if (Algorithm == 1) //
//		{
//			int nFndMin = 0x7FFFFFFF;
//			int nFndDist = nTo - nFrom;
//			ASSERT(nFndDist > 0);
//			for (int n = nFrom + 1; n < nTo; n++)
//			{
//				int nValue = histo[n];
//				int nDist = abs(n - nCenter);
//				if (nValue < nFndMin ||
//					nValue == nFndMin && nDist < nFndDist)
//				{
//					nFndPos = n;
//					nFndMin = nValue;
//					nFndDist = nDist;
//				}
//			}
//			ASSERT(nFndMin != 0x7FFFFFFF);
//		}
//		else
//		{
//			double fFndMax = -1;
//			for (int n = nFrom + 1; n < nTo; n++)
//			{
//
//				int nValue = histo[n];
//				BOOL bMin = nValue <= histo[n - 1] &&
//					nValue <= histo[n + 1];
//				double fLevel = (fVal1*(nTo-n)+fVal2*(n-nFrom))/(nTo-nFrom);
//				double fVal = fLevel - nValue;
//				if (fVal > fFndMax && bMin)
//				{
//					nFndPos = n;
//					fFndMax = fVal;
//				}
//			}
//			ASSERT(fFndMax != -1);
//		}
//		ASSERT(nFndPos != -1);
//		peak.pos = nFndPos;
//		peak.value = histo[nFndPos];
//		limits.Add(peak);
//	}
//	
//	peak.pos = dataCount-1;
//	peak.value = histo[peak.pos];
//	limits.Add(peak); // so now we've a 3 limits - first, min and last points of histogramm 
//	return true;
//}

void CHistogramm::Equalize()
{	
	ASSERT(bCreated);
	int nDataNum = histo.GetSize();
	float k = fUseFract;
	int nFind = (int)(k * pixCount);
	int nPtNum = pixCount;
	// Find the area, in which 99% of points are situated.
	// Find the lower bound.
	for (register int i = 0; i < nDataNum; i++)
	{
		nPtNum -= histo[i];
		if (nPtNum <= nFind)
		{
			nFrom = i;
			break;
		}
	}
	ASSERT(nFrom != -1);
	// Find the higher bound.
	nPtNum = pixCount;
	for (i = nDataNum - 1; i >= 0; i--)
	{
		nPtNum -= histo[i];
		if (nPtNum <= nFind)
		{
			nTo = i;
			break;
		}
	}
	CalculatePixCount();
	ASSERT(nTo != -1);
	ASSERT(nTo >= nFrom);
}

bool CActionThresGrayMute::Invoke()
{

	IApplicationPtr app = ::GetAppUnknown();

	IUnknown* punkDoc = 0;
	app->GetActiveDocument(&punkDoc);

	IUnknown* punkObj = GetActiveObjectFromDocument(punkDoc, szTypeImage);
	if(!punkObj) return false;
	CImageWrp	image( punkObj );
	if(punkDoc)
		punkDoc->Release();
	if(punkObj)
		punkObj->Release();
	if(image==0) return false;



	BOOL bLoD = GetArgumentInt("LightOnDark");
	int nPane = GetArgumentInt("PaneNum");

	long	cx = image.GetWidth(), cy = image.GetHeight();
	if( cy == 0 ) return false;

	IUnknown	*punkCC = 0;
	image->GetColorConvertor( &punkCC );
	if( !punkCC )
		return false;
	IColorConvertorPtr	ptrC( punkCC );
	punkCC->Release();

	long	colors = image.GetColorsCount();

	_ptr_t2<color*>	ptr_colors;
	_ptr_t2<byte>	ptr_byte;

	color** ppcolors = ptr_colors.alloc( colors );
	byte* pbuff = ptr_byte.alloc( cx+4 );

	int nBinaryRowSize = 0;

	CHistogramm histo;
	histo.Create("GrayThresholdingBinAutoMute",bLoD);
	//Now converting to gray and fill out histogramm
	for( long y = 0; y < cy; y++ )
	{
		if(nPane != -1)
		{
			color *pcolor = image.GetRow( y, nPane );
			for(int x=0; x<cx; x++) pbuff[x] = pcolor[x] >> 8;
		}
		else
		{
			for( long c = 0; c  < colors; c++ )
				ppcolors[c]= image.GetRow( y, c );
			ptrC->ConvertImageToDIB( ppcolors, pbuff, cx, false );
		}
		byte* pmasksource = image.GetRowMask( y );
		histo.AddData(pbuff, pmasksource, cx);

		//Notify(y/2*);
	}

	long nLow=0, nHigh=255;

	histo.SmoothData(1);
	histo.Equalize();	


	if(bLoD) nLow = histo.CallAlgorithm();
	else nHigh = histo.CallAlgorithm();


	::SetValue( app, "GrayThresholdingBinAutoMute\\Result", "Low", nLow);
	::SetValue( app, "GrayThresholdingBinAutoMute\\Result", "High", nHigh);

	return true;


}
