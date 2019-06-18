#include "StdAfx.h"
#include "nameconsts.h"
#include "measurechord.h"
#include "aam_utils.h"
#include "docview5.h"
#include "data5.h"
#include "misc_utils.h"
#include "alloc.h"
#include "measure5.h"
#include "binimageint.h"
#include "\vt5\common2\misc_calibr.h"
#include <math.h>
#include "\vt5\awin\misc_map.h"
#include "\vt5\awin\profiler.h"


_ainfo_base::arg CMeasureChordInfo::s_pargs[] =
{
	{"Image",	szArgumentTypeString, 0, true, true },
	{"PaneNum",	szArgumentTypeInt, "0", true, false }, // по которой пане работать
	{"UseBin",	szArgumentTypeInt, "-1", true, false }, // 0 - Grayscale/Color, 1 - Binary, -1 - взять, какой дают (предпочитая Image)
	{"FromAngle",	szArgumentTypeInt, "0", true, false }, // FromAngle <= угол < ToAngle
	{"ToAngle",	szArgumentTypeInt, "180", true, false }, // 
	{0, 0, 0, false, false },
};

CMeasureChord::CMeasureChord(void)
{
}

CMeasureChord::~CMeasureChord(void)
{
}

static _bstr_t GetObjectName( IUnknown *punk )
{
	INamedObject2Ptr	sptr( punk );
	if (sptr == 0)
		return _bstr_t("");

	BSTR	bstr = 0;
	if (FAILED(sptr->GetName(&bstr)) || !bstr) 
		return  _bstr_t("");

	bstr_t bstr1(bstr);
	::SysFreeString(bstr);

	return bstr1;
}

IUnknown *CMeasureChord::GetContextObject(_bstr_t cName, _bstr_t cType)
{ //Возвращает с AddRef

	IDataContext3Ptr ptrC = m_ptrTarget;
	_bstr_t	bstrType = cType;

	if( cName==_bstr_t("") )
	{
		IUnknown *punkImage = 0;
		ptrC->GetActiveObject( bstrType, &punkImage );
		//if( punkImage )
			//punkImage->Release();

		return punkImage;
	}

	long lPos = 0;
	ptrC->GetFirstObjectPos( bstrType, &lPos );
	
	while( lPos )
	{
		IUnknown *punkImage = 0;
		ptrC->GetNextObject( bstrType, &lPos, &punkImage );

		if( punkImage )
		{
			if( GetObjectName( punkImage ) == cName )
			{
				//if( punkImage )
					//punkImage->Release();
				return punkImage;
			}

			punkImage->Release();
		}
	}
	return 0;
}

#define szSection "\\MeasureChord"
#define szSectionResult (szSection "\\Result")

#define TESTCHORD(len, lensum, lencnt, histchord, lenmax) \
{ \
	if(len>=nMinChord) \
	{ \
		lencnt++; \
		lensum+=len; \
		histchord[ min(cx - 1, len) ] ++; \
		if( len > lenmax )	\
			lenmax = len;	\
	} \
	len=0; \
}

int CMeasureChord::CalcThreshold(color **srcRows, byte **maskRows, int cx, int cy)
{
	smart_alloc(hist, int, NC);
	CalcHist(srcRows, cx, cy, hist);

	//поиск пика гистограммы (фон)
	int max_peak=0;
	for(int i=0; i<NC; i++)
	{
		if(hist[i]>hist[max_peak]) max_peak=i;
	}


	//поиск второго пика
	int peak2=0;
	double h_max=0;
	for(i=0; i<NC; i++)
	{
		double h=double(hist[i])*(i-max_peak)*(i-max_peak);
		if(h>h_max)
		{
			peak2=i;
			h_max=h;
		}
	}

	SetValue(GetAppUnknown(),szSectionResult, "Background", long(max_peak*256.0/NC));
	SetValue(GetAppUnknown(),szSectionResult, "Foreground", long(peak2*256.0/NC));

	int xor_mask=(peak2>max_peak)?65535:0;
	
	int peak1=min(peak2,max_peak); 
	peak2=max(peak2,max_peak);
	
	int h_min=INT_MAX;
	int th=peak1;
	for(i=peak1; i<=peak2; i++)
	{
		int h=hist[peak1]+
			(hist[peak2]-hist[peak1])*(i-peak1)/(peak2-peak1);
		h=hist[i]-h;
		if(h<h_max)
		{
			th=i;
			h_max=h;
		}
	}

	SetValue(GetAppUnknown(),szSectionResult, "Threshold", long(th*256.0/NC));

	th = th*(65536/NC);

	return th;
}

HRESULT CMeasureChord::DoInvoke()
{
	PROFILE_TEST( "MeasureChord::DoInvoke"  );

	// [vanek] SBT:712 19.12.2003 - reset result
	SetValue(GetAppUnknown(),szSectionResult, "Threshold", (long)(0) );
	SetValue(GetAppUnknown(),szSectionResult, "Background", (long)(0) );
	SetValue(GetAppUnknown(),szSectionResult, "Foreground", (long)(0) );
	SetValue(GetAppUnknown(),szSectionResult, "Chord0Avg", (double)(0) );
	SetValue(GetAppUnknown(),szSectionResult, "Chord1Avg", (double)(0) );
	SetValue(GetAppUnknown(),szSectionResult, "Chord0FRV", "");
	SetValue(GetAppUnknown(),szSectionResult, "Chord1FRV", "");
	SetValue(GetAppUnknown(),szSectionResult, "Chord0Percent", (long)(0) );
	SetValue(GetAppUnknown(),szSectionResult, "Chord1Percent", (long)(0) );
	SetValue(GetAppUnknown(),szSectionResult, "Chord0Max", (double)(0) );
	SetValue(GetAppUnknown(),szSectionResult, "Chord1Max", (double)(0) );

	if (m_ptrTarget==0) return E_FAIL;
	IDocumentPtr doc(m_ptrTarget);
	if(doc==0) return E_FAIL;

	long nUseBin = GetArgLong("UseBin");
	_bstr_t bstrImage = GetArgString("Image");
	long lPaneNum = GetArgLong("PaneNum");
	long nFromAngle = GetArgLong("FromAngle");
	long nToAngle = GetArgLong("ToAngle");
	nToAngle = max(nFromAngle+1,nToAngle);

	IImage3Ptr ptrImage;
	IBinaryImagePtr ptrBin;

	if(nUseBin!=1) // 0 или -1 - Image или что дали
	{
		IUnknown *punk2 = GetContextObject(bstrImage, szTypeImage);
		ptrImage = punk2;
		if (punk2!=0) punk2->Release();
	}
	if(nUseBin!=0) // 1 или -1 - Binary или что дали
	{
		IUnknown *punk2 = GetContextObject(bstrImage, szTypeBinaryImage);
		ptrBin = punk2;
		if (punk2!=0) punk2->Release();
	}

	if (ptrImage==0 && ptrBin==0) return E_FAIL; // хоть кто-то из них должен быть

	bool bUseBin = (ptrImage==0);

	int cx=0, cy=0;
	POINT ptOffset={0,0};
	if(bUseBin)
	{
		ptrBin->GetSizes(&cx,&cy);
		ptrBin->GetOffset(&ptOffset);
	}
	else
	{
		ptrImage->GetSize(&cx,&cy);
		ptrImage->GetOffset(&ptOffset);
		int nPaneCount = ::GetImagePaneCount( ptrImage );
		if(nPaneCount<=0) return E_FAIL;
		lPaneNum = max(0,min(nPaneCount-1,lPaneNum));
	}
	if(cy<1 || cx<1) return E_FAIL;

	// [vanek]: 
	long lscan_points = GetValueInt(GetAppUnknown(),szSection, "ScanPoints", 100);
	double pi = 3.1415926535;
	
	// [vanek]: учет заданного шага по углу - пока решили брать случайный угол
	/*long ldelta = GetValueInt( GetAppUnknown( ), szSection, "AngleDelta", 5);
	if( !ldelta )
		return E_FAIL;

	_list_map_t<_step, long, cmp_long> lmap_angles;
	for( long langle = 0; langle < 180; langle += ldelta )
	{
		// _step - структура, содержащая поля: fcos_angle и fsin_angle
        _step step = {0};
		double fradian = langle * pi / 180.;
        step.fcos_angle = cos( fradian ); // dx
		step.fsin_angle = sin( fradian ); // dy
		lmap_angles.set( step, langle );
	}*/
	//

	int nMinChord = GetValueInt(GetAppUnknown(),szSection, "MinChord", 2);

 	smart_alloc(srcRows, color *, cy);
	smart_alloc(srcRowMasks, byte *, cy);
 	smart_alloc(binRows, byte *, cy);

	for(int y2=0; y2<cy; y2++)
	{
		if(bUseBin)
		{
			ptrBin->GetRow(&binRows[y2], y2, FALSE);
		}
		else
		{
			ptrImage->GetRow(y2, lPaneNum, &srcRows[y2]);
			ptrImage->GetRowMask(y2, &srcRowMasks[y2] );
		}
	}
	// получили "изображение" cx*cy

	int th = 32767;
	if(!bUseBin)
	{
		th = CalcThreshold(srcRows, srcRowMasks, cx, cy);
	}

	//сегментация, 1 проход
	double len0sum=0, len1sum=0;
	int len0cnt=0, len1cnt=0;

	// [vanek]
	int nhisthord_size = (int)(sqrt((double)(cx * cx + cy * cy)) + 0.5);
	smart_alloc( hist0chord, double, nhisthord_size );
	smart_alloc( hist1chord, double, nhisthord_size );
	ZeroMemory( hist0chord, nhisthord_size  * sizeof( double ) );
	ZeroMemory( hist1chord, nhisthord_size  * sizeof( double ) );

	// [vanek] 26.02.2004: подсчет max длины черных и белых хорд
	long len0max = 0, len1max = 0;

	srand( (unsigned)time( NULL ) ^ rand() );
	for( long l = 0; l < lscan_points; l++ )
	{ // случайно берем заданное число точек
		_point pt;
		pt.x = rand() % cx;
		pt.y = rand() % cy;

		//
		//long langle = rand() % 180;
		long langle = nFromAngle + rand() % (nFromAngle-nToAngle);
		
		double	fradian = langle * pi / 180.,
				fcos_angle = cos( fradian ), // dx
				fsin_angle = sin( fradian ); // dy
		//
		
		//for( long lpos_angle = lmap_angles.head( ); lpos_angle; lpos_angle = lmap_angles.next( lpos_angle ) )
		{	// идем по всем заданным углам
			//_step step = lmap_angles.get( lpos_angle );
			//long langle = lmap_angles.get_key( lpos_angle );

			double fx_begin = 0, fx_end = 0, fy_begin = 0, fy_end = 0, fb = 0., ftan_angle = 0.;
			if( langle == 90 )
			{	// исключение - вертикальная прямая
				fx_begin = fx_end = pt.x; 
				fy_begin = 0.;
				fy_end = cy - 1.;
			}
			else
			{	// y = kx + b
				ftan_angle = fsin_angle / fcos_angle;
				fb = pt.y - pt.x * ftan_angle;
				
				if( ftan_angle > 0. || ftan_angle < 0. )
				{
					fx_begin = -1. * fb / ftan_angle;
					fx_end = (cy - 1.- fb) / ftan_angle;
				}
				else
				{
					fx_begin = 0.;
					fx_end = cx - 1.;
				}

				fx_begin = min( cx - 1., max(0., fx_begin) );
				fy_begin = ftan_angle * fx_begin + fb;
				fx_end = min( cx - 1., max(0., fx_end) );
				fy_end = ftan_angle * fx_end + fb;
			}
            
			double fx = fx_begin, fy = fy_begin;
			int len0 = 0, len1 = 0;
			while( (fx_begin < fx_end ? fx <= fx_end : fx >= fx_end) &&
				   (fy_begin < fy_end ? fy <= fy_end : fy >= fy_end)  )
			{
                int nx = (int)(fx + 0.5), ny = (int)(fy + 0.5);
				if( nx < 0 || nx >= cx || ny < 0 || ny >= cy )
					break;

				if( bUseBin || (128 & srcRowMasks[ny][nx]) )
				{
					bool bWhite;
					if(bUseBin) bWhite=binRows[ny][nx]!=0;
					else bWhite=srcRows[ny][nx]>=th;
					if(bWhite)
					{
						len1++;
						TESTCHORD(len0, len0sum, len0cnt, hist0chord, len0max)
					}
					else
					{
						len0++;
						TESTCHORD(len1, len1sum, len1cnt, hist1chord, len1max)
					}
				}

				fx += fcos_angle; // dx
				fy += fsin_angle; // dy
			}
			TESTCHORD(len0, len0sum, len0cnt, hist0chord, len0max)
			TESTCHORD(len1, len1sum, len1cnt, hist1chord, len1max)
		}

	}

	// подсчет ФРВ
    for( int n = 0; n < nhisthord_size ; n ++ )
	{

		if( len0cnt > 0 )	
			hist0chord[ n ] /= len0cnt;
		else
			hist0chord[ n ] = 0;

		if( len1cnt > 0 )	
			hist1chord[ n ] /= len1cnt;
		else
			hist1chord[ n ] = 0;

		if( n )
		{
			hist0chord[ n ] += hist0chord[ n - 1 ];
			hist1chord[ n ] += hist1chord[ n - 1 ];
		}
	}

    double	fCalibr = 0.0025;
	GUID guidC;
	::GetDefaultCalibration( &fCalibr, &guidC );

	ICalibrPtr	ptrCalibr = bUseBin ? ptrBin : ptrImage;
	if( ptrCalibr != 0 )ptrCalibr->GetCalibration( &fCalibr, &guidC );

	SetValue(GetAppUnknown(),szSectionResult, "Chord0Avg", len0sum/max(len0cnt,1)*fCalibr);
	SetValue(GetAppUnknown(),szSectionResult, "Chord1Avg", len1sum/max(len1cnt,1)*fCalibr);

	// [vanek]
	CString sfrv0( _T("") ), sfrv1( _T("") );
	FormingFRVString( 0.01, hist0chord, nhisthord_size , fCalibr, &sfrv0 );
	FormingFRVString( 0.01, hist1chord, nhisthord_size , fCalibr, &sfrv1 );

	SetValue(GetAppUnknown(),szSectionResult, "Chord0FRV", sfrv0);
	SetValue(GetAppUnknown(),szSectionResult, "Chord1FRV", sfrv1);

	// [vanek] 20.01.2004: подсчет процентного содержания черных и белых хорд
	double fpercent0chord = 100. * len0sum / (len0sum + len1sum);
	SetValue(GetAppUnknown(),szSectionResult, "Chord0Percent", (long)(fpercent0chord + 0.5) );
	SetValue(GetAppUnknown(),szSectionResult, "Chord1Percent", (long)(99.5 - fpercent0chord) );

	SetValue(GetAppUnknown(),szSectionResult, "Chord0Max", len0max * fCalibr );
	SetValue(GetAppUnknown(),szSectionResult, "Chord1Max", len1max * fCalibr );

    return S_OK;
}