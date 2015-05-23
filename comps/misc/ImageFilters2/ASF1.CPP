// ASF1.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ASF1.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//		ASF1
//
//
/////////////////////////////////////////////////////////////////////////////
#define szMaskType "mask_type"
#define szMaskSize "mask_size"
#define szColorEpsilon "color_epsilon"
#define szCount "count"

_ainfo_base::arg	CASF1Info::s_pargs[] = 
{	
	{szMaskType,		szArgumentTypeInt, "1", false, false },
	{szMaskSize,		szArgumentTypeInt, "2", false, false },
	{szColorEpsilon,	szArgumentTypeInt, "0", false, false },
	{szCount,			szArgumentTypeInt, "1", false, false },
	{"Img",				szArgumentTypeImage, 0, false, true },
	{"Result",			szArgumentTypeImage, 0, true, true },	
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////

bool CASF1Filter::InvokeFilter()
{
	long nMaskType = GetArgLong( szMaskType );
	if(nMaskType==0)
		nMaskType=1;

	long nCount = GetArgLong( szCount );
	if(nCount==0)
		nCount=1;

	long nMaskSize = GetArgLong( szMaskSize );
	nMaskSize=(int) nMaskSize/2;    // размер маски есть число пикселов от центра
	if(nMaskSize==0)
		nMaskSize=1;
	long nColorEpsilon = GetArgLong( szColorEpsilon );

	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDestImage( GetDataResult() );
	
	if( ptrSrcImage == NULL || ptrDestImage == NULL )
		return false;

	return true;//CloseByReconstruction(nMaskType,nMaskSize,nColorEpsilon,ptrSrcImage,ptrDestImage);
}