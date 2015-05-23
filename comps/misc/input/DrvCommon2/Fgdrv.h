#if !defined(__FGDrv_H__)
#define __FGDrv_H__

// Flags
#define FG_CANGETLINE	   1
#define FG_HAS16BIT		   2
#define FG_HASSETUPDLG	   4
#define FG_CANNOMODDLG	   8
#define FG_HASBRCONTR	   16
#define FG_HASAUTOBC	   32
#define FG_HASEXPOSTIME	   64
#define FG_PERMTRANSFER    128
#define FG_INTERNAL	       256
#define FG_MCI	           512
#define FG_UICONTROLLABLE  1024
#define FG_TWAIN           2048
#define FG_SUPPORTSAVI     4096

// structures
typedef struct tagFGFormatDescr {
	short cx;
	short cy;
	BOOL TrueColor;
	char szDescr[40];
} FGFormatDescr;

typedef struct tagFGFormat
{
	FGFormatDescr fd;
	char szCmd[100];
	char szCmdLine[100];
} FGFormat;

#define FGUI_AUTO        0
#define FGUI_EMBEDDED    1
#define FGUI_NONE        2

#define FGXFER_AUTO      0
#define FGXFER_NATIVE    1
#define FGXFER_MEMORY    2

class CFGDrvBase
{
public:
	virtual BOOL fgInit() = 0;
	virtual const char *GetFunctionName() = 0;
	virtual BOOL fgIsHdwPresent() = 0;
	virtual UINT fgGetHdwFlags() = 0;
	// Returns information, saved during capability negotiation.
	virtual int fgGetFormatDescr( FGFormatDescr **fd ) = 0; // returns number of formats
	virtual int fgGetFormat() = 0;
	virtual void fgSetFormat( int iNum ) = 0;
	virtual BYTE * fgGetLine( short LineNum ) = 0;
	virtual BOOL fgGetBrContrLimits( int *iBrMin, int *iBrMax, int *iCntrMin, int *iCntrMax ) = 0;
	virtual BOOL fgSetBrContr( int iBr, int iContr, BOOL bAuto ) = 0;
	virtual BOOL fgGetBrContr( int *iBr, int *iContr ) = 0;
	virtual void fgSetExposTime( int iTime ) = 0;
	virtual int fgGetExposTime() = 0;
	virtual void fgForse8bit( BOOL bForce ) = 0;
	virtual BOOL fgIsForsed8bit() = 0;
	virtual LPCSTR fgGetDeviceName() = 0;
	virtual void fgSetUIType(int nType) = 0;
	virtual void fgSetXferType(int nType) = 0;
	virtual BOOL fgIsReverseBits() = 0;

	// Only if GetAquisMech has return Permanently, else setup
	// dialog box already will be created on begin of function.
	// Data source select dialog box will be created inside fgInit.
	virtual HWND fgCreateSetupDialog( HWND hWndParent, BOOL bModal ) = 0;
	// Here both DS and DSM will be closed.
	virtual void fgExit() = 0;
	virtual void fgRelease() = 0;
	virtual LPSTR fgDrvDescrBright(BOOL bRGB, int r, int g, int b) {return NULL;}; 
};

class CFGDrvAVIBase : public CFGDrvBase
{
public:
	virtual int CaptureAVI(char *pszNameBuffer, int nBuffSize) = 0;
};

// Here DSM will be opened, default source will be selected,
// (or dialog created) and opened such as aquisition and capablity
// negotiation will be available.
extern "C" BOOL _declspec(dllexport) fgInit(HWND hWndParent,
	CFGDrvBase **, int *, int);
extern "C" void _declspec(dllexport) fgExit();

typedef BOOL (*FGINITPROC)( HWND hWndParent, CFGDrvBase **, int *, int );
typedef void (*FGEXITPROC)();




#endif