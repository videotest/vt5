#include "StdAfx.h"
#include "stageobject.h"
#include "resource.h"
#include "misc_utils.h"
#include "Comm.h"
#include "Debug.h"
#include "LStep4X.h"





//extern "C++" CLStep4X LStep1;

static int _Range(int nValue, int nMin, int nMax)
{
	if (nValue < nMin) return nMin;
	if (nValue > nMax) return nMax;
	return nValue;
}

CStageLimit::CStageLimit()
{
	m_nMin = m_nMax = 0;
	m_bEnable = FALSE;
}

void CStageLimit::Read(const char *pszSecName)
{
	m_bEnable = ::GetValueInt(::GetAppUnknown(), pszSecName, "Enable", FALSE);
	m_nMin = ::GetValueInt(::GetAppUnknown(), pszSecName, "Minimum", 0);
	m_nMax = ::GetValueInt(::GetAppUnknown(), pszSecName, "Maximum", 0);
	if (m_nMin >= m_nMax) m_bEnable = FALSE;
}

int CStageLimit::Range(int nValue)
{
	if (m_bEnable)
		return _Range(nValue, m_nMin, m_nMax);
	else
		return nValue;
}

CStageObject::CStageObject(void)
{
	//Sergey 28/07/06
	//LStep1 = new CLStep4X();
	//LStep1.ConnectSimple(4,"",0,true);
	//end
	m_bstrName = "Stage";
	m_bstrUserName = "Stage";
	m_StageState = NotInited;
	m_dwByteTimeout = 0;
	InitStage();
	m_LimX.Read("Stage\\LimitX");
	m_LimY.Read("Stage\\LimitY");
	m_LimZ.Read("Stage\\LimitZ");
	m_bPosSaved = false;
	
}

CStageObject::~CStageObject(void)
{
}

IUnknown *CStageObject::DoGetInterface(const IID &iid)
{
	//serg
	//VTMessageBox("DoGetInterface", "VT5", MB_OK);
	//
	if (iid == IID_INamedObject2)
	{//serg
	//VTMessageBox("DoGetInterfaceNameOb", "VT5", MB_OK);
	//
		return (INamedObject2 *)this;
	}
	else if (iid == IID_IDispatch)
	{//serg
	VTMessageBox("DoGetInterfaceIDispatch", "VT5", MB_OK);
	//
		return (IDispatch *)this;
	}
	else if (iid == IID_IStage)
	{//serg
	VTMessageBox("DoGetInterfaceIStage", "VT5", MB_OK);
	//
		return (IStage *)this;
	}
	else
	{//serg
	//VTMessageBox("DoGetInterfacePosl", "VT5", MB_OK);
	//
		return ComObjectBase::DoGetInterface(iid);
}
}

int CStageObject::GetPropertyInt(int dispid)
{
	if (dispid == DISPID_AVAIL)
		return m_StageState==Present?1:0;
	if (m_StageState == Absent)
		return 0;
	char szReplyBuff[256];
	if (dispid == DISPID_X)
	{
		if (SendCommandCR("px\r", szReplyBuff, 256) == 0)
			return 0;
		return atoi(szReplyBuff);
	}
	else if (dispid == DISPID_Y)
	{
		if (SendCommandCR("py\r", szReplyBuff, 256) == 0)
			return 0;
		return atoi(szReplyBuff);
	}
	else if (dispid == DISPID_Z)
	{
		if (SendCommandCR("pz\r", szReplyBuff, 256) == 0)
			return 0;
		return atoi(szReplyBuff);
	}
	else if (dispid == DISPID_READY)
		return IsReady()==S_OK?1:0;
	else if (dispid == DISPID_V)
	{
		if (SendCommandCR("sms\r", szReplyBuff, 256) == 0)
			return 0;
		return atoi(szReplyBuff);
	}
	else return 0;
}

void CStageObject::SetPropertyInt(int dispid, int nValue)
{
	if (m_StageState == Absent)
		return;
	char szCmd[256];
	if (dispid == DISPID_X)
	{
		sprintf(szCmd, "gx %d\r", m_LimX.Range(nValue));
		SendCommandChecked(szCmd, "R\r", 2);
	}
	else if (dispid == DISPID_Y)
	{
		sprintf(szCmd, "gy %d\r", m_LimY.Range(nValue));
		SendCommandChecked(szCmd, "R\r", 2);
	}
	else if (dispid == DISPID_Z)
		MoveToZ(nValue);
	else if (dispid == DISPID_V)
		SetVelocity(nValue);
}

void CStageObject::MethodProc2Int(int dispid, int i1, int i2)
{
	if (m_StageState == Absent)
		return;
	if (dispid == DISPID_MOVETO)
		MoveTo(_point(i1,i2));
	else if (dispid == DISPID_MOVE)
		Move(_point(i1,i2));
}

void CStageObject::MethodProc1Int(int dispid, int i)
{
	if (m_StageState == Absent)
		return;
	if (dispid == DISPID_MOVETOZ)
		MoveToZ(i);
	else if (dispid == DISPID_MOVEZ)
		MoveZ(i);
	//Sergey 21.11.2005
	// else if (dispid == DISPID_EXEC)
		//Exec((char *)i);
	//End
}


DISPID CStageObject::DispidOfMember(OLECHAR FAR* pName)
{
	if (_wcsicmp(pName, L"x") == 0)
		return DISPID_X;
	else if (_wcsicmp(pName, L"y") == 0)
		return DISPID_Y;
	else if (_wcsicmp(pName, L"z") == 0)
		return DISPID_Z;
	else if (_wcsicmp(pName, L"MoveTo") == 0)
		return DISPID_MOVETO;
	else if (_wcsicmp(pName, L"Move") == 0)
		return DISPID_MOVE;
	else if (_wcsicmp(pName, L"MoveToZ") == 0)
		return DISPID_MOVETOZ;
	else if (_wcsicmp(pName, L"MoveZ") == 0)
		return DISPID_MOVEZ;
	else if (_wcsicmp(pName, L"Ready") == 0)
		return DISPID_READY;
	else if (_wcsicmp(pName, L"Avail") == 0)
		return DISPID_AVAIL;
	else if (_wcsicmp(pName, L"v") == 0)
		return DISPID_V;
	//Sergey 21.11.2005
	 else if (_wcsicmp(pName, L"Exec") == 0)
		return DISPID_EXEC;
	//End
	else
		return DISPID_UNKNOWN;
}

static bool _IntByVariantArg(VARIANTARG *pvar, int &n)
{
	if (pvar->vt == VT_I4)
	{
		n = pvar->lVal;
		return true;
	}
	else if (pvar->vt == VT_I2)
	{
		n = pvar->iVal;
		return true;
	}
	return false;
}

HRESULT CStageObject::DispatchPropertyInt(int dispid, WORD wFlags, DISPPARAMS FAR *pdispparams,
	VARIANT FAR *pvarResult)
{
	//serg
	VTMessageBox("DispatchPropertyInt", "VT5", MB_OK);
	//
	if (wFlags & DISPATCH_PROPERTYGET)
	{
		pvarResult->vt = VT_I4;
		pvarResult->lVal = GetPropertyInt(dispid);
		return S_OK;
	}
	else if (wFlags & DISPATCH_PROPERTYPUT)
	{
		if (pdispparams->cArgs == 1)
		{
			int n;
			if (_IntByVariantArg(&pdispparams->rgvarg[0], n))
			{
				SetPropertyInt(dispid, n);
				return S_OK;
			}
			else return DISP_E_BADVARTYPE;
		}
		else
			return DISP_E_BADPARAMCOUNT;
	}
	return DISP_E_MEMBERNOTFOUND;
}

HRESULT CStageObject::DispatchProc2Int(int dispid, WORD wFlags, DISPPARAMS FAR *pdispparams)
{
	if (wFlags & DISPATCH_METHOD)
	{
		if (pdispparams->cArgs == 2)
		{
			int i1,i2;
			if (!_IntByVariantArg(&pdispparams->rgvarg[1], i1))
				return DISP_E_BADVARTYPE;
			if (!_IntByVariantArg(&pdispparams->rgvarg[0], i2))
				return DISP_E_BADVARTYPE;
			MethodProc2Int(dispid, i1, i2);
			return S_OK;
		}
		else
			return DISP_E_BADPARAMCOUNT;
	}
	return DISP_E_MEMBERNOTFOUND;
}

HRESULT CStageObject::DispatchProc1Int(int dispid, WORD wFlags, DISPPARAMS FAR *pdispparams)
{
	if (wFlags & DISPATCH_METHOD)
	{
		if (pdispparams->cArgs == 1)
		{
			int i;
			if (!_IntByVariantArg(&pdispparams->rgvarg[0], i))
				return DISP_E_BADVARTYPE;
			MethodProc1Int(dispid, i);
			return S_OK;
		}
		else
			return DISP_E_BADPARAMCOUNT;
	}
	return DISP_E_MEMBERNOTFOUND;
}

HRESULT CStageObject::GetIDsOfNames(REFIID riid, OLECHAR FAR* FAR* rgszNames,
	unsigned int cNames, LCID lcid, DISPID FAR* rgdispid)
{
	//serg
	VTMessageBox("getName", "VT5", MB_OK);
	//
	if (cNames == 0)
		return S_OK;
	else if (cNames >= 1)
	{
		rgdispid[0] = DispidOfMember(*rgszNames);
		for (unsigned i = 1; i < cNames; i++)
			rgdispid[i] = DISPID_UNKNOWN;
		return rgdispid[0]==DISPID_UNKNOWN||cNames>1?DISP_E_UNKNOWNNAME:S_OK;
	}
	else
		return E_FAIL;
}

HRESULT CStageObject::GetTypeInfo(unsigned int itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
	return E_NOTIMPL;
}

HRESULT CStageObject::GetTypeInfoCount(unsigned int FAR * pctinfo)
{
	return E_NOTIMPL;
}

HRESULT CStageObject::Invoke(DISPID dispid, REFIID riid, LCID lcid, WORD wFlags,
	DISPPARAMS FAR *pdispparams, VARIANT FAR *pvarResult, EXCEPINFO FAR * pexecinfo,
	unsigned int FAR *puArgErr)
{
	//serg
	VTMessageBox("InvokeStageObject", "VT5", MB_OK);
	//
	InitStage();
	if ((pvarResult == NULL) && (wFlags == DISPATCH_PROPERTYGET))
		return (E_INVALIDARG);
    switch (dispid)
    {
	case DISPID_X:
	case DISPID_Y:
	case DISPID_Z:
	case DISPID_V:
	case DISPID_READY:
	case DISPID_AVAIL:
		return DispatchPropertyInt(dispid, wFlags, pdispparams, pvarResult);
	case DISPID_MOVETO:
	case DISPID_MOVE:
		return DispatchProc2Int(dispid, wFlags, pdispparams);
	case DISPID_MOVETOZ:
	case DISPID_MOVEZ:
		return DispatchProc1Int(dispid, wFlags, pdispparams);
    //Sergey 21.11.2005
     case DISPID_EXEC:
		 return DispatchProc1BSTR(dispid, wFlags, pdispparams,pvarResult);
     //End

	}
	return DISP_E_MEMBERNOTFOUND;
}

//Sergey 21.11.2005

void CStageObject::MethodProc1BSTR(int dispid, BSTR i,VARIANT FAR *pvarResult,WORD wFlags)
{
	BSTR   m_bstrReply;		
	char *m_pcReply;

	if (m_StageState == Absent)
		return;	
	
	else if (dispid == DISPID_EXEC)
          
	if (wFlags & DISPATCH_PROPERTYGET)
	    {
		
			m_pcReply=Exec(i);
		 WCHAR wszUserName[256];
		 MultiByteToWideChar( CP_ACP, 0, m_pcReply,
        strlen(m_pcReply)+1, wszUserName,   
     sizeof(wszUserName)/sizeof(wszUserName[0]) );		
		 m_bstrReply=SysAllocString(wszUserName);
		 
         pvarResult->vt=VT_BSTR;
		 pvarResult->bstrVal=m_bstrReply;		 
		}
	else m_pcReply=Exec(i);
		
}

static bool _BSTRByVariantArg(VARIANTARG *pvar, BSTR * n)
{
	if (pvar->vt == VT_BSTR)
	{
		*n =pvar->bstrVal;
		return true;
	}
	
	return false;
}


   HRESULT CStageObject::DispatchProc1BSTR(int dispid, WORD wFlags, DISPPARAMS FAR *pdispparams,VARIANT FAR *pvarResult)
{
	if (wFlags & DISPATCH_METHOD)
	{
		if (pdispparams->cArgs == 1)
		{
			BSTR i;
			if (!_BSTRByVariantArg(&pdispparams->rgvarg[0], &i))
				return DISP_E_BADVARTYPE;
			MethodProc1BSTR(dispid, i,pvarResult, wFlags);
			return S_OK;
		}
		else
			return DISP_E_BADPARAMCOUNT;
	}
	return DISP_E_MEMBERNOTFOUND;
}

//End


void CStageObject::InitStage()
{
	
	if (m_StageState == NotInited)
	{
		
		m_StageState = Absent;
		if (::GetValueInt(::GetAppUnknown(), "Stage", "Present", FALSE) == FALSE)
			return;
		//Sergey 27/07/06
		VTMessageBox("LStep1", "VT5", MB_OK);
		LStep1 = new CLStep4X();
		//if (LStep1->m_DLLResultat==-1){
		//	VTMessageBox("Result=-1", "VT5", MB_OK);
		//return;}
		
		/*if (!LStep1.ConnectSimple(4, "", 0, true)){VTMessageBox("LStep1=0", "VT5", MB_OK);
		return;}*/
		
		//if (LStep1->ConnectSimple(4, "", 0, true)==-1){VTMessageBox("LStep1=-1", "VT5", MB_OK);
		//return;}
        //LStep1->SetJoystickOn(true,false);
		// LStep1.SetJoystickOn(true,false);
		/*g_Comm.Open();
		if (!g_Comm.IsOpened())
			return;*/
		//end
		m_dwByteTimeout = ::GetValueInt(::GetAppUnknown(), "Stage", "TimeoutBetweenBytes", 0);
		DWORD nTimeout = ::GetValueInt(::GetAppUnknown(), "Stage", "TimeoutAfterOpenCom", 0);
		if (nTimeout >0)
			Sleep(nTimeout);
		char szReplyBuffer[500];
		char szReplyBegin[] = "PROSCAN INFORMATION";
		int nAttepts = ::GetValueInt(::GetAppUnknown(), "Stage", "InitializationAttempts", 10);
		bool bWasReply = false;
		//koment
		//bWasReply = true;
		//int i=nAttepts;
		/*for (int i = 0; i < nAttepts; i++)
		{
			int n = SendCommand("?\r", szReplyBuffer, 500);
			dprintf("Initializing: try %d. Sent \"?\\r\", received %s", i, szReplyBuffer);
			if (n != 0 && strncmp(szReplyBuffer, szReplyBegin, strlen(szReplyBegin)) == 0)
				break;
			if (n != 0)
				bWasReply = true;
		}
		if (i == nAttepts)
		{
			if (bWasReply){
				VTMessageBox("InitStage1", "VT5", MB_OK);
				VTMessageBox(IDS_NOT_PRIOR_STAGE, app::instance()->handle(), "VT5", MB_OK|MB_ICONEXCLAMATION);
			}
			else
			{VTMessageBox("InitStage2", "VT5", MB_OK);
				VTMessageBox(IDS_NO_RESPONSE, app::instance()->handle(), "VT5", MB_OK|MB_ICONEXCLAMATION);
			}
			return;
		}
		else if (i > 0 && ::GetValueInt(GetAppUnknown(), __DEBUG_SECTION__, "Debug", 0))
		{VTMessageBox("InitStage3", "VT5", MB_OK);
			VTMessageBox("Errors during initialization", "VT5", MB_OK|MB_ICONEXCLAMATION);
		}*/
		
		VTMessageBox("InitStage", "VT5", MB_OK);
		m_StageState = Present;
		/*SendCommand("comp 0\r", szReplyBuffer, 500);
		SendCommand("blzh 0\r", szReplyBuffer, 500);*/

		//end koment
	}
}

unsigned CStageObject::SendCommand(const char *pszCommand, char *pszReplyBuff, unsigned int nBuffSize)
{
	PurgeComm(g_Comm.m_hFile, PURGE_RXCLEAR);
	//sergey 21.04.06
	int i=0;
	while(!g_Comm.Write((unsigned)strlen(pszCommand), pszCommand )&& i<3)
	{
     if (WaitForReadyZ(100) != S_OK)
	        {
              i++;
		      dprintf("Error during write to com: %d\n",i);
		     // VTMessageBox(IDS_TIMEOUT_EXPIRED, app::get_instance()->handle(), "VT5", MB_OK|MB_ICONEXCLAMATION);
		      //return false;
	           }
		
	}
	/*if (!g_Comm.Write((unsigned)strlen(pszCommand), pszCommand))		
		VTMessageBox("Error during write to com", "VT5", MB_OK|MB_ICONEXCLAMATION);*/
	   //end
	for (unsigned n= 0; n < nBuffSize-1; n++)
		if (!g_Comm.Read(1, &pszReplyBuff[n]))
			break;
	pszReplyBuff[n] = 0;
	return n;
}

bool CStageObject::SendCommandChecked(const char *pszCommand, const char *pszReply, unsigned nReplySize)
{
	PurgeComm(g_Comm.m_hFile, PURGE_RXCLEAR);
	char szBuff[256];
	//sergey 21.04.06
	int i=0;
	while(!g_Comm.Write((unsigned)strlen(pszCommand), pszCommand)&& i<3)
	{
     if (WaitForReadyZ(100) != S_OK)
	         {
				 i++;
		        dprintf("Error during write to com: %d\n",i);
		     // VTMessageBox(IDS_TIMEOUT_EXPIRED, app::get_instance()->handle(), "VT5", MB_OK|MB_ICONEXCLAMATION);
		      //return false;
	           }
		
	}
	//if (!g_Comm.Write((unsigned)strlen(pszCommand), pszCommand))
	//	VTMessageBox("Error during write to com", "VT5", MB_OK|MB_ICONEXCLAMATION);
	//end
	if (nReplySize > 255) nReplySize = 255;
	for (unsigned n = 0; n < 255; n++)
	{
		if (!g_Comm.Read(1, &szBuff[n]))
			break;
		if (strncmp(szBuff,pszReply,nReplySize)==0)
			return true;
	}
	return false;
}

bool CStageObject::SendCommandCR(const char *pszCommand, char *pszReplyBuff, unsigned nBuffSize)
{
	PurgeComm(g_Comm.m_hFile, PURGE_RXCLEAR);
	//sergey 21.04.06
	int i=0;
	while(!g_Comm.Write((unsigned)strlen(pszCommand), pszCommand)&& i<3)
	{
     if (WaitForReadyZ(100) != S_OK)
	         {
               i++;
		       dprintf("Error during write to com: %d\n",i);
		     // VTMessageBox(IDS_TIMEOUT_EXPIRED, app::get_instance()->handle(), "VT5", MB_OK|MB_ICONEXCLAMATION);
		      //return false;
	           }
		
	}
	//if (!g_Comm.Write((unsigned)strlen(pszCommand), pszCommand))
	//	VTMessageBox("Error during write to com", "VT5", MB_OK|MB_ICONEXCLAMATION);
	//end
	for (unsigned n= 0; n < nBuffSize-1; n++)
	{
		if (!g_Comm.Read(1, &pszReplyBuff[n]))
			break;
		if (pszReplyBuff[n]=='\r')
		{
			pszReplyBuff[n+1] = 0;
			return true;
		}
	}
	pszReplyBuff[n] = 0;
	return false;
}

HRESULT CStageObject::IsAvailable()
{
	return m_StageState==Present?S_OK:S_FALSE;
}

HRESULT CStageObject::IsReady()
{
	char szReplyBuff[256];
	if (SendCommandCR("$\r", szReplyBuff, 256) == 0)
		return 0;
	int nReply = atoi(szReplyBuff);
	return nReply==0?S_OK:S_FALSE;
}

HRESULT CStageObject::WaitForReady(DWORD nTimeoutMs)
{
	char szReplyBuff[256];
	DWORD dwStart = GetTickCount();
	DWORD dwEnd = dwStart+nTimeoutMs;
	DWORD dwNow = dwStart;
	while (dwNow < dwEnd)
	{
		if (SendCommandCR("$\r", szReplyBuff, 256) != 0)
		{
			dprintf("Reply to $ - %s", szReplyBuff);
			int nReply = atoi(szReplyBuff);
			if (nReply == 0)
				return S_OK;
		}
		Sleep(0);
		dwNow = GetTickCount();
	}
	return S_FALSE;
}

HRESULT CStageObject::GetLimits(int nLimType, BOOL *pExists, int *pnLim1, int *pnLim2)
{
	CStageLimit *pLim = nLimType==0?&m_LimX:nLimType==1?&m_LimY:&m_LimZ;
	*pExists = pLim->m_bEnable;
	*pnLim1 = pLim->m_nMin;
	*pnLim2 = pLim->m_nMax;
	return S_OK;
}

HRESULT CStageObject::GetPosition(POINT *pptPos)
{
	char szReplyBuff[256];
	if (!SendCommandCR("p\r", szReplyBuff, 256))
		return E_FAIL;
	sscanf(szReplyBuff,"%d,%d", &pptPos->x, &pptPos->y);
	return S_OK;
}

HRESULT CStageObject::MoveTo(POINT ptPos)
{
	char szCmd[256];
	sprintf(szCmd, "g %d,%d\r", m_LimX.Range(ptPos.x), m_LimY.Range(ptPos.y));
	if (!SendCommandChecked(szCmd, "R\r", 2))
		return E_FAIL;
	return S_OK;
}

HRESULT CStageObject::Move(POINT ptMove)
{
	char szCmd[256];
	sprintf(szCmd, "gr %d,%d\r", ptMove.x, ptMove.y);
	if (!SendCommandChecked(szCmd, "R\r", 2))
		return E_FAIL;
	return S_OK;
}

HRESULT CStageObject::MoveZ(int dz)
{
	char szCmd[256];
	sprintf(szCmd, "gr 0,0,%d\r", dz);
	if (!SendCommandChecked(szCmd, "R\r", 2))
		return E_FAIL;
	return S_OK;
}


HRESULT CStageObject::GetPositionZ(int *pz)
{
	char szReplyBuff[256];
	if (!SendCommandCR("pz\r", szReplyBuff, 256))
		return E_FAIL;
	*pz = atoi(szReplyBuff);
	return S_OK;
}

HRESULT CStageObject::MoveToZ(int z)
{
	char szCmd[256];
	sprintf(szCmd, "gz %d\r", m_LimZ.Range(z));
	SendCommandChecked(szCmd, "R\r", 2);
	return S_OK;
}

HRESULT CStageObject::GetVelocity(int *v)
{
	char szReplyBuff[256];
	if (!SendCommandCR("sms\r", szReplyBuff, 256))
		return E_FAIL;
	*v = atoi(szReplyBuff);
	return S_OK;
}

HRESULT CStageObject::SetVelocity(int v)
{
	char szCmd[256];
	sprintf(szCmd, "sms %d\r", _Range(v,1,100));
	if (!SendCommandChecked(szCmd, "0\r", 2))
		return E_FAIL;
	return S_OK;
}

HRESULT CStageObject::SavePosition()
{
	GetPosition(&m_ptSaved);
	m_bPosSaved = true;
	return S_OK;
}

HRESULT CStageObject::RestorePosition()
{
	MoveTo(m_ptSaved);
	m_bPosSaved = false;
	return S_OK;
}

HRESULT CStageObject::IsRestorePositionAvail()
{
	return m_bPosSaved?S_OK:S_FALSE;
}

//    Метод Stage.Exec-Sergey 10.11.05

HRESULT CStageObject::MoveToZZ(int z)
{
	char szCmd[256];
	sprintf(szCmd, "gz %d\r", z);
	SendCommandChecked(szCmd, "R\r", 2);
	return S_OK;
}

 HRESULT CStageObject::StopI()
{
	char szCmd[256];
	sprintf(szCmd, "I \r");
	if (!SendCommandChecked(szCmd, "R\r", 2))
		return E_FAIL;
	return S_OK;
}



  HRESULT CStageObject::PozZ()
{
	char szCmd[256];
	sprintf(szCmd, "Z \r");
	if (!SendCommandChecked(szCmd, "0\r", 2))
		return E_FAIL;
	return S_OK;
}
//new
HRESULT CStageObject::MoveU(int dz)
{
	char szCmd[256];
	sprintf(szCmd, "U %d\r", dz);
	if (!SendCommandChecked(szCmd, "R\r", 2))
		return E_FAIL;
	return S_OK;
}

 HRESULT CStageObject::MoveD(int dz)
{
	char szCmd[256];
	sprintf(szCmd, "D %d\r", dz);
	if (!SendCommandChecked(szCmd, "R\r", 2))
		return E_FAIL;
	return S_OK;
}
//end new

char * CStageObject::Exec(BSTR  bstrControl)
{ 
 char szReplyBuff[256]; 
char pszArrayTemp[256];
char pszArray[256];

WideCharToMultiByte( CP_ACP, 0, bstrControl, -1,
        pszArrayTemp, 256, NULL, NULL );

sprintf(pszArray, "%s\r", pszArrayTemp);
 if(!SendCommand(pszArray,szReplyBuff,500))
 {     
	 //return SysAllocString(L"NULL");
	 return "NULL";
 } 
return szReplyBuff; 
}

HRESULT CStageObject::WaitForReadyZ(DWORD nTimeoutMs)
{
	char szReplyBuff[256];
	DWORD dwStart = GetTickCount();
	DWORD dwEnd = dwStart+nTimeoutMs;
	DWORD dwNow = dwStart;
	while (dwNow < dwEnd)
	{
		/*if (SendCommandCR("$\r", szReplyBuff, 256) != 0)
		{
			dprintf("Reply to $ - %s", szReplyBuff);
			int nReply = atoi(szReplyBuff);
			if (nReply == 0)
				return S_OK;
		}*/
		Sleep(0);
		dwNow = GetTickCount();
	}
	//return S_FALSE;
	return S_OK;
}

