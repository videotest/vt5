#include "stdafx.h"
#include "resource.h"
#include "StageActions.h"
#include "misc_utils.h"
#include "misc_calibr.h"
#include "input.h"
#include "nameconsts.h"
#include "debug.h"
#include "trace.h"

FILE *fp1;

static void CalibratePoint(POINT &pt, double dMeterPerPixel)
{
	pt.x = long(pt.x*dMeterPerPixel*1000000);// in microns
	pt.y = long(pt.y*dMeterPerPixel*1000000);// in microns
};

// {C0D5CB20-6ABF-4DCC-ADBA-9E9C998965A8}
static const IID IID_IIPreviewView =
{ 0xc0d5cb20, 0x6abf, 0x4dcc, { 0xad, 0xba, 0x9e, 0x9c, 0x99, 0x89, 0x65, 0xa8 } };

static bool IsPreviewView(IUnknown *punk)
{
	return CheckInterface(punk,IID_IIPreviewView);
}

IStagePtr CStageAction::m_Stage;

void CStageAction::Init()
{
	//serg
	VTMessageBox("InitAction", "VT5", MB_OK);
	//
	if (m_Stage == 0)
	{
		IUnknownPtr punk(_GetOtherComponent(::GetAppUnknown(), IID_IStage), false);
		m_Stage = punk;
	}
}


_ainfo_base::arg	CMoveStageInfo::s_pargs[] = 
{
	{0, 0, 0, false, false },
};

HICON CMoveStage::m_hIcon = NULL;
POINT CMoveStage::m_ptStageMove1s = {0,0};

CMoveStage::CMoveStage()
{
	m_bTrackOk = false;
	m_nZAxisAccel = GetValueInt(GetAppUnknown(), "Stage", "ZAxisSensitivity", 1);
}

HRESULT CMoveStage::DoInvoke()
{
	return S_OK;
}

void CMoveStage::LoadIcon()
{
	if (m_hIcon == NULL)
		m_hIcon = ::LoadIcon(app::instance()->handle(), MAKEINTRESOURCE(IDI_CENTER));
}

HRESULT CMoveStage::Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache )
{
	if (m_bTracking && !m_bLeftButton)
		DrawIcon(hDC, m_pointStartTracking.x-16, m_pointStartTracking.y-16, m_hIcon);
	return S_OK;
}

LRESULT	CMoveStage::DoMouseMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (nMsg == 0x020A) //0x020A==WM_MOUSEWHEEL, to avoid defining _WIN32_WINNT or _WIN32_WINDOWS
	{ // see WinUser.h
		Init();
		int zDelta = ((int)wParam)>>16;
		if (m_Stage!=0) m_Stage->MoveZ(zDelta*m_nZAxisAccel/120);
		return 0;
	}
	else
		return CInteractiveAction::DoMouseMessage(nMsg,wParam,lParam);
}

HRESULT CMoveStage::OnMessage(MSG *pmsg, LRESULT *plReturn)
{
	if (pmsg->message == 0x020A)
	{
		__super::OnMessage(pmsg, plReturn);
		return S_FALSE;
	}
	else
		return __super::OnMessage(pmsg, plReturn);
}

UINT_PTR g_nTimer = 0;
static DWORD s_dwBeg;
void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	CStageAction::m_Stage->Move(CMoveStage::m_ptStageMove1s);
}

bool CMoveStage::DoStartTracking(_point point)
{
	Init();
	if (m_Stage == 0) return false;
	if (FAILED(m_Stage->GetPosition(&m_ptPosStage0)))
		return false;
	GUID guid;
	GetDefaultCalibration(&m_dCalibr, &guid);
	m_bTrackOk = true;
	if (!m_bLeftButton)
	{
		LoadIcon();
		m_ptStageMove1s = _point(0,0);
		s_dwBeg = GetTickCount();
		g_nTimer = ::SetTimer(NULL, 1, 100, (TIMERPROC)TimerProc);
	}
	m_Stage->SavePosition();
	return true;
}

bool CMoveStage::DoTrack(_point point)
{
	if (m_bTrackOk)
	{
		_point ptDiff(point.x-m_pointStartTracking.x,point.y-m_pointStartTracking.y);
		CalibratePoint(ptDiff, m_dCalibr); // in microns
		if (m_bLeftButton)
		{
			_point ptSet(m_ptPosStage0.x+ptDiff.x,m_ptPosStage0.y+ptDiff.y);
			m_Stage->MoveTo(ptSet);
		}
		else
			m_ptStageMove1s = ptDiff;
	}
	return true;
}

bool CMoveStage::DoFinishTracking(_point point)
{
	if (g_nTimer)
	{
		KillTimer(NULL, g_nTimer);
		g_nTimer = 0;
	}
	return true;
}

HRESULT CMoveStage::GetActionState(DWORD *pdwState)
{
	Init();
	*pdwState = IsStageOk()&&IsPreviewView(m_ptrTarget)?afEnabled:0;
	return S_OK;
}

_ainfo_base::arg	CAutofocusInfo::s_pargs[] = 
{
	{"InitialStepMcs",szArgumentTypeInt, "0", true, false },
	{"StepsUp",szArgumentTypeInt, "0", true, false },
	{"StepsDown",szArgumentTypeInt, "0", true, false },
	{0, 0, 0, false, false },
};

CAutofocus::CAutofocus()
{
	m_nPane = 0;
}

IUnknown *CAutofocus::DoGetInterface( const IID &iid )
{
	//serg
	VTMessageBox("DoGetInterfaceAutoFocus", "VT5", MB_OK);
	//
	if (iid == IID_ILongOperation)
		return (ILongOperation*)this;
	else
		return CAction::DoGetInterface(iid);
}

static int s_nRowsPer1Calc = 1;

int _CalcFocus0(IImage3 *ptrImage, int nPane)
{
	int cx,cy;
	ptrImage->GetSize(&cx, &cy);
	int y = cy/2;
	color *pRow;
	double dSum = 0.;
	int nSum = 0;
	for (int y = cy/4; y < cy-cy/4; y += s_nRowsPer1Calc)
	{
		ptrImage->GetRow(y, nPane, &pRow);
		color nMin = 0xFFFF,nMax = 0;
		for (int x = cx/4; x < cx-cx/4; x++)
		{
			if (pRow[x] < nMin) nMin = pRow[x];
			if (pRow[x] > nMax) nMax = pRow[x];
		}
		dSum += double(abs(nMax-nMin));
		nSum++;
	}
	return nSum>0?int(dSum/nSum):0;
}

int _CalcFocus1(IImage3 *ptrImage, int nPane)
{
	int cx,cy;
	ptrImage->GetSize(&cx, &cy);
	color *pRow;
	double dSum = 0.;
	int nSum = 0;
	for (int y = cy/4; y < cy-cy/4; y += s_nRowsPer1Calc)
	{
		ptrImage->GetRow(y, nPane, &pRow);
		int nMax = 0;
		for (int x = cx/4; x < cx-1-cx/4; x++)
		{
			int nDiff = pRow[x+1]-pRow[x];
			if (nDiff > nMax) nMax = nDiff;
		}
		dSum += double(nMax);
		nSum++;
	}
	return nSum>0?int(dSum/nSum):0;
}

static BOOL _sort(const void*p1,const void*p2)
{
	return *(color*)p1>*(color*)p2?1:*(color*)p1==*(color*)p2?0:-1;
}

int _CalcFocus2(IImage3 *ptrImage, int nPane)
{
	int cx,cy;
	ptrImage->GetSize(&cx, &cy);
	int y = cy/2;
	color *pRow;
	int nSum = 0;
	double dSum = 0;
	_ptr_t2<color> arr(cx/2);
	for (int y = cy/4; y < cy-cy/4; y += s_nRowsPer1Calc)
	{
		ptrImage->GetRow(y, nPane, &pRow);
		color nMin = 0xFFFF,nMax = 0;
		memcpy((color*)arr,pRow+cx/4,cx/2);
		qsort((color*)arr,cx/2,sizeof(color),_sort);
		int n = max(cx/16,1);
		double dSum1 = 0.,dSum2 = 0.;
		for (int i = 0; i < n; i++)
			dSum1 += double(arr[i]);
		dSum1 /= n;
		for (i = 0; i < n; i++)
			dSum2 += double(arr[cx/2-i-1]);
		dSum2 /= n;
		double dDiff = dSum1>dSum2?dSum1-dSum2:dSum2-dSum1;
		dSum += dDiff;
		nSum++;
	}
	return int(dSum/nSum);
}

static int s_nCalcMethod = 0;
static bool s_bHardwareFocus = false;

int _CalcFocus(IImage3 *pimg, int nPane)
{
	if (s_nCalcMethod == 1)
		return _CalcFocus1(pimg, nPane);
	else if (s_nCalcMethod == 2)
		return _CalcFocus2(pimg, nPane);
	else
		return _CalcFocus0(pimg, nPane);
}

class _CDriverSite : public IDriverSite
{
public:
	int nFrames;
	_CDriverSite()
	{
		nFrames = 0;
	}
	dummy_unknown()
	com_call Invalidate()
	{
		nFrames++;
		return S_OK;
	}
	com_call OnChangeSize()
	{
		return S_OK;
	}
};

static void WaitNextFrame(IUnknown *punkDrv, int nDevice)
{
	//???????????????????????????????????	
	char temp[150];
	char temp1[150];
	DWORD dwStart = GetTickCount();
	DWORD dwEnd;
	DWORD dwSub;
	DWORD dwNow = dwStart;	
	 DWORD  dwEndF;
	//?????????????????????????????????????

	IInputPreview2Ptr sptrIP2(punkDrv);
	IInputPreviewPtr sptrIP(punkDrv);
	if (sptrIP2 != 0)
	{
		DWORD dwFreq = 0;
		if (SUCCEEDED(sptrIP2->GetPreviewFreq(nDevice, &dwFreq)))
		{
			if (dwFreq>0)
				Sleep(2*dwFreq);
			else
			{
				//?????????????????????????????
	   dwEnd = GetTickCount();
       dwSub=dwEnd-dwStart;
	   sprintf(temp,"Glav if 1 else  Время =%d мс ---------",dwSub);
	   fprintf(fp1,"%s\n",temp);	
	//???????????????????????????????????????
				_CDriverSite DS;
				sptrIP2->BeginPreview(nDevice,&DS);
		//?????????????????????????????
	 DWORD  dwEndB = GetTickCount();
       dwSub=dwEndB-dwEnd;
	   sprintf(temp,"BeginPreview  Время =%d мс ---------",dwSub);
	   fprintf(fp1,"%s\n",temp);	
	//???????????????????????????????????????
				while (DS.nFrames < 2)
					Sleep(0);
		//?????????????????????????????
	  DWORD  dwEndW = GetTickCount();
       dwSub=dwEndW-dwEndB;
	   sprintf(temp,"while (DS.nFrames < 2) Sleep(0)  Время =%d мс ---------",dwSub);
	   fprintf(fp1,"%s\n",temp);	
	//???????????????????????????????????????
				sptrIP2->EndPreview(nDevice,&DS);
			}
		}
		else
			Sleep(200);
	}
	else if (sptrIP != 0)
	{
		DWORD dwFreq = 0;
		if (SUCCEEDED(sptrIP->GetPreviewFreq(&dwFreq)))
		{
			if (dwFreq>0)
				Sleep(2*dwFreq);
			else
			{
				_CDriverSite DS;
				sptrIP->BeginPreview(&DS);
				while (DS.nFrames < 2)
					Sleep(0);
				sptrIP->EndPreview(&DS);
			}
		}
		else
			Sleep(200);
	}
	else
		Sleep(200);
}

static CTracePoint m_TPMove;
static CTracePoint m_TPWaitFrame;
static CTracePoint1 m_TPInput(&m_TPWaitFrame);
static CTracePoint1 m_TPCalcFocus(&m_TPInput);
static DWORD s_nSteps = 0;

int _SafeCalcFocus(IImage4 *pimg, IDriver *pDrv, IDriver5 *pDrv5, int nDevice, int nAttempts, int nPane)
{
	//???????????????????????????????????
	//if((fp1=fopen("TimeFocus.txt","w"))==NULL)
	//	VTMessageBox("ERROR OPEN FILE SERGEY.txt", "VT5", MB_OK);
	char temp[150];
	char temp1[150];
	DWORD dwStart = GetTickCount();
	DWORD dwEnd;
	DWORD dwSub;
	DWORD dwNow = dwStart;	
	 DWORD  dwEndF;
	//?????????????????????????????????????
	if (s_bHardwareFocus)
	{
		WaitNextFrame(pDrv, nDevice);
		int nSum = 0;
		nAttempts = max(nAttempts,1);
		for (int i = 0; i < nAttempts; i++)
		{
			m_TPWaitFrame.start();
			WaitNextFrame(pDrv, nDevice);
			int nFocus = 0;
			pDrv5->GetFocusIndicator(&nFocus);
			nSum += nFocus;
		}
		return nSum/nAttempts;
	}
	else
	{
		int nSum = 0,nImages = 0;
		for (int i = 0; i < nAttempts; i++)
		{
	//?????????????????????????????
	   dwEnd = GetTickCount();
       dwSub=dwEnd-dwStart;
	   sprintf(temp,"FOCUS BEGIN Время =%d мс ---------",dwSub);
	   fprintf(fp1,"%s\n",temp);	
	//???????????????????????????????????????
			m_TPWaitFrame.start();
			WaitNextFrame(pDrv, nDevice);
			//?????????????????????????????
	  DWORD dwEndW = GetTickCount();
       dwSub=dwEndW-dwEnd;
	   sprintf(temp,"WaitNextFrame Время =%d мс ---------",dwSub);
	   fprintf(fp1,"%s\n",temp);	
	//???????????????????????????????????????
			m_TPInput.start();
			if (FAILED(pDrv->InputNative(pimg, NULL, nDevice, FALSE)))
				continue;
			m_TPCalcFocus.start();
			int nPanesCount;
			pimg->GetPanesCount(&nPanesCount);
		//?????????????????????????????
	   dwEndF = GetTickCount();
       dwSub=dwEndF-dwEndW;
	   sprintf(temp,"_SafeCalcFocus UNDO _CalcFocus Время =%d мс ---------",dwSub);
	   fprintf(fp1,"%s\n",temp);	
	//???????????????????????????????????????
			int nFocus = _CalcFocus(pimg, min(nPane,nPanesCount-1));
			m_TPCalcFocus.stop();
			nSum += nFocus;
			nImages++;
		}
		//?????????????????????????????
	DWORD dwEndC = GetTickCount();
    dwSub=dwEndC-dwEndF;
	sprintf(temp,"_CalcFocus Время =%d мс ---------",dwSub);
	dwSub=dwEndC-dwStart;
	sprintf(temp,"TIME FOCUS Время =%d мс ---------",dwSub);
	fprintf(fp1,"%s\n",temp);
	//fclose(fp1);
	//???????????????????????????????????????
		return nImages?nSum/nImages:0;
	}
}

class CSetFastPreview
{
public:
	int m_nMode;
	IInputPreview2Ptr m_sptrIP2;
	int m_nDevice;
	CSetFastPreview() {};
	void Set(IUnknown *punkDrv, int nDevice)
	{
		m_sptrIP2 = punkDrv;
		m_nDevice = nDevice;
		if (m_sptrIP2)
		{
			if (m_sptrIP2->GetPreviewMode(nDevice, &m_nMode) != S_OK)
				m_sptrIP2 = 0;
			else
				m_sptrIP2->SetPreviewMode(nDevice, Preview_FastCaptureSmall);
		}
	}
	void Restore()
	{
		if (m_sptrIP2)
			m_sptrIP2->SetPreviewMode(m_nDevice, m_nMode);
	}
};
//sergey 8.12.05
bool CAutofocus::StepZ(IDriver *pDrv, IDriver5 *pDrv5, int nDevice, int dz, IImage4 *pimg, int *pnFocus, int nFrames,int ZTime)
{
	//???????????????????????????????????
	char temp[150];
	char temp1[150];
	DWORD dwStart = GetTickCount();
	DWORD dwEnd;
	DWORD dwSub;
	DWORD dwNow = dwStart;
	int Poz=0;
	//?????????????????????????????????????
	m_TPMove.start();
	
	
		//if (m_Stage->MoveU(dz) != S_OK)
		//return false;
	
	if (m_Stage->MoveZ(dz) != S_OK)
		return false;
	//?????????????????????????????
	dwEnd = GetTickCount();
    dwSub=dwEnd-dwStart;
	sprintf(temp,"MoveZ Время =%d мс ---------",dwSub);
	fprintf(fp,"%s\n",temp);
	//???????????????????????????????????????
    //int ZTime=GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "ZTime", 20);
	if (m_Stage->WaitForReadyZ(ZTime) != S_OK)
	{
		//VTMessageBox(IDS_TIMEOUT_EXPIRED, app::get_instance()->handle(), "VT5", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	//?????????????????????????????
	DWORD dwEndW = GetTickCount();
    dwSub=dwEndW-dwEnd;
	sprintf(temp,"WaitForReadyZ Время =%d мс ---------",dwSub);
	fprintf(fp,"%s\n",temp);
	//???????????????????????????????????????
    //sergey 24.11.2005
	  //m_Stage->Exec(SysAllocString(L"I")); 
	//m_Stage->StopI();
	//?????????????????????????????
	DWORD dwEndWI = GetTickCount();
    dwSub=dwEndWI-dwEndW;
	sprintf(temp,"StopI() Время =%d мс ---------",dwSub);
	fprintf(fp,"%s\n",temp);
	//???????????????????????????????????????
		
	//end 
   
	m_TPMove.stop();
	
	s_nSteps++;	
	//?????????????????????????????
	 if (m_Stage->GetPositionZ(&Poz) !=S_OK)
		return false;
	dwEnd = GetTickCount();
    dwSub=dwEnd-dwStart;
	sprintf(temp,"STEP Шаг № %d,Длина шага %d мкм,Время %d мс ---------Poz=%d---",s_nSteps,dz,dwSub,Poz);	
	dwStart = GetTickCount();
	//???????????????????????????????????????
	if (pnFocus)
		*pnFocus = _SafeCalcFocus(pimg,pDrv,pDrv5,nDevice,nFrames,m_nPane);
	//??????????????????????????????????????????????
	dwEnd = GetTickCount();
	dwSub=dwEnd-dwStart;
	sprintf(temp1,"STEP Фокус- %d,Время %d мс",*pnFocus ,dwSub);
	strcat(temp,temp1);
	fprintf(fp,"%s\n",temp);

	//??????????????????????????????????????????????
	return true;
}

//end

bool CAutofocus::Step(IDriver *pDrv, IDriver5 *pDrv5, int nDevice, int dz, IImage4 *pimg, int *pnFocus, int nFrames,int ZTime)
{
	m_TPMove.start();
	if (m_Stage->MoveZ(dz) != S_OK)
		return false;
	//sergey 19.04.06
	 //int ZTime=GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "ZTime", 20);
	    if (m_Stage->WaitForReady(ZTime) != S_OK)
	//end
	//if (m_Stage->WaitForReady(10000) != S_OK)	
	{
		dprintf("IDS_TIMEOUT_EXPIRED ZTime = %d\n", ZTime);
		//VTMessageBox(IDS_TIMEOUT_EXPIRED, app::get_instance()->handle(), "VT5", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	m_TPMove.stop();
	s_nSteps++;
	//sergey 19.04.06
	/*while(m_Stage->IsReady()!= S_OK)
	{
	}*/
	//end
	if (pnFocus)
		*pnFocus = _SafeCalcFocus(pimg,pDrv,pDrv5,nDevice,nFrames,m_nPane);
	return true;
}

bool CAutofocus::Search(IDriver *pDrv, IDriver5 *pDrv5, int nDev, int nStep, IImage4 *pimg, int nFrames,
	int nSteps, int &nFocus, int nNotifyPos,int ZTime)
{
	
	int nFocusNow = 0;
	Notify(nNotifyPos);
	bool bFocusChanged = false;
	for (int i = 0; i < nSteps; i++)
	{
		if (!Step(pDrv,pDrv5,nDev,nStep,pimg,&nFocusNow,nFrames,ZTime))
		{
			dprintf("Step failed in autofocus, i = %d, nStep = %d\n", i, nStep);
			break;
		}
		nNotifyPos += 10;
		dprintf(" step %d, nFocusNow = %d, nFocus = %d, nNotifyPos = %d\n", i, nFocusNow, nFocus, nNotifyPos);
		Notify(nNotifyPos);
		bool b = nFocusNow < nFocus;
		if (nFocus != nFocusNow) bFocusChanged = true;
		nFocus = nFocusNow;
		if (b) break;
	}
	return bFocusChanged;
}




//.....Новая функция Автофокуса-Sergey 10.11.05



bool CAutofocus::SearchZ(IDriver *pDrv, IDriver5 *pDrv5, int nDev, int nStep, IImage4 *pimg, int nFrames,
	 int &nFocus, int nNotifyPos, int nZMax, int nZMin,int ZTime)
{    
	//?????????????????????????????????????
	//???????????????????????????????????
	char temp[150];
	char temp1[150];
	DWORD dwStart;
	DWORD dwEnd;
	DWORD dwSub;
	DWORD dwNow;
	//??????????????????????
		//dwStart = GetTickCount();		 
		//???????????????????????
	//?????????????????????????????????????
	
	int nFocusNow = 0;
    bool bFocusChanged = false;
	int PozLast =0;
	int Poz =0;	
	bool bFocus=false;	
	int i=0;   

	Notify(nNotifyPos);	

     if (m_Stage->GetPositionZ(&Poz) !=S_OK)
		return false;
	 sprintf(temp,"BEGIN SEARCHZ Шаг  № %d,Длина шага %d мкм,POZ= %d  ---------",s_nSteps,nStep,Poz);
		fprintf(fp,"%s\n",temp);

    while(!bFocus && Poz <= nZMax && Poz >= nZMin)
	{    
		i++;
		PozLast=Poz;	 
     if (!StepZ(pDrv,pDrv5,nDev,nStep,pimg,&nFocusNow,nFrames,ZTime))
		 {
			dprintf("Step failed in autofocus, i = %d, nStep = %d\n", i, nStep);
			break;
		 }
		if (m_Stage->GetPositionZ(&Poz) !=S_OK)
		return false;

     nNotifyPos += 10;
     dprintf(" step %d, nFocusNow = %d, nFocus = %d, nNotifyPos = %d\n", i, nFocusNow, nFocus, nNotifyPos);
	 Notify(nNotifyPos);

        bFocus = nFocusNow < nFocus;
		if (nFocus != nFocusNow) bFocusChanged = true;		
		nFocus = nFocusNow;
		 //?????????????????????????????
	
	//sprintf(temp,"Шаг обратно № %d,Длина шага %d мкм,Время %d мс ---------",s_nSteps,-nStep,dwSub);	
		sprintf(temp,"Шаг  № %d,Длина шага %d мкм,POZ %d  ---------LastPoz=%d------",s_nSteps,nStep,Poz,PozLast);
		fprintf(fp,"%s\n",temp);
	
	//???????????????????????????????????????
	 }
	
	if (bFocus)			  
	{   		
		//??????????????????????
		dwStart = GetTickCount();
		 dwNow = dwStart;
		//???????????????????????
		s_nSteps++;
		//if (m_Stage->MoveU(-nStep) != S_OK)
		//return false;
		m_Stage->MoveToZZ(PozLast);
		//m_Stage->MoveZ(-nStep);
		//int ZTime=GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "ZTime", 20);
		if (m_Stage->WaitForReadyZ(ZTime) != S_OK)
	{
		//VTMessageBox(IDS_TIMEOUT_EXPIRED, app::get_instance()->handle(), "VT5", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
    //sergey 24.11.2005
	  //m_Stage->Exec(SysAllocString(L"I")); 
	m_Stage->StopI();

	  //?????????????????????????????
	 if (m_Stage->GetPositionZ(&Poz) !=S_OK)
		return false;
	dwEnd = GetTickCount();
    dwSub=dwEnd-dwStart;
	sprintf(temp,"Шаг обратно № %d,Длина шага %d мкм,Время %d мс -----Poz=%d------",s_nSteps,-nStep,dwSub,Poz);	
	dwStart = GetTickCount();
	//???????????????????????????????????????
		
	//end 
        nFocus = _SafeCalcFocus(pimg,pDrv,pDrv5,nDev,nFrames,m_nPane);

    //??????????????????????????????????????????????
	dwEnd = GetTickCount();
	dwSub=dwEnd-dwStart;
	sprintf(temp1,"Фокус- %d,Время %d мс",nFocus ,dwSub);
	strcat(temp,temp1);
	fprintf(fp,"%s\n",temp);	
	//??????????????????????????????????????????????
	}
	
	return bFocusChanged;
}
//    New ALGORITM
   bool CAutofocus::ALGORITM(IDriver *pDrv, IDriver5 *pDrv5, int nDev, int nStep, IImage4 *pimg, int nFrames,
	 int &nFocus, int nNotifyPos,bool bUpDown)
{
	//???????????????????????????????????
	char temp[150];
	char temp1[150];
	DWORD dwStart = GetTickCount();
	DWORD dwEnd;
	DWORD dwSub;
	DWORD dwNow = dwStart;
	DWORD dwEndW;	
	DWORD dwEndP;
	//?????????????????????????????????????
	int nFocusNow = 0;
    bool bFocusChanged = false;
	int PozLast =0;
	int Poz =0;	
	bool bFocus=false;	
	int i=0; 
	int j=2; 
	int max=0;
	int nmax=0;
	int bUpD=1;

	Notify(nNotifyPos);
	for(int s=0;s<10;s++)
	{
	if(bUpDown)	m_Stage->MoveU(8);
	else 
	{
        bUpD=0;
		m_Stage->MoveD(8);
	}
	//?????????????????????????????
	m_Stage->GetPositionZ(&Poz);
	dwEnd = GetTickCount();
    dwSub=dwEnd-dwStart;	
	sprintf(temp,"MoveU ||MoveD Время =%d мс ---------POZ=%d------UP ||DOWN =%d",dwSub,Poz,bUpD);
	fprintf(fp,"%s\n",temp);
	ArFoPoz[1].iFocus=nFocus;
	ArFoPoz[1].iPoz=Poz;
	//???????????????????????????????????????
	//while((Poz<70)&&(Poz>-70))	
	//{
    nFocus = _SafeCalcFocus(pimg,pDrv,pDrv5,nDev,nFrames,m_nPane);
	//?????????????????????????????
	 dwEndW = GetTickCount();
    dwSub=dwEndW-dwEnd;
	sprintf(temp,"J=%d-----Focus=%d----Время =%d мс ---------",j,nFocus,dwSub);
	fprintf(fp,"%s\n",temp);
	//???????????????????????????????????????
    if (m_Stage->GetPositionZ(&Poz) !=S_OK)
		return false;
	//?????????????????????????????
	dwEndP = GetTickCount();
    dwSub=dwEndP-dwEndW;
	sprintf(temp,"J=%d-----Poz=%d----Время =%d мс ---------",j,Poz,dwSub);
	fprintf(fp,"%s\n",temp);
	dwEnd = GetTickCount();
	dwStart=GetTickCount();
	//???????????????????????????????????????
	
	ArFoPoz[j].iFocus=nFocus;
	ArFoPoz[j].iPoz=Poz;
	j++;	
	}
	max=ArFoPoz[0].iFocus;
	for(int y=1;y<=j;y++)
	{
     if (ArFoPoz[y].iFocus > max)
	 {
		 max=ArFoPoz[y].iFocus;
	     nmax=y;
	  }
	}
	//?????????????????????????????
	m_Stage->StopI();
	m_Stage->GetPositionZ(&Poz);
	 dwEndW = GetTickCount();
    dwSub=dwEndW-dwEnd;
	sprintf(temp,"Время undo =%d мс ---------Pozi=%d----",dwSub,Poz);
	fprintf(fp,"%s\n",temp);
    
	//???????????????????????????????????????
    m_Stage->MoveToZ(ArFoPoz[nmax].iPoz);
    
	//?????????????????????
	/*if (m_Stage->WaitForReady(100) != S_OK)
	{
		VTMessageBox(IDS_TIMEOUT_EXPIRED, app::get_instance()->handle(), "VT5", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}*/
	//m_Stage->StopI();
	//?????????????????????????????
	m_Stage->GetPositionZ(&Poz);
		
	 dwEnd = GetTickCount();
    dwSub=dwEnd-dwEndW;
	sprintf(temp,"Время MoveToZ =%d мс ---------Poz=%d-----ARRAY Focus=%d---POz=%d---",dwSub,Poz,ArFoPoz[nmax].iFocus,ArFoPoz[nmax].iPoz);
	fprintf(fp,"%s\n",temp);
	//???????????????????????????????????????
     nNotifyPos += 10;
     dprintf(" step %d, nFocusNow = %d, nFocus = %d, nNotifyPos = %d\n", i, nFocusNow, nFocus, nNotifyPos);
	 Notify(nNotifyPos);
}
//  END ALGORITM

HRESULT CAutofocus::DoInvoke()
{
	//????????????????????????????????
	ArFoPoz=new FocusPoz[550];
	
	bool bUpDown=true;
	DWORD dwStart=GetTickCount();
	DWORD dwEnd;
	DWORD dwNow;
	char TemAutoFocus[50];
	char TemAutoFocusEnd[50];
	sprintf(TemAutoFocus,"Time  autofocus begin= %d ------ ",dwStart);
	//???????????????????????????????????????????????????
	if((fp1=fopen("TimeFocus.txt","w"))==NULL)
		VTMessageBox("ERROR OPEN FILE SERGEY.txt", "VT5", MB_OK);
    if((fp=fopen("sergey.txt","w"))==NULL)
		VTMessageBox("ERROR OPEN FILE SERGEY.txt", "VT5", MB_OK);
        // MessageBox("ERROR OPEN FILE SERGEY.txt","VT5",MB_OK|MB_ICONEXCLAMATION);	
	fprintf(fp,"%s\n",TemAutoFocus);
	//?????????????????????????????????
	int StepsZ=0;
	Init();
	CTracePoint AFTotal;
	AFTotal.start();
	IUnknownPtr punkDrvMan(_GetOtherComponent(GetAppUnknown(), IID_IDriverManager), false);
	IDriverManagerPtr sptrDrvMan(punkDrvMan);
	BOOL bExist;
	IUnknownPtr punkDrv;
	int nDev;
	sptrDrvMan->GetCurrentDeviceAndDriver(&bExist, &punkDrv, &nDev);
	IDriverPtr sptrDrv(punkDrv);
	IDriver5Ptr sptrDrv5(punkDrv);
	if (bExist == FALSE || sptrDrv == 0)
	{
		VTMessageBox(IDS_DEVICE_NOT_SELECT, app::get_instance()->handle(), "VT5", MB_OK|MB_ICONEXCLAMATION);
		return E_FAIL;
	}
	DWORD dwDrvFlags = 0;
	sptrDrv->GetFlags(&dwDrvFlags);
	IUnknownPtr punkImage(::CreateTypedObject(_bstr_t("Image")), false);
	IImage4Ptr sptrImage(punkImage);
	sptrDrv->InputNative(sptrImage, NULL, nDev, FALSE);
	/*int nStep = GetArgLong("InitialStepMcs");
	if (nStep == 0)
		nStep = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "StepMcm", -10);*/

	int nFrames = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "Frames", 10);
	s_nCalcMethod = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "CalcMethod", 0);
	s_bHardwareFocus = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "HardwareFocus", FALSE)?true:false;
	if ((dwDrvFlags&FG_FOCUSINDICATOR)==0||sptrDrv5==0)
		s_bHardwareFocus = false;
	s_nRowsPer1Calc = max(GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "RowsPerCalc", 1), 1);
	m_nPane = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "ColorPane", 1);

	CSetFastPreview FastPreview;
	if (!s_bHardwareFocus)
		FastPreview.Set(sptrDrv,nDev);
	int nFocus = _SafeCalcFocus(sptrImage,sptrDrv,sptrDrv5,nDev,sptrImage,m_nPane);
	//??????????????????????????????????
	dwEnd=GetTickCount();
    dwNow=dwEnd-dwStart;
	sprintf(TemAutoFocusEnd,"Time  Focus begin End= %d ------ Time= %d------Focus=%d-----",dwEnd,dwNow,nFocus);
	fprintf(fp,"%s\n",TemAutoFocusEnd);
	
	//????????????????
	dprintf("Autofocus started, Focus=%d\n", nFocus);
	int nNotify = 10;


    bool ZEnable = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "ZEnable", FALSE)?true:false;//выбор функции фокуса
	
    int CaseFocus=0;
    if(ZEnable) CaseFocus=1;
    int nZMax;
	int nZMin;
	int nStep;	
	int ZTime;
	int Poz =0;
	int nFocusNow = 0;
	BSTR s;
	DWORD dwEndP;
	DWORD dwEndE;
	ZTime = GetArgLong("ZTimeWait");
	if (ZTime == 0)
	ZTime = GetValueInt(GetAppUnknown(),"Stage\\Autofocus\\ZEnabled", "ZTimeWait",20);
	
	
 switch(CaseFocus)
      {
      case 1: 
		  
         nStep = GetArgLong("InitialStepMcs");
	     if (nStep == 0)
		     nStep = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "StepMcmZ", 27);
          
		 nZMax = GetArgLong("ZMaxMcm");
	     if (nZMax == 0)
		      nZMax = GetValueInt(GetAppUnknown(),"Stage\\Autofocus\\ZEnabled", "ZMaxMcm",200);
	     nZMin = GetArgLong("ZMinMcm");
	     if (nZMin == 0)
	          nZMin = GetValueInt(GetAppUnknown(),"Stage\\Autofocus\\ZEnabled", "ZMinMcm",-200);	
	       
		 StepsZ=10;
	     StartNotification((13*(StepsZ+StepsZ)+1)*10);      
	     Notify(10);
      //One STEP

		 char* szReplyBuff;
	    
		//s=SysAllocString(L"Z");
		//szReplyBuff=m_Stage->Exec(s);
		m_Stage->PozZ();
		//??????????????????????????????????
	 dwEndE=GetTickCount();
    dwNow=dwEndE-dwEnd;
	sprintf(TemAutoFocusEnd,"Time  PozZerro End= %d ------ Time= %d------",dwEndE,dwNow);
	fprintf(fp,"%s\n",TemAutoFocusEnd);
	
	//????????????????
		
		//if(strcmp(szReplyBuff,"Eror"))
			//break;
        if (m_Stage->GetPositionZ(&Poz) !=S_OK)
		break;
		//??????????????????????????????????
	 dwEndP=GetTickCount();
    dwNow=dwEndP-dwEndE;
	sprintf(TemAutoFocusEnd,"DOINVOKE Time  GetPoz End= %d ------ Time= %d------POZ=%d",dwEndP,dwNow,Poz);
	fprintf(fp,"%s\n",TemAutoFocusEnd);
	ArFoPoz[0].iFocus=nFocus;
	ArFoPoz[0].iPoz=Poz;
	//????????????????
	 if(Poz==0)
	 { 
		 int i=0;
		 if (!StepZ(sptrDrv,sptrDrv5,nDev,nStep,sptrImage,&nFocusNow,nFrames,ZTime))
		 {
			dprintf("Step failed in autofocus, i = %d, nStep = %d\n", i, nStep);
			break;
		 }

		 //?????????????????????????????
	dwEndE = GetTickCount();
    dwNow=dwEndE-dwEndP;
	sprintf(TemAutoFocusEnd,"DOINVOKE Шаг № %d,Длина шага %d мкм,Время %d мс ---------",s_nSteps,nStep,dwNow);	
	fprintf(fp,"%s\n",TemAutoFocusEnd);
	//???????????????????????????????????????
		if (nFocusNow < nFocus)
		{
			nStep = -nStep;
			bUpDown=false;
			//...........
			m_Stage->MoveToZ(0);
			//if (m_Stage->MoveU(nStep ) != S_OK)
		    // return false;
			//int ZTime=GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "ZTime", 20);
			if (m_Stage->WaitForReadyZ(ZTime) != S_OK)
	         {
		      //VTMessageBox(IDS_TIMEOUT_EXPIRED, app::get_instance()->handle(), "VT5", MB_OK|MB_ICONEXCLAMATION);
		      return false;
	           }
			// m_Stage->StopI();
			 nFocus = _SafeCalcFocus(sptrImage,sptrDrv,sptrDrv5,nDev,sptrImage,m_nPane);
			  //?????????????????????????????
	          dwEndP = GetTickCount();
             dwNow=dwEndP-dwEndE;
	       sprintf(TemAutoFocusEnd,"DOINVOKE MoveToZ(0)Время %d мс ---------Focus=%d--------",dwNow,nFocus);	
	      fprintf(fp,"%s\n",TemAutoFocusEnd);
	//???????????????????????????????????????
	   //sprintf(TemAutoFocusEnd,"DOINVOKE MoveToZ(0)");	
	  //fprintf(fp,"%s\n",TemAutoFocusEnd);
		}
		else   nFocus = nFocusNow;

		nNotify += 10;
        dprintf(" step %d, nFocusNow = %d, nFocus = %d, nNotify = %d\n", i, nFocusNow, nFocus, nNotify);
	    

	//END One Step
		//New ALGORITM
		//bool bFocusChg1 = ALGORITM(sptrDrv,sptrDrv5,nDev,nStep,sptrImage,nFrames,nFocus,nNotify,bUpDown);
		//END ALGORITM
//ОТЛАДКА
	   for (int j = 0; j < 3; j++)
	      {
	       dprintf("phase %d, nStep = %d\n", i, nStep);
		   
		   bool bFocusChg1 = SearchZ(sptrDrv,sptrDrv5,nDev,nStep,sptrImage,nFrames,nFocus,nNotify,nZMax,nZMin,ZTime);
		   if (!bFocusChg1) break;
		   nNotify += StepsZ*10;
		   nStep /= 3;
		   if (abs(nStep) < 1) break;
		   StepsZ *=3;		   		   
	       }
	 }
	    break ; 
      case 0:
		  nStep = GetArgLong("InitialStepMcs");
	      if (nStep == 0)
		  nStep = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "StepMcm", -10);
		  int nStepsUp = GetArgLong("StepsUp");
	      if (nStepsUp == 0)
		      nStepsUp = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "StepsUp", 10);
	      int nStepsDown = GetArgLong("StepsDown");
	      if (nStepsDown == 0)
		      nStepsDown = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "StepsDown", 10);

          StartNotification((13*(nStepsUp+nStepsDown)+1)*10);
		  Notify(10);

	   for (int i = 0; i < 3; i++)
	     {
		  dprintf("phase %d, nStep = %d\n", i, nStep);
		  int nSteps = nStep>0?nStepsUp:nStepsDown;
		  bool bFocusChg1 = Search(sptrDrv,sptrDrv5,nDev,nStep,sptrImage,nFrames,nSteps,nFocus,nNotify,ZTime);
		  nNotify += nSteps*10;
		  dprintf("phase %d, nStep = %d\n", i, -nStep);
		  nSteps = nStep>0?nStepsDown:nStepsUp;
		  bool bFocusChg2 = Search(sptrDrv,sptrDrv5,nDev,-nStep,sptrImage,nFrames,nSteps,nFocus,nNotify,ZTime);
		  if (!bFocusChg1 && !bFocusChg2) break;
		  nNotify += nSteps*10;
		  nStep /= 3;
		  if (abs(nStep) < 1) break;
		  nStepsUp *= 3;
		  nStepsDown *= 3;
	      }
	   
	    break;
     }

    FinishNotification();	
	if (!s_bHardwareFocus)
		FastPreview.Restore();
	AFTotal.stop();
	dprintf("Total autofocus time: %d\nsteps: %d, moving stage: %dms\n", AFTotal.m_dwTotal,
		s_nSteps, m_TPMove.m_dwTotal);
	dprintf("waiting 2 frames: %d, input: %d, calculating focus: %d\n\n", m_TPWaitFrame.m_dwTotal,
		m_TPInput.m_dwTotal, m_TPCalcFocus.m_dwTotal);
	//sergey
	//??????????????????????????????????
	m_Stage->GetPositionZ(&Poz);
	dwEnd=GetTickCount();
    dwNow=dwEnd-dwStart;
	sprintf(TemAutoFocusEnd,"Time  autofocus End= %d ------ Time= %d-----Poz=%d---",dwEnd,dwNow,Poz);
	fprintf(fp,"%s\n",TemAutoFocusEnd);
	fclose(fp);
	fclose(fp1);
	delete ArFoPoz;
	//????????????????
	char* temp;
	sprintf(temp,"%d",s_nSteps);
	//VTMessageBox(temp, "VT5", MB_OK);
	//end
	m_TPMove.m_dwTotal = m_TPInput.m_dwTotal = m_TPCalcFocus.m_dwTotal = m_TPWaitFrame.m_dwTotal = 0;
	s_nSteps = 0;
	dflush();
	//sergey 21.04.06
	//dprintf("Error during write to com: RS232 ");
	//end
	return S_OK;

  }



//.....END


/*
HRESULT CAutofocus::DoInvoke()
{
	Init();
	CTracePoint AFTotal;
	AFTotal.start();
	IUnknownPtr punkDrvMan(_GetOtherComponent(GetAppUnknown(), IID_IDriverManager), false);
	IDriverManagerPtr sptrDrvMan(punkDrvMan);
	BOOL bExist;
	IUnknownPtr punkDrv;
	int nDev;
	sptrDrvMan->GetCurrentDeviceAndDriver(&bExist, &punkDrv, &nDev);
	IDriverPtr sptrDrv(punkDrv);
	IDriver5Ptr sptrDrv5(punkDrv);
	if (bExist == FALSE || sptrDrv == 0)
	{
		VTMessageBox(IDS_DEVICE_NOT_SELECT, app::get_instance()->handle(), "VT5", MB_OK|MB_ICONEXCLAMATION);
		return E_FAIL;
	}
	DWORD dwDrvFlags = 0;
	sptrDrv->GetFlags(&dwDrvFlags);
	IUnknownPtr punkImage(::CreateTypedObject(_bstr_t("Image")), false);
	IImage4Ptr sptrImage(punkImage);
	sptrDrv->InputNative(sptrImage, NULL, nDev, FALSE);
	int nStep = GetArgLong("InitialStepMcs");
	if (nStep == 0)
		nStep = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "StepMcm", -10);
	int nStepsUp = GetArgLong("StepsUp");
	if (nStepsUp == 0)
		nStepsUp = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "StepsUp", 10);
	int nStepsDown = GetArgLong("StepsDown");
	if (nStepsDown == 0)
		nStepsDown = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "StepsDown", 10);
	int nFrames = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "Frames", 10);
	s_nCalcMethod = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "CalcMethod", 0);
	s_bHardwareFocus = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "HardwareFocus", FALSE)?true:false;
	if ((dwDrvFlags&FG_FOCUSINDICATOR)==0||sptrDrv5==0)
		s_bHardwareFocus = false;
	s_nRowsPer1Calc = max(GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "RowsPerCalc", 1), 1);
	m_nPane = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "ColorPane", 1);
	CSetFastPreview FastPreview;
	if (!s_bHardwareFocus)
		FastPreview.Set(sptrDrv,nDev);
	StartNotification((13*(nStepsUp+nStepsDown)+1)*10);
	int nFocus = _SafeCalcFocus(sptrImage,sptrDrv,sptrDrv5,nDev,sptrImage,m_nPane);
	dprintf("Autofocus started, Focus=%d\n", nFocus);
	Notify(10);
	int nNotify = 10;
	for (int i = 0; i < 3; i++)
	{
		dprintf("phase %d, nStep = %d\n", i, nStep);
		int nSteps = nStep>0?nStepsUp:nStepsDown;
		bool bFocusChg1 = Search(sptrDrv,sptrDrv5,nDev,nStep,sptrImage,nFrames,nSteps,nFocus,nNotify);
		nNotify += nSteps*10;
		dprintf("phase %d, nStep = %d\n", i, -nStep);
		nSteps = nStep>0?nStepsDown:nStepsUp;
		bool bFocusChg2 = Search(sptrDrv,sptrDrv5,nDev,-nStep,sptrImage,nFrames,nSteps,nFocus,nNotify);
		if (!bFocusChg1 && !bFocusChg2) break;
		nNotify += nSteps*10;
		nStep /= 3;
		if (abs(nStep) < 1) break;
		nStepsUp *= 3;
		nStepsDown *= 3;
	}
	FinishNotification();
	if (!s_bHardwareFocus)
		FastPreview.Restore();
	AFTotal.stop();
	dprintf("Total autofocus time: %d\nsteps: %d, moving stage: %dms\n", AFTotal.m_dwTotal,
		s_nSteps, m_TPMove.m_dwTotal);
	dprintf("waiting 2 frames: %d, input: %d, calculating focus: %d\n\n", m_TPWaitFrame.m_dwTotal,
		m_TPInput.m_dwTotal, m_TPCalcFocus.m_dwTotal);
	m_TPMove.m_dwTotal = m_TPInput.m_dwTotal = m_TPCalcFocus.m_dwTotal = m_TPWaitFrame.m_dwTotal = 0;
	s_nSteps = 0;
	dflush();
	return S_OK;
}*/

HRESULT CAutofocus::GetActionState(DWORD *pdwState)
{
	Init();
	*pdwState = IsStageOk()&&IsPreviewView(m_ptrTarget)?afEnabled:0;
	return S_OK;
}

_ainfo_base::arg	CFocusInfo::s_pargs[] = 
{
	{0, 0, 0, false, false },
};

CFocus::CFocus()
{
}

HRESULT CFocus::DoInvoke()
{
//	Init();
	//???????????????????????????????????????????????????sergey 24/04/06
	if((fp1=fopen("TimeFocus.txt","w"))==NULL)
		VTMessageBox("ERROR OPEN FILE SERGEY.txt", "VT5", MB_OK);
	//end
	IUnknownPtr punkDrvMan(_GetOtherComponent(GetAppUnknown(), IID_IDriverManager), false);
	IDriverManagerPtr sptrDrvMan(punkDrvMan);
	BOOL bExist;
	IUnknownPtr punkDrv;
	int nDev;
	sptrDrvMan->GetCurrentDeviceAndDriver(&bExist, &punkDrv, &nDev);
	IDriverPtr sptrDrv(punkDrv);
	IDriver5Ptr sptrDrv5(punkDrv);
	if (bExist == FALSE || sptrDrv == 0)
	{
		VTMessageBox(IDS_DEVICE_NOT_SELECT, app::get_instance()->handle(), "VT5", MB_OK|MB_ICONEXCLAMATION);
		return E_FAIL;
	}
	DWORD dwDrvFlags = 0;
	sptrDrv->GetFlags(&dwDrvFlags);
	IUnknownPtr punkImage(::CreateTypedObject(_bstr_t("Image")), false);
	IImage4Ptr sptrImage(punkImage);
	sptrDrv->InputNative(sptrImage, NULL, nDev, FALSE);
	s_nCalcMethod = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "CalcMethod", 0);
	s_bHardwareFocus = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "HardwareFocus", FALSE)?true:false;
	if ((dwDrvFlags&FG_FOCUSINDICATOR)==0||sptrDrv5==0)
		s_bHardwareFocus = false;
	s_nRowsPer1Calc = max(GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "RowsPerCalc", 1), 1);
	int nPane = GetValueInt(GetAppUnknown(), "Stage\\Autofocus", "ColorPane", 1);
	int nPosPrevZ;
	m_Stage->GetPositionZ(&nPosPrevZ);
	//sergey 26.04.06
	int nFocus = _SafeCalcFocus(sptrImage, sptrDrv, sptrDrv5, nDev, s_nRowsPer1Calc, nPane);
	//int nFocus = _SafeCalcFocus(sptrImage, sptrDrv, sptrDrv5, nDev, 10, nPane);
	//end
	char szBuff[50];
	sprintf(szBuff, "%d, %d", nFocus, nPosPrevZ);

// Запись фокуса в ключ Sergey 10.11.05
	bool s_bOutMessage=GetValueInt(GetAppUnknown(),"Stage\\StageFocusIndicator","MessageBox",0)?true:false;
	SetValue(GetAppUnknown(),"Stage\\StageFocusIndicator","Result",(long)nFocus);
	if (s_bOutMessage) VTMessageBox(szBuff, "VT5", MB_OK);

	fclose(fp1);
//  Конец записи фокуса в ключ End

	//VTMessageBox(szBuff, "VT5", MB_OK);
	return S_OK;
}

HRESULT CFocus::GetActionState(DWORD *pdwState)
{
	Init();
	*pdwState = IsStageOk()&&IsPreviewView(m_ptrTarget)?afEnabled:0;
	return S_OK;
}

_ainfo_base::arg	CUndoMoveInfo::s_pargs[] = 
{
	{0, 0, 0, false, false },
};

CUndoMove::CUndoMove()
{
}

HRESULT CUndoMove::DoInvoke()
{
	m_Stage->RestorePosition();
	return S_OK;
}

HRESULT CUndoMove::GetActionState(DWORD *pdwState)
{
	Init();
	*pdwState = IsStageOk()&&IsPreviewView(m_ptrTarget)&&
		m_Stage->IsRestorePositionAvail()==S_OK?afEnabled:0;
	return S_OK;
}

