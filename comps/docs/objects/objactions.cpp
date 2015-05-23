#include "stdafx.h"
#include "objactions.h"
#include "resource.h"
#include "math.h"
#include "NameConsts.h"
#include "objectlist.h"
#include "float.h"
#include "ObjectFilterDlg.h"
#include "ClassifierDlg.h"
#include "\vt5\common2\misc_calibr.h"
#include "\vt5\common2\class_utils.h"
#include "units_const.h"
#include "alloc.h"
#include "misc_utils.h"
#include "\vt5\awin\profiler.h"




//Kir --composite support
int get_parent_class(IUnknown* childUnk, IUnknown* listUnk)
{
	ICompositeObjectPtr co(listUnk);
	CTreeNotion* tn;
	co->GetTreeNotion((void**)&tn);
	CTreeNode* tnode = tn->Find(childUnk);
	CTreeNode* parent = tnode;
	while(parent)
	{
		parent = tnode->GetParent();
		if(parent)
		{
			tnode = parent;
		}
	}
	ICalcObjectPtr calc(tnode->GetData());
	
	return get_object_class(calc);
	
}

void UniqueName(IUnknown *punkDocData, CString& strName)
{
	while(true)
	{
		IUnknown* punkObj = FindObjectByName(punkDocData, strName);
		if(punkObj)
		{
			int nPos = strName.Find("_", 0);
			if(nPos == -1)
				strName += "_0";
			else
			{
				CString strNum = strName.Right(strName.GetLength() - nPos - 1);
				int nNum = atoi(strNum);
				nNum++;
				strNum.Format("%d", nNum);
				strName = strName.Left(nPos+1);
				strName += strNum;
			}
			punkObj->Release();
		}
		else
			break;
	}
}

void AddClassName(IUnknown *punkDocData, IUnknown* punkObj, CString strClassName)
{
	CString strName = ::GetObjectName(punkObj);

	strName += " ";
	strName += strClassName;

	UniqueName(punkDocData, strName);
	
	SetObjectName(punkObj, strName);
}

void CopyParams( ICalcObjectContainerPtr in, ICalcObjectContainerPtr out )
{
	if( in == 0 || out == 0)
		return;

	LONG_PTR lParamPos = 0;
	in->GetFirstParameterPos( &lParamPos );
	while( lParamPos )
	{
		ParameterContainer	*pc;
		in->GetNextParameter( &lParamPos, &pc );
		out->DefineParameterFull( pc, 0 );
	}
}

void DrawWfiRect( CWalkFillInfo *pwfi, BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, RECT rectDest, double fZoom, POINT ptScroll, DWORD dwFillColor )
{
	if( !pwfi )return;
	int		cxImg, cyImg;
	CPoint	pointImageOffest;
	cxImg = pwfi->GetRect().Width();
	cyImg = pwfi->GetRect().Height();
	pointImageOffest = pwfi->GetRect().TopLeft();

//	if( dwFlags & cidrNoOffset )
//		pointImageOffest = CPoint( 0, 0 );

	int		x, y, xImg, yImg, yBmp;
	byte	fillR = GetRValue(dwFillColor);
	byte	fillG = GetGValue(dwFillColor);
	byte	fillB = GetBValue(dwFillColor);
	byte	*p, *pmask;
	int		cx4Bmp = (pbi->biWidth*3+3)/4*4;
	int	xBmpStart = max( int(ceil(( pointImageOffest.x)*fZoom-ptScroll.x)), rectDest.left );
	int	xBmpEnd = min( int(floor((pointImageOffest.x+cxImg)*fZoom-ptScroll.x+.5)), rectDest.right );
	int	yBmpStart = max( int(ceil((pointImageOffest.y)*fZoom-ptScroll.y)), rectDest.top );
	int	yBmpEnd = min( int(floor((pointImageOffest.y+cyImg)*fZoom-ptScroll.y+.5)), rectDest.bottom );
	int	yImgOld = -1;
	int	t, cx = max(xBmpEnd-xBmpStart, 0), cy = max(yBmpEnd-yBmpStart, 0);
	int	*pxofs = new int[cx];
	int	*pyofs = new int[cy];

	for( t = 0; t < xBmpEnd-xBmpStart; t++ )
		pxofs[t] = int((t+xBmpStart+ptScroll.x)/fZoom-pointImageOffest.x);
	for( t = 0; t < yBmpEnd-yBmpStart; t++ )
		pyofs[t] = int((t+yBmpStart+ptScroll.y)/fZoom-pointImageOffest.y);
																											
	for( y = yBmpStart, yBmp = yBmpStart; y < yBmpEnd; y++, yBmp++ )
	{
		/*get the pointer to the BGR struct*/																
		p = pdibBits+(pbi->biHeight-yBmp-1+ptBmpOfs.y)*cx4Bmp+(xBmpStart-ptBmpOfs.x)*3;
		/*get the y coord of source image*/																	
		yImg =pyofs[y-yBmpStart];
		if( yImgOld != yImg )
		{
			pmask = pwfi->GetRowMask( yImg );
			yImgOld = yImg;
		}
		for( x = xBmpStart; x < xBmpEnd; x++ )
		{
			/*get the x coord of source image*/
			xImg = pxofs[x-xBmpStart];
			if( !pmask[xImg] )
			{
				p++;
				p++;
				p++;
			}
			else
			{
				*p = fillB; p++;
				*p = fillG; p++;
				*p = fillR; p++;
			}
		}
	}

	delete pxofs;
	delete pyofs;
}

class CEditorCtrl: public CDrawControllerBase
{
public:
	CEditorCtrl()
	{
		m_nR = 0;
		m_nRBack = 0;
	}
	void DoDraw( CDC &dc, CPoint point, IUnknown *punkVS,  bool bErase )
	{
		CRect	rect;

		rect.left = point.x-m_nR/2;
		rect.top = point.y-m_nR/2;
		rect.right = rect.left + m_nR;
		rect.bottom = rect.top + m_nR;

		rect = ::ConvertToWindow( punkVS, rect );
		// A.M. fix SBT1513. In some computers CDC::Ellipse works bad
		// with ellipses 2x2 (videoboard? driver? Windows version?).
		if (rect.Width() >= 3 && rect.Height() >= 3)
		dc.Ellipse( rect );

		if( m_nRBack != m_nR )
		{
			rect.left = point.x-m_nRBack/2;
			rect.top = point.y-m_nRBack/2;
			rect.right = rect.left + m_nRBack;
			rect.bottom = rect.top + m_nRBack;

			rect = ::ConvertToWindow( punkVS, rect );
			if (rect.Width() >= 3 && rect.Height() >= 3)
			dc.Ellipse( rect );
		}
	}
	void SetSizes( int nR, int nRBack )
	{
		m_nR = nR;
		m_nRBack = nRBack;
	}
protected:
	int	m_nR, m_nRBack;
};


class CRectCtrl: public CDrawControllerBase
{
public:
	void DoDraw( CDC &dc, CPoint point, IUnknown *punkVS,  bool bErase )
	{
		sptrIScrollZoomSite	sptrS( punkVS );
		CSize	size;

		sptrS->GetClientSize( &size );

		CPoint	point1h( 0, point.y );
		CPoint	point2h( size.cx, point.y );
		CPoint	point1v( point.x, 0 );
		CPoint	point2v( point.x, size.cy );

		point1h = ::ConvertToWindow( punkVS, point1h );
		point2h = ::ConvertToWindow( punkVS, point2h );
		point1v = ::ConvertToWindow( punkVS, point1v );
		point2v = ::ConvertToWindow( punkVS, point2v );

		dc.MoveTo( point1h );
		dc.LineTo( point2h );
		dc.MoveTo( point1v );
		dc.LineTo( point2v );
		
	}
};

class CFixedRectCtrl: public CDrawControllerBase
{
public:
	CFixedRectCtrl()
	{
		m_size.cx = 0;
		m_size.cy = 0;
	}
	void DoDraw( CDC &dc, CPoint point, IUnknown *punkVS,  bool bErase )
	{
		CRect	rect;

		rect.left = point.x-m_size.cx/2;
		rect.top = point.y-m_size.cy/2;
		rect.right = rect.left + m_size.cx;
		rect.bottom = rect.top + m_size.cy;

		rect = ::ConvertToWindow( punkVS, rect );
		dc.Rectangle( rect );
		
	}
	void SetSize( CSize	size )
	{
		m_size = size;
	}
protected:
	CSize	m_size;
};

class CFixedEllipseCtrl : public CFixedRectCtrl
{
	void DoDraw( CDC &dc, CPoint point, IUnknown *punkVS,  bool bErase )
	{
		CRect	rect;

		rect.left = point.x-m_size.cx/2;
		rect.top = point.y-m_size.cy/2;
		rect.right = rect.left + m_size.cx;
		rect.bottom = rect.top + m_size.cy;

		rect = ::ConvertToWindow( punkVS, rect );
		dc.Ellipse( rect );
	}
};



/*class CActionsDrawController : public CDrawControllerBase
{
	void DoDraw( CDC &dc, double fZpoom
};*/

//common defines
const char szFixedObjectSizes[] = _T("ObjectEditor");
const char szEllipseCX[] = _T("EllipseCX");
const char szEllipseCY[] = _T("EllipseCY");
const char szRectCX[] = _T("RectCX");
const char szRectCY[] = _T("RectCY");
const char szMagic[] = _T("ObjectEditor");
const char szMagicInt[] = _T("MagickInt");
const char szMagicFillHoles[] = _T("MagicFillHoles");
const char szDynamicBrightness[] = _T("DynamicBrightness");
const char szUseCalubration[] =  _T("UseCalibration");


//calc the point on the line nearest to specified
CPoint	CalcNearestLocation( CPoint point1, CPoint point2, CPoint pointN, bool bInsideInterval = false )
{
	if( point1 == point2 )
		return point1;

	double x1 = point1.x;
	double y1 = point1.y;
	double x2 = point2.x;
	double y2 = point2.y;

	double xc = pointN.x;
	double yc = pointN.y;

	double	a1 = y1-y2;
	double	b1 = x2-x1;
	double	c1 = -a1*x1-b1*y1;

	double	a2 = b1;
	double	b2 = -a1;
	double	c2 = -a2*xc-b2*yc;

	double	d = a1*b2-a2*b1;
	double	dx = -c1*b2+c2*b1;
	double	dy = -a1*c2+a2*c1;

	if( fabs( d ) < 0.000001 )
		return point1;

	ASSERT( d != 0 );

	double	xp = dx/d;
	double	yp = dy/d;

	if( bInsideInterval )
	{
		if( xp < min( x1, x2 ) )xp = min( x1, x2 );
		if( xp > max( x1, x2 ) )xp = max( x1, x2 );
		if( yp < min( y1, y2 ) )yp = min( y1, y2 );
		if( yp > max( y1, y2 ) )yp = max( y1, y2 );
	}

	return CPoint( int( xp+.5 ), int( yp+.5 ) );
}

//calc the distance between pointe
double CalcLen( double dx, double dy )
{
	return ::sqrt( dx*dx+dy*dy );
}


//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionObjectClassifyByLimits, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectFilter, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectScissors, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAutoAssignClasses, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectsSetClass, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectsMerge, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectsRemove, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectsSplit, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectsMakeLight, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectMultiLine, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectLine, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRecalcOnChangeCalibr, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectEditor, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectEditorSpec, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectEllipseDirect, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectEllipseFixed, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectRectDirect, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectRectFixed, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectFreeLine, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectSpline, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectEllipse, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectRect, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectMagickStick, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionCalcMeasResult,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionJoinAllObjects , CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSetListZOrder, CCmdTargetEx);

// olecreate

// {CD499C02-141A-4605-9942-5649F44FB01A}
GUARD_IMPLEMENT_OLECREATE(CActionObjectClassifyByLimits, "FiltersMain.ObjectClassifyByLimits", 
0xcd499c02, 0x141a, 0x4605, 0x99, 0x42, 0x56, 0x49, 0xf4, 0x4f, 0xb0, 0x1a);
// {C329DFA2-1C28-4c0a-B04F-4B527594DC0A}
GUARD_IMPLEMENT_OLECREATE(CActionObjectFilter, "FiltersMain.ObjectFilter", 
0xc329dfa2, 0x1c28, 0x4c0a, 0xb0, 0x4f, 0x4b, 0x52, 0x75, 0x94, 0xdc, 0xa);
// {13C0376D-7AE5-4066-975B-0C1773A841A3}
GUARD_IMPLEMENT_OLECREATE(CActionObjectScissors, "FiltersMain.ObjectScissors", 
0x13c0376d, 0x7ae5, 0x4066, 0x97, 0x5b, 0xc, 0x17, 0x73, 0xa8, 0x41, 0xa3);
// {1298134C-85D0-4f44-ACBE-C2BD0C4649EB}
GUARD_IMPLEMENT_OLECREATE(CActionAutoAssignClasses, "FiltersMain.AutoAssignClasses", 
0x1298134c, 0x85d0, 0x4f44, 0xac, 0xbe, 0xc2, 0xbd, 0xc, 0x46, 0x49, 0xeb);
// {70BD44BE-3DB6-4eda-88BD-EE65F69DC304}
GUARD_IMPLEMENT_OLECREATE(CActionObjectsRemove, "FiltersMain.ObjectsRemove", 
0x70bd44be, 0x3db6, 0x4eda, 0x88, 0xbd, 0xee, 0x65, 0xf6, 0x9d, 0xc3, 0x4);
// {8A116847-6D7F-4188-A193-D6D022FBEF28}
GUARD_IMPLEMENT_OLECREATE(CActionObjectsMerge, "FiltersMain.ObjectsMerge", 
0x8a116847, 0x6d7f, 0x4188, 0xa1, 0x93, 0xd6, 0xd0, 0x22, 0xfb, 0xef, 0x28);
// {A87ABBCA-2083-479d-B5DC-00E87CFD72EC}
GUARD_IMPLEMENT_OLECREATE(CActionObjectsSetClass, "FiltersMain.ObjectsSetClass",
0xa87abbca, 0x2083, 0x479d, 0xb5, 0xdc, 0x0, 0xe8, 0x7c, 0xfd, 0x72, 0xec);
// {6E2AD88F-A142-4900-B551-EAD8F8BD6215}
GUARD_IMPLEMENT_OLECREATE(CActionObjectsSplit, "FiltersMain.ObjectsSplit",
0x6e2ad88f, 0xa142, 0x4900, 0xb5, 0x51, 0xea, 0xd8, 0xf8, 0xbd, 0x62, 0x15);
// {56D49B05-5BCD-4752-AFA8-801C7B0FDBE8}
GUARD_IMPLEMENT_OLECREATE(CActionObjectsMakeLight, "FiltersMain.ObjectsMakeLight",
0x56d49b05, 0x5bcd, 0x4752, 0xaf, 0xa8, 0x80, 0x1c, 0x7b, 0xf, 0xdb, 0xe8);
// {AF925AEB-BB1C-41c7-8CFE-99937D37A4AE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectMultiLine, "Objects.ObjectMultiLine",
0xaf925aeb, 0xbb1c, 0x41c7, 0x8c, 0xfe, 0x99, 0x93, 0x7d, 0x37, 0xa4, 0xae);
// {649B243D-62AC-4b1a-AF31-A6BCCC544B86}
GUARD_IMPLEMENT_OLECREATE(CActionObjectLine, "Objects.ObjectLine",
0x649b243d, 0x62ac, 0x4b1a, 0xaf, 0x31, 0xa6, 0xbc, 0xcc, 0x54, 0x4b, 0x86);
// {0260662C-9D16-4bfb-B0CA-1D2DD4760149}
GUARD_IMPLEMENT_OLECREATE(CActionRecalcOnChangeCalibr, "objects.RecalcOnChangeCalibr",
0x260662c, 0x9d16, 0x4bfb, 0xb0, 0xca, 0x1d, 0x2d, 0xd4, 0x76, 0x1, 0x49);
// {DDC34946-9CEA-439a-A9C8-479F6550950E}
GUARD_IMPLEMENT_OLECREATE(CActionObjectEditor, "Objects.ObjectEditor",
0xddc34946, 0x9cea, 0x439a, 0xa9, 0xc8, 0x47, 0x9f, 0x65, 0x50, 0x95, 0xe);
// {8E9F9ACF-AB11-478c-92A1-A3CBD6A7C216}
GUARD_IMPLEMENT_OLECREATE(CActionObjectEditorSpec, "Objects.ObjectEditorSpec",
0x8e9f9acf, 0xab11, 0x478c, 0x92, 0xa1, 0xa3, 0xcb, 0xd6, 0xa7, 0xc2, 0x16);
// {2BB2547F-9CC0-11d3-A69E-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectEllipseDirect, "objects.ObjectEllipseDirect",
0x2bb2547f, 0x9cc0, 0x11d3, 0xa6, 0x9e, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {2BB2547B-9CC0-11d3-A69E-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectEllipseFixed, "objects.ObjectEllipseFixed",
0x2bb2547b, 0x9cc0, 0x11d3, 0xa6, 0x9e, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {2BB25477-9CC0-11d3-A69E-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectRectDirect, "objects.ObjectRectDirect",
0x2bb25477, 0x9cc0, 0x11d3, 0xa6, 0x9e, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {2BB25473-9CC0-11d3-A69E-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectRectFixed, "objects.ObjectRectFixed",
0x2bb25473, 0x9cc0, 0x11d3, 0xa6, 0x9e, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {C79B0E7F-9353-11d3-A690-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectFreeLine, "objects.ObjectFreeLine",
0xc79b0e7f, 0x9353, 0x11d3, 0xa6, 0x90, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {C79B0E7B-9353-11d3-A690-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectSpline, "objects.ObjectSpline",
0xc79b0e7b, 0x9353, 0x11d3, 0xa6, 0x90, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {C79B0E73-9353-11d3-A690-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectEllipse, "objects.ObjectEllipse",
0xc79b0e73, 0x9353, 0x11d3, 0xa6, 0x90, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {3143AC33-8C66-11d3-A687-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectRect, "objects.ObjectRect",
0x3143ac33, 0x8c66, 0x11d3, 0xa6, 0x87, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {93E8CBE3-8C41-11d3-A686-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectMagickStick, "objects.ObjectMagickStick",
0x93e8cbe3, 0x8c41, 0x11d3, 0xa6, 0x86, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {CC70EBDB-F359-4027-95F5-6AC9BC05440E}
GUARD_IMPLEMENT_OLECREATE(CActionCalcMeasResult, "Classes.CalcMeasResult",
0xcc70ebdb, 0xf359, 0x4027, 0x95, 0xf5, 0x6a, 0xc9, 0xbc, 0x5, 0x44, 0xe);
// {FFF96CE3-96BD-435b-9505-3F98F66C9381}
GUARD_IMPLEMENT_OLECREATE(CActionJoinAllObjects, "Objects.JoinAllObjects", 
0xfff96ce3, 0x96bd, 0x435b, 0x95, 0x5, 0x3f, 0x98, 0xf6, 0x6c, 0x93, 0x81);
// {C76FBD43-9BA2-4c4b-8E53-B5CFAE710029}
GUARD_IMPLEMENT_OLECREATE(CActionSetListZOrder, "Objects.SetListZOrder", 
0xc76fbd43, 0x9ba2, 0x4c4b, 0x8e, 0x53, 0xb5, 0xcf, 0xae, 0x71, 0x0, 0x29);

// Action info

// {330BFBA8-9E18-4aac-95A4-7D587F126C6D}
static const GUID guidObjectClassifyByLimits = 
{ 0x330bfba8, 0x9e18, 0x4aac, { 0x95, 0xa4, 0x7d, 0x58, 0x7f, 0x12, 0x6c, 0x6d } };
// {70641B6A-6879-4a5a-906B-6CBDFD8F612E}
static const GUID guidObjectFilter = 
{ 0x70641b6a, 0x6879, 0x4a5a, { 0x90, 0x6b, 0x6c, 0xbd, 0xfd, 0x8f, 0x61, 0x2e } };
// {0677C3DA-94DE-46f0-98CD-68265EA95150}
static const GUID guidObjectScissors = 
{ 0x677c3da, 0x94de, 0x46f0, { 0x98, 0xcd, 0x68, 0x26, 0x5e, 0xa9, 0x51, 0x50 } };
// {4C582237-E54B-4542-9611-FDBED9B89B35}
static const GUID guidAutoAssignClasses = 
{ 0x4c582237, 0xe54b, 0x4542, { 0x96, 0x11, 0xfd, 0xbe, 0xd9, 0xb8, 0x9b, 0x35 } };
// {3C02B4ED-6D4E-4cfa-B222-47FE30BBEB8D}
static const GUID guidObjectsRemove = 
{ 0x3c02b4ed, 0x6d4e, 0x4cfa, { 0xb2, 0x22, 0x47, 0xfe, 0x30, 0xbb, 0xeb, 0x8d } };
// {2ABC1EB4-2C04-436b-A750-5BEA3DFA8132}
static const GUID guidObjectsMerge = 
{ 0x2abc1eb4, 0x2c04, 0x436b, { 0xa7, 0x50, 0x5b, 0xea, 0x3d, 0xfa, 0x81, 0x32 } };
// {FBC04A96-DDC8-4075-ACD7-BDED20799C6C}
static const GUID guidObjectsSetClass = 
{ 0xfbc04a96, 0xddc8, 0x4075, { 0xac, 0xd7, 0xbd, 0xed, 0x20, 0x79, 0x9c, 0x6c } };
// {657550AC-F0B6-4584-B837-D66042CA4F32}
static const GUID guidObjectsSplit =
{ 0x657550ac, 0xf0b6, 0x4584, { 0xb8, 0x37, 0xd6, 0x60, 0x42, 0xca, 0x4f, 0x32 } };
// {EE6794CB-0726-4838-AB80-765EA346381F}
static const GUID guidObjectsMakeLight =
{ 0xee6794cb, 0x726, 0x4838, { 0xab, 0x80, 0x76, 0x5e, 0xa3, 0x46, 0x38, 0x1f } };
// {DCB0A94E-7F4C-4ec2-9DA8-093480CDD660}
static const GUID guidObjectMultiLine =
{ 0xdcb0a94e, 0x7f4c, 0x4ec2, { 0x9d, 0xa8, 0x9, 0x34, 0x80, 0xcd, 0xd6, 0x60 } };
// {146A76CC-B288-4a44-99F1-5D231BCFCC49}
static const GUID guidObjectLine =
{ 0x146a76cc, 0xb288, 0x4a44, { 0x99, 0xf1, 0x5d, 0x23, 0x1b, 0xcf, 0xcc, 0x49 } };
// {5B9250BB-FA05-4b0d-8EB1-A67A76021675}
static const GUID guidRecalcOnChangeCalibr =
{ 0x5b9250bb, 0xfa05, 0x4b0d, { 0x8e, 0xb1, 0xa6, 0x7a, 0x76, 0x2, 0x16, 0x75 } };
// {4D192295-C1F2-4154-8BCC-6C14C2F071DF}
static const GUID guidObjectEditor =
{ 0x4d192295, 0xc1f2, 0x4154, { 0x8b, 0xcc, 0x6c, 0x14, 0xc2, 0xf0, 0x71, 0xdf } };
// {F756D99F-2DEA-4645-9CA1-D3B3CD394F59}
static const GUID guidObjectEditorSpec = 
{ 0xf756d99f, 0x2dea, 0x4645, { 0x9c, 0xa1, 0xd3, 0xb3, 0xcd, 0x39, 0x4f, 0x59 } };
// {2BB2547D-9CC0-11d3-A69E-0090275995FE}
static const GUID guidObjectEllipseDirect =
{ 0x2bb2547d, 0x9cc0, 0x11d3, { 0xa6, 0x9e, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {2BB25479-9CC0-11d3-A69E-0090275995FE}
static const GUID guidObjectEllipseFixed =
{ 0x2bb25479, 0x9cc0, 0x11d3, { 0xa6, 0x9e, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {2BB25475-9CC0-11d3-A69E-0090275995FE}
static const GUID guidObjectRectDirect =
{ 0x2bb25475, 0x9cc0, 0x11d3, { 0xa6, 0x9e, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {2BB25471-9CC0-11d3-A69E-0090275995FE}
static const GUID guidObjectRectFixed =
{ 0x2bb25471, 0x9cc0, 0x11d3, { 0xa6, 0x9e, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {C79B0E7D-9353-11d3-A690-0090275995FE}
static const GUID guidObjectFreeLine =
{ 0xc79b0e7d, 0x9353, 0x11d3, { 0xa6, 0x90, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {C79B0E79-9353-11d3-A690-0090275995FE}
static const GUID guidObjectSpline =
{ 0xc79b0e79, 0x9353, 0x11d3, { 0xa6, 0x90, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {C79B0E71-9353-11d3-A690-0090275995FE}
static const GUID guidObjectEllipse =
{ 0xc79b0e71, 0x9353, 0x11d3, { 0xa6, 0x90, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {3143AC31-8C66-11d3-A687-0090275995FE}
static const GUID guidObjectRect =
{ 0x3143ac31, 0x8c66, 0x11d3, { 0xa6, 0x87, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {93E8CBE1-8C41-11d3-A686-0090275995FE}
static const GUID guidObjectMagickStick =
{ 0x93e8cbe1, 0x8c41, 0x11d3, { 0xa6, 0x86, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {9293137A-6C7A-4aaf-99CD-5765DDB6FCCD}
static const GUID guidCalcMeasResult =
{ 0x9293137a, 0x6c7a, 0x4aaf, { 0x99, 0xcd, 0x57, 0x65, 0xdd, 0xb6, 0xfc, 0xcd } };
// {83BC2209-F211-4291-9F53-765876DF9BF6}
static const GUID guidJoinAllObjects = 
{ 0x83bc2209, 0xf211, 0x4291, { 0x9f, 0x53, 0x76, 0x58, 0x76, 0xdf, 0x9b, 0xf6 } };
// {B56B629C-420E-44d8-949B-D18C03FFB4F4}
static const GUID guidSetListZOrder = 
{ 0xb56b629c, 0x420e, 0x44d8, { 0x94, 0x9b, 0xd1, 0x8c, 0x3, 0xff, 0xb4, 0xf4 } };




//[ag]6. action info
ACTION_INFO_FULL(CActionObjectClassifyByLimits, IDS_ACTION_OBJECT_CLASSIFY_BY_LIMITS, -1, -1, guidObjectClassifyByLimits);
ACTION_INFO_FULL(CActionObjectFilter, IDS_ACTION_OBJECT_FILTER, -1, -1, guidObjectFilter);
ACTION_INFO_FULL(CActionObjectScissors, IDS_ACTION_OBJECT_SCISSORS, -1, -1, guidObjectScissors);
ACTION_INFO_FULL(CActionAutoAssignClasses, IDS_ACTION_AUTOASSIGNCLASSES, -1, -1, guidAutoAssignClasses);
ACTION_INFO_FULL(CActionObjectsRemove, IDS_ACTION_REMOVE, -1, -1, guidObjectsRemove);
ACTION_INFO_FULL(CActionObjectsMerge, IDS_ACTION_MERGE, -1, -1, guidObjectsMerge);
ACTION_INFO_FULL(CActionObjectsSetClass, IDS_ACTION_SETCLASS, -1, -1, guidObjectsSetClass);
ACTION_INFO_FULL(CActionObjectsSplit, IDS_ACTION_SPLIT, -1, -1, guidObjectsSplit);
ACTION_INFO_FULL(CActionObjectsMakeLight, IDS_ACTION_MAKE_LIGHT, -1, -1, guidObjectsMakeLight);
ACTION_INFO_FULL(CActionObjectMultiLine, IDS_ACTION_OBJECTPOLYLINE, -1, -1, guidObjectMultiLine);
ACTION_INFO_FULL(CActionObjectLine, IDS_ACTION_OBJECTLINE, -1, -1, guidObjectLine);
ACTION_INFO_FULL(CActionObjectEditor, IDS_ACTION_OBJECT_EDITOR, -1, -1, guidObjectEditor);
ACTION_INFO_FULL(CActionObjectEditorSpec, IDS_ACTION_OBJECT_EDITOR_SPEC, -1, -1, guidObjectEditorSpec);
ACTION_INFO_FULL(CActionObjectEllipseDirect, IDS_ACTION_OBJECTELLIPSE_DIRECT, -1, -1, guidObjectEllipseDirect);
ACTION_INFO_FULL(CActionObjectEllipseFixed, IDS_ACTION_OBJECTELLIPSE_FIXED, -1, -1, guidObjectEllipseFixed);
ACTION_INFO_FULL(CActionObjectRectDirect, IDS_ACTION_OBJECTRECT_DIRECT, -1, -1, guidObjectRectDirect);
ACTION_INFO_FULL(CActionObjectRectFixed, IDS_ACTION_OBJECTRECT_FIXED, -1, -1, guidObjectRectFixed);
ACTION_INFO_FULL(CActionObjectFreeLine, IDS_ACTION_OBJECTFREE, -1, -1, guidObjectFreeLine);
ACTION_INFO_FULL(CActionObjectSpline, IDS_ACTION_OBJECTSPLINE, -1, -1, guidObjectSpline);
ACTION_INFO_FULL(CActionObjectEllipse, IDS_ACTION_OBJECTELLIPSE, -1, -1, guidObjectEllipse);
ACTION_INFO_FULL(CActionObjectRect, IDS_ACTION_OBJECTRECT, -1, -1, guidObjectRect);
ACTION_INFO_FULL(CActionObjectMagickStick, IDS_ACTION_OBJECTMAGICKSTICK, -1, -1, guidObjectMagickStick);
ACTION_INFO_FULL(CActionCalcMeasResult, IDS_ACTION_CALCMEASRESULT, -1, -1, guidCalcMeasResult);
ACTION_INFO_FULL(CActionRecalcOnChangeCalibr, IDS_ACTION_RECALC_ON_CHANGE_CALIBR, -1, -1, guidRecalcOnChangeCalibr);
ACTION_INFO_FULL(CActionJoinAllObjects , IDS_ACTION_JOIN_ALL_OBJECTS, -1, -1, guidJoinAllObjects);
ACTION_INFO_FULL(CActionSetListZOrder, IDS_ACTION_SET_Z_ORDER_FOR_LIST, -1, -1, guidSetListZOrder);

//[ag]7. targets


ACTION_TARGET(CActionObjectClassifyByLimits, szTargetAnydoc);
ACTION_TARGET(CActionObjectFilter, szTargetAnydoc);
ACTION_TARGET(CActionObjectScissors, szTargetViewSite);
ACTION_TARGET(CActionAutoAssignClasses, szTargetAnydoc);
ACTION_TARGET(CActionObjectsRemove, szTargetAnydoc);
ACTION_TARGET(CActionObjectsMerge, szTargetAnydoc);
ACTION_TARGET(CActionObjectsSetClass, szTargetAnydoc);
ACTION_TARGET(CActionObjectsSplit, szTargetAnydoc);
ACTION_TARGET(CActionObjectsMakeLight, szTargetAnydoc);
ACTION_TARGET(CActionObjectMultiLine, szTargetViewSite);
ACTION_TARGET(CActionObjectLine, szTargetViewSite);
ACTION_TARGET(CActionObjectEditor, szTargetViewSite);
ACTION_TARGET(CActionObjectEditorSpec, szTargetViewSite);
ACTION_TARGET(CActionObjectEllipseDirect, szTargetViewSite);
ACTION_TARGET(CActionObjectEllipseFixed, szTargetViewSite);
ACTION_TARGET(CActionObjectRectDirect, szTargetViewSite);
ACTION_TARGET(CActionObjectRectFixed, szTargetViewSite);
ACTION_TARGET(CActionObjectFreeLine, szTargetViewSite);
ACTION_TARGET(CActionObjectSpline, szTargetViewSite);
ACTION_TARGET(CActionObjectEllipse, szTargetViewSite);
ACTION_TARGET(CActionObjectRect, szTargetViewSite);
ACTION_TARGET(CActionObjectMagickStick, szTargetViewSite);
ACTION_TARGET(CActionCalcMeasResult,	szTargetAnydoc);
ACTION_TARGET(CActionRecalcOnChangeCalibr, szTargetAnydoc);
ACTION_TARGET(CActionJoinAllObjects, szTargetAnydoc); //!!!
ACTION_TARGET(CActionSetListZOrder, szTargetAnydoc);

//[ag]8. arguments

ACTION_ARG_LIST(CActionObjectRect)
	ARG_INT(_T("Left"), 0)
	ARG_INT(_T("Top"), 0)
	ARG_INT(_T("Right"), 0)
	ARG_INT(_T("Bottom"), 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectEllipse)
	ARG_INT(_T("X"), -1)
	ARG_INT(_T("Y"), -1)
	ARG_INT(_T("R"), -1)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectEllipseDirect)
	ARG_INT(_T("X1"), -1)
	ARG_INT(_T("Y1"), -1)
	ARG_INT(_T("X2"), -1)
	ARG_INT(_T("Y2"), -1)
	ARG_INT(_T("Width"), -1)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectRectDirect)
	ARG_INT(_T("X1"), -1)
	ARG_INT(_T("Y1"), -1)
	ARG_INT(_T("X2"), -1)
	ARG_INT(_T("Y2"), -1)
	ARG_INT(_T("Width"), -1)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectEllipseFixed)
	ARG_DOUBLE(_T("X"), -1)
	ARG_DOUBLE(_T("Y"), -1)
	ARG_DOUBLE(_T("Width"), 0)
	ARG_DOUBLE(_T("Height"), 0)
	ARG_INT(_T("UseCalibration"), 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectRectFixed)
	ARG_DOUBLE(_T("X"), -1)
	ARG_DOUBLE(_T("Y"), -1)
	ARG_DOUBLE(_T("Width"), 0)
	ARG_DOUBLE(_T("Height"), 0)
	ARG_INT(_T("UseCalibration"), 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionRecalcOnChangeCalibr)
	ARG_STRING(_T("ObjList"), 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectsMakeLight)
	ARG_OBJECT(_T("MeasObjects"))
	RES_OBJECT(_T("MeasObjects light"))
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionObjectsSplit)
	ARG_OBJECT(_T("MeasObjects"))
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionObjectsRemove)
	ARG_INT(_T("Class"), -2)
	ARG_OBJECT(_T("MeasObjects"))
	RES_OBJECT(_T("ObjectsRemove"))
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionObjectsMerge)
	ARG_OBJECT(_T("MeasObjects1"))
	ARG_OBJECT(_T("MeasObjects2"))
	RES_OBJECT(_T("ObjectsMerge"))
	ARG_INT(_T("VerifyType"), 0)
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionObjectsSetClass)
	ARG_STRING(_T("Class"), 0)
	ARG_OBJECT(_T("MeasObjects"))
	RES_OBJECT(_T("ObjectsSetClass"))
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionAutoAssignClasses)
	ARG_OBJECT(_T("MeasObjects"))
	RES_OBJECT(_T("AutoAssignClasses"))
END_ACTION_ARG_LIST();


ACTION_ARG_LIST(CActionObjectFilter)
	ARG_STRING(_T("SectionName"), "Objects\\ObjectFilter")
	ARG_INT(_T("ShowInterface"), 1)
	ARG_OBJECT(_T("MeasObjects"))
	RES_OBJECT(_T("ObjectFilter"))
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionObjectClassifyByLimits)
	ARG_STRING(_T("SectionName"), "Objects\\ObjectLimitsClassifier")
	ARG_INT(_T("ShowInterface"), 1)
	ARG_OBJECT(_T("MeasObjects"))
	RES_OBJECT(_T("ObjectClassifier"))
END_ACTION_ARG_LIST();


ACTION_ARG_LIST(CActionCalcMeasResult)
	ARG_STRING("ObjectList", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionJoinAllObjects)
	ARG_INT( _T("UseClasses"), 0 )
	ARG_STRING( _T("ClassifierName"), 0 )
	ARG_INT( _T("InitContours"), 0 )
	ARG_OBJECT( _T("MeasObjects") )
	RES_OBJECT( _T("TotalObject") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionSetListZOrder)
	ARG_INT( _T("ZOrder"), 0 )
	ARG_INT( _T("UsedClass"), -1 )
	ARG_STRING( _T("ClassifierName"), 0 )
	ARG_STRING( _T("ListName"), 0 )
END_ACTION_ARG_LIST();




//[ag]9. implementation
//////////////////////////////////////////////////////////////////////
//CActionObjectClassifyByLimits implementation
CActionObjectClassifyByLimits::CActionObjectClassifyByLimits()
{
}

CActionObjectClassifyByLimits::~CActionObjectClassifyByLimits()
{
}

bool CActionObjectClassifyByLimits::InvokeFilter()
{
	CObjectListWrp objectsIn(GetDataArgument());
	CObjectListWrp objectsOut(GetDataResult());
	CString strSectionName = GetArgumentString(_T("SectionName"));
	bool bShowInterface = GetArgumentInt(_T("ShowInterface")) == 1;

	long nCountIn = objectsIn.GetCount();
	if(!nCountIn)
		return false;

	
	if(bShowInterface)
	{
		CClassifierDlg dlg(NULL, strSectionName);
		dlg.DoModal();
	}
	
	INamedDataPtr	sptrData(GetAppUnknown());
	sptrData->SetupSection(_bstr_t(strSectionName));
	long nEntries = 0;
	sptrData->GetEntriesCount(&nEntries);
	if(nEntries <= 0)
		return true;
	
	//CStringArray arrClassesGuids;
	CArray<IUnknown*, IUnknown*&> arrClasses;
	long i = 0;
	for(i = 0; i < nEntries; i++)
	{
		BSTR bstrName = 0;
		sptrData->GetEntryName(i, &bstrName);

		CString str(bstrName);

		/*_variant_t	var;
		
		sptrData->GetValue(bstrName, &var);

		_VarChangeType(var, VT_BSTR);

		CString str(var.bstrVal);*/
		
		//arrClassesGuids.Add(bstrName);
		GUID guid;
		CLSIDFromString(_bstr_t(str), &guid);

		IApplicationPtr sptrA(GetAppUnknown());
		LONG_PTR	lPosTemplate = 0;
		sptrA->GetFirstDocTemplPosition(&lPosTemplate);
		while(lPosTemplate)
		{
			LONG_PTR	lPosDoc = 0;
			sptrA->GetFirstDocPosition(lPosTemplate, &lPosDoc);
			while(lPosDoc)
			{
				IUnknown *punkDoc = 0;
				sptrA->GetNextDoc(lPosTemplate, &lPosDoc, &punkDoc);
				if(punkDoc) 
				{
					IUnknown* punk = ::GetObjectByKeyEx(punkDoc, guid);
					if(punk) arrClasses.Add(punk);
					punkDoc->Release();
				}
			}
			sptrA->GetNextDocTempl(&lPosTemplate, 0, 0);
		}

		::SysFreeString(bstrName);
	}
	
	/*CTypedPtrList<CPtrList, ClassInfo*> listClasses;

	for(i = 0; i < nEntries; i++)
	{
		sptrData->SetupSection(_bstr_t(strSectionName + "\\" + arrClassesGuids[i]));
		long nEntriesParams = 0;
		sptrData->GetEntriesCount(&nEntriesParams);
		CStringArray arrParamsSections;
		for(long j = 0; j < nEntriesParams; j++)
		{
			BSTR bstrName = 0;
			sptrData->GetEntryName(j, &bstrName);
			arrParamsSections.Add(bstrName);
			::SysFreeString(bstrName);
		}

		ClassInfo* pClassinfo = new ClassInfo;
		CLSIDFromString(_bstr_t(arrClassesGuids[i]), &pClassinfo->guidClassKey);

		listClasses.AddTail(pClassinfo);
		
		for(j = 0; j < nEntriesParams; j++)
		{
			double fValLo = ::GetValueDouble(GetAppUnknown(), strSectionName+"\\"+arrParamsSections[j], "lo", 0.0);
			double fValHi = ::GetValueDouble(GetAppUnknown(), strSectionName+"\\"+arrParamsSections[j], "hi", 100.0);

			pClassinfo->listParams.AddHead(new ParamInfo(atoi(arrParamsSections[j]), fValLo, fValHi));
		}
	}*/


	//classify

	CopyParams(objectsIn, objectsOut);

	long nNotificator = 0;
	StartNotification(nCountIn);

	POSITION pos = objectsIn.GetFirstObjectPosition();
	while(pos)
	{
		IUnknown* punkObj = objectsIn.GetNextObject(pos);
		ICalcObjectPtr sptrCalc(punkObj);
		if(punkObj)
			punkObj->Release();
		if(sptrCalc == 0)
		{
			Notify(nNotificator++);
			continue;
		}

		long	guidNewClassKey = -1;

		for(long i = 0; i < arrClasses.GetSize(); i++)
		{
			bool bClassApplied = false;
			IClassObjectPtr sptrCO(arrClasses[i]);
			if(sptrCO != 0)
			{
				guidNewClassKey = get_object_class( ICalcObjectPtr( sptrCO ) );
				LONG_PTR lPos = 0;
				LONG_PTR bFirstTime = true;
				sptrCO->GetFirstParamLimitsPos(&lPos);
				while(lPos)
				{
					if(bFirstTime)
					{
						bClassApplied = true;
						bFirstTime = false;
					}
					long lParamKey = 0;
					double fLo = 0., fHi = 0.;
					sptrCO->GetNextParamLimits(&lPos, &lParamKey, &fLo, &fHi);

					double fValue = 0;
					sptrCalc->GetValue(lParamKey, &fValue);

					if(fValue <= fLo || fValue >= fHi)
					{
						guidNewClassKey = -1;
						bClassApplied = false;
						break;
					}
				}
			}
			

			if(bClassApplied)
				break;
		}
		
		
		ICalcObjectPtr sptrCalcOut;
		IClonableObjectPtr sptrClone(sptrCalc);		
		if(sptrClone != 0)
		{
			IUnknown* punkCloned = 0;
			sptrClone->Clone(&punkCloned);

			sptrCalcOut = punkCloned;
			INamedDataObject2Ptr ptrObject(punkCloned); 
			if(ptrObject != 0)
				ptrObject->SetParent(objectsOut, 0);

			if(punkCloned)
				punkCloned->Release();
		}

		if(sptrCalcOut != 0)
			set_object_class( sptrCalcOut, guidNewClassKey );

		
		Notify(nNotificator++);
	}
	

	FinishNotification();


	for(i = 0; i < arrClasses.GetSize(); i++)
	{
		arrClasses[i]->Release();
	}

	/*pos = listClasses.GetHeadPosition();
	while(pos)
		delete listClasses.GetNext(pos);*/
	
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionObjectFilter implementation
CActionObjectFilter::CActionObjectFilter()
{
}

CActionObjectFilter::~CActionObjectFilter()
{
}

bool CActionObjectFilter::InvokeFilter()
{
	CObjectListWrp objectsIn(GetDataArgument());
	CObjectListWrp objectsOut(GetDataResult());
	CString strSectionName = GetArgumentString(_T("SectionName"));
	bool bShowInterface = GetArgumentInt(_T("ShowInterface")) == 1;

//	long nCountIn = objectsIn.GetCount();
//	if(!nCountIn)
//		return false;
	
	
	if(!bShowInterface)
	{		
		INamedDataPtr	sptrData(GetAppUnknown());
		sptrData->SetupSection(_bstr_t(strSectionName));
		long nEntries = 0;
		sptrData->GetEntriesCount(&nEntries);
		if(nEntries <= 0)
		{
			CString strText, strCaption;
			strText.LoadString(IDS_PARAMS_NOT_SPECIFIED);
			strCaption.LoadString(IDS_OBJECT_FILTER_ERROR_CAPTION);
			if(MessageBox(NULL, strText, strCaption, MB_YESNO) == IDYES)
				bShowInterface = true;
			else
				return true;
		}
	}
	
	if(bShowInterface)
	{
		CObjectFilterDlg dlg(NULL, strSectionName);
		if (dlg.DoModal() != IDOK)
			return false;
	}

	long nCountIn = objectsIn.GetCount();
	if(!nCountIn)
		return false;


	INamedDataPtr	sptrData(GetAppUnknown());
	sptrData->SetupSection(_bstr_t(strSectionName));
	long nEntries = 0;
	sptrData->GetEntriesCount(&nEntries);
	if(nEntries <= 0)
		return true;
	
	CTypedPtrList<CPtrList, ParamInfo*> listParams;

	CStringArray arrParamsSections;
	long i = 0;
	for(i = 0; i < nEntries; i++)
	{
		BSTR bstrName = 0;
		sptrData->GetEntryName(i, &bstrName);
		arrParamsSections.Add(bstrName);
		::SysFreeString(bstrName);
	}
	
	for(i = 0; i < nEntries; i++)
	{
		if(arrParamsSections[i].IsEmpty())
			continue;

		double fValLo = ::GetValueDouble(GetAppUnknown(), strSectionName+"\\"+arrParamsSections[i], "lo", 0.0);
		double fValHi = ::GetValueDouble(GetAppUnknown(), strSectionName+"\\"+arrParamsSections[i], "hi", 100.0);

		listParams.AddTail(new ParamInfo(atoi(arrParamsSections[i]), fValLo, fValHi));
	}

	CopyParams(objectsIn, objectsOut);

	StartNotification(nCountIn);
	long nNotificator = 0;
	POSITION pos = objectsIn.GetFirstObjectPosition();
	while(pos)
	{
		IUnknown* punkObj = objectsIn.GetNextObject(pos);
		ICalcObjectPtr sptrCalc(punkObj);
		if(punkObj)
			punkObj->Release();
		if(sptrCalc == 0)
		{
			Notify(nNotificator++);
			continue;
		}

		bool bDelete = false;
		
		INamedDataObject2Ptr sptrNDO2(sptrCalc);
		IUnknownPtr punkMM(GetActiveMeasureObject(sptrNDO2), false);
		if (punkMM == 0)
		{
		POSITION pos = listParams.GetHeadPosition();
		while(pos)
		{
			ParamInfo* pParam = listParams.GetNext(pos);
			double fVal = 0.;
			
			if(sptrCalc->GetValue(pParam->nParamID, &fVal) != S_OK)
			{
/*				CString strText, strCaption;
				strText.LoadString(IDS_PARAMS_NOT_CALCED);
				strCaption.LoadString(IDS_OBJECT_FILTER_ERROR_CAPTION);
				MessageBox(NULL, strText, strCaption, MB_OK);

				POSITION pos1 = listParams.GetHeadPosition();
				while(pos1)
					delete listParams.GetNext(pos1);

				return true;*/
				bDelete = true;
				break;
			}
			double fCoeffToUnits = 1.;
			ICalcObjectContainerPtr sptrCntr(objectsIn);
			ParameterContainer *pParamCont;
			sptrCntr->ParamDefByKey(pParam->nParamID, &pParamCont);
			if (pParamCont && pParamCont->pDescr)
				fCoeffToUnits = pParamCont->pDescr->fCoeffToUnits;
			//!!! fVal *= fCoeffToUnits; // !!! удалить - чтобы работать в метрах; оставить - в текущих ед.

			if(fVal < pParam->fLo || fVal > pParam->fHi)
			{
				bDelete = true;
				break;
			}
		}
		}
	
		if(!bDelete)	
		{
			IClonableObjectPtr sptrClone(sptrCalc);		
			if(sptrClone != 0)
			{
				IUnknown* punkCloned = 0;
				sptrClone->Clone(&punkCloned);

				INamedDataObject2Ptr ptrObject(punkCloned);
				if(ptrObject != 0)
					ptrObject->SetParent(objectsOut, 0);

				if(punkCloned)
					punkCloned->Release();
			}
		}

		Notify(nNotificator++);
	}
	

	GUID guidNewBase;
	::CoCreateGuid(&guidNewBase);
	INamedDataObject2Ptr sptrNO(objectsOut);
	if(sptrNO != 0)
		sptrNO->SetBaseKey(&guidNewBase);

	FinishNotification();


	pos = listParams.GetHeadPosition();
	while(pos)
		delete listParams.GetNext(pos);




	// Now all OK. Redraw document.
/*	INamedDataObject2Ptr NDO(objectsIn);
	IUnknown *punk = NULL;
	HRESULT hr = NDO->GetData(&punk);
	if (SUCCEEDED(hr) && punk)
	{
		long l = cncReset;
		INotifyControllerPtr sptr(punk);
		sptr->FireEvent(_bstr_t(szEventChangeObjectList), NULL, NDO, &l, 0);
		punk->Release();
	}*/



	return true;
}

bool CActionObjectFilter::Invoke()
{
	IUnknown *punkRes = GetDataResult();
	bool b = CFilterBase::Invoke();
	if (punkRes)
	{
		if (b)
		{
			IUnknown	*punkView = 0;
			IDocumentSitePtr	ptrDocSite = m_punkTarget;
			ptrDocSite->GetActiveView(&punkView);
			if( punkView )
			{
				sptrIDataContext sptrDC(punkView);
				sptrDC->SetActiveObject(_bstr_t(szTypeObjectList), punkRes, 0);
				punkView->Release();
			}
		}
//		punkRes->Release();
	}
	return b;
}



//////////////////////////////////////////////////////////////////////
//CActionObjectScissors implementation
CActionObjectScissors::CActionObjectScissors() : m_rcHotLine(NORECT)
{
	m_bTrackNow = false;
	m_pContour = 0;
	m_crColor = RGB(0,0,0);
}

CActionObjectScissors::~CActionObjectScissors()
{
	if(m_pContour)
		delete m_pContour;
	m_pContour = 0;
}

void CActionObjectScissors::DoDraw( CDC &dc )
{
	CPen pen( PS_SOLID, 1, m_crColor ); 
	CPen *pold = dc.SelectObject( &pen );

	if(m_rcHotLine != NORECT)
	{
		dc.MoveTo(m_rcHotLine.TopLeft());
		dc.LineTo(m_rcHotLine.BottomRight());
	}
	if(m_pContour)
		::ContourDraw(dc, m_pContour, m_punkTarget, 0);

	dc.SelectObject( pold );
}

bool CActionObjectScissors::Initialize()
{
	m_rcHotLine = NORECT;
	m_hcurActive = AfxGetApp()->LoadCursor( IDC_SCISSORS );
	m_crColor = ::GetValueColor( GetAppUnknown(), "\\ObjectScissors", "Color", RGB( 0, 0, 0 ) );
	return CActionObjectBase::Initialize();
}

static CPoint s_ptPrevFilled(-1,-1);
static CRect s_rcBounds(NORECT);
void __stdcall LineDDAProcContour(int x, int y, LPARAM lData)
{
	CWalkFillInfo* pwfi = (CWalkFillInfo*)lData;
	byte	*pimageMask = pwfi->GetRowMask(y) + x;
	*pimageMask = 255;

	if(s_ptPrevFilled != CPoint(-1,-1))
	{
		if(abs(s_ptPrevFilled.x-x) + abs(s_ptPrevFilled.y-y) > 1)
		{
			long nX = s_ptPrevFilled.x + (x-s_ptPrevFilled.x);
			if(nX > s_rcBounds.left && nX < s_rcBounds.right)
			{
				pimageMask = pwfi->GetRowMask(s_ptPrevFilled.y) + nX;
				*pimageMask = 255;
			}
		}
	}
	s_ptPrevFilled = CPoint(x,y);
}

bool CActionObjectScissors::DoChangeObjects()
{
	s_ptPrevFilled = CPoint(-1,-1);
	if(!m_pContour)
		return false;

	m_image.Attach(GetActiveImage(), false);

	//s_rectContour = ::ContourCalcRect(m_pContour);
	m_pwfi = new CWalkFillInfo(m_image.GetRect());

	CRect	rect = m_pwfi->GetRect();
	s_rcBounds = rect;
	int	x, y;
	for( x = rect.left-1; x < rect.right+1; x++ )
	{
		m_pwfi->SetPixel( x, rect.top-1, 0 );
		m_pwfi->SetPixel( x, rect.bottom, 0 );
	}
	for( y = rect.top-1; y < rect.bottom+1; y++ )
	{
		m_pwfi->SetPixel( rect.left-1, y, 0 );
		m_pwfi->SetPixel( rect.right, y, 0 );
	}

	x = m_pContour->ppoints[0].x;
	y = m_pContour->ppoints[0].y;
	for(int nPoint = 1; nPoint < m_pContour->nContourSize; nPoint++)
	{
		int	nOldX = x;
		int	nOldY = y;
		x = m_pContour->ppoints[nPoint].x;
		y = m_pContour->ppoints[nPoint].y;
		if(rect.PtInRect(CPoint(x,y)) && rect.PtInRect(CPoint(nOldX, nOldY)))
		{
			::LineDDA(nOldX, nOldY, x, y, LineDDAProcContour, (long)m_pwfi); 
			LineDDAProcContour(x,y, (long)m_pwfi); // добавим последнюю точку линии - просто на всякий случай
		}
	}

	SeparateObjects(m_pwfi);

	delete m_pwfi;
	m_pwfi = 0;

	return true;
}

static CPoint s_ptPrevMouseMove(0,0);
bool CActionObjectScissors::DoMouseMove( CPoint pt )
{
	if(pt == s_ptPrevMouseMove)
		// vanek - 24.09.2003
		return false;//true; return false to highlight objects

	s_ptPrevMouseMove = pt;

	_Draw();
	m_rcHotLine = NORECT;
	if( m_objects==0 )
		m_objects.Attach( GetActiveList(), false );
	POSITION	pos = m_objects.GetFirstObjectPosition();
	while( pos )
	{
		CObjectWrp	object( m_objects.GetNextObject( pos ), false );
		
		if(object.PtInObject(pt))
		{			
			CImageWrp image(object.GetImage());
			int nContourCount = image.GetContoursCount();
			if(nContourCount > 0)
			{
				Contour* pContour = image.GetContour(0);
				if(pContour)
				{
					m_rcHotLine.TopLeft() = ::ConvertToWindow(m_punkTarget, pt);
					long nMinDist = INT_MAX;
					for(long k = 0; k < pContour->nContourSize; k++)
					{
						CPoint ptContour = CPoint(pContour->ppoints[k].x, pContour->ppoints[k].y);
						long nDist = (ptContour.x-pt.x)*(ptContour.x-pt.x) + (ptContour.y-pt.y)*(ptContour.y-pt.y);
						if(nDist < nMinDist)
						{
							nMinDist = nDist;
							m_rcHotLine.BottomRight() = ::ConvertToWindow(m_punkTarget, ptContour);
						}
					}
				}
			}
			break;
		}
	
	}
	_Draw();

	// vanek - 24.09.2003
	return false;//true; return false to highlight objects
}

bool CActionObjectScissors::DoLButtonDown( CPoint point )
{
	return StartTracking(point);
}

bool CActionObjectScissors::DoStartTracking( CPoint point )
{
	if(m_bTrackNow)
		return true;
	m_bTrackNow = true;

	if(m_pContour)
		delete m_pContour;
	m_pContour = 0;
	
	_Draw();
	m_pContour = ::ContourCreate();
	if(m_rcHotLine != NORECT)
	{
		m_rcHotLine = ::ConvertToClient(m_punkTarget, m_rcHotLine);
		long nLen = max(
			abs(m_rcHotLine.left-m_rcHotLine.right),
			abs(m_rcHotLine.top-m_rcHotLine.bottom) );
		long dx = 0;
		long dy = 0;
		for(int i=1; i>=0; i--)
		{
			if(nLen)
			{
				dx = i*(m_rcHotLine.BottomRight().x-m_rcHotLine.TopLeft().x)/nLen;
				dy = i*(m_rcHotLine.BottomRight().y-m_rcHotLine.TopLeft().y)/nLen;
			}
			::ContourAddPoint(m_pContour, m_rcHotLine.BottomRight().x+dx, m_rcHotLine.BottomRight().y+dy, false);
		}
		//::ContourAddPoint(m_pContour, m_rcHotLine.BottomRight().x, m_rcHotLine.BottomRight().y, false);
		::ContourAddPoint(m_pContour, m_rcHotLine.TopLeft().x, m_rcHotLine.TopLeft().y, false);
		m_rcHotLine = NORECT;
	}
	::ContourAddPoint(m_pContour, point.x, point.y, false);
	_Draw();

	return true;
}

bool CActionObjectScissors::DoFinishTracking( CPoint point )
{
	m_bTrackNow = false;

	if(!m_pContour)
		return true; 
	_Draw();
	::ContourAddPoint(m_pContour, point.x, point.y);
	if(m_rcHotLine != NORECT)
	{
		m_rcHotLine = ::ConvertToClient(m_punkTarget, m_rcHotLine);
		long nLen = max(
			abs(m_rcHotLine.left-m_rcHotLine.right),
			abs(m_rcHotLine.top-m_rcHotLine.bottom) );
		long dx = 0;
		long dy = 0;
		::ContourAddPoint(m_pContour, m_rcHotLine.TopLeft().x, m_rcHotLine.TopLeft().y, false);
		for(int i=0; i<=3; i++)
		{
			if(nLen)
			{
				dx = i*(m_rcHotLine.BottomRight().x-m_rcHotLine.TopLeft().x)/nLen;
				dy = i*(m_rcHotLine.BottomRight().y-m_rcHotLine.TopLeft().y)/nLen;
			}
			::ContourAddPoint(m_pContour, m_rcHotLine.BottomRight().x+dx, m_rcHotLine.BottomRight().y+dy, false);
		}
		m_rcHotLine = NORECT;
	}
	_Draw();
	return true;
}

bool CActionObjectScissors::DoTrack( CPoint point )
{
	_Draw();
	if(m_pContour)::ContourAddPoint(m_pContour, point.x, point.y, false);
	_Draw();
	return true;
}

bool CActionObjectScissors::DoLButtonUp( CPoint point )
{
	Finalize();
	return true;
}

bool CActionObjectScissors::IsAvaible()
{
	IUnknownPtr punkOL(GetActiveList(), false);
	if (punkOL == 0)
		return false;
	if (!CheckInterface(m_punkTarget, IID_IImageView))
		return false;
	return true;
}



//////////////////////////////////////////////////////////////////////
//CActionAutoAssignClasses implementation
CActionAutoAssignClasses::CActionAutoAssignClasses()
{
}

CActionAutoAssignClasses::~CActionAutoAssignClasses()
{
}

bool CActionAutoAssignClasses::InvokeFilter()
{
	CObjectListWrp objectsIn(GetDataArgument());
	CObjectListWrp objectsOut(GetDataResult());
	
	long nCountIn = objectsIn.GetCount();
	if(!nCountIn)
		return false;


	IUnknown* punkClassList = CreateTypedObject(szTypeClassList);
	CObjectListWrp classOut(punkClassList);
	if(punkClassList)
	{
		CString strName = "AutoAssignClasses classlist";
		UniqueName(m_punkTarget, strName);
		SetObjectName(punkClassList, strName);
		m_changes.SetToDocData(m_punkTarget, punkClassList);
		punkClassList->Release();
	}

	INamedDataPtr sptrND(m_punkTarget);
	if(sptrND != 0)
	{
		IUnknown* punkContext = 0;
		sptrND->GetActiveContext(&punkContext);
		IDataContext2Ptr sptrContext(punkContext);
		if(punkContext)
			punkContext->Release();
		if(sptrContext != 0)
			sptrContext->SetObjectSelect(classOut, TRUE);
	}

	long   lParamKey = GetValueInt   (GetAppUnknown(), "\\measurement\\AutoAssignClasses", "ParamKey", 0);
	long   lCount    = GetValueInt   (GetAppUnknown(), "\\measurement\\AutoAssignClasses", "Count",   -1);
	double fMax      = GetValueDouble(GetAppUnknown(), "\\measurement\\AutoAssignClasses", "Max",     -1);
	double fMin      = GetValueDouble(GetAppUnknown(), "\\measurement\\AutoAssignClasses", "Min",     -1);

	bool   bAutoCount = lCount == -1;
	bool   bAutoMax   = fMax   == -1;
	bool   bAutoMin   = fMin   == -1;

	if(lCount < 0)
	{
		if(nCountIn > 260)
		{
			lCount = 26;
		}
		else
		{
			if(nCountIn > 100)
				lCount = nCountIn/10 + 1;
			else
				lCount = long( (3.2*log((double)nCountIn)) + 3 );
		}
	}

	CopyParams(objectsIn, objectsOut);

	long nNotificator = 0;
	if(bAutoMax || bAutoMin)
	{
		if(bAutoMax)
			fMax = 0;
		if(bAutoMin)
			fMin = DBL_MAX;

		StartNotification(nCountIn, 2);

		POSITION pos = objectsIn.GetFirstObjectPosition();
		while(pos)
		{
			IUnknown* punkObj = objectsIn.GetNextObject(pos);
			ICalcObjectPtr sptrCalc(punkObj);
			if(punkObj)
				punkObj->Release();
			if(sptrCalc == 0)
			{
				Notify(nNotificator++);
				continue;
			}

			double fValue = 0;
			sptrCalc->GetValue(lParamKey, &fValue);

			if(bAutoMax)
				fMax = max(fMax, fValue);
			if(bAutoMin)
				fMin = min(fMin, fValue);

			Notify(nNotificator++);
		}

		NextNotificationStage();
	}
	else
		StartNotification(nCountIn);

	if(fMax == fMin)
		lCount = 1;

	struct object_class
	{
		object_class()
		{
			fHiBound	 = 0;
			guidClassKey = -1;
		};
		double			fHiBound;
		long			guidClassKey;
	};
	object_class* pClassesArr = new object_class[lCount];
	
	double fStep = (fMax-fMin)/lCount;
	for(long i = 0; i < lCount; i++)
	{
		pClassesArr[i].fHiBound = fMin + fStep*(i+1);

		IUnknown* punkClass = ::CreateTypedObject(szTypeClass);
		if(punkClass)
		{
			INamedDataObject2Ptr ptrObject(punkClass);
			if(ptrObject != 0)
				ptrObject->SetParent(classOut, 0);
			pClassesArr[i].guidClassKey = get_object_class( ICalcObjectPtr( punkClass) );
			CString strName;
			strName.Format("Class%d", i);
			SetObjectName(punkClass, strName);
			punkClass->Release();
		}
	}

	nNotificator = 0;
	POSITION pos = objectsIn.GetFirstObjectPosition();
	while(pos)
	{
		IUnknown* punkObj = objectsIn.GetNextObject(pos);
		ICalcObjectPtr sptrCalc(punkObj);
		if(punkObj)
			punkObj->Release();
		if(sptrCalc == 0)
		{
			Notify(nNotificator++);
			continue;
		}

		double fValue = 0;
		sptrCalc->GetValue(lParamKey, &fValue);

		long lClassKey = -1;
		if(fValue <= fMax || fValue >= fMin)
		{
			for(long i = 0; i < lCount; i++)
			{
				if(fValue <= pClassesArr[i].fHiBound)
				{
					lClassKey = pClassesArr[i].guidClassKey;
					break;
				}
			}
		}

		ICalcObjectPtr sptrCalcOut;
		IClonableObjectPtr sptrClone(sptrCalc);		
		if(sptrClone != 0)
		{
			IUnknown* punkCloned = 0;
			sptrClone->Clone(&punkCloned);

			sptrCalcOut = punkCloned;
			INamedDataObject2Ptr ptrObject(punkCloned); 
			if(ptrObject != 0)
				ptrObject->SetParent(objectsOut, 0);

			if(punkCloned)
				punkCloned->Release();
		}

		if(sptrCalcOut != 0)
			set_object_class( sptrCalcOut, lClassKey);

		
		Notify(nNotificator++);
	}
	

	delete pClassesArr;

	GUID guidNewBase;
	::CoCreateGuid(&guidNewBase);
	INamedDataObject2Ptr sptrNO(objectsOut);
	if(sptrNO != 0)
		sptrNO->SetBaseKey(&guidNewBase);

	FinishNotification();


	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionObjectsRemove implementation
CActionObjectsRemove::CActionObjectsRemove()
{
}

CActionObjectsRemove::~CActionObjectsRemove()
{
}

bool CActionObjectsRemove::InvokeFilter()
{
	CObjectListWrp objectsIn(GetDataArgument());
	CObjectListWrp objectsOut(GetDataResult());
	
	ICompositeObjectPtr o_list(objectsIn);
	long bC = 0;
	if(o_list)
	{
		o_list->IsComposite(&bC);
	}

	long lclass_number = -2;
	lclass_number = GetArgumentInt( "Class" );
    
	// [vanek] : если класс не задан - удаляем все объекты класса активного объекта, если активного объекта нет - 
	// все объекты неизвестного класса (см. документ "Доработки по классификации в методиках.doc") - 16.11.2004
	if( lclass_number == -2 )
	{
		POSITION pos_active = 0;
		pos_active = objectsIn.GetCurPosition();
		if( pos_active )
		{
            IUnknown* punkObj = 0;
			punkObj = objectsIn.GetNextObject( pos_active );
            ICalcObjectPtr sptrCalc(punkObj);
			if(punkObj)
				punkObj->Release(); punkObj = 0; 

			lclass_number  = get_object_class( sptrCalc );
		}
		else
			lclass_number = -1;
	}
	
	long nCountIn = objectsIn.GetCount();
	if(!nCountIn)
		return false;

	CopyParams(objectsIn, objectsOut);

	StartNotification(nCountIn, 1, 1);

	long nNotificator = 0;
	POSITION pos = objectsIn.GetFirstObjectPosition();
	while(pos)
	{
		IUnknown* punkObj = objectsIn.GetNextObject(pos);
		ICalcObjectPtr sptrCalc(punkObj);
		if(punkObj)
			punkObj->Release();
		if(sptrCalc == 0)
		{
			Notify(nNotificator++);
			continue;
		}
		bool bRemove = false;
		if(bC)
		{
			if(get_parent_class(sptrCalc, objectsIn)==lclass_number) bRemove=true; 
		}

		if( lclass_number != get_object_class( sptrCalc ) && !bRemove)
		{
			
			IClonableObjectPtr sptrClone(sptrCalc);		
			if(sptrClone != 0)
			{
				IUnknown* punkCloned = 0;
				sptrClone->Clone(&punkCloned);

				INamedDataObject2Ptr ptrObject(punkCloned);
				if(ptrObject != 0)
					ptrObject->SetParent(objectsOut, 0);

				if(punkCloned)
					punkCloned->Release();
			}
		}

		Notify(nNotificator++);
	}
	
	GUID guidNewBase;
	::CoCreateGuid(&guidNewBase);
	INamedDataObject2Ptr sptrNO(objectsOut);
	if(sptrNO != 0)
		sptrNO->SetBaseKey(&guidNewBase);

	/*CopyNamedData(objectsOut, objectsIn);*/

	{//поддержка фазовых листов
		INamedDataPtr ndin(objectsIn);
		InitAttachedData(objectsOut);
		INamedDataPtr ndout(objectsOut);
		if(ndin!=0 && ndout!=0)
		{
			bstr_t bstrName("Phases");
			variant_t var;
			ndin->GetValue( bstrName, &var );
			ndout->SetValue( bstrName, var );
		}
	}

	if(bC)
	{
		long binCount=0;
		o_list->GetPlaneCount(&binCount);
		ICompositeObjectPtr co(objectsOut);
		co->SetCompositeFlag();
		co->SetPlaneCount(binCount);
		CCompositeInfo* ci;
		o_list->GetCompositeInfo(&ci);
		co->SetCompositeInfo(ci);
		co->RestoreTreeByGUIDs();	
	}
	FinishNotification();

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionObjectsMerge implementation
CActionObjectsMerge::CActionObjectsMerge()
{
}

CActionObjectsMerge::~CActionObjectsMerge()
{
}

bool CActionObjectsMerge::InvokeFilter()
{
	int nVerType = GetArgumentInt(_T("VerifyType"));
	CObjectListWrp objectsOut(GetDataResult());
	IUnknown* punkList[2] = {GetDataArgument("MeasObjects1"), GetDataArgument("MeasObjects2")};

	CObjectListWrp objectsIn1(punkList[0]);
	CObjectListWrp objectsIn2(punkList[1]);
	long nCountIn1 = objectsIn1.GetCount();
	long nCountIn2 = objectsIn2.GetCount();
	if(!nCountIn1 && !nCountIn2)
		return false;

	long nNotificator = 0;
	StartNotification(nCountIn1+nCountIn2);

	for(long i = 0; i < 2; i++)
	{
		CObjectListWrp objectsIn(punkList[i]);
		CopyParams(objectsIn, objectsOut);

		POSITION pos = objectsIn.GetFirstObjectPosition();
		while(pos)
		{
			IUnknown* punkObj = objectsIn.GetNextObject(pos);
			IClonableObjectPtr sptrClone(punkObj);		
			if(punkObj)
				punkObj->Release();
//			if(nVerType>0&&i>0&&TestExist(sptrClone,objectsIn,objectsIn1,nVerType))
//				continue;
			if(nVerType>0&&!TestNewObjects(punkList,2,i,sptrClone,nVerType))
				continue;
			if(sptrClone != 0)
			{
				IUnknown* punkCloned = 0;
				sptrClone->Clone(&punkCloned);

				INamedDataObject2Ptr ptrObject(punkCloned);
				if(ptrObject != 0)
					ptrObject->SetParent(objectsOut, 0);

//				if(nVerType>0)
				SaveObjectInfo(punkCloned,objectsIn,sptrClone);

				if(punkCloned)
					punkCloned->Release();
			}

			Notify(nNotificator++);
		}

	}

	GUID guidNewBase;
	::CoCreateGuid(&guidNewBase);
	INamedDataObject2Ptr sptrNO(objectsOut);
	if(sptrNO != 0)
		sptrNO->SetBaseKey(&guidNewBase);

	FinishNotification();

	{

		IDataContextPtr sptrContext( m_punkTarget );
		sptrContext->SetActiveObject(_bstr_t(szTypeObjectList), objectsOut, aofActivateDepended);
	}

	
	return true;
}

//VerifyType 
#define VER_TYPE_NONE			0
#define VER_TYPE_IMAGE			1
#define VER_TYPE_OBJECT_LIST	2
#define VER_TYPE_OBJECT			3

static BOOL GetSavedObjectInfo(IUnknown* punkObj, _bstr_t &bstrBaseGuid, _bstr_t &bstrGuidOL,
							   _bstr_t &bstrGuidObj)
{
	INamedPropBagSerPtr sptrNPB(punkObj);
	if (sptrNPB == 0)
		return FALSE;
	_variant_t var;
	sptrNPB->GetProperty(_bstr_t("BaseKey"), &var);
	if (var.vt == VT_BSTR)
		bstrBaseGuid = var.bstrVal;
	else
		return FALSE;
	sptrNPB->GetProperty(_bstr_t("ObjectListKey"), &var);
	if (var.vt == VT_BSTR)
		bstrGuidOL = var.bstrVal;
	else
		return FALSE;
	sptrNPB->GetProperty(_bstr_t("ObjectKey"), &var);
	if (var.vt == VT_BSTR)
		bstrGuidObj = var.bstrVal;
	else
		return FALSE;
	return TRUE;
}

static _bstr_t StringByGuid(GUID guid)
{
	LPOLESTR lpOleStr = NULL;
	StringFromCLSID(guid, &lpOleStr);
	_bstr_t bstrGuid = lpOleStr;
	CoTaskMemFree(lpOleStr);
	return bstrGuid;
}

BOOL CActionObjectsMerge::TestNewObjects(IUnknown* punkLists[], int nObjLists, int nSkipOL, IUnknown* punkObj, int nVerType)
{
	_bstr_t bstrBaseGuid,bstrGuidOL,bstrGuidObj;
	if (!GetSavedObjectInfo(punkObj,bstrBaseGuid,bstrGuidOL,bstrGuidObj))
		return TRUE; // This is a new object
	// Existing object. Test whether it must be replaced by new one.
	for (int i = 0; i < nObjLists; i++)
	{
		if (i == nSkipOL) continue;
		IUnknown *punkList = punkLists[i];
		if (nVerType == VER_TYPE_IMAGE)
		{
			INamedDataObject2Ptr sptrNDO2OLCur = punkList;
			GUID guidBase = {0};
			sptrNDO2OLCur->GetBaseKey(&guidBase);
			_bstr_t bstrBaseGuid1 = StringByGuid(guidBase);
			if (bstrBaseGuid1 == bstrBaseGuid)
				return FALSE;
		}
		else if (nVerType == VER_TYPE_OBJECT_LIST)
		{
			GUID guidOL = ::GetObjectKey(punkList);
			_bstr_t bstrGuidOL1 = StringByGuid(guidOL);
			if (bstrGuidOL == bstrGuidOL1)
				return FALSE;

		}
		else if (nVerType == VER_TYPE_OBJECT)
		{
			CObjectListWrp ObjLstTest(punkList);
			POSITION pos = ObjLstTest.GetFirstObjectPosition();
			while(pos)
			{
				IUnknownPtr punkObjTest(ObjLstTest.GetNextObject(pos), false);
				GUID guidObj = ::GetObjectKey(punkObjTest);
				_bstr_t bstrGuidObj1 = StringByGuid(guidObj);
				if (bstrGuidObj1 == bstrGuidObj)
					return FALSE;
			}
		}
	}
	return TRUE;
}


BOOL CActionObjectsMerge::TestExist(IUnknown* punkObj, CObjectListWrp &ObjLstCur, CObjectListWrp &ObjLstTest, int nVerType)
{
	INamedDataObject2Ptr sptrNDO2OLCur = ObjLstCur;
	GUID guidOL = ::GetObjectKey(ObjLstCur);
	GUID guidBase = {0};
	sptrNDO2OLCur->GetBaseKey(&guidBase);
	GUID guidObj = ::GetObjectKey(punkObj);
	LPOLESTR lpOleStr = NULL;
	StringFromCLSID(guidBase, &lpOleStr);
	_bstr_t bstrBaseGuid = lpOleStr;
	CoTaskMemFree(lpOleStr);
	lpOleStr = NULL;
	StringFromCLSID(guidOL, &lpOleStr);
	_bstr_t bstrGuidOL = lpOleStr;
	CoTaskMemFree(lpOleStr);
	lpOleStr = NULL;
	StringFromCLSID(guidObj, &lpOleStr);
	_bstr_t bstrGuidObj = lpOleStr;
	CoTaskMemFree(lpOleStr);
	POSITION pos = ObjLstTest.GetFirstObjectPosition();
	while(pos)
	{
		IUnknownPtr punkObjTest(ObjLstTest.GetNextObject(pos), false);
		INamedPropBagSerPtr sptrNPB(punkObjTest);
		if (nVerType == VER_TYPE_IMAGE)
		{
			_variant_t var;
			sptrNPB->GetProperty(_bstr_t("BaseKey"), &var);
			if (var.vt == VT_BSTR)
			{
				_bstr_t bstrBaseGuid1 = var.bstrVal;
				if (bstrBaseGuid == bstrBaseGuid1)
					return true;
			}
		}
		else if (nVerType == VER_TYPE_OBJECT_LIST)
		{
			_variant_t var;
			sptrNPB->GetProperty(_bstr_t("ObjectListKey"), &var);
			if (var.vt == VT_BSTR)
			{
				_bstr_t bstrOLGuid1 = var.bstrVal;
				if (bstrGuidOL == bstrOLGuid1)
					return true;
			}
		}
		else if (nVerType == VER_TYPE_OBJECT)
		{
			_variant_t var;
			sptrNPB->GetProperty(_bstr_t("ObjectKey"), &var);
			if (var.vt == VT_BSTR)
			{
				_bstr_t bstrObjGuid1 = var.bstrVal;
				if (bstrGuidObj == bstrObjGuid1)
					return true;
			}
		}
	}
	return false;
}

void CActionObjectsMerge::SaveObjectInfo(IUnknown* punkObj, CObjectListWrp &ObjLstCur, IUnknown* punkObjSrc)
{
	INamedDataObject2Ptr sptrNDO2OLCur = ObjLstCur;
	INamedPropBagSerPtr sptrNPB(punkObj);
	GUID guidOL = ::GetObjectKey(ObjLstCur);
	GUID guidBase = {0};
	sptrNDO2OLCur->GetBaseKey(&guidBase);
	GUID guidObj = ::GetObjectKey(punkObjSrc);
	LPOLESTR lpOleStr = NULL;
	StringFromCLSID(guidBase, &lpOleStr);
	_bstr_t bstrBaseGuid = lpOleStr;
	CoTaskMemFree(lpOleStr);
	lpOleStr = NULL;
	StringFromCLSID(guidOL, &lpOleStr);
	_bstr_t bstrGuidOL = lpOleStr;
	CoTaskMemFree(lpOleStr);
	lpOleStr = NULL;
	StringFromCLSID(guidObj, &lpOleStr);
	_bstr_t bstrGuidObj = lpOleStr;
	CoTaskMemFree(lpOleStr);
	_variant_t varBaseGuid(bstrBaseGuid);
	_variant_t varOLGuid(bstrGuidOL);
	_variant_t varObjGuid(bstrGuidObj);
	_variant_t var;
	if (FAILED(sptrNPB->GetProperty(_bstr_t("BaseKey"), &var)) || var.vt != VT_BSTR)
		sptrNPB->SetProperty(_bstr_t("BaseKey"), varBaseGuid);
	if (FAILED(sptrNPB->GetProperty(_bstr_t("ObjectListKey"), &var)) || var.vt != VT_BSTR)
		sptrNPB->SetProperty(_bstr_t("ObjectListKey"), varOLGuid);
	if (FAILED(sptrNPB->GetProperty(_bstr_t("ObjectKey"), &var)) || var.vt != VT_BSTR)
		sptrNPB->SetProperty(_bstr_t("ObjectKey"), varObjGuid);
}



//////////////////////////////////////////////////////////////////////
//CActionObjectsSetClass implementation
CActionObjectsSetClass::CActionObjectsSetClass()
{
}

CActionObjectsSetClass::~CActionObjectsSetClass()
{
}

bool CActionObjectsSetClass::InvokeFilter()
{
	CObjectListWrp objectsIn(GetDataArgument());
	CObjectListWrp objectsOut(GetDataResult());
	CString strClassName = GetArgumentString(_T("Class"));

	CString strUnknownClassName = get_class_name( -1 );
		//::GetValueString(m_punkTarget, szPluginClasses, szUnkClassName, "Unknown");
	bool bUnknown = strUnknownClassName == strClassName;

	long nCountIn = objectsIn.GetCount();
	if(!nCountIn)
		return false;

	long lClassKey = -1;
	IUnknown* punkClassList = GetActiveObjectFromDocument(m_punkTarget, szTypeClassList);
	CObjectListWrp classlist(punkClassList);
	if(punkClassList)
		punkClassList->Release();

	bool bClassFound = false;
	POSITION pos = classlist.GetFirstObjectPosition();
	if(bUnknown)
	{
		bClassFound = true;
	}
	else
	{
		while(pos)
		{
			IUnknown* punkClass = classlist.GetNextObject(pos);
			if(punkClass)
			{
				if(::GetObjectName(punkClass) == strClassName)
				{
					lClassKey = get_object_class( ICalcObjectPtr( punkClass ) );
					punkClass->Release();
					bClassFound = true;
					break;
				}
				punkClass->Release();
			}
		}
	}

	if(!bClassFound)
		return false;

	if(!bUnknown)
	{
		AddClassName(m_punkTarget, objectsOut, strClassName);
	}

	StartNotification(nCountIn);

	CopyParams(objectsIn, objectsOut);

	long nNotificator = 0;
	pos = objectsIn.GetFirstObjectPosition();
	while(pos)
	{
		IUnknown* punkObj = objectsIn.GetNextObject(pos);
		IClonableObjectPtr sptrClone(punkObj);		
		if(punkObj)
			punkObj->Release();
		if(sptrClone != 0)
		{
			IUnknown* punkCloned = 0;
			sptrClone->Clone(&punkCloned);

			INamedDataObject2Ptr ptrObject(punkCloned);
			if(ptrObject != 0)
				ptrObject->SetParent(objectsOut, 0);

			ICalcObjectPtr sptrCalcOut = punkCloned;
			if(punkCloned)
				punkCloned->Release();
			if(sptrCalcOut != 0)
				set_object_class( sptrCalcOut, lClassKey);
		}

		Notify(nNotificator++);
	}


	GUID guidNewBase;
	::CoCreateGuid(&guidNewBase);
	INamedDataObject2Ptr sptrNO(objectsOut);
	if(sptrNO != 0)
		sptrNO->SetBaseKey(&guidNewBase);

	FinishNotification();

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionObjectsSplit implementation
CActionObjectsSplit::CActionObjectsSplit()
{
}

CActionObjectsSplit::~CActionObjectsSplit()
{
}

bool CActionObjectsSplit::InvokeFilter()
{
	CObjectListWrp objectsIn(GetDataArgument());

	long nCountIn = objectsIn.GetCount();
	if(!nCountIn)
		return false;

	StartNotification(nCountIn, 2);

	long nClasses = 0;
	long nNotificator = 0;
	CMap<long, long&, long, long> mapClasess;
	CArray<CString, CString&> arrClassNames;
	POSITION pos = objectsIn.GetFirstObjectPosition();
	while(pos)
	{
		IUnknown* punkObj = objectsIn.GetNextObject(pos);
		ICalcObjectPtr sptrCalc(punkObj);
		if(punkObj)
			punkObj->Release();
		long ClassKey = get_object_class( sptrCalc );
		long nVal = 0;
		if(mapClasess.Lookup(ClassKey, nVal) == FALSE)
		{
			CString strName = get_class_name( ClassKey );
			arrClassNames.Add(strName);
			mapClasess.SetAt(ClassKey, nClasses);
			nClasses++;
		}
		

		Notify(nNotificator++);
	}

	if(!nClasses)
		return false;

	IUnknown** ppunkLists = new IUnknown*[nClasses];
	
	GUID guidNewBase;
	::CoCreateGuid(&guidNewBase);

	for(long  i = 0; i < nClasses; i++)
	{
		ppunkLists[i] = ::CreateTypedObject( szTypeObjectList );
		if(ppunkLists[i])
		{
			CopyParams(objectsIn, ppunkLists[i]);
			INamedDataObject2Ptr sptrNO(ppunkLists[i]);
			if(sptrNO != 0)
				sptrNO->SetBaseKey(&guidNewBase);
			
			SetObjectName(ppunkLists[i], "ObjectsSplit");
			AddClassName(m_punkTarget, ppunkLists[i], arrClassNames[i]);
			
			m_changes.SetToDocData(m_punkTarget, ppunkLists[i]);
		}
	}

	NextNotificationStage();
	nNotificator = 0;
	pos = objectsIn.GetFirstObjectPosition();
	while(pos)
	{
		IUnknown* punkObj = objectsIn.GetNextObject(pos);
		ICalcObjectPtr sptrCalc(punkObj);
		if(punkObj)
			punkObj->Release();
		if(sptrCalc == 0)
			continue;
		long ClassKey = get_object_class( sptrCalc );

		long nVal = 0;
		VERIFY(mapClasess.Lookup(ClassKey, nVal) == TRUE);


		IClonableObjectPtr sptrClone(sptrCalc);		
		if(sptrClone != 0)
		{
			IUnknown* punkCloned = 0;
			sptrClone->Clone(&punkCloned);

			INamedDataObject2Ptr ptrObject(punkCloned);
			if(ptrObject != 0)
				ptrObject->SetParent(ppunkLists[nVal], 0);

			if(punkCloned)
				punkCloned->Release();
		}

		Notify(nNotificator++);
	}

	for(i = 0; i < nClasses; i++)
	{
		if(ppunkLists[i])
			ppunkLists[i]->Release();
	}
	delete ppunkLists;

	FinishNotification();

	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionObjectsMakeLight implementation
CActionObjectsMakeLight::CActionObjectsMakeLight()
{
}

CActionObjectsMakeLight::~CActionObjectsMakeLight()
{
}

bool CActionObjectsMakeLight::InvokeFilter()
{
	CObjectListWrp objectsIn(GetDataArgument());
	CObjectListWrp objectsOut(GetDataResult());

	long nCountIn = objectsIn.GetCount();
	if(!nCountIn)
		return false;

	CopyParams(objectsIn, objectsOut);

	StartNotification(nCountIn);
	long nCounter = 0;

	POSITION pos = objectsIn.GetFirstObjectPosition();
	while(pos)
	{
		IUnknown* punkObj = objectsIn.GetNextObject(pos);
		ICalcObjectPtr sptrCalc(punkObj);
		if(punkObj)
			punkObj->Release();
		if(sptrCalc == 0)
			continue;

		IUnknown* punkLightObj = ::CreateTypedObject(szTypeLightObject);
		ICalcObjectPtr sptrCalcLight(punkLightObj);
		if(punkLightObj)
			punkLightObj->Release();
		if(sptrCalcLight == 0)
			continue;

		INamedDataObject2Ptr ptrObject(sptrCalcLight);
		if(ptrObject != 0)
			ptrObject->SetParent(objectsOut, 0);

		CopyCalcVals(sptrCalc, sptrCalcLight);
		
		Notify(nCounter++);
	}

	GUID guidNewBase;
	::CoCreateGuid(&guidNewBase);
	INamedDataObject2Ptr sptrNO(objectsOut);
	if(sptrNO != 0)
		sptrNO->SetBaseKey(&guidNewBase);

	FinishNotification();
	
	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionObjectMultiLine implementation
CActionObjectMultiLine::CActionObjectMultiLine()
{
	m_bFinalizeAfterFirstClick = false;
	m_pwfi = 0;
	m_nLineWidth = 5;
	m_nIDCursor = IDC_MULTYLINE;
}

CActionObjectMultiLine::~CActionObjectMultiLine()
{
	if( m_pwfi )
		delete m_pwfi;
}

void CActionObjectMultiLine::DoDraw( CDC &dc )
{
	if( !m_pwfi || !m_pbi )return;

	COLORREF	crFill;
	crFill = get_class_color( -1 );

	::DrawWfiRect( m_pwfi, 
		m_pbi, 
		m_pdib, 
		m_rcInvalid.TopLeft(),
		m_rcInvalid, 
		::GetZoom( m_punkTarget ), 
		::GetScrollPos( m_punkTarget ), 
		crFill );
}
bool CActionObjectMultiLine::Initialize()
{
	AddPropertyPageDescription( c_szCEditObjectPage );
	if( !CActionObjectBase::Initialize() )return false;
	ASSERT( !m_pwfi );
	m_pwfi = new CWalkFillInfo( m_image );
	SetLastPoint( CPoint( 0, 0 ) );
	return true;
	
}

bool CActionObjectMultiLine::DoLButtonDown( CPoint point )
{
	return StartTracking( point );
}

bool CActionObjectMultiLine::DoMouseMove( CPoint point )
{
	if( !m_bInTrackMode && !m_bFinalizeAfterFirstClick && m_points.GetSize() )
		DoTrack( point );

	return true;
}

bool CActionObjectMultiLine::DoChangeObjects()
{
	CTimeTest	tt;
	CRect	rect = m_pwfi->CalcNoEmptyRect();
	CImageWrp	image( m_image.CreateVImage( rect ), false );
	image.InitRowMasks();

	CWalkFillInfo	wfi( rect );
	wfi.ClearMasks();
	CDC	*pdc = wfi.GetDC();

	pdc->SelectStockObject( NULL_BRUSH );
	CPen	pen( PS_SOLID, m_nLineWidth, RGB( 255, 255, 255 ) );
	CPen	*ppenOld = 
	pdc->SelectObject( &pen );
	pdc->Polyline( &m_points[0], m_points.GetSize() );
	pdc->SelectObject( ppenOld );

	wfi.AttachMasksToImage( image );

	CObjectWrp	object( CreateMeasureObject() );
	object.SetImage( image );

	return true;
}

bool CActionObjectMultiLine::DoStartTracking( CPoint point )
{
	if( m_bTrackByRButton )
		m_nLineWidth = GetValueInt( GetAppUnknown(), "ObjectEditor", "SeparatePenWidth", 3 );
	else
		m_nLineWidth = GetValueInt( GetAppUnknown(), "ObjectEditor", "MergePenWidth", 20 );

	if( m_points.GetSize()==0 )
	{
		m_points.Add( point );
	}
	m_points.Add( point );
	SetLastPoint( point );

	return true;
}

bool CActionObjectMultiLine::DoFinishTracking( CPoint point )
{
	DoTrack( point );
	
	if( m_bFinalizeAfterFirstClick )
		Finalize();
	else
		m_points.Add( point );

	return true;
}

bool CActionObjectMultiLine::DoTrack( CPoint point )
{
	SetLastPoint( point );
	return true;
}

bool CActionObjectMultiLine::DoLButtonDblClick( CPoint pt )
{
	if( !m_bFinalizeAfterFirstClick )
		Finalize();
	return true;
}

void CActionObjectMultiLine::Invalidate()
{
	CPoint	ptMin, ptMax;

	if( !m_points.GetSize() )
		return;

	m_hcurActive = AfxGetApp()->LoadCursor( m_nIDCursor );

	ptMin = m_points[0];
	ptMax = m_points[0];

	for( int i = 0; i < m_points.GetSize(); i++ )
	{
		ptMin.x = min( ptMin.x, m_points[i].x );
		ptMin.y = min( ptMin.y, m_points[i].y );
		ptMax.x = max( ptMax.x, m_points[i].x );
		ptMax.y = max( ptMax.y, m_points[i].y );
	}

	CRect rect( ptMin.x-m_nLineWidth, ptMin.y-m_nLineWidth, ptMax.x+m_nLineWidth+1, ptMax.y+m_nLineWidth+1 );
	rect = ::ConvertToWindow( m_punkTarget, rect );
	CWnd	*pwnd = GetWindowFromUnknown( m_punkTarget );
	if( pwnd )pwnd->InvalidateRect( rect );
}

void CActionObjectMultiLine::SetLastPoint( CPoint point )
{
	Invalidate();

	if( m_points.GetSize() )
		m_points[m_points.GetSize()-1] = point;

	m_pwfi->ClearMasks();
	CDC	*pdc = m_pwfi->GetDC();
	pdc->SelectStockObject( NULL_BRUSH );
	CPen	pen( PS_SOLID, m_nLineWidth, RGB( 255, 255, 255 ) );
	CPen	*ppenOld = 
	pdc->SelectObject( &pen );
	if( m_points.GetSize() )
		pdc->Polyline( &m_points[0], m_points.GetSize() );
	pdc->SelectObject( ppenOld );


	Invalidate();
}

//////////////////////////////////////////////////////////////////////
//CActionObjectEditor implementation
CActionObjectEditor::CActionObjectEditor()
{	
	m_pwfi = 0;
	m_nLineWidth = 5;

	m_bNoProcessOperations = true;

	m_pointLast = CPoint( -1, -1 );
	m_pctrl = 0;
	m_bSeparate = false;
	m_bUseRButton = ::GetValueInt(::GetAppUnknown(), "ObjectEditor", "UseRButton", 1) != 0;
	m_bCanTrackByRButton = m_bUseRButton;
	m_pRgn =0;
	m_bPhased =false;
	m_nPhaseEdit = -1;
}

CActionObjectEditor::~CActionObjectEditor()
{
	if( m_pwfi )	delete m_pwfi;
	if(m_pRgn)		
	{
		m_pRgn->DeleteObject(); 
		delete m_pRgn;
	}
	ASSERT( !m_pctrl );
}

bool CActionObjectEditor::Initialize()
{
	

	AddPropertyPageDescription( c_szCObjectEditorPage );

	if( !CActionObjectBase::Initialize() )return false;
	m_hcurActive = AfxGetApp()->LoadCursor( IDC_EDITOR );

	m_pctrl = new CEditorCtrl();

	DoUpdateSettings();

	m_pctrl->Install();
	return true;
}

bool CActionObjectEditor::DoUpdateSettings()
{
	int nRBack = GetValueInt( GetAppUnknown(), "ObjectEditor", "SeparatePenWidth", 3 );
	int nR = GetValueInt( GetAppUnknown(), "ObjectEditor", "MergePenWidth", 20 );

	m_nPhaseEdit = GetValueInt( GetAppUnknown(), "ObjectEditor", "EditorClassNum", -1 );
	m_colDrawing = GetValueInt(GetAppUnknown(), "ObjectEditor", "EditorColor", RGB(255,0,0) );
	_InitClippingRgn();

	if( m_pctrl )
	{
		m_pctrl->Redraw( false );
		m_pctrl->SetSizes( nR, nRBack );
		m_pctrl->Redraw( true );
	}

	return CActionObjectBase::DoUpdateSettings();
}
void	CActionObjectEditor::_InitClippingRgn()
{
	
	IUnknown* o_list = GetActiveList();
	if(!o_list) return;
	ICompositeObjectPtr comp(o_list);
	long bC;
	comp->IsComposite(&bC);
	if(bC)
	{
		long i = GetValueInt(GetAppUnknown(), "Composite", "EditorClassNum",0);
		IUnknown* unkBase =0;
		comp->GetMainLevelObjects(&unkBase);
		IUnknown* unkCurr =0;
		comp->GetLevelbyClass(i, &unkCurr);
		if(unkCurr == unkBase)
		{
			clear_clipping_region();
			if(unkCurr) unkCurr->Release();
		}
		else
		{
			if(unkCurr) unkCurr->Release();
			if(m_pRgn) 
			{
				m_pRgn->DeleteObject();
			}
			else m_pRgn = new CRgn();
			m_pRgn->CreateRectRgn(-2,-2,-1,-1);

			if(unkBase ==0)
			{
				o_list->Release();
				return;
			}
			INamedDataObject2Ptr ndo(unkBase);

			TPOS pos;
			ndo->GetFirstChildPosition(&pos);
			while(pos)
			{
				IUnknown* object;
				ndo->GetNextChild(&pos, &object);
				IManualMeasureObjectPtr mmo(object);
				if(mmo) 
				{
					object->Release();
					continue;
				}
				IMeasureObject2Ptr mo(object);
				if(mo)
				{
					IUnknown* image;
					mo->GetImage(&image);
					IImage4Ptr imPtr(image);
					if(image) image->Release();
					Contour* c;
					int cCount =0;
					imPtr->GetContoursCount(&cCount);
					for(int i=0;i<cCount;i++)
					{
						imPtr->GetContour(i, &c);
						POINT* pts = new POINT[c->nContourSize];
						for(int i=0;i<c->nContourSize;i++)
						{
							pts[i].x = c->ppoints[i].x;
							pts[i].y = c->ppoints[i].y;
						}

						CRgn rgn;
						rgn.CreatePolygonRgn(pts, c->nContourSize, WINDING);

						delete[] pts;

						m_pRgn->CombineRgn(m_pRgn, &rgn, RGN_XOR);    
						rgn.DeleteObject();
					}

					object->Release();	
				}

			}
			m_pRgn->OffsetRgn(1,1);
		}

	}
	o_list->Release();

}
void CActionObjectEditor::Finalize()
{
	if( m_pctrl )
	{
		m_pctrl->Uninstall();
		m_pctrl->GetControllingUnknown()->Release();
		m_pctrl = 0;
	}

	CActionObjectBase::Finalize();

}


bool CActionObjectEditor::DoChangeObjects()
{
	if( !m_pwfi )
		return false;

	if(m_bPhased)
	{
		bool ret = DoChangePhases(m_pwfi, m_bTrackByRButton || m_bSeparate);
		delete m_pwfi;
		m_pwfi = 0;
		return ret;
	}
		
	//ajust wfi

	CRect	rect = m_pwfi->GetRect();
	CRect	rectCalc = m_pwfi->CalcNoEmptyRect();

	int	x, y;

	for( x = rect.left-1; x < rect.right+1; x++ )
	{
		m_pwfi->SetPixel( x, rect.top-1, 0 );
		m_pwfi->SetPixel( x, rect.bottom, 0 );
	}

	for( y = rect.top-1; y < rect.bottom+1; y++ )
	{
		m_pwfi->SetPixel( rect.left-1, y, 0 );
		m_pwfi->SetPixel( rect.right, y, 0 );
	}

	if( m_bTrackByRButton || m_bSeparate )
	{
		SeparateObjects( m_pwfi );
	}
	else
	{
		MergeObjects( m_pwfi );
	}

	

	delete m_pwfi;
	m_pwfi = 0;
//invalidate the rect
	/*CRect	rectInvalidate = ::ConvertToWindow( m_punkDrawTarget, m_rectInterest );
	CWnd	*pwnd = GetWindowFromUnknown(m_punkDrawTarget);
	pwnd->InvalidateRect( rectInvalidate );
	pwnd->UpdateWindow();*/


	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	rect = ConvertToWindow( m_punkTarget, rectCalc );
	pwnd->InvalidateRect( rect );

	return true;
}

bool CActionObjectEditor::DoLButtonDown( CPoint pt )
{
	// A.M. BT4479. pt already in image coordinates, CMouseImpl::StartTracking requires window
	// coordinates of view.
	CPoint	pointCursor;
	GetCursorPos( &pointCursor );
	//if(m_pRgn!=0) //Kir -- support composite objects
	//{
	//	if(!m_pRgn->PtInRegion(pt)) return true;
	//}
	CWnd	*pwnd = GetWindowFromUnknown( GetTarget() );
	if( pwnd )
		pwnd->ScreenToClient( &pointCursor );
	if (!m_bUseRButton)
	{
		short wKeys = GetAsyncKeyState(VK_MENU);
		m_bSeparate = (wKeys & 0x8000) == 0x8000;
	}
	else
		m_bSeparate = false;
		
	return StartTracking( pointCursor );
}

bool CActionObjectEditor::DoRButtonDown( CPoint pt )
{
	if (!m_bUseRButton)
	{
		_activate_object( pt );
		return true;
	}
	else
	{
		if(m_pRgn!=0) //Kir -- support composite objects
		{
			if(!m_pRgn->PtInRegion(pt)) return true;
		}
	return StartTracking( pt );
}
}
void CActionObjectEditor::clear_clipping_region()
{
	if(m_pRgn)
	{
		m_pRgn->DeleteObject();
		delete m_pRgn;
		m_pRgn =0;
	}
}



bool CActionObjectEditor::DoStartTracking( CPoint pt )
{
	m_image.Attach( GetActiveImage(), false );
	m_rectAction = m_image.GetRect();

	
	if( m_bTrackByRButton || m_bSeparate)
		m_nLineWidth = GetValueInt( GetAppUnknown(), "ObjectEditor", "SeparatePenWidth", 3 );
	else
		m_nLineWidth = GetValueInt( GetAppUnknown(), "ObjectEditor", "MergePenWidth", 20 );
	
	m_penDivider.CreatePen( PS_SOLID, m_nLineWidth, RGB( 255, 255, 255 ) );

	delete m_pwfi;
	m_pwfi = new CWalkFillInfo( m_rectAction );

	CDC *pdc = m_pwfi->GetDC();
	pdc->SelectObject( &m_penDivider );

	_AddLineSegment( pt );
	return true;
}

bool CActionObjectEditor::DoTrack( CPoint pt )
{
	_AddLineSegment( pt );

	return true;
}

bool CActionObjectEditor::DoFinishTracking( CPoint pt )
{
	if( !m_pwfi )return true;
	_AddLineSegment( pt );

	Finalize();

	return true;
}

void CActionObjectEditor::DoDraw( CDC &dc )
{
	if( !m_pbi || !m_pdib )
		return;
	COLORREF	crFill;
	if( m_bTrackByRButton || m_bSeparate )	//separate - fill by objects
		crFill = ::GetValueColor( GetAppUnknown(), "Colors", "Background", RGB( 128, 128, 255 ) );
	else if(m_bPhased)
	{
		crFill = m_colDrawing;
	}
	else					//merge
		crFill = get_class_color( -1 );

	::DrawWfiRect( m_pwfi, 
		m_pbi, 
		m_pdib, 
		m_rcInvalid.TopLeft(), 
		m_rcInvalid, 
		::GetZoom( m_punkTarget ), 
		::GetScrollPos( m_punkTarget ), 
		crFill );

/*	if( !m_pbi || !m_pdib )
		return;//unsupported

	DWORD	dwShowFlags = 0;

	sptrIImageView	sptrV( m_punkTarget );
	sptrV->GetImageShowFlags( &dwShowFlags );

	bool	bFill = true;
	COLORREF	crFill = RGB( 0, 0, 0 );

	if( m_bTrackByRButton )	//separate - fill by objects
	{
		if( dwShowFlags & isfImageBinary )
		{
			crFill = ::GetValueColor( GetAppUnknown(), "Colors", "Background", RGB( 128, 128, 255 ) );
			bFill = true;
		}
		else
			bFill = false;
	}
	else					//merge
	{
		if( dwShowFlags & isfObjectBinary )
		{
			crFill = ::GetObjectClassColor(0, 0);
			bFill = true;
		}
		else
			bFill = false;
	}
	
	byte	red = GetRValue( crFill );
	byte	green = GetGValue( crFill );
	byte	blue = GetBValue( crFill );


	

	CRect	rectClip = m_rcInvalid;
	
	CPoint	pointScroll = GetScrollPos( m_punkTarget );
	double fZoom = GetZoom( m_punkTarget );

	CPoint	pointOffsetRgn = rectClip.TopLeft();
//convert clip to the client coordinates
	rectClip+=pointScroll;

	rectClip.left = (int)::floor( rectClip.left/fZoom );
	rectClip.top = (int)::floor( rectClip.top/fZoom );
	rectClip.right = (int)::ceil( rectClip.right/fZoom );
	rectClip.bottom = (int)::ceil( rectClip.bottom/fZoom );

//get the image rect
	CRect	rectImage;
	CRect	rectDraw;

	rectImage = m_image.GetRect();
//ajust rectangle
	rectImage.left = max( rectClip.left, rectImage.left );
	rectImage.top = max( rectClip.top, rectImage.top );
	rectImage.right = min( rectClip.right, rectImage.right );
	rectImage.bottom = min( rectClip.bottom, rectImage.bottom );

	if( rectImage.left >= rectImage.right ||
		rectImage.top >= rectImage.bottom )
		return;

	rectDraw.left = (int)::floor( rectImage.left*fZoom-pointScroll.x );
	rectDraw.top = (int)::floor( rectImage.top*fZoom-pointScroll.y );
	rectDraw.right = (int)::ceil( rectImage.right*fZoom-pointScroll.x );
	rectDraw.bottom = (int)::ceil( rectImage.bottom*fZoom-pointScroll.y );
	rectDraw-=pointOffsetRgn;


	int	cxDIB = m_pbi->biWidth;
	int	cyDIB = m_pbi->biHeight;
	int	cx4DIB = (cxDIB*3+3)/4*4;

	CPoint	point = m_image.GetOffset();

	int	cx = m_image.GetWidth();
	int	cy = m_image.GetHeight();

	CRect	rectDest = rectDraw;
	CRect	rect = rectImage;

	int	xStart = rectDest.left;
	int	yStart = rectDest.top;
	int	xEnd = rectDest.right;
	int	yEnd = rectDest.bottom;

	xStart = max( 0, min( m_pbi->biWidth, xStart ) );
	xEnd = max( 0, min( m_pbi->biWidth, xEnd ) );
	yStart = max( 0, min( m_pbi->biHeight, yStart ) );
	yEnd = max( 0, min( m_pbi->biHeight, yEnd ) );

	int x, y, xsrc, ysrc;


	int		cxDest = rectDest.right-rectDest.left;
	int		cyDest = rectDest.bottom-rectDest.top;
	int		cxSrc = rect.right-rect.left;
	int		cySrc = rect.bottom-rect.top;

	int	colors = m_image.GetColorsCount(), c;

	byte	*pmask = 0;

	int	nImageColorsOffest = rect.left-point.x-rectDest.left*cxSrc/cxDest;
	int	nImageOffestCY = rect.top-point.y-rectDest.top*cySrc/cyDest;
	int	nInitialFlag = (cxDest>=cxSrc)?cxDest+(rectDest.left%(cxDest/cxSrc))*cxSrc:0;

	byte	*prgb = 0;
	color	**pcnv = new color*[ colors ];

	if( !bFill )
		prgb = new byte[cxSrc*3];

	IUnknown	*punk = 0;
	m_image->GetColorConvertor( &punk );
	sptrIColorConvertor	sptrC( punk );
	punk->Release();

	for( y = yStart; y < yEnd; y++ )
	{
		ysrc = (y-rectDest.top)*cySrc/cyDest+rect.top-point.y;

		pmask = m_pwfi->GetRowMask( ysrc )+nImageColorsOffest;

		if( !bFill )
		{
			for( c = 0; c < colors; c++ )
				pcnv[c] = m_image.GetRow( ysrc, c )+nImageColorsOffest;

			sptrC->ConvertImageToDIB( pcnv, prgb, cxSrc, true );
		}

		byte	*pdib = m_pdib+cx4DIB*(cyDIB-y-1)+xStart*3;
		byte	*pdibSrc = prgb;

		if( cxDest >= cxSrc )
		{
			long	lFlag = nInitialFlag;
			pmask += xStart*cxSrc/cxDest;
			//pdibSrc += xStart*cxSrc/cxDest;

			for( x = xStart; x < xEnd; x++ )
			{										
				lFlag	-= cxSrc;
				if( lFlag < 0 )
				{
					lFlag+=cxDest;
					pdibSrc+=3;
					pmask++;
				}

				//xsrc = x*cxSrc/cxDest;
				if( !*pmask )
				{
					pdib++;
					pdib++;
					pdib++;
					continue;
				}
				
				if( bFill )
				{
					*pdib = blue;
					pdib++;
					*pdib = green;
					pdib++;
					*pdib = red;
					pdib++;	
				}
				else
				{
					*pdib = *(pdibSrc+0);
					pdib++;
					*pdib = *(pdibSrc+1);
					pdib++;
					*pdib = *(pdibSrc+2);
					pdib++;	
				}
			}
		}
		else
		{
			for( x = xStart; x < xEnd; x++ )
			{
				xsrc = (x-xStart)*cxSrc/cxDest;

				if( !pmask[xsrc] )
				{
					pdib++;
					pdib++;
					pdib++;
					continue;
				}
				
				if( !bFill )
				{
					*pdib = pdibSrc[xsrc*3];
					pdib++;
					*pdib = pdibSrc[xsrc*3+1];
					pdib++;
					*pdib = pdibSrc[xsrc*3+2];
					pdib++;
				}
				else
				{
					*pdib = blue;
					pdib++;
					*pdib = green;
					pdib++;
					*pdib = red;
					pdib++;
				}
			}
		}
	}

	delete prgb;
	delete pcnv;*/
}



void OnePixelLine( int x1, int y1, int x2, int y2, LPARAM param, void (*pfunc)( int x, int y, LPARAM param ) )
{
	int	distX = x2-x1;if(distX<0)distX=-distX;
	int	distY = y2-y1;if(distY<0)distY=-distY;

	int	incX = (x2-x1>0)?1:-1;
	int	incY = (y2-y1>0)?1:-1;
	
	int	flag = 0;

	if( distX>=distY )
	{
		do
		{
			x1 += incX;

			pfunc( x1, y1, param );
			if( flag < 0 )
			{
				y1 += incY;
				flag += distX;
				pfunc( x1, y1, param );
			}
			flag -= distY;
		}
		while( x1 != x2 );

		if( y1 != y2 )
			pfunc( x2, y2, param );
	}
	else
	{
		do
		{
			y1 += incY;

			pfunc( x1, y1, param );
			if( flag < 0 )
			{
				x1 += incX;
				flag += distY;
				pfunc( x1, y1, param );
			}
			flag -= distX;
		}
		while( y1 != y2 );

		if( x1 != x2 )
			pfunc( x2, y2, param );
	}
}

void _funcWFI( int x, int y, LPARAM param )
{
	CWalkFillInfo *pwfi	=  (CWalkFillInfo *)param;
	if( !pwfi->GetRect().PtInRect( CPoint( x, y ) ) )
		return;
	pwfi->SetPixel( x, y, 255 );
}

void LineWFI( CPoint pt1, CPoint pt2, CWalkFillInfo *pwfi )
{
	OnePixelLine( pt1.x, pt1.y, pt2.x, pt2.y, (LPARAM)pwfi, _funcWFI );
}

void CActionObjectEditor::_AddLineSegment( CPoint point )
{
	if( m_pointLast == point )
		return;

	if( !m_pwfi )
		return;

//update wfi
	CDC *pdc = m_pwfi->GetDC();
	pdc->SelectClipRgn(m_pRgn);
	pdc->SelectObject( &m_penDivider );

/*if (m_pointLast.x < 20 && m_pointLast != CPoint(-1,-1))
{
	CString s;
	s.Format("%d %d", m_pointLast.x, m_pointLast.y);
	AfxMessageBox(s);
}*/

	if( m_nLineWidth <= 1 && (m_bTrackByRButton || m_bSeparate) )
	{
		if( m_pointLast != CPoint( -1, -1  ) )
			LineWFI( point, m_pointLast, m_pwfi );
	}
	else
	{
		if( m_pointLast != CPoint( -1, -1  ) )
		{
			pdc->MoveTo( m_pointLast );
			pdc->LineTo( point );
		}
		else
		{
			CPen pen(PS_SOLID, 0, RGB(255,255,255));
			CBrush brush(RGB(255,255,255));
			CPen *pOldPen = pdc->SelectObject(&pen);
			CBrush *pBrush = pdc->SelectObject(&brush);
			int n = m_nLineWidth%2;
			pdc->Ellipse(point.x-m_nLineWidth/2,point.y-m_nLineWidth/2,
				point.x+m_nLineWidth/2+n,point.y+m_nLineWidth/2+n);
			pdc->SelectObject(pBrush);
			pdc->SelectObject(pOldPen);
//			pdc->MoveTo( point );
//			pdc->LineTo( CPoint(point.x+1,point.y) );
		}

	}

	CPoint pointLast = m_pointLast!=CPoint(-1,-1)?m_pointLast:point;
	CRect	rect;
	rect.left = min( point.x, pointLast.x )-m_nLineWidth;
	rect.top = min( point.y, pointLast.y )-m_nLineWidth;
	rect.right = max( point.x, pointLast.x )+m_nLineWidth;
	rect.bottom = max( point.y, pointLast.y )+m_nLineWidth;

	rect = ConvertToWindow( m_punkTarget, rect );

	m_pointLast = point;
/*if (m_pointLast.x < 20)
{
	CString s;
	s.Format("%d %d !!!", m_pointLast.x, m_pointLast.y);
	AfxMessageBox(s);
}*/
	pdc->SelectClipRgn(NULL);

	CWnd	*pwnd = GetWindowFromUnknown( m_punkTarget );
	pwnd->InvalidateRect( rect );
	pwnd->UpdateWindow();
}

void CActionObjectEditor::MergeObjects( CWalkFillInfo *pwfi )
{
	_CreateIntersectList( pwfi );
//create a merged list

	POSITION	pos = m_listIntersect.GetHeadPosition();
	while( pos )
	{
		CObjectWrp	object( (IUnknown*)m_listIntersect.GetNext( pos ) );
		CImageWrp	image( object.GetImage(), false );

		CRect	rectObject = image.GetRect();

		int	x, y;

		for( y = rectObject.top; y < rectObject.bottom; y++ )
		{
			byte	*pimageMask = image.GetRowMask( y - rectObject.top );
			byte	*psourceMask = pwfi->GetRowMask( y )+rectObject.left;

			for( x = rectObject.left; x < rectObject.right; x++ )
			{
				if( *pimageMask== 0xFF )
					*psourceMask = 0xFF;
				pimageMask++;
				psourceMask++;
			}
		}
	}

	MeasureFillInfo( m_pwfi, m_rectInterest );
//remove intersect objects from list
	pos = m_listIntersect.GetHeadPosition();

	while( pos )
	{
		IUnknown	*punk = (IUnknown*)m_listIntersect.GetNext( pos );
		DeleteMeasureObject( punk );
	}

	_DestroyIntersectList();
}

void CActionObjectEditor::SeparateObjects( CWalkFillInfo *pwfi )
{
	_CreateIntersectList( pwfi );

	POSITION	pos = m_listIntersect.GetHeadPosition();

	while( pos )
	{
		CObjectWrp	object( (IUnknown*)m_listIntersect.GetNext( pos ) );
		CImageWrp	image( object.GetImage(), false );

		CWalkFillInfo wfi( image );

		CRect	rectObject = image.GetRect();
		int		x, y;

		for( y = rectObject.top; y < rectObject.bottom; y++ )
		{
			byte	*pimageMask = wfi.GetRowMask( y - rectObject.top );
			byte	*psourceMask = pwfi->GetRowMask( y )+rectObject.left;

			for( x = rectObject.left; x < rectObject.right; x++ )
			{
				if( *psourceMask )
					*pimageMask = 0;
				psourceMask++;
				pimageMask++;

			}
		}
		MeasureFillInfo( &wfi );


	}
//remove intersect objects from list
	pos = m_listIntersect.GetHeadPosition();

	while( pos )
	{
		IUnknown	*punk = (IUnknown*)m_listIntersect.GetNext( pos );
		DeleteMeasureObject( punk );
	}

	_DestroyIntersectList();
}

void CActionObjectEditor::MeasureFillInfo( CWalkFillInfo *pwfi, CRect rectBounds )
{
	CPoint	point( -1, -1 );

	IUnknown* punkImage = 0;
	
	while( pwfi->InitContours(true, point, &point, rectBounds, m_image, &punkImage) )
	{
		CImageWrp	image(punkImage, false);
		CRect	rectObject = pwfi->CalcContoursRect();
		_InitObject( pwfi, rectObject );

		CObjectWrp	object( CreateMeasureObject() );
		object.SetImage( image );


		pwfi->ClearContours();
	}
}

void CActionObjectEditor::_InitObject( CWalkFillInfo *pwfi, CRect rectObject )
{
	//clear mask
	for( long y = rectObject.top-1; y < rectObject.bottom+1; y++ )
	{
		if( pwfi->GetPixel( rectObject.left-1, y ) == 1 )
			pwfi->SetPixel( rectObject.left-1, y, 0 );
		if( pwfi->GetPixel( rectObject.right, y ) == 1 )
			pwfi->SetPixel( rectObject.right, y, 0 );
	}
	for( long x = rectObject.left-1; x < rectObject.right+1; x++ )
	{
		if( pwfi->GetPixel( x, rectObject.top-1 ) == 1 )
			pwfi->SetPixel( x, rectObject.top-1, 0 );
		if( pwfi->GetPixel( x, rectObject.bottom ) == 1 )
			pwfi->SetPixel( x, rectObject.bottom, 0 );
	}
}

CActionObjectEditorSpec::CActionObjectEditorSpec()
{
	m_bCanTrackByRButton = false;
}


bool CActionObjectEditorSpec::Initialize()
{
	bool b = CActionObjectEditor::Initialize();
	if (b)
		m_hcurActive = AfxGetApp()->LoadCursor( IDC_EDITOR_SPEC );
	return b;
}


//////////////////////////////////////////////////////////////////////
//CActionObjectEllipseDirect implementation
CActionObjectEllipseDirect::CActionObjectEllipseDirect()
{
}

CActionObjectEllipseDirect::~CActionObjectEllipseDirect()
{
}

bool CActionObjectEllipseDirect::Initialize()
{
	if( !CActionObjectRectDirect::Initialize() )return false;
	m_hcurActive = AfxGetApp()->LoadCursor( IDC_ELLIPSE_OBJECT_BIG );
	m_hcurAdd = AfxGetApp()->LoadCursor( IDC_ELLIPSE_BIG_ADD );
	m_hcurSub = AfxGetApp()->LoadCursor( IDC_ELLIPSE_BIG_SUB );
	return true;
}

bool CActionObjectEllipseDirect::DoLButtonUp( CPoint pt )
{
	bool	bReturn = CActionObjectRectDirect::DoLButtonUp( pt );

	if( m_stagedef == defineWidth )
	{
		m_hcurActive = AfxGetApp()->LoadCursor( IDC_ELLIPSE_OBJECT_SMALL );
		m_hcurAdd = AfxGetApp()->LoadCursor( IDC_ELLIPSE_SMALL_ADD );
		m_hcurSub = AfxGetApp()->LoadCursor( IDC_ELLIPSE_SMALL_SUB );
	}

	return bReturn;
}
	
void CActionObjectEllipseDirect::CalcEllipse( CPoint *ppt )
{
	double	fAngle = ::atan2( (double)(m_point2.y-m_point1.y), (double)(m_point2.x-m_point1.x) );
	double width = ::CalcLen( (double)(m_point2.y-m_point1.y), (double)(m_point2.x-m_point1.x) );
	double height = m_nWidth;
	double	cx = (m_point1.x+m_point2.x)/2.;
	double	cy = (m_point1.y+m_point2.y)/2.;


    // MAGICAL CONSTANT to map ellipse to beziers
    //  			2/3*(sqrt(2)-1) 
    const double EToBConst =	0.2761423749154; 

//  Use the following line instead for mapping systems where +ve Y is upwards
	double	xx[13], yy[13];

    xx[0]  =                            //------------------------/
    xx[1]  =                            //                        /
    xx[11] = cx-width/2;                //        2___3___4       /
    xx[5]  =                            //     |             |    /
    xx[6]  =                            //     |             |    /
    xx[7]  = cx+width/2;                //     0,12          6    /
    xx[2]  =                            //     |             |    /
    xx[10] = cx - width*EToBConst;      //     |             |    /
    xx[4]  =                            //    11             7    /
    xx[8]  = cx + width*EToBConst;      //       10___9___8       /
    xx[3]  =                            //                        /
    xx[9]  = cx;		                //------------------------*

    yy[2]  =
    yy[3]  =
    yy[4]  = cy-height/2;
    yy[8]  =
    yy[9]  =
    yy[10] = cy+height/2;
    yy[7]  =
    yy[11] = cy + height*EToBConst;
    yy[1]  =
    yy[5]  = cy - height*EToBConst;
    yy[0]  =
    yy[6]  = cy;

	

	//rotate
	double sinAng           = sin(-fAngle);
    double cosAng           = cos(-fAngle);

	int	n;

	for( n = 0; n < 13; n++ )
	{
		ppt[n].x = int( (xx[n]-cx)*cosAng+(yy[n]-cy)*sinAng+cx +.5);
		ppt[n].y = int( -(xx[n]-cx)*sinAng+(yy[n]-cy)*cosAng+cy +.5);
	}

	ppt[12] = ppt[0];
}

void CActionObjectEllipseDirect::DoDraw( CDC &dc )
{
	if( m_stagedef == defineNothing )
		return;

	CPoint	pt[13];
	CalcEllipse( pt );

	for( int n = 0; n < 13; n++ )
		pt[n] = ::ConvertToWindow( m_punkTarget, pt[n] );

	dc.PolyBezier( pt, 13 );
	dc.MoveTo( pt[0] );
	dc.LineTo( pt[6] );
	dc.MoveTo( pt[3] );
	dc.LineTo( pt[9] );
}

static bool IsImageEmpty(CImageWrp	&image)
{
	int cx,cy;
	image->GetSize(&cx,&cy);
	for (int y = 0; y < cy; y++)
	{
		byte *pMask = image.GetRowMask(y);
		for (int x = 0; x < cx; x++)
		{
			if (pMask[x] & 0x80)
				return false;
		}
	}
	return true;
}

bool CActionObjectEllipseDirect::DoChangeObjects()
{
	CRect	rect;
	CPoint	pt[13];
	CalcEllipse( pt );

	rect.left = pt[12].x;
	rect.right = pt[12].x+1;
	rect.top = pt[12].y;
	rect.bottom = pt[12].y+1;

	for( int n = 0; n < 12; n++ )
	{
		rect.left = min( rect.left, pt[n].x );
		rect.right = max( rect.right, pt[n].x+1 );
		rect.top = min( rect.top, pt[n].y );
		rect.bottom = max( rect.bottom, pt[n].y+1 );
	}
	int nMinSize = GetValueInt(GetAppUnknown(), "\\ObjectEditor", "MinSize", 2);
	if (nMinSize < 2) nMinSize = 2;
	if (rect.Width() < nMinSize && rect.Height() < nMinSize ||
		m_nWidth < nMinSize-1)
		return false;

	CImageWrp	image( m_image.CreateVImage( rect ), false );
	image.InitRowMasks();

	CWalkFillInfo	wfi( rect );
	CDC	*pDC = wfi.GetDC();
	pDC->SelectStockObject( WHITE_PEN );
	pDC->SelectStockObject( WHITE_BRUSH );

	pDC->BeginPath();
	pDC->PolyBezier( pt, 13 );
	pDC->EndPath();
	pDC->FillPath();
	wfi.AttachMasksToImage( image );

	// A.M. BT4974
	if (IsImageEmpty(image))
	{
		for (int i = 0; i < 13; i++)
		{
			int y = pt[i].y-rect.top;
			int x = pt[i].x-rect.left;
			byte *pMask = image.GetRowMask(y);
			pMask[x] = 0xFF;
		}
	}


	CObjectWrp	object( CreateMeasureObject() );
	object.SetImage( image );

	return true;
}
//////////////////////////////////////////////////////////////////////
//CActionObjectEllipseFixed implementation


CActionObjectEllipseFixed::CActionObjectEllipseFixed()
{
	m_left = m_top = m_width = m_height = -1;
	m_calibr = 1;
	m_bCalibr = false;
	m_pctrl = 0;
}

CActionObjectEllipseFixed::~CActionObjectEllipseFixed()
{
	ASSERT(m_pctrl==0);
}

bool CActionObjectEllipseFixed::DoUpdateSettings()
{
	m_width = GetValueDouble( GetAppUnknown(), szFixedObjectSizes, szEllipseCX, 100 );
	m_height = GetValueDouble( GetAppUnknown(), szFixedObjectSizes, szEllipseCY, 100 );
	m_bCalibr = GetValueInt( GetAppUnknown(), szFixedObjectSizes, szUseCalubration, 0 )!=0;
	UpdateCalibration();

	if( m_pctrl )
	{
		m_pctrl->Redraw( false );
		m_pctrl->SetSize( CalcPixelSize() );
		m_pctrl->Redraw( true );
	}
	return CActionObjectBase::DoUpdateSettings();
}

bool CActionObjectEllipseFixed::Initialize()
{
	AddPropertyPageDescription( c_szCObjectGeneralPage );
	AddPropertyPageDescription( c_szCFixedEllipsePage );

	if( !CActionObjectBase::Initialize() )return false;

	m_left = GetArgumentDouble( _T("X") );
	m_top = GetArgumentDouble( _T("Y") );
	m_width = GetArgumentDouble( _T("Width") );
	m_height = GetArgumentDouble( _T("Height") );
	m_bCalibr = GetArgumentInt( _T("UseCalibration") )!=0;
	UpdateCalibration();

	m_pctrl = new CFixedEllipseCtrl();
	m_pctrl->SetSize( CalcPixelSize() );
	m_pctrl->Install();
	
	if( m_width>0 )
	{
		Finalize();
		return false;
	}
	else
		DoUpdateSettings();
	return true;
}

void CActionObjectEllipseFixed::StoreCurrentExecuteParams()
{
	SetArgument( _T("X"), _variant_t(m_left) );
	SetArgument( _T("Y"), _variant_t(m_top) );
	SetArgument( _T("Width"), _variant_t(m_width) );
	SetArgument( _T("Height"), _variant_t(m_height) );
	SetArgument( _T("UseCalibration"), _variant_t((long)m_bCalibr) );
}


bool CActionObjectEllipseFixed::DoChangeObjects()
{
	CRect	rect = CalcPixelRect();
	if (rect.Width() <= 1 && rect.Height() <= 1) return false;

	CImageWrp	image( m_image.CreateVImage( rect ), false );
	image.InitRowMasks();

	CWalkFillInfo	wfi( rect );
	CDC	*pDC = wfi.GetDC();
	pDC->SelectStockObject( WHITE_PEN );
	pDC->SelectStockObject( WHITE_BRUSH );
	pDC->Ellipse( rect );

	wfi.AttachMasksToImage( image );

	CObjectWrp	object( CreateMeasureObject() );
	object.SetImage( image );

	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionObjectRectDirect implementation
CActionObjectRectDirect::CActionObjectRectDirect()
{
	m_point1 = CPoint( 0, 0 );
	m_point2 = CPoint( 0, 0 );
	m_nWidth = 0;

	m_stagedef = defineNothing;
}

CActionObjectRectDirect::~CActionObjectRectDirect()
{
}

bool CActionObjectRectDirect::Initialize()
{
	AddPropertyPageDescription( c_szCObjectGeneralPage );
	if( !CActionObjectBase::Initialize() )return false;

	m_point1.x = GetArgumentInt( _T("X1") );
	m_point1.y = GetArgumentInt( _T("Y1") );
	m_point2.x = GetArgumentInt( _T("X2") );
	m_point2.y = GetArgumentInt( _T("Y2") );
	m_nWidth = GetArgumentInt( _T("Width") );

	if( m_nWidth != -1 )
	{
		Finalize();
		return false;
	}

	m_hcurActive = AfxGetApp()->LoadCursor( IDC_RECT_OBJECT_BIG );
	m_hcurAdd = AfxGetApp()->LoadCursor( IDC_RECT_OBJECT_BIG_ADD );
	m_hcurSub = AfxGetApp()->LoadCursor( IDC_RECT_OBJECT_BIG_SUB );
	return true;
}

void CActionObjectRectDirect::StoreCurrentExecuteParams()
{
	SetArgument( _T("X1"), _variant_t( m_point1.x ) );
	SetArgument( _T("Y1"), _variant_t( m_point1.y ) );
	SetArgument( _T("X2"), _variant_t( m_point2.x ) );
	SetArgument( _T("Y2"), _variant_t( m_point2.y ) );
	SetArgument( _T("Width"), _variant_t( (long)m_nWidth ) );
}


bool CActionObjectRectDirect::DoChangeObjects()
{
	int nMinSize = GetValueInt(GetAppUnknown(), "\\ObjectEditor", "MinSize", 2);
	if (nMinSize < 2) nMinSize = 2;
	if (m_point2.y-m_point1.y < nMinSize-1 && m_point2.x-m_point1.x < nMinSize-1 &&
		m_nWidth < nMinSize-1)
		return false;
	double	fAngle = ::atan2((double)(m_point2.y-m_point1.y), (double)(m_point2.x-m_point1.x));
	CPoint	pt[4];

	pt[0].x = int( m_point1.x+cos( fAngle+A90 )*m_nWidth/2+.5 );
	pt[0].y = int( m_point1.y+sin( fAngle+A90 )*m_nWidth/2+.5 );
	pt[1].x = int( m_point1.x+cos( fAngle-A90 )*m_nWidth/2+.5 );
	pt[1].y = int( m_point1.y+sin( fAngle-A90 )*m_nWidth/2+.5 );
	pt[2].x = int( m_point2.x+cos( fAngle-A90 )*m_nWidth/2+.5 );
	pt[2].y = int( m_point2.y+sin( fAngle-A90 )*m_nWidth/2+.5 );
	pt[3].x = int( m_point2.x+cos( fAngle+A90 )*m_nWidth/2+.5 );
	pt[3].y = int( m_point2.y+sin( fAngle+A90 )*m_nWidth/2+.5 );

	CRect	rect = CRect( pt[0].x-1, pt[0].y-1, pt[0].x+1, pt[0].y+1 );

	rect.left   = min( pt[1].x-1, rect.left );
	rect.right  = max( pt[1].x+1, rect.right );
	rect.top    = min( pt[1].y-1, rect.top );
	rect.bottom = max( pt[1].y+1, rect.bottom );

	rect.left   = min( pt[2].x-1, rect.left );
	rect.right  = max( pt[2].x+1, rect.right );
	rect.top    = min( pt[2].y-1, rect.top );
	rect.bottom = max( pt[2].y+1, rect.bottom );

	rect.left   = min( pt[3].x-1, rect.left );
	rect.right  = max( pt[3].x+1, rect.right );
	rect.top    = min( pt[3].y-1, rect.top );
	rect.bottom = max( pt[3].y+1, rect.bottom );

	CImageWrp	image( m_image.CreateVImage( rect ), false );
	image.InitRowMasks();

	if (rect.Width() == 2 && rect.Height() == 2)
	{
		byte *pMask = image.GetRowMask(0);
		pMask[0] = pMask[1] = 0;
		pMask = image.GetRowMask(1);
		pMask[0] = 0;
		pMask[1] = 255;
	}
	else
	{
	CWalkFillInfo	wfi( rect );
	CDC	*pDC = wfi.GetDC();
	pDC->SelectStockObject( WHITE_PEN );
	pDC->SelectStockObject( WHITE_BRUSH );
	pDC->Polygon( pt, 4 );

	wfi.AttachMasksToImage( image );
	}

	CObjectWrp	object( CreateMeasureObject() );
	object.SetImage( image );

	return true;
}

void CActionObjectRectDirect::DoDraw( CDC &dc )
{
	double	fAngle = ::atan2( double(m_point2.y-m_point1.y), double(m_point2.x-m_point1.x));

	CPoint	pt[4];

	pt[0].x = int( m_point1.x+cos( fAngle+A90 )*m_nWidth/2+.5 );
	pt[0].y = int( m_point1.y+sin( fAngle+A90 )*m_nWidth/2+.5 );
	pt[1].x = int( m_point1.x+cos( fAngle-A90 )*m_nWidth/2+.5 );
	pt[1].y = int( m_point1.y+sin( fAngle-A90 )*m_nWidth/2+.5 );
	pt[2].x = int( m_point2.x+cos( fAngle-A90 )*m_nWidth/2+.5 );
	pt[2].y = int( m_point2.y+sin( fAngle-A90 )*m_nWidth/2+.5 );
	pt[3].x = int( m_point2.x+cos( fAngle+A90 )*m_nWidth/2+.5 );
	pt[3].y = int( m_point2.y+sin( fAngle+A90 )*m_nWidth/2+.5 );

	for( int n=0; n < 4; n++ )
		pt[n] = ::ConvertToWindow( m_punkTarget, pt[n] );

	dc.Polygon( pt, 4 );

	CPoint	pt1, pt2;

	pt1 = ::ConvertToWindow( m_punkTarget, m_point1 );
	pt2 = ::ConvertToWindow( m_punkTarget, m_point2 );

	dc.MoveTo( pt1 );
	dc.LineTo( pt2 );
}

bool CActionObjectRectDirect::DoStartTracking( CPoint pt )
{
	if( m_stagedef == defineNothing )
		m_stagedef = defineLength;
	if( m_stagedef != defineLength )
		return false;
	

	m_point1 = pt;
	m_point2 = pt;

	_Draw();

	return true;
}

bool CActionObjectRectDirect::DoTrack( CPoint pt )
{
	if( m_stagedef != defineLength )
		return false;

	_Draw();

	m_point2 = pt;

	_Draw();

	return true;
}

bool CActionObjectRectDirect::DoFinishTracking( CPoint pt )
{
	if( m_stagedef != defineLength )
		return false;
	return true;
}

bool CActionObjectRectDirect::DoMouseMove( CPoint pt )
{
	if( m_stagedef != defineWidth )
		return false;

	_Draw();
	CPoint	point = ::CalcNearestLocation( m_point1, m_point2, pt );
	m_nWidth = int( ::CalcLen( point.x-pt.x, point.y-pt.y )*2+.5 );
	_Draw();


	return true;
}

bool CActionObjectRectDirect::DoLButtonUp( CPoint pt )
{
	if( m_stagedef == defineLength )
	{
		m_stagedef = defineWidth;
		m_hcurActive = AfxGetApp()->LoadCursor( IDC_RECT_OBJECT_SMALL );
		m_hcurAdd = AfxGetApp()->LoadCursor( IDC_RECT_OBJECT_SMALL_ADD );
		m_hcurSub = AfxGetApp()->LoadCursor( IDC_RECT_OBJECT_SMALL_SUB );
		

		return true;
	}
	if( m_stagedef == defineWidth )
	{
		Finalize();
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////
//CActionObjectRectFixed implementation
CActionObjectRectFixed::CActionObjectRectFixed()
{
	m_left = m_top = m_width = m_height = -1;
	m_calibr = 1;
	m_bCalibr = false;
	m_pctrl = 0;
}

CActionObjectRectFixed::~CActionObjectRectFixed()
{
	ASSERT(m_pctrl==0);
}

void CActionObjectRectFixed::StoreCurrentExecuteParams()
{
	SetArgument( _T("X"), _variant_t(m_left) );
	SetArgument( _T("Y"), _variant_t(m_top) );
	SetArgument( _T("Width"), _variant_t(m_width) );
	SetArgument( _T("Height"), _variant_t(m_height) );
	SetArgument( _T("UseCalibration"), _variant_t((long)m_bCalibr) );
}

bool CActionObjectRectFixed::Initialize()
{
	AddPropertyPageDescription( c_szCObjectGeneralPage );
	AddPropertyPageDescription( c_szCFixedRectPage );

	if( !CActionObjectBase::Initialize() )return false;

	m_left = GetArgumentDouble( _T("X") );
	m_top = GetArgumentDouble( _T("Y") );
	m_width = GetArgumentDouble( _T("Width") );
	m_height = GetArgumentDouble( _T("Height") );
	m_bCalibr = GetArgumentInt( _T("UseCalibration") )!=0;
	UpdateCalibration();

	m_pctrl = new CFixedRectCtrl();
	m_pctrl->SetSize( CalcPixelSize() );
	m_pctrl->Install();

	if( m_width>0 )
	{
		Finalize();
		return false;
	}
	else
		DoUpdateSettings();
	return true;
}

bool CActionObjectRectFixed::DoLButtonDown( CPoint pt )
{
	if( CanActivateObjectOnLbutton() )
		ActivateObject( pt );

	m_left = pt.x*m_calibr-m_width/2;
	m_top = pt.y*m_calibr-m_height/2;


	Finalize();
	return true;
}

bool CActionObjectRectFixed::DoUpdateSettings()
{
	m_width = GetValueDouble( GetAppUnknown(), szFixedObjectSizes, szRectCX, 100 );
	m_height = GetValueDouble( GetAppUnknown(), szFixedObjectSizes, szRectCY, 100 );
	m_bCalibr = GetValueInt( GetAppUnknown(), szFixedObjectSizes, szUseCalubration, 0 )!=0;
	UpdateCalibration();

	if( m_pctrl )
	{
		m_pctrl->Redraw( false );
		m_pctrl->SetSize( CalcPixelSize() );
		m_pctrl->Redraw( true );
	}

	return CActionObjectBase::DoUpdateSettings();
}

void CActionObjectRectFixed::Finalize()
{
	if( m_pctrl )
	{
		m_pctrl->Uninstall();
		m_pctrl->GetControllingUnknown()->Release();
		m_pctrl = 0;
	}

	CActionObjectBase::Finalize();
}
bool CActionObjectRectFixed::DoChangeObjects()
{
	if( m_width < 0. || m_height < 0. )	return false;
	if( m_width < 2. && m_height < 2. ) return false;

	CRect	rect = CalcPixelRect();

	CImageWrp	image( m_image.CreateVImage( rect ), false );
	image.InitRowMasks();

	CWalkFillInfo	wfi( rect );
	CDC	*pDC = wfi.GetDC();
	pDC->SelectStockObject( WHITE_PEN );
	pDC->SelectStockObject( WHITE_BRUSH );
	pDC->Rectangle( rect );

	wfi.AttachMasksToImage( image );

	CObjectWrp	object( CreateMeasureObject() );
	object.SetImage( image );

	return true;
}

void CActionObjectRectFixed::UpdateCalibration()
{
	m_calibr = 1;
	if( m_bCalibr ) 
	{
		ICalibrPtr	ptrC = m_image;
		if( ptrC!= 0 )
		{
			ptrC->GetCalibration( &m_calibr, 0 );
			double	fUnitC = ::GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
			m_calibr*=fUnitC;
		}
	}
}

CRect CActionObjectRectFixed::CalcPixelRect()
{
	CRect	rect( int( m_left/m_calibr ), int( m_top/m_calibr ),
		int( (m_left+m_width)/m_calibr ), int( (m_top+m_height)/m_calibr ) );
	return rect;
}

//////////////////////////////////////////////////////////////////////
//CActionObjectFreeLine implementation
CActionObjectFreeLine::CActionObjectFreeLine()
{
	m_pcontour = 0;
	m_bTracking = false;
	m_nFreeMode = fmUndef;
}

CActionObjectFreeLine::~CActionObjectFreeLine()
{
	::ContourDelete( m_pcontour );
}

bool CActionObjectFreeLine::DoChangeObjects()
{
	CRect	rect = ::ContourCalcRect( m_pcontour );
	int nMinSize = GetValueInt(GetAppUnknown(), "\\ObjectEditor", "MinSize", 2);
	if (nMinSize < 2) nMinSize = 2;
	if (rect.Width() < nMinSize && rect.Height() < nMinSize)
		return false;

	CImageWrp	image( m_image.CreateVImage( rect ), false );
	image.InitRowMasks();

	CWalkFillInfo	wfi( rect );
	CDC	*pDC = wfi.GetDC();
	::ContourDraw( *pDC, m_pcontour, 0, cdfClosed|cdfFill );

	wfi.AttachMasksToImage( image );

	CObjectWrp	object( CreateMeasureObject() );
	object.SetImage( image );

	return true;
}

bool CActionObjectFreeLine::Initialize()
{
	AddPropertyPageDescription( c_szCObjectFreePage );
	if( !CActionObjectBase::Initialize() )return false;
	m_hcurActive = AfxGetApp()->LoadCursor( IDC_FREELINE_OBJECT );
	m_hcurAdd = AfxGetApp()->LoadCursor( IDC_FREE_ADD );
	m_hcurSub = AfxGetApp()->LoadCursor( IDC_FREE_SUB );
	m_pcontour = ::ContourCreate();
	return true;
}

bool CActionObjectFreeLine::DoLButtonDown( CPoint pt )
{
	if( CanActivateObjectOnLbutton() )
		ActivateObject( pt );

	_Draw();
	::ContourAddPoint( m_pcontour, pt.x, pt.y );
	_Draw();

	return false;
}

bool CActionObjectFreeLine::DoRButtonDown( CPoint pt ) 
{ 
	//_activate_object( pt );
	return CActionObjectBase::DoRButtonDown( pt );
}

bool CActionObjectFreeLine::DoLButtonUp( CPoint pt )
{
	if( GetState() != asActive )return false;
	_Draw();
	::ContourAddPoint( m_pcontour, pt.x, pt.y );
	_Draw();

	return true;
}

bool CActionObjectFreeLine::DoLButtonDblClick( CPoint pt )
{
	if( m_pcontour->nContourSize  )
		Finalize();
	return true;
}

bool CActionObjectFreeLine::DoMouseMove( CPoint pt )
{
	if( m_pcontour->nContourSize > 0 && !m_bTracking )
	{
		_Draw();

		m_pcontour->ppoints[m_pcontour->nContourSize-1].x = (short)pt.x;
		m_pcontour->ppoints[m_pcontour->nContourSize-1].y = (short)pt.y;

		_Draw();
		return CActionObjectBase::DoMouseMove(pt); //AAM - перенесем сюда, а в прочих случаях вызываем старую

	}
	//AAM return true;
	return CActionObjectBase::DoMouseMove(pt); //AAM
}

bool CActionObjectFreeLine::DoStartTracking( CPoint pt )
{
	m_bTracking = true;
	return true;
}

bool CActionObjectFreeLine::DoTrack( CPoint pt )
{
	ASSERT(m_pcontour);
	ASSERT(m_pcontour->nContourSize > 0);

	ContourPoint	cpt = m_pcontour->ppoints[m_pcontour->nContourSize-1];

	if( cpt.x  == pt.x && cpt.y == pt.y )
		return true;

	_Draw();
	::ContourAddPoint( m_pcontour, pt.x, pt.y );
	_Draw();

	return true;
}

bool CActionObjectFreeLine::DoFinishTracking( CPoint pt )
{
	ASSERT(m_pcontour);
	ASSERT(m_pcontour->nContourSize > 0);

	ContourPoint	cpt = m_pcontour->ppoints[m_pcontour->nContourSize-1];

	TRACE( "Finish at %d %d %d %d\n", cpt.x, cpt.y, pt.x, pt.y );

	if( cpt.x != pt.x && cpt.y != pt.y )
	{
		_Draw();
		::ContourAddPoint( m_pcontour, pt.x, pt.y );
		_Draw();
	}

	m_bTracking = false;

	if( m_nFreeMode == fmClick )
	{
		//_Draw();
		Finalize();
	}

	return true;
}

void CActionObjectFreeLine::DoDraw( CDC &dc )
{
	::ContourDraw( dc, m_pcontour, m_punkTarget, 0 );
}

bool CActionObjectFreeLine::DoUpdateSettings()
{
	m_nFreeMode = (FreeMode)::GetValueInt( GetAppUnknown(), "ObjectEditor", "FreeMode", fmClick );
	return CActionObjectBase::DoUpdateSettings();
}

//////////////////////////////////////////////////////////////////////
//CActionObjectSpline implementation
CActionObjectSpline::CActionObjectSpline()
{
	m_pcontour  = 0;
	m_bDrawBoxes = true;
}

CActionObjectSpline::~CActionObjectSpline()
{
	::ContourDelete( m_pcontour );
}

bool CActionObjectSpline::DoChangeObjects()
{
	CWalkFillInfo	wfi( m_image.GetRect() );

	CDC	*pDC = wfi.GetDC();
	::ContourDraw( *pDC, m_pcontour, 0, cdfSpline|cdfClosed|cdfFill );

	CRect	rect;
	bool	bInit = false;

	CRect	rectAll = wfi.GetRect();

	int	x, y;
	for( y = rectAll.top; y < rectAll.bottom; y++ )
		for( x = rectAll.left; x < rectAll.right; x++ )
			if( wfi.GetPixel( x, y ) )
			{
				if( !bInit )
				{
					rect.left = x;
					rect.top = y;
					rect.right = x+1;
					rect.bottom = y+1;
					bInit = true;
				}
				else
				{
					rect.left = min( x, rect.left );
					rect.top = min( y, rect.top );
					rect.right = max( x+1, rect.right );
					rect.bottom = max( y+1, rect.bottom );
				}
			}
	if (!bInit) return false;
	int nMinSize = GetValueInt(GetAppUnknown(), "\\ObjectEditor", "MinSize", 2);
	if (nMinSize < 2) nMinSize = 2;
	if (rect.Width() < nMinSize && rect.Height() < nMinSize)
		return false;


	CImageWrp	image( m_image.CreateVImage( rect ), false );
	image.InitRowMasks();

	int	cx = image.GetWidth();
	int	cy = image.GetHeight();

	for( y = 0; y < cy; y++ )
	{
		byte	*pbyte = image.GetRowMask( y );
		for( x = 0; x < cx; x++ )
		{
			if( wfi.GetPixel( x+rect.left, y+rect.top ) )
				*pbyte = 0xFF;
			else
				*pbyte = 0;
			pbyte++;
		}
	}
	CObjectWrp	object( CreateMeasureObject() );
	object.SetImage( image );

	return true;
}

bool CActionObjectSpline::Initialize()
{
	AddPropertyPageDescription( c_szCObjectGeneralPage );
	if( !CActionObjectBase::Initialize() )return false;
	m_hcurActive = AfxGetApp()->LoadCursor( IDC_SPLINE_OBJECT );
	m_pcontour  = ::ContourCreate();

	m_hCursourCycled = AfxGetApp()->LoadCursor( IDC_TARGET );
	return true;
}

bool CActionObjectSpline::IsCycled()
{
	if( m_pcontour->nContourSize < 3 )
		return false;

	CPoint	point1;
	CPoint	point2;

	point1.x = m_pcontour->ppoints[m_pcontour->nContourSize-1].x;
	point2.x = m_pcontour->ppoints[0].x;
	point1.y = m_pcontour->ppoints[m_pcontour->nContourSize-1].y;
	point2.y = m_pcontour->ppoints[0].y;

	point1 = ::ConvertToWindow( m_punkTarget, point1 );
	point2 = ::ConvertToWindow( m_punkTarget, point2 );

	return ::abs( point1.x-point2.x ) < 5 && abs( point1.y-point2.y ) < 5;
}

void CActionObjectSpline::DoDraw( CDC &dc )
{
	if( !m_bDrawBoxes )
		return;

	dc.SelectStockObject( NULL_BRUSH );

	bool bCycled = IsCycled();
	
	
	if( bCycled )
	{
		m_pcontour->nContourSize--;
		::ContourDraw( dc, m_pcontour, m_punkTarget, cdfSpline|cdfClosed|cdfFill );
		m_pcontour->nContourSize++;
	}
	else
	{
		if( m_bFinished )
			return; 

		::ContourDraw( dc, m_pcontour, m_punkTarget, cdfSpline );

		dc.SetROP2( R2_COPYPEN );

		if( m_pcontour->nContourSize > 0  )
		{
			CPoint	point;
			point.x = m_pcontour->ppoints[0].x;
			point.y = m_pcontour->ppoints[0].y;
			point = ::ConvertToWindow( m_punkTarget, point );

			CRect	rect;
			rect.left = point.x - 5;
			rect.right = point.x + 5;
			rect.top = point.y - 5;
			rect.bottom = point.y + 5;

			dc.SelectStockObject( WHITE_BRUSH );
			dc.SelectStockObject( BLACK_PEN );

			dc.Rectangle( rect );

			/*point.x = m_pcontour->ppoints[m_pcontour->nContourSize-1].x;
			point.y = m_pcontour->ppoints[m_pcontour->nContourSize-1].y;
			point = ::ConvertToWindow( m_punkTarget, point );

			rect.left = point.x - 5;
			rect.right = point.x + 5;
			rect.top = point.y - 5;
			rect.bottom = point.y + 5;

			dc.Rectangle( rect );*/
		}
		dc.SetROP2( R2_XORPEN );
	}
}

bool CActionObjectSpline::DoLButtonDown( CPoint pt )
{
	if( CanActivateObjectOnLbutton() )
		ActivateObject( pt );

	_Draw();

	if( IsCycled() )
	{
		m_pcontour->nContourSize--;
		m_bFinished = true;
		Finalize();
		return true;
	}

	if( !m_pcontour->nContourSize )
	{
		::ContourAddPoint( m_pcontour, pt.x, pt.y );
		m_bFinished = false;
	}
	::ContourAddPoint( m_pcontour, pt.x, pt.y );

	
	_Draw();
	return true;
}

bool CActionObjectSpline::DoLButtonDblClick( CPoint pt )
{
	//_Draw();

	if( m_pcontour )
		if( m_pcontour->nContourSize > 0 )
			m_pcontour->nContourSize--;

	Finalize();

	return true;
}

void CActionObjectSpline::RedrawBoxes( bool bOnlyLast )
{
	if( !m_pcontour )
		return;
	if( !m_pcontour->nContourSize )
		return;

	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );

/*
	int nPoint = m_pcontour->nContourSize-1;

	CPoint	point;
	point.x = m_pcontour->ppoints[0].x;
	point.y = m_pcontour->ppoints[0].y;
	point = ::ConvertToWindow( m_punkTarget, point );

	CRect	rect;

	rect.left = point.x - 6;
	rect.right = point.x + 6;
	rect.top = point.y - 6;
	rect.bottom = point.y + 6;

	pwnd->InvalidateRect( rect );
*/

	int nPoint = m_pcontour->nContourSize;

	CRect rc( m_pcontour->ppoints[0].x, m_pcontour->ppoints[0].y, m_pcontour->ppoints[0].x, m_pcontour->ppoints[0].y );
	for( int i = 0; i < nPoint; i++ )
	{
		if( m_pcontour->ppoints[i].x < rc.left )
			rc.left = m_pcontour->ppoints[i].x;

		if( m_pcontour->ppoints[i].y < rc.top )
			rc.top = m_pcontour->ppoints[i].y;

		if( m_pcontour->ppoints[i].x > rc.right )
			rc.right = m_pcontour->ppoints[i].x;

		if( m_pcontour->ppoints[i].y > rc.bottom )
			rc.bottom = m_pcontour->ppoints[i].y;
	}

	rc.right  += 6;
	rc.left   -= 6;

	rc.top    -= 6;
	rc.bottom += 6;

	pwnd->InvalidateRect( rc );


	/*nPoint = 0;

	point.x = m_pcontour->ppoints[nPoint].x;
	point.y = m_pcontour->ppoints[nPoint].y;
	point = ::ConvertToWindow( m_punkTarget, point );
	rect.left = point.x - 6;
	rect.right = point.x + 6;
	rect.top = point.y - 6;
	rect.bottom = point.y + 6;

	if( !bOnlyLast )
		pwnd->InvalidateRect( rect );*/

	
}

bool CActionObjectSpline::DoSetCursor( CPoint point )
{
	if( !IsCycled() )
		return CActionObjectBase::DoSetCursor( point );
	else
		SetCursor( m_hCursourCycled );
	return true;
}

bool CActionObjectSpline::DoMouseMove( CPoint pt )
{
	if( m_pcontour->nContourSize == 0 ||
		( m_pcontour->ppoints[m_pcontour->nContourSize-1].x == pt.x &&
		  m_pcontour->ppoints[m_pcontour->nContourSize-1].y == pt.y ) )
		  return false;
	_Draw();

	RedrawBoxes( );

	m_bDrawBoxes = false;
	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	pwnd->UpdateWindow();
	m_bDrawBoxes = true;

	if( m_pcontour->nContourSize > 0 )
	{
		m_pcontour->ppoints[m_pcontour->nContourSize-1].x = (short)pt.x;
		m_pcontour->ppoints[m_pcontour->nContourSize-1].y = (short)pt.y;
	}

	_Draw();

	return true;
}

void CActionObjectSpline::Finalize()
{
	RedrawBoxes();

	/*int	nSaveCount = 0;
	if( m_pcontour )
	{
		nSaveCount = m_pcontour->nContourSize;
		m_pcontour->nContourSize = 0;
	}

	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	pwnd->UpdateWindow();

	if( m_pcontour )
		m_pcontour->nContourSize = nSaveCount;*/

	CActionObjectBase::Finalize();
}
//////////////////////////////////////////////////////////////////////
//CActionObjectEllipse implementation
CActionObjectEllipse::CActionObjectEllipse()
{
	m_point1 = CPoint( -1, -1 );
	m_point2 = CPoint( -1, -1 );

	m_pointCenter = CPoint( -1, -1 );
	m_nRadius = 0;

}

CActionObjectEllipse::~CActionObjectEllipse()
{
}


bool CActionObjectEllipse::DoChangeObjects()
{
	if( m_image==0 )
	{
		SetError( IDS_ERR_NOIMAGE );
		return false;
	}


	CObjectWrp	object( CreateMeasureObject() );

	CRect	rect;
	
	rect.left = int( m_pointCenter.x - m_nRadius+.5 );
	rect.right = int( m_pointCenter.x + m_nRadius+.5 )+1;
	rect.top = int( m_pointCenter.y - m_nRadius+.5 );
	rect.bottom = int( m_pointCenter.y + m_nRadius+.5 )+1;

	CRect	rectInfo = rect;

	rectInfo.left = max( min( rectInfo.left, m_image.GetWidth() ), 0 );
	rectInfo.right = max( min( rectInfo.right, m_image.GetWidth() ), 0 );
	rectInfo.top = max( min( rectInfo.top, m_image.GetHeight() ), 0 );
	rectInfo.bottom = max( min( rectInfo.bottom, m_image.GetHeight() ), 0 );

	int nMinSize = GetValueInt(GetAppUnknown(), "\\ObjectEditor", "MinSize", 2);
	if (nMinSize < 2) nMinSize = 2;
	if (rectInfo.Width() < nMinSize && rectInfo.Height() < nMinSize)
		return false;

	CWalkFillInfo	info( rectInfo );

	CDC	*pDC = info.GetDC();
	pDC->SelectStockObject( WHITE_BRUSH );
	pDC->SelectStockObject( WHITE_PEN );
	if (m_nRadius == 0)
		info.SetPixel(rect.left, rect.top, 255);
	else
	pDC->Ellipse( rect );
	//info.FinalizeDC();

	IUnknown *punk = m_image.CreateVImage( rect );
	info.AttachMasksToImage( punk );

	object.SetImage( punk );

	punk->Release();
	
	return true;
}

bool CActionObjectEllipse::Initialize()
{
	AddPropertyPageDescription( c_szCObjectGeneralPage );
	if( !CActionObjectBase::Initialize() )return false;
	m_hcurActive = AfxGetApp()->LoadCursor( IDC_ELLIPSE_OBJECT );
	m_hcurAdd = AfxGetApp()->LoadCursor( IDC_ELLIPSE_ADD );
	m_hcurSub = AfxGetApp()->LoadCursor( IDC_ELLIPSE_SUB );

	m_pointCenter.x = GetArgumentInt( _T("X") );
	m_pointCenter.y = GetArgumentInt( _T("Y") );
	m_nRadius = GetArgumentInt( _T("R") );

	if( m_pointCenter.x != -1 &&
		m_pointCenter.y != -1 &&
		m_nRadius != 0 )
	{
		Finalize();
		return false;
	}
	return true;

}

void CActionObjectEllipse::DoDraw( CDC &dc )
{
	CRect	rect;
	
	rect.left = int( m_pointCenter.x - m_nRadius+.5 );
	rect.right = int( m_pointCenter.x + m_nRadius+.5 );
	rect.top = int( m_pointCenter.y - m_nRadius+.5 );
	rect.bottom = int( m_pointCenter.y + m_nRadius+.5 );

	rect = ::ConvertToWindow( m_punkTarget, rect );
	dc.Ellipse( rect );

	DrawPoint( dc, m_point1 );
	DrawPoint( dc, m_point2 );
}

bool CActionObjectEllipse::DoStartTracking( CPoint pt )
{
	_Draw();
	m_point1 = pt;
	m_point2 = pt;
	m_pointCenter = pt;
	m_nRadius = 0;
	_Draw();
	return true;
}

bool CActionObjectEllipse::DoTrack( CPoint pt )
{
	_Draw();
	
	m_point2 = pt;

	m_pointCenter.x = ( m_point1.x + m_point2.x )>>1;
	m_pointCenter.y = ( m_point1.y + m_point2.y )>>1;
	m_nRadius = (long)::sqrt(double( (m_point1.x - m_point2.x)*(m_point1.x - m_point2.x)+
						(m_point1.y - m_point2.y)*(m_point1.y - m_point2.y) ))/2;

	_Draw();
	return true;
}

void CActionObjectEllipse::DrawPoint( CDC &dc, CPoint point )
{
	if( point.x == -1 && point.y == -1 )
		return;
	CPoint	pointDraw = ::ConvertToWindow( m_punkTarget, point );

	int	dx = 10;
	int	dy = 10;

	dc.MoveTo( pointDraw.x - dx, pointDraw.y - dy );
	dc.LineTo( pointDraw.x + dx, pointDraw.y + dy );
	dc.MoveTo( pointDraw.x + dx, pointDraw.y - dy );
	dc.LineTo( pointDraw.x - dx, pointDraw.y + dy );

}


bool CActionObjectEllipse::DoFinishTracking( CPoint pt )
{
	//_Draw();
	Finalize();
	return true;
}

void CActionObjectEllipse::StoreCurrentExecuteParams()
{
	SetArgument( _T("X"), _variant_t(m_pointCenter.x) );
	SetArgument( _T("Y"), _variant_t(m_pointCenter.y) );
	SetArgument( _T("R"), _variant_t(m_nRadius) );
}



//////////////////////////////////////////////////////////////////////
//CActionObjectRect implementation
CActionObjectRect::CActionObjectRect()
{
	m_rectNewObject = NORECT;
	m_pctrl = 0;
}

CActionObjectRect::~CActionObjectRect()
{
}

BOOL CActionObjectRect::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if( message == WM_TIMER || message == WM_KEYDOWN )CheckController();
	return CActionObjectBase::OnWndMsg( message, wParam, lParam, pResult );
}

void CActionObjectRect::CheckController()
{
	if( !m_bInTrackMode )
	{
		if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
		{
			if( !m_pctrl )
			{
				m_pctrl = new CRectCtrl();
				m_pctrl->Install();
			}
		}
		else
		{
			if( m_pctrl )
			{
				m_pctrl->Uninstall();
				m_pctrl->GetControllingUnknown()->Release();
				m_pctrl = 0;
			}
		}
	}
}


bool CActionObjectRect::DoChangeObjects()
{
	if( m_image==0 )
	{
		SetError( IDS_ERR_NOIMAGE );
		return false;
	}
	
	m_rectNewObject.NormalizeRect();
	int nMinSize = GetValueInt(GetAppUnknown(), "\\ObjectEditor", "MinSize", 2);
	if (nMinSize < 2) nMinSize = 2;
	if (m_rectNewObject.Width() < nMinSize && m_rectNewObject.Height() < nMinSize)
		return false;

	CObjectWrp	object( CreateMeasureObject() );
	CImageWrp	image( m_image.CreateVImage( m_rectNewObject ), false );
	object.SetImage( image );

	return true;
}

bool CActionObjectRect::DoStartTracking( CPoint pt )
{
	_Draw();

	if( m_pctrl )
	{
		m_pctrl->Uninstall();
		m_pctrl->GetControllingUnknown()->Release();
	}
	m_pctrl = 0;


	m_rectNewObject.left = pt.x;
	m_rectNewObject.top = pt.y;
	m_rectNewObject.right = pt.x+1;
	m_rectNewObject.bottom = pt.y+1;

	_Draw();

	return true;
}

bool CActionObjectRect::DoTrack( CPoint pt )
{
	_Draw();

	m_rectNewObject.right = pt.x+1;
	m_rectNewObject.bottom = pt.y+1;

	_Draw();

	return true;
}

bool CActionObjectRect::DoFinishTracking( CPoint pt )
{
//	_Draw();

	Finalize();
	return true;
}

void CActionObjectRect::DoDraw( CDC &dc )
{
	if( m_rectNewObject == NORECT )
		return;

	CRect	rect = ::ConvertToWindow( m_punkTarget, m_rectNewObject );
	dc.SelectStockObject( NULL_BRUSH );
	dc.Rectangle( rect );
}

bool CActionObjectRect::Initialize()
{
	AddPropertyPageDescription( c_szCObjectGeneralPage );
	if( !CActionObjectBase::Initialize() )return false;

	m_hcurActive = AfxGetApp()->LoadCursor( IDC_RECT_OBJECT );
	m_hcurAdd = AfxGetApp()->LoadCursor( IDC_RECT_ADD );
	m_hcurSub = AfxGetApp()->LoadCursor( IDC_RECT_SUB );

	m_rectNewObject.left = GetArgumentInt( _T("Left") );
	m_rectNewObject.top = GetArgumentInt( _T("Top") );
	m_rectNewObject.right = GetArgumentInt( _T("Right") );
	m_rectNewObject.bottom = GetArgumentInt( _T("Bottom") );

	_Draw();

	CheckController();

	if( m_rectNewObject != NORECT )
	{
		Finalize();
		return false;
	}
	return true;
}

void CActionObjectRect::Finalize()
{
	if( m_pctrl )
	{
		m_pctrl->Uninstall();
		m_pctrl->GetControllingUnknown()->Release();
	}
	m_pctrl = 0;

	CActionObjectBase::Finalize();
}

void CActionObjectRect::StoreCurrentExecuteParams()
{
	SetArgument( _T("Left"), _variant_t( m_rectNewObject.left ) );
	SetArgument( _T("Top"), _variant_t( m_rectNewObject.top ) );
	SetArgument( _T("Right"), _variant_t( m_rectNewObject.right ) );
	SetArgument( _T("Bottom"), _variant_t( m_rectNewObject.bottom ) );
}



//////////////////////////////////////////////////////////////////////
//CActionObjectMagickStick implementation
CActionObjectMagickStick::CActionObjectMagickStick()
{
	m_colorDelta = ::ByteAsColor( 5 );
	m_point = CPoint( -1, -1 );

	m_cx = 0;
	m_cy = 0;
	m_colors = 0;
	m_ppoints = 0;
	m_pbyteMark = 0;
	m_prows = 0;
	m_nPointCount = 0;
	m_rect = NORECT;

	m_bDynamicBrightness	= false;	
}

CActionObjectMagickStick::~CActionObjectMagickStick()
{
}

bool CActionObjectMagickStick::DoLButtonDown( CPoint pt )
{
	GetSettings();

	if( CanActivateObjectOnLbutton() )
		ActivateObject( pt );

	m_point = pt;
	Finalize();
	return true;
}

#define AddBlank1(n)\
{}

static void FillHoles(byte *mask, int cx, int cy, CRect r)
{ //заполнить дыры в изображении, работаем только в пределах rect
	if(r.Width()<3) return;
	if(r.Height()<3) return;

	int n;
	smart_alloc(pts, int, cx*cy);
	int pos0=0, pos1=0;

	int x,y;

	for (y=r.top+1;y<r.bottom-1;y++)
	{
		x=r.left+1; n=y*cx+x;
		if(mask[n-1]==0)
		{
			mask[n-1]=1;
			if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		}
		x=r.right-2; n=y*cx+x;
		if(mask[n+1]==0)
		{
			mask[n+1]=1;
			if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		}
	}
	for (x=r.left+1;x<r.right-1;x++)
	{
		y=r.top+1; n=y*cx+x;
		if(mask[n-cx]==0)
		{
			mask[n-cx]=1;
			if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		}
		y=r.bottom-2; n=y*cx+x;
		if(mask[n+cx]==0)
		{
			mask[n+cx]=1; 
			if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		}
	}
	// края изображения заполнили 1, точки на расстоянии шага от края внесли в список
	
	y=r.top; x=r.left; n=y*cx+x; if(mask[n]==0) mask[n]=1;
	y=r.top; x=r.right-1; n=y*cx+x; if(mask[n]==0) mask[n]=1;
	y=r.bottom-1; x=r.left; n=y*cx+x; if(mask[n]==0) mask[n]=1;
	y=r.bottom-1; x=r.right-1; n=y*cx+x; if(mask[n]==0) mask[n]=1;
	//заткнули уголки

	/*
	// debug - output mask to file
	FILE *f=fopen("e:\\mylog.log","a");
	fputs("\n\r",f);
	for (y=0;y<cy;y++)
	{
		byte *imgy=mask+y*cx;
		for (x=0;x<cx;x++)
		{
			char c='.';
			if(*imgy==1) c='*';
			if(*imgy==255) c='#';
			fputc(c,f);
			imgy++;
		}
		fputs("\n\r",f);
	}
	fputs("\n\r",f);
	fclose(f);
	*/

	while(pos0<pos1)
	{
		n=pts[pos0]+1; if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		n=pts[pos0]-1; if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		n=pts[pos0]+cx; if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		n=pts[pos0]-cx; if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		pos0++;
	}

	for (y=r.top;y<r.bottom;y++)
	{
		for (x=r.left;x<r.right;x++)
		{
			n=y*cx+x;
			switch(mask[n])
			{
			case 0: mask[n]=255; break;
			case 1: mask[n]=0; break;
			}
			
		}
	}
}

bool CActionObjectMagickStick::Invoke()
{
	CWaitCursor wait;
	return CActionObjectBase::Invoke();
}


bool CActionObjectMagickStick::DoChangeObjects()
{
	GetSettings();

	m_cx = m_image.GetWidth();
	m_cy = m_image.GetHeight();
	m_colors = m_image.GetColorsCount();


	if( !m_image.GetRect().PtInRect( m_point ) )
	{
		SetError( IDS_ERR_NOTINIMAGE );
		return false;
	}
	
	m_ppoints = new CPoint[m_cx*m_cy];
	m_pbyteMark = new byte[m_cx*m_cy];
	m_prows = new color*[m_cy*m_colors];

	for( int n = 0; n < m_cy; n++ )
		for( int c = 0; c < m_colors; c++ )
			m_prows[n*m_colors+c] = m_image.GetRow( n, c );

	ZeroMemory( m_pbyteMark, m_cx*m_cy );

	m_nPointCount = 0;
	int	nPointPos = 0;

	m_ppoints[m_nPointCount].x = m_point.x;
	m_ppoints[m_nPointCount].y = m_point.y;
	m_nPointCount++;

	m_pbyteMark[m_point.y*m_cx+m_point.x] = 0xFF;

	m_rect.left = m_point.x;
	m_rect.top = m_point.y;
	m_rect.right = m_point.x+1;
	m_rect.bottom = m_point.y+1;
	
	//TestPoint( m_ppoints[0].x, m_ppoints[0].y );

	
	while( nPointPos < m_nPointCount )
	{
		int	x = m_ppoints[nPointPos].x;
		int	y = m_ppoints[nPointPos].y;

//		CheckAddArea( x, y, x-1, y );
//		CheckAddArea( x, y, x+1, y );
//		CheckAddArea( x, y, x, y-1 );
//		CheckAddArea( x, y, x, y+1 );

//		CheckAddArea( x, y, x-1, y-1 );
//		CheckAddArea( x, y, x+1, y+1 );
//		CheckAddArea( x, y, x+1, y-1 );
//		CheckAddArea( x, y, x-1, y+1 );
		
		CheckAddArea( m_point.x, m_point.y, x-1, y, x, y );
		CheckAddArea( m_point.x, m_point.y, x+1, y, x, y );
		CheckAddArea( m_point.x, m_point.y, x, y-1, x, y );
		CheckAddArea( m_point.x, m_point.y, x, y+1, x, y );

		CheckAddArea( m_point.x, m_point.y, x-1, y-1, x, y );
		CheckAddArea( m_point.x, m_point.y, x+1, y+1, x, y );
		CheckAddArea( m_point.x, m_point.y, x+1, y-1, x, y );
		CheckAddArea( m_point.x, m_point.y, x-1, y+1, x, y );

		nPointPos++;
	}
	
	//удаление дырок в m_pbyteMark - в пределах m_rect
	if(m_bFillHoles) FillHoles(m_pbyteMark, m_cx, m_cy, m_rect);	
	
	CImageWrp	image( m_image.CreateVImage( m_rect ), false );

	image.InitRowMasks();
	for( n = 0; n < m_rect.Height(); n++ )
		memcpy( image.GetRowMask( n ), m_pbyteMark+(n+m_rect.top)*m_cx+m_rect.left, m_rect.Width() );

	CObjectWrp	object( CreateMeasureObject() );
	object.SetImage( image );


	delete m_ppoints;
	delete m_pbyteMark;
	delete m_prows;

	m_cx = 0;
	m_cy = 0;
	m_colors = 0;
	m_ppoints = 0;
	m_pbyteMark = 0;
	m_prows = 0;

	return true;
}


void CActionObjectMagickStick::CheckAddArea( int x, int y, int x1, int y1, int nNearX , int nNearY )
{

	if( x1 < 0 || y1 < 0 || x1 >= m_cx || y1 >= m_cy )
		return;

	if( m_pbyteMark[y1*m_cx+x1] )
		return;

	for( int c = 0; c < m_colors; c++ )
	{
		color	color1;

		if( m_bDynamicBrightness )
			color1 = m_prows[nNearY*m_colors+c][nNearX];
		else
			color1 = m_prows[y*m_colors+c][x];

		
		color	color2 = m_prows[y1*m_colors+c][x1];

		if( ::abs( color2-color1 ) > m_colorDelta )
			return;
	}
	
	m_ppoints[m_nPointCount].x = x1;
	m_ppoints[m_nPointCount].y = y1;
	m_nPointCount++;

	m_rect.left = min( x1, m_rect.left );
	m_rect.top = min( y1, m_rect.top );
	m_rect.right = max( x1+1, m_rect.right );
	m_rect.bottom = max( y1+1, m_rect.bottom );

	m_pbyteMark[y1*m_cx+x1] = 0xFF;
}

bool CActionObjectMagickStick::Initialize()
{
	AddPropertyPageDescription( c_szCObjectGeneralPage );
	AddPropertyPageDescription( c_szCMagickObjectPage );
	if( !CActionObjectBase::Initialize() )return false;

	m_hcurActive = AfxGetApp()->LoadCursor( IDC_MAGICK_OBJECT );
	m_hcurAdd = AfxGetApp()->LoadCursor( IDC_MAGICK_ADD );
	m_hcurSub = AfxGetApp()->LoadCursor( IDC_MAGICK_SUB );


	DoUpdateSettings();
	return true;
}

bool CActionObjectMagickStick::DoUpdateSettings()
{
	GetSettings();
	return CActionObjectBase::DoUpdateSettings();
}


void CActionObjectMagickStick::GetSettings()
{
	m_colorDelta = ByteAsColor( GetValueInt( GetAppUnknown(), szMagic, szMagicInt, 5 ) );

	m_bFillHoles = ::GetValueInt( ::GetAppUnknown(), szMagic, szMagicFillHoles, 0 ) != 0;

	m_bDynamicBrightness = 
		( ::GetValueInt( ::GetAppUnknown(), szMagic, szDynamicBrightness, 0 ) == 1 ? true : false );

}


//measurement
//////////////////////////////////////////////////////////////////////
//CActionCalcMeasResult implementation
CActionCalcMeasResult::CActionCalcMeasResult()
{
}

CActionCalcMeasResult::~CActionCalcMeasResult()
{
//	m_sptrMan = 0;
	m_manGroup.FreeComponents();
}

bool CActionCalcMeasResult::Invoke()
{
	if (!m_punkTarget)
		return false;

	// get meas_param_group_manager
	IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IMeasureManager, szMeasurement);
	ICompManagerPtr	ptrManager( punk );
	if( punk )punk->Release();

	if( ptrManager == 0 )return false;

	// check manager contains groups
	int	nGroup, nGroupCount;
	ptrManager->GetCount( &nGroupCount );


	IUnknown* punk_object_list = ::FindObjectByName( m_punkTarget, GetArgumentString( "ObjectList" ));
	if( !punk_object_list )
		punk_object_list = GetActiveList(szTypeObjectList);

	// get active object list
	m_listObjects.Attach( punk_object_list, false );
	// check list contains objects
	if (!m_listObjects.GetCount())
		return false;

	CPtrArray	objects;
	CPtrArray	images;

	POSITION pos = m_listObjects.GetFirstObjectPosition();		//get the first object position
	while( pos )
	{
		IUnknown *punk = m_listObjects.GetNextObject( pos );//get the object at the specified position and move the POS
		if( !punk )continue;
		IMeasureObjectPtr	ptrO( punk );
		punk->Release();
		if( ptrO == 0 )continue;
		ptrO->GetImage( &punk );
		objects.Add( ptrO.Detach() );
		images.Add( punk );
	}

	//remove everything from objects

	IDataObjectListPtr	ptrList( m_listObjects );
	BOOL	bLock = false;
	ptrList->GetObjectUpdateLock( &bLock );
	ptrList->LockObjectUpdate( true );

	int	nCount = objects.GetSize();

	for( int i = 0; i < nCount; i++ )
	{
		ICalcObjectPtr	ptrCalc( (IUnknown*)objects[i] );
		ptrCalc->ClearValues();
	}

	
	//remove from container
	LONG_PTR	lposParam = 0;
	ICalcObjectContainerPtr	ptrContainer( m_listObjects );
	ptrContainer->GetFirstParameterPos( &lposParam );
	ptrContainer->SetCurentPosition( 0 );

	while( lposParam )
	{
		ParameterContainer *pContainer = 0;
		LONG_PTR	lPosRemove = lposParam;
		ptrContainer->GetNextParameter( &lposParam, &pContainer );

		if( pContainer->pDescr->pos == 0 )
			continue;
		ptrContainer->RemoveParameterAt( pContainer->lpos );
	}


	for( nGroup = 0; nGroup < nGroupCount; nGroup++ )
	{
		IUnknownPtr	ptrG = 0;
		ptrManager->GetComponentUnknownByIdx( nGroup, &ptrG );
		IMeasParamGroupPtr	ptrGroup( ptrG );
		if( ptrGroup == 0 )continue;
		ptrGroup->InitializeCalculation( m_listObjects );

		for( i = 0; i < nCount; i++ )
		{
			IUnknown	*punkO = (IUnknown*)objects[i];
			IUnknown	*punkI = (IUnknown*)images[i];
			ptrGroup->CalcValues( punkO, punkI );
		}
		
		ptrGroup->FinalizeCalculation();
	}

	for( i = 0; i < nCount; i++ )
	{
		IUnknown	*punkO = (IUnknown*)objects[i];
		IUnknown	*punkI = (IUnknown*)images[i];
		if( punkO )punkO->Release();
		if( punkI )punkI->Release();
	}

	ptrList->LockObjectUpdate( bLock );
	ptrList->UpdateObject( 0 );
	INamedDataObjectPtr ptrNDO(ptrList);
	if( ptrNDO )
		ptrNDO->SetModifiedFlag(TRUE);
	return true;
}

//extended UI
bool CActionCalcMeasResult::IsAvaible()
{
	if (!m_punkTarget)
		return false;

	CObjectListWrp list;

	IUnknown* punk_object_list = ::FindObjectByName( m_punkTarget, GetArgumentString( "ObjectList" ));
	if( !punk_object_list )
		punk_object_list = GetActiveList(szTypeObjectList);

	list.Attach( punk_object_list, false);

	return list.GetCount() != 0;
}

IUnknown* CActionCalcMeasResult::GetActiveList(LPCTSTR szObjectType)
{
	if (CheckInterface(m_punkTarget, IID_IDocument))
		return ::GetActiveObjectFromDocument(m_punkTarget, szObjectType);
	else
		return ::GetActiveObjectFromContext(m_punkTarget, szObjectType);
}


//////////////////////////////////////////////////////////////////////
//CActionRecalcOnChangeCalibr implementation
CActionRecalcOnChangeCalibr::CActionRecalcOnChangeCalibr()
{
}

CActionRecalcOnChangeCalibr::~CActionRecalcOnChangeCalibr()
{
}

bool CActionRecalcOnChangeCalibr::Invoke()
{
	// Read calibration from shell.data
	GUID	guidCalibr;
	double	fVal;
	::GetDefaultCalibration( &fVal, &guidCalibr );

	IUnknown	*punk = GetActiveObjectFromDocument( m_punkTarget,  szTypeImage );
	if (punk )
	{
		INamedDataObject2Ptr	ptrNamed( punk );
		punk->Release();
		punk = 0;
		ptrNamed->GetParent( &punk ); // A.M. fix 5581
		if (punk != NULL)
		{
			ptrNamed = punk;
			punk->Release();
			punk = 0;
		}
		GUID guidKey;
		ptrNamed->GetBaseKey( &guidKey );
		INamedDataPtr	ptrData( m_punkTarget );
		long	lpos = 0;
		ptrData->GetBaseGroupObjectFirstPos( &guidKey, &lpos );
		while( lpos )
		{
			IUnknown	*punk = 0;
			ptrData->GetBaseGroupNextObject( &guidKey, &lpos, &punk );
			if( !punk )continue;
			StoreCalibration( punk, guidCalibr, fVal, true );
			punk->Release();
		}
	}
	else
	{
		IUnknown *punk = GetActiveObjectFromDocument(m_punkTarget, szTypeObjectList);
		if (punk)
		{
			StoreCalibration( punk, guidCalibr, fVal, true );
			punk->Release();
		}
	}

	//walk all data objects
	IApplication	*papp = GetAppUnknown();
	LONG_PTR	lposDoc, lposTempl;
	papp->GetFirstDocTemplPosition( &lposTempl );
	while( lposTempl )
	{
		papp->GetFirstDocPosition( lposTempl, &lposDoc );
		while( lposDoc )
		{
			IUnknown	*punkDoc = 0;
			papp->GetNextDoc( lposTempl, &lposDoc, &punkDoc );

			ASSERT(punkDoc);

			IDataTypeManagerPtr ptrMan( punkDoc );

			punkDoc->Release();

			long	lTypesCount;
			ptrMan->GetTypesCount( &lTypesCount );

			for( int lType = 0; lType < lTypesCount; lType++ )
			{
				LONG_PTR	lposObject;
				ptrMan->GetObjectFirstPosition( lType, &lposObject );

				while( lposObject )
				{	
					IUnknown	*punkObject = 0;
					ptrMan->GetNextObject( lType, &lposObject, &punkObject );
					ASSERT(punkObject);
					StoreCalibration( punkObject, guidCalibr, fVal, false );
					punkObject->Release();
				}
			}
		}
		papp->GetNextDocTempl( &lposTempl, 0, 0 );
	}

	return true;
}

void CActionRecalcOnChangeCalibr::StoreCalibration( IUnknown *punk, GUID &guid, double fCalibr, bool bReplace )
{
	INamedDataObject2Ptr	ptrNamed( punk );
	TPOS	lpos = 0;
	ptrNamed->GetFirstChildPosition( &lpos );

	while( lpos )
	{
		IUnknown	*punkC = 0;
		ptrNamed->GetNextChild( &lpos, &punkC );
		if( punkC )
		{
			StoreCalibration( punkC, guid, fCalibr, bReplace );
			punkC->Release();
		}
	}

	ICalibrPtr	ptrCalibr( punk );
	if( ptrCalibr != 0 )
	{
		if( !bReplace )
		{
			GUID	guidC;
			double d;
			ptrCalibr->GetCalibration( &d, &guidC );
			if( guidC != guid )return;
		}
		ptrCalibr->SetCalibration( fCalibr, &guid );
	}
}


//////////////////////////////////////////////////////////////////////
// CActionJoinAllObjects Implementation

//////////////////////////////////////////////////////////////////////
CActionJoinAllObjects::CActionJoinAllObjects()
{

}

//////////////////////////////////////////////////////////////////////
CActionJoinAllObjects::~CActionJoinAllObjects()
{
	m_map_class_to_objects.clear( );
}

//////////////////////////////////////////////////////////////////////
bool CActionJoinAllObjects::InvokeFilter()
{
	CObjectListWrp objectOut( GetDataResult() );
	CObjectListWrp objectsIn( GetDataArgument("MeasObjects") );
	CString strClassifireName = GetArgumentString(_T("ClassifierName"));
	if( strClassifireName.IsEmpty() )
		strClassifireName = short_classifiername( 0 );	// получение имени активного классификатора

	BOOL bUseClasses = GetArgumentInt(_T("UseClasses")) == 1;
	BOOL bInitContours = GetArgumentInt(_T("InitContours")) == 1;

	m_map_class_to_objects.clear( );

	if( !init_class_list( objectsIn, strClassifireName, bUseClasses ) )
			return false;
	
	long lClass = -1, lClassCount = 0;
	if( bUseClasses )
	{
        lClassCount = class_count( strClassifireName.GetBuffer( strClassifireName.GetLength() ) );		
		strClassifireName.ReleaseBuffer();
	}
	else
	{
		lClass = -2;
		lClassCount = -1;
	}


	long lChildsCount = objectsIn.GetCount( );
	StartNotification(  lChildsCount, 1, 1);
	
	int nNotify = 0;
	for(  ; lClass < lClassCount; lClass ++ )
        join_objects_by_class( objectsIn, objectOut, nNotify, strClassifireName, bInitContours, lClass );
        	
	FinishNotification( );

    m_map_class_to_objects.clear( );
	return true;
}

BOOL	CActionJoinAllObjects::join_objects_by_class( IUnknown *punkObjListIn, IUnknown *punkObjListOut, 
													 int &nNotify, CString strClassifierName, 
													 BOOL bInitContours, long lClass /*= -2*/ )
{
	//PROFILE_TEST( "CActionJoinAllObjects::join_objects_by_class" );	

	if( !punkObjListIn || !punkObjListOut )
		return false;

	CObjectListWrp objectOut( punkObjListOut );
	CObjectListWrp objectsIn( punkObjListIn );
	
	RECT rc_all_images = {0};
	_bstr_t bstrCCName( "" );
	int nObjNum = 0, nPaneNum = -1;

	if( !init( objectsIn, strClassifierName, lClass, &nObjNum, &rc_all_images, bstrCCName.GetAddress(), &nPaneNum ) )
		return false;

	// create new image
	IUnknown *punk_image = ::CreateTypedObject( _bstr_t( szTypeImage ) );
	IImage3Ptr	sptrNewImage = punk_image;
	if( punk_image )
		punk_image->Release(); punk_image = 0;

    if( sptrNewImage == 0 )
		return false;

	INamedDataObject2Ptr sptrNDImg = sptrNewImage;
	if( sptrNDImg == 0 )
		return false;

	if( S_OK != sptrNewImage->CreateImage(rc_all_images.right - rc_all_images.left, rc_all_images.bottom -
		rc_all_images.top, bstrCCName, nPaneNum) )
		return false;

	sptrNewImage->InitRowMasks();
	sptrNewImage->SetOffset( *(POINT*)(&rc_all_images), FALSE);

	// create new measure object
	IUnknown *punk_newobj = 0;
	punk_newobj = ::CreateTypedObject( _bstr_t(szTypeObject) );
	IMeasureObjectPtr	sptr_measobj( punk_newobj );
	if( punk_newobj )
		punk_newobj->Release(); punk_newobj = 0;

	if( sptr_measobj == 0 )
		return false;


//---------------------------------------------

	long	lrows_new_image = rc_all_images.bottom - rc_all_images.top,
			lcols_new_image = rc_all_images.right - rc_all_images.left;

	// clear mask
	for( int irow = 0; irow < lrows_new_image; irow ++ )
	{
		BYTE *pbyte_mask_new_image = 0;
        if( S_OK == sptrNewImage->GetRowMask( irow,	&pbyte_mask_new_image ) && pbyte_mask_new_image )
			::ZeroMemory( pbyte_mask_new_image, sizeof(BYTE) * lcols_new_image );	
		int l = 0;
	}

	
	SPUNK_LIST *plist = 0;
	TPOS list_pos = m_map_class_to_objects.find(lClass);
	if( !list_pos )
		return FALSE;

    plist = m_map_class_to_objects.get( list_pos );
	if( !plist  )
		return FALSE;

	// set masks and colors of images to new image
	for (TPOS lpos = plist->head(); lpos; lpos = plist->next(lpos))
	{
		IUnknownPtr sptr_child = plist->get( lpos );
		if( sptr_child == 0 )
            continue;

		IMeasureObjectPtr sptr_measure = sptr_child;
		if( sptr_measure == 0 )
			continue;

		IUnknown* punk_child_image = 0;
		sptr_measure->GetImage( &punk_child_image );

		IImage3Ptr sptr_child_image ( punk_child_image );
		if( punk_child_image )
			punk_child_image->Release();	punk_child_image = 0;
		
		if( sptr_child_image == 0)
			continue;
		
		int cx = 0, cy = 0;
		POINT pt_offset = {0};
		BYTE *prow_mask_child = 0;
		
		if( S_OK != sptr_child_image->GetSize( &cx, &cy ) )
			continue;

		if( ( cy > (rc_all_images.bottom - rc_all_images.top) ) || 
			( cx > (rc_all_images.right - rc_all_images.left) ) )
			continue;	//!!! может дальше вообще не продолжать?

		if( S_OK != sptr_child_image->GetOffset( &pt_offset ) )
			continue;
        
		for( int irow_child = 0; irow_child < cy; irow_child ++ )
		{	
			BYTE *pbyte_mask_child = 0;

			sptr_child_image->GetRowMask( irow_child, &pbyte_mask_child );
			if( !pbyte_mask_child )
				break;

			BYTE *pbyte_mask_new_image = 0;
			sptrNewImage->GetRowMask( pt_offset.y - rc_all_images.top + irow_child,	&pbyte_mask_new_image );
			if( !pbyte_mask_new_image )
				break;

			// copy colors
			color	**ppcolor_child = new color*[ nPaneNum ],
					**ppcolor_new_image = new color*[ nPaneNum ];

			for( int npane = 0; npane < nPaneNum; npane ++ )
			{
				sptr_child_image->GetRow( irow_child, npane, &ppcolor_child[ npane ] );
				sptrNewImage->GetRow( pt_offset.y - rc_all_images.top + irow_child,	npane, 
					&ppcolor_new_image[ npane ] );
			}

			for( int icol_child = 0; icol_child < cx; icol_child ++ )
			{
				if( pbyte_mask_child[ icol_child ] > pbyte_mask_new_image[ pt_offset.x - rc_all_images.left + icol_child ] )
				{
					// filling colors for every pane
					for( int npane = 0; npane < nPaneNum; npane ++ )
					{
						if( !ppcolor_new_image[ npane ] || !ppcolor_child[ npane ] )
							continue;

						*(ppcolor_new_image[ npane ] + pt_offset.x - rc_all_images.left + icol_child) =
							*(ppcolor_child[ npane ] + icol_child);
					}
						
					pbyte_mask_new_image[ pt_offset.x - rc_all_images.left + icol_child ] =
						pbyte_mask_child[ icol_child ];
				}
			}

			if( ppcolor_child )
				delete[] ppcolor_child;	ppcolor_child = 0;
			
			if( ppcolor_new_image )
				delete[] ppcolor_new_image;	ppcolor_new_image = 0;
		}

		Notify( nNotify++ );
	}

	if( bInitContours )
		sptrNewImage->InitContours();
   
	// add new object to objectlist
	INamedDataObject2Ptr sptr_ndobj = sptr_measobj;
	// set image
	sptr_measobj->SetImage( sptrNewImage );
	DWORD dw = apfNotifyNamedData;
	sptr_ndobj->SetParent( objectOut, 0 );

	// set class to new object
	if( lClass != -2 )
	{
		ICalcObjectPtr sptrCalc = sptr_ndobj;
		if( sptrCalc != 0 )
		{
            set_object_class( sptrCalc, lClass, strClassifierName.GetBuffer(strClassifierName.GetLength()) );
			strClassifierName.ReleaseBuffer( );
		}
	}

	// calc object
	bool bRecalc = ::GetValueInt(GetAppUnknown(), "\\measurement", "DynamicRecalc", 1) == 1;
	if( bRecalc  )
		::CalcObject( sptr_ndobj, sptrNewImage, true, 0 );

	return TRUE;
}


BOOL	CActionJoinAllObjects::init( IUnknown *punkObjList, CString strClassifierName, long lClass,  
									int *pnObjNum, LPRECT lprc_total, BSTR *pbstrCCName, int *pnPaneNum )
{
	// PROFILE_TEST( "CActionJoinAllObjects::init" );	 
	if( !punkObjList || !lprc_total || !pbstrCCName || !pnPaneNum || !pnObjNum )
		return FALSE;

    CObjectListWrp objList( punkObjList );

	_bstr_t bstrtCCName( "" );
	::SetRectEmpty( lprc_total );
	*pnPaneNum = -1;
	*pnObjNum = 0;
		
	SPUNK_LIST *plist = 0;
	TPOS list_pos = m_map_class_to_objects.find(lClass);
	if( !list_pos )
		return FALSE;

    plist = m_map_class_to_objects.get( list_pos );
	if( !plist  )
		return FALSE;

	for (TPOS lpos = plist->head(); lpos; lpos = plist->next(lpos))
	{
		IUnknownPtr sptr_child = plist->get( lpos );
		if( sptr_child == 0 )
			continue;

		IMeasureObjectPtr sptr_measure = sptr_child;
		if( sptr_measure == 0 )
			continue;
		
		IUnknown* punk_child_image = 0;
		sptr_measure->GetImage( &punk_child_image );

        IImage4Ptr sptr_child_image( punk_child_image );
		if( punk_child_image )
			punk_child_image->Release();	punk_child_image = 0;

		if( sptr_child_image == 0 )
			continue;
		
		// calc total rectangle
		RECT rc_image = {0};
        if( S_OK != sptr_child_image->GetOffset( (POINT*)(&rc_image) ) )
			continue;

		if( S_OK != sptr_child_image->GetSize( (int*)(&rc_image.right), (int*)(&rc_image.bottom) ) )
			continue;
        
		rc_image.right += rc_image.left;
		rc_image.bottom += rc_image.top;

		::UnionRect( lprc_total, lprc_total, &rc_image );
	
        // get ColorConvertor
		DWORD	dwFlags = 0;
		sptr_child_image->GetImageFlags( &dwFlags );
		IUnknown *punkCC  = 0;
		if( (dwFlags & ifVirtual) != 0 )
		{ // virtual
            INamedDataObject2Ptr sptrNDObj =  sptr_child_image;
			if( sptrNDObj == 0 )
				continue;

			IUnknown *punk_parent = 0;
			sptrNDObj->GetParent( &punk_parent );
			IImage4Ptr sptr_parent_image( punk_parent );
			if( punk_parent )
				punk_parent->Release(); punk_parent = 0;

			if( sptr_parent_image == 0 )
				continue;

			sptr_parent_image->GetColorConvertor( &punkCC );
		}
		else
			sptr_child_image->GetColorConvertor( &punkCC );
        
		IColorConvertor2Ptr sptrCC( punkCC );
		if( punkCC )
			punkCC->Release(); punkCC = 0;

		if( sptrCC == 0 )
			continue;
		
		_bstr_t bstrCCName("");
		if( S_OK != sptrCC->GetCnvName( bstrCCName.GetAddress() ) )
			continue;

		if( 0 == bstrtCCName.length( ) )
			bstrtCCName = bstrCCName;

		if( bstrtCCName != bstrCCName )
			return FALSE;

		// get panes count
		int nCurrPaneCount = ::GetImagePaneCount( sptr_child_image );

		if( *pnPaneNum == -1 )
			*pnPaneNum = nCurrPaneCount;

		if( *pnPaneNum != nCurrPaneCount )
			return FALSE;

		(*pnObjNum) ++;
	}
	
	if( ( 0 >= (*pnObjNum) ) || (0 > bstrtCCName.length()) || (*pnPaneNum == -1) || ::IsRectEmpty( lprc_total ) )
		return FALSE;
	else
	{
		*pbstrCCName = bstrtCCName.Detach();	
		return TRUE;
	}
}

BOOL	CActionJoinAllObjects::init_class_list( IUnknown *punkObjList, CString strClassifierName, BOOL bUseGroupByClass )
{
    if( !punkObjList )
		return FALSE;

	CObjectListWrp objList( punkObjList );

	POSITION pos = objList.GetFirstObjectPosition();
	while( pos )
	{
		IUnknownPtr sptr_child( objList.GetNextObject( pos ), false );
		if( sptr_child == 0 )
			continue;

		long lchild_class = -1;
		if( bUseGroupByClass )
		{
			ICalcObjectPtr sptrCalc = sptr_child;
			if( sptrCalc != 0 )
			{
					lchild_class = get_object_class( sptrCalc, strClassifierName.GetBuffer(strClassifierName.GetLength()) );
                    strClassifierName.ReleaseBuffer( );
			}
				
		}
		else
			lchild_class = -2;
        
		SPUNK_LIST *plist = 0;
		TPOS list_pos = m_map_class_to_objects.find(lchild_class);
		if( list_pos )
			plist = m_map_class_to_objects.get( list_pos );
		else
		{
            plist = new SPUNK_LIST;
			m_map_class_to_objects.set( plist, lchild_class );
		}

		if( plist )
			plist->add_tail( sptr_child );
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CActionSetListZOrder Implementation

CActionSetListZOrder::CActionSetListZOrder()
{
}

CActionSetListZOrder::~CActionSetListZOrder()
{
}

bool CActionSetListZOrder::Invoke()
{
	int nZOrder = GetArgumentInt("ZOrder");
	int nClass = GetArgumentInt("UsedClass");
	CString sClassifierName = GetArgumentString("ClassifierName");
	CString sListName = GetArgumentString("ListName");
	IUnknownPtr punkList;
	if (sListName.IsEmpty())
		punkList = IUnknownPtr(::GetActiveObjectFromDocument(m_punkTarget, szTypeObjectList),
			false);
	else
		punkList = IUnknownPtr(::GetObjectByName(m_punkTarget, sListName, szTypeObjectList),
			false);
	INamedDataObject2Ptr sptrND(punkList);
	CString sName = ::GetObjectName(punkList);
	TPOS lPos = 0;
	sptrND->GetFirstChildPosition(&lPos);
	while( lPos )
	{
		IUnknownPtr punkChild;
		sptrND->GetNextChild(&lPos, &punkChild);
		if (nClass == -1 || get_object_class(ICalcObjectPtr(punkChild), sClassifierName.GetBuffer()) == nClass)
		{
			IMeasureObject2Ptr sptrMO2(punkChild);
			if (sptrMO2 != 0)
				sptrMO2->SetZOrder(nZOrder);
		}
	}

	return true;
}

bool CActionSetListZOrder::IsAvaible()
{
	return true;
}

