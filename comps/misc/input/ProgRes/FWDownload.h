// FWDownload.h
// declaration for firmware update of camera (download from PC to Camera) 
// see C10Plus-SDK source code for C10PlusWin.exe for  realization of
// firmware update.
//////////////////////////////////////////////////////////////////////////////

#ifndef _FWDOWNLOAD
#define _FWDOWNLOAD

//#define DLLEXPORT __declspec(dllimport)

#ifdef __cplusplus
 extern "C" {			/* allow use with C++ */
#endif

// progress state for firmware update of camera ///////////////////////////////////////////////////
typedef enum
{
  STATUS_UNKNOWN                =0,
  CONNECT_LOADER                =1,
  START_LOADER                  =2,
  READ_LOADER_VERSION           =3,
  READ_FLASH_ID                 =4,
  MARK_CAMERA_PROGRAM_INVALID   =5,
  EVALUATE_FLASH_ID             =6,
  READ_LOADER                   =7,
  ERASE_WHOLE_FLASH             =8,
  REWRITE_LOADER                =9,
  ERASE_CAMERA_PROGRAM          =10,
  PREPARE_PROGRAMMING           =11,
  PERFORM_PROGRAMMING           =12,
  COMPLETE_PROGRAMMING          =13,
  MARK_NEW_CAMERA_PROGRAM_VALID =14,
  RESTART_CAMERA_PROGRAM        =15,
  DISCONNECT_CAMERA             =16,
	UPDATE_FIRMWARE_OK            =17,
	INITIALIZING                  =18,
	READY_TO_WORK                 =19,
} FIRMWARE_UPDATE_STATUS;


// struct for version of camera firmware //////////////////////////////////////////////////////////
typedef struct tagFirmwareVersion
{
	long Major;
	long Minor;
	long Year;
	long Month;
	long Day;
} FirmwareVersion;



// function to perform download of camera firmware from PC to camera///////////////////////////////

C10PLUS_SPEC long  WINAPI meDownloadFirmware( meImageProc pCallBack,unsigned long UserValue);



// returns in *pFWV  verwsion of camera firmware coded in DLL-resource data
C10PLUS_SPEC long  WINAPI meGetFirmwareVersionFromData(FirmwareVersion* pFWV);
																	

// returns in *pFWV version of firmware from currently connected camera
C10PLUS_SPEC long  WINAPI meGetFirmwareVersionFromCamera(FirmwareVersion* pFWV);

// returns in *pString a textual description of camera type coded in DLL-resource data
C10PLUS_SPEC long  WINAPI meGetProductStringFromData(char* pString, unsigned int stringlength);

// returns in *pString a textual description of camera type from connected camera
C10PLUS_SPEC long  WINAPI meGetProductStringFromCamera(char* pString, unsigned int stringlength);
////////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
	}
#endif

#endif