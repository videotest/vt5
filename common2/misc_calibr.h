#ifndef __calibr_impl_h__
#define __calibr_impl_h__

#include "defs.h"

#ifdef new
#undef new
#endif //new
#include "string"

namespace MeasureUnitTable
{
	const double mmPerInch = 25.2;
};


#ifdef _COMMON_LIB
#define calibr_exp_class class export_data
#define calibr_exp_func std_dll
#else
#define calibr_exp_class class
#define calibr_exp_func
#endif

/*
calibr_exp_class Calibration 
{
public:
	Calibration( IUnknown *punkDataContext = 0 );
	void Load( IUnknown *punkDataContext );
	void Store();
public:
	double	fCalibrX, fCalibrY;
	double	fOffsetX, fOffsetY;

	std::string	strUnitName, strCalibrName;
	long	nUnitIndex;
	double	fMeterPerUnit;

	double	GetMeterPerUnit();	
	
};
*/


calibr_exp_class DisplayCalibration
{
public:
	DisplayCalibration();

public:
	long	m_nWidthPixel;
	long	m_nHeightPixel;

	long	m_nWidthMM;
	long	m_nHeightMM;


	double	GetHorzPixelPerMM();
	double	GetVertPixelPerMM();

	double	GetPixelPerMM();
	
};


#define MAX_CALIBR_NAME	100
#define DEFAULT_CALIBRATION	0.00025
calibr_exp_func bool IsCalibrationForPrintEnable();
calibr_exp_func bool GetDefaultCalibration( double *pfCalibr, GUID *pguid );
calibr_exp_func bool GetCalibration( IUnknown *punk, double *pfCalibr, GUID *pguid );
calibr_exp_func bool GetCalibrationFromTable( const GUID &guid, double *pfCalibr, char *pszName, bool *pbActive );
calibr_exp_func bool GetCalibrationUnit( double *pfUnitPerMeter, char *psz );



#endif//__calibr_impl_h__