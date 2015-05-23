#include "StdAfx.h"
#include "resource.h"
#include "vt4support.h"
#include "SumixDriver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class CVT4SumixDrv : public CVT4Drv
{
public:
	virtual const char *GetFunctionName() {return "SumixGrabImage";}
	virtual LPCSTR fgGetDeviceName() {return _T("Sumix");}
	virtual CDriver *CreateDriverVT5() {return new CSumixDriver;}
};

CVT4SumixDrv g_VT4SumixDrv;

std_dll_c void SumixGrabImage(CInvokeStruct *pInvokeStruct, void **pArguments, void *pResult, void **pErrInfo)
{
	CSumixDriver *pDrv = (CSumixDriver*)g_VT4SumixDrv.GetDriverVT5();//new CProgResDriver;
	DoGrabImage(pDrv, pInvokeStruct, pArguments, pResult, pErrInfo);
//	pDrv->Release();
}

CIntFuncInfo FInfo=
{0, 0, 6, 0,"SumixGrabImage","SumixGrabImage",0};


DEFINE_VT4_FUNCTIONS()

