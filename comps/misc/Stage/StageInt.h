#pragma once
#define  com_call_char virtual char* _stdcall

interface IStage : public IUnknown
{
	com_call IsAvailable() = 0; // S_OK if available, S_FALSE if not
	com_call IsReady() = 0; // S_OK if available, S_FALSE if not
	com_call WaitForReady(DWORD nTimeoutMs) = 0;
	// nLimType: 0 - x, 1 - y, 2 - z
	com_call GetLimits(int nLimType, BOOL *pExists, int *pnLim1, int *pnLim2) = 0;
	com_call GetPosition(POINT *pptPos) = 0;
	com_call MoveTo(POINT ptPos) = 0;
	com_call Move(POINT ptMove) = 0;
	com_call GetPositionZ(int *pz) = 0;
	com_call MoveToZ(int z) = 0;
	//Sergey 10.11.05
	com_call MoveToZZ(int z) = 0;
	com_call PozZ()=0;
	com_call StopI()=0;
	//new 
	com_call MoveU(int z)=0;
	com_call MoveD(int z)=0;
   //com_call_char  Exec(char *pszControl)=0;//,char *pzsReply)=0;
	com_call_char  Exec(BSTR  bstrControl)=0;
	com_call WaitForReadyZ(DWORD nTimeoutMs) = 0;
	//END
	com_call MoveZ(int dz) = 0;
	com_call GetVelocity(int *v) = 0;
	com_call SetVelocity(int v) = 0;
	com_call SavePosition() = 0;
	com_call RestorePosition() = 0;
	com_call IsRestorePositionAvail() = 0;
	
};

declare_interface(IStage, "EA83E939-6EEC-43cc-8C5D-2DA9F492101D");
