#include "StdAfx.h"
#include "resource.h"
#include "vt4support.h"
#include "BaumerBxDriver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class CVT4BaumerBxDrv : public CVT4Drv
{
public:
	virtual const char *GetFunctionName() {return "BaumerBxGrabImage";}
	virtual LPCSTR fgGetDeviceName() {return _T("Baumer");}
};

CVT4BaumerBxDrv g_VT4BaumerBxDrv;

std_dll_c void BaumerBxGrabImage(CInvokeStruct *pInvokeStruct, void **pArguments, void *pResult, void **pErrInfo)
{
	CBaumerBxDriver *pDrv = new CBaumerBxDriver;
	DoGrabImage(pDrv, pInvokeStruct, pArguments, pResult, pErrInfo);
	pDrv->Release();
}

CIntFuncInfo FInfo=
{0, 0, 6, 0,"BaumerBxGrabImage","BaumerBxGrabImage",0};


DEFINE_VT4_FUNCTIONS()

