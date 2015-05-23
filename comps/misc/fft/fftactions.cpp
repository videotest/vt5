#include "stdafx.h"
#include "fftactions.h"
#include "resource.h"
#include "fftdraw.h"

#include "cwlib\cwp.h"
#pragma comment( lib, "cwlib.lib" )

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionFourierHilight, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFourierRemove, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFourierBack, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFourier, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionPreformFFT, CCmdTargetEx);

// olecreate 

// {93D21E02-955D-461a-B605-2921C9958A32}
GUARD_IMPLEMENT_OLECREATE(CActionFourierHilight, "fft.FourierHilight",
0x93d21e02, 0x955d, 0x461a, 0xb6, 0x5, 0x29, 0x21, 0xc9, 0x95, 0x8a, 0x32);
// {3E4FCDFC-A5AC-41e5-BA0D-E98B78FDA049}
GUARD_IMPLEMENT_OLECREATE(CActionFourierRemove, "fft.FourierRemove",
0x3e4fcdfc, 0xa5ac, 0x41e5, 0xba, 0xd, 0xe9, 0x8b, 0x78, 0xfd, 0xa0, 0x49);
// {3C702301-6A5E-438c-A7F9-DF8E6B495636}
GUARD_IMPLEMENT_OLECREATE(CActionFourierBack, "fft.FourierBack",
0x3c702301, 0x6a5e, 0x438c, 0xa7, 0xf9, 0xdf, 0x8e, 0x6b, 0x49, 0x56, 0x36);
// {4048E6EC-08B2-43fd-AB8C-2C17C9115358}
GUARD_IMPLEMENT_OLECREATE(CActionFourier, "fft.Fourier",
0x4048e6ec, 0x8b2, 0x43fd, 0xab, 0x8c, 0x2c, 0x17, 0xc9, 0x11, 0x53, 0x58);
// {395D2046-BA1D-4e8b-B3B0-CE7649B02CB6}
GUARD_IMPLEMENT_OLECREATE(CActionPreformFFT, "fft.PreformFFT",
0x395d2046, 0xba1d, 0x4e8b, 0xb3, 0xb0, 0xce, 0x76, 0x49, 0xb0, 0x2c, 0xb6);

// guidinfo 

// {5F7E3A0B-1174-4ecc-8BD1-47F699070B06}
static const GUID guidFourierHilight =
{ 0x5f7e3a0b, 0x1174, 0x4ecc, { 0x8b, 0xd1, 0x47, 0xf6, 0x99, 0x7, 0xb, 0x6 } };
// {B4E06693-A830-4897-9ECB-0CEE522B11BF}
static const GUID guidFourierRemove =
{ 0xb4e06693, 0xa830, 0x4897, { 0x9e, 0xcb, 0xc, 0xee, 0x52, 0x2b, 0x11, 0xbf } };
// {8D5F919C-3FBC-47c0-80BF-89C6E47B2C92}
static const GUID guidFourierBack =
{ 0x8d5f919c, 0x3fbc, 0x47c0, { 0x80, 0xbf, 0x89, 0xc6, 0xe4, 0x7b, 0x2c, 0x92 } };
// {FB8F4329-EE09-4d1f-8C7D-B7A04560E62C}
static const GUID guidFourier =
{ 0xfb8f4329, 0xee09, 0x4d1f, { 0x8c, 0x7d, 0xb7, 0xa0, 0x45, 0x60, 0xe6, 0x2c } };
// {B888F0C1-8165-44c7-8F05-4D41787165F7}
static const GUID guidPreformFFT =
{ 0xb888f0c1, 0x8165, 0x44c7, { 0x8f, 0x5, 0x4d, 0x41, 0x78, 0x71, 0x65, 0xf7 } };


//[ag]6. action info

ACTION_INFO_FULL(CActionFourierHilight, IDS_ACTION_FFT_HILIGHT, -1, -1, guidFourierHilight);
ACTION_INFO_FULL(CActionFourierRemove, IDS_ACTION_FFT_REMOVE, -1, -1, guidFourierRemove);
ACTION_INFO_FULL(CActionFourierBack, IDS_ACTION_FFT_BACK, -1, -1, guidFourierBack);
ACTION_INFO_FULL(CActionFourier, IDS_ACTION_FFT, -1, -1, guidFourier);
ACTION_INFO_FULL(CActionPreformFFT, IDS_ACTION_FFT_MODE, -1, -1, guidPreformFFT);

//[ag]7. targets

ACTION_TARGET(CActionFourierHilight, "anydoc");
ACTION_TARGET(CActionFourierRemove, "anydoc");
ACTION_TARGET(CActionFourierBack, "anydoc");
ACTION_TARGET(CActionFourier, "anydoc");
ACTION_TARGET(CActionPreformFFT, "anydoc");

//[ag]8. arguments

ACTION_ARG_LIST(CActionFourier)
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("FFT Image") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionFourierBack)
	ARG_IMAGE( _T("FFT") )
	RES_IMAGE( _T("RestoredFFT") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionFourierHilight)
	ARG_BOOL( _T("Inside"), 1 )
	ARG_IMAGE( _T("SelectedArea") )
	RES_IMAGE( _T("ResImage") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionFourierRemove)
	ARG_BOOL( _T("Inside"), 1 )
	ARG_IMAGE( _T("SelectedArea") )
	RES_IMAGE( _T("ResImage") )
END_ACTION_ARG_LIST();

//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionFourierHilight implementation
CActionFourierHilight::CActionFourierHilight()
{
}

CActionFourierHilight::~CActionFourierHilight()
{
}

/*bool CActionFourierHilight::Invoke()
{
	bool	bInside = GetArgumentInt( "Inside" ) != 0;
	CImageWrp	imageMask( ::GetActiveObjectFromDocument( m_punkTarget, szTypeImage ), false );

	if( imageMask == 0 )
		return false;

	CImageWrp	imageNew;
	CImageWrp	imageNewMask;

	IUnknown	*punkParent = 0;
	{
		INamedDataObject2Ptr	ptrN( imageMask );
		ptrN->GetParent( &punkParent );
	}

	ASSERT( punkParent );
	CImageWrp	image( punkParent );
	punkParent->Release();

	imageNew.CreateCompatibleImage( image );
	imageNewMask.CreateCompatibleImage( imageMask );

	{
		INamedDataObject2Ptr	ptrN( imageNewMask );
		ptrN->SetParent( imageNew, sdfAttachParentData|apfNotifyNamedData );
	}


	int	cx = imageNew.GetWidth(), cy = imageNew.GetHeight();
	int	colors = imageNew.GetColorsCount();

	int	x, y, c;
	CRect	rect = imageMask.GetRect();

	color	*psrc, *pdest;
	byte	*pmask;
	
	for( c = 0; c < colors; c++ )
	{
		for( y = 0; y < cy; y++ )
		{
			pmask = 0;
			psrc = image.GetRow( y, c );
			pdest = imageNew.GetRow( y, c );

			if( y >= rect.top && y < rect.bottom )
				pmask = imageMask.GetRowMask( y - rect.top );

			for( x = 0; x < cx; x++, psrc++, pdest++ )
			{
				if( pmask )
				{
					if( x >= rect.left && x < rect.right )
					{
						if( *pmask )
						{
							if( bInside )
								ChangeColor( pdest, psrc );
							else
								*pdest = *psrc;
							pmask++;
							continue;
						}
						pmask++;
					}
				}
				if( bInside )
					*pdest = *psrc;
				else
					ChangeColor( pdest, psrc );
			}
		}
	}


	m_changes.SetToDocData( m_punkTarget, imageNew );
	m_changes.SetToDocData( m_punkTarget, imageNewMask );
	m_changes.RemoveFromDocData( m_punkTarget, image );
	m_changes.RemoveFromDocData( m_punkTarget, imageMask );

	return true;
}
bool CActionFourierHilight::DoUndo()
{	m_changes.DoUndo( m_punkTarget ); return true;}
bool CActionFourierHilight::DoRedo()
{	m_changes.DoRedo( m_punkTarget ); return true;}
bool CActionFourierHilight::IsAvaible()
{
	CImageWrp	image( GetActiveObjectFromDocument( m_punkTarget, szTypeImage ), false );
	if( image.IsEmpty() )return false;

	CString	str  = image.GetColorConvertor();
	return str == "Complex" && GetParentKey( image ) != INVALID_KEY;
}
*/

bool CActionFourierHilight::InvokeFilter()
{
	bool	bInside = GetArgumentInt( "Inside" ) != 0;
	CImageWrp	image( GetDataArgument() );
	CImageWrp	imageNew( GetDataResult() );

	imageNew.CreateCompatibleImage( image );

	int	cx = imageNew.GetWidth(), cy = imageNew.GetHeight();
	int	colors = imageNew.GetColorsCount();

	int	x, y, c;
	color	*psrc, *pdest;
	byte	*pmask;

	StartNotification( cy );
	
	for( y = 0; y < cy; y++ )
	{
		for( c = 0; c < colors; c++ )	
		{
			psrc = image.GetRow( y, c );
			pdest = imageNew.GetRow( y, c );
			pmask = image.GetRowMask( y );

			for( x = 0; x < cx; x++, psrc++, pdest++, pmask++ )
			{
				if( *pmask )
					ChangeColor( pdest, psrc );
					
			}
		}

		Notify( y );
	}

	FinishNotification( );

	return true;
}


void CActionFourierHilight::ChangeColor( color *pdest, color *psrc )
{
	*pdest = color( max( 0, min( 65535, (*psrc-65535/2)*1.5+65535/2 ) ) );
}


//////////////////////////////////////////////////////////////////////
//CActionFourierRemove implementation

void CActionFourierRemove::ChangeColor( color *pdest, color *psrc )
{
	*pdest = 65536/2;
}


//////////////////////////////////////////////////////////////////////
//CActionFourierBack implementation
CActionFourierBack::CActionFourierBack()
{
}

CActionFourierBack::~CActionFourierBack()
{
}

bool CActionFourierBack::InvokeFilter()
{
	CTimeTest	test;
	CImageWrp	imageF( GetDataArgument() );
	CImageWrp	image( GetDataResult() );

	INamedDataObject2Ptr	ptr2( imageF );
	IUnknown	*punkParent = 0;

	ptr2->GetParent( &punkParent );

	if( punkParent )
	{
		imageF = punkParent;
		punkParent->Release();
	}

	int	cx = imageF.GetWidth();
	int	cy = imageF.GetHeight();
	int	x, y;

	if( imageF.GetColorConvertor() != "Complex" )
	{
		SetError( IDS_NOFOURIERSOURCE );
		return false;
	}
//	double	fMax, fMin;

	int	cxn = npfa( cx );
	int	cyn = npfa( cy );

	if( cxn != cx || cyn != cy )
	{
		SetError( IDS_NOFOURIERSOURCE );
		return false;
	}

	double fMulR = ::GetValueDouble( imageF, "fft", "MulR", 1 );
	double fAddR = ::GetValueDouble( imageF, "fft", "AddR", 0 );
	double fMulI = ::GetValueDouble( imageF, "fft", "MulI", 1 );
	double fAddI = ::GetValueDouble( imageF, "fft", "AddI", 0 );
	double fMax = ::GetValueDouble( imageF, "fft", "Max", 65535 );
	double fMin = ::GetValueDouble( imageF, "fft", "Min", 0 );

	cx = GetValueInt( imageF, "fft", "cx", (long)cx );
	cy = GetValueInt( imageF, "fft", "cy", (long)cy );

	if( !image.CreateNew( cx, cy, "gray" ) )
		return false;

	StartNotification( cy );

	complex	*pcomplex = new complex[cxn*cyn+1];

	if( !pcomplex )
	{
		SetError( IDS_NOMEMORY );
		return false;
	}

	for( y = 0; y < cyn; y++ )
	{
		complex *pc =pcomplex+y*cxn;
		color	*pcolorRe = imageF.GetRow( y, 0 );
		color	*pcolorIm = imageF.GetRow( y, 1 );

		for( x = 0; x < cxn; x++, pc++, pcolorRe++, pcolorIm++ )
		{
			pc->r = ((double)/**pcolorRe**/*pcolorRe-fAddR)/fMulR;
			pc->i = ((double)/**pcolorIm**/*pcolorIm-fAddI)/fMulI;
		}
	}

	pfa2cc(1, 1, cxn, cyn, pcomplex );
	pfa2cc(1, 2, cxn, cyn, pcomplex );

	//pcomplex++;
	double	fmax = pcomplex->r, fmin = pcomplex->r;
	int	nCount = 0;

	for( y = 0; y < cyn; y++ )
	{
		complex	*pc = pcomplex+y*cxn;
		for( x = 0; x < cxn; x++, pc++ )
		{
			if( (x + y)%2 )
				pc->r = -pc->r;
			else
				pc->r = pc->r;
				

			if( nCount == 1 )
			{
				fmax = pc->r;
				fmin = pc->r;
			}
			else
			{
				fmax = max( fmax, pc->r );
				fmin = min( fmin, pc->r );
			}
			nCount++;
		}
	}

	//(fMax-fMin)65535
	double	fmul = (fMax-fMin)/(fmax-fmin);

	for( y = 0; y < cy; y++ )
	{
		complex	*pc = pcomplex+y*cxn;
		color	*pcolor = image.GetRow( y, 0 );

		for( x = 0; x < cx; x++, pc++, pcolor++ )
		{
			double	re = (pc->r-fmin)*fmul+fMin;
			*pcolor = re;
			
		}
	}

	//pcomplex--;
	delete pcomplex;

	::SetBaseObject( image, imageF, m_punkTarget );
	
	FinishNotification( );

	return true;
}

/*IUnknown *CActionFourierBack::GetObjectArgument( const char *szType, int nCurrentPos, IDataContext2 *punkContext )
{
	ASSERT(punkContext);

	int	nActiveContextObjectCount = 0;
	_bstr_t	bstrType = szType;
	IUnknown	*punkDataObject = 0;
	
	punkContext->GetObjectCount( bstrType, &nActiveContextObjectCount );

	if( nCurrentPos >= nActiveContextObjectCount )
		return 0;

	punkContext->GetObject( bstrType, nCurrentPos, &punkDataObject );

	return punkDataObject;
}*/
//////////////////////////////////////////////////////////////////////
//CActionFourier implementation
bool CChechAvailable::IsAvaible( CString strConvertor )
{
	IApplicationPtr ptrApp = ::GetAppUnknown();

	IUnknown *punkDoc = 0;

	ptrApp->GetActiveDocument( &punkDoc );

	if( !punkDoc )
		return 0;

	IDocumentSitePtr ptrDoc = punkDoc;
	punkDoc->Release();


	if( ptrDoc == 0 )
		return 0;

	IUnknown* punkView = 0;
	ptrDoc->GetActiveView( &punkView );

	if( !punkView )
		return  0;


	IUnknown *punk = ::GetActiveObjectFromContext( punkView, szTypeImage );
	punkView->Release();

	if( !punk )
		return 0;

	CImageWrp image( punk );
	punk->Release();

	if( image == 0 )
		return 0;
	

	if( image.GetColorConvertor().CompareNoCase( strConvertor ) )
		return 0;

	return true;
}


CActionFourier::CActionFourier()
{
}

CActionFourier::~CActionFourier()
{
}


bool CActionFourier::InvokeFilter()
{
	CTimeTest	test;
	CImageWrp	image( GetDataArgument() );
	
	if( image.GetColorConvertor().CompareNoCase( "Gray" ) )
	{
		SetError( IDS_NOGRAYSOURCE );
		return false;
	}

	CImageWrp	imageF( GetDataResult() );

	int	cx = image.GetWidth();
	int	cy = image.GetHeight();
	int	x, y;
//	double	fMax, fMin;

	int	cxn = npfa( cx );
	int	cyn = npfa( cy );

	if( !imageF.CreateNew( cxn, cyn, "Complex" ) )
		return false;

	StartNotification( cy );


	//float	*pimage = new float[(cx+1)*(cy+1)];
	complex	*pcomplex = new complex[cxn*cyn];
	double	fmax, fmin;
	int	nCount = 0;

	for( y = 0; y < cy; y++ )
	{
		complex *pc =pcomplex+y*cxn;
		color	*pcolor = image.GetRow( y, 0 );

		for( x = 0; x < cx; x++, pc++, pcolor++ )
		{

			if( nCount == 1 )
			{
				fmax = *pcolor;
				fmin = *pcolor;
			}
			else
			{
				fmax = max( fmax, *pcolor );
				fmin = min( fmin, *pcolor );
			}
			nCount++;

			if( (x + y)  %2 ) pc->r = -*pcolor;
			else pc->r = *pcolor;
			pc->i = 0;
		}
		pcolor = image.GetRow( y, 0 );
		for( ; x < cxn; x++, pc++, pcolor++ )
		{
			if( (x + y)  %2 ) pc->r = -*pcolor;
			else pc->r = *pcolor;
			pc->i = 0;
		}
	}
	for( ; y < cyn; y++ )
	{
		complex *pc =pcomplex+y*cxn;
		color	*pcolor = image.GetRow( y-cy, 0 );

		for( x = 0; x < cx; x++, pc++, pcolor++ )
		{
			if( (x + y)  %2 ) pc->r = -*pcolor;
			else pc->r = *pcolor;
			pc->i = 0;
		}
		pcolor = image.GetRow( y-cy, 0 );
		for( ; x < cxn; x++, pc++, pcolor++ )
		{
			if( (x + y)  %2 ) pc->r = -*pcolor;
			else pc->r = *pcolor;
				
			pc->i = 0;
		}
	}

 	pfa2cc(-1, 2, cxn, cyn, pcomplex );
	pfa2cc(-1, 1, cxn, cyn, pcomplex );
	

	double	re, im;
	double	minRe, minIm, maxRe, maxIm;
	bool	bFirst = true;

	
	for( y = 0; y < cyn; y++ )
	{
		complex	*pc = pcomplex+y*cxn;

		for( x = 0; x < cxn; x++, pc++ )
		{
			re = pc->r;
			im = pc->i;

			if( bFirst )
			{
				maxRe = minRe = re;
				maxIm = minIm = im;
				bFirst = false;
			}
			else
			{
				maxRe = max( re, maxRe );
				maxIm = max( im, maxIm );
				minRe = min( re, minRe );
				minIm = min( im, minIm );
			}
		}
	}

	double	mulRe, mulIm, addRe, addIm;

	if( maxRe == minRe )
		maxRe = minRe+1;
	if( maxIm == minIm )
		maxIm = minIm+1;

	double	fmaxR = max( -minRe, maxRe );
	double	fmaxI = max( -minIm, maxIm );

	maxRe = fmaxR;
	minRe = -fmaxR;
	maxIm = fmaxI;
	minIm = -fmaxI;

	mulRe = /*65535**/65535./(maxRe-minRe);
	addRe = -minRe*mulRe;

	mulIm = /*65535**/65535./(maxIm-minIm);
	addIm = -minIm*mulIm;

	
	for( y = 0; y < cyn; y++ )
	{
		complex	*pc = pcomplex+y*cxn;
		color	*pcolorRe = imageF.GetRow( y, 0 );
		color	*pcolorIm = imageF.GetRow( y, 1 );
		color	*pcolorN =  imageF.GetRow( y, 2 );

		for( x = 0; x < cxn; x++, pc++, pcolorRe++, pcolorIm++, pcolorN++ )
		{
			re = /*::sqrt*/( ( pc->r*mulRe+addRe) );
			im = /*::sqrt*/( ( pc->i*mulIm+addIm) );


			*pcolorRe = re;
			*pcolorIm = im;
		}
	}

	::InitAttachedData( imageF );

	::SetValue( imageF, "fft", "MulR", mulRe );
	::SetValue( imageF, "fft", "AddR", addRe );
	::SetValue( imageF, "fft", "MulI", mulIm );
	::SetValue( imageF, "fft", "AddI", addIm );
	::SetValue( imageF, "fft", "Max", fmax );
	::SetValue( imageF, "fft", "Min", fmin );
	::SetValue( imageF, "fft", "cx", (long)cx );
	::SetValue( imageF, "fft", "cy", (long)cy );

	delete pcomplex;
	
	FinishNotification( );

	return true;
}

/*void CActionFourier::Notify( int n )
{
}*/

//////////////////////////////////////////////////////////////////////
//CActionPreformFFT implementation
CActionPreformFFT::CActionPreformFFT()
{
}

CActionPreformFFT::~CActionPreformFFT()
{
}

bool CActionPreformFFT::Invoke()
{
	ICompManagerPtr	ptrMan( m_punkTarget );

	int	nCount = 0;
	ptrMan->GetCount( &nCount );

	for( int idx =0; idx < nCount; idx++ )
	{
		IUnknown	*punkComp = 0;
		ptrMan->GetComponentUnknownByIdx( idx, &punkComp );

		if( CheckInterface( punkComp, IID_IFFTDrawController ) )
		{
			ptrMan->RemoveComponent( idx );

			IFFTDrawControllerPtr	ptrFFTDraw( punkComp );
			ptrFFTDraw->DeInit();


			punkComp->Release();
			return true;
		}
		punkComp->Release();
	}


	CFFTDrawController	*pdraw = new CFFTDrawController();

	IUnknown	*punkNewDraw = pdraw->GetControllingUnknown();
	ptrMan->AddComponent( punkNewDraw, 0 );

	IFFTDrawControllerPtr	ptrFFTDraw( punkNewDraw );
	ptrFFTDraw->Init( m_punkTarget );

	punkNewDraw->Release();

	return true;
}

/*
bool CActionPreformFFT::IsAvaible()
{
	return true;
}
*/

bool CActionPreformFFT::IsChecked()
{
	ICompManagerPtr	ptrMan( m_punkTarget );
	IUnknown	*punkFFTDraw = 0;
	ptrMan->GetComponentUnknownByIID( IID_IFFTDrawController, &punkFFTDraw );

	if( punkFFTDraw )
	{
		punkFFTDraw->Release();
		return true;
	}
	else
		return false;
}


