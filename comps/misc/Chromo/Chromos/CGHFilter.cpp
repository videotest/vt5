#include "stdafx.h"
#include "resource.h"
#include "CGHFilter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CCGHFileFilter, CCmdTargetEx);

// {BFF20644-EF0A-4515-9E8B-DE7BD7EC76EB}
GUARD_IMPLEMENT_OLECREATE(CCGHFileFilter, "Chromos.CGHFileFilter", 
0xbff20644, 0xef0a, 0x4515, 0x9e, 0x8b, 0xde, 0x7b, 0xd7, 0xec, 0x76, 0xeb);


const char* szWidth		= "Width";
const char* szHeight	= "Height";
const char* szColors	= "Colors";
const char* szDepth		= "Depth";
const char* szNumPages	= "NumPages";
const char* szCompress	= "Compress";


CCGHFileFilter::CCGHFileFilter()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nColors = 0;
	m_nDepth = 0;

	m_strDocumentTemplate = _T("Image");
	m_strFilterName.LoadString(IDS_CGHFILTERNAME);
	m_strFilterExt = _T(".seq");
	AddDataObjectType(szTypeImage);
	m_strCompression = "";
}

void CCGHFileFilter::DoAttachPaneData(IImage *pimg, int nPane, LPBITMAPINFOHEADER pbi, LPBYTE lpData)
{
	ASSERT(pbi->biBitCount == 8);
	DWORD dwRow = ((pbi->biWidth+3)>>2)<<2;
	for (int y = 0; y < pbi->biHeight; y++)
	{
		color *pRow;
		pimg->GetRow(y, nPane, &pRow);
		for (int x = 0; x < pbi->biWidth; x++)
			pRow[x] = ((color)lpData[x])<<8;
		lpData += dwRow;
	}
}

bool CCGHFileFilter::ReadBmp(const char *pszFileName, IUnknown *punk, int nPane, LPCTSTR pszCC)
{
	try
	{
	CFile	file( pszFileName, CFile::modeRead );
	BITMAPFILEHEADER		bfh;
	if( file.Read( &bfh, sizeof( bfh ) )!= sizeof( bfh ) )
		return false;
	if( bfh.bfType != ((int( 'M' )<<8)|int( 'B' )) )
		return false;
	//read BITMAPINFOHEADER
	pointer<byte>	ptrB( sizeof( BITMAPINFOHEADER )+1024 );
	BITMAPINFOHEADER	*pbi = (BITMAPINFOHEADER*)(byte*)ptrB;
	if( file.Read( pbi, sizeof( BITMAPINFOHEADER ) )!= sizeof( BITMAPINFOHEADER ) )
		return false;
	int	cx = pbi->biWidth;
	int	cy = pbi->biHeight;
	int	cx4;
	long	nBitCount = pbi->biBitCount;
//calc line length
	if( nBitCount == 1 )
		cx4 = ((cx+7)/8+3)/4*4;
	else if( nBitCount == 2 )
		cx4 = ((cx+3)/4+3)/4*4;
	else if( nBitCount == 4 )
		cx4 = ((cx+1)/2+3)/4*4;
	else if( nBitCount == 8 )
		cx4 = (cx+3)/4*4;
	/*else if( nBitCount == 15 )
		cx4 = (cx*2+3)/4*4;
	else if( nBitCount == 16 )
		cx4 = (cx*2+3)/4*4;*/
	else if( nBitCount == 24 )
		cx4 = (cx*3+3)/4*4;
	else
		return false;	//unsupported format
	if (nBitCount != 8)
		return false;
	if (nPane == 0)
	{
		m_nWidth = cx;
		m_nHeight = cy;
	}
	else if (m_nWidth != cx || m_nHeight != cy)
		return false;
	m_nDepth = nBitCount;
	m_nColors = 1<<nBitCount;
	switch(pbi->biCompression)
	{
	case BI_RGB:
		m_strCompression.LoadString(IDS_BI_RGB);
		break;
	case BI_RLE8:
		m_strCompression.LoadString(IDS_BI_RLE8);
		break;
	case BI_RLE4:
		m_strCompression.LoadString(IDS_BI_RLE4);
		break;
	case BI_BITFIELDS:
		m_strCompression.LoadString(IDS_BI_BITFIELDS);
		break;
	//case BI_JPEG:
	//	m_strCompression.LoadString(IDS_BI_JPEG);
	//	break;
	default:
		m_strCompression.LoadString(IDS_UndefinedCompression);
		break;
	}
	long	nPalSize = (nBitCount <= 8) ? 1<<nBitCount : 0;
	if( nPalSize != 0 )
	{
		if( file.Read( pbi+1, nPalSize*4 )!= nPalSize*4 )
			return false;
	}
	long	nFileOffset = (bfh.bfOffBits-sizeof(bfh))-sizeof( BITMAPINFOHEADER )-nPalSize*4;
	long offBits = (WORD)pbi->biSize + nPalSize * sizeof(RGBQUAD);
	if( bfh.bfOffBits == 0 )
		nFileOffset = 0;
    if (pbi->biSizeImage == 0)
    {
		pbi->biSizeImage = ((((pbi->biWidth * (DWORD)pbi->biBitCount) + 31) & ~31) >> 3)
			* pbi->biHeight;
		//pbi->biSizeImage = cx4*cy;
    }
	long	nFileSize1 = cx4*cy+nFileOffset;
	long	nFileSize = pbi->biSizeImage;
	if(pbi->biCompression==BI_RGB) nFileSize = nFileSize1; //вычисленный размер надежнее прописанного в хедере :-(
	pointer<byte> ptr( nFileSize );
	if( ptr==0 )
		return false;
	::ZeroMemory( ptr, nFileSize );
	if(bfh.bfOffBits)
		file.Seek(bfh.bfOffBits,CFile::begin);
	int nread = file.Read( ptr, nFileSize );
	ASSERT( nread == nFileSize );
	CImageWrp	sptrI(punk);
	if (nPane == 0)
		sptrI->CreateImage(m_nWidth, m_nHeight, _bstr_t(pszCC),-1);
	if(pbi->biCompression!=BI_RGB)
	{
		int cx4_1=(cx*3+3)/4*4;
		int nFileSize1=cx4_1*cy;
		pointer<byte> ptr1( nFileSize1 );
		pointer<byte>	ptrB1( sizeof( BITMAPINFOHEADER )+1024 );
		BITMAPINFOHEADER	*pbi1 = (BITMAPINFOHEADER*)(byte*)ptrB1;
		MoveMemory(pbi1, pbi, sizeof( BITMAPINFOHEADER )+1024); //копируем из pbi
		pbi1->biBitCount=8;
		pbi1->biCompression=BI_RGB;
		pbi1->biSizeImage=nFileSize1;
		pbi1->biClrUsed=0;
		pbi1->biClrImportant=0;
		CDC dc;
		CClientDC dcScreen( 0 );						
		dc.CreateCompatibleDC(&dcScreen);
		CBitmap bmp;
		//bmp.CreateBitmap(pbi->biWidth,pbi->biHeight,
			//pbi->biPlanes, pbi->biBitCount, 0);
		bmp.CreateCompatibleBitmap(&dcScreen,pbi->biWidth,pbi->biHeight);
		CBitmap* pOldBitmap = dc.SelectObject(&bmp);
		int i1=SetDIBits(dc.GetSafeHdc(), HBITMAP(bmp), 0,cy, ptr, (BITMAPINFO *)pbi, DIB_RGB_COLORS); //поправить последнее значение
		//и pbi лучше бы объявить сразу как BITMAPINFO
		DWORD err1=GetLastError();
		int i3=GetDIBits(dc.GetSafeHdc(), HBITMAP(bmp), 0,cy, ptr1, (BITMAPINFO *)pbi1, DIB_RGB_COLORS); //поправить последнее значение
		DWORD err3=GetLastError();
		dc.SelectObject(pOldBitmap);
		DoAttachPaneData(sptrI, nPane, pbi1, ((byte*)ptr1));
	}
	else
		DoAttachPaneData(sptrI, nPane, pbi, ((byte*)ptr));
	return true;
	}
	catch(CTerminateException* e)
	{
		e->Delete();
		return false;
	}
	catch(CFileException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch( ... )
	{
		return false;
	}
}

bool CCGHFileFilter::InitPanesArray(LPCTSTR pszFileName, CStringArray &sa, LPCTSTR lpszCC)
{
	// Parse .cgh name for .bmp names
	TCHAR szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME];
	_tsplitpath(pszFileName, szDrive, szDir, szFName, NULL);
	// Obtain path names for pane file path
	IColorConvertor2Ptr sptrCC(GetCCByName(lpszCC));
	IHumanColorConvertorPtr sptrHCC(sptrCC);
	if (sptrCC == 0 || sptrHCC == 0)
		return false;
	m_aPanesNames.RemoveAll();
	int nPanes;
	sptrCC->GetColorPanesDefCount(&nPanes);
	for (int i = 0; i < nPanes; i++)
	{
		CString s;
		if (i < sa.GetSize() && !sa[i].IsEmpty())
			s = sa[i];
		else
		{
			BSTR bstr;
			sptrHCC->GetPaneShortName(i, &bstr);
			s = bstr;
			::SysFreeString(bstr);
		}
		CString sName = CString(szFName)+"_"+s;
		TCHAR szPathName[_MAX_PATH];
		_tmakepath(szPathName, szDrive, szDir, sName, _T(".bmp"));
		m_aPanesNames.Add(szPathName);
	}
	return true;
}

void CCGHFileFilter::InitPanesNames(CImageWrp &pImgWrp, CStringArray &sa)
{
	CString sCCName = pImgWrp.GetColorConvertor();
	int nColors = pImgWrp.GetColorsCount();
	if (sCCName.CompareNoCase(_T("CGH"))==0 && nColors == 7)
	{
		sa.Add(_T("DAPI"));
		sa.Add(_T("FITC"));
		sa.Add(_T("TRITC"));
		sa.Add(_T("Ratio"));
		sa.Add(_T("DAPIu"));
		sa.Add(_T("FITCu"));
		sa.Add(_T("TRITCu"));
	}
	else
	{
		CString s;
		for (int i = 0; i < nColors; i++)
		{
			s.Format(_T("Pane%d"), i);
			sa.Add(s);
		}

	}
}

bool CCGHFileFilter::ReadFile( const char *pszFileName )
{
	// Read names from file
	CStringArray saTokens;
	CString s;
	CStdioFile file(pszFileName,CFile::modeRead|CFile::typeText);
	while (file.ReadString(s))
	{
		int n = s.Find(_T(" \t"));
		if (n != -1) s = s.Left(n);
		if (!s.IsEmpty()) saTokens.Add(s);
	}
	CString sCC = saTokens[0];
	saTokens.RemoveAt(0);
	if (!InitPanesArray(pszFileName,saTokens,sCC))
		return false;
	IUnknownPtr punk(CreateNamedObject(_T("Image")),false);
	bool bOk = false;
	for (int i = 0; i < m_aPanesNames.GetSize(); i++)
		bOk |= ReadBmp(m_aPanesNames[i], punk, i, sCC);
	return bOk;
}

void CCGHFileFilter::DoGetPaneData(IImage *pimg, int nPane, LPBITMAPINFOHEADER pbi, LPBYTE lpData)
{
	ASSERT(pbi->biBitCount == 8);
	DWORD dwRow = ((pbi->biWidth+3)>>2)<<2;
	for (int y = 0; y < pbi->biHeight; y++)
	{
		color *pRow;
		pimg->GetRow(y, nPane, &pRow);
		for (int x = 0; x < pbi->biWidth; x++)
			lpData[x] = (BYTE)(pRow[x]>>8);
		lpData += dwRow;
	}
}


bool CCGHFileFilter::WriteBmp(const char *pszFileName, IUnknown *punk, int nPane)
{
	try
	{
	CImageWrp	image(punk);
	BITMAPINFOHEADER bi;
	memset(&bi, 0, sizeof(bi));
	bi.biSize = sizeof(bi);
	bi.biWidth = image.GetWidth();
	bi.biHeight = image.GetHeight();
	bi.biBitCount = 8;
	bi.biClrUsed = bi.biClrImportant = 256;
	bi.biPlanes = 1;
	bi.biSizeImage = ((bi.biWidth+3)/4*4)*bi.biHeight;
	BITMAPFILEHEADER		bfh;
	ZeroMemory(&bfh, sizeof( bfh ));
	bfh.bfType = ((int('M')<<8)|int('B'));
	bfh.bfSize = sizeof(bfh)+bi.biSizeImage;
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD);
	CFile	file( pszFileName, CFile::modeCreate|CFile::modeWrite );
	file.Write(&bfh, sizeof(bfh));
	file.Write(&bi, sizeof(bi));
	RGBQUAD rgb[256];
	for (int i = 0; i < 256; i++)
	{
		rgb[i].rgbRed = nPane==2||nPane==3||nPane==4?i:0;
		rgb[i].rgbGreen = nPane==1||nPane==3||nPane==5?i:0;
		rgb[i].rgbBlue = nPane==0||nPane==3||nPane==6?i:0;
		rgb[i].rgbReserved = 0;
	}
	file.Write(rgb, 256*sizeof(RGBQUAD));
	pointer<BYTE> pData(bi.biSizeImage);
	DoGetPaneData(image, nPane, &bi, pData);
	file.Write(pData, bi.biSizeImage);
	return true;
	}
	catch(CTerminateException* e)
	{
		e->Delete();
		return false;
	}
	catch(CFileException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch( ... )
	{
		return false;
	}
}

bool CCGHFileFilter::WriteFile( const char *pszFileName )
{
	// convert object type to BSTR
	_bstr_t bstrType( GetDataObjectType(0) );
	// get active object in which type == m_strObjKind
	IUnknownPtr punk;
	if (!(m_bObjectExists && m_sptrAttachedObject != NULL) && m_sptrIDataContext != NULL)
	{
		m_sptrIDataContext->GetActiveObject(bstrType, &punk);
		INamedDataObject2Ptr	ptrN( punk );
		if( ptrN != 0 )
		{
			IUnknownPtr punkP;
			ptrN->GetParent(&punkP);
			if (punkP != 0)
				punk = punkP;
		}
	}
	else
		punk = m_sptrAttachedObject.GetInterfacePtr();
	if (punk == 0)
		return false;
	CImageWrp image(punk);
	CStringArray sa;
	InitPanesNames(image, sa);
	CStdioFile file(pszFileName,CFile::modeCreate|CFile::modeWrite|CFile::typeText);
	fprintf(file.m_pStream, "%s\n", (LPCTSTR)image.GetColorConvertor());
	for (int i = 0; i < sa.GetSize(); i++)
		fprintf(file.m_pStream, "%s\n", (LPCTSTR)sa[i]);
	InitPanesArray(pszFileName, sa, (LPCTSTR)image.GetColorConvertor());
	bool bOk = false;
	for (i = 0; i < m_aPanesNames.GetSize(); i++)
		bOk |= WriteBmp(m_aPanesNames[i], punk, i);
	return bOk;
}

int  CCGHFileFilter::GetPropertyCount()
{
	return 5;
}

void CCGHFileFilter::GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx)
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