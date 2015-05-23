/* CCApi.h */

#if !defined( DLLIMPORT )
  #if !defined( DLLEXPORT )
    // should not happen
    #define DLLINOUT
  #else
    #define DLLINOUT	__declspec( dllexport )
  #endif
#else
  #define DLLINOUT	__declspec( dllimport )
#endif

/* -----------------6/3/99 8:55AM--------------------
added cplusplus ...

 --------------------------------------------------*/
/* -----------------22/5/2002 11:58------------------
2.1
 added extended error handling
 --------------------------------------------------*/
#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif /* __cplusplus */

typedef enum {
	CC_NO_TRIGGER = 0x0,
	CC_INTERFACE_TRIGGER_SINGLE,		// if interface card supports trigger input
	CC_INTERFACE_TRIGGER_CONTINUOUS,
	CC_CAMERA_TRIGGER_SINGLE,		// if camera supports trigger input
	CC_CAMERA_TRIGGER_CONTINUOUS,
	CC_CAMERA_CONTINUOUS
	} CC_TRIGGER_MODE ;

typedef enum {
	CC_CAPTURE_WAIT = 0x0,
	CC_CAPTURE_NOWAIT,
	} CC_CAPTURE_MODE ;

typedef enum {
	CC_PAR_INTEGRATION_TIME = 0x0,
	CC_PAR_GAIN,
	CC_PAR_OFFSET,
	CC_PAR_DATA_MODE,
	CC_PAR_PIXEL_PRECHARGE,
	CC_PAR_CAMERA_MODE,
	CC_PAR_YSTART,
	CC_PAR_YINC,
	CC_PAR_YEND,
	CC_PAR_XSTART,
	CC_PAR_XINC,
	CC_PAR_XEND,
	CC_PAR_PWM,
	CC_PAR_ADC_DELAY,
	CC_PAR_Y_DELAY,
	CC_PAR_X_DELAY1,
	CC_PAR_X_DELAY2,
	CC_PAR_SENSOR_RESET,
	CC_PAR_SENSOR_CALIBRATE,
	CC_PAR_CORRECTION_MODE,
	CC_PAR_ANAVAL0,
	CC_PAR_ANAVAL1,
	CC_PAR_ANAVAL2,
	CC_PAR_ANAVAL3,
	CC_PAR_ANAVAL01P,
	CC_PAR_ANAVAL23P,
	CC_PAR_CTRLBIT,
	CC_PAR_YPARK,
	CC_PAR_XPARK,
	CC_PAR_LINE_CNT,
	CC_PAR_LINE_TIME,
	CC_PAR_ANASEL,
	CC_PAR_GAIN_RED,
	CC_PAR_GAIN_GREEN,
	CC_PAR_GAIN_BLUE,
	CC_PAR_NLIST_OPS,
	CC_PAR_RESET_SM ,
	CC_PAR_INTERFACE_FLASH_DELAY,
	CC_PAR_INTERFACE_FLASH_WIDTH,
	CC_PAR_INTERFACE_FLASH_SETTINGS,
	CC_PAR_SKIP_XNCS_WAIT,
	CC_PAR_OFFSET_B_FINE,
	CC_PAR_INTSEL,
	CC_PAR_CAMERA_FLASH_DELAY,
	CC_PAR_CAMERA_FLASH_WIDTH,
	CC_PAR_CAMERA_FLASH_SETTINGS,
	CC_PAR_OFFSET_RED,
	CC_PAR_OFFSET_GREEN,
	CC_PAR_OFFSET_BLUE,
	} CC_PARAMETER ;

enum CC_SELDAC {
	DAC0 = 0x0 ,
	DAC1 = 0x1000 ,
	DAC2 = 0x2000 ,
	DAC3 = 0x3000 ,
	DAC4 = 0x4000 ,
	DAC5 = 0x5000 ,
	DAC6 = 0x6000 ,
	DAC7 = 0x7000 ,
	DAC8 = 0x8000 ,
	DAC9 = 0x9000 ,
	DAC10= 0xa000 ,
	DAC11= 0xb000 ,
	DAC12= 0xc000 ,
	DAC13= 0xd000 ,
	DAC14= 0xe000 ,
	DAC15= 0xf000 ,
 	} ;


typedef enum {
	CC_CAMERA_NORMAL = 0x0,
	CC_CAMERA_DIAG_X,
	CC_CAMERA_DIAG_Y,
	CC_CAMERA_DIAG_X_XOR_Y,
	 } CC_CAMERA_MODE ;

typedef enum {
	CC_WOI_LEFTTOP_RIGHTBOTTOM = 0x0,
	CC_WOI_LEFTTOP_WIDTHHEIGHT,
	CC_WOI_CENTER_WIDTHHEIGHT,
	CC_WOI_RIGHTBOTTOM_WIDTHHEIGHT
	} CC_WOI_MODE ;

typedef enum {
	CC_CORRECTION_OFF = 0x0,
	CC_CORRECTION_FPN,
	CC_CORRECTION_PRNU,
	CC_CORRECTION_BAD_PIX,
	CC_CORRECTION_CORR_DATA,
	 } CC_CORRECTION_MODE ;

typedef enum {
	CC_DATA_8BIT_7_DOWNTO_0 = 0x0,
	CC_DATA_8BIT_8_DOWNTO_1,
	CC_DATA_8BIT_9_DOWNTO_2,
	CC_DATA_8BIT_11_DOWNTO_4,
	CC_DATA_16BIT_9_DOWNTO_0,
	CC_DATA_16BIT_11_DOWNTO_2,
	CC_DATA_16BIT_11_DOWNTO_0,
	CC_DATA_8BIT_10_DOWNTO_3,
	 } CC_DATA_MODE ;

typedef enum {
	CC_ERROR_NOTIFICATION_HANDLED_BY_APPLICATION = 0 ,
	CC_ERROR_NOTIFICATION_HANDLED_BY_CALLBACK ,
	CC_ERROR_NOTIFICATION_HANDLED_BY_API_MESSAGEBOX
	} CC_ERROR_NOTIFICATION ;


typedef enum {
	CC_CAMERA_STATUS_CC = 0 ,
	CC_CAMERA_STATUS_ASTAT
	} CC_CAMERA_STATUS ;

typedef enum {
	// this enum will not be used as such, because all values can be ored
	// but we preferred to group it this way
	CC_FLASH_SETTINGS_DAISY = 0x80 ,
	CC_FLASH_SETTINGS_INVERT_ARMIN = 0x01 ,
	CC_FLASH_SETTINGS_INVERT_TRIGGERIN = 0x02,
	CC_FLASH_SETTINGS_INVERT_ARMEDOUT = 0x04 ,
	CC_FLASH_SETTINGS_INVERT_FLASHOUT = 0x08
	} CC_FLASH_SETTINGS ;


DLLINOUT  BOOL		WINAPI	CC_GetDriverVersion( HANDLE Cam , DWORD * pVersionNumber) ;
DLLINOUT  BOOL		WINAPI	CC_GetDLLVersion( DWORD * pVersionNumber ) ;
DLLINOUT	BOOL 	WINAPI	CC_RegisterErrorHandler( HANDLE Cam , CC_ERROR_NOTIFICATION operation , void (* CallbackFunction)( int )) ;

DLLINOUT	HANDLE 	WINAPI	CC_Open( LPCSTR CameraName , ULONG CameraNumber , CC_CAPTURE_MODE CaptureMode ) ;
DLLINOUT  BOOL		WINAPI	CC_Close( HANDLE Cam ) ;
DLLINOUT  BOOL		WINAPI	CC_LoadInterface( HANDLE Cam , char * FileSpec ) ;
DLLINOUT  BOOL		WINAPI	CC_LoadCamera(  HANDLE Cam , char * FileSpec ) ;
DLLINOUT	BOOL 	WINAPI	CC_GetPCIBoardInfo( ULONG CameraNumber, char * pBoardType, char * pBoardRev ) ;

DLLINOUT  BOOL		WINAPI	CC_CaptureSingle( HANDLE Cam , PVOID Buffer , ULONG TransferSize ,
										CC_TRIGGER_MODE TriggerMode , USHORT TimeOut ,
										OVERLAPPED * pUserOverlapped ) ;
DLLINOUT	BOOL		WINAPI	CC_CaptureContinuous( HANDLE Cam , PVOID Buffer , ULONG BufferSize ,
										ULONG NotificationInterval , ULONG FrameSize ,
										void (* CallbackFunction)( void ) , HANDLE UserEvent ,
										CC_TRIGGER_MODE TriggerMode , USHORT TimeOut ) ;
DLLINOUT	BOOL		WINAPI	CC_CaptureSequence( HANDLE Cam , PVOID Buffer , ULONG BufferSize ,
										ULONG NumberOfFrames , ULONG NotificationInterval ,
										ULONG FrameSize , void (* CallbackFunction)( void ) ,
										CC_TRIGGER_MODE TriggerMode , USHORT TimeOut ) ;
DLLINOUT	BOOL		WINAPI	CC_CaptureWait( HANDLE Cam ) ;
DLLINOUT	BOOL		WINAPI	CC_CaptureStatus( HANDLE Cam ) ;
DLLINOUT	BOOL 	WINAPI	CC_CaptureAbort( HANDLE Cam ) ;
DLLINOUT	BOOL 	WINAPI	CC_SetWOI( HANDLE Cam , USHORT XPar1 , USHORT YPar1 , USHORT XPar2 ,
										USHORT YPar2 , USHORT XInc ,
										USHORT YInc , CC_WOI_MODE WOIMode , ULONG *FrameSize ) ;
DLLINOUT  BOOL		WINAPI	CC_SetParameter( HANDLE Cam , CC_PARAMETER Par , ULONG Value ) ;
DLLINOUT  BOOL		WINAPI	CC_GetStatus( HANDLE Cam , ULONG * Status ) ;

DLLINOUT  BOOL		WINAPI	CC_GetCameraStatus( HANDLE Cam , CC_CAMERA_STATUS idx , ULONG * Status ) ;



/* not yet documented in CCAPI.pdf */
DLLINOUT	BOOL		WINAPI	CC_MemoryRead( HANDLE Cam, PVOID Buffer, ULONG Address, ULONG Size ) ;
DLLINOUT	BOOL		WINAPI	CC_MemoryWrite( HANDLE Cam, PVOID Buffer, ULONG Address, ULONG Size ) ;
DLLINOUT	BOOL		WINAPI	CC_CameraMemoryWrite( HANDLE Cam, PVOID Buffer, ULONG Address, ULONG Size ) ;
DLLINOUT	BOOL 	WINAPI	CC_SetRegister( HANDLE Cam , USHORT Register , ULONG Value ) ;
DLLINOUT	BOOL 	WINAPI	CC_GetRegister( HANDLE Cam , USHORT Register , ULONG * Value ) ;
DLLINOUT	BOOL	WINAPI	CC_GetHistogramData(	HANDLE Cam ,
												USHORT *usMin ,
												USHORT *usGravity ,
												USHORT *usMax,
												ULONG *ulValue );
DLLINOUT BOOL		WINAPI	CC_GetCameraNumber (  HANDLE Cam , DWORD * dwCameraNumber );



#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif       /* __cplusplus */

/* end of  ccapi.h */