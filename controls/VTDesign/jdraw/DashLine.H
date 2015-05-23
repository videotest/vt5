// Line.h : main header file for the CDashLine
//
//	The Bresenham function in this file is derived from code from 
//			Jean-Claude Lanz mailto:Jclanz@bluewin.ch
//		and he presumably shares copyright to it
//	Otherwise the copyright belongs to Llew S. Goodstadt 
//		http://www.lg.ndirect.co.uk    mailto:lg@ndirect.co.uk
//		who hereby grants you fair use and distribution rights of the code 
//		in both commercial and non-commercial applications.
//
/////////////////////////////////////////////////////////////////////////////

#include "types.h"

#ifndef _DASHLINE_H__
#define _DASHLINE_H__
// CDashLine

class CDashLine
{
public:
	CDashLine(CDC& dc, unsigned* pattern, unsigned count);
	~CDashLine();
	void SetPattern(unsigned* pattern, unsigned count);
// Returns count of elements (dash/dot and gaps)
// You must be careful to pass in enough memory for pattern
// It is probably safest to always have an array of [8]
	static unsigned GetPattern(unsigned* pattern, bool round, unsigned pensize, unsigned style);

protected:
	CDC& m_DC;
	unsigned int	m_CurPat;
	unsigned int	m_Count;
	unsigned int	m_CurStretch;
	unsigned int*	m_Pattern;
	CPoint			m_CurPos;

	void Reset();
	void Bresenham(LONG x, LONG y);

public:
	void Rectangle(CRect& rect, COLORREF color, int penWidth, BOOL bRound, unsigned style );
	void BezierTo(POINT* dest);
	void MoveTo(const POINT& p) {MoveTo(p.x, p.y);}
	void MoveTo(int x, int y);
	void LineTo(const POINT& p) {LineTo(p.x, p.y);}
	void LineTo(int x, int y);
};

#endif//_DASHLINE_H__