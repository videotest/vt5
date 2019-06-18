#include "stdafx.h"
#include "FindPhasesGray.h"
#include "misc_utils.h"
#include <math.h>
#include <stdio.h>

#include "nameconsts.h"
#include "ImagePane.h"
#include "binimageint.h"
#include "core5.h"
#include "alloc.h"

#define MaxColor color(-1)

/////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CFindPhasesGrayInfo::s_pargs[] = 
{
	{"CreateResult",	szArgumentTypeInt, "1", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"FindPhasesGray",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

bool CFindPhasesGray::InvokeFilter()
{
	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IBinaryImagePtr	ptrBin = GetDataResult();
			
	if( ptrSrcImage == NULL || ptrBin==NULL )
		return false;

    m_nWorkPane = GetValueInt( GetAppUnknown( ), "\\ThresholdSettings", "WorkPane", 1 ); 

	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);
	ptrBin->CreateNew(size.cx,size.cy);
	POINT point;
	ptrSrcImage->GetOffset(&point);
	ptrBin->SetOffset(point,true);

	bool bCreateResult = GetArgLong("CreateResult")!=0;

	try
	{
		StartNotification( size.cy, 2 );

		long hist[256]={0};

		// ������� �����������
		for(int y=0;y<size.cy;y++ )
		{
			byte *pmask=0;
			ptrSrcImage->GetRowMask( y ,&pmask );
			color* pSrc=0;
			ptrSrcImage->GetRow(y , m_nWorkPane, &pSrc);

			for( long x = 0; x < size.cx; x++, pmask++, pSrc++)
			{
				if( (*pmask) & 128 )
				{
					hist[*pSrc/256] ++;
				}
			}			

			Notify( y );
		}

		// ������� ������ �����������
		double s0=0, s1=0, s2=0;
		for(int i=0; i<255; i++)
		{
			s0 += hist[i];
			s1 += hist[i]*double(i);
			s2 += hist[i]*double(i)*double(i);
		}
		s1 /= max(1,s0); // s1 = ������� ���� (0..255)
		s2 /= max(1,s0);
		s2 -= s1*s1;
		s2 = sqrt(max(0,s2)); // s2 = ��� �����

		// ������� �����������
		long di = long( ceil(s2/8) ); // ������ ������ ��� �����������
		di = max(di,1);
		di = min(di,32);
		double hist2[512], hist3[512];
		for(int i=0; i<512; i++) hist2[i]=hist3[i]=0;
		for(int i=0; i<256; i++) hist2[i+128]=hist3[i+128]=hist[i];

		for(int n=0; n<2; n++)
		{
			double s=0;
			for(int i=di+1; i<512-di; i++)
			{
				s -= hist2[i-di-1];
				s += hist2[i+di];
				hist3[i] = s/(di*2+1);
			}
			for(int i=0; i<512; i++) hist2[i]=hist3[i];
		}

		// ������ �� ����������� �� ��, �� ������ ��������
		di = long( ceil(s2) ); // ������ ������ ��� �����������
		di = max(di,1);
		di = min(di,47);
		{
			double s=0;
			double h3max=1.0; // ������ 1 - ��� �����������, ������� 1
			for(int i=di+1; i<512-di; i++)
			{
				s -= hist2[i-di-1];
				s += hist2[i+di];
				hist3[i] = s/(di*2+1);
				if(hist3[i]>h3max) h3max=hist3[i];
			}
			for(int i=0; i<512; i++) hist2[i] /= sqrt(max(1.0, hist3[i]+h3max/5));
		}

		// ������ ���� �����������
		long nPeaks=0;
		int peaks[256];
		for(int i=-100; i<255+100; i++)
		{
			bool bIsMax=true;
			if(hist2[i+128]<=hist2[i+127]) bIsMax = false;
			if(hist2[i+128]<=hist2[i+129]) bIsMax = false;
			if(bIsMax)
				peaks[nPeaks++] = i;
		}

		// ������ ������� ����� ������
		int valleys[256];
		for(int j=0; j<nPeaks-1; j++)
		{
			int imin=peaks[j];
			for(int i=peaks[j]; i<=peaks[j+1]; i++)
				if(hist2[i+128]<hist2[imin+128]) imin=i;
			valleys[j] = imin;
		}

		// ������� ������ ������ �������� ���� - �� �������.
		double peak_max=0.0;
		for(int j=0; j<nPeaks; j++)
			if(hist2[peaks[j]]>peak_max) peak_max = hist2[peaks[j]];

		// ������ ������ (������� ������) ������� � ����
		double fThreshold = ::GetValueDouble( GetAppUnknown( ), "\\FindPhasesGray", "Threshold", 0.8 );
		int nMaxPhases = ::GetValueInt( GetAppUnknown( ), "\\FindPhasesGray", "MaxPhases", 10 );
		while(1)
		{
			double r_worst = 0.0; // 1.0 - ������� ��� �������
			int k_worst = -1; // ����� ����, ������� ����� �������
			for(int k=0; k<nPeaks*2-2; k++)
			{ // ������ ����� ������ (������������) ������� ����� �������� � �����
				int j1 = (k+1)/2; // ����� ����
				int j0 = k/2; // ����� �������
				double h0 = hist2[valleys[j0]+128];
				double h1 = hist2[peaks[j1]+128];
				if(valleys[j0]<0 || valleys[j0]>=256)
					h0 = peak_max; // ���� ������� �� ��������� ��������� 0-255 - �� ��� �� � ����.
				double r = (h0)/max(0.01,h1);
				if(r>r_worst)
				{
					r_worst = r;
					k_worst = k;
				}
			}
			if(k_worst==-1) break;
			if( (r_worst<=fThreshold) && (nPeaks<=nMaxPhases) ) break;
			int j1 = (k_worst+1)/2; // ����� ����
			int j0 = k_worst/2; // ����� �������
			for(int j=j1; j<nPeaks-1; j++)
				peaks[j] = peaks[j+1];
			for(int j=j0; j<nPeaks-2; j++)
				valleys[j] = valleys[j+1];
			nPeaks--;
		}

		NextNotificationStage();

		// ����� ����������� � shell.data
		DeleteEntry( GetAppUnknown(), "\\FindPhasesGray\\Result");
		SetValue( GetAppUnknown( ), "\\FindPhasesGray\\Result", "Count", long(nPeaks) );
		char sz[256];
		char sz2[256]; // �������� 32 ���� �� 3 ����� - 32*4+2 ��������, ���� ������ 256
		int i0=0, i1=0;
		for(int j=0; j<nPeaks; j++)
		{
			sprintf(sz, "Peak%u", j);
			SetValue( GetAppUnknown( ), "\\FindPhasesGray\\Result", sz, max(0,min(255,long(peaks[j]))) );
			sprintf(sz, "Color%u", j);
			sprintf(sz2, "(%u,%u,%u)", peaks[j],peaks[j],peaks[j]);
			SetValue( GetAppUnknown( ), "\\FindPhasesGray\\Result", sz, sz2 );

			i0 = i1;
			if(j<nPeaks-1)
				i1 = min(255, max(0, valleys[j]));
			else
				i1 = 255;
			double s = (hist[i0]+hist[i1])/2.0;
			for(int i=i0+1; i<i1; i++)
				s += hist[i];
			sprintf(sz, "Percent%u", j);
			SetValue( GetAppUnknown( ), "\\FindPhasesGray\\Result", sz, s/max(1,s0) );
		}
		for(int j=0; j<nPeaks-1; j++)
		{
			sprintf(sz, "Threshold%u", j);
			SetValue( GetAppUnknown( ), "\\FindPhasesGray\\Result", sz, long(valleys[j]) );
		}

		if(!bCreateResult) return false; // ��� ��� ���. ���� ��� �� ���� ��������� Binary - ������ ������, � shell.data ��� ��� ���������

		// �������� ��������������� Binary	
		for( y=0;y<size.cy;y++ )
		{
			byte *pmask=0;
			ptrSrcImage->GetRowMask( y ,&pmask );
			byte* pDst;
			ptrBin->GetRow(&pDst,y,0);
			color* pSrc=0;
			ptrSrcImage->GetRow(y , m_nWorkPane, &pSrc);

			for( long x = 0; x < size.cx; x++, pmask++, pDst++, pSrc++)
			{
				*pDst = 0;
				if( (*pmask) & 128 )
				{
					for(int j=nPeaks-2; j>=0; j--)
					{
						if(*pSrc>valleys[j]*256)
						{
                            *pDst = j+1;
							break;
						}
					}
				}
			}
			Notify( y );
		}

		FinishNotification();
		return true;
	}
	catch( ... )
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////



bool CFindPhasesGray::CanDeleteArgument( CFilterArgument *pa )
{
	return false;
}
