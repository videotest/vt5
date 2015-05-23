#include "StdAfx.h"
#include <math.h>
#include "nameconsts.h"
#include "CompareStatSelections.h"
#include "aam_utils.h"
#include "misc_utils.h"


#define szSection "\\CompareStatSelections"
#define szSectionResult (szSection "\\Result")

_ainfo_base::arg CCompareStatSelectionsInfo::s_pargs[] =
{
	{"frv1",	szArgumentTypeString, 0, true, true },
	{"frv2",	szArgumentTypeString, 0, true, true },
	{0, 0, 0, false, false },
};

CCompareStatSelections::CCompareStatSelections(void)
{

}

CCompareStatSelections::~CCompareStatSelections(void)
{

}

HRESULT CCompareStatSelections::DoInvoke()
{
	// [vanek] SBT:712 19.12.2003 - reset result
	SetValue(GetAppUnknown(),szSectionResult, "Compare", (double)(0) );
    
	CString strFRV1 = GetArgString( "frv1" ).Detach();
	CString strFRV2 = GetArgString( "frv2" ).Detach();

	double *pfrv1 = 0, *pfrv2 = 0;
	int nsize1 = 0, nsize2 = 0,
		nfullsize1 = 0, nfullsize2 = 0;
	
	ParseFRVString( &strFRV1, 0, &nsize1 );
	ParseFRVString( &strFRV2, 0, &nsize2 );

	if( !nsize1 || !nsize2)
		return E_FAIL;

	// [vanek] SBT: 715
	// увеличиваем количество точек на 2, чтобы установить первую и последнию точки в значения (0,0) и 
	// (<большое число>,1)
	nfullsize1 = nsize1 + 4;
	nfullsize2 = nsize2 + 4;
    pfrv1 = new double[ nfullsize1 ];
	pfrv2 = new double[ nfullsize2 ];

	ParseFRVString( &strFRV1, (pfrv1 + 2), &nsize1 );
	ParseFRVString( &strFRV2, (pfrv2 + 2), &nsize2 );
	
	// устанавливаем первые точки массивов
	pfrv1[ 0 ] = 0.;  pfrv1[ 1 ] = 0.;
	pfrv2[ 0 ] = 0.;  pfrv2[ 1 ] = 0.;
	
	// устанавливаем последние точки массивов
	double fmax = max( pfrv1[nfullsize1 - 4], pfrv2[nfullsize2 - 4] );
	pfrv1[nfullsize1 - 2] = fmax * 10.; pfrv1[nfullsize1 - 1] = 1.;
	pfrv2[nfullsize2 - 2] = fmax * 10.; pfrv2[nfullsize2 - 1] = 1.;

    double fdif = compare( pfrv1, nfullsize1, pfrv2, nfullsize2 );
	if( fdif < 0 )
		fdif = 0;

    SetValue(GetAppUnknown(),szSectionResult, "Compare", fdif);    	


	if( pfrv1 )
		delete[] pfrv1; pfrv1 = 0;

	if( pfrv2 )
		delete[] pfrv2; pfrv2 = 0;



    return S_OK;
}

// если сравнение прошло корректно - возвращает степень похожести [0,1], иначе - отрицательное значение
double CCompareStatSelections::compare( double *pfrv1, int nsize_frv1, double *pfrv2, int nsize_frv2 )
{
	if( !pfrv1 || !pfrv2 || !nsize_frv1 || !nsize_frv2 )
		return -1.;
	
	double	fd = -1.;

    // нахождение fd = max| frv1 - frv2 |            	
	int nidx_x1 = 0,
		nidx_x2 = 0;
	 
	while( (nidx_x1 < nsize_frv1 - 2) && (nidx_x2 < nsize_frv2 - 2) )
	{
		double	frv1 = 0.,
				frv2 = 0.;

	
		if( pfrv1[ nidx_x1+2 ] < pfrv2[ nidx_x2+2 ] )                        
		{
			nidx_x1 += 2;
			
			if( (pfrv2[ nidx_x2 + 2 ] - pfrv2[ nidx_x2 ]) >  (pfrv2[ nsize_frv2 - 2 ] - pfrv2[ 0 ])*1e-6 )
			{
                frv1 = pfrv1[ nidx_x1 + 1 ];
				frv2 = ( pfrv1[ nidx_x1 ] - pfrv2[ nidx_x2 ] ) * 
						( pfrv2[ nidx_x2 + 3 ] - pfrv2[ nidx_x2 + 1 ] ) / 
						( pfrv2[ nidx_x2 + 2 ] - pfrv2[ nidx_x2 ] ) + pfrv2[ nidx_x2 + 1 ];
			}

		}
		else 
		{
			nidx_x2 += 2;

			if( (pfrv1[ nidx_x1 + 2 ] - pfrv1[ nidx_x1 ]) >  (pfrv1[ nsize_frv1 - 2 ] - pfrv1[ 0 ])*1e-6 )
			{
			    frv2 = pfrv2[ nidx_x2 + 1 ];
				frv1 = ( pfrv2[ nidx_x2 ] - pfrv1[ nidx_x1 ] ) * 
						( pfrv1[ nidx_x1 + 3 ] - pfrv1[ nidx_x1 + 1 ] ) /
						( pfrv1[ nidx_x1 + 2 ] - pfrv1[ nidx_x1 ] ) + pfrv1[ nidx_x1 + 1 ];
			}
		}
		
		fd = max( fd, fabs( frv1 - frv2 ) );
	}
	
	return fd < 0. ? fd : (1. - fd);
}


