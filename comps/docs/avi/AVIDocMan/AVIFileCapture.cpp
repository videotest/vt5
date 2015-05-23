#include "StdAfx.h"
#include "Common.h"
#include "AVIFileCapture.h"

CAVIFileCapture::CAVIFileCapture()
{
	AVIFileInit();
	m_bInitialized = false;
	m_pfile = NULL;
	m_pstream = NULL;
	m_pgetframe = NULL;
	m_pFormatInfo = NULL;
}

CAVIFileCapture::~CAVIFileCapture()
{
	Close();
	AVIFileExit();
}

bool CAVIFileCapture::Open(LPCTSTR lpFileName, VTVideoInfo *pInfo)
{
	HRESULT hr = AVIFileOpen(&m_pfile, lpFileName, OF_SHARE_DENY_WRITE, 0L);
	if (hr != 0)
	{
		GetLogFile() << "AVIFileOpen FAILED!!!, err " << int(hr) << " \n";
		return false;
	}
	m_pstream = NULL;
	hr = AVIFileGetStream(m_pfile, &m_pstream, streamtypeVIDEO, 0);
	if (hr != AVIERR_OK)
	{
		GetLogFile() << "AVIFileGetStream FAILED!!!, err " << int(hr) << " \n";
		return false;
	}
	BITMAPINFOHEADER bi;
	int nPos = 0;
	long nStreamSize;
	VERIFY(!AVIStreamFormatSize(m_pstream, nPos, &nStreamSize));
	m_pFormatInfo = (LPBITMAPINFOHEADER)malloc(nStreamSize);
    hr = AVIStreamReadFormat(m_pstream, nPos, m_pFormatInfo, &nStreamSize);
	if (hr)
	{
		ASSERT(FALSE);
		GetLogFile() << "AVIStreamReadFormat FAILED!!!, err " << int(hr) << " \n";
		return false;
	}
	memcpy(&bi, m_pFormatInfo, min(nStreamSize,sizeof(bi)));
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	m_pgetframe = AVIStreamGetFrameOpen(m_pstream,&bi);
	if (!m_pgetframe)
	{
		ASSERT(FALSE);
		GetLogFile() << "AVIStreamGetFrameOpen FAILED!!!, err " << int(hr) << " \n";
		return false;
	}
	// Read supplementary info
	LONG lSize = sizeof(*pInfo);
	hr = AVIFileReadData(m_pfile, streamtypeVTVI, pInfo, &lSize);
	BOOL bFail = TRUE;
	if (SUCCEEDED(hr))
	{
		if (strcmp(pInfo->szLabel, "Videotest video info") == 0)
			bFail = FALSE;
	}
	if (bFail)
		memset(pInfo, 0, sizeof(*pInfo));
	m_bInitialized = true;
	return true;
}

void CAVIFileCapture::Close()
{
	m_bInitialized = false;
	if (m_pFormatInfo)
	{
		free(m_pFormatInfo);
		m_pFormatInfo = NULL;
	}
	if (m_pgetframe)
	{
		AVIStreamGetFrameClose(m_pgetframe);
		m_pgetframe = NULL;
	}
	if (m_pstream)
	{
		AVIStreamRelease(m_pstream);
		m_pstream = NULL;
	}
	if (m_pfile)
	{
		AVIFileRelease(m_pfile);
		m_pfile = NULL;
	}
}

bool CAVIFileCapture::Capture(int nPos, IUnknown *punkImg)
{
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(m_pgetframe, nPos);
	if( !lpbi )
		return false;
	if (lpbi->biClrUsed)
	{
		LPBITMAPINFOHEADER lpbiFmt = m_pFormatInfo;
		int nColors = min(lpbi->biClrUsed,lpbiFmt->biClrUsed);
		memcpy(lpbi+1,lpbiFmt+1,nColors*sizeof(RGBQUAD));
	}
	CImageWrp sptrI(punkImg);
	if (lpbi->biClrUsed)
	{
		BYTE *pData = (BYTE *)(((RGBQUAD *)(lpbi+1))+lpbi->biClrUsed);
		sptrI.AttachDIBBits(lpbi, pData);
	}
	else
		sptrI.AttachDIBBits(lpbi, (BYTE *)(lpbi+1));
	return true;
}

bool CAVIFileCapture::IsAviFile()
{
	return m_pgetframe != NULL;
}




