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


static int checkcmap(int n, uint16* r, uint16* g, uint16* b)
{
    while (n-- > 0)
        if (*r++ >= 256 || *g++ >= 256 || *b++ >= 256)
        return (16);
    
    return (8);
}

void __TIFFWarningHandle(const char* module, const char* fmt,  char* qq) 
{
}


CTIFFilter::CTIFFilter()
{
	m_bstrDocumentTemplate = _bstr_t("Image");

	char szFilterName[255];	szFilterName[0] = 0;
	::LoadString(App::handle(), IDS_TIF_FILTERNAME, szFilterName, 255);
	m_bstrFilterName = szFilterName;//_bstr_t("TIFF files");
	m_bstrFilterExt = _bstr_t(".tif\n.tiff");
	AddDataObjectType(szArgumentTypeImage);
	AddDataObjectType(szArgumentTypeBinaryImage);
}

CTIFFilter::~CTIFFilter()
{
}

bool CTIFFilter::ReadBinary(IUnknown *punk, TIFF *tif, ULONG imageWidth, ULONG imageLength, ULONG LineSize)
{
	::LoadString(App::handle(), IDS_TRACE, (char*)m_strCompression, 255);
	int nColorPanes = 1;
	IBinaryImagePtr	sptrI(punk);
	if (punk)
		punk->Release();
	if(sptrI == 0)
		return false;
	sptrI->CreateNew(imageWidth, imageLength);
	_ptr_t2<byte> buf(LineSize);
	byte* pRow = 0;
	StartNotification(imageLength);
	for (unsigned int row = 0; row < imageLength; row ++) 
	{    
		if(TIFFReadScanline(tif, buf, row) < 0)
			return false;
		sptrI->GetRow(&pRow, row, FALSE);
		memcpy(pRow, buf, LineSize);
		Notify(row);
	}
	FinishNotification();
	return true;
}

bool CTIFFilter::ReadImage(IUnknown *punk, TIFF *tif, ULONG imageWidth, ULONG imageLength, ULONG LineSize,
	int PhotometricInterpretation, unsigned int BitsPerSample)
{
	IImagePtr	sptrI(punk);
	if (punk)
		punk->Release();
	if(sptrI == 0)
		return false;
	char szCC[255];
	char szError[255];
	//now we support these PhotometricInterpretation
	//image is Gray
	//image is RGB
	//image have a color palette, RGB
	//image is YUV
	long nColorPanes = 3;
	::LoadString(App::handle(), IDS_BAD_TIFF_FORMAT, (char*)szError, 255);
	switch(PhotometricInterpretation)
	{
	case PHOTOMETRIC_MINISWHITE:
	case PHOTOMETRIC_MINISBLACK:
		strcpy(szCC, "GRAY");
		nColorPanes = 1;
		break;
	case PHOTOMETRIC_RGB:
	case PHOTOMETRIC_PALETTE:
		strcpy(szCC, "RGB");
		break;
	case PHOTOMETRIC_YCBCR:
		strcpy(szCC, "YUV");
		break;
	default:
		::MessageBox(NULL, szError, "Error", MB_OK);
		return false;
	}
	sptrI->CreateImage(imageWidth, imageLength, _bstr_t(szCC), -1);
	bool bPalette = false;
	bool   Palette16Bits = false;          
	uint16* pPalette[3];
	if (PhotometricInterpretation == 3)
	{
		//read palette    
		bPalette = true;
		TIFFGetField(tif, TIFFTAG_COLORMAP, &pPalette[0], &pPalette[1], &pPalette[2]); 
		//Is the palette 16 or 8 bits ?
		if (checkcmap(1<<BitsPerSample, pPalette[0], pPalette[1], pPalette[2]) == 16) 
			Palette16Bits = true;
		else
			Palette16Bits = false;
   }
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
			for(long c = 0; c < nColorPanes; c++)
			{
				sptrI->GetRow(row, c, &pRow);
				for (unsigned int i=0;i< imageWidth;i++)
				{
					if(bPalette)
					{
						if (Palette16Bits)
							pRow[i] = pPalette[c][buf16[i]];
						else
						{
							if(bCanUse16bit)
								pRow[i] = pPalette[c][buf16[i]];
							else
								pRow[i] = pPalette[c][buf16[i]]>>8;
						}
					}
					else
					{
						if(bCanUse16bit)
						{
							pRow[i] = buf16[i*nColorPanes+c];
						}
						else
						{
							pRow[i] = buf16[i*nColorPanes+c]>>8; 
						}
					}
				}
			}
		}
		else
		{
			if(TIFFReadScanline(tif, buf, row) < 0)
				return false;
			for(long c = 0; c < nColorPanes; c++)
			{
				sptrI->GetRow(row, c, &pRow);
				for (unsigned int i=0;i< imageWidth;i++)
				{
					if(bPalette)
					{
						if (Palette16Bits)
							pRow[i] = pPalette[c][buf[i]];
						else
							pRow[i] = ByteAsColor(pPalette[c][buf[i]]);
					}
					else
					{
						pRow[i] = ByteAsColor(buf[i*nColorPanes+c]); 
					}
				}
			}
		}
		Notify(row);
	}
	FinishNotification();
	return true;
}

IUnknown *CTIFFilter::GetCGHImage()
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
		if (_bstr_t("CGH") == _bstr_t(bstr))
		{
			IUnknown *punk1 = punk;
			punk1->AddRef();
			return punk1;
		}
	}
	IUnknown *punk = CreateNamedObject("Image");
	if (punk)
	{
		_StoreToNamedData(punk, KARYO_ACTIONROOT, CGH_PANE0, 1L);
		_StoreToNamedData(punk, KARYO_ACTIONROOT, CGH_PANE1, 1L);
		_StoreToNamedData(punk, KARYO_ACTIONROOT, CGH_PANE2, 1L);
		_StoreToNamedData(punk, KARYO_ACTIONROOT, CGH_PANE3, 1L);
	}
	return punk;
}

bool CTIFFilter::ReadCGHPane(IUnknown *punk, TIFF *tif, ULONG imageWidth, ULONG imageLength, ULONG LineSize,
	int PhotometricInterpretation, unsigned int BitsPerSample, const char *szDocName)
{
	if (strcmp(szDocName, "VT5 CGH RGB")==0) return true; // Used only for PSP!
	int nPane = 0;
	sscanf(szDocName, "VT5 CGH Pane%d", &nPane);
	IImagePtr	sptrI(punk);
	if (punk)
		punk->Release();
	if(sptrI == 0)
		return false;
	char szError[255];
	long nColorPanes = 3;
	::LoadString(App::handle(), IDS_BAD_TIFF_FORMAT, (char*)szError, 255);
	if (PhotometricInterpretation != PHOTOMETRIC_MINISWHITE && PhotometricInterpretation != PHOTOMETRIC_MINISBLACK &&
		PhotometricInterpretation != PHOTOMETRIC_PALETTE)
		return false;
	int cx,cy;
	sptrI->GetSize(&cx,&cy);
	if (cx != (int)imageWidth || cy != (int)imageLength)
		sptrI->CreateImage(imageWidth, imageLength, _bstr_t("CGH"),-1);
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
			sptrI->GetRow(row, nPane, &pRow);
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
			sptrI->GetRow(row, nPane, &pRow);
			for (unsigned int i=0;i< imageWidth;i++)
				pRow[i] = ByteAsColor(buf[i]); 
		}
		Notify(row);
	}
	FinishNotification();
	return true;
}

static BOOL IsCGHImage()
{
	IUnknownPtr punkCC(GetCCByName("CGH"));
	IColorConvertor2Ptr sptrCC(punkCC);
	BSTR bstr;
	sptrCC->GetCnvName(&bstr);
	return _bstr_t(bstr) == _bstr_t("CGH");
}

bool CTIFFilter::ReadFile( const char *pszFileName )
{
	bool          bMPMessage = false;
	byte          *buf = 0;
	unsigned short *buf16 = 0;
	TIFF		  *tif = 0;
	try
	{
		unsigned long imageLength = 0;  
		unsigned long imageWidth = 0; 
		unsigned int  BitsPerSample = 0;
		unsigned long LineSize = 0;
		unsigned int  SamplePerPixel = 0;
		unsigned long RowsPerStrip = 0;  
		unsigned long Compression = 0;
		int           PhotometricInterpretation = 0;
		BOOL bReadCGH = GetValueInt(GetAppUnknown(), "FileFilters2", "ReadCghTiff", TRUE) && IsCGHImage();
		
	
		TIFFSetWarningHandler(__TIFFWarningHandle);  

		tif = TIFFOpen(pszFileName, "r");
		if(!tif)
			return false;

		m_nNumPages = TIFFNumberOfDirectories(tif);

		long idx = 1;
		do
		{
			
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);  
			TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &BitsPerSample);
			//TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &RowsPerStrip);   
			TIFFGetField(tif, TIFFTAG_COMPRESSION, &Compression);
			TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &PhotometricInterpretation);

			char* szDocName = 0;
			TIFFGetField(tif, TIFFTAG_DOCUMENTNAME, &szDocName);

			
			LineSize = TIFFScanlineSize(tif); //Number of byte in ine line
			SamplePerPixel = (unsigned int) (LineSize/imageWidth);
			long nColorPanes = 3;
			if (bReadCGH && szDocName && strcmp(szDocName, "VT5 CGH RGB") == 0)
				continue;
			CImageData ImageData;
			ImageData.Read(tif);
			if (ImageData.bVT5)
			{
				if (ImageData.bPreview) // Multipage preview
					continue;
				if (ImageData.IsPageOfMultipage())
				{
					if (!IsConvertorExists(ImageData.szCCName))
					{
						char szBuff1[255],szBuff2[255];
						LoadString(app::get_instance()->m_hInstance, IDS_NO_COLORCONVERTOR, szBuff1, 255);
						sprintf(szBuff2, szBuff1, (char *)ImageData.szCCName);
						if (!bMPMessage)
							VTMessageBox(szBuff2, "VT5", MB_OK|MB_ICONEXCLAMATION);
						bMPMessage = true;
						continue;
					}
					if (!ReadPageOfImage(tif,imageWidth,imageLength,LineSize,PhotometricInterpretation,
						BitsPerSample,ImageData))
					{
						TIFFClose(tif);
						return false;
					}
					continue;
				}
			}
			bool bIsBinary = szDocName && strcmp(szDocName, "VT5 binary") == 0;
			bool bCGH = bReadCGH && szDocName && strncmp(szDocName, "VT5 CGH", 7) == 0;
			IUnknown* punk = bCGH?GetCGHImage():CreateNamedObject( GetDataObjectType(bIsBinary?1:0) );

			// get object name from path and check it's exists
			//char*	strObjName = ::GetObjectNameFromPath(pszFileName);
			TCHAR szName[_MAX_PATH];
			TCHAR drive[_MAX_DRIVE];
			TCHAR dir[_MAX_PATH];
			TCHAR ext[_MAX_PATH];
			_splitpath(pszFileName, drive, dir, szName, ext);

			if (ImageData.bstrPageName.length() > 0)
				sprintf(szName, "%s %s", szName, (const char*)ImageData.bstrPageName);
			else if(m_nNumPages > 1)
				sprintf(szName, "%s_page%d", szName, idx);

			_bstr_t bstrName(szName);
			long	NameExists = 0;
			if( m_sptrINamedData != 0 )
				m_sptrINamedData->NameExists(bstrName, &NameExists);
			// if new name not exists allready
			if (!NameExists && punk && !bCGH)
			{
				// set this name to object
				INamedObject2Ptr sptrObj(punk);
				sptrObj->SetName(bstrName);
			}

			char szError[255];
			//now we support these Compression
			//CCITT Group 3 & 4 algorithms
			//PackBits algorithm
			//ThunderScan 4-bit RLE algorithm
			//NeXT 2-bit RLE algorithm
			//JPEG DCT algorithms
			::LoadString(App::handle(), IDS_BAD_TIFF_COMPRESSION, (char*)szError, 255);
			switch(Compression)
			{
			case COMPRESSION_NONE:
				 ::LoadString(App::handle(), IDS_NoCompression, (char*)m_strCompression, 255);
				break;
			case COMPRESSION_CCITTFAX3:
				::LoadString(App::handle(), IDS_Group3Compression, (char*)m_strCompression, 255);
				break;
			case COMPRESSION_CCITTFAX4:
				::LoadString(App::handle(), IDS_Group4Compression, (char*)m_strCompression, 255);
				break;
			case COMPRESSION_JPEG:
				::LoadString(App::handle(), IDS_JPEGCompression, (char*)m_strCompression, 255);
				break;
			case COMPRESSION_NEXT:
				::LoadString(App::handle(), IDS_NEXT_COMPRESSION, (char*)m_strCompression, 255);
				break;
			case COMPRESSION_PACKBITS:
				::LoadString(App::handle(), IDS_PACKBITS_COMPRESSION, (char*)m_strCompression, 255);
				break;
			case COMPRESSION_LZW:
				::LoadString(App::handle(), IDS_LZWCompression, (char*)m_strCompression, 255);
				break;
			case COMPRESSION_THUNDERSCAN:
				::LoadString(App::handle(), IDS_THUNDERSCAN_COMPRESSION, (char*)m_strCompression, 255);
				break;
			default:
				::MessageBox(NULL, szError, "Error", MB_OK);
			   return false;
			}
			if (bCGH)
			{
				if (!ReadCGHPane(punk,tif,imageWidth,imageLength,LineSize,PhotometricInterpretation,
					BitsPerSample,szDocName))
				{
					TIFFClose(tif);
					return false;
				}
			}
			else if(bIsBinary)
			{
				if (!ReadBinary(punk,tif,imageWidth,imageLength,LineSize))
				{
					TIFFClose(tif);
					return false;
				}
			}
			else
			{
				if (!ReadImage(punk,tif,imageWidth,imageLength,LineSize,PhotometricInterpretation,
					BitsPerSample))
				{
					TIFFClose(tif);
					return false;
				}
			}

			if(idx == 1)
			{
				m_nWidth = imageWidth;
				m_nHeight = imageLength;
				m_nDepth = BitsPerSample*nColorPanes;
				m_nColors = 1<<m_nDepth;
			}

			idx++;

		}while(TIFFReadDirectory(tif) == TRUE);
		
	}
	catch(TerminateLongException* e)
	{
		if(buf)
			delete buf;
		if(buf16)
			delete buf16;


		TIFFClose(tif);

		e->Delete();
		return false;
	}

	TIFFClose(tif);
	return true;
}

bool CTIFFilter::WriteBinary(IUnknown *punk, TIFF *tif)
{
	IBinaryImagePtr	sptrI(punk);
	if(sptrI == 0)
		return false;
	punk->Release();
	int nCX = 0, nCY = 0;
	sptrI->GetSizes(&nCX, &nCY);
	const long nBitsPerSample = 8;
	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, nCX);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, nCY);
	TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_PALETTE);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, nBitsPerSample);
	TIFFSetField(tif, TIFFTAG_DOCUMENTNAME, "VT5 binary");
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	//long nScanLineWidth = ((nCX+7)/8)+4;
	long nScanLineWidth = nCX;
	_ptr_t2<byte> pBinScanLine(nScanLineWidth);
	long nPaletteSize = 1<<nBitsPerSample;
	uint16 *rmap = (uint16*) malloc(nPaletteSize*sizeof(uint16));
	uint16 *gmap = (uint16*) malloc(nPaletteSize*sizeof(uint16));
	uint16 *bmap = (uint16*) malloc(nPaletteSize*sizeof(uint16));
	ZeroMemory(rmap, nPaletteSize*sizeof(uint16));
	ZeroMemory(gmap, nPaletteSize*sizeof(uint16));
	ZeroMemory(bmap, nPaletteSize*sizeof(uint16));
	DWORD dwBack = GetValueColor(GetAppUnknown(), "\\Binary", "BackColor", RGB(0,0,0));
	DWORD dwDefColor = RGB(255,255,0);
	long nCounter = GetValueInt(GetAppUnknown(), "\\Binary", "IndexedColorsCount", 1);
	int nMul = GetValueInt(GetAppUnknown(), "\\FileFilters2", "TiffBinaryPalette16", 1)?256:1;
	nCounter = max(0, min(nPaletteSize, nCounter));
	char sz[256];
	DWORD dwColor = 0;
	rmap[0] = GetRValue(dwBack)*nMul;
	gmap[0] = GetGValue(dwBack)*nMul;
	bmap[0] = GetBValue(dwBack)*nMul;
	for(long i = 1; i < nPaletteSize; i++)
	{
		if(i < nCounter)
		{
			sprintf(sz, "Fore_%d", i-1);
			 dwColor = GetValueColor(GetAppUnknown(), "\\Binary", sz, dwDefColor);
		}
		else
			dwColor = dwDefColor;

		rmap[i] = GetRValue(dwColor)*nMul;
		gmap[i] = GetGValue(dwColor)*nMul;
		bmap[i] = GetBValue(dwColor)*nMul;
	}
	TIFFSetField(tif, TIFFTAG_COLORMAP, rmap, gmap, bmap);
	free(rmap);
	free(gmap);
	free(bmap);
	byte* pRow = 0;
	StartNotification(nCY);
	for (long row = 0; row < nCY; row++)
	{
		sptrI->GetRow(&pRow, row, FALSE);

		for(long x = 0; x < nCX; x++)
			pBinScanLine[x] = pRow[x];
		if (TIFFWriteScanline(tif, pBinScanLine, row) < 0) 
			return false;
		Notify(row);
	}
	FinishNotification();
	return true;
}

bool CTIFFilter::WriteCGHImage(IUnknown *punkImage, IUnknown *punkCC, TIFF *tif, int nCX, int nCY)
{
	IImage2Ptr	sptrI(punkImage);
	IColorConvertor2Ptr sptrCC(punkCC);
	BOOL bWrite16bitTiff = (BYTE)GetValueInt(GetAppUnknown(), "FileFilters2", "Write16bitTiff", 0);
	long nBitsPerSample = bWrite16bitTiff?sizeof(color)*8:8;
	// Write RGB image page (same programs, PSP7.0 e.g., doesn't read image if first page 8-bit properly.
	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, nCX);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, nCY);
	TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, nBitsPerSample);
	TIFFSetField(tif, TIFFTAG_DOCUMENTNAME, "VT5 CGH RGB");
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	long nScanLineWidth = 3*nCX;
	_ptr_t2<color> pScanLine16(nScanLineWidth);
	_ptr_t2<byte> pScanLine8(nScanLineWidth);
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
			{
				if (bWrite16bitTiff)
					pScanLine16[x*3+2-c] = pRow[x];
				else
					pScanLine8[x*3+2-c] = ColorAsByte(pRow[x]);
			}
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
	TIFFWriteDirectory(tif);
	// Write 8 - bit panes
	int nNumPanes = 1;
	// sptrCC->GetColorPanesCount(&nNumPanes);
	nNumPanes = ::GetImagePaneCount( punkImage ); 
	BYTE clrBack = (BYTE)GetValueInt(GetAppUnknown(), "FileFilters2", "GrayBackground", 128);
	BOOL bColorMap = GetValueInt(GetAppUnknown(), "FileFilters2", "SaveCghInTiffWithPalette", FALSE);
	short nPhotometric = bColorMap?PHOTOMETRIC_PALETTE:PHOTOMETRIC_MINISBLACK;
	for (int c = 0; c < nNumPanes;  c++)
	{
		char szDocName[255];
		sprintf(szDocName, "VT5 CGH Pane%d", c);
		TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
		TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, nCX);
		TIFFSetField(tif, TIFFTAG_IMAGELENGTH, nCY);
		TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
		TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, nPhotometric);
		TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, nBitsPerSample);
		TIFFSetField(tif, TIFFTAG_DOCUMENTNAME, szDocName);
		TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
		TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		if (bColorMap)
		{
			int nPaletteSize = 1<<nBitsPerSample;
			_ptr_t2<uint16> arRed(nPaletteSize),arGreen(nPaletteSize),arBlue(nPaletteSize);
			for (int i = 0; i < nPaletteSize; i++)
			{
				arRed[i] = c==2||c==3||c==4?i*256:0;
				arGreen[i] = c==1||c==3||c==5?i*256:0;
				arBlue[i] = c==0||c==3||c==6?i*256:0;
			}
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

bool CTIFFilter::WriteImage(IUnknown *punk, TIFF *tif)
{
	IImage2Ptr	sptrI(punk);
	if(sptrI == 0)
		return false;
	punk->Release();
	int nCX = 0, nCY = 0;
	sptrI->GetSize(&nCX, &nCY);
	IUnknown* punkCC = 0;
	sptrI->GetColorConvertor(&punkCC );
	IColorConvertor2Ptr sptrCC(punkCC);
	if(punkCC)
		punkCC->Release();
	char szCCName[255];
	if(sptrCC != 0)
	{
		BSTR bstrName = 0;
		sptrCC->GetCnvName(&bstrName);
		W2A(bstrName, szCCName, 255);
		::SysFreeString(bstrName);
	}
	if (IsMultipage(sptrI, sptrCC, szCCName))
		return WriteMultipageImage(sptrI,sptrCC,tif,nCX,nCY,szCCName);
/*	if (!strcmp(szCCName,"CGH"))
		return WriteCGHImage(sptrI,sptrCC,tif,nCX,nCY);*/
#ifdef _Write_16_bit_TIFF
	long nBitsPerSample = sizeof(color)*8;
#else
	long nBitsPerSample = 8;
#endif
	int nNumPanes = 3;
	BYTE clrBack[3];
	short nPhotometric = 0;
	if(!strcmp(szCCName, "RGB"))
	{
		nPhotometric =  PHOTOMETRIC_RGB;
		COLORREF clr = GetValueColor(GetAppUnknown(), "FileFilters2", "RGBBackground", RGB(128,128,128));
		clrBack[0] = GetRValue(clr);
		clrBack[1] = GetGValue(clr);
		clrBack[2] = GetBValue(clr);
	}
	else if(!strcmp(szCCName, "YUV"))
	{
		nPhotometric = PHOTOMETRIC_YCBCR;
		COLORREF clr = GetValueColor(GetAppUnknown(), "FileFilters2", "YUVBackground", RGB(128,128,128));
		clrBack[0] = GetRValue(clr);
		clrBack[1] = GetGValue(clr);
		clrBack[2] = GetBValue(clr);
	}
	else if(!strcmp(szCCName, "GRAY"))
	{
		nPhotometric = PHOTOMETRIC_MINISBLACK;
		nNumPanes = 1;
		clrBack[0] = (BYTE)GetValueInt(GetAppUnknown(), "FileFilters2", "GrayBackground", 128);
	}
	else
	{
		char szError[255];
		::LoadString(App::handle(), IDS_CANT_SAVE_COLORSPACE, (char*)szError, 255);
		::MessageBox(NULL, szError, "Error", MB_OK);
		return false;
	}
	//long rowsperstrip = (nBitsPerSample*1024)/(((nBitsPerSample*nNumPanes*nCX + 15) >> 3) & ~1);
	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, nCX);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, nCY);
	TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, nPhotometric);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, nBitsPerSample);
	TIFFSetField(tif, TIFFTAG_DOCUMENTNAME, "VT5");
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, nNumPanes);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	long nScanLineWidth = nNumPanes*nCX;
#ifdef _Write_16_bit_TIFF
	_ptr_t2<color> pScanLine(nScanLineWidth);
#else
	_ptr_t2<byte> pScanLine(nScanLineWidth);
#endif
	color* pRow = 0;
	byte* pRowMask = 0;
	StartNotification(nCY);
	for (long row = 0; row < nCY; row++)
	{
		sptrI->GetRowMask(row, &pRowMask);
		for(long c = 0; c < nNumPanes; c++)
		{
			sptrI->GetRow(row, c, &pRow);
			for(long x = 0; x < nCX; x++)
			{
				if (pRowMask[x])
#ifdef _Write_16_bit_TIFF
					pScanLine[x*nNumPanes + c] = pRow[x];
#else
					pScanLine[x*nNumPanes + c] = ColorAsByte(pRow[x]);
#endif
				else
#ifdef _Write_16_bit_TIFF
					pScanLine[x*nNumPanes + c] = ByteAsColor(clrBack[c]);
#else
					pScanLine[x*nNumPanes + c] = clrBack[c];
#endif
			}
		}
		if (TIFFWriteScanline(tif, (byte*)(color*)pScanLine, row) < 0) 
			return false;
		Notify(row);
	}
	FinishNotification();
	return true;
}

bool CTIFFilter::WriteFile( const char *pszFileName )
{
	TIFF		  *tif = 0;
#ifdef _Write_16_bit_TIFF
	color* pScanLine = 0;
#else
	byte* pScanLine = 0;
#endif

	byte* pBinScanLine = 0;
	try
	{
		TIFFSetWarningHandler(__TIFFWarningHandle);  

		if ((tif = TIFFOpen(pszFileName, "w")) == NULL) 
			return false;
		IUnknown* punk = 0;        

		// convert object type to BSTR
		_bstr_t bstrType( GetDataObjectType(0) );
		// get active object in which type == m_strObjKind
		if (!(m_bObjectExists && m_sptrAttachedObject != NULL) && m_sptrIDataContext != NULL)
		{
			m_sptrIDataContext->GetActiveObject( bstrType, &punk );

			INamedDataObject2Ptr	ptrN( punk );
			if( ptrN != 0 )
			{
				IUnknown	*punkP = 0;
				ptrN->GetParent( &punkP );

				if( punkP )
				{
					punk->Release();
					punk = punkP;
				}
			}
		}
		else
		{
			punk = (IUnknown*)m_sptrAttachedObject.GetInterfacePtr();
			punk->AddRef();
		}
		if( !punk )
		{
			TIFFClose(tif);
			return false;
		}


		bool bIsBinary = false;
		sptrINamedDataObject	sptrO( punk );
		BSTR	bstrKind = 0;
		if(sptrO != 0)
		{
			sptrO->GetType(&bstrKind);
			bIsBinary = (_bstr_t(bstrKind) == _bstr_t(szArgumentTypeBinaryImage));
			::SysFreeString(bstrKind);
		}

		if(bIsBinary)
		{
			if (!WriteBinary(punk,tif))
			{
				TIFFClose(tif);
				return false;
			}
		}
		else
		{
			if (!WriteImage(punk,tif))
			{
				TIFFClose(tif);
				return false;
			}
		}
	
	}
	catch(TerminateLongException* e)
	{
		e->Delete();
		if(pScanLine)
			delete pScanLine;
		if(pBinScanLine)
			delete pBinScanLine;
		TIFFFlushData(tif);
		TIFFClose(tif);
		return false;
	}

	TIFFFlushData(tif);
	TIFFClose(tif);
	return true;
}

bool CTIFFilter::_InitNew()
{
	return true;
}


