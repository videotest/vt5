#include "stdafx.h"
#include "actions.h"
#include "resource.h"
#include "math.h"
#include "binary.h"
#include "statusutils.h"
#include "convert_help.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PropPageInt.h"
#include "PropPage.h"

#define MAX_DETAIL		17
#define MAX_INTENSITY	64
#define MAX_EXPOSURE	200


byte	curve_coeff[3][MAX_DETAIL+1] = 
{
	{64,60,56,52,48,44,40,36,32,28,24,20,16,12,8,4,0, 0},
	{0,5,10,15,20,25,29,31,32,31,29,25,20,15,10,5,0, 0},
	{0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64, 64}
};

 byte get_binary_image_color(CBinImageWrp& image)
 {
	 int cx,cy;
	 image->GetSizes( &cx, &cy );
	 byte* row;
	 for(int i=0;i<cy;i++)
	 {
		image->GetRow( &row, i, FALSE );
		for(int j=0;j<cx;j++,row++)
		{
			if(*row) return *row;
		}
	 }
	 return 0xff;
 }
#define PROVIDE_LAB_COLORIMETER()														\
	_convert	cnv;																	\
	cnv.init( image );																	\
	int	color_pane, color_panes_count = cnv.get_panes(), offset;						\
	double	light_coeff = (double)colorMax/MAX_INTENSITY*m_nExposure/MAX_EXPOSURE;

// A.M. 9.09.2002. BT 2692
#define DO_BURN(start, counter)															\
	for( color_pane = 0; color_pane < color_panes_count; color_pane++ )					\
		{																				\
			for( offset = 0; offset < (counter); offset++ )								\
			{																			\
				pInfo->pColors[color_pane+color_panes_count*offset] = 					\
					pColor[color_pane][(start)+offset];									\
				if( !cnv.is_gray_pane( color_pane ) )									\
					continue;															\
				double	light = pColor[color_pane][(start)+offset];						\
				double	flt = light*MAX_DETAIL/colorMax;								\
				int		idx1 = (int)flt;												\
				double	f = curve_coeff[int(m_nDodgeBurnType)][idx1]*(idx1+1-flt)		\
						   +curve_coeff[int(m_nDodgeBurnType)][idx1+1]*(flt-idx1);		\
				pColor[color_pane][(start)+offset]=max(0, min( 65535, 					\
					light-f*light_coeff ) );											\
			}																			\
		}

#define DO_DODGE(start, counter)															\
	for( color_pane = 0; color_pane < color_panes_count; color_pane++ )					\
		{																				\
			for( offset = 0; offset < (counter); offset++ )								\
			{																			\
				pInfo->pColors[color_pane+color_panes_count*offset] = 					\
					pColor[color_pane][(start)+offset];									\
				if( !cnv.is_gray_pane( color_pane ) )									\
					continue;															\
				double	light = pColor[color_pane][(start)+offset];						\
				double	flt = light*MAX_DETAIL/colorMax;								\
				int		idx1 = (int)flt;												\
				double	f = curve_coeff[int(m_nDodgeBurnType)][idx1]*(idx1+1-flt)		\
						   +curve_coeff[int(m_nDodgeBurnType)][idx1+1]*(flt-idx1);		\
				pColor[color_pane][(start)+offset]=max(0, min( 65535, 					\
					light+f*light_coeff ) );											\
			}																			\
		}



CRect GetTargetImageRect(IImageViewPtr sptrV)
{
	CRect rc = NORECT;
	if(sptrV != 0)
	{
		IUnknown* punkImage = 0;
		sptrV->GetActiveImage(&punkImage, 0);
		CImageWrp image(punkImage);
		if(punkImage)
			punkImage->Release();

		rc = image.GetRect();
	}
	return rc;
}


static bool m_bEnabled = true;
class CLinePreviewCtrl: public CDrawControllerBase
{
public:
	CLinePreviewCtrl(int nSize)
	{
		m_nSize = nSize;
		m_fZoom = 0;
		m_pPreviewPoints.RemoveAll();
		//m_bEnabled = true;
	}
	~CLinePreviewCtrl()
	{
		_EraseArray();
	}

	void Enable(bool bEnable)
	{
		if(m_bEnabled == bEnable)return;
		m_bEnabled = true;
		Redraw(bEnable);
		m_bEnabled = bEnable;
	}
	void DoDraw( CDC &dc, CPoint point, IUnknown *punkVS,  bool bErase )
	{
		if(!m_bEnabled)return;
		CPoint ptScroll = GetScrollPos(punkVS);
		double fZoom = 0;
		sptrIScrollZoomSite	sptrS( punkVS );
		if (sptrS != 0)
				sptrS->GetZoom(&fZoom);	
		CPoint pt1, pt2;
		double fMagicOffset = (m_nSize%2 == 1) ? .5 : 0;
		if(fZoom < 2)
		{
			m_fZoom = fZoom;
			if(m_nSize > 2)
			{
				CRect rc(point.x - (long)(m_nSize/2.0 + fMagicOffset), point.y - (long)(m_nSize/2.0 + fMagicOffset), point.x + (long)(m_nSize/2.0 + fMagicOffset), point.y + (long)(m_nSize/2.0 + fMagicOffset));
				rc = ::ConvertToWindow(punkVS, rc);
				// A.M. fix SBT1776. In some computers CDC::Ellipse works bad
				// with ellipses 2x2 (videoboard? driver? Windows version?).
				if (rc.Width() >= 3 && rc.Height() >= 3)
				dc.Ellipse(&rc);
			}
			return;
		}

		if(m_fZoom != fZoom)
		{
			m_fZoom = fZoom;
			_InitCashe(punkVS);
		}
		int i = 0;
		while(i < m_pPreviewPoints.GetSize())
		{
			pt1 = pt2 = CPoint((long)((point.x - m_nSize/2.0 - fMagicOffset)*m_fZoom) - ptScroll.x, (long)((point.y - m_nSize/2.0 - fMagicOffset)*m_fZoom) - ptScroll.y);
			pt1.Offset(*m_pPreviewPoints[i]);
			i++;
			pt2.Offset(*m_pPreviewPoints[i]);
			i++;
			dc.MoveTo(pt1);
			dc.LineTo(pt2);
		}

	}

private:
	
	void _EraseArray()
	{
		for(int i = 0; i < m_pPreviewPoints.GetSize(); i++)
		{
			delete m_pPreviewPoints[i];
		}
		m_pPreviewPoints.RemoveAll();
	}

	void _InitCashe(IUnknown *punkVS)
	{
		_EraseArray();
		const int BodyMask = 1<<7;
		CRect rcBounds(0, 0, m_nSize+1, m_nSize+1);
		CPoint pt = rcBounds.CenterPoint();
		rcBounds.InflateRect(3,3,3,3);
		CWalkFillInfo* pwfi = new CWalkFillInfo(rcBounds);
		CDC	*pDC = pwfi->GetDC();


		CPen	pen;
		LOGBRUSH	lb;
		lb.lbColor = RGB( 255, 255, 255 );
		lb.lbStyle = BS_SOLID;
		lb.lbHatch = 0;

		pen.Attach( ::ExtCreatePen( PS_GEOMETRIC|PS_SOLID, m_nSize, &lb, 0, 0 ) );

		//CPen pen(PS_SOLID, m_nSize, RGB(255, 255, 255));
		CPen* pOldPen = pDC->SelectObject(&pen);
		CBrush brush(RGB(255, 255, 255));
		CBrush* pOldBrush = pDC->SelectObject(&brush);

//		pDC->BeginPath();
		if( m_nSize == 1 )
			pDC->SetPixel( pt, RGB( 255, 255, 255 ) );
		else if( m_nSize == 2 )
		{
			pDC->FillRect( CRect( pt.x-1, pt.y-1, pt.x+1, pt.y+1), &brush );
		}
		else
		{
			pDC->MoveTo(pt);
			pDC->LineTo(pt);
		}
//		pDC->SelectStockObject( WHITE_BRUSH );
//		pDC->StrokePath();
//		pDC->EndPath();
		

		byte pixel = 0;
		CPoint* ppoint = 0;
		for(int i = rcBounds.left; i < rcBounds.right; i++)
		{
			for(int j = rcBounds.top; j < rcBounds.bottom; j++)
			{
				pixel = pwfi->GetPixel(i, j);
				if(pixel & BodyMask)
				{
					if((i == 0) || (!(pwfi->GetPixel(i-1, j)&BodyMask)))
					{
						//left side line
						ppoint = new CPoint(long((i+0.0)*m_fZoom+.5), long((j+0.0)*m_fZoom+.5));
						m_pPreviewPoints.Add(ppoint);
						ppoint = new CPoint(long((i+0.0)*m_fZoom+.5), long((j+1.0)*m_fZoom+.5));
						m_pPreviewPoints.Add(ppoint);
						ppoint = 0;
					}
					if((j == 0)|| (!(pwfi->GetPixel(i, j-1)&BodyMask)))
					{
						//top side line
						ppoint = new CPoint(long((i+0.0)*m_fZoom+.5), long((j+0.0)*m_fZoom+.5));
						m_pPreviewPoints.Add(ppoint);
						ppoint = new CPoint(long((i+1.0)*m_fZoom+.5), long((j+0.0)*m_fZoom+.5));
						m_pPreviewPoints.Add(ppoint);
						ppoint = 0;
					}
					if((i == rcBounds.Height()-1) || (!(pwfi->GetPixel(i+1, j)&BodyMask)))
					{
						//right side line
						ppoint = new CPoint(long((i+1.0)*m_fZoom+.5), long((j+0.0)*m_fZoom+.5));
						m_pPreviewPoints.Add(ppoint);
						ppoint = new CPoint(long((i+1.0)*m_fZoom+.5), long((j+1.0)*m_fZoom+.5));
						m_pPreviewPoints.Add(ppoint);
						ppoint = 0;
					}
					if((j == rcBounds.Width()-1)|| (!(pwfi->GetPixel(i, j+1)&BodyMask)))
					{
						//bottom side line
						ppoint = new CPoint(long((i+0.0)*m_fZoom+.5), long((j+1.0)*m_fZoom+.5));
						m_pPreviewPoints.Add(ppoint);
						ppoint = new CPoint(long((i+1.0)*m_fZoom+.5), long((j+1.0)*m_fZoom+.5));
						m_pPreviewPoints.Add(ppoint);
						ppoint = 0;
					}
				}
			}
		}
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		delete pwfi;
	}
protected:
	CTypedPtrArray<CPtrArray, CPoint*> m_pPreviewPoints;
	int		m_nSize;
	double	m_fZoom;
};

class CBinLinePreviewCtrl: public CLinePreviewCtrl
{
protected:
	int m_nSize2;
public:
	CBinLinePreviewCtrl(int nSize, int nSize2) : CLinePreviewCtrl(nSize)
	{
		m_nSize2 = nSize2;
	}
	void DoDraw( CDC &dc, CPoint point, IUnknown *punkVS,  bool bErase )
	{
		CRect	rect;

		rect.left = point.x-m_nSize/2;
		rect.top = point.y-m_nSize/2;
		rect.right = rect.left + m_nSize;
		rect.bottom = rect.top + m_nSize;

		rect = ::ConvertToWindow( punkVS, rect );
		// A.M. fix SBT1776. In some computers CDC::Ellipse works bad
		// with ellipses 2x2 (videoboard? driver? Windows version?).
		if (rect.Width() >= 3 && rect.Height() >= 3)
			dc.Ellipse( rect );

		if( m_nSize2 != m_nSize )
		{
			rect.left = point.x-m_nSize2/2;
			rect.top = point.y-m_nSize2/2;
			rect.right = rect.left + m_nSize2;
			rect.bottom = rect.top + m_nSize2;

			rect = ::ConvertToWindow( punkVS, rect );
			if (rect.Width() >= 3 && rect.Height() >= 3)
				dc.Ellipse( rect );
		}
	}
};


//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionPolylineBin, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionPolyline, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionBinEditSplainLine, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionBinEditFreeLine, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionBinEditLine, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionBurn, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDodge, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEditFloodFill, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEditPickColor, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEditAutoBackground, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEditRect, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEditCircle, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEditSplainLine, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEditFreeLine, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEditLine, CCmdTargetEx);

// olecreate 

// {ABC19B87-7F5F-49e1-B9B9-60282F2C70A7}
GUARD_IMPLEMENT_OLECREATE(CActionBinEditSplainLine, "Editor.BinEditSplainLine",
0xabc19b87, 0x7f5f, 0x49e1, 0xb9, 0xb9, 0x60, 0x28, 0x2f, 0x2c, 0x70, 0xa7);
// {02050DB8-74DE-429e-9BCF-F705D92F1C62}
GUARD_IMPLEMENT_OLECREATE(CActionBinEditFreeLine, "Editor.BinEditFreeLine",
0x2050db8, 0x74de, 0x429e, 0x9b, 0xcf, 0xf7, 0x5, 0xd9, 0x2f, 0x1c, 0x62);
// {FC528A91-8A1D-4adf-BB62-A3B9388EA1B4}
GUARD_IMPLEMENT_OLECREATE(CActionBinEditLine, "Editor.BinEditLine",
0xfc528a91, 0x8a1d, 0x4adf, 0xbb, 0x62, 0xa3, 0xb9, 0x38, 0x8e, 0xa1, 0xb4);
// {1F6711C3-070C-11d4-A0C7-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionBurn, "editor.Burn",
0x1f6711c3, 0x70c, 0x11d4, 0xa0, 0xc7, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {F130C1F3-0544-11d4-A0C7-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionDodge, "editor.Dodge",
0xf130c1f3, 0x544, 0x11d4, 0xa0, 0xc7, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {B6E6CD13-9141-11d3-A52A-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionEditFloodFill, "Editor.EditFloodFill",
0xb6e6cd13, 0x9141, 0x11d3, 0xa5, 0x2a, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {99255E13-8869-11d3-A51D-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionEditPickColor, "Editor.EditPickColor",
0x99255e13, 0x8869, 0x11d3, 0xa5, 0x1d, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {4E93EB73-87C9-11d3-A51B-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionEditAutoBackground, "Editor.EditAutoBackground",
0x4e93eb73, 0x87c9, 0x11d3, 0xa5, 0x1b, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {641BDED3-8605-11d3-A518-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionEditRect, "Editor.EditRect",
0x641bded3, 0x8605, 0x11d3, 0xa5, 0x18, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {D75D5863-813B-11d3-A514-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionEditCircle, "Editor.EditCircle",
0xd75d5863, 0x813b, 0x11d3, 0xa5, 0x14, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {D75D585F-813B-11d3-A514-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionEditSplainLine, "Editor.EditSplineLine",
0xd75d585f, 0x813b, 0x11d3, 0xa5, 0x14, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {D75D585B-813B-11d3-A514-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionEditFreeLine, "Editor.EditFreeLine",
0xd75d585b, 0x813b, 0x11d3, 0xa5, 0x14, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {D75D5857-813B-11d3-A514-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionEditLine, "Editor.EditLine",
0xd75d5857, 0x813b, 0x11d3, 0xa5, 0x14, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {B497034B-ABFD-4fd3-9E1A-7661BC3D3527}
GUARD_IMPLEMENT_OLECREATE(CActionPolylineBin, "Editor.PolylineBin",
0xb497034b, 0xabfd, 0x4fd3, 0x9e, 0x1a, 0x76, 0x61, 0xbc, 0x3d, 0x35, 0x27);
// {E21B2902-F1F1-43d3-A3D7-4904003B24A6}
GUARD_IMPLEMENT_OLECREATE(CActionPolyline, "Editor.Polyline",
0xe21b2902, 0xf1f1, 0x43d3, 0xa3, 0xd7, 0x49, 0x4, 0x0, 0x3b, 0x24, 0xa6);

// guidinfo 

// {0504A150-28D4-439c-9428-EB18E19D5D80}
static const GUID guidBinEditSplainLine =
{ 0x504a150, 0x28d4, 0x439c, { 0x94, 0x28, 0xeb, 0x18, 0xe1, 0x9d, 0x5d, 0x80 } };
// {FD983664-6908-45ed-BE0E-00696003C5E0}
static const GUID guidBinEditFreeLine =
{ 0xfd983664, 0x6908, 0x45ed, { 0xbe, 0xe, 0x0, 0x69, 0x60, 0x3, 0xc5, 0xe0 } };
// {0CC0C418-158F-4f25-A165-DE5B3F8BD8B5}
static const GUID guidBinEditLine =
{ 0xcc0c418, 0x158f, 0x4f25, { 0xa1, 0x65, 0xde, 0x5b, 0x3f, 0x8b, 0xd8, 0xb5 } };
// {1F6711C1-070C-11d4-A0C7-0000B493A187}
static const GUID guidBurn =
{ 0x1f6711c1, 0x70c, 0x11d4, { 0xa0, 0xc7, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {F130C1F1-0544-11d4-A0C7-0000B493A187}
static const GUID guidDodge =
{ 0xf130c1f1, 0x544, 0x11d4, { 0xa0, 0xc7, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {B6E6CD11-9141-11d3-A52A-0000B493A187}
static const GUID guidEditFloodFill =
{ 0xb6e6cd11, 0x9141, 0x11d3, { 0xa5, 0x2a, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {99255E11-8869-11d3-A51D-0000B493A187}
static const GUID guidEditPickColor =
{ 0x99255e11, 0x8869, 0x11d3, { 0xa5, 0x1d, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {4E93EB71-87C9-11d3-A51B-0000B493A187}
static const GUID guidEditAutoBackground =
{ 0x4e93eb71, 0x87c9, 0x11d3, { 0xa5, 0x1b, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {641BDED1-8605-11d3-A518-0000B493A187}
static const GUID guidEditRect =
{ 0x641bded1, 0x8605, 0x11d3, { 0xa5, 0x18, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {D75D5861-813B-11d3-A514-0000B493A187}
static const GUID guidEditCircle =
{ 0xd75d5861, 0x813b, 0x11d3, { 0xa5, 0x14, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {D75D585D-813B-11d3-A514-0000B493A187}
static const GUID guidEditSplineLine =
{ 0xd75d585d, 0x813b, 0x11d3, { 0xa5, 0x14, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {D75D5859-813B-11d3-A514-0000B493A187}
static const GUID guidEditFreeLine =
{ 0xd75d5859, 0x813b, 0x11d3, { 0xa5, 0x14, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {D75D5855-813B-11d3-A514-0000B493A187}
static const GUID guidEditLine =
{ 0xd75d5855, 0x813b, 0x11d3, { 0xa5, 0x14, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {C82FAE43-C9D9-44a0-9DA2-CC529CD68C8B}
static const GUID guidPolylineBin =
{ 0xc82fae43, 0xc9d9, 0x44a0, { 0x9d, 0xa2, 0xcc, 0x52, 0x9c, 0xd6, 0x8c, 0x8b } };
// {EA9F0683-D1E5-4b37-8233-12CDCF591A59}
static const GUID guidPolyline =
{ 0xea9f0683, 0xd1e5, 0x4b37, { 0x82, 0x33, 0x12, 0xcd, 0xcf, 0x59, 0x1a, 0x59 } };
//[ag]6. action info

ACTION_INFO_FULL(CActionPolylineBin, ID_ACTION_BIN_POLYLINE, -1, -1, guidPolylineBin);
ACTION_INFO_FULL(CActionPolyline, ID_ACTION_POLYLINE, -1, -1, guidPolyline);
ACTION_INFO_FULL(CActionBinEditSplainLine, ID_ACTION_BIN_EDIT_SPLINELINE, -1, -1, guidBinEditSplainLine);
ACTION_INFO_FULL(CActionBinEditFreeLine, ID_ACTION_BIN_EDIT_FREELINE, -1, -1, guidBinEditFreeLine);
ACTION_INFO_FULL(CActionBinEditLine, ID_ACTION_BIN_EDIT_LINE, -1, -1, guidBinEditLine);
ACTION_INFO_FULL(CActionBurn, ID_ACTION_BURN, -1, -1, guidBurn);
ACTION_INFO_FULL(CActionDodge, ID_ACTION_DODGE, -1, -1, guidDodge);
ACTION_INFO_FULL(CActionEditFloodFill, ID_ACTION_EDIT_FLOODFILL, -1, -1, guidEditFloodFill);
ACTION_INFO_FULL(CActionEditPickColor, ID_ACTION_EDIT_PICKCOLOR, -1, -1, guidEditPickColor);
ACTION_INFO_FULL(CActionEditAutoBackground, ID_ACTION_EDIT_AUTOBACK, -1, -1, guidEditAutoBackground);
ACTION_INFO_FULL(CActionEditRect, ID_ACTION_EDIT_RECT, -1, -1, guidEditRect);
ACTION_INFO_FULL(CActionEditCircle, ID_ACTION_EDIT_CIRCLE, -1, -1, guidEditCircle);
ACTION_INFO_FULL(CActionEditSplainLine, ID_ACTION_EDIT_SPLINELINE, -1, -1, guidEditSplineLine);
ACTION_INFO_FULL(CActionEditFreeLine, ID_ACTION_EDIT_FREELINE, -1, -1, guidEditFreeLine);
ACTION_INFO_FULL(CActionEditLine, ID_ACTION_EDIT_LINE, -1, -1, guidEditLine);
//[ag]7. targets

ACTION_TARGET(CActionPolylineBin, szTargetViewSite);
ACTION_TARGET(CActionPolyline, szTargetViewSite);
ACTION_TARGET(CActionBinEditSplainLine, szTargetViewSite);
ACTION_TARGET(CActionBinEditFreeLine, szTargetViewSite);
ACTION_TARGET(CActionBinEditLine, szTargetViewSite);
ACTION_TARGET(CActionBurn, szTargetViewSite);
ACTION_TARGET(CActionDodge, szTargetViewSite);
ACTION_TARGET(CActionEditFloodFill, szTargetViewSite);
ACTION_TARGET(CActionEditPickColor, szTargetViewSite);
ACTION_TARGET(CActionEditAutoBackground, szTargetViewSite);
ACTION_TARGET(CActionEditRect, szTargetViewSite);
ACTION_TARGET(CActionEditCircle, szTargetViewSite);
ACTION_TARGET(CActionEditSplainLine, szTargetViewSite);
ACTION_TARGET(CActionEditFreeLine, szTargetViewSite);
ACTION_TARGET(CActionEditLine, szTargetViewSite);
//[ag]8. arguments

ACTION_ARG_LIST(CActionEditLine)
	ARG_INT(_T("X1"), 0)
	ARG_INT(_T("Y1"), 0)
	ARG_INT(_T("X2"), 0)
	ARG_INT(_T("Y2"), 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionBinEditLine)
	ARG_INT(_T("X1"), 0)
	ARG_INT(_T("Y1"), 0)
	ARG_INT(_T("X2"), 0)
	ARG_INT(_T("Y2"), 0)
END_ACTION_ARG_LIST()


ACTION_ARG_LIST(CActionEditRect)
	ARG_INT(_T("Left"), 0)
	ARG_INT(_T("Top"), 0)
	ARG_INT(_T("Right"), 0)
	ARG_INT(_T("Bottom"), 0)
END_ACTION_ARG_LIST()


ACTION_ARG_LIST(CActionEditCircle)
	ARG_INT(_T("X"), 0)
	ARG_INT(_T("Y"), 0)
	ARG_INT(_T("D"), 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionEditPickColor)
	ARG_INT(_T("X"), -1)
	ARG_INT(_T("Y"), -1)
	ARG_INT(_T("SetBack"), -1)
	ARG_INT(_T("SaveResult"), 1)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionEditFloodFill)
	ARG_INT(_T("SetBack"), -1)
END_ACTION_ARG_LIST()


//[ag]9. implementation



//////////////////////////////////////////////////////////////////////
//CActionBurn implementation
CActionBurn::CActionBurn()
{
	m_strActionName = "Burn";
}

bool CActionBurn::DoRedo()
{
	CRect rcTmp = m_rectInvalid;
	m_rectInvalid.InflateRect(m_nLineWidth, m_nLineWidth, m_nLineWidth, m_nLineWidth);
	
	color** pColor = 0;

	_try(CActionBurn, DoRedo)
	{
		if(m_rectInvalid == NORECT)
			return false;
		CImageWrp image(m_punkImage);
		int nPanes = image.GetColorsCount();
		int i, j;
		FillInfo* pInfo = 0;
		pColor = new color*[nPanes];
		m_rectInvalid.NormalizeRect();

		IUnknown *punkCC = 0;
		image->GetColorConvertor( &punkCC );
		sptrIColorConvertor3 sptrCC(punkCC);
		if(punkCC)
			punkCC->Release();
		if(sptrCC == 0)
			return false;

		PROVIDE_LAB_COLORIMETER();
		color	**ppcolorBuf = new color*[nPanes];
		
		for(i = max(m_rectInvalid.top, 0); i < min(m_ptrFillInfo.GetSize(), m_rectInvalid.bottom); i++ )
		{
			pInfo = m_ptrFillInfo.GetAt(i);
			if( !pInfo )continue;
			if (pInfo->nCount != 0)
			{
				for(j = 0; j < nPanes; j++)
				{
					pColor[j] = image.GetRow(i, j);	//image row
				}
				while(pInfo)
				{
				DO_BURN(pInfo->nX, pInfo->nCount);

					pInfo = pInfo->pNext;
				}
				
			}
		}

		if(ppcolorBuf)
			delete ppcolorBuf;	

	}
	_catch
	{
	}

	if(pColor)
		delete pColor;
		
	m_rectInvalid.OffsetRect(m_imgOffset);	
	
	//paul 21.06.2003 BT 3272. Why only 1 view, need event fire on document
	//::FireEvent(m_punkTarget, szNeedUpdateView, m_punkTarget, GetControllingUnknown(), 0);
	::UpdateDataObject( m_punkTarget, m_punkImage );

	m_rectInvalid = rcTmp;

	return true;

}

void CActionBurn::Dodge(CRect rcDraw)
{
	//m_nExposure = 100;
	ASSERT(m_pwfi != 0);
	const int BodyMask = 1<<7;

	color** pColor = 0;
	if(rcDraw == NORECT)
		return;
	//save new data in m_ptrFillInfo
	CImageWrp image(m_punkImage);
	int nPanes = image.GetColorsCount();

	int nImageWidth = image.GetWidth();			

	IUnknown *punkCC = 0;
	image->GetColorConvertor( &punkCC );
	sptrIColorConvertor3 sptrCC(punkCC);
	if(punkCC)
		punkCC->Release();
	if(sptrCC == 0)
		return;

	bool bIsSeq = false;
	int i, j, k;
	FillInfo* pInfo = 0;
	FillInfo* pInfoOrig = 0;
	pColor = new color*[nPanes];
	rcDraw.NormalizeRect();
	CRect rect = m_pwfi->GetRect();

	//get brightness of base colors
	////////////
	PROVIDE_LAB_COLORIMETER();
	////////////
	
	
	for(j = max(rcDraw.top, rect.top); j <= min(rcDraw.bottom, rect.bottom-1); j++)
	{	
		for(k = 0; k < nPanes; k++)
		{
			pColor[k] = image.GetRow(j, k);	//image row
		}
		byte	*pbyteMask = image.GetRowMask( j );
		pInfo = 0;
		bIsSeq = false;
		byte pixel;
		bool bProcessSeq = false;
		for(i = max(rcDraw.left, rect.left); i <= min(rcDraw.right, rect.right-1); i++)
		{
			pixel = m_pwfi->GetPixel(i, j );
			bool bAlreadyPresent = m_pbMask[j*nImageWidth + i] == TRUE;

			if((pixel & BodyMask) && (pbyteMask[i] != 0) && (!bAlreadyPresent))
			{
				m_pbMask[j*nImageWidth + i] = TRUE;

				if(!bIsSeq)
				{
					pInfoOrig = m_ptrFillInfo.GetAt(j);
					if(pInfoOrig->nCount == 0)
					{
						pInfo = pInfoOrig;
					}
					else
					{
						pInfo = FillInfo::Create();
						pInfo->pNext = pInfoOrig;
						pInfo->pColors = 0;
						m_ptrFillInfo.SetAt(j, pInfo);
					}

					pInfo->nX = i;
					pInfo->nCount = 1;
						
					bIsSeq = true;
					bProcessSeq = false;
				}
				else
				{
					pInfo->nCount += 1;
				}
			}
			else
			{
				bProcessSeq = true;
			}
			if(bProcessSeq || i == min(rcDraw.right, rect.right-1))
			{
				if(bIsSeq)
				{		
					pInfo->CreateColors(nPanes*pInfo->nCount);

					DO_BURN(pInfo->nX, pInfo->nCount);
				}
				bIsSeq = false;
			}
		}

		ASSERT(!bIsSeq);

	}

	if(pColor)delete pColor;
}

//////////////////////////////////////////////////////////////////////
//CActionDodgeBase implementation

CActionDodgeBase::CActionDodgeBase()
{
	m_bDrawMode = false;
	m_nLineWidth = 1;
	m_bInitialized = false;
	m_bInteractive = false;
	m_bCanTrackByRButton = true;
	m_bUseBackColor = false;
	m_bTracking = false;
	m_nExposure = 100;
	//m_bSmooth = false;
	m_nDodgeBurnType = 0;

	m_pbMask = 0;
}

CActionDodgeBase::~CActionDodgeBase()
{
}

bool CActionDodgeBase::DoTrack( CPoint pt )
{
	m_rect.right = pt.x;
	m_rect.bottom = pt.y;
	_DoDraw();
	return true;
}

bool CActionDodgeBase::DoLButtonDown( CPoint pt )
{
	if(CActionEditorBase::DoLButtonDown(pt))return true;

	m_bUseBackColor = false;
	m_bDrawMode = true;
	m_rect.left = m_rect.right = pt.x;
	m_rect.top = m_rect.bottom = pt.y;
	if(_Begin(false))
	{
		m_bInteractive = true;

		CImageWrp image(m_punkImage);
		int nImageWidth = image.GetWidth();			
		int nImageHeight = image.GetHeight();		

		m_pbMask = new BOOL[nImageWidth*nImageHeight];
		ZeroMemory(m_pbMask, sizeof(BOOL)*nImageWidth*nImageHeight);

		_DoDraw();
	}
	return false;
}

bool CActionDodgeBase::DoLButtonUp( CPoint pt )
{
	if(!m_bDrawMode)return false;
	_End();
	if(m_pbMask)
	{
		delete m_pbMask;
		m_pbMask = 0;
	}
	Finalize();
	return false;
}

bool CActionDodgeBase::DoStartTracking( CPoint pt )
{
	m_bTracking = true;
	return true;
}


bool CActionDodgeBase::DoFinishTracking( CPoint pt )
{
	m_bTracking = false;
	return true;
}

void CActionDodgeBase::OnFinalize()
{
	m_bDrawMode = false;
}

bool CActionDodgeBase::DoSetCursor( CPoint point )
{
	if(!CActionEditorBase::DoSetCursor(point))
	{
		m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDITFREE);
	}
	if( m_state == asActive && m_hcurActive )
	{
		::SetCursor( m_hcurActive );
		return true;
	}
	return CMouseImpl::DoSetCursor( point );
}


bool CActionDodgeBase::Initialize()
{ 
	AddPropertyPageDescription( c_szCDodge1PropPage );
	if( !CActionEditorBase::Initialize() )return false;
	return true;
}

bool CActionDodgeBase::DoUpdateSettings()
{
	CActionEditorBase::DoUpdateSettings();
	m_nExposure = ::GetValueInt(GetAppUnknown(), "Editor", "Exposure", 100);
	m_nDodgeBurnType = ::GetValueInt(GetAppUnknown(), "Editor", "DodgeBurnType", 0);
	//m_bSmooth = ::GetValueInt(GetAppUnknown(), "Editor", "Smooth", 0) != 0;

	return true;
}


void CActionDodgeBase::_DoDraw()
{
	{
		CRect rc(m_rect);
		rc.OffsetRect(-m_imgOffset);
		if(m_punkImage == 0)
			return;
		CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
		CDC	*pDC = m_pwfi->GetDC();
		CPen pen(PS_SOLID, m_nLineWidth, RGB(255, 255, 255));
		CPen* pOldPen = pDC->SelectObject(&pen);
		pDC->SelectStockObject(WHITE_BRUSH);
	
		pDC->MoveTo(rc.left, rc.top);
		if((!m_bTracking) && (m_nLineWidth==1 || m_nLineWidth==2))
			pDC->LineTo(rc.right+1, rc.bottom+1);
		else
			pDC->LineTo(rc.right, rc.bottom);

		pDC->SelectObject(pOldPen);
		rc.NormalizeRect();

		rc.InflateRect(m_nLineWidth, m_nLineWidth, m_nLineWidth, m_nLineWidth);
		
		rc.left = max(rc.left, 0);
		rc.top = max(rc.top, 0);

		if(rc.left < m_rectInvalid.left)
			m_rectInvalid.left = rc.left;
		if(rc.top < m_rectInvalid.top)
			m_rectInvalid.top = rc.top;
		if(rc.right > m_rectInvalid.right)
			m_rectInvalid.right = rc.right;
		if(rc.bottom > m_rectInvalid.bottom)
			m_rectInvalid.bottom = rc.bottom;

		
		Dodge(rc);//m_pwfi->AttachFillInfoToImage(&m_ptrFillInfo, m_punkImage, rc, m_pColors, 0);
		m_pwfi->ClearMasks();

		rc.OffsetRect(m_imgOffset);

		rc = ConvertToWindow( m_punkTarget, rc );
		pwnd->InvalidateRect(rc);

		pwnd->UpdateWindow();

		m_rect.left = m_rect.right;
		m_rect.top = m_rect.bottom;
	}
}

void CActionDodgeBase::OnBegin()
{
	m_bInitialized = true;
}

//////////////////////////////////////////////////////////////////////
//CActionDodge implementation




CActionDodge::CActionDodge()
{
	m_strActionName = "Dodge";
}

bool CActionDodge::DoRedo()
{
	CRect rcTmp = m_rectInvalid;
	m_rectInvalid.InflateRect(m_nLineWidth, m_nLineWidth, m_nLineWidth, m_nLineWidth);
	
	color** pColor = 0;

	_try(CActionBurn, DoRedo)
	{
		if(m_rectInvalid == NORECT)
			return false;
		CImageWrp image(m_punkImage);
		int nPanes = image.GetColorsCount();
		int i, j;
		FillInfo* pInfo = 0;
		pColor = new color*[nPanes];
		m_rectInvalid.NormalizeRect();

		IUnknown *punkCC = 0;
		image->GetColorConvertor( &punkCC );
		sptrIColorConvertor3 sptrCC(punkCC);
		if(punkCC)
			punkCC->Release();
		if(sptrCC == 0)
			return false;

		PROVIDE_LAB_COLORIMETER();
		color	**ppcolorBuf = new color*[nPanes];
		
		for(i = max(m_rectInvalid.top, 0); i < min(m_ptrFillInfo.GetSize(), m_rectInvalid.bottom); i++ )
		{
			pInfo = m_ptrFillInfo.GetAt(i);
			if( !pInfo )continue;
			if (pInfo->nCount != 0)
			{
				for(j = 0; j < nPanes; j++)
				{
					pColor[j] = image.GetRow(i, j);	//image row
				}
				while(pInfo)
				{
					DO_DODGE(pInfo->nX, pInfo->nCount);
					pInfo = pInfo->pNext;
				}
				
			}
		}
		if(ppcolorBuf)
			delete ppcolorBuf;	

	}
	_catch
	{
	}

	if(pColor)
		delete pColor;
		
	m_rectInvalid.OffsetRect(m_imgOffset);

	//paul 21.06.2003
	//::FireEvent(m_punkTarget, szNeedUpdateView, m_punkTarget, GetControllingUnknown(), 0);
	::UpdateDataObject( m_punkTarget, m_punkImage );
	

	m_rectInvalid = rcTmp;

	return true;

}

void CActionDodge::Dodge(CRect rcDraw)
{
	//m_nExposure = 100;
	ASSERT(m_pwfi != 0);
	const int BodyMask = 1<<7;

	color** pColor = 0;
	if(rcDraw == NORECT)
		return;
	//save new data in m_ptrFillInfo
	CImageWrp image(m_punkImage);
	int nPanes = image.GetColorsCount();

	int nImageWidth = image.GetWidth();			

	IUnknown *punkCC = 0;
	image->GetColorConvertor( &punkCC );
	sptrIColorConvertor3 sptrCC(punkCC);
	if(punkCC)
		punkCC->Release();
	if(sptrCC == 0)
		return;

	bool bIsSeq = false;
	int i, j, k;
	FillInfo* pInfo = 0;
	FillInfo* pInfoOrig = 0;
	pColor = new color*[nPanes];
	rcDraw.NormalizeRect();
	CRect rect = m_pwfi->GetRect();

	color	**ppcolorBuf = new color*[nPanes];

	//get brightness of base colors
	PROVIDE_LAB_COLORIMETER()

	for(j = max(rcDraw.top, rect.top); j <= min(rcDraw.bottom, rect.bottom-1); j++)
	{	
		for(k = 0; k < nPanes; k++)
		{
			pColor[k] = image.GetRow(j, k);	//image row
		}
		byte	*pbyteMask = image.GetRowMask( j );
		pInfo = 0;
		bIsSeq = false;
		byte pixel;
		bool bProcessSeq = false;
		for(i = max(rcDraw.left, rect.left); i <= min(rcDraw.right, rect.right-1); i++)
		{
			pixel = m_pwfi->GetPixel(i, j );
			bool bAlreadyPresent = m_pbMask[j*nImageWidth + i] == TRUE;

			if((pixel & BodyMask) && (pbyteMask[i] != 0) && (!bAlreadyPresent))
			{
				m_pbMask[j*nImageWidth + i] = TRUE;

				if(!bIsSeq)
				{
					pInfoOrig = m_ptrFillInfo.GetAt(j);
					if(pInfoOrig->nCount == 0)
					{
						pInfo = pInfoOrig;
					}
					else
					{
						pInfo = FillInfo::Create();
						pInfo->pNext = pInfoOrig;
						pInfo->pColors = 0;
						m_ptrFillInfo.SetAt(j, pInfo);
					}

					pInfo->nX = i;
					pInfo->nCount = 1;
						
					bIsSeq = true;
					bProcessSeq = false;
				}
				else
				{
					pInfo->nCount += 1;
				}
			}
			else
			{
				bProcessSeq = true;
			}
			if(bProcessSeq || i == min(rcDraw.right, rect.right-1))
			{
				if(bIsSeq)
				{		
					//pInfo->pColors = new color[nPanes*pInfo->nCount];
					pInfo->CreateColors(nPanes*pInfo->nCount);

					DO_DODGE(pInfo->nX, pInfo->nCount);
					
				}
				bIsSeq = false;
			}
		}

		ASSERT(!bIsSeq);

	}

	if(pColor)delete pColor;
	if( ppcolorBuf )delete ppcolorBuf;
}

//////////////////////////////////////////////////////////////////////
// common utils for CActionEditPickColor and CActionEditFloodFill

static IUnknownPtr GetImagePtr(IUnknown *punkTarget)
{
	sptrIDataContext sptrDC(punkTarget);
	if(sptrDC == 0)
		return 0;

	IUnknownPtr ptrImage;
	{
		IUnknown* punkImage = 0;
		sptrDC->GetActiveObject( _bstr_t(szArgumentTypeImage), &punkImage );
		if( !punkImage )
			return 0;

		ptrImage = punkImage;

		punkImage->Release();	punkImage = 0;
		
	}

	{
		IUnknown* punkParent = 0;
		INamedDataObject2Ptr ptrChild( ptrImage );
		if( ptrChild )
		{
			ptrChild->GetParent( &punkParent );
			if( punkParent )
			{
				ptrImage = punkParent;
				punkParent->Release();	punkParent = 0;
			}
		}
		
	}


	return ptrImage;
}


static bool GetColor( CPoint pt, COLORREF& dwRGB, color& H, color& L, color& S,
					  IUnknown *punkTarget)
{
	int nX, nY;
	nX = pt.x;
	nY = pt.y;

	IUnknownPtr ptrImage = GetImagePtr(punkTarget);
	if( ptrImage == 0 )
		return false;

	/*
	sptrIDataContext sptrDC(m_punkTarget);
	if(sptrDC == 0)
		return false;
	IUnknown* punkImage = 0;
	sptrDC->GetActiveObject( _bstr_t(szArgumentTypeImage), &punkImage );
	if( !punkImage )
		return false;
		*/
	
	CImageWrp image( ptrImage );
	//punkImage->Release();
	//punkImage = NULL;

	if( image == NULL )
		return false;

	CSize imgOffset = image.GetOffset();
	int	nColorsCount = image.GetColorsCount();
	IUnknown *punkCC = NULL;
	image->GetColorConvertor( &punkCC );
	if( punkCC == NULL )
		return false;

	sptrIColorConvertor sptrCC(punkCC);
	punkCC->Release();

	if(sptrCC == 0)
		return false;

	CRect	rc = image.GetRect();
	if(rc.PtInRect(CPoint( nX, nY)) == FALSE) 
	{		
		return false;
	}

	nX -= imgOffset.cx;
	nY -= imgOffset.cy;

	color	**ppcolorBuf = new color*[nColorsCount];

		
	for( int nColor = 0; nColor < nColorsCount; nColor++ )
	{
		ppcolorBuf[nColor] = image.GetRow( nY, nColor );
		ppcolorBuf[nColor] = ppcolorBuf[nColor] + nX;
	}

	long R, G, B;
	color c0,c1,c2;

	if(image.IsColor())
	{
		byte pRGB[3];
		sptrCC->ConvertImageToDIB( ppcolorBuf, pRGB, 1, TRUE ); 		
		B = pRGB[0];
		G = pRGB[1];
		R = pRGB[2];


		c0 = *ppcolorBuf[0];
		c1 = *ppcolorBuf[1];
		c2 = *ppcolorBuf[2];
	}
	else
	{
		byte pGray[1];
		sptrCC->ConvertImageToDIB( ppcolorBuf, pGray, 1, FALSE ); 
		R = G = B = pGray[0];

		c2 = c1 = c0 = *ppcolorBuf[0];
	}

	dwRGB = RGB( R, G, B );


	if( ppcolorBuf )delete ppcolorBuf;

	//HSB
	{
		
		IColorConvertor4Ptr ptrCCRGB = GetCCByName( "RGB" );		
		IColorConvertor4Ptr ptrCCHSB = GetCCByName( "HSB" );

		if( ptrCCRGB == NULL || ptrCCHSB == NULL )
			return false;

		byte	dib[3];
		dib[0] = (byte)B;
		dib[1] = (byte)G;
		dib[2] = (byte)R;

		
		double pLab[3];		

		color	colorsRGB[3];
		colorsRGB[0] = c0;//(byte)B;
		colorsRGB[1] = c1;//(byte)G;
		colorsRGB[2] = c2;//(byte)R;


		color	*ppcolorRGB[3];
		for( int i = 0; i < 3; i++ )
		{			
			ppcolorRGB[i] = &colorsRGB[i];
		}

		IColorConvertor3Ptr ptrCC3( sptrCC );		
		if( ptrCC3 )
		{
			/*ptrCCRGB*/ptrCC3->ConvertImageToLAB( ppcolorRGB, pLab, 1 );
			ptrCCHSB->ConvertLABToImage( pLab, ppcolorRGB, 1 );
		}

		IHumanColorConvertorPtr ptrHC( ptrCCHSB );
		if( ptrHC )
		{
			ptrHC->Convert( colorsRGB[0], 0, &H );
			ptrHC->Convert( colorsRGB[1], 1, &S );
			ptrHC->Convert( colorsRGB[2], 2, &L );
		}
		/*

		H = ( *ppcolorRGB[0] - 65535 + 65535/3);
		H = -( (double)H * 360.0 /65535 - 360);

		S = ( (double)*ppcolorRGB[1] * 360.0 /65535 );
		L = *ppcolorRGB[2];		
		*/

	}


	
	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionEditFloodFill implementation
CActionEditFloodFill::CActionEditFloodFill()
{
	m_strActionName = "FloodFill";
}

CActionEditFloodFill::~CActionEditFloodFill()
{
}

bool CActionEditFloodFill::DoUpdateSettings()
{
	return true;
}

bool CActionEditFloodFill::Initialize()
{
	AddPropertyPageDescription( c_szCFloodFillPropPage );
	if( !IsCompatibleTarget())
		if(!SetAppropriateView(m_punkTarget, m_bWorkWithBinary ? avtBinary : avtSource) || m_state == asTerminate )
			return false;


	if( !CInteractiveActionBase::Initialize() )return false;
	/*CViewBase* view = 0;
	view = (CViewBase*)GetWindowFromUnknown(m_punkTarget);
	if(view)
		view->m_dataSource.ForceEnabled(false);*/
	m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_FLOODFILL);
	::SetCursor( m_hcurActive );


	LockMovement(true, GetTargetImageRect(m_punkTarget));
	
	int nBack = GetArgumentInt( _T("SetBack") );
	if(nBack >= 0)
	{
		m_bInteractive = false;
		m_bUseBackColor = nBack == 1;
		if(_Begin())
		{
			_DoDraw();
			Finalize();
			return false;
		}
	}
	return true;
}

bool CActionEditFloodFill::DoLButtonDown( CPoint pt )
{
	m_bUseBackColor = false;
	if((short)GetKeyState(VK_CONTROL) < 0 && !m_bWorkWithBinary)
	{
		CString	strExecuteParams;
		strExecuteParams.Format( "%d, %d, %d", pt.x, pt.y, 0 );
		ExecuteAction(_T("PickColor"), strExecuteParams, 0);
		if(!m_strActionName.IsEmpty())
			ExecuteAction(m_strActionName, "", 0);
		UpdatePropertyPages();
		return true;
	}
	else 
		CActionEditorBase::DoButtonDown(pt);
	if(_Begin())
	{
		_DoDraw();
		m_bInteractive = true;
		Finalize();
	}
	return true;
}

bool CActionEditFloodFill::DoRButtonDown( CPoint pt )
{
	m_bUseBackColor = true;
	if((short)GetKeyState(VK_CONTROL) < 0 && !m_bWorkWithBinary)
	{
		CString	strExecuteParams;
		strExecuteParams.Format( "%d, %d, %d", pt.x, pt.y, 1 );
		ExecuteAction(_T("PickColor"), strExecuteParams, 0);
		if(!m_strActionName.IsEmpty())
			ExecuteAction(m_strActionName, "", 0);
		UpdatePropertyPages();
		return true;
	}
	else 
		CActionEditorBase::DoButtonDown(pt);
//	if(CActionEditorBase::DoLButtonDown(pt))return true;
//	m_bUseBackColor = true;
	m_bInteractive = true;
	if(_Begin())
	{
		_DoDraw();
		Finalize();
	}
	return true;
}

void CActionEditFloodFill::_End()
{
	SetArgument( _T("SetBack"), _variant_t((long)(m_bUseBackColor?1:0)) );
}

bool CActionEditFloodFill::DoSetCursor( CPoint point )
{
	if(!CActionEditorBase::DoSetCursor(point))
	{
		m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_FLOODFILL);
	}
	if( m_state == asActive && m_hcurActive )
	{
		::SetCursor( m_hcurActive );
		return true;
	}
	return CMouseImpl::DoSetCursor( point );
}

//undo/redo
/*bool CActionEditFloodFill::DoUndo()
{
	return true;
}

bool CActionEditFloodFill::DoRedo()
{
	return true;
}*/

void CActionEditFloodFill::_DoDraw()
{
	if(!m_bWorkWithBinary || m_bBinaryIsActive)
	{
		if( !m_pwfi )
			return;
		//rc.OffsetRect(-m_imgOffset);
		CDC	*pDC = m_pwfi->GetDC();
	
		m_rectInvalid = m_pwfi->GetRect();
		m_rectInvalid.InflateRect(0,0,1,1);
		pDC->FillRect(m_rectInvalid, &CBrush(RGB(255,255,255)));

		RestoreImageFromChanges(m_rectInvalid, false);

		MoveFillInfoToImage(m_rectInvalid, m_pColors, m_pFillColors);
	
	}
}

bool CActionEditFloodFill::DoMouseMove( CPoint pt )
{
	COLORREF colorRGB = CLR_NONE;
	color H, L, S;
	H = L = S = 0;

	//Get color
	bool bResult = GetColor( pt, colorRGB, H, L, S, m_punkTarget );

	//Update page
	IUnknown* punkPage = NULL;
	int nPageNum = -1;

	if( GetPropertyPageByProgID( szEditorFloodFillPropPageProgID, &punkPage, &nPageNum ))
	{
		IColorPickerPropPagePtr ptrPage(punkPage);
		punkPage->Release();

		if( ptrPage != NULL )
		{
			ptrPage->ChangeColor( (bResult == true ? colorRGB : CLR_NONE), H, S, L );
		}
	}

	return false;
}


//////////////////////////////////////////////////////////////////////
//CActionEditPickColor implementation

CActionEditPickColor::CActionEditPickColor()
{
	m_bByLButton = true;
	m_nX = -1;
	m_nY = -1;
	m_bCanTrackByRButton = true;
	m_nPaneNum = 0;
}

CActionEditPickColor::~CActionEditPickColor()
{
}

bool CActionEditPickColor::IsCompatibleTarget()
{
	DWORD dwFlags = 0;
	IImageViewPtr sptrIV(m_punkTarget);
	if(sptrIV != 0)sptrIV->GetImageShowFlags(&dwFlags);
	if (::GetValueInt(::GetAppUnknown(), "Editor", "PickColorWorksOnBinary", FALSE))
		return (dwFlags&(isfSourceImage|isfBinaryFore|isfBinaryBack|isfBinary))!=0;
	else
		return (dwFlags & isfSourceImage) == isfSourceImage;
}


bool CActionEditPickColor::ExecuteSettings( CWnd *pwndParent )
{
	m_nX = GetArgumentInt( _T("X") );
	m_nY = GetArgumentInt( _T("Y") );
	m_bByLButton = GetArgumentInt( _T("SetBack") ) == 0;
	return true;
}

void CActionEditPickColor::_SetPaneValue(CPoint pt)
{
	CString strText = "";
	CString	strTextToShellData,strTextToShellData8;

	sptrIDataContext sptrDC(m_punkTarget);
	if(sptrDC != 0)
	{
		IUnknownPtr ptrImage = GetImagePtr(m_punkTarget);
		if( ptrImage )
		{
			

		/*
		IUnknown* punkImage = 0;
		sptrDC->GetActiveObject( _bstr_t(szArgumentTypeImage), &punkImage );
		
		if(punkImage)
		{*/
			CImageWrp image( ptrImage );
			CSize imgOffset = image.GetOffset();
			int	nColorsCount = image.GetColorsCount();
		
			CRect	rc = image.GetRect();
			if(rc.PtInRect(pt) != FALSE) 
			{
				pt.x -= imgOffset.cx;
				pt.y -= imgOffset.cy;

				strText = "(";
				strTextToShellData = "(";
				for( int nColor = 0; nColor < nColorsCount; nColor++ )
				{
					color* pRow =  image.GetRow( pt.y, nColor );
					CString strColorVal;

					strColorVal.Format("%d", ColorAsByte(*(pRow + pt.x)));
					strText += strColorVal;

					strColorVal.Format("%d", *(pRow + pt.x));
					strTextToShellData += strColorVal;

					if(nColor < nColorsCount-1)
					{
						strTextToShellData += ", ";
						strText += ", ";
					}
				}
				strText += ")";
				strTextToShellData += ")";
			}

			::SetValue( GetAppUnknown(), "\\PickedColor", "Value", strTextToShellData );
			::SetValue( GetAppUnknown(), "\\PickedColor", "Value8", strText );
			//punkImage->Release();
		}
	}

	::StatusSetPaneText( guidPaneAction, strText );
}


bool CActionEditPickColor::DoRButtonDown( CPoint pt )
{
	m_bByLButton = false;
	_SetPaneValue(pt);
	return true;
}

bool CActionEditPickColor::DoLButtonDown( CPoint pt )
{
	m_bByLButton =  true;
	_SetPaneValue(pt);
	return false;
}

bool CActionEditPickColor::DoRButtonUp( CPoint pt )
{
	if(!m_bByLButton)
	{
		m_nX = pt.x;
		m_nY = pt.y;
		Finalize();
	}
	return true;
}

bool CActionEditPickColor::DoLButtonUp( CPoint pt )
{
	if(m_bByLButton)
	{
		m_nX = pt.x;
		m_nY = pt.y;
		Finalize();
	}
	return false;
}

bool CActionEditPickColor::DoTrack( CPoint pt )
{
	_SetPaneValue(pt);
	return false;
}

bool CActionEditPickColor::DoMouseMove( CPoint pt )
{
	COLORREF colorRGB = CLR_NONE;
	color H, L, S;
	H = L = S = 0;

	//Get color
	bool bResult = GetColor( pt, colorRGB, H, L, S, m_punkTarget );

	//Update page
	IUnknown* punkPage = NULL;
	int nPageNum = -1;

	if( GetPropertyPageByProgID( szEditorColorPickerPropPageProgID, &punkPage, &nPageNum ))
	{
		IColorPickerPropPagePtr ptrPage(punkPage);
		punkPage->Release();

		if( ptrPage != NULL )
		{
			ptrPage->ChangeColor( (bResult == true ? colorRGB : CLR_NONE), H, S, L );
		}
	}

	return false;
}

#if 0
bool CActionEditPickColor::GetColor( CPoint pt, COLORREF& dwRGB, color& H, color& L, color& S )
{
	int nX, nY;
	nX = pt.x;
	nY = pt.y;

	IUnknownPtr ptrImage = GetImagePtr();
	if( ptrImage == 0 )
		return false;

	/*
	sptrIDataContext sptrDC(m_punkTarget);
	if(sptrDC == 0)
		return false;
	IUnknown* punkImage = 0;
	sptrDC->GetActiveObject( _bstr_t(szArgumentTypeImage), &punkImage );
	if( !punkImage )
		return false;
		*/
	
	CImageWrp image( ptrImage );
	//punkImage->Release();
	//punkImage = NULL;

	if( image == NULL )
		return false;

	CSize imgOffset = image.GetOffset();
	int	nColorsCount = image.GetColorsCount();
	IUnknown *punkCC = NULL;
	image->GetColorConvertor( &punkCC );
	if( punkCC == NULL )
		return false;

	sptrIColorConvertor sptrCC(punkCC);
	punkCC->Release();

	if(sptrCC == 0)
		return false;

	CRect	rc = image.GetRect();
	if(rc.PtInRect(CPoint( nX, nY)) == FALSE) 
	{		
		return false;
	}

	nX -= imgOffset.cx;
	nY -= imgOffset.cy;

	color	**ppcolorBuf = new color*[nColorsCount];

		
	for( int nColor = 0; nColor < nColorsCount; nColor++ )
	{
		ppcolorBuf[nColor] = image.GetRow( nY, nColor );
		ppcolorBuf[nColor] = ppcolorBuf[nColor] + nX;
	}

	long R, G, B;
	color c0,c1,c2;

	if(image.IsColor())
	{
		byte pRGB[3];
		sptrCC->ConvertImageToDIB( ppcolorBuf, pRGB, 1, TRUE ); 		
		B = pRGB[0];
		G = pRGB[1];
		R = pRGB[2];


		c0 = *ppcolorBuf[0];
		c1 = *ppcolorBuf[1];
		c2 = *ppcolorBuf[2];
	}
	else
	{
		byte pGray[1];
		sptrCC->ConvertImageToDIB( ppcolorBuf, pGray, 1, FALSE ); 
		R = G = B = pGray[0];

		c2 = c1 = c0 = *ppcolorBuf[0];
	}

	dwRGB = RGB( R, G, B );


	if( ppcolorBuf )delete ppcolorBuf;

	//HSB
	{
		
		IColorConvertor4Ptr ptrCCRGB = GetCCByName( "RGB" );		
		IColorConvertor4Ptr ptrCCHSB = GetCCByName( "HSB" );

		if( ptrCCRGB == NULL || ptrCCHSB == NULL )
			return false;

		byte	dib[3];
		dib[0] = (byte)B;
		dib[1] = (byte)G;
		dib[2] = (byte)R;

		
		double pLab[3];		

		color	colorsRGB[3];
		colorsRGB[0] = c0;//(byte)B;
		colorsRGB[1] = c1;//(byte)G;
		colorsRGB[2] = c2;//(byte)R;


		color	*ppcolorRGB[3];
		for( int i = 0; i < 3; i++ )
		{			
			ppcolorRGB[i] = &colorsRGB[i];
		}

		IColorConvertor3Ptr ptrCC3( sptrCC );		
		if( ptrCC3 )
		{
			/*ptrCCRGB*/ptrCC3->ConvertImageToLAB( ppcolorRGB, pLab, 1 );
			ptrCCHSB->ConvertLABToImage( pLab, ppcolorRGB, 1 );
		}

		IHumanColorConvertorPtr ptrHC( ptrCCHSB );
		if( ptrHC )
		{
			ptrHC->Convert( colorsRGB[0], 0, &H );
			ptrHC->Convert( colorsRGB[1], 1, &S );
			ptrHC->Convert( colorsRGB[2], 2, &L );
		}
		/*

		H = ( *ppcolorRGB[0] - 65535 + 65535/3);
		H = -( (double)H * 360.0 /65535 - 360);
		
		S = ( (double)*ppcolorRGB[1] * 360.0 /65535 );
		L = *ppcolorRGB[2];		
		*/

	}


	
	return true;
}

IUnknownPtr CActionEditPickColor::GetImagePtr()
{
	sptrIDataContext sptrDC(m_punkTarget);
	if(sptrDC == 0)
		return 0;

	IUnknownPtr ptrImage;
	{
		IUnknown* punkImage = 0;
		sptrDC->GetActiveObject( _bstr_t(szArgumentTypeImage), &punkImage );
		if( !punkImage )
			return 0;

		ptrImage = punkImage;

		punkImage->Release();	punkImage = 0;
		
	}

	{
		IUnknown* punkParent = 0;
		INamedDataObject2Ptr ptrChild( ptrImage );
		if( ptrChild )
		{
			ptrChild->GetParent( &punkParent );
			if( punkParent )
			{
				ptrImage = punkParent;
				punkParent->Release();	punkParent = 0;
			}
		}
		
	}


	return ptrImage;
}
#endif

bool CActionEditPickColor::Invoke()
{
	_try(CActionEditPickColor, Invoke)
	{
		if (GetArgumentInt(_T("SaveResult"))==0)
			return true;

		if(m_nX < 0 || m_nY < 0) return false;
		sptrIDataContext sptrDC(m_punkTarget);
		if(sptrDC == 0)
			return false;

		IUnknownPtr ptrImage = GetImagePtr(m_punkTarget);
		if( ptrImage == 0 )
			return false;
		
		CImageWrp image( ptrImage );
		if( image == 0 )
			return false;

		CSize imgOffset = image.GetOffset();
		int	nColorsCount = image.GetColorsCount();
		IUnknown *punkCC;
		image->GetColorConvertor( &punkCC );
		sptrIColorConvertor6 sptrCC(punkCC);
		punkCC->Release();
		if(sptrCC == 0)
			return false;

		CRect	rc = image.GetRect();
		if(rc.PtInRect(CPoint(m_nX, m_nY)) == FALSE) 
			return false;

		m_nX -= imgOffset.cx;
		m_nY -= imgOffset.cy;

		color	**ppcolorBuf = new color*[nColorsCount];

			
		for( int nColor = 0; nColor < nColorsCount; nColor++ )
		{
			ppcolorBuf[nColor] = image.GetRow( m_nY, nColor );
			ppcolorBuf[nColor] = ppcolorBuf[nColor] + m_nX;
		}
		int  nEditMode = image.GetEditMode();
		long R, G, B;
//		if(image.IsColor())
		if (nEditMode == 1)
		{
			byte pRGB[3];
			sptrCC->ConvertImageToDIB2( ppcolorBuf, pRGB, 1, TRUE, INT_MAX, image, m_punkTarget ); 
			B = pRGB[0];
			G = pRGB[1];
			R = pRGB[2];

			::SetValueColor(GetAppUnknown(), "Editor", m_bByLButton?"Fore":"Back", RGB(R,G,B));
		}
		else
		{
			byte pGray[1];
			sptrCC->ConvertImageToDIB2( ppcolorBuf, pGray, 1, FALSE, INT_MAX, image, m_punkTarget ); 
			R = G = B = pGray[0];

			::SetValueColor(GetAppUnknown(), "Editor", m_bByLButton?"Fore gray":"Back gray", RGB(R,G,B));
		}

		if( ppcolorBuf )delete ppcolorBuf;

		//IUnknown* punkChooser = _GetOtherComponent(GetAppUnknown(), IID_IChooseColor);
		//sptrIChooseColor	sptrChooser(punkChooser);
		//if(punkChooser)
		//	punkChooser->Release();
		IUnknown* punkChooser = ::GetColorChooser();
		sptrIChooseColor	sptrChooser(punkChooser);
		if(punkChooser)
			punkChooser->Release();
		if(sptrChooser != 0)
		{
			if(m_bByLButton)
				sptrChooser->SetLButtonColor((OLE_COLOR)RGB(R,G,B));
			else
				sptrChooser->SetRButtonColor((OLE_COLOR)RGB(R,G,B));
		}

		UpdatePropertyPages();


		//????SetArgument( _T("Color"), _variant_t( (long) ) );
		SetArgument( _T("X"), _variant_t( (long)m_nX ) );
		SetArgument( _T("Y"), _variant_t( (long)m_nY ) );

		LeaveMode();
		return true;
	}
	_catch;
	{
		return false;
	}
}

bool CActionEditPickColor::Initialize()
{	
	TRACE("CActionEditPickColor::Initialize\n");
	AddPropertyPageDescription( c_szCColorPickerPropPage );
	if( !IsCompatibleTarget())
		if(!SetAppropriateView(m_punkTarget, avtSource) || m_state == asTerminate )
			return false;

  if(m_nX >= 0 && m_nY >= 0)
	{
		Finalize();
		return false;
	}

	if( !CInteractiveActionBase::Initialize() )return false;

	LockMovement(true, GetTargetImageRect(m_punkTarget));

	IUnknown* punkPage = NULL;
	int nPageNum = -1;

	if( GetPropertyPageByProgID( szEditorColorPickerPropPageProgID, &punkPage, &nPageNum ))
	{
		IColorPickerPropPagePtr ptrPage(punkPage);
		punkPage->Release();

		if( ptrPage != NULL )
		{
			ptrPage->ChangeColor( CLR_NONE, 0, 0, 0 );
		}
	}


	m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_PICKCOLOR);
	

	return true;
}

void CActionEditPickColor::Finalize()
{
	TRACE("CActionEditPickColor::Finalize\n");
	CInteractiveActionBase::Finalize();
}


//extended UI
bool CActionEditPickColor::IsAvaible()
{
	bool	bAvaible = false;
	if (CheckInterface( m_punkTarget, IID_IImageView ))
	{
		IUnknown* punk_img = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );
		if( punk_img )
		{
			punk_img->Release();	punk_img = 0;
			bAvaible = true;
		}
	}
	return bAvaible;
}

bool CActionEditPickColor::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionEditAutoBackground implementation
CActionEditAutoBackground::CActionEditAutoBackground()
{
}

CActionEditAutoBackground::~CActionEditAutoBackground()
{
}

bool CActionEditAutoBackground::Invoke()
{
	_try(CActionEditAutoBackground,Invoke)
	{
		sptrIDataContext sptrDC(m_punkTarget);
		if(sptrDC == 0)
			return false;
		IUnknown* punkImage;
		sptrDC->GetActiveObject( _bstr_t(szArgumentTypeImage), &punkImage );
		if( !punkImage )
			return false;
		
		CImageWrp image(punkImage);
		int	nColorsCount = image.GetColorsCount();
		int nRows = image.GetHeight();
		int nCols = image.GetWidth();
		IUnknown *punkCC;
		image->GetColorConvertor( &punkCC );
		sptrIColorConvertor sptrCC(punkCC);
		punkCC->Release();
		if(sptrCC == 0)
			return false;

		color*	maxColors;
		maxColors = new color[nColorsCount];// = {0,0,0,0};
		ZeroMemory(maxColors, nColorsCount);
		long*	maxColorCount;
		maxColorCount = new long[nColorsCount];// = {0,0,0,0};
		ZeroMemory(maxColorCount, nColorsCount);
		
		color	curColor;
		color	**ppcolorBuf = new color*[nColorsCount];

		long**	numColors;
		numColors = new long*[nColorsCount];//[65536];
		for( int i = 0; i < nColorsCount; i++)
		{
			numColors[i] = new long[65536];
			ZeroMemory(numColors[i], 65536*sizeof(long));
		}
		for( int nRow = 0; nRow < nRows; nRow++ )
		{
			for( int nColor = 0; nColor < nColorsCount; nColor++ )
			{
				ppcolorBuf[nColor] = image.GetRow( nRows-nRow- 1, nColor );
				int idx = 0;
				for( int nCol = 0; nCol < nCols; nCol++ )
				{
					curColor = ppcolorBuf[nColor][idx++];

					numColors[nColor][curColor] += 1;
					if(numColors[nColor][curColor] > maxColorCount[nColor])
					{
						maxColorCount[nColor] = numColors[nColor][curColor];
						maxColors[nColor] = curColor;
					}
				}
			}
		}

		for(i = 0; i < nColorsCount; i++)
		{
			delete numColors[i];
		}
		
		color** ppColors = new color*[nColorsCount];
		long R, G, B;
		for (i = 0; i < nColorsCount; i++)
		{
			ppColors[i] = new color[1];
			ppColors[i][0] = maxColors[i];
		}
		if(image.IsColor())
		{
			byte pRGB[3];
			sptrCC->ConvertImageToDIB( ppColors, pRGB, 1, TRUE ); 
			B = (long)pRGB[0];
			G = (long)pRGB[1];
			R = (long)pRGB[2];
		}
		else
		{
			byte pGray[1];
			sptrCC->ConvertImageToDIB( ppColors, pGray, 1, FALSE ); 
			R = G = B = (long)pGray[0];
		}

		for (i = 0; i < nColorsCount; i++)
		{
			delete ppColors[i];
		}
		delete ppColors;
		delete maxColorCount;
		delete maxColors;

		if( ppcolorBuf )delete ppcolorBuf;
		
		punkImage->Release();

		/*_variant_t var;
		var = R;
		::SetValue(GetAppUnknown(), "Editor", "R_back", var);
		var = G;
		::SetValue(GetAppUnknown(), "Editor", "G_back", var);
		var = B;
		::SetValue(GetAppUnknown(), "Editor", "B_back", var);
		*/
		::SetValueColor(GetAppUnknown(), "Editor", "Back", RGB(R,G,B));

		IUnknown* punkChooser = ::GetColorChooser();
		sptrIChooseColor	sptrChooser(punkChooser);
		if(punkChooser)
			punkChooser->Release();
		if(sptrChooser != 0)
		{
			sptrChooser->SetRButtonColor((OLE_COLOR)RGB(R,G,B));
		}

		return true;
	}
	_catch;
	{
		return false;
	}
}

//extended UI
bool CActionEditAutoBackground::IsAvaible()
{
	bool	bAvaible;
	bAvaible = CheckInterface( m_punkTarget, IID_IImageView );;
	return bAvaible;
}

bool CActionEditAutoBackground::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionEditRect implementation
CActionEditRect::CActionEditRect()
{
	m_strActionName = "EditRect";
	m_bInteractive = false;
	m_nLineWidth = 1;
	m_bCanTrackByRButton = true;
}

CActionEditRect::~CActionEditRect()
{
}

bool CActionEditRect::DoUpdateSettings()
{
	m_nLineWidth = ::GetValueInt(GetAppUnknown(), "Editor", "RectLineWidth", 1);
	return true;
}

void CActionEditRect::OnBegin()
{
	m_bNeedToFill = ::GetValueInt(GetAppUnknown(), "Editor", "RectNeedToFill", 0) == 1;
	m_bFillByDrawColor = ::GetValueInt(GetAppUnknown(), "Editor", "RectFillByDrawColor", 0) == 1;
}

void CActionEditRect::_End()
{
	SetArgument( _T("Left"), _variant_t( m_rect.left ) );
	SetArgument( _T("Top"), _variant_t( m_rect.top ) );
	SetArgument( _T("Right"), _variant_t( m_rect.right ) );
	SetArgument( _T("Bottom"), _variant_t( m_rect.bottom ) );
}

void CActionEditRect::_DoDraw()
{
	//draw in WalkFillInfo dc
	if(!m_bWorkWithBinary || m_bBinaryIsActive)
	{
		CDC	*pDC = m_pwfi->GetDC();
		CPen pen(PS_SOLID, m_nLineWidth, RGB(255, 255, 255));
		CPen* pOldPen = pDC->SelectObject(&pen);
		byte fillByte = m_pwfi->GetFillBody();
		CBrush brush(RGB(fillByte, fillByte, fillByte));
		CBrush* pOldBrush;
		if(m_bNeedToFill)
			pOldBrush = pDC->SelectObject(&brush);
		else
			pOldBrush = (CBrush*)pDC->SelectStockObject(BLACK_BRUSH);
		
		if((short)GetKeyState(VK_SHIFT) < 0)
		{
			//draw square
			CRect rcTmp(m_rect);
			rcTmp.NormalizeRect();
			if(rcTmp.Height() > rcTmp.Width())
			{
				m_rect.bottom = m_rect.top + ((m_rect.bottom > m_rect.top)? rcTmp.Width() : -rcTmp.Width());
			}
			else if(rcTmp.Height() < rcTmp.Width())
			{
				m_rect.right = m_rect.left + ((m_rect.right > m_rect.left)? rcTmp.Height() : -rcTmp.Height());
			}
		}

		CRect rc(m_rect);
		rc.OffsetRect(-m_imgOffset);

		pDC->Rectangle(&rc);

		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);

		rc.NormalizeRect();

		rc.InflateRect(m_nLineWidth, m_nLineWidth, m_nLineWidth, m_nLineWidth);
	
		RestoreImageFromChanges(m_rectInvalid);

		MoveFillInfoToImage(rc, m_pColors, m_bFillByDrawColor?m_pColors:m_pFillColors);

		m_rectInvalid = m_rect;
		m_rectInvalid.NormalizeRect();
		m_rectInvalid.OffsetRect(-m_imgOffset);
	}

}

bool CActionEditRect::DoButtonDown( CPoint pt )
{
	if(_Begin())
	{
		m_bInteractive = true;
		m_rect.left = pt.x;
		m_rect.right = pt.x;
		m_rect.top = pt.y;
		m_rect.bottom = pt.y;
		m_rectInvalid = m_rect;
		_DoDraw();
	}
	return true;
}

/*bool CActionEditRect::DoStartTracking( CPoint pt )
{
	if(_Begin())
	{
		m_bInteractive = true;
		m_rect.left = pt.x;
		m_rect.right = pt.x;
		m_rect.top = pt.y;
		m_rect.bottom = pt.y;
		m_rectInvalid = m_rect;
		_DoDraw();
	}
	return true;
}*/

bool CActionEditRect::DoTrack( CPoint pt )
{
	m_rect.right = pt.x;
	m_rect.bottom = pt.y;
	_DoDraw();
	return true;
}

bool CActionEditRect::DoFinishTracking( CPoint pt )
{
	_End();
	Finalize();
	return true;
}

bool CActionEditRect::DoSetCursor( CPoint point )
{
	if(!CActionEditorBase::DoSetCursor(point))
	{
		m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDITRECT);
	}
	if( m_state == asActive && m_hcurActive )
	{
		::SetCursor( m_hcurActive );
		return true;
	}
	return CMouseImpl::DoSetCursor( point );
}


bool CActionEditRect::Initialize()
{
	AddPropertyPageDescription( c_szCRect1PropPage );

	if( !IsCompatibleTarget())
		if(!SetAppropriateView(m_punkTarget, avtSource) || m_state == asTerminate )
			return false;

	UpdatePropertyPages();

	if( !CInteractiveActionBase::Initialize() )return false;
	m_rect.left = GetArgumentInt( _T("Left") );
	m_rect.top = GetArgumentInt( _T("Top") );
	m_rect.right = GetArgumentInt( _T("Right") );
	m_rect.bottom = GetArgumentInt( _T("Bottom") );
	if( m_rect != NORECT )
	{
		Finalize();
		return false;
	}

	LockMovement(true, GetTargetImageRect(m_punkTarget));

	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionEditCircle implementation
CActionEditCircle::CActionEditCircle()
{
	m_strActionName = "EditCircle";
	m_bInteractive = false;
	m_nLineWidth = 1;
	m_bCanTrackByRButton = true;
}

CActionEditCircle::~CActionEditCircle()
{
}

bool CActionEditCircle::DoUpdateSettings()
{
	m_nLineWidth = ::GetValueInt(GetAppUnknown(), "Editor", "CircleLineWidth", 1);
	return true;
}


void CActionEditCircle::OnBegin()
{
	m_bNeedToFill = ::GetValueInt(GetAppUnknown(), "Editor", "CircleNeedToFill", 0) == 1;
	m_bFillByDrawColor = ::GetValueInt(GetAppUnknown(), "Editor", "CircleFillByDrawColor", 0) == 1;
}



void CActionEditCircle::_End()
{
	SetArgument( _T("X"), _variant_t( m_rect.left ) );
	SetArgument( _T("Y"), _variant_t( m_rect.top ));
	SetArgument( _T("D"), _variant_t( (long)m_rect.Width() ) );
}

void CActionEditCircle::_DoDraw()
{
	//draw in WalkFillInfo dc
	if(!m_bWorkWithBinary || m_bBinaryIsActive)
	{
		CRect rcTmp(m_rect);
		rcTmp.OffsetRect(-m_imgOffset);
		CDC	*pDC = m_pwfi->GetDC();
		CPen pen(PS_SOLID, m_nLineWidth, RGB(255, 255, 255));
		CPen* pOldPen = pDC->SelectObject(&pen);
		byte fillByte = m_pwfi->GetFillBody();
		CBrush brush(RGB(fillByte, fillByte, fillByte));
		CBrush* pOldBrush;
		if(m_bNeedToFill)
			pOldBrush = pDC->SelectObject(&brush);
		else
			pOldBrush = (CBrush*)pDC->SelectStockObject(BLACK_BRUSH);
	
		//if((short)GetKeyState(VK_SHIFT) < 0)
		//{
			//draw circle
			/*CRect rcTmp(m_rect);
			rcTmp.NormalizeRect();
			if(rcTmp.Height() > rcTmp.Width())
			{
				m_rect.bottom = m_rect.top + ((m_rect.bottom > m_rect.top)? rcTmp.Width() : -rcTmp.Width());
			}
			else if(rcTmp.Height() < rcTmp.Width())
			{
				m_rect.right = m_rect.left + ((m_rect.right > m_rect.left)? rcTmp.Height() : -rcTmp.Height());
			}*/
			rcTmp.NormalizeRect();
			int nRadius = sqrt(pow((double)rcTmp.Height(),2) + pow((double)rcTmp.Width(),2))/2;
			CPoint ptCenter = rcTmp.CenterPoint();
			rcTmp.left = ptCenter.x - nRadius;
			rcTmp.top = ptCenter.y - nRadius; 
			rcTmp.right = ptCenter.x + nRadius;
			rcTmp.bottom = ptCenter.y + nRadius; 
		//}

		//pDC->Ellipse(&m_rect);
		pDC->Ellipse(&rcTmp);

		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);

		CRect rc(rcTmp);
		rc.InflateRect(m_nLineWidth, m_nLineWidth, m_nLineWidth, m_nLineWidth);
		
		RestoreImageFromChanges(m_rectInvalid);

		MoveFillInfoToImage(rc, m_pColors, m_bFillByDrawColor?m_pColors:m_pFillColors);
		
		m_rectInvalid = rcTmp;
		m_rectInvalid.NormalizeRect();
		//m_rectInvalid.OffsetRect(m_imgOffset);
	
	}
	
}

bool CActionEditCircle::DoButtonDown( CPoint pt )
{
	if(_Begin())
	{
		m_bInteractive = true;
		m_rect.left = pt.x;
		m_rect.right = pt.x;
		m_rect.top = pt.y;
		m_rect.bottom = pt.y;
		m_rectInvalid = m_rect;
		_DoDraw();
	}
	return true;
}


/*bool CActionEditCircle::DoStartTracking( CPoint pt )
{
	if(_Begin())
	{
		m_bInteractive = true;
		m_rect.left = pt.x;
		m_rect.right = pt.x;
		m_rect.top = pt.y;
		m_rect.bottom = pt.y;
		m_rectInvalid = m_rect;
		_DoDraw();
	}
	return true;
}*/

bool CActionEditCircle::DoTrack( CPoint pt )
{
	m_rect.right = pt.x;
	m_rect.bottom = pt.y;
	_DoDraw();
	return true;
}

bool CActionEditCircle::DoFinishTracking( CPoint pt )
{
	_End();
	Finalize();
	return true;
}


bool CActionEditCircle::DoSetCursor( CPoint point )
{
	if(!CActionEditorBase::DoSetCursor(point))
	{
		m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDITCIRCLE);
	}
	if( m_state == asActive && m_hcurActive )
	{
		::SetCursor( m_hcurActive );
		return true;
	}
	return CMouseImpl::DoSetCursor( point );
}


bool CActionEditCircle::Initialize()
{
	AddPropertyPageDescription( c_szCCircle1PropPage );
	if( !IsCompatibleTarget())
		if(!SetAppropriateView(m_punkTarget, avtSource) || m_state == asTerminate )
			return false;

	UpdatePropertyPages();

	CInteractiveActionBase::Initialize();

	//m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDITCIRCLE);
	m_rect.left = GetArgumentInt( _T("X") );
	m_rect.top = GetArgumentInt( _T("Y") );
	m_rect.right = GetArgumentInt( _T("X") ) + GetArgumentInt( _T("D") );
	m_rect.bottom = GetArgumentInt( _T("Y") ) + GetArgumentInt( _T("D") );
	if( m_rect != NORECT )
	{
		
		Finalize();
		return false;
	}

	LockMovement(true, GetTargetImageRect(m_punkTarget));
	return true;
}



//////////////////////////////////////////////////////////////////////
//CActionEditSplainLine implementation
CActionEditSplainLine::CActionEditSplainLine()
{
	m_strActionName = "EditSplainLine";
	m_rectOld = NORECT;
	m_rcFirstPoint = NORECT;
	m_nLineWidth = 1;
	m_bInitialized = false;
	m_bInteractive = false;
	m_bByBackColor = false;
	m_bEditMode = false;
	m_bEditPlacement = false;
	m_nDraggingPt = -1;
	m_bRing = false;
	m_bCanTrackByRButton = true;
	m_pContour = 0;
	m_bRightMouseButton = false;
}

bool CActionEditSplainLine::IsRequiredInvokeOnTerminate()
{
	if(m_bEditMode)return true;
	return false;
}

CActionEditSplainLine::~CActionEditSplainLine()
{
	//_ClearHotPoints();

	if(m_pContour != 0)
		::ContourDelete(m_pContour);
	
	for(int i = 0; i <= m_pHotPoints.GetUpperBound(); i++)
	{
		delete m_pHotPoints[i];
	}
	m_pHotPoints.RemoveAll();
}


int CActionEditSplainLine::_DeterminateHotPt(CPoint pt)
{
	CRect	rcHot = NORECT;
	CPoint ptArrow;
	for(int i = 0; i <= m_pHotPoints.GetUpperBound(); i++)
	{
		ptArrow = *m_pHotPoints[i];
		ptArrow = ConvertToWindow( m_punkTarget, ptArrow );
		rcHot.left = ptArrow.x - hotPoint/2;
		rcHot.right = rcHot.left + hotPoint;
		rcHot.top = ptArrow.y - hotPoint/2;
		rcHot.bottom = rcHot.top + hotPoint;
		// A.M. error BT2583 !
		/*rcHot.left = ptArrow.x;
		rcHot.right = rcHot.left + hotPoint;
		rcHot.top = ptArrow.y;
		rcHot.bottom = rcHot.top + hotPoint;*/
		rcHot = ConvertToClient( m_punkTarget, rcHot );
		rcHot.InflateRect(0,0,1,1);
		if(rcHot.PtInRect(pt)) return i;
	}
	return -1;
}

bool CActionEditSplainLine::DoSetCursor( CPoint point )
{
	//point = m_pointCurrent; //set window coordinates

	//TRACE("DoSetCursor, point=(%d, %d)\n", point.x, point.y);

	if(!CActionEditorBase::DoSetCursor(point))
	{
		
		m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDITSPLAIN);

		if(!m_bEditPlacement && m_bEditMode)
			m_nDraggingPt = _DeterminateHotPt(point);
		if(m_nDraggingPt >= 0)// && m_bEditPlacement)
		{
			::SetCursor( AfxGetApp()->LoadCursor(IDC_MOVE) );
			return true;
		}
	}
	if( m_state == asActive && m_hcurActive )
	{
		::SetCursor( m_hcurActive );
		return true;
	}
	return CMouseImpl::DoSetCursor( point );
}

bool CActionEditSplainLine::DoUpdateSettings()
{
	m_nLineWidth = ::GetValueInt(GetAppUnknown(), "Editor", "LineWidth", 1);
	return true;
}


bool CActionEditSplainLine::DoLButtonDown( CPoint pt )
{
	if((!m_bInitialized) && CActionEditorBase::DoLButtonDown(pt))return true;
	if(m_bEditMode)
	{
		m_nDraggingPt = _DeterminateHotPt(pt);
		m_bEditPlacement = true;
		if(m_nDraggingPt < 0)
		{
			m_bEditPlacement = false;
			m_bEditMode = false;
			Finalize();
		}
		return false;
	}
	if(!m_bInitialized)
	{
		if(_Begin())
		{
			m_bInteractive = true;
			m_bByBackColor = m_bUseBackColor;
			m_bInitialized = true;
			m_rect.left = pt.x;
			m_rect.top = pt.y;
			m_rcFirstPoint = m_rect;
			m_rcFirstPoint.left -= hotPoint/2;
			m_rcFirstPoint.top -= hotPoint/2;
			m_rcFirstPoint.right = m_rcFirstPoint.left + hotPoint;
			m_rcFirstPoint.bottom = m_rcFirstPoint.top + hotPoint;
			m_pContour = ::ContourCreate();
			_Draw();
		}
		else
			return true;
	}
	if(!m_bByBackColor)
	{
		CPoint* point;
		if(m_pHotPoints.GetUpperBound() >= 2 && m_rcFirstPoint.PtInRect(pt))
		{
			pt.x = m_rcFirstPoint.left+hotPoint/2;
			pt.y = m_rcFirstPoint.top+hotPoint/2;
			
		}
		//m_bRing = false;
		point = new CPoint(pt);
		m_pHotPoints.Add(point);
		m_rect.right = pt.x;
		m_rect.bottom = pt.y;
		if(m_pHotPoints.GetUpperBound() >= 2 && *m_pHotPoints[0] == *m_pHotPoints[m_pHotPoints.GetUpperBound()])
		{
			if(!m_bByBackColor)
			{
				m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDITSPLAIN);
				m_bEditMode = true;
				//m_bRing = true;		
				//_Draw();
			}
		}
		if(!m_bEditMode)
		{
			pt.Offset(-m_imgOffset);
			::ContourAddPoint(m_pContour, pt.x, pt.y);
		}
		_DoDraw();
		if(m_bEditMode)
			_Draw();
	}
	return false;
}

bool CActionEditSplainLine::DoRButtonDown( CPoint pt )
{
	if((!m_bInitialized) && CActionEditorBase::DoRButtonDown(pt))return true;
	m_bUseBackColor = true;
	if(m_bEditMode)
	{
		m_nDraggingPt = _DeterminateHotPt(pt);
		m_bEditPlacement = true;
		if(m_nDraggingPt < 0)
		{
			m_bEditPlacement = false;
			m_bEditMode = false;
			Finalize();
		}
		return true;
	}
	if(!m_bInitialized)
	{
		if(_Begin())
		{
			m_bInteractive = true;
			m_bByBackColor = m_bUseBackColor;
			m_bInitialized = true;
			m_rect.left = pt.x;
			m_rect.top = pt.y;
			m_rcFirstPoint = m_rect;
			m_rcFirstPoint.left -= hotPoint/2;
			m_rcFirstPoint.top -= hotPoint/2;
			m_rcFirstPoint.right = m_rcFirstPoint.left + hotPoint;
			m_rcFirstPoint.bottom = m_rcFirstPoint.top + hotPoint;
			m_pContour = ::ContourCreate();
		}
		else
			return true;
	}
	if(m_bByBackColor)
	{
		if(m_pHotPoints.GetUpperBound() >= 2 && m_rcFirstPoint.PtInRect(pt))
		{
			pt.x = m_rcFirstPoint.left+hotPoint/2;
			pt.y = m_rcFirstPoint.top+hotPoint/2;
		}
		CPoint* point;
		point = new CPoint(pt);
		m_pHotPoints.Add(point);
		m_rect.right = pt.x;
		m_rect.bottom = pt.y;
		if(m_pHotPoints.GetUpperBound() >= 2 && *m_pHotPoints[0] == *m_pHotPoints[m_pHotPoints.GetUpperBound()])
		{
			if(m_bByBackColor)
			{
				m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDITSPLAIN);
				m_bEditMode = true;
				//_Draw();
			}
		}
		if(!m_bEditMode)
		{
			pt.Offset(-m_imgOffset);
			::ContourAddPoint(m_pContour, pt.x, pt.y);
		}
		_DoDraw();
		if(m_bEditMode)
			_Draw();

	}
	return false;
}

void CActionEditSplainLine::_ClearHotPoints(int nNumber)
{
	int nNumPoints = m_pHotPoints.GetSize();
	int nBegin = 0;
	int nEnd = 0;
	if(nNumber < 0 || nNumber >= nNumPoints)
	{
		nEnd = nNumPoints - 1;
	}
	else
	{
		nBegin = nNumber;
		nEnd = nNumber;
	}
	CRect rc = NORECT;
	CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
	if(!pwnd)
		return;

	
	for(int i = nBegin; i <= nEnd; i++)
	{
		rc.left = m_pHotPoints[i]->x - hotPoint/2;
		rc.right = m_pHotPoints[i]->x + hotPoint/2;
		rc.top = m_pHotPoints[i]->y - hotPoint/2;
		rc.bottom = m_pHotPoints[i]->y + hotPoint/2;
		rc.InflateRect(5,5,5,5);
		rc = ConvertToWindow( m_punkTarget, rc );
		pwnd->InvalidateRect(&rc);
	}
	//pwnd->UpdateWindow();
}

void CActionEditSplainLine::DoDraw( CDC &dc )
{
	//TRACE("###CActionEditSplainLine::DoDraw\n");
	CRect	rcHot = NORECT;
	if(m_bEditMode)
	{	
		for(int i = 0; i < m_pHotPoints.GetSize(); i++)
		{
			rcHot.left = m_pHotPoints[i]->x;
			rcHot.top = m_pHotPoints[i]->y;
			rcHot = ConvertToWindow( m_punkTarget, rcHot );
//			rcHot.left -=hotPoint/2;
//			rcHot.top -= hotPoint/2;
			rcHot.left -= hotPoint/2;
			rcHot.top -= hotPoint/2;
			rcHot.right = rcHot.left + hotPoint;
			rcHot.bottom = rcHot.top + hotPoint;
			if(i == 0)
				dc.SelectStockObject(LTGRAY_BRUSH);
			else
				dc.SelectStockObject(WHITE_BRUSH);
			dc.SelectStockObject(BLACK_PEN);
			dc.SetROP2(R2_COPYPEN);
			dc.Rectangle(&rcHot);
		}
	}
	else
	{
		if(m_rcFirstPoint != NORECT && (m_pHotPoints.GetSize() < 3 || !m_rcFirstPoint.PtInRect(m_rect.BottomRight())))
		{
			rcHot = CRect(m_rcFirstPoint.left+hotPoint/2,m_rcFirstPoint.top+hotPoint/2,0,0);
			rcHot = ConvertToWindow( m_punkTarget, rcHot );
			rcHot.left -=hotPoint/2;
			rcHot.top -= hotPoint/2;
			rcHot.right = rcHot.left + hotPoint;
			rcHot.bottom = rcHot.top + hotPoint;
			
			dc.SelectStockObject(LTGRAY_BRUSH);
			dc.SelectStockObject(BLACK_PEN);
			dc.SetROP2(R2_COPYPEN);
			dc.Rectangle(&rcHot);
		}
	}
}


bool CActionEditSplainLine::DoLButtonDblClick( CPoint pt )
{
	if(m_bEditMode)
	{
		//_Draw();
		_ClearHotPoints();
		Finalize();
		return true;
	}
	if(!m_bByBackColor)
	{
		if(m_pHotPoints.GetSize() < 2)
		{
			Finalize();
			return true;
		}
		m_bEditMode = true;
		_Draw();
		//Finalize();
		return true;
	}
	return false;
}

bool CActionEditSplainLine::DoRButtonDblClick( CPoint pt )
{
	if(m_bEditMode)
	{
		_ClearHotPoints();
		Finalize();
		return true;
	}
	if(m_bByBackColor)
	{
		if(m_pHotPoints.GetSize() < 2)
		{
			Finalize();
			return true;
		}
		m_bEditMode = true;
		_Draw();
		return true;
	}
	return false;
}

bool CActionEditSplainLine::DoMouseMove( CPoint pt )
{
	if(m_bInitialized && !m_bEditMode)
	{
		// *m_pHotPoints[m_pHotPoints.GetUpperBound()] = pt;
		if(!m_bEditMode && m_pHotPoints.GetUpperBound() >= 2)
		{
			if(m_rcFirstPoint.PtInRect(pt))
			{
				pt.x = m_rcFirstPoint.left+hotPoint/2;
				pt.y = m_rcFirstPoint.top+hotPoint/2;
				m_hcurActive = AfxGetApp()->LoadCursor(IDC_TARGET);
				//m_bRing = true;		
			}
			else
			{
				m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDITSPLAIN);
				//m_bRing = false;		
			}

			::SetCursor( m_hcurActive );	
		}
		m_rect.right = pt.x;
		m_rect.bottom = pt.y;
		_DoDraw();
		return true;
	}
	return false;
}

bool CActionEditSplainLine::DoStartTracking( CPoint pt )
{
	if(m_bEditMode && m_nDraggingPt >= 0)
	{
		m_bEditPlacement = true;
	}
	//if(m_bEditMode)
	//	*m_pHotPoints[m_nDraggingPt] = pt;
	return true;
}

bool CActionEditSplainLine::DoTrack( CPoint pt )
{
	if(m_bEditMode)
	{
		m_rect.left = m_rect.right = pt.x;
		m_rect.top = m_rect.bottom = pt.y;

		if(m_nDraggingPt < 0)
			return true;

		//_Draw();

		_ClearHotPoints(m_nDraggingPt);
		pt.Offset(-m_imgOffset);
		::ContourSetPoint(m_pContour, m_nDraggingPt, pt.x, pt.y);
		pt.Offset(m_imgOffset);

		int nUpBound = m_pHotPoints.GetUpperBound();
		if(*m_pHotPoints[0] == *m_pHotPoints[nUpBound] && nUpBound >= 2)
		{
			if(m_nDraggingPt == 0)
				*m_pHotPoints[nUpBound] = pt;
			else if(m_nDraggingPt == nUpBound)
			{
				*m_pHotPoints[nUpBound] = pt;
				*m_pHotPoints[0] = pt;
				m_nDraggingPt = 0;
			}
			
		}

		

		*m_pHotPoints[m_nDraggingPt] = pt;
		_DoDraw();
		_Draw();
		
	}
	return true;
}

bool CActionEditSplainLine::DoFinishTracking( CPoint pt )
{
	m_bEditPlacement = false;
	_Draw();
	//if(m_bEditMode)
	//	*m_pHotPoints[m_nDraggingPt] = pt;
	return true;
}

bool CActionEditSplainLine::DoTerminate()
{
	m_bInitialized = false;
	//UndoImage(true);
	return CActionEditorBase::DoTerminate();
}

bool CActionEditSplainLine::Initialize()
{ 
	AddPropertyPageDescription( c_szCLine1PropPage );
	m_nLineWidth = ::GetValueInt(GetAppUnknown(), "Editor", "LineWidth", 1);

	if( !CActionEditorBase::Initialize() )
		return false;

	if( m_pctrl )
	{
		m_pctrl->Uninstall();
		m_pctrl->GetControllingUnknown()->Release();
		m_pctrl = 0;
	}
	//CInteractiveActionBase::Initialize();
	//
	return true;
}



void CActionEditSplainLine::_DoDraw()
{
	//draw in WalkFillInfo dc
	if(!m_bWorkWithBinary || m_bBinaryIsActive)
	{
		CRect rc(m_rect);
		rc.OffsetRect(-m_imgOffset);
		CDC	*pDC = m_pwfi->GetDC();
		CPen pen(PS_SOLID, m_nLineWidth, RGB(255, 255, 255));
		CPen* pOldPen = pDC->SelectObject(&pen);
		CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(WHITE_BRUSH);
		int idx = m_pHotPoints.GetUpperBound();

		
		rc.NormalizeRect();

		rc.InflateRect(m_nLineWidth, m_nLineWidth, m_nLineWidth, m_nLineWidth);
		
		RestoreImageFromChanges(m_rectOld);

		bool	bKillPt = false;
		CPoint	ptToAdd(-1,-1);
		if(!m_bEditMode)
		{
			m_bRing = (*m_pHotPoints[0] == CPoint(m_rect.right, m_rect.bottom))==TRUE;
			if(!m_bRing)
			{
				CPoint pt(m_rect.right, m_rect.bottom);
				pt.Offset(-m_imgOffset);
				::ContourAddPoint(m_pContour, pt.x, pt.y);	
				bKillPt = true;      
			}
		}		
		else
		{
			m_bRing = (*m_pHotPoints[0] == *m_pHotPoints[m_pHotPoints.GetUpperBound()])==TRUE;
			if(m_bRing && (m_pContour->nContourSize>=m_pHotPoints.GetSize()))
			{
				int x = -1;
				int	y = -1;
				::ContourGetPoint(m_pContour, m_pContour->nContourSize-1, x, y);
				ptToAdd = CPoint(x, y);
				::ContourKillLastPoint(m_pContour);
			}
		}	

		CRect rect = ::ContourCalcRect(m_pContour);
		//rect.OffsetRect(-m_imgOffset);
		if (m_pContour->nContourSize == 1)
		{
			pDC->MoveTo(rect.TopLeft());
			pDC->LineTo(rect.TopLeft());
		}
		else
		::ContourDraw(*pDC, m_pContour, 0, m_bRing?cdfSpline|cdfClosed:cdfSpline);
		if(bKillPt)
		{
			::ContourKillLastPoint(m_pContour);
		}
		if(ptToAdd != CPoint(-1, -1))
			::ContourAddPoint(m_pContour, ptToAdd.x, ptToAdd.y);	
		
		rect.InflateRect(m_nLineWidth, m_nLineWidth, m_nLineWidth, m_nLineWidth);

		MoveFillInfoToImage(rect, m_pColors, 0);

		m_rectOld = rect;
		m_rectInvalid = rect; 
		
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);

		if(m_bEditMode)
			_Draw();

	}
}

void CActionEditSplainLine::OnBegin()
{
	m_bInitialized = true;
}

void CActionEditSplainLine::_End()
{

}


void CActionEditSplainLine::OnFinalize()
{
	m_bInitialized = false;
	_ClearHotPoints();
	//m_pHotPoints.RemoveAll();
}


//////////////////////////////////////////////////////////////////////
//CActionEditFreeLine implementation
CActionEditFreeLine::CActionEditFreeLine()
{
	m_strActionName = "EditFreeLine";
	m_bDrawMode = false;
	m_nLineWidth = 1;
	m_bInitialized = false;
	m_bInteractive = false;
	m_bCanTrackByRButton = true;
	m_bUseBackColor = false;
	m_bTracking = false;
	m_bRightMouseButton = false;
}

CActionEditFreeLine::~CActionEditFreeLine()
{
}

bool CActionEditFreeLine::DoTrack( CPoint pt )
{
	//TRACE("CActionEditFreeLine::DoTrack\n");
	//m_timeTest.Report(false);
	m_rect.right = pt.x;
	m_rect.bottom = pt.y;
	_DoDraw();
	return true;
}

bool CActionEditFreeLine::DoLButtonDown( CPoint pt )
{
	if(CActionEditorBase::DoLButtonDown(pt))return true;
	m_bUseBackColor = false;
	m_rect.left = m_rect.right = pt.x;
	m_rect.top = m_rect.bottom = pt.y;
	if(_Begin())
	{
		m_bDrawMode = true;
		m_bInteractive = true;
		_DoDraw();
	}
	return false;
}

bool CActionEditFreeLine::DoRButtonDown( CPoint pt )
{
	if(CActionEditorBase::DoRButtonDown(pt))return true;
	m_bUseBackColor = true;
	m_rect.left = m_rect.right = pt.x;
	m_rect.top = m_rect.bottom = pt.y;
	if(_Begin())
	{
		m_bDrawMode = true;
		m_bInteractive = true;
		_DoDraw();
	}
	return false;
}


bool CActionEditFreeLine::DoRButtonUp( CPoint pt )
{
	if(!m_bDrawMode)return true;
	_End();
	Finalize();
	return true;
}

bool CActionEditFreeLine::DoLButtonUp( CPoint pt )
{
	if(!m_bDrawMode)return true;
	_End();
	Finalize();
	return true;
}

bool CActionEditFreeLine::DoStartTracking( CPoint pt )
{
	m_bTracking = true;
	return true;
}


bool CActionEditFreeLine::DoFinishTracking( CPoint pt )
{
	//_End();
	//Finalize();
	m_bTracking = false;
	return true;
}

void CActionEditFreeLine::OnFinalize()
{
	m_bDrawMode = false;
}

bool CActionEditFreeLine::DoSetCursor( CPoint point )
{
	if(!CActionEditorBase::DoSetCursor(point))
	{
		m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDITFREE);
	}
	if( m_state == asActive && m_hcurActive )
	{
		::SetCursor( m_hcurActive );
		return true;
	}
	return CMouseImpl::DoSetCursor( point );
}


bool CActionEditFreeLine::Initialize()
{ 
	AddPropertyPageDescription( c_szCLine1PropPage );
	return CActionEditorBase::Initialize();
}


void CActionEditFreeLine::_DoDraw()
{
	//draw in WalkFillInfo dc
	if(!m_bWorkWithBinary || m_bBinaryIsActive)
	{
		int nLineWidth = m_bWorkWithBinary&&m_bUseBackColor?m_nLineWidth2:m_nLineWidth;
		CRect rc(m_rect);
		rc.OffsetRect(-m_imgOffset);
		CDC	*pDC = m_pwfi->GetDC();
		CPen pen(PS_SOLID, nLineWidth, RGB(255, 255, 255));
		CBrush brush(RGB(255, 255, 255));
		CPen* pOldPen = pDC->SelectObject(&pen);
		pDC->SelectStockObject(WHITE_BRUSH);
	

		pDC->MoveTo(rc.left, rc.top);
		if( !m_bTracking && nLineWidth==1 )
			pDC->SetPixel( rc.BottomRight(), RGB( 255, 255, 255 ) );
		else if( !m_bTracking && nLineWidth==2 )
			pDC->FillRect( CRect( rc.right-1, rc.bottom-1, rc.right+1, rc.bottom+1), &brush );
		else
			pDC->LineTo(rc.right, rc.bottom);

		pDC->SelectObject(pOldPen);
		rc.NormalizeRect();

		rc.InflateRect(nLineWidth, nLineWidth, nLineWidth, nLineWidth);
		
		rc.left = max(rc.left, 0);
		rc.top = max(rc.top, 0);

		if(rc.left < m_rectInvalid.left)
			m_rectInvalid.left = rc.left;
		if(rc.top < m_rectInvalid.top)
			m_rectInvalid.top = rc.top;
		if(rc.right > m_rectInvalid.right)
			m_rectInvalid.right = rc.right;
		if(rc.bottom > m_rectInvalid.bottom)
			m_rectInvalid.bottom = rc.bottom;


		MoveFillInfoToImage(rc, m_pColors, 0);
		
		m_rect.left = m_rect.right;
		m_rect.top = m_rect.bottom;
	}
}

void CActionEditFreeLine::OnBegin()
{
	m_bInitialized = true;
}

void CActionEditFreeLine::_End()
{
}

/*
bool CActionEditFreeLine::DoUndo()
{
	CRect rcTmp = m_rectInvalid;
	RestoreFromPixArray(m_punkImage, m_rectInvalid, false);
	m_rectInvalid.OffsetRect(m_imgOffset);
	::FireEvent(m_punkTarget, szNeedUpdateView, m_punkTarget, GetControllingUnknown(), 0);
	m_rectInvalid = rcTmp;
	return true;
}

bool CActionEditFreeLine::DoRedo()
{	
	CRect rcTmp = m_rectInvalid;
	m_pwfi->AttachArrayToImage(m_punkImage, m_rectInvalid, m_pColors);
	m_rectInvalid.OffsetRect(m_imgOffset);
	::FireEvent(m_punkTarget, szNeedUpdateView, m_punkTarget, GetControllingUnknown(), 0);
	m_rectInvalid = rcTmp;
	return true;
}
*/

bool CActionBinEditFreeLine::Initialize()
{ 
	AddPropertyPageDescription( c_szCLineBinPropPage );
	if (!CActionEditorBase::Initialize())
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionEditLine implementation
CActionEditLine::CActionEditLine()
{
	m_strActionName = "EditLine";
	m_bInteractive = false;
	m_nLineWidth = 1;
	m_bCanTrackByRButton = true;
}

CActionEditLine::~CActionEditLine()
{
}

void CActionEditLine::OnBegin()
{
}

void CActionEditLine::_End()
{
	SetArgument( _T("X1"), _variant_t( m_rect.left ) );
	SetArgument( _T("Y1"), _variant_t( m_rect.top ) );
	SetArgument( _T("X2"), _variant_t( m_rect.right ) );
	SetArgument( _T("Y2"), _variant_t( m_rect.bottom ) );
}

void CActionEditLine::_DoDraw()
{
	//draw in WalkFillInfo dc
	if(!m_bWorkWithBinary || m_bBinaryIsActive)
	{
		CRect rc(m_rect);
		rc.OffsetRect(-m_imgOffset);
		CDC	*pDC = m_pwfi->GetDC();

		CPen pen(PS_SOLID, m_nLineWidth, RGB(255, 255, 255));
		CPen* pOldPen = pDC->SelectObject(&pen);
		
		if((short)GetKeyState(VK_SHIFT) < 0)
		{
			double SIN = 0;
			double COS = 0;
			int nLength = 0;
			int dX = rc.right - rc.left;
			int dY = rc.top - rc.bottom;
			nLength = sqrt(pow((double)dX,2) + pow((double)dY, 2));
			if (nLength == 0)
				return;
			SIN = dY*1./nLength;
			COS = dX*1./nLength;
			double absSIN = fabs(SIN); //by first quarter
			double kX = 0;
			double kY = 0;
			if(absSIN >= 0 && absSIN < 0.5)
			{
				//0
				kX = 1; 
				kY = 0;
			}

			else if(absSIN >= 0.5 && absSIN < 0.866)
			{
				//45
				kX = kY = 0.707; 
			}
			else if(absSIN >= 0.866 && absSIN <= 1)
			{
				//90
				kX = 0; 
				kY = 1;
			}
			rc.right = rc.left + (int)(kX*nLength*(dX < 0 ? -1 : 1));
			rc.bottom = rc.top - (int)(kY*nLength*(dY < 0 ? -1 : 1));
			m_rect.right = m_rect.left + (int)(kX*nLength*(dX < 0 ? -1 : 1));
			m_rect.bottom = m_rect.top - (int)(kY*nLength*(dY < 0 ? -1 : 1));
		}

		pDC->MoveTo(rc.left, rc.top);
		pDC->LineTo(rc.right, rc.bottom);
		
		pDC->SelectObject(pOldPen);
		
		rc.NormalizeRect();
		rc.InflateRect(m_nLineWidth, m_nLineWidth, m_nLineWidth, m_nLineWidth);
		
		RestoreImageFromChanges(m_rectInvalid);

		MoveFillInfoToImage(rc, m_pColors, m_pColors);

		m_rectInvalid = m_rect;
		m_rectInvalid.NormalizeRect();
		m_rectInvalid.OffsetRect(-m_imgOffset);
	}
}

bool CActionEditLine::DoButtonDown( CPoint pt )
{
	if(_Begin())
	{
		m_bInteractive = true;
		m_rect.left = pt.x;
		m_rect.right = pt.x;
		m_rect.top = pt.y;
		m_rect.bottom = pt.y;
		m_rectInvalid = m_rect;
		_DoDraw();
	}
	return true;
}

bool CActionEditLine::DoButtonUp( CPoint pt )
{
	_End();
	Finalize();
	return true;
}


/*bool CActionEditLine::DoStartTracking( CPoint pt )
{
	if(_Begin())
	{
		m_bInteractive = true;
		m_rect.left = pt.x;
		m_rect.right = pt.x;
		m_rect.top = pt.y;
		m_rect.bottom = pt.y;
		m_rectInvalid = m_rect;
		_DoDraw();
	}
	return true;
}*/

bool CActionEditLine::DoTrack( CPoint pt )
{
	m_rect.right = pt.x;
	m_rect.bottom = pt.y;
	_DoDraw();
	return true;
}

bool CActionEditLine::DoFinishTracking( CPoint pt )
{
//	_End();
//	Finalize();
	return true;
}

bool CActionEditLine::DoSetCursor( CPoint point )
{
	if(!CActionEditorBase::DoSetCursor(point))
	{
		m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDITLINE);
	}
	if( m_state == asActive && m_hcurActive )
	{
		::SetCursor( m_hcurActive );
		return true;
	}
	return CMouseImpl::DoSetCursor( point );
}


bool CActionEditLine::Initialize()
{
	AddPropertyPageDescription( c_szCLine1PropPage );
	if( !CActionEditorBase::Initialize() )return false;
	//CInteractiveActionBase::Initialize();
	//m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDITLINE);
	m_rect.left = GetArgumentInt( _T("X1") );
	m_rect.top = GetArgumentInt( _T("Y1") );
	m_rect.right = GetArgumentInt( _T("X2") );
	m_rect.bottom = GetArgumentInt( _T("Y2") );
	if( m_rect != NORECT )
	{
		if(_Begin())
		{
			_DoDraw();
			m_bInteractive = false;
			Finalize();
		}
	}
	return true;
}



//////////////////////////////////////////////////////////////////////
//CActionEditorBase implementation

BEGIN_INTERFACE_MAP(CActionEditorBase, CInteractiveActionBase)
	//INTERFACE_PART(CActionEditorBase, IID_IInteractiveAction, IntAction)
	//INTERFACE_PART(CActionEditorBase, IID_IMsgListener, MsgList)
	//INTERFACE_PART(CActionEditorBase, IID_IDrawObject, Draw)
	INTERFACE_PART(CActionEditorBase, IID_IPaintInfo, Info)
	INTERFACE_PART(CActionEditorBase, IID_IEditorAction, Edit)
END_INTERFACE_MAP();


IMPLEMENT_UNKNOWN(CActionEditorBase, Info);
IMPLEMENT_UNKNOWN(CActionEditorBase, Edit);

HRESULT CActionEditorBase::XInfo::GetInvalidRect( RECT* pRect )
{
	_try_nested(CActionEditorBase, Info, GetInvalidRect)
	{
		if(pRect)
		{
			*pRect = (RECT)pThis->m_rectInvalid;
			return S_OK;
		}
		else return S_FALSE;
	}
	_catch_nested;
}

HRESULT CActionEditorBase::XEdit::GetImage( IUnknown **ppunkImage )
{
	METHOD_PROLOGUE_EX(CActionEditorBase, Edit);
	*ppunkImage = pThis->m_punkImage;
	if( *ppunkImage )(*ppunkImage)->AddRef();
	return S_OK;
}

HRESULT CActionEditorBase::XEdit::GetDifferences( void ***pppDifferences, int *pnStart, int *pnCount, DWORD* pdwFlags )
{
	METHOD_PROLOGUE_EX(CActionEditorBase, Edit);

	*pnStart = pThis->m_imgOffset.y;
	*pppDifferences = pThis->m_ptrFillInfo.GetData();
	*pnCount = pThis->m_ptrFillInfo.GetSize();
	*pdwFlags = (pThis->m_bWorkWithBinary ? dfBinary : 0)|(pThis->m_bUseBackColor ? dfInvertColor : 0);
	return S_OK;
}

HRESULT CActionEditorBase::XEdit::SetDifferences( void **ppDifferences, RECT rectSource, DWORD dwFlags )
{
	METHOD_PROLOGUE_EX(CActionEditorBase, Edit);

	pThis->m_bUseBackColor = ((dwFlags&dfInvertColor)==dfInvertColor)?true:false;

	if(!pThis->_Begin(true, ((dwFlags&dfBinary)==dfBinary) ? 1 : 0))
		return S_FALSE;

	
	if(pThis->m_bWorkWithBinary)
	{
		CBinImageWrp	binary( pThis->m_punkImage );
		CPoint ptTopLeft(0,0);
		binary->GetOffset(&ptTopLeft);
		CRect	rectLocal = CRect(ptTopLeft, CSize(binary.GetWidth(),binary.GetHeight()) );

		FillInfo* pInfo = 0;
		FillInfo* pInfoNext = 0;

		int		y;

		int nBinRowSize = 0;
		
		byte* pbin = 0;

		ASSERT( pThis->m_ptrFillInfo.GetSize() == rectLocal.Height() );

		for( y = rectLocal.top; y < rectLocal.bottom; y++)
		{
			int	nRow = y-rectLocal.top;
			//delete all row
			pInfo = pThis->m_ptrFillInfo[nRow];
			while(pInfo)
			{
				pInfoNext = pInfo->pNext;
				pInfo->Destroy();
				pInfo = pInfoNext;
			}
			pThis->m_ptrFillInfo[nRow] = 0;

			FillInfo	*pFillInfoSrc = 0;
			if( y >= rectSource.top && y < rectSource.bottom )
				pFillInfoSrc = (FillInfo*)ppDifferences[y - rectSource.top];

			FillInfo	*pFillInfoLast = 0;
						
			binary->GetRow(&pbin, nRow, FALSE);

			while( pFillInfoSrc )
			{
				int	xs = max( pFillInfoSrc->nX+rectSource.left, rectLocal.left );
				int	xe = min( pFillInfoSrc->nX+pFillInfoSrc->nCount+rectSource.left, rectLocal.right );

				if( xs < xe )
				{
					//has intersection
					FillInfo	*pFillInfo = FillInfo::Create();
					ZeroMemory( pFillInfo, sizeof( FillInfo ) );

					if( pFillInfoLast )pFillInfoLast->pNext = pFillInfo;
					pFillInfoLast = pFillInfo;
					if( !pThis->m_ptrFillInfo[nRow] )pThis->m_ptrFillInfo[nRow]	 = pFillInfo;

					pFillInfo->nCount = xe-xs;
					pFillInfo->nX = xs-rectLocal.left;
					pFillInfo->bFill = pFillInfoSrc->bFill;



					/*long nColorSize = sizeof(color)*8;
					long nAllocSize = pFillInfo->nCount/nColorSize+(pFillInfo->nCount%nColorSize==0 ? 0 : 1);
					pFillInfo->CreateColors(nAllocSize);
					ZeroMemory(pFillInfo->pColors, nAllocSize*sizeof(color));*/

          pFillInfo->CreateColors(pFillInfo->nCount);
          long nX = pFillInfo->nX;
					for (int n = 0; n < pFillInfo->nCount; n++, nX++)
					{
						/*if((pbin[nX/8] & (0x80>>(nX%8))) != 0)
							pFillInfo->pColors[n/nColorSize] |= 0x8000>>( n % nColorSize );*/
						// vk
						pFillInfo->pColors[n] = pbin[nX];
						//pFillInfo->pColors[n] = pbin[nX]^128;
						//pFillInfo->pColors[n] = pbin[nX]-128;
						
						//pFillInfo->pColors[n] = pbin[nX];
						// end vk
					}	
				}
				pFillInfoSrc = pFillInfoSrc->pNext;
			}
		}
		
		pThis->m_rectInvalid = rectSource;

		::AttachArrayToImageBin(&pThis->m_ptrFillInfo, pThis->m_punkImage, 
			pThis->m_rectInvalid, pThis->m_pColors, pThis->m_pFillColors );
	}
	else
	{
		CImageWrp	image( pThis->m_punkImage );
		CRect	rectLocal = image.GetRect();

		FillInfo* pInfo = 0;
		FillInfo* pInfoNext = 0;

		int		y;

		ASSERT( pThis->m_ptrFillInfo.GetSize() == rectLocal.Height() );

		for( y = rectLocal.top; y < rectLocal.bottom; y++)
		{
			int	nRow = y-rectLocal.top;
			//delete all row
			pInfo = pThis->m_ptrFillInfo[nRow];
			while(pInfo)
			{
				pInfoNext = pInfo->pNext;
				pInfo->Destroy();
				pInfo = pInfoNext;
			}
			pThis->m_ptrFillInfo[nRow] = 0;

			FillInfo	*pFillInfoSrc = 0;
			if( y >= rectSource.top && y < rectSource.bottom )
				pFillInfoSrc = (FillInfo*)ppDifferences[y - rectSource.top];

			FillInfo	*pFillInfoLast = 0;
			int	colors = image.GetColorsCount();

			while( pFillInfoSrc )
			{
				int	xs = max( pFillInfoSrc->nX+rectSource.left, rectLocal.left );
				int	xe = min( pFillInfoSrc->nX+pFillInfoSrc->nCount+rectSource.left, rectLocal.right );

				if( xs < xe )
				{
					//has intersection
					FillInfo	*pFillInfo = FillInfo::Create();
					ZeroMemory( pFillInfo, sizeof( FillInfo ) );

					if( pFillInfoLast )pFillInfoLast->pNext = pFillInfo;
					pFillInfoLast = pFillInfo;
					if( !pThis->m_ptrFillInfo[nRow] )pThis->m_ptrFillInfo[nRow]	 = pFillInfo;

					pFillInfo->nCount = xe-xs;
					pFillInfo->CreateColors( pFillInfo->nCount*colors );
					pFillInfo->nX = xs-rectLocal.left;
					pFillInfo->bFill = pFillInfoSrc->bFill;

					for( int c = 0; c < colors; c++ )
					{
						color	*pc = image.GetRow( y - rectLocal.top, c )+pFillInfo->nX;
						color	*pcd = pFillInfo->pColors+c;

						for( int n = 0; n < pFillInfo->nCount; n++, pc++, pcd+=colors )
							*pcd = *pc;
						
						//memcpy( pcd, pc, sizeof( color )*pFillInfo->nCount );
					}
				}
				pFillInfoSrc = pFillInfoSrc->pNext;
			}
		}
		
		pThis->m_rectInvalid = rectSource;

		::AttachArrayToImage(&pThis->m_ptrFillInfo, pThis->m_punkImage, 
			pThis->m_rectInvalid, pThis->m_pColors, pThis->m_pFillColors, pThis->m_ImagePaneCache );
	}


	pThis->m_state = asReady;


	return S_OK;
}



CActionEditorBase::CActionEditorBase() : m_ImagePaneCache(NULL)
{
	m_bOldModified = false;
	m_bUseBackColor = false;
	m_rect = NORECT;
	m_rectInvalid = NORECT;
	m_pColors = 0;
	m_pFillColors = 0;
	m_punkImage = 0;
	m_pwfi = 0;
	m_iTrackDeltaPos = 0;
	m_imgOffset = CPoint( 0, 0 );
	m_pctrl = 0;
	m_bWorkWithBinary = false;

	m_bBinaryIsActive = false;

	m_bRightMouseButton = false;

	m_bTerminateOnActivateObj = true;


	m_nLineWidth = 0;
	m_nLineWidth2 = 0;
	m_bUseWidth2 = false;
}




CActionEditorBase::~CActionEditorBase()
{
	//DeletePixArray();
	FillInfo* pInfo = 0;
	FillInfo* pInfoNext = 0;
	for(int i = 0; i <= m_ptrFillInfo.GetUpperBound(); i++)
	{
		pInfo = m_ptrFillInfo[i];
		while(pInfo)
		{
			pInfoNext = pInfo->pNext;
			//delete pInfo->pColors;
			//delete pInfo;
			pInfo->Destroy();
			pInfo = pInfoNext;
		}
	}
	m_ptrFillInfo.RemoveAll();

	if(m_punkImage)
	{
		m_punkImage->Release();
		m_punkImage = 0;
	}
	if (m_pwfi)
	{
		delete m_pwfi;
		m_pwfi = 0;
	}
	if(m_pFillColors) delete m_pFillColors;
	if(m_pColors) delete m_pColors;

}

bool CActionEditorBase::IsCompatibleTarget()
{
	DWORD dwFlags = 0;
	IImageViewPtr sptrIV(m_punkTarget);
	if(sptrIV != 0)sptrIV->GetImageShowFlags(&dwFlags);
	if(m_bWorkWithBinary)
	{
		return ((dwFlags & isfBinaryFore) == isfBinaryFore) || 
			   ((dwFlags & isfBinaryBack) == isfBinaryBack) ||
			   ((dwFlags & isfBinary) == isfBinary) ||
			   ((dwFlags & isfBinaryContour) == isfBinaryContour);
	}
	else
	{
		return (dwFlags & isfSourceImage) == isfSourceImage;
	}
	
}



bool CActionEditorBase::Initialize()
{
	//view manipulation
	if( !IsCompatibleTarget())
		if(!SetAppropriateView(m_punkTarget, m_bWorkWithBinary ? avtBinary : avtSource) || m_state == asTerminate )
			return false;

	//_ReportCounter( m_punkTarget );

	::SetValue(GetAppUnknown(), "Editor", "BinaryMode", _variant_t((long)(m_bWorkWithBinary?1:0)));
	bool bSelector = (::GetValueInt(GetAppUnknown(), "Editor", "SelectBinary", 0)==1);
	if(bSelector && m_bWorkWithBinary)
	{
		AddPropertyPageDescription( c_szBinEditorPropPage );
	}
	UpdatePropertyPages();

	if( !CInteractiveActionBase::Initialize() )return false;
	_UpdateLinePreview();
/*	m_nLineWidth = ::GetValueInt(GetAppUnknown(), "Editor", "LineWidth", 1);
	m_nLineWidth2 = ::GetValueInt(GetAppUnknown(), "Editor", "LineWidth2", 1);
	if( m_pctrl )
	{
		m_pctrl->Uninstall();
		m_pctrl->GetControllingUnknown()->Release();
		m_pctrl = 0;
	}
	m_pctrl = new CLinePreviewCtrl(m_nLineWidth);
	m_pctrl->Install();*/


	// [Max] Draw trace outside soucre image rect if binary image has moved 
	if(m_bWorkWithBinary || m_bBinaryIsActive)
	{

		IDataContext2Ptr sptrDC = m_punkTarget;
		if(sptrDC == 0)
		{
			LockMovement(true, GetTargetImageRect(m_punkTarget));
			return true;
		}

		
		IUnknown *punkImg = 0;
		sptrDC->GetActiveObject( _bstr_t(szArgumentTypeBinaryImage), &punkImg );

		if( !punkImg )
		{
			LockMovement(true, GetTargetImageRect(m_punkTarget));
			return true;
		}

		IBinaryImagePtr ptrI = punkImg;
		punkImg->Release();

		if( ptrI == 0 )
		{
			LockMovement(true, GetTargetImageRect(m_punkTarget));
			return true;
		}

		int cx, cy;
		POINT pt;
		
		ptrI->GetSizes( &cx, &cy );
		ptrI->GetOffset( &pt );

		LockMovement(true, CRect( pt, CSize( cx, cy ) ) );
	}
	else
		LockMovement(true, GetTargetImageRect(m_punkTarget));

	return true;
}

void CActionEditorBase::Finalize()
{
	OnFinalize();
	if( m_pctrl )
	{
		m_pctrl->Uninstall();
		m_pctrl->GetControllingUnknown()->Release();
		m_pctrl = 0;
	}
	/*CViewBase* view = 0;
	view = (CViewBase*)GetWindowFromUnknown(m_punkTarget);
	if(view)
		view->m_dataSource.ForceEnabled(true);*/
	CInteractiveActionBase::Finalize();
}
	

bool CActionEditorBase::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	UndoImage(false);

	//[AY]set modified flag
	{
		INamedDataObject2Ptr ptrNamed( m_punkImage );
		ptrNamed->SetModifiedFlag( m_bOldModified );
	}

	return true;
}

bool CActionEditorBase::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	//_DoDraw(false); //not restore old
	CRect rcTmp = m_rectInvalid;
	m_rectInvalid.InflateRect(m_nLineWidth, m_nLineWidth, m_nLineWidth, m_nLineWidth);

	
	if(m_bWorkWithBinary)
	{

//		color clr = -(*m_pColors); // [Max] Redo after undo trace
		AttachArrayToImageBin(&m_ptrFillInfo, m_punkImage, m_rectInvalid, m_pColors, m_pFillColors);
	}
	else
		AttachArrayToImage(&m_ptrFillInfo, m_punkImage, m_rectInvalid, m_pColors, m_pFillColors,
			m_ImagePaneCache);
	m_rectInvalid.OffsetRect(m_imgOffset);

	//paul 21.06.2003
	//::FireEvent(m_punkTarget, szNeedUpdateView, m_punkTarget, GetControllingUnknown(), 0);
	::UpdateDataObject( m_punkTarget, m_punkImage );

	m_rectInvalid = rcTmp;

		//[AY]set modified flag
	{
		INamedDataObject2Ptr ptrNamed( m_punkImage );
		ptrNamed->SetModifiedFlag( true );
	}

/*	if( m_bWorkWithBinary )
	{
		IViewPtr sptrView = m_punkTarget;
		IUnknown *punk = 0;
		sptrView->GetDocument( &punk );

		IDocumentSitePtr sptrDoc = punk;
		if( punk )
			punk->Release();
		punk = 0;

		sptrDoc->GetActiveView( &punk );
		IViewPtr sptrActiveView = punk;
		if( punk )
			punk->Release();
		punk = 0;
		sptrDoc->SetActiveView( m_punkTarget );	 */
//		sptrDoc->SetActiveView( sptrActiveView );
//	}


	return true;
}

bool CActionEditorBase::Invoke()
{
	_try(CActionEditorBase, Invoke)
	{
		//get NameData
		IUnknownPtr	sptrD;
		{
			sptrIView	sptrV( m_punkTarget );
			sptrV->GetDocument( &sptrD );
		}

		m_lTargetKey = ::GetObjectKey( sptrD );

		if(!m_bInteractive)
		{
			//too late, too late...

			//draw figure
			//if (!_Begin())
			//	return false;
			//_DoDraw();
			
		}

		_End();

		if (m_pwfi)
		{
			delete m_pwfi;
			m_pwfi = 0;
		}
		
		//m_rectInvalid = m_rect;
		//m_rectInvalid = ConvertToWindow( m_punkTarget, m_rectInvalid);
		
		m_rectInvalid.InflateRect(2*m_nLineWidth, 2*m_nLineWidth, 2*m_nLineWidth, 2*m_nLineWidth);
		
		//paul 12.09.2001
		//::FireEvent(sptrD, szNeedUpdateView, sptrD, GetControllingUnknown(), 0);
		::UpdateDataObject( sptrD, m_punkImage );

		//[AY]set modified flag
		{
			INamedDataObject2Ptr ptrNamed( m_punkImage );
			if (ptrNamed != 0)
			{
			ptrNamed->IsModified( &m_bOldModified );
			ptrNamed->SetModifiedFlag( true );
		}
		}

		
		return true;
	}
	_catch;
	{
		return false;
	}
}


/*bool CActionEditorBase::OnRButtonDown( CPoint pt )
{
	//m_bUseBackColor = true;
	return false;
}

bool CActionEditorBase::OnLButtonDown( CPoint pt )
{
	return false;
}
*/

bool CActionEditorBase::_Begin(bool bCanUseBinary, long nDiffType)
{
	//nDiffType = -1	nothing matters
	//nDiffType = 0		we will use normal differences
	//nDiffType = 1		we will use binary differences
	{
		if(m_punkImage)
		{
			m_punkImage->Release();
			m_punkImage = 0;
		}

		if(nDiffType >= 0)
		{
			m_bWorkWithBinary = (nDiffType == 1);
		}
		else
		{
			if(bCanUseBinary)
			{
			//	m_bWorkWithBinary = ::GetValueInt(GetAppUnknown(), "Editor", "WorkWithBinary", 0) == 1;
			}
			else
				m_bWorkWithBinary = false;
		}

		sptrIDataContext2 sptrDC(m_punkTarget);
		if(sptrDC == 0)
		{
			Terminate();
			return false;
		}

		
		if(m_bWorkWithBinary)
		{
			sptrDC->GetActiveObject( _bstr_t(szArgumentTypeBinaryImage), &m_punkImage );
			if(!m_punkImage)
			{
				IUnknown* punkImage = 0;

				//if((nDiffType < 0))
					sptrDC->GetActiveObject( _bstr_t(szArgumentTypeImage), &punkImage );

				if(!punkImage)
				{
					Terminate();
					return false;
				}

				CImageWrp image(punkImage);
				punkImage->Release();

				//if(AfxMessageBox(IDS_CREATE_NEW_BINARY, MB_YESNO) == IDYES)
				{
					m_bTerminateOnActivateObj = false;

					IViewPtr sptrView(m_punkTarget);

					IUnknown *punkDoc = 0;
					sptrView->GetDocument(&punkDoc);
					m_punkImage = ::CreateTypedObject( szArgumentTypeBinaryImage );

					CBinImageWrp binary(m_punkImage);
					if( binary == 0 ) return false;

					INamedDataObject2Ptr sptrN = image;
					IUnknown *punkParent = 0;
					if(sptrN != 0)
					{
						sptrN->GetParent(&punkParent);
						if(punkParent)
						{
							image = punkParent;
							punkParent->Release();
						}
					}


					if( !binary.CreateNew(image.GetWidth(), image.GetHeight()) )
						return false;
					
					binary->SetOffset(image.GetOffset(), TRUE);

					if(punkDoc)
					{
						::SetBaseObject( binary, image, punkDoc );
						_variant_t	var( m_punkImage );
//						::SetValue( punkDoc, 0, 0, var );		
						m_changes.SetToDocData( punkDoc, m_punkImage );

						punkDoc->Release();
					}

					sptrDC->SetActiveObject(_bstr_t(szArgumentTypeBinaryImage), m_punkImage, aofActivateDepended);

					m_bTerminateOnActivateObj = true;
					
				}
				//else
				//	Terminate();

				//return false;
				
			}

			IImageViewPtr	sptrV(m_punkTarget);
			if(sptrV != 0)
				sptrV->SetSelectionMode(2);

			m_bBinaryIsActive = true;
		}
		else
		{
			sptrDC->GetActiveObject( _bstr_t(szArgumentTypeImage), &m_punkImage );
			
		}

		if( !m_punkImage )
		{
			Terminate();
			return false;
		}

	
		CRect rc = NORECT;

		if(m_bWorkWithBinary)
		{
			CBinImageWrp binary(m_punkImage);
			binary->GetOffset(&m_imgOffset);
			rc = CRect(0,0, binary.GetWidth(), binary.GetHeight());

			if (m_pColors != NULL)
			{
				delete m_pColors;
				m_pColors = NULL;
			}
			m_pColors = new color[1];
			if (m_pFillColors != NULL)
			{
				delete m_pFillColors;
				m_pFillColors = NULL;
			}
			m_pFillColors = new color[1];

			if(m_bUseBackColor)
			{
				m_pColors[0] = 0;
				m_pFillColors[0] = 1;
			}
			else
			{
				if (GetValueInt(GetAppUnknown(), "\\Binary", "Multyphase", 1))
					// vk
					m_pColors[0] = ::GetValueInt( GetAppUnknown(), "\\Binary", "FillColor", 128);
					//m_pColors[0] = ::GetValueInt( GetAppUnknown(), "\\Binary", "FillColor", 1);
					// end vk
				else
					m_pColors[0] = get_binary_image_color(binary);
				m_pFillColors[0] = 0;
			}
			
		}
		else
		{
			CImageWrp image(m_punkImage);
			m_imgOffset = image.GetOffset();
			rc = image.GetRect();

//			bool bColor = image.IsColor();
			IUnknown *punkCC;
			image->GetColorConvertor( &punkCC );
			IColorConvertor6Ptr sptrCC = punkCC;
			punkCC->Release();
			int nColorMode;
			sptrCC->GetEditMode(image, &nColorMode);

			//get colors
			COLORREF Fore, Back;
			byte b1[3], b2[3];
			if(m_bUseBackColor)
			{
				Back = ::GetValueColor(GetAppUnknown(), "Editor", (nColorMode==1 ? "Fore" : "Fore gray"), RGB(255,255,255));	
				Fore = ::GetValueColor(GetAppUnknown(), "Editor", (nColorMode==1 ? "Back" : "Back gray"), RGB(0,0,0));	
			}
			else
			{
				Fore = ::GetValueColor(GetAppUnknown(), "Editor", (nColorMode==1 ? "Fore" : "Fore gray"), RGB(255,255,255));	
				Back = ::GetValueColor(GetAppUnknown(), "Editor", (nColorMode==1 ? "Back" : "Back gray"), RGB(0,0,0));	
			}

			int nColors = image.GetColorsCount();
			if (m_pColors != NULL)
			{
				delete m_pColors;
				m_pColors = NULL;
			}
			m_pColors = new color[nColors];
			if (m_pFillColors == NULL)
			{
				delete m_pFillColors;
				m_pFillColors = NULL;
			}
			m_pFillColors = new color[nColors];
			if(sptrCC != 0)
			{
				b1[0] = (byte)GetBValue(Fore); 
				b1[1] = (byte)GetGValue(Fore);
				b1[2] = (byte)GetRValue(Fore);
				
				b2[0] = (byte)GetBValue(Back); 
				b2[1] = (byte)GetGValue(Back);
				b2[2] = (byte)GetRValue(Back);
				color** ppColors1 = new color*[nColors];
				color** ppColors2 = new color*[nColors];
				for (int i = 0; i < nColors; i++)
				{
					ppColors1[i] = new color[1];
					ppColors2[i] = new color[1];
				}
				sptrCC->ConvertDIBToImageEx( b1, ppColors1, 1, TRUE, nColors ); 
				sptrCC->ConvertDIBToImageEx( b2, ppColors2, 1, TRUE, nColors ); 
				for (i = 0; i < nColors; i++)
				{
					m_pColors[i] = *ppColors1[i];
					m_pFillColors[i] = *ppColors2[i];
					delete ppColors1[i];
					delete ppColors2[i];
				}
				delete ppColors1;
				delete ppColors2;
			}
		}

		
		if (m_pwfi)
		{
			delete m_pwfi;
			m_pwfi = 0;
		}
		m_pwfi = new CWalkFillInfo(CRect(0, 0, rc.Width(), rc.Height() ));
		m_pwfi->InitFillInfo(&m_ptrFillInfo);
	
		
		OnBegin();

	
	}
	return true;
}

bool CActionEditorBase::DoRButtonDown( CPoint pt )
{
	m_bUseBackColor = true;
	if((short)GetKeyState(VK_CONTROL) < 0  && !m_bWorkWithBinary)
	{
		m_bRightMouseButton = true;

		CString	strExecuteParams;
		strExecuteParams.Format( "%d, %d, %d", pt.x, pt.y, 1 );
		::ExecuteAction(_T("PickColor"), strExecuteParams, 0);
		if(!m_strActionName.IsEmpty())
			ExecuteAction(m_strActionName, "", 0);
		return true;
	}
	//else
	//	return OnRButtonDown( pt );

	DoButtonDown(pt);
	return m_bRightMouseButton;
}

bool CActionEditorBase::DoRButtonUp( CPoint pt )
{
	DoButtonUp(pt);
	return true;
}


bool CActionEditorBase::DoLButtonDown( CPoint pt )
{
	m_bUseBackColor = false;
	if((short)GetKeyState(VK_CONTROL) < 0 && !m_bWorkWithBinary)
	{
		CString	strExecuteParams;
		strExecuteParams.Format( "%d, %d, %d", pt.x, pt.y, 0 );
		ExecuteAction(_T("PickColor"), strExecuteParams, 0);
		if(!m_strActionName.IsEmpty())
			ExecuteAction(m_strActionName, "", 0);
		return true;
	}
	//else
	//	return OnLButtonDown( pt );
	DoButtonDown(pt);
	return false;
}

bool CActionEditorBase::DoLButtonUp( CPoint pt )
{
	DoButtonUp(pt);
	return true;
}


bool CActionEditorBase::IsAvaible()
{
	if( !CheckInterface( m_punkTarget, IID_IImageView ) )
		return false;

	IUnknown* punk_img = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );
	if( punk_img )
	{
		punk_img->Release();	punk_img = 0;
		return true;
	}

	return false;
}

bool CActionEditorBase::IsChecked()
{
	return false;
}


bool CActionEditorBase::DoSetCursor( CPoint point )
{
	if((short)GetKeyState(VK_CONTROL) < 0  && !m_bWorkWithBinary)
	{
		if(m_pctrl) m_pctrl->Enable(false);
		m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_PICKCOLOR);		
		return true;
	}
	if(m_pctrl) m_pctrl->Enable(true);
	return false;
}

void RestoreFromPixArray(CTypedPtrArray<CPtrArray, FillInfo*> *pptrFillInfo, IUnknown* punkImage, CRect rcInvalid, bool bDeleteArray)
{
	//restore from m_ptrFillInfo & empty it if bDeleteArray
	color** pColor = 0;
	{
		if(rcInvalid == NORECT || !punkImage)
			return;
		CImageWrp image(punkImage);
		int nPanes = image.GetColorsCount();
		int i, j, k;
		FillInfo* pInfo = 0;
		FillInfo* pInfoOrig = 0;
		FillInfo* pInfoNext = 0;
		pColor = new color*[nPanes];
		rcInvalid.NormalizeRect();
		for(i = max(rcInvalid.top, 0); i < min(pptrFillInfo->GetSize(), rcInvalid.bottom); i++ )
		{
			pInfoOrig = pInfo = pptrFillInfo->GetAt(i);
			if( !pInfo )continue;
			if (pInfo->nCount != 0)
			{
				for(j = 0; j < nPanes; j++)
				{
					pColor[j] = image.GetRow(i, j);	//image row
				}
				while(pInfo)
				{
					for(k = 0; k < pInfo->nCount; k ++)
					{
						for(j = 0; j < nPanes; j++)	
						{
							pColor[j][pInfo->nX + k] = pInfo->pColors[j+ k*nPanes];// + k*nLineWidth*nPanes];
						}
					}

					pInfoNext = pInfo->pNext;
					if(bDeleteArray)
					{
						if (pInfo->pColors)
						{
							//delete pInfo->pColors;
							//pInfo->pColors = 0;
							pInfo->KillColors();
						}
						if (pInfoOrig != pInfo)
						{
							//delete pInfo;
							pInfo->Destroy();
							pInfo = 0;
						}
						else
						{
							pInfoOrig->nX = 0;
							pInfoOrig->nCount = 0;
							pInfoOrig->pNext = 0;
						}
					}
					pInfo = pInfoNext;
				}
			}
		}
		delete pColor;
	}
}

void AttachArrayToImage(CTypedPtrArray<CPtrArray, FillInfo*> *pptrFillInfo, IUnknown* punkImage,
	CRect rcInvalid, color* pColors, color* pBackColor, CImagePaneCache &ImagePaneCache)
{
	color** pColor = 0;
	{
		if(rcInvalid == NORECT || !punkImage)
			return;
		CImageWrp image(punkImage);
		int nPanes = image.GetColorsCount();
		int i, j, k;
		FillInfo* pInfo = 0;
		pColor = new color*[nPanes];
		rcInvalid.NormalizeRect();
		for(i = max(rcInvalid.top, 0); i < min(pptrFillInfo->GetSize(), rcInvalid.bottom); i++ )
		{
			pInfo = pptrFillInfo->GetAt(i);
			if( !pInfo )continue;
			if (pInfo->nCount != 0)
			{
				for(j = 0; j < nPanes; j++)
				{
					pColor[j] = image.GetRow(i, j);	//image row
				}
				while(pInfo)
				{
					for(k = 0; k < pInfo->nCount; k ++)
					{
						for(j = 0; j < nPanes; j++)	
						{
							if (ImagePaneCache.IsPaneEnable(j))
							pColor[j][pInfo->nX + k] = pInfo->bFill?pBackColor[j]:pColors[j];//pInfo->pColors[j+ k*nPanes];// + k*nLineWidth*nPanes];
						}
					}
					pInfo = pInfo->pNext;
				}
				
			}
		}
		delete pColor;
	}
}

void RestoreFromPixArrayBin(CTypedPtrArray<CPtrArray, FillInfo*> *pptrFillInfo, IUnknown* punkImage, CRect rcInvalid, bool bDeleteArray)
{
	//restore from m_ptrFillInfo & empty it if bDeleteArray
	color** pColor = 0;
	{
		if(rcInvalid == NORECT || !punkImage)
			return;
		CBinImageWrp binary(punkImage);

		int i, k;
		FillInfo* pInfo = 0;
		FillInfo* pInfoOrig = 0;
		FillInfo* pInfoNext = 0;
		rcInvalid.NormalizeRect();

		int nBinRowSize = 0;

		long nColorSize = sizeof(color)*8;
		byte* pbin = 0;

		for(i = max(rcInvalid.top, 0); i < min(pptrFillInfo->GetSize(), rcInvalid.bottom); i++ )
		{
			pInfoOrig = pInfo = pptrFillInfo->GetAt(i);
			if( !pInfo )continue;
			if (pInfo->nCount != 0)
			{
				binary->GetRow(&pbin, i, FALSE);

				while(pInfo)
				{
					long nX = pInfo->nX;
					for(k = 0; k < pInfo->nCount; k++, nX++)
					{
						/*if((pInfo->pColors[k/nColorSize] & (0x8000>>(k%nColorSize))) != 0)
							pbin[nX/8] |= 0x80>>(nX%8);
						else
							pbin[nX/8] &= (~(0x80>>( nX % 8 )));
              */
						// vk
						pbin[nX] = pInfo->pColors[k];
						//pbin[nX] = pInfo->pColors[k]|128;
						//pbin[nX] = pInfo->pColors[k]+128;
						
						//pbin[nX] = pInfo->pColors[k];
						// end vk
					}


					pInfoNext = pInfo->pNext;
					if(bDeleteArray)
					{
						if (pInfo->pColors)
						{
							pInfo->KillColors();
						}
						if (pInfoOrig != pInfo)
						{
							pInfo->Destroy();
							pInfo = 0;
						}
						else
						{
							pInfoOrig->nX = 0;
							pInfoOrig->nCount = 0;
							pInfoOrig->pNext = 0;
						}
					}
					pInfo = pInfoNext;
				}
			}
		}
	}
}

void AttachArrayToImageBin(CTypedPtrArray<CPtrArray, FillInfo*> *pptrFillInfo, IUnknown* punkImage, CRect rcInvalid, color* pColors, color* pBackColor)
{
	{
		if(rcInvalid == NORECT || !punkImage)
			return;
		CBinImageWrp binary(punkImage);
		int i, k;
		FillInfo* pInfo = 0;
		rcInvalid.NormalizeRect();

		int nBinRowSize = 0;
		byte* pbin = 0;

		for(i = max(rcInvalid.top, 0); i < min(pptrFillInfo->GetSize(), rcInvalid.bottom); i++ )
		{
			pInfo = pptrFillInfo->GetAt(i);
			if( !pInfo )continue;
			if (pInfo->nCount != 0)
			{
				binary->GetRow(&pbin, i, FALSE);

				while(pInfo)
				{
					long nX = pInfo->nX;
					for(k = 0; k < pInfo->nCount; k++, nX++)
					{

/**
						if( pColors[0] < 0xFF )
							pColors[0] = 0xFF;

						if( pColors[0] > 0xFF )
							pbin[nX] = (BYTE)-pColors[0];
						else
							pbin[nX] = 0xFF;
*/
						// vk
						pbin[nX] = pColors[0];
						//pbin[nX] = pColors[0]|128;
						
						//pbin[nX] = pColors[0];
						// end vk
						
						//pColors[0];
						/*if(pColors[0] == 0)
							pbi n[nX/8] &= (~(0x80>>( nX % 8 )));
						else
              pbin[nX/8] |= 0x80>>(nX%8);
              */
					}
					pInfo = pInfo->pNext;
				}
				
			}
		}
	}
}


void CActionEditorBase::_UpdateLinePreview()
{
	m_nLineWidth = ::GetValueInt(GetAppUnknown(), "Editor", "LineWidth", 1);
	m_nLineWidth2 = ::GetValueInt(GetAppUnknown(), "Editor", "LineWidth2", 1);

	if( m_pctrl )
	{
		m_pctrl->Uninstall();
		m_pctrl->GetControllingUnknown()->Release();
		m_pctrl = 0;
	}
	if (m_bUseWidth2)
		m_pctrl = new CBinLinePreviewCtrl(m_nLineWidth,m_nLineWidth2);
	else
	m_pctrl = new CLinePreviewCtrl(m_nLineWidth);
	m_pctrl->Install();
}


bool CActionEditorBase::DoUpdateSettings()
{
	m_nLineWidth = ::GetValueInt(GetAppUnknown(), "Editor", "LineWidth", 1);
	m_nLineWidth2 = ::GetValueInt(GetAppUnknown(), "Editor", "LineWidth2", 1);
	_UpdateLinePreview();
	return true;
}


void CActionEditorBase::UndoImage(bool bDeleteArray)
{
	CRect rcTmp = m_rectInvalid;
	m_rectInvalid.InflateRect(m_nLineWidth, m_nLineWidth, m_nLineWidth, m_nLineWidth);
	if(m_bWorkWithBinary)
		RestoreFromPixArrayBin(&m_ptrFillInfo, m_punkImage, m_rectInvalid, bDeleteArray);
	else
		RestoreFromPixArray(&m_ptrFillInfo, m_punkImage, m_rectInvalid, bDeleteArray);
	m_rectInvalid.OffsetRect(m_imgOffset);
	
	//paul 21.06.2003 BT 3272. Why only 1 view, need event fire on document
	//::FireEvent(m_punkTarget, szNeedUpdateView, m_punkTarget, GetControllingUnknown(), 0);	
	::UpdateDataObject( m_punkTarget, m_punkImage );

	m_rectInvalid = rcTmp;
}

bool CActionEditorBase::DoTerminate()
{
	UndoImage(true);
	if (CheckInterface(m_punkTarget, IID_IDocumentSite))
		m_changes.DoUndo(m_punkTarget);
	else if (CheckInterface(m_punkTarget, IID_IView))
	{
		IUnknownPtr	sptrD;
		sptrIView sptrV(m_punkTarget);
		sptrV->GetDocument(&sptrD);
		m_changes.DoUndo(sptrD);
	}
	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionPolyline implementation
CActionPolyline::CActionPolyline()
{
	m_strActionName = "EditPolyline";
	m_pcontour = ::ContourCreate();
	m_pctrl = 0;
	m_bCanTrackByRButton = true;
}

CActionPolyline::~CActionPolyline()
{
	::ContourDelete( m_pcontour );
}

void CActionPolyline::_add_point( CPoint &pt, bool bReplaceLast )
{
	if( !IsActive() )
	{
		m_rect.right = m_rect.left = pt.x;
		m_rect.bottom = m_rect.top = pt.y;
		m_rect_old = m_rect;
		_Begin();
		ContourAddPoint( m_pcontour, pt.x - m_imgOffset.x, pt.y - m_imgOffset.y, true );
	}
 	if( bReplaceLast )
	{
		m_rect = m_rect_old;
		m_rect.left = min( m_rect.left, pt.x );
		m_rect.top = min( m_rect.top, pt.y );
		m_rect.right = max( m_rect.right, pt.x+1 );
		m_rect.bottom = max( m_rect.bottom, pt.y+1 );
		m_pcontour->ppoints[m_pcontour->nContourSize-1].x = pt.x  - m_imgOffset.x;
		m_pcontour->ppoints[m_pcontour->nContourSize-1].y = pt.y  - m_imgOffset.y;
	}
	else
	{
		m_rect_old = m_rect;
		m_rect.left = min( m_rect.left, pt.x );
		m_rect.top = min( m_rect.top, pt.y );
		m_rect.right = max( m_rect.right, pt.x+1 );
		m_rect.bottom = max( m_rect.bottom, pt.y+1 );
		ContourAddPoint( m_pcontour, pt.x  - m_imgOffset.x , pt.y - m_imgOffset.y, true );
	}

	_DoDraw();
	_Draw();
}

bool CActionPolyline::DoMouseMove( CPoint pt )
{
	if( IsActive() )
		_add_point( pt, true );

	return CActionEditorBase::DoMouseMove( pt );

}

bool CActionPolyline::DoLButtonDown( CPoint pt )
{
	m_bUseBackColor = false;
	_add_point( pt, false );
	return CActionEditorBase::DoLButtonDown( pt );
}

bool CActionPolyline::DoRButtonDown( CPoint pt )
{
	m_bUseBackColor = true;
	_add_point( pt, false );
	return CActionEditorBase::DoRButtonDown( pt );
}

bool CActionPolyline::DoFinishTracking( CPoint pt )
{
	_add_point( pt, false );
	return CActionEditorBase::DoFinishTracking( pt );
}

bool CActionPolyline::DoLButtonDblClick( CPoint pt )
{
	if( !m_bUseBackColor )
	{
		_End();
		Finalize();
	}
	return true;
}

bool CActionPolyline::DoRButtonDblClick( CPoint pt )
{
	if( m_bUseBackColor )
	{
		_End();
		Finalize();
	}
	return true;
}

void CActionPolyline::_DoDraw()
{
	if(!m_bWorkWithBinary || m_bBinaryIsActive)
	{
		CRect rc(m_rect);
		rc.OffsetRect(-m_imgOffset);

		CDC	*pDC = m_pwfi->GetDC();
		CPen pen(PS_SOLID, m_nLineWidth, RGB(255, 255, 255));
		CPen* pOldPen = pDC->SelectObject(&pen);

		::ContourDraw( *pDC, m_pcontour, 0, 0 );

		pDC->SelectObject(pOldPen);
		
		rc.NormalizeRect();
		rc.InflateRect(m_nLineWidth, m_nLineWidth, m_nLineWidth, m_nLineWidth);
		
		RestoreImageFromChanges(m_rectInvalid);

		MoveFillInfoToImage(rc, m_pColors, m_pColors);

		m_rectInvalid = m_rect;
		m_rectInvalid.NormalizeRect();
		m_rectInvalid.OffsetRect(-m_imgOffset);
	}
}

bool CActionPolyline::Initialize()
{
	AddPropertyPageDescription( c_szCLine1PropPage );


	m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_POLYGON);

	if( !CActionEditorBase::Initialize() )
		return false;

	_UpdateLinePreview();

	return true;
}

void CActionPolyline::Finalize()
{
	CActionEditorBase::Finalize();
}

bool CActionPolyline::DoSetCursor( CPoint point )
{
	if(!CActionEditorBase::DoSetCursor(point))
	{
		m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDITLINE);
	}
	if( m_state == asActive && m_hcurActive )
	{
		::SetCursor( m_hcurActive );
		return true;
	}
	return CMouseImpl::DoSetCursor( point );
}
