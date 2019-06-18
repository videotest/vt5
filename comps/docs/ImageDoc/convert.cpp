#include "stdafx.h"
#include "convert.h"
#include "resource.h"
#include "ConvertImageDlg.h"
#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//dyncreate

IMPLEMENT_DYNCREATE(CActionConvertImage, CCmdTargetEx);

// {438A2D93-B221-11d3-A552-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionConvertImage, "ImageDoc.ConvertImage",
0x438a2d93, 0xb221, 0x11d3, 0xa5, 0x52, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);

// {438A2D91-B221-11d3-A552-0000B493A187}
static const GUID guidConvertImage =
{ 0x438a2d91, 0xb221, 0x11d3, { 0xa5, 0x52, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };



//[ag]6. action info

ACTION_INFO_FULL(CActionConvertImage, IDS_ACTION_CONVERTIMAGE, -1, -1, guidConvertImage);

//[ag]7. targets

ACTION_TARGET(CActionConvertImage, szTargetAnydoc);

//[ag]8. arguments
ACTION_ARG_LIST(CActionConvertImage)
	ARG_STRING(_T("newColorSystem"), "")
	ARG_INT(_T("DeleteArg"), -1)
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Converted") )
END_ACTION_ARG_LIST();


//[ag]9. implementation
//////////////////////////////////////////////////////////////////////
//CActionConvertImage implementation
CActionConvertImage::CActionConvertImage()
{
}

CActionConvertImage::~CActionConvertImage()
{
}

/* void Lab2RGB(int L, int a, int b,
     unsigned char *R, unsigned char *G, unsigned char *B)
 {
   float X, Y, Z, fX, fY, fZ;
   int RR, GG, BB;

   fY = pow((L + 16.0) / 116.0, 3.0);
   if (fY < 0.008856)
       fY = L / 903.3;
   Y = fY;

   if (fY > 0.008856)
     fY = pow(fY, 1.0/3.0);
   else
     fY = 7.787 * fY + 16.0/116.0;

   fX = a / 500.0 + fY;
   if (fX > 0.206893)
       X = pow(fX, 3.0);
   else
       X = (fX - 16.0/116.0) / 7.787;

   fZ = fY - b /200.0;
   if (fZ > 0.206893)
       Z = pow(fZ, 3.0);
   else
       Z = (fZ - 16.0/116.0) / 7.787;

   X *= (0.950456 * 255);
   Y *=             255;
   Z *= (1.088754 * 255);

   RR =  (int)(3.240479*X - 1.537150*Y - 0.498535*Z + 0.5);
   GG = (int)(-0.969256*X + 1.875992*Y + 0.041556*Z + 0.5);
   BB =  (int)(0.055648*X - 0.204043*Y + 1.057311*Z + 0.5);

   *R = (unsigned char)(RR < 0 ? 0 : RR > 255 ? 255 : RR);
   *G = (unsigned char)(GG < 0 ? 0 : GG > 255 ? 255 : GG);
   *B = (unsigned char)(BB < 0 ? 0 : BB > 255 ? 255 : BB);

 }

void RGB2Lab(unsigned char R, unsigned char G, unsigned char B,
      int *L, int *a, int *b)
 {
   float X, Y, Z, fX, fY, fZ;

   X = 0.412453*R + 0.357580*G + 0.180423*B;
   Y = 0.212671*R + 0.715160*G + 0.072169*B;
   Z = 0.019334*R + 0.119193*G + 0.950227*B;

   X /= (255 * 0.950456);
   Y /=  255;
   Z /= (255 * 1.088754);

   if (Y > 0.008856)
     {
       fY = pow(Y, 1.0/3.0);
       *L = (int)(116.0*fY - 16.0 + 0.5);
     }
   else
     {
       fY = 7.787*Y + 16.0/116.0;
       *L = (int)(903.3*Y + 0.5);
    }

   if (X > 0.008856)
       fX = pow(X, 1.0/3.0);
   else
       fX = 7.787*X + 16.0/116.0;

   if (Z > 0.008856)
       fZ = pow(Z, 1.0/3.0);
   else
       fZ = 7.787*Z + 16.0/116.0;

   *a = (int)(500.0*(fX - fY) + 0.5);
   *b = (int)(200.0*(fY - fZ) + 0.5);

 }
*/

bool CActionConvertImage::CanDeleteArgument( CFilterArgument *pa )
{
	long nDeleteArg = GetArgumentInt(_T("DeleteArg"));

	if(nDeleteArg == -1)
		return CFilterBase::CanDeleteArgument( pa );

	return nDeleteArg != 0;
}

IUnknown *CActionConvertImage::GetObjectArgument( const char *szType, int nCurrentPos, IDataContext2 *punkContext )
{
	ASSERT(punkContext);
	ASSERT( nCurrentPos == 0 );

	_bstr_t	bstrType = szType;
	IUnknown	*punkDataObject = 0;
	
	IDataContext3Ptr	ptrC( punkContext );

	if( ptrC == NULL ) return NULL;

	long	lPosSelected = 0;
	ptrC->GetFirstSelectedPos( bstrType, &lPosSelected );
	if( !lPosSelected )return 0;
	ptrC->GetNextSelected( bstrType, &lPosSelected, &punkDataObject );
	if( !punkDataObject )return 0;

	INamedDataObject2Ptr	ptrN( punkDataObject );
	punkDataObject->Release();
	IUnknown	*punkParent = 0;
	ptrN->GetParent( &punkParent );

	if( punkParent )
	{
		ptrN =  punkParent;
		punkParent->Release();
	}

	return ptrN.Detach();
}


bool CActionConvertImage::InvokeFilter()
{
	
	CImageWrp	image( GetDataArgument() );
	CImageWrp	imageNew( GetDataResult() );

	INamedDataObject2Ptr	ptrN( image );
	IUnknown	*punkParent = 0;
	ptrN->GetParent( &punkParent );

	if( punkParent )
	{
		image = punkParent;
		punkParent->Release();
	}

	long	cx = image.GetWidth(), cy = image.GetHeight();

	CString strCurCCName = "";
	strCurCCName = image.GetColorConvertor();

	IUnknown* punk;
	image->GetColorConvertor(&punk);
	sptrIColorConvertor4 sptrCCScr(punk);
	punk->Release();

	DWORD dwFlags = 0;
	sptrCCScr->GetConvertorFlags(&dwFlags);

	if((dwFlags & ccfLabCompatible) != ccfLabCompatible)
	{
		AfxMessageBox(IDS_CANT_CONVERT_FROM);
		return false;
	}
	

	CString strCCName = "";
	strCCName = GetArgumentString( _T("newColorSystem") );

	if(!IsCCValid(strCCName))
	{
		//need determinate CC. raise dialog.
		CConvertImageDlg dlg;
		dlg.m_strCurSystem = strCurCCName;
		BSTR bstr;
		m_pActionInfo->GetTargetName( &bstr);
		CString strTargetName(bstr);
		::SysFreeString(bstr);
		dlg.m_stFileName = strTargetName;
		if (dlg.DoModal() == IDCANCEL) return true;
		strCCName = dlg.m_strCurSystem;
		
	}
	
//	if(strCCName == strCurCCName)
//		return true;
	
	imageNew.CreateCompatibleImage( image,  true, NORECT, (const char*)strCCName);

	long	colors = max(image.GetColorsCount(), imageNew.GetColorsCount());

	double*		pLab;
	pLab = new double[cx*3];
	
	StartNotification( cy );
	imageNew->GetColorConvertor(&punk);
	sptrIColorConvertor3 sptrCCDest(punk);
	punk->Release();

	color	**ppcolorscr = new color*[colors];
	color	**ppcolordest = new color*[colors];
	for( long y = 0; y < cy; y++ )
	{
		for( long c = 0; c  < colors; c++ )
		{
			ppcolorscr[c] = image.GetRow( y, c );
			ppcolordest[c] = imageNew.GetRow( y, c );
		}
		sptrCCScr->ConvertImageToLAB(ppcolorscr, pLab, cx);
		sptrCCDest->ConvertLABToImage(pLab, ppcolordest, cx);
		Notify( y );
	}

	delete pLab;
	delete ppcolorscr;
	delete ppcolordest;
	//FinishNotification();

	return true;
}

/****************************************************************************************************/
IMPLEMENT_DYNCREATE(CActionToGrayAvail,	CCmdTargetEx);

// {AB0ED46F-E413-4803-BB80-7E6CAC65E0B9}
GUARD_IMPLEMENT_OLECREATE(CActionToGrayAvail, "ImageDoc.ToGrayAvail", 
0xab0ed46f, 0xe413, 0x4803, 0xbb, 0x80, 0x7e, 0x6c, 0xac, 0x65, 0xe0, 0xb9);

// Action info
// {F7A3577B-E407-437b-9726-B51E07E8A641}
static const GUID guidToGrayAvail = 
{ 0xf7a3577b, 0xe407, 0x437b, { 0x97, 0x26, 0xb5, 0x1e, 0x7, 0xe8, 0xa6, 0x41 } };


ACTION_INFO_FULL(CActionToGrayAvail, IDS_ACTION_TO_GRAY_AVAIL, -1, -1, guidToGrayAvail );
ACTION_TARGET(CActionToGrayAvail, szTargetViewSite );

CActionToGrayAvail::CActionToGrayAvail()
{
	
}

CActionToGrayAvail::~CActionToGrayAvail()
{

}

bool CActionToGrayAvail::Invoke()
{
	return false;
}

bool CActionToGrayAvail::IsAvaible()
{
	IUnknown *ptrDocImage = 0;
	ptrDocImage = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );

	if( ptrDocImage )
	{
		IImage3Ptr ptrImage = ptrDocImage;
		ptrDocImage->Release();

		if( ptrImage == 0 )
			return false;

		IUnknown	*punkCC1 = 0;
		ptrImage->GetColorConvertor( &punkCC1 );

		if( punkCC1 == 0 )
			return false;

		IColorConvertor2Ptr sptrCC1 = punkCC1;
		punkCC1->Release();

		if( sptrCC1 == 0 )
			return false;

		BSTR bstr;
		sptrCC1->GetCnvName( &bstr );
		_bstr_t bstrN1(bstr);
		::SysFreeString(bstr);
		
		if( bstrN1 == _bstr_t("GRAY") )
			return false;
		else
			return true;
	}
	return false;
}
/****************************************************************************************************/

