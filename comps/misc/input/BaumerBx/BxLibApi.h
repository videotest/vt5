/*****************************************************************************\
*    Program:             BxLib2M                                             *
*    Version:                                                                 *
*    Module:              BxLibApi.h                                          *
*    Compiler:            MS Visual C++ 6.0                                   *
*    Operating System:    Windows NT/2000/XP & Windows 98                     *
*    Purpose:                                                                 *
*                                                                             *
*    Notes:                                                                   *
*    Created:             23.01.2002                                          *
*    Last modified:       24.05.2002                                          *
*    Author:              M. Pester                                           *
*    Copyright:           © Baumer Optronic GmbH, Radeberg                    *
\*****************************************************************************/
// mp 24.05.2002 BX_GetBmpDirect  add allocMemSize - Parameter 

#ifndef _VIDEO_API_
#define _VIDEO_API_

// Microsoft specific defines
#ifndef _WINDEF_
//#include <windef.h>     // Basic Windows Type Definitions
    typedef unsigned long       DWORD;
    typedef int                 int;
    typedef unsigned char       BYTE;
    typedef unsigned short      WORD;
    typedef void                *PVOID;
#endif

// Calling Conventions 
#if defined(__BORLANDC__) || (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
    #ifndef _WINDEF_
        #define WINAPI __stdcall
    #endif
#else
    #define WINAPI
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */

#include "camera.h"
#include "BoShading.h"

#ifndef _BOCAMDEFS_	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define _BOCAMDEFS_

enum eCAMERASNAPSHOTFORMAT {
	CF_NOTDEF,				// no defined format
	CF_STANDARD,			// for camera without a special readout mode
	// 
	CF_C32PREVIEW,
	CF_C32FULLFRAME,
	CF_C32FOKUS1,
	CF_C32FOKUS2,
    CF_C32PREVIEW1,
	// 

	CF_C282PROGRESSIVE,
	CF_C282CENTERVGA,
	CF_C282CENTERSXGA,
	CF_C282FLUORRED,
	CF_C282FLUORGREEN,
	CF_C282FLUORBLUE,
	CF_C282FULLFRAME,

	CF_C32FRAMEA,
	CF_C32FRAMEB,
	
	BMPF_08BIT = 0x00010000,	//  8 Bit per pixelcanal is required
	BMPF_16BIT = 0x00020000,	// 16 Bit per pixelcanal is required, 
								// in dependence of the type of camera, 
								// 10 or 12-bit digitized sensor data are written 
								// left justified into the destination bit-map 
};

enum eSCMDID {
	SC_SPEEDUNDEFINED		= 0,	// -	option not defined
	SC_SPEEDSLOW			= 1,
	SC_SPEEDFAST			= 2,
};

// supported modes
#define SM_PARTIALSCAN		TRUE	// mode supports partialscan
#define SM_PROGESSSCAN		TRUE	// mode supports prog.scan mode( or similar mode )
#define	SM_DATAWIDTH8BIT    TRUE	// mode supports 8 Bit per pixelcanal
#define	SM_DATAWIDTH16BIT   TRUE	// mode supports 16Bit per pixelcanal
#define	SM_MONOCHROM        TRUE	// mode supports monochrom images
#define	SM_COLOR            TRUE	// mode supports color images

typedef struct {
	int time2set;	// time to set
	int timeNear;	// time that can be realized
	int timePrev;	// time that can be realized before
	int timeNext;	// time that can be realized next
	int timeMin;	// minimal possible time
	int timeMax;	// maximal possible time
} tSHT, * ptSHT; 

typedef struct {
	double gain2set;// gain to set
	double gainNear;// gain that can be realized
	double gainPrev;// gain that can be realized before
	double gainNext;// gain that can be realized next
	double gainMin;	// minimal possible gain
	double gainMax;	// maximal possible gain
} tGAIN, * ptGAIN; 

typedef struct {
	int  offset2set;// offset to set
	int  offsetNear;// offset that can be realized
	int	 offsetMin;	// minimal possible offset
	int  offsetMax;	// maximal possible offset
} tBLOFFSET, * ptBLOFFSET; 

typedef struct _BXSFORMAT_  {
	int     iSizeof;			// sizeof this struct
	int		dmaImFrameXMax;		// max. horiz. data frame witdth in pixel
	int		dmaImFrameYMax;		// max. vert.  data frame height in pixel
	int		dmaImFrameX;		// current horiz. data frame witdth in pixel
	int		dmaImFrameY;		// current vert.  data frame height in pixel
	int		dmaImOffX;			// current horiz. offset in pixel 
	int		dmaImOffY;			// current vert.  offset in pixel 
	int		dmaImSizeX;			// current hori. effective witdth in pixel
	int		dmaImSizeY;			// current vert. effective height in pixel
	int		dmaImBpp;			// current number of bytes per pixel
	int	dmaFrametransfer;		// flag for frametransfer frame
} tFormat, * ptFormat;

// collected commands for camera commands
enum eBXCOLLECTCMDID {

	CC_OPTIONNOTDEFINED		= 0,	// -	option not defined
	CC_SETSHUTTERTIME		= 1,	// -	set of shuttertime	
	CC_SETGAINFACTOR		= 2,	// -	set of global gain
	CC_SETBLACKOFFSET		= 3,	// -	set og global blackleveloffset
	CC_SETPARTIALSCAN 		= 4,	// -	set of partialscan parameters
	CC_SETBINNINGMODE		= 5,	// -	binningmode 
	CC_SETMODE1				= 6,	// -	moderegister1  
	CC_SETUSER0   			= 7,	// -	userregister0
	CC_SEPRELOAD  			= 8,	// -	preloadregister
	CC_SETFCNT0				= 9,	// -	fcntregister0   
	CC_SETFASTCOM 			= 10,	// -	baumerlink tranfer mode
	CC_DOTRIGGER 			= 11,	// -	softwaretrigger 
	CC_RESARBITER 			= 12,	// -	reset arbiter
	CC_CORBINNING 			= 13,	// -	correction of binning brightness
	CC_SETTRIGGERMODE		= 14,	// -    triggermode
	CC_SETFREEFORMAT		= 15,	// -    freeformat
	CC_SETBINNINGSIZE		= 16,	// -    set of binning size 2x2,4x4
	CC_INVTRIGGER			= 17,	// -    invert camera triggersignal 
	CC_EXTTRIGGER			= 18,	// -    switch to extern camera trigger
	CC_STOPFRAME			= 19,	// -    stop image frame	
	CC_ENABLEARBITER		= 20,	// -	enable arbiter function
	CC_DISABLEIMAGECOUNTER	= 21,	// -	disable imagecounter
	CC_ENABLETESTPATTERN	= 22,	// -	enable testpattern
	CC_ENABLEFLASH			= 23,	// -    enable flash signal on camera  
	CC_INVFLASH				= 24,	// -    invert flash signal on camera  
	CC_CAMERAOUTPUT1		= 25,	// -	res/set io1 cam
	CC_CAMERAOUTPUT2		= 26,	// -	res/set io2 cam
	CC_CAMERAINPUT1			= 27,	// -	in      io3 cam
	CC_FASTSAMPLING			= 28,	// -	fast sampling mode to increase readout rate 
	CC_DATAWIDTH			= 29,	// -    switch data transfer width( 1/2 Byte ) 
	CC_COOLER				= 30,	// -    cooleroption
	CC_NOPARAMRESTORE		= 1000,	// -    no restore of old camera state
};

typedef union _CPARAM_ {
	struct {
		int		tms; 
		ptSHT	sht;
		int     ret;
    } AsShutter;
	struct {
		double	factor; 
		ptGAIN  gain;
		int     ret;
    } AsGlGain;
	struct {
		int     offset;
		int     ret;
    } AsGlOffset;
	struct {
		int     enable;
		RECT    rect;
		int		ret;
    } AsParScan;
	struct {
		int     enable;
		int		ret;
    } AsBinning;
	struct {
		int     value;
		int		ret;
    } AsPreload;
	struct {
		WORD    value ;
		int		ret;
    } AsMode1;
	struct {
		WORD    value ;
		int		ret;
    } AsUser0;
	struct {
		int     enable;
		int		ret;
    } AsFastCom;
	struct {
		int     set;
		int		ret;
    } AsDoTrigger;
	struct {
		int		ret;
    } AsResArbiter;
	struct {
		int     enable;
		int		ret;
    } AsCorBinning;
	struct {
		int     value;
		int		ret;
    } AsBinningSize;
	struct {
		int     enable;
		int		ret;
    } AsTrigger;
	struct {
		int     set;
		int		ret;
    } AsExtTrigger;
	struct {
		int     set;
		int		ret;
    } AsInvTrigger;
	struct {
		int     set;
		int		ret;
    } AsFreeFormat;
	struct {
		int     set;
		int		ret;
    } AsStopFrame;
	struct {
		int     set;
		int		ret;
    } AsEnableArbiter;
	struct {
		int     set;
		int		ret;
    } AsDisableImageCounter;
	struct {
		int     enable;
		int		ret;
    } AsEnableTestPattern;
	struct {
		int     enable;
		int		ret;
    } AsEnableFlash;
	struct {
		int     set;
		int		ret;
    } AsInvertFlash;
	struct {
		int     set;
		int		ret;
    } AsCameraIo;
	struct {
		int     set;
		int		ret;
    } AsFastSampling;
	struct {
		int     set;
		int		ret;
    } AsCooler;
	struct {
		int     set;
		int		ret;
    } AsDataWidth;
	struct {
		int     set;
		int		ret;
    } AsParNoRestore4SnapShot;

} tCmdP, * tpCmdP; 

// collected commands for ifc commands
enum eCOLLECTCMDIDFORIFC {
	IFC_INVERTFLASH			= 0,	// - invert flash signal
	IFC_O2CONFIG			= 1,	// - config output2 for signal reaction  *)
	IFC_DIGITALOUT          = 2,	// - config of complet digital output as byte **) 
	// *) only usable for ifc with id c004
	// **) only usable for ifc with id c001, c002, 4282 
};

enum eOUTPUT2FORIFC {
	IFC_O2CONFIGLOW				= 1,	// - switch output2 to static low  signal level  *)
	IFC_O2CONFIGHIGH			= 2,	// - switch output2 to static high signal level  *)
	IFC_O2CONFIGFLASHHIGHACTIV	= 3,	// - output2 is used as high activ flash signal  *)
	IFC_O2CONFIGFLASHLOWACTIV	= 4,	// - output2 is used as low activ flash signal   *)
	IFC_O2CONFIGREADYFORTRIGGER	= 5,	// - output2 is used as ready for trigger signal *)
	// *) only usable for ifc with id c004
};

typedef union _IFCPARAM_ {
	struct {
		int     set;
		int		ret;
    } AsIfcIo;

} tIfcCmdP, * tpIfcCmdP; 

typedef struct _CCOMMAND_ {
	int	  cmdId;
	tCmdP cmdPa;
} tCmdS, * tpCmdS; 

typedef struct _COMMANDDESC_ {
	int	  num;
	tCmdS cmd[1];
} tCmdDesc, * tpCmdDesc; 

typedef struct _CCOMMANDIFC_ {
	int		 cmdId;
	tIfcCmdP cmdPa;
} tIfcCmdS, * tpIfcCmdS; 

typedef struct _COMMANDDESCIFC_ {
	int	  num;
	tIfcCmdS cmd[1];
} tIfcCmdDesc, * tpIfcCmdDesc; 

#ifndef _BO_CAM_DEF1_H_
#define _BO_CAM_DEF1_H_
typedef	struct _T_CFLUT_ { double red, green, blue; }	FLUT, * PFLUT;
typedef	struct _T_CFAWB_ { double red, green, blue; }	FAWB, * PFAWB;
#endif

#endif // _BOCAMDEFS_	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef struct _T_CT_ {
	
	int		type;			// sw initialisation type of camera, IX14
	int     hwtype;			// hw identifier 
	char *  typeByName;		// name of camera
	SIZE	tpix;			// internal use: clock frame
	SIZE	gpix;			// current frame extension in pixel
	SIZE	ppix;			// current effective frame 
	SIZE	opix;			// current effective frame offset
	SIZE	apix;			// algorith. offset of color calculation 
	double	freq;			// internal use: camera clock frequenz 

	int     boff;			// byte offset for start of frame 
	int		bpp;			// bit per pixel
	int		bColor;			// colorcamera

	int		bFrameCounter;	// framecounteroption
	int		bStartStop;		// start/stop option	 
	int     bArbiter;		// arbitersupport
	int     bCooler;		// cooler control
	
	int		cOffset;		// blacklevel offset control
	int		vOffsetMin;		// blacklevel offset minimal
	int     vOffsetMax;		// blacklevel offset maximal

	int		cAmpl;			// global gain control 
	int		vAmplMin;		// gain minimal
	int     vAmplMax;		// gain maximal

	int     bExposure;		// eposure
	int		cShutLS;		// low speed shutteroption
	int		cShutHS;		// high speedshutteroption
	int		cShutAS;		// automaticshutter
	int		cShutFS;		// fast speed shutteroption
	int		cTrigMode;		// triggeroption
	int		cIlacMode;		// interlacedmode
	int		cPartialScan;	// partialscan 
	int		cBinnMode;		// binningmode

	int		cCamInterface;  // controlinterface:  Centronics, Debi ...

	int		vShutLsMin;	    // LsShutter minimal
	int		vShutLsMax;	    // LsShutter maximal
	int		vShutLsDis;
	int		vShutHsMin;	    // HsShutter minimal
	int		vShutHsMax;	    // HsShutter maximal
	int		vShutHsDis;
	int		vShutFsMin;	    // HsShutter minimal
	int		vShutFsMax;	    // HsShutter maximal
	int		vShutFsDis;

	DWORD	econfig1;	    // internal, enable config bits --> see config register
	DWORD	econfig2;		// internal 	

    int		cShading;       // Shading capability

} tBxCameraType, * tPBxCameraType;

typedef struct  _T_CS_ {
	
	struct {
		unsigned okFg		: 1;	// ifc initialized
		unsigned okIo		: 1;	// controlinterface 
		unsigned okMem		: 1;	// statical memory 
		unsigned okMacroL	: 1;	// startmacro found
		unsigned okMacroE	: 1;	// startsacro executed
		unsigned okFgFound	: 1;	// ifc found
		unsigned okCam		: 1;	// camera found
		
	} init;
	
	int		cShutMode;		// current shuttermode
	int		vShutLS;		// current io-values of shuttermode 
	int		vShutHS;		// current io-values of shuttermode
	int		vShutFS;		// current io-values of shuttermode

	int		vShutAS;		// desired value for automaticshutter
	int		vShutAS_SM;		// current shuttermode for automaticshutter
	int		vShutAS_LS;
	int		vShutAS_HS;

	double  vShutLSTime;	// current shuttertime in µs

	int		vOffset;		// current io value for blacklevel offset
	int		vAmplif;		// current io value for global gain 
	double	vAmplifFactor;	// current absolut factor value for global gain 

	RECT    rInspect;		// window for inspection

	int		bLutEnable;		// enable rgb-Lut
	FLUT	vLut;			// factors of rgb-Lut							
	
	int		bAwbEnable;		// enable awb-rgb-Lut
	FAWB	vAwb;			// factors of awb-rgb-Lut							

	int		bMemoryDmaDisable;
	int		bMemoryDoubleBuffer;
	int		bMemoryRingBuffer;
	int		bMemoryRingBufferNum;

#ifdef CXLIBINTERN
	CRect rcCamMax;			// max.border for image frame
	CRect rcCamInp;			// current image frame
	CRect rcZoomIn;			// required image frame for translation 
	CRect rcCaZoomInCor;	// corrected  rcZoomIn for camera
	CRect rcFgZoomInCor;	// corrected  rcZoomIn for ifc
#else
	RECT rcCamMax;			// max.border for image frame
	RECT rcCamInp;			// current image frame
	RECT rcZoomIn;			// required image frame for translation 
	RECT rcCaZoomInCor;		// corrected  rcZoomIn for camera
	RECT rcFgZoomInCor;		// corrected  rcZoomIn for ifc
#endif //CXLIBINTERN

	// dma3-parameter
	int nrSubZ;				// number of calculated sublines
	int szSubZ;				// site of one subline
	int nrLines;			// number of image lines
	// camera-modis
	int bBinning;			// state of binning mode
	int iBinningMode;		// size of binningmode
	int bTriggerEn;			// state if trigger mode 
	int bTrigger;			// softtrigger
	int bPartialScan;		// state of partial scan mode 
	int iBytePerPixel;      // current number of bytes per pixel
	int bVideoDma;
	int bMemoryDma;
	int bStandBy;			// internal use
	int bEsync; 
	int bPnp1;
	int bPnp2;
	int bPnp1fl;

	// ScanModis
	int scanmode;			// scanmode see BX_SetScanMode
	int scanbuffs;			// number of buffers
	PVOID  scandynmm;	    // pointer auf speichertabelle

	DWORD cconfig1;
	DWORD cconfig2;

    int	  bShading;         // Shading capability

} tBxCameraStatus, * tPBxCameraStatus;

typedef struct  {
	int		iFlag;					// flag for correct content
	int		iCamNr;					// logical camera number.
	int		iCamId;					// camera init ident
	char    sName[MAX_PATH];		// namera name
	int		iFgNr;					// logical Ifc-Number
	int     iFgPort;				// ifc port
	int     bMux;					// multiplexer behind ifc port
	int     bMuxPort;				// multiplexerport
	
} tDefineDigitalCamera, *ptDefineDigitalCamera;


#define MAX_DYNBUF_ANZ	512   // see #include "7146api.h" --> MAX_BUF_ANZ
#define MAX_STABUF_ANZ	512

// VIRTUAL BUFFER * VIRTUAL BUFFER * VIRTUAL BUFFER *************************
// Description structure of one virtual memory buffer
typedef struct 
{
    void      *vmem;        // virtuel memory pointer to buffer
    DWORD     vmemLen;      // buffer size in bytes
    DWORD     status;       // information about buffer status, only reading
                            // access
    DWORD     picNr;        // picture number if status is BUF_FILLED, only
                            // reading access 
}   DMEM_BUF, *PDMEM_BUF;

// Description structure of a field of virtual memory buffers filled by user
typedef struct _DVMEM_STRUCT
{	int  n;
    DMEM_BUF  buffer[MAX_DYNBUF_ANZ];  // field of buffer descriptors 

}   DVMEM, *PDVMEM;

typedef struct _LVMEM_STRUCT
{	
    DMEM_BUF  buffer[MAX_DYNBUF_ANZ];  // field of buffer descriptors 

}   LVMEM, *PLVMEM;


// STATIC BUFFER * STATIC BUFFER * STATIC BUFFER ****************************
// Description structure of one virtual memory buffer
enum eSmemLockBuf {
	eLockBuf4 = 4,
	eLockBuf8 = 8,
};

typedef struct 
{
    void      *vmem;        // virtuel memory pointer to static buffer
    DWORD     vmemLen;      // buffer size in bytes
    DWORD     status;       // information about buffer status, only reading
                            // access
    DWORD     picNr;        // picture number if status is BUF_FILLED, only
                            // reading access 
}   STAMEM_BUF, *PSTAMEM_BUF;

typedef struct _LSMEM_STRUCT
{	
    STAMEM_BUF  buffer[MAX_STABUF_ANZ];  // field of buffer descriptors 

}   LSMEM, *PLSMEM;

typedef struct _LSTAMEM_STRUCT
{	eSmemLockBuf  reqNum;
	int			  reqSize;
    STAMEM_BUF  buffer[MAX_STABUF_ANZ];  // field of buffer descriptors 

}   SVMEM, *PSVMEM;

/*
// STATIC BUFFER * STATIC BUFFER * STATIC BUFFER
// Description structure of one STATIC memory buffer
typedef struct 
{
    void      *smem;        // static memory pointer to buffer
	DWORD     smemphy;      // static memory phys. pointer to buffer
}   SLMEM_BUF, *PSLMEM_BUF;

// Description structure of a field of virtual memory buffers filled by user
typedef struct _SLMEM_STRUCT
{	DWORD	  smemNum;
	DWORD     smemLen;
    SLMEM_BUF  buffer[MAX_STABUF_ANZ];  // field of buffer descriptors 

}   SLMEM, *PSLMEM;
*/




// Description structure of a command field  
// filled by user , only for special camera
typedef struct _PARAM_NUM_ {
	double		systime;	// systemtime of useage
	int         imnumber;	// wird nur bei preloads benutzt
	int			delay;		// delay time
	int         mode;		// io-mode
	int         canal;		// Kanal
	int			command;	// Kommandotype
	int			param1;
	int			param2;
	int			param3;
	int			param4;
	int			param5;
	int			param6;
} tParameterNum, * tpParameterNum;

// source definition 
enum eEVENTSRC {
	SRC_IFC,
	SRC_CAM,
};

/*****************************************************************************\
Function:            BX_GetVersion
Description:		 Versionnumber with Major, Minor1, Minor2, Releasebuildnumber	
Parameters :		 no	
Return Value:
	char*			 pointer to null terminated char string
Notes:
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
char* WINAPI BX_GetVersion();	

/*****************************************************************************\
Function:           BX_SetMsgOutput
Description:		Enable or disable the output of error messages
Parameters :
	<int enable >	 - enable & ERRMSG_BOX  = TRUE    - enable error output to MessageBox
					 - enable & ERRMSG_FILE = TRUE    - enable error output to File
Return Value:
	void 
Notes:				The standard setting is to have output of error messages
                    switched off	
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
#define ERRMSG_BOX  1
#define ERRMSG_FILE 2
void WINAPI BX_SetMsgOutput( IN int flags);

/*****************************************************************************\
Function:            BX_GetErrorCode
Description:		 Providing of the latest error code	
Parameters :
Return Value:
	int 
Notes:				 errorcode see cerror.h
See Also:
******************************************************************************
Created:             08.04.2002
Last modified:       08.04.2002
\*****************************************************************************/
int WINAPI BX_GetErrorCode( ); 


/*****************************************************************************\
Function:           BX_CameraTable
Description:		Liefert eine Beschrebungstabelle der initialisierbaren Kameras,
					d.h Kameraname, InitialisierunsId und KamerahardwareId 
Parameters :
	<tDcName ** table>		 - pointer <TABLE> auf einen strukturpointer vom Typ <tDcNAme> 
Return Value:
	int 		Anzahl der unterschiedlichen Kameraeinträge
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
typedef struct {
	TCHAR* name;
	int	    type;
	int     hwid; 
} tDcName, ptDcName;

int WINAPI BX_CameraTable(  OUT tDcName ** table );


/*****************************************************************************\
Function:           BX_CameraTableInstalled
Description:		Liefert eine Beschreibungstabelle der installierten Kameras,
					d.h Kameraname, InitialisierunsId und KamerahardwareId 
Parameters :
	<tDcName ** table>		 - pointer <TABLE> auf einen strukturpointer vom Typ <tDcNAme> 
Return Value:
	int 		Anzahl der unterschiedlichen Kameraeinträge
Notes:
See Also:
******************************************************************************
Created:             26.02.2003
Last modified:       26.02.2003
\*****************************************************************************/
int WINAPI BX_CameraTableInstalled(  tDcName ** table  );


/*****************************************************************************\
Function:            BX_RegCameraByDesc
Description:		 userdefined entrys to define a camera system	
Parameters :
	<int iCamAnz>					 - number of descriptor elements of array  
	<ptDefineDigitalCamera pDefCam>	 - pointer to description array 
	<int bClearRegistry>			 - clear old registry entrys before 
Return Value:
	int  
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int  WINAPI BX_RegCameraByDesc( IN int iCamAnz, IN ptDefineDigitalCamera pDefCam, IN int bClearRegistry );    

/*****************************************************************************\
Function:            BX_CameraSystemManager
Description:		 dialogbased cameramanager to define a camera system	
Parameters :
	<void(CALLBACK * lpfnTimer>		 - callback function to signal a changed system
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
typedef void ( *CamSysInfoProc )( DWORD changeSystem );
int WINAPI BX_CameraSystemManager( IN CamSysInfoProc pCallback ); 

/*****************************************************************************\
Function:			BX_InitCamSys
Description:		Init the camerasystem that was defined in registry in 
					library BxLib
Parameters :
Return Value:
	int  
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int  WINAPI BX_InitCamSys(   );   
// for test only
int  WINAPI BX_InitCamSysX( DWORD sel );    

/*****************************************************************************\
Function:           BX_InitCamByNr
Description:		Initialisation of a logical camera   
Parameters :
	<int nr>		 - camera label
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int WINAPI BX_InitCamByNr( IN int nr  );    


/*****************************************************************************\
Function:           BX_InitCamByNrAndType
Description:		Initialisation of a logical camera   	    
Parameters :
	<int nr>		 - camera label
	<int type>		 - type of camera
Return Value:
	int 
Notes:				the logical camera must be defined by registry,
					but you can overwrite the registry type permanently	
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int WINAPI BX_InitCamByNrAndType( IN int nr, IN int type  );    

/*****************************************************************************\
Function:       BX_EnumeratePcLabelevices
Description:    Asking about installed and supported baumer interfacecards
Parameters :
	<tpBOIFCDEV DevAsk>		 - pointer to a description structure 
Return Value:
	int   		TRUE/FALSE
Notes:			structure is for max. MAXBOIFCNR interfacecards prepared
See Also:
******************************************************************************
Created:			 04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
#define MAXBOIFCNR	8
typedef struct _BOIFC  {
	int		hwid;				// hardware ident of baumer ifc 
	char    name[64];			// name of baumer ifc
} tBOIFC, * tpBOIFC;

typedef struct _BOIFCHD  {
	int		 itemNr;			// number of Items
	tBOIFC	 item[MAXBOIFCNR];	// item itself, max. MAXBOIFCNR
} tBOIFCDEV, * tpBOIFCDEV; 

int WINAPI BX_EnumeratePciDevices( OUT tpBOIFCDEV DevAsk );   

/*****************************************************************************\
Function:            BX_EnumerateCamSystem
Description:    
Parameters :
	<OUT int * piCamCount>				 - number of defined cameras
	<OUT ptDefineDigitalCamera * ppCam>	 - array of camera descriptors
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int WINAPI BX_EnumerateCamSystem( OUT int * piCamCount, OUT ptDefineDigitalCamera * ppCam);

/*****************************************************************************\
Function:            BX_ValidateCamSystem
Description:    
Parameters :
	<OUT int * piCamCount>				 - number of detected cameras
	<OUT ptDefineDigitalCamera * ppCam>	 - array of camera descriptors
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             14.08.2002
Last modified:       14.08.2002
\*****************************************************************************/
int WINAPI BX_ValidateCamSystem(  OUT int * piCamCount, OUT ptDefineDigitalCamera * ppCam);


/*****************************************************************************\
Function:            BX_ValidateCamSystemSD 
Description:         Validation of camera system   
Parameters :
	<OUT int * piCamCount>				 - number of detected cameras
	<OUT ptDefineDigitalCamera * ppCam>	 - array of camera descriptors
Return Value:
	int 
Notes:               Use it only before InitCamSys and only for iX-cameras
See Also:            BX_RegCameraByDesc
******************************************************************************
Created:             14.08.2002
Last modified:       14.08.2002
\*****************************************************************************/
int WINAPI BX_ValidateCamSystemSD(  OUT int * opiCamCount, OUT ptDefineDigitalCamera * oppCam);

/*****************************************************************************\
Function:            BX_SetScanMode
Description:    
Parameters :
	<int fId>		 - ifc label
	<int sm>		 - scanmode
	<PVOID pvmem>	 - parameter as function of scanmode
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int WINAPI BX_SetScanMode( IN int fId, IN int sm,  IN PVOID pvmem  );


/*****************************************************************************\
Function:            BX_SetDigOutput
Description:    
Parameters :
	<int fId>		 - ifc label
	<WORD output>	 - output value
Return Value:
	int              - TRUE, CE_NOINIT, CE_NOSUPFUNCTION
Notes:
See Also:
******************************************************************************
Created:             12.01.2004
Last modified:       12.01.2004
\*****************************************************************************/
int WINAPI BX_SetDigOutput( IN int fId, OUT WORD output );


/*****************************************************************************\
Function:            BX_GetDigInput
Description:    
Parameters :
	<int fId>		 - ifc label
	<WORD* input>	 - pointer for input value
Return Value:
	int              - TRUE, CE_NOINIT, CE_NOSUPFUNCTION 
Notes:
See Also:
******************************************************************************
Created:             12.01.2004
Last modified:       12.01.2004
\*****************************************************************************/
int WINAPI BX_GetDigInput( IN int fId, OUT WORD* input );




/*****************************************************************************\
Function:       BX_StopScanMode
Description:    vor jeder Speicherfreigabe von virtuellem Speicher, der für den MemoryDMA
				( Bildaufnahme ) genutzt wurde ist dieser Befehl auszuführen . 
Parameters :
	<int fId>	- ifc label
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int WINAPI BX_StopScanMode( IN int fId );

/*****************************************************************************\
Function:           BX_LoopControl
Description:		Controls the usage of memory buffers for image reception
                    in loop mode
Parameters :
	<int fId>		 - logical interface card number
	<PLVMEM buffers> - Data structure with virtual memory blocks and status
					   information
	<int freeBufNr>	 - Buffer number that should be freed for next 
					   image receptions  
Return Value:
	int WINAPI
Notes:				The function needs a valid buffers structure with the
                    previously allocated virtual memory blocks.
                    freeBuf is only valid, if status is BUF_FILLED 

                    This function is working under Windows 2000
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int WINAPI BX_LoopControl( IN int fId, IN PLVMEM buffers, IN int freeBufNr );

/*****************************************************************************\
Function:            BX_GetActiveLoopBuffer
Description:		Watching the current memory buffer for image reception
                    in loop mode
Parameters :
	<int fId>		 - logical interface card number
Return Value:
	0...MAX_BUF_ANZ-1  Current loop buffer for image reception
    < 0             If there was an error
Notes:This function is working  only under Windows 2000
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int WINAPI BX_GetActiveLoopBuffer( IN int fId  );

/*****************************************************************************
Functionname :      BX_LoopControlSMem
Description:        Controls the usage of memory buffers for image reception
                    in loop mode with static buffers
Parameters:
    <buffers>       Data structure with static memory blocks and status
                    information initialized by LoadPictureExSMem
    <freeBufNr>     Buffer number that should be freed for next 
                    image receptions  
    <fId>           Number of the interface card
Return Value:        
    
	TRUE            If action was successful
    FALSE           If there was an error

Notes:              This function is working under Windows NT/2000/XP
                    and needs an installed "mapmem.sys" or "boifcdrv.sys"
                    driver
                    
                    freeBuf is only valid, if status is BUF_FILLED 

                    The following interface cards are supported:
                    0x4282, 0xC001, 0xC002, 0xC006
*****************************************************************************/
BOOL WINAPI BX_LoopControlSMem(IN int fgNr, PSVMEM buffers, int freeBufNr );

/*****************************************************************************
Functionname :      GetActiveLoopBufferSMem
Description:        Watching the current memory buffer for image reception
                    in loop mode with static memory buffers
Parameters:
    <fgNr>          Number of the interface card
    
Return Value:        
 0...MAX_BUF_ANZ_SMEM -1  Current loop buffer for image reception
    < 0             If there was an error

Notes:              This function is working under Windows NT/2000/XP
                    and needs an installed "mapcon.sys" or "boifcdrv.sys"
                    driver

                    The following interface cards are supported:
                    0x4282, 0xC001, 0xC002, 0xC006
*****************************************************************************/
int WINAPI BX_GetActiveLoopBufferSMem( IN int fgNr );

/*****************************************************************************\
Function:       BX_GetCameraInfo
Description:    after camera initialisation current status and type of camera is
				readably
Parameters :
	<int cLabel>		- camera label 
	<tPCameraType T>	- pointer to  structure of type <tPCameraType>
	<int sizeT>			- size of structure
	<tPCameraStatus S>	- pointer to  structure of type <tPCameraStatus>
	<int sizeS>			- size of structure
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int WINAPI BX_GetCameraInfo( IN int cLabel, OUT tPBxCameraType T, IN int sizeT, OUT tPBxCameraStatus S, IN int sizeS ); 

/*****************************************************************************\
Function:       BX_SetDataWidth
Description:    Change data with of camera 
Parameters :
	<int cLabel> - camera label
	<int type>	 - type data width --> enum-definition CXPIXWIDTH
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
#ifndef DEF_PIXWIDTH
#define DEF_PIXWIDTH
enum ePIXWIDTH	{
	W8BIT,
	W10BIT,
	W12BIT,
	W16BIT
};
#endif
int WINAPI  BX_SetDataWidth( IN int cLabel, IN ePIXWIDTH type	);

/*****************************************************************************\
Function:       BX_DmaImFormat
Description:    Set special readout mode for cameras
Parameters :
	<int cLabel>			- camera label
	<ptFormat pDmaImFormat>	- structure with format entrys 
Return Value:
	int  
Notes:
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int    WINAPI BX_DmaImFormat( IN int cLabel, OUT ptFormat pDmaImFormat );

/*****************************************************************************\
Function:       BX_ResetCamera
Description:    Switch camera to power up status
Parameters :
	<int cLabel>	- logical camer number 
Return Value:
	int    WINAPI
Notes:
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int    WINAPI BX_ResetCamera( IN int cLabel );

/*****************************************************************************\
Function:            BX_SetBlackOffset
Description:		 Set of BlackLevelOffset value   
Parameters :
	<int cLabel>		  - camera label
	<int offset>		  - offsetvalue
	<ptBLOFFSET bloffset> - pointer to structure of type tBLOFFSET
Return Value:
	int WINAPI
Notes:	
See Also:
******************************************************************************
Created:             22.03.2002
Last modified:       22.03.2002
\*****************************************************************************/
int WINAPI BX_SetBlackOffset( IN int cLabel, IN int offset, OUT ptBLOFFSET bloffset );

/*****************************************************************************\
Function:           BX_SetGainFactor
Description:		Set the Gainfactor to the nearest possible value 
Parameters :
	<int cLabel>	 - camera label
	<double factor>	 - GainFactor
	<ptGAIN gain>	 - pointer to structure of type tGAIN
Return Value:
	int  WINAPI
Notes:				if gain   == NULL the structure is not filled
					if factor == 0.0  the structure is filled with values,
					so the range can be detected
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int  WINAPI BX_SetGainFactor( IN int cLabel, IN double factor, OUT ptGAIN gain );

/*****************************************************************************\
Function:           BX_SetShutterTime
Description:		Set the Shuttertime to the nearest possible value   
Parameters :
	<int cLabel>	- camera label
	<int tms>		- Shuttertime
	<ptSHT sht>		- Pointer to structure of type tSHT
Return Value:
	int  
Notes:				if sht == NULL the structure is not filled
					if tms == -1   the structure is filled with values,
					so the range can be detected
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int  WINAPI BX_SetShutterTime( IN int cLabel, IN int tms, OUT ptSHT sht );

/*****************************************************************************\
Function:           BX_SetTriggerMode
Description:		Setz triggermode parameter
Parameters :
	<int cLabel>	- camera label
	<int set>		- enable/disable
	<int disch>		- not used
Return Value:
	true / false
Notes:
See Also:	supported add. trigger options are invert triggersignal and switch 
			to extern trigger signal.
			use id CC_INVTRIGGER to  invert triggersignal 
			use id CC_EXTTRIGGER to  switch to external trigger
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int WINAPI BX_SetTriggerMode( IN int cLabel, IN int set, IN tpCmdDesc c );

/*****************************************************************************\
Function:            BX_EnableCamArbiter
Description:		 Enable/Disable of Arbiter for logical camera 	
Parameters :
	<int cLabel>	 - camera label
Return Value:
	int				 < 0 - Error
					 >=0 - Number of cameras in non trigger mode( free running) 
						   attached by this interfacecard
Notes:				 not all cameras support this feature
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int WINAPI BX_EnableCamArbiter( IN int cLabel, IN int set );

/*****************************************************************************\
Function:           BX_DoTrigger
Description:		Softwaretrigger
Parameters :
	<int cLabel>	- camera label
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int WINAPI  BX_DoTrigger( IN int cLabel );

/*****************************************************************************\
Function:            BX_CamStart
Description:		 Start/Stop of Cameraframegeneration 	
Parameters :
	<int cLabel>	 - locical camera number 
	<int  set>		 - Start: 1 / Stop: 0
Return Value:
	int
Notes:				
See Also:
******************************************************************************
Created:             18.03.2002
Last modified:       18.03.2002
\*****************************************************************************/
int  WINAPI BX_CamStart( IN int cLabel, IN int  set );

/*****************************************************************************\
Function:       BX_SetBinningMode
Description:    Enable of binningmode, extended for different binningmodes     
Parameters :
				<cLabel>			 - number of selected camera
				<pCmdExt>		 - pointer to a structure of type tpCmdDesc
				<bSet>			 - enable binning option
Return Value:
    int		TRUE / FALSE - Operation erfolgt

Notes:			pointer of struct <c> is usable for id's CC_CORBINNING CC_SETBINNINGSIZE
				pointer can be NULL, only usable for cameras of type iX...
See Also:
\*****************************************************************************/

enum eCXBINNINGMODE {
	BINNO  = 0,
	BIN2X2 = 1,
	BIN4X4 = 2,
};

int WINAPI  BX_SetBinningMode( IN int cLabel, IN int bSet, IN tpCmdDesc pCmdExt	);

/*****************************************************************************\
Function:        BX_SetPartialScan
Description:     enable/disable  partialscanmode
Parameters :
	<int cLabel>	 - camera label
	<int enable>	 - enable of mode
	<PRECT proi>	 - region of interest
Return Value:
	int WINAPI
Notes:
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int WINAPI  BX_SetPartialScan( IN int cLabel, IN int enable, IN PRECT proi );

/*****************************************************************************\
Function:           BX_SetExtImFormat
Description:		Select a non standard image format
Parameters :
	<int cLabel>	 - camera label
	<int mode>		 - extended image format
	<PVOID todo>	 - reserverd parameter 
Return Value:
	int WINAPI
Notes:		<todo> currently not used, reserved for future design 
See Also:
******************************************************************************
Created:             08.04.2002
Last modified:       08.04.2002
\*****************************************************************************/
int WINAPI  BX_SetExtImFormat( IN int cLabel, IN int mode, IN PVOID todo );

/*****************************************************************************\
Function:           BX_CamIoSettings
Description:		Enable set/reset of camera-io's, read input signals
Parameters :
	<int cLabel>		 - camera label
	<tpCmdDesc pCmdExt>	 - command descriptor
Return Value:
	int
Notes:				descriptors intended are:
					CC_CAMERAOUTPUT1,	CC_CAMERAOUTPUT2,	CC_CAMERAINPUT1
					pointer <pCmdExt> can be NULL
See Also:
******************************************************************************
Created:             18.03.2002
Last modified:       18.03.2002
\*****************************************************************************/
int  WINAPI BX_CamIoSettings( IN int cLabel, IN tpCmdDesc pCmdExt );
			
/*****************************************************************************\
Function:       BX_CamAuxSettings
Description:    
Parameters :
	<int cLabel>		 - locical camera number 
	<tpCmdDesc pCmdExt>	 - command descriptor 
Return Value:
	int
Notes:			descriptors intended are:
 					CC_ENABLETESTPATTERN
See Also:
******************************************************************************
Created:             18.03.2002
Last modified:       18.03.2002
\*****************************************************************************/
int  WINAPI BX_CamAuxSettings( IN int cLabel, IN tpCmdDesc pCmdExt );

/*****************************************************************************\
Function:            BX_CamFlashSettings
Description:    
Parameters :
	<int cLabel>		 - locical camera number 
	<int enable>		 - enable: 1 / disable:0
	<tpCmdDesc pCmdExt>	 - command descriptor
Return Value:
	int
Notes:				descriptors intended are:
						CC_INVFLASH
See Also:
******************************************************************************
Created:             18.03.2002
Last modified:       18.03.2002
\*****************************************************************************/
int  WINAPI BX_CamFlashSettings( IN int cLabel, IN int enable, IN tpCmdDesc pCmdExt );

/*****************************************************************************\
Function:           BX_SetFrameCounter
Description:		framecounteroption of camera
Parameters :
	<int cLabel>	 - locical camera number 
	<int enable>	 - enable: 1 / disable:0
	<int set>		 - set counter: 1 / ignore <counter>: 0
	<WORD counter>	 - 16 bit counter value 
Return Value:
	int
Notes:			
See Also:
******************************************************************************
Created:             18.03.2002
Last modified:       18.03.2002
\*****************************************************************************/
int  WINAPI BX_SetFrameCounter( IN int cLabel, IN int enable, IN int set, IN WORD counter );

/*****************************************************************************\
Function:           BX_ControlCameraByCmdDesc
Description:		Configuration of camera with a collect command
Parameters :
	<int cLabel>		 - number of selected camera
	<tpCmdDesc collect>	 - pointer to a structure of type tCmdDesc
Return Value:
	int				TRUE / FALSE
Notes:				Possible commands for <collect> are designated by enum _COLLECTCMDID_.
					Don't forget to set the right number of commands in <collect>->num for 
					a proper work of this function
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int WINAPI BX_ControlCameraByCmdDesc( IN int cLabel, IN tpCmdDesc collect );

/*****************************************************************************\
Function:           BX_SwitchToCamera
Description:		Switch to an other camera attached by the same interfacecard
Parameters :
	<int cLabel>	 - camera label
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int WINAPI BX_SwitchToCamera( IN int cLabel  );    


/*****************************************************************************\
Function:            BX_EnableIfcArbiter
Description:		 Enable/Disable of Arbiter for Interfacecard in IxCamerasystems 	
Parameters :
	<int fId>		 - ident of interfacecard
Return Value:
	int				 < 0 - Error
					 >=0 - Number of cameras in non trigger mode( free running) 
						   attached by this interfacecard
Notes:
See Also:
******************************************************************************
Created:             12.02.2002
Last modified:       12.02.2002
\*****************************************************************************/
int WINAPI BX_EnableIfcArbiter( IN int fId, IN int set );

/*****************************************************************************\
Function:            BX_IfcIo
Description:    
Parameters :
	<int fId>			- ifc label
	<tpCmdDesc collect> - pointer to a structure of type tCmdDesc	 
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int WINAPI BX_IfcIo( IN int fId, IN tpIfcCmdDesc collect  );    

/*****************************************************************************\
Function:            BX_IfcFlashSignal
Description:    
Parameters :
	<int fId>		- ifc label
	<int enable>	- enable flashsignal
	<int invert>	- invert flashsignal 
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             04.04.2002
Last modified:       04.04.2002
\*****************************************************************************/
int WINAPI BX_IfcFlashSignal( IN int fId, IN int enable, IN int invert  );    

/*****************************************************************************\
Function:           BX_ShowIfcMonitor
Description:		Interfacecard monitor for experienced developers only	
Parameters :
	<int bShow>	 -  show/hide state of monitor
Return Value:
	HWND 
Notes:				dont call this monitor without kenntnis des pci-controllers
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
HWND WINAPI BX_ShowIfcMonitor( IN int bShow );

/*****************************************************************************\
Function:           BX_SetAutoExposure
Description:		Start an automatic  mechanism to control the shutter time
					for a given brightness 
Parameters :
	<int cLabel>	 - camera label
	<int start>		 - enable(TRUE)/disable(FAlSE); 
	<int bright>	 - brightness to control
Return Value:
	int  
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int  WINAPI BX_SetAutoExposure( IN int cLabel, IN int start, IN int bright );

/*****************************************************************************\
Function:            BX_DoWhiteBalance
Description:    
Parameters :
	<int cLabel>	 - camera label
	<int enable>	 - enable(TRUE)/disable(FAlSE)
	<double * fr>	 - pointer to result value for red canal
	<double * fg>	 - pointer to result value for green canal
	<double * fb>	 - pointer to result value for blue canal
	<RECT roi>		 - region of interest, if empty then whole area of sensor is
					   used	
Return Value:
	DWORD 
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int WINAPI BX_DoWhiteBalance( IN int cLabel, IN int enable, OUT double * fr, OUT double * fg, OUT double * fb, IN RECT roi  );

/*****************************************************************************\
Function:            BX_SetWhiteBalance
Description:		 set the factors for whitebalance 
					
Parameters :
	<int cLabel>	 - camera label
	<int enable>	 - enable usage of current factors in internal LUT's
	<int set>		 - moving of parameter factors to usable factors
	<double * fr>	 - pointer to value for red canal
	<double * fg>	 - pointer to value for green canal
	<double * fb>	 - pointer to value for blue canal
Return Value:
	int 
Notes:				From these factors the RGB LUT's is calculated. 
					Into this LUT's enter likewise manually set RGB factors. 
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int WINAPI BX_SetWhiteBalance( IN int cLabel, IN int enable, IN int set, IN double * fr, IN double * fg, IN double * fb );

/*****************************************************************************\
Function:           BX_SetRgbLut
Description:		set the factors for RGB-LUT-manipulation  
Parameters :
	<int cLabel>	 - camera label
	<int enable>	 - enable usage of current factors in internal LUT's
	<int set>		 - moving of parameter factors to usable factors
	<double * fr>	 - pointer to value for red canal
	<double * fg>	 - pointer to value for green canal
	<double * fb>	 - pointer to value for blue canal
Return Value:
	int 
Notes:				 From these factors the RGB LUT's is calculated. 
					 Into this LUT's enter likewise RGB factors detected by whitebalance. 
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int WINAPI BX_SetRgbLut( IN int cLabel, IN int enable, IN int set, IN double * fr, IN double * fg, IN double * fb );

/*****************************************************************************\
Function:            BX_SetGammaLut
Description:		 additional universal characteristic for r,g,b
Parameters :
	<int cLabel>			- camera label
	<int bEnable>			- enable/disable the use of lut-arrays
	<WORD awRed[4096]>		- array of max. 4096 entrys for red  -lut
	<WORD awGreen[4096]>	- array of max. 4096 entrys for green-lut
	<WORD awBlue[4096]>		- array of max. 4096 entrys for blue -lut
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int  WINAPI BX_SetGammaLut( IN int cLabel, IN int bEnable, IN WORD awRed[4096], IN WORD awGreen[4096], IN WORD awBlue[4096] );
  
/*****************************************************************************\
Function:            BX_SetMacbethColors
Description:    
Parameters :
	<int cLabel>				- camera label
	<COLORREF acrMacbeth[24]>	- area of measured color patterns
	<double adblMatrix[4][4]>	- transformation matrix
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int WINAPI BX_SetMacbethColors(IN int cLabel, IN COLORREF acrMacbeth[24], OUT double adblMatrix[4][4] );

/*****************************************************************************\
Function:           BX_SetTransformationMatrix
Description:		set the current transformation matrix	
Parameters :
	<int cLabel>				-	camera label
	<double adblMatrix[4][4]>	-	transformation matrix
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int WINAPI BX_SetTransformationMatrix( IN int cLabel, IN double adblMatrix[4][4] );

/*****************************************************************************\
Function:            BX_EnableTransformationMatrix
Description:		 enable the curent transformation matrix	
Parameters :
	<int cLabel>	 - camera label
	<int enable>	 - enable/disable 
Return Value:
	int 
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int WINAPI BX_EnableTransformationMatrix( IN int cLabel, IN int enable  );

/*****************************************************************************\
Function:            BX_DefineImageNotificationEvent
Description:		 Set an eventhandle that can signal a new image event
Parameters :
	<int src>		 - source IFC or camera
	<int id>		 - label of source
	<HANDLE event>	 - handle to a self reset event 
Return Value:
	int WINAPI
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int WINAPI BX_DefineImageNotificationEvent( IN int src, IN int id, IN HANDLE event );

/*****************************************************************************\
Function:            BX_GetCurrentImNumber
Description:		 Delivers the curent imagenumber generated by ifc
					 and the current DMA-index of pool buffer
Parameters :
	<int src>	    - source IFC or camera
	<int id>	    - label of source
Return Value:
	DWORD			0xFFFFFFFF - error
					High Word  - Dma-index für loopmacros is corresponding with 
								 the number of buffers aguired by BX_SetScanmode
					Low  Word  - ImageNumber
Notes:				ImageNumber is modulo 12 bit 
See Also:
******************************************************************************
Created:             08.03.2002
Last modified:       08.03.2002
\*****************************************************************************/
DWORD WINAPI BX_GetCurrentImNumber( IN int src, IN int Id  );	 

/*****************************************************************************\
Function:            BX_GetDataPtr
Description:		 Delivers a pointer to the current pool range of scanner data
Parameters :
	<int fId>	   - ifc label
Return Value:
	PBYTE   WINAPI
Notes:				 
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
PBYTE   WINAPI  BX_GetDataPtr( IN int fId );

/*****************************************************************************\
Function:            BX_GetDataPtrByIndex
Description:		 Delivers a pointer to an indexed range of data pool
Parameters :
	<int fId>		 - ifc label
Return Value:
	PBYTE   WINAPI
Notes:				 scanmode
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
PBYTE   WINAPI  BX_GetDataPtrByIndex( IN int fId, IN int nr );


// types for GetBmp....
enum eBMPACQTYPE 
{
	NATIV		= 0x00000000,// 1*256  colors: nativ image of sensor as 
	CONVERT		= 0x00000001,// 3*256  colors: convert bayer pattern to true color format
	G256		= 0x00000002,// 1*256  colors: monochrome
	G1024		= 0x0a000002,// 1*1024 colors: monochrome
	G4096		= 0x0c000002,// 1*4096 colors: monochrome
	C256		= 0x08000001,// 3*256  colors: convert bayer pattern to true color format
    C1024		= 0x0a000001,// 3*1024 colors: convert bayer pattern to true color format
	C4096		= 0x0c000001,// 3*4096 colors: convert bayer pattern to true color format
};

/*****************************************************************************\
Function:            BX_GetBmpDirect
Description:		 Convert the current image to bitmap format
Parameters :
	<int cLabel>			- camera label
	<PBITMAPFILEHEADER pBmf>- destinantion pointer to file header
	<PBITMAPINFOHEADER pBmi>- destinantion pointer to info header
	<PBYTE  pBmp>			- destinantion pointer to date buffer
	<DWORD allocMemSize>    - allocated memory size
	<int modus	>			- deliver type of bitmap
Return Value:
	DWORD 
Notes:				The information inclusive bit-map file headers & 
					bit-map info. header are supplied. 
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/ 
DWORD WINAPI BX_GetBmpDirect( IN int cLabel, OUT PBITMAPFILEHEADER pBmf, OUT PBITMAPINFOHEADER pBmi, OUT PBYTE  pBmp, IN DWORD allocMemSize, IN int modus	);

/*****************************************************************************\
Function:            BX_GetBmp
Description:		 Convert the current image to bitmap format
Parameters :
	<int cLabel>	 - camera label
	<PVOID bp>		 - destinantion pointer to date buffer inclusive file/info header
	<DWORD allocSrcMemSize> - allocated memory size
	<int modus	>	 - deliver type of bitmap
Return Value:
	DWORD 
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
DWORD WINAPI BX_GetBmp( IN int cLabel, OUT PVOID bp, IN DWORD allocSrcMemSize, IN int modus	);

/*****************************************************************************\
Function:            BX_GetBmpHandle
Description:		 Convert the current image to bitmap format
Parameters :
	<int cLabel>	 - camera label
	<int modus>		 - deliver type of bitmap
Return Value:
	HGLOBAL			 - handle of a bitmap
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
HGLOBAL WINAPI BX_GetBmpHandle( IN int cLabel, IN int modus );

/*****************************************************************************\
Function:            BX_GetSnapShotBmp
Description:    
Parameters :
	<int cLAbel>			- camera label
	<tpFCmdDesc pImageType>	- 
	<int bPerformReset>		- 
	<ProgressProc pCallback>- 
	<DWORD dwData>			- 
Return Value:
	HGLOBAL 
Notes:				
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/

typedef union _FORMATCPARAM_ {
	DWORD ImageFormat;
	int   valTrigger;		// setTrigger parameter valid
	int   setTrigger;
	int   valBytePerPixel;	// setTrigger parameter valid
	int   setBytePerPixel;
	int   valFlush;			// setFlush parameter valid
	int   setFlush;
	// int   valFastSlow;		// setFastSlow parameter valid 
	// int   setFastSlow;
	int   valBinning;		// setBinning parameter valid
	int   setBinning;
	int   setBinningSize;
	int   valPartialScan;	// setPartialScan parameter valid	
	int   setPartialScan;
	RECT  recPartialScan;
} tSnapShot, *tpSnapShot; 
typedef int (*ProgressProc)(long lStepsDone, long lStepsTotal, DWORD dwData );
HGLOBAL WINAPI BX_GetSnapShotBmp( IN int cLabel, IN DWORD CamSpecImFormatTyp,  IN tpCmdDesc cmdSnapSpec, IN ProgressProc pCallback, IN DWORD dwData );


/*****************************************************************************\
Function:           BX_GetHistogram
Description:		histogrammcalculation of current image frame
Parameters :
	<int cLabel>			- camera label
	<int aiHistogram[4096]>	- array of 4096 items, for up to 12 bit pixelresolution
Return Value:
	int				<= 0 - FALSE
					>0   - Number of valid histogrammitems 
Notes:
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
int WINAPI BX_GetHistogram( IN int cLabel, OUT int aiHistogram[4096] ); 

/*****************************************************************************\
Function:            BX_GetFocusIndicator
Description:    
Parameters :
	<int cLabel>	 - camera label
	<int raster>	 - pixel distance between mesurement rowess or colums 
	<RECT roi>		 - region of interest
Return Value:
	double			measurement value mv
Notes:				mv ist the average value of all
See Also:
******************************************************************************
Created:             05.04.2002
Last modified:       05.04.2002
\*****************************************************************************/
double  WINAPI BX_GetFocusIndicator( IN int cLabel, IN int raster, IN RECT roi );

/*****************************************************************************\
Function:            BX_SetCooler
Description:		 Enable cooler element of camera	
Parameters :
	<int cLabel>		 - camera label
	<int enable>		 - enable cooler 
Return Value:
	int  
Notes:
See Also:
******************************************************************************
Created:             19.04.2002
Last modified:       19.04.2002
\*****************************************************************************/
int  WINAPI BX_SetCooler( IN int cLabel, IN int enable );

/*****************************************************************************\
Function:           BX_EnumerateImFormats
Description:		Enumeration of possible image formats of camera, 
Parameters :
	<IN int cLabel>					 - camera label
	<OUT int * piResCount>			 - number of objects of type <tDefineImageFormat> 
									   refered by pointer <ppRes>	
	<OUT ptDefineImageFormat * ppRes> - pointer of enumeration table
Return Value:
	int WINAPI
Notes:
See Also:
******************************************************************************
Created:             12.07.2002
Last modified:       07.10.2002
\*****************************************************************************/

typedef struct
{
  int    iMode;				// ID of resolution (CF_STANDARD, CF_282FULLFRAME, etc.)
  int    iWidth;			// image width  in pixel
  int    iHeight;			// image height in pixel
  int    iBinning;			// NOBINNING (to be defined), BIN2X2, BIN4X4
  
  struct {
     unsigned   bPartialScan    : 1;	// mode supports partialscan
     unsigned   bProgressiveScan: 1;	// mode supports prog.scan mode( or similar mode )
     unsigned   b8Bit           : 1;	// mode supports 8 Bit per pixelcanal
     unsigned   b16Bit		    : 1;	// mode supports 16Bit per pixelcanal
	 unsigned   bMonochrom	    : 1;	// mode supports monochrom images
     unsigned   bColor	        : 1;	// mode supports color     images
  } support;
} tDefineImageFormat, *ptDefineImageFormat;

int WINAPI BX_EnumerateImFormats( IN int cLabel, OUT int * piResCount, OUT ptDefineImageFormat * ppRes);
int WINAPI BX_EnumerateBmpImFormats( IN int cLabel, OUT int * piResCount, OUT ptDefineImageFormat * ppRes);


/*****************************************************************************\
Function:            BX_CommandStrToCam
Description:		 Testfunction for register i/o	
Parameters :
	<int cId>		 - cameralabel 
	<char * buf>	 - camera command
Return Value:
	char *    WINAPI
Notes:				Baumer internal use. Not recommend for external API user
See Also:
******************************************************************************
Created:             06.09.2002
Last modified:       06.09.2002
\*****************************************************************************/
char *    WINAPI BX_CommandStrToCam( int cId, char * buf  );


/*****************************************************************************\
Function:				BX_SetReadOutSpeed
Description:			change the read out speed for camers
Parameters :
	<int cLabel>		 - locical camera number
	<int iSpeedType>	 - type of read out speed
Return Value:
	int  
Notes:
See Also:				not supported by all types
******************************************************************************
Created:             08.04.2003
Last modified:       08.04.2003
\*****************************************************************************/
int  WINAPI BX_SetReadOutSpeed( int cId, eSCMDID iSpeedType );

/*****************************************************************************\
Function:            BX_DeInitLibrary
Description:		 DeInit internal multiprocessing classes	 
Parameters :
Return Value:
	DWORD  WINAPI
Notes:
See Also:
******************************************************************************
Created:             13.03.2002
Last modified:       13.03.2002
\*****************************************************************************/
DWORD  WINAPI BX_DeInitLibrary();


/**
\fn          DWORD WINAPI BX_WriteShadingTable( int cLabel, tpShading psShading )
\brief       Write a complete shading table down to the camera from an array that is
             specified by <psShading>    
\param       psShading		descriptor of shading data applied to raw datas
\retval      TRUE/FALSE
\note		 
\see     
\internal    Created:       06.10.2003
\internal    Last modified: 06.10.2003
*/
DWORD WINAPI BX_WriteShadingTable( IN int cLabel, IN tpShading psShading );

/**
\fn          DWORD WINAPI BX_ReadShadingTable( int cLabel, tpShading psShading )
\brief       Read the complete shading values from the camera in an array that is
             specified by <psShading>    
\param       psShading	descriptor of shading data applied to raw datas
\retval      TRUE/FALSE
\note		 
\see     
\internal    Created:       06.10.2003
\internal    Last modified: 06.10.2003
*/
DWORD WINAPI BX_ReadShadingTable( IN int cLabel, OUT tpShading  psShading );

/**
\fn          DWORD WINAPI BX_SetShadingTable( int cLabel, tpShading psShading )
\brief       Enable/disable shading functionality
\param       enable			enable/disable shading functionality, with -1 only the 
                            the parameter shading is respected
\param       shading        pointer to info of a linear io relationship 
\retval      TRUE/FALSE 
\note		 
\see     
\internal    Created:       07.10.2003
\internal    Last modified: 07.10.2003
*/
DWORD WINAPI BX_SetShadingTable( IN int cLabel, IN int enable, IN OUT tpSHD shading  );


typedef struct _BOBXCAM_INFO {
	///< identification
	unsigned long CamType;		    ///< 16 Bit used, hardware id of camera   
	unsigned long VendorID;		    ///< 24 Bit used, baumer ieee id 0x0006BE
	unsigned long ChipIDHi;		    ///< 16 Bit used, serial number: order number
	unsigned long ChipIDLo;		    ///< 16 Bit used, serial number: week | year
	char 	 	  FpgaVersion[32];	///< null terminated ansii string of camera fpga version
	///< label
	unsigned long CamLabel;		    ///< 32 Bit used, label as logical camera id
	char *		  CamName;		    ///< null terminated ansii string
	
} tBOBXCAM_INFO, * tpBOBXCAM_INFO;

/**
\fn          DWORD WINAPI BX_DeviceInfo( int iDevIndex,  tpBOFXCAM_INFO pDevEnum  )
\brief       Get Baumer Optronic device hardware and logical information (device identifier, Baumer IEEE ID,
			 serial number 8 Bit high part and 32 Bit low part, camera label)
\param       <cLabel>    device index
\param       <pDevInfo>  pointer for descriptor of device
\retval      TRUE/FALSE/eFXCERROR
\note		 
\see    
\internal    Created:       10.10.2003
\internal    Last modified: 10.10.2003
*/
DWORD WINAPI BX_DeviceInfo( int cLabel,  tpBOBXCAM_INFO pDevInfo  );   

#ifdef __cplusplus
}
#endif
#endif // _VIDEO_API_