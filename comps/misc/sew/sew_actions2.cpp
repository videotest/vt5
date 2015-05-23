#include "stdafx.h"
#include "sew_actions2.h"
#include "nameconsts.h"
#include "misc_utils.h"
#include "misc_templ.h"
#include "data5.h"
#include "docview5.h"
#include "sew.h"
#include "stdio.h"
#include "math.h"

_ainfo_base::arg	CAutoSew2FilterInfo::s_pargs[] = 
{
	{"Stitch Result",	szArgumentTypeImage, 0, false, true },
	{"Img0",	szArgumentTypeImage, 0, true, true },
	{"Img1",	szArgumentTypeImage, 0, true, true },
	{0, 0, 0, false, false },
};

CAutoSew2Filter::CAutoSew2Filter()
{
	m_nMinOvr = 10;
	m_nStages = 1;
}

HRESULT CAutoSew2Filter::CanDeleteArgument(IUnknown *punk, BOOL *pbCan)
{
	*pbCan = FALSE;
	return TRUE;
}

void CAutoSew2Filter::MakeNotification(int nStage, int nNow, int nTotal)
{
	int nStepsInStage = 100/m_nStages;
	int nNotify = nNow*nStepsInStage/nTotal;
	nNotify += nStage*nStepsInStage;
	Notify(nNotify);
}

static int GetDifference(IImage3 *pimgBase, IImage3 *pimgSew, POINT ptOffset)
{
	int cxBase,cyBase,cxSew,cySew;
	pimgBase->GetSize(&cxBase, &cyBase);
	pimgSew->GetSize(&cxSew, &cySew);
	_rect rcBase(0,0,cxBase,cyBase); // sew rect in base coordinates
	if (ptOffset.x > 0)
		rcBase.left = ptOffset.x;
	if (ptOffset.x+cxSew < cxBase)
		rcBase.right = ptOffset.x+cxSew;
	if (ptOffset.y > 0)
		rcBase.top = ptOffset.y;
	if (ptOffset.y+cySew < cyBase)
		rcBase.bottom = ptOffset.y+cySew;
	_point ptSew(rcBase.left-ptOffset.x,rcBase.top-ptOffset.y);
	int nPanes = GetImagePaneCount(pimgBase);
	long lSumDiff = 0, lCount = 0;
	int n = 3;
	for (int yBase = rcBase.top; yBase < rcBase.bottom; yBase += n)
	{
		int ySew = yBase-ptOffset.y;
		color *pRowBase,*pRowSew;
		pimgBase->GetRow(yBase, 0, &pRowBase);
		pimgSew->GetRow(ySew, 0, &pRowSew);
		for (int xBase = rcBase.left; xBase < rcBase.right; xBase += n)
		{
			long l = pRowBase[xBase]-pRowSew[xBase-ptOffset.x];
			if (l < 0) l = -l;
			lSumDiff += l;
			lCount++;
		}
	}
	lSumDiff /= lCount;
	return lSumDiff;
}

static POINT s_aDirs1[] =
{
	{-1,0},
	{0,1},
	{1,0},
	{0,-1},
};

const int s_nDirs1 = 4;

static POINT s_aDirs2[] =
{
	{-2,0},
	{0,2},
	{2,0},
	{0,-2},
	{-1,1},
	{1,1},
	{-1,-1},
	{1,-1},
};

const int s_nDirs2 = 8;

static bool ScanDirections(_rect rcBound, LPWORD pDiffs, IImage3 *pimgBase, IImage3 *pimgSew,
	int &nFoundOpt, POINT ptOffs, POINT &ptOffs1, DWORD dwRow, POINT *aDirs, int nDirs)
{
	bool bFound = false;
	for (int i = 0; i < nDirs; i++)
	{
		POINT ptTry;
		ptTry.x = ptOffs.x+aDirs[i].x;
		if (ptTry.x < rcBound.left || ptTry.x >= rcBound.right)
			continue;
		ptTry.y = ptOffs.y+aDirs[i].y;
		if (ptTry.y < rcBound.top || ptTry.y >= rcBound.bottom)
			continue;
		int nPos = (ptTry.y-rcBound.left)*dwRow+(ptTry.x-rcBound.top);
		WORD n = pDiffs[nPos];
		if (n == 0xFFFF)
		{
			n = GetDifference(pimgBase, pimgSew, ptTry);
			pDiffs[nPos] = n;
		}
		if (n < nFoundOpt)
		{
			bFound = true;
			nFoundOpt = n;
			ptOffs1 = ptTry;
		}
	}
	return bFound;
}

POINT CAutoSew2Filter::GradientDecent(IImage3 *pimgRes, IImage3 *pimgBase, IImage3 *pimgSew)
{
	_point ptOffsStart(0,0);
	int cxBase,cyBase,cxSew,cySew;
	pimgBase->GetSize(&cxBase, &cyBase);
	pimgSew->GetSize(&cxSew, &cySew);
	_rect rcBound(-cxSew+m_nMinOvr,-cySew+m_nMinOvr,cxBase-m_nMinOvr,cyBase-m_nMinOvr);
	_ptr_t2<WORD> pDiffs(rcBound.width()*rcBound.height());
	memset(pDiffs.ptr,0xFF,rcBound.width()*rcBound.height()*sizeof(WORD));
	DWORD dwRow = rcBound.width();
	int nOptDiff = GetDifference(pimgBase, pimgSew, ptOffsStart);
	int nInitDiff = nOptDiff;
	POINT ptOffs = ptOffsStart;
	while (1)
	{
		int nFoundOpt = nOptDiff;
		POINT ptOffs1;
		bool bFound = ScanDirections(rcBound, pDiffs, pimgBase, pimgSew, nFoundOpt, ptOffs,
			ptOffs1, dwRow, s_aDirs1, s_nDirs1);
		if (!bFound)
		{
			bFound = ScanDirections(rcBound, pDiffs, pimgBase, pimgSew, nFoundOpt, ptOffs,
				ptOffs1, dwRow, s_aDirs2, s_nDirs2);
			if (!bFound)
				break;
		}
		ptOffs = ptOffs1;
		nOptDiff = nFoundOpt;
		MakeNotification(0, nInitDiff-nOptDiff, nInitDiff);
	}
	m_nFoundOpt = nOptDiff;
	return ptOffs;
}

bool CAutoSew2Filter::InvokeFilter()
{
	CHourglass	wait;
	DWORD	dwTickCount = GetTickCount();
	IImage3Ptr	imgDest( GetDataResult() );
	long	lReportTime = ::GetValueInt( GetAppUnknown(), "\\Sew", "TimeReport", 0 );
	IImage3Ptr imgBase(GetDataArgument("Img0"));
	IImage3Ptr imgSew(GetDataArgument("Img1"));
	StartNotification(100);
	m_nStages = 2;
	_point pt = GradientDecent(imgDest, imgBase, imgSew);
	FinishNotification();
	char szBuff[50];
	dwTickCount = GetTickCount()-dwTickCount;
	sprintf(szBuff, "(%d,%d) - %d, %d ms", pt.x, pt.y, m_nFoundOpt, dwTickCount);
	VTMessageBox(szBuff, "VT5", MB_OK|MB_ICONEXCLAMATION);
	if( lReportTime )
	{
		char	szTime[40];
		dwTickCount = GetTickCount()-dwTickCount;
		sprintf( szTime, "Takes %d ms", dwTickCount );
		MessageBox( 0, szTime, "SEW", MB_OK );
	}

	return true;
}

HRESULT CAutoSew2Filter::GetActionState(DWORD *pdwState)
{
	*pdwState = 1;
	return S_OK;
}
