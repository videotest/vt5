#if !defined(__DllInfo_H__)
#define __DllInfo_H__


enum InputParamType
{
	iptUndefined=0,
	iptString,
	iptImage,
	iptMaskImg = iptImage,
	iptInt,
	iptSlideInt,
	iptRadio,
	iptCheck,
	iptDataList,
	iptBinImage,
	iptButton,
	iptChild,
	iptDriver,
	iptHisto
};



// DLL must export 5 functions.
// 1. DllGetFunctionsCount doesn't receipt any parameters
//    and returns 32-bit integer value - number of functions in DLL.
// 2. DllGetFunctionInfo receipt two parameters - 32-bit number
//    of function and pointer to structure CIntFuncInfo with initialized
//    first member (m_nSize). Function must initialize all other
//    members and return 0 if Ok or not 0 if any error.
// 3. DllGetFuncArgsCount receipt only parameter - 32-bit number of 
//    function and returns the number of argument of the function or
//    0xFFFFFFFF if any error.
// 4. DllGetFuncArgInfo receipt two 32-bit integer parameters - 
//    number of function and number of argument and returns the pointer
//    to structure CArgInfo or NULL if any error. This pointer never
//    will be deleted by framework and may point to static data or
//    dynamic data, freed by dll itself.
// 5. DllGetFuncResultInfo receipt one 32-bit integer parameters - 
//    number of function  and returns the pointer to structure 
//    CArgInfo or NULL if any error. This pointer never
//    will be deleted by framework and may point to static data or
//    dynamic data, freed by dll itself.
//
//    All function must treate situation when number of function or
// number of argument is invalid.
#define  DLLGETFUNCTIONSCOUNT  "DllGetFunctionsCount"
// extern "C" __declspec(dllexport) unsigned DllGetFunctionsCount();
#define  DLLGETFUNCTIONINFO    "DllGetFunctionInfo"
// extern "C" __declspec(dllexport) unsigned DllGetFunctionInfo(unsigned nFuncNo, CIntFuncInfo *pBuffer);
#define  DLLGETFUNCARGSCOUNT   "DllGetFuncArgsCount"
// extern "C" __declspec(dllexport) unsigned DllGetFuncArgsCount(unsigned nFuncNo);
#define  DLLGETFUNCARGINFO     "DllGetFuncArgInfo"
// extern "C" __declspec(dllexport) CArgInfo *DllGetFuncArgInfo(unsigned nFuncNo, unsigned nArgNo);
#define  DLLGETFUNCRESULTINFO  "DllGetFuncResultInfo"
// extern "C" __declspec(dllexport) CArgInfo *DllGetFuncResultInfo(unsigned nFuncNo);

typedef unsigned (*FUNCU)( void );
typedef unsigned (*FUNCI_UPV)( unsigned, void * );
typedef unsigned (*FUNCU_U)( unsigned );
typedef void *(*FUNCPV_UU)( unsigned, unsigned );
typedef void *(*FUNCPV_U)( unsigned );

// Structure CIntFuncInfo used in DllGetFunctionInfo
struct CIntFuncInfo
{
	unsigned m_nSize;   // size of structure - initialized by
	                    // framework, must be checked by DLL.
	// All another member must be set by DLL.
	unsigned m_nFuncNo; // Number of function, such as parameter.
	unsigned m_nFuncType;
	unsigned m_nFuncSubType;
	// Pointer members will never be freed by framework and may be
	// static.
	char *m_pszIntFuncName; // Exported name for GetProcAddress(...)
	char *m_pszExtFuncName; // User-readable name for framework.
	char *m_pszResourceName; // Name of resource ( usually bitmap ).
	                        // Currently not used.
	char *m_pszInitFuncName; // Exported name for GetProcAddress(...)
};

// This structure needed for seamless interaction between CFunction
// object and function, placed, likely, inside main program module.
// It will be passed into function as main parameter.
// Extern function must check the nSize member on the entry for
// version consistency and periodically call the NotifyFunction.
struct CInvokeStruct
{
	unsigned   nSize; // Size of structure
	long       (*NotifyFunc)(WPARAM,LPARAM); // Call this function periodically
	LPARAM     NotifyLParam; // First parameter for NotifyFunc
	WPARAM     NotifyWParam; // Second parameter for NotifyFunc, 
	                         // usually 0, may be replaced by number of
	                         // ready result part.
	int        nParams;      // Parameters number of the function.
	void       *m_pTrueFunc; // CFunction - derived object from .exe
	void       *m_pProcessor;// CProcessor - derived object.
	char       *m_pszTextParams;
};

typedef long ARG_PART;

// constructor of this class use newfor allocating arrays.
class CArgument
{
public:
	unsigned   m_nType;
	ARG_PART	*m_partArray;
	unsigned	m_nArrSize;
	HANDLE		m_hEventPartComplete;	
								//Это событие устанавливается
								//при завершении обработки одного участка
	CArgument( unsigned nArrSize );
	virtual ~CArgument();
	void SetSize( unsigned nArrSize );
	void WaitForReady( ARG_PART ap );
	void SetReady( ARG_PART ap );
};

class CDIBArgument : public CArgument
{
public:
	enum DIBArgumentOptions
	{
		Temp    =  0x00000001, // m_lpbi must be freed after 
			                   // function's termination      
		Little	=  0x00000002, // m_rcMask.right-m_rcMask.left and 
		                       // m_rcMask.bottom-m_rcMask.top are 
							   // real m_lpbi dimension 
		Undo    =  0x00000004  // will be saved in undo buffer inside
		                       // function's InitWork
	};
	CDIBArgument( BITMAPINFOHEADER *pbi, RECT *prc = NULL, BITMAPINFOHEADER *lpbiMask = NULL );
	CDIBArgument(CDIBArgument &Sample);
	int	  m_fsOpts;
	RECT  m_rcMask;
	BITMAPINFOHEADER *m_lpbi;
	BITMAPINFOHEADER *m_lpbiMask;
};

class CBINArgument : public CDIBArgument
{
public:
	CBINArgument( BITMAPINFOHEADER *pbi, RECT *prc = NULL, BITMAPINFOHEADER *lpbiMask = NULL ) :
		CDIBArgument( pbi, prc, lpbiMask )
	{
		m_nType = iptBinImage;
		m_lpbi = pbi;
		if (prc)
			memcpy(&m_rcMask,prc,sizeof(RECT));
		else
			memset(&m_rcMask,0,sizeof(RECT));
		m_lpbiMask = lpbiMask;
	}
	CBINArgument(CBINArgument &Sample) :
		CDIBArgument(Sample)
	{ m_nType = iptBinImage;}
};

// class CArgInfo stores the information about name and type of
// argument. Pointer to it will be obtained by DllGetFuncArgInfo
// or DllGetFuncResultInfo.
struct CArgInfo
{
	int Size; // size of structure
	enum InputParamType Type;
	char *Caption;
	char *TipLine;
};

struct CNumberArg
{
	CArgInfo ArgInfo;
	int Number;
	int LeftLimit;
	int RightLimit;
	int DefValue;
};

struct CButtonArg
{
	CArgInfo ArgInfo;
        int CommandId;
        SIZE Size;
        LPCSTR Bitmap;
        int IsVertical;
};

struct CRuntimeClass;
struct CChildArg
{
	CArgInfo ArgInfo;
        CRuntimeClass * RuntimeClass;
        const char * WndClassName;
        DWORD Style;
        LPARAM LParam;
};

struct CSliderArg
{
	CNumberArg NumberArg;
	int Step;
};

struct CRadioArg
{
	CArgInfo ArgInfo;
	int Choice;
	int DefChoice;
	int NumOfChoices;
	char **Captions;
};

struct CCheckArg
{
	CArgInfo ArgInfo;
	BOOL Checked;
	BOOL DefChecked;
};

struct CStringArg
{
	CArgInfo ArgInfo;
	char *Value;
	char *DefValue;
	int Length;
};

struct CImageArg
{
	CArgInfo ArgInfo;
	LPBITMAPINFOHEADER pImg;
};

struct CMaskImgArg
{
	CImageArg ImageArg;
	LPBITMAPINFOHEADER pMask;
	RECT RectMask;
};

struct CDataListArg
{
	CArgInfo ArgInfo;
	void *pData;	//pointe to CObjListArgument	srtuct
	int  nCX;  // sizes of the full image
	int  nCY;
};

class CFGDrvBase;
struct CDriverArg
{
	CArgInfo   ArgInfo;
	CFGDrvBase *pDrv;
};

struct CHistoArg
{
	CArgInfo ArgInfo;
	BOOL bValid;
	BYTE GrayHisto[256];
	BYTE RedHisto[256];
	BYTE GreenHisto[256];
	BYTE BlueHisto[256];
};

enum Severity
{
	svNone,
	svError,
	svFatal
};

struct CErrorInfo
{
	int  nSize;
	int  nErrCode;
	Severity SeverityCode;
	char *pszErrorStr;
};

// Function in DLL have three parameter :
// 1. - Pointer to structure CInvokeStruct. All members of it
//      already initialized.
// 2. - Array of pointers to arguments. All argumentes allocated
//      by framework.
// 3. - Pointer to result.
// 4. - Pointer to CErrorInfo is any error or NULL if no error
//    DLL must assume that type of structures in arguments and result
// is defined by DLL's information-retrieving functions.
typedef void (*EXTFUNC)( void *, void **, void *, void ** );

// Initialization function( called from InitWork).
typedef BOOL (*INITFUNC)( void *, void *, void **, void *, int );


#define FUNCTYPE_IO	0x02
#define FUNCSUBTYPE_IMPORT	0x00
#define FUNCSUBTYPE_EXPORT	0x01

#define FUNCTYPE_MEAS	0x03

#define DECLARE_NOTIFY_INFO()	\
	WPARAM	wParam;\
	LPARAM	lParam;\
	int		iMaxPercent = 1;\
	int		iLastPercent = 0;

#define NOTIFY_PARENT( iPercent ) \
	if( iPercent && !iPercent%iMaxPercent )	\
	{\
		wParam = MAKEWPARAM( iPercent, iLastPercent );\
		iLastPercent = iPercent;\
		lParam = pInvokeStruct->NotifyLParam;\
		if (pInvokeStruct->NotifyFunc( wParam, lParam ))\
		{\
			*pErrInfo = &errUserBreak;\
			return;\
		}\
	}

#define std_dll_c	extern "C" __declspec(dllexport) 


#endif

