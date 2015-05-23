#ifndef __MANUAL_MEAS_ACTIONS_H
#define __MANUAL_MEAS_ACTIONS_H

#include "ActionBase.h"
#include "CompManager.h"


#import <dataview.tlb> exclude ("IUnknown", "GUID", "_GUID", "tagPOINT") no_namespace raw_interfaces_only named_guids 


//[ag]1. classes

class CActionMeasParallelLines : public CMMActionObjectBase
{
	DECLARE_DYNCREATE(CActionMeasParallelLines)
	GUARD_DECLARE_OLECREATE(CActionMeasParallelLines)
	ENABLE_UNDO();

	void SetParamsKeys();
	bool _CheckCoords(CPoint pt, CPoint ptEnd = CPoint(-1,-1));
public:
	CActionMeasParallelLines();
	virtual ~CActionMeasParallelLines();

public:
	virtual bool Initialize();
	virtual void Finalize();
	virtual void Terminate();
	virtual bool IsAvaible();
	virtual bool DoSetCursor(CPoint point);
	
	//paul
	virtual bool IsCompatibleTarget();
	virtual void DoActivateObjects( IUnknown *punkObject );

	
	virtual bool DoLButtonDown(CPoint pt);
	virtual bool DoMouseMove(CPoint pt);
	virtual bool DoRButtonUp(CPoint pt);
	virtual bool DoRButtonDown(CPoint pt) {return CInteractiveActionBase::DoRButtonDown(pt);}
	//virtual bool DoRButtonUp(CPoint pt){return true;};

	virtual void DoDraw(CDC &dc);

	virtual bool Invoke();
	
	virtual bool DoRedo();
	virtual bool DoUndo(); 
protected:
	IUnknown* GetActiveImage();
	bool AddMMObject(long nPos);
	CPoint _GetParallelPoint();
	CPoint _GetParallelPoint(CPoint pt);
	double _GetDistance(long i, long j);
	virtual bool DoCalcParam() {return true;}
private:
	CArray<CRect, CRect&>	m_arrLinesCoords;
	CArray<long, long>		m_arrParamKeys;
	bool					m_bFirstLineComplete;
	COLORREF				m_clDefColor;
	CObjectListWrp			m_ObjListWrp;
	CObjectListUndoRecord	m_changes;	//undo
	long					m_nObjectCounter;
};

class CActionMeasAngle2Line : public CMMActionObjectBase
{
	DECLARE_DYNCREATE(CActionMeasAngle2Line)
	GUARD_DECLARE_OLECREATE(CActionMeasAngle2Line)

public:
	CActionMeasAngle2Line();
	virtual ~CActionMeasAngle2Line();

public:
	virtual bool Initialize();
	virtual bool DoCalcParam();
	virtual bool DoUpdateSettings();

	virtual bool DoLButtonDown(CPoint pt);
	virtual bool DoLButtonUp(CPoint pt);
	virtual bool DoMouseMove(CPoint pt);
	
	virtual bool DoStartTracking(CPoint pt);
	virtual bool DoTrack(CPoint pt);
	virtual bool DoFinishTracking(CPoint pt);
	
	virtual void DoDraw(CDC &dc);

protected:
	virtual bool	SupportType(DWORD dwType); // return true if action can measure params of given type
	virtual DWORD	GetMMObjectType();
	virtual DWORD	GetParamType();
//	double  CalcAngles(double & fA1, double & fA2);

protected:
	CPoint	m_point1;
	CPoint	m_point2;
	CPoint	m_point3;
	CPoint	m_point4;

//	double  m_fAngle;
	int		m_nMode;
	bool	m_bTracking;

};

class CActionMeasAngle3Point : public CMMActionObjectBase
{
	DECLARE_DYNCREATE(CActionMeasAngle3Point)
	GUARD_DECLARE_OLECREATE(CActionMeasAngle3Point)

public:
	CActionMeasAngle3Point();
	virtual ~CActionMeasAngle3Point();

public:
	virtual bool Invoke();
	virtual bool Initialize();
	virtual bool DoCalcParam();
	virtual bool DoUpdateSettings();

	virtual bool DoLButtonDown(CPoint pt);
	virtual bool DoLButtonUp(CPoint pt);
	virtual bool DoMouseMove(CPoint pt);
	virtual bool DoLButtonDblClick(CPoint pt) {return false;};
	
	virtual bool DoStartTracking(CPoint pt);
	virtual bool DoTrack(CPoint pt);
	virtual bool DoFinishTracking(CPoint pt);
	
	virtual void DoDraw(CDC &dc);

protected:
	virtual bool	SupportType(DWORD dwType); // return true if action can measure params of given type
	virtual DWORD	GetMMObjectType();
	virtual DWORD	GetParamType();
	void	DrawAngle(CDC * pDC);
	double  CalcAngles(double & fA1, double & fA2);
	void	CheckDirection();

protected:
	CPoint	m_point1;
	CPoint	m_point2;
	CPoint	m_point3;

	double  m_fAngle;
	int		m_nMode;
	bool	m_bTracking;
//	bool	m_bBig;
//	bool	m_bPISign;
	bool	m_bRight;
	bool	m_bCurSideRight;
	bool	m_bFirst;
};

class CActionMeasCycle3 : public CMMActionObjectBase
{
	DECLARE_DYNCREATE(CActionMeasCycle3)
	GUARD_DECLARE_OLECREATE(CActionMeasCycle3)

public:
	CActionMeasCycle3();
	virtual ~CActionMeasCycle3();

public:
	virtual bool DoCalcParam();
	virtual bool Initialize();
	virtual bool DoUpdateSettings();

	virtual bool DoLButtonDown(CPoint pt);
	virtual bool DoLButtonUp(CPoint pt);
	virtual bool DoMouseMove(CPoint pt);
	
	virtual bool DoStartTracking(CPoint pt);
	virtual bool DoTrack(CPoint pt);
	virtual bool DoFinishTracking(CPoint pt);
	
	virtual void DoDraw(CDC &dc);

protected:
	virtual bool	SupportType(DWORD dwType); // return true if action can measure params of given type
	virtual DWORD	GetMMObjectType();
	virtual DWORD	GetParamType();
	bool			CalcCycle(CRect & rc);

protected:
	CPoint	m_point1;
	CPoint	m_point2;
	CPoint	m_point3;

	CPoint	m_pointCenter;
	double  m_fRadius;
	int		m_nMode;
	bool	m_bTracking;
};

class CActionMeasCycle2 : public CMMActionObjectBase
{
	DECLARE_DYNCREATE(CActionMeasCycle2)
	GUARD_DECLARE_OLECREATE(CActionMeasCycle2)

public:
	CActionMeasCycle2();
	virtual ~CActionMeasCycle2();

public:
	virtual bool DoCalcParam();
	virtual bool Initialize();
	virtual bool DoUpdateSettings();

	virtual bool DoStartTracking(CPoint pt);
	virtual bool DoTrack(CPoint pt);
	virtual bool DoFinishTracking(CPoint pt);

	virtual void DoDraw(CDC &dc);

protected:
	virtual bool	SupportType(DWORD dwType); // return true if action can measure params of given type
	virtual DWORD	GetMMObjectType();
	virtual DWORD	GetParamType();


protected:
	CPoint	m_point1, m_point2;
	CPoint	m_pointCenter;
	long	m_nRadius;
};

class CActionMeasBrokenLine : public CMMActionObjectBase
{
	DECLARE_DYNCREATE(CActionMeasBrokenLine)
	GUARD_DECLARE_OLECREATE(CActionMeasBrokenLine)

public:
	CActionMeasBrokenLine();
	virtual ~CActionMeasBrokenLine();

public:
	virtual bool Initialize();
	virtual bool DoCalcParam();
	virtual bool DoUpdateSettings();

	virtual bool DoLButtonDown(CPoint pt);
	virtual bool DoLButtonUp(CPoint pt);
	virtual bool DoLButtonDblClick(CPoint pt);
	virtual bool DoMouseMove(CPoint pt);

	virtual bool DoStartTracking(CPoint pt);
	virtual bool DoTrack(CPoint pt);
	virtual bool DoFinishTracking(CPoint pt);

	virtual void DoDraw(CDC &dc);

protected:
	virtual bool	SupportType(DWORD dwType); // return true if action can measure params of given type
	virtual DWORD	GetMMObjectType();
	virtual DWORD	GetParamType();

	Contour	*	m_pContour;
	bool		m_bTracking;
};

class CActionMeasLine : public CMMActionObjectBase
{
	DECLARE_DYNCREATE(CActionMeasLine)
	GUARD_DECLARE_OLECREATE(CActionMeasLine)

public:
	CActionMeasLine();
	virtual ~CActionMeasLine();

public:
	virtual bool Initialize();
	virtual bool DoCalcParam();
	virtual bool DoUpdateSettings();

	virtual bool DoLButtonDown(CPoint pt);
	virtual bool DoLButtonUp(CPoint pt);
	virtual bool DoMouseMove(CPoint pt);
	
	virtual bool DoStartTracking(CPoint pt);
	virtual bool DoTrack(CPoint pt);
	virtual bool DoFinishTracking(CPoint pt);
	virtual bool DoRButtonDown(CPoint pt){return true;}
	
	virtual void DoDraw(CDC &dc);

protected:
	virtual bool	SupportType(DWORD dwType); // return true if action can measure params of given type
	virtual DWORD	GetMMObjectType();
	virtual DWORD	GetParamType();

protected:
	CPoint	m_point1;
	CPoint	m_point2;
	bool	m_bTracking;
	int		m_nMode;
};

class CActionMeasCount : public CMMActionObjectBase
{
	DECLARE_DYNCREATE(CActionMeasCount)
	GUARD_DECLARE_OLECREATE(CActionMeasCount)

public:
	CActionMeasCount();
	virtual ~CActionMeasCount();

public:
	virtual bool Initialize();
	virtual void Finalize();
	virtual bool DoCalcParam();
	virtual bool DoUpdateSettings();
	virtual bool Invoke();
	
	virtual bool DoLButtonDown(CPoint pt);
	virtual bool DoLButtonUp(CPoint pt);
	virtual bool DoRButtonUp(CPoint pt);
	virtual bool DoRButtonDown(CPoint pt){return true;}

	virtual void DoDraw(CDC &dc);
	

protected:
	virtual bool	SupportType(DWORD dwType); // return true if action can measure params of given type
	virtual DWORD	GetMMObjectType();
	virtual DWORD	GetParamType();
//	virtual	bool	NeedShiftParam()	
//	{	return false;	}

	bool		CheckPrevObject();
//	bool		UpdateObjectData();

	Contour	*	m_pContour;
	bool		m_bFinalizeOnClick;
};

class CActionMeasFreeLine : public CMMActionObjectBase
{
	DECLARE_DYNCREATE(CActionMeasFreeLine)
	GUARD_DECLARE_OLECREATE(CActionMeasFreeLine)

public:
	CActionMeasFreeLine();
	virtual ~CActionMeasFreeLine();

public:
	virtual bool Initialize();
	virtual bool DoCalcParam();
	virtual bool DoUpdateSettings();

	virtual bool DoLButtonDown(CPoint pt);
	virtual bool DoLButtonUp(CPoint pt);
	virtual bool DoLButtonDblClick(CPoint pt);
	virtual bool DoMouseMove(CPoint pt);
	virtual bool DoStartTracking(CPoint pt);
	virtual bool DoTrack(CPoint pt);
	virtual bool DoFinishTracking(CPoint pt);
	virtual void DoDraw(CDC &dc);

protected:
	virtual bool	SupportType(DWORD dwType); // return true if action can measure params of given type
	virtual DWORD	GetMMObjectType();
	virtual DWORD	GetParamType();

protected:
	Contour	*	m_pContour;
	bool		m_bTracking;
	EMouseMode	m_nMouseMode;
};

class CActionMeasSpline : public CMMActionObjectBase
{
	DECLARE_DYNCREATE(CActionMeasSpline)
	GUARD_DECLARE_OLECREATE(CActionMeasSpline)

public:
	CActionMeasSpline();
	virtual ~CActionMeasSpline();

public:
	virtual bool Initialize();
	virtual void Finalize();

	virtual bool DoCalcParam();
	virtual bool DoLButtonDown(CPoint pt);
	virtual bool DoLButtonDblClick(CPoint pt);
	virtual bool DoMouseMove(CPoint pt);
	virtual bool DoSetCursor(CPoint point);

	virtual void DoDraw(CDC &dc);

protected:
	virtual bool	SupportType(DWORD dwType); // return true if action can measure params of given type
	virtual DWORD	GetMMObjectType();
	virtual DWORD	GetParamType();

	bool	IsCycled();

protected:
	HCURSOR	m_hCursourCycled;

	Contour	*	m_pContour;
};

/*
public:
	virtual bool DoLButtonDown( CPoint pt ){return false;};
	virtual bool DoLButtonUp( CPoint pt ){return false;};
	virtual bool DoLButtonDblClick( CPoint pt ){return false;};
	virtual bool DoRButtonDown( CPoint pt ){return false;};
	virtual bool DoRButtonUp( CPoint pt ){return false;};
	virtual bool DoRButtonDblClick( CPoint pt ){return false;};
	virtual bool DoMouseMove( CPoint pt ){return false;};
	virtual bool DoStartTracking( CPoint pt ){return false;};
	virtual bool DoTrack( CPoint pt ){return false;};
	virtual bool DoFinishTracking( CPoint pt ){return false;};
	virtual bool DoSetCursor( CPoint point ){return false;};
	virtual bool DoChar( UINT nChar ){return false;};
	virtual bool DoVirtKey( UINT nVirtKey ){return false;};

	virtual bool CanStartTracking( CPoint pt ){return true;};
protected:	//advanced overrideable
	virtual bool DoEnterMode(CPoint point);
	virtual bool DoLeaveMode();

*/


#endif //__MANUAL_MEAS_ACTIONS_H
