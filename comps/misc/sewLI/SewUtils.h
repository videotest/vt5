#pragma once

#include "misc_list.h"
#include "ISewLi.h"
#include "DistMap.h"
#include "ini_values.h"
#include "atltypes.h"

extern SIZE g_szBase;
extern int g_nRangeCoef;
int GetRangeBySize(SIZE sz, int nRangeCoef);
SIZE GetSizeByRange(int nRange, int nRangeCoef);
int GetZoomByRange(int nRange, int nRangeCoef);
int GetViewRangeByZoom(double dZoom, int nRangeCoef);
bool DoLoadImage(BSTR bstrPathName, IUnknown **ppunkImage);
bool DoSaveImage(BSTR bstrPathName, IUnknown *punkImage, IUnknown* pNamedData);
inline int Range(int n, int nMin, int nMax) {return n<nMin?nMin:n>nMax?nMax:n;}

void CopyImage(IImage *pDest, IImage *pSrc, POINT ptDest, POINT ptSrc, SIZE sz,
	int nZoom, IDistanceMap *pTotalDM, IDistanceMap *pCurDM);
void CopyImage(IImage *pDest, IImage *pSrc, POINT ptDest, POINT ptSrc, SIZE sz,
	double dZoom, IDistanceMap *pTotalDM, IDistanceMap *pCurDM);
void FillImage(IImage *pDest, int cx, int cy, bool bGray, COLORREF clrBack);
IUnknownPtr BuildZoomedImage(IUnknown *punkImg11, int nZoom);

bool RemoveDirContent(const char *pszDir, bool bMessage, bool bSliDir);
bool IsDirectoryExist(const char *pszDir);
bool CheckDirectory(const char *pszDir, GUID guid);
bool CheckSliVt5(const char *pszDir, GUID guid);
bool RecursiveCopyDir(const char *pszSrcDir, const char *pszDstDir);

CRect GetFragmentRect(ISewFragment *pFrag);
ISewFragmentPtr GetLastFragByPoint(TPOS lPos, ISewImageList *pSLI, POINT point);

void FreeAndUnloadFragment(void *p);
void FreeFragment(void *p);
class CFragList : public _list_t2<ISewFragment *,FreeFragment>
{
public:
	TPOS	insert_before(ISewFragment *p, TPOS lpos = 0);
	TPOS	insert(ISewFragment *p, TPOS lpos = 0);
};

struct CSewLoadedFragment
{
	ISewFragment *m_pFrag;
	int m_nZoom;
	TPOS m_lpos;
	CSewLoadedFragment(ISewFragment *pFrag, int nZoom, TPOS lpos);
	~CSewLoadedFragment();
};

typedef _list_t2<CSewLoadedFragment *,FreeAndUnloadFragment> CLoadFragList;

class CLoadFragment
{
	ISewFragment *m_pFrag;
	int m_nZoom;
public:
	CLoadFragment();
	CLoadFragment(ISewFragment *pFrag, int nZoom);
	void Load(ISewFragment *pFrag, int nZoom);
	void Unload();
	~CLoadFragment();

	ISewFragment *GetFrag() {return m_pFrag;}
	int GetZoom() {return m_nZoom;}
};

class CLockDocument
{
	IDocumentSite2Ptr m_ptrDS;
	BOOL m_bPrevLock;
public:
	CLockDocument(IUnknown *punk);
	~CLockDocument();
};

class CSetBool
{
	bool *m_pbValue;
	bool m_bPrevValue;
public:
	CSetBool(bool *pbValue, bool bValSet);
	~CSetBool();
};

extern CIntIniValue g_SmoothTransition;
extern CColorIniValue g_IntersectionColor1;
extern CColorIniValue g_IntersectionColor2;
