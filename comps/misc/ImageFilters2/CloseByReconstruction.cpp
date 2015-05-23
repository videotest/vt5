// CloseByReconstructionFilter.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CloseByReconstruction.h"
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
//		COpenByReconstructionFilter
//
//
/////////////////////////////////////////////////////////////////////////////
#define szMaskType "mask_type"
#define szMaskSize "mask_size"
#define szColorEpsilon "color_epsilon"

_ainfo_base::arg	CCloseByReconstructionInfo::s_pargs[] = 
{	
	{szMaskSize,		szArgumentTypeInt, "5", true, false },
	{szMaskType,		szArgumentTypeInt, "1", true, false },
	{szColorEpsilon,	szArgumentTypeInt, "0", true, false },
	{"Img",				szArgumentTypeImage, 0, true, true },
	{"Result",			szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////

bool CCloseByReconstructionFilter::InvokeFilter()
{
	long nMaskType = GetArgLong( szMaskType );
	if(nMaskType==0)
		nMaskType=1;

	long nMaskSize = GetArgLong( szMaskSize );
	nMaskSize=(int) nMaskSize/2;    // размер маски есть число пикселов от центра
	if(nMaskSize==0)
		nMaskSize=1;
	long nColorEpsilon = GetArgLong( szColorEpsilon );

	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDestImage( GetDataResult() );
	
	if( ptrSrcImage == NULL || ptrDestImage == NULL )
		return false;
	{	
		ICompatibleObjectPtr ptrCO( ptrDestImage );
		
		if( ptrCO == NULL )
			return false;

		if( S_OK != ptrCO->CreateCompatibleObject( ptrSrcImage, NULL, 0 ) )
		{			
			return false;
		}		
	}

	return CloseByReconstruction(nMaskType,nMaskSize,nColorEpsilon,ptrSrcImage,ptrDestImage);
}