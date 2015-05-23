#if !defined(SMX150_H)
#define SMX150_H


#define SMX150_API_VERSION "1.5"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define SMX150_API      _cdecl

#define MaxFrameSize    (1400*1024)


#if defined(__cplusplus)
extern "C" {
#endif

//
// Status of camera main stream (video)
//
typedef enum {
    CAMERA_STOP = 0,
    CAMERA_START = 1
} SMX150_CameraStatus;


typedef enum { 
    FREQ_40 = 0, FREQ_24, FREQ_20, FREQ_16, FREQ_13, FREQ_12, FREQ_10, FREQ_8, FREQ_6
} SMX150_Frequency;


//
// Frame info :
//
typedef struct _TFrameParams {
     int StartX,StartY; // start position
     int Width,Height;  // of frame
     int Decimation;    // 1,2,4 (,8 for color camera)
     int ColorDeep;     // 8 or 24
     BOOLEAN MirrorV;   // vertical
     BOOLEAN MirrorH;   // horizontal
} TFrameParams, *PFrameParams;


//
// info about camera
//
typedef struct _TCameraInfo {
    int SensorType;
    int MaxWidth;
    int MaxHeight;
    char DeviceName[64];
} TCameraInfo;


//
// extended info about camera
//
typedef struct _TCameraInfoEx {
    unsigned short HWModelID;
    unsigned short HWVersion;
    unsigned long HWSerial;
    unsigned char Reserved[5];
} TCameraInfoEx;


//
// RGB
//
typedef struct _TRgbPix {
   BYTE b;
   BYTE g;
   BYTE r;
} TRgbPix;


HANDLE SMX150_API CxOpenDevice(int DeviceId);
BOOL SMX150_API CxCloseDevice(HANDLE DeviceHandle);

BOOL SMX150_API CxGetCameraInfo  (HANDLE DeviceHandle, TCameraInfo   *CameraInfo );
BOOL SMX150_API CxGetCameraInfoEx(HANDLE DeviceHandle, TCameraInfoEx *CameraInfoEx);

BOOL SMX150_API CxGetStreamMode(HANDLE DeviceHandle, BYTE *StreamMode);
BOOL SMX150_API CxSetStreamMode(HANDLE DeviceHandle, BYTE  StreamMode);

BOOL SMX150_API CxGetScreenParams(HANDLE DeviceHandle, TFrameParams *Params);
BOOL SMX150_API CxSetScreenParams(HANDLE DeviceHandle, TFrameParams *Params);
BOOL SMX150_API CxActivateScreenParams(HANDLE DeviceHandle);

BOOL SMX150_API CxGetFrequency(HANDLE DeviceHandle,BYTE *Frequency);
BOOL SMX150_API CxSetFrequency(HANDLE DeviceHandle,BYTE  Frequency);

BOOL SMX150_API CxGetExposureMinMax(HANDLE DeviceHandle,int *MinExposure,int *MaxExposure);
BOOL SMX150_API CxGetExposure(HANDLE DeviceHandle,int *Exposure);
BOOL SMX150_API CxSetExposure(HANDLE DeviceHandle,int  Exposure);

BOOL SMX150_API CxGetExposureMinMaxMs(HANDLE DeviceHandle,float *MinExposure,float *MaxExposure);
BOOL SMX150_API CxGetExposureMs(HANDLE DeviceHandle,float *ExposureMs);
BOOL SMX150_API CxSetExposureMs(HANDLE DeviceHandle,float ExposureMs,float *ExposureSet);

BOOL SMX150_API CxGetGain(HANDLE DeviceHandle,int *Main,int *R,int *G,int *B);
BOOL SMX150_API CxSetGain(HANDLE DeviceHandle,int  Main,int  R,int  G,int  B);

BOOL  SMX150_API CxGrabVideoFrame(HANDLE DeviceHandle,void *Buffer,int BufSize);
PVOID SMX150_API CxGetFramePtr(HANDLE DeviceHandle);

BOOL SMX150_API CxSetBrightnessContrastGamma(HANDLE DeviceHandle,int B,int C,int G);
BOOL SMX150_API CxSetConvertionTab(HANDLE DeviceHandle, BYTE *Buf1024);
BOOL SMX150_API CxGetConvertionTab(HANDLE DeviceHandle, BYTE *Buf1024);
BOOL SMX150_API CxSetDefaultConvertionTab(HANDLE DeviceHandle);

BOOL SMX150_API CxStartVideo(HDC WindowDC,int DeviceId);
BOOL SMX150_API CxStopVideo(int DeviceId);
BOOL SMX150_API CxSetBayerAlg(int BayerAlg);

TRgbPix* SMX150_API CxBayerToRgb(void *BayerMatrix, int ScW, int ScH, int AlgId, TRgbPix *Frame);

BOOL SMX150_API CxGetFrameCounter(int DeviceId,long *FrameCounter);

BOOL SMX150_API CxGetDACRawOffset (HANDLE DeviceHandle, BYTE *Raw);
BOOL SMX150_API CxSetDACRawOffset (HANDLE DeviceHandle, BYTE  Raw);
BOOL SMX150_API CxGetDACFineOffset(HANDLE DeviceHandle, BYTE *Fine);
BOOL SMX150_API CxSetDACFineOffset(HANDLE DeviceHandle, BYTE  Fine);

BOOL SMX150_API CxGetSnapshotExposureMinMax(HANDLE DeviceHandle,int *MinExposure,int *MaxExposure);
BOOL SMX150_API CxGetSnapshotExposure(HANDLE DeviceHandle,int *SnapshotExp);
BOOL SMX150_API CxSetSnapshotExposure(HANDLE DeviceHandle,int  SnapshotExp);

BOOL SMX150_API CxGetSnapshotExposureMinMaxMs(HANDLE DeviceHandle,float *SnapshotExpMinMs,float *SnapshotExpMaxMs);
BOOL SMX150_API CxGetSnapshotExposureMs(HANDLE DeviceHandle,float *ExpMs);
BOOL SMX150_API CxSetSnapshotExposureMs(HANDLE DeviceHandle,float  ExpMs,float *ExpSet);

BOOL SMX150_API CxSetSnapshotExposureMultiplier(HANDLE DeviceHandle, DWORD Val);
BOOL SMX150_API CxGetSnapshotExposureMultiplier(HANDLE DeviceHandle, DWORD *Val);

BOOL SMX150_API CxGetSnapshot(HANDLE DeviceHandle,int Timeout,BOOL ExtTrgEnabled,BOOL ExtTrgPositive,BOOL NoWaitEvent,void *Buffer,int BufSize,DWORD *RetLen);
BOOL SMX150_API CxCancelSnapshot(HANDLE DeviceHandle);

BOOL SMX150_API CxSetContinuousPrecharge(HANDLE DeviceHandle, BOOL  ContinuousPrecharge);
BOOL SMX150_API CxGetContinuousPrecharge(HANDLE DeviceHandle, BOOL *ContinuousPrecharge);

BOOL SMX150_API CxSetTristateOut(HANDLE DeviceHandle, BOOL  TristateOut);
BOOL SMX150_API CxGetTristateOut(HANDLE DeviceHandle, BOOL *TristateOut);


BOOL SMX150_API CxWriteSensorReg(HANDLE DeviceHandle, DWORD RegNo, DWORD  Value);
BOOL SMX150_API CxReadSensorReg (HANDLE DeviceHandle, DWORD RegNo, DWORD *Value);
                                
BOOL SMX150_API CxSetControlReg (HANDLE DeviceHandle, BYTE  Value, BYTE Mask, BYTE ControlRegNo);
BOOL SMX150_API CxGetControlReg (HANDLE DeviceHandle, BYTE *Value,            BYTE ControlRegNo);


#if defined(__cplusplus)
}
#endif



#if defined(__cplusplus) // C++ stuff

typedef HANDLE   (*CxOpenDevice_t)(int DeviceId);
typedef BOOL     (*CxCloseDevice_t)(HANDLE DeviceHandle);

typedef BOOL     (*CxGetCameraInfo_t)(HANDLE DeviceHandle, TCameraInfo   *CameraInfo );
typedef BOOL     (*CxGetCameraInfoEx_t)(HANDLE DeviceHandle, TCameraInfoEx *CameraInfoEx);

typedef BOOL     (*CxGetStreamMode_t)(HANDLE DeviceHandle, BYTE *StreamMode);
typedef BOOL     (*CxSetStreamMode_t)(HANDLE DeviceHandle, BYTE  StreamMode);

typedef BOOL     (*CxGetScreenParams_t)(HANDLE DeviceHandle, TFrameParams *Params);
typedef BOOL     (*CxSetScreenParams_t)(HANDLE DeviceHandle, TFrameParams *Params);
typedef BOOL     (*CxActivateScreenParams_t)(HANDLE DeviceHandle);

typedef BOOL     (*CxGetFrequency_t)(HANDLE DeviceHandle,BYTE *Frequency);
typedef BOOL     (*CxSetFrequency_t)(HANDLE DeviceHandle,BYTE  Frequency);

typedef BOOL     (*CxGetExposureMinMax_t)(HANDLE DeviceHandle,int *MinExposure,int *MaxExposure);
typedef BOOL     (*CxGetExposure_t)(HANDLE DeviceHandle,int *Exposure);
typedef BOOL     (*CxSetExposure_t)(HANDLE DeviceHandle,int  Exposure);

typedef BOOL     (*CxGetExposureMinMaxMs_t)(HANDLE DeviceHandle,float *MinExposure,float *MaxExposure);
typedef BOOL     (*CxGetExposureMs_t)(HANDLE DeviceHandle,float *ExposureMs);
typedef BOOL     (*CxSetExposureMs_t)(HANDLE DeviceHandle,float ExposureMs,float *ExposureSet);

typedef BOOL     (*CxGetGain_t)(HANDLE DeviceHandle,int *Main,int *R,int *G,int *B);
typedef BOOL     (*CxSetGain_t)(HANDLE DeviceHandle,int  Main,int  R,int  G,int  B);

typedef BOOL     (*CxGrabVideoFrame_t)(HANDLE DeviceHandle,void *Buffer,int BufSize);
typedef PVOID    (*CxGetFramePtr_t)(HANDLE DeviceHandle);

typedef BOOL     (*CxSetBrightnessContrastGamma_t)(HANDLE DeviceHandle,int B,int C,int G);
typedef BOOL     (*CxSetConvertionTab_t)(HANDLE DeviceHandle,BYTE *Buf1024);
typedef BOOL     (*CxGetConvertionTab_t)(HANDLE DeviceHandle,BYTE *Buf1024);
typedef BOOL     (*CxSetDefaultConvertionTab_t)(HANDLE DeviceHandle);

typedef BOOL     (*CxStartVideo_t)(HDC WindowDC,int DeviceId);
typedef BOOL     (*CxStopVideo_t)(int DeviceId);

typedef BOOL     (*CxSetBayerAlg_t)(int BayerAlg);
typedef TRgbPix* (*CxBayerToRgb_t)(void *BayerMatrix, int ScW, int ScH, int AlgId, TRgbPix *Frame);

typedef BOOL     (*CxGetFrameCounter_t)(int DeviceId,long *FrameCounter);

typedef BOOL     (*CxGetSnapshotExposureMinMax_t)(HANDLE DeviceHandle,int *MinExposure,int *MaxExposure);
typedef BOOL     (*CxGetSnapshotExposure_t)(HANDLE DeviceHandle,int *SnapshotExp);
typedef BOOL     (*CxSetSnapshotExposure_t)(HANDLE DeviceHandle,int  SnapshotExp);

typedef BOOL     (*CxGetSnapshotExposureMinMaxMs_t)(HANDLE DeviceHandle,float *SnapshotExpMinMs,float *SnapshotExpMaxMs);
typedef BOOL     (*CxGetSnapshotExposureMs_t)(HANDLE DeviceHandle,float *ExpMs);
typedef BOOL     (*CxSetSnapshotExposureMs_t)(HANDLE DeviceHandle,float  ExpMs,float *ExpSet);

typedef BOOL     (*CxSetSnapshotExposureMultiplier_t)(HANDLE DeviceHandle, DWORD Val);
typedef BOOL     (*CxGetSnapshotExposureMultiplier_t)(HANDLE DeviceHandle, DWORD *Val);

typedef BOOL     (*CxGetSnapshot_t)(HANDLE DeviceHandle,int Timeout,BOOL ExtTrgEnabled,BOOL ExtTrgPositive,BOOL NoWaitEvent,void *Buffer,int BufSize,DWORD *RetLen);
typedef BOOL     (*CxCancelSnapshot_t)(HANDLE DeviceHandle);

typedef BOOL     (*CxGetDACRawOffset_t) (HANDLE DeviceHandle, BYTE *Raw);
typedef BOOL     (*CxSetDACRawOffset_t) (HANDLE DeviceHandle, BYTE  Raw);
typedef BOOL     (*CxGetDACFineOffset_t)(HANDLE DeviceHandle, BYTE *Fine);
typedef BOOL     (*CxSetDACFineOffset_t)(HANDLE DeviceHandle, BYTE  Fine);

typedef BOOL	 (*CxSetContinuousPrecharge_t)(HANDLE DeviceHandle, BOOL  ContinuousPrecharge);
typedef BOOL	 (*CxGetContinuousPrecharge_t)(HANDLE DeviceHandle, BOOL *ContinuousPrecharge);

typedef BOOL	 (*CxSetTristateOut_t)(HANDLE DeviceHandle, BOOL  CxSetTristateOut);
typedef BOOL	 (*CxGetTristateOut_t)(HANDLE DeviceHandle, BOOL *CxGetTristateOut);

typedef BOOL	 (*CxWriteSensorReg_t)(HANDLE DeviceHandle, DWORD RegNo, DWORD  Value);                             
typedef BOOL	 (*CxReadSensorReg_t) (HANDLE DeviceHandle, DWORD RegNo, DWORD *Value);                             
                                                                                                       
typedef BOOL	 (*CxSetControlReg_t) (HANDLE DeviceHandle, BYTE  Value, BYTE Mask, BYTE ControlRegNo);
typedef BOOL	 (*CxGetControlReg_t) (HANDLE DeviceHandle, BYTE *Value,            BYTE ControlRegNo);





class CSumixCam {
    
public:
        
    HINSTANCE m_scamlib;

    CSumixCam() {
        ResetFunctions();
//      Initialize();
    }

    virtual ~CSumixCam() {
        Shutdown();
    };

    CxOpenDevice_t                  CxOpenDevice;
    CxCloseDevice_t                 CxCloseDevice;
                                                               
    CxGetCameraInfo_t               CxGetCameraInfo;
    CxGetCameraInfoEx_t             CxGetCameraInfoEx;
                                                               
    CxGetStreamMode_t               CxGetStreamMode;
    CxSetStreamMode_t               CxSetStreamMode;
                                                               
    CxGetScreenParams_t             CxGetScreenParams;
    CxSetScreenParams_t             CxSetScreenParams;
    CxActivateScreenParams_t        CxActivateScreenParams;
                                                               
    CxGetFrequency_t                CxGetFrequency;
    CxSetFrequency_t                CxSetFrequency;
                                                               
    CxGetExposureMinMax_t           CxGetExposureMinMax;
    CxGetExposure_t                 CxGetExposure;
    CxSetExposure_t                 CxSetExposure;
                                                               
    CxGetExposureMinMaxMs_t         CxGetExposureMinMaxMs;
    CxGetExposureMs_t               CxGetExposureMs;
    CxSetExposureMs_t               CxSetExposureMs;
                                                               
    CxSetGain_t                     CxSetGain;
    CxGetGain_t                     CxGetGain;
                                                               
    CxGrabVideoFrame_t              CxGrabVideoFrame;
    CxGetFramePtr_t                 CxGetFramePtr;
                                                               
    CxSetBrightnessContrastGamma_t  CxSetBrightnessContrastGamma;
    CxSetConvertionTab_t            CxSetConvertionTab;
    CxGetConvertionTab_t            CxGetConvertionTab;
    CxSetDefaultConvertionTab_t   CxSetDefaultConvertionTab;
                                                               
    CxStartVideo_t                  CxStartVideo;
    CxStopVideo_t                   CxStopVideo;
    CxSetBayerAlg_t                 CxSetBayerAlg;
    CxBayerToRgb_t                  CxBayerToRgb;

    CxGetFrameCounter_t             CxGetFrameCounter;
                                                               
    CxGetSnapshotExposureMinMax_t   CxGetSnapshotExposureMinMax;
    CxGetSnapshotExposure_t         CxGetSnapshotExposure;
    CxSetSnapshotExposure_t         CxSetSnapshotExposure;
                                                               
    CxGetSnapshotExposureMinMaxMs_t CxGetSnapshotExposureMinMaxMs;
    CxGetSnapshotExposureMs_t       CxGetSnapshotExposureMs;
    CxSetSnapshotExposureMs_t       CxSetSnapshotExposureMs;

    CxSetSnapshotExposureMultiplier_t CxSetSnapshotExposureMultiplier;
    CxGetSnapshotExposureMultiplier_t CxGetSnapshotExposureMultiplier;
                                                               
    CxGetSnapshot_t                 CxGetSnapshot;
    CxCancelSnapshot_t				CxCancelSnapshot;

    CxGetDACRawOffset_t             CxGetDACRawOffset;
    CxSetDACRawOffset_t             CxSetDACRawOffset;
    CxGetDACFineOffset_t            CxGetDACFineOffset;
    CxSetDACFineOffset_t            CxSetDACFineOffset;

    CxSetContinuousPrecharge_t		CxSetContinuousPrecharge;
    CxGetContinuousPrecharge_t      CxGetContinuousPrecharge;

    CxSetTristateOut_t 				CxSetTristateOut;
    CxGetTristateOut_t				CxGetTristateOut;
                      
    CxWriteSensorReg_t              CxWriteSensorReg; 
    CxReadSensorReg_t               CxReadSensorReg; 
                                                  
    CxSetControlReg_t               CxSetControlReg; 
    CxGetControlReg_t               CxGetControlReg; 


    void Shutdown() {
        // unload the library
        if (m_scamlib != NULL) {
            ResetFunctions();
            ::FreeLibrary(m_scamlib);
            m_scamlib = NULL;
        }
    }

    BOOL Initialize() {
        // load the library
        m_scamlib = ::LoadLibrary("SMX150.dll");
    
        if (m_scamlib == NULL) {
            // AfxThrowFileException(CFileException::fileNotFound, -1, "SMX150.dll");
            return FALSE;
        };

        // init the functions as pointers from the dll

        CxOpenDevice = (CxOpenDevice_t)::GetProcAddress( m_scamlib, "CxOpenDevice" );
        CxCloseDevice = (CxCloseDevice_t)::GetProcAddress( m_scamlib, "CxCloseDevice" );
                                      
        CxGetCameraInfo = (CxGetCameraInfo_t)::GetProcAddress( m_scamlib, "CxGetCameraInfo" );
        CxGetCameraInfoEx = (CxGetCameraInfoEx_t)::GetProcAddress( m_scamlib, "CxGetCameraInfoEx" );
                                      
        CxGetStreamMode = (CxGetStreamMode_t)::GetProcAddress( m_scamlib, "CxGetStreamMode" );
        CxSetStreamMode = (CxSetStreamMode_t)::GetProcAddress( m_scamlib, "CxSetStreamMode" );
                                      
        CxGetScreenParams = (CxGetScreenParams_t)::GetProcAddress( m_scamlib, "CxGetScreenParams" );
        CxSetScreenParams = (CxSetScreenParams_t)::GetProcAddress( m_scamlib, "CxSetScreenParams" );
        CxActivateScreenParams = (CxActivateScreenParams_t)::GetProcAddress( m_scamlib, "CxActivateScreenParams" );
                                      
        CxGetFrequency = (CxGetFrequency_t)::GetProcAddress( m_scamlib, "CxGetFrequency" );
        CxSetFrequency = (CxSetFrequency_t)::GetProcAddress( m_scamlib, "CxSetFrequency" );
                                      
        CxGetExposureMinMax = (CxGetExposureMinMax_t)::GetProcAddress( m_scamlib, "CxGetExposureMinMax" );
        CxGetExposure = (CxGetExposure_t)::GetProcAddress( m_scamlib, "CxGetExposure" );
        CxSetExposure = (CxSetExposure_t)::GetProcAddress( m_scamlib, "CxSetExposure" );
                                      
        CxGetExposureMinMaxMs = (CxGetExposureMinMaxMs_t)::GetProcAddress( m_scamlib, "CxGetExposureMinMaxMs" );
        CxGetExposureMs = (CxGetExposureMs_t)::GetProcAddress( m_scamlib, "CxGetExposureMs" );
        CxSetExposureMs = (CxSetExposureMs_t)::GetProcAddress( m_scamlib, "CxSetExposureMs" );
                                      
        CxSetGain = (CxSetGain_t)::GetProcAddress( m_scamlib, "CxSetGain" );
        CxGetGain = (CxGetGain_t)::GetProcAddress( m_scamlib, "CxGetGain" );
                                      
        CxGrabVideoFrame = (CxGrabVideoFrame_t)::GetProcAddress( m_scamlib, "CxGrabVideoFrame" );
        CxGetFramePtr = (CxGetFramePtr_t)::GetProcAddress( m_scamlib, "CxGetFramePtr" );
                                      
        CxSetBrightnessContrastGamma = (CxSetBrightnessContrastGamma_t)::GetProcAddress( m_scamlib, "CxSetBrightnessContrastGamma" );
        CxSetConvertionTab = (CxSetConvertionTab_t)::GetProcAddress( m_scamlib, "CxSetConvertionTab" );
        CxGetConvertionTab = (CxGetConvertionTab_t)::GetProcAddress( m_scamlib, "CxGetConvertionTab" );
        CxSetDefaultConvertionTab = (CxSetDefaultConvertionTab_t)::GetProcAddress( m_scamlib, "CxSetDefaultConvertionTab" );
                                      
        CxStartVideo = (CxStartVideo_t)::GetProcAddress( m_scamlib, "CxStartVideo" );
        CxStopVideo = (CxStopVideo_t)::GetProcAddress( m_scamlib, "CxStopVideo" );
        CxSetBayerAlg = (CxSetBayerAlg_t)::GetProcAddress( m_scamlib, "CxSetBayerAlg" );
        CxBayerToRgb = (CxBayerToRgb_t)::GetProcAddress( m_scamlib, "CxBayerToRgb" );
                     
        CxGetFrameCounter = (CxGetFrameCounter_t)::GetProcAddress( m_scamlib, "CxGetFrameCounter" );
                              
        CxGetSnapshotExposureMinMax = (CxGetSnapshotExposureMinMax_t)::GetProcAddress( m_scamlib, "CxGetSnapshotExposureMinMax" );
        CxGetSnapshotExposure = (CxGetSnapshotExposure_t)::GetProcAddress( m_scamlib, "CxGetSnapshotExposure" );
        CxSetSnapshotExposure = (CxSetSnapshotExposure_t)::GetProcAddress( m_scamlib, "CxSetSnapshotExposure" );
                              
        CxGetSnapshotExposureMinMaxMs = (CxGetSnapshotExposureMinMaxMs_t)::GetProcAddress( m_scamlib, "CxGetSnapshotExposureMinMaxMs" );
        CxGetSnapshotExposureMs = (CxGetSnapshotExposureMs_t)::GetProcAddress( m_scamlib, "CxGetSnapshotExposureMs" );
        CxSetSnapshotExposureMs = (CxSetSnapshotExposureMs_t)::GetProcAddress( m_scamlib, "CxSetSnapshotExposureMs" );

        CxSetSnapshotExposureMultiplier = (CxSetSnapshotExposureMultiplier_t)::GetProcAddress( m_scamlib, "CxSetSnapshotExposureMultiplier" );
        CxGetSnapshotExposureMultiplier = (CxGetSnapshotExposureMultiplier_t)::GetProcAddress( m_scamlib, "CxGetSnapshotExposureMultiplier" );
                              
        CxGetSnapshot = (CxGetSnapshot_t)::GetProcAddress( m_scamlib, "CxGetSnapshot" );
        CxCancelSnapshot = (CxCancelSnapshot_t)::GetProcAddress( m_scamlib, "CxCancelSnapshot" );

        CxGetDACRawOffset   = (CxGetDACRawOffset_t)::GetProcAddress( m_scamlib, "CxGetDACRawOffset" );
        CxSetDACRawOffset   = (CxSetDACRawOffset_t)::GetProcAddress( m_scamlib, "CxSetDACRawOffset" );
        CxGetDACFineOffset  = (CxGetDACFineOffset_t)::GetProcAddress( m_scamlib, "CxGetDACFineOffset" );
        CxSetDACFineOffset  = (CxSetDACFineOffset_t)::GetProcAddress( m_scamlib, "CxSetDACFineOffset" );

        CxSetContinuousPrecharge = (CxSetContinuousPrecharge_t)::GetProcAddress( m_scamlib, "CxSetContinuousPrecharge");
        CxGetContinuousPrecharge = (CxGetContinuousPrecharge_t)::GetProcAddress( m_scamlib, "CxGetContinuousPrecharge");

        CxSetTristateOut = (CxSetTristateOut_t)::GetProcAddress( m_scamlib, "CxSetTristateOut");
        CxGetTristateOut = (CxGetTristateOut_t)::GetProcAddress( m_scamlib, "CxGetTristateOut");

        CxWriteSensorReg =  (CxWriteSensorReg_t)::GetProcAddress( m_scamlib, "CxWriteSensorReg");
        CxReadSensorReg  =  (CxReadSensorReg_t)::GetProcAddress( m_scamlib, "CxReadSensorReg");
                                                                
        CxSetControlReg  =  (CxSetControlReg_t)::GetProcAddress( m_scamlib, "CxSetControlReg");
        CxGetControlReg  =  (CxGetControlReg_t)::GetProcAddress( m_scamlib, "CxGetControlReg");


        return TRUE;
    }


private:

    void ResetFunctions() {

        CxOpenDevice = NULL;                 
        CxCloseDevice = NULL;                
                                      
        CxGetCameraInfo = NULL;              
        CxGetCameraInfoEx = NULL;            
                                      
        CxGetStreamMode = NULL;              
        CxSetStreamMode = NULL;              
                                      
        CxGetScreenParams = NULL;            
        CxSetScreenParams = NULL;            
        CxActivateScreenParams = NULL;       
                                      
        CxGetFrequency = NULL;               
        CxSetFrequency = NULL;               
                                      
        CxGetExposureMinMax = NULL;          
        CxGetExposure = NULL;                
        CxSetExposure = NULL;                
                                      
        CxGetExposureMinMaxMs = NULL;        
        CxGetExposureMs = NULL;              
        CxSetExposureMs = NULL;              
                                      
        CxSetGain = NULL;                    
        CxGetGain = NULL;                    
                                      
        CxGrabVideoFrame = NULL;             
        CxGetFramePtr = NULL;                
                                      
        CxSetBrightnessContrastGamma = NULL;
        CxSetConvertionTab = NULL;
        CxGetConvertionTab = NULL;
        CxSetDefaultConvertionTab = NULL;
                                      
        CxStartVideo = NULL;                 
        CxStopVideo = NULL;
        CxSetBayerAlg = NULL;
        CxBayerToRgb = NULL;

        CxGetFrameCounter = NULL;
                                    
        CxGetSnapshotExposureMinMax = NULL;  
        CxGetSnapshotExposure = NULL;        
        CxSetSnapshotExposure = NULL;        
                                      
        CxGetSnapshotExposureMinMaxMs = NULL;
        CxGetSnapshotExposureMs = NULL;      
        CxSetSnapshotExposureMs = NULL;

        CxSetSnapshotExposureMultiplier = NULL;
        CxGetSnapshotExposureMultiplier = NULL;
                                      
        CxGetSnapshot = NULL;
        CxCancelSnapshot = NULL;

        CxGetDACRawOffset   = NULL;
        CxSetDACRawOffset   = NULL;
        CxGetDACFineOffset  = NULL;
        CxSetDACFineOffset  = NULL;

        CxSetContinuousPrecharge = NULL;
        CxGetContinuousPrecharge = NULL;

        CxSetTristateOut = NULL;
        CxGetTristateOut = NULL;

        CxWriteSensorReg = NULL; 
        CxReadSensorReg  = NULL; 
                            
        CxSetControlReg  = NULL; 
        CxGetControlReg  = NULL; 
    }   
};      
        
#endif // #if defined(__cplusplus)

#endif // #if !defined(SMX150_H)
