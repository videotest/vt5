#include "stdafx.h"
#include "nameconsts.h"
#include "image5.h"
#include "data5.h"
#include "misc_utils.h"
#include "DilateObjects.h"
#include <stdio.h>
#include <alloc.h>

#pragma intrinsic(_rotl8, _rotl16)

class CExpand
{
public:
	LPVOID m_lpBits1;
	LPVOID m_lpBits2;
	int m_nCol;
	int m_nCol64;
	int m_nRow;
	int m_nSize;
	void Init(IBinaryImage *pimg, int cx, int cy);
	void Deinit();
	void SetBits(IBinaryImage *pimg, int cx, int cy);
	void GetBits(IBinaryImage *pimg, int cx, int cy);
	BOOL Expand(bool bShift);
};

void CExpand::Init(IBinaryImage *pimg, int cx, int cy)
{
	m_nCol64 = (cx+63)/64;
	m_nCol = m_nCol64*8;
	m_nRow = cy;
	m_nSize = m_nCol*m_nRow;
	m_lpBits1 = new BYTE[m_nSize];
	memset(m_lpBits1, 0, m_nSize);
	m_lpBits2 = new BYTE[m_nSize];
	SetBits(pimg, cx, cy);
	memcpy(m_lpBits2, m_lpBits1, m_nSize);
}

void CExpand::Deinit()
{
	delete m_lpBits1;
	delete m_lpBits2;
}

void CExpand::SetBits(IBinaryImage *pimg, int cx, int cy)
{
//	LPBYTE lpData = ((LPBYTE)(lpbiIn+1))+1024;
//	int nSrcCol = ((lpbiIn->biWidth+3)>>2)<<2;
	BYTE btMask1 = 1;
	BYTE btMask2 = 0xFE;
	LPBYTE lpDst = (LPBYTE)m_lpBits1;
	int nCol8 = cx/8;
	for (int i = 0; i < m_nRow; i++)
	{
		LPBYTE lpSrc1;
		pimg->GetRow(&lpSrc1, i, TRUE);
		LPBYTE lpDst1 = lpDst;
		btMask1 = 1;
		btMask2 = 0xFE;
		for (int j1 = 0; j1 < nCol8; j1++)
		{
			for (int j2 = 0; j2 < 8; j2++)
			{
				if (*lpSrc1++)
					*lpDst1 |= btMask1;
				else
					*lpDst1 &= btMask2;
				btMask1 = _rotl8(btMask1,1);
				btMask2 = _rotl8(btMask2,1);
			}
			lpDst1++;
		}
		for (int j2 = 0; j2 < cx%8; j2++)
		{
			if (*lpSrc1++)
				*lpDst1 |= btMask1;
			else
				*lpDst1 &= btMask2;
				btMask1 = _rotl8(btMask1,1);
				btMask2 = _rotl8(btMask2,1);
		}
		lpDst1++;
		lpDst += m_nCol;
	}
}

void CExpand::GetBits(IBinaryImage *pimg, int cx, int cy)
{
//	LPBYTE lpData = ((LPBYTE)(lpbiOut+1))+1024;
	int nDataCol = ((cx+3)>>2)<<2;
	BYTE btMask = 1;
	LPBYTE lpBits = (LPBYTE)m_lpBits1;
	int nCol8 = cx/8;
	for (int i = 0; i < m_nRow; i++)
	{
		LPBYTE lpData1;
		pimg->GetRow(&lpData1, i, TRUE);
		LPBYTE lpBits1 = lpBits;
		btMask = 1;
		for (int j1 = 0; j1 < nCol8; j1++)
		{
			for (int j2 = 0; j2 < 8; j2++)
			{
				if ((*lpBits1)&btMask)
					*lpData1 = 0xFF;
				else
					*lpData1 = 0;
				lpData1++;
				btMask = _rotl8(btMask,1);
			}
			lpBits1++;
		}
		for (int j2 = 0; j2 < cx%8; j2++)
		{
			if ((*lpBits1)&btMask)
				*lpData1 = 0xFF;
			else
				*lpData1 = 0;
			lpData1++;
			btMask = _rotl8(btMask,1);
		}
		lpBits += m_nCol;
	}
}

BOOL CExpand::Expand(bool bShift)
{
	LPBYTE lpBits1 = ((LPBYTE)m_lpBits1)+m_nCol;
	LPBYTE lpBits2 = ((LPBYTE)m_lpBits2)+m_nCol;
	int nCol4 = m_nCol/4;
	int nCol4_ = bShift?nCol4-1:nCol4;
	if (bShift)
	{
		lpBits1 += 2;
		lpBits2 += 2;
	}
	DWORD dw,dwPrev;
	BOOL bRet = FALSE;
	int nRow = m_nRow-2;
	for (int i = 0; i < nRow; i++)
	{
		LPDWORD lp1 = (LPDWORD)lpBits1;
		LPDWORD lp2 = (LPDWORD)lpBits2;
		for (int j = 0; j < nCol4_; j++)
		{
			if (lp2[0]!=0xFFFFFFFF)
			{
				//     1  x  0
				//     1  0  0
				//     1  x  0
//				dw = (lp1[0]>>1)&(lp1[-nCol4]>>1)&(lp1[nCol4]>>1)&(~lp2[0]<<1)&(~lp2[-nCol4]<<1)&(~lp2[nCol4]<<1);
				dwPrev = lp2[0];
				dw = (lp1[0]>>1)&(lp1[-nCol4]>>1)&(lp1[nCol4]>>1)&(lp1[-nCol4]&lp1[nCol4]|(~lp2[0]<<1)&(~lp2[-nCol4]<<1)&(~lp2[nCol4]<<1));
				lp2[0] |= dw&0x7FFFFFFE;
				//     0  x  1
				//     0  0  1
				//     0  x  1
//				dw |= (lp1[0]<<1)&(lp1[-nCol4]<<1)&(lp1[nCol4]<<1)&(~lp2[0]>>1)&(~lp2[-nCol4]>>1)&(~lp2[nCol4]>>1);
				dw |= (lp1[0]<<1)&(lp1[-nCol4]<<1)&(lp1[nCol4]<<1)&(lp1[-nCol4]&lp1[nCol4]|(~lp2[0]>>1)&(~lp2[-nCol4]>>1)&(~lp2[nCol4]>>1));
				lp2[0] |= dw&0x7FFFFFFE;
				//     0  0  0
				//     x  0  x
				//     1  1  1
//				dw |= lp1[nCol4]&(lp1[nCol4]<<1)&(lp1[nCol4]>>1)&(~lp2[-nCol4])&(~lp2[-nCol4]<<1)&(~lp2[-nCol4]>>1);
				dw |= lp1[nCol4]&(lp1[nCol4]<<1)&(lp1[nCol4]>>1)&((lp1[0]>>1)&(lp1[0]<<1)|(~lp2[-nCol4])&(~lp2[-nCol4]<<1)&(~lp2[-nCol4]>>1));
				lp2[0] |= dw&0x7FFFFFFE;
				//     1  1  1
				//     x  0  x
				//     0  0  0
//				dw |= lp1[-nCol4]&(lp1[-nCol4]<<1)&(lp1[-nCol4]>>1)&(~lp2[nCol4])&(~lp2[nCol4]<<1)&(~lp2[nCol4]>>1);
				dw |= lp1[-nCol4]&(lp1[-nCol4]<<1)&(lp1[-nCol4]>>1)&((lp1[0]>>1)&(lp1[0]<<1)|(~lp2[nCol4])&(~lp2[nCol4]<<1)&(~lp2[nCol4]>>1));
				lp2[0] |= dw&0x7FFFFFFE;
				//     x  1  1
				//     0  0  1
				//     0  0  x
				dw |= lp1[-nCol4]&(lp1[-nCol4]>>1)&(lp1[0]>>1)&(~lp2[nCol4])&(~lp2[nCol4]<<1)&(~lp2[0]<<1);
				lp2[0] |= dw&0x7FFFFFFE;
				dw |= lp1[-nCol4]&(lp1[-nCol4]<<1)&(lp1[0]<<1)&(~lp2[nCol4])&(~lp2[nCol4]>>1)&(~lp2[0]>>1);
				lp2[0] |= dw&0x7FFFFFFE;
				dw |= lp1[nCol4]&(lp1[nCol4]>>1)&(lp1[0]>>1)&(~lp2[-nCol4])&(~lp2[-nCol4]<<1)&(~lp2[0]<<1);
				lp2[0] |= dw&0x7FFFFFFE;
				dw |= lp1[nCol4]&(lp1[nCol4]<<1)&(lp1[0]<<1)&(~lp2[-nCol4])&(~lp2[-nCol4]>>1)&(~lp2[0]>>1);
				lp2[0] |= dw&0x7FFFFFFE;
				if (lp2[0]&~dwPrev)
					bRet = TRUE;
			}
			lp1++;
			lp2++;
		}
		lpBits1 += m_nCol;
		lpBits2 += m_nCol;
	}
	if (bRet)
		memcpy(m_lpBits1, m_lpBits2, m_nSize);
	return bRet;
}

_ainfo_base::arg	CBinDilateObjectsInfo::s_pargs[] = 
{
	{"DilateCount",szArgumentTypeInt, "0", true, false },
	{"BinImage",szArgumentTypeBinaryImage, 0, true, true },
	{"BinResult",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinDilateObjects
////////////////////////////////////////////////////////////////////////////////////////////////////
CBinDilateObjects::CBinDilateObjects()
{
}

bool CBinDilateObjects::InvokeFilter()
{
	IBinaryImagePtr pSrc = GetDataArgument();
	IBinaryImagePtr pDst = GetDataResult();
	int nDilateCount = GetArgLong("DilateCount");
	if (nDilateCount == 0) nDilateCount = 1000000;
	if( pSrc==NULL || pDst==NULL) return false;
	_point offset;
	pSrc->GetOffset(&offset);
	int cx,cy;
	pSrc->GetSizes(&cx,&cy);
	if(pDst->CreateNew(cx, cy)!=S_OK) return false;
	pDst->SetOffset( offset, TRUE);
	StartNotification(cy);
	CExpand Expand;
	Expand.Init(pSrc, cx, cy);
	BOOL bCont = true;
	for (int i = 0; i < nDilateCount; i++)
	{
		bCont = Expand.Expand(false);
		if (!bCont) break;
		bCont = Expand.Expand(true);
		if (!bCont) break;
	}
	Expand.GetBits(pDst, cx, cy);
	Expand.Deinit();
	FinishNotification();
	return true;
}

