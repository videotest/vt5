// Accumulate.h: interface for the CAccumulate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCUMULATE_H__75797F78_BA77_445F_8BFF_A4AD4544D495__INCLUDED_)
#define AFX_ACCUMULATE_H__75797F78_BA77_445F_8BFF_A4AD4544D495__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef WORD AccPix;
const AccPix AccNull = 0;
typedef CTypedPtrArray<CPtrArray,AccPix*> AccArray;

class CAccumulate  
{
	int m_cx;
	int m_cy;
	int m_nImages;
	AccArray m_RowsR;
	AccArray m_RowsG;
	AccArray m_RowsB;
	RGBQUAD m_rgb[256];
	void DeinitArray();
	void InitNew(int nWidth, int nHeight, bool bRGB);
	void ReinitRGB();
	void ReinitSize(int cx, int cy);
	void DoAddGS(LPBITMAPINFOHEADER lpbi);
	void DoAddTrueColor(LPBITMAPINFOHEADER lpbi);
	void DoGetGS(LPBITMAPINFOHEADER lpbi);
	void DoGetTrueColor(LPBITMAPINFOHEADER lpbi);
	void InitHeader(LPBITMAPINFOHEADER lpbi);
public:
	CAccumulate();
	~CAccumulate();
	void Add(LPBITMAPINFOHEADER lpbi);
	bool IsTrueColor() {return m_RowsG.GetSize()!=0;};
	int GetCX() {return m_cx;};
	int GetCY() {return m_cy;};
	unsigned CalcSize();
	void GetResult(LPBITMAPINFOHEADER lpbi, bool bInitBI = false);
	void Clear();
};

#endif // !defined(AFX_ACCUMULATE_H__75797F78_BA77_445F_8BFF_A4AD4544D495__INCLUDED_)
