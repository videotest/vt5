#include "stdafx.h"
#include "drops.h"
#include "alloc.h"
#include "measure5.h"
#include "core5.h"
#include "docview5.h"
#include "misc_utils.h"
#include "binimageint.h"
#include <stdio.h>
#include <math.h>


//////////////////////////////////////////////////////////////////////
// CActionFindDropAngle
//////////////////////////////////////////////////////////////////////
_ainfo_base::arg CActionFindDropAngleInfo ::s_pargs[] =
{
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////
CActionFindDropAngle::CActionFindDropAngle()
{
}

/////////////////////////////////////////////////////////////////////
CActionFindDropAngle::~CActionFindDropAngle()
{
}

//////////////////////////////////////////////////////////////////////
IUnknown* CActionFindDropAngle::DoGetInterface( const IID &iid )
{
//	if( iid == IID_ILongOperation )
//		return (ILongOperation*)this;
	
	return __super::DoGetInterface( iid );
}

/////////////////////////////////////////////////////////////////////
HRESULT	CActionFindDropAngle::GetActionState(DWORD *pdwState)
{
	HRESULT hr = S_OK;
	hr = __super::GetActionState( pdwState );
    
/*	*pdwState &= ~afEnabled;
	IUnknown *punk_objlist = 0;
	punk_objlist = GetActiveObjectFromDocument( m_ptrTarget, szTypeObjectList );
	INamedDataObject2Ptr sptr_ndo = punk_objlist;
	if( punk_objlist )
		punk_objlist->Release(); punk_objlist = 0;

	if( sptr_ndo != 0 )
	{
		LPOS lpos_chld = 0; 
        sptr_ndo->GetActiveChild( &lpos_chld );
		if( lpos_chld )
				*pdwState |= afEnabled;
	}*/

    return hr;
}

/////////////////////////////////////////////////////////////////////


struct CFindDropPoints
{
	POINT ptUpperLeft;
	POINT ptDownLeft;
	POINT ptUpperRight;
	POINT ptDownRight;
};

static bool FindPoints(IBinaryImage *pBinImage, RECT rcSearch, CFindDropPoints &fdp)
{
	bool b1stLeft = true, b1stRight = true;
	int xCenter = (rcSearch.left+rcSearch.right)/2;
	for(int y = rcSearch.top; y < rcSearch.bottom; y++)
	{
		byte *pRow;
		pBinImage->GetRow(&pRow, y, FALSE);
		if (b1stLeft) // Finding first row containing object points on binary image
		{
			bool bFound = false;
			for (int x = rcSearch.left; x < xCenter; x++)
			{
				if (pRow[x] >= 128)
				{
					fdp.ptUpperLeft = fdp.ptDownLeft = _point(x,y);
					bFound = true;
					break;
				}
			}
			if (bFound)
				b1stLeft = false;
		}
		else
		{
			dbg_assert(fdp.ptUpperLeft.x <= fdp.ptDownLeft.x, "error");
			for (int x = rcSearch.left; x < xCenter; x++)
			{
				if (pRow[x] >= 128)
				{
					if (x < fdp.ptUpperLeft.x)
						fdp.ptUpperLeft = _point(x,y);
					fdp.ptDownLeft = _point(x,y);
					break;
				}
			}
		}
		if (b1stRight) // Finding first row containing object points on binary image
		{
			bool bFound = false;
			for (int x = rcSearch.right-1; x > xCenter; x--)
			{
				if (pRow[x] >= 128)
				{
					fdp.ptUpperRight = fdp.ptDownRight = _point(x,y);
					bFound = true;
					break;
				}
			}
			if (bFound)
				b1stRight = false;
		}
		else
		{
			dbg_assert(fdp.ptUpperRight.x >= fdp.ptDownRight.x, "error");
			for (int x = rcSearch.right-1; x > xCenter; x--)
			{
				if (pRow[x] >= 128)
				{
					if (x > fdp.ptUpperRight.x)
						fdp.ptUpperRight = _point(x,y);
					fdp.ptDownRight = _point(x,y);
					break;
				}
			}
		}
	}
	return !b1stLeft && !b1stRight;
}


HRESULT	CActionFindDropAngle::DoInvoke()
{
	BOOL bDebug = ::GetValueInt(::GetAppUnknown(), "FindDropAngle", "Debug", 0);
	// 1. Find binary
	IUnknownPtr punkBin(GetActiveObjectFromDocument(m_ptrTarget, szTypeBinaryImage), false);
	IBinaryImagePtr sptrBinImage(punkBin);
	// 2. Find rectangle of interest
	bool bMask = false;
	_rect rcSearch;
	IUnknownPtr punkImage(GetActiveObjectFromDocument(m_ptrTarget, szTypeImage), false);
	INamedDataObject2Ptr sptrNDO2Image(punkImage);
	if (sptrNDO2Image != 0)
	{
		IUnknownPtr punkParent;
		sptrNDO2Image->GetParent(&punkParent);
		if (punkParent != 0)
		{
			IImage4Ptr sptrImage(punkImage);
			if (sptrImage != 0)
			{
				POINT ptOffset;
				sptrImage->GetOffset(&ptOffset);
				int cx,cy;
				sptrImage->GetSize(&cx, &cy);
				rcSearch = _rect(ptOffset, _size(cx,cy));
				bMask = true;
			}
			punkImage = punkParent;
		}
	}
	if (!bMask) // If no mask, use entire binary image
	{
		int cxBin,cyBin;
		sptrBinImage->GetSizes(&cxBin, &cyBin);
		POINT ptOffset;
		sptrBinImage->GetOffset(&ptOffset);
		rcSearch = _rect(ptOffset.x, ptOffset.y, ptOffset.x+cxBin, ptOffset.y+cyBin);
	}
	// Find alse object's name
	_bstr_t bstrName;
	INamedObject2Ptr sptrNO(punkImage);
	if (sptrNO != 0)
		sptrNO->GetName(bstrName.GetAddress());
	// 3. Find angles
	CFindDropPoints fdp;
	FindPoints(sptrBinImage, rcSearch, fdp);
	FILE *f = NULL;
	char szBuff[256],szBuff1[256];
	if (bDebug)
	{
		static bool b1 = true;
		if (b1)
			f = fopen("c:\\drops.log", "wt");
		else
			f = fopen("c:\\drops.log", "at");
		b1 = false;
		sprintf(szBuff, "(%d,%d)-(%d,%d), (%d,%d)-(%d,%d)", fdp.ptUpperLeft.x, fdp.ptUpperLeft.y,
			fdp.ptDownLeft.x, fdp.ptDownLeft.y, fdp.ptUpperRight.x, fdp.ptUpperRight.y,
			fdp.ptDownRight.x, fdp.ptDownRight.y);
//		VTMessageBox(szBuff, NULL, MB_OK);
	}
	double dxLeft = fabs(double(fdp.ptUpperLeft.x-fdp.ptDownLeft.x));
	double dyLeft = fabs(double(fdp.ptUpperLeft.y-fdp.ptDownLeft.y));
	double dLenLeft = sqrt(dxLeft*dxLeft+dyLeft*dyLeft);
	double dAngleLeft = acos(dxLeft/dLenLeft);
	double dxRight = fabs(double(fdp.ptUpperRight.x-fdp.ptDownRight.x));
	double dyRight = fabs(double(fdp.ptUpperRight.y-fdp.ptDownRight.y));
	double dLenRight = sqrt(dxRight*dxRight+dyRight*dyRight);
	double dAngleRight = acos(dxRight/dLenRight);
	double dAngleLeftD = dAngleLeft*180./PI;
	double dAngleRightD = dAngleRight*180./PI;
	double dAngleD = 180.-dAngleLeftD-dAngleRightD;
	::SetValue(::GetAppUnknown(), "FindDropAngle", "DropAngle", dAngleD);
	if (bDebug)
	{
		sprintf(szBuff1, "%g (%g, %g)", dAngleD, dAngleLeftD, dAngleRightD);
		VTMessageBox(szBuff1, NULL, MB_OK);
		IDocumentSitePtr sptrDS(m_ptrTarget);
		_bstr_t bstr;
		sptrDS->GetPathName(bstr.GetAddress());
		if (bstrName.length() > 0)
			fprintf(f, "%s,%s: %s (%s)\n", (char *)bstr, (char*)bstrName, szBuff1, szBuff);
		else
			fprintf(f, "%s: %s (%s)\n", (char *)bstr,szBuff1, szBuff);
		fclose(f);
	}
	return S_OK;
}

