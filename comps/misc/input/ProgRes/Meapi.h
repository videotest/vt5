#ifndef _MEAPI
#define _MEAPI

#ifdef __cplusplus
 extern "C" {			/* allow use with C++ */
#endif

#if defined C10PLUS_EXPORTS
#define C10PLUS_SPEC __declspec(dllexport)
#else
#define C10PLUS_SPEC __declspec(dllimport)
#endif

// enumeration for codes about connection events on FireWire-Bus //////////////////////////////////
typedef enum meGEN_STATUS
{
	GEN_STATUS_1394_BUS_RESET=0,    // general 1394-bus reset
	GEN_STATUS_DEVNODES_CHANGED=1,  // other device connection state changed
	GEN_STATUS_NEW_CAMERA=2,        // camera physical reconnected
	GEN_STATUS_CAMERA_REMOVED=3,    // camera physical removed
	GEN_STATUS_NEW_DEVICE=4,        // other device reconnected
	GEN_STATUS_DEVICE_REMOVED=5,    // other device physical removed
  GEN_STATUS_ON_USER_ENTRY_GET_ONCONNECT=6, // client can get user entry on connect
  GEN_STATUS_ON_USER_ENTRY_SET_ONCONNECT=7, // client can set user entry on connect
  GEN_STATUS_ON_USER_ENTRY_GET_ONDISCONNECT=8, // client can get user entry on disconnect
  GEN_STATUS_ON_USER_ENTRY_SET_ONDISCONNECT=9, // client can set user entry on disconnect
}meGEN_STATUS;

typedef enum { 
	TRANS_MODE_ONE_CHANNEL_LARGE_PACKET = 0,
	TRANS_MODE_TWO_CHANNEL_LARGE_PACKET = 1,
	TRANS_MODE_ONE_CHANNEL_SMALL_PACKET = 2,
	TRANS_MODE_TWO_CHANNEL_SMALL_PACKET = 3
} meTRANS_MODE;

typedef enum XFERHANDLING
{
  abort_on_xferr =1, ///< Bit 0 - Automatic call to meAbortAcquisition after a transfer error.
  resurrect_live_on_xferr =2, ///< Bit 1 - Automatic resurrect live mode after a transfer error.
  disable_notify_snap =4, ///< Bit 2 - Disable notifying a transfer error in case of e singel snap
  disable_notify_live =8, ///< Bit 3 - Disbale notifying a transfer error in case of live mode
};

// struct for  camera type ////////////////////////////////////////////////////////////////////////
typedef enum
{ meBW=0,
  meRGB=1
} meCamType;


// struct for camera info compatible to C10.dll,c12.dll,c14.dll ///////////////////////////////////
typedef struct meInfo
{ long Revision;
  unsigned char SerienNummer[6];
  meCamType Type;
  long Features,OEMtag;
  unsigned char DriverVersionMajor,DriverVersionMinor;
  unsigned char Day,Month;
  unsigned short Year;
} meInfo;

// struct for date (calendar) /////////////////////////////////////////////////////////////////////
typedef struct meDATE
{
	unsigned int Year;	// for instance 2001
	unsigned int Month;	// 1...12
	unsigned int Day;		// day in month 1...31 
} meDATE;

// struct for version of camera firmware //////////////////////////////////////////////////////////
typedef struct meVERSION
{
	unsigned int Major;    // major version number
	unsigned int Minor;    // minor version number
	meDATE Date;		       // creation date
	char Text[128];        // additional readable information
} meVERSION;

// struct for image info //////////////////////////////////////////////////////////////////////////
typedef struct meImageInfo
{ 
	unsigned int DimX;						// image width                                 
	unsigned int DimY;						// image height                                
	unsigned int BitDeepth;				// number of bits per pixel                    
	unsigned int PlaneBytes;			// number of bytes for one color channel       
	unsigned int Channels;				// number of color channels                    
} meImageInfo;

// Enumeration for Bayermask on ccd sensor: comments describe the geometric ///////////////////////
// arrangement of colormask of the left top corner (2 x 2 pixel) of ccd sensor
// The mask is periodic in X and Y over whole sensor
typedef enum
{
  meBlackWhite                  = 0,  // black-white camera

  meMaskGRBG		                = 1,  // color mask   green  red
                                      //              blue  green 

  meMaskRGGB		                = 2,  // color mask   red  green
                                      //              green  blue

  meMaskGBRG		                = 3,  // color mask   green  blue
                                      //              red  green

  meMaskBGGR		                = 4,  // color mask   blue  green
                                      //              green  red
} meColorMask;


// struct for camera info /////////////////////////////////////////////////////////////////////////
typedef struct meCamInfo
{
  unsigned int SensorX; // Sensor width  (pixel)
  unsigned int SensorY; // Sensor height (pixel)
  unsigned int BitDepth;// Number of significant bits per color pixel
  meColorMask ColorMask;// code for bayer mask of sensor
} meCamInfo;

#ifndef _MEIMAGEPARAMS
#define _MEIMAGEPARAMS

// enumeration for acquisition mode (scan mode) ///////////////////////////////////////////////////
typedef enum meScanMode	
{
  meSingleShotRaw=0,		              // raw data image (copy of sensor), up to 2080 x 1542
  meSingleShotHighFrameRateCol,	      // fast color image                 up to  346 x  256
  meSingleShotHighFrameRateColLarge,  // fast color image                 up to  692 x  512
  meSingleShotlowqualitycol,	  // low  quality interpolated color image  up to 2080 x 1542
  meSingleShothighqualitycol,	  // high quality  interpolated color image up to 2080 x 1542
  meSingleShotbw,							        // black/white image, up to 1040x770, not implemented
  meSingleShotbin2col,		            // 2-fold binning color image       up to 1040 x 770
  meSingleShotbin3col,		            // 3-fold binning color image       up to  692 x 514
  meSingleShotbin4col,		            // 4-fold binning color image       up to  520 x 384
  meSingleShotbin5col,		            // 5-fold binning color image       up to  416 x 308
} meScanMode;			

#endif

// Callback function for progress state of image acquisition //////////////////////////////////////
#ifndef _MEIMAGEPROC
#define _MEIMAGEPROC
typedef BOOL (__cdecl *meImageProc)(long done, long total, DWORD dwData);
// notice explicitly the calling convention __cdecl in opposite to other
// callback functions (for compatibility to C10.dll, C12.dll, C14,dll)
#endif


// Callback function for final state of image acquisition /////////////////////////////////////////
#ifndef _MEIMAGEFINALPROC
#define _MEIMAGEFINALPROC
typedef BOOL (__stdcall * meImageFinalProc)(unsigned long status,
                                            unsigned short* colorchannels[3], // order is blue,green,red
                                            unsigned long UserValue);
#endif


// Callback function to signalize connection events on FireWire-Bus /////////////////////////////// 
#ifndef _MEINITCALLBACK
#define _MEINITCALLBACK

typedef void (__stdcall* meInitCB)(unsigned long status,unsigned long UserValue, 
                                   unsigned __int64 CamGUID);
#endif

// include function for updating firmware of camera ///////////////////////////////////////////////
#include "FWDownload.h"

// struct for image acquisition ///////////////////////////////////////////////////////////////////
typedef struct meImageParams 
{ 
  meScanMode mode;			// Scanmode
  RECT rcimageBounds;		// Auf Sensoraufloesung (2080x1542) bezogene ROI.
  meImageFinalProc pImgFinalProc; // if image is ready Dll will call this Callback
																	// client of DLL can processing color channels
																	// per own ideas
	unsigned long UserValue;
  long exposureTime;		// Exposuretime [camera units] ( 1 camera unit = 186.2us). 
												// maximal value  is 180s respectively 966702 camera units.
  BOOL buseblackref;		// perform dark correction with black reference
  BOOL busewhiteref;		// perform  shading correction with white referenc
  BOOL bwhitebalance;		// perform white balance
  BOOL bcorrectColors;		// perform color transformation with standard matrix
  meImageProc pImageproc;	// Progress Callbackroutine, may be NULL
  DWORD dwData;						// User Data, 
  long multiShotDelay;		// not used
} meImageParams;
		

// result codes for functions of C10Plus.dll //////////////////////////////////////////////////////	
#ifndef _MEERRORCODES	
#define _MEERRORCODES

#define NOERR                         0 // no error (OK)
#define DOWNLOADERR                   1 // Fehler beim Initialisieren der Kamera
#define INITERR                       2 // error on initializing camera
#define NOCAMERA                      3 // camera not found
#define ABORTERR                      4 // image acquisition aborted
#define WHITEERR                      5 // invalid white point
#define IMAGESIZEERR                  6 // image size error (not used for C10Plus) 
#define NOMEMERR                      7 // low memory on PC
#define PARAMERR                      8 // invalid calling parameter
#define PIEZOCALBADIMAGE              9 // not used for C10Plus.dll
#define CAMERABUSY                    10// camera busy
#define CAMERANOTSTARTED              11// acquisition not started
#define BLACKREFTOOBRIGHT             12// black reference to bright
#define WHITEREFTOOBRIGHT             13// white reference to bright
#define WHITEREFTOODARK               14// white reference to dark
#define COLORCALBADVALUES             15// not used for C10Plud.dll
#define COLORCALOVEREXPOSED           16// not used for C10Plud.dll
#define INVALIDCAMERATYPE             17// invalid camera type
#define NOTIMPLEMENTED                18// Not implemented for C10

#define NOHOMEFOLDER                  19// not used for C10Plus.dll 
#define NODISKSPACE                   20// low disc space available
#define BLACKREFERENCE_NOTAVAILABLE   21// black reference not availbale
#define ACQUISITION_RUNNING           22// acquisition already running
#define FAST_ACQUISITION_RUNNING      23// fast acquisition already running
#define CAMERAINLOADERMODE            30// camera in loader mode

#endif

// states for user defined function of type meImageFinalProc //////////////////////////////////////
#ifndef _MEIMGSTATUSCODES
#define _MEIMGSTATUSCODES

#define IMAGE_READY         0
#define IMAGE_TRANSFER_ERR  1
#define IMAGE_STARTAQC_ERR  2

#endif 

// exported functions of C10Plus.dll ///////////////////////////////////////////////////////////////

// initialize C10Plus.dll an camera
C10PLUS_SPEC long WINAPI meInit(meInitCB pCB,unsigned long UserValue);

// initialize C10Plus.dll and camera with FireWire-GUID  CameraGUID
C10PLUS_SPEC long WINAPI meInitEx(unsigned __int64 CameraGUID,meInitCB pCB,unsigned long UserValue);

// finished C10Plus.dll and disconnect camera
C10PLUS_SPEC void WINAPI meClose();

// disconnect only camera but doesn't finish C10Plud.dll
C10PLUS_SPEC void WINAPI meDisconnect();

// disconnect camera with FireWire-GUID  CameraGUID (if connected before)
C10PLUS_SPEC void WINAPI meDisconnectEx(unsigned __int64 CameraGUID);

// returns FireWire-GUID of currently connected camera
C10PLUS_SPEC long WINAPI meGetCurrentCamGuid(unsigned __int64* pGuid);

// returns info from currently connected camera (compatible to C10.dll,C12.dll,C14.dll)
C10PLUS_SPEC long WINAPI meGetInfo(meInfo *pmeInfo);

// returns camera info of currently connected camera (C10Plus-specific)
C10PLUS_SPEC long WINAPI meGetCameraInfo(meCamInfo *pCamInfo);

// returns image info for image ordered by *pImgPars
C10PLUS_SPEC long WINAPI meGetImageInfo(meImageParams *ImgPars,meImageInfo* pImgInfo);

// acquire an image specified by *ImageParameters
C10PLUS_SPEC long WINAPI meAcquisition(meImageParams *ImageParameters);

// start fast image acquisition
C10PLUS_SPEC long WINAPI meStartFastAcquisition(meImageParams *ImageParameters);

// start image acquisition and take image memory from caller
C10PLUS_SPEC long WINAPI meStartAcquisition(meImageParams *ImageParameters,
                                     unsigned short* colorchannels[3],
                                     unsigned long MemsizePerChannel);

// abort started image acquisition
C10PLUS_SPEC long WINAPI meAbortAcquisition();

// returns state of running fast image acquisition
C10PLUS_SPEC long WINAPI meIsFastAcquisitionReady(BOOL continueAcquisition);

// Frees internal allocated image memory
C10PLUS_SPEC long WINAPI meFreeImageMemory();

// create an black reference 
C10PLUS_SPEC long WINAPI meGetBlackRef(meImageProc pCallBack, DWORD dwData);

// Get length for memory to save black reference
C10PLUS_SPEC long WINAPI meGetBlackRefLength(unsigned int * pLength);

// create white reference
C10PLUS_SPEC long WINAPI meGetWhiteRef(meImageParams *ImageParameters);

// Get length of memory to save white reference
C10PLUS_SPEC long WINAPI meGetWhiteRefLength(unsigned int* pLength);

// Get white reference from C10Plus.dll created before by meGetWhiteRef(...)
C10PLUS_SPEC long WINAPI meGetWhiteTable(void *table);

// Set white reference to C10Plus.dll created before by meGetWhiteTable(...)
C10PLUS_SPEC long WINAPI meSetWhiteTable(void *table);

// Set  image white balance to C10Plus.dll 
C10PLUS_SPEC long WINAPI meSetImageWhiteBalance(double Red, double Green, double Blue);

// Set basic white balance to C10Plus.dll
C10PLUS_SPEC long WINAPI meSetWhiteBalance(double Red, double Green, double Blue);

// Get currently valid white balance from C10Plus.dll
C10PLUS_SPEC long WINAPI meGetCurrentWhiteBalance(double *pRed, double *pGreen, double *pBlue);

// Save black reference created by meGetBlackRef(...)
C10PLUS_SPEC long WINAPI meSaveBlackRef(void *ref);

// Set black reference to C10Plus.dll previous created by meSaveBlackRef
C10PLUS_SPEC long WINAPI meRestoreBlackRef(void *ref);

// find cameras currently connected to PC (identified by FireWire-GUID's)
C10PLUS_SPEC long WINAPI meFindCameras(unsigned int* pNumber, unsigned __int64 GUID_List[]);

// find cameras currently connected to PC by exteded conditions
C10PLUS_SPEC long WINAPI meFindCamerasEx(const unsigned __int64 GUID_Mask,
                                         unsigned int * pNumber, 
                                         unsigned __int64 GUID_List[]);

// Set Callback function whis is called on completion of image acqusition
C10PLUS_SPEC	long WINAPI meSetImageFinalProc(meImageFinalProc pIFP,unsigned long UserValue);

// modify acquisition parameter while fast acquisition is running (live mode)
// exposure time and constant size rect of interest
C10PLUS_SPEC	long WINAPI meModifyLiveParameters(long exposureTime,RECT* prcimageBounds);

// modify acquisition parameters while fast acquisition is running (live mode)
// Boolean flags  buseblackref,busewhiteref,bwhitebalance,bcorrectcolors in this order
C10PLUS_SPEC	long WINAPI meModifyLiveBools(BOOL b[4]);


// special function for multiple semantic.
// Semantic is controlled by code,
// parameters are taken from pV
C10PLUS_SPEC	long WINAPI meDispose(void* pV, unsigned long code);


#ifdef __cplusplus
	}
#endif

#endif
