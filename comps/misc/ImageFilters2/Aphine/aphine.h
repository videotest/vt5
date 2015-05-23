
#if !defined(AFX_APHINE_H__E46B1F61_1E45_11D5_8F84_008048FD42FE__INCLUDED_)
#define AFX_APHINE_H__E46B1F61_1E45_11D5_8F84_008048FD42FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AphineParams.h"	// Added by ClassView

#include "image5.h"
#include "misc_classes.h"

void atCalcNewSizeAndOffset(IImage2Ptr &image, CAphineParams& params,
							_size &new_size, _point &new_offset );

void atCalcNewSizeAndOffsetMasked(IImage2Ptr &image, CAphineParams& params,
								  _size &new_size, _point &new_offset );

bool atExecuteFast( IImage2Ptr &image, IImage2Ptr &imageNew,
				   CAphineParams& params,
				   bool (*pCallBack)(int, long), long lParam );

bool atExecute( IImage2Ptr &image, IImage2Ptr &imageNew,
			   CAphineParams& params,
			   bool (*pCallBack)(int, long), long lParam );


#endif // !defined(AFX_APHINE_H__E46B1F61_1E45_11D5_8F84_008048FD42FE__INCLUDED_)
