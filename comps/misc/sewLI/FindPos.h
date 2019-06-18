#pragma once

#include "iSewLI.h"

bool CorrectPos(ISewImageList *pFragList, ISewFragment *pFrag, POINT ptOrig, POINT *pptRes);
bool FindOptimalPosition(ISewImageList *pFragList, ISewFragment *pFragPrev, IImage3 *pimgNew, POINT *pptPos);
bool FindOptimalPosition1(ISewImageList *pFragList, ISewFragment *pFragPrev, IImage3 *pimgNew, POINT *pptPos);
bool FindOptimalPositionLive(ISewImageList *pFragList, ISewFragment *pFragPrev,
	LPBITMAPINFOHEADER lpbiLV, SIZE szLV, POINT *pptPos);
bool FindOptimalPositionByPoint(ISewImageList *pFragList, ISewFragment *pFragPrev,
	ISewFragment *pFragTest, POINT *pptPos);
//bool FindOptimalPositionByPointLive(ISewImageList *pFragList, ISewFragment *pFragPrev,
//	LPBITMAPINFOHEADER lpbiLV, SIZE szLV, POINT *pptPos);
bool FindOptimalPositionCorrect(ISewImageList *pFragList, ISewFragment *pFragPrev,
	IImage3 *pimgNew, double dZoom, POINT *pptPos);
bool FindOptimalPositionCorrectLive(ISewImageList *pFragList, ISewFragment *pFragPrev,
	LPBITMAPINFOHEADER lpbiLV, SIZE szLV, double dZoom, POINT *pptPos);
int CalcDifferenceLive(ISewImageList *pFragList, ISewFragment *pFragPrev,
	LPBITMAPINFOHEADER lpbiLV, SIZE szLV, POINT ptPos);
int GetOptimalDiff();

class CSewImage
{
public:
	byte	*m_pbytes;
	int		m_x, m_y, m_cx, m_cy;

	CSewImage();
	~CSewImage();
	void	SetImage(IImage3	*pimg, BOOL bContrast);
	void	SetImage(LPBITMAPINFOHEADER lpbi, SIZE sz, BOOL bContrast);
public:	
	byte *GetRow( int x, int y )
	{	x-=m_x;	y-=m_y;	return m_pbytes+x+y*m_cx;}

public:
	void	Contrast();
	void	Median();
	void	Kirsch();
};

