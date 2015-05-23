#include "StdAfx.h"
#include "ActiveMovieWnd.h"
#include <mmsystem.h>
#include <amstream.h>
#include <control.h>
#include <uuids.h>
#include "common.h"

class CMediaSeeking
{
public:
	IMediaSeeking *m_p;
	CMediaSeeking(CActiveMovieWnd *pWnd);
	~CMediaSeeking();
	IMediaSeeking *operator->() {return m_p;}
};

CMediaSeeking::CMediaSeeking(CActiveMovieWnd *pWnd)
{
	IUnknown *punk = pWnd->GetFilterGraph();
	IMediaSeeking *pims;
	HRESULT hr = punk->QueryInterface(IID_IMediaSeeking, (void **)&pims);
	if (SUCCEEDED(hr))
		m_p = pims;
	if (punk) punk->Release();
}

CMediaSeeking::~CMediaSeeking()
{
	m_p->Release();
}

unsigned CActiveMovieWnd::AMTimeToFrame(double dTime)
{
	CMediaSeeking p(this);
	LONGLONG l = (LONGLONG)(dTime*10000000.);
	p->ConvertTimeFormat(&l, &TIME_FORMAT_FRAME, l, &TIME_FORMAT_MEDIA_TIME);
	return unsigned(l);
}

double CActiveMovieWnd::AMMidFrameToTime(unsigned nFrame)
{
	CMediaSeeking p(this);
	LONGLONG l1 = 0, l2 = 0;
	p->ConvertTimeFormat(&l1, &TIME_FORMAT_MEDIA_TIME, (LONGLONG)nFrame, &TIME_FORMAT_FRAME);
	p->ConvertTimeFormat(&l2, &TIME_FORMAT_MEDIA_TIME, (LONGLONG)nFrame+1, &TIME_FORMAT_FRAME);
	return double((l1+l2)/2)/10000000.;
}

double CActiveMovieWnd::AMBegFrameToTime(unsigned nFrame)
{
	CMediaSeeking p(this);
	LONGLONG l = 0;
	p->ConvertTimeFormat(&l, &TIME_FORMAT_MEDIA_TIME, (LONGLONG)nFrame, &TIME_FORMAT_FRAME);
	return double(l)/10000000.;
}

unsigned CActiveMovieWnd::AMGetPos()
{
	CMediaSeeking p(this);
	LONGLONG l = 0;
	p->SetTimeFormat(&TIME_FORMAT_FRAME);
	p->GetCurrentPosition(&l);
	return unsigned(l);
}

double CActiveMovieWnd::AMGetTime()
{
	CMediaSeeking p(this);
	LONGLONG l = 0;
	p->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
	p->GetCurrentPosition(&l);
	return double(l)/10000000.;
}

unsigned CActiveMovieWnd::AMGetLength()
{
	CMediaSeeking p(this);
	LONGLONG l = 0;
	HRESULT hr1 = p->SetTimeFormat(&TIME_FORMAT_FRAME);
	p->GetDuration(&l);
	return unsigned(l);
}

LPBITMAPINFOHEADER ConvertImage16To24(LPBITMAPINFOHEADER lpbiSrc)
{
	DWORD dwRowSrc = ((lpbiSrc->biWidth*2+3)>>2)<<2;;
	DWORD dwRowDst = ((lpbiSrc->biWidth*3+3)>>2)<<2;
	LPBITMAPINFOHEADER lpbiDst = (LPBITMAPINFOHEADER)malloc(sizeof(BITMAPINFOHEADER)+dwRowDst*lpbiSrc->biHeight);
	if (!lpbiDst) return lpbiSrc;
	*lpbiDst = *lpbiSrc;
	lpbiDst->biBitCount = 24;
	bool b565 = false;
	if (lpbiSrc->biCompression==BI_BITFIELDS)
	{
		DWORD *pdw = (DWORD *)(lpbiSrc+1);
		if (*pdw == 0xF800)
			b565 = true;
	}
	WORD *pSrc = lpbiSrc->biCompression==BI_BITFIELDS?((WORD *)(lpbiSrc+1))+6:(WORD *)(lpbiSrc+1);
	BYTE *pDst = (BYTE *)(lpbiDst+1);
	for (int y = 0; y < lpbiSrc->biHeight; y++)
	{
		for (int x = 0; x < lpbiSrc->biWidth; x++)
		{
			WORD w = pSrc[x];
			if (b565)
			{
				pDst[3*x+2] = (BYTE)((w>>8)&0xF8);
				pDst[3*x+1] = (BYTE)((w>>3)&0xFC);
				pDst[3*x+0] = (BYTE)((w<<3)&0xF8);
			}
			else
			{
				pDst[3*x+2] = (BYTE)((w>>7)&0xF8);
				pDst[3*x+1] = (BYTE)((w>>2)&0xF8);
				pDst[3*x+0] = (BYTE)((w<<3)&0xF8);
			}
		}
		pSrc += dwRowSrc/2;
		pDst += dwRowDst;
	}
	free(lpbiSrc);
	return lpbiDst;
}

LPBITMAPINFOHEADER ConvertImage32To24(LPBITMAPINFOHEADER lpbiSrc)
{
	DWORD dwRowSrc = 4*lpbiSrc->biWidth;
	DWORD dwRowDst = ((lpbiSrc->biWidth*3+3)>>2)<<2;
	LPBITMAPINFOHEADER lpbiDst = (LPBITMAPINFOHEADER)malloc(sizeof(BITMAPINFOHEADER)+dwRowDst*lpbiSrc->biHeight);
	if (!lpbiDst) return lpbiSrc;
	*lpbiDst = *lpbiSrc;
	lpbiDst->biBitCount = 24;
	BYTE *pSrc = (BYTE *)(lpbiSrc+1);
	BYTE *pDst = (BYTE *)(lpbiDst+1);
	for (int y = 0; y < lpbiSrc->biHeight; y++)
	{
		for (int x = 0; x < lpbiSrc->biWidth; x++)
		{
			WORD w = pSrc[x];
			pDst[3*x+2] = pSrc[4*x+2];
			pDst[3*x+1] = pSrc[4*x+1];
			pDst[3*x+0] = pSrc[4*x+0];
		}
		pSrc += dwRowSrc;
		pDst += dwRowDst;
	}
	free(lpbiSrc);
	return lpbiDst;
}

void CActiveMovieWnd::Capture(IUnknown *punkImage)
{
	IUnknown *punk = GetFilterGraph();
	IBasicVideo *pibv;
	HRESULT hr = punk->QueryInterface(IID_IBasicVideo, (void **)&pibv);
	if (SUCCEEDED(hr))
	{
		long lBuffSize = 0;
		HRESULT hr1 = pibv->GetCurrentImage(&lBuffSize, NULL);
		if (SUCCEEDED(hr1))
		{
			LPBITMAPINFOHEADER p = (LPBITMAPINFOHEADER)malloc(lBuffSize);
			hr1 = pibv->GetCurrentImage(&lBuffSize, (long *)p);
			if (SUCCEEDED(hr1))
			{
				if (p->biBitCount == 16)
					p = ConvertImage16To24(p);
				else if (p->biBitCount == 32)
					p = ConvertImage32To24(p);
				CImageWrp sptrI(punkImage);
				if (p->biClrUsed)
				{
					BYTE *pData = (BYTE *)(((RGBQUAD *)(p+1))+p->biClrUsed);
					sptrI.AttachDIBBits(p, pData);
				}
				else
					sptrI.AttachDIBBits(p, (BYTE *)(p+1));
			}
			free(p);
		}
		pibv->Release();
	}
	punk->Release();
}


