#pragma once

#include "action_main.h"
#include "action_interactive.h"
#include "impl_long.h"
#include "StageInt.h"
#include "image5.h"
#include "LStep4X.h"

class CStageAction
{
friend void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
protected:
	static IStagePtr m_Stage;
	void Init();
	bool IsStageOk() {return m_Stage!=0&&m_Stage->IsAvailable()==S_OK;}
};

class CMoveStage : public CInteractiveAction, public CStageAction, public _dyncreate_t<CMoveStage>
{
friend void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
	_point m_ptPosStage0;
	bool m_bTrackOk;
	double m_dCalibr;
	static HICON m_hIcon;
	void LoadIcon();
	static POINT m_ptStageMove1s;
	int m_nZAxisAccel;

//protected:
//	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
public:
	
	CMoveStage();
	com_call DoInvoke();
	com_call GetActionState(DWORD *pdwState);
	com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
	virtual LRESULT	DoMouseMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);
	com_call OnMessage( MSG *pmsg, LRESULT *plReturn );

protected:
	virtual bool DoStartTracking(_point point);
	virtual bool DoTrack(_point point);
	virtual bool DoFinishTracking(_point point);
//	virtual bool DoSetCursor();
};

class CMoveStageInfo : public _ainfo_t<ID_ACTION_MOVE_STAGE, _dyncreate_t<CMoveStage>::CreateObject, 0>,
	public _dyncreate_t<CMoveStageInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};

interface IDriver;
interface IDriver5;
interface IImage3;
class CAutofocus : public CAction, public CLongOperationImpl, public CStageAction,
	public _dyncreate_t<CAutofocus>
{
	bool Step(IDriver *pDrv, IDriver5 *pDrv5, int nDev, int dz, IImage4 *pimg, int *pnFocus, int nFrames,int ZTime);
	bool Search(IDriver *pDrv, IDriver5 *pDrv5, int nDevice, int dz, IImage4 *pimg, int nFrames, int nSteps,
		int &nFocus, int nNotifyPos,int ZTime);
	//Sergey 10.11.05
	bool SearchZ(IDriver *pDrv, IDriver5 *pDrv5, int nDevice, int dz, IImage4 *pimg, int nFrames, 
		int &nFocus, int nNotifyPos, int nZMax, int nZMin,int ZTime);
	bool StepZ(IDriver *pDrv, IDriver5 *pDrv5, int nDev, int dz, IImage4 *pimg, int *pnFocus, int nFrames,int ZTime);
	/////////NEW
	bool ALGORITM(IDriver *pDrv, IDriver5 *pDrv5, int nDev, int nStep, IImage4 *pimg, int nFrames,
	 int &nFocus, int nNotifyPos,bool bUpDown);
	struct FocusPoz
	{	
		int iFocus;
		int iPoz;
	};
    FocusPoz* ArFoPoz;
	//end
	int m_nPane;
	FILE *fp;
public:
	route_unknown();
protected:
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	CAutofocus();
	com_call DoInvoke();
	com_call GetActionState(DWORD *pdwState);
};

class CAutofocusInfo : public _ainfo_t<ID_ACTION_AUTOFOCUS, _dyncreate_t<CAutofocus>::CreateObject, 0>,
	public _dyncreate_t<CAutofocusInfo>
{
	route_unknown();
public:
	CAutofocusInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};

class CFocus : public CAction, public CLongOperationImpl, public CStageAction,
	public _dyncreate_t<CFocus>
{
public:
	route_unknown();
public:
	CFocus();
	com_call DoInvoke();
	com_call GetActionState(DWORD *pdwState);
};

class CFocusInfo : public _ainfo_t<ID_ACTION_FOCUS, _dyncreate_t<CFocus>::CreateObject, 0>,
	public _dyncreate_t<CFocusInfo>
{
	route_unknown();
public:
	CFocusInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};

class CUndoMove: public CAction, public CStageAction,	public _dyncreate_t<CUndoMove>
{
public:
	route_unknown();
public:

	CUndoMove();
	com_call DoInvoke();
	com_call GetActionState(DWORD *pdwState);
};

class CUndoMoveInfo : public _ainfo_t<ID_ACTION_UNDO_MOVE, _dyncreate_t<CUndoMove>::CreateObject, 0>,
	public _dyncreate_t<CUndoMoveInfo>
{
	route_unknown();
public:
	CUndoMoveInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};

