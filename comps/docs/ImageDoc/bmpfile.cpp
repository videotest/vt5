#include "stdafx.h"
#include "resource.h"
#include "bmpfile.h"
#include "image.h"




IMPLEMENT_DYNCREATE(CBMPFileFilter, CCmdTargetEx);

// {EFA23161-44C2-11d3-A612-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CBMPFileFilter, "ImageDoc.BMPFileFilter", 
0xefa23161, 0x44c2, 0x11d3, 0xa6, 0x12, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

const char* szWidth		= "Width";
const char* szHeight	= "Height";
const char* szColors	= "Colors";
const char* szDepth		= "Depth";
const char* szNumPages	= "NumPages";
const char* szCompress	= "Compress";


CBMPFileFilter::CBMPFileFilter()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nColors = 0;
	m_nDepth = 0;

	m_strDocumentTemplate = _T("Image");
	m_strFilterName.LoadString( IDS_FILTERNAME );
	m_strFilterExt = _T(".bmp");
	AddDataObjectType(szTypeImage);
	m_strCompression = "";
}

bool CBMPFileFilter::ReadFile( const char *pszFileName )
{
	try
	{
	//CTimeTest timeTest(true, pszFileName, false);
	//timeTest.m_bEnableFileOutput = true;


	CFile	file( pszFileName, CFile::modeRead|CFile::shareDenyWrite );

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


	m_nWidth = cx;
	m_nHeight = cy;
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

	pointer<byte>	ptr( nFileSize );

	if( ptr==0 )
		return false;

	::ZeroMemory( ptr, nFileSize );


	if(bfh.bfOffBits)
	{
		file.Seek(bfh.bfOffBits,CFile::begin);
	}

	//if( file.Read( ptr, nFileSize ) != nFileSize )
	//	return false;

	int nread = file.Read( ptr, nFileSize );
	ASSERT( nread == nFileSize );


	IUnknown	*punk = CreateNamedObject( GetDataObjectType(0) );

	// get object name from path and check it's exists
	CString	strObjName = ::GetObjectNameFromPath(pszFileName);
	_bstr_t bstrName(strObjName);
	long	NameExists = 0;
	
	if( m_sptrINamedData != 0 )
		m_sptrINamedData->NameExists(bstrName, &NameExists);
	// if new name not exists allready
	if (!NameExists && punk)
	{
		// set this name to object
		INamedObject2Ptr sptrObj(punk);
		sptrObj->SetName(bstrName);
	}

	CImageWrp	sptrI( punk );

	punk->Release();

	if(pbi->biCompression!=BI_RGB)
	{
		int cx4_1=(cx*3+3)/4*4;
		int nFileSize1=cx4_1*cy;
		pointer<byte> ptr1( nFileSize1 );

		pointer<byte>	ptrB1( sizeof( BITMAPINFOHEADER )+1024 );
		BITMAPINFOHEADER	*pbi1 = (BITMAPINFOHEADER*)(byte*)ptrB1;
		MoveMemory(pbi1, pbi, sizeof( BITMAPINFOHEADER )+1024); //копируем из pbi

		pbi1->biBitCount=24;
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

		sptrI.AttachDIBBits( pbi1, ((byte*)ptr1) );
	}
	else
	{
		//sptrI.AttachDIBBits( pbi, ((byte*)ptr)+nFileOffset );
		sptrI.AttachDIBBits( pbi, ((byte*)ptr) );
	}

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

bool CBMPFileFilter::WriteFile( const char *pszFileName )
{
	try
	{
	// convert object type to BSTR
	_bstr_t bstrType( GetDataObjectType(0) );

	// get active object in which type == m_strObjKind
	IUnknown* punk = NULL;

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
		return false;

	CImageWrp	image( punk, false );

	BITMAPINFOHEADER	*pbi = image.ExtractDIBBits();

	BITMAPFILEHEADER		bfh;

	ZeroMemory( &bfh, sizeof( bfh ) );

	bfh.bfType = ((int( 'M' )<<8)|int( 'B' ));
	bfh.bfSize = sizeof( bfh )+pbi->biSizeImage;
	bfh.bfOffBits = sizeof( BITMAPFILEHEADER )+sizeof( BITMAPINFOHEADER )+ ((pbi->biBitCount == 24)?0:(1<<pbi->biBitCount<<2));

	CFile	file( pszFileName, CFile::modeCreate|CFile::modeWrite );
	file.Write( &bfh, sizeof( bfh ) );

	file.Write( pbi, pbi->biSizeImage+pbi->biSize );

	delete pbi;

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

int  CBMPFileFilter::GetPropertyCount()
{
	return 5;
}

void CBMPFileFilter::GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx)
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