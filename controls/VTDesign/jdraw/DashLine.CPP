// Line.cpp : Defines the class behaviors for CDashLine
//
//	The Bresenham function in this file is derived from code from 
//			Jean-Claude Lanz mailto:Jclanz@bluewin.ch
//		and he presumably shares copyright to it
//	Otherwise the copyright belongs to Llew S. Goodstadt 
//		http://www.lg.ndirect.co.uk    mailto:lg@ndirect.co.uk
//		who hereby grants you fair use and distribution rights of the code 
//		in both commercial and non-commercial applications.

#include "stdafx.h"
#include "DashLine.h"
#include "bezier.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



// define _TIMING to null out the GDI calls.
//	i.e. you can find out what is the cost of the calculations

//#define _TIMING 1

/////////////////////////////////////////////////////////////////////////////
// helper

/////////////////////////////////////////////////////////////////////////////
// CDashLine

CDashLine::CDashLine(CDC& dc, unsigned* pattern, unsigned count)
	:m_DC(dc), m_Pattern(0)
{
	SetPattern(pattern, count);
}

CDashLine::~CDashLine()
{
	// destroy GDI objects
	delete[] m_Pattern;
}

void CDashLine::SetPattern(unsigned* pattern, unsigned count)
{
	// Must be an even number of dash and gaps
	ASSERT(count >=0);
	ASSERT(!(count % 2));

	m_Count = count;
	delete[] m_Pattern;
	if (m_Count)
	{
		m_Pattern = new UINT[count];
		memcpy(m_Pattern, pattern, count * sizeof(UINT));
	}	
	else
		m_Pattern = 0;
	Reset();
}

void CDashLine::Reset()
{
	m_CurPat = 0;
	if (m_Count)
		m_CurStretch = m_Pattern[0];
}

// use linear interpolation to compute next position
void CDashLine::Bresenham(LONG x, LONG y)
{

	// Setup Bresenham
	LONG dx = x - m_CurPos.x;
	LONG dy = y - m_CurPos.y;

	LONG *p1, *p2, *pd1, *pd2;
	if (abs(dx) >= abs(dy))
	{
		p1 = &m_CurPos.x; 
		p2 = &m_CurPos.y; 
		pd1 = &dx; 
		pd2 = &dy; 
	}
	else
	{
		p1 = &m_CurPos.y; 
		p2 = &m_CurPos.x; 
		pd1 = &dy; 
		pd2 = &dx;
	}

	int max = abs(*pd1);
	int dec = abs(*pd2);
	int s1 = (*pd1 >= 0) ? 1: -1;
	int s2 = (*pd2 >= 0) ? 1: -1;
	int val = max;

	// count past correct number of pixels in current segment
	// 		or until end of this line
	for (int i = 0; i < max; i++)
	{
		val -= dec;
		if (val <= 0) 
		{	
			*p2 += s2; 
			val += max; 
		}
		*p1 += s1;

		--m_CurStretch;
		if (!m_CurStretch)
		{
			// use next pattern for next segment
			m_CurPat = (m_CurPat + 1) % m_Count;
			m_CurStretch = m_Pattern[m_CurPat];

			// draw segment or skip gap
#ifndef _TIMING
			if (m_CurPat % 2)
				m_DC.LineTo(m_CurPos);
			else
				m_DC.MoveTo(m_CurPos);
#endif _TIMING

		
			// if last point, return
			if (i == max - 1) 
				return;
		}
	}

	// draw to last point if necessary
#ifndef _TIMING
	if (!(m_CurPat % 2))
		m_DC.LineTo(m_CurPos);
#endif _TIMING
}

void CDashLine::MoveTo(int x, int y)
{
	// with MoveTo, reset parameters
	Reset();

	// save current position
	m_CurPos.x = x; 
	m_CurPos.y = y;

	// move to position
	m_DC.MoveTo(m_CurPos);
}


void CDashLine::LineTo(int x, int y)
{
	// line type and color
	if (!m_Count)
	{
		m_DC.LineTo(x, y);
		m_CurPos.x = x; m_CurPos.y = y;
		return;
	}
#ifdef _TIMING
	m_DC.LineTo(x, y);
#endif _TIMING

	// calculate and draw next points
	Bresenham(x, y);
}


// use linear interpolation to compute next position
void CDashLine::BezierTo(POINT* dest)
{


	/*	if (m_DC.m_hDC != m_DC.m_hAttribDC)
		{
			CPoint pt;			   
			if( !::GetCurrentPositionEx(m_DC.m_hDC, &pt) )
			{
				

LPVOID lpMsgBuf;
FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
    FORMAT_MESSAGE_FROM_SYSTEM | 
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,
    0,
    NULL 
);
// Process any inserts in lpMsgBuf.
// ...
// Display the string.
MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
// Free the buffer.
LocalFree( lpMsgBuf );

			}
			
		}

			*/



#ifdef _TIMING
	m_DC.PolyBezierTo(dest, 3);	
#endif _TIMING
	if (!m_Count)
	{
		m_DC.PolyBezierTo(dest, 3);
		
		m_CurPos = dest[2];
		return;
	}	


	// Setup Bezier representing curve
	LBezier currentBez;
	currentBez.p[0] = m_CurPos;
	for (int i = 0; i < 3; ++i)
		currentBez.p[i+1] = dest[i];

	LBezier drawBezier;
	CPoint Output[4];

	//for (int ii = 0; ii < 100; ++ii)
	while(1)
	{
		// split off segment corresponding to current stretch, ie drawBezier
		currentBez.TSplit(drawBezier, currentBez.TAtLength(m_CurStretch));
		drawBezier.GetCPoint(Output);

		m_CurPos = Output[3];

#ifndef _TIMING
		if (m_CurPat % 2)
			m_DC.MoveTo(m_CurPos);
		else
			::PolyBezierTo( m_DC.GetSafeHdc(), Output+1, 3);			
			
#endif _TIMING

		// break if some part of segment did not fit on bezier
		if (!m_CurStretch)
		{
			// use next pattern for next segment
			m_CurPat = (m_CurPat + 1) % m_Count;
			m_CurStretch = m_Pattern[m_CurPat];
		}	
		else
			break;		
	}		
}


// Returns count of elements (dash/dot and gaps)
// You must be careful to pass in enough memory for pattern
// It is probably safest to always have an array of [8]
unsigned CDashLine::GetPattern(unsigned* pattern, bool round, unsigned penSize, unsigned style)
{
	ASSERT(style <= DL_DASHDOTDOTDOT_GAP);
	int gapLen	= round	?	penSize * 2 : penSize;
	int dotLen	= round	?	1			: penSize;
	int dashLen	= round	?	penSize * 2 : penSize * 3;
	if (style >= DL_DASH_GAP)
		gapLen *= 2;
	if (style == DL_SOLID)
		return(0);
	
	switch (style)
	{
		case DL_DASH:
		case DL_DASH_GAP:
				pattern[0] = dashLen;
				pattern[1] = gapLen;
				return 2;
		case DL_DOT:
		case DL_DOT_GAP:
				pattern[0] = dotLen;
				pattern[1] = gapLen;
				return 2;
		case DL_DASHDOT:
		case DL_DASHDOT_GAP:
				pattern[0] = dashLen;
				pattern[2] = dotLen;
				pattern[1] = 
				pattern[3] = gapLen;
				return 4;
		case DL_DASHDOTDOT:
		case DL_DASHDOTDOT_GAP:
				pattern[0] = dashLen;
				pattern[2] =
				pattern[4] = dotLen;
				pattern[1] =
				pattern[3] =
				pattern[5] = gapLen;
				return 6;
		case DL_DASHDOTDOTDOT:
		case DL_DASHDOTDOTDOT_GAP:
		default:
				pattern[0] = dashLen;
				pattern[2] =
				pattern[4] = 
				pattern[6] = dotLen;
				pattern[1] =
				pattern[3] =
				pattern[5] = 
				pattern[7] = gapLen;
				return 8;
		
	}	
		
}	

void CDashLine::Rectangle(CRect &rect, COLORREF color, int penWidth, BOOL bRound, unsigned style )
{
	int nDelta = penWidth/2;

	if( ((penWidth/100) % 2) != 0 )
		nDelta -= 50;

	m_DC.BeginPath();
	MoveTo(rect.left+nDelta,  rect.top+nDelta);		//left up
	LineTo(rect.right-nDelta, rect.top+nDelta);		//right up
	LineTo(rect.right-nDelta, rect.bottom-nDelta);	//right down
	LineTo(rect.left+nDelta,  rect.bottom-nDelta);	//left down
	LineTo(rect.left+nDelta,  rect.top+nDelta);		//left up

	if( style == (unsigned)DL_SOLID )
		LineTo(rect.left+penWidth,  rect.top+nDelta);

	m_DC.EndPath();

		
	
	LOGBRUSH lbrush;
	lbrush.lbStyle = BS_SOLID;
	lbrush.lbColor = color;
	CPen Pen(PS_GEOMETRIC | PS_SOLID | 
				(bRound ? 
				(PS_JOIN_ROUND | PS_ENDCAP_ROUND):
				(PS_JOIN_MITER | PS_ENDCAP_FLAT)	)
				, penWidth, &lbrush);
	CPen *pOldPen = m_DC.SelectObject(&Pen);
	m_DC.StrokePath();
	m_DC.SelectObject(pOldPen);
}
