#if !defined(__WoolzIface_H__)
#define __WoolzIface_H__

#include "Chromosome.h"

#define COMPARE_OBJECT             0
#define COMPARE_ORIGOBJECT         1
#define COMPARE_SHORTAXIS          4
#define COMPARE_SMOOTHSHORTAXIS    5
#define COMPARE_LONGAXIS           6
#define COMPARE_PROFILE0           7
#define COMPARE_PROFILE1           8
#define COMPARE_SPROFILE0          9
#define COMPARE_SPROFILE1          10
#define COMPARE_PARAMETERS         11
#define COMPARE_FEATURES           12
#define COMPARE_NFEATURES          13


struct FPOINT;

struct  INTERVAL
{
	int left;
	int right;
};

struct INTERVALROW
{
	int nints;
	INTERVAL *pints;
};

struct INTERVALIMAGE
{
	int nrows;
	INTERVALROW *prows;
	POINT ptOffset;
	SIZE  szInt;
};

struct CHROMOFEATURES
{
	int nObj;
	int nFeatsNum;
	int *pnFeats;
};

interface IChromoAxis : public IUnknown
{
	com_call SetData(double *pfSegX, double *pfSegY, int nSize, POINT ptOffset, BOOL bLong) = 0;
	com_call SetSkelFlag(int nFlag) = 0;
};

interface ICentromere : public IUnknown
{
	com_call SetLocation(POINT ptCen, POINT ptOffs) = 0;
};

interface IChromosome;
interface IWoolzObject : public IUnknown
{
	com_call GetWoolzObject(void **) = 0;
	com_call SetWoolzObject(void *) = 0;
//	com_call CalculateParameters(IUnknown *punkCO, int nParamsNum, double *pdParams,
//		IChromoAxis *pAxis, ICentromere *pCentromere) = 0;
	com_call CalculateParameters(IUnknown *punkCO, int nParamsNum, double *pdParams,
		IChromosome *pChromos) = 0;
	com_call NormalizeParameters(IUnknown *punkOL) = 0;
	com_call Compare(int nData, int nSize, void *pData) = 0;
	com_call GetDebugData(void **) = 0;
	com_call FreeDebugData() = 0;
};


declare_interface(IChromoAxis, "812D1F33-C7DC-438F-8231-FF51C07CCBC6");
declare_interface(ICentromere, "0941D23E-414D-4021-9EE0-27EBB3457C13");
declare_interface(IWoolzObject, "A5760DB3-F955-4AF4-B37E-39B44C365020");


#endif