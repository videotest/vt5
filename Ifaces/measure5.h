#ifndef __measure5_h__
#define __measure5_h__

#include "defs.h"
#include "Classes5.h"

#import <measure.tlb> exclude ("IUnknown", "GUID", "_GUID", "tagPOINT", "LONG_PTR") no_namespace raw_interfaces_only named_guids 

/*
interface IMeasureObjectList : public IUnknown
{
	
};
interface IManualMeasParamMgr : public IUnknown
{
	com_call CreateNewParam(DWORD dwType, long * plParamKey) = 0;
	com_call FreeParam(long lParamKey) = 0;
	com_call GetParamTypeInfo(DWORD dwType, BSTR * pbstrName, BSTR * pbstrFormat, DWORD * pdwFlag, BSTR * pbstrActionName) = 0;
	com_call SetParamTypeInfo(DWORD dwType, BSTR * pbstrName, BSTR * pbstrFormat, DWORD * pdwFlag, BSTR * pbstrActionName) = 0;
};
*/

interface IMeasureEventProvider : public IUnknown
{
	com_call FireEventBeforeMeasure(BSTR bstrParamType) = 0;
	com_call FireEventAfterMeasure(BSTR bstrParamType, double fVal) = 0;
};

interface IOpticMeasureMan : public IUnknown
{
	//com_call DoMeasOptic(long x, long y, long nBrightness, double* pfIntOptDens) = 0;
	com_call GetMeasOpticStuff(long* pnMethod, BYTE* pnAvgBack, IUnknown** ppunkBack, IUnknown** ppunkDark) = 0;
	com_call CalibrateOpticalDensity(double* pfOptDens) = 0;
};

declare_interface(IMeasureEventProvider,"CAFAED16-9E7B-4267-8FF7-F9A753BBF568");
declare_interface(IManualMeasParamMgr,  "0A0AC5CA-70D8-4af6-878B-DCB51C065104");
declare_interface(IOpticMeasureMan,		"B29F3A5F-3D32-406e-B7C4-F53FB9268107");


#endif //__measure5_h__

