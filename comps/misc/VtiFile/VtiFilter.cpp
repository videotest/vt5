// VtiFilter.cpp: implementation of the CVtiFileFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VtiFilter.h"
#include "resource.h"
#include "DemoData.h"
#include "params.h"
#include "\vt5\common2\class_utils.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define _SDEBUG

#if defined(_SDEBUG)
void dprintf(char * szFormat, ...)
{
	FILE *f = fopen("c:\\chromos.log", "at");
	if (!f)
		f = fopen("c:\\chromos.log", "wt");
	if (!f) return;
    va_list va;
    va_start(va, szFormat);
    vfprintf(f, szFormat, va);
    va_end(va);
	fclose(f);
}
#endif



IMPLEMENT_DYNCREATE(CVtiFileFilter, CCmdTargetEx);

// {EAFC2853-C304-4739-A2E4-79991B6B4B3E}
GUARD_IMPLEMENT_OLECREATE(CVtiFileFilter, "FileFilters.VTIFileFilter", 
0xeafc2853, 0xc304, 0x4739, 0xa2, 0xe4, 0x79, 0x99, 0x1b, 0x6b, 0x4b, 0x3e);

// Signatures of VTI file

CString strSignature = "VT4File";
CString strSignature1 = "VT4File1";
CString strSignIcon = "VT4Icon";
CString strSignBitmap = "VT4Bmp";
CString strSignBinImage = "VT4BinImage";
CString strSignDatList = "VT4DataList";
CString strSignTableParam = "VT4DataInfo";
CString strSignTableInfo = "VT4ClassTableInfo";
CString strSignText = "VT4ImageText";
CString strSignGalText = "VT4GalText";
CString strSignOrigMask = "VT4OrigMask";
CString strSignEnd = "VT4EndOfFile";

/*void VerifyExist( CTypeInfo *pInfo )
{
	ASSERT( pInfo );

	if( !pInfo->IsEnable() )
		return;

	if( pInfo->GetGroup() == TG_MANUAL )
	{
		if( !m_TypeListHandy.Find( pInfo ) )
			m_TypeListHandy.AddTail( pInfo );
	}
	else
	{
		if( !m_TypeList.Find( pInfo ) )
			m_TypeList.AddTail( pInfo );
	}
}*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CVtiFileFilter::CVtiFileFilter()
{
	_OleLockApp( this );
	m_nWidth = 0;
	m_nHeight = 0;
	m_nColors = 0;
	m_nDepth = 0;
	m_strCompression = "";
	m_pImg = NULL;
	m_strDocumentTemplate = _T("Image");
	m_strFilterName.LoadString( IDS_VTI_FILTERNAME );
	m_strFilterExt = _T(".vti");
	AddDataObjectType(szArgumentTypeImage);
	AddDataObjectType(szArgumentTypeObjectList);
	AddDataObjectType(szArgumentTypeClassList);
}

CVtiFileFilter::~CVtiFileFilter()
{
	_OleUnlockApp( this );
}

/*void CImgDoc::CVtiFileFilter()
{
	m_piTypesCacheUssage = new int[c_iCacheSize];
	m_pwTypesCacheData = new WORD[c_iCacheSize];
	m_ppTypesCache = new CTypeInfo*[c_iCacheSize];

	for( int i = 0; i <c_iCacheSize; i++ )
	{
		m_pwTypesCacheData[i] = NOKEY;
		m_ppTypesCache[i] = 0;
	}
}*/

extern int g_iCurrentLoadVersion;


void CVtiFileFilter::DoLoadDemoVTI(CArchive &ar)
{
	CString str;
	ar>>str;
	SerializeIcon( ar );
	BYTE baData[1024];
	ar.Read(baData,1024);
	ar.Flush();
	DWORD dw = ar.GetFile()->GetPosition();
	CFile   *pFile = ar.GetFile();
	CImageBase *pimg = m_pImg;
	CImageBase *pimg1 = CImageBase::MakeFromFile( *pFile );
	m_pImg = DataCnv::ConvertImage(pimg1,baData);
	if (pimg1->IsGrayScale() && !pimg1->Is16BitColor())
		m_pImg->ReplacePalette(((C8Image *)pimg1)->GetRGBQuad());
	delete pimg1;
	delete pimg;
}

void CVtiFileFilter::SerializeImage( CArchive &ar )
{
	ar.Flush();
	CFile   *pFile = ar.GetFile();
	if( !ar.IsStoring() )
	{
		CImageBase *pimg = m_pImg;
		m_pImg = CImageBase::MakeFromFile( *pFile );
		delete pimg;
		m_nWidth = m_pImg->GetCX();
		m_nHeight = m_pImg->GetCY();
		m_nDepth = m_pImg->Is16BitColor()?16:8;
		m_nColors = m_pImg->IsTrueColor()?3:1;
		m_strCompression.Empty();
		m_nNumPages = 1;
	}
	else
		m_pImg->Write( pFile );
}

void CVtiFileFilter::SerializeBinImage( CArchive &ar )
{
	ar.Flush();
	CFile   *pFile = ar.GetFile();
	CImageBase      *p;

	if( !ar.IsStoring() )
	{
		p = CImageBase::MakeFromFile( *pFile );
		m_BinImg.Assign8(*(C8Image *)p);
		delete p;
	}
	else
		m_BinImg.Write( pFile );
}

void CVtiFileFilter::SerializeIcon( CArchive &ar )
{
	ar.Flush();
	CFile   *pFile = ar.GetFile();
	WORD cx,cy;

	if( !ar.IsStoring() )
	{
		m_pIconImage = (C8Image *)CImageBase::MakeFromFile( *pFile );
		safedelete(m_pIconImage);
		ar >> cx;
		ar >> cy;
	}
	else
	{
		if( !m_pIconImage )
		{
			int dx = (int)ceil(((double)m_pImg->GetCX())/120);
			int dy = (int)ceil(((double)m_pImg->GetCY())/120);
			m_pIconImage = new C8Image(dx,dy,CImageBase::GrayScale);
		}
		m_pIconImage->Write( pFile );
		cx = m_pImg?m_pImg->GetCX():0;
		cy = m_pImg?m_pImg->GetCY():0;
		ar << cx;
		ar << cy;
	}
}

void CVtiFileFilter::SerializeDataList( CArchive &ar )
{
	ASSERT_VALID( this );
	m_pparamAuto = new CGXGridParam;
	m_pparamAuto->Serialize( ar );
	m_pparamHandy = new CGXGridParam;
	m_pparamHandy->Serialize( ar );
	m_ObjArray.Serialize( ar );
	ASSERT_VALID( this );
	if( !ar.IsStoring() )
	{
		ar>>m_iHandyObjCount>>m_fArea;
	}
	else
		ar<<m_iHandyObjCount<<m_fArea;
	delete m_pparamAuto;
	delete m_pparamHandy;
	m_pparamAuto = m_pparamHandy = NULL;
	ASSERT_VALID( this );
}

void CVtiFileFilter::SerializeTableParam( CArchive &ar )
{
	m_pGalParam = new CGalParams;
	m_pGalParam->Serialize( ar );
	delete m_pGalParam;
	m_pGalParam = NULL;
}

void CVtiFileFilter::SerializeClassTableInfo( CArchive &ar )
{
	CString str, strName;

/*	CDocument       *pdoc = (CDocument*)pTableDocTemplate->GetSingleDocument();
	ASSERT( pdoc );

	strName = pdoc->GetPathName();*/
	
	if( !ar.IsStoring() )
	{
		ar>>str;

/*		if( str.IsEmpty() )
			return;

		if( str != strName && _GetProfileInt("Settings", "ReloadClasTableForVTI", 1))
			if( AfxMessageBox( IDS_CHANGETABLE, MB_YESNO )== IDYES )
				pTableDocTemplate->OpenDocumentFile( str );*/
	}
	else
	{
		ar<<strName;
	}
}


void CVtiFileFilter::DoLoadVTI( CArchive &ar )
{
	ASSERT(ar.IsLoading());
	CString str;
	do
	{
		ar>>str;
		if( str == strSignBitmap )
			SerializeImage( ar );
		else if( str == strSignBinImage )
			SerializeBinImage( ar );
		else if( str == strSignDatList )
			SerializeDataList( ar );
		else if( str == strSignTableParam )
			SerializeTableParam( ar );
		else if( str == strSignIcon )
			SerializeIcon( ar );
		else if( str == strSignTableInfo )
			SerializeClassTableInfo( ar );
		else if( str == strSignText )
			m_Text.Serialize( ar );
		else if( str == strSignGalText )
			m_GalText.Serialize( ar );
		else if( str == strSignOrigMask )
			m_objOrig.Serialize( ar );
		else
			break;

		ASSERT_VALID( this );
	}
	while( str != strSignEnd );
	for( int i = 0; i < m_ObjArray.GetSize(); i++ )
	{
		CObjMeasure     *pobj = (CObjMeasure*)m_ObjArray[i];
		POSITION        pos = pobj->GetFirstArgPosition();
		while( pos )
		{
			CArg    *parg = pobj->GetNextArg( pos );
			WORD    wKey = parg->GetKey();
			CTypeInfo       *pinfo = g_TypesManager.Find( wKey );
			if( !pinfo )continue;
//			VerifyExist( pinfo );
		}
	}

//	UpdateObjList( 0 );
}


static void TransRGB_8(CImageWrp &Dest, CImage<BYTE> &Src, int cx, int cy)
{
	color *pRed,*pGreen,*pBlue;
	BYTE *pSrc;
	for (int y = 0; y < cy; y++)
	{
		pRed = Dest.GetRow(y, 0);
		pGreen = Dest.GetRow(y, 1);
		pBlue = Dest.GetRow(y, 2);
		pSrc = Src[y];
		for (int x = 0; x <cx; x++)
		{
			pBlue[x] = ((WORD)pSrc[3*x+0])<<8;
			pGreen[x] = ((WORD)pSrc[3*x+1])<<8;
			pRed[x] = ((WORD)pSrc[3*x+2])<<8;
		}
	}
}

static void TransRGB_16(CImageWrp &Dest, CImage<WORD> &Src, int cx, int cy)
{
	color *pRed,*pGreen,*pBlue;
	WORD *pSrc;
	for (int y = 0; y < cy; y++)
	{
		pRed = Dest.GetRow(y, 0);
		pGreen = Dest.GetRow(y, 1);
		pBlue = Dest.GetRow(y, 2);
		pSrc = Src[y];
		for (int x = 0; x <cx; x++)
		{
			pBlue[x] = pSrc[3*x+0];
			pGreen[x] = pSrc[3*x+1];
			pRed[x] = pSrc[3*x+2];
		}
	}
}

void InvertImage(CImageWrp &pImgWrp1)
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


static void TransGS_8(CImageWrp &Dest, CImage<BYTE> &Src, int cx, int cy)
{
	color *pDest;
	BYTE *pSrc;
	for (int y = 0; y < cy; y++)
	{
		pDest = Dest.GetRow(y, 0);
		pSrc = Src[y];
		for (int x = 0; x <cx; x++)
			pDest[x] = ((WORD)pSrc[x])<<8;
//			pDest[x] = ((WORD)(255-pSrc[x]))<<8;
	}
}

static void TransGS_16(CImageWrp &Dest, CImage<WORD> &Src, int cx, int cy)
{
	color *pDest;
	WORD *pSrc;
	for (int y = 0; y < cy; y++)
	{
		pDest = Dest.GetRow(y, 0);
		pSrc = Src[y];
		for (int x = 0; x <cx; x++)
			pDest[x] = pSrc[x];
	}
}

static void Image4To5(CImageWrp &Dest, CImageBase *pImg, int cx, int cy)
{
	if (pImg->Is16BitColor())
	{
		if (pImg->IsTrueColor())
			TransRGB_16(Dest, *(C16Image *)pImg, cx, cy);
		else
			TransGS_16(Dest, *(C16Image *)pImg, cx, cy);
	}
	else
	{
		if (pImg->IsTrueColor())
			TransRGB_8(Dest, *(C8Image *)pImg, cx, cy);
		else
			TransGS_8(Dest, *(C8Image *)pImg, cx, cy);
	}
}

static void _CopyImageToLocal(CImageWrp &Dest, CImageWrp &Src, CPoint pt1)
{
	int nSrcWidth = Src.GetWidth();
	int nSrcHeight = Src.GetHeight();
	int nWidth = Dest.GetWidth();
	int nHeight = Dest.GetHeight();
	int nPanes = Dest.GetColorsCount();
	for (int iPane = 0; iPane < nPanes; iPane++)
	{
		for (int y = 0; y < nHeight; y++)
		{
			if (y+pt1.y<0 || y+pt1.y >= nSrcHeight)
				continue;
			int x1 = min(max(0,pt1.x),nSrcWidth);
			int nWidth1 = min(nWidth-x1+pt1.x,nSrcWidth-x1);
			color *pSrc = Src.GetRow(y+pt1.y, iPane)+x1;
			color *pDst = Dest.GetRow(y, iPane)+x1-pt1.x;
			memcpy(pDst, pSrc, nWidth*sizeof(color));
		}
	}
}

static void _CopyBinaryToLocal(CImageWrp &Dest, C8Image &Src)
{
	int nWidth = Dest.GetWidth();
	int nHeight = Dest.GetHeight();
	int nWidth1 = Src.GetCX();
	int nHeight1 = Src.GetCY();
	nWidth = min(nWidth1,nWidth);
	nHeight = min(nHeight1,nHeight);
#if defined(_SDEBUG)
	BOOL bIsAny = FALSE;
#endif
	for (int y = 0; y < nHeight; y++)
	{
		byte *pSrc = Src[y];
		byte *pDst = Dest.GetRowMask(y);
		for (int x = 0; x < nWidth; x++)
		{
			pDst[x] = pSrc[x]==255?255:0;
		}
//		memcpy(pDst, pSrc, nWidth);
#if defined(_SDEBUG)
		for (int x = 0; x < nWidth; x++)
			if (pDst[x])
				bIsAny = TRUE;
#endif
	}
#if defined(_SDEBUG)
	if (!bIsAny)
		dprintf("Empty object in VtiFile\n");
#endif
}

bool CVtiFileFilter::DoReadFile(const char *pszFileName)
{
	CFile file(pszFileName, CFile::modeRead);
	CArchive ar(&file, CArchive::load);
	try
	{
		CString str;
		WORD wVersion;
		ar>>str;
		if (str == strSignature1)
		{
			ar>>wVersion;
			g_iCurrentLoadVersion = wVersion;
			if (wVersion == 0xFFFF)
				DoLoadDemoVTI(ar);
			else
				DoLoadVTI(ar);
		}
		else if (str == strSignature)
		{
			int nFirstVTIFormat = AfxGetApp()->GetProfileInt(_T("Settings"), _T("FirstVTIFormat"), -1);
			g_iCurrentLoadVersion = nFirstVTIFormat?0:1;
			DoLoadVTI(ar);
		}
	}
	catch( unsigned int )
	{
		AfxMessageBox(IDS_ERROR_VTI, MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}

static BYTE __ind3[] = {0, 255, 128};
static BYTE __ind4[] = {0, 255, 85, 170};

static COLORREF CalcColor(int nIndex)
{
	nIndex++;
	int nBlue = __ind3[nIndex%3];
	int nGreen = __ind4[(nIndex/3)%4];
	int nRed = __ind4[(nIndex/12)%4];
	return RGB(nRed,nGreen,nBlue);
}



void CVtiFileFilter::InitImage(const char *pszFileName)
{
	// Create image
	IUnknown *punk = CreateNamedObject( GetDataObjectType(0) );
	// get object name from path and check it's exists
	CString	strObjName = ::GetObjectNameFromPath(pszFileName);
	_bstr_t bstrName(strObjName);
	long	NameExists = 0;
	if (m_sptrINamedData != 0)
		m_sptrINamedData->NameExists(bstrName, &NameExists);
	// if new name not exists allready
	if (!NameExists && punk)
	{
		// set this name to object
		INamedObject2Ptr sptrObj(punk);
		sptrObj->SetName(bstrName);
	}
	INamedDataObject2Ptr sptrNDO2(punk);
	sptrNDO2->GetBaseKey(&m_guid1);
	CImageWrp sptrI( punk );
	if(punk)
	{
		m_Image.Attach(punk);
		punk = 0;	
	}
	CString strColorSpace;
	m_bIsRGB = m_pImg->IsTrueColor();
	if (m_pImg->IsTrueColor()) 
		strColorSpace = "RGB";
	else
		strColorSpace = "GRAY";
	if (sptrI.CreateNew(m_pImg->GetCX(), m_pImg->GetCY(), strColorSpace))
		Image4To5(sptrI, m_pImg, m_pImg->GetCX(), m_pImg->GetCY());
//	color a1 = sptrI.GetRow(0,0)[0];
//	InvertImage(sptrI);
//	color a2 = sptrI.GetRow(0,0)[0];
}

void CVtiFileFilter::InitClassesList(const char *pszFileName)
{
	if (m_ObjArray.GetSize() == 0) return;
	// Find number of classes
	CObjMeasure *pobj = (CObjMeasure*)m_ObjArray.GetAt(0);
	int nMaxClass = pobj->GetClass();
//	int nMinClass = pobj->GetClass();
	for (int i = 0; i < m_ObjArray.GetSize(); i++)
	{
		pobj = (CObjMeasure*)m_ObjArray.GetAt(i);
		int nClass = pobj->GetClass();
		nMaxClass = max(nClass,nMaxClass);
//		nMinClass = min(nClass,nMinClass);
	}
//	int nClasses = nMaxClass-nMinClass+2;
	int nClasses = nMaxClass+2;
	// Initialize classes list
	m_arrGuids.SetSize(nClasses);
	memset(m_arrGuids.GetData(), 0, nClasses*sizeof(GuidKey));
	IUnknown *punk = CreateNamedObject(GetDataObjectType(2));
	INamedDataObject2Ptr sptrCL(punk);
	if(sptrCL != 0)
	{
		for (int i = 0; i < nClasses; i++)
		{
			IUnknown *punk1 = CreateTypedObject(szArgumentTypeClass);
			if (punk1)
			{
				INamedDataObject2Ptr sptrND(punk1);
				if (sptrND != 0)
					sptrND->SetParent(punk, 0);
			}
			INumeredObjectPtr sptrCNO(punk1);
			GuidKey lClassKey;
			sptrCNO->GetKey(&lClassKey);
			m_arrGuids[i] = lClassKey;
			char szBuff[50];
//			if (i < 22)
				_itoa(i+1, szBuff, 10);
//			else if (i == 22)
//				strcpy(szBuff, "X");
//			else
//				strcpy(szBuff, "Y");
			_bstr_t	bstrNewName = szBuff;
			INamedObject2Ptr sptrNO2(punk1);
			sptrNO2->SetName(bstrNewName);
			COLORREF clr = CalcColor(i);
			IClassObjectPtr sptrCO(punk1);
			sptrCO->SetColor(clr);
			if (punk1) punk1->Release();
		}
	}
	if (punk) punk->Release();
}

void CVtiFileFilter::InitObjectsList(const char *pszFileName)
{
	if (m_ObjArray.GetSize() == 0 || m_iHandyObjCount == m_ObjArray.GetSize())
		return;
	int nObjOffset = ::GetValueInt(::GetAppUnknown(), "VTIFile", "ObjectsOffset", 0);
	// Create objects
	IUnknown *punk = CreateNamedObject( GetDataObjectType(1) );
	// get object name from path and check it's exists
	CString	strObjName = ::GetObjectNameFromPath(pszFileName);
	strObjName += " Objects";
	_bstr_t bstrName(strObjName);
	long	NameExists = 0;
	if (m_sptrINamedData != 0)
		m_sptrINamedData->NameExists(bstrName, &NameExists);
	// if new name not exists allready
	if (!NameExists && punk)
	{
		// set this name to object
		INamedObject2Ptr sptrObj(punk);
		sptrObj->SetName(bstrName);
	}
	INamedDataObject2Ptr sptrNDO2(punk);
	sptrNDO2->SetBaseKey(&m_guid1);
	INamedDataObject2Ptr sptrOL(punk);
	if(punk)
	{
		punk->Release();
		punk = 0;	
	}
	for (int i = m_iHandyObjCount; i < m_ObjArray.GetSize(); i++)
	{
		CObjMeasure *pobj = (CObjMeasure*)m_ObjArray.GetAt(i);
		// Create new object
		IUnknown *punk = CreateTypedObject(szArgumentTypeObject);
		ICalcObjectPtr sptrCO(punk);
		if (punk)
		{
			INamedDataObject2Ptr sptrND(punk);
			if (sptrND != 0)
				sptrND->SetParent(sptrOL, 0);
		}
		IMeasureObjectPtr sptrMO(punk);
		if (punk) punk->Release();
		// Create image for it
		CImageWrp sptrIDoc(m_Image);
		punk = CreateTypedObject(szArgumentTypeImage);
		CImageWrp sptrIObj(punk);
		if (punk) punk->Release();
		CString strColorSpace;
		if (m_bIsRGB) 
			strColorSpace = "RGB";
		else
			strColorSpace = "GRAY";
		CRect rcObj(pobj->GetRect());
		if (sptrIObj.CreateNew(rcObj.Width(), rcObj.Height(), strColorSpace))
		{
			// Init image, binary image and masks
			sptrIObj.InitRowMasks();
			sptrIObj.SetOffset(rcObj.TopLeft());
			_CopyImageToLocal(sptrIObj, sptrIDoc, rcObj.TopLeft());
			if (pobj->GetBIN())
				_CopyBinaryToLocal(sptrIObj, *pobj->GetBIN());
			sptrIObj.InitContours();
#if defined(_SDEBUG)
			if (sptrIObj.GetContoursCount()==0)
				dprintf("Empty contours\n");
#endif
			sptrMO->SetImage(sptrIObj);
		}
		// Now set class
		int nClass = pobj->GetClass();
		if (nClass >= 0)
		{
			char szEntry[50];
			_itoa(nClass, szEntry, 10);
			nClass = GetValueInt(GetAppUnknown(), "VTIFile", szEntry, nClass);
			nClass = max(nClass-nObjOffset,0);
			set_object_class( sptrCO, nClass );
		}
		// Set up parameters
//		IDataObjectListPtr sptrObjectList; // Object list will be needed 
		ICalcObjectContainerPtr	sptrCOContOL;
		INamedDataObject2Ptr sptrNDO2(sptrCO);
		IUnknownPtr ptrOL;
		sptrNDO2->GetParent(&ptrOL);
//		sptrObjectList = punkParent;
		sptrCOContOL = ptrOL;
		int nMaxIdentParam = 500;
		CArray<double,double&> arrParamValues;
		arrParamValues.SetSize(nMaxIdentParam);
		CArray<BOOL,BOOL&> arrParamsFound;
		arrParamsFound.SetSize(nMaxIdentParam);
		memset(arrParamsFound.GetData(), 0, nMaxIdentParam*sizeof(BOOL));
		POSITION pos = pobj->GetFirstArgPosition();
		while (pos)
		{
			CArg *pArg = pobj->GetNextArg(pos);
			WORD nVT4Key = pArg->GetKey();
			COleVariant var = pArg->GetValue();
			if (nVT4Key < nMaxIdentParam)
			{
				if (var.vt == VT_R8)
				{
					arrParamValues[nVT4Key] = var.dblVal;
					arrParamsFound[nVT4Key] = TRUE;
				}
			}
		}
		IUnknown *punkMMan = FindComponentByName(GetAppUnknown(false), IID_IMeasureManager, szMeasurement);
		ICompManagerPtr sptrCManMMan(punkMMan);
		if (punkMMan) punkMMan->Release();
		int nGroups;
		sptrCManMMan->GetCount(&nGroups);
		for (int iGroup = 0; iGroup < nGroups; iGroup++)
		{
			IUnknownPtr ptrGroup;
			sptrCManMMan->GetComponentUnknownByIdx(iGroup, &ptrGroup);
			IMeasParamGroupPtr sptrMPGr(ptrGroup);
			long lPosParam;
			sptrMPGr->GetFirstPos(&lPosParam);
			while (lPosParam)
			{
				ParameterDescriptor *ppDescr;
				sptrMPGr->GetNextParam(&lPosParam,&ppDescr);
				if (ppDescr->lEnabled && ppDescr->lKey >= 0 && ppDescr->lKey < nMaxIdentParam &&
					arrParamsFound[ppDescr->lKey])
				{
					double dVal = arrParamValues[ppDescr->lKey];
					sptrCOContOL->DefineParameter(ppDescr->lKey, etUndefined, sptrMPGr, 0 );
					sptrCO->SetValue(ppDescr->lKey, dVal);
				}
			}
		}
	}
}

void CVtiFileFilter::ClearAfterRead()
{
	delete m_pImg;
	m_pImg = NULL;
	m_ObjArray.DeInit();
	m_Image = 0;
}

bool CVtiFileFilter::ReadFile( const char *pszFileName )
{
//	InitTypesCache();
	try
	{
#if defined(_SDEBUG)
		dprintf("File %s \n", pszFileName);
#endif
		if (DoReadFile(pszFileName))
		{
			InitImage(pszFileName);
//			InitClassesList(pszFileName);
			InitObjectsList(pszFileName);
			ClearAfterRead();
		}
	}
	catch(...)
	{
		ClearAfterRead();
		throw;
	}
	return true;
}

bool CVtiFileFilter::WriteFile( const char *pszFileName )
{
	AfxMessageBox(IDS_EXPORT, MB_OK|MB_ICONEXCLAMATION);
	return false;
}


int  CVtiFileFilter::GetPropertyCount()
{
	return 6;
}

void CVtiFileFilter::GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx)
{
	CString strName;
	DWORD dwFlags = 0;
	DWORD dwGroup = 0;
	_variant_t var;
	switch (nNum)
	{
	case 0:
		dwGroup = 1;
		var = (long)m_nWidth;
		strName.LoadString(IDS_WIDTH);
		break;
	case 1:
		dwGroup = 1;
		var = (long)m_nHeight;
		strName.LoadString(IDS_HEIGHT);
		break;
	case 2:
		dwGroup = 2;
		var = (long)m_nDepth;
		dwFlags = ffpfTextAfterProp;
		strName.LoadString(IDS_DEPTH);
		break;
	case 3:
		dwGroup = 2;
		var = (long)m_nColors;
		dwFlags = ffpfTextAfterProp|ffpfNeedBrackets;
		strName.LoadString(IDS_COLORS);
		break;
	case 4:
		var = m_strCompression;
		strName.LoadString(IDS_COMPRESS);
		break;
	case 5:
		var = (long)m_nNumPages;
		strName.LoadString(IDS_PAGES);
		break;
	}
	if(pbstrName)
		*pbstrName = strName.AllocSysString();
	if(pGroupIdx)
		*pGroupIdx = dwGroup;
	if(pdwFlags)
		*pdwFlags = dwFlags;
	if(pvarVal)
		*pvarVal = var.Detach();
}

void CVtiFileFilter::GetPropertyByName(BSTR bstrName, VARIANT *pvarVal)
{
	_bstr_t bstrName1(bstrName);
	if (bstrName1 == _bstr_t("OnlyImport"))
	{
		::VariantClear(pvarVal);
		pvarVal->vt = VT_BOOL;
		pvarVal->boolVal = TRUE;
	}
}
