#ifndef __MANUAL_MEAS_ACTIONBASE_H__
#define __MANUAL_MEAS_ACTIONBASE_H__


#include "wndbase.h"

enum EMouseMode
{
	fmUndef = -1,
	fmClick = 0,
	fmDblClick = 1,
};



class CUndoInfo
{
public:
	//undo/redo
	bool					m_bWasCreatedObjectList;
	bool					m_bWasCreatedMeasObject;

	IManualMeasureObjectPtr m_ptrPrevManualMeasObject;
	
	GUID					m_guidPrevActiveObject;
	long					m_lPrevActiveParam;
	//param info
	bool					m_bWasCreateNewParam;
	long					m_lParamKey;
	CString					m_strPrevActionName;

	
	GUID					m_guidNewActiveObject;
	long					m_lNewActiveParam;

	CUndoInfo()
	{
		m_bWasCreatedObjectList	= false;
		m_bWasCreatedMeasObject	= false;		
		
		m_guidPrevActiveObject	= INVALID_KEY;
		m_lPrevActiveParam		= -1;
		//param info
		m_bWasCreateNewParam	= false;
		m_lParamKey				= -1;

		m_guidNewActiveObject	= INVALID_KEY;
		m_lNewActiveParam		= -1;		
	}

};

class CMeasurePane;

//base class for object actions
class CMMActionObjectBase : public CInteractiveActionBase
{
	ENABLE_UNDO();
public:
	CMMActionObjectBase();
	~CMMActionObjectBase();
protected:

public:
//overrided virtuals
	virtual bool Initialize();
	virtual void Finalize();
	virtual bool Invoke();
	virtual bool DoRedo();
	virtual bool DoUndo(); 
	virtual bool DoUpdateSettings();
	virtual bool DoLButtonDown(CPoint pt);
	virtual bool DoRButtonDown(CPoint pt);
	virtual void DoActivateObjects( IUnknown *punkObject );
	virtual bool DoTerminate();

	//paul
	virtual bool IsCompatibleTarget();
	virtual bool IsAvaible();

protected:
//"abstract" part
	virtual bool DoCalcParam() = 0;

// must be overrided in derived classes
	virtual bool	SupportType(DWORD dwType); // return true if action can measure params of given type
	virtual DWORD	GetMMObjectType();
	virtual DWORD	GetParamType();

//helpers
	virtual bool CreateMMObject();
	virtual void DeleteMMObject(IUnknown *punkObject);
	
	virtual bool CalcValue();
	virtual CRect GetInvalidateRect();

	void DrawCross(CDC *pDC, const CPoint & pt);

	IUnknownPtr	_GetActiveObjectList( );	//return the active object list
	IUnknownPtr	_GetActiveImage( );	//return the active image
	IUnknownPtr	_GetActiveDocument();	//return the active document
	IUnknownPtr	_GetActiveView();	//return the active document
	IUnknownPtr	_GetActiveMMObject();	//return the active child	
protected:
	CObjectListWrp			m_ObjListWrp;
	IMeasureObjectPtr		m_ptrMeasObject;
	IManualMeasureObjectPtr m_ptrManualMeasObject;
	
	CObjectListUndoRecord	m_changes;	//undo

	bool					m_bNewParam; 
	double					m_fZoom;
	double					m_fCurrentValue;
	double					m_fUnitCoeff;
	CString					m_strUnitName;


	COLORREF				m_clDefColor;
	int						m_nCrossOffset;
	int						m_nArcSize;

	CString					m_strPaneText;

	bool					CreateObjectList();
	bool					CreateMeasObject();
	bool					ProcessMeasManualObject();
	bool					CalcObject();
	bool					ShiftParamater();
	bool					PutToData();
	bool					InvalidateView( );	
	bool					UpdatePropPage( bool bonly_calibr_name = false );	

	CUndoInfo				m_undoInfo;

};


#define szCurrentValue	"MeasValue"
#define szCurrentParam	"MeasParam"
#define szCurrentUnit	"MeasUnit"
#define szCurrentParamFmt	"MeasParamFmt"

#endif // __MANUAL_MEAS_ACTIONBASE_H__