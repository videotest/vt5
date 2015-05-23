#include "StdAfx.h"
#include "BayerUtils.h"
#include "StdProfile.h"

CDeadPxlMask::CDeadPxlMask()
{
	m_nThreshold = 100;
	m_bVideoMode = false;
}


CString CDeadPxlMask::MakeMaskName(LPCTSTR lpstrMask)
{
	TCHAR szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME],szExt[_MAX_EXT];
	GetModuleFileName(AfxGetInstanceHandle(), szPath, _MAX_PATH);
	_tsplitpath(szPath, szDrive, szDir, szFName, NULL);
	szExt[0] = 0;
	if (lpstrMask != NULL)
		_tsplitpath(lpstrMask, NULL, NULL, szFName, szExt);
	if (m_bVideoMode)
		_tcscat(szFName, _T("v"));
	if (szExt[0] == 0)
		_tcscpy(szExt, ".msk");
	_tmakepath(szPath, szDrive, szDir, szFName, szExt);
	CString sPath(szPath);
	return sPath;
}

void CDeadPxlMask::ReadPxlMask(LPCTSTR lpstrMask)
{
	m_Dib.Empty();
	try
	{
		CString sPath = MakeMaskName(lpstrMask);
		CFile file(sPath, CFile::modeRead|CFile::shareDenyWrite);
		BITMAPFILEHEADER bfh;
		if (file.Read(&bfh, sizeof(bfh)) != sizeof(bfh))
			return;
		if (bfh.bfType != ((int('M')<<8)|int('B')))
			return;
		BITMAPINFO256 bi;
		if (file.Read(&bi.bmiHeader, sizeof(bi.bmiHeader)) != sizeof(bi.bmiHeader))
			return;
		if (bi.bmiHeader.biBitCount != 8)
			return;
		long nPalSize = (bi.bmiHeader.biBitCount<=8)?1<<bi.bmiHeader.biBitCount:0;
		if (nPalSize != 0)
		{
			if (file.Read(&bi.bmiColors, nPalSize*sizeof(RGBQUAD)) != nPalSize*sizeof(RGBQUAD))
				return;
		}
		m_Dib.InitBitmap(bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, bi.bmiHeader.biBitCount);
		if (nPalSize != 0)
			m_Dib.InitGrayPalette();
		long nFileOffset = bfh.bfOffBits==0?0:(bfh.bfOffBits-sizeof(bfh))-sizeof( BITMAPINFOHEADER )-nPalSize*4;
		DWORD dwSizeImage = bi.bmiHeader.biSizeImage;
		if (dwSizeImage == 0)
			dwSizeImage = m_Dib.Size(false);
		else
			dwSizeImage = min(dwSizeImage,(DWORD)m_Dib.Size(false));
		if (nFileOffset)
			file.Seek(nFileOffset,CFile::current);
		file.Read(m_Dib.GetData(),dwSizeImage);
		m_nThreshold = CStdProfileManager::m_pMgr->GetProfileInt(_T("DeadPixels"),_T("Threshold"),100,true,true);
	}
	catch(CFileException *e)
	{
		e->Delete();
		m_Dib.Empty();
	}
}

void CDeadPxlMask::WritePxlMask(LPCTSTR lpstrMask)
{
	if (m_Dib.IsEmpty() || m_Dib.bpp() != 8) return;
	CString sPath = MakeMaskName(lpstrMask);
	CFile file(sPath, CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite);
	BITMAPFILEHEADER bfh = {0};
	bfh.bfType = ((int('M')<<8)|int('B'));
	bfh.bfSize = sizeof(bfh)+m_Dib.Size(true);
	bfh.bfOffBits = sizeof( BITMAPFILEHEADER )+sizeof( BITMAPINFOHEADER )+256*sizeof(RGBQUAD);
	file.Write(&bfh,sizeof(bfh));
	file.Write(m_Dib.m_lpbi,m_Dib.Size(true));
}


void CDeadPxlMask::FreeMask()
{
	m_Dib.Empty();
}

void CDeadPxlMask::CorrectBayerDib(LPBITMAPINFOHEADER lpbiImg, LPBYTE lpDataDest, int nBitCount, int xOffset, int yOffset)
{
	if (m_Dib.IsEmpty()) return;
	if (m_Dib.cx() < lpbiImg->biWidth+xOffset) return;
	if (m_Dib.cy() < lpbiImg->biHeight+yOffset) return;
	LPBYTE lpDataMask = (LPBYTE)m_Dib.GetData();
//	LPBYTE lpDataDest = (LPBYTE)::DIBData(lpbiImg);
	DWORD dwRowMask = m_Dib.RowSize();
	DWORD dwRowDest = ::DIBRowSize(lpbiImg->biWidth, nBitCount);
	lpDataMask += (yOffset+2)*dwRowMask;
	lpDataDest += 2*dwRowDest;
	for (int y = 2; y < lpbiImg->biHeight-2; y++)
	{
		LPBYTE lpDataMask1 = lpDataMask+xOffset;
		LPBYTE lpDataDest1 = lpDataDest;
		for (int x = 2; x < lpbiImg->biWidth-2; x++)
		{
			if (lpDataMask1[x] >= m_nThreshold && lpDataMask1[x-2] < m_nThreshold && lpDataMask1[x+2] < m_nThreshold)
			{
				int nSum = (int)lpDataDest1[x-2]+(int)lpDataDest1[x+2];
				lpDataDest1[x] = (BYTE)(nSum/2);
			}
		}
		lpDataMask += dwRowMask;
		lpDataDest += dwRowDest;
	}
}

void CDeadPxlMask::InitMask(LPBITMAPINFOHEADER lpbi)
{
	m_Dib.InitBitmap(lpbi->biWidth, lpbi->biHeight, lpbi->biBitCount);
	memcpy(m_Dib.m_lpbi, lpbi, ::DIBSize(lpbi, true));
	WritePxlMask();
}


void CDeadPxlMask::SetVideoMode(bool bVideoMode)
{
	if (bVideoMode != m_bVideoMode)
	{
		Empty();
		m_bVideoMode = bVideoMode;
		ReadPxlMask();
	}
}
