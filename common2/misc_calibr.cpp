#include "stdafx.h"
#include "misc_calibr.h"
#include "data5.h"
#include "nameconsts.h"
#include "calibrint.h"
#ifdef _COMMON_LIB
#include "utils.h"
#else
#include "misc_utils.h"
#define GetObjectKey GetKey
#endif //_COMMON_LIB
#include "units_const.h"


/*
Calibration::Calibration( IUnknown *punkDataContext )
{
	strUnitName = "pixel";
	strCalibrName = "default";
	fCalibrX = fCalibrY = 1;
	fOffsetX = fOffsetY = 0;
	nUnitIndex = 0;
	fMeterPerUnit = 1;

	Load( punkDataContext );
}

void Calibration::Load( IUnknown *punkDataContext )
{
	IUnknownPtr	ptrData( GetAppUnknown() );
//load from application
	fCalibrX = ::GetValueDouble( ptrData, szCalibration, szCalibrX, fCalibrX );
	fCalibrY = ::GetValueDouble( ptrData, szCalibration, szCalibrY, fCalibrY );
	fOffsetX = ::GetValueDouble( ptrData, szCalibration, szOffsetX, fOffsetX );
	fOffsetY = ::GetValueDouble( ptrData, szCalibration, szOffsetY, fOffsetY );
	nUnitIndex = ::GetValueInt( ptrData, szCalibration, szMeasUnit, nUnitIndex );
	strUnitName = ::GetValueString( ptrData, szCalibration, szCurrentUnitName, strUnitName.c_str() );
	fMeterPerUnit = ::GetValueDouble( ptrData, szCalibration, szMeterPerUnit, fMeterPerUnit );
	strCalibrName = ::GetValueString( ptrData, szCalibration, szCurrentCalibrName, strCalibrName.c_str() );
	

	bool	bNeedReload = false;

	
	if( CheckInterface( punkDataContext, IID_INamedDataObject ) )
		if( CheckInterface( punkDataContext, IID_INamedData ) )
		{
			ptrData = punkDataContext;
			bNeedReload = true;
		}

	if( CheckInterface( punkDataContext, IID_IDataContext2 ) )
	{
		IDataContext2Ptr	ptrC( punkDataContext );
		IUnknown	*punkImage = 0;
		ptrC->GetActiveObject( _bstr_t(szTypeImage), &punkImage );
		if( punkImage )
		{
			if( CheckInterface( punkImage, IID_INamedData ) )
			{
				ptrData = punkImage;
				bNeedReload = true;
			}
			punkImage->Release();
		}
	}
	
	
	if( bNeedReload )
	{
		fCalibrX = ::GetValueDouble( ptrData, szCalibration, szCalibrX, fCalibrX );
		fCalibrY = ::GetValueDouble( ptrData, szCalibration, szCalibrY, fCalibrY );
		fOffsetX = ::GetValueDouble( ptrData, szCalibration, szOffsetX, fOffsetX );
		fOffsetY = ::GetValueDouble( ptrData, szCalibration, szOffsetY, fOffsetY );
		nUnitIndex = ::GetValueInt( ptrData, szCalibration, szMeasUnit, nUnitIndex );
		strUnitName = ::GetValueString( ptrData, szCalibration, szCurrentUnitName, strUnitName.c_str() );
		fMeterPerUnit = ::GetValueDouble( ptrData, szCalibration, szMeterPerUnit, fMeterPerUnit );
		strCalibrName = ::GetValueString( ptrData, szCalibration, szCurrentCalibrName, strCalibrName.c_str() );
	}
}

void Calibration::Store()
{
	IUnknown	*punkData = GetAppUnknown();
	::SetValue( punkData, szCalibration, szCalibrX, fCalibrX );
	::SetValue( punkData, szCalibration, szCalibrY, fCalibrY );
	::SetValue( punkData, szCalibration, szOffsetX, fOffsetX );
	::SetValue( punkData, szCalibration, szOffsetY, fOffsetY );
	::SetValue( punkData, szCalibration, szMeasUnit, nUnitIndex );
	::SetValue( punkData, szCalibration, szCurrentUnitName, strUnitName.c_str() );
	::SetValue( punkData, szCalibration, szMeterPerUnit, fMeterPerUnit );
	::SetValue( punkData, szCalibration, szCurrentCalibrName, strCalibrName.c_str() );
	
}

double	Calibration::GetMeterPerUnit()
{
	return fMeterPerUnit;
}
*/

/////////////////////////////////////////////////////////////////////////////
// DisplayCalibration class
//
DisplayCalibration::DisplayCalibration()
{
	m_nWidthPixel	= 0;
	m_nHeightPixel	= 0;

	m_nWidthMM		= 0;
	m_nHeightMM		= 0;


	HDC hdc = ::CreateDC( "DISPLAY", NULL, NULL, NULL );
	if( hdc )
	{
		m_nWidthPixel	= GetDeviceCaps( hdc, HORZRES );
		m_nWidthMM		= GetDeviceCaps( hdc, HORZSIZE );

		m_nHeightPixel	= GetDeviceCaps( hdc, VERTRES );
		m_nHeightMM		= GetDeviceCaps( hdc, VERTSIZE );

		::DeleteDC( hdc );	hdc = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
double DisplayCalibration::GetHorzPixelPerMM()
{
	if( m_nWidthMM > 0.0 )
		return double(m_nWidthPixel) / m_nWidthMM;

	return 0.0;
}

/////////////////////////////////////////////////////////////////////////////
double DisplayCalibration::GetVertPixelPerMM()
{
	if( m_nHeightMM > 0.0 )
		return double(m_nHeightPixel) / m_nHeightMM;

	return 0.0;
}

/////////////////////////////////////////////////////////////////////////////
double DisplayCalibration::GetPixelPerMM()
{
	return max( GetHorzPixelPerMM(), GetVertPixelPerMM() );
}


calibr_exp_func bool IsCalibrationForPrintEnable()
{
	long lEnable = ::GetValueInt( ::GetAppUnknown(), "Calibration", "EnableForPrinting", 1L );

	return ( lEnable == 1L );
}

const char szNewCalibrSect[] = "\\NewCalibr";
const char szNewCalibrMPP[] = "MeterPerPixel";
const char szNewCalibrName[] = "Name";
const char szNewCalibrActive[] = "Active";

calibr_exp_func bool GetCalibrationFromTable( const GUID &guid, double *pfCalibr, char *pszName, bool *pbActive )
{
	BSTR	bstrGUID;
	if( ::StringFromCLSID( guid, &bstrGUID ) != S_OK )return false;
	_bstr_t	bstrT = bstrGUID;
	::CoTaskMemFree( bstrGUID );

	char	szSection[100];
	sprintf( szSection, "%s\\%s", szNewCalibrSect, (const char*)bstrT );

	if( pfCalibr )*pfCalibr = ::GetValueDouble( GetAppUnknown(), szSection, szNewCalibrMPP, *pfCalibr );
	if( pszName )strncpy( pszName, ::GetValueString( GetAppUnknown(), szSection, szNewCalibrName, pszName ), MAX_CALIBR_NAME );
	if( pbActive ) *pbActive = ::GetValueInt( GetAppUnknown(), szSection, szNewCalibrActive, *pbActive ) != 0;

	return true;
}

calibr_exp_func bool GetDefaultCalibration( double *pfCalibr, GUID *pguid )
{
	_bstr_t	bstrCLSID = ::GetValueString( GetAppUnknown(), szNewCalibrSect, "Active", "{55D2F44D-1C07-4f7f-A54E-7426272A247C}" );

	GUID	guid;
	if( ::CLSIDFromString( bstrCLSID, &guid ) != S_OK )return false;

	char	sz[MAX_CALIBR_NAME] = "Default Calibration";
	double	fCalibr = 0.0025;
	bool	bActive = 1;
	if( !GetCalibrationFromTable( guid, &fCalibr, sz, &bActive ) )
		return false;
	if( pfCalibr )*pfCalibr = fCalibr;
	if( pguid )*pguid = guid;

	return true;


}

calibr_exp_func bool GetCalibration( IUnknown *punk, double *pfCalibr, GUID *pguid )
{
	IUnknown	*punkT = punk;
	bool	bOneFound = false;

	GUID	guid;
	double	fCalibr = 1;
	if( pfCalibr )fCalibr = *pfCalibr;

	if( punkT )punkT->AddRef();

	while( punkT ) 
	{
		ICalibrPtr	ptrC( punkT );
		if( ptrC != 0 )
		{
			GUID	g;
			double	f;
			ptrC->GetCalibration( &f, &g );

			if( g != GUID_NULL )
			{
				guid = g;
				fCalibr = f;
				bOneFound |= true;
			}
		}
		INamedDataObject2Ptr	ptrN( punkT );
		if( ptrN != 0 )
		{
			punkT->Release();
			punkT = 0;

			ptrN->GetParent( &punkT );

			if( !punkT )	//parent absent, try to load from base opbject
			{

				IUnknown	*punkData;
				ptrN->GetData( &punkData );
				INamedDataPtr	ptrData( punkData );
				if( punkData )punkData->Release();

				if( ptrData )
				{
					GUID	guidBase;
					ptrN->GetBaseKey( &guidBase );
					ptrData->GetBaseGroupBaseObject( &guidBase, &punkT );

					if( GetObjectKey( punkT ) == GetObjectKey( ptrN ) )
					{
						punkT->Release();
						punkT = 0;
					}
				}
			}
		}
	}

	if( bOneFound )
	{
		if( pfCalibr )*pfCalibr = fCalibr;
		if( pguid )*pguid = guid;
		return true;
	}
	else
		return GetDefaultCalibration( pfCalibr, pguid );
}

calibr_exp_func bool GetCalibrationUnit( double *pfCalibrPerUnit, char *psz )
{
	if( pfCalibrPerUnit )
		*pfCalibrPerUnit = ::GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
	if( psz )
	{
		strncpy( psz, GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits ), MAX_CALIBR_NAME );
		psz[MAX_CALIBR_NAME-1]=0;
	}

	return true;
}
