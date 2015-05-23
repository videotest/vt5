#include "stdafx.h"
#include "filteractions.h"
#include "resource.h"
#include "math.h"

#include "binimageint.h"
#include "binary.h"

#define PI	3.1415926535897932384626433832795

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionEqualContrast, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEqualHists, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionUnsharpenMask, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionExtractPhases, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionKircsh, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionLocalEqualize, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEqualize, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionLaplas, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAverage, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMedian, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSharping, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionBriContrast, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEmboss, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionNegative, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionBinaryExtractPhases, CCmdTargetEx);

//olecreate 

// {2BC695C2-4F82-4af0-9E1E-56E5B67AE18A}
GUARD_IMPLEMENT_OLECREATE(CActionEqualContrast, "FiltersMain.EqualContrast",
0x2bc695c2, 0x4f82, 0x4af0, 0x9e, 0x1e, 0x56, 0xe5, 0xb6, 0x7a, 0xe1, 0x8a);
// {13A9F272-CF1F-4779-8CD9-EEA3E346DFEA}
GUARD_IMPLEMENT_OLECREATE(CActionEqualHists, "FiltersMain.EqualHists",
0x13a9f272, 0xcf1f, 0x4779, 0x8c, 0xd9, 0xee, 0xa3, 0xe3, 0x46, 0xdf, 0xea);
// {5F55BED2-B21D-416b-A107-9761C2DD1524}
GUARD_IMPLEMENT_OLECREATE(CActionUnsharpenMask, "FiltersMain.UnsharpenMask",
0x5f55bed2, 0xb21d, 0x416b, 0xa1, 0x7, 0x97, 0x61, 0xc2, 0xdd, 0x15, 0x24);
// {309248A3-0499-11d4-A0C7-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionExtractPhases, "filtersmain.ExtractPhases",
0x309248a3, 0x499, 0x11d4, 0xa0, 0xc7, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {B85F6F2D-90F2-4f15-91AA-43DC8A437731}
GUARD_IMPLEMENT_OLECREATE(CActionKircsh, "FiltersMain.Kircsh",
0xb85f6f2d, 0x90f2, 0x4f15, 0x91, 0xaa, 0x43, 0xdc, 0x8a, 0x43, 0x77, 0x31);
// {250EA9A5-E159-46f1-8021-160F736833D5}
GUARD_IMPLEMENT_OLECREATE(CActionLocalEqualize, "FiltersMain.LocalEqualize",
0x250ea9a5, 0xe159, 0x46f1, 0x80, 0x21, 0x16, 0xf, 0x73, 0x68, 0x33, 0xd5);
// {94192524-C3C9-4f63-9D9D-C99C5DC5B0A6}
GUARD_IMPLEMENT_OLECREATE(CActionEqualize, "FiltersMain.Equalize",
0x94192524, 0xc3c9, 0x4f63, 0x9d, 0x9d, 0xc9, 0x9c, 0x5d, 0xc5, 0xb0, 0xa6);
// {18F0B866-A07C-42bf-BF4D-E76673CC00CA}
GUARD_IMPLEMENT_OLECREATE(CActionLaplas, "FiltersMain.Laplas",
0x18f0b866, 0xa07c, 0x42bf, 0xbf, 0x4d, 0xe7, 0x66, 0x73, 0xcc, 0x0, 0xca);
// {4DDE99B8-CDE7-4087-B287-02308A250736}
GUARD_IMPLEMENT_OLECREATE(CActionAverage, "FiltersMain.Average",
0x4dde99b8, 0xcde7, 0x4087, 0xb2, 0x87, 0x2, 0x30, 0x8a, 0x25, 0x7, 0x36);
// {91C6D9EC-7774-40cf-A3FC-682EA7DF4820}
GUARD_IMPLEMENT_OLECREATE(CActionMedian, "FiltersMain.Median",
0x91c6d9ec, 0x7774, 0x40cf, 0xa3, 0xfc, 0x68, 0x2e, 0xa7, 0xdf, 0x48, 0x20);
// {56B4DDEA-2134-4037-90DC-7CD4EED2B923}
GUARD_IMPLEMENT_OLECREATE(CActionSharping, "FiltersMain.Sharping",
0x56b4ddea, 0x2134, 0x4037, 0x90, 0xdc, 0x7c, 0xd4, 0xee, 0xd2, 0xb9, 0x23);
// {E50E3F2B-9997-47e0-B014-8A9A4D65947B}
GUARD_IMPLEMENT_OLECREATE(CActionBriContrast, "FiltersMain.BriContrast",
0xe50e3f2b, 0x9997, 0x47e0, 0xb0, 0x14, 0x8a, 0x9a, 0x4d, 0x65, 0x94, 0x7b);
// {F7690357-50B2-11d3-A623-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionEmboss, "FiltersMain.Emboss",
0xf7690357, 0x50b2, 0x11d3, 0xa6, 0x23, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {F7690353-50B2-11d3-A623-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionNegative, "FiltersMain.Negative",
0xf7690353, 0x50b2, 0x11d3, 0xa6, 0x23, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {49D76A99-4D88-45a9-8E2A-561C2722F692}
GUARD_IMPLEMENT_OLECREATE(CActionBinaryExtractPhases, "FiltersMain.BinayExtractPhases", 
0x49d76a99, 0x4d88, 0x45a9, 0x8e, 0x2a, 0x56, 0x1c, 0x27, 0x22, 0xf6, 0x92);

// guidinfo 

// {3DC78A22-3530-4bc1-A2D6-770EE447C628}
static const GUID guidEqualContrast =
{ 0x3dc78a22, 0x3530, 0x4bc1, { 0xa2, 0xd6, 0x77, 0xe, 0xe4, 0x47, 0xc6, 0x28 } };
// {66EAFFD5-AB3D-49bb-9A34-6AF10A1FBCA7}
static const GUID guidEqualHists =
{ 0x66eaffd5, 0xab3d, 0x49bb, { 0x9a, 0x34, 0x6a, 0xf1, 0xa, 0x1f, 0xbc, 0xa7 } };
// {DAA266E0-BF64-44a8-8CE3-A5B8D9621D0D}
static const GUID guidUnsharpenMask =
{ 0xdaa266e0, 0xbf64, 0x44a8, { 0x8c, 0xe3, 0xa5, 0xb8, 0xd9, 0x62, 0x1d, 0xd } };
// {309248A1-0499-11d4-A0C7-0000B493A187}
static const GUID guidExtractPhases =
{ 0x309248a1, 0x499, 0x11d4, { 0xa0, 0xc7, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {DAFE9D62-F743-4697-8A0F-00227F453BC4}
static const GUID guidKircsh =
{ 0xdafe9d62, 0xf743, 0x4697, { 0x8a, 0xf, 0x0, 0x22, 0x7f, 0x45, 0x3b, 0xc4 } };
// {F995FE11-89A5-4473-AEAA-00A365F69325}
static const GUID guidLocalEqualize =
{ 0xf995fe11, 0x89a5, 0x4473, { 0xae, 0xaa, 0x0, 0xa3, 0x65, 0xf6, 0x93, 0x25 } };
// {B6625AFE-91DE-48be-95B4-A25891A048E7}
static const GUID guidEqualize =
{ 0xb6625afe, 0x91de, 0x48be, { 0x95, 0xb4, 0xa2, 0x58, 0x91, 0xa0, 0x48, 0xe7 } };
// {3B7A85BE-894F-4cc1-AE2F-BB2CED841E22}
static const GUID guidLaplas =
{ 0x3b7a85be, 0x894f, 0x4cc1, { 0xae, 0x2f, 0xbb, 0x2c, 0xed, 0x84, 0x1e, 0x22 } };
// {CA757FD5-BC58-4ff3-A020-44DE00BA06B8}
static const GUID guidAverage =
{ 0xca757fd5, 0xbc58, 0x4ff3, { 0xa0, 0x20, 0x44, 0xde, 0x0, 0xba, 0x6, 0xb8 } };
// {0A6A050B-BE39-4521-9455-B38AB9B0BD2C}
static const GUID guidMedian =
{ 0xa6a050b, 0xbe39, 0x4521, { 0x94, 0x55, 0xb3, 0x8a, 0xb9, 0xb0, 0xbd, 0x2c } };
// {8650F113-5B0D-48a8-9D95-1B9168544171}
static const GUID guidSharping =
{ 0x8650f113, 0x5b0d, 0x48a8, { 0x9d, 0x95, 0x1b, 0x91, 0x68, 0x54, 0x41, 0x71 } };
// {B92FBF0A-54F2-45a9-842C-03D01953AC18}
static const GUID guidBriContrast =
{ 0xb92fbf0a, 0x54f2, 0x45a9, { 0x84, 0x2c, 0x3, 0xd0, 0x19, 0x53, 0xac, 0x18 } };
// {F7690355-50B2-11d3-A623-0090275995FE}
static const GUID guidEmboss =
{ 0xf7690355, 0x50b2, 0x11d3, { 0xa6, 0x23, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {F7690351-50B2-11d3-A623-0090275995FE}
static const GUID guidNegative =
{ 0xf7690351, 0x50b2, 0x11d3, { 0xa6, 0x23, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {5B97B7A1-EDFF-11d4-9D41-009027982EB4}
static const GUID guidBinayExtractPhases = 
{ 0x5b97b7a1, 0xedff, 0x11d4, { 0x9d, 0x41, 0x0, 0x90, 0x27, 0x98, 0x2e, 0xb4 } };


//[ag]6. action info

ACTION_INFO_FULL(CActionEqualContrast, IDS_ACTION_EQUALCONTRAST, IDS_MENU_FILTERS, IDS_TB_MAIN, guidEqualContrast);
ACTION_INFO_FULL(CActionEqualHists, IDS_ACTION_EQUALHISTS, IDS_MENU_FILTERS, IDS_TB_MAIN, guidEqualHists);
ACTION_INFO_FULL(CActionUnsharpenMask, IDS_ACTION_UNSHARPENMASK, IDS_MENU_FILTERS, IDS_TB_MAIN, guidUnsharpenMask);
ACTION_INFO_FULL(CActionExtractPhases, IDS_ACTION_EXTRACT_PHASES, IDS_MENU_FILTERS, IDS_TB_MAIN, guidExtractPhases);
ACTION_INFO_FULL(CActionKircsh, IDS_ACTION_KIRCSH, IDS_MENU_FILTERS, IDS_TB_MAIN, guidKircsh);
ACTION_INFO_FULL(CActionLocalEqualize, IDS_ACTION_LOCAL_EQUALIZE, IDS_MENU_FILTERS, IDS_TB_MAIN, guidLocalEqualize);
ACTION_INFO_FULL(CActionEqualize, IDS_ACTION_EQUALIZE, IDS_MENU_FILTERS, IDS_TB_MAIN, guidEqualize);
ACTION_INFO_FULL(CActionLaplas, IDS_ACTION_LAPLAS, IDS_MENU_FILTERS, IDS_TB_MAIN, guidLaplas);
ACTION_INFO_FULL(CActionAverage, IDS_ACTION_AVERAGE, IDS_MENU_FILTERS, IDS_TB_MAIN, guidAverage);
ACTION_INFO_FULL(CActionMedian, IDS_ACTION_MEDIAN, IDS_MENU_FILTERS, IDS_TB_MAIN, guidMedian);
ACTION_INFO_FULL(CActionSharping, IDS_ACTION_SHARPING, IDS_MENU_FILTERS, IDS_TB_MAIN, guidSharping);
ACTION_INFO_FULL(CActionBriContrast, IDS_ACTION_BRI_CONTRAST, IDS_MENU_FILTERS, IDS_TB_MAIN, guidBriContrast);
ACTION_INFO_FULL(CActionEmboss, IDS_ACTION_EMBOSS, IDS_MENU_FILTERS, IDS_TB_MAIN, guidEmboss);
ACTION_INFO_FULL(CActionNegative, IDS_ACTION_NEGATIVE, IDS_MENU_FILTERS, IDS_TB_MAIN, guidNegative);

ACTION_INFO_FULL(CActionBinaryExtractPhases, IDS_ACTION_BINARY_EXTRACT_PHASES, IDS_MENU_FILTERS, IDS_TB_MAIN, guidBinayExtractPhases);



//[ag]7. targets

ACTION_TARGET(CActionEqualContrast, "anydoc");
ACTION_TARGET(CActionEqualHists, szTargetAnydoc);
ACTION_TARGET(CActionUnsharpenMask, szTargetAnydoc);
ACTION_TARGET(CActionExtractPhases, szTargetAnydoc);
ACTION_TARGET(CActionKircsh, "anydoc");
ACTION_TARGET(CActionLocalEqualize, "anydoc");
ACTION_TARGET(CActionEqualize, "anydoc");
ACTION_TARGET(CActionLaplas, "anydoc");
ACTION_TARGET(CActionAverage, "anydoc");
ACTION_TARGET(CActionMedian, "anydoc");
ACTION_TARGET(CActionSharping, "anydoc");
ACTION_TARGET(CActionBriContrast, szTargetAnydoc);
ACTION_TARGET(CActionEmboss, szTargetAnydoc);
ACTION_TARGET(CActionNegative, szTargetAnydoc);
ACTION_TARGET(CActionBinaryExtractPhases, szTargetAnydoc);

//[ag]8. arguments

ACTION_ARG_LIST(CActionEqualContrast)
	ARG_IMAGE( _T("Image0") )
	ARG_IMAGE( _T("Image1") )
	RES_IMAGE( _T("Result Equal") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionEqualHists)
	ARG_IMAGE( _T("Image0") )
	ARG_IMAGE( _T("Image1") )
	RES_IMAGE( _T("EqualHists") )
END_ACTION_ARG_LIST();


ACTION_ARG_LIST(CActionUnsharpenMask)
	ARG_INT(_T("MaskSize"), 5 )
	ARG_INT(_T("Threshold"), 0 )
	ARG_INT(_T("Strength"), 100 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("UnsharpenMask") )
END_ACTION_ARG_LIST();


ACTION_ARG_LIST(CActionExtractPhases)
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Phases") )
END_ACTION_ARG_LIST();


ACTION_ARG_LIST(CActionKircsh)
	ARG_INT(_T("Level"), 1 )
	ARG_INT(_T("Add"), 0 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Kirsch") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionLocalEqualize)
	ARG_INT(_T("MaskSize"), 5 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Equalize") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionEqualize)
	ARG_INT(_T("FullByFrame"), 0 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Equalize") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionMedian)
	ARG_INT(_T("MaskSize"), 5 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Median") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionAverage)
	ARG_INT(_T("MaskSize"), 5 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Average") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionLaplas)
	ARG_INT(_T("Add"), 0 )	
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Laplas") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionEmboss)
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Emboss") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionNegative)
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Negative") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionBriContrast)
	ARG_INT(_T("Brightness"), 0 )
	ARG_INT(_T("Contrast"), 0 )
	ARG_DOUBLE(_T("Gamma"), 0 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("BriContrast") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionSharping)
	ARG_INT(_T("Strength"), 0 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Sharping") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionBinaryExtractPhases)
	ARG_IMAGE( _T("Source") )
	RES_BINIMAGE( _T("Phase") )	
	//RES_BINIMAGE( _T("Phase2") )	
END_ACTION_ARG_LIST();



inline color MakeColor( double f )
{
	f = min( colorMax, max( 0, f ) );
	return (color)f;
}

//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionEqualContrast implementation
CActionEqualContrast::CActionEqualContrast()
{
}

CActionEqualContrast::~CActionEqualContrast()
{
}

bool CActionEqualContrast::CanDeleteArgument( CFilterArgument *pa )
{
	if( pa->m_strArgName == "Image1" )
		return false;
	return CFilterBase::CanDeleteArgument( pa );
}

bool CActionEqualContrast::InvokeFilter()
{
	const WORD MaxColor = (color)-1;
	long	lPercent = 10;

	CImageWrp	image0( GetDataArgument("Image0") );
	CImageWrp	image1( GetDataArgument("Image1") );
	if (image0.GetColorConvertor() != image1.GetColorConvertor())
	{
		AfxMessageBox(IDS_DIFFERENT_MODEL, MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	long	colors = min(image0.GetColorsCount(), image1.GetColorsCount());

	long	cx0 = image0.GetWidth(), cy0 = image0.GetHeight();
	long	cx1 = image1.GetWidth(), cy1 = image1.GetHeight();
		
	CImageWrp	imageNew( GetDataResult() );

	{
		ICompatibleObjectPtr ptrCO( imageNew );
		ptrCO->CreateCompatibleObject( image0, NULL, 0 );
	}

	//imageNew.CreateCompatibleImage( image0 );

	long nCX = max(cx0, cx1);
	long nCY = max(cy0, cy1);

	long	plHist1[MaxColor+1];
	long	plHist2[MaxColor+1];
	
	long	plColorCoding[MaxColor+1];


	long	x, y, c;
	color	*p, *pn;
	byte	*pbyteMask = 0;

	StartNotification( colors*4 );

	CImagePaneCache paneCache( imageNew );

	for( c = 0; c < colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		ZeroMemory( plHist1, sizeof(plHist1) );
		ZeroMemory( plHist2, sizeof(plHist2) );
		ZeroMemory( plColorCoding, sizeof(plColorCoding) );

		for( y = 0; y < cy0; y++ )
		{
			p = image0.GetRow( y, c );
			pbyteMask = image0.GetRowMask( y );
			for( x = 0; x < cx0; x++, p++, pbyteMask++ )
				if( *pbyteMask )
					plHist1[*p]++;
		}

		Notify( 4*c + 0 );

		for( y = 0; y < cy1; y++ )
		{
			p = image1.GetRow( y, c );
			pbyteMask = image1.GetRowMask( y );
			for( x = 0; x < cx1; x++, p++, pbyteMask++ )
				if( *pbyteMask )
					plHist2[*p]++;
		}

		Notify( 4*c + 1 );

		long	lLim1 = cx0*cy0*lPercent/100;
		long	lLim2 = cx1*cy1*lPercent/100;

		long	mn1 = 0, mn2 = 0, mx1 = MaxColor, mx2 = MaxColor;
		long	s;

		s = 0;
		while( true )
		{
			s += plHist1[mn1];
			if( s>=lLim1 )break;
			if( mn1 == mx1-1 )
				break;
			mn1++;
		}

		s = 0;
		while( true )
		{
			s += plHist1[mx1];
			if( s>=lLim1 )break;
			if( mx1 == mn1+1 )
				break;
			mx1--;
		}

		s = 0;
		while( true )
		{
			s += plHist2[mn2];
			if( s>=lLim2 )break;
			if( mn2 == mx2-1 )
				break;
			mn2++;
		}

		s = 0;
		while( true )
		{
			s += plHist2[mx2];
			if( s>=lLim2 )break;
			if( mx2 == mn2+1 )
				break;
			mx2--;
		}

		for( int cc = 0; cc <= colorMax; cc++ )
			plColorCoding[cc] = max( 0, min( colorMax, 
				long( (double)(cc-mn1)*(mx2-mn2)/(mx1-mn1)+mn2)));

		Notify( 4*c + 2 );

		for( y = 0; y < cy0; y++ )
		{
			p = image0.GetRow( y, c );
			pn = imageNew.GetRow( y, c );
			pbyteMask = imageNew.GetRowMask( y );
			
			for( x = 0; x < cx0; x++, p++, pn++, pbyteMask++ )
				if( *pbyteMask )
					*pn = plColorCoding[*p];
		}

		Notify( 4*c + 3 );
	}

	FinishNotification();

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionEqualHists implementation
CActionEqualHists::CActionEqualHists()
{
}

CActionEqualHists::~CActionEqualHists()
{
}

bool CActionEqualHists::InvokeFilter()
{
	const WORD MaxColor = (color)-1;

	CImageWrp	image0( GetDataArgument("Image0") );
	CImageWrp	image1( GetDataArgument("Image1") );

	long	colors = min(image0.GetColorsCount(), image1.GetColorsCount());

	long	cx0 = image0.GetWidth(), cy0 = image0.GetHeight();
	long	cx1 = image1.GetWidth(), cy1 = image1.GetHeight();
		
	double	dCoef = double(cx0*cy0)/double(cx1*cy1);
		
	CImageWrp	imageNew( GetDataResult() );

	{
		ICompatibleObjectPtr ptrCO( imageNew );
		ptrCO->CreateCompatibleObject( image1, NULL, 0 );
	}	

	//imageNew.CreateCompatibleImage( image1 );

	long nCX = max(cx0, cx1);
	long nCY = max(cy0, cy1);

	long	plHistSource[MaxColor+1];
	long	plHistResult[MaxColor+1];
	
	long	plColorCoding[MaxColor+1];


	StartNotification(nCY, 2*colors);

	
	CImagePaneCache paneCache( imageNew );

	for(long c = 0; c < colors; c++)
	{

		if( !paneCache.IsPaneEnable( c ) ) 
			continue;


		ZeroMemory(plHistSource, sizeof(plHistSource));
		ZeroMemory(plHistResult, sizeof(plHistResult));
		//ZeroMemory(plColorCoding, sizeof(plColorCoding));
		memset(plColorCoding, -1, sizeof(plColorCoding));
		DWORD	dwDestColorSum = 0, dwCurColorSum = 0;
		long	lCurrColor = 0, lDestColor = 0;
		long lastGoodColor = -1;
		
	
		if(c != 0)
			NextNotificationStage();

		for(long y = 0; y < nCY; y++)
		{
			color* psrc0 = 0;
			color* psrc1 = 0;
			if(y < cy0)
				psrc0 = image0.GetRow(y, c);
			if(y < cy1)
				psrc1 = image1.GetRow(y, c);

			for(long x = 0; x < nCX; x++)
			{
				if(x < cx0 && psrc0)
					plHistSource[psrc0[x]]++;
				if(x < cx1 && psrc1)
					plHistResult[psrc1[x]]++;
			}

			Notify(y);
		}
	
		for (int nClr = 0; nClr < MaxColor+1; nClr++)
			plHistResult[nClr] = (long)(dCoef*plHistResult[nClr]);

#if 0
		for (lCurrColor = 0; lCurrColor < MaxColor; lCurrColor++)
		{
			dwCurColorSum += plHistResult[lCurrColor];
			dwDestColorSum = 0;
			for (lDestColor = 0; lDestColor < MaxColor; lDestColor++)
			{
				if (dwDestColorSum >= dwCurColorSum)
					break;
				dwDestColorSum += plHistSource[lDestColor];
			}
			plColorCoding[lCurrColor] = lDestColor;
		}
#else
		while( lDestColor < MaxColor )
		{
			if(lDestColor > 0)
			{
				plColorCoding[lDestColor-1] = lCurrColor-1;
			}
			
			while( (dwDestColorSum <= dwCurColorSum) && (lDestColor < MaxColor) )
			{
				dwDestColorSum+=plHistResult[lDestColor];
				
				lDestColor++;
			}
			while( (dwCurColorSum < dwDestColorSum) && (lCurrColor < MaxColor) )
			{
				dwCurColorSum += plHistSource[lCurrColor];
				lCurrColor++;
			}

			if( lCurrColor == MaxColor )
				dwCurColorSum = (DWORD)0x9FFFFFFF;
		}

		// A.M. BT4880
		long lPrevSrcColor = 0;
		long lPrevDstColor = 0;
		bool bGap = false;
		for (long lColor = 0; lColor <= MaxColor; lColor++)
		{
			if (plColorCoding[lColor] == -1)
				bGap = true;
			else
			{
				if (bGap)
				{
					for (long l = lPrevSrcColor+1; l < lColor; l++)
						plColorCoding[l] = lPrevDstColor+(plColorCoding[lColor]-
							lPrevDstColor)*(l-lPrevSrcColor)/(lColor-lPrevSrcColor);
				}
				lPrevSrcColor = lColor;
				lPrevDstColor = plColorCoding[lColor];
				bGap = false;
			}
		}
		if (bGap)
		{
			for (long l = lPrevSrcColor+1; l <= MaxColor; l++)
				plColorCoding[l] = lPrevDstColor+(MaxColor-
					lPrevDstColor)*(l-lPrevSrcColor)/(MaxColor-lPrevSrcColor);
		}
#endif

		NextNotificationStage();

		for(y = 0; y < cy1; y++)
		{
			color* pdst = imageNew.GetRow(y, c);
			color* psrc1 = image1.GetRow(y, c);
			for(long x = 0; x < cx1; x++)
			{
				if(plColorCoding[psrc1[x]] >= 0)
					pdst[x] = min(max(plColorCoding[psrc1[x]] , 0), MaxColor);
				else
					pdst[x] = psrc1[x];
			}

			Notify(y);
		}

	}

	FinishNotification();

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionUnsharpenMask implementation
CActionUnsharpenMask::CActionUnsharpenMask()
{
}

CActionUnsharpenMask::~CActionUnsharpenMask()
{
}


void CActionUnsharpenMask::DoDrawMaskShape( CDC *pdc, int xCenter, int yCenter, int nMaskSize )
{
	pdc->Rectangle( CRect( xCenter-nMaskSize/2, yCenter-nMaskSize/2, 
						xCenter-nMaskSize/2+nMaskSize, yCenter-nMaskSize/2+nMaskSize ) );
}

bool CActionUnsharpenMask::InvokeFilter()
{
	int	nMaskSize = GetArgumentInt( "MaskSize" );
	int	nThreshold = GetArgumentInt( "Threshold" )*256;
	double fStrength = GetArgumentInt( "Strength" )/100.;

	
	if( nMaskSize  < 2 )
		return false;

	nThreshold = max(nThreshold, 0);
	fStrength = max(fStrength, 0);

	if( !InitImageFilter() )return false;
	if( !InitHistMask( nMaskSize ) )return false;

	int	x, y, c;
	color	*pdest;
	color	*psrc;
	byte	*pmask;

	CImagePaneCache paneCache( m_imageNew );

	for( c = 0; c <  m_colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		InitColorCache( c );
		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			pdest = m_imageNew.GetRow( y, c );
			psrc = m_image.GetRow( y, c );


			InitMask( y );

			for( x = 0; x < m_cx; x++, pmask++, pdest++, psrc++)
			{
				if( *pmask )
				{
					long nRes = m_nCurrentHistArea*(*psrc) - m_nCurrentHistSum;
					if(nRes >= nThreshold||nRes <= -nThreshold)
					{
						*pdest = max(0, min((color)-1, ((*psrc)*m_nCurrentHistArea + fStrength*nRes)/m_nCurrentHistArea));
					}
					else
					{
						*pdest = *psrc;
					}
				}

				MoveMaskRight();
			}

			NotifyImage( y, c );
		}
	}

		
	FinalizeImageFilter();
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionExtractPhases implementation
CActionExtractPhases::CActionExtractPhases()
{
	m_ppcolorscr = 0;
	m_ppcolordest = 0;
	m_ppHiIntervals = 0;
	m_ppLoIntervals = 0;
	m_pClassColors = 0;
	m_nClasses = 0;
}

CActionExtractPhases::~CActionExtractPhases()
{
	if(m_ppcolordest)
		delete m_ppcolordest;
	if(m_ppcolorscr)
		delete m_ppcolorscr;

	for(long i = 0; i < m_nClasses; i++)
	{
		if(m_ppHiIntervals[i]) delete m_ppHiIntervals[i];
		if(m_ppLoIntervals[i]) delete m_ppLoIntervals[i];
		if(m_pClassColors[i]) delete m_pClassColors[i];
	}
	if(m_ppHiIntervals)
		delete m_ppHiIntervals;
	if(m_ppLoIntervals)
		delete m_ppLoIntervals;
	if(m_pClassColors)
		delete m_pClassColors;
}

bool CActionExtractPhases::InvokeFilter()
{
	const char* szSectionName = "ExtractPhases";

	CImageWrp	image( GetDataArgument() );
	CImageWrp	imageNew( GetDataResult() );

	long	cx = image.GetWidth(), cy = image.GetHeight();

	imageNew.CreateCompatibleImage( image,  true, NORECT);

	CString strEntry;

	m_nClasses = ::GetValueInt(GetAppUnknown(), szSectionName, "ClassCount", 0);
	if(m_nClasses <= 0) return true;

	m_ppHiIntervals = new DWORD*[m_nClasses];
	m_ppLoIntervals = new DWORD*[m_nClasses];
	m_pClassColors = new DWORD*[m_nClasses];

	int	colors = image.GetColorsCount();

	IUnknown *punkCC;
	image->GetColorConvertor( &punkCC );
	sptrIColorConvertor5 sptrCC(punkCC);
	if(punkCC)
		punkCC->Release();

	
	
	for(long i = 0; i < m_nClasses; i++)
	{
		strEntry.Format("Class%dColor", i);
		long nClassColor = ::GetValueInt(GetAppUnknown(), szSectionName, strEntry, RGB(0,0,0));
		if(colors > 0)
		{
			color** ppColors = new color*[colors];
			byte b[3];
			b[0] = (byte)GetBValue(nClassColor); 
			b[1] = (byte)GetGValue(nClassColor);
			b[2] = (byte)GetRValue(nClassColor);
			for (int j = 0; j < colors; j++)
			{
				ppColors[j] = new color[1];
			}
			sptrCC->ConvertDIBToImageEx( b, ppColors, 1, TRUE, colors ); 

			m_ppHiIntervals[i] = new DWORD[colors];
			m_ppLoIntervals[i] = new DWORD[colors];
			m_pClassColors[i]  = new DWORD[colors];
			for(j = 0; j < colors; j++)
			{
				strEntry.Format("Class%dPane%d_Hi", i, j);
				m_ppHiIntervals[i][j] = ::GetValueInt(GetAppUnknown(), szSectionName, strEntry, 0);
				strEntry.Format("Class%dPane%d_Lo", i, j);
				m_ppLoIntervals[i][j] = ::GetValueInt(GetAppUnknown(), szSectionName, strEntry, 0);
				m_pClassColors[i][j] = *ppColors[j];
				delete ppColors[j];
			}
			delete ppColors;
		}
		else
		{
			m_ppHiIntervals[i] = 0;
			m_ppLoIntervals[i] = 0;
		}
	}


	StartNotification( cy );

	
	m_ppcolorscr = new color*[colors];
	m_ppcolordest = new color*[colors];
	for( long y = 0; y < cy; y++ )
	{
		for( long c = 0; c  < colors; c++ )
		{
			m_ppcolorscr[c] = image.GetRow( y, c );
			m_ppcolordest[c] = imageNew.GetRow( y, c );
		}
		for(long x = 0; x < cx; x++)
		{
			bool bUnderClass = false;
			for(long i = m_nClasses-1; i >= 0; i--)
			{
				bool bInRange = true;
				for(int j = 0; j < colors; j++)
				{
					bInRange = bInRange &&(*(m_ppcolorscr[j]+x) >= m_ppLoIntervals[i][j] && *(m_ppcolorscr[j]+x) < m_ppHiIntervals[i][j]);
					if(!bInRange) break;
				}

				if(bInRange)
				{
					for(j = 0; j < colors; j++)
					{
						*(m_ppcolordest[j]+x) = m_pClassColors[i][j];
					}

					bUnderClass = true;
				}
			}
			if(!bUnderClass)
			for(int j = 0; j < colors; j++)
			{
				*(m_ppcolordest[j]+x) = *(m_ppcolorscr[j]+x);
			}
		}

		Notify( y );
	}

	
	FinishNotification();


	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionKircsh implementation
CActionKircsh::CActionKircsh()
{
}

CActionKircsh::~CActionKircsh()
{
}

bool CActionKircsh::InvokeFilter()
{
	int nLevel = GetArgumentInt( "Level" );
	int nAdd = GetArgumentInt( "Add" );

	if( nLevel < 0 || nLevel > 20 )
		return false;

	if( !InitImageFilter() )return false;

	int	x, y, c;
	color	*psrc1, *psrc2, *psrc3;
	color	*pdest;
	byte	*pmask;

	int		a, b, iDelta, kirsch, h;
	color	c11, c12, c13, c21, c22, c23, c31, c32, c33;


	CImagePaneCache paneCache( m_imageNew );


	for( c = 0; c <  m_colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		psrc1 = psrc2 = psrc3 = m_image.GetRow( 0, c );

		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			pdest = m_imageNew.GetRow( y, c );
			psrc1 = psrc2;
			psrc2 = psrc3;
			if( y < m_cy-1 )psrc3 = m_image.GetRow( y+1, c );

			c11 = c12 = c13 = *psrc1;
			c21 = c22 = c23 = *psrc2;
			c31 = c32 = c33 = *psrc3;


			for( x = 0; x < m_cx; x++, pmask++, pdest++ )
			{
				c11 = c12; c21 = c22; c31 = c32;
				c12 = c13; c22 = c23; c32 = c33;

				if( x < m_cx-1 )
				{
					c13 = psrc1[x+1];
					c23 = psrc2[x+1];
					c33 = psrc3[x+1];
				}


				if( !*pmask )
					continue;

				a  = c11 + c12 + c13;
				b  = c21 + c23 + c31 + c32 + c33;
	
				kirsch = abs( 5 * a - 3 * b );
				iDelta = c13 - c33;
				a -= iDelta;
				b += iDelta;
				h = abs( 5 * a - 3 * b );
				kirsch = max( kirsch, h );
				iDelta = c12 - c32;
				a -= iDelta;
				b += iDelta;
				h = abs( 5 * a - 3 * b );
				kirsch = max( kirsch, h );
				iDelta = c11 - c31;
				a -= iDelta;
				b += iDelta;
				h = abs( 5 * a - 3 * b );
				kirsch = max( kirsch, h );
				iDelta = c33 - c23;
				a -= iDelta;
				b += iDelta;
				h = abs( 5 * a - 3 * b );
				kirsch = max( kirsch, h );
				iDelta = c32 - c21;
				a -= iDelta;
				b += iDelta;
				h = abs( 5 * a - 3 * b );
				kirsch = max( kirsch, h );
				iDelta = c31 - c13;
				a -= iDelta;
				b += iDelta;
				h = abs( 5 * a - 3 * b );
				kirsch = max( kirsch, h );
				iDelta = c23 - c12;
				a -= iDelta;
				b += iDelta;
				h = abs( 5 * a - 3 * b );

				kirsch = max( kirsch, h )*nLevel / 15;

				if( nAdd )
					*pdest = min( kirsch+c22, colorMax );
				else
					*pdest = kirsch;

				


				//*pdest = min( colorMax, max( 0, abs( (c22<<3)-c11-c12-c13-c21-c23-c31-c32-c33 )<<1 ) );
			}

			NotifyImage( y, c );
		}
	}
	FinalizeImageFilter();
	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionLocalEqualize implementation
CActionLocalEqualize::CActionLocalEqualize()
{
}

CActionLocalEqualize::~CActionLocalEqualize()
{
}

bool CActionLocalEqualize::InvokeFilter()
{
	int	nMaskSize = GetArgumentInt( "MaskSize" );

	if( nMaskSize  < 2 )
		return false;

	if( !InitImageFilter() )return false;
	if( !InitHistMask( nMaskSize ) )return false;

	int	x, y, c;
	color	*pdest;
	byte	*pmask;


	CImagePaneCache paneCache( m_imageNew );

	color	*psrc  = 0;
	for( c = 0; c <  m_colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		InitColorCache( c );
		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			pdest = m_imageNew.GetRow( y, c );
			psrc = m_pRowCache[y+nMaskSize];

			InitApprHistMask( y );

			for( x = 0; x < m_cx; x++, pmask++, pdest++, psrc++ )
			{
				if( *pmask )
				{
					byte 	byteCur = ColorAsByte( *psrc );
					DWORD	dwVmin = 0;
					DWORD dwHistSum = 0;
	
					for( int i = 0; i <= byteCur; i++ )
					{
						if( dwHistSum == 0 && m_pCurrentHistAppr[i] )
							dwVmin = m_pCurrentHistAppr[i];

						dwHistSum += m_pCurrentHistAppr[i];
					}

					if( !dwVmin )
						*pdest = 0;
					else
						*pdest = ByteAsColor( byte( min( 255, max( 0, ((double)dwHistSum - dwVmin)*255/max(m_nCurrentHistArea - dwVmin, 1) ) ) ) );

				}
				MoveApprHistMaskRight();
			}

			NotifyImage( y, c );
		}
	}

		
	FinalizeImageFilter();
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionEqualize implementation
CActionEqualize::CActionEqualize()
{
}

CActionEqualize::~CActionEqualize()
{
}

bool CActionEqualize::InvokeFilter()
{
	if( !InitImageFilter() )return false;

	color	*psrc, *pdest;
	byte	*pmask;
	int		x, y, c, i;

	long	*pnHist = new long[colorMax+1];
	color	*pcolorCnv = new color[colorMax+1];

	try
	{

	CImagePaneCache paneCache( m_imageNew );

	for( c = 0; c <  m_colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		DWORD	dwSize = 0;
		ZeroMemory( pnHist, sizeof( long )*(colorMax+1) );
		ZeroMemory( pcolorCnv, sizeof( color )*(colorMax+1) );

		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			psrc = m_image.GetRow( y, c );

			for( x = 0; x < m_cx; x++, pmask++, psrc++ )
			{
				if( *pmask )
				{
						pnHist[*psrc]++;
					dwSize++;
				}
			}

			NotifyImage( y/2, c );
		}

		DWORD	dwVmin = pnHist[0];
		for( i = 0; i <= colorMax; i++ )
		{
			if( pnHist[i] != 0 )
			{
				dwVmin = pnHist[i];
				break;
			}
		}

		pcolorCnv[0] = 0;
		DWORD dwHistSum = pnHist[0];
		DWORD	dwVminVal = i;

		for( i = 1; i <= colorMax; i++ )
		{
			dwHistSum += pnHist[i];
			pcolorCnv[i] = MakeColor( ((double)dwHistSum - dwVmin)*colorMax/(dwSize - dwVmin) );
		}

		for( y = 0; y < m_cyBig; y++ )
		{
			pdest = m_imageNew.GetRow( y, c );
			psrc = m_imageBigSource.GetRow( y, c );

			for( x = 0; x < m_cxBig; x++, psrc++, pdest++ )
				*pdest = pcolorCnv[*psrc];

			NotifyImage( (m_cy+y*m_cy/m_cyBig)/2, c );
		}
	}
	}
	catch( CException *pe )
	{
		pe->Delete();
	}

	delete pcolorCnv;
	delete pnHist;


	return true;
}

void CActionEqualize::InitImageResult()
{
	bool	bByFrame = GetArgumentInt( "FullByFrame" )!=0;

	if( GetParentKey( m_image ) == INVALID_KEY || !bByFrame )
	{
		m_imageBigSource = m_image;
	}
	else
	{
		INamedDataObject2Ptr	ptrN( m_image );
		IUnknown	*punkParent = 0;
		ptrN->GetParent( &punkParent );

		ASSERT( punkParent );
		
		m_imageBigSource = punkParent;
		punkParent->Release();
	}

	ICompatibleObjectPtr ptrCO( m_imageNew );
	if( ptrCO == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	if( S_OK != ptrCO->CreateCompatibleObject( m_imageBigSource, NULL, 0 ) )
	{	
		ASSERT( FALSE );
		return;
	}		
	m_cxBig = m_imageNew.GetWidth();
	m_cyBig = m_imageNew.GetHeight();
}

//////////////////////////////////////////////////////////////////////
//CActionLaplas implementation
CActionLaplas::CActionLaplas()
{
}

CActionLaplas::~CActionLaplas()
{
}

bool CActionLaplas::InvokeFilter()
{
	int nAdd = GetArgumentInt( "Add" );

	if( !InitImageFilter() )return false;

	int	x, y, c;
	color	*psrc1, *psrc2, *psrc3;
	color	*pdest;
	byte	*pmask;

	color	c11, c12, c13, c21, c22, c23, c31, c32, c33;

	CImagePaneCache paneCache( m_imageNew );

	for( c = 0; c <  m_colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		psrc1 = psrc2 = psrc3 = m_image.GetRow( 0, c );

		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			pdest = m_imageNew.GetRow( y, c );
			psrc1 = psrc2;
			psrc2 = psrc3;
			if( y < m_cy-1 )psrc3 = m_image.GetRow( y+1, c );

			c11 = c12 = c13 = *psrc1;
			c21 = c22 = c23 = *psrc2;
			c31 = c32 = c33 = *psrc3;


			for( x = 0; x < m_cx; x++, pmask++, pdest++ )
			{
				c11 = c12; c21 = c22; c31 = c32;
				c12 = c13; c22 = c23; c32 = c33;

				if( x < m_cx-1 )
				{
					c13 = psrc1[x+1];
					c23 = psrc2[x+1];
					c33 = psrc3[x+1];
				}

				if( !*pmask )
					continue;

				//[-1] [-1] [-1]
				//[-1] [ 8] [-1]
				//[-1] [-1] [-1]

				if( nAdd )
					*pdest = min( colorMax, max( 0, psrc2[x]+(abs( (c22<<3)-c11-c12-c13-c21-c23-c31-c32-c33)<<1 ) ) );
				else
					*pdest = min( colorMax, max( 0, abs( (c22<<3)-c11-c12-c13-c21-c23-c31-c32-c33 )<<1 ) );
			}

			NotifyImage( y, c );
		}
	}
	FinalizeImageFilter();
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionAverage implementation
CActionAverage::CActionAverage()
{
}

CActionAverage::~CActionAverage()
{
}


bool CActionAverage::InvokeFilter()
{
	int	nMaskSize = GetArgumentInt( "MaskSize" );

	if( nMaskSize  < 2 )
		return false;

	if( !InitImageFilter() )return false;
	if( !InitHistMask( nMaskSize ) )return false;

	int	x, y, c;
	color	*pdest;
	byte	*pmask;

	CImagePaneCache paneCache( m_imageNew );

	for( c = 0; c <  m_colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		InitColorCache( c );
		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			pdest = m_imageNew.GetRow( y, c );


			InitMask( y );

			for( x = 0; x < m_cx; x++, pmask++, pdest++ )
			{
				if( *pmask )
					*pdest = m_nCurrentHistSum / m_nCurrentHistArea;

				MoveMaskRight();
			}

			NotifyImage( y, c );
		}
	}

		
	FinalizeImageFilter();
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionMedian implementation
CActionMedian::CActionMedian()
{
}

CActionMedian::~CActionMedian()
{
}

bool CActionMedian::InvokeFilter()
{
	//CTimeTest	test( true, "CActionMedian::InvokeFilter()", true );
	int	nMaskSize = GetArgumentInt( "MaskSize" );

	if( nMaskSize  < 2 )
		return false;

	if( !InitImageFilter() )return false;
	if( !InitHistMask( nMaskSize ) )return false;

	int	x, y, c;
	color	*pdest;
	byte	*pmask;


	long	*plHist;
	int	nMedianPos = m_nCurrentHistArea/2, nSum;

	CImagePaneCache paneCache( m_imageNew );

	for( c = 0; c <  m_colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		InitColorCache( c );
		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			pdest = m_imageNew.GetRow( y, c );

			InitApprHistMask( y );

			for( x = 0; x < m_cx; x++, pmask++, pdest++ )
			{
				if( *pmask )
				{
					nSum = 0;
					plHist = m_pCurrentHistAppr;
					int	n = 0;

					while( 	nSum+*plHist < nMedianPos )
					{
						nSum+=*plHist;
						plHist++; 
						n++;
					}
					
					*pdest = ::ByteAsColor( n );
					//*pdest = m_nCurrentMedian;
				}
				MoveApprHistMaskRight();
			}

			NotifyImage( y, c );
		}
	}

		
	FinalizeImageFilter();
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionSharping implementation
CActionSharping::CActionSharping()
{
}

CActionSharping::~CActionSharping()
{
}

bool CActionSharping::InvokeFilter()
{
	int	nStrength = GetArgumentInt( "Strength" );

	if( nStrength  < 0 || nStrength > 2 )
		return false;

	if( !InitImageFilter() )return false;

	int	x, y, c;
	color	*psrc1, *psrc2, *psrc3;
	color	*pdest;
	byte	*pmask;

	color	c11, c12, c13, c21, c22, c23, c31, c32, c33;

	CImagePaneCache paneCache( m_imageNew );


	for( c = 0; c <  m_colors; c++ )
	{

		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		psrc1 = psrc2 = psrc3 = m_image.GetRow( 0, c );

		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			pdest = m_imageNew.GetRow( y, c );
			psrc1 = psrc2;
			psrc2 = psrc3;
			if( y < m_cy-1 )psrc3 = m_image.GetRow( y+1, c );

			c11 = c12 = c13 = *psrc1;
			c21 = c22 = c23 = *psrc2;
			c31 = c32 = c33 = *psrc3;


			if( nStrength  == 0 )//weak
				for( x = 0; x < m_cx; x++, pmask++, pdest++ )
				{
					c11 = c12; c21 = c22; c31 = c32;
					c12 = c13; c22 = c23; c32 = c33;

					if( x < m_cx-1 )
					{
						c13 = psrc1[x+1];
						c23 = psrc2[x+1];
						c33 = psrc3[x+1];
					}

					if( !*pmask )
						continue;

					//[ 1] [-2] [ 1]
					//[-2] [ 5] [-2]
					//[ 1] [-2] [ 1]
					*pdest = min( colorMax, max( 0, 1*c11-2*c12+1*c13-2*c21+5*c22-2*c23+1*c31-2*c32+1*c33 ) );
				}
			else if( nStrength  == 1 )//normal
				for( x = 0; x < m_cx; x++, pmask++, pdest++ )
				{
					c11 = c12; c21 = c22; c31 = c32;
					c12 = c13; c22 = c23; c32 = c33;

					if( x < m_cx-1 )
					{
						c13 = psrc1[x+1];
						c23 = psrc2[x+1];
						c33 = psrc3[x+1];
					}

					if( !*pmask )
						continue;

					//[ 0] [-1] [ 0]
					//[-1] [ 5] [-1]
					//[ 0] [-1] [ 0]
					*pdest = min( colorMax, max( 0, 0*c11-1*c12+0*c13-1*c21+5*c22-1*c23+0*c31-1*c32+0*c33 ) );
				}
			else  if( nStrength  == 2 )//strong
				for( x = 0; x < m_cx; x++, pmask++, pdest++ )
				{
					c11 = c12; c21 = c22; c31 = c32;
					c12 = c13; c22 = c23; c32 = c33;

					if( x < m_cx-1 )
					{
						c13 = psrc1[x+1];
						c23 = psrc2[x+1];
						c33 = psrc3[x+1];
					}

					if( !*pmask )
						continue;

					//[-1] [-1] [-1]
					//[-1] [ 9] [-1]
					//[-1] [-1] [-1]
					*pdest = min( colorMax, max( 0, -1*c11-1*c12-1*c13-1*c21+9*c22-1*c23-1*c31-1*c32-1*c33 ) );
				}
			

			NotifyImage( y, c );
		}
	}
	FinalizeImageFilter();
	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionBriContrast implementation
CActionBriContrast::CActionBriContrast()
{
}

CActionBriContrast::~CActionBriContrast()
{
}


bool CActionBriContrast::InvokeFilter()
{
	CImageWrp	image( GetDataArgument() );
	
	long	colors = image.GetColorsCount();
	long	cx = image.GetWidth(), cy = image.GetHeight();

	//check the entered values. Calc if not specified
	int	nContrast = GetArgumentInt( "Contrast" );
	double	fG = GetArgumentDouble( "Gamma" );
	double	a = GetArgumentInt( "Brightness" )*ByteAsColor( 1 )+colorMax/2;
	double	b = (nContrast != 100) ?::tan( (nContrast+100)/400.*PI ):colorMax;
	double	g = max( 0.001, fG );

	CImageWrp	imageNew( GetDataResult() );

	ICompatibleObjectPtr ptrCO( imageNew );
	ptrCO->CreateCompatibleObject( image, NULL, 0 );

	//imageNew.CreateCompatibleImage( image );

	


	StartNotification( cy );

	color	*pcnvTable = new color[colorMax+1];

	try
	{


	double CnvMul = colorMax/pow((double)colorMax, 1/g);
	for( int i = 0; i <= colorMax; i++ )
	{
		//pcnvTable[i] = MakeColor( (CnvMul*pow(max( 0, min( colorMax, (i-colorMax/2)*b+a) ), 1./g)) );
		//pcnvTable[i] = MakeColor( (i-colorMax/2)*b+a );

		double	fColor = i;
		fColor = CnvMul*pow( fColor, 1/g );
		fColor = (fColor-colorMax/2)*b+a;
		pcnvTable[i] = MakeColor( fColor );
		//pcnvTable[i] = pow( (i-colorMax/2)*b+a, g);
		//pcnvTable[i] = MakeColor( CnvMul*pow( (i-colorMax/2)*b+a, g));
	}


	CImagePaneCache paneCache( imageNew );


	for( long y = 0; y < cy; y++ )
	{
		byte	*pbyteMask = image.GetRowMask( y );

		for( long c = 0; c  < colors; c++ )
		{
			if( !paneCache.IsPaneEnable( c ) ) 
				continue;

			color *psrc = image.GetRow( y, c );
			color *pdst = imageNew.GetRow( y, c );
			for( long x = 0; x < cx; x++ )
				if( pbyteMask[x] )
					pdst[x] = pcnvTable[psrc[x]];

		}
		Notify( y );
	}

	FinishNotification();
	}
	catch( CException *pe )
	{
		pe->Delete();
	}

	delete pcnvTable;

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionEmboss implementation
CActionEmboss::CActionEmboss()
{
}

CActionEmboss::~CActionEmboss()
{
}

bool CActionEmboss::InvokeFilter()
{
	CImageWrp	image( GetDataArgument() );
	CImageWrp	imageNew( GetDataResult() );

	long	cx = image.GetWidth(), cy = image.GetHeight();

	ICompatibleObjectPtr ptrCO( imageNew );
	ptrCO->CreateCompatibleObject( image, NULL, 0 );

	//imageNew.CreateCompatibleImage( image );

	long	colors = image.GetColorsCount();

	StartNotification( cy );

	CImagePaneCache paneCache( imageNew );


	for( long y = 0; y < cy; y++ )
	{
		byte	*pbyteMask = image.GetRowMask( y );

		for( long c = 0; c  < colors; c++ )
		{

			if( !paneCache.IsPaneEnable( c ) ) 
				continue;

			color *psrc = image.GetRow( y, c );
			color *pdst = imageNew.GetRow( y, c );

			if( y == 0 )
			{
				for( long x = 0; x < cx; x++ )
					if( pbyteMask[x] )
						pdst[x] = colorMax/2;
			}		
			else
			{
				color *psrcTop = image.GetRow( y-1, c );
				for( long x = 1; x < cx; x++ )
					if( pbyteMask[x] )
						pdst[x] = MakeColor( colorMax/2+(psrcTop[x-1] - psrc[x]));
				if( pbyteMask[0] )
					pdst[0] = colorMax/2;
			}
		}
		Notify( y );
	}

	FinishNotification();

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionNegative implementation
CActionNegative::CActionNegative()
{
}

CActionNegative::~CActionNegative()
{
}

bool CActionNegative::InvokeFilter()
{
//	CTimeTest	test( true, "negative", true );

	CImageWrp	image( GetDataArgument() );
	CImageWrp	imageNew( GetDataResult() );

	long	cx = image.GetWidth(), cy = image.GetHeight();
	//imageNew.CreateCompatibleImage( image );

	ICompatibleObjectPtr ptrCO( imageNew );
	ptrCO->CreateCompatibleObject( image, NULL, 0 );


	if( cy == 0 )
		return false;

	long	colors = image.GetColorsCount();

	

	StartNotification( cy );

	byte	*pmask;
	color	*psrc;
	color	*pdst;

	CImagePaneCache paneCache( imageNew );

	
	for( long y = 0; y < cy; y++ )
	{
		for( long c = 0; c  < colors; c++ )
		{
			if( !paneCache.IsPaneEnable( c ) ) 
				continue;

			pmask = image.GetRowMask( y );
			psrc = image.GetRow( y, c );
			pdst = imageNew.GetRow( y, c );
			
			for( long x = 0; x < cx; x++, pmask++, pdst++, psrc++ )
			{
				if( *pmask )
					*pdst = ~*psrc;
			}
		}

		Notify( y );
	}

	FinishNotification();


	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionBinaryExtractPhases implementation
CActionBinaryExtractPhases::CActionBinaryExtractPhases()
{
	m_ppcolorscr = 0;
	m_ppcolordest = 0;
	m_ppHiIntervals = 0;
	m_ppLoIntervals = 0;
	m_pClassColors = 0;
	m_nClasses = 0;

	m_pbyteMark = 0;
	m_prcVImage = 0;
}

//////////////////////////////////////////////////////////////////////
CActionBinaryExtractPhases::~CActionBinaryExtractPhases()
{
	DestroyImageMaskAndRects();
	DestroyHelpersArray();
}

//////////////////////////////////////////////////////////////////////
bool CActionBinaryExtractPhases::CanDeleteArgument( CFilterArgument *pa )
{

	if( pa->m_strArgName == "Source" )
		return false;
	return CFilterBase::CanDeleteArgument( pa );	
}


//////////////////////////////////////////////////////////////////////
void CActionBinaryExtractPhases::DestroyImageMaskAndRects()
{
	if( m_pbyteMark )	
	{
		delete m_pbyteMark;
		m_pbyteMark = 0;
	}

	if( m_prcVImage )	
	{
		delete m_prcVImage;
		m_prcVImage = 0;
	}

}

//////////////////////////////////////////////////////////////////////
void CActionBinaryExtractPhases::DestroyHelpersArray()
{
	if(m_ppcolordest)
	{
		delete m_ppcolordest;
		m_ppcolordest = 0;
	}
	if(m_ppcolorscr)
	{
		delete m_ppcolorscr;
		m_ppcolorscr = 0;
	}

	for(long i = 0; i < m_nClasses; i++)
	{
		if(m_ppHiIntervals[i]) {delete m_ppHiIntervals[i]; m_ppHiIntervals[i] = 0; }
		if(m_ppLoIntervals[i]) {delete m_ppLoIntervals[i]; m_ppLoIntervals[i] = 0; }
		if(m_pClassColors[i])  {delete m_pClassColors[i];  m_ppLoIntervals[i] = 0; }
	}
	if(m_ppHiIntervals)
	{
		delete m_ppHiIntervals;
		m_ppHiIntervals = 0;
	}
	if(m_ppLoIntervals)
	{
		delete m_ppLoIntervals;
		m_ppLoIntervals = 0;
	}
	if(m_pClassColors)
	{
		delete m_pClassColors;
		m_pClassColors = 0;
	}
	
}



//////////////////////////////////////////////////////////////////////
void CActionBinaryExtractPhases::DoFinalizePreview()
{
	/*
	CImageWrp image( GetDataArgument() );
	if( image == NULL )
		return;
	int cy = image.GetHeight();
	*/

	//StartNotification( cy, 1 );
	//CreateSelection();	
	//FinishNotification();
}

//////////////////////////////////////////////////////////////////////
bool CActionBinaryExtractPhases::InvokeFilter()
{
	bool bRes = false;

	if( !CreateImageMask() )
		return false;

	CImageWrp image( GetDataArgument() );
	if( image == NULL )
		return false;
	int cy = image.GetHeight();

	
	StartNotification( cy, 1 );

	bRes = ProcessImage( );

	FinishNotification();

	return bRes;
}

//////////////////////////////////////////////////////////////////////
IUnknown* CActionBinaryExtractPhases::GetSource()
{
	IUnknown* punkImage = GetDataArgument();
	if( !punkImage )
		return NULL;

	INamedDataObject2Ptr ptrNDO2( punkImage );
	if( ptrNDO2 )
	{
		IUnknown* punkParent = NULL;
		ptrNDO2->GetParent( &punkParent );
		if( punkParent )
			return punkParent;
	}
	punkImage->AddRef();

	return punkImage;	
}

//////////////////////////////////////////////////////////////////////
bool CActionBinaryExtractPhases::CreateImageMask()
{
	DestroyImageMaskAndRects();
	DestroyHelpersArray();

	const char* szSectionName = "ExtractPhases";

	IUnknown* punkImage = NULL;
	punkImage = GetSource();

	if( punkImage == NULL )
		return false;
	
	CImageWrp	image( punkImage );	
	punkImage->Release();

	if( image == NULL )
		return false;

	long	cx = image.GetWidth(), cy = image.GetHeight();

	CString strEntry;

	m_nClasses = ::GetValueInt(GetAppUnknown(), szSectionName, "ClassCount", 0);
	if(m_nClasses <= 0) return true;

	m_ppHiIntervals = new DWORD*[m_nClasses];
	m_ppLoIntervals = new DWORD*[m_nClasses];
	m_pClassColors = new DWORD*[m_nClasses];

	int	colors = image.GetColorsCount();

	IUnknown *punkCC;
	image->GetColorConvertor( &punkCC );
	sptrIColorConvertor5 sptrCC(punkCC);
	if(punkCC)
		punkCC->Release();

	
	
	for(long i = 0; i < m_nClasses; i++)
	{
		strEntry.Format("Class%dColor", i);
		long nClassColor = ::GetValueInt(GetAppUnknown(), szSectionName, strEntry, RGB(0,0,0));
		if(colors > 0)
		{
			color** ppColors = new color*[colors];
			byte b[3];
			b[0] = (byte)GetBValue(nClassColor); 
			b[1] = (byte)GetGValue(nClassColor);
			b[2] = (byte)GetRValue(nClassColor);
			for (int j = 0; j < colors; j++)
			{
				ppColors[j] = new color[1];
			}
			sptrCC->ConvertDIBToImageEx( b, ppColors, 1, TRUE, colors ); 

			m_ppHiIntervals[i] = new DWORD[colors];
			m_ppLoIntervals[i] = new DWORD[colors];
			m_pClassColors[i]  = new DWORD[colors];
			for(j = 0; j < colors; j++)
			{
				strEntry.Format("Class%dPane%d_Hi", i, j);
				m_ppHiIntervals[i][j] = ::GetValueInt(GetAppUnknown(), szSectionName, strEntry, 0);
				strEntry.Format("Class%dPane%d_Lo", i, j);
				m_ppLoIntervals[i][j] = ::GetValueInt(GetAppUnknown(), szSectionName, strEntry, 0);
				m_pClassColors[i][j] = *ppColors[j];
				delete ppColors[j];
			}
			delete ppColors;
		}
		else
		{
			m_ppHiIntervals[i] = 0;
			m_ppLoIntervals[i] = 0;
		}
	}


	m_pbyteMark = new byte[cx*cy*m_nClasses];
	::ZeroMemory( m_pbyteMark, cx*cy*m_nClasses );
	m_prcVImage = new CRect[ m_nClasses ];

	for( i=0;i<m_nClasses;i++ )
		m_prcVImage[i] = NORECT;


	m_ppcolorscr = new color*[colors];	
	for( long y = 0; y < cy; y++ )
	{
		for( long c = 0; c  < colors; c++ )
		{
			m_ppcolorscr[c] = image.GetRow( y, c );			
		}
		for(long x = 0; x < cx; x++)
		{			
			for(long i = m_nClasses-1; i >= 0; i--)
			{				
				bool bInRange = true;
				for(int j = 0; j < colors; j++)
				{
					bInRange = bInRange &&(*(m_ppcolorscr[j]+x) >= m_ppLoIntervals[i][j] && *(m_ppcolorscr[j]+x) < m_ppHiIntervals[i][j]);
					if(!bInRange) break;
				}

				if(bInRange)
				{					
					if( m_prcVImage[i] == NORECT )
					{						
						m_prcVImage[i].left = m_prcVImage[i].right = x;
						m_prcVImage[i].top  = m_prcVImage[i].bottom = y;
					}

					m_prcVImage[i].left		= min( m_prcVImage[i].left , x );
					m_prcVImage[i].top		= min( m_prcVImage[i].top , y );

					m_prcVImage[i].right	= max( m_prcVImage[i].right , x );					
					m_prcVImage[i].bottom	= max( m_prcVImage[i].bottom , y );

					m_pbyteMark[ cx*cy*i + cx*y + x ] = 0xFF;
				}
			}			
		}

		//Notify( y );
	}		

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionBinaryExtractPhases::ProcessImage( )
{
	if( m_pbyteMark == NULL || m_nClasses < 1 )
		return false;

	IUnknown* punkImage = NULL;
	punkImage = GetSource();

	if( punkImage == NULL )
		return false;
	
	CImageWrp	image( punkImage );	
	punkImage->Release();

	if( image == NULL )
		return false;
	
	long	cx = image.GetWidth(), cy = image.GetHeight();

	CBinImageWrp	binimage( GetDataResult( "Phase" ) );
	
	if( binimage == NULL )
		return false;

	if( !binimage.CreateNew(cx, cy) )
		return false;

	byte* binptr = 0;

	for( int y = 0; y < cy; y++ )
	{
		binimage->GetRow(&binptr, y, FALSE);
		for( long x = 0; x < cx; x++ )
		{
			byte bMask = m_pbyteMark[ cx*y + x ];
			if( bMask == 0xFF )
			{
				binptr[x/8] |= 0x80>>( x % 8 );
			}
		}

		Notify( y );
	}


	return true;
}

