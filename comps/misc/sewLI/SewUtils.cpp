#include "stdafx.h"
#include "SewUtils.h"
#include "resource.h"
#include "clone.h"
#include "clonedata.h"
#include <direct.h>
#include <atltypes.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#define VT5SLISIGNATURE "VT5 Sew Large Image directory"


CIntIniValue g_SmoothTransition("\\Sew Large Image", "SmoothTransition", 1);
CColorIniValue g_IntersectionColor1("\\Sew Large Image", "IntersectionColor1", RGB(255,0,0));
CColorIniValue g_IntersectionColor2("\\Sew Large Image", "IntersectionColor2", RGB(255,0,0));

SIZE g_szBase = {1280, 1024};
int g_nRangeCoef = 2;
int GetRangeBySize(SIZE sz, int nRangeCoef)
{
	int nRange = 1;
	_size szRange(g_szBase.cx*nRangeCoef, g_szBase.cy*nRangeCoef);
	while (1)
	{
		if (sz.cx < szRange.cx && sz.cy < szRange.cy)
			return nRange;
		szRange = _size(szRange.cx*nRangeCoef, szRange.cy*nRangeCoef);
	}
}

SIZE GetSizeByRange(int nRange, int nRangeCoef)
{
	int n = 1;
	for (int i = 0; i < nRange; i++)
		n *= nRangeCoef;
	_size sz(g_szBase.cx*n, g_szBase.cy*n);
	return sz;
}

int GetZoomByRange(int nRange, int nRangeCoef)
{
	int nZoom = 1;
	for (int i = 0; i < nRange; i++)
		nZoom *= nRangeCoef;
	return nZoom;
}

int GetViewRangeByZoom(double dZoom, int nRangeCoef)
{
	double dZoom1 = 1./dZoom;
	int nZoomLim = 1;
	for (int i = 0; ; i++)
	{
		nZoomLim *= nRangeCoef;
		if (dZoom1 < double(nZoomLim))
			return i;
	}
}

static WORD MakePixel(color cPrev, color cAdd, WORD wSum, WORD wCur)
{
	if (wSum == 0)
		return cAdd;
	else
		return WORD(((DWORD)cPrev*(DWORD)(wSum-wCur)+(DWORD)cAdd*(DWORD)wCur)/(DWORD)wSum);
}

void CopyImage(IImage *pDest, IImage *pSrc, POINT ptDest1, POINT ptSrc,
	SIZE sz, int nZoom, IDistanceMap *pTotalDM,	IDistanceMap *pCurDM)
{
	// Get color convertors
	IUnknownPtr punkCCDest;
	pDest->GetColorConvertor(&punkCCDest);
	IColorConvertor2Ptr sptrCCDest(punkCCDest);
	if (sptrCCDest == 0) return;
	_bstr_t bstrCnvDest;
	sptrCCDest->GetCnvName(bstrCnvDest.GetAddress());
	IUnknownPtr punkCCSrc;
	pSrc->GetColorConvertor(&punkCCSrc);
	IColorConvertor2Ptr sptrCCSrc(punkCCSrc);
	if (sptrCCSrc == 0) return;
	_bstr_t bstrCnvSrc;
	sptrCCSrc->GetCnvName(bstrCnvSrc.GetAddress());
//	_point ptDest(ptDest1.x/nZoom,ptDest1.y/nZoom);
	_point ptDest(ptDest1);
	_size szDest(sz.cx/nZoom,sz.cy/nZoom);
	if (bstrCnvDest == _bstr_t("Gray"))
	{
		if (bstrCnvSrc == _bstr_t("Gray"))
		{
			for (int y = 0; y < szDest.cy; y++)
			{
				color *pRowDest;
				pDest->GetRow(ptDest.y+y, 0, &pRowDest);
				color *pRowSrc;
				pSrc->GetRow(ptSrc.y+y*nZoom, 0, &pRowSrc);
				WORD *pRowDMTot;
				pTotalDM->GetRow(ptDest.y+y, &pRowDMTot);
				WORD *pRowDMCur;
				pCurDM->GetRow(ptDest.y+y, &pRowDMCur);
				for (int x = 0; x < szDest.cx; x++)
					pRowDest[ptDest.x+x] = MakePixel(pRowDest[ptDest.x+x],
						pRowSrc[ptSrc.x+x*nZoom],pRowDMTot[ptDest.x+x],
						pRowDMCur[ptDest.x+x]);
			}
		}
	}
	else
	{
		if (bstrCnvSrc == _bstr_t("Gray"))
		{
			for (int y = 0; y < szDest.cy; y++)
			{
				color *pRowDestR,*pRowDestG,*pRowDestB;
				pDest->GetRow(ptDest.y+y, 0, &pRowDestR);
				pDest->GetRow(ptDest.y+y, 1, &pRowDestG);
				pDest->GetRow(ptDest.y+y, 2, &pRowDestB);
				color *pRowSrc;
				pSrc->GetRow(ptSrc.y+y*nZoom, 0, &pRowSrc);
				WORD *pRowDMTot;
				pTotalDM->GetRow(ptDest.y+y, &pRowDMTot);
				WORD *pRowDMCur;
				pCurDM->GetRow(ptDest.y+y, &pRowDMCur);
				for (int x = 0; x < szDest.cx; x++)
				{
					pRowDestR[ptDest.x+x] = MakePixel(pRowDestR[ptDest.x+x],
						pRowSrc[ptSrc.x+x*nZoom],pRowDMTot[ptDest.x+x],
						pRowDMCur[ptDest.x+x]);
					pRowDestG[ptDest.x+x] = MakePixel(pRowDestG[ptDest.x+x],
						pRowSrc[ptSrc.x+x*nZoom],pRowDMTot[ptDest.x+x],
						pRowDMCur[ptDest.x+x]);
					pRowDestB[ptDest.x+x] = MakePixel(pRowDestB[ptDest.x+x],
						pRowSrc[ptSrc.x+x*nZoom],pRowDMTot[ptDest.x+x],
						pRowDMCur[ptDest.x+x]);
				}
			}
		}
		else
		{
			for (int y = 0; y < szDest.cy; y++)
			{
				color *pRowDestR,*pRowDestG,*pRowDestB;
				pDest->GetRow(ptDest.y+y, 0, &pRowDestR);
				pDest->GetRow(ptDest.y+y, 1, &pRowDestG);
				pDest->GetRow(ptDest.y+y, 2, &pRowDestB);
				color *pRowSrcR,*pRowSrcG,*pRowSrcB;
				pSrc->GetRow(ptSrc.y+y*nZoom, 0, &pRowSrcR);
				pSrc->GetRow(ptSrc.y+y*nZoom, 1, &pRowSrcG);
				pSrc->GetRow(ptSrc.y+y*nZoom, 2, &pRowSrcB);
				WORD *pRowDMTot;
				pTotalDM->GetRow(ptDest.y+y,&pRowDMTot);
				WORD *pRowDMCur;
				pCurDM->GetRow(ptDest.y+y,&pRowDMCur);
				for (int x = 0; x < szDest.cx; x++)
				{
					pRowDestR[ptDest.x+x] = MakePixel(pRowDestR[ptDest.x+x],
						pRowSrcR[ptSrc.x+x*nZoom],pRowDMTot[ptDest.x+x],
						pRowDMCur[ptDest.x+x]);
					pRowDestG[ptDest.x+x] = MakePixel(pRowDestG[ptDest.x+x],
						pRowSrcG[ptSrc.x+x*nZoom],pRowDMTot[ptDest.x+x],
						pRowDMCur[ptDest.x+x]);
					pRowDestB[ptDest.x+x] = MakePixel(pRowDestB[ptDest.x+x],
						pRowSrcB[ptSrc.x+x*nZoom],pRowDMTot[ptDest.x+x],
						pRowDMCur[ptDest.x+x]);
				}
			}
		}
	}
}

void CopyImage(IImage *pDest, IImage *pSrc, POINT ptDest, POINT ptSrc, SIZE sz,
	double dZoom, IDistanceMap *pTotalDM, IDistanceMap *pCurDM)
{
	CSize szDest((int)ceil(sz.cx*dZoom),(int)ceil(sz.cy*dZoom));
	int nPanesDst = ::GetImagePaneCount(pDest);
	int nPanesSrc = ::GetImagePaneCount(pSrc);
	_ptr_t2<int> arXF(szDest.cx),arXC(szDest.cx),arKX(szDest.cx);
	for (int x = 0; x < szDest.cx; x++)
	{
		double dx = x/dZoom;
		double dxf = floor(dx);
		double dxc = ceil(dx);
		if (dxc >= sz.cx)
			dxc = dxf;
		arXF[x] = (int)dxf;
		arXC[x] = (int)dxc;
		arKX[x] = (int)(256.*(dx-dxf));
	}
	for (int y = 0; y < szDest.cy; y++)
	{
		WORD *pRowDMTot;
		pTotalDM->GetRow(ptDest.y+y, &pRowDMTot);
		WORD *pRowDMCur;
		pCurDM->GetRow(ptDest.y+y, &pRowDMCur);
		for (int i = 0; i < nPanesDst; i++)
		{
			color *pRowDest;
			pDest->GetRow(ptDest.y+y, i, &pRowDest);
			color *pRowSrc1,*pRowSrc2;
			double dy = y/dZoom;
			double dyf = floor(dy);
			double dyc = ceil(dy);
			if (dyc >= sz.cy)
				dyc = dyf;
			int ky = (int)(256.*(dy-dyf));
			pSrc->GetRow(ptSrc.y+(int)dyf, i, &pRowSrc1);
			pSrc->GetRow(ptSrc.y+(int)dyc, i, &pRowSrc2);
			for (int x = 0; x < szDest.cx; x++)
			{
				int xf = arXF[x];
				int xc = arXC[x];
				int kx = arKX[x];
				DWORD dwYF = (((DWORD)pRowSrc1[xf])*(256-kx)+((DWORD)pRowSrc1[xc])*kx)/256;
				DWORD dwYC = (((DWORD)pRowSrc2[xf])*(256-kx)+((DWORD)pRowSrc2[xc])*kx)/256;
				DWORD dwColor = (dwYF*(256-ky)+dwYC*ky)/256;
				pRowDest[ptDest.x+x] = MakePixel(pRowDest[ptDest.x+x],
					(color)dwColor, pRowDMTot[ptDest.x+x], pRowDMCur[ptDest.x+x]);
			}
		}
	}
}

void FillImage(IImage *pDest, int cx, int cy, bool bGray, COLORREF clrBack)
{
	if (bGray)
	{
		color nBright = Bright(GetRValue(clrBack),GetGValue(clrBack),GetBValue(clrBack));
		nBright <<= 8;
		for (int y = 0; y < cy; y++)
		{
			color *pRowDest;
			pDest->GetRow(y, 0, &pRowDest);
			for (int x = 0; x < cx; x++)
				pRowDest[x] = nBright;
		}
	}
	else
	{
		color clrR = GetRValue(clrBack)<<8;
		color clrG = GetGValue(clrBack)<<8;
		color clrB = GetBValue(clrBack)<<8;
		for (int y = 0; y < cy; y++)
		{
			color *pRowDestR,*pRowDestG,*pRowDestB;
			pDest->GetRow(y, 0, &pRowDestR);
			pDest->GetRow(y, 1, &pRowDestG);
			pDest->GetRow(y, 2, &pRowDestB);
			for (int x = 0; x < cx; x++)
			{
				pRowDestR[x] = clrR;
				pRowDestG[x] = clrG;
				pRowDestB[x] = clrB;
			}
		}
	}
}

static bool IsDirChar(char ch)
{
	return ch == '\\' || ch == '/';
};

static bool CheckCreateDir(const char *pszDir)
{
	char szMsgFmt[256];
	char szMsg[_MAX_PATH+256];
	WIN32_FIND_DATA ffd;
	HANDLE hf = FindFirstFile(pszDir, &ffd);
	if (hf == INVALID_HANDLE_VALUE)
	{
		int r = _mkdir(pszDir);
		if (r != 0)
		{
			LoadString(app::handle(), IDS_DIR_NOT_CREATED, szMsgFmt, 256);
			wsprintf(szMsg,szMsgFmt,pszDir);
			VTMessageBox(szMsg, "VT5", MB_OK|MB_ICONEXCLAMATION);
			return false;
		}
	}
	else
	{
		FindClose(hf);
		if ((ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			LoadString(app::handle(), IDS_DIR_NOT_CREATED, szMsgFmt, 256);
			wsprintf(szMsg,szMsgFmt,pszDir);
			VTMessageBox(szMsg, "VT5", MB_OK|MB_ICONEXCLAMATION);
			return false;
		}
	}
	return true;
}

static bool IsDot(const char *pszFName)
{
	return strcmp(pszFName,".")==0||strcmp(pszFName,"..")==0;
}

static bool CheckForSLIDir(const char *pszDir)
{
	char szPath[_MAX_PATH];
	strcpy(szPath, pszDir);
	strcat(szPath, "sli.vt5");
	WIN32_FIND_DATA ffd;
	HANDLE hf = FindFirstFile(szPath, &ffd);
	if (hf == INVALID_HANDLE_VALUE)
		return false;
	return true;
}

bool RemoveDirContent(const char *pszDir, bool bMessage, bool bSliDir)
{
	char szPath[_MAX_PATH];
	char szMsgFmt[256];
	char szMsg[_MAX_PATH+256];
	strcpy(szPath, pszDir);
	int nLen = strlen(pszDir);
	if (!IsDirChar(szPath[nLen-1]))
		strcat(szPath,"\\");
	strcat(szPath, "*");
	WIN32_FIND_DATA ffd;
	HANDLE hf = FindFirstFile(szPath, &ffd);
	if (hf == INVALID_HANDLE_VALUE)
		return true;
	bool bNotEmpty = false;
	while (1)
	{
		if (!IsDot(ffd.cFileName))
		{
			if (!bNotEmpty)
			{
				if (bMessage)
				{
					if (bSliDir)
					{
						LoadString(app::handle(), IDS_DIR_NOT_EMPTY, szMsgFmt, 256);
						wsprintf(szMsg,szMsgFmt,pszDir);
						if (VTMessageBox(szMsg, "VT5", MB_YESNO|MB_ICONEXCLAMATION) == IDNO)
							return false;
					}
					else
					{
						LoadString(app::handle(), IDS_DIR_NOT_EMPTY_NOT_SLI, szMsgFmt, 256);
						wsprintf(szMsg,szMsgFmt,pszDir);
						VTMessageBox(szMsg, "VT5", MB_OK|MB_ICONEXCLAMATION);
						return false;
					}
				}
				bNotEmpty = true;
			}
			char szFilePath[_MAX_PATH];
			strcpy(szFilePath,pszDir);
			if (!IsDirChar(szFilePath[nLen-1]))
				strcat(szFilePath,"\\");
			strcat(szFilePath,ffd.cFileName);
			if (ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				if (!RemoveDirContent(szFilePath, false, bSliDir))
					return false;
				int r = _rmdir(szFilePath);
				if (r == -1)
				{
					LoadString(app::handle(), IDS_FILE_NOT_REMOVED, szMsgFmt, 256);
					wsprintf(szMsg,szMsgFmt,szFilePath);
					VTMessageBox(szMsg, "VT5", MB_OK|MB_ICONEXCLAMATION);
					return false;
				}
			}
			else
			{
				int r = _unlink(szFilePath);
				if (r == -1)
				{
					LoadString(app::handle(), IDS_FILE_NOT_REMOVED, szMsgFmt, 256);
					wsprintf(szMsg,szMsgFmt,szFilePath);
					VTMessageBox(szMsg, "VT5", MB_OK|MB_ICONEXCLAMATION);
					return false;
				}
			}
		}
		if (!FindNextFile(hf, &ffd))
			break;
	}
	FindClose(hf);
	return true;
};

bool RecursiveCopyDir(const char *pszSrcDir, const char *pszDstDir)
{
	char szSrcPath[_MAX_PATH],szDstPath[_MAX_PATH];
	strcpy(szSrcPath, pszSrcDir);
	int nLen = strlen(pszSrcDir);
	if (!IsDirChar(szSrcPath[nLen-1]))
		strcat(szSrcPath,"\\");
	strcat(szSrcPath, "*");
	strcpy(szDstPath, pszDstDir);
	int nLen1 = strlen(pszDstDir);
	if (IsDirChar(szDstPath[nLen1-1]))
		szDstPath[nLen1-1] = 0;
	_mkdir(szDstPath);
	strcat(szDstPath,"\\");
	WIN32_FIND_DATA ffd;
	HANDLE hf = FindFirstFile(szSrcPath, &ffd);
	if (hf == INVALID_HANDLE_VALUE)
		return true;
	bool bNotEmpty = false;
	while (1)
	{
		if (!IsDot(ffd.cFileName))
		{
			if (!bNotEmpty)
				bNotEmpty = true;
			char szFilePath[_MAX_PATH];
			strcpy(szFilePath,pszSrcDir);
			if (!IsDirChar(szFilePath[nLen-1]))
				strcat(szFilePath,"\\");
			strcat(szFilePath,ffd.cFileName);
			char szDFilePath[_MAX_PATH];
			strcpy(szDFilePath,pszDstDir);
			if (!IsDirChar(szDFilePath[nLen1-1]))
				strcat(szDFilePath,"\\");
			strcat(szDFilePath,ffd.cFileName);
			if (ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				if (!RecursiveCopyDir(szFilePath,szDFilePath))
					return false;
			}
			else
				CopyFile(szFilePath,szDFilePath,FALSE);
		}
		if (!FindNextFile(hf, &ffd))
			break;
	}
	FindClose(hf);
	return true;
}


bool CheckDirectory(const char *pszDir, GUID guidDoc)
{
	if (pszDir[0] == 0) return false;
	char szPath[_MAX_PATH];
	// 1. Check for existance of the file path.
	for (int i = 1; pszDir[i] != 0; i++)
	{
		if (IsDirChar(pszDir[i]) && pszDir[i-1] != ':' && !IsDirChar(i-1) && !
			IsDirChar(i+1))
		{
			strncpy(szPath, pszDir, i);
			szPath[i] = 0;
			if (!CheckCreateDir(szPath))
				return false;
		}
	};
	if (i == 0) return false;
	strcpy(szPath, pszDir);
	if (!IsDirChar(pszDir[i-1]))
	{
		strcat(szPath, "\\");
		if (!CheckCreateDir(szPath))
			return false;
	}
	// 2. Path exists. Empty the directory.
	bool bSliDir = CheckForSLIDir(szPath);
	if (!RemoveDirContent(szPath, true, bSliDir))
		return false;
	char szPathSli[_MAX_PATH];
	strcpy(szPathSli, szPath);
	strcat(szPathSli, "sli.vt5");
	int h = open(szPathSli, _O_CREAT|_O_RDWR|_O_TEXT|_O_TRUNC, _S_IREAD|_S_IWRITE);
	if (h == 0)
	{
		char szMsgFmt[256],szMsg[256+_MAX_PATH];
		LoadString(app::handle(), IDS_FILE_NOT_CREATED, szMsgFmt, 256);
		wsprintf(szMsg,szMsgFmt,szPathSli);
		VTMessageBox(szMsg, "VT5", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	char *pszText = "VT5 Sew Large Image directory \n";
	write(h, pszText, strlen(pszText));
	wchar_t szWBuff[512];
	StringFromGUID2(guidDoc, szWBuff, 512);
	char szBuff[512];
	WideCharToMultiByte(CP_ACP, 0, szWBuff, -1, szBuff, 512, NULL, NULL);
	strcat(szBuff, "\n");
	write(h, szBuff, strlen(szBuff));
	close(h);
	return true;
}

bool CheckSliVt5(const char *pszPathName, GUID guidDoc)
{
	FILE *f = fopen(pszPathName, "rt");
	bool bOk = false;
	if (f)
	{
		char szBuff[512];
		fgets(szBuff,sizeof(szBuff),f);
		if (strncmp(szBuff,VT5SLISIGNATURE,strlen(VT5SLISIGNATURE))	== 0)
		{
			fgets(szBuff,sizeof(szBuff),f);
			wchar_t szWBuff[512];
			MultiByteToWideChar(CP_ACP, 0, szBuff, -1, szWBuff, 512);
			GUID guid;
			CLSIDFromString(szWBuff, (LPCLSID)&guid);
			if (guid == guidDoc)
				bOk = true;
		}
		fclose(f);
	}
	return bOk;
}

bool IsDirectoryExist(const char *pszDir)
{
	int nLen = strlen(pszDir);
	if (IsDirChar(pszDir[nLen-1]))
		nLen--;
	char szDir[_MAX_PATH];
	strncpy(szDir,pszDir,nLen);
	szDir[nLen] = 0;
	WIN32_FIND_DATA ffd;
	HANDLE hf = FindFirstFile(szDir, &ffd);
	if (hf == INVALID_HANDLE_VALUE)
		return false;
	bool bDir = (ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0;
	FindClose(hf);
	return bDir;
}

IUnknownPtr BuildZoomedImage(IUnknown *punkImg11, int nZoom)
{
	IImagePtr imgSrc(punkImg11);
	int cx,cy;
	imgSrc->GetSize(&cx,&cy);
	IUnknownPtr punkImgNew(::CreateTypedObject(_bstr_t(szArgumentTypeImage)), false);
	ICompatibleObjectPtr sptrComp(punkImgNew);
	int cx1 = cx/nZoom;
	int cy1 = cy/nZoom;
	ImageCloneData icd = {0};
	icd.rect = CRect(0,0,cx1,cy1);
	sptrComp->CreateCompatibleObject(punkImg11, &icd, sizeof(icd));
	IImagePtr imgNew(punkImgNew);
	int nPanes = ::GetImagePaneCount(punkImg11);
	for (int y = 0; y < cy1; y++)
	{
		for (int c = 0; c < nPanes; c++)
		{
			color *pRowSrc,*pRowDst;
			imgSrc->GetRow(y*nZoom, c, &pRowSrc);
			imgNew->GetRow(y, c, &pRowDst);
			for (int x = 0; x < cx1; x++)
				pRowDst[x] = pRowSrc[x*nZoom];
		}
	}
	return punkImgNew;
}

CRect GetFragmentRect(ISewFragment *pFrag)
{
	POINT ptFrag;
	pFrag->GetOffset(&ptFrag);
	SIZE szFrag;
	pFrag->GetSize(&szFrag);
	return CRect(ptFrag, szFrag);
}

ISewFragmentPtr GetLastFragByPoint(TPOS lPos, ISewImageList *pSLI, POINT point)
{
	ISewFragmentPtr sptrFragFound;
	while (lPos)
	{
		ISewFragmentPtr sptrFrag;
		pSLI->GetPrevFragment(&lPos, &sptrFrag);
		POINT ptFrag;
		sptrFrag->GetOffset(&ptFrag);
		SIZE szFrag;
		sptrFrag->GetSize(&szFrag);
		if (point.x >= ptFrag.x && point.x < ptFrag.x+szFrag.cx &&
			point.y >= ptFrag.y && point.y < ptFrag.y+szFrag.cy)
		{
			sptrFragFound = sptrFrag;
			break;
		}
	}
	return sptrFragFound;
}


class _CLibrary
{
public:
	HINSTANCE hinstLib;
	BOOL(*procLoadPic)(BSTR,IUnknown**) ;
	BOOL(*procSavePic)(BSTR,IUnknown*,IUnknown*) ;
	_CLibrary()
	{
		hinstLib=LoadLibrary( "comps\\sewLoad.dll" );
		procLoadPic = (BOOL(*)(BSTR,IUnknown**)) GetProcAddress(hinstLib, "SewLoadPic"); 
		procSavePic = (BOOL(*)(BSTR,IUnknown*,IUnknown*)) GetProcAddress(hinstLib, "SewSavePic"); 
	}
	~_CLibrary()
	{
		if(hinstLib) FreeLibrary(hinstLib);
	}
};

static _CLibrary s_LoadLib;

bool DoLoadImage(BSTR bstrPathName, IUnknown **ppunkImage)
{
	return s_LoadLib.procLoadPic(bstrPathName, ppunkImage)==TRUE;
}

bool DoSaveImage(BSTR bstrPathName, IUnknown *punkImage, IUnknown* pNamedData)
{
	return s_LoadLib.procSavePic(bstrPathName, punkImage, pNamedData)==TRUE;
}


void FreeFragment(void *p)
{
	((ISewFragment *)p)->Release();
}

TPOS	CFragList::insert_before(ISewFragment *p, TPOS lpos)
{
	p->AddRef();
	return __super::insert_before(p, lpos);
}

TPOS	CFragList::insert(ISewFragment *p, TPOS lpos)
{
	p->AddRef();
	return __super::insert(p, lpos);
}

CSewLoadedFragment::CSewLoadedFragment(ISewFragment *pFrag, int nZoom, TPOS lpos)
{
	m_pFrag = pFrag;
	m_nZoom = nZoom;
	m_lpos = lpos;
	m_pFrag->AddRef();
	m_pFrag->LoadImage(m_nZoom);
}

CSewLoadedFragment::~CSewLoadedFragment()
{
	m_pFrag->FreeImage(m_nZoom);
	m_pFrag->Release();
}

void FreeAndUnloadFragment(void *p)
{
	delete (CSewLoadedFragment*)p;
}

CLoadFragment::CLoadFragment()
{
	m_pFrag = NULL;
	m_nZoom = 0;
}

CLoadFragment::CLoadFragment(ISewFragment *pFrag, int nZoom)
{
	m_pFrag = pFrag;
	m_nZoom = nZoom;
	m_pFrag->AddRef();
	m_pFrag->LoadImage(m_nZoom);
}

void CLoadFragment::Load(ISewFragment *pFrag, int nZoom)
{
	if (m_pFrag)
	{
		m_pFrag->FreeImage(m_nZoom);
		m_pFrag->Release();
		m_pFrag = NULL;
	}
	m_pFrag = pFrag;
	m_nZoom = nZoom;
	if (m_pFrag)
	{
		m_pFrag->AddRef();
		m_pFrag->LoadImage(m_nZoom);
	}
}

CLoadFragment::~CLoadFragment()
{
	if (m_pFrag)
	{
		m_pFrag->FreeImage(m_nZoom);
		m_pFrag->Release();
	}
}

void CLoadFragment::Unload()
{
	if (m_pFrag)
	{
		m_pFrag->FreeImage(m_nZoom);
		m_pFrag->Release();
		m_pFrag = NULL;
		m_nZoom = 0;
	}
}

CLockDocument::CLockDocument(IUnknown *punk)
{
	m_ptrDS = punk;
	m_ptrDS->IsDocumentLocked(&m_bPrevLock);
	m_ptrDS->LockDocument(TRUE);
}

CLockDocument::~CLockDocument()
{
	if (m_ptrDS != 0 && !m_bPrevLock)
		m_ptrDS->LockDocument(FALSE);
}

CSetBool::CSetBool(bool *pbValue, bool bValSet)
{
	m_pbValue = pbValue;
	m_bPrevValue = *m_pbValue;
	*m_pbValue = bValSet;
}

CSetBool::~CSetBool()
{
	*m_pbValue = m_bPrevValue;
}
