#ifndef __MeasArg_h__
#define __MeasArg_h__


//contour data storage element
struct ContourData
{
//points array
	POINTS	*pPoints;
//size of array - number of points
	__int32	iCount;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Need to insert initialization.
	__int32 *pSingulPts;
	__int32 iCountSingul;
};

struct CPointEx
{
	POINT pt;
	BYTE  nFlag;
};

//contour data storage element
struct ContourDataEx
{
//points array
	CPointEx	*pPoints;
//size of array - number of points
	__int32		iCount;
};


struct CObjArgument
{
//contour info storage. 1st - external, next - internal borders
	ContourData	*pCntrData;
//count of contour information
	__int32		iCount;
//binary image (mask)
	LPBITMAPINFOHEADER	lpbmiBody;
//colour image (source)
	LPBITMAPINFOHEADER	lpbmiImg;
//frame rect. Determine place of object body in Image
	RECT	rc;
//calibration by x and y
	double	fCalibrX, fCalibrY;
//type of object (as in VideoTesT class Table), -1 = undefined
	int iObjType;
};

struct CDataCell
{
//key value
	WORD	wKey;
//double value - data
	double	fValue;
};

struct CDataTransportArg
{
//size of data transport
	__int32	lCount, lMaxCount;
//data storage array
	CDataCell	*pData;
};


struct CDataDescriptionCell
{
//key value
	WORD	wKey;
//param name string
	char	szName[128];
//units name
	int		iUnits;
};

struct CDataDescription
{
//description array
	CDataDescriptionCell	*pDataDesc;
//it's size
	__int32	m_iCount;
//used unit's
	char	**pszUnits;
};

//kind's of measure action. When application start's, it call
//with mfaGetCalcValues action to receive set of measure params in given
//plug-in. After registration in g_TypesManager framevork call 
//DLL function again to set KEY value. plug-in must store this values
//for later ussage. in measure time framework call plug-in function 
//with ExecuteCalc value. 
enum MeasFuncAction
{
	mfaGetCalcValues,
	mfaSetKeys,
	mfaExecuteCalc
};

class CObjNative;
class CObjNativeArray;
//typedef for call-back plug-in
typedef void *(*FUNCDOMEAS)( CObjNative *, CObjNativeArray * );
typedef void *(*FUNCMEAS)( MeasFuncAction, void *ptr1, void *ptr2, char **ppszCallBackName );

//mfaGetCalcValues function 
//void *func( MeasFuncAction = mfaGetCalcValues, 0, 0 );
//function must return pointer to CDataDescription struct

//mfaSetKeys function 
//void *func( MeasFuncAction = mfaSetKeys, 0, 0 );
//indicate what framework fill wKey field

//mfaExecuteCalc function 
//void *func( MeasFuncAction = mfaExecuteCalc, CObjArgument *, CDataTransportArg *);
//indicate what framework fill wKey field




#endif //__MeasArg_h__