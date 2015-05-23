#include "stdafx.h"
#include "DistMap.h"

CDistMap::CDistMap()
{
	m_ppRows = NULL;
	m_cx = m_cy = 0;
}

CDistMap::~CDistMap()
{
	Free();
	m_nRefCounter = 0;
}

void CDistMap::Free()
{
	for (unsigned y = 0; y < m_cy; y++)
		delete m_ppRows[y];
	delete m_ppRows;
	m_ppRows = NULL;
}

HRESULT CDistMap::GetRow(int nRow, WORD **ppRow)
{
	*ppRow = m_ppRows[nRow];
	return S_OK;
}

HRESULT CDistMap::GetCX(int *pcx)
{
	*pcx = m_cx;
	return S_OK;
}

HRESULT CDistMap::GetCY(int *pcy)
{
	*pcy = m_cy;
	return S_OK;
}


HRESULT CDistMap::CreateNew(int cx, int cy)
{
	Free();
	m_ppRows = new WORD*[cy];
	for (int y = 0; y < cy; y++)
	{
		m_ppRows[y] = new WORD[cx];
		memset(m_ppRows[y], 0, cx*sizeof(WORD));
	}
	m_cx = cx;
	m_cy = cy;
	return S_OK;
}

HRESULT CDistMap::Destroy()
{
	Free();
	m_ppRows = NULL;
	m_cx = m_cy = 0;
	return S_OK;
}


HRESULT CDistMap::AddRect(RECT rc)
{
	int x,y;
	RECT rc1;
	rc1.left = min(max(rc.left,0),(int)m_cx);
	rc1.right = min(max(rc.right,0),(int)m_cx);
	rc1.top = min(max(rc.top,0),(int)m_cy);
	rc1.bottom = min(max(rc.bottom,0),(int)m_cy);
	int xMed = (rc.left+rc.right)/2;
	for (y = rc1.top; y < rc1.bottom; y++)
	{
		WORD *pRow;
		GetRow(y, &pRow);
		int nDistY = min(y-rc.top+1,rc.bottom-y);
		int xr = min(xMed,rc1.right);
		for (x = rc1.left; x < xr; x++)
			pRow[x] += (WORD)min(nDistY,x-rc.left+1);
		for (; x < rc1.right; x++)
			pRow[x] += (WORD)min(nDistY,rc.right-x);
	}
	return S_OK;
}


CTranspMap::CTranspMap()
{
	m_ppRows = NULL;
	m_cx = m_cy = 0;
}

CTranspMap::~CTranspMap()
{
	Deinit();
	m_nRefCounter = 0;
}

void CTranspMap::Deinit()
{
	for (unsigned y = 0; y < m_cy; y++)
		delete m_ppRows[y];
	delete m_ppRows;
}

HRESULT CTranspMap::CreateNew( int cx, int cy )
{
	Deinit();
	m_ppRows = new byte*[cy];
	for (int y = 0; y < cy; y++)
	{
		m_ppRows[y] = new byte[cx];
		memset(m_ppRows[y], 0, cx);
	}
	m_cx = cx;
	m_cy = cy;
	return S_OK;
}

HRESULT CTranspMap::Free()
{
	Deinit();
	m_ppRows = NULL;
	m_cx = m_cy = 0;
	return S_OK;
}

HRESULT CTranspMap::GetRow( int nRow, byte **pprow )
{
	*pprow = m_ppRows[nRow];
	return S_OK;
}

HRESULT CTranspMap::GetSize( int *pnCX, int *pnCY )
{
	*pnCX = m_cx;
	*pnCY = m_cy;
	return S_OK;
}

void MakeTranspMapByDistMaps(CTranspMap &TranspMap, CDistMap &DMTotal,
	CDistMap &DMCur, POINT ptOffs, SIZE szTM)
{
	TranspMap.CreateNew(szTM.cx,szTM.cy);
	int cxTotal,cyTotal;
	DMTotal.GetCX(&cxTotal);
	DMTotal.GetCY(&cyTotal);
	int nLeft = min(max(0,ptOffs.x),szTM.cx);
	int nRight = min(max(0,ptOffs.x+cxTotal),szTM.cx);
	int nTop = min(max(0,ptOffs.y),szTM.cy);
	int nBottom = min(max(0,ptOffs.y+cyTotal),szTM.cy);
	if (nRight-nLeft==0||nBottom-nTop==0) return;
	for (int y = nTop; y < nBottom; y++)
	{
		WORD *pRowTot;
		DMTotal.GetRow(y-ptOffs.y,&pRowTot);
		WORD *pRowCur;
		DMCur.GetRow(y-ptOffs.y,&pRowCur);
		byte *pRowDst;
		TranspMap.GetRow(y, &pRowDst);
		for (int x = nLeft; x < nRight; x++)
			pRowDst[x] = (byte)(((DWORD)pRowCur[x-ptOffs.x])*255/pRowTot[x-ptOffs.x]);
	}
}
