#pragma once
#include "com_unknown.h"
#include "impl_misc.h"
#include "StageInt.h"
#include "LStep4X.h"

#define DISPID_X           0
#define DISPID_Y           1
#define DISPID_MOVETO      2
#define DISPID_MOVE        3
#define DISPID_Z           4
#define DISPID_MOVETOZ     5
#define DISPID_MOVEZ       6
#define DISPID_READY       7
#define DISPID_V           8
#define DISPID_AVAIL       9
//Sergey 21.11.2005
#define DISPID_EXEC       10
//end
struct CStageLimit
{
	int m_nMin;
	int m_nMax;
	BOOL m_bEnable;
	CStageLimit();
	void Read(const char *pszSecName);
	int Range(int nValue);
};

class CStageObject : public ComObjectBase, public CNamedObjectImpl, public IDispatch,
	public IStage, public _dyncreate_t<CStageObject>
{
	route_unknown();
public:
	CStageObject(void);
	~CStageObject(void);
	virtual IUnknown *DoGetInterface(const IID &iid);
	//void InitStage();

protected:
	enum StageState
	{
		NotInited = -1,
		Absent = 0,
		Present = 1
	};
	StageState m_StageState;
	//Sergey 27/07/06
	CLStep4X* LStep1;
	//CLStep4X LStep1;
	
	
	//end
	void InitStage();
	unsigned SendCommand(const char *pszCommand, char *pszReplyBuff, unsigned nBuffSize);
	bool SendCommandChecked(const char *pszCommand, const char *pszReply, unsigned nReplySize);
	bool SendCommandCR(const char *pszCommand, char *pszReplyBuff, unsigned nBuffSize);
	CStageLimit m_LimX;
	CStageLimit m_LimY;
	CStageLimit m_LimZ;
	POINT m_ptSaved;
	bool m_bPosSaved;
	DWORD m_dwByteTimeout;
	

protected:
	int GetPropertyInt(int dispid);
	void SetPropertyInt(int dispid, int nValue);
	void MethodProc2Int(int dispid, int i1, int i2);
	void MethodProc1Int(int dispid, int i);
	//sergey 22.11.2005
	void MethodProc1BSTR(int dispid, BSTR i,VARIANT FAR *pvarResult,WORD wFlags);
	//end

protected:
	DISPID DispidOfMember(OLECHAR FAR* pName);
	HRESULT DispatchPropertyInt(int dispid, WORD wFlags, DISPPARAMS FAR *pdispparams, VARIANT FAR *pvarResult);
	HRESULT DispatchProc2Int(int dispid, WORD wFlags, DISPPARAMS FAR *pdispparams);
	HRESULT DispatchProc1Int(int dispid, WORD wFlags, DISPPARAMS FAR *pdispparams);
	//sergey 21.11.2005
	HRESULT DispatchProc1BSTR(int dispid, WORD wFlags, DISPPARAMS FAR *pdispparams,VARIANT FAR *pvarResult);
	//end

public:
	// IDispatch
	com_call GetIDsOfNames(REFIID riid, OLECHAR FAR* FAR* rgszNames, unsigned int cNames,
		LCID lcid, DISPID FAR* rgdispid);
	com_call GetTypeInfo(unsigned int itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo);
	com_call GetTypeInfoCount(unsigned int FAR* pctinfo);
	com_call Invoke(DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR *pdispparams,
		VARIANT FAR *pvarResult, EXCEPINFO FAR* pexecinfo, unsigned int FAR *puArgErr);

public:
	com_call IsAvailable(); // S_OK if available, S_FALSE if not
	com_call IsReady(); // S_OK if available, S_FALSE if not
	com_call WaitForReady(DWORD nTimeoutMs);
	com_call GetLimits(int nLimType, BOOL *pExists, int *pnLim1, int *pnLim2);
	com_call GetPosition(POINT *pptPos);
	com_call MoveTo(POINT ptPos);
	com_call Move(POINT ptMove);
	com_call GetPositionZ(int *pz);
	com_call MoveToZ(int z);
   //Sergey 10.11.05
	com_call MoveToZZ(int z);
	com_call PozZ();
	com_call StopI();
	//new 
	com_call MoveU(int z);
	com_call MoveD(int z);
   //com_call_char  Exec(char *pszControl);//,char *pzsReply); 
	com_call_char  Exec(BSTR bstrControl);
	com_call WaitForReadyZ(DWORD nTimeoutMs);
	//END
	com_call MoveZ(int dz);
	com_call GetVelocity(int *v);
	com_call SetVelocity(int v);
	com_call SavePosition();
	com_call RestorePosition();
	com_call IsRestorePositionAvail();
};
