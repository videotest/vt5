/*!  \file
Basic definitions for the <b> MexAPI. </b>

\version 2.5
\author Wolfgang Radecke Jenoptik, Laser Optik, Systeme GmbH \n
\date 2004/10/06 Ersterstellung

\date 2004/11/25
- New enumeration ::meCOMPONENT added. 

\date 2005/01/28
- Description of meInfo added.
*/

#ifndef _MEBASE_H
#define _MEBASE_H

#ifndef XXCODES
#define XXCODES

//! result codes for functions of CxxPlus.dll
//!
#define NOERR                         0   //!< No error (OK).
#define DOWNLOADERR                   1   //!< Error while ::mexUpdateFirmware. 
#define INITERR                       2   //!< Error on initializing camera.
#define NOCAMERA                      3   //!< Camera not found.
#define ABORTERR                      4   //!< Image acquisition aborted.
#define WHITEERR                      5   //!< Invalid white point.
#define IMAGESIZEERR                  6   //!< Image size error (not used ).
#define NOMEMERR                      7   //!< Low memory on PC.
#define PARAMERR                      8   //!< Invalid calling parameter.
#define PIEZOCALBADIMAGE              9   //!< Bad image for piezo calibration.
#define CAMERABUSY                    10  //!< Camera busy.
#define CAMERANOTSTARTED              11  //!< Acquisition not started.
#define BLACKREFTOOBRIGHT             12  //!< Black reference to bright.
#define WHITEREFTOOBRIGHT             13  //!< White reference to bright.
#define WHITEREFTOODARK               14  //!< White reference to dark.
#define COLORCALBADVALUES             15  //!< Used for mexCalibrateColors.  
#define COLORCALOVEREXPOSED           16  //!< Not used.  
#define INVALIDCAMERATYPE             17  //!< Invalid camera type.
#define NOTIMPLEMENTED                18  //!< Not implemented. 
                                          
#define NOTSUPPORTED                  19  //!<  Function not supported for the requested camera. 
#define NODISKSPACE                   20  //!<  Low disc space available.
#define BLACKREFERENCE_NOTAVAILABLE   21  //!<  Black reference not availbale.
#define ACQUISITION_RUNNING           22  //!<  Acquisition already running.
#define FAST_ACQUISITION_RUNNING      23  //!<  Fast acquisition already running.
#define FALSECAMERA                   24  //!<  Invalid calling parameter with respect to camera.
#define IMAGETRANSFER_NOTOPEN         25  //!<  Image transfer not open.
#define TOOMANYCAMERASOPEN            29  //!<  To many cameras open allowed are only 4. 
#define CAMERAINLOADERMODE            30  //!<  Camera in loader mode.


//! Values for parameter status of user defined function of type ::mexImageFinalProc 
//!
#define IMAGE_READY         0          //!< Image is complete an OK, used as value of status in a call to ::mexImageFinalProc.
#define IMAGE_TRANSFER_ERR  1          //!< Imagetransfer failed, used as value of status in a call to ::mexImageFinalProc.
#define IMAGE_STARTAQC_ERR  2          //!< Start of image transfer failed, used as value of status in a call to ::mexImageFinalProc.
#define IMAGE_READY_BUTNOTEQUALIZED  3 //!< ImageReady but not equalized because out of range (for multi shot images only).
#define NOTIFY_TRIGGER_RELEASED      4 //!< External trigger released
#define NOTIFY_EXPOSURE_START        5 //!< Exposure start
#define NOTIFY_EXPOSURE_COMPLETE     6 //!< Exposure complete
#define NOTIFY_DATATRANSFER_COMPLETE 7 //!< Data transfer complete

//! Progress state for firmware update of camera.
//!
typedef enum
{
    XSTATUS_UNKNOWN                = 0,
    XCONNECT_CAMERA                = 1,
    XSTART_LOADER                  = 2,
    XREAD_LOADER_VERSION           = 3,
    XCHECK_SUBSIDARY_ID            = 4,
    XOLD_LOADER_VERSION            = 5,
    XUNKNOWN_LOADER_VERSION        = 6,
    XUNKNOWN_DEVICE                = 7,
    XUNKNOWN_FLASH_TYPE            = 8,
    XCHECK_UPDATE_FILE             = 9,
    XREAD_FLASH_ID                 = 10,
    XMARK_CAMERA_PROGRAM_INVALID   = 11,
    XEVALUATE_FLASH_ID             = 12,
    XREAD_LOADER                   = 13,
    XERASE_WHOLE_FLASH             = 14,
    XREWRITE_LOADER                = 15,
    XERASE_CAMERA_PROGRAM          = 16,
    XPREPARE_PROGRAMMING           = 17,
    XPERFORM_PROGRAMMING           = 18,
    XCOMPLETE_PROGRAMMING          = 19,
    XMARK_NEW_CAMERA_PROGRAM_VALID = 20,
    XRESTART_CAMERA_PROGRAM        = 21,
    XDISCONNECT_CAMERA             = 22,
    XUPDATE_FIRMWARE_OK            = 23,
} FIRMWARE_UPDATE_STATUS;

//! Enumeration for codes about connection events on FireWire-Bus 
//!
typedef enum meGEN_STATUS
{
	GEN_STATUS_1394_BUS_RESET=0,    //!< General 1394-bus reset.
	GEN_STATUS_DEVNODES_CHANGED=1,  //!< Other device connection state changed.
	GEN_STATUS_NEW_CAMERA=2,        //!< Camera physical reconnected.
	GEN_STATUS_CAMERA_REMOVED=3,    //!< Camera physical removed.
	GEN_STATUS_NEW_DEVICE=4,        //!< Other device reconnected.
	GEN_STATUS_DEVICE_REMOVED=5,    //!< Other device physical removed.
}meGEN_STATUS;

//! Enumeration for Fire Wire transfer packet size.
//!
typedef enum { 
	TRANS_MODE_ONE_CHANNEL_LARGE_PACKET = 0,
	TRANS_MODE_TWO_CHANNEL_LARGE_PACKET = 1,
	TRANS_MODE_ONE_CHANNEL_SMALL_PACKET = 2,
	TRANS_MODE_TWO_CHANNEL_SMALL_PACKET = 3
} meTRANS_MODE;

//! Enumeration for handling of treansfer errors.
//!
typedef enum XFERHANDLING
{
  abort_on_xferr =1,          //!< Bit 0 - Automatic call to mexAbortAcquisition after a transfer error.
  resurrect_live_on_xferr =2, //!< Bit 1 - Automatic resurrect live mode after a transfer error.
  disable_notify_snap =4,     //!< Bit 2 - Disable notifying a transfer error in case of a single snap.
  disable_notify_live =8,     //!< Bit 3 - Disbale notifying a transfer error in case of live mode.
};

#endif

//! \brief Enumeration for  camera  color type  
//!
typedef enum
{ meBW=0, //!< Black-white camera.
  meRGB=1 //!< Color camera.
} meCamType;

//! \brief Enumeration for camera type used by ::mexGetCameraType .
//!
typedef enum CamType
{
  cam10plus=0,   //!< <b> ProgresC10Plus </b>
  cam12plus=1,   //!< <b> ProgResC12Plus </b>
  cam14Cplus=2,  //!< <b> ProgResC14Plus </b> and \link page25 derived models. \endlink
  cam14Mplus=3,  //!< <b> ProgResM14Plus </b> and \link page25 derived models. \endlink
  camunknown=4,  //!< <b> unknown camera </b>
}CamType;


//! Structure for camera info compatible to C10.dll,c12.dll,c14.dll,C10Plus.dll. 
//! \note
//! - The structure is compiled with standard alignment from the Microsoft Visual-C++ enviorment.
//! - Compared to structures from Mexapi.h there are no #pragma pack(...) to control the alignment.
//! - This results in following offsets for the members with respect to the begin of the structure.
//!     - [0]	 0  <b> Revision </b>
//!     - [1]	 4  <b> SerialNumber </b>
//!     - [2]	12  <b> Type </b>
//!     - [3]	16  <b> Features </b>
//!     - [4]	20  <b> OEMtag </b>
//!     - [5]	24  <b> DriverVersionMajor </b>
//!     - [6]	25  <b> DriverVersionMinor </b>
//!     - [7]	26  <b> Day </b>
//!     - [8]	27  <b> Month </b>
//!     - [9]	28  <b> Year </b>
//! - If an application uses non standard alignment it must adjust  its own structure of type meInfo
//!   so that it matches the offsets above.
//! 
typedef struct meInfo
{ 
  long Revision;                  //!< currently alwaays 0.
  unsigned char SerienNummer[6];  //!< binary serial number of camera.
  meCamType Type;                 //!< type of camera.
  long   Features;                //!< currently always 0.
  long   OEMtag;                  //!< unused, always -1.
  unsigned char  DriverVersionMajor; //!< Major version number of firmware of camera.
  unsigned char  DriverVersionMinor; //!< Minor version number of firmware of camera.
  unsigned char  Day;                //!< Day in month  1,...,31.
  unsigned char  Month;              //!< Month in year 1,...,12
  unsigned short Year;               //!< year for instance 2005
} meInfo;

//! struct for date (calendar)
//!
typedef struct meDATE
{
	unsigned int Year;	//!< For instance 2001.
	unsigned int Month;	//!< 1...12.
	unsigned int Day;		//!< Day in month 1...31. 
} meDATE;

//! Enumeration for components of mexGetVersion
//!
typedef enum meCOMPONENT{
 meCOMP_CXXDLL=0,   //!< For version of this DLL.
 meCOMP_CAMCONJ=1,  //!< For version of camconj.dll
 meCOMP_FIRECAMJ=2, //!< For version of FireCamJ.dll
 meCOMP_FIREDRIV=3, //!< For version of FireCamJ.sys
 meCOMP_CAM1=4,     //!< For version of firmware of camera
 meCOMP_CAM2=5,      //!< for version of FPGA code of camera
}meCOMPONENT;

//! Structure for version of camera firmware.
//!
typedef struct meVERSION
{
	unsigned int Major;    //!< Major version number.
	unsigned int Minor;    //!< Minor version number.
	meDATE Date;		       //!< Creation date.
	char Text[128];        //!< Additional readable information.
} meVERSION;

//! struct for image info 
//!
typedef struct meImageInfo
{ 
	unsigned int DimX;						//!< Image width (pixel).                                 
	unsigned int DimY;						//!< Image height (pixel).                                
	unsigned int BitDeepth;				//!< Number of significat bits per pixel (<b> Sorry: </b> Deepth should be Depth but I cannot decide to correct the spelling).                    
	unsigned int PlaneBytes;			//!< Number of bytes per pixel  for one color channel (1 or 2).       
	unsigned int Channels;				//!< Number of color channels (1 or 3).                   
} meImageInfo;

//! Enumeration for Bayermask on ccd sensor: 
//! \note 
//! The description indicates the arrangement of colors on CCD-Sensor in the upper left
//! corner. The arrangement is periodic repeated over the whole sensor. Fore instance
//! meMaskGRBG is schematically given at the upper left corner by \n
//! \n
//! G | R     \n
//! B | G     \n
//! \n
//! Color green is twofold available against color red and color blue
//! The information about color mask is only useful for raw images (acquired by acquisition mode ::mexShot2F_Raw).
typedef enum
{
  meBlackWhite                  = 0,  //!< Black-white camera.

  meMaskGRBG		                = 1,  //!< Color mask   green  red blue green.

  meMaskRGGB		                = 2,  //!< Color mask   red  green green blue.

  meMaskGBRG		                = 3,  //!< Color mask   green  blue red green.

  meMaskBGGR		                = 4,  //!< Color mask   blue  green green red.
} meColorMask;


//! Structure for camera info. 
//!
typedef struct meCamInfo
{
  unsigned int SensorX; //!< Sensor width  (pixel).
  unsigned int SensorY; //!< Sensor height (pixel).
  unsigned int BitDepth;//!< Number of significant bits per color pixel.
  meColorMask ColorMask;//!< Code for bayer mask of sensor.
} meCamInfo;

//! Callback function for progress state of image acquisition .
//! <b> Notice explicitly the calling convention __cdecl in opposite to other
//! callback functions </b> (for compatibility to C10.dll, C12.dll, C14,dll)
//! \param done
//! Indicates that done step of total steps are complete.
//! \param total 
//! Indicates that total steps are necessary to complete the task.
//! \param dwData 
//! Generic value taken from ::mexImageParams::pImageproc
//! \note The return value is ignored in the actual implementation.
//!
typedef BOOL (__cdecl *mexImageProc)(long done,
                                    long total,
                                    DWORD dwData
                                    );


//! Callback function for completion state of images requested by one of the \ref page6 .
//! In case of status=::IMAGE_TRANSFER_ERR the color channel pointers  may be NULL. Hence the
//! callers image processing after a call must test the incoming values carefully.
//! \param status 
//! Value to indicate the completion state of the image peviously ordered by one of the \ref page6 .
//! For a table of all values see: \link page1b Notifications for image acquisition \endlink 
//! \param collorchannels
//! Array of 3 pointers to the color channels of the incoming image (in order blue,green,red).
//! In case of status=::IMAGE_TRANSFER_ERR the pointers may be NULL.
//! \param UserValue
//! The generic user value ::mexImageParams::UserValue from a call to a acquisition function.
//! \note In case of an image acquired by mexAcqMode::mexShot2F_Raw only the first channel 
//! <b> colorchannels[0] </b> is used. The other channels are set to NULL.
//!
//! \copydoc page18
typedef BOOL (__stdcall * mexImageFinalProc)(unsigned long status, // success code for incoming image
                                             void * colorchannels[3],      // color chanesl of incoming image order is blue,green,red
                                              unsigned long UserValue                // generic user value from caller
                                            );


//! Callback function to signalize <b> plug and play events </b> on the FireWire bus.
//! \note The callback reports events on the FireWire-Bus that affects changing connections to FireWire-Devices. 
//! Hence the value for CamGUID can be any identifier of a FireWire-Device. If the value is 0, the implementation
//! was unable to find out the identifier. Any incoming call of this callback should be handled so as if all 
//! FireWire devices are new arranged on the FireWire bus. After that a new establishment of the connection
//! between all cameras and the DLL is necessary.For examples of plug and play handling see 
//! ::InitCB, ::CxxHost::OnRemoved, CxxHost::OnReconnected and ::CxxHost::ReopenCamera in the source code of CxxWin.exe.
//! Note also the very lazy reaction of the operating system (Windows...) to plug and play events.
//!
//! \attention The handling of plug and play events for multiple camera is <b> highly sophisticated </b> both
//! on side of PC and on side of camera. For instance if one of the involved cameras is in  a running image acquisition
//! for instance CC-Live, AC-Live, the handling may fail. Hence do not strain this feature.
//! If something goes wrong unplug an plug in again the affected camera and restart your application to solve the problem.
//! By the way this handling is not importantly in the most application cases.
//! \param[in] status
//! Value taken from enum ::meGEN_STATUS to indicate the reason of the call.
//! \param[in] UserValue
//! Value taken from the parameter ::UserValue from an initial call to ::mexInit
//! \param CamGUID  Global unique identifier of the relevant FireWire device. The value 
//! may be <b> (unsigned __int64)0 </b> if the implementation was unable to find out the identifier.
typedef void (__stdcall* meInitCB)(unsigned long status,    
                                   unsigned long UserValue,
                                   unsigned __int64 CamGUID 
                                   );
//! Callback to report the state of the  calibration procedure for the piezo scanner,
//! \param[in] step Number of actual calibration step (starts with 1).
//! \param[in] all  Maximal steps ordered in a call of ::mexCalibrateScanner (taken from ::mexPiezoCalibSet::IterCount).
//! \param[in] DeviationX Maximal deviation of position in X-direction.
//! \param[in] DeviationY Maximal deviation of position in Y-direction.
//! \param[in] UserValue Value from call of ::mexCalibrateScanner.
//! \retval FALSE  If the procedure should be continued.
//! \retval TRUE   If the procedure should be aborted
//! \attention
//! - The callback function should return a <b> value !=0  if the caller would like abort the procedure </b>,
//! either because he is satisfied for the achieved precision or he would like abort for other reasons.
typedef int (__stdcall* mexPiezoCB)(int step,int all,double DeviationX,double DeviationY,unsigned long UserValue);

//! Callback to report the focus value for the actual acquired image
//! \param[in] focus A value that indicates the sharpness of the actual acquired image. High values indicate
//! high sharpness. The value -1 indicates an error (for instance if the focus rectangle does not fit into the image).
//! \param[in] FocusUser Generic user value to propagate on any call, The value is taken from the recent call 
//! of ::mexSetFocusCallback.
typedef void (__stdcall* mexFocusCB)(int focus, unsigned long FocusUser);

//! Callback to report the actual exposure time (camera ticks) if exposure control is activated by
//! ::mexActivateExposureControl.
//! \param[in] ticks atual used exposure time, units are camera ticks.
//! \param[in] ExpCtrlUser Gerneric user value to propagate on any call. The value is taken from the recent call
//! of ::mexActivateExposureControl.
typedef void (__stdcall* mexExpCtrlCB)(unsigned long ticks, unsigned long ExpCtrlUser);
#endif // _MEBASE_H