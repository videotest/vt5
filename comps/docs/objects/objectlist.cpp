#include "stdafx.h"
#include "objectlist.h"

#include "ClassBase.h"
#include "Utils.h"
#include "Utils2.h"
#include "NameConsts.h"
#include "Params.h"
#include "\vt5\common2\misc_calibr.h"
#include "\vt5\common2\class_utils.h"
#include "\vt5\common\misc_utils.h"
#include "\vt5\AWIN\profiler.h"

#include "resource.h"
#pragma warning ( disable : 4244)
#define BASE_PARAMS_IMAGE		"Image"
#define BASE_PARAMS_OBJECTLIST	"ObjectList"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static GUID GetObjectBaseKey( IUnknown *punk )
{
	INamedDataObject2Ptr	ptrN( punk );
	if( ptrN != 0 )
	{
		GUID	guid;
		ptrN->GetBaseKey( &guid );
		return guid;
	}
	return GUID_NULL;
}
static IUnknown *GetObjectByBaseKey( IUnknown *punkData, GuidKey lKey )
{

	sptrIDataTypeManager	sptrM( punkData );

	if (sptrM == 0)
		return 0;

	long	nTypesCounter = 0;
	sptrM->GetTypesCount( &nTypesCounter );

	for( long nType = 0; nType < nTypesCounter; nType ++ )
	{
		IUnknown	*punkObj = 0;
		LONG_PTR	lpos = 0;

		sptrM->GetObjectFirstPosition( nType, &lpos );

		while( lpos )
		{
			punkObj = 0;
			sptrM->GetNextObject( nType, &lpos, &punkObj );

			if( GetObjectBaseKey( punkObj ) == lKey )
				return punkObj;
			if(punkObj) punkObj->Release();
		}
	}
	return 0;
}

static IUnknown *FindObjectByBaseKey( GuidKey lKey )
{
	//try to locate data object with specified key in all documents

	sptrIApplication	sptrApp( ::GetAppUnknown() );

	LONG_PTR	lPosTempl, lPosDoc;

	sptrApp->GetFirstDocTemplPosition( &lPosTempl );

	while( lPosTempl )
	{
		sptrApp->GetFirstDocPosition( lPosTempl, &lPosDoc );

		while( lPosDoc )
		{
			IUnknownPtr	ptrData;
			sptrApp->GetNextDoc( lPosTempl, &lPosDoc, &ptrData );
			IUnknown *punkObject =GetObjectByBaseKey( ptrData, lKey );

			if( punkObject )
			{
				return punkObject;
			}
		}
		sptrApp->GetNextDocTempl( &lPosTempl, 0, 0 );
	}
	return GetObjectByBaseKey( GetAppUnknown(), lKey );
}




static void set_composite_class(IUnknown* pUnk, long level)
{
	CString classifier = GetValueString(GetAppUnknown(),_T("Classes"),_T("CompositeObjectsClassFile"),_T("Composite.complex"));
	
	INamedPropBagPtr bag(pUnk);
	if(bag!=0)
	{	
		int pos = classifier.ReverseFind('\\')+1;
		TCHAR* buffer = classifier.GetBuffer();
		buffer+=pos;
		bstr_t clName(buffer);
		bag->SetProperty(clName, _variant_t(level));
	}
}

static int get_composite_class(IUnknown* object)
{
	CString classifier = GetValueString(GetAppUnknown(),_T("Classes"),_T("CompositeObjectsClassFile"),_T("Composite.complex"));
	if( !object )return -1;
	INamedPropBagPtr bag(object);
	if(bag!=0)
	{
		_variant_t	var;
		int pos = classifier.ReverseFind('\\')+1;
		TCHAR* buffer = classifier.GetBuffer();
		buffer+=pos;
		bag->GetProperty(bstr_t(buffer), &var);
		if(var.vt == VT_I4) return var.lVal;
	}
	return -1;
}


static void fill_contour(byte **dst, int cx, int cy, ContourPoint *pPoints, int n, byte color)
{
	if(n<=1) return;
	int* x0 = new int[cy];
	for(int i=0; i<cy; i++) x0[i]=-1;

	int lastDir=0;
	int x1=pPoints[n-1].x, y1=pPoints[n-1].y;
	for(i=0; i<n; i++)
	{
		if(y1!=pPoints[i].y)
		{
			int dy= pPoints[i].y>y1 ? 1 : -1;
			if(y1>=0 && y1<cy)
			{
				if(x0[y1]!=-1)
				{
					int xx1=min(x1,x0[y1]);
					int xx2=max(x1,x0[y1]);
					for(int fillx=xx1+1; fillx<=xx2; fillx++)
					{
						if(fillx>=0 && fillx<cx)
							dst[y1+1][fillx+1] ^= color;
					}
					x0[y1]=-1;
				}
				else x0[y1]=x1;
				if(dy!=lastDir && lastDir!=0)
				{
					if(x0[y1]==-1) x0[y1]=x1;
					else x0[y1]=-1;
				}
			}
			lastDir=dy;
		}
		x1=pPoints[i].x;
		y1=pPoints[i].y;
	}
	delete[] x0;
}

static void offset_contour(Contour* cDest , const Contour* cSource, POINT offset)
{
	ZeroMemory(cDest, sizeof(Contour));
	for(int i=0;i<cSource->nContourSize; i++)
	{
		ContourAddPoint(cDest,cSource->ppoints[i].x - offset.x, cSource->ppoints[i].y - offset.y);
	}
}
static void fill_rowmask(IUnknown* measObject)
{
//mark contours on rowmask bytefield 
		IMeasureObject2Ptr mo(measObject); 
		IUnknown* imUnk;
		mo->GetImage(&imUnk);
        IImage4Ptr image(imUnk);
		if(imUnk) imUnk->Release();
		if(image ==0) return;
		UINT pixCount =0;
		int cx,cy;
		image->GetSize(&cx,&cy);
		/*byte** rows = new byte*[cy];*/
		//for(int i=0;i<cy; i++)
		//{
		//	image->GetRowMask(0,&rows[i]);
		//	/*ZeroMemory(rows[i], cx);*/
		//}
		//int cCount;
		/*image->GetContoursCount( &cCount);
		for(i=0;i<cCount;i++)
		{
			Contour* contour; 
			image->GetContour( i, &contour );
			POINT offset;
			image->GetOffset(&offset);
			Contour c;
			offset_contour(&c, contour, offset);
			fill_contour(rows,cx, cy, c.ppoints, c.nContourSize, 255);
			for(int n =0; n<contour->nContourSize; n++)
			{
				rows[c.ppoints[n].y][c.ppoints[n].x] = 255;
			}
			delete[] c.ppoints;
		}*/
		for(int j=0;j<cy;j++)
		{
			byte* row;
			image->GetRowMask(j,&row);
			for(int i=0;i<cx;i++)
			{
				if(row[i]==255) pixCount++;
			}
		}
		//delete[] rows; 
		ICompositeSupportPtr comp(measObject);
		comp->SetPixCount(pixCount);
}
static bool is_child(IUnknown* parentUnk, IUnknown* childUnk, double coverFactor = 1. )
{
	//compare binary images (rowmasks) for child<=>parent relation
	//if images coincide (within coverFactor range) they must be treatened as parent and child 
	IMeasureObject2Ptr moParent(parentUnk), moChild(childUnk);
	IUnknown *imcUnk,*impUnk;
	moParent->GetImage(&impUnk);
	moChild->GetImage(&imcUnk);

	IImage4Ptr imageChild(imcUnk), imageParent(impUnk);
	if(impUnk) impUnk->Release();
	if(imcUnk) imcUnk->Release();
	if(imageChild ==0 || imageParent==0) return false;

	RECT rectChild, rectParent;

	imageChild->GetOffset((POINT*)&rectChild);
	imageParent->GetOffset((POINT*)&rectParent);

	imageChild->GetSize((int*)&rectChild.right, (int*)&rectChild.bottom);
	imageParent->GetSize((int*)&rectParent.right, (int*)&rectParent.bottom);

	rectChild.right+=rectChild.left;
	rectChild.bottom+=rectChild.top;

	rectParent.right+=rectParent.left;
	rectParent.bottom+=rectParent.top;

	RECT inter;

	if(IntersectRect(&inter, &rectParent, &rectChild))
	{
		POINT ptOffsetChild, ptOffsetParent;
		ptOffsetChild.x = inter.left - rectChild.left;
		ptOffsetChild.y = inter.top  - rectChild.top;
		ptOffsetParent.x = inter.left - rectParent.left;
		ptOffsetParent.y = inter.top  - rectParent.top;
		int cx = inter.right - inter.left;
		int cy = inter.bottom - inter.top;
		byte* childRow, *parentRow; 
		UINT hitCount =0;
		ICompositeSupportPtr compChild(childUnk), compParent(parentUnk);
		long parentPixCount=0, childPixCount;
		compChild->GetPixCount(&childPixCount);
		compParent->GetPixCount(&parentPixCount);
		UINT breakPoint = childPixCount*coverFactor;
		for(int j=0;j<cy;j++)
		{
			imageChild->GetRowMask(j+ptOffsetChild.y, &childRow);
			imageParent->GetRowMask(j+ptOffsetParent.y, &parentRow);
			childRow+=ptOffsetChild.x;
			parentRow+=ptOffsetParent.x;
			for(int i=0;i<cx;i++)
			{
				if((*childRow) == 255 && (*parentRow)==255)
				{
					hitCount++;
					if(hitCount>=breakPoint) return true;

				}
				childRow++;
				parentRow++;
			}
		}
	}
	return false;
}

void _free_param_c( ParameterContainer	*pdescr )
{
	if( pdescr->pGroup )pdescr->pGroup->Release();
	if( pdescr->bstrAction )::SysFreeString( pdescr->bstrAction );

	if( pdescr->pDescr && pdescr->pDescr->pos == 0  )
	{
		if( pdescr->pDescr->bstrName )SysFreeString( pdescr->pDescr->bstrName );
		if( pdescr->pDescr->bstrUserName )SysFreeString( pdescr->pDescr->bstrUserName );
		if( pdescr->pDescr->bstrDefFormat )SysFreeString( pdescr->pDescr->bstrDefFormat );
		if( pdescr->pDescr->bstrUnit )SysFreeString( pdescr->pDescr->bstrUnit );
		delete pdescr->pDescr;
	}

	delete pdescr;
}

const char *CMeasureObjectList::c_szType = szTypeObjectList;
const char *CMeasureObject::c_szType = szTypeObject;

const char *CMeasureObjectLight::c_szType = szTypeLightObject;


IMPLEMENT_DYNCREATE(CMeasureObjectList, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CMeasureObject, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CMeasureObjectLight, CCmdTargetEx);

BEGIN_INTERFACE_MAP(CMeasureObjectList, CDataObjectListBase)
	INTERFACE_PART(CMeasureObjectList, IID_ICalcObjectContainer,	CalcCntr)
	INTERFACE_PART(CMeasureObjectList, IID_ICompositeObject, Composite)
END_INTERFACE_MAP()

BEGIN_DISPATCH_MAP(CMeasureObjectList, CDataObjectListBase)
	//{{AFX_DISPATCH_MAP(CMeasureObjectList)
	DISP_FUNCTION(CMeasureObjectList, "GetParametersCount", GetParametersCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CMeasureObjectList, "GetFirstParameterPosition", GetFirstParameterPosition, VT_I4, VTS_NONE)
	DISP_FUNCTION(CMeasureObjectList, "GetNextParameter", GetNextParameter, VT_I4, VTS_I4 VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT)
	DISP_FUNCTION(CMeasureObjectList, "SetProperty", SetBagProperty, VT_BOOL, VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CMeasureObjectList, "GetProperty", GetBagProperty, VT_VARIANT, VTS_BSTR )

	DISP_FUNCTION(CMeasureObjectList, "GetSimilarObject", GetSimilarObject, VT_DISPATCH, VTS_DISPATCH )

	DISP_FUNCTION(CMeasureObjectList, "InitSimilarityHelper", InitSimilarityHelper, VT_EMPTY, VTS_NONE )
	DISP_FUNCTION(CMeasureObjectList, "DeInitSimilarityHelper", DeInitSimilarityHelper, VT_EMPTY, VTS_NONE )
	DISP_FUNCTION(CMeasureObjectList, "SimilarityHelper_ProcessObject", SimilarityHelper_ProcessObject, VT_EMPTY, VTS_DISPATCH )
	DISP_FUNCTION(CMeasureObjectList, "SimilarityHelper_ProcessObjectError", SimilarityHelper_ProcessObjectError, VT_EMPTY, VTS_NONE )
	DISP_FUNCTION(CMeasureObjectList, "SimilarityHelper_TestObjectList", SimilarityHelper_TestObjectList, VT_EMPTY, VTS_DISPATCH )
	DISP_FUNCTION(CMeasureObjectList, "SimilarityHelper_TestObjectList2", SimilarityHelper_TestObjectList2, VT_EMPTY, VTS_DISPATCH )
	DISP_FUNCTION(CMeasureObjectList, "SimilarityHelper_GetClass", SimilarityHelper_GetClass, VT_I4, VTS_NONE )
	DISP_FUNCTION(CMeasureObjectList, "SimilarityHelper_Get", SimilarityHelper_Get, VT_I4, VTS_NONE )
	DISP_FUNCTION(CMeasureObjectList, "SimilarityHelper_GetInClass", SimilarityHelper_GetInClass, VT_I4, VTS_I4 )
	DISP_FUNCTION(CMeasureObjectList, "SimilarityHelper_GetInOwnClass", SimilarityHelper_GetInOwnClass, VT_I4, VTS_NONE )
	DISP_FUNCTION(CMeasureObjectList, "SimilarityHelper_GetParamDistance", SimilarityHelper_GetParamDistance, VT_R8, VTS_I4 )
	DISP_FUNCTION(CMeasureObjectList, "SimilarityHelper_WriteObjectReport", SimilarityHelper_WriteObjectReport, VT_EMPTY, VTS_WBSTR )
	DISP_FUNCTION(CMeasureObjectList, "SimilarityHelper_WriteErrorReport", SimilarityHelper_WriteErrorReport, VT_EMPTY, VTS_WBSTR )

	DISP_FUNCTION(CMeasureObjectList, "SimilarityHelper_GetError2", SimilarityHelper_GetError2, VT_R8, VTS_NONE ) // получить ошибку, которая была при TestObjectList2
	DISP_FUNCTION(CMeasureObjectList, "SimilarityHelper_Improve2", SimilarityHelper_Improve2, VT_R8, VTS_NONE ) // один шаг оптимизации, возвращает, на сколько прооптимизировали; только после TestObjectList2

	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()
//{{AFX_MSG_MAP(CMeasureObjectList)
//}}AFX_MSG_MAP

 
BEGIN_INTERFACE_MAP(CMeasureObjectLight, CDataObjectBase)
	INTERFACE_PART(CMeasureObjectLight, IID_ICalcObject, CalcObj)
	INTERFACE_PART(CMeasureObjectLight, IID_ICalcObject2, CalcObj)
	INTERFACE_PART(CMeasureObjectLight, IID_IClonableObject, Clone)
	//INTERFACE_PART(CMeasureObjectLight, IID_INamedPropBag, NamedPropBag)
	//INTERFACE_PART(CMeasureObjectLight, IID_INamedPropBagSer, NamedPropBag)
END_INTERFACE_MAP()

BEGIN_INTERFACE_MAP(CMeasureObject, CMeasureObjectLight)
	INTERFACE_PART(CMeasureObject, IID_IClipboard,		Clipboard)
	INTERFACE_PART(CMeasureObject, IID_IMeasureObject,	MeasObj)
	INTERFACE_PART(CMeasureObject, IID_IMeasureObject2,	MeasObj)
	INTERFACE_PART(CMeasureObject, IID_ICalibr,	Calibr)
	INTERFACE_PART(CMeasureObject, IID_IRectObject,	Rect)
	INTERFACE_PART(CMeasureObject, IID_ICompositeSupport,	Composite)
END_INTERFACE_MAP()




// {59438A92-8B86-11d3-A684-0090275995FE}
static const GUID clsidDataObjectList = 
{ 0x59438a92, 0x8b86, 0x11d3, { 0xa6, 0x84, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {6D849A63-8C44-11d3-A686-0090275995FE}
static const GUID clsidDataObject = 
{ 0x6d849a63, 0x8c44, 0x11d3, { 0xa6, 0x86, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {81158BD6-2649-4201-9EAB-D9A3156B7372}
static const GUID clsidDataObjectLight = 
{ 0x81158bd6, 0x2649, 0x4201, { 0x9e, 0xab, 0xd9, 0xa3, 0x15, 0x6b, 0x73, 0x72 } };
// {59438A95-8B86-11d3-A684-0090275995FE}
GUARD_IMPLEMENT_OLECREATE_PROGID(CMeasureObjectList, "Objects.MeasureObjectList", 
0x59438a95, 0x8b86, 0x11d3, 0xa6, 0x84, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {6D849A62-8C44-11d3-A686-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CMeasureObject, "Objects.MeasureObject", 
0x6d849a62, 0x8c44, 0x11d3, 0xa6, 0x86, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {060FC17F-EAF0-4b1c-9561-83E03596B44E}
GUARD_IMPLEMENT_OLECREATE(CMeasureObjectLight, "Objects.MeasureObjectLight", 
0x60fc17f, 0xeaf0, 0x4b1c, 0x95, 0x61, 0x83, 0xe0, 0x35, 0x96, 0xb4, 0x4e);


DATA_OBJECT_INFO(IDS_MEASURE_LIST_TYPE, CMeasureObjectList, CMeasureObjectList::c_szType, clsidDataObjectList, IDI_THUMBNAIL_OBJECT );
DATA_OBJECT_INFO_FULL(IDS_MEASUREOBJECT_TYPE, CMeasureObject, CMeasureObject::c_szType, CMeasureObjectList::c_szType, clsidDataObject, IDI_THUMBNAIL_OBJECT );
DATA_OBJECT_INFO_FULL(IDS_MEASURELIGHT_TYPE, CMeasureObjectLight, CMeasureObjectLight::c_szType, CMeasureObjectList::c_szType, clsidDataObjectLight, IDI_THUMBNAIL_OBJECT );


IMPLEMENT_UNKNOWN(CMeasureObjectLight,	Clone);
IMPLEMENT_UNKNOWN_BASE(CCalcObjectContainerImpl,	CalcCntr);
IMPLEMENT_UNKNOWN(CMeasureObject,		MeasObj);
IMPLEMENT_UNKNOWN(CMeasureObject,		Calibr);
IMPLEMENT_UNKNOWN(CMeasureObject,		Rect);
IMPLEMENT_UNKNOWN(CMeasureObjectList, Composite);
IMPLEMENT_UNKNOWN(CMeasureObject, Composite);

//static int gObjListCount =0;
void CopyCalcVals(ICalcObjectPtr sptrIn, ICalcObjectPtr sptrOut)
{
	if(sptrIn == 0 || sptrOut == 0)
		return;

	LPOS nPos = 0;
	sptrIn->GetFirstValuePos(&nPos);
	while(nPos)
	{
		double fVal = 0;
		long nKey = 0;
		sptrIn->GetNextValue(&nPos, &nKey, &fVal);
		sptrOut->SetValue(nKey, fVal);
	}
}

CMeasureObjectList::CMeasureObjectList()
{
	_OleLockApp( this );
	purpose =Flat;
	m_bDirty =TRUE;
	m_posActive = 0;
	m_binaryCount =0;
	bComposite = FALSE;
	m_bLockRemoveNotification = false;
	m_bDelay = false;
	m_bFinalRelease = false;
	m_ci.actual() = false;
	//gObjListCount++;
	//CString s;
	//s.Format("\n--- Object list created: %X , %i\n\n", this, gObjListCount);
	//OutputDebugStr(s);
	m_pSimilarityHelper = 0;	
}

CMeasureObjectList::~CMeasureObjectList()
{
	delete m_pSimilarityHelper;
	_OleUnlockApp( this );
	/*CString s;
	gObjListCount--;
	s.Format("\n--- Object list destroyed: %X, %i\n\n", this, gObjListCount);
	OutputDebugStr(s);*/
}


bool SerializeBSTR( BSTR* pbstr, CStreamEx &ar )
{
	if( !pbstr )
		return false;

	if( ar.IsStoring() )
	{		
		UINT uiLen = SysStringByteLen( *pbstr );
		ar << uiLen;
		if( uiLen > 0 )
		{
			ar.Write( (BYTE*)(*pbstr), uiLen );
		}
	}
	else
	{
		*pbstr = 0;
		UINT uiLen = 0;
		ar >> uiLen;
		if( uiLen > 0 )
		{			
			BYTE *pBuf = new BYTE[uiLen];
			ar.Read( pBuf, uiLen );
			*pbstr = SysAllocStringByteLen( (LPCSTR)pBuf, uiLen );
			delete [] pBuf;	pBuf = 0;
			
		}
		
	}

	return true;
}

std_dll IUnknown* FindMeasGroup( GUID clsidGroup );	//defined in utils.h


//verrided function

LPSTREAM AFXAPI _AfxCreateMemoryStream()
{
	LPSTREAM lpStream = NULL;

	// Create a stream object on a memory block.
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, 0);
	if (hGlobal != NULL)
	{
		if (FAILED(CreateStreamOnHGlobal(hGlobal, TRUE, &lpStream)))
		{
			TRACE0("CreateStreamOnHGlobal failed.\n");
			GlobalFree(hGlobal);
			return NULL;
		}

		ASSERT_POINTER(lpStream, IStream);
	}
	else
	{
		TRACE0("Failed to allocate memory for stream.\n");
		return NULL;
	}

	return lpStream;
}

bool CMeasureObjectList::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	//TIME_TEST( "CMeasureObjectList::SerializeObject" )
	PROFILE_TEST( "CMeasureObjectList::SerializeObject" )
//	OutputDebugString("Start of CMeasureObjectList::SerializeObject\n");
//	CTimeTest tt(true, "CMeasureObjectList::SerializeObject");

	long lVersion = 4;


	{		
		//TIME_TEST( "CDataObjectListBase::SerializeObject" )
		//PROFILE_TEST( "CDataObjectListBase::SerializeObject" )
//		CTimeTest tt(true, "CMeasureObjectList::SerializeObject inside");
		BOOL bC =bComposite;
		bComposite = FALSE;
		if (!CDataObjectListBase::SerializeObject(ar, pparams))
			return false;
		bComposite = bC;
	}

	if( !CCalcObjectContainerImpl::Serialize( ar ) )
		return false;

	IStream *punkStream = _AfxCreateMemoryStream();

	if( ar.IsStoring() )
	{
		ar << lVersion;

		if(lVersion>=3)
		{
			ar<<bComposite;
			ar<<m_binaryCount;
			
		}
		if(lVersion>=4)
		{
			m_ci.Serialize(ar);
		}
		
		long lCount = m_params.GetCount();
		ar << lCount;
		
		POSITION	pos = m_params.GetHeadPosition();
		while( pos )
		{
			ParameterContainer	*pParCont = m_params.GetNext( pos );

			ar << pParCont->cbSize;
			ar << pParCont->lpos;
			CLSID clsidGroup = INVALID_KEY;
			if( pParCont->pGroup )
			{
				IPersistPtr ptrPersist( pParCont->pGroup );
				if( ptrPersist )
					ptrPersist->GetClassID( &clsidGroup );

				clsidGroup = ::GetInternalGuid( clsidGroup );
			}
			ar.Write( &clsidGroup, sizeof(CLSID) );

			BOOL bHaveDesc = ( pParCont->pDescr->pos ? FALSE : TRUE );
			ar << bHaveDesc;
			
			if( bHaveDesc )
			{
				ar << pParCont->pDescr->cbSize;
				SerializeBSTR( &pParCont->pDescr->bstrName, ar );				
				SerializeBSTR( &pParCont->pDescr->bstrUserName, ar );				
				SerializeBSTR( &pParCont->pDescr->bstrDefFormat, ar );				
				ar << pParCont->pDescr->lKey;
				ar << pParCont->pDescr->lEnabled;
			}
			else
				ar << pParCont->pDescr->lKey;
			
			ar << (long)pParCont->type;
			SerializeBSTR( &pParCont->bstrAction, ar );
			ar << pParCont->cxWidth;

		}

		if( punkStream )
		{
			((INamedPropBagSer*)this)->Store( punkStream );

			COleStreamFile sfile( punkStream );
			DWORD dwLen = (DWORD)sfile.GetLength();
			if( dwLen )
			{
				BYTE *lpBuf = new BYTE[dwLen];

				sfile.Seek( 0, CFile::begin );
				sfile.Read( (void*)lpBuf, dwLen );
							
				ar << dwLen;
				ar.Write( lpBuf, dwLen );

				delete []lpBuf;
			}
		}
	}
	else//loading
	{
		bComposite =FALSE;
		ar >> lVersion;

		m_ci.clear();
		m_lObjects.RemoveAll();

		if(lVersion>=3)
		{
			ar>>bComposite;
			ar>>m_binaryCount;
		}
		if(lVersion>=4)
		{
			m_ci.Serialize(ar);
		}
		
		long lCount = -1;
		ar >> lCount;			  
		
		for( int i=0;i<lCount;i++ )
		{
			ParameterContainer	*pParCont = new ParameterContainer;			
			::ZeroMemory( pParCont, sizeof(ParameterContainer) );

			ar >> pParCont->cbSize;
			ar >> pParCont->lpos;

			CLSID clsidGroup = INVALID_KEY;
			ar.Read( &clsidGroup, sizeof(CLSID) );

			if( clsidGroup != INVALID_KEY )
			{				
				clsidGroup = ::GetExternalGuid( clsidGroup );
				IUnknown* punkGroup = FindMeasGroup( clsidGroup );
				if( punkGroup )
				{
					IMeasParamGroupPtr ptrGroup( punkGroup );
					pParCont->pGroup = ptrGroup.Detach();
					punkGroup->Release();
				}
			}
			

			BOOL bHaveDesc = FALSE;
			ar >> bHaveDesc;
			
			if( bHaveDesc )
			{
				pParCont->pDescr = new ParameterDescriptor;
				::ZeroMemory( pParCont->pDescr, sizeof(ParameterDescriptor) );
				ar >> pParCont->pDescr->cbSize;				
				SerializeBSTR( &pParCont->pDescr->bstrName, ar );
				SerializeBSTR( &pParCont->pDescr->bstrUserName, ar );
				SerializeBSTR( &pParCont->pDescr->bstrDefFormat, ar );
				ar >> pParCont->pDescr->lKey;
				ar >> pParCont->pDescr->lEnabled;
				pParCont->pDescr->pos = 0;
			}
			else
			{
				long	lKey; LONG_PTR lpos = 0;
				ar >> lKey;
				if( pParCont->pGroup )
				{
					pParCont->pGroup->GetPosByKey( lKey, &lpos );
					if( lpos )
						pParCont->pGroup->GetNextParam( &lpos, &pParCont->pDescr );
				}
			}

			long nType = 0;
			ar >> nType;
			pParCont->type = (ParamType)nType;
			SerializeBSTR( &pParCont->bstrAction, ar );
			ar >> pParCont->cxWidth;

			//paul 3.06.2003
			if( !pParCont->pDescr )
			{
				_free_param_c( pParCont );
				continue;
			}

			if( pParCont->pGroup )
			{
				if( bHaveDesc )pParCont->pGroup->GetUnit( pParCont->type, &pParCont->pDescr->bstrUnit, &pParCont->pDescr->fCoeffToUnits );
				ICompositeMeasureGroupPtr cmg(pParCont->pGroup);
				if(cmg) 
					cmg->UpdateClassName(pParCont);
				pParCont->lpos = (LONG_PTR)m_params.AddTail( pParCont );
			}
			else
			{
				_free_param_c( pParCont );
			}


		}

		if( lVersion >= 2 && punkStream )
		{
			DWORD dwLen = 0;

			ar >> dwLen;
			if( dwLen )
			{
				BYTE *lpBuf = new BYTE[dwLen];

				ar.Read( (LPVOID)lpBuf, dwLen );

				COleStreamFile sfile( punkStream );
				sfile.Write( (void*)lpBuf, dwLen );
				sfile.Seek( 0, CFile::begin );

				((INamedPropBagSer*)this)->Load( punkStream );
				delete []lpBuf;
			}
		}

		if(bComposite) 
		{
			restore_tree_from_archive();
			m_bDirty = FALSE;
			/*bool lock = m_bLockUpdate;
			m_bLockUpdate = false;
			UpdateObject( 0 );
			m_bLockUpdate = lock;*/

		}
	/*CString s,s1="not composite";
	if(bComposite) s1 = "composite";
	s.Format("\n--- Object list loaded: %X, %s\n\n", this, s1);
	OutputDebugStr(s);*/
		
	}

	if( punkStream )
		punkStream->Release();


	CDataObjectListBase::OnAfterSerialize(ar);
	
	return true;
}

void CMeasureObjectList::OnAfterSerialize( CStreamEx& ar )
{
	// SBT 1228
	// CCalcObjectContainerImpl is invalid when OnAfterSerialize called from
	// CDataObjectListBase::SerializeObject. Notifications must be fired after 
	// CMeasureObjectList::SerializeObject completion.
}

void CMeasureObjectList::OnFinalRelease( )
{
	m_bFinalRelease = true;
	m_ci.clear();
	m_lObjects.RemoveAll();
	__super::OnFinalRelease();
}
bool CMeasureObjectList::IsObjectEmpty()
{
	return ( GetFirstChildPosition() == 0 );
}
HRESULT CMeasureObjectList::XComposite::IsComposite(long* bC)
{
	METHOD_PROLOGUE_BASE(CMeasureObjectList, Composite)	
	*bC = pThis->bComposite;
	return S_OK;
}
HRESULT CMeasureObjectList::XComposite::SetCompositeFlag()
{
	METHOD_PROLOGUE_BASE(CMeasureObjectList, Composite)
	pThis->bComposite = TRUE;
	return S_OK;
}

HRESULT CMeasureObjectList::XComposite::GetTreeNotion(void** tn)
{
	METHOD_PROLOGUE_BASE(CMeasureObjectList, Composite)	
	*tn = &pThis->m_lObjects;
	return S_OK;
}

HRESULT CMeasureObjectList::XComposite::GetLevel(int level, IUnknown** unkLevel)
{
	METHOD_PROLOGUE_BASE(CMeasureObjectList, Composite)
	if(!pThis->bComposite) return S_OK;
	*unkLevel  = pThis->m_lObjects.GetLevel(level);
	if(!(*unkLevel)) return E_FAIL;
	(*unkLevel)->AddRef();
	return S_OK;
}
HRESULT CMeasureObjectList::XComposite::GetPurpose(int* purpose)
{
	METHOD_PROLOGUE_BASE(CMeasureObjectList, Composite)	
	*purpose = pThis->purpose;
	return S_OK;
}
HRESULT CMeasureObjectList::XComposite::SetPurpose(int purpose)
{
	METHOD_PROLOGUE_BASE(CMeasureObjectList, Composite)
	if(purpose>1 || purpose<0) return E_INVALIDARG;
	pThis->purpose = (Purpose)purpose;
	return S_OK;
}
HRESULT CMeasureObjectList::XComposite::BuildTree(	LONG binaryCount)
{
	METHOD_PROLOGUE_BASE(CMeasureObjectList, Composite)

	IDataObjectListPtr ol(pThis);
	CListLockUpdate lu(ol, false);
	if(!pThis->m_bDirty || pThis->bComposite==FALSE) return S_OK;
	/*OutputDebugStr("---Building Tree---\n");*/
	INamedDataObject2Ptr ndo(pThis);
	pThis->m_lObjects.RemoveAll();
	if(binaryCount!=-1) 
	{
		pThis->m_binaryCount =binaryCount;

	}
	if(!pThis->m_ci. actual())
		pThis->m_ci.Init(binaryCount);
	int n = pThis->m_binaryCount;

	while(n)
	{
		pThis->m_lObjects.AddLevel(pThis->GetControllingUnknown());
		n--;
	}
	if(pThis->m_ci. actual())
	{	
		for(n=0;n<pThis->m_binaryCount;n++)
		{
			CTreeLevel* tl = pThis->m_lObjects.GetLevel(n);
			tl->SetClass(pThis->m_ci.Get(n));
		}
	}
	else pThis->m_ci.Init(pThis->m_binaryCount);
	TPOS pos;
	ndo->GetFirstChildPosition( &pos );
	long cnt;
	ndo->GetChildsCount( &cnt );
	while(cnt && pos)
	{
		IUnknown* unk;
		ndo->GetNextChild(&pos, &unk);
		pThis->store_child(unk,0);
		if(unk) unk->Release();
		cnt--;
	}	

	pThis->m_bDirty =FALSE;
	/*pThis->RegisterCompositeMeasgroup();*/
	return S_OK;
}
HRESULT CMeasureObjectList::XComposite::GetMainLevelObjects(IUnknown** unk)
{
	METHOD_PROLOGUE_BASE(CMeasureObjectList, Composite)
	*unk = 0;
	if(!pThis->bComposite) return S_OK;
	*unk = pThis->m_lObjects.GetBaseLevel();
	return S_OK;
}

HRESULT CMeasureObjectList::XComposite::RestoreTreeByGUIDs()
{
	METHOD_PROLOGUE_EX(CMeasureObjectList, Composite)
	pThis->restore_tree_from_archive();
	return S_OK;
}
HRESULT CMeasureObjectList::XComposite::GetLevelbyClass(int cls, IUnknown** unkLevel)
{
	METHOD_PROLOGUE_EX(CMeasureObjectList, Composite)
	CTreeLevel* tl;
	CLevels* lvls = pThis->m_lObjects.GetLevels();
	POSITION pos;
	pos = lvls->GetHeadPosition();
	while(pos)
	{
		tl =(CTreeLevel*) lvls->GetNext(pos);
		if(tl && (tl->GetClass()==cls)) break;
		tl =0;
	}
	*unkLevel = tl;
	if(tl) tl->AddRef();
	return S_OK;
}

HRESULT CMeasureObjectList::XComposite::GetElderLevel(int cls, IUnknown** unkLevel)
{
	METHOD_PROLOGUE_EX(CMeasureObjectList, Composite)
	CLevels* lvls = pThis->m_lObjects.GetLevels();
	POSITION pos = lvls->GetHeadPosition();
	while (pos)
	{
		POSITION prev = pos;
		CTreeLevel* tl = lvls->GetNext(pos);
		if(tl->GetClass() == cls)
		{
			lvls->GetPrev(prev);
			if(prev)
			{
				tl = lvls->GetPrev(prev);
			}
			else tl = 0;
			*unkLevel = (IUnknown*)tl;
			if(tl) tl->AddRef();
			return S_OK;
		}
	}
	return E_FAIL;
}
HRESULT CMeasureObjectList::XComposite::GetPlaneCount(long* count)
{
	METHOD_PROLOGUE_EX(CMeasureObjectList, Composite)
	*count = pThis->m_binaryCount;
	return S_OK;
}
HRESULT CMeasureObjectList::XComposite::SetPlaneCount(long count)
{
	METHOD_PROLOGUE_EX(CMeasureObjectList, Composite)
	pThis->m_binaryCount = count;
	return S_OK;
}
HRESULT CMeasureObjectList::XComposite::GetCompositeInfo(CCompositeInfo** ci)
{
	METHOD_PROLOGUE_EX(CMeasureObjectList, Composite)
	*ci = &pThis->m_ci;
	return S_OK;
}

HRESULT CMeasureObjectList::XComposite::SetCompositeInfo(CCompositeInfo* ci)
{
	METHOD_PROLOGUE_EX(CMeasureObjectList, Composite)
	pThis->m_ci.Copy(*ci);
	return S_OK;
}


HRESULT CMeasureObjectList::XComposite::CalculateComposite()
{
	METHOD_PROLOGUE_EX(CMeasureObjectList, Composite)

	IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IMeasureManager, szMeasurement);
	ICompManagerPtr ptrManager(punk) ;
	if( punk )punk->Release();
	if(ptrManager ==0) return E_FAIL;
	int	nGroup, nGroupCount;
	ICompositeMeasureGroupPtr cmg;
	IMeasParamGroupPtr mg;
	ptrManager->GetCount( &nGroupCount );
	for( nGroup = 0; nGroup < nGroupCount; nGroup++ )
	{
		IUnknown*	unkGroup;
		ptrManager->GetComponentUnknownByIdx( nGroup, &unkGroup );
		if(unkGroup)
		{
			cmg = unkGroup;
			unkGroup->Release();
			if(cmg!=0) 
			{
				mg=cmg;
				break;
			}
		}
	}

	if(cmg==0) return E_FAIL;
	
	mg->InitializeCalculation(pThis->GetControllingUnknown());

	CTreeLevel* tl = pThis->m_lObjects.GetBaseLevel();
	POSITION pos = tl->GetHeadPosition();
	while(pos)
	{
		CTreeNode* tn = tl->GetNext(pos);
		mg-> CalcValues(tn->GetData(), 0);
	}
	mg->FinalizeCalculation();
	return S_OK;
}

HRESULT CMeasureObjectList::XComposite::DelayRemove(long delay, IUnknown* pUnkLevel)
{
	METHOD_PROLOGUE_EX(CMeasureObjectList, Composite)
	if(!pThis->bComposite) return S_OK;
	pThis->m_bDelay = (delay!=0);
	CTreeLevel* tl = (CTreeLevel*)pUnkLevel;
	if(tl)
	{
		tl->DelayRemove(delay);
        return S_OK;
	}
	else
	{
		pThis->m_lObjects.DelayRemove(delay);
		return S_OK;
	}
	return E_FAIL;

}
static void NotifyObject(IUnknown *punk, int nCode, IUnknown *punkHint)
{
	INamedDataObject2Ptr sptrObj(punk);
	if (sptrObj == 0) return;
	TPOS lPos;
	if (FAILED(sptrObj->GetFirstChildPosition(&lPos)))
		return;
	while (lPos)
	{
		IUnknownPtr punkChild;
		sptrObj->GetNextChild(&lPos, &punkChild);
		INotifyObject2Ptr sptrNObj2(punkChild);
		if (sptrNObj2 != 0)
			sptrNObj2->NotifyDataListChanged(nCode, punkHint);
		NotifyObject(punkChild, nCode, punkHint);
	}
}

void CMeasureObjectList::OnAddChild(IUnknown *punkNewChild, POSITION pos)
{
	if(bComposite  /*&& m_bDelay*/)
	{
		ICompositeSupportPtr comSupp(punkNewChild);
		if(comSupp)
		{
			CListLockUpdate lock(GetControllingUnknown(),false);
			long f;
			comSupp->IsSyncronized(&f);
			f|=2;
			comSupp->SetSyncronizedFlag(f);
			INamedDataObject2Ptr ndo(punkNewChild);
			if(ndo)
			{
				ndo->SetObjectPosInParent( pos );
			}
			if( !(f&1))
			{
				long level;
				comSupp->GetLevel(&level);
				CTreeLevel* tl = m_lObjects.GetLevel(level);
				if(tl) tl->AddChild(punkNewChild);
			}
		}
	}
	__super::OnAddChild(punkNewChild,pos);
//	NotifyObject(GetControllingUnknown(),0,punkNewChild);
	NotifyObject(punkNewChild,0,punkNewChild);
}

void CMeasureObjectList::OnDeleteChild(IUnknown *punkChildToDelete, POSITION pos)
{
	if(bComposite && !m_bFinalRelease)
	{
		CListLockUpdate lck(GetControllingUnknown());
		ICompositeSupportPtr comSupp(punkChildToDelete);
		long f;
		if(comSupp)
		{			
			comSupp->IsSyncronized(&f);
			f&= (~2);
			comSupp->SetSyncronizedFlag(f);
			CTreeNode* tn = m_lObjects.Find(punkChildToDelete);		
			m_lObjects.root_out_branch(tn, m_bDelay, false);
		}
	}

	__super::OnDeleteChild(punkChildToDelete,pos);
//	NotifyObject(GetControllingUnknown(),1,punkChildToDelete);
	NotifyObject(punkChildToDelete,1,punkChildToDelete);

}
//void CMeasureObjectList::RegisterCompositeMeasgroup()
//{
//	IUnknownPtr unkPtr;
//	if(m_punkNamedData==0) 
//	{
//		INamedDataObject2Ptr nd(GetControllingUnknown());
//		if(nd)
//		{
//
//			nd->GetData(&unkPtr);
//		}
//		
//	}
//	else unkPtr = m_punkNamedData;
//	if(unkPtr =0) return;
//	IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IMeasureManager, szMeasurement);
//	ICompManagerPtr ptrManager(punk) ;
//	if( punk )punk->Release();
//	if(ptrManager ==0) return;
//	int	nGroup, nGroupCount;
//	ICompositeMeasureGroupPtr cmg;
//	ptrManager->GetCount( &nGroupCount );
//	for( nGroup = 0; nGroup < nGroupCount; nGroup++ )
//	{
//		IUnknown*	unkGroup;
//		ptrManager->GetComponentUnknownByIdx( nGroup, &unkGroup );
//		if(unkGroup)
//		{
//			cmg = unkGroup;
//			unkGroup->Release();
//			if(cmg!=0) break;
//		}
//	}
//	INotifyControllerPtr nc( m_punkNamedData );
//	if(nc!=0)
//	{
//		nc->RegisterEventListener(bstr_t( szEventChangeObjectList), cmg);
//	}
//
//}

POSITION CMeasureObjectList::find_child(IUnknown* punkChild)
{
	return _FindChild(punkChild);
}

POSITION CMeasureObjectList::DoGetActiveChild()
{
	/*if(bComposite) return m_posActive;
	else */return __super::DoGetActiveChild();
}

bool CMeasureObjectList::DoSetActiveChild(POSITION pos)
{
	if(pos == this->m_posActiveObject) return true;
		if(pos==0)
		{
			if(bComposite)
			{
				CTreeLevel* tl = m_lObjects.GetBaseLevel();
				if(tl)	tl->SetActiveChild(0,0);
			}

			m_bLockUpdate = false;
			return __super::DoSetActiveChild(pos);
		}

		if(bComposite)
		{
			IUnknown* punkChild;
			POSITION pSave = pos;
			INamedDataObject2Ptr ndo(this);
			ndo->GetNextChild((TPOS*) &pSave, &punkChild );
			if(punkChild && m_lObjects.GetLevelsCount()!=0)
			{
				CTreeNode* tn = m_lObjects.Find(punkChild);
				if(tn!=NULL)
				{
					CTreeNode* parentNode =tn;
					while(1)
					{
						parentNode = tn->GetParent();
						if(parentNode)
						{
							tn=parentNode;
						}
						else break;
					}
					INamedDataObject2Ptr nd(tn->GetData());
					nd->GetObjectPosInParent( &pos );
					m_posActive = pos;
					CTreeLevel* tl = m_lObjects.GetBaseLevel();
					if(tl)
					{
						POSITION p = tl->Find(tn);
						
						tl->SetActiveChild(p,0);
					}
				}
				punkChild->Release();
			}
			m_bLockUpdate = false;
		}
	
	return __super::DoSetActiveChild(pos);
}


//force declaration
void CopyParams(ICalcObjectContainerPtr in, ICalcObjectContainerPtr out);

HRESULT CMeasureObjectList::CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize )
{
	if( !punkObjSource )	return E_POINTER;

	CObjectListWrp ptr_src( punkObjSource );
	CObjectListWrp ptr_target( GetControllingUnknown() );

	if( ptr_src == 0 || ptr_target == 0 )	return S_FALSE;

	CopyParams( ptr_src, ptr_target );

	POSITION pos = ptr_src.GetFirstObjectPosition();
	while(pos)
	{
		IUnknown* punk_obj = ptr_src.GetNextObject(pos);
		if( !punk_obj )			continue;

		IClonableObjectPtr ptr_clone( punk_obj );
		punk_obj->Release();	punk_obj = 0;

		if( ptr_clone == 0 )	continue;

		IUnknown* punk_cloned = 0;
		ptr_clone->Clone( &punk_cloned );
		if( !punk_cloned )	continue;

		INamedDataObject2Ptr ptr_obj_new( punk_cloned );
		punk_cloned->Release();	punk_cloned = 0;

		if( ptr_obj_new != 0)
			ptr_obj_new->SetParent( ptr_target, 0);
	}

	CopyObjectNamedData( punkObjSource, GetControllingUnknown(), "\\", true );

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////
//CMeasureObjectLight

HRESULT CMeasureObjectLight::XClone::Clone(IUnknown** ppunkCloned)
{
	_try_nested(CMeasureObjectLight, Clone, Clone)
	{
		if (!ppunkCloned)
			return E_INVALIDARG;
		*ppunkCloned = pThis->Clone();
		return S_OK;
	}
	_catch_nested;
}


CMeasureObjectLight::CMeasureObjectLight()
{

}

CMeasureObjectLight::~CMeasureObjectLight()
{
}



IUnknown* CMeasureObjectLight::Clone()
{
	IUnknown* punkCloned = ::CreateTypedObject(szTypeLightObject);
	CopyCalcVals(GetControllingUnknown(), punkCloned);
	return punkCloned;
}

bool CMeasureObjectLight::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	if (!CDataObjectBase::SerializeObject(ar, pparams))
		return false;

	if (!CCalcObjectImpl::Serialize(ar))
		return false;

	return true;
}

//10.05.2001
//Need addref child to avoid them killing
void CMeasureObjectLight::OnAddChild(IUnknown *punkNewChild, POSITION pos)
{
	//[Max & Monster]
//	if( !m_punkNamedData && m_pParent )
//		m_pParent->GetData( &m_punkNamedData );

	if( punkNewChild )
		punkNewChild->AddRef();

	if( !m_punkNamedData && m_pParent )
		m_pParent->GetData( &m_punkNamedData );

	if( m_punkNamedData /*&& !m_bLockUpdate*/ )
		::FireEventNotify( m_punkNamedData, szEventChangeObjectList, GetControllingUnknown(), punkNewChild, cncAdd );

	NotifyObject(GetControllingUnknown(),0,GetControllingUnknown());
	ParentUpdate();
}

void CMeasureObjectLight::OnDeleteChild(IUnknown *punkChildToDelete, POSITION pos)
{
	if( !m_punkNamedData && m_pParent )
		m_pParent->GetData( &m_punkNamedData );

	if( m_punkNamedData /*&& !m_bLockUpdate*/ )
		::FireEventNotify( m_punkNamedData, szEventChangeObjectList, GetControllingUnknown(), punkChildToDelete, cncRemove );

	if( punkChildToDelete )
		punkChildToDelete->Release();

	NotifyObject(GetControllingUnknown(),1,GetControllingUnknown());
	ParentUpdate();
}


/*void CMeasureObjectLight::OnAddChild(IUnknown *punkNewChild, POSITION pos)
{
	punkNewChild->AddRef();
//	m_posActiveChild = pos;

	ICalcObjectPtr			ptrC = GetControllingUnknown();
	IMeasureObjectPtr		ptrMeas = GetControllingUnknown();

	IManualMeasureObjectPtr	ptrMM = punkNewChild;
	IManualMeasParamMgrPtr  ptrMan = m_pParent;
	IMeasParamGroup2Ptr     ptrGroup = m_pParent;

//	com_call GetChildByParam(long lParamKey, IUnknown **ppunkChild) = 0;

	if (ptrMan != 0 && ptrGroup != 0 && ptrMeas != 0 && ptrMM != 0)
	{
		long lParamKey = -1;
		long lNewParamKey = -1;
		DWORD dwParamType = 0;

		// get manual_meas_obj param
		if (SUCCEEDED(ptrMM->GetParamInfo(&lParamKey, &dwParamType)) && lParamKey != -1)
		{
			// try to find pos by param_key
			long lParamPos = 0;
			ptrGroup->GetPosByKey(lParamKey, &lParamPos);

			if (lParamPos) // needed param found
			{
				DWORD dwFlag = 0;
				DWORD dwType = 0;
				// get param info				
				if (SUCCEEDED(ptrGroup->GetParamInfo2(lParamPos, &lNewParamKey, 0, 0, &dwFlag, &dwType, 0)))
				{
					// if found such param
					ASSERT(lNewParamKey == lParamKey);
					if (dwType == dwParamType && (dwFlag & CCalcParamInfo::efManual) == CCalcParamInfo::efManual)
					{
						// param is same as need
						IUnknownPtr sptrU;
						ptrMeas->GetChildByParam(lNewParamKey, &sptrU);
						if (sptrU != 0 && ::GetObjectKey(sptrU) != ::GetObjectKey(ptrMM))
						{
							// if param is busy 
							lNewParamKey = -1;
						}
					}
					else
						lNewParamKey = -1;
				}
			}
			// if param is not found 
			if (lNewParamKey == -1)
			{
				//  try to find appropriate param
				ptrGroup->GetFirstPos(&lParamPos);
				while (lParamPos)
				{
					DWORD dwFlag = 0;
					DWORD dwType = 0;
					
					if (SUCCEEDED(ptrGroup->GetParamInfo2(lParamPos, &lNewParamKey, 0, 0, &dwFlag, &dwType, 0)))
					{
						if (dwType == dwParamType && (dwFlag & CCalcParamInfo::efManual) == CCalcParamInfo::efManual)
						{
							// if found param is same as need
							IUnknownPtr sptrU;
							if (FAILED(ptrMeas->GetChildByParam(lNewParamKey, &sptrU)) || sptrU == 0 ||
								::GetObjectKey(sptrU) != ::GetObjectKey(ptrMM))
								// we found needed param -> so break;
								break;
						}
					}
					// set new paramkey to -1 and advance to next param
					lNewParamKey = -1;
					ptrGroup->GetNextPos(&lParamPos);
				}
			}
			// if param is not found yet
			if (lNewParamKey == -1)
			{
				// create new param
				if (SUCCEEDED(ptrMan->CreateNewParam(dwParamType, &lNewParamKey)) && lNewParamKey != -1)
				{
					BSTR bName = 0;
					BSTR bFormat = 0;
					BSTR bAction = 0;
					DWORD dwFlag = 0;
					// get default attributes fro new param
					HRESULT hr = ptrMan->GetParamTypeInfo(dwParamType, &bName, &bFormat, &dwFlag, &bAction);
					_bstr_t bstrN(bName, false);
					_bstr_t bstrF(bFormat, false);
					_bstr_t bstrA(bAction, false);

					dwFlag |= CCalcParamInfo::efManual|CCalcParamInfo::efEnable;

					if (SUCCEEDED(hr))
					{
						// insert new param
						if (FAILED(ptrGroup->InsertParam(lNewParamKey, bstrN, bstrF, dwFlag, dwParamType, bstrA, -1)))
						{
							ptrMan->FreeParam(lNewParamKey);
							lNewParamKey = -1;
						}
					}
					else
					{
						ptrMan->FreeParam(lNewParamKey);
						lNewParamKey = -1;
					}
				}
			}
			
			// if new param
			if (lNewParamKey != -1)
			{
				if (lNewParamKey != lParamKey)
					ptrMM->SetParamInfo(&lNewParamKey, 0);

				double fVal = 0;
				if (SUCCEEDED(ptrMM->CalcValue(&fVal)))
					ptrC->SetValue(lNewParamKey, fVal);
				else
					ptrC->RemoveValue(lNewParamKey);
			}
		}
	}

	ParentUpdate();
}

void CMeasureObjectLight::OnDeleteChild(IUnknown *punkChildToDelete, POSITION pos)
{
	IManualMeasureObjectPtr sptrMM = punkChildToDelete;
	ICalcObjectPtr sptrC = GetControllingUnknown();
	if (sptrC != 0 && sptrMM != 0)
	{
		long lParamKey = -1;
		if (SUCCEEDED(sptrMM->GetParamInfo(&lParamKey, 0)) && lParamKey != -1)
			sptrC->RemoveValue(lParamKey);
	}

	punkChildToDelete->Release();
	ParentUpdate();
}*/

void CMeasureObjectLight::ParentUpdate()
{
	IDataObjectListPtr ptrParent = m_pParent;
	if (ptrParent)ptrParent->UpdateObject( GetControllingUnknown() );
}

DWORD CMeasureObjectLight::GetNamedObjectFlags()
{
	DWORD	dwFlags = nofNormal;
	dwFlags |= nofStoreByParent;
	dwFlags &= ~nofHasHost;

	return dwFlags;
}


BEGIN_DISPATCH_MAP(CMeasureObjectLight, CDataObjectBase)
	//{{AFX_DISPATCH_MAP(CMeasureObjectLight)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		DISP_FUNCTION(CMeasureObjectLight, "GetValue", GetValueDisp, VT_R8, VTS_I4)
		DISP_FUNCTION(CMeasureObjectLight, "SetValue", SetValueDisp, VT_EMPTY, VTS_I4 VTS_R8)
		DISP_FUNCTION(CMeasureObjectLight, "GetValueInUnits", GetValueInUnitsDisp, VT_R8, VTS_I4)
		DISP_FUNCTION(CMeasureObjectLight, "SetValueInUnits", SetValueInUnitsDisp, VT_EMPTY, VTS_I4 VTS_R8)
		DISP_FUNCTION(CMeasureObjectLight, "GetValuesCount", GetValuesCount, VT_I4, VTS_NONE)
		DISP_FUNCTION(CMeasureObjectLight, "GetClass", GetClassDisp, VT_I4, VTS_BSTR)
		DISP_FUNCTION(CMeasureObjectLight, "SetClass", SetClassDisp, VT_EMPTY, VTS_I4 VTS_BSTR)
		DISP_FUNCTION(CMeasureObjectLight, "GetValueEx", GetValueExDisp, VT_R8, VTS_I4 VTS_I4 VTS_I4)
		DISP_FUNCTION(CMeasureObjectLight, "SetValueEx", SetValueExDisp, VT_EMPTY, VTS_I4 VTS_I4 VTS_I4 VTS_R8)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

double CMeasureObjectLight::GetValueDisp(long nKey)
{
	double fValue = 0;
	GetValue(nKey, &fValue);
	return fValue;
}

double CMeasureObjectLight::GetValueInUnitsDisp(long nKey)
{
	double fValue = 0;
	GetValue(nKey, &fValue);
	ICalcObjectContainerPtr ptrParent = m_pParent;
	if (ptrParent)
	{
		ParameterContainer	*pi = 0;
		ptrParent->ParamDefByKey(nKey, &pi);
		if(pi!=0) fValue *= pi->pDescr->fCoeffToUnits;
	}

	return fValue;
}

void CMeasureObjectLight::SetValueInUnitsDisp(long nKey, double fValue)
{
	ICalcObjectContainerPtr ptrParent = m_pParent;
	if (ptrParent)
	{
		ParameterContainer	*pi = 0;
		ptrParent->ParamDefByKey(nKey, &pi);
		if(pi!=0) fValue /= max(pi->pDescr->fCoeffToUnits, 1e-30);
	}
	SetValue(nKey, fValue);
}

void CMeasureObjectLight::SetValueDisp(long nKey, double fValue)
{
	SetValue(nKey, fValue);
}

double CMeasureObjectLight::GetValueExDisp(long nKey, long lPane, long lPhase)
{
	ParameterAdditionalInfo add_info;
	add_info.byPane = (byte)min(max(lPane,0),255);
	add_info.byPhase = (byte)min(max(lPhase,0),255);
	double dValue = 0.;
	GetValueEx(nKey, &add_info, &dValue);
	return dValue;
}

void CMeasureObjectLight::SetValueExDisp(long nKey, long lPane, long lPhase, double fValue)
{
	ParameterAdditionalInfo add_info;
	add_info.byPane = (byte)min(max(lPane,0),255);
	add_info.byPhase = (byte)min(max(lPhase,0),255);
	SetValueEx(nKey, &add_info, fValue);
}


long CMeasureObjectLight::GetClassDisp(LPCTSTR strClassifierName)
{
	_bstr_t s=strClassifierName;
	return get_object_class(ICalcObjectPtr(GetControllingUnknown()), s);
}

void CMeasureObjectLight::SetClassDisp(long lClass, LPCTSTR strClassifierName)
{
	_bstr_t s=strClassifierName;
	INamedPropBagPtr bag = GetControllingUnknown();
	set_object_class(ICalcObjectPtr(GetControllingUnknown()), lClass, s);
}

long CMeasureObjectLight::GetValuesCount()
{
	return m_ValueList.GetCount();
}




/////////////////////////////////////////////////////////////////////////////////////////
//CMeasureObject

BEGIN_DISPATCH_MAP(CMeasureObject, CMeasureObjectLight)
	//{{AFX_DISPATCH_MAP(CMeasureObject)
		DISP_FUNCTION(CMeasureObjectLight, "GetImage", GetImageDisp, VT_DISPATCH, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

LPDISPATCH CMeasureObject::GetImageDisp()
{
	if (m_image == NULL)
		return 0;
	IDispatch* pdisp = NULL;
	m_image->QueryInterface(IID_IDispatch, (void**)&pdisp);		

	return pdisp;
}

HRESULT CMeasureObject::XRect::SetRect(RECT rc)
{
	METHOD_PROLOGUE_EX(CMeasureObject, Rect);
	IRectObjectPtr	ptrO(pThis->m_image);
	if( ptrO == 0 )return S_FALSE;
	return ptrO->SetRect( rc );
}

HRESULT CMeasureObject::XRect::GetRect(RECT* prc)
{
	METHOD_PROLOGUE_EX(CMeasureObject, Rect);
	IRectObjectPtr	ptrO(pThis->m_image);
	if( ptrO == 0 )return S_FALSE;
	return ptrO->GetRect( prc );
}

HRESULT CMeasureObject::XRect::HitTest( POINT	point, long *plHitTest )
{
	METHOD_PROLOGUE_EX(CMeasureObject, Rect);
	IRectObjectPtr	ptrO(pThis->m_image);
	if( ptrO == 0 )return S_FALSE;
	return ptrO->HitTest( point, plHitTest );
}

HRESULT CMeasureObject::XRect::Move( POINT point )
{
	METHOD_PROLOGUE_EX(CMeasureObject, Rect);
	IRectObjectPtr	ptrO(pThis->m_image);
	if( ptrO == 0 )return S_FALSE;
	//RECT rect;
	//ptrO->GetRect(&rect);
	//INamedDataObject2Ptr nd(pThis);
	//long pos;
	//nd->GetFirstChildPosition( &pos );
	//while(pos)
	//{
	//	IUnknown* unk;
	//	nd->GetNextChild( &pos, &unk );
	//	if(CheckInterface(unk, IID_ICompositeSupport))
	//	{
	//		IRectObjectPtr r(unk);
	//		if(r!=0)
	//		{
	//			RECT rCh;
	//			r->GetRect(&rCh);
	//			POINT dP = {point.x + rCh.left-rect.left, point.y + rCh.top-rect.top};
	//			r->Move( dP );
	//			
	//		}
	//	}
	//	if(unk) unk->Release();
	//}
	//OutputDebugStr("moved\n");
	return ptrO->Move( point );
}

HRESULT CMeasureObject::XMeasObj::SetImage( IUnknown *punkImage )
{
	METHOD_PROLOGUE_EX(CMeasureObject, MeasObj);
	pThis->m_image = punkImage;

	if( punkImage  )
	{
		sptrINamedDataObject2	sptrN( punkImage );
		sptrN->SetUsedFlag( true );
		sptrN->SetHostedFlag( true );
	}

	return S_OK;
}

HRESULT CMeasureObject::XMeasObj::GetImage( IUnknown **ppunkImage )
{
	METHOD_PROLOGUE_EX(CMeasureObject, MeasObj);
	*ppunkImage = pThis->m_image;

	if( *ppunkImage )
		(*ppunkImage)->AddRef();

	return S_OK;
}

HRESULT CMeasureObject::XMeasObj::UpdateParent()
{
	METHOD_PROLOGUE_EX(CMeasureObject, MeasObj)
	{
		pThis->ParentUpdate();
		return S_OK;
	}
}

HRESULT CMeasureObject::XMeasObj::GetChildByParam(long lParamKey, IUnknown **ppunkChild)
{
	METHOD_PROLOGUE_EX(CMeasureObject, MeasObj)
	{
		if (!ppunkChild || lParamKey == -1)
			return E_INVALIDARG;

		*ppunkChild = pThis->GetObjectByParamKey(lParamKey);
		return S_OK;
	}
}

HRESULT CMeasureObject::XMeasObj::SetDrawingNumber(long   nNum)
{
	METHOD_PROLOGUE_EX(CMeasureObject, MeasObj)
	{
		pThis->m_nDrawingNumber = nNum;
		return S_OK;
	}
}

HRESULT CMeasureObject::XMeasObj::GetDrawingNumber(long* pnNum)
{
	METHOD_PROLOGUE_EX(CMeasureObject, MeasObj)
	{
		if (!pnNum)
			return E_INVALIDARG;

		*pnNum = pThis->m_nDrawingNumber;
		return S_OK;
	}
}

HRESULT CMeasureObject::XMeasObj::SetNumberPos( POINT point )
{
	METHOD_PROLOGUE_EX(CMeasureObject, MeasObj)
	{
		pThis->m_point_num = point;
		return S_OK;
	}
}
HRESULT CMeasureObject::XMeasObj::GetNumberPos( POINT *ppoint )
{
	METHOD_PROLOGUE_EX(CMeasureObject, MeasObj)
	{
		*ppoint = pThis->m_point_num;
		return S_OK;
	}
}


HRESULT CMeasureObject::XMeasObj::SetZOrder( int nZOrder )
{
	METHOD_PROLOGUE_EX(CMeasureObject, MeasObj)
	{
		pThis->m_nZOrder = nZOrder;
		return S_OK;
	}
}

HRESULT CMeasureObject::XMeasObj::GetZOrder( int *pnZOrder )
{
	METHOD_PROLOGUE_EX(CMeasureObject, MeasObj)
	{
		*pnZOrder = pThis->m_nZOrder;
		return S_OK;
	}
}
HRESULT CMeasureObject::XComposite::IsSyncronized(long* flag)
{
	METHOD_PROLOGUE_EX(CMeasureObject, Composite)
	{
		*flag = pThis->m_lSync;
		return S_OK;
	}	
}
HRESULT CMeasureObject::XComposite::SetSyncronizedFlag(long flag)
{
	METHOD_PROLOGUE_EX(CMeasureObject, Composite)
	{
		pThis->m_lSync = flag;
		return S_OK;
	}
}


HRESULT CMeasureObject::XComposite::GetLevel(long* lvl)
{
	METHOD_PROLOGUE_EX(CMeasureObject, Composite)
	*lvl = pThis->m_lLevel;
	return S_OK;
}
HRESULT CMeasureObject::XComposite::SetLevel(long lvl)
{
	METHOD_PROLOGUE_EX(CMeasureObject, Composite)
	pThis->m_lLevel = lvl;
	pThis->m_nZOrder = lvl;
	if(lvl==0)
	{
		INamedPropBagPtr bag(pThis->GetControllingUnknown());
		if(bag!=0)
		{

			bstr_t flag("MainObject");
			bag->SetProperty(flag, _variant_t(1L));
		}
	}


	return S_OK;
}
HRESULT CMeasureObject::XComposite::GetPixCount(long *pixCount)
{
	METHOD_PROLOGUE_EX(CMeasureObject, Composite)
	*pixCount = pThis->m_lPixCount;
	return S_OK;
}
HRESULT CMeasureObject::XComposite::SetPixCount(long pixCount)
{
	METHOD_PROLOGUE_EX(CMeasureObject, Composite)
	pThis->m_lPixCount = pixCount;
	return S_OK;	
}
HRESULT CMeasureObject::XComposite::GetParentKey(GUID* key)
{
	METHOD_PROLOGUE_EX(CMeasureObject, Composite)
	*key = pThis->m_guidParentKey;
	return S_OK;	
}
HRESULT CMeasureObject::XComposite::SetParentKey(const GUID key)
{
	METHOD_PROLOGUE_EX(CMeasureObject, Composite)
	pThis->m_guidParentKey = key ;
	return S_OK;
}
HRESULT CMeasureObject::XComposite::GetTreeKey(GUID* key)
{
	METHOD_PROLOGUE_EX(CMeasureObject, Composite)
	if(pThis->m_guidTreeKey == GUID_NULL)
	{
		::CoCreateGuid(&pThis->m_guidTreeKey); 
	}
	*key = pThis->m_guidTreeKey;
	return S_OK;
}
HRESULT CMeasureObject::XComposite::SetTreeKey(const GUID key)
{
	METHOD_PROLOGUE_EX(CMeasureObject, Composite)
	pThis->m_guidTreeKey = key ;
	return S_OK;
}
HRESULT CMeasureObject::XComposite::CopyAllCompositeData(IUnknown* destObject)
{
	METHOD_PROLOGUE_EX(CMeasureObject, Composite)
	ICompositeSupportPtr cs(destObject);
	if(cs==0) return E_INVALIDARG;

	cs->SetLevel(pThis->m_lLevel);
	cs->SetParentKey(pThis->m_guidParentKey);
	cs->SetTreeKey(pThis->m_guidTreeKey);

	fill_rowmask(destObject);

	set_composite_class(destObject, get_composite_class(pThis->GetControllingUnknown()));
	return S_OK;
}

HRESULT CMeasureObject::XComposite::GetGrandParent(IUnknown** unkParent)
{
	METHOD_PROLOGUE_EX(CMeasureObject, Composite)
	*unkParent =0;
	INamedDataObject2Ptr nd(pThis->GetControllingUnknown());
	IUnknown* list;
	nd->GetParent(&list);
    ICompositeObjectPtr co(list);
	if(list) list->Release();
	if(co!=0)
	{
		CTreeNotion* tn;
		co->GetTreeNotion((void**)&tn);
		CTreeNode* treeNode = tn->Find(pThis->GetControllingUnknown());
		
		if(treeNode)
		{
			CTreeNode* parent = treeNode;
			while(parent)
			{
				treeNode = parent;
				parent = treeNode->GetParent();				
			}
			*unkParent = treeNode->GetData();
			(*unkParent)->AddRef();
		}
	}
	return S_OK;
}

IUnknown * CMeasureObject::GetObjectByParamKey(long lParamKey)
{
	if (lParamKey == -1)
		return 0;

	IUnknown * punk = 0;
	POSITION pos = GetFirstChildPosition();

	while (pos)
	{
		punk = GetNextChild(pos);
		IManualMeasureObjectPtr sptrChild = punk;
		
		long lKey = -1;
		if (sptrChild != 0 && SUCCEEDED(sptrChild->GetParamInfo(&lKey, 0)) && lKey == lParamKey)
		{
			punk->AddRef();
			return punk;
		}

		punk = 0;

	}
	return 0;
}

static long dd = 0;

CMeasureObject::CMeasureObject()
{
	m_posActiveChild = 0;

	//init clipboard support
	m_arrAvailFormats.Add(CF_DIB);
	m_bSupportPaste = false;
	m_point_num = CPoint( 0, 0 );

	m_nDrawingNumber = 0;
	m_nZOrder = 0;
	m_lLevel = 0;

	m_sName.Format("Object%d", dd++);
	m_guidTreeKey = GUID_NULL;
	m_guidParentKey = GUID_NULL;
	m_lSync = 0;
	m_lPixCount =0;
}

CMeasureObject::~CMeasureObject()
{
}

IUnknown* CMeasureObject::Clone()
{
	IUnknown* punkCloned = ::CreateTypedObject(szTypeObject);
	CopyCalcVals(GetControllingUnknown(), punkCloned);
	IMeasureObjectPtr sptrMeasCloned(punkCloned);
	if(sptrMeasCloned != 0 && m_image != 0)
	{
		// A.M. BT4477 прим 2.
		CImageWrp imgNew;
		imgNew.CreateCompatibleImage(m_image, true);
		if (!m_image.IsVirtual()) // A.M. BT5284
		{
			int cx = m_image.GetWidth();
			int cy = m_image.GetHeight();
			for (int y = 0; y < cy; y++)
			{
				int nPanes = GetImagePaneCount(m_image);
				for (int i = 0; i < nPanes; i++)
				{
					color *prowSrc = m_image.GetRow(y, i);
					color *prowDst = imgNew.GetRow(y, i);
					memcpy(prowDst, prowSrc, cx*sizeof(color));
				}
			}
		}
		sptrMeasCloned->SetImage(imgNew);
	}
	IMeasureObject2Ptr sptrMeas2Cloned(punkCloned);
	if (sptrMeas2Cloned != 0)
		sptrMeas2Cloned->SetZOrder(m_nZOrder);
	INamedObject2Ptr sptrNamed(GetControllingUnknown());
	INamedObject2Ptr sptrNamedCloned(punkCloned);
	if(sptrNamed!=0 && sptrNamedCloned!=0)
	{
		_bstr_t bstr;
		sptrNamed->GetName(bstr.GetAddress());
		sptrNamedCloned->SetName(bstr);
	}
	INamedPropBagPtr ptrSrcBag(GetControllingUnknown());
	INamedPropBagPtr ptrDstBag(punkCloned);
	if(ptrSrcBag!=0 && ptrDstBag!=0)
	{
		long lPos = 0;

		ptrSrcBag->GetFirstPropertyPos( &lPos );

		while( lPos != 0 )
		{
			_bstr_t varName;
			_variant_t varValue;
			ptrSrcBag->GetNextProperty( varName.GetAddress(), &varValue, &lPos );
			ptrDstBag->SetProperty( varName, varValue );
		}
	}

	CObjectListWrp objectsSrc(GetControllingUnknown());
	CObjectListWrp objects(punkCloned);

	if(objects != 0)
	{
		POSITION pos = objectsSrc.GetFirstObjectPosition();
		while(pos)
		{
			IUnknown* punkObject = objectsSrc.GetNextObject(pos);
			IClonableObjectPtr sptrClone(punkObject);
			if(punkObject)
				punkObject->Release();
			if(sptrClone != 0)
			{
				IUnknown* punkCloned = 0;
				sptrClone->Clone(&punkCloned);
				INamedDataObject2Ptr ptrObject(punkCloned);
				if(ptrObject != 0)
					ptrObject->SetParent( sptrMeasCloned, 0 );
				if(punkCloned)
					punkCloned->Release();
			}
		}
	}
	//Kir-- Composite support
	ICompositeSupportPtr cs(punkCloned);
	if(cs) 
	{
		cs->SetTreeKey(m_guidTreeKey);
		cs->SetLevel(m_lLevel);
		cs->SetPixCount(m_lPixCount);
		cs->SetParentKey(m_guidParentKey);
	}
	return punkCloned;
}


DWORD CMeasureObject::GetNamedObjectFlags()
{
	DWORD	dwFlags = nofNormal;

	sptrINamedDataObject2	sptrN( m_image );
	if (sptrN)	
		sptrN->GetObjectFlags( &dwFlags );

	dwFlags |= nofStoreByParent;
	dwFlags &= ~nofHasHost;

	return dwFlags;
}


bool CMeasureObject::ExchangeDataWithParent(DWORD dwFlags)
{
	sptrINamedDataObject2	sptrN(m_image);

	if( sptrN != 0 )
		return sptrN->StoreData(dwFlags) == S_OK;
	return true;
}

bool CMeasureObject::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	//PROFILE_TEST( "CMeasureObject::SerializeObject" )

	if (!CMeasureObjectLight::SerializeObject(ar, pparams))
		return false;	

	if (ar.IsStoring())
	{
		long lVersion = 7;

		ar << lVersion;	//

		BOOL bImageExists = ::CheckInterface(m_image, IID_INamedDataObject2) == true;
		ar << bImageExists;

		if (bImageExists)
		{
			GuidKey ParentKey = GetParentKey( m_image );
			long lFlags = GetObjectFlags( m_image );
			ar << ParentKey;
			ar << lFlags;

			unsigned old_flags = pparams->flags;
			pparams->flags |= sf_DontCheckSignatureInBig;
			::StoreDataObjectToArchive( ar, m_image, pparams );
			pparams->flags |= old_flags;
		}

		ar << m_nDrawingNumber;
		ar << m_nZOrder;
		ar << m_lLevel;
		ar.Write(&m_guidParentKey, sizeof(GUID));
		ar.Write(&m_guidTreeKey, sizeof(GUID));
		ar<<m_lPixCount;
	}
	else
	{
		long lVersion = 1;
		ar >> lVersion;

		if (lVersion > 2)
		{
			BOOL bImageExists = false;
			ar >> bImageExists;
			if (bImageExists)
			{
				long  lFlags;
				GuidKey	lParentKey;
				ar >> lParentKey;
				ar >> lFlags;

				IUnknown * punk = 0;
				{
					//PROFILE_TEST( "CMeasureObject::LoadImage" )
					unsigned old_flags = pparams->flags;
					pparams->flags |= sf_DontCheckSignatureInBig;
					punk = ::LoadDataObjectFromArchive(ar, pparams);
					pparams->flags |= old_flags;
				}

				/*if (lParentKey != -1)
				{
					sptrINamedDataObject2 sptrN(punk);
					IUnknown * punkParent = ::FindObjectByKeyEx(lParentKey);

					if (punkParent)
					{
						DWORD dwFlags = 0;
						sptrN->GetObjectFlags(&dwFlags);
						
						sptrN->SetParent(punkParent, ((dwFlags & nofHasData) != nofHasData) ? sdfAttachParentData : 0);
						punkParent->Release();
					}
				}*/
				m_xMeasObj.SetImage(punk);
				if (punk)
					punk->Release();
			}
		}

		if(lVersion >= 5)
			ar >> m_nDrawingNumber;

		if(lVersion >= 6 )
			ar >> m_nZOrder;
		
		if(lVersion >= 7 )
		{
			ar >> m_lLevel;
			ar.Read(&m_guidParentKey, sizeof(GUID));
			ar.Read(&m_guidTreeKey, sizeof(GUID));
			ar>>m_lPixCount;
		}
	}

	return true;
}


bool CMeasureObject::Copy(UINT nFormat, HANDLE* pHandle)
{
	if(!pHandle)
		return false;
	if(nFormat == CF_DIB)
	{
		//IClipboardPtr sptrClip(m_image);
		//ASSERT(sptrClip != 0);

		//sptrClip->Copy(CF_DIB, pHandle);
		
		BITMAPINFOHEADER	*pbi = m_image.ExtractDIBBits(m_image.GetRect());

		HGLOBAL	hGlobal = ::GlobalAlloc( GMEM_MOVEABLE|GMEM_DDESHARE, pbi->biSizeImage+pbi->biSize );
		void	*pdata = ::GlobalLock( hGlobal );

		ASSERT( pdata );

		::memcpy( pdata, pbi, pbi->biSizeImage+pbi->biSize );

		delete pbi;

		::GlobalUnlock( hGlobal );

		*pHandle = hGlobal;
		
	}

	return true;
}

HRESULT CMeasureObject::XCalibr::GetCalibration( double *pfPixelPerMeter, GUID *pguidC )
{
	METHOD_PROLOGUE_EX(CMeasureObject, Calibr);
	ICalibrPtr	ptrC(pThis->m_image);
	if( ptrC == 0 )
	{
		::GetDefaultCalibration( pfPixelPerMeter, pguidC );
		return S_OK;
	}
	return ptrC->GetCalibration( pfPixelPerMeter, pguidC );
}

HRESULT CMeasureObject::XCalibr::SetCalibration( double fPixelPerMeter, GUID *guidC )
{
	METHOD_PROLOGUE_EX(CMeasureObject, Calibr);
	ICalibrPtr	ptrC(pThis->m_image);
	if( ptrC == 0 )
		return S_OK;

	return ptrC->SetCalibration( fPixelPerMeter, guidC );
}


///////////////////////////////////////////
CCalcObjectContainerImpl::CCalcObjectContainerImpl()
{
	m_lCurrentPosition = 0;
}
CCalcObjectContainerImpl::~CCalcObjectContainerImpl()
{
	DeInit();
}
bool CCalcObjectContainerImpl::Serialize( CStreamEx &ar )
{
	if( ar.IsLoading() ) 
	{
		m_lCurrentPosition = 0;
		DeInit();
	}
	else
	{
	}
	return true;
}

void CCalcObjectContainerImpl::DeInit()
{
	POSITION	pos = m_params.GetHeadPosition();
	while( pos )
	{
		ParameterContainer	*pdescr = m_params.GetNext( pos );

		_free_param_c( pdescr );
	}
	m_params.RemoveAll();
}

POSITION CCalcObjectContainerImpl::AddParameter(ParameterContainer *pcNew)
{
	ParameterDescriptor *pdescrNew = pcNew->pDescr;
	if (pdescrNew && pdescrNew->lTableOrder > 0)
	{
		POSITION pos = m_params.GetHeadPosition();
		while (pos)
		{
			POSITION posPrev = pos;
			ParameterContainer	*pc = m_params.GetNext(pos);
			if (pc->pDescr && (pc->pDescr->lTableOrder == 0 ||
				pdescrNew->lTableOrder < pc->pDescr->lTableOrder))
				return m_params.InsertBefore(posPrev, pcNew);
		}
	}
	return m_params.AddTail(pcNew);
}


HRESULT CCalcObjectContainerImpl::XCalcCntr::GetCurentPosition(LONG_PTR *plpos, long *plKey)
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);
	if( plpos )
		*plpos = pThis->m_lCurrentPosition;
	if( plKey )
	{
		*plKey = -1;
		if( pThis->m_lCurrentPosition != 0 ) 
		{
			ParameterContainer *pi = (ParameterContainer*)pThis->m_params.GetAt( (POSITION&)pThis->m_lCurrentPosition );
			ASSERT( pi->pDescr );
			*plKey = pi->pDescr->lKey;			
		}
	}
	return S_OK;
}
HRESULT CCalcObjectContainerImpl::XCalcCntr::SetCurentPosition(LONG_PTR lpos)
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);

	if( pThis->m_lCurrentPosition == lpos )
		return S_OK;
	pThis->m_lCurrentPosition = lpos;
	
	long lKey = 0;

	if( lpos > 0 )
	{
		ParameterContainer *pi = (ParameterContainer*)pThis->m_params.GetAt( (POSITION&)lpos );
		if( pi && pi->pDescr )
			lKey = pi->pDescr->lKey;
		else
			ASSERT( FALSE );
	}

	pThis->container_notify( cncActivateParameter, lKey );
	
	return S_OK;
}
HRESULT CCalcObjectContainerImpl::XCalcCntr::Move( long lDirection )
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);
	return S_OK;
}

HRESULT CCalcObjectContainerImpl::XCalcCntr::GetFirstParameterPos(LONG_PTR *plpos)
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);
	*plpos = (LONG_PTR)pThis->m_params.GetHeadPosition();
	return S_OK;
}

HRESULT CCalcObjectContainerImpl::XCalcCntr::GetLastParameterPos(LONG_PTR *plpos)
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);
	*plpos = (LONG_PTR)pThis->m_params.GetTailPosition();
	return S_OK;
}

HRESULT CCalcObjectContainerImpl::XCalcCntr::GetNextParameter(LONG_PTR *plpos, struct ParameterContainer **ppContainer)
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);
	ParameterContainer *pi = (ParameterContainer*)pThis->m_params.GetNext( (POSITION&)*plpos );
	if( ppContainer )*ppContainer = pi;
	return S_OK;
}

HRESULT CCalcObjectContainerImpl::XCalcCntr::GetPrevParameter(LONG_PTR *plpos, struct ParameterContainer **ppContainer)
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);
	ParameterContainer *pi = (ParameterContainer*)pThis->m_params.GetPrev( (POSITION&)*plpos );
	if( ppContainer )*ppContainer = pi;
	return S_OK;
}

HRESULT CCalcObjectContainerImpl::XCalcCntr::MoveParameterAfter(LONG_PTR lpos, struct ParameterContainer *p)
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);
	pThis->m_params.RemoveAt( (POSITION)p->lpos );
	if( lpos == 0 )p->lpos = (LONG_PTR)pThis->m_params.AddHead( p );
	else p->lpos = (LONG_PTR)pThis->m_params.InsertAfter((POSITION)lpos, p);
	pThis->container_notify( cncMoveParameter, p->pDescr->lKey );
	return S_OK;
}

HRESULT CCalcObjectContainerImpl::XCalcCntr::DefineParameter(long lKey, enum ParamType type, IMeasParamGroup *pGroup, LONG_PTR *plpos)
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);

	if( lKey == -1 )//wanna have uniq key
	{
		long lUniqKey = MANUAL_PARAMS_FIRST_KEY;
		POSITION pos = pThis->m_params.GetHeadPosition();
		while( pos )
		{
			ParameterContainer	*p = pThis->m_params.GetNext( pos );
			if( MANUAL_PARAMS_FIRST_KEY <= p->pDescr->lKey 
				&& p->pDescr->lKey <= MANUAL_PARAMS_LAST_KEY)
			{
				lUniqKey = max( p->pDescr->lKey+1, lUniqKey );			
			}
		}

		lKey = lUniqKey;
	}

	ParameterContainer	*pi = pThis->find( lKey );
	if( pi )return S_FALSE;

	pi = new  ParameterContainer;
	::ZeroMemory( pi, sizeof( ParameterContainer ) );

	pi->cbSize = sizeof( ParameterContainer );
	pi->pGroup = pGroup;

	pi->cxWidth = GetValueInt( GetAppUnknown(), "\\measurement", "DefWidth", 70 );
	pi->bstrAction = 0;
	pi->type = type;

	ASSERT( pi->pGroup );

	if( pi->pGroup )
	{
		pi->pGroup->AddRef();
		LONG_PTR	lposG = 0;
		pGroup->GetPosByKey( lKey, &lposG );
		if( !lposG )
		{
			// it is unique parameter
			pi->pDescr = new ParameterDescriptor;
			::ZeroMemory( pi->pDescr, sizeof( ParameterDescriptor ) );

			pi->pDescr->cbSize = sizeof( *pi->pDescr );

			CString	strName;
			if( type == etUndefined )strName.Format( IDS_TYPE_UNDEF, lKey-2000+1 );
			else if( type == etLinear )strName.Format( IDS_TYPE_LINEAR, lKey-2000+1 );
			else if( type == etCounter )strName.Format( IDS_TYPE_COUNTER, lKey-2000+1 );
			else if( type == etAngle )strName.Format( IDS_TYPE_ANGLE, lKey-2000+1 );
			pi->pDescr->bstrName = strName.AllocSysString();
			pi->pDescr->bstrUserName = strName.AllocSysString();
			if( type == etUndefined )strName = "%g";
            else if( type == etLinear )
				// [vanek] SBT: 757 - 05.03.2004
				strName = ::GetValueString( ::GetAppUnknown(), sParamsTypesSection "\\" sLinear, sFormat, "%0.2f" );
			else if( type == etCounter )strName = "%0.0f";
			else if( type == etAngle )
				strName = ::GetValueString( ::GetAppUnknown(), sParamsTypesSection "\\" sAngle, sFormat, "%0.1f°" );
			pi->pDescr->bstrDefFormat = strName.AllocSysString();
            pi->pDescr->lKey = lKey;
			pi->pDescr->lEnabled = 1;
			pi->pDescr->pos = 0;
			if( pi->pGroup )pi->pGroup->GetUnit( pi->type, &pi->pDescr->bstrUnit, &pi->pDescr->fCoeffToUnits );
		}
		else
		{
			pi->pGroup->GetNextParam( &lposG, &pi->pDescr );
		}
	}
	
	pi->lpos = (LONG_PTR)pThis->AddParameter(pi);
//	pi->lpos = (LONG_PTR)pThis->m_params.AddTail( pi );

	pThis->container_notify( cncAddParameter, lKey );


	if( plpos )
		*plpos = pi->lpos;


	if( plpos )*plpos = pi->lpos;

	return S_OK;
}

HRESULT CCalcObjectContainerImpl::XCalcCntr::DefineParameterFull( struct ParameterContainer *ps, LONG_PTR *plpos )
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);
	ParameterContainer *p = pThis->find( ps->pDescr->lKey );
	if( p )
	{
		pThis->container_notify( cncChangeParameter, p->pDescr->lKey );
		if( plpos )*plpos = p->lpos;
		return S_OK;
	}

	p = new ParameterContainer;
	p->bstrAction = _bstr_t( ps->bstrAction ).copy();
	p->cbSize = sizeof( *p );
	p->cxWidth = ps->cxWidth;
	p->type = ps->type;
	p->pGroup = ps->pGroup;
	if( p->pGroup )p->pGroup->AddRef();
	p->type = ps->type;
	if( ps->pDescr->pos == 0 )
	{
		p->pDescr = new ParameterDescriptor;
		p->pDescr->cbSize = sizeof( *p->pDescr );
		p->pDescr->bstrName = _bstr_t( ps->pDescr->bstrName ).copy();
		p->pDescr->bstrUserName = _bstr_t( ps->pDescr->bstrUserName ).copy();
		p->pDescr->bstrDefFormat = _bstr_t( ps->pDescr->bstrDefFormat ).copy();
		p->pDescr->lKey = ps->pDescr->lKey;
		p->pDescr->lEnabled = ps->pDescr->lEnabled;
		p->pDescr->pos = 0;
		if( p->pGroup )p->pGroup->GetUnit( p->type, &p->pDescr->bstrUnit, &p->pDescr->fCoeffToUnits );
		p->pDescr->lTableOrder = ps->pDescr->lTableOrder;
	}
	else
		p->pDescr = ps->pDescr;
	p->lpos = (LONG_PTR)pThis->AddParameter(p);
//	p->lpos = (LONG_PTR)pThis->m_params.AddTail( p );
	pThis->container_notify( cncAddParameter, p->pDescr->lKey );

	if( plpos )*plpos = p->lpos;

	return S_OK;
}

HRESULT CCalcObjectContainerImpl::XCalcCntr::RemoveParameter( long lKey )
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);
	ParameterContainer	*p = pThis->find( lKey );
	if( !p )return S_FALSE;

	return RemoveParameterAt( p->lpos );
}

HRESULT CCalcObjectContainerImpl::XCalcCntr::RemoveParameterAt( LONG_PTR lpos )
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);

	if( lpos == pThis->m_lCurrentPosition )
		pThis->m_params.GetPrev( (POSITION&)pThis->m_lCurrentPosition );

	
	ParameterContainer	*p = pThis->m_params.GetAt( (POSITION)lpos );
	long	lKey = p->pDescr->lKey;
	pThis->m_params.RemoveAt( (POSITION)lpos );
	
	if( pThis->m_lCurrentPosition == 0 )
		pThis->m_lCurrentPosition = (long )pThis->m_params.GetHeadPosition();

	pThis->container_notify( cncRemoveParameter, lKey );
	_free_param_c( p );

	return S_OK;
}
HRESULT CCalcObjectContainerImpl::XCalcCntr::Reset()
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);
	pThis->DeInit();
	pThis->container_notify( cncResetParameters, -1 );

	return S_OK;
}

HRESULT CCalcObjectContainerImpl::XCalcCntr::ParamDefByKey( long lKey, struct ParameterContainer * * ppContainer )
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);
	*ppContainer = pThis->find( lKey );
	return *ppContainer?S_OK:S_FALSE;
}

HRESULT CCalcObjectContainerImpl::XCalcCntr::GetParameterCount( long *pnCount )
{
	METHOD_PROLOGUE_BASE(CCalcObjectContainerImpl, CalcCntr);
	*pnCount = pThis->m_params.GetCount();

	long	l = 0;
	POSITION	pos = pThis->m_params.GetHeadPosition();
	while( pos ){pThis->m_params.GetNext( pos );l++;}
	*pnCount = l;

	return S_OK;
}

ParameterContainer	*CCalcObjectContainerImpl::find( long lKey )
{
	POSITION	pos = m_params.GetHeadPosition();
	while( pos )
	{
		ParameterContainer	*p = m_params.GetNext( pos );
		if( p && p->pDescr && p->pDescr->lKey == lKey )
			return p;

	}
	return 0;
}

void CCalcObjectContainerImpl::container_notify( long lCode, long lKey )
{
	IUnknown	*punkData;
	INamedDataObject2	*p = GetList();
	p->GetData( &punkData );
	IDataObjectListPtr	ptr = p;
	BOOL	bLock = false;
	ptr->GetObjectUpdateLock( &bLock );

	if( bLock )return;

	if( punkData )
	{
		struct{long code; long key;}info;
		info.code = lCode;
		info.key = lKey;
		::FireEvent( punkData, szEventChangeObjectList, Unknown(), 0, &info, sizeof(info) );
		punkData->Release();
	}

}

long CMeasureObjectList::GetParametersCount() 
{
	long	n;
	ICalcObjectContainerPtr	ptrC( GetControllingUnknown() );
	ptrC->GetParameterCount( &n );

	return n;
}

LONG_PTR CMeasureObjectList::GetFirstParameterPosition() 
{
	LONG_PTR	lpos = 0;
	ICalcObjectContainerPtr	ptrC( GetControllingUnknown() );
	ptrC->GetFirstParameterPos( &lpos );
	return lpos;
}

LONG_PTR CMeasureObjectList::GetNextParameter( LONG_PTR lpos, VARIANT FAR* refName, VARIANT FAR* refUnits, VARIANT FAR* refKey, VARIANT FAR* refCalibr) 
{
	ICalcObjectContainerPtr	ptrC( GetControllingUnknown() );
	ParameterContainer	*p = 0;
	ptrC->GetNextParameter( &lpos, &p );

	VariantCopy( refName, &_variant_t( p->pDescr->bstrUserName ) );
	VariantCopy( refUnits, &_variant_t( p->pDescr->bstrUnit ) );
	VariantCopy( refKey, &_variant_t( p->pDescr->lKey ) );
	VariantCopy( refCalibr, &_variant_t( p->pDescr->fCoeffToUnits ) );


	return lpos;
}


bool CMeasureObjectList::SetBagProperty( LPCTSTR strName, const VARIANT FAR&var )
{
	INamedPropBagPtr sptrB = GetControllingUnknown();
	
	if( sptrB == 0 )
		return false;

	if( S_OK != sptrB->SetProperty( _bstr_t( strName ), var ) )
		return false;

	return true;
}

VARIANT CMeasureObjectList::GetBagProperty( LPCTSTR strName )
{
	INamedPropBagPtr sptrB = GetControllingUnknown();
	
	_variant_t ret_var;

	if( sptrB == 0 )
		return ret_var;

	if( S_OK != sptrB->GetProperty( _bstr_t( strName ), &ret_var ) )
		return ret_var;

	return ret_var;
}

void CMeasureObjectList::OnSetBaseObject(GuidKey & NewKey)
{
	if (NewKey != INVALID_KEY)
	{
		IUnknownPtr punkImage(FindObjectByBaseKey(NewKey), false);
		if(punkImage==0) punkImage.Attach( FindObjectByKey(NewKey), false);
		 
		IImagePtr image(punkImage);
		if (image != 0)
		{

			_bstr_t bstr_guid = "";
			{
				INumeredObjectPtr	punkNum = image;
				if(0==punkNum) return;

				GuidKey	key;
				punkNum->GetKey( &key );

				BSTR bstr = 0;
				::StringFromCLSID(key,&bstr);
				if( bstr )
				{
					bstr_guid = bstr;
					CoTaskMemFree(bstr); bstr = 0;
				}
			}					

			_bstr_t bstr_image_path = _T("\\" BASE_PARAMS_SECTION "\\" BASE_PARAMS_IMAGE "\\");
			bstr_image_path += bstr_guid;

			int cx = 0, cy = 0;
			image->GetSize( &cx, &cy );
			double	fCalibr = 1.;
			::GetDefaultCalibration( &fCalibr, 0 );
			ICalibrPtr	ptrCalibr = image;
			if( ptrCalibr != 0 )
				ptrCalibr->GetCalibration( &fCalibr, 0 );
			char sz_buf[20];
			sprintf( sz_buf, "%d", BASE_PARAMS_IMAGE_WIDTH );
			::SetValue2(GetControllingUnknown(), bstr_image_path, sz_buf, (long)cx );
			sprintf( sz_buf, "%d", BASE_PARAMS_IMAGE_HEIGHT );
			::SetValue2(GetControllingUnknown(), bstr_image_path, sz_buf, (long)cy );
			sprintf( sz_buf, "%d", BASE_PARAMS_IMAGE_CALIBR );
			::SetValue2(GetControllingUnknown(), bstr_image_path, sz_buf, (double)fCalibr );
		}
	}
}

bool CMeasureObjectList::store_child(IUnknown* unk, int level, CTreeNode* parent)
{
	if(!unk) return false;
	if(!CheckInterface(unk, IID_IMeasureObject)) return false; //не храним объекты, кроме MeasureObject 
	INamedDataObject2Ptr ndo(unk);
	if(ndo==0) return false;
	ICompositeSupportPtr cosupp(unk);
	if(cosupp) cosupp->GetLevel((long*)&level);

	CTreeLevel* tl = m_lObjects.GetLevel(level);
	if(!tl)
	{
		int lvlCount = m_lObjects.AddLevel(GetControllingUnknown());
		/*ASSERT(lvlCount==level+1);*/
		tl = m_lObjects.GetLevel(level);
	}
	CTreeNode* tn = m_lObjects.AddElement(level);
	
	tn->SetData(unk);


	if(tl->GetClass()==-1) 
	{
		int c = get_composite_class(unk);
		tl->SetClass(c);
		this->m_ci.Set(level, c);
	}
	tn->SetParent(parent);
	tn->SetLevel(level);

	ICompositeSupportPtr comSupp(unk);
	if(comSupp)
	{
		long f;
		comSupp->IsSyncronized(&f);
		f|=1;
		comSupp->SetSyncronizedFlag(f);
	}

////////////////////////////////////


	//
	//int t = level;
	//CString str;
	//str.Format("---- Adding element %i to level %i. Elements count %i\n", unk, t, tl->GetCount());
	//OutputDebugStr(str);
	//if(t==3 && tl->GetCount()==28)
	//	__asm nop
///////////////////////////////////

	ndo->SetParent(this->GetControllingUnknown(),0);
	TPOS pos;
	ndo->GetFirstChildPosition( &pos );
	while(pos)
	{
		IUnknown* pUnk;
		ndo->GetNextChild(&pos, &pUnk);
		store_child(pUnk, level+1, tn);
		if(pUnk) pUnk->Release();//объекты в списке не нужны, если они недействительны в NamedData
	}
	return true;
}



void CMeasureObjectList::restore_tree_from_archive(void)
{
	INamedDataObject2Ptr list(GetControllingUnknown());

	m_lObjects.RemoveAll();
	for(int i=0;i<m_binaryCount;i++)
	{
		m_lObjects.AddLevel(GetControllingUnknown());
	}
	if(m_ci.actual())
	{
		for(int i=0;i<m_binaryCount;i++)
		{
			CTreeLevel* tl = m_lObjects.GetLevel(i);
			tl->SetClass(m_ci.Get(i));
		}
	}
	else m_ci.Init(m_binaryCount);
	
	//populate tree levels
	TPOS pos;
	list->GetFirstChildPosition( &pos );
	while(pos)
	{
		IUnknown* objectUnk;
		list->GetNextChild( &pos, &objectUnk );
		ICompositeSupportPtr cs(objectUnk);
		if(objectUnk) objectUnk->Release();
		long l;
		cs->GetLevel(&l);
		ASSERT(l<m_lObjects.GetLevelsCount() && l>=0);
		CTreeNode* tn = m_lObjects.AddElement(l);
		CTreeLevel* tl = m_lObjects.GetLevel(l);
		tn->SetData(objectUnk);
		if(tl->GetClass()==-1) 
		{
			int c = get_composite_class(objectUnk);
			tl->SetClass(c);
			m_ci.Set(l,c);
		}
		tn->SetLevel(l);
	}
	m_ci.actual()=TRUE;
	//assign parents by parents id
	CLevels* lvls = m_lObjects.GetLevels();
	POSITION lp = lvls->GetTailPosition();
	POSITION base = lvls->GetHeadPosition();
	while(lp!=base )
	{
		CTreeLevel* tl = lvls->GetPrev(lp);
		POSITION cp = tl->GetHeadPosition();
		while(cp)
		{
			CTreeNode* tn = tl->GetNext(cp);
			ICompositeSupportPtr cs(tn->GetData());
			GUID pK = GUID_NULL;
			cs->GetParentKey(&pK);
			CTreeNode* parentNode  = m_lObjects.FindElementByKey(pK);
			if(parentNode ==0 && tn->GetLevel()!=0)//потеряшка
			{

				IUnknown* tempUnk = tn->GetData();
				m_lObjects.root_out_branch(tn, false, false);
				INamedDataObject2Ptr ndo(tempUnk);
				ndo->SetParent(0, 0);			
			}
			else
			tn->SetParent(parentNode);
		}
	}
	m_bDirty = FALSE;
	/*RegisterCompositeMeasgroup();*/
	
}
///////////////////////////////////////////
CTreeNotion::CTreeNotion()
{
	cf = GetValueDouble(GetAppUnknown(),_T("Composite"), _T("CoverFactor"),.95);
}
CTreeNotion::~CTreeNotion()
{
	RemoveAll();
}
CTreeNode* CTreeNotion::FindElementByKey(GUID key)
{
	if(key == GUID_NULL) return 0;
	POSITION pos = levels.GetHeadPosition();
	while(pos)
	{
		CTreeLevel* tl = levels.GetNext(pos);
		POSITION p = tl->GetHeadPosition();
		while(p)
		{
			CTreeNode* tn = tl->GetNext(p);
			ICompositeSupportPtr cs(tn->GetData());
			GUID g = GUID_NULL;
			cs->GetTreeKey(&g);
			if(g==key)
			{
				return tn;
			}
		}
	}
	return 0;
}

void CTreeNotion::root_out_branch(CTreeNode* root, bool bAddToDelay, bool bRecursively)
{
	if(root==0) return;
	static int delay;
	delay++;
	


	ICompositeSupportPtr comSupp(root->GetData());
	if(comSupp)
	{
		long flag;
		comSupp->IsSyncronized(&flag);
		flag &= ~1;//объект будет удален из виртуального дерева
		comSupp->SetSyncronizedFlag(flag);


	}

	POSITION pos;
	root->GetFirstChildPosition(pos);
	CTreeNode* parentNode = root->GetParent();
	while(pos)			
	{
		CTreeNode* tn;
		root->GetNextChild(pos, &tn);
		if(tn) 
		{
			if(delay==1 && parentNode!=0 && is_child(parentNode->GetData(), tn->GetData(), cf))
			{
				tn->SetParent(parentNode);

			}
			else
			{
				INamedDataObject2Ptr child(tn->GetData());
				child->SetParent(root->GetData(),0);
				if(bRecursively) root_out_branch(tn, bAddToDelay, bRecursively);
			}
		}
	}


	CTreeLevel* tl = GetLevel(root->GetLevel());
	if(delay==1)
	{
		CTreeNode* pn, *temp;
		temp = pn = root->GetParent();
		while(temp)
		{
			pn = temp;
			temp =  pn->GetParent();
		}


		root->SetParent(0);

		if(pn!=0)
		{
			CalcCompositeObject(pn->GetData());
		}

		if(bAddToDelay) tl->AddToDeleted(root);
	}
	delay--;
	//CString s;
	//s.Format("delay = %i\n", delay);
	//OutputDebugStr(s);
	POSITION p = tl->Find(root);
	if(root->GetLevel()==0)
	{
		POSITION pNext = p;
		tl->GetNext(pNext);
		tl->SetActiveChild(pNext);
	}
	tl->RemoveAt(p);
	if(!bAddToDelay) delete root;
}
int CTreeNotion::GetLevelsCount()
{
	return levels.GetCount();
}
void CTreeNotion::DelayRemove(BOOL delay)
{
	POSITION pos =	levels.GetHeadPosition();
	while(pos)
	{
		CTreeLevel* tl = levels.GetNext(pos);
		tl->DelayRemove(delay);
	}
}
void CTreeNotion::RemoveAll()
{	
	POSITION p = levels.GetHeadPosition();
	while(p)
	{
		CTreeLevel* tl = levels.GetNext(p);
		POSITION pl = tl->GetHeadPosition();
		while(pl)
		{
			CTreeNode* node = tl->GetNext(pl);
			delete node;
		}
		tl->RemoveAll();
		/*delete */tl->Release();
	}
	levels.RemoveAll();
}
int CTreeNotion::AddLevel(IUnknown* list)
{
	CTreeLevel* tl = new CTreeLevel(list);
	levels.AddTail(tl);
	int l = levels.GetCount();
	tl->SetLevel(l-1);
	return l;
}

bool CTreeNotion::DeleteElement(IUnknown* punkToDelete, int level)
{
	CTreeNode* node =0;
	CTreeLevel* tl =0;
	POSITION pos;
	pos = levels.GetHeadPosition();
	POSITION pSave;
	while(pos)
	{
		pSave = pos;
		tl = levels.GetNext(pos);
		node = find_on_level(punkToDelete, pSave);
		if(node) break;
	}
	if(node==0) return false;
	tl->RemoveChild(punkToDelete);	
	return true;
}


CTreeLevel* CTreeNotion::GetBaseLevel()
{
	return GetLevel(0);
}
CTreeNode* CTreeNotion::AddElement(int level)
{
	CTreeLevel* tl = GetLevel(level);
	CTreeNode* node = new CTreeNode();
	POSITION pos  = tl->AddTail(node);
	node->position_in_parent() = pos;
	node->SetLevel(level);
	return node;
}


void CTreeNode::remove_child(CTreeNode* node, POSITION pos, BOOL del = TRUE)
{
	if(pos)
	{
		node = children.GetAt(pos);
		if(del) delete node;
		children.RemoveAt(pos);
		return;
	}
	else if(node)
	{
		POSITION pos = children.GetHeadPosition();
		while(pos)
		{
			POSITION p = pos;
			CTreeNode* tn = children.GetNext(pos);
			if(tn == node)
			{
				if(del) delete node;
				children.RemoveAt(p);
			}
			return;
		}
	}
	ASSERT(false);
}
void CTreeNotion::CalcCompositeObject(IUnknown* object)
{
	if(m_group==0)
	{
		IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IMeasureManager, szMeasurement);
		ICompManagerPtr ptrManager(punk) ;
		if( punk )punk->Release();
		if(ptrManager ==0) return;
		int	nGroup, nGroupCount;
		ICompositeMeasureGroupPtr cmg;
		ptrManager->GetCount( &nGroupCount );
		for( nGroup = 0; nGroup < nGroupCount; nGroup++ )
		{
			IUnknown*	unkGroup;
			ptrManager->GetComponentUnknownByIdx( nGroup, &unkGroup );
			if(unkGroup)
			{
				cmg = unkGroup;
				unkGroup->Release();
				if(cmg!=0) 
				{
					m_group = cmg;
						break;
				}
			}
		}
	}
	if(m_group!=0)
	{
		CTreeLevel* tl = GetBaseLevel();		
		m_group->InitializeCalculation(tl->m_oList);
		m_group->CalcValues(object, 0);
		m_group->FinalizeCalculation();
	}
}
void  CTreeNode::KillChildren()
{
	if(children.IsEmpty()) return;
	POSITION pos = children.GetHeadPosition();
	while(pos)
	{
		CTreeNode* node = children.GetNext(pos);
		node->KillChildren();
		delete node;
	}
}

void CTreeNode::AddChild(CTreeNode* child)
{
	children.AddTail(child);
}
void CTreeNode::SetParent(CTreeNode* parent)
{
	if(this->parent!=0)
	{
		POSITION p =(this->parent)->children.Find(this);
		(this->parent)->children.RemoveAt(p);
	}

	GUID pG = GUID_NULL;
	ICompositeSupportPtr csChild(data);

	this->parent = parent;

	if(parent)
	{
		parent->AddChild(this);
		ICompositeSupportPtr csParent(parent->data);
		csParent->GetTreeKey(&pG);

	}

	csChild->SetParentKey(pG);


}

/////////////////////////////////////////////////////////////
//INamedDataObject2
//Warning!!! Implemented urgently needed methods only, be carefull whilst using this class
HRESULT CTreeLevel::CreateChild( IUnknown **ppunkChild ){ ASSERT(false); return E_NOTIMPL; }
HRESULT CTreeLevel::GetParent( IUnknown **ppunkParent )
{
	INamedDataObject2Ptr ndo(m_oList);
	return ndo->GetParent(ppunkParent); 
}

HRESULT CTreeLevel::SetParent( IUnknown *punkParent, DWORD dwFlags /*AttachParentFlag*/ ){ ASSERT(false); return E_NOTIMPL; }
HRESULT CTreeLevel::StoreData( DWORD dwDirection/*StoreDataFlags*/ ){ ASSERT(false); return E_NOTIMPL; }
HRESULT CTreeLevel::RemoveChild( IUnknown *punkChild )
{
	if(m_bIgnoreRemove) 
	{
		m_bIgnoreRemove =false;
		return S_OK;
	}
	POSITION pos = this->GetHeadPosition();
	CTreeNode* tn;
	POSITION nodePos;
	while(pos)
	{
		nodePos =pos;
		tn = GetNext(pos);
		if(tn->GetData() == punkChild) break;
		
	}
	if(nodePos==0) return E_FAIL;

	INamedDataObject2Ptr ndo(punkChild);
	IUnknown* parentUnk;
	ndo->GetParent(&parentUnk);
	if(parentUnk)
	{
		CListLockUpdate lock(parentUnk);

		tn->truly_adopt_children(!m_bDelay);
		if(m_bDelay)
		{
			m_listDeleted.AddTail(punkChild);
			
		}
		parentUnk->Release();
	}

	CTreeNode* parentNode = tn->GetParent();
	if(parentNode)
	{
		parentNode->remove_child(tn, 0, !m_bDelay);
	}

	return S_OK; 
}

//build real nameddata notion tree
void CTreeNode::truly_adopt_children(BOOL del)
{
		POSITION p;
		GetFirstChildPosition(p);
		CTreeNode* treeNode;
		while(p)
		{
			POSITION pos = p;
			GetNextChild(p, &treeNode );
			INamedDataObject2Ptr ndo(treeNode->data);
			ndo->SetParent(data,0);
			treeNode->truly_adopt_children(del);
			remove_child(0,pos,del);
		}
}
CTreeNode* CTreeLevel::find_parent_for(CTreeNode* node, double coverFactor)
{
	POSITION pos = GetHeadPosition();
	CTreeNode* tn = 0;
	while(pos)
	{
		tn = GetNext(pos);
		if(is_child(tn->GetData(), node->GetData(), coverFactor)) 
			return tn;
	}
	return 0;
}
void CTreeLevel::adopt_children(CTreeNode* object, CTreeNotion* tn, IUnknown* o_list)
{
	INamedDataObject2Ptr ndo2(object->GetData());
	TPOS l_pos;
	ndo2->GetFirstChildPosition(&l_pos);
	IUnknown* unk;
	while(l_pos)
	{
		ndo2->GetNextChild(&l_pos, &unk);
		if(CheckInterface(unk, IID_IMeasureObject2))
		{
			ICompositeSupportPtr c(unk);
			if(c) 
			{
				long level;
				c->GetLevel(&level);
				CTreeNode* tnode = tn->AddElement(level);
				tnode->SetData(unk);
				tnode->SetLevel(level);
				tnode->SetParent(object);//назначаем родителя в виртуальном листе
				CTreeLevel* tl = tn->GetLevel(level);
				tl->adopt_children(tnode, tn, o_list);
				long flag;
				if(c)
				{
					c->IsSyncronized(&flag);
					flag|=1;
					c->SetSyncronizedFlag(flag);
				}
				
				if(!(flag&2))
				{
					INamedDataObject2Ptr nd(unk);
					CListLockUpdate lock( m_oList);
					nd->SetParent(o_list,0);//назначаем NamedData родителя
				}
			}
		}
		unk->Release();
	}	
}
int CTreeLevel::GetClass()
{
	return m_nClass;
}

void CTreeLevel::SetClass(int c)
{
	m_nClass =c;
}

//Метод добавляет объект в виртуальный (CTreeLevel) и 
HRESULT CTreeLevel::AddChild( IUnknown *punkChild )
{
	CListLockUpdate* lupdate = new CListLockUpdate(m_oList);
	IMeasureObject2Ptr mo(punkChild);
	if(mo==0) {delete lupdate; return S_OK;}
	double cf = GetValueDouble(GetAppUnknown(), "Composite", "CoverFactor", 0.95);
	ICompositeObjectPtr comp(m_oList);
	if(comp) 
	{
		CTreeNotion* tn;
		comp->GetTreeNotion((void**) &tn);
		CTreeNode* object;
		if(tn)
		{
			fill_rowmask(punkChild);
			object = tn->AddElement(level);//добавили объект
			object->SetData(punkChild);

			//-+-{ Ищем подходящего родителя
			if(level>0)//not main level
			{
				int l = 0;
				CTreeLevel* parentTL;
				CTreeNode* parentNode =0;
				parentTL = tn->GetLevel(l);
				parentNode = parentTL->find_parent_for(object,cf);
				if(parentNode!=0) 
				{
					CTreeNode* temp = 0;
					while(temp!=parentNode)
					{//ищем родителя на более высоких уровнях
						temp=parentNode;
						POSITION pos; 
						parentNode->GetFirstChildPosition(pos);
						while(pos)
						{
							CTreeNode* node;
							parentNode->GetNextChild(pos, &node);
							if(node->GetLevel()<level && is_child(node->GetData(), punkChild, cf))
							{
								parentNode = node;
								break;
							}
						}
					}
					object->SetParent(parentNode);
				}
				else// ignore object because no parent found
				{
					POSITION pos = Find(object);
					RemoveAt(pos);
					delete object;
					delete lupdate;
					return S_OK;
				}
				//}-+-

			}
			else //added main level object
			{
				object->SetParent(0);
			}

			//-+-{ добавляем его детей if any (вероятно, вернули ранее удаленный объект)
			adopt_children(object, tn, m_oList);
			//}-+-

			//-+-{ ищем подходящих детей среди удаленных объектов
			find_children_in_deleted(object,cf);
			//}-+-


			//-+-{ ищем возможных детей среди дочерних объектов родительского
			find_children_in_parent(object,cf);
			//}-+-

			TPOS pinp =0;
			CTreeLevel* baseLevel =this;
			ICompositeSupportPtr comS(punkChild);
			if(comS) 
			{
				comS->SetLevel(level);

				set_composite_class(punkChild, m_nClass);

				long sync;
				comS->IsSyncronized(&sync);	
				sync|=1;//
				comS->SetSyncronizedFlag(sync);

				IMeasureObjectPtr mo(punkChild);
				IUnknown *image;
				mo->GetImage(&image);
				if(image)
				{
					::CalcObject(punkChild, image, true, 0);
					image->Release();
				}

				IUnknown* grandUnk;
				comS->GetGrandParent(&grandUnk);
				IUnknownPtr baseObject;
				if(grandUnk) 	
				{
					baseObject = grandUnk;
					grandUnk->Release();
				}
				else baseObject = punkChild;

				tn->CalcCompositeObject(baseObject);


				IUnknown* punkNamedData;
				this->GetData(&punkNamedData);
				if( punkNamedData && (sync & 2))
				{
					::FireEventNotify( punkNamedData, szEventChangeObjectList, m_oList, baseObject, cncChange );

				}
				if( punkNamedData) punkNamedData->Release();


				if(level!=0)
				{
					baseLevel = tn->GetBaseLevel();
				}
				else baseLevel=this;

				CTreeNode* bn = tn->Find(baseObject,0);
				pinp = bn->position_in_parent();

				if(sync & 2) //объект уже был добавлен в реальный лист 
				{
					baseLevel->SetActiveChild(pinp);
					delete lupdate;
					return S_OK; 
				}

			}
			delete lupdate;
			INamedDataObject2Ptr ndo(punkChild);
			m_bIgnoreRemove =true;//передаем объект object list'у (без лишнего шума)
			ndo->SetParent(0,0);//это удаление не вызовет RemoveObject

			ndo->SetParent(m_oList, 0);

			baseLevel->SetActiveChild(pinp);

			return S_OK;

		}
	}

	return E_FAIL; 
}
void CTreeLevel::find_children_in_parent(CTreeNode* object, double cf)
{
	CTreeNode* parent = object->GetParent();
	if(parent==0) return;

	int level = object->GetLevel();

	POSITION pos, currPos;
	parent->GetFirstChildPosition(pos);
	CTreeNode* temp;
	while(pos)
	{
		currPos = pos;
		parent->GetNextChild(pos, &temp);
		if(temp->GetLevel()>level)
		{
			if(is_child(object->GetData(), temp->GetData(), cf))
			{
				temp->SetParent(object);
			}
		}
	}
}
void CTreeLevel::find_children_in_deleted(CTreeNode* object, double cf)
{
	if(m_listDeleted.IsEmpty()) return;
	POSITION pos =	m_listDeleted.GetHeadPosition();
	CTreeNode* tn;
	while(pos)
	{
		tn = (CTreeNode*)GetNext(pos);
		if(tn->GetLevel()==object->GetLevel())
		{
			POSITION p;
			tn->GetFirstChildPosition(p);
			CTreeNode* child;
			ICompositeObjectPtr co(m_oList);
			while(p)
			{
				tn->GetNextChild(p, &child);
				if(is_child(object->GetData(), child->GetData(), cf))
				{
					child->SetParent(object);
					CTreeNotion* treeNotion;
					co->GetTreeNotion((void**)&treeNotion);
					child->plant_tree(treeNotion, m_oList);
				}
			}
		}

	}

}
void CTreeLevel::DelayRemove(BOOL delay)
{
	m_bDelay = delay;
	if(delay == FALSE &&  !m_listDeleted.IsEmpty())
	{

		POSITION pos = m_listDeleted.GetHeadPosition();
		CTreeNode* tn;
		while(pos)
		{
			tn = (CTreeNode*)m_listDeleted.GetNext(pos);
			tn->KillChildren();
			delete tn;
		}
		m_listDeleted.RemoveAll();
	}
}
void CTreeLevel::AddToDeleted(CTreeNode* node)
{
	if(m_bDelay)
	{
		m_listDeleted.AddTail(node);
	}
}


void CTreeNode::plant_tree(CTreeNotion* tn, IUnknown* o_list)
{
	CTreeLevel* tl = tn->GetLevel(level);
	if(tl)
	{
		tl->AddTail(this);
		ICompositeSupportPtr comSupp(data);
		if(comSupp)
		{

			long flag;
			if(comSupp)
			{
				comSupp->IsSyncronized(&flag);
				flag|=1;//сигнал о том, что объект добавлен в виртуальный лист
				comSupp->SetSyncronizedFlag(flag);
			}
		}
		INamedDataObject2Ptr ndo(data);
		ndo->SetParent(o_list, 0);

	}
	POSITION pos;
	GetFirstChildPosition(pos);
	CTreeNode* node;
	while(pos)
	{
		GetNextChild(pos, &node);
		node->plant_tree(tn, o_list);
	}
}

HRESULT CTreeLevel::GetChildsCount( long *plCount )
{ 
	*plCount = this->GetCount();
	return S_OK;
}
HRESULT CTreeLevel::GetFirstChildPosition(TPOS *plPos)
{
	*plPos = GetHeadPosition();
	return S_OK;
}
HRESULT CTreeLevel::GetNextChild(TPOS *plPos, IUnknown **ppunkChild)
{ 
	CTreeNode* tn = GetNext((TPOS&)*plPos);
	*ppunkChild  = tn->GetData();
	if(*ppunkChild)(*ppunkChild)->AddRef();
	return S_OK;
}
HRESULT CTreeLevel::GetLastChildPosition(TPOS *plPos){ ASSERT(false); return E_NOTIMPL; }
HRESULT CTreeLevel::GetPrevChild(TPOS *plPos, IUnknown **ppunkChild){ ASSERT(false); return E_NOTIMPL; }

HRESULT CTreeLevel::AttachData( IUnknown *punkNamedData )
{ 
	INamedDataObject2Ptr ndo(m_oList);
	if(ndo) return ndo->AttachData(punkNamedData);

	return E_FAIL; 

}	//called from NamedData
HRESULT CTreeLevel::GetObjectFlags( DWORD *pdwObjectFlags ){ ASSERT(false); return E_NOTIMPL; }

HRESULT CTreeLevel::SetActiveChild( TPOS lPos )
{
	m_lActive = lPos;
	CTreeNode* tn;
	if(IsEmpty() ) lPos=0;
	if(lPos==0) //next code for synchronizing with main object list 
	{
		tn = 0;
	}
	else
	{
		tn = GetAt((POSITION)lPos);
	}
	INamedDataObject2Ptr ndo(m_oList);
		if(ndo)
		{
			if(lPos!=0) ndo->GetChildPos(tn->data, &lPos);
			ndo->SetActiveChild( lPos );
		}

	return S_OK; 
}
HRESULT CTreeLevel::GetActiveChild( TPOS *plPos )
{
	*plPos = m_lActive;
	return S_OK; 
}
HRESULT CTreeLevel::SetUsedFlag( BOOL bSet ){ ASSERT(false); return E_NOTIMPL; }//set flag object used as part of another object, doesn't 
//require to store object in NamedData
HRESULT CTreeLevel::SetHostedFlag( BOOL bSet ){ ASSERT(false); return E_NOTIMPL; }//set flag if object controlled by host
HRESULT CTreeLevel::GetData(IUnknown **ppunkNamedData )
{ 
	INamedDataObject2Ptr ndo(m_oList);
	IUnknown* unk;
	if(ndo) ndo->GetData(&unk);
	*ppunkNamedData = unk;
	if(unk)	return S_OK;
	return E_FAIL; 
}

HRESULT CTreeLevel::GetObjectPosInParent(TPOS *plPos){ ASSERT(false); return E_NOTIMPL; }// return object position in parent's object child list
HRESULT CTreeLevel::SetObjectPosInParent(TPOS lPos){ ASSERT(false); return E_NOTIMPL; }

HRESULT CTreeLevel::GetChildPos(IUnknown *punkChild, TPOS *plPos)
{
	POSITION pos, posOld;
	*plPos =0;
	pos = GetHeadPosition();
	while(pos)
	{
		posOld = pos;
		CTreeNode* tn = GetNext(pos);
		ICalcObjectPtr co(tn->data);
		if(co == punkChild) 
		{
			*plPos = posOld;
			return S_OK;
		}
	}
	return S_OK;
}
HRESULT CTreeLevel::InitAttachedData(){ return S_OK; }
// base and source key functionality
HRESULT CTreeLevel::GetBaseKey(GUID * pBaseKey)
{ 
	if(this->m_oList)
	{
		INamedDataObject2Ptr nd(m_oList);
		return nd->GetBaseKey(pBaseKey);
	}
	return E_FAIL; 
}
HRESULT CTreeLevel::SetBaseKey(GUID * pBaseKey){ ASSERT(false); return E_NOTIMPL; }
HRESULT CTreeLevel::IsBaseObject(BOOL * pbFlag)
{ 
	if(this->m_oList)
	{
		INamedDataObject2Ptr nd(m_oList);
		return nd->IsBaseObject(pbFlag);
	}
	return E_FAIL; 
}

HRESULT CTreeLevel::GetType( BSTR *pbstrType )
{ 
	if(!pbstrType) return E_INVALIDARG;
	*pbstrType = CString("Object List").AllocSysString();
	return S_OK; 
}
HRESULT CTreeLevel::GetDataInfo( IUnknown **ppunk ){ ASSERT(false); return E_NOTIMPL; }
HRESULT CTreeLevel::IsModified( BOOL *pbModified ){ ASSERT(false); return E_NOTIMPL; }
HRESULT CTreeLevel::SetModifiedFlag( BOOL bSet )
{

	INamedDataObject2Ptr ndo(m_oList);

	return ndo->SetModifiedFlag( bSet );

}
HRESULT CTreeLevel::SetTypeInfo( INamedDataInfo *pinfo ){ ASSERT(false); return E_NOTIMPL; }



void CTreeLevel::SetActiveChild( TPOS pos, CTreeNode* childToSet=0 )
{
	if(childToSet!=0)
	{
		m_lActive = Find(childToSet);
	}
	else  m_lActive = pos;
}
// восстанавливает структуру дерева после сериализации



LPDISPATCH CMeasureObjectList::GetSimilarObject(LPDISPATCH pdispObject)
{
	ICalcObjectPtr sptrObj(pdispObject);
	if(sptrObj==0) return 0;

	int nClasses = class_count();
	int nObjects = GetCount();
	int nParams = GetParametersCount();

	smart_alloc(params, long, nParams);
	smart_alloc(params_weight, double, nParams);
	smart_alloc(values, double, nParams*nObjects);
	smart_alloc(values_obj, double, nParams);
	smart_alloc(classes, long, nObjects);
	smart_alloc(classes_weight, double, nClasses);

	ICalcObjectContainerPtr	ptrC( GetControllingUnknown() );

	LONG_PTR lpos = 0;
	ptrC->GetFirstParameterPos( &lpos );
	char szBuf[100];
	for(int i=0; i<nParams; i++)
	{
		ParameterContainer	*p = 0;
		ptrC->GetNextParameter( &lpos, &p );
		params[i] = p->pDescr->lKey;
		values_obj[i] = 1e222; // >1e200 - ошибочное число, в расчете не участвует
		sptrObj->GetValue(params[i], &values_obj[i]);
		GetPrivateProfileString( "ParamsWeight", CString(p->pDescr->bstrName), "",
			szBuf, sizeof(szBuf), full_classifiername(0) );
		if(szBuf[0]) params_weight[i] = atof(szBuf);
		else WritePrivateProfileString( "ParamsWeight", CString(p->pDescr->bstrName),
			"1.0", full_classifiername(0) ); // Сразу воткнем обратно - на случай, если не было прописано
	}

	char szNumBuf[10];
	for(int k=0; k<nClasses; k++)
	{
		_itoa(k, szNumBuf, 10);
		GetPrivateProfileString( "ClassesWeight", szNumBuf, "",
			szBuf, sizeof(szBuf), full_classifiername(0) );
		classes_weight[k]=1.0;
		if(szBuf[0]) classes_weight[k] = atof(szBuf);
		else WritePrivateProfileString( "ClassesWeight", szNumBuf,
			"1.0", full_classifiername(0) ); // Сразу воткнем обратно - на случай, если не было прописано

	}

	POSITION pos = GetFirstChildPosition();
	for(int j=0; j<nObjects; j++)
	{
		classes[j] = -1;
		for(int i=0; i<nParams; i++) values[j*nParams+i] = 1e222; // >1e200 - ошибочное число, в расчете не участвует
		IUnknown * punk = GetNextChild(pos); // без AddRef
		ICalcObjectPtr sptrChild = punk;
		if (sptrChild != 0)
		{
			classes[j] = get_object_class(sptrChild);
			for(int i=0; i<nParams; i++)
			{
				values[j*nParams+i] = 1e222; // >1e200 - ошибочное число, в расчете не участвует
				sptrChild->GetValue(params[i], &values[j*nParams+i]);
			}
		}
	}

	// На данный момент все данные в массивах: номера параметров в params[i],
	// значения для списка - в values[j*nParams+i], для объекта - в values_obj[i]
	// классы в classes[j]

	smart_alloc(num,int,nClasses*nParams)
	smart_alloc(avg,double,nClasses*nParams)
	smart_alloc(disp,double,nClasses*nParams)
	smart_alloc(num_total,int,nParams)
	smart_alloc(disp_total,double,nParams)

	for(int i=0; i<nParams; i++)
	{
		for(int k=0; k<nClasses; k++)
		{
			num[k*nParams+i] = 0;
			avg[k*nParams+i] = 0;
			disp[k*nParams+i] = 0;
		}
		num_total[i] = 0;
		disp_total[i] = 0;
	}

	for(int i=0; i<nParams; i++)
	{
		// расчет средних
		for(int j=0; j<nObjects; j++)
		{
			if(values[j*nParams+i]>1e200)
				continue; // ошибочное значение
			int k = classes[j];
			if(k<0 || k>=nClasses) continue;
			avg[k*nParams+i] = ( avg[k*nParams+i] * num[k*nParams+i] + values[j*nParams+i] ) / (num[k*nParams+i]+1);
			num[k*nParams+i]++;
			num_total[i]++;
		}

		// расчет дисперсий
		for(int j=0; j<nObjects; j++)
		{
			int k = classes[j];
			if(k<0 || k>=nClasses) continue;
			if(values[j*nParams+i]>1e200)
				continue; // ошибочное значение
			double df = values[j*nParams+i] - avg[k*nParams+i];
			disp[k*nParams+i] += df*df / num[k*nParams+i] ;
			disp_total[i] = disp_total[i] + df*df / num_total[i];
		}
	}

	// Сравнение
	double best_d = 1e200;
	int best_j = -1;

	int nClass = get_object_class(sptrObj);
	double best_d_same_class = 1e200;
	int best_j_same_class = -1;

	smart_alloc(classes_best_d, double, nClasses);
	for(int k=0; k<nClasses; k++) classes_best_d[k] = 1e200;

	for(int j=0; j<nObjects; j++)
	{
		int k = classes[j];
		if(k<0 || k>=nClasses) continue;
		double d = 0;
		for(int i=0; i<nParams; i++)
		{
			if(values[j*nParams+i]>1e200)
				continue; // ошибочное значение
			if(values_obj[i]>1e200)
				continue; // ошибочное значение

			double df = values_obj[i] - values[j*nParams+i];
			df *= params_weight[i];
			double scale = disp_total[i];
			if(k>=0 && k<nClasses) scale = disp[k*nParams+i];
			if(scale<=0) scale = 1e-100;
			d += df*df/scale;
		}
		d *= classes_weight[k];
		if(d<best_d)
		{
			best_d = d;
			best_j = j;
		}
		if( k==nClass && d<best_d_same_class )
		{
			best_d_same_class = d;
			best_j_same_class = j;
		}
		classes_best_d[k] = min(classes_best_d[k], d);
	}

	if( nClass!=-1 && best_j_same_class!=best_j && best_j_same_class!=-1 )
	{	// Если расклассифицировали в другой класс
		int k_best = classes[best_j];
		int k_same = classes[best_j_same_class];
		for(int i=0; i<nParams; i++)
		{
			if(values[best_j*nParams+i]>1e200)
				continue; // ошибочное значение
			if(values[best_j_same_class*nParams+i]>1e200)
				continue; // ошибочное значение
			if(values_obj[i]>1e200)
				continue; // ошибочное значение
			double df_best = values_obj[i] - values[best_j*nParams+i];
			double df_same = values_obj[i] - values[best_j_same_class*nParams+i];
			df_best *= params_weight[i];
			df_same *= params_weight[i];
			double scale_best = disp_total[i];
			double scale_same = disp_total[i];
			if(k_best>=0 && k_best<nClasses) scale_best = disp[k_best*nParams+i];
			if(k_same>=0 && k_same<nClasses) scale_same = disp[k_same*nParams+i];
			if(scale_best<=0) scale_best = 1e-100;
			if(scale_same<=0) scale_same = 1e-100;
			double penalty = 
				df_same*df_same/scale_same * classes_weight[k_same] -
				df_best*df_best/scale_best * classes_weight[k_best] ;
			penalty /= max(best_d, best_d_same_class);

			// Добавим полученный штраф в shell.data
			ParameterContainer	*p = 0;
			ptrC->ParamDefByKey( params[i], &p );
			double fParamQuality = ::GetValueDouble(::GetAppUnknown(),
				"\\Objects\\SimilarObject\\ParamPenalty", CString(p->pDescr->bstrName), 0.0);
			::SetValue(::GetAppUnknown(),
				"\\Objects\\SimilarObject\\ParamPenalty", CString(p->pDescr->bstrName), fParamQuality + penalty);
		}
	}

	// Поиск второго по близости класса
	double best_d2 = 1e200;
	for(int k=0; k<nClasses; k++)
	{
		if ( k == classes[best_j] ) continue;
		best_d2 = min(best_d2, classes_best_d[k]);
	}
	double ratio_d = 1.0;
	if(best_d2>0) ratio_d = best_d / best_d2;

	::SetValue(::GetAppUnknown(), "\\Objects\\SimilarObject", "Distance", best_d);
	::SetValue(::GetAppUnknown(), "\\Objects\\SimilarObject", "Accuracy", 1 - ratio_d);



	//if(0)
	//{ // !!! для ошибок - чтобы оценить важность параметров
	//	int nClass = get_object_class(sptrObj);
	//	if(nClass!=-1)
	//	{
	//		for(int i=0; i<nParams; i++)
	//		{ // сравнение по 1 параметру
	//			double best_d1 = 1e200;
	//			int best_j1 = -1;
	//			for(int j=0; j<nObjects; j++)
	//			{
	//				int k = classes[j];
	//				double df = values_obj[i] - values[j*nParams+i];
	//				df *= params_weight[i];
	//				double scale = disp_total[i];
	//				if(k>=0 && k<nClasses) scale = disp[k*nParams+i];
	//				if(scale<=0) scale = 1e-100;
	//				double d = df*df/scale;
	//				d *= classes_weight[k];
	//				if( d < best_d1 )
	//				{
	//					best_d1 = d;
	//					best_j1 = j;
	//				}
	//			}
	//			ParameterContainer	*p = 0;
	//			ptrC->ParamDefByKey( params[i], &p );
	//			int nGood = GetPrivateProfileInt( "ParamsCountGood",
	//				CString(p->pDescr->bstrName), 0, full_classifiername(0) );
	//			int nBad = GetPrivateProfileInt( "ParamsCountBad",
	//				CString(p->pDescr->bstrName), 0, full_classifiername(0) );
	//			if(nClass == classes[best_j1])
	//			{
	//				nGood++;
	//				sprintf(szBuf, "%u", nGood);
	//				WritePrivateProfileString( "ParamsCountGood",
	//					CString(p->pDescr->bstrName), szBuf, full_classifiername(0) );
	//			}
	//			else
	//			{
	//				nBad++;
	//				sprintf(szBuf, "%u", nBad);
	//				WritePrivateProfileString( "ParamsCountBad",
	//					CString(p->pDescr->bstrName), szBuf, full_classifiername(0) );
	//			}
	//			double fRatio = double(nGood) / (nGood + nBad);
	//			sprintf(szBuf, "%1.3f", fRatio);
	//			WritePrivateProfileString( "ParamsCountRatio",
	//				CString(p->pDescr->bstrName), szBuf, full_classifiername(0) );

	//		}
	//	} // end if(nClass!=-1)
	//}

	return GetObject(best_j);
}

void CMeasureObjectList::InitSimilarityHelper()
{
	_try(CMeasureObjectList,InitSimilarityHelper)
	{
		m_pSimilarityHelper = new CSimilarityHelper;
		m_pSimilarityHelper -> Init(GetControllingUnknown());
	}
	_catch;
}

void CMeasureObjectList::DeInitSimilarityHelper()
{
	_try(CMeasureObjectList,DeInitSimilarityHelper)
	{
		delete m_pSimilarityHelper;
		m_pSimilarityHelper = 0;
	}
	_catch;
}

void CMeasureObjectList::SimilarityHelper_ProcessObject(LPDISPATCH pdispObject)
{ // обсчитать все для объекта; должен быть уже вызван Init
	_try(CMeasureObjectList,SimilarityHelper_ProcessObject)
	{
		//ASSSERT(m_pSimilarityHelper!=0);
		if(m_pSimilarityHelper==0) return; // ошибка - не проинитили

		m_pSimilarityHelper->ProcessObject(IUnknownPtr(pdispObject));
	}
	_catch;
}

void CMeasureObjectList::SimilarityHelper_ProcessObjectError()
{ // обсчитать для объекта статистику значимости параметров; необязательное дополнение к ProcessObject
	_try(CMeasureObjectList,SimilarityHelper_ProcessObjectError)
	{
		//ASSSERT(m_pSimilarityHelper!=0);
		if(m_pSimilarityHelper==0) return; // ошибка - не проинитили

		m_pSimilarityHelper->ProcessObjectError();
	}
	_catch;
}

long CMeasureObjectList::SimilarityHelper_GetClass()
{ // вернуть класс наиболее похожего
	_try(CMeasureObjectList,SimilarityHelper_GetСlass)
	{
		//ASSSERT(m_pSimilarityHelper!=0);
		if(m_pSimilarityHelper==0) return -1; // ошибка - не проинитили

		return m_pSimilarityHelper->GetClassSimilar();
	}
	_catch;
	return -1;
}

long CMeasureObjectList::SimilarityHelper_Get()
{ // вернуть номер наиболее похожего
	_try(CMeasureObjectList,SimilarityHelper_Get)
	{
		//ASSSERT(m_pSimilarityHelper!=0);
		if(m_pSimilarityHelper==0) return -1; // ошибка - не проинитили

		return m_pSimilarityHelper->GetSimilar();
	}
	_catch;
	return -1;
}

long CMeasureObjectList::SimilarityHelper_GetInClass(long nClass)
{ // вернуть номер наиболее похожего в данном классе
	_try(CMeasureObjectList,SimilarityHelper_GetInClass)
	{
		//ASSSERT(m_pSimilarityHelper!=0);
		if(m_pSimilarityHelper==0) return -1; // ошибка - не проинитили

		return m_pSimilarityHelper->GetSimilarInClass(nClass);
	}
	_catch;
	return -1;
}

long CMeasureObjectList::SimilarityHelper_GetInOwnClass()
{ // вернуть номер наиболее похожего в собственном классе
	_try(CMeasureObjectList,SimilarityHelper_GetInOwnClass)
	{
		//ASSSERT(m_pSimilarityHelper!=0);
		if(m_pSimilarityHelper==0) return -1; // ошибка - не проинитили

		return m_pSimilarityHelper->GetSimilarInOwnClass();
	}
	_catch;
	return -1;
}

double CMeasureObjectList::SimilarityHelper_GetParamDistance(int nParam)
{ // вернуть "расстояние" по указанному параметру
	_try(CMeasureObjectList,SimilarityHelper_GetParamDistance)
	{
		//ASSSERT(m_pSimilarityHelper!=0);
		if(m_pSimilarityHelper==0) return -1; // ошибка - не проинитили

		return m_pSimilarityHelper->GetParamDistance(nParam);
	}
	_catch;
	return 1e200;
}

void CMeasureObjectList::SimilarityHelper_WriteObjectReport(LPCWSTR fileName)
{ // сохранить отчет по объекту; должен быть уже вызван Init и Process
	_try(CMeasureObjectList,SimilarityHelper_WriteObjectReport)
	{
		//ASSSERT(m_pSimilarityHelper!=0);
		if(m_pSimilarityHelper==0) return; // ошибка - не проинитили

		m_pSimilarityHelper->WriteObjectReport(fileName);
	}
	_catch;
	return;
}

void CMeasureObjectList::SimilarityHelper_WriteErrorReport(LPCWSTR fileName)
{ // сохранить отчет по тестированию
	_try(CMeasureObjectList,SimilarityHelper_WriteErrorReport)
	{
		//ASSSERT(m_pSimilarityHelper!=0);
		if(m_pSimilarityHelper==0) return; // ошибка - не проинитили

		m_pSimilarityHelper->WriteTestReport(fileName);
	}
	_catch;
	return;
}

void CMeasureObjectList::SimilarityHelper_TestObjectList(LPDISPATCH pdispList)
{	// обсчитать по всем объектам листа - для каждого ProcessObject и ProcessObjectError
	// можно вызвать для нескольких листов подряд - накопится общая статистика
	_try(CMeasureObjectList,SimilarityHelper_TestObjectList)
	{
		//ASSSERT(m_pSimilarityHelper!=0);
		if(m_pSimilarityHelper==0) return; // ошибка - не проинитили

		m_pSimilarityHelper->TestObjectList(pdispList);
	}
	_catch;
	return;

}

void CMeasureObjectList::SimilarityHelper_TestObjectList2(LPDISPATCH pdispList)
{	// посчитать ошибки и найти оптимальные параметры для вкл/выкл
	// вызывать только для одного листа за раз
	_try(CMeasureObjectList,SimilarityHelper_TestObjectList2)
	{
		//ASSSERT(m_pSimilarityHelper!=0);
		if(m_pSimilarityHelper==0) return; // ошибка - не проинитили

		m_pSimilarityHelper->TestObjectList2(pdispList);
	}
	_catch;
	return;
}

double CMeasureObjectList::SimilarityHelper_GetError2()
{ // получить ошибку, которая была при TestObjectList2
	_try(CMeasureObjectList,SimilarityHelper_GetError2)
	{
		//ASSSERT(m_pSimilarityHelper!=0);
		if(m_pSimilarityHelper==0) return -1; // ошибка - не проинитили

		return m_pSimilarityHelper->GetError2();
	}
	_catch;
	return 1e200;
}

double CMeasureObjectList::SimilarityHelper_Improve2()
{ // один шаг оптимизации, возвращает, на сколько прооптимизировали; только после TestObjectList2
	_try(CMeasureObjectList,SimilarityHelper_Improve2)
	{
		//ASSSERT(m_pSimilarityHelper!=0);
		if(m_pSimilarityHelper==0) return -1; // ошибка - не проинитили

		return m_pSimilarityHelper->Improve2();
	}
	_catch;
	return 1e200;
}
