#include "StdAfx.h"
#include "resource.h"
#include "vt4support.h"
#include "progresdriver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class CVT4ProgressDrv : public CVT4Drv
{
public:
	virtual const char *GetFunctionName() {return "ProgResGrabImage";}
	virtual LPCSTR fgGetDeviceName() {return _T("ProgRes");}
	virtual CDriver *CreateDriverVT5() {return new CProgResDriver;}
};

CVT4ProgressDrv g_VT4ProgressDrv;

std_dll_c void ProgResGrabImage(CInvokeStruct *pInvokeStruct, void **pArguments, void *pResult, void **pErrInfo)
{
	CProgResDriver *pDrv = (CProgResDriver*)g_VT4ProgressDrv.GetDriverVT5();//new CProgResDriver;
	DoGrabImage(pDrv, pInvokeStruct, pArguments, pResult, pErrInfo);
//	pDrv->Release();
}

CIntFuncInfo FInfo=
{0, 0, 6, 0,"ProgResGrabImage","ProgResGrabImage",0};


DEFINE_VT4_FUNCTIONS()

