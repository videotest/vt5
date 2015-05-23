#include "stdafx.h"
#include "FillHoles.h"
#include "resource.h"
#include "binary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int BodyMask = 1<<7;

static void ContourFillAreaWithHoles2(CWalkFillInfo &info, byte byteRawBody,
	byte byteExtMark, byte byteFill, byte byteIntMark, byte byteIntMark2, CRect rect, int nSize)
{
	if( rect == NORECT )
		rect = info.GetRect();
	int	x, y;
	byte	bytePrev, byteCur;
	bool	bFill = false;
	for( y = rect.top-1; y < rect.bottom+1; y++ )
	{
		byteCur = 0x00;
		for( x = rect.left-1; x < rect.right+1; x++)
		{
			bytePrev = byteCur;
			byteCur = info.GetPixel( x, y );
			if ((bytePrev == byteExtMark  && byteCur == byteRawBody) ||
				(bytePrev == byteIntMark && byteCur == byteRawBody) )
			{
				ASSERT( !bFill );
				bFill = true;
			}
			if ((bytePrev == byteRawBody && byteCur == byteExtMark) ||
				(byteCur == byteIntMark && bytePrev == byteRawBody))
			{
				ASSERT( bFill );
				bFill = false;
			}
			if (bFill && (bytePrev&BodyMask) == BodyMask && byteCur == 0)
			{ // Hole
				Contour *p = ::ContourWalk(info, x-1, y, 2);
				if( !p )
					continue;
				p->lFlags = cfInternal;
				CRect rc = ::ContourCalcRect(p);
				// AM BT4972.
				// 1. Contour lays on object points.
				// 2. ContourCalcRect expands rectangle on 1 pixel.
				if (rc.Width()-2 > nSize || rc.Height()-2 > nSize)
				{
					::ContourExternalMark(info, p, byteIntMark, byteRawBody);
					bFill = false;
					byteCur = info.GetPixel( x, y );
					ASSERT( byteCur == byteIntMark );
				}
				else
				{
					::ContourExternalMark(info, p, byteIntMark2, byteRawBody);
					byteCur = info.GetPixel(x, y);
					ASSERT(byteCur == byteIntMark2);
				}
				info.AddContour(p);
			}
			if( bFill )
				info.SetPixel(x, y, byteFill);
		}
		ASSERT(!bFill);
	}
	for(int c = 0; c < info.GetContoursCount(); c++)
		::ContourMark( info, info.GetContour( c ), byteFill);
	info.ClearContours();
}


class CWalkFillInfo2 : public CWalkFillInfo
{
public:
	CWalkFillInfo2(const CRect rect) : CWalkFillInfo(rect)
	{
	}
	bool FillObjects(int nSize)
	{
		CRect rectBounds = m_rect;
		CPoint pointStart = rectBounds.TopLeft();
		bool	bFound = false;
		for (int j = pointStart.y; j < rectBounds.bottom; j++)
			for (int i = rectBounds.left; i < rectBounds.right; i++)
				if (GetPixel(i, j) == m_byteRawBody)
				{
					Contour	*p = ::ContourWalk(*this, i, j, 6);
					if( !p )
						continue;
					bFound = true;
					p->lFlags = cfExternal;
					CRect	rect = ::ContourCalcRect(p);
					::ContourExternalMark(*this, p, m_byteExternalMark, m_byteContourMark);
					::ContourFillAreaWithHoles2(*this, m_byteContourMark,
						m_byteExternalMark, m_byteFillBody, m_byteInternalMark,
						0x03, rect, nSize);
					//kill external mark
					::ContourExternalMark(*this, p, 0, 0);
					::ContourDelete(p);
				}
		return bFound;
	}
};

IMPLEMENT_DYNCREATE(CActionFillHolesBin, CCmdTargetEx);

// {C7D26191-23F0-4F73-AC48-E43BE5CCE0F2}
GUARD_IMPLEMENT_OLECREATE(CActionFillHolesBin, "FiltersMain.FillHolesBin",
0xc7d26191, 0x23f0, 0x4f73, 0xac, 0x48, 0xe4, 0x3b, 0xe5, 0xcc, 0xe0, 0xf2);

// {8B905DA9-C1DC-4D65-9703-C087F82C0EF0}
static const GUID guidFillHoles = 
{ 0x8b905da9, 0xc1dc, 0x4d65, { 0x97, 0x3, 0xc0, 0x87, 0xf8, 0x2c, 0xe, 0xf0 } };

ACTION_INFO_FULL(CActionFillHolesBin, IDS_ACTION_FILL_HOLES_BIN, IDS_MENU_FILTERS, IDS_TB_MAIN, guidFillHoles);

ACTION_TARGET(CActionFillHolesBin, szTargetAnydoc);

ACTION_ARG_LIST(CActionFillHolesBin)
	ARG_INT(_T("Size"), 0 )
	ARG_BINIMAGE( _T("BinImage") )
	RES_BINIMAGE( _T("BinResult") )
END_ACTION_ARG_LIST();


//////////////////////////////////////////////////////////////////////
//CActionFillHolesBin implementation
CActionFillHolesBin::CActionFillHolesBin()
{
}

CActionFillHolesBin::~CActionFillHolesBin()
{
}

bool CActionFillHolesBin::InvokeFilter()
{
	int	nSize = GetArgumentInt( "Size" );
	if (nSize == 0) nSize = 1000000;
	CBinImageWrp	imgSrc( GetDataArgument() );
	CBinImageWrp	imgDst( GetDataResult() );
	long cx = imgSrc.GetWidth(), cy = imgSrc.GetHeight();
	if( cy == 0 )
		return false;
	CPoint	ptOffset;
	imgSrc->GetOffset(&ptOffset);
	if( !imgDst.CreateNew(cx, cy) )
		return false;
	imgDst->SetOffset(ptOffset, true);
	CRect rcBound(CPoint(0,0), CSize(cx, cy));
	CWalkFillInfo2* pwfi = new CWalkFillInfo2(rcBound);
	int nRowSize = 0;
	byte* pbin = 0;
	StartNotification(cy);
	for(long y = 0; y < cy; y++ )
	{
		imgSrc->GetRow(&pbin, y, FALSE);
		byte* pmaskdest = pwfi->GetRowMask(y);
		for(long x = 0; x < cx; x++, pmaskdest++)
		{
			if(pbin[x]!= 0)
				*pmaskdest = 255;
			else
				*pmaskdest = 0;
		}
		Notify( y );
	}
	pwfi->FillObjects(nSize);
	for (long y = 0; y < cy; y++)
	{
		byte* pmasksrc = pwfi->GetRowMask(y);
		imgDst->GetRow(&pbin, y, FALSE);
		for(long x = 0; x < cx; x++)
			pbin[x] = pmasksrc[x]&BodyMask?255:0;
	}
	delete pwfi;
	FinishNotification();
	return true;
}

