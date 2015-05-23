#include "StdAfx.h"
#include "Cromos.h"
#include "ChromParam.h"
#include "ChildObjects.h"
#include "PUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


void CalcAxisIntAlg(CImageWrp &pImgWrp, CChromParam* pParam)
{
	ASSERT(pParam->m_ShortAxis.m_Type == IntegerAxis);
	ASSERT(pParam->m_ShortAxis.m_nScale == 8);
	POINT *ptShAx = pParam->m_ShortAxis.m_iptAxis;
	int nAxisNodes = pParam->m_ShortAxis.m_nSize;
	int i;
	int nSum = 31;
	for (i = 1; i < nAxisNodes; i++)
	{
		int x = ptShAx[i].x-ptShAx[i-1].x;
		int y = ptShAx[i].y-ptShAx[i-1].y;
		nSum += abs(x)+abs(y);
	}
	POINT *ptLgAx = new POINT[nSum/8+7];
	int nPoints;
	if (nAxisNodes == 1)
	{
		ptLgAx[0] = ptShAx[0];
		nPoints = 1;
	}
	else
	{
		int done = 0;
		int input = 0;
		int input2 = 1;
		int output = 0;
   		while (1)
		{
   			int xdist = ptShAx[input2].x - ptShAx[input].x;
   			int ydist = ptShAx[input2].y - ptShAx[input].y;
   			int length = hypot8(xdist,ydist);
			if ((length >= done) && (length > 0))
			{
	   			do
				{
   					// values calculated are *8 to compensate for hypot8 and +32 
   					// to round the value up 
					ASSERT(output < nSum/8);
   					ptLgAx[output].x = ptShAx[input].x + ((xdist*done + 32)/length);
	   				ptLgAx[output].y = ptShAx[input].y + ((ydist*done + 32)/length);
   					output++;
   					done += 64;
	   			} while (done < length);
	   			int xleft = ptShAx[input2].x - ptLgAx[output-1].x;
   				int yleft = ptShAx[input2].y - ptLgAx[output-1].y;
   				int left = hypot8(xleft, yleft);
	   			done = 64 - left;
	   			input = input2;
	   		}
   			if (input2 >= nAxisNodes - 1)
			{
				ASSERT(output < nSum/8);
   				ptLgAx[output].x = ptShAx[input2].x;
   				ptLgAx[output].y = ptShAx[input2].y;
   				nPoints = output + 1;
				break;
   			}
   			input2++;
   		}
	}
	pParam->m_LongAxis.Init(IntegerAxis, nPoints, ptLgAx);
	pParam->m_LongAxis.m_nScale = 8;
	delete ptLgAx;
}


bool CChromParam::CalcLongAxis()
{
	// If manual axis exists, use it
	BOOL bLongAxisManual;
	long nLongAxisSize;
	m_sptrChromos->GetAxisSize(AT_LongAxis, &nLongAxisSize, &bLongAxisManual);
	if (bLongAxisManual && nLongAxisSize)
	{
		m_LongAxis.Init(IntegerAxis, nLongAxisSize);
		m_LongAxis.m_ptOffset = pImgWrp.GetOffset();
		m_sptrChromos->GetAxis(bRotated?AT_LongAxis|AT_Rotated:AT_LongAxis, m_LongAxis.m_iptAxis);
		m_LongAxis.Offset(pImgWrp.GetOffset()); // Will be needed for profile calculation
		if (bRotated) m_LongAxis.Rotate(CFPoint(m_ptCenter), dAngle);
		m_LongAxis.Scale(8);
		// Short axis will be used still sometimes during centromere calculation (VT4 algorythm).
		// Make short axis from long if it was not alse set manually.
		BOOL bShortAxisManual;
		long nShortAxisSize;
//		m_sptrChromos->GetAxisSize(bRotated?AT_ShortAxis|AT_Rotated:AT_ShortAxis, &nShortAxisSize, &bShortAxisManual);
		m_sptrChromos->GetAxisSize(AT_ShortAxis, &nShortAxisSize, &bShortAxisManual);
		if (bShortAxisManual && nShortAxisSize)
		{
			m_ShortAxis.Init(IntegerAxis, nShortAxisSize);
			m_sptrChromos->GetAxis(bRotated?AT_ShortAxis|AT_Rotated:AT_ShortAxis, m_ShortAxis.m_iptAxis);
			m_ShortAxis.Offset(pImgWrp.GetOffset()); // Will be needed for profile calculation
			if (bRotated) m_ShortAxis.Rotate(CFPoint(m_ptCenter), dAngle);
			m_ShortAxis.Scale(8);
		}
		else
			m_ShortAxis.Init(IntegerAxis, m_LongAxis.m_nSize, m_LongAxis.m_iptAxis);
	}
	else
	{
		// If there are no manual long axis, calculate it automatically from short axis
		if (!CalcShortAxis()) // obtain short axis
			return false;
		CalcAxisIntAlg(pImgWrp, this); // calculate long axis from short it
		if (bOk) bOk = wo->Compare(COMPARE_LONGAXIS, 1, (CHROMOAXIS *)&m_LongAxis)==S_OK;
	}
	return true;
}


