#include "StdAfx.h"
#include "image.h"
#include "appdefs.h"
//#include "DLLInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int g_iRedCoef   = 60;
int g_iGreenCoef = 118;
int g_iBlueCoef  = 22;

// tested
int CColorConvert<BYTE,BYTE>::Convert(BYTE *pTo, const BYTE *pFrom, int nSize)
{
	memcpy(pTo,pFrom,nSize);
	return 0;
}

// tested
int CColorConvert<BYTE,WORD>::Convert(WORD *pTo, const BYTE *pFrom, int nSize)
{
	for (; nSize--;) *pTo++ = ((WORD)(*pFrom++))<<8;
	return 0;
}

// tested
int CColorConvert<WORD,BYTE>::Convert(BYTE *pTo, const WORD *pFrom, int nSize)
{
	for (; nSize--;) *pTo++ = (BYTE)((*pFrom++)>>8);
	return 0;
}

// not tested yet
int CColorConvert<WORD,WORD>::Convert(WORD *pTo, const WORD *pFrom, int nSize)
{
	memcpy(pTo,pFrom,nSize*sizeof(WORD));
	return 0;
}

// tested for conversion from 8-bit to 16-bit data for truecolor and grayscale images
// call stack:
//  CColorConvert<unsigned char,unsigned short>::DoAttach(CImage<unsigned short> & {CImage<unsigned short>}, const CImage<unsigned char> & {CImage<unsigned char>}) line 31
//  CImage<unsigned short>::Attach(const CImage<unsigned char> & {CImage<unsigned char>}) line 135 + 13 bytes
//  CImage<unsigned short>::operator=(CImage<unsigned char> & {CImage<unsigned char>}) line 163
template <class FROMCOLORREPR, class TOCOLORREPR>  void CColorConvert<FROMCOLORREPR,TOCOLORREPR>::DoAttach( CImage<TOCOLORREPR> &Dest, const CImage<FROMCOLORREPR> &Src)
{
	if (Src.IsEmpty())
	{
		Dest.Free();
		return;
	}
	Dest.m_cx = Src.m_cx;
	Dest.m_cy = Src.m_cy;
	Dest.m_iCols = Src.m_iCols;
	Dest.m_iIncr = Src.m_iIncr;
	int fsOptions = Src.IsTrueColor()?CImageBase::TrueColor:CImageBase::GrayScale;
	Dest.AllocBuffer( Dest.GetDataSize(Dest.m_iCols, Dest.m_cy, fsOptions), Dest.m_cy, fsOptions );
	Dest.AllocRows();
	memcpy(Dest.m_lpbi,Src.m_lpbi,sizeof(BITMAPINFOHEADER)+Src.GetPaletteSize());
	Dest.ConvertHeader();
	Convert(Dest.m_pBits,Src.m_pBits,Src.GetConvBitsSize());
}

void CHisto::InitHisto(long nDataNum, BOOL fTrueColor)
{
	if (m_nDataNum != nDataNum)
	{
		if (m_pData == m_pData0)
			m_pData = NULL;
		FreeMem(m_pData0);
		m_pData0 = (long *)AllocMem(nDataNum*sizeof(long));
	}
	memset(m_pData0,0,nDataNum*sizeof(long));
	if (m_nDataNum != nDataNum || !fTrueColor && m_fTrueColor)
	{
		FreeMem(m_pData1);
		FreeMem(m_pData2);
		FreeMem(m_pData);
	}
	if (fTrueColor)
	{
		if (m_nDataNum != nDataNum || fTrueColor && !m_fTrueColor)
			m_pData1 = (long *)AllocMem(nDataNum*sizeof(long));
		memset(m_pData1,0,nDataNum*sizeof(long));
		if (m_nDataNum != nDataNum || fTrueColor && !m_fTrueColor)
			m_pData2 = (long *)AllocMem(nDataNum*sizeof(long));
		memset(m_pData2,0,nDataNum*sizeof(long));
		if (m_nDataNum != nDataNum || fTrueColor && !m_fTrueColor)
			m_pData = (long *)AllocMem(nDataNum*sizeof(long));
		memset(m_pData,0,nDataNum*sizeof(long));
	}
	else
	{
			m_pData1 = NULL;
			m_pData2 = NULL;
			m_pData  = m_pData0;
	}
	m_nDataNum = nDataNum;
	m_fTrueColor = fTrueColor;
	m_nPointsNum = 0;
}

static void SmoothData(long *pDst, long *pSrc, int nLen, int nMask)
{
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
			nSum += pSrc[j];
		pDst[i] = nSum/nSiz;
	}
}

void CHisto::SmoothHisto(int nLev)
{
	long *pData;
	if (m_pData0)
	{
		pData = m_pData0;
		m_pData0 = (long *)AllocMem(m_nDataNum*sizeof(long));
		SmoothData(m_pData0,pData,m_nDataNum,nLev);
		FreeMem(pData);
	}
	if (m_fTrueColor)
	{
		if (m_pData1)
		{
			pData = m_pData1;
			m_pData1 = (long *)AllocMem(m_nDataNum*sizeof(long));
			SmoothData(m_pData1,pData,m_nDataNum,nLev);
			FreeMem(pData);
		}
		if (m_pData2)
		{
			pData = m_pData2;
			m_pData2 = (long *)AllocMem(m_nDataNum*sizeof(long));
			SmoothData(m_pData2,pData,m_nDataNum,nLev);
			FreeMem(pData);
		}
		if (m_pData)
		{
			pData = m_pData;
			m_pData = (long *)AllocMem(m_nDataNum*sizeof(long));
			SmoothData(m_pData,pData,m_nDataNum,nLev);
			FreeMem(pData);
		}
	}
	else
		m_pData  = m_pData0;
	CalcSum();
}

void CHisto::CalcSum()
{
	m_nPointsNum = 0;
	for (int i = 0; i < m_nDataNum; i++)
		m_nPointsNum += m_pData[i];
}

void CHisto::ClearHisto()
{
	if (m_pData != m_pData0)
		FreeMem(m_pData);
	FreeMem(m_pData0);
	FreeMem(m_pData1);
	FreeMem(m_pData2);
	m_pData0 = NULL;
	m_pData1 = NULL;
	m_pData2 = NULL;
	m_pData  = NULL;
	m_nDataNum = 0;
	m_nPointsNum = 0;
	m_fTrueColor = FALSE;
}

CHisto::CHisto(long nDataNum, BOOL fTrueColor)
{
	m_pData0 = NULL;
	m_pData1 = NULL;
	m_pData2 = NULL;
	m_pData  = NULL;
	InitHisto(nDataNum,fTrueColor);
}

CHisto::CHisto()
{
	m_pData0 = NULL;
	m_pData1 = NULL;
	m_pData2 = NULL;
	m_pData  = NULL;
	m_nDataNum = 0;
	m_nPointsNum = 0;
	m_fTrueColor = FALSE;
}

CHisto::~CHisto()
{
	if (m_pData != m_pData0)
		FreeMem(m_pData);
	FreeMem(m_pData0);
	FreeMem(m_pData1);
	FreeMem(m_pData2);
}

int CImageBase::m_nBitsOffset = 8;
BOOL CImageBase::m_fUseAllBits = FALSE;

CImageBase::CImageBase(void)
{
}

void CImageBase::Attach( CImageBase *pImage)
{
	if (pImage == NULL)
		return;
	if (pImage->Is16BitColor())
		Attach16(*(C16Image *)pImage);
	else
		Attach8(*(C8Image *)pImage);
}

void CImageBase::Attach( CImageBase *pImage, CRect const &Rect)
{
	if (pImage->Is16BitColor())
		Attach16(*(C16Image *)pImage,Rect);
	else
		Attach8(*(C8Image *)pImage,Rect);
}

void CImageBase::operator=( CImageBase *pImage )
{
	if (pImage->Is16BitColor())
		this->Assign16(*(C16Image *)pImage);
	else
		this->Assign8(*(C8Image *)pImage);
}

HRESULT CImageBase::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	if (riid == IID_IUnknown)
	{
		AddRef();
		*ppvObj = (IUnknown *)this;
		return S_OK;
	}
/*	else if (riid == IID_IImage)
	{
		AddRef();
		*ppvObj = (IImage *)this;
		return S_OK;
	}
	else*/
		return E_NOINTERFACE;
}

IMPLEMENT_DYNAMIC(CImageBase,CObject);

template<class COLORREPR1, class COLORREPR2> CImage<COLORREPR1> *CreateImage(CImage<COLORREPR2> const &img,
	const CRect *pRect,	CImage<COLORREPR1> &imgDst, CDIBArgument *pDibArgument, CDIBArgument *pDibResult)
{
	CImage<COLORREPR1> *pImageDst = new CImage<COLORREPR1>;
	AttachImage(*pImageDst,img,pRect,pDibArgument,pDibResult);
	return pImageDst;
}

C8Image *Create8_8(C8Image *pImage, const CRect *pRect, 
	C8Image &imgDst, CDIBArgument *pDibArgument,
	CDIBArgument *pDibResult)
{
	return CreateImage(*pImage,pRect,imgDst,pDibArgument,pDibResult);
}

C16Image *Create16_8(C8Image *pImage, const CRect *pRect, 
	C16Image &imgDst, CDIBArgument *pDibArgument,
	CDIBArgument *pDibResult)
{
	return CreateImage(*pImage,pRect,imgDst,pDibArgument,pDibResult);
}

C8Image *Create8_16(C16Image *pImage, const CRect *pRect, 
	C8Image &imgDst, CDIBArgument *pDibArgument,
	CDIBArgument *pDibResult)
{
	return CreateImage(*pImage,pRect,imgDst,pDibArgument,pDibResult);
}

C16Image *Create16_16(C16Image *pImage, const CRect *pRect, 
	C16Image &imgDst, CDIBArgument *pDibArgument,
	CDIBArgument *pDibResult)
{
	return CreateImage(*pImage,pRect,imgDst,pDibArgument,pDibResult);
}


template CColorConvert<BYTE,BYTE>;
template CColorConvert<BYTE,WORD>;
template CColorConvert<WORD,BYTE>;
template CColorConvert<WORD,WORD>;

template<class COLORREPR1, class COLORREPR2> void GetImage(
	CImage<COLORREPR1> &imgDst, CImage<COLORREPR2> const &img,
	int xOffs, int yOffs)
{
	short cx = min(img.GetCX()-xOffs,imgDst.GetCX());
	short cy = min(img.GetCY()-yOffs,imgDst.GetCY());
	for( register short y=0; y<cy; y++ )
		for( register short x=0; x<cx*img.GetIncr(); x++ )
			imgDst[y][x] = (COLORREPR1)img[y+yOffs][x+xOffs*img.GetIncr()];
}

void SetImage(C16Image &imgDst,	C8Image const &img,	int xOffs, int yOffs)
{
	short cx = min(img.GetCX(),imgDst.GetCX()-xOffs);
	short cy = min(img.GetCY(),imgDst.GetCY()-yOffs);
	cx = max( 0, cx ); cy = max( 0, cy );
	for( register short y=0; y<cy; y++ )
		for( register short x=0; x<cx*img.GetIncr(); x++ )
			imgDst[y+yOffs][x+xOffs*imgDst.GetIncr()] = ((WORD)img[y][x])<<8;
}

void SetImage(C8Image &imgDst, C16Image const &img, int xOffs, int yOffs)
{
	short cx = min(img.GetCX(),imgDst.GetCX()-xOffs);
	short cy = min(img.GetCY(),imgDst.GetCY()-yOffs);
	cx = max( 0, cx ); cy = max( 0, cy );
	for( register short y=0; y<cy; y++ )
		for( register short x=0; x<cx*img.GetIncr(); x++ )
			imgDst[y+yOffs][x+xOffs*imgDst.GetIncr()] = (BYTE)(img[y][x]>>8);
}

void SetImage(C16Image &imgDst, C16Image const &img, int xOffs, int yOffs)
{
	short cx = min(img.GetCX(),imgDst.GetCX()-xOffs);
	short cy = min(img.GetCY(),imgDst.GetCY()-yOffs);
	cx = max( 0, cx ); cy = max( 0, cy );
	size_t iLineWid = img.GetIncr()*2*cx;
	int x1 = xOffs*imgDst.GetIncr();
	for( register short y=0; y<cy; y++ )
	{
		/*
		WORD *p = img[y];
		{
			int *p = new int;
			delete p;
		}
		WORD *p1 = imgDst[y+yOffs];
		WORD *p2 = img[y];
		*/
		memcpy( &imgDst[y+yOffs][x1], img[y], iLineWid );
	}
}

void SetImage(C8Image &imgDst, C8Image const &img, int xOffs, int yOffs)
{
	short cx, cy;
	cx = min(img.GetCX(),imgDst.GetCX()-xOffs);
	cy = min(img.GetCY(),imgDst.GetCY()-yOffs);
	cx = max( 0, cx ); cy = max( 0, cy );
	size_t iLineWid = img.GetIncr()*cx;
	int x1 = xOffs*imgDst.GetIncr();
	for( register short y=0; y<cy; y++ )
		memcpy( &imgDst[y+yOffs][x1], img[y], iLineWid );
}

void SetImage( CImageBase *pimgDst, CImageBase *pimg, int xOffs, int yOffs)
{
	if (pimgDst->Is16BitColor())
		if (pimg->Is16BitColor())
			SetImage(*(C16Image *)pimgDst,*(C16Image *)pimg,xOffs,yOffs);
		else
			SetImage(*(C16Image *)pimgDst,*(C8Image *)pimg,xOffs,yOffs);
	else
		if (pimg->Is16BitColor())
			SetImage(*(C8Image *)pimgDst,*(C16Image *)pimg,xOffs,yOffs);
		else
			SetImage(*(C8Image *)pimgDst,*(C8Image *)pimg,xOffs,yOffs);
}

void GetImage( CImageBase *pimgDst, CImageBase *pimg, int xOffs, int yOffs)
{
	if (pimgDst->Is16BitColor())
		if (pimg->Is16BitColor())
			GetImage(*(C16Image *)pimgDst,*(C16Image *)pimg,xOffs,yOffs);
		else
			GetImage(*(C16Image *)pimgDst,*(C8Image *)pimg,xOffs,yOffs);
	else
		if (pimg->Is16BitColor())
			GetImage(*(C8Image *)pimgDst,*(C16Image *)pimg,xOffs,yOffs);
		else
			GetImage(*(C8Image *)pimgDst,*(C8Image *)pimg,xOffs,yOffs);
}

void SetImage2(C16Image &imgDst, C8Image const &img, int xOffs, int yOffs)
{
	short cx = min(img.GetCX(),imgDst.GetCX()-xOffs);
	short cy = min(img.GetCY(),imgDst.GetCY()-yOffs);
	cx = max( 0, cx ); cy = max( 0, cy );
	for( register short y=0; y<cy; y++ )
		for( register short x=0; x<cx*img.GetIncr(); x++ )
			imgDst[y+yOffs][x+xOffs*imgDst.GetIncr()] = ((WORD)img[y][x])<<8;
}

void SetImage2(C8Image &imgDst, C16Image const &img, int xOffs, int yOffs)
{
	short cx = min(img.GetCX(),imgDst.GetCX()-xOffs);
	short cy = min(img.GetCY(),imgDst.GetCY()-yOffs);
	cx = max( 0, cx ); cy = max( 0, cy );
	for( register short y=0; y<cy; y++ )
		for( register short x=0; x<cx*img.GetIncr(); x++ )
			imgDst[y+yOffs][x+xOffs*imgDst.GetIncr()] = (BYTE)(img[y][x]>>8);
}

void SetImage2(C16Image &imgDst, C16Image const &img, int xOffs, int yOffs)
{
	short cx = min(img.GetCX()-xOffs,imgDst.GetCX());
	short cy = min(img.GetCY()-yOffs,imgDst.GetCY());
	cx = max( 0, cx ); cy = max( 0, cy );
	size_t iLineWid = img.GetIncr()*2*cx;
	int x1 = xOffs*imgDst.GetIncr();
	for( register short y=0; y<cy; y++ )
	{
		WORD *p1 = &img[y+yOffs][x1];
		WORD *p2 = imgDst[y];
		memcpy( p2, p1, iLineWid );
	}
}

void SetImage2(C8Image &imgDst, C8Image const &img, int xOffs, int yOffs)
{
	short cx, cy;
	cx = min(img.GetCX()-xOffs,imgDst.GetCX());
	cy = min(img.GetCY()-yOffs,imgDst.GetCY());
	cx = max( 0, cx ); cy = max( 0, cy );
	size_t iLineWid = img.GetIncr()*cx;
	int x1 = xOffs*imgDst.GetIncr();
	for( register short y=0; y<cy; y++ )
		memcpy( imgDst[y], &img[y+yOffs][x1], iLineWid );
}

void SetImage2( CImageBase *pimgDst, CImageBase *pimg, int xOffs, int yOffs)
{
	if (pimgDst->Is16BitColor())
		if (pimg->Is16BitColor())
			SetImage2(*(C16Image *)pimgDst,*(C16Image *)pimg,xOffs,yOffs);
		else
			SetImage2(*(C16Image *)pimgDst,*(C8Image *)pimg,xOffs,yOffs);
	else
		if (pimg->Is16BitColor())
			SetImage2(*(C8Image *)pimgDst,*(C16Image *)pimg,xOffs,yOffs);
		else
			SetImage2(*(C8Image *)pimgDst,*(C8Image *)pimg,xOffs,yOffs);
}


// GetImage  adds offset to pImageSrc - 2nd arg
// SetImage  adds offset to pImageDst - 1st arg
// SetImage2 adds offset to pImageSrc - 2nd arg

void SwapImage( CImageBase *pimgDst, CImageBase *pimg, int xOffs, int yOffs)
{
	CImageBase *pimgTmp = CImageBase::MakeFromBits(pimg->GetBI(),
		TRUE);
	GetImage(pimgTmp,pimg,xOffs,yOffs);
	SetImage2(pimg,pimgDst,xOffs,yOffs);
	SetImage(pimgDst,pimgTmp,xOffs,yOffs);
	delete pimgTmp;
}

void SwapImage( CImageBase *pimgDst, CImageBase *pimg, CRect rcDst, CRect rcSrc)
{
	CImageBase *pimgTmp;
	if (pimg->Is16BitColor())
		pimgTmp = new C16Image(rcSrc.Width(), rcSrc.Height(), pimg->IsTrueColor()?
			CImageBase::TrueColor:CImageBase::GrayScale);
	else
		pimgTmp = new C8Image(rcSrc.Width(), rcSrc.Height(), pimg->IsTrueColor()?
			CImageBase::TrueColor:CImageBase::GrayScale);

	GetImage(pimgTmp,pimg,rcSrc.left,rcSrc.top);
	SetImage2(pimg,pimgDst,rcDst.left,rcDst.top);
	SetImage(pimgDst,pimgTmp,rcDst.left,rcDst.top);
	delete pimgTmp;
}

void qq()
{
	C16Image i1;
	C8Image i2;
	const CRect *pr = NULL;
	CDIBArgument *pDibArg = NULL;

	AttachImage( i1, i1, pr, pDibArg, pDibArg );
	AttachImage( i2, i1, pr, pDibArg, pDibArg );
	AttachImage( i1, i2, pr, pDibArg, pDibArg );
	AttachImage( i2, i2, pr, pDibArg, pDibArg );
	CreateImage( i1, pr, i1, pDibArg, pDibArg );
	CreateImage( i1, pr, i2, pDibArg, pDibArg );
	CreateImage( i2, pr, i1, pDibArg, pDibArg );
	CreateImage( i2, pr, i2, pDibArg, pDibArg );
	C8Image *pi2 = CreateImage( C8Image(NULL), pr, i2, pDibArg, pDibArg );
	C16Image *pi1 = CreateImage( C16Image(NULL), pr, i1, pDibArg, pDibArg );
}

// {2099EC94-4AF5-11D2-B8E7-000000000000}
GUID IID_IImage = 
{ 0x2099EC94, 0x4AF5, 0x11D2, { 0xB8, 0xE7, 0, 0, 0, 0, 0, 0 } };
