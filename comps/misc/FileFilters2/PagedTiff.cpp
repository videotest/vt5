#include "stdafx.h"
#include "TIFFilter.h"
#include "tiffio.h" 
#include "image5.h"
#include "impl_long.h"
#include "misc5.h"
#include "nameconsts.h"
#include "resource.h"
#include "ansiapi.h"
#include "binimageint.h"
#include "misc_utils.h"
#include "cgh_int.h"
#include "histint.h"
#include <strstream>
#include "core5.h"
#include "docview5.h"

using namespace std;


bool IsConvertorExists(const char *pCCName)
{
	IUnknownPtr punkCC(GetCCByName(pCCName));
	IColorConvertor2Ptr sptrCC(punkCC);
	BSTR bstr;
	sptrCC->GetCnvName(&bstr);
	return _bstr_t(bstr) == _bstr_t(pCCName);
}

bool IsPageEmpty(IUnknown *punkImage, int nPane, const char *pszCCName)
{
	return false;
//	return ::GetValueInt(punkImage, KARYO_ACTIONROOT, CGH_PANE0, 0)?false:true;
}

static void _copystring(_ptr_t2<char> &dest, const char *src)
{
	dest.alloc(strlen(src)+1);
	strcpy((char *)dest, src);
}

static void _copynstring(_ptr_t2<char> &dest, const char *src, size_t nLen)
{
	dest.alloc(nLen+1);
	strncpy((char *)dest, src, nLen);
	dest[nLen] = 0;
}

struct CEntryInfo
{
	BSTR bstr;
	VARIANT var;
};

static bool SerializeNamedData(IUnknown *punkND, _bstr_t bstrRoot, ostrstream &stream)
{
	INamedDataPtr sptrND(punkND);
	sptrND->SetupSection(bstrRoot);
	long nEntries;
	sptrND->GetEntriesCount(&nEntries);
	_ptr_t2<CEntryInfo> arrNames(nEntries);
	for (int i = 0; i < nEntries; i++)
	{
		arrNames[i].bstr = NULL;
		sptrND->GetEntryName(i, &arrNames[i].bstr);
		VariantInit(&arrNames[i].var);
		sptrND->GetValue(arrNames[i].bstr, &arrNames[i].var);
	}
	bool bAnyValueInRoot = false;
	for (i = 0; i < nEntries; i++)
	{
		_bstr_t bstrName(arrNames[i].bstr);
		_bstr_t bstrPath = bstrRoot + bstrName + _bstr_t("\\");
		streampos pos = stream.tellp();
		stream << (const char*)bstrName;
		bool bAnyValue = false;
		if (arrNames[i].var.vt == VT_I2 || arrNames[i].var.vt == VT_I4 || arrNames[i].var.vt == VT_BSTR)
		{
			stream << ":";
			if (arrNames[i].var.vt == VT_I2)
				stream << "2=" << arrNames[i].var.iVal;
			else if (arrNames[i].var.vt == VT_I4)
				stream << "4=" << arrNames[i].var.lVal;
			else if (arrNames[i].var.vt == VT_BSTR)
			{
				_bstr_t bstr1(arrNames[i].var.bstrVal);
				char *pstr = bstr1;
				if (strpbrk(pstr,":={};"))
				{
					char *ptemp = pstr;
					do
					{
						ptemp = strchr(ptemp, '\"');
						if (ptemp) *ptemp = '\'';
					}
					while (ptemp);
					stream << "Str=\"" << pstr << "\"";
				}
				else
					stream << "Str=" << (const char *)bstr1;
			}
			bAnyValue = true;
		}
		streampos pos1 = stream.tellp();
		stream << "{";
		bool bAnyChild = SerializeNamedData(punkND, bstrPath, stream);
		if (bAnyChild)
		{
			stream << "};";
			bAnyValueInRoot = true;
		}
		else if (bAnyValue)
		{
			stream.seekp(pos1, ios::beg);
			stream << ";";
			bAnyValueInRoot = true;
		}
		else
			stream.seekp(pos, ios::beg);
	}
	for (i = 0; i < nEntries; i++)
	{
		if (arrNames[i].bstr)
			::SysFreeString(arrNames[i].bstr);
		::VariantClear(&arrNames[i].var);
	}
	return bAnyValueInRoot;
}

static const char *RestoreNamedData(IUnknown *pNamedData, _bstr_t bstrSection, const char *pszData)
{
	const char *p1 = pszData,*p2;
	bool bSpecSect = false;
	do
	{
		// 1. Name of entry or section.
		p2 = strpbrk(p1, ":={};");
		_ptr_t2<char> sName;
		if (p2)
			_copynstring(sName, p1, p2-p1);
		else
			_copystring(sName, p1);
		bSpecSect = sName[0]=='!';
		_bstr_t bstrName((const char *)sName);
		// 2. Type of entry
		int vt = VT_EMPTY;
		if (p2 && *p2 == ':')
		{
			p2++;
			if (*p2 == '2')
				vt = VT_I2;
			else if (*p2 == '4')
				vt = VT_I4;
			else if (strncmp(p2,"R8",2)==0)
				vt = VT_R8;
			else if (strncmp(p2,"Str",3)==0)
				vt = VT_BSTR;
			p2 = strpbrk(p2, "={};");
		}
		// 3. Value of entry
		if (p2 && *p2 == '=')
		{
			p2++;
			_variant_t var;
			if (vt == VT_I2)
				var = _variant_t((short)atoi(p2), VT_I2);
			else if (vt == VT_I4)
				var = _variant_t((long)atoi(p2), VT_I4);
			else if (vt == VT_R8)
				var = _variant_t((double)atof(p2), VT_R8);
			else if (vt == VT_BSTR)
			{
				_ptr_t2<char> sStr;
				if (*p2 == '\"')
				{
					p2++;
					const char *p3 = strchr(p2, '\"');
					if (p3 == 0) return NULL;
					_copynstring(sStr, p2, p3-p2);
					p2 = strpbrk(p3+1, "{};");
				}
				else
				{
					const char *p3 = strpbrk(p2, "{};");
					if (p3)
						_copynstring(sStr,p2,p3-p2);
					else
						_copystring(sStr,p2);
					p2 = p3;
				}
				var = _variant_t((char *)sStr);
			}
			if (var.vt != VT_EMPTY && !bSpecSect)
				SafeSetValue(pNamedData, bstrSection, bstrName, var);
//				::SetValue(pNamedData, bstrSection, bstrName, var);
			if (p2) p2 = strpbrk(p2, "{};");
		}
		// 4. Children sections and entries of section
		if (*p2 == '{')
			p2 = RestoreNamedData(pNamedData, bstrSection+bstrName+_bstr_t("\\"), p2+1);
			// p2 now points to '}' or it is NULL
		p1 = p2 ? strchr(p2, ';') : NULL;
		if (p1 && *p1 == ';')
			p1++;
	}
	while (p1 && *p1 && *p1 != '}');
	return p1;
}

static IUnknownPtr _GetActiveView()
{
	IUnknownPtr punkView;
	IApplicationPtr sptrApp(GetAppUnknown());
	IUnknownPtr punkDoc;
	sptrApp->GetActiveDocument(&punkDoc);
	IDocumentSitePtr sptrDS(punkDoc);
	if (sptrDS != 0)
		sptrDS->GetActiveView(&punkView);
	return punkView;
}

CImageData::CImageData()
{
	bVT5 = false;
	bPreview = false;
	nPage = -1;
	nPanes = -1;
	pszND = NULL;
}

CImageData::CImageData(bool bPrv, const char *pszCCName, IColorConvertor2 *pCC, IImage4 *pimg)
{
	bVT5 = true;
	nPage = -1;
	bPreview = bPrv;
	nPanes = -1;
	pszND = NULL;
	if (!bPrv)
	{
		if (pszCCName)
		{
			if (pCC && pimg)
			{
				szCCName.alloc(strlen(pszCCName)+1);
				strcpy(szCCName, pszCCName);
				pimg->GetPanesCount(&nPanes);
				ostrstream stream;
				stream << (char*)szCCName;
				if (nPanes >= 0)
					stream << ";!NumOfPanes:4=" << nPanes << ";";
				SerializeNamedData(pimg, _bstr_t("\\"), stream);
				const char *p = stream.str();
				int nLen = stream.pcount();
				szImgDescr.alloc(nLen+1);
				_copynstring(szImgDescr,p,nLen);
				pszND = strchr(szImgDescr, ';');
				if (pszND) pszND++;
			}
			else
				_copystring(szImgDescr,pszCCName);
		}
		else
			_copystring(szImgDescr,"Gray");
	}
	else
		_copystring(szImgDescr,"Preview");
}

void CImageData::Read(TIFF *tif)
{
	char *p;
	int n;
	n = TIFFGetField(tif, TIFFTAG_SOFTWARE, &p);
	bVT5 = n > 0 && strcmp(p, VT5SOFTWARE) == 0;
	n = TIFFGetField(tif, TIFFTAG_IMAGEDESCRIPTION, &p);
	if (n > 0)
	{
		if (strcmp(p,"Preview")==0)
			bPreview = true;
		else
		{
			// Parse image description in form <CCName>;!NumOfPanes:4=<NumOfPanes>;<NamedData>.
			int nLen = strlen(p);
			szImgDescr.alloc(nLen+1);
			strcpy(szImgDescr,p);
			// First field : name of color convertor
			char *p1 = (char *)szImgDescr;
			char *p2 = strchr(p1,';');
			if (p2 == NULL)
				_copystring(szCCName,p1);
			else // Second field number of panes
			{
				_copynstring(szCCName,p1,p2-p1);
				p2++;
				const char pszNumOfPanes[] = "!NumOfPanes";
				if (strncmp(p2,pszNumOfPanes,sizeof(pszNumOfPanes)-1)==0)
				{
					char *p3 = strchr(p2,'=');
					char *p4 = strchr(p2,';');
					if (p3 < p4)
						nPanes = atoi(p3+1);
				}
				pszND = p2;
			}
			// Third field : named data of image (will be processed latter)
			bstrSecName = "FileFilters2\\";
			bstrSecName += (char*)szCCName;
		}
	}
	n = TIFFGetField(tif, TIFFTAG_PAGENAME, &p);
	if (n > 0) bstrPageName = p;
	uint16 nPage1[20];
	n = TIFFGetField(tif, TIFFTAG_PAGENUMBER, &nPage1[0], &nPage1[1]);
	if (n > 0) nPage = nPage1[0];
	n = TIFFGetField(tif, TIFFTAG_DOCUMENTNAME, &p);
	if (n > 0) bstrDocName = p;
}

bool CImageData::IsPageOfMultipage()
{
	if (GetValueInt(GetAppUnknown(), bstrSecName, "ReadMultipaged", 0))
		return true;
	return false;
}

void CImageData::Write(TIFF *tif)
{
	TIFFSetField(tif, TIFFTAG_IMAGEDESCRIPTION, (char *)szImgDescr);
}

int PaneByName(IImage4 *pImage, IColorConvertor2 *pCC, _bstr_t bstrPaneName)
{
	int nPaneNum = -1;
	int nPanes;
	pImage->GetPanesCount(&nPanes);
	for (int i = 0; i < nPanes; i++)
	{
		BSTR bstr;
		pCC->GetPaneName(i, &bstr);
		if (_bstr_t(bstr,false) == bstrPaneName)
		{
			nPaneNum = i;
			break;
		}
	}
	return nPaneNum;
}

/*static BOOL IsCCExist(const char *pszCCName)
{
	IUnknownPtr punkCC(GetCCByName(pszCCName));
	IColorConvertor2Ptr sptrCC(punkCC);
	BSTR bstr;
	sptrCC->GetCnvName(&bstr);
	return _bstr_t(bstr) == _bstr_t(pszCCName);
}

bool CTIFFilter::IsPageOfMultipage(TIFF *tif, bool &bSkip)
{
	int n = strlen("VT5 Page:");
	if (strncmp(pszDocName,"VT5 Page:",n)!=0) // If saved as multipage
		return false;
	pszDocName+=n;
	const char *p = strchr(pszDocName, ':');
	strncpy(szCCName,pszDocName,p-pszDocName); // Find convertor
	char szSection[100]; // If convertor exists and enabled
	strcpy(szSection,"FileFilters2\\");
	strcat(szSection,szCCName);
	if (!::GetValueInt(::GetAppUnknown(), szSection, "EnableRead", 1))
		return false;
	if (!IsCCExist(szCCName))
		return false;
	p++; // If pane exist in convertor
	if (strcmp(p,"Preview")==0)
	{
		strcpy(szPaneName,"Preview");
		nPaneNum=-1;
		return true;
	}
	IUnknownPtr punkCC(GetCCByName(szCCName));
	IColorConvertor2Ptr sptrCC(punkCC);
	nPaneNum = -1;
	int nPanes;
	sptrCC->GetColorPanesCount(&nPanes);
	for (int i = 0; i < nPanes; i++)
	{
		BSTR bstr = NULL;
		sptrCC->GetPaneName(i, &bstr);
		_bstr_t bstrPaneName = bstr;
		::SysFreeString(bstr);
		if (bstrPaneName == _bstr_t(p))
		{
			nPaneNum = i;
			break;
		}
	}
	if (nPaneNum == -1) return false;
	strcpy(szPaneName,p);
	return true;
}*/


bool CTIFFilter::IsMultipage(IImage2 *pImage, IColorConvertor2 *pCC, const char *pszCCName)
{
	char szBuff[100];
	strcpy(szBuff,"FileFilters2\\");
	strcat(szBuff,pszCCName);
	return ::GetValueInt(::GetAppUnknown(), szBuff, "WriteMultipaged", 0)?true:false;
}

static const char *MakeDocName(char *pszDocName, int nPane, IColorConvertor2 *pCC, const char *pszCCName)
{
	strcpy(pszDocName, "VT5 Page:");
	strcat(pszDocName,pszCCName);
	strcat(pszDocName,":");
	if (nPane == -1)
		strcat(pszDocName,"Preview");
	else
	{
		BSTR bstrPaneName;
		pCC->GetPaneName(nPane, &bstrPaneName);
		_bstr_t sPaneName(bstrPaneName);
		::SysFreeString(bstrPaneName);
		if (sPaneName.length() == 0)
		{
			char szBuff[15];
			_itoa(nPane, szBuff, 10);
			strcat(pszDocName,szBuff);
		}
		else
			strcat(pszDocName,sPaneName);
	}
	return pszDocName;
};

static void MakePalette(uint16 *pRed, uint16 *pGreen, uint16 *pBlue, int nPaletteSize,
	int nPane, const char *szCCName, IUnknown *punkCC)
{
	IProvideHistColorsPtr sptrHist(punkCC);
	COLORREF clrArray[256],clrCurve;
	sptrHist->GetHistColors(nPane,clrArray,&clrCurve);
	if (nPaletteSize==256)
	{
		for (int i = 0; i < nPaletteSize; i++)
		{
			pRed[i] = GetRValue(clrArray[i])*256+i;
			pGreen[i] = GetGValue(clrArray[i])*256+i;
			pBlue[i] = GetBValue(clrArray[i])*256+i;
		}
	}
	else
	{
		for (int i = 0; i < nPaletteSize; i++)
		{
			int n = i*256/nPaletteSize;
			pRed[i] = GetRValue(clrArray[n])*256+n;
			pGreen[i] = GetGValue(clrArray[n])*256+n;
			pBlue[i] = GetBValue(clrArray[n])*256+n;
		}
	}
/*	for (int i = 0; i < nPaletteSize; i++)
	{
		pRed[i] = nPane==2||nPane==3||nPane==4?i*256:0;
		pGreen[i] = nPane==1||nPane==3||nPane==5?i*256:0;
		pBlue[i] = nPane==0||nPane==3||nPane==6?i*256:0;
	}*/
}

bool CTIFFilter::WritePageHdr(IImage4 *pImage, IColorConvertor2 *pCC, TIFF *tif, int nCX, int nCY,
	int nBitsPerSample, int nSamplesPerPixel, int nPhotometicInterpretation, const char *pszCCName,
	int nPage)
{
//	CImageData Descr(nPage==-1,pszCCName,nPage==0?pCC:NULL,nPage==0?pImage:NULL);
	CImageData Descr(nPage==-1,pszCCName,pCC,pImage);
	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, nCX);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, nCY);
	TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, nPhotometicInterpretation);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, nBitsPerSample);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, nSamplesPerPixel);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tif, TIFFTAG_SOFTWARE, VT5SOFTWARE);
	if (nPage == -1)
	{
		_bstr_t bstrName = GetName(pImage);
		bstrName += ": Preview";
		TIFFSetField(tif, TIFFTAG_DOCUMENTNAME, (const char *)bstrName);
	}
	else
	{
		TIFFSetField(tif, TIFFTAG_DOCUMENTNAME, (const char *)GetName(pImage));
		int nPanesCount;
		pImage->GetPanesCount(&nPanesCount);
		TIFFSetField(tif, TIFFTAG_PAGENUMBER, (uint16)nPage, (uint16)nPanesCount);
		BSTR bstr;
		pCC->GetPaneName(nPage, &bstr);
		_bstr_t bstrPageName(bstr);
		::SysFreeString(bstr);
		TIFFSetField(tif, TIFFTAG_PAGENAME, (const char*)bstrPageName);
	}
	Descr.Write(tif);
	return true;
}


bool CTIFFilter::WriteMultipageImage(IUnknown *punkImage, IUnknown *punkCC, TIFF *tif,
	int nCX, int nCY,const char *szCCName)
{
	IImage4Ptr	sptrI(punkImage);
	IColorConvertor2Ptr sptrCC(punkCC);
	IUnknownPtr sptrActiveView(_GetActiveView());
	BOOL bWrite16bitTiff = (BYTE)GetValueInt(GetAppUnknown(), "FileFilters2", "Write16bitTiff", 0);
	long nBitsPerSample = bWrite16bitTiff?sizeof(color)*8:8;
	char szSection[100];
	strcpy(szSection,"FileFilters2\\");
	strcat(szSection,szCCName);
	// Write RGB image page (same programs, PSP7.0 e.g., doesn't read image if first page 8-bit properly.
	WritePageHdr(sptrI, sptrCC, tif, nCX, nCY, 8, 3, PHOTOMETRIC_RGB, szCCName, -1);
	long nScanLineWidth = (3*nCX+3)/4*4;
/*	_ptr_t<byte> pScanLine8(nScanLineWidth);
	color* pRow = 0;
	byte* pRowMask = 0;
	StartNotification(nCY);
	for (long row = 0; row < nCY; row++)
	{
		sptrI->GetRowMask(row, &pRowMask);
		for(long c = 0; c < 3; c++)
		{
			sptrI->GetRow(row, c, &pRow);
			for(long x = 0; x < nCX; x++)
				pScanLine8[x*3+2-c] = ColorAsByte(pRow[x]);
		}
		if (TIFFWriteScanline(tif, pScanLine8, row) < 0) 
			return false;
		Notify(row);
	}
	FinishNotification();*/

	BITMAPINFOHEADER bi;
	memset(&bi, 0, sizeof(bi));
	bi.biSize = sizeof(bi);
	bi.biBitCount = 24;
	bi.biPlanes = 1;
	bi.biWidth = nCX;
	bi.biHeight = nCY;
	IColorConvertorExPtr sptrCCEx(punkCC);
	if (sptrCCEx != 0)
	{
		_ptr_t2<byte> pScanLine8(nScanLineWidth*nCY);
		sptrCCEx->ConvertImageToDIBRect(&bi, pScanLine8, _point(0,0), sptrI, _rect(0,0,nCX,nCY),
			_point(0,0), 1., _point(0,0), RGB(0,0,0), cidrExportImage, sptrActiveView);
		StartNotification(nCY);
		for (long row = 0; row < nCY; row++)
		{
			LPBYTE p = ((LPBYTE)pScanLine8)+nScanLineWidth*(nCY-1-row);
			for (int x = 0; x < nCX; x++)
			{
				BYTE t = p[3*x];
				p[3*x+0] = p[3*x+2];
				p[3*x+2] = t;
			}
			if (TIFFWriteScanline(tif, p, row) < 0) 
				return false;
			Notify(row);
		}
		FinishNotification();
	}

	TIFFWriteDirectory(tif);
	// Write grayscale pages
	int nNumPanes = 1;
	sptrI->GetPanesCount(&nNumPanes);
	BYTE clrBack = (BYTE)GetValueInt(GetAppUnknown(), "FileFilters2", "GrayBackground", 128);
	BOOL bColorMap = GetValueInt(GetAppUnknown(), szSection, "PanesWithPalette", FALSE);
	short nPhotometric = bColorMap?PHOTOMETRIC_PALETTE:PHOTOMETRIC_MINISBLACK;
	for (int c = 0; c < nNumPanes;  c++)
	{
		char szEntry[50];
		sprintf(szEntry, "WritePane%d", c);
		if (GetValueInt(GetAppUnknown(), szSection, szEntry, 1)==0)
			continue;
		if (IsPageEmpty(punkImage,c,szCCName))
			continue;
		WritePageHdr(sptrI, sptrCC, tif, nCX, nCY, nBitsPerSample, 1, nPhotometric, szCCName, c);
		if (bColorMap)
		{
			int nPaletteSize = 1<<nBitsPerSample;
			_ptr_t2<uint16> arRed(nPaletteSize),arGreen(nPaletteSize),arBlue(nPaletteSize);
			MakePalette(arRed, arGreen, arBlue, nPaletteSize, c, szCCName, punkCC);
			TIFFSetField(tif, TIFFTAG_COLORMAP, (uint16*)arRed, (uint16*)arGreen, (uint16*)arBlue); 
		}
		long nScanLineWidth = nCX;
		_ptr_t2<color> pScanLine16(nScanLineWidth);
		_ptr_t2<byte> pScanLine8(nScanLineWidth);
		color* pRow = 0;
		byte* pRowMask = 0;
		StartNotification(nCY);
		for (long row = 0; row < nCY; row++)
		{
			sptrI->GetRowMask(row, &pRowMask);
			sptrI->GetRow(row, c, &pRow);
			for(long x = 0; x < nCX; x++)
			{
				if (pRowMask[x])
					if (bWrite16bitTiff)
						pScanLine16[x] = pRow[x];
					else
						pScanLine8[x] = ColorAsByte(pRow[x]);
				else
					if (bWrite16bitTiff)
						pScanLine16[x] = ByteAsColor(clrBack);
					else
						pScanLine8[x] = clrBack;
			}
			if (bWrite16bitTiff)
			{
				if (TIFFWriteScanline(tif, (byte*)(color*)pScanLine16, row) < 0) 
					return false;
			}
			else
			{
				if (TIFFWriteScanline(tif, pScanLine8, row) < 0) 
					return false;
			}
			Notify(row);
		}
		FinishNotification();
		if (TIFFWriteDirectory(tif) < 0)
			break;
	}
	return true;
}

HRESULT _StoreToNamedData(IUnknown *punk, const _bstr_t &bstrSect, const _bstr_t &bstrKey, long l);

IUnknownPtr CTIFFilter::FindOrCreateImage(CImageData &ImageData)
{
	int nObjects;
	GetCreatedObjectCount(&nObjects);
	for (int i = 0; i < nObjects; i++)
	{
		IUnknownPtr punk,punkCC;
		GetCreatedObject(i, &punk);
		IImagePtr sptrI(punk);
		if (sptrI == 0) continue;
		sptrI->GetColorConvertor(&punkCC);
		IColorConvertor2Ptr sptrCC(punkCC);
		if (sptrCC == 0) continue;
		BSTR bstr;
		sptrCC->GetCnvName(&bstr);
		bool bOk = _bstr_t(bstr) == _bstr_t(ImageData.szCCName);
		::SysFreeString(bstr);
		if (!bOk) continue;
		INamedObject2Ptr sptrObj(punk);
		sptrObj->GetName(&bstr);
		_bstr_t bstrName(bstr);
		::SysFreeString(bstr);
		if (bstrName == ImageData.bstrDocName)
			return punk;
	}
	IUnknownPtr punk(CreateNamedObject("Image"), false);
	INamedObject2Ptr sptrObj(punk);
	sptrObj->SetName(ImageData.bstrDocName);
	if (ImageData.pszND == NULL && strcmp(ImageData.szCCName,"CGH")==0)
	{
		_StoreToNamedData(punk, KARYO_ACTIONROOT, CGH_PANE0, 1L);
		_StoreToNamedData(punk, KARYO_ACTIONROOT, CGH_PANE1, 1L);
		_StoreToNamedData(punk, KARYO_ACTIONROOT, CGH_PANE2, 1L);
		_StoreToNamedData(punk, KARYO_ACTIONROOT, CGH_PANE3, 1L);
	}
	return punk;
}

bool CTIFFilter::ReadPageOfImage(TIFF *tif, ULONG imageWidth, ULONG imageLength,
	ULONG LineSize,	int PhotometricInterpretation, unsigned int BitsPerSample,
	CImageData &ImageData)
{
	IUnknownPtr punk = FindOrCreateImage(ImageData);
	IImage4Ptr sptrI(punk);
	if (sptrI == 0)	return false;
	char szError[255];
	IUnknownPtr punkCC(GetCCByName(ImageData.szCCName));
	IColorConvertor2Ptr sptrCC(punkCC);
	long nColorPanes = 3;
	::LoadString(App::handle(), IDS_BAD_TIFF_FORMAT, (char*)szError, 255);
	if (PhotometricInterpretation != PHOTOMETRIC_MINISWHITE && PhotometricInterpretation != PHOTOMETRIC_MINISBLACK &&
		PhotometricInterpretation != PHOTOMETRIC_PALETTE)
		return false;
	int nPaneNum = ImageData.nPage;//PaneByName(sptrI, sptrCC, ImageData.bstrPageName);
	if (nPaneNum==-1)
		return false;
	int cx,cy;
	sptrI->GetSize(&cx,&cy);
	if (cx != (int)imageWidth || cy != (int)imageLength)
		sptrI->CreateImage(imageWidth, imageLength, _bstr_t(ImageData.szCCName),ImageData.nPanes);
	bool bCanUse16bit = (BitsPerSample == 16 && sizeof(color) == 2);
	_ptr_t2<unsigned short> buf16;
	_ptr_t2<byte> buf;
	if(BitsPerSample == 16)
		buf16.alloc(imageWidth*nColorPanes);
	else
		buf.alloc(LineSize);
	color* pRow = 0;
	StartNotification(imageLength);
	for (unsigned int row = 0; row < imageLength; row ++) 
	{    
		if(BitsPerSample == 16)
		{
			if(TIFFReadScanline(tif, (byte*)(color*)buf16, row) < 0)
				return false;
			sptrI->GetRow(row, nPaneNum, &pRow);
			for (unsigned int i=0;i< imageWidth;i++)
			{
				if(bCanUse16bit)
					pRow[i] = buf16[i];
				else
					pRow[i] = buf16[i]>>8; 
			}
		}
		else
		{
			if(TIFFReadScanline(tif, buf, row) < 0)
				return false;
			sptrI->GetRow(row, nPaneNum, &pRow);
			for (unsigned int i=0;i< imageWidth;i++)
				pRow[i] = ByteAsColor(buf[i]); 
		}
		Notify(row);
	}
	if (ImageData.pszND)
		RestoreNamedData(punk, _bstr_t("\\"), ImageData.pszND);
	FinishNotification();
	return true;
}

HRESULT SafeSetValue(IUnknown *punk, const _bstr_t bstrSect, const _bstr_t bstrKey, _variant_t var)
{
	INamedDataObject2Ptr ptrNDO(punk);
	if (ptrNDO == 0) return S_FALSE;
	INamedDataPtr sptrNDO = punk;
	if (sptrNDO == 0)
	{
		ptrNDO->InitAttachedData();
		sptrNDO = ptrNDO;
	}
	if (sptrNDO == 0) return E_FAIL;
	sptrNDO->SetupSection(bstrSect);
	sptrNDO->SetValue(bstrKey, var);
	return S_OK;
}

HRESULT _StoreToNamedData(IUnknown *punk, const _bstr_t &bstrSect, const _bstr_t &bstrKey, long l)
{
	INamedDataObject2Ptr ptrNDO(punk);
	if (ptrNDO == 0) return S_FALSE;
	INamedDataPtr sptrNDO = punk;
	if (sptrNDO == 0)
	{
		ptrNDO->InitAttachedData();
		sptrNDO = ptrNDO;
	}
	sptrNDO->SetupSection(bstrSect);
	sptrNDO->SetValue(bstrKey, _variant_t(l));
	return S_OK;
}

