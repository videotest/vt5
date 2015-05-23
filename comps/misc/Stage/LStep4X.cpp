
#include	"stdafx.h"
#include	"LStep4X.h"
#include "misc_utils.h"
//#include "Library.h"





/////////////////////////////////////////////////////////////////////////////
// Klasse CLStep4X


// Konstruktor: Versucht DLL und alle Zeiger auf Funktionsaufrufe zu laden
CLStep4X::CLStep4X()
{

        m_DLLResultat=0;		// DLL-Zugriff ok

        // Zeiger auf alle Funktionen Nullsetzen
        m_pConnect=NULL;
        m_pConnectSimple=NULL;
        m_pConnectEx=NULL;
        m_pDisconnect=NULL;
        m_pLoadConfig=NULL;
        m_pSaveConfig=NULL;
        m_pMoveAbs=NULL;
        m_pMoveRel=NULL;
        m_pCalibrate=NULL;
        m_pRMeasure=NULL;
        m_pSetPitch=NULL;
        m_pSetPos=NULL;
        m_pSetActiveAxes=NULL;
        m_pSetVel=NULL;
        m_pSetAccel=NULL;
        m_pSetReduction=NULL;
        m_pSetDelay=NULL;
        m_pSetSwitchPolarity=NULL;
        m_pSetSwitchActive=NULL;
        m_pSetJoystickOn=NULL;
        m_pSetJoystickOff=NULL;
        m_pSoftwareReset=NULL;
        m_pSetDigitalOutput=NULL;
        m_pGetDigitalInputs=NULL;
        m_pSetAnalogOutput=NULL;
        m_pGetAnalogInput=NULL;
        m_pSetLimit=NULL;
        m_pSetLimitControl=NULL;
        m_pGetPos=NULL;
        m_pGetStatus=NULL;
        m_pGetEncoderMask=NULL;
        m_pStopAxes=NULL;
        m_pSetAbortFlag=NULL;
        m_pMoveRelSingleAxis=NULL;
        m_pMoveAbsSingleAxis=NULL;
        m_pSetControlPars=NULL;
        m_pSetMotorCurrent=NULL;
        m_pSetVelSingleAxis=NULL;
        m_pSetAccelSingleAxis=NULL;
        m_pCalibrateEx=NULL;
        m_pRMeasureEx=NULL;
        m_pSetShowProt=NULL;
        m_pGetAnalogInputs2=NULL;
        m_pSendString=NULL;
        m_pSetSpeedPoti=NULL;
        m_pSetTVRMode=NULL;
        m_pSetAutoStatus=NULL;
        m_pGetStatusAxis=NULL;
        m_pSetDigIO_Off=NULL;
        m_pSetDigIO_Polarity=NULL;
        m_pSetDigIO_EmergencyStop=NULL;
        m_pSetDigIO_Distance=NULL;
        m_pSetDimensions=NULL;
        m_pMoveRelShort=NULL;
        m_pSetEncoderPeriod=NULL;
        m_pSetJoystickDir=NULL;
        m_pSetEncoderMask=NULL;
        m_pSetGear=NULL;
        m_pSetHandWheelOn=NULL;
        m_pSetHandWheelOff=NULL;
        m_pSetFactorTVR=NULL;
        m_pSetTargetWindow=NULL;
        m_pSetController=NULL;
        m_pSetControllerCall=NULL;
        m_pSetControllerSteps=NULL;
        m_pSetControllerFactor=NULL;
        m_pSetControllerTWDelay=NULL;
        m_pSetEncoderRefSignal=NULL;
        m_pSetEncoderPosition=NULL;
        m_pGetVersionStr=NULL;
        m_pGetError=NULL;
        m_pGetPosSingleAxis=NULL;
        m_pSetDistance=NULL;
        m_pGetPosEx=NULL;
        m_pSetShowCmdList=NULL;
        m_pSetWriteLogText=NULL;
        m_pSetControllerTimeout=NULL;
        m_pSetEncoderActive=NULL;
        m_pGetSnapshotCount=NULL;
        m_pGetSnapshotPos=NULL;
        m_pSetCorrTblOff=NULL;
        m_pSetCorrTblOn=NULL;
        m_pSetFactorMode=NULL;
        m_pSetSnapshot=NULL;
        m_pSetSnapshotPar=NULL;
        m_pSetTrigger=NULL;
        m_pSetTriggerPar=NULL;
        m_pSetLanguage=NULL;
        m_pGetSwitches=NULL;
        m_pGetSerialNr=NULL;
        m_pSetCalibOffset=NULL;
        m_pSetRMOffset=NULL;
        m_pGetSnapshotPosArray=NULL;
        m_pSetAxisDirection=NULL;
        m_pSetCalibrateDir=NULL;
        m_pLStepSave=NULL;
        m_pSetBPZ=NULL;
        m_pSetBPZTrackballFactor=NULL;
        m_pSetBPZTrackballBackLash=NULL;
        m_pSetBPZJoyspeed=NULL;
        m_pSetJoystickWindow=NULL;
        m_pSetCurrentDelay=NULL;
        m_pMoveEx=NULL;
        m_pWaitForAxisStop=NULL;
        m_pSetProcessMessagesProc=NULL;
        m_pSendStringPosCmd=NULL;
        m_pSetDigitalOutputs=NULL;
        m_pSetWriteLogTextFN=NULL;
        m_pSetDigitalOutputsE=NULL;
        m_pGetDigitalInputsE=NULL;
        m_pEnableCommandRetry=NULL;
        m_pSetXYAxisComp=NULL;
        m_pGetVersionStrDet=NULL;
        m_pSetCommandTimeout=NULL;
        m_pSetExtValue=NULL;
        m_pFlushBuffer=NULL;
        m_pGetEEPRomValue=NULL;
        m_pSetEEPRomValue=NULL;
        m_pGetXYAxisComp=NULL;
        m_pGetDimensions=NULL;
        m_pGetPitch=NULL;
        m_pGetGear=NULL;
        m_pGetVel=NULL;
        m_pGetAccel=NULL;
        m_pSetVelFac=NULL;
        m_pGetVelFac=NULL;
        m_pGetSpeedPoti=NULL;
        m_pGetMotorCurrent=NULL;
        m_pGetReduction=NULL;
        m_pGetCurrentDelay=NULL;
        m_pSetOutFuncLev=NULL;
        m_pGetOutFuncLev=NULL;
        m_pGetActiveAxes=NULL;
        m_pGetAxisDirection=NULL;
        m_pGetCalibOffset=NULL;
        m_pGetRMOffset=NULL;
        m_pGetCalibrateDir=NULL;
        m_pSetCalibBackSpeed=NULL;
        m_pGetCalibBackSpeed=NULL;
        m_pSetRefSpeed=NULL;
        m_pGetRefSpeed=NULL;
        m_pSetPowerAmplifier=NULL;
        m_pGetPowerAmplifier=NULL;
        m_pSetMotorTablePatch=NULL;
        m_pGetMotorTablePatch=NULL;
        m_pSetJoystickFilter=NULL;
        m_pGetJoystickFilter=NULL;
        m_pSetStopPolarity=NULL;
        m_pGetStopPolarity=NULL;
        m_pSetVLevel=NULL;
        m_pGetVLevel=NULL;
        m_pSetStopAccel=NULL;
        m_pGetStopAccel=NULL;
        m_pGetVersionStrInfo=NULL;
        m_pGetStatusLimit=NULL;
        m_pGetSecurityErr=NULL;
        m_pGetSecurityStatus=NULL;
        m_pGetDelay=NULL;
        m_pGetDistance=NULL;
        m_pClearPos=NULL;
        m_pSetDigJoySpeed=NULL;
        m_pGetDigJoySpeed=NULL;
        m_pGetJoystickDir=NULL;
        m_pGetJoystick=NULL;
        m_pGetJoystickWindow=NULL;
        m_pGetHandWheel=NULL;
        m_pGetBPZ=NULL;
        m_pGetBPZTrackballFactor=NULL;
        m_pGetBPZTrackballBackLash=NULL;
        m_pGetBPZJoyspeed=NULL;
        m_pGetLimitControl=NULL;
        m_pSetAutoLimitAfterCalibRM=NULL;
        m_pGetAutoLimitAfterCalibRM=NULL;
        m_pGetSwitchPolarity=NULL;
        m_pGetSwitchActive=NULL;
        m_pGetTVRMode=NULL;
        m_pGetFactorTVR=NULL;
        m_pSetTVROutMode=NULL;
        m_pGetTVROutMode=NULL;
        m_pSetTVROutResolution=NULL;
        m_pGetTVROutResolution=NULL;
        m_pSetTVROutPitch=NULL;
        m_pGetTVROutPitch=NULL;
        m_pSetVelTVRO=NULL;
        m_pGetVelTVRO=NULL;
        m_pSetAccelTVRO=NULL;
        m_pGetAccelTVRO=NULL;
        m_pSetVelSingleAxisTVRO=NULL;
        m_pSetAccelSingleAxisTVRO=NULL;
        m_pSetPosTVRO=NULL;
        m_pGetPosTVRO=NULL;
        m_pMoveAbsTVRO=NULL;
        m_pMoveRelTVRO=NULL;
        m_pMoveRelTVROSingleAxis=NULL;
        m_pMoveAbsTVROSingleAxis=NULL;
        m_pGetStatusTVRO=NULL;
        m_pGetTargetWindow=NULL;
        m_pGetEncoderPeriod=NULL;
        m_pGetEncoderRefSignal=NULL;
        m_pGetEncoderPosition=NULL;
        m_pGetEncoderActive=NULL;
        m_pGetController=NULL;
        m_pGetControllerCall=NULL;
        m_pGetControllerSteps=NULL;
        m_pGetControllerFactor=NULL;
        m_pGetControllerTWDelay=NULL;
        m_pGetControllerTimeout=NULL;
        m_pSetCtrFastMoveOn=NULL;
        m_pSetCtrFastMoveOff=NULL;
        m_pGetCtrFastMove=NULL;
        m_pClearCtrFastMoveCounter=NULL;
        m_pGetCtrFastMoveCounter=NULL;
        m_pClearEncoder=NULL;
        m_pGetEncoder=NULL;
        m_pGetTrigger=NULL;
        m_pGetTriggerPar=NULL;
        m_pSetTrigCount=NULL;
        m_pGetTrigCount=NULL;
        m_pSetTVRInPulse=NULL;
        m_pGetSnapshot=NULL;
        m_pSetSnapshotFilter=NULL;
        m_pGetSnapshotFilter=NULL;
        m_pGetSnapshotPar=NULL;
        m_pGetLimit=NULL;
        m_pJoyChangeAxis=NULL;
        m_pGetJoyChangeAxis=NULL;
        m_pCreateLSID=NULL;
        m_pFreeLSID=NULL;

        //if ((m_LStep4=::LoadLibrary(_T("LSTEP4X.DLL")))==0)
		//if ((m_LStep4=::LoadLibrary("C:\\vt5\comps\\MISC\\Stage\\Phoenix1280.dll"))==0)
		
			/*try
	            {
	       // m_LStep4=::LoadLibrary("SMX150.dll");
			//m_LStep4=::LoadLibrary("Phoenix1280.dll");
			//m_LStep4=::LoadLibrary("LSTEP4X.DLL");
			m_LStep4=::LoadLibraryEx("LSTEP4X.DLL",NULL,DONT_RESOLVE_DLL_REFERENCES);
	               }
				catch(...){
					DWORD error=GetLastError();
				VTMessageBox("Error", "VT5", MB_OK);
				}*/
	        /* catch( CException* e )
                  {
      
	             e->ReportError();
				 
	  
	                   throw;
                     }*/
		//if ((m_LStep4=::LoadLibrary("LSTEP4X.DLL"))==0)
	if ((m_LStep4=::LoadLibraryEx("LSTEP4X.DLL",NULL,LOAD_IGNORE_CODE_AUTHZ_LEVEL))==0)
		//if (m_LStep4==0)
        {
           DWORD error=GetLastError();
			//VTMessageBox("LoadLibrary==0", "VT5", MB_OK);
                m_DLLResultat=-1; // DLL konnte nicht geladen werden
        }
		
        else
        {
                // Zeiger auf jede Methode der Dll laden und prüfen
			////////SERGEY PAST
			m_pConnect= (PConnect) GetProcAddress(m_LStep4,"LSX_Connect");
                if (!m_pConnect) m_DLLResultat=-2;

               m_pConnectSimple= (PConnectSimple) GetProcAddress(m_LStep4,"LSX_ConnectSimple");
                if (!m_pConnectSimple) m_DLLResultat=-2;

                m_pConnectEx= (PConnectEx) GetProcAddress(m_LStep4,"LSX_ConnectEx");
                if (!m_pConnectEx) m_DLLResultat=-2;

                m_pDisconnect= (PDisconnect) GetProcAddress(m_LStep4,"LSX_Disconnect");
                if (!m_pDisconnect) m_DLLResultat=-2;

                m_pLoadConfig= (PLoadConfig) GetProcAddress(m_LStep4,"LSX_LoadConfig");
                if (!m_pLoadConfig) m_DLLResultat=-2;

                m_pSaveConfig= (PSaveConfig) GetProcAddress(m_LStep4,"LSX_SaveConfig");
                if (!m_pSaveConfig) m_DLLResultat=-2;

                m_pMoveAbs= (PMoveAbs) GetProcAddress(m_LStep4,"LSX_MoveAbs");
                if (!m_pMoveAbs) m_DLLResultat=-2;

                m_pMoveRel= (PMoveRel) GetProcAddress(m_LStep4,"LSX_MoveRel");
                if (!m_pMoveRel) m_DLLResultat=-2;

                m_pCalibrate= (PCalibrate) GetProcAddress(m_LStep4,"LSX_Calibrate");
                if (!m_pCalibrate) m_DLLResultat=-2;

                m_pRMeasure= (PRMeasure) GetProcAddress(m_LStep4,"LSX_RMeasure");
                if (!m_pRMeasure) m_DLLResultat=-2;

                m_pSetPitch= (PSetPitch) GetProcAddress(m_LStep4,"LSX_SetPitch");
                if (!m_pSetPitch) m_DLLResultat=-2;

                m_pSetPos= (PSetPos) GetProcAddress(m_LStep4,"LSX_SetPos");
                if (!m_pSetPos) m_DLLResultat=-2;

                m_pSetActiveAxes= (PSetActiveAxes) GetProcAddress(m_LStep4,"LSX_SetActiveAxes");
                if (!m_pSetActiveAxes) m_DLLResultat=-2;

                m_pSetVel= (PSetVel) GetProcAddress(m_LStep4,"LSX_SetVel");
                if (!m_pSetVel) m_DLLResultat=-2;

                m_pSetAccel= (PSetAccel) GetProcAddress(m_LStep4,"LSX_SetAccel");
                if (!m_pSetAccel) m_DLLResultat=-2;

               m_pSetReduction= (PSetReduction) GetProcAddress(m_LStep4,"LSX_SetReduction");
                if (!m_pSetReduction) m_DLLResultat=-2;

                m_pSetDelay= (PSetDelay) GetProcAddress(m_LStep4,"LSX_SetDelay");
                if (!m_pSetDelay) m_DLLResultat=-2;

                m_pSetSwitchPolarity= (PSetSwitchPolarity) GetProcAddress(m_LStep4,"LSX_SetSwitchPolarity");
                if (!m_pSetSwitchPolarity) m_DLLResultat=-2;

                m_pSetSwitchActive= (PSetSwitchActive) GetProcAddress(m_LStep4,"LSX_SetSwitchActive");
                if (!m_pSetSwitchActive) m_DLLResultat=-2;

                m_pSetJoystickOn= (PSetJoystickOn) GetProcAddress(m_LStep4,"LSX_SetJoystickOn");
                if (!m_pSetJoystickOn) m_DLLResultat=-2;

                m_pSetJoystickOff= (PSetJoystickOff) GetProcAddress(m_LStep4,"LSX_SetJoystickOff");
                if (!m_pSetJoystickOff) m_DLLResultat=-2;

                m_pSoftwareReset= (PSoftwareReset) GetProcAddress(m_LStep4,"LSX_SoftwareReset");
                if (!m_pSoftwareReset) m_DLLResultat=-2;

                m_pSetDigitalOutput= (PSetDigitalOutput) GetProcAddress(m_LStep4,"LSX_SetDigitalOutput");
                if (!m_pSetDigitalOutput) m_DLLResultat=-2;

                m_pGetDigitalInputs= (PGetDigitalInputs) GetProcAddress(m_LStep4,"LSX_GetDigitalInputs");
                if (!m_pGetDigitalInputs) m_DLLResultat=-2;

                m_pSetAnalogOutput= (PSetAnalogOutput) GetProcAddress(m_LStep4,"LSX_SetAnalogOutput");
                if (!m_pSetAnalogOutput) m_DLLResultat=-2;

                m_pGetAnalogInput= (PGetAnalogInput) GetProcAddress(m_LStep4,"LSX_GetAnalogInput");
                if (!m_pGetAnalogInput) m_DLLResultat=-2;

                m_pSetLimit= (PSetLimit) GetProcAddress(m_LStep4,"LSX_SetLimit");
                if (!m_pSetLimit) m_DLLResultat=-2;

                m_pSetLimitControl= (PSetLimitControl) GetProcAddress(m_LStep4,"LSX_SetLimitControl");
                if (!m_pSetLimitControl) m_DLLResultat=-2;

                m_pGetPos= (PGetPos) GetProcAddress(m_LStep4,"LSX_GetPos");
                if (!m_pGetPos) m_DLLResultat=-2;

                m_pGetStatus= (PGetStatus) GetProcAddress(m_LStep4,"LSX_GetStatus");
                if (!m_pGetStatus) m_DLLResultat=-2;

                m_pGetEncoderMask= (PGetEncoderMask) GetProcAddress(m_LStep4,"LSX_GetEncoderMask");
                if (!m_pGetEncoderMask) m_DLLResultat=-2;

                m_pStopAxes= (PStopAxes) GetProcAddress(m_LStep4,"LSX_StopAxes");
                if (!m_pStopAxes) m_DLLResultat=-2;

                m_pSetAbortFlag= (PSetAbortFlag) GetProcAddress(m_LStep4,"LSX_SetAbortFlag");
                if (!m_pSetAbortFlag) m_DLLResultat=-2;

                m_pMoveRelSingleAxis= (PMoveRelSingleAxis) GetProcAddress(m_LStep4,"LSX_MoveRelSingleAxis");
                if (!m_pMoveRelSingleAxis) m_DLLResultat=-2;

                m_pMoveAbsSingleAxis= (PMoveAbsSingleAxis) GetProcAddress(m_LStep4,"LSX_MoveAbsSingleAxis");
                if (!m_pMoveAbsSingleAxis) m_DLLResultat=-2;

                m_pSetControlPars= (PSetControlPars) GetProcAddress(m_LStep4,"LSX_SetControlPars");
                if (!m_pSetControlPars) m_DLLResultat=-2;

                m_pSetMotorCurrent= (PSetMotorCurrent) GetProcAddress(m_LStep4,"LSX_SetMotorCurrent");
                if (!m_pSetMotorCurrent) m_DLLResultat=-2;

                m_pSetVelSingleAxis= (PSetVelSingleAxis) GetProcAddress(m_LStep4,"LSX_SetVelSingleAxis");
                if (!m_pSetVelSingleAxis) m_DLLResultat=-2;

                m_pSetAccelSingleAxis= (PSetAccelSingleAxis) GetProcAddress(m_LStep4,"LSX_SetAccelSingleAxis");
                if (!m_pSetAccelSingleAxis) m_DLLResultat=-2;

                m_pCalibrateEx= (PCalibrateEx) GetProcAddress(m_LStep4,"LSX_CalibrateEx");
                if (!m_pCalibrateEx) m_DLLResultat=-2;

                m_pRMeasureEx= (PRMeasureEx) GetProcAddress(m_LStep4,"LSX_RMeasureEx");
                if (!m_pRMeasureEx) m_DLLResultat=-2;

                m_pSetShowProt= (PSetShowProt) GetProcAddress(m_LStep4,"LSX_SetShowProt");
                if (!m_pSetShowProt) m_DLLResultat=-2;

                m_pGetAnalogInputs2= (PGetAnalogInputs2) GetProcAddress(m_LStep4,"LSX_GetAnalogInputs2");
                if (!m_pGetAnalogInputs2) m_DLLResultat=-2;

                m_pSendString= (PSendString) GetProcAddress(m_LStep4,"LSX_SendString");
                if (!m_pSendString) m_DLLResultat=-2;

                m_pSetSpeedPoti= (PSetSpeedPoti) GetProcAddress(m_LStep4,"LSX_SetSpeedPoti");
                if (!m_pSetSpeedPoti) m_DLLResultat=-2;

                m_pSetTVRMode= (PSetTVRMode) GetProcAddress(m_LStep4,"LSX_SetTVRMode");
                if (!m_pSetTVRMode) m_DLLResultat=-2;

                m_pSetAutoStatus= (PSetAutoStatus) GetProcAddress(m_LStep4,"LSX_SetAutoStatus");
                if (!m_pSetAutoStatus) m_DLLResultat=-2;

                m_pGetStatusAxis= (PGetStatusAxis) GetProcAddress(m_LStep4,"LSX_GetStatusAxis");
                if (!m_pGetStatusAxis) m_DLLResultat=-2;

                m_pSetDigIO_Off= (PSetDigIO_Off) GetProcAddress(m_LStep4,"LSX_SetDigIO_Off");
                if (!m_pSetDigIO_Off) m_DLLResultat=-2;

                m_pSetDigIO_Polarity= (PSetDigIO_Polarity) GetProcAddress(m_LStep4,"LSX_SetDigIO_Polarity");
                if (!m_pSetDigIO_Polarity) m_DLLResultat=-2;

                m_pSetDigIO_EmergencyStop= (PSetDigIO_EmergencyStop) GetProcAddress(m_LStep4,"LSX_SetDigIO_EmergencyStop");
                if (!m_pSetDigIO_EmergencyStop) m_DLLResultat=-2;

                m_pSetDigIO_Distance= (PSetDigIO_Distance) GetProcAddress(m_LStep4,"LSX_SetDigIO_Distance");
                if (!m_pSetDigIO_Distance) m_DLLResultat=-2;

                m_pSetDimensions= (PSetDimensions) GetProcAddress(m_LStep4,"LSX_SetDimensions");
                if (!m_pSetDimensions) m_DLLResultat=-2;

                m_pMoveRelShort= (PMoveRelShort) GetProcAddress(m_LStep4,"LSX_MoveRelShort");
                if (!m_pMoveRelShort) m_DLLResultat=-2;

                m_pSetEncoderPeriod= (PSetEncoderPeriod) GetProcAddress(m_LStep4,"LSX_SetEncoderPeriod");
                if (!m_pSetEncoderPeriod) m_DLLResultat=-2;

                m_pSetJoystickDir= (PSetJoystickDir) GetProcAddress(m_LStep4,"LSX_SetJoystickDir");
                if (!m_pSetJoystickDir) m_DLLResultat=-2;

                m_pSetEncoderMask= (PSetEncoderMask) GetProcAddress(m_LStep4,"LSX_SetEncoderMask");
                if (!m_pSetEncoderMask) m_DLLResultat=-2;

                m_pSetGear= (PSetGear) GetProcAddress(m_LStep4,"LSX_SetGear");
                if (!m_pSetGear) m_DLLResultat=-2;

                m_pSetHandWheelOn= (PSetHandWheelOn) GetProcAddress(m_LStep4,"LSX_SetHandWheelOn");
                if (!m_pSetHandWheelOn) m_DLLResultat=-2;

                m_pSetHandWheelOff= (PSetHandWheelOff) GetProcAddress(m_LStep4,"LSX_SetHandWheelOff");
                if (!m_pSetHandWheelOff) m_DLLResultat=-2;

                m_pSetFactorTVR= (PSetFactorTVR) GetProcAddress(m_LStep4,"LSX_SetFactorTVR");
                if (!m_pSetFactorTVR) m_DLLResultat=-2;

                m_pSetTargetWindow= (PSetTargetWindow) GetProcAddress(m_LStep4,"LSX_SetTargetWindow");
                if (!m_pSetTargetWindow) m_DLLResultat=-2;

               m_pSetController= (PSetController) GetProcAddress(m_LStep4,"LSX_SetController");
                if (!m_pSetController) m_DLLResultat=-2;

                m_pSetControllerCall= (PSetControllerCall) GetProcAddress(m_LStep4,"LSX_SetControllerCall");
                if (!m_pSetControllerCall) m_DLLResultat=-2;

                m_pSetControllerSteps= (PSetControllerSteps) GetProcAddress(m_LStep4,"LSX_SetControllerSteps");
                if (!m_pSetControllerSteps) m_DLLResultat=-2;

                m_pSetControllerFactor= (PSetControllerFactor) GetProcAddress(m_LStep4,"LSX_SetControllerFactor");
                if (!m_pSetControllerFactor) m_DLLResultat=-2;

                m_pSetControllerTWDelay= (PSetControllerTWDelay) GetProcAddress(m_LStep4,"LSX_SetControllerTWDelay");
                if (!m_pSetControllerTWDelay) m_DLLResultat=-2;

                m_pSetEncoderRefSignal= (PSetEncoderRefSignal) GetProcAddress(m_LStep4,"LSX_SetEncoderRefSignal");
                if (!m_pSetEncoderRefSignal) m_DLLResultat=-2;

                m_pSetEncoderPosition= (PSetEncoderPosition) GetProcAddress(m_LStep4,"LSX_SetEncoderPosition");
                if (!m_pSetEncoderPosition) m_DLLResultat=-2;

                m_pGetVersionStr= (PGetVersionStr) GetProcAddress(m_LStep4,"LSX_GetVersionStr");
                if (!m_pGetVersionStr) m_DLLResultat=-2;

                m_pGetError= (PGetError) GetProcAddress(m_LStep4,"LSX_GetError");
                if (!m_pGetError) m_DLLResultat=-2;

                m_pGetPosSingleAxis= (PGetPosSingleAxis) GetProcAddress(m_LStep4,"LSX_GetPosSingleAxis");
                if (!m_pGetPosSingleAxis) m_DLLResultat=-2;

                m_pSetDistance= (PSetDistance) GetProcAddress(m_LStep4,"LSX_SetDistance");
                if (!m_pSetDistance) m_DLLResultat=-2;

                m_pGetPosEx= (PGetPosEx) GetProcAddress(m_LStep4,"LSX_GetPosEx");
                if (!m_pGetPosEx) m_DLLResultat=-2;

                m_pSetShowCmdList= (PSetShowCmdList) GetProcAddress(m_LStep4,"LSX_SetShowCmdList");
                if (!m_pSetShowCmdList) m_DLLResultat=-2;

                m_pSetWriteLogText= (PSetWriteLogText) GetProcAddress(m_LStep4,"LSX_SetWriteLogText");
                if (!m_pSetWriteLogText) m_DLLResultat=-2;

                m_pSetControllerTimeout= (PSetControllerTimeout) GetProcAddress(m_LStep4,"LSX_SetControllerTimeout");
                if (!m_pSetControllerTimeout) m_DLLResultat=-2;

                m_pSetEncoderActive= (PSetEncoderActive) GetProcAddress(m_LStep4,"LSX_SetEncoderActive");
                if (!m_pSetEncoderActive) m_DLLResultat=-2;

                m_pGetSnapshotCount= (PGetSnapshotCount) GetProcAddress(m_LStep4,"LSX_GetSnapshotCount");
                if (!m_pGetSnapshotCount) m_DLLResultat=-2;

                m_pGetSnapshotPos= (PGetSnapshotPos) GetProcAddress(m_LStep4,"LSX_GetSnapshotPos");
                if (!m_pGetSnapshotPos) m_DLLResultat=-2;

                m_pSetCorrTblOff= (PSetCorrTblOff) GetProcAddress(m_LStep4,"LSX_SetCorrTblOff");
                if (!m_pSetCorrTblOff) m_DLLResultat=-2;

                m_pSetCorrTblOn= (PSetCorrTblOn) GetProcAddress(m_LStep4,"LSX_SetCorrTblOn");
                if (!m_pSetCorrTblOn) m_DLLResultat=-2;

                m_pSetFactorMode= (PSetFactorMode) GetProcAddress(m_LStep4,"LSX_SetFactorMode");
                if (!m_pSetFactorMode) m_DLLResultat=-2;

                m_pSetSnapshot= (PSetSnapshot) GetProcAddress(m_LStep4,"LSX_SetSnapshot");
                if (!m_pSetSnapshot) m_DLLResultat=-2;

                m_pSetSnapshotPar= (PSetSnapshotPar) GetProcAddress(m_LStep4,"LSX_SetSnapshotPar");
                if (!m_pSetSnapshotPar) m_DLLResultat=-2;

                m_pSetTrigger= (PSetTrigger) GetProcAddress(m_LStep4,"LSX_SetTrigger");
                if (!m_pSetTrigger) m_DLLResultat=-2;

                m_pSetTriggerPar= (PSetTriggerPar) GetProcAddress(m_LStep4,"LSX_SetTriggerPar");
                if (!m_pSetTriggerPar) m_DLLResultat=-2;

                m_pSetLanguage= (PSetLanguage) GetProcAddress(m_LStep4,"LSX_SetLanguage");
                if (!m_pSetLanguage) m_DLLResultat=-2;

                m_pGetSwitches= (PGetSwitches) GetProcAddress(m_LStep4,"LSX_GetSwitches");
                if (!m_pGetSwitches) m_DLLResultat=-2;

                m_pGetSerialNr= (PGetSerialNr) GetProcAddress(m_LStep4,"LSX_GetSerialNr");
                if (!m_pGetSerialNr) m_DLLResultat=-2;

                m_pSetCalibOffset= (PSetCalibOffset) GetProcAddress(m_LStep4,"LSX_SetCalibOffset");
                if (!m_pSetCalibOffset) m_DLLResultat=-2;

                m_pSetRMOffset= (PSetRMOffset) GetProcAddress(m_LStep4,"LSX_SetRMOffset");
                if (!m_pSetRMOffset) m_DLLResultat=-2;

                m_pGetSnapshotPosArray= (PGetSnapshotPosArray) GetProcAddress(m_LStep4,"LSX_GetSnapshotPosArray");
                if (!m_pGetSnapshotPosArray) m_DLLResultat=-2;

                m_pSetAxisDirection= (PSetAxisDirection) GetProcAddress(m_LStep4,"LSX_SetAxisDirection");
                if (!m_pSetAxisDirection) m_DLLResultat=-2;

                m_pSetCalibrateDir= (PSetCalibrateDir) GetProcAddress(m_LStep4,"LSX_SetCalibrateDir");
                if (!m_pSetCalibrateDir) m_DLLResultat=-2;

                m_pLStepSave= (PLStepSave) GetProcAddress(m_LStep4,"LSX_LStepSave");
                if (!m_pLStepSave) m_DLLResultat=-2;

                m_pSetBPZ= (PSetBPZ) GetProcAddress(m_LStep4,"LSX_SetBPZ");
                if (!m_pSetBPZ) m_DLLResultat=-2;

                m_pSetBPZTrackballFactor= (PSetBPZTrackballFactor) GetProcAddress(m_LStep4,"LSX_SetBPZTrackballFactor");
                if (!m_pSetBPZTrackballFactor) m_DLLResultat=-2;

                m_pSetBPZTrackballBackLash= (PSetBPZTrackballBackLash) GetProcAddress(m_LStep4,"LSX_SetBPZTrackballBackLash");
                if (!m_pSetBPZTrackballBackLash) m_DLLResultat=-2;

                m_pSetBPZJoyspeed= (PSetBPZJoyspeed) GetProcAddress(m_LStep4,"LSX_SetBPZJoyspeed");
                if (!m_pSetBPZJoyspeed) m_DLLResultat=-2;

                m_pSetJoystickWindow= (PSetJoystickWindow) GetProcAddress(m_LStep4,"LSX_SetJoystickWindow");
                if (!m_pSetJoystickWindow) m_DLLResultat=-2;

                m_pSetCurrentDelay= (PSetCurrentDelay) GetProcAddress(m_LStep4,"LSX_SetCurrentDelay");
                if (!m_pSetCurrentDelay) m_DLLResultat=-2;

                m_pMoveEx= (PMoveEx) GetProcAddress(m_LStep4,"LSX_MoveEx");
                if (!m_pMoveEx) m_DLLResultat=-2;

                m_pWaitForAxisStop= (PWaitForAxisStop) GetProcAddress(m_LStep4,"LSX_WaitForAxisStop");
                if (!m_pWaitForAxisStop) m_DLLResultat=-2;

                m_pSetProcessMessagesProc= (PSetProcessMessagesProc) GetProcAddress(m_LStep4,"LSX_SetProcessMessagesProc");
                if (!m_pSetProcessMessagesProc) m_DLLResultat=-2;

                m_pSendStringPosCmd= (PSendStringPosCmd) GetProcAddress(m_LStep4,"LSX_SendStringPosCmd");
                if (!m_pSendStringPosCmd) m_DLLResultat=-2;

                m_pSetDigitalOutputs= (PSetDigitalOutputs) GetProcAddress(m_LStep4,"LSX_SetDigitalOutputs");
                if (!m_pSetDigitalOutputs) m_DLLResultat=-2;

                m_pSetWriteLogTextFN= (PSetWriteLogTextFN) GetProcAddress(m_LStep4,"LSX_SetWriteLogTextFN");
                if (!m_pSetWriteLogTextFN) m_DLLResultat=-2;

                m_pSetDigitalOutputsE= (PSetDigitalOutputsE) GetProcAddress(m_LStep4,"LSX_SetDigitalOutputsE");
                if (!m_pSetDigitalOutputsE) m_DLLResultat=-2;

                m_pGetDigitalInputsE= (PGetDigitalInputsE) GetProcAddress(m_LStep4,"LSX_GetDigitalInputsE");
                if (!m_pGetDigitalInputsE) m_DLLResultat=-2;

                m_pEnableCommandRetry= (PEnableCommandRetry) GetProcAddress(m_LStep4,"LSX_EnableCommandRetry");
                if (!m_pEnableCommandRetry) m_DLLResultat=-2;

                m_pSetXYAxisComp= (PSetXYAxisComp) GetProcAddress(m_LStep4,"LSX_SetXYAxisComp");
                if (!m_pSetXYAxisComp) m_DLLResultat=-2;

                m_pGetVersionStrDet= (PGetVersionStrDet) GetProcAddress(m_LStep4,"LSX_GetVersionStrDet");
                if (!m_pGetVersionStrDet) m_DLLResultat=-2;

                m_pSetCommandTimeout= (PSetCommandTimeout) GetProcAddress(m_LStep4,"LSX_SetCommandTimeout");
                if (!m_pSetCommandTimeout) m_DLLResultat=-2;

                m_pSetExtValue= (PSetExtValue) GetProcAddress(m_LStep4,"LSX_SetExtValue");
                if (!m_pSetExtValue) m_DLLResultat=-2;

                m_pFlushBuffer= (PFlushBuffer) GetProcAddress(m_LStep4,"LSX_FlushBuffer");
                if (!m_pFlushBuffer) m_DLLResultat=-2;

                m_pGetEEPRomValue= (PGetEEPRomValue) GetProcAddress(m_LStep4,"LSX_GetEEPRomValue");
                if (!m_pGetEEPRomValue) m_DLLResultat=-2;

                m_pSetEEPRomValue= (PSetEEPRomValue) GetProcAddress(m_LStep4,"LSX_SetEEPRomValue");
                if (!m_pSetEEPRomValue) m_DLLResultat=-2;

                m_pGetXYAxisComp= (PGetXYAxisComp) GetProcAddress(m_LStep4,"LSX_GetXYAxisComp");
                if (!m_pGetXYAxisComp) m_DLLResultat=-2;

                m_pGetDimensions= (PGetDimensions) GetProcAddress(m_LStep4,"LSX_GetDimensions");
                if (!m_pGetDimensions) m_DLLResultat=-2;

                m_pGetPitch= (PGetPitch) GetProcAddress(m_LStep4,"LSX_GetPitch");
                if (!m_pGetPitch) m_DLLResultat=-2;

                m_pGetGear= (PGetGear) GetProcAddress(m_LStep4,"LSX_GetGear");
                if (!m_pGetGear) m_DLLResultat=-2;

                m_pGetVel= (PGetVel) GetProcAddress(m_LStep4,"LSX_GetVel");
                if (!m_pGetVel) m_DLLResultat=-2;

                m_pGetAccel= (PGetAccel) GetProcAddress(m_LStep4,"LSX_GetAccel");
                if (!m_pGetAccel) m_DLLResultat=-2;

                m_pSetVelFac= (PSetVelFac) GetProcAddress(m_LStep4,"LSX_SetVelFac");
                if (!m_pSetVelFac) m_DLLResultat=-2;

                m_pGetVelFac= (PGetVelFac) GetProcAddress(m_LStep4,"LSX_GetVelFac");
                if (!m_pGetVelFac) m_DLLResultat=-2;

                m_pGetSpeedPoti= (PGetSpeedPoti) GetProcAddress(m_LStep4,"LSX_GetSpeedPoti");
                if (!m_pGetSpeedPoti) m_DLLResultat=-2;

                m_pGetMotorCurrent= (PGetMotorCurrent) GetProcAddress(m_LStep4,"LSX_GetMotorCurrent");
                if (!m_pGetMotorCurrent) m_DLLResultat=-2;

                m_pGetReduction= (PGetReduction) GetProcAddress(m_LStep4,"LSX_GetReduction");
                if (!m_pGetReduction) m_DLLResultat=-2;

                m_pGetCurrentDelay= (PGetCurrentDelay) GetProcAddress(m_LStep4,"LSX_GetCurrentDelay");
                if (!m_pGetCurrentDelay) m_DLLResultat=-2;

                m_pSetOutFuncLev= (PSetOutFuncLev) GetProcAddress(m_LStep4,"LSX_SetOutFuncLev");
                if (!m_pSetOutFuncLev) m_DLLResultat=-2;

                m_pGetOutFuncLev= (PGetOutFuncLev) GetProcAddress(m_LStep4,"LSX_GetOutFuncLev");
                if (!m_pGetOutFuncLev) m_DLLResultat=-2;

                m_pGetActiveAxes= (PGetActiveAxes) GetProcAddress(m_LStep4,"LSX_GetActiveAxes");
                if (!m_pGetActiveAxes) m_DLLResultat=-2;

                m_pGetAxisDirection= (PGetAxisDirection) GetProcAddress(m_LStep4,"LSX_GetAxisDirection");
                if (!m_pGetAxisDirection) m_DLLResultat=-2;

                m_pGetCalibOffset= (PGetCalibOffset) GetProcAddress(m_LStep4,"LSX_GetCalibOffset");
                if (!m_pGetCalibOffset) m_DLLResultat=-2;

               m_pGetRMOffset= (PGetRMOffset) GetProcAddress(m_LStep4,"LSX_GetRMOffset");
                if (!m_pGetRMOffset) m_DLLResultat=-2;

                m_pGetCalibrateDir= (PGetCalibrateDir) GetProcAddress(m_LStep4,"LSX_GetCalibrateDir");
                if (!m_pGetCalibrateDir) m_DLLResultat=-2;

                m_pSetCalibBackSpeed= (PSetCalibBackSpeed) GetProcAddress(m_LStep4,"LSX_SetCalibBackSpeed");
                if (!m_pSetCalibBackSpeed) m_DLLResultat=-2;

                m_pGetCalibBackSpeed= (PGetCalibBackSpeed) GetProcAddress(m_LStep4,"LSX_GetCalibBackSpeed");
                if (!m_pGetCalibBackSpeed) m_DLLResultat=-2;

                m_pSetRefSpeed= (PSetRefSpeed) GetProcAddress(m_LStep4,"LSX_SetRefSpeed");
                if (!m_pSetRefSpeed) m_DLLResultat=-2;

                m_pGetRefSpeed= (PGetRefSpeed) GetProcAddress(m_LStep4,"LSX_GetRefSpeed");
                if (!m_pGetRefSpeed) m_DLLResultat=-2;

               m_pSetPowerAmplifier= (PSetPowerAmplifier) GetProcAddress(m_LStep4,"LSX_SetPowerAmplifier");
                if (!m_pSetPowerAmplifier) m_DLLResultat=-2;

                m_pGetPowerAmplifier= (PGetPowerAmplifier) GetProcAddress(m_LStep4,"LSX_GetPowerAmplifier");
                if (!m_pGetPowerAmplifier) m_DLLResultat=-2;

                m_pSetMotorTablePatch= (PSetMotorTablePatch) GetProcAddress(m_LStep4,"LSX_SetMotorTablePatch");
                if (!m_pSetMotorTablePatch) m_DLLResultat=-2;

                m_pGetMotorTablePatch= (PGetMotorTablePatch) GetProcAddress(m_LStep4,"LSX_GetMotorTablePatch");
                if (!m_pGetMotorTablePatch) m_DLLResultat=-2;

                m_pSetJoystickFilter= (PSetJoystickFilter) GetProcAddress(m_LStep4,"LSX_SetJoystickFilter");
                if (!m_pSetJoystickFilter) m_DLLResultat=-2;

                m_pGetJoystickFilter= (PGetJoystickFilter) GetProcAddress(m_LStep4,"LSX_GetJoystickFilter");
                if (!m_pGetJoystickFilter) m_DLLResultat=-2;

                m_pSetStopPolarity= (PSetStopPolarity) GetProcAddress(m_LStep4,"LSX_SetStopPolarity");
                if (!m_pSetStopPolarity) m_DLLResultat=-2;

                m_pGetStopPolarity= (PGetStopPolarity) GetProcAddress(m_LStep4,"LSX_GetStopPolarity");
                if (!m_pGetStopPolarity) m_DLLResultat=-2;

                m_pSetVLevel= (PSetVLevel) GetProcAddress(m_LStep4,"LSX_SetVLevel");
                if (!m_pSetVLevel) m_DLLResultat=-2;

                m_pGetVLevel= (PGetVLevel) GetProcAddress(m_LStep4,"LSX_GetVLevel");
                if (!m_pGetVLevel) m_DLLResultat=-2;

                m_pSetStopAccel= (PSetStopAccel) GetProcAddress(m_LStep4,"LSX_SetStopAccel");
                if (!m_pSetStopAccel) m_DLLResultat=-2;

                m_pGetStopAccel= (PGetStopAccel) GetProcAddress(m_LStep4,"LSX_GetStopAccel");
                if (!m_pGetStopAccel) m_DLLResultat=-2;

               m_pGetVersionStrInfo= (PGetVersionStrInfo) GetProcAddress(m_LStep4,"LSX_GetVersionStrInfo");
                if (!m_pGetVersionStrInfo) m_DLLResultat=-2;

                m_pGetStatusLimit= (PGetStatusLimit) GetProcAddress(m_LStep4,"LSX_GetStatusLimit");
                if (!m_pGetStatusLimit) m_DLLResultat=-2;

                m_pGetSecurityErr= (PGetSecurityErr) GetProcAddress(m_LStep4,"LSX_GetSecurityErr");
                if (!m_pGetSecurityErr) m_DLLResultat=-2;

                m_pGetSecurityStatus= (PGetSecurityStatus) GetProcAddress(m_LStep4,"LSX_GetSecurityStatus");
                if (!m_pGetSecurityStatus) m_DLLResultat=-2;

                m_pGetDelay= (PGetDelay) GetProcAddress(m_LStep4,"LSX_GetDelay");
                if (!m_pGetDelay) m_DLLResultat=-2;

                m_pGetDistance= (PGetDistance) GetProcAddress(m_LStep4,"LSX_GetDistance");
                if (!m_pGetDistance) m_DLLResultat=-2;

                m_pClearPos= (PClearPos) GetProcAddress(m_LStep4,"LSX_ClearPos");
                if (!m_pClearPos) m_DLLResultat=-2;

                m_pSetDigJoySpeed= (PSetDigJoySpeed) GetProcAddress(m_LStep4,"LSX_SetDigJoySpeed");
                if (!m_pSetDigJoySpeed) m_DLLResultat=-2;

                m_pGetDigJoySpeed= (PGetDigJoySpeed) GetProcAddress(m_LStep4,"LSX_GetDigJoySpeed");
                if (!m_pGetDigJoySpeed) m_DLLResultat=-2;

                m_pGetJoystickDir= (PGetJoystickDir) GetProcAddress(m_LStep4,"LSX_GetJoystickDir");
                if (!m_pGetJoystickDir) m_DLLResultat=-2;

                m_pGetJoystick= (PGetJoystick) GetProcAddress(m_LStep4,"LSX_GetJoystick");
                if (!m_pGetJoystick) m_DLLResultat=-2;

                m_pGetJoystickWindow= (PGetJoystickWindow) GetProcAddress(m_LStep4,"LSX_GetJoystickWindow");
                if (!m_pGetJoystickWindow) m_DLLResultat=-2;

                m_pGetHandWheel= (PGetHandWheel) GetProcAddress(m_LStep4,"LSX_GetHandWheel");
                if (!m_pGetHandWheel) m_DLLResultat=-2;

                m_pGetBPZ= (PGetBPZ) GetProcAddress(m_LStep4,"LSX_GetBPZ");
                if (!m_pGetBPZ) m_DLLResultat=-2;

                m_pGetBPZTrackballFactor= (PGetBPZTrackballFactor) GetProcAddress(m_LStep4,"LSX_GetBPZTrackballFactor");
                if (!m_pGetBPZTrackballFactor) m_DLLResultat=-2;

               m_pGetBPZTrackballBackLash= (PGetBPZTrackballBackLash) GetProcAddress(m_LStep4,"LSX_GetBPZTrackballBackLash");
                if (!m_pGetBPZTrackballBackLash) m_DLLResultat=-2;

                m_pGetBPZJoyspeed= (PGetBPZJoyspeed) GetProcAddress(m_LStep4,"LSX_GetBPZJoyspeed");
                if (!m_pGetBPZJoyspeed) m_DLLResultat=-2;

                m_pGetLimitControl= (PGetLimitControl) GetProcAddress(m_LStep4,"LSX_GetLimitControl");
                if (!m_pGetLimitControl) m_DLLResultat=-2;

                m_pSetAutoLimitAfterCalibRM= (PSetAutoLimitAfterCalibRM) GetProcAddress(m_LStep4,"LSX_SetAutoLimitAfterCalibRM");
                if (!m_pSetAutoLimitAfterCalibRM) m_DLLResultat=-2;

                m_pGetAutoLimitAfterCalibRM= (PGetAutoLimitAfterCalibRM) GetProcAddress(m_LStep4,"LSX_GetAutoLimitAfterCalibRM");
                if (!m_pGetAutoLimitAfterCalibRM) m_DLLResultat=-2;

                m_pGetSwitchPolarity= (PGetSwitchPolarity) GetProcAddress(m_LStep4,"LSX_GetSwitchPolarity");
                if (!m_pGetSwitchPolarity) m_DLLResultat=-2;

                m_pGetSwitchActive= (PGetSwitchActive) GetProcAddress(m_LStep4,"LSX_GetSwitchActive");
                if (!m_pGetSwitchActive) m_DLLResultat=-2;

                m_pGetTVRMode= (PGetTVRMode) GetProcAddress(m_LStep4,"LSX_GetTVRMode");
                if (!m_pGetTVRMode) m_DLLResultat=-2;

                m_pGetFactorTVR= (PGetFactorTVR) GetProcAddress(m_LStep4,"LSX_GetFactorTVR");
                if (!m_pGetFactorTVR) m_DLLResultat=-2;

                m_pSetTVROutMode= (PSetTVROutMode) GetProcAddress(m_LStep4,"LSX_SetTVROutMode");
                if (!m_pSetTVROutMode) m_DLLResultat=-2;

                m_pGetTVROutMode= (PGetTVROutMode) GetProcAddress(m_LStep4,"LSX_GetTVROutMode");
                if (!m_pGetTVROutMode) m_DLLResultat=-2;

                m_pSetTVROutResolution= (PSetTVROutResolution) GetProcAddress(m_LStep4,"LSX_SetTVROutResolution");
                if (!m_pSetTVROutResolution) m_DLLResultat=-2;

                m_pGetTVROutResolution= (PGetTVROutResolution) GetProcAddress(m_LStep4,"LSX_GetTVROutResolution");
                if (!m_pGetTVROutResolution) m_DLLResultat=-2;

                m_pSetTVROutPitch= (PSetTVROutPitch) GetProcAddress(m_LStep4,"LSX_SetTVROutPitch");
                if (!m_pSetTVROutPitch) m_DLLResultat=-2;

                m_pGetTVROutPitch= (PGetTVROutPitch) GetProcAddress(m_LStep4,"LSX_GetTVROutPitch");
                if (!m_pGetTVROutPitch) m_DLLResultat=-2;

                m_pSetVelTVRO= (PSetVelTVRO) GetProcAddress(m_LStep4,"LSX_SetVelTVRO");
                if (!m_pSetVelTVRO) m_DLLResultat=-2;

                m_pGetVelTVRO= (PGetVelTVRO) GetProcAddress(m_LStep4,"LSX_GetVelTVRO");
                if (!m_pGetVelTVRO) m_DLLResultat=-2;

                m_pSetAccelTVRO= (PSetAccelTVRO) GetProcAddress(m_LStep4,"LSX_SetAccelTVRO");
                if (!m_pSetAccelTVRO) m_DLLResultat=-2;

                m_pGetAccelTVRO= (PGetAccelTVRO) GetProcAddress(m_LStep4,"LSX_GetAccelTVRO");
                if (!m_pGetAccelTVRO) m_DLLResultat=-2;

                m_pSetVelSingleAxisTVRO= (PSetVelSingleAxisTVRO) GetProcAddress(m_LStep4,"LSX_SetVelSingleAxisTVRO");
                if (!m_pSetVelSingleAxisTVRO) m_DLLResultat=-2;

                m_pSetAccelSingleAxisTVRO= (PSetAccelSingleAxisTVRO) GetProcAddress(m_LStep4,"LSX_SetAccelSingleAxisTVRO");
                if (!m_pSetAccelSingleAxisTVRO) m_DLLResultat=-2;

                m_pSetPosTVRO= (PSetPosTVRO) GetProcAddress(m_LStep4,"LSX_SetPosTVRO");
                if (!m_pSetPosTVRO) m_DLLResultat=-2;

                m_pGetPosTVRO= (PGetPosTVRO) GetProcAddress(m_LStep4,"LSX_GetPosTVRO");
                if (!m_pGetPosTVRO) m_DLLResultat=-2;

                m_pMoveAbsTVRO= (PMoveAbsTVRO) GetProcAddress(m_LStep4,"LSX_MoveAbsTVRO");
                if (!m_pMoveAbsTVRO) m_DLLResultat=-2;

                m_pMoveRelTVRO= (PMoveRelTVRO) GetProcAddress(m_LStep4,"LSX_MoveRelTVRO");
                if (!m_pMoveRelTVRO) m_DLLResultat=-2;

                m_pMoveRelTVROSingleAxis= (PMoveRelTVROSingleAxis) GetProcAddress(m_LStep4,"LSX_MoveRelTVROSingleAxis");
                if (!m_pMoveRelTVROSingleAxis) m_DLLResultat=-2;

                m_pMoveAbsTVROSingleAxis= (PMoveAbsTVROSingleAxis) GetProcAddress(m_LStep4,"LSX_MoveAbsTVROSingleAxis");
                if (!m_pMoveAbsTVROSingleAxis) m_DLLResultat=-2;

                m_pGetStatusTVRO= (PGetStatusTVRO) GetProcAddress(m_LStep4,"LSX_GetStatusTVRO");
                if (!m_pGetStatusTVRO) m_DLLResultat=-2;

                m_pGetTargetWindow= (PGetTargetWindow) GetProcAddress(m_LStep4,"LSX_GetTargetWindow");
                if (!m_pGetTargetWindow) m_DLLResultat=-2;

                m_pGetEncoderPeriod= (PGetEncoderPeriod) GetProcAddress(m_LStep4,"LSX_GetEncoderPeriod");
                if (!m_pGetEncoderPeriod) m_DLLResultat=-2;

                m_pGetEncoderRefSignal= (PGetEncoderRefSignal) GetProcAddress(m_LStep4,"LSX_GetEncoderRefSignal");
                if (!m_pGetEncoderRefSignal) m_DLLResultat=-2;

                m_pGetEncoderPosition= (PGetEncoderPosition) GetProcAddress(m_LStep4,"LSX_GetEncoderPosition");
                if (!m_pGetEncoderPosition) m_DLLResultat=-2;

                m_pGetEncoderActive= (PGetEncoderActive) GetProcAddress(m_LStep4,"LSX_GetEncoderActive");
                if (!m_pGetEncoderActive) m_DLLResultat=-2;

                m_pGetController= (PGetController) GetProcAddress(m_LStep4,"LSX_GetController");
                if (!m_pGetController) m_DLLResultat=-2;

                m_pGetControllerCall= (PGetControllerCall) GetProcAddress(m_LStep4,"LSX_GetControllerCall");
                if (!m_pGetControllerCall) m_DLLResultat=-2;

                m_pGetControllerSteps= (PGetControllerSteps) GetProcAddress(m_LStep4,"LSX_GetControllerSteps");
                if (!m_pGetControllerSteps) m_DLLResultat=-2;

                m_pGetControllerFactor= (PGetControllerFactor) GetProcAddress(m_LStep4,"LSX_GetControllerFactor");
                if (!m_pGetControllerFactor) m_DLLResultat=-2;

                m_pGetControllerTWDelay= (PGetControllerTWDelay) GetProcAddress(m_LStep4,"LSX_GetControllerTWDelay");
                if (!m_pGetControllerTWDelay) m_DLLResultat=-2;

                m_pGetControllerTimeout= (PGetControllerTimeout) GetProcAddress(m_LStep4,"LSX_GetControllerTimeout");
                if (!m_pGetControllerTimeout) m_DLLResultat=-2;

                m_pSetCtrFastMoveOn= (PSetCtrFastMoveOn) GetProcAddress(m_LStep4,"LSX_SetCtrFastMoveOn");
                if (!m_pSetCtrFastMoveOn) m_DLLResultat=-2;

                m_pSetCtrFastMoveOff= (PSetCtrFastMoveOff) GetProcAddress(m_LStep4,"LSX_SetCtrFastMoveOff");
                if (!m_pSetCtrFastMoveOff) m_DLLResultat=-2;

                m_pGetCtrFastMove= (PGetCtrFastMove) GetProcAddress(m_LStep4,"LSX_GetCtrFastMove");
                if (!m_pGetCtrFastMove) m_DLLResultat=-2;

                m_pClearCtrFastMoveCounter= (PClearCtrFastMoveCounter) GetProcAddress(m_LStep4,"LSX_ClearCtrFastMoveCounter");
                if (!m_pClearCtrFastMoveCounter) m_DLLResultat=-2;

                m_pGetCtrFastMoveCounter= (PGetCtrFastMoveCounter) GetProcAddress(m_LStep4,"LSX_GetCtrFastMoveCounter");
                if (!m_pGetCtrFastMoveCounter) m_DLLResultat=-2;

                m_pClearEncoder= (PClearEncoder) GetProcAddress(m_LStep4,"LSX_ClearEncoder");
                if (!m_pClearEncoder) m_DLLResultat=-2;

                m_pGetEncoder= (PGetEncoder) GetProcAddress(m_LStep4,"LSX_GetEncoder");
                if (!m_pGetEncoder) m_DLLResultat=-2;

                m_pGetTrigger= (PGetTrigger) GetProcAddress(m_LStep4,"LSX_GetTrigger");
                if (!m_pGetTrigger) m_DLLResultat=-2;

                m_pGetTriggerPar= (PGetTriggerPar) GetProcAddress(m_LStep4,"LSX_GetTriggerPar");
                if (!m_pGetTriggerPar) m_DLLResultat=-2;

                m_pSetTrigCount= (PSetTrigCount) GetProcAddress(m_LStep4,"LSX_SetTrigCount");
                if (!m_pSetTrigCount) m_DLLResultat=-2;

                m_pGetTrigCount= (PGetTrigCount) GetProcAddress(m_LStep4,"LSX_GetTrigCount");
                if (!m_pGetTrigCount) m_DLLResultat=-2;

                m_pSetTVRInPulse= (PSetTVRInPulse) GetProcAddress(m_LStep4,"LSX_SetTVRInPulse");
                if (!m_pSetTVRInPulse) m_DLLResultat=-2;

                m_pGetSnapshot= (PGetSnapshot) GetProcAddress(m_LStep4,"LSX_GetSnapshot");
                if (!m_pGetSnapshot) m_DLLResultat=-2;

                m_pSetSnapshotFilter= (PSetSnapshotFilter) GetProcAddress(m_LStep4,"LSX_SetSnapshotFilter");
                if (!m_pSetSnapshotFilter) m_DLLResultat=-2;

               m_pGetSnapshotFilter= (PGetSnapshotFilter) GetProcAddress(m_LStep4,"LSX_GetSnapshotFilter");
                if (!m_pGetSnapshotFilter) m_DLLResultat=-2;

               m_pGetSnapshotPar= (PGetSnapshotPar) GetProcAddress(m_LStep4,"LSX_GetSnapshotPar");
                if (!m_pGetSnapshotPar) m_DLLResultat=-2;

                m_pGetLimit= (PGetLimit) GetProcAddress(m_LStep4,"LSX_GetLimit");
                if (!m_pGetLimit) m_DLLResultat=-2;

                m_pJoyChangeAxis= (PJoyChangeAxis) GetProcAddress(m_LStep4,"LSX_JoyChangeAxis");
                if (!m_pJoyChangeAxis) m_DLLResultat=-2;

                m_pGetJoyChangeAxis= (PGetJoyChangeAxis) GetProcAddress(m_LStep4,"LSX_GetJoyChangeAxis");
                if (!m_pGetJoyChangeAxis) m_DLLResultat=-2;

                m_pCreateLSID= (PCreateLSID) GetProcAddress(m_LStep4,"LSX_CreateLSID");
                if (!m_pCreateLSID) m_DLLResultat=-2;

                m_pFreeLSID= (PFreeLSID) GetProcAddress(m_LStep4,"LSX_FreeLSID");
                if (!m_pFreeLSID) m_DLLResultat=-2;

                //if (!m_DLLResultat) m_pCreateLSID(&m_LSID);
				try{
				if (!m_DLLResultat) this->m_pCreateLSID(&m_LSID);
				}
				catch(...){
				DWORD error=GetLastError();
				//VTMessageBox("Error", "VT5", MB_OK);
				}
				

			///////SERGEY END


               /* VERIFY(m_pConnect= (PConnect) GetProcAddress(m_LStep4,"LSX_Connect"));
                if (!m_pConnect) m_DLLResultat=-2;

                VERIFY(m_pConnectSimple= (PConnectSimple) GetProcAddress(m_LStep4,"LSX_ConnectSimple"));
                if (!m_pConnectSimple) m_DLLResultat=-2;

                VERIFY(m_pConnectEx= (PConnectEx) GetProcAddress(m_LStep4,"LSX_ConnectEx"));
                if (!m_pConnectEx) m_DLLResultat=-2;

                VERIFY(m_pDisconnect= (PDisconnect) GetProcAddress(m_LStep4,"LSX_Disconnect"));
                if (!m_pDisconnect) m_DLLResultat=-2;

                VERIFY(m_pLoadConfig= (PLoadConfig) GetProcAddress(m_LStep4,"LSX_LoadConfig"));
                if (!m_pLoadConfig) m_DLLResultat=-2;

                VERIFY(m_pSaveConfig= (PSaveConfig) GetProcAddress(m_LStep4,"LSX_SaveConfig"));
                if (!m_pSaveConfig) m_DLLResultat=-2;

                VERIFY(m_pMoveAbs= (PMoveAbs) GetProcAddress(m_LStep4,"LSX_MoveAbs"));
                if (!m_pMoveAbs) m_DLLResultat=-2;

                VERIFY(m_pMoveRel= (PMoveRel) GetProcAddress(m_LStep4,"LSX_MoveRel"));
                if (!m_pMoveRel) m_DLLResultat=-2;

                VERIFY(m_pCalibrate= (PCalibrate) GetProcAddress(m_LStep4,"LSX_Calibrate"));
                if (!m_pCalibrate) m_DLLResultat=-2;

                VERIFY(m_pRMeasure= (PRMeasure) GetProcAddress(m_LStep4,"LSX_RMeasure"));
                if (!m_pRMeasure) m_DLLResultat=-2;

                VERIFY(m_pSetPitch= (PSetPitch) GetProcAddress(m_LStep4,"LSX_SetPitch"));
                if (!m_pSetPitch) m_DLLResultat=-2;

                VERIFY(m_pSetPos= (PSetPos) GetProcAddress(m_LStep4,"LSX_SetPos"));
                if (!m_pSetPos) m_DLLResultat=-2;

                VERIFY(m_pSetActiveAxes= (PSetActiveAxes) GetProcAddress(m_LStep4,"LSX_SetActiveAxes"));
                if (!m_pSetActiveAxes) m_DLLResultat=-2;

                VERIFY(m_pSetVel= (PSetVel) GetProcAddress(m_LStep4,"LSX_SetVel"));
                if (!m_pSetVel) m_DLLResultat=-2;

                VERIFY(m_pSetAccel= (PSetAccel) GetProcAddress(m_LStep4,"LSX_SetAccel"));
                if (!m_pSetAccel) m_DLLResultat=-2;

                VERIFY(m_pSetReduction= (PSetReduction) GetProcAddress(m_LStep4,"LSX_SetReduction"));
                if (!m_pSetReduction) m_DLLResultat=-2;

                VERIFY(m_pSetDelay= (PSetDelay) GetProcAddress(m_LStep4,"LSX_SetDelay"));
                if (!m_pSetDelay) m_DLLResultat=-2;

                VERIFY(m_pSetSwitchPolarity= (PSetSwitchPolarity) GetProcAddress(m_LStep4,"LSX_SetSwitchPolarity"));
                if (!m_pSetSwitchPolarity) m_DLLResultat=-2;

                VERIFY(m_pSetSwitchActive= (PSetSwitchActive) GetProcAddress(m_LStep4,"LSX_SetSwitchActive"));
                if (!m_pSetSwitchActive) m_DLLResultat=-2;

                VERIFY(m_pSetJoystickOn= (PSetJoystickOn) GetProcAddress(m_LStep4,"LSX_SetJoystickOn"));
                if (!m_pSetJoystickOn) m_DLLResultat=-2;

                VERIFY(m_pSetJoystickOff= (PSetJoystickOff) GetProcAddress(m_LStep4,"LSX_SetJoystickOff"));
                if (!m_pSetJoystickOff) m_DLLResultat=-2;

                VERIFY(m_pSoftwareReset= (PSoftwareReset) GetProcAddress(m_LStep4,"LSX_SoftwareReset"));
                if (!m_pSoftwareReset) m_DLLResultat=-2;

                VERIFY(m_pSetDigitalOutput= (PSetDigitalOutput) GetProcAddress(m_LStep4,"LSX_SetDigitalOutput"));
                if (!m_pSetDigitalOutput) m_DLLResultat=-2;

                VERIFY(m_pGetDigitalInputs= (PGetDigitalInputs) GetProcAddress(m_LStep4,"LSX_GetDigitalInputs"));
                if (!m_pGetDigitalInputs) m_DLLResultat=-2;

                VERIFY(m_pSetAnalogOutput= (PSetAnalogOutput) GetProcAddress(m_LStep4,"LSX_SetAnalogOutput"));
                if (!m_pSetAnalogOutput) m_DLLResultat=-2;

                VERIFY(m_pGetAnalogInput= (PGetAnalogInput) GetProcAddress(m_LStep4,"LSX_GetAnalogInput"));
                if (!m_pGetAnalogInput) m_DLLResultat=-2;

                VERIFY(m_pSetLimit= (PSetLimit) GetProcAddress(m_LStep4,"LSX_SetLimit"));
                if (!m_pSetLimit) m_DLLResultat=-2;

                VERIFY(m_pSetLimitControl= (PSetLimitControl) GetProcAddress(m_LStep4,"LSX_SetLimitControl"));
                if (!m_pSetLimitControl) m_DLLResultat=-2;

                VERIFY(m_pGetPos= (PGetPos) GetProcAddress(m_LStep4,"LSX_GetPos"));
                if (!m_pGetPos) m_DLLResultat=-2;

                VERIFY(m_pGetStatus= (PGetStatus) GetProcAddress(m_LStep4,"LSX_GetStatus"));
                if (!m_pGetStatus) m_DLLResultat=-2;

                VERIFY(m_pGetEncoderMask= (PGetEncoderMask) GetProcAddress(m_LStep4,"LSX_GetEncoderMask"));
                if (!m_pGetEncoderMask) m_DLLResultat=-2;

                VERIFY(m_pStopAxes= (PStopAxes) GetProcAddress(m_LStep4,"LSX_StopAxes"));
                if (!m_pStopAxes) m_DLLResultat=-2;

                VERIFY(m_pSetAbortFlag= (PSetAbortFlag) GetProcAddress(m_LStep4,"LSX_SetAbortFlag"));
                if (!m_pSetAbortFlag) m_DLLResultat=-2;

                VERIFY(m_pMoveRelSingleAxis= (PMoveRelSingleAxis) GetProcAddress(m_LStep4,"LSX_MoveRelSingleAxis"));
                if (!m_pMoveRelSingleAxis) m_DLLResultat=-2;

                VERIFY(m_pMoveAbsSingleAxis= (PMoveAbsSingleAxis) GetProcAddress(m_LStep4,"LSX_MoveAbsSingleAxis"));
                if (!m_pMoveAbsSingleAxis) m_DLLResultat=-2;

                VERIFY(m_pSetControlPars= (PSetControlPars) GetProcAddress(m_LStep4,"LSX_SetControlPars"));
                if (!m_pSetControlPars) m_DLLResultat=-2;

                VERIFY(m_pSetMotorCurrent= (PSetMotorCurrent) GetProcAddress(m_LStep4,"LSX_SetMotorCurrent"));
                if (!m_pSetMotorCurrent) m_DLLResultat=-2;

                VERIFY(m_pSetVelSingleAxis= (PSetVelSingleAxis) GetProcAddress(m_LStep4,"LSX_SetVelSingleAxis"));
                if (!m_pSetVelSingleAxis) m_DLLResultat=-2;

                VERIFY(m_pSetAccelSingleAxis= (PSetAccelSingleAxis) GetProcAddress(m_LStep4,"LSX_SetAccelSingleAxis"));
                if (!m_pSetAccelSingleAxis) m_DLLResultat=-2;

                VERIFY(m_pCalibrateEx= (PCalibrateEx) GetProcAddress(m_LStep4,"LSX_CalibrateEx"));
                if (!m_pCalibrateEx) m_DLLResultat=-2;

                VERIFY(m_pRMeasureEx= (PRMeasureEx) GetProcAddress(m_LStep4,"LSX_RMeasureEx"));
                if (!m_pRMeasureEx) m_DLLResultat=-2;

                VERIFY(m_pSetShowProt= (PSetShowProt) GetProcAddress(m_LStep4,"LSX_SetShowProt"));
                if (!m_pSetShowProt) m_DLLResultat=-2;

                VERIFY(m_pGetAnalogInputs2= (PGetAnalogInputs2) GetProcAddress(m_LStep4,"LSX_GetAnalogInputs2"));
                if (!m_pGetAnalogInputs2) m_DLLResultat=-2;

                VERIFY(m_pSendString= (PSendString) GetProcAddress(m_LStep4,"LSX_SendString"));
                if (!m_pSendString) m_DLLResultat=-2;

                VERIFY(m_pSetSpeedPoti= (PSetSpeedPoti) GetProcAddress(m_LStep4,"LSX_SetSpeedPoti"));
                if (!m_pSetSpeedPoti) m_DLLResultat=-2;

                VERIFY(m_pSetTVRMode= (PSetTVRMode) GetProcAddress(m_LStep4,"LSX_SetTVRMode"));
                if (!m_pSetTVRMode) m_DLLResultat=-2;

                VERIFY(m_pSetAutoStatus= (PSetAutoStatus) GetProcAddress(m_LStep4,"LSX_SetAutoStatus"));
                if (!m_pSetAutoStatus) m_DLLResultat=-2;

                VERIFY(m_pGetStatusAxis= (PGetStatusAxis) GetProcAddress(m_LStep4,"LSX_GetStatusAxis"));
                if (!m_pGetStatusAxis) m_DLLResultat=-2;

                VERIFY(m_pSetDigIO_Off= (PSetDigIO_Off) GetProcAddress(m_LStep4,"LSX_SetDigIO_Off"));
                if (!m_pSetDigIO_Off) m_DLLResultat=-2;

                VERIFY(m_pSetDigIO_Polarity= (PSetDigIO_Polarity) GetProcAddress(m_LStep4,"LSX_SetDigIO_Polarity"));
                if (!m_pSetDigIO_Polarity) m_DLLResultat=-2;

                VERIFY(m_pSetDigIO_EmergencyStop= (PSetDigIO_EmergencyStop) GetProcAddress(m_LStep4,"LSX_SetDigIO_EmergencyStop"));
                if (!m_pSetDigIO_EmergencyStop) m_DLLResultat=-2;

                VERIFY(m_pSetDigIO_Distance= (PSetDigIO_Distance) GetProcAddress(m_LStep4,"LSX_SetDigIO_Distance"));
                if (!m_pSetDigIO_Distance) m_DLLResultat=-2;

                VERIFY(m_pSetDimensions= (PSetDimensions) GetProcAddress(m_LStep4,"LSX_SetDimensions"));
                if (!m_pSetDimensions) m_DLLResultat=-2;

                VERIFY(m_pMoveRelShort= (PMoveRelShort) GetProcAddress(m_LStep4,"LSX_MoveRelShort"));
                if (!m_pMoveRelShort) m_DLLResultat=-2;

                VERIFY(m_pSetEncoderPeriod= (PSetEncoderPeriod) GetProcAddress(m_LStep4,"LSX_SetEncoderPeriod"));
                if (!m_pSetEncoderPeriod) m_DLLResultat=-2;

                VERIFY(m_pSetJoystickDir= (PSetJoystickDir) GetProcAddress(m_LStep4,"LSX_SetJoystickDir"));
                if (!m_pSetJoystickDir) m_DLLResultat=-2;

                VERIFY(m_pSetEncoderMask= (PSetEncoderMask) GetProcAddress(m_LStep4,"LSX_SetEncoderMask"));
                if (!m_pSetEncoderMask) m_DLLResultat=-2;

                VERIFY(m_pSetGear= (PSetGear) GetProcAddress(m_LStep4,"LSX_SetGear"));
                if (!m_pSetGear) m_DLLResultat=-2;

                VERIFY(m_pSetHandWheelOn= (PSetHandWheelOn) GetProcAddress(m_LStep4,"LSX_SetHandWheelOn"));
                if (!m_pSetHandWheelOn) m_DLLResultat=-2;

                VERIFY(m_pSetHandWheelOff= (PSetHandWheelOff) GetProcAddress(m_LStep4,"LSX_SetHandWheelOff"));
                if (!m_pSetHandWheelOff) m_DLLResultat=-2;

                VERIFY(m_pSetFactorTVR= (PSetFactorTVR) GetProcAddress(m_LStep4,"LSX_SetFactorTVR"));
                if (!m_pSetFactorTVR) m_DLLResultat=-2;

                VERIFY(m_pSetTargetWindow= (PSetTargetWindow) GetProcAddress(m_LStep4,"LSX_SetTargetWindow"));
                if (!m_pSetTargetWindow) m_DLLResultat=-2;

                VERIFY(m_pSetController= (PSetController) GetProcAddress(m_LStep4,"LSX_SetController"));
                if (!m_pSetController) m_DLLResultat=-2;

                VERIFY(m_pSetControllerCall= (PSetControllerCall) GetProcAddress(m_LStep4,"LSX_SetControllerCall"));
                if (!m_pSetControllerCall) m_DLLResultat=-2;

                VERIFY(m_pSetControllerSteps= (PSetControllerSteps) GetProcAddress(m_LStep4,"LSX_SetControllerSteps"));
                if (!m_pSetControllerSteps) m_DLLResultat=-2;

                VERIFY(m_pSetControllerFactor= (PSetControllerFactor) GetProcAddress(m_LStep4,"LSX_SetControllerFactor"));
                if (!m_pSetControllerFactor) m_DLLResultat=-2;

                VERIFY(m_pSetControllerTWDelay= (PSetControllerTWDelay) GetProcAddress(m_LStep4,"LSX_SetControllerTWDelay"));
                if (!m_pSetControllerTWDelay) m_DLLResultat=-2;

                VERIFY(m_pSetEncoderRefSignal= (PSetEncoderRefSignal) GetProcAddress(m_LStep4,"LSX_SetEncoderRefSignal"));
                if (!m_pSetEncoderRefSignal) m_DLLResultat=-2;

                VERIFY(m_pSetEncoderPosition= (PSetEncoderPosition) GetProcAddress(m_LStep4,"LSX_SetEncoderPosition"));
                if (!m_pSetEncoderPosition) m_DLLResultat=-2;

                VERIFY(m_pGetVersionStr= (PGetVersionStr) GetProcAddress(m_LStep4,"LSX_GetVersionStr"));
                if (!m_pGetVersionStr) m_DLLResultat=-2;

                VERIFY(m_pGetError= (PGetError) GetProcAddress(m_LStep4,"LSX_GetError"));
                if (!m_pGetError) m_DLLResultat=-2;

                VERIFY(m_pGetPosSingleAxis= (PGetPosSingleAxis) GetProcAddress(m_LStep4,"LSX_GetPosSingleAxis"));
                if (!m_pGetPosSingleAxis) m_DLLResultat=-2;

                VERIFY(m_pSetDistance= (PSetDistance) GetProcAddress(m_LStep4,"LSX_SetDistance"));
                if (!m_pSetDistance) m_DLLResultat=-2;

                VERIFY(m_pGetPosEx= (PGetPosEx) GetProcAddress(m_LStep4,"LSX_GetPosEx"));
                if (!m_pGetPosEx) m_DLLResultat=-2;

                VERIFY(m_pSetShowCmdList= (PSetShowCmdList) GetProcAddress(m_LStep4,"LSX_SetShowCmdList"));
                if (!m_pSetShowCmdList) m_DLLResultat=-2;

                VERIFY(m_pSetWriteLogText= (PSetWriteLogText) GetProcAddress(m_LStep4,"LSX_SetWriteLogText"));
                if (!m_pSetWriteLogText) m_DLLResultat=-2;

                VERIFY(m_pSetControllerTimeout= (PSetControllerTimeout) GetProcAddress(m_LStep4,"LSX_SetControllerTimeout"));
                if (!m_pSetControllerTimeout) m_DLLResultat=-2;

                VERIFY(m_pSetEncoderActive= (PSetEncoderActive) GetProcAddress(m_LStep4,"LSX_SetEncoderActive"));
                if (!m_pSetEncoderActive) m_DLLResultat=-2;

                VERIFY(m_pGetSnapshotCount= (PGetSnapshotCount) GetProcAddress(m_LStep4,"LSX_GetSnapshotCount"));
                if (!m_pGetSnapshotCount) m_DLLResultat=-2;

                VERIFY(m_pGetSnapshotPos= (PGetSnapshotPos) GetProcAddress(m_LStep4,"LSX_GetSnapshotPos"));
                if (!m_pGetSnapshotPos) m_DLLResultat=-2;

                VERIFY(m_pSetCorrTblOff= (PSetCorrTblOff) GetProcAddress(m_LStep4,"LSX_SetCorrTblOff"));
                if (!m_pSetCorrTblOff) m_DLLResultat=-2;

                VERIFY(m_pSetCorrTblOn= (PSetCorrTblOn) GetProcAddress(m_LStep4,"LSX_SetCorrTblOn"));
                if (!m_pSetCorrTblOn) m_DLLResultat=-2;

                VERIFY(m_pSetFactorMode= (PSetFactorMode) GetProcAddress(m_LStep4,"LSX_SetFactorMode"));
                if (!m_pSetFactorMode) m_DLLResultat=-2;

                VERIFY(m_pSetSnapshot= (PSetSnapshot) GetProcAddress(m_LStep4,"LSX_SetSnapshot"));
                if (!m_pSetSnapshot) m_DLLResultat=-2;

                VERIFY(m_pSetSnapshotPar= (PSetSnapshotPar) GetProcAddress(m_LStep4,"LSX_SetSnapshotPar"));
                if (!m_pSetSnapshotPar) m_DLLResultat=-2;

                VERIFY(m_pSetTrigger= (PSetTrigger) GetProcAddress(m_LStep4,"LSX_SetTrigger"));
                if (!m_pSetTrigger) m_DLLResultat=-2;

                VERIFY(m_pSetTriggerPar= (PSetTriggerPar) GetProcAddress(m_LStep4,"LSX_SetTriggerPar"));
                if (!m_pSetTriggerPar) m_DLLResultat=-2;

                VERIFY(m_pSetLanguage= (PSetLanguage) GetProcAddress(m_LStep4,"LSX_SetLanguage"));
                if (!m_pSetLanguage) m_DLLResultat=-2;

                VERIFY(m_pGetSwitches= (PGetSwitches) GetProcAddress(m_LStep4,"LSX_GetSwitches"));
                if (!m_pGetSwitches) m_DLLResultat=-2;

                VERIFY(m_pGetSerialNr= (PGetSerialNr) GetProcAddress(m_LStep4,"LSX_GetSerialNr"));
                if (!m_pGetSerialNr) m_DLLResultat=-2;

                VERIFY(m_pSetCalibOffset= (PSetCalibOffset) GetProcAddress(m_LStep4,"LSX_SetCalibOffset"));
                if (!m_pSetCalibOffset) m_DLLResultat=-2;

                VERIFY(m_pSetRMOffset= (PSetRMOffset) GetProcAddress(m_LStep4,"LSX_SetRMOffset"));
                if (!m_pSetRMOffset) m_DLLResultat=-2;

                VERIFY(m_pGetSnapshotPosArray= (PGetSnapshotPosArray) GetProcAddress(m_LStep4,"LSX_GetSnapshotPosArray"));
                if (!m_pGetSnapshotPosArray) m_DLLResultat=-2;

                VERIFY(m_pSetAxisDirection= (PSetAxisDirection) GetProcAddress(m_LStep4,"LSX_SetAxisDirection"));
                if (!m_pSetAxisDirection) m_DLLResultat=-2;

                VERIFY(m_pSetCalibrateDir= (PSetCalibrateDir) GetProcAddress(m_LStep4,"LSX_SetCalibrateDir"));
                if (!m_pSetCalibrateDir) m_DLLResultat=-2;

                VERIFY(m_pLStepSave= (PLStepSave) GetProcAddress(m_LStep4,"LSX_LStepSave"));
                if (!m_pLStepSave) m_DLLResultat=-2;

                VERIFY(m_pSetBPZ= (PSetBPZ) GetProcAddress(m_LStep4,"LSX_SetBPZ"));
                if (!m_pSetBPZ) m_DLLResultat=-2;

                VERIFY(m_pSetBPZTrackballFactor= (PSetBPZTrackballFactor) GetProcAddress(m_LStep4,"LSX_SetBPZTrackballFactor"));
                if (!m_pSetBPZTrackballFactor) m_DLLResultat=-2;

                VERIFY(m_pSetBPZTrackballBackLash= (PSetBPZTrackballBackLash) GetProcAddress(m_LStep4,"LSX_SetBPZTrackballBackLash"));
                if (!m_pSetBPZTrackballBackLash) m_DLLResultat=-2;

                VERIFY(m_pSetBPZJoyspeed= (PSetBPZJoyspeed) GetProcAddress(m_LStep4,"LSX_SetBPZJoyspeed"));
                if (!m_pSetBPZJoyspeed) m_DLLResultat=-2;

                VERIFY(m_pSetJoystickWindow= (PSetJoystickWindow) GetProcAddress(m_LStep4,"LSX_SetJoystickWindow"));
                if (!m_pSetJoystickWindow) m_DLLResultat=-2;

                VERIFY(m_pSetCurrentDelay= (PSetCurrentDelay) GetProcAddress(m_LStep4,"LSX_SetCurrentDelay"));
                if (!m_pSetCurrentDelay) m_DLLResultat=-2;

                VERIFY(m_pMoveEx= (PMoveEx) GetProcAddress(m_LStep4,"LSX_MoveEx"));
                if (!m_pMoveEx) m_DLLResultat=-2;

                VERIFY(m_pWaitForAxisStop= (PWaitForAxisStop) GetProcAddress(m_LStep4,"LSX_WaitForAxisStop"));
                if (!m_pWaitForAxisStop) m_DLLResultat=-2;

                VERIFY(m_pSetProcessMessagesProc= (PSetProcessMessagesProc) GetProcAddress(m_LStep4,"LSX_SetProcessMessagesProc"));
                if (!m_pSetProcessMessagesProc) m_DLLResultat=-2;

                VERIFY(m_pSendStringPosCmd= (PSendStringPosCmd) GetProcAddress(m_LStep4,"LSX_SendStringPosCmd"));
                if (!m_pSendStringPosCmd) m_DLLResultat=-2;

                VERIFY(m_pSetDigitalOutputs= (PSetDigitalOutputs) GetProcAddress(m_LStep4,"LSX_SetDigitalOutputs"));
                if (!m_pSetDigitalOutputs) m_DLLResultat=-2;

                VERIFY(m_pSetWriteLogTextFN= (PSetWriteLogTextFN) GetProcAddress(m_LStep4,"LSX_SetWriteLogTextFN"));
                if (!m_pSetWriteLogTextFN) m_DLLResultat=-2;

                VERIFY(m_pSetDigitalOutputsE= (PSetDigitalOutputsE) GetProcAddress(m_LStep4,"LSX_SetDigitalOutputsE"));
                if (!m_pSetDigitalOutputsE) m_DLLResultat=-2;

                VERIFY(m_pGetDigitalInputsE= (PGetDigitalInputsE) GetProcAddress(m_LStep4,"LSX_GetDigitalInputsE"));
                if (!m_pGetDigitalInputsE) m_DLLResultat=-2;

                VERIFY(m_pEnableCommandRetry= (PEnableCommandRetry) GetProcAddress(m_LStep4,"LSX_EnableCommandRetry"));
                if (!m_pEnableCommandRetry) m_DLLResultat=-2;

                VERIFY(m_pSetXYAxisComp= (PSetXYAxisComp) GetProcAddress(m_LStep4,"LSX_SetXYAxisComp"));
                if (!m_pSetXYAxisComp) m_DLLResultat=-2;

                VERIFY(m_pGetVersionStrDet= (PGetVersionStrDet) GetProcAddress(m_LStep4,"LSX_GetVersionStrDet"));
                if (!m_pGetVersionStrDet) m_DLLResultat=-2;

                VERIFY(m_pSetCommandTimeout= (PSetCommandTimeout) GetProcAddress(m_LStep4,"LSX_SetCommandTimeout"));
                if (!m_pSetCommandTimeout) m_DLLResultat=-2;

                VERIFY(m_pSetExtValue= (PSetExtValue) GetProcAddress(m_LStep4,"LSX_SetExtValue"));
                if (!m_pSetExtValue) m_DLLResultat=-2;

                VERIFY(m_pFlushBuffer= (PFlushBuffer) GetProcAddress(m_LStep4,"LSX_FlushBuffer"));
                if (!m_pFlushBuffer) m_DLLResultat=-2;

                VERIFY(m_pGetEEPRomValue= (PGetEEPRomValue) GetProcAddress(m_LStep4,"LSX_GetEEPRomValue"));
                if (!m_pGetEEPRomValue) m_DLLResultat=-2;

                VERIFY(m_pSetEEPRomValue= (PSetEEPRomValue) GetProcAddress(m_LStep4,"LSX_SetEEPRomValue"));
                if (!m_pSetEEPRomValue) m_DLLResultat=-2;

                VERIFY(m_pGetXYAxisComp= (PGetXYAxisComp) GetProcAddress(m_LStep4,"LSX_GetXYAxisComp"));
                if (!m_pGetXYAxisComp) m_DLLResultat=-2;

                VERIFY(m_pGetDimensions= (PGetDimensions) GetProcAddress(m_LStep4,"LSX_GetDimensions"));
                if (!m_pGetDimensions) m_DLLResultat=-2;

                VERIFY(m_pGetPitch= (PGetPitch) GetProcAddress(m_LStep4,"LSX_GetPitch"));
                if (!m_pGetPitch) m_DLLResultat=-2;

                VERIFY(m_pGetGear= (PGetGear) GetProcAddress(m_LStep4,"LSX_GetGear"));
                if (!m_pGetGear) m_DLLResultat=-2;

                VERIFY(m_pGetVel= (PGetVel) GetProcAddress(m_LStep4,"LSX_GetVel"));
                if (!m_pGetVel) m_DLLResultat=-2;

                VERIFY(m_pGetAccel= (PGetAccel) GetProcAddress(m_LStep4,"LSX_GetAccel"));
                if (!m_pGetAccel) m_DLLResultat=-2;

                VERIFY(m_pSetVelFac= (PSetVelFac) GetProcAddress(m_LStep4,"LSX_SetVelFac"));
                if (!m_pSetVelFac) m_DLLResultat=-2;

                VERIFY(m_pGetVelFac= (PGetVelFac) GetProcAddress(m_LStep4,"LSX_GetVelFac"));
                if (!m_pGetVelFac) m_DLLResultat=-2;

                VERIFY(m_pGetSpeedPoti= (PGetSpeedPoti) GetProcAddress(m_LStep4,"LSX_GetSpeedPoti"));
                if (!m_pGetSpeedPoti) m_DLLResultat=-2;

                VERIFY(m_pGetMotorCurrent= (PGetMotorCurrent) GetProcAddress(m_LStep4,"LSX_GetMotorCurrent"));
                if (!m_pGetMotorCurrent) m_DLLResultat=-2;

                VERIFY(m_pGetReduction= (PGetReduction) GetProcAddress(m_LStep4,"LSX_GetReduction"));
                if (!m_pGetReduction) m_DLLResultat=-2;

                VERIFY(m_pGetCurrentDelay= (PGetCurrentDelay) GetProcAddress(m_LStep4,"LSX_GetCurrentDelay"));
                if (!m_pGetCurrentDelay) m_DLLResultat=-2;

                VERIFY(m_pSetOutFuncLev= (PSetOutFuncLev) GetProcAddress(m_LStep4,"LSX_SetOutFuncLev"));
                if (!m_pSetOutFuncLev) m_DLLResultat=-2;

                VERIFY(m_pGetOutFuncLev= (PGetOutFuncLev) GetProcAddress(m_LStep4,"LSX_GetOutFuncLev"));
                if (!m_pGetOutFuncLev) m_DLLResultat=-2;

                VERIFY(m_pGetActiveAxes= (PGetActiveAxes) GetProcAddress(m_LStep4,"LSX_GetActiveAxes"));
                if (!m_pGetActiveAxes) m_DLLResultat=-2;

                VERIFY(m_pGetAxisDirection= (PGetAxisDirection) GetProcAddress(m_LStep4,"LSX_GetAxisDirection"));
                if (!m_pGetAxisDirection) m_DLLResultat=-2;

                VERIFY(m_pGetCalibOffset= (PGetCalibOffset) GetProcAddress(m_LStep4,"LSX_GetCalibOffset"));
                if (!m_pGetCalibOffset) m_DLLResultat=-2;

                VERIFY(m_pGetRMOffset= (PGetRMOffset) GetProcAddress(m_LStep4,"LSX_GetRMOffset"));
                if (!m_pGetRMOffset) m_DLLResultat=-2;

                VERIFY(m_pGetCalibrateDir= (PGetCalibrateDir) GetProcAddress(m_LStep4,"LSX_GetCalibrateDir"));
                if (!m_pGetCalibrateDir) m_DLLResultat=-2;

                VERIFY(m_pSetCalibBackSpeed= (PSetCalibBackSpeed) GetProcAddress(m_LStep4,"LSX_SetCalibBackSpeed"));
                if (!m_pSetCalibBackSpeed) m_DLLResultat=-2;

                VERIFY(m_pGetCalibBackSpeed= (PGetCalibBackSpeed) GetProcAddress(m_LStep4,"LSX_GetCalibBackSpeed"));
                if (!m_pGetCalibBackSpeed) m_DLLResultat=-2;

                VERIFY(m_pSetRefSpeed= (PSetRefSpeed) GetProcAddress(m_LStep4,"LSX_SetRefSpeed"));
                if (!m_pSetRefSpeed) m_DLLResultat=-2;

                VERIFY(m_pGetRefSpeed= (PGetRefSpeed) GetProcAddress(m_LStep4,"LSX_GetRefSpeed"));
                if (!m_pGetRefSpeed) m_DLLResultat=-2;

                VERIFY(m_pSetPowerAmplifier= (PSetPowerAmplifier) GetProcAddress(m_LStep4,"LSX_SetPowerAmplifier"));
                if (!m_pSetPowerAmplifier) m_DLLResultat=-2;

                VERIFY(m_pGetPowerAmplifier= (PGetPowerAmplifier) GetProcAddress(m_LStep4,"LSX_GetPowerAmplifier"));
                if (!m_pGetPowerAmplifier) m_DLLResultat=-2;

                VERIFY(m_pSetMotorTablePatch= (PSetMotorTablePatch) GetProcAddress(m_LStep4,"LSX_SetMotorTablePatch"));
                if (!m_pSetMotorTablePatch) m_DLLResultat=-2;

                VERIFY(m_pGetMotorTablePatch= (PGetMotorTablePatch) GetProcAddress(m_LStep4,"LSX_GetMotorTablePatch"));
                if (!m_pGetMotorTablePatch) m_DLLResultat=-2;

                VERIFY(m_pSetJoystickFilter= (PSetJoystickFilter) GetProcAddress(m_LStep4,"LSX_SetJoystickFilter"));
                if (!m_pSetJoystickFilter) m_DLLResultat=-2;

                VERIFY(m_pGetJoystickFilter= (PGetJoystickFilter) GetProcAddress(m_LStep4,"LSX_GetJoystickFilter"));
                if (!m_pGetJoystickFilter) m_DLLResultat=-2;

                VERIFY(m_pSetStopPolarity= (PSetStopPolarity) GetProcAddress(m_LStep4,"LSX_SetStopPolarity"));
                if (!m_pSetStopPolarity) m_DLLResultat=-2;

                VERIFY(m_pGetStopPolarity= (PGetStopPolarity) GetProcAddress(m_LStep4,"LSX_GetStopPolarity"));
                if (!m_pGetStopPolarity) m_DLLResultat=-2;

                VERIFY(m_pSetVLevel= (PSetVLevel) GetProcAddress(m_LStep4,"LSX_SetVLevel"));
                if (!m_pSetVLevel) m_DLLResultat=-2;

                VERIFY(m_pGetVLevel= (PGetVLevel) GetProcAddress(m_LStep4,"LSX_GetVLevel"));
                if (!m_pGetVLevel) m_DLLResultat=-2;

                VERIFY(m_pSetStopAccel= (PSetStopAccel) GetProcAddress(m_LStep4,"LSX_SetStopAccel"));
                if (!m_pSetStopAccel) m_DLLResultat=-2;

                VERIFY(m_pGetStopAccel= (PGetStopAccel) GetProcAddress(m_LStep4,"LSX_GetStopAccel"));
                if (!m_pGetStopAccel) m_DLLResultat=-2;

                VERIFY(m_pGetVersionStrInfo= (PGetVersionStrInfo) GetProcAddress(m_LStep4,"LSX_GetVersionStrInfo"));
                if (!m_pGetVersionStrInfo) m_DLLResultat=-2;

                VERIFY(m_pGetStatusLimit= (PGetStatusLimit) GetProcAddress(m_LStep4,"LSX_GetStatusLimit"));
                if (!m_pGetStatusLimit) m_DLLResultat=-2;

                VERIFY(m_pGetSecurityErr= (PGetSecurityErr) GetProcAddress(m_LStep4,"LSX_GetSecurityErr"));
                if (!m_pGetSecurityErr) m_DLLResultat=-2;

                VERIFY(m_pGetSecurityStatus= (PGetSecurityStatus) GetProcAddress(m_LStep4,"LSX_GetSecurityStatus"));
                if (!m_pGetSecurityStatus) m_DLLResultat=-2;

                VERIFY(m_pGetDelay= (PGetDelay) GetProcAddress(m_LStep4,"LSX_GetDelay"));
                if (!m_pGetDelay) m_DLLResultat=-2;

                VERIFY(m_pGetDistance= (PGetDistance) GetProcAddress(m_LStep4,"LSX_GetDistance"));
                if (!m_pGetDistance) m_DLLResultat=-2;

                VERIFY(m_pClearPos= (PClearPos) GetProcAddress(m_LStep4,"LSX_ClearPos"));
                if (!m_pClearPos) m_DLLResultat=-2;

                VERIFY(m_pSetDigJoySpeed= (PSetDigJoySpeed) GetProcAddress(m_LStep4,"LSX_SetDigJoySpeed"));
                if (!m_pSetDigJoySpeed) m_DLLResultat=-2;

                VERIFY(m_pGetDigJoySpeed= (PGetDigJoySpeed) GetProcAddress(m_LStep4,"LSX_GetDigJoySpeed"));
                if (!m_pGetDigJoySpeed) m_DLLResultat=-2;

                VERIFY(m_pGetJoystickDir= (PGetJoystickDir) GetProcAddress(m_LStep4,"LSX_GetJoystickDir"));
                if (!m_pGetJoystickDir) m_DLLResultat=-2;

                VERIFY(m_pGetJoystick= (PGetJoystick) GetProcAddress(m_LStep4,"LSX_GetJoystick"));
                if (!m_pGetJoystick) m_DLLResultat=-2;

                VERIFY(m_pGetJoystickWindow= (PGetJoystickWindow) GetProcAddress(m_LStep4,"LSX_GetJoystickWindow"));
                if (!m_pGetJoystickWindow) m_DLLResultat=-2;

                VERIFY(m_pGetHandWheel= (PGetHandWheel) GetProcAddress(m_LStep4,"LSX_GetHandWheel"));
                if (!m_pGetHandWheel) m_DLLResultat=-2;

                VERIFY(m_pGetBPZ= (PGetBPZ) GetProcAddress(m_LStep4,"LSX_GetBPZ"));
                if (!m_pGetBPZ) m_DLLResultat=-2;

                VERIFY(m_pGetBPZTrackballFactor= (PGetBPZTrackballFactor) GetProcAddress(m_LStep4,"LSX_GetBPZTrackballFactor"));
                if (!m_pGetBPZTrackballFactor) m_DLLResultat=-2;

                VERIFY(m_pGetBPZTrackballBackLash= (PGetBPZTrackballBackLash) GetProcAddress(m_LStep4,"LSX_GetBPZTrackballBackLash"));
                if (!m_pGetBPZTrackballBackLash) m_DLLResultat=-2;

                VERIFY(m_pGetBPZJoyspeed= (PGetBPZJoyspeed) GetProcAddress(m_LStep4,"LSX_GetBPZJoyspeed"));
                if (!m_pGetBPZJoyspeed) m_DLLResultat=-2;

                VERIFY(m_pGetLimitControl= (PGetLimitControl) GetProcAddress(m_LStep4,"LSX_GetLimitControl"));
                if (!m_pGetLimitControl) m_DLLResultat=-2;

                VERIFY(m_pSetAutoLimitAfterCalibRM= (PSetAutoLimitAfterCalibRM) GetProcAddress(m_LStep4,"LSX_SetAutoLimitAfterCalibRM"));
                if (!m_pSetAutoLimitAfterCalibRM) m_DLLResultat=-2;

                VERIFY(m_pGetAutoLimitAfterCalibRM= (PGetAutoLimitAfterCalibRM) GetProcAddress(m_LStep4,"LSX_GetAutoLimitAfterCalibRM"));
                if (!m_pGetAutoLimitAfterCalibRM) m_DLLResultat=-2;

                VERIFY(m_pGetSwitchPolarity= (PGetSwitchPolarity) GetProcAddress(m_LStep4,"LSX_GetSwitchPolarity"));
                if (!m_pGetSwitchPolarity) m_DLLResultat=-2;

                VERIFY(m_pGetSwitchActive= (PGetSwitchActive) GetProcAddress(m_LStep4,"LSX_GetSwitchActive"));
                if (!m_pGetSwitchActive) m_DLLResultat=-2;

                VERIFY(m_pGetTVRMode= (PGetTVRMode) GetProcAddress(m_LStep4,"LSX_GetTVRMode"));
                if (!m_pGetTVRMode) m_DLLResultat=-2;

                VERIFY(m_pGetFactorTVR= (PGetFactorTVR) GetProcAddress(m_LStep4,"LSX_GetFactorTVR"));
                if (!m_pGetFactorTVR) m_DLLResultat=-2;

                VERIFY(m_pSetTVROutMode= (PSetTVROutMode) GetProcAddress(m_LStep4,"LSX_SetTVROutMode"));
                if (!m_pSetTVROutMode) m_DLLResultat=-2;

                VERIFY(m_pGetTVROutMode= (PGetTVROutMode) GetProcAddress(m_LStep4,"LSX_GetTVROutMode"));
                if (!m_pGetTVROutMode) m_DLLResultat=-2;

                VERIFY(m_pSetTVROutResolution= (PSetTVROutResolution) GetProcAddress(m_LStep4,"LSX_SetTVROutResolution"));
                if (!m_pSetTVROutResolution) m_DLLResultat=-2;

                VERIFY(m_pGetTVROutResolution= (PGetTVROutResolution) GetProcAddress(m_LStep4,"LSX_GetTVROutResolution"));
                if (!m_pGetTVROutResolution) m_DLLResultat=-2;

                VERIFY(m_pSetTVROutPitch= (PSetTVROutPitch) GetProcAddress(m_LStep4,"LSX_SetTVROutPitch"));
                if (!m_pSetTVROutPitch) m_DLLResultat=-2;

                VERIFY(m_pGetTVROutPitch= (PGetTVROutPitch) GetProcAddress(m_LStep4,"LSX_GetTVROutPitch"));
                if (!m_pGetTVROutPitch) m_DLLResultat=-2;

                VERIFY(m_pSetVelTVRO= (PSetVelTVRO) GetProcAddress(m_LStep4,"LSX_SetVelTVRO"));
                if (!m_pSetVelTVRO) m_DLLResultat=-2;

                VERIFY(m_pGetVelTVRO= (PGetVelTVRO) GetProcAddress(m_LStep4,"LSX_GetVelTVRO"));
                if (!m_pGetVelTVRO) m_DLLResultat=-2;

                VERIFY(m_pSetAccelTVRO= (PSetAccelTVRO) GetProcAddress(m_LStep4,"LSX_SetAccelTVRO"));
                if (!m_pSetAccelTVRO) m_DLLResultat=-2;

                VERIFY(m_pGetAccelTVRO= (PGetAccelTVRO) GetProcAddress(m_LStep4,"LSX_GetAccelTVRO"));
                if (!m_pGetAccelTVRO) m_DLLResultat=-2;

                VERIFY(m_pSetVelSingleAxisTVRO= (PSetVelSingleAxisTVRO) GetProcAddress(m_LStep4,"LSX_SetVelSingleAxisTVRO"));
                if (!m_pSetVelSingleAxisTVRO) m_DLLResultat=-2;

                VERIFY(m_pSetAccelSingleAxisTVRO= (PSetAccelSingleAxisTVRO) GetProcAddress(m_LStep4,"LSX_SetAccelSingleAxisTVRO"));
                if (!m_pSetAccelSingleAxisTVRO) m_DLLResultat=-2;

                VERIFY(m_pSetPosTVRO= (PSetPosTVRO) GetProcAddress(m_LStep4,"LSX_SetPosTVRO"));
                if (!m_pSetPosTVRO) m_DLLResultat=-2;

                VERIFY(m_pGetPosTVRO= (PGetPosTVRO) GetProcAddress(m_LStep4,"LSX_GetPosTVRO"));
                if (!m_pGetPosTVRO) m_DLLResultat=-2;

                VERIFY(m_pMoveAbsTVRO= (PMoveAbsTVRO) GetProcAddress(m_LStep4,"LSX_MoveAbsTVRO"));
                if (!m_pMoveAbsTVRO) m_DLLResultat=-2;

                VERIFY(m_pMoveRelTVRO= (PMoveRelTVRO) GetProcAddress(m_LStep4,"LSX_MoveRelTVRO"));
                if (!m_pMoveRelTVRO) m_DLLResultat=-2;

                VERIFY(m_pMoveRelTVROSingleAxis= (PMoveRelTVROSingleAxis) GetProcAddress(m_LStep4,"LSX_MoveRelTVROSingleAxis"));
                if (!m_pMoveRelTVROSingleAxis) m_DLLResultat=-2;

                VERIFY(m_pMoveAbsTVROSingleAxis= (PMoveAbsTVROSingleAxis) GetProcAddress(m_LStep4,"LSX_MoveAbsTVROSingleAxis"));
                if (!m_pMoveAbsTVROSingleAxis) m_DLLResultat=-2;

                VERIFY(m_pGetStatusTVRO= (PGetStatusTVRO) GetProcAddress(m_LStep4,"LSX_GetStatusTVRO"));
                if (!m_pGetStatusTVRO) m_DLLResultat=-2;

                VERIFY(m_pGetTargetWindow= (PGetTargetWindow) GetProcAddress(m_LStep4,"LSX_GetTargetWindow"));
                if (!m_pGetTargetWindow) m_DLLResultat=-2;

                VERIFY(m_pGetEncoderPeriod= (PGetEncoderPeriod) GetProcAddress(m_LStep4,"LSX_GetEncoderPeriod"));
                if (!m_pGetEncoderPeriod) m_DLLResultat=-2;

                VERIFY(m_pGetEncoderRefSignal= (PGetEncoderRefSignal) GetProcAddress(m_LStep4,"LSX_GetEncoderRefSignal"));
                if (!m_pGetEncoderRefSignal) m_DLLResultat=-2;

                VERIFY(m_pGetEncoderPosition= (PGetEncoderPosition) GetProcAddress(m_LStep4,"LSX_GetEncoderPosition"));
                if (!m_pGetEncoderPosition) m_DLLResultat=-2;

                VERIFY(m_pGetEncoderActive= (PGetEncoderActive) GetProcAddress(m_LStep4,"LSX_GetEncoderActive"));
                if (!m_pGetEncoderActive) m_DLLResultat=-2;

                VERIFY(m_pGetController= (PGetController) GetProcAddress(m_LStep4,"LSX_GetController"));
                if (!m_pGetController) m_DLLResultat=-2;

                VERIFY(m_pGetControllerCall= (PGetControllerCall) GetProcAddress(m_LStep4,"LSX_GetControllerCall"));
                if (!m_pGetControllerCall) m_DLLResultat=-2;

                VERIFY(m_pGetControllerSteps= (PGetControllerSteps) GetProcAddress(m_LStep4,"LSX_GetControllerSteps"));
                if (!m_pGetControllerSteps) m_DLLResultat=-2;

                VERIFY(m_pGetControllerFactor= (PGetControllerFactor) GetProcAddress(m_LStep4,"LSX_GetControllerFactor"));
                if (!m_pGetControllerFactor) m_DLLResultat=-2;

                VERIFY(m_pGetControllerTWDelay= (PGetControllerTWDelay) GetProcAddress(m_LStep4,"LSX_GetControllerTWDelay"));
                if (!m_pGetControllerTWDelay) m_DLLResultat=-2;

                VERIFY(m_pGetControllerTimeout= (PGetControllerTimeout) GetProcAddress(m_LStep4,"LSX_GetControllerTimeout"));
                if (!m_pGetControllerTimeout) m_DLLResultat=-2;

                VERIFY(m_pSetCtrFastMoveOn= (PSetCtrFastMoveOn) GetProcAddress(m_LStep4,"LSX_SetCtrFastMoveOn"));
                if (!m_pSetCtrFastMoveOn) m_DLLResultat=-2;

                VERIFY(m_pSetCtrFastMoveOff= (PSetCtrFastMoveOff) GetProcAddress(m_LStep4,"LSX_SetCtrFastMoveOff"));
                if (!m_pSetCtrFastMoveOff) m_DLLResultat=-2;

                VERIFY(m_pGetCtrFastMove= (PGetCtrFastMove) GetProcAddress(m_LStep4,"LSX_GetCtrFastMove"));
                if (!m_pGetCtrFastMove) m_DLLResultat=-2;

                VERIFY(m_pClearCtrFastMoveCounter= (PClearCtrFastMoveCounter) GetProcAddress(m_LStep4,"LSX_ClearCtrFastMoveCounter"));
                if (!m_pClearCtrFastMoveCounter) m_DLLResultat=-2;

                VERIFY(m_pGetCtrFastMoveCounter= (PGetCtrFastMoveCounter) GetProcAddress(m_LStep4,"LSX_GetCtrFastMoveCounter"));
                if (!m_pGetCtrFastMoveCounter) m_DLLResultat=-2;

                VERIFY(m_pClearEncoder= (PClearEncoder) GetProcAddress(m_LStep4,"LSX_ClearEncoder"));
                if (!m_pClearEncoder) m_DLLResultat=-2;

                VERIFY(m_pGetEncoder= (PGetEncoder) GetProcAddress(m_LStep4,"LSX_GetEncoder"));
                if (!m_pGetEncoder) m_DLLResultat=-2;

                VERIFY(m_pGetTrigger= (PGetTrigger) GetProcAddress(m_LStep4,"LSX_GetTrigger"));
                if (!m_pGetTrigger) m_DLLResultat=-2;

                VERIFY(m_pGetTriggerPar= (PGetTriggerPar) GetProcAddress(m_LStep4,"LSX_GetTriggerPar"));
                if (!m_pGetTriggerPar) m_DLLResultat=-2;

                VERIFY(m_pSetTrigCount= (PSetTrigCount) GetProcAddress(m_LStep4,"LSX_SetTrigCount"));
                if (!m_pSetTrigCount) m_DLLResultat=-2;

                VERIFY(m_pGetTrigCount= (PGetTrigCount) GetProcAddress(m_LStep4,"LSX_GetTrigCount"));
                if (!m_pGetTrigCount) m_DLLResultat=-2;

                VERIFY(m_pSetTVRInPulse= (PSetTVRInPulse) GetProcAddress(m_LStep4,"LSX_SetTVRInPulse"));
                if (!m_pSetTVRInPulse) m_DLLResultat=-2;

                VERIFY(m_pGetSnapshot= (PGetSnapshot) GetProcAddress(m_LStep4,"LSX_GetSnapshot"));
                if (!m_pGetSnapshot) m_DLLResultat=-2;

                VERIFY(m_pSetSnapshotFilter= (PSetSnapshotFilter) GetProcAddress(m_LStep4,"LSX_SetSnapshotFilter"));
                if (!m_pSetSnapshotFilter) m_DLLResultat=-2;

                VERIFY(m_pGetSnapshotFilter= (PGetSnapshotFilter) GetProcAddress(m_LStep4,"LSX_GetSnapshotFilter"));
                if (!m_pGetSnapshotFilter) m_DLLResultat=-2;

                VERIFY(m_pGetSnapshotPar= (PGetSnapshotPar) GetProcAddress(m_LStep4,"LSX_GetSnapshotPar"));
                if (!m_pGetSnapshotPar) m_DLLResultat=-2;

                VERIFY(m_pGetLimit= (PGetLimit) GetProcAddress(m_LStep4,"LSX_GetLimit"));
                if (!m_pGetLimit) m_DLLResultat=-2;

                VERIFY(m_pJoyChangeAxis= (PJoyChangeAxis) GetProcAddress(m_LStep4,"LSX_JoyChangeAxis"));
                if (!m_pJoyChangeAxis) m_DLLResultat=-2;

                VERIFY(m_pGetJoyChangeAxis= (PGetJoyChangeAxis) GetProcAddress(m_LStep4,"LSX_GetJoyChangeAxis"));
                if (!m_pGetJoyChangeAxis) m_DLLResultat=-2;

                VERIFY(m_pCreateLSID= (PCreateLSID) GetProcAddress(m_LStep4,"LSX_CreateLSID"));
                if (!m_pCreateLSID) m_DLLResultat=-2;

                VERIFY(m_pFreeLSID= (PFreeLSID) GetProcAddress(m_LStep4,"LSX_FreeLSID"));
                if (!m_pFreeLSID) m_DLLResultat=-2;

                if (!m_DLLResultat) m_pCreateLSID(&m_LSID);*/
        }
		
}


// Destruktor
CLStep4X::~CLStep4X()
{
	if (m_LStep4!=0)
	{
             m_pFreeLSID(m_LSID);
             FreeLibrary(m_LStep4); // DLL wieder freigeben
	}
}


// gekapselte Funktionen der DLL

int CLStep4X::Connect ()
{
        if (m_pConnect)
        {
                return m_pConnect (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::ConnectSimple (int lAnInterfaceType, char *pcAComName, int lABaudRate, BOOL bAShowProt)
{
        if (m_pConnectSimple)
        {
                return m_pConnectSimple (m_LSID, lAnInterfaceType, pcAComName, lABaudRate, bAShowProt);
        }
        else
                return -1;
}

int CLStep4X::ConnectEx (TLS_ControlInitPar *pAControlInitPar)
{
        if (m_pConnectEx)
        {
                return m_pConnectEx (m_LSID, pAControlInitPar);
        }
        else
                return -1;
}

int CLStep4X::Disconnect ()
{
        if (m_pDisconnect)
        {
                return m_pDisconnect (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::LoadConfig (char *pcFileName)
{
        if (m_pLoadConfig)
        {
                return m_pLoadConfig (m_LSID, pcFileName);
        }
        else
                return -1;
}

int CLStep4X::SaveConfig (char *pcFileName)
{
        if (m_pSaveConfig)
        {
                return m_pSaveConfig (m_LSID, pcFileName);
        }
        else
                return -1;
}

int CLStep4X::MoveAbs (double dX, double dY, double dZ, double dA, BOOL bWait)
{
        if (m_pMoveAbs)
        {
                return m_pMoveAbs (m_LSID, dX, dY, dZ, dA, bWait);
        }
        else
                return -1;
}

int CLStep4X::MoveRel (double dX, double dY, double dZ, double dA, BOOL bWait)
{
        if (m_pMoveRel)
        {
                return m_pMoveRel (m_LSID, dX, dY, dZ, dA, bWait);
        }
        else
                return -1;
}

int CLStep4X::Calibrate ()
{
        if (m_pCalibrate)
        {
                return m_pCalibrate (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::RMeasure ()
{
        if (m_pRMeasure)
        {
                return m_pRMeasure (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::SetPitch (double dX, double dY, double dZ, double dA)
{
        if (m_pSetPitch)
        {
                return m_pSetPitch (m_LSID, dX, dY, dZ, dA);
        }
        else
                return -1;
}

int CLStep4X::SetPos (double dX, double dY, double dZ, double dA)
{
        if (m_pSetPos)
        {
                return m_pSetPos (m_LSID, dX, dY, dZ, dA);
        }
        else
                return -1;
}

int CLStep4X::SetActiveAxes (int lFlags)
{
        if (m_pSetActiveAxes)
        {
                return m_pSetActiveAxes (m_LSID, lFlags);
        }
        else
                return -1;
}

int CLStep4X::SetVel (double dX, double dY, double dZ, double dA)
{
        if (m_pSetVel)
        {
                return m_pSetVel (m_LSID, dX, dY, dZ, dA);
        }
        else
                return -1;
}

int CLStep4X::SetAccel (double dX, double dY, double dZ, double dA)
{
        if (m_pSetAccel)
        {
                return m_pSetAccel (m_LSID, dX, dY, dZ, dA);
        }
        else
                return -1;
}

int CLStep4X::SetReduction (double dX, double dY, double dZ, double dA)
{
        if (m_pSetReduction)
        {
                return m_pSetReduction (m_LSID, dX, dY, dZ, dA);
        }
        else
                return -1;
}

int CLStep4X::SetDelay (int lDelay)
{
        if (m_pSetDelay)
        {
                return m_pSetDelay (m_LSID, lDelay);
        }
        else
                return -1;
}

int CLStep4X::SetSwitchPolarity (int lXP, int lYP, int lZP, int lAP)
{
        if (m_pSetSwitchPolarity)
        {
                return m_pSetSwitchPolarity (m_LSID, lXP, lYP, lZP, lAP);
        }
        else
                return -1;
}

int CLStep4X::SetSwitchActive (int lXA, int lYA, int lZA, int lAA)
{
        if (m_pSetSwitchActive)
        {
                return m_pSetSwitchActive (m_LSID, lXA, lYA, lZA, lAA);
        }
        else
                return -1;
}

int CLStep4X::SetJoystickOn (BOOL bPositionCount, BOOL bEncoder)
{
        if (m_pSetJoystickOn)
        {
                return m_pSetJoystickOn (m_LSID, bPositionCount, bEncoder);
        }
        else
                return -1;
}

int CLStep4X::SetJoystickOff ()
{
        if (m_pSetJoystickOff)
        {
                return m_pSetJoystickOff (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::SoftwareReset ()
{
        if (m_pSoftwareReset)
        {
                return m_pSoftwareReset (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::SetDigitalOutput (int lIndex, BOOL bValue)
{
        if (m_pSetDigitalOutput)
        {
                return m_pSetDigitalOutput (m_LSID, lIndex, bValue);
        }
        else
                return -1;
}

int CLStep4X::GetDigitalInputs (int *plValue)
{
        if (m_pGetDigitalInputs)
        {
                return m_pGetDigitalInputs (m_LSID, plValue);
        }
        else
                return -1;
}

int CLStep4X::SetAnalogOutput (int lIndex, int lValue)
{
        if (m_pSetAnalogOutput)
        {
                return m_pSetAnalogOutput (m_LSID, lIndex, lValue);
        }
        else
                return -1;
}

int CLStep4X::GetAnalogInput (int lIndex, int *plValue)
{
        if (m_pGetAnalogInput)
        {
                return m_pGetAnalogInput (m_LSID, lIndex, plValue);
        }
        else
                return -1;
}

int CLStep4X::SetLimit (int lAxis, double dMinRange, double dMaxRange)
{
        if (m_pSetLimit)
        {
                return m_pSetLimit (m_LSID, lAxis, dMinRange, dMaxRange);
        }
        else
                return -1;
}

int CLStep4X::SetLimitControl (int lAxis, BOOL bActive)
{
        if (m_pSetLimitControl)
        {
                return m_pSetLimitControl (m_LSID, lAxis, bActive);
        }
        else
                return -1;
}

int CLStep4X::GetPos (double *pdX, double *pdY, double *pdZ, double *pdA)
{
        if (m_pGetPos)
        {
                return m_pGetPos (m_LSID, pdX, pdY, pdZ, pdA);
        }
        else
                return -1;
}

int CLStep4X::GetStatus (char *pcStat, int lMaxLen)
{
        if (m_pGetStatus)
        {
                return m_pGetStatus (m_LSID, pcStat, lMaxLen);
        }
        else
                return -1;
}

int CLStep4X::GetEncoderMask (int *plFlags)
{
        if (m_pGetEncoderMask)
        {
                return m_pGetEncoderMask (m_LSID, plFlags);
        }
        else
                return -1;
}

int CLStep4X::StopAxes ()
{
        if (m_pStopAxes)
        {
                return m_pStopAxes (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::SetAbortFlag ()
{
        if (m_pSetAbortFlag)
        {
                return m_pSetAbortFlag (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::MoveRelSingleAxis (int lAxis, double dValue, BOOL bWait)
{
        if (m_pMoveRelSingleAxis)
        {
                return m_pMoveRelSingleAxis (m_LSID, lAxis, dValue, bWait);
        }
        else
                return -1;
}

int CLStep4X::MoveAbsSingleAxis (int lAxis, double dValue, BOOL bWait)
{
        if (m_pMoveAbsSingleAxis)
        {
                return m_pMoveAbsSingleAxis (m_LSID, lAxis, dValue, bWait);
        }
        else
                return -1;
}

int CLStep4X::SetControlPars ()
{
        if (m_pSetControlPars)
        {
                return m_pSetControlPars (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::SetMotorCurrent (double dX, double dY, double dZ, double dA)
{
        if (m_pSetMotorCurrent)
        {
                return m_pSetMotorCurrent (m_LSID, dX, dY, dZ, dA);
        }
        else
                return -1;
}

int CLStep4X::SetVelSingleAxis (int lAxis, double dVel)
{
        if (m_pSetVelSingleAxis)
        {
                return m_pSetVelSingleAxis (m_LSID, lAxis, dVel);
        }
        else
                return -1;
}

int CLStep4X::SetAccelSingleAxis (int lAxis, double dAccel)
{
        if (m_pSetAccelSingleAxis)
        {
                return m_pSetAccelSingleAxis (m_LSID, lAxis, dAccel);
        }
        else
                return -1;
}

int CLStep4X::CalibrateEx (int lFlags)
{
        if (m_pCalibrateEx)
        {
                return m_pCalibrateEx (m_LSID, lFlags);
        }
        else
                return -1;
}

int CLStep4X::RMeasureEx (int lFlags)
{
        if (m_pRMeasureEx)
        {
                return m_pRMeasureEx (m_LSID, lFlags);
        }
        else
                return -1;
}

int CLStep4X::SetShowProt (BOOL bShowProt)
{
        if (m_pSetShowProt)
        {
                return m_pSetShowProt (m_LSID, bShowProt);
        }
        else
                return -1;
}

int CLStep4X::GetAnalogInputs2 (int *plPT100, int *plMV, int *plV24)
{
        if (m_pGetAnalogInputs2)
        {
                return m_pGetAnalogInputs2 (m_LSID, plPT100, plMV, plV24);
        }
        else
                return -1;
}

int CLStep4X::SendString (char *pcStr, char *pcRet, int lMaxLen, BOOL bReadLine, int lTimeOut)
{
        if (m_pSendString)
        {
                return m_pSendString (m_LSID, pcStr, pcRet, lMaxLen, bReadLine, lTimeOut);
        }
        else
                return -1;
}

int CLStep4X::SetSpeedPoti (BOOL bSpeedPoti)
{
        if (m_pSetSpeedPoti)
        {
                return m_pSetSpeedPoti (m_LSID, bSpeedPoti);
        }
        else
                return -1;
}

int CLStep4X::SetTVRMode (int lXT, int lYT, int lZT, int lAT)
{
        if (m_pSetTVRMode)
        {
                return m_pSetTVRMode (m_LSID, lXT, lYT, lZT, lAT);
        }
        else
                return -1;
}

int CLStep4X::SetAutoStatus (int lValue)
{
        if (m_pSetAutoStatus)
        {
                return m_pSetAutoStatus (m_LSID, lValue);
        }
        else
                return -1;
}

int CLStep4X::GetStatusAxis (char *pcStatusAxisStr, int lMaxLen)
{
        if (m_pGetStatusAxis)
        {
                return m_pGetStatusAxis (m_LSID, pcStatusAxisStr, lMaxLen);
        }
        else
                return -1;
}

int CLStep4X::SetDigIO_Off (int lIndex)
{
        if (m_pSetDigIO_Off)
        {
                return m_pSetDigIO_Off (m_LSID, lIndex);
        }
        else
                return -1;
}

int CLStep4X::SetDigIO_Polarity (int lIndex, BOOL bHigh)
{
        if (m_pSetDigIO_Polarity)
        {
                return m_pSetDigIO_Polarity (m_LSID, lIndex, bHigh);
        }
        else
                return -1;
}

int CLStep4X::SetDigIO_EmergencyStop (int lIndex)
{
        if (m_pSetDigIO_EmergencyStop)
        {
                return m_pSetDigIO_EmergencyStop (m_LSID, lIndex);
        }
        else
                return -1;
}

int CLStep4X::SetDigIO_Distance (int lIndex, BOOL bFkt, double dDist, int lAxis)
{
        if (m_pSetDigIO_Distance)
        {
                return m_pSetDigIO_Distance (m_LSID, lIndex, bFkt, dDist, lAxis);
        }
        else
                return -1;
}

int CLStep4X::SetDimensions (int lXD, int lYD, int lZD, int lAD)
{
        if (m_pSetDimensions)
        {
                return m_pSetDimensions (m_LSID, lXD, lYD, lZD, lAD);
        }
        else
                return -1;
}

int CLStep4X::MoveRelShort ()
{
        if (m_pMoveRelShort)
        {
                return m_pMoveRelShort (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::SetEncoderPeriod (double dX, double dY, double dZ, double dA)
{
        if (m_pSetEncoderPeriod)
        {
                return m_pSetEncoderPeriod (m_LSID, dX, dY, dZ, dA);
        }
        else
                return -1;
}

int CLStep4X::SetJoystickDir (int lXD, int lYD, int lZD, int lAD)
{
        if (m_pSetJoystickDir)
        {
                return m_pSetJoystickDir (m_LSID, lXD, lYD, lZD, lAD);
        }
        else
                return -1;
}

int CLStep4X::SetEncoderMask (int lValue)
{
        if (m_pSetEncoderMask)
        {
                return m_pSetEncoderMask (m_LSID, lValue);
        }
        else
                return -1;
}

int CLStep4X::SetGear (double dX, double dY, double dZ, double dA)
{
        if (m_pSetGear)
        {
                return m_pSetGear (m_LSID, dX, dY, dZ, dA);
        }
        else
                return -1;
}

int CLStep4X::SetHandWheelOn (BOOL bPositionCount, BOOL bEncoder)
{
        if (m_pSetHandWheelOn)
        {
                return m_pSetHandWheelOn (m_LSID, bPositionCount, bEncoder);
        }
        else
                return -1;
}

int CLStep4X::SetHandWheelOff ()
{
        if (m_pSetHandWheelOff)
        {
                return m_pSetHandWheelOff (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::SetFactorTVR (double dX, double dY, double dZ, double dA)
{
        if (m_pSetFactorTVR)
        {
                return m_pSetFactorTVR (m_LSID, dX, dY, dZ, dA);
        }
        else
                return -1;
}

int CLStep4X::SetTargetWindow (double dX, double dY, double dZ, double dA)
{
        if (m_pSetTargetWindow)
        {
                return m_pSetTargetWindow (m_LSID, dX, dY, dZ, dA);
        }
        else
                return -1;
}

int CLStep4X::SetController (int lXC, int lYC, int lZC, int lAC)
{
        if (m_pSetController)
        {
                return m_pSetController (m_LSID, lXC, lYC, lZC, lAC);
        }
        else
                return -1;
}

int CLStep4X::SetControllerCall (int lCtrCall)
{
        if (m_pSetControllerCall)
        {
                return m_pSetControllerCall (m_LSID, lCtrCall);
        }
        else
                return -1;
}

int CLStep4X::SetControllerSteps (double dX, double dY, double dZ, double dA)
{
        if (m_pSetControllerSteps)
        {
                return m_pSetControllerSteps (m_LSID, dX, dY, dZ, dA);
        }
        else
                return -1;
}

int CLStep4X::SetControllerFactor (double dX, double dY, double dZ, double dA)
{
        if (m_pSetControllerFactor)
        {
                return m_pSetControllerFactor (m_LSID, dX, dY, dZ, dA);
        }
        else
                return -1;
}

int CLStep4X::SetControllerTWDelay (int lCtrTWDelay)
{
        if (m_pSetControllerTWDelay)
        {
                return m_pSetControllerTWDelay (m_LSID, lCtrTWDelay);
        }
        else
                return -1;
}

int CLStep4X::SetEncoderRefSignal (int lXR, int lYR, int lZR, int lAR)
{
        if (m_pSetEncoderRefSignal)
        {
                return m_pSetEncoderRefSignal (m_LSID, lXR, lYR, lZR, lAR);
        }
        else
                return -1;
}

int CLStep4X::SetEncoderPosition (BOOL bValue)
{
        if (m_pSetEncoderPosition)
        {
                return m_pSetEncoderPosition (m_LSID, bValue);
        }
        else
                return -1;
}

int CLStep4X::GetVersionStr (char *pcVers, int lMaxLen)
{
        if (m_pGetVersionStr)
        {
                return m_pGetVersionStr (m_LSID, pcVers, lMaxLen);
        }
        else
                return -1;
}

int CLStep4X::GetError (int *plErrorCode)
{
        if (m_pGetError)
        {
                return m_pGetError (m_LSID, plErrorCode);
        }
        else
                return -1;
}

int CLStep4X::GetPosSingleAxis (int lAxis, double *pdPos)
{
        if (m_pGetPosSingleAxis)
        {
                return m_pGetPosSingleAxis (m_LSID, lAxis, pdPos);
        }
        else
                return -1;
}

int CLStep4X::SetDistance (double dX, double dY, double dZ, double dA)
{
        if (m_pSetDistance)
        {
                return m_pSetDistance (m_LSID, dX, dY, dZ, dA);
        }
        else
                return -1;
}

int CLStep4X::GetPosEx (double *pdX, double *pdY, double *pdZ, double *pdR, BOOL bEncoder)
{
        if (m_pGetPosEx)
        {
                return m_pGetPosEx (m_LSID, pdX, pdY, pdZ, pdR, bEncoder);
        }
        else
                return -1;
}

int CLStep4X::SetShowCmdList (BOOL bShowCmdList)
{
        if (m_pSetShowCmdList)
        {
                return m_pSetShowCmdList (m_LSID, bShowCmdList);
        }
        else
                return -1;
}

int CLStep4X::SetWriteLogText (BOOL bAWriteLogText)
{
        if (m_pSetWriteLogText)
        {
                return m_pSetWriteLogText (m_LSID, bAWriteLogText);
        }
        else
                return -1;
}

int CLStep4X::SetControllerTimeout (int lACtrTimeout)
{
        if (m_pSetControllerTimeout)
        {
                return m_pSetControllerTimeout (m_LSID, lACtrTimeout);
        }
        else
                return -1;
}

int CLStep4X::SetEncoderActive (int lFlags)
{
        if (m_pSetEncoderActive)
        {
                return m_pSetEncoderActive (m_LSID, lFlags);
        }
        else
                return -1;
}

int CLStep4X::GetSnapshotCount (int *plSnsCount)
{
        if (m_pGetSnapshotCount)
        {
                return m_pGetSnapshotCount (m_LSID, plSnsCount);
        }
        else
                return -1;
}

int CLStep4X::GetSnapshotPos (double *pdX, double *pdY, double *pdZ, double *pdA)
{
        if (m_pGetSnapshotPos)
        {
                return m_pGetSnapshotPos (m_LSID, pdX, pdY, pdZ, pdA);
        }
        else
                return -1;
}

int CLStep4X::SetCorrTblOff ()
{
        if (m_pSetCorrTblOff)
        {
                return m_pSetCorrTblOff (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::SetCorrTblOn (char *pcAFileName)
{
        if (m_pSetCorrTblOn)
        {
                return m_pSetCorrTblOn (m_LSID, pcAFileName);
        }
        else
                return -1;
}

int CLStep4X::SetFactorMode (BOOL bAFactorMode, double dX, double dY, double dZ, double dA)
{
        if (m_pSetFactorMode)
        {
                return m_pSetFactorMode (m_LSID, bAFactorMode, dX, dY, dZ, dA);
        }
        else
                return -1;
}

int CLStep4X::SetSnapshot (BOOL bASnapshot)
{
        if (m_pSetSnapshot)
        {
                return m_pSetSnapshot (m_LSID, bASnapshot);
        }
        else
                return -1;
}

int CLStep4X::SetSnapshotPar (BOOL bHigh, BOOL bAutoMode)
{
        if (m_pSetSnapshotPar)
        {
                return m_pSetSnapshotPar (m_LSID, bHigh, bAutoMode);
        }
        else
                return -1;
}

int CLStep4X::SetTrigger (BOOL bATrigger)
{
        if (m_pSetTrigger)
        {
                return m_pSetTrigger (m_LSID, bATrigger);
        }
        else
                return -1;
}

int CLStep4X::SetTriggerPar (int lAxis, int lMode, int lSignal, double dDistance)
{
        if (m_pSetTriggerPar)
        {
                return m_pSetTriggerPar (m_LSID, lAxis, lMode, lSignal, dDistance);
        }
        else
                return -1;
}

int CLStep4X::SetLanguage (char *pcPLN)
{
        if (m_pSetLanguage)
        {
                return m_pSetLanguage (m_LSID, pcPLN);
        }
        else
                return -1;
}

int CLStep4X::GetSwitches (int *plFlags)
{
        if (m_pGetSwitches)
        {
                return m_pGetSwitches (m_LSID, plFlags);
        }
        else
                return -1;
}

int CLStep4X::GetSerialNr (char *pcSerialNr, int lMaxLen)
{
        if (m_pGetSerialNr)
        {
                return m_pGetSerialNr (m_LSID, pcSerialNr, lMaxLen);
        }
        else
                return -1;
}

int CLStep4X::SetCalibOffset (double dX, double dY, double dZ, double dR)
{
        if (m_pSetCalibOffset)
        {
                return m_pSetCalibOffset (m_LSID, dX, dY, dZ, dR);
        }
        else
                return -1;
}

int CLStep4X::SetRMOffset (double dX, double dY, double dZ, double dR)
{
        if (m_pSetRMOffset)
        {
                return m_pSetRMOffset (m_LSID, dX, dY, dZ, dR);
        }
        else
                return -1;
}

int CLStep4X::GetSnapshotPosArray (int lIndex, double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetSnapshotPosArray)
        {
                return m_pGetSnapshotPosArray (m_LSID, lIndex, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::SetAxisDirection (int lXD, int lYD, int lZD, int lAD)
{
        if (m_pSetAxisDirection)
        {
                return m_pSetAxisDirection (m_LSID, lXD, lYD, lZD, lAD);
        }
        else
                return -1;
}

int CLStep4X::SetCalibrateDir (int lXD, int lYD, int lZD, int lAD)
{
        if (m_pSetCalibrateDir)
        {
                return m_pSetCalibrateDir (m_LSID, lXD, lYD, lZD, lAD);
        }
        else
                return -1;
}

int CLStep4X::LStepSave ()
{
        if (m_pLStepSave)
        {
                return m_pLStepSave (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::SetBPZ (int lAValue)
{
        if (m_pSetBPZ)
        {
                return m_pSetBPZ (m_LSID, lAValue);
        }
        else
                return -1;
}

int CLStep4X::SetBPZTrackballFactor (double dAValue)
{
        if (m_pSetBPZTrackballFactor)
        {
                return m_pSetBPZTrackballFactor (m_LSID, dAValue);
        }
        else
                return -1;
}

int CLStep4X::SetBPZTrackballBackLash (double dX, double dY, double dZ, double dR)
{
        if (m_pSetBPZTrackballBackLash)
        {
                return m_pSetBPZTrackballBackLash (m_LSID, dX, dY, dZ, dR);
        }
        else
                return -1;
}

int CLStep4X::SetBPZJoyspeed (int lAPar, double dAValue)
{
        if (m_pSetBPZJoyspeed)
        {
                return m_pSetBPZJoyspeed (m_LSID, lAPar, dAValue);
        }
        else
                return -1;
}

int CLStep4X::SetJoystickWindow (int lAValue)
{
        if (m_pSetJoystickWindow)
        {
                return m_pSetJoystickWindow (m_LSID, lAValue);
        }
        else
                return -1;
}

int CLStep4X::SetCurrentDelay (int lX, int lY, int lZ, int lR)
{
        if (m_pSetCurrentDelay)
        {
                return m_pSetCurrentDelay (m_LSID, lX, lY, lZ, lR);
        }
        else
                return -1;
}

int CLStep4X::MoveEx (double dX, double dY, double dZ, double dR, BOOL bRelative, BOOL bWait, int lAxisCount)
{
        if (m_pMoveEx)
        {
                return m_pMoveEx (m_LSID, dX, dY, dZ, dR, bRelative, bWait, lAxisCount);
        }
        else
                return -1;
}

int CLStep4X::WaitForAxisStop (int lAFlags, int lATimeoutValue, BOOL *pbATimeout)
{
        if (m_pWaitForAxisStop)
        {
                return m_pWaitForAxisStop (m_LSID, lAFlags, lATimeoutValue, pbATimeout);
        }
        else
                return -1;
}

int CLStep4X::SetProcessMessagesProc (void* pProc)
{
        if (m_pSetProcessMessagesProc)
        {
                return m_pSetProcessMessagesProc (m_LSID, pProc);
        }
        else
                return -1;
}

int CLStep4X::SendStringPosCmd (char *pcStr, char *pcRet, int lMaxLen, BOOL bReadLine, int lTimeOut)
{
        if (m_pSendStringPosCmd)
        {
                return m_pSendStringPosCmd (m_LSID, pcStr, pcRet, lMaxLen, bReadLine, lTimeOut);
        }
        else
                return -1;
}

int CLStep4X::SetDigitalOutputs (int lValue)
{
        if (m_pSetDigitalOutputs)
        {
                return m_pSetDigitalOutputs (m_LSID, lValue);
        }
        else
                return -1;
}

int CLStep4X::SetWriteLogTextFN (BOOL bAWriteLogText, char *pcALogFN)
{
        if (m_pSetWriteLogTextFN)
        {
                return m_pSetWriteLogTextFN (m_LSID, bAWriteLogText, pcALogFN);
        }
        else
                return -1;
}

int CLStep4X::SetDigitalOutputsE (int lValue)
{
        if (m_pSetDigitalOutputsE)
        {
                return m_pSetDigitalOutputsE (m_LSID, lValue);
        }
        else
                return -1;
}

int CLStep4X::GetDigitalInputsE (int *plValue)
{
        if (m_pGetDigitalInputsE)
        {
                return m_pGetDigitalInputsE (m_LSID, plValue);
        }
        else
                return -1;
}

int CLStep4X::EnableCommandRetry (BOOL bAValue)
{
        if (m_pEnableCommandRetry)
        {
                return m_pEnableCommandRetry (m_LSID, bAValue);
        }
        else
                return -1;
}

int CLStep4X::SetXYAxisComp (int lValue)
{
        if (m_pSetXYAxisComp)
        {
                return m_pSetXYAxisComp (m_LSID, lValue);
        }
        else
                return -1;
}

int CLStep4X::GetVersionStrDet (char *pcVersDet, int lMaxLen)
{
        if (m_pGetVersionStrDet)
        {
                return m_pGetVersionStrDet (m_LSID, pcVersDet, lMaxLen);
        }
        else
                return -1;
}

int CLStep4X::SetCommandTimeout (int lAtoRead, int lAtoMove, int lAtoCalibrate)
{
        if (m_pSetCommandTimeout)
        {
                return m_pSetCommandTimeout (m_LSID, lAtoRead, lAtoMove, lAtoCalibrate);
        }
        else
                return -1;
}

int CLStep4X::SetExtValue (int lAName, int lAValue)
{
        if (m_pSetExtValue)
        {
                return m_pSetExtValue (m_LSID, lAName, lAValue);
        }
        else
                return -1;
}

int CLStep4X::FlushBuffer (int lAValue)
{
        if (m_pFlushBuffer)
        {
                return m_pFlushBuffer (m_LSID, lAValue);
        }
        else
                return -1;
}

int CLStep4X::GetEEPRomValue (byte Offset, byte *pValue)
{
        if (m_pGetEEPRomValue)
        {
                return m_pGetEEPRomValue (m_LSID, Offset, pValue);
        }
        else
                return -1;
}

int CLStep4X::SetEEPRomValue (byte Offset, byte Value)
{
        if (m_pSetEEPRomValue)
        {
                return m_pSetEEPRomValue (m_LSID, Offset, Value);
        }
        else
                return -1;
}

int CLStep4X::GetXYAxisComp (int *plValue)
{
        if (m_pGetXYAxisComp)
        {
                return m_pGetXYAxisComp (m_LSID, plValue);
        }
        else
                return -1;
}

int CLStep4X::GetDimensions (int *plXD, int *plYD, int *plZD, int *plAD)
{
        if (m_pGetDimensions)
        {
                return m_pGetDimensions (m_LSID, plXD, plYD, plZD, plAD);
        }
        else
                return -1;
}

int CLStep4X::GetPitch (double *pdX, double *pdY, double *pdZ, double *pdA)
{
        if (m_pGetPitch)
        {
                return m_pGetPitch (m_LSID, pdX, pdY, pdZ, pdA);
        }
        else
                return -1;
}

int CLStep4X::GetGear (double *pdX, double *pdY, double *pdZ, double *pdA)
{
        if (m_pGetGear)
        {
                return m_pGetGear (m_LSID, pdX, pdY, pdZ, pdA);
        }
        else
                return -1;
}

int CLStep4X::GetVel (double *pdX, double *pdY, double *pdZ, double *pdA)
{
        if (m_pGetVel)
        {
                return m_pGetVel (m_LSID, pdX, pdY, pdZ, pdA);
        }
        else
                return -1;
}

int CLStep4X::GetAccel (double *pdX, double *pdY, double *pdZ, double *pdA)
{
        if (m_pGetAccel)
        {
                return m_pGetAccel (m_LSID, pdX, pdY, pdZ, pdA);
        }
        else
                return -1;
}

int CLStep4X::SetVelFac (double dX, double dY, double dZ, double dR)
{
        if (m_pSetVelFac)
        {
                return m_pSetVelFac (m_LSID, dX, dY, dZ, dR);
        }
        else
                return -1;
}

int CLStep4X::GetVelFac (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetVelFac)
        {
                return m_pGetVelFac (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::GetSpeedPoti (BOOL *pbSpePoti)
{
        if (m_pGetSpeedPoti)
        {
                return m_pGetSpeedPoti (m_LSID, pbSpePoti);
        }
        else
                return -1;
}

int CLStep4X::GetMotorCurrent (double *pdX, double *pdY, double *pdZ, double *pdA)
{
        if (m_pGetMotorCurrent)
        {
                return m_pGetMotorCurrent (m_LSID, pdX, pdY, pdZ, pdA);
        }
        else
                return -1;
}

int CLStep4X::GetReduction (double *pdX, double *pdY, double *pdZ, double *pdA)
{
        if (m_pGetReduction)
        {
                return m_pGetReduction (m_LSID, pdX, pdY, pdZ, pdA);
        }
        else
                return -1;
}

int CLStep4X::GetCurrentDelay (int *plX, int *plY, int *plZ, int *plR)
{
        if (m_pGetCurrentDelay)
        {
                return m_pGetCurrentDelay (m_LSID, plX, plY, plZ, plR);
        }
        else
                return -1;
}

int CLStep4X::SetOutFuncLev (double dX, double dY, double dZ, double dR)
{
        if (m_pSetOutFuncLev)
        {
                return m_pSetOutFuncLev (m_LSID, dX, dY, dZ, dR);
        }
        else
                return -1;
}

int CLStep4X::GetOutFuncLev (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetOutFuncLev)
        {
                return m_pGetOutFuncLev (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::GetActiveAxes (int *plFlags)
{
        if (m_pGetActiveAxes)
        {
                return m_pGetActiveAxes (m_LSID, plFlags);
        }
        else
                return -1;
}

int CLStep4X::GetAxisDirection (int *plXD, int *plYD, int *plZD, int *plAD)
{
        if (m_pGetAxisDirection)
        {
                return m_pGetAxisDirection (m_LSID, plXD, plYD, plZD, plAD);
        }
        else
                return -1;
}

int CLStep4X::GetCalibOffset (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetCalibOffset)
        {
                return m_pGetCalibOffset (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::GetRMOffset (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetRMOffset)
        {
                return m_pGetRMOffset (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::GetCalibrateDir (int *plXD, int *plYD, int *plZD, int *plAD)
{
        if (m_pGetCalibrateDir)
        {
                return m_pGetCalibrateDir (m_LSID, plXD, plYD, plZD, plAD);
        }
        else
                return -1;
}

int CLStep4X::SetCalibBackSpeed (int lSpeed)
{
        if (m_pSetCalibBackSpeed)
        {
                return m_pSetCalibBackSpeed (m_LSID, lSpeed);
        }
        else
                return -1;
}

int CLStep4X::GetCalibBackSpeed (int *plSpeed)
{
        if (m_pGetCalibBackSpeed)
        {
                return m_pGetCalibBackSpeed (m_LSID, plSpeed);
        }
        else
                return -1;
}

int CLStep4X::SetRefSpeed (int lSpeed)
{
        if (m_pSetRefSpeed)
        {
                return m_pSetRefSpeed (m_LSID, lSpeed);
        }
        else
                return -1;
}

int CLStep4X::GetRefSpeed (int *plSpeed)
{
        if (m_pGetRefSpeed)
        {
                return m_pGetRefSpeed (m_LSID, plSpeed);
        }
        else
                return -1;
}

int CLStep4X::SetPowerAmplifier (BOOL bAmplifier)
{
        if (m_pSetPowerAmplifier)
        {
                return m_pSetPowerAmplifier (m_LSID, bAmplifier);
        }
        else
                return -1;
}

int CLStep4X::GetPowerAmplifier (BOOL *pbAmplifier)
{
        if (m_pGetPowerAmplifier)
        {
                return m_pGetPowerAmplifier (m_LSID, pbAmplifier);
        }
        else
                return -1;
}

int CLStep4X::SetMotorTablePatch (BOOL bActive)
{
        if (m_pSetMotorTablePatch)
        {
                return m_pSetMotorTablePatch (m_LSID, bActive);
        }
        else
                return -1;
}

int CLStep4X::GetMotorTablePatch (BOOL *pbActive)
{
        if (m_pGetMotorTablePatch)
        {
                return m_pGetMotorTablePatch (m_LSID, pbActive);
        }
        else
                return -1;
}

int CLStep4X::SetJoystickFilter (BOOL bActive)
{
        if (m_pSetJoystickFilter)
        {
                return m_pSetJoystickFilter (m_LSID, bActive);
        }
        else
                return -1;
}

int CLStep4X::GetJoystickFilter (BOOL *pbActive)
{
        if (m_pGetJoystickFilter)
        {
                return m_pGetJoystickFilter (m_LSID, pbActive);
        }
        else
                return -1;
}

int CLStep4X::SetStopPolarity (BOOL bHighActiv)
{
        if (m_pSetStopPolarity)
        {
                return m_pSetStopPolarity (m_LSID, bHighActiv);
        }
        else
                return -1;
}

int CLStep4X::GetStopPolarity (BOOL *pbHighActiv)
{
        if (m_pGetStopPolarity)
        {
                return m_pGetStopPolarity (m_LSID, pbHighActiv);
        }
        else
                return -1;
}

int CLStep4X::SetVLevel (int lVRegion, double dDownLevel, double dUppLevel)
{
        if (m_pSetVLevel)
        {
                return m_pSetVLevel (m_LSID, lVRegion, dDownLevel, dUppLevel);
        }
        else
                return -1;
}

int CLStep4X::GetVLevel (int lVRegion, double *pdDownLevel, double *pdUppLevel)
{
        if (m_pGetVLevel)
        {
                return m_pGetVLevel (m_LSID, lVRegion, pdDownLevel, pdUppLevel);
        }
        else
                return -1;
}

int CLStep4X::SetStopAccel (double dXD, double dYD, double dZD, double dAD)
{
        if (m_pSetStopAccel)
        {
                return m_pSetStopAccel (m_LSID, dXD, dYD, dZD, dAD);
        }
        else
                return -1;
}

int CLStep4X::GetStopAccel (double *pdXD, double *pdYD, double *pdZD, double *pdAD)
{
        if (m_pGetStopAccel)
        {
                return m_pGetStopAccel (m_LSID, pdXD, pdYD, pdZD, pdAD);
        }
        else
                return -1;
}

int CLStep4X::GetVersionStrInfo (char *pcVersInfo, int lMaxLen)
{
        if (m_pGetVersionStrInfo)
        {
                return m_pGetVersionStrInfo (m_LSID, pcVersInfo, lMaxLen);
        }
        else
                return -1;
}

int CLStep4X::GetStatusLimit (char *pcLimit, int lMaxLen)
{
        if (m_pGetStatusLimit)
        {
                return m_pGetStatusLimit (m_LSID, pcLimit, lMaxLen);
        }
        else
                return -1;
}

int CLStep4X::GetSecurityErr (DWORD *pdwValue)
{
        if (m_pGetSecurityErr)
        {
                return m_pGetSecurityErr (m_LSID, pdwValue);
        }
        else
                return -1;
}

int CLStep4X::GetSecurityStatus (DWORD *pdwValue)
{
        if (m_pGetSecurityStatus)
        {
                return m_pGetSecurityStatus (m_LSID, pdwValue);
        }
        else
                return -1;
}

int CLStep4X::GetDelay (int *plDelay)
{
        if (m_pGetDelay)
        {
                return m_pGetDelay (m_LSID, plDelay);
        }
        else
                return -1;
}

int CLStep4X::GetDistance (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetDistance)
        {
                return m_pGetDistance (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::ClearPos (int lFlags)
{
        if (m_pClearPos)
        {
                return m_pClearPos (m_LSID, lFlags);
        }
        else
                return -1;
}

int CLStep4X::SetDigJoySpeed (double dX, double dY, double dZ, double dR)
{
        if (m_pSetDigJoySpeed)
        {
                return m_pSetDigJoySpeed (m_LSID, dX, dY, dZ, dR);
        }
        else
                return -1;
}

int CLStep4X::GetDigJoySpeed (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetDigJoySpeed)
        {
                return m_pGetDigJoySpeed (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::GetJoystickDir (int *plXD, int *plYD, int *plZD, int *plRD)
{
        if (m_pGetJoystickDir)
        {
                return m_pGetJoystickDir (m_LSID, plXD, plYD, plZD, plRD);
        }
        else
                return -1;
}

int CLStep4X::GetJoystick (BOOL *pbJoystickOn, BOOL *pbManual, BOOL *pbPositionCount, BOOL *pbEncoder)
{
        if (m_pGetJoystick)
        {
                return m_pGetJoystick (m_LSID, pbJoystickOn, pbManual, pbPositionCount, pbEncoder);
        }
        else
                return -1;
}

int CLStep4X::GetJoystickWindow (int *plAValue)
{
        if (m_pGetJoystickWindow)
        {
                return m_pGetJoystickWindow (m_LSID, plAValue);
        }
        else
                return -1;
}

int CLStep4X::GetHandWheel (BOOL *pbHandWheelOn, BOOL *pbPositionCount, BOOL *pbEncoder)
{
        if (m_pGetHandWheel)
        {
                return m_pGetHandWheel (m_LSID, pbHandWheelOn, pbPositionCount, pbEncoder);
        }
        else
                return -1;
}

int CLStep4X::GetBPZ (int *plAValue)
{
        if (m_pGetBPZ)
        {
                return m_pGetBPZ (m_LSID, plAValue);
        }
        else
                return -1;
}

int CLStep4X::GetBPZTrackballFactor (double *pdAValue)
{
        if (m_pGetBPZTrackballFactor)
        {
                return m_pGetBPZTrackballFactor (m_LSID, pdAValue);
        }
        else
                return -1;
}

int CLStep4X::GetBPZTrackballBackLash (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetBPZTrackballBackLash)
        {
                return m_pGetBPZTrackballBackLash (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::GetBPZJoyspeed (int lAPar, double *pdAValue)
{
        if (m_pGetBPZJoyspeed)
        {
                return m_pGetBPZJoyspeed (m_LSID, lAPar, pdAValue);
        }
        else
                return -1;
}

int CLStep4X::GetLimitControl (int lAxis, BOOL *pbActive)
{
        if (m_pGetLimitControl)
        {
                return m_pGetLimitControl (m_LSID, lAxis, pbActive);
        }
        else
                return -1;
}

int CLStep4X::SetAutoLimitAfterCalibRM (int lFlags)
{
        if (m_pSetAutoLimitAfterCalibRM)
        {
                return m_pSetAutoLimitAfterCalibRM (m_LSID, lFlags);
        }
        else
                return -1;
}

int CLStep4X::GetAutoLimitAfterCalibRM (int *plFlags)
{
        if (m_pGetAutoLimitAfterCalibRM)
        {
                return m_pGetAutoLimitAfterCalibRM (m_LSID, plFlags);
        }
        else
                return -1;
}

int CLStep4X::GetSwitchPolarity (int *plXP, int *plYP, int *plZP, int *plRP)
{
        if (m_pGetSwitchPolarity)
        {
                return m_pGetSwitchPolarity (m_LSID, plXP, plYP, plZP, plRP);
        }
        else
                return -1;
}

int CLStep4X::GetSwitchActive (int *plXA, int *plYA, int *plZA, int *plRA)
{
        if (m_pGetSwitchActive)
        {
                return m_pGetSwitchActive (m_LSID, plXA, plYA, plZA, plRA);
        }
        else
                return -1;
}

int CLStep4X::GetTVRMode (int *plXT, int *plYT, int *plZT, int *plRT)
{
        if (m_pGetTVRMode)
        {
                return m_pGetTVRMode (m_LSID, plXT, plYT, plZT, plRT);
        }
        else
                return -1;
}

int CLStep4X::GetFactorTVR (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetFactorTVR)
        {
                return m_pGetFactorTVR (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::SetTVROutMode (int lXT, int lYT, int lZT, int lRT)
{
        if (m_pSetTVROutMode)
        {
                return m_pSetTVROutMode (m_LSID, lXT, lYT, lZT, lRT);
        }
        else
                return -1;
}

int CLStep4X::GetTVROutMode (int *plXT, int *plYT, int *plZT, int *plRT)
{
        if (m_pGetTVROutMode)
        {
                return m_pGetTVROutMode (m_LSID, plXT, plYT, plZT, plRT);
        }
        else
                return -1;
}

int CLStep4X::SetTVROutResolution (int lX, int lY, int lZ, int lR)
{
        if (m_pSetTVROutResolution)
        {
                return m_pSetTVROutResolution (m_LSID, lX, lY, lZ, lR);
        }
        else
                return -1;
}

int CLStep4X::GetTVROutResolution (int *plX, int *plY, int *plZ, int *plR)
{
        if (m_pGetTVROutResolution)
        {
                return m_pGetTVROutResolution (m_LSID, plX, plY, plZ, plR);
        }
        else
                return -1;
}

int CLStep4X::SetTVROutPitch (double dX, double dY, double dZ, double dR)
{
        if (m_pSetTVROutPitch)
        {
                return m_pSetTVROutPitch (m_LSID, dX, dY, dZ, dR);
        }
        else
                return -1;
}

int CLStep4X::GetTVROutPitch (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetTVROutPitch)
        {
                return m_pGetTVROutPitch (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::SetVelTVRO (double dX, double dY, double dZ, double dR)
{
        if (m_pSetVelTVRO)
        {
                return m_pSetVelTVRO (m_LSID, dX, dY, dZ, dR);
        }
        else
                return -1;
}

int CLStep4X::GetVelTVRO (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetVelTVRO)
        {
                return m_pGetVelTVRO (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::SetAccelTVRO (double dX, double dY, double dZ, double dR)
{
        if (m_pSetAccelTVRO)
        {
                return m_pSetAccelTVRO (m_LSID, dX, dY, dZ, dR);
        }
        else
                return -1;
}

int CLStep4X::GetAccelTVRO (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetAccelTVRO)
        {
                return m_pGetAccelTVRO (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::SetVelSingleAxisTVRO (int lAxis, double dVel)
{
        if (m_pSetVelSingleAxisTVRO)
        {
                return m_pSetVelSingleAxisTVRO (m_LSID, lAxis, dVel);
        }
        else
                return -1;
}

int CLStep4X::SetAccelSingleAxisTVRO (int lAxis, double dAccel)
{
        if (m_pSetAccelSingleAxisTVRO)
        {
                return m_pSetAccelSingleAxisTVRO (m_LSID, lAxis, dAccel);
        }
        else
                return -1;
}

int CLStep4X::SetPosTVRO (double dX, double dY, double dZ, double dR)
{
        if (m_pSetPosTVRO)
        {
                return m_pSetPosTVRO (m_LSID, dX, dY, dZ, dR);
        }
        else
                return -1;
}

int CLStep4X::GetPosTVRO (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetPosTVRO)
        {
                return m_pGetPosTVRO (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::MoveAbsTVRO (double dX, double dY, double dZ, double dR, BOOL bWait)
{
        if (m_pMoveAbsTVRO)
        {
                return m_pMoveAbsTVRO (m_LSID, dX, dY, dZ, dR, bWait);
        }
        else
                return -1;
}

int CLStep4X::MoveRelTVRO (double dX, double dY, double dZ, double dR, BOOL bWait)
{
        if (m_pMoveRelTVRO)
        {
                return m_pMoveRelTVRO (m_LSID, dX, dY, dZ, dR, bWait);
        }
        else
                return -1;
}

int CLStep4X::MoveRelTVROSingleAxis (int lAxis, double dValue, BOOL bWait)
{
        if (m_pMoveRelTVROSingleAxis)
        {
                return m_pMoveRelTVROSingleAxis (m_LSID, lAxis, dValue, bWait);
        }
        else
                return -1;
}

int CLStep4X::MoveAbsTVROSingleAxis (int lAxis, double dValue, BOOL bWait)
{
        if (m_pMoveAbsTVROSingleAxis)
        {
                return m_pMoveAbsTVROSingleAxis (m_LSID, lAxis, dValue, bWait);
        }
        else
                return -1;
}

int CLStep4X::GetStatusTVRO (char *pcStat, int lMaxLen)
{
        if (m_pGetStatusTVRO)
        {
                return m_pGetStatusTVRO (m_LSID, pcStat, lMaxLen);
        }
        else
                return -1;
}

int CLStep4X::GetTargetWindow (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetTargetWindow)
        {
                return m_pGetTargetWindow (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::GetEncoderPeriod (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetEncoderPeriod)
        {
                return m_pGetEncoderPeriod (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::GetEncoderRefSignal (int *plXR, int *plYR, int *plZR, int *plRR)
{
        if (m_pGetEncoderRefSignal)
        {
                return m_pGetEncoderRefSignal (m_LSID, plXR, plYR, plZR, plRR);
        }
        else
                return -1;
}

int CLStep4X::GetEncoderPosition (BOOL *pbValue)
{
        if (m_pGetEncoderPosition)
        {
                return m_pGetEncoderPosition (m_LSID, pbValue);
        }
        else
                return -1;
}

int CLStep4X::GetEncoderActive (int *plFlags)
{
        if (m_pGetEncoderActive)
        {
                return m_pGetEncoderActive (m_LSID, plFlags);
        }
        else
                return -1;
}

int CLStep4X::GetController (int *plXC, int *plYC, int *plZC, int *plRC)
{
        if (m_pGetController)
        {
                return m_pGetController (m_LSID, plXC, plYC, plZC, plRC);
        }
        else
                return -1;
}

int CLStep4X::GetControllerCall (int *plCtrCall)
{
        if (m_pGetControllerCall)
        {
                return m_pGetControllerCall (m_LSID, plCtrCall);
        }
        else
                return -1;
}

int CLStep4X::GetControllerSteps (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetControllerSteps)
        {
                return m_pGetControllerSteps (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::GetControllerFactor (double *pdX, double *pdY, double *pdZ, double *pdR)
{
        if (m_pGetControllerFactor)
        {
                return m_pGetControllerFactor (m_LSID, pdX, pdY, pdZ, pdR);
        }
        else
                return -1;
}

int CLStep4X::GetControllerTWDelay (int *plCtrTWDelay)
{
        if (m_pGetControllerTWDelay)
        {
                return m_pGetControllerTWDelay (m_LSID, plCtrTWDelay);
        }
        else
                return -1;
}

int CLStep4X::GetControllerTimeout (int *plACtrTimeout)
{
        if (m_pGetControllerTimeout)
        {
                return m_pGetControllerTimeout (m_LSID, plACtrTimeout);
        }
        else
                return -1;
}

int CLStep4X::SetCtrFastMoveOn ()
{
        if (m_pSetCtrFastMoveOn)
        {
                return m_pSetCtrFastMoveOn (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::SetCtrFastMoveOff ()
{
        if (m_pSetCtrFastMoveOff)
        {
                return m_pSetCtrFastMoveOff (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::GetCtrFastMove (BOOL *pbActive)
{
        if (m_pGetCtrFastMove)
        {
                return m_pGetCtrFastMove (m_LSID, pbActive);
        }
        else
                return -1;
}

int CLStep4X::ClearCtrFastMoveCounter ()
{
        if (m_pClearCtrFastMoveCounter)
        {
                return m_pClearCtrFastMoveCounter (m_LSID);
        }
        else
                return -1;
}

int CLStep4X::GetCtrFastMoveCounter (int *plXC, int *plYC, int *plZC, int *plRC)
{
        if (m_pGetCtrFastMoveCounter)
        {
                return m_pGetCtrFastMoveCounter (m_LSID, plXC, plYC, plZC, plRC);
        }
        else
                return -1;
}

int CLStep4X::ClearEncoder (int lAxis)
{
        if (m_pClearEncoder)
        {
                return m_pClearEncoder (m_LSID, lAxis);
        }
        else
                return -1;
}

int CLStep4X::GetEncoder (double *pdXP, double *pdYP, double *pdZP, double *pdRP)
{
        if (m_pGetEncoder)
        {
                return m_pGetEncoder (m_LSID, pdXP, pdYP, pdZP, pdRP);
        }
        else
                return -1;
}

int CLStep4X::GetTrigger (BOOL *pbATrigger)
{
        if (m_pGetTrigger)
        {
                return m_pGetTrigger (m_LSID, pbATrigger);
        }
        else
                return -1;
}

int CLStep4X::GetTriggerPar (int *plAxis, int *plMode, int *plSignal, double *pdDistance)
{
        if (m_pGetTriggerPar)
        {
                return m_pGetTriggerPar (m_LSID, plAxis, plMode, plSignal, pdDistance);
        }
        else
                return -1;
}

int CLStep4X::SetTrigCount (int lValue)
{
        if (m_pSetTrigCount)
        {
                return m_pSetTrigCount (m_LSID, lValue);
        }
        else
                return -1;
}

int CLStep4X::GetTrigCount (int *plValue)
{
        if (m_pGetTrigCount)
        {
                return m_pGetTrigCount (m_LSID, plValue);
        }
        else
                return -1;
}

int CLStep4X::SetTVRInPulse (int lAxis, BOOL bDirection)
{
        if (m_pSetTVRInPulse)
        {
                return m_pSetTVRInPulse (m_LSID, lAxis, bDirection);
        }
        else
                return -1;
}

int CLStep4X::GetSnapshot (BOOL *pbASnapshot)
{
        if (m_pGetSnapshot)
        {
                return m_pGetSnapshot (m_LSID, pbASnapshot);
        }
        else
                return -1;
}

int CLStep4X::SetSnapshotFilter (int lTime)
{
        if (m_pSetSnapshotFilter)
        {
                return m_pSetSnapshotFilter (m_LSID, lTime);
        }
        else
                return -1;
}

int CLStep4X::GetSnapshotFilter (int *plTime)
{
        if (m_pGetSnapshotFilter)
        {
                return m_pGetSnapshotFilter (m_LSID, plTime);
        }
        else
                return -1;
}

int CLStep4X::GetSnapshotPar (BOOL *pbHigh, BOOL *pbAutoMode)
{
        if (m_pGetSnapshotPar)
        {
                return m_pGetSnapshotPar (m_LSID, pbHigh, pbAutoMode);
        }
        else
                return -1;
}

int CLStep4X::GetLimit (int lAxis, double *pdMinRange, double *pdMaxRange)
{
        if (m_pGetLimit)
        {
                return m_pGetLimit (m_LSID, lAxis, pdMinRange, pdMaxRange);
        }
        else
                return -1;
}

int CLStep4X::JoyChangeAxis (BOOL bValue)
{
        if (m_pJoyChangeAxis)
        {
                return m_pJoyChangeAxis (m_LSID, bValue);
        }
        else
                return -1;
}

int CLStep4X::GetJoyChangeAxis (BOOL *pbValue)
{
        if (m_pGetJoyChangeAxis)
        {
                return m_pGetJoyChangeAxis (m_LSID, pbValue);
        }
        else
                return -1;
}
