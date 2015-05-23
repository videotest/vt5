#ifndef __objactions_h__
#define __objactions_h__

#include "actionobjectbase.h"
#include "CompManager.h"
#include "ObListWrp.h"
#include "resource.h"
#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_map.h"

inline long cmp_spunk( IUnknownPtr spunk1, IUnknownPtr spunk2 )
{	
	if( spunk1 < spunk2 )
		return -1;
	else if( spunk1 > spunk2 )
		return 1;
	else 
		return 0;
};

class CRectCtrl;
class CFixedRectCtrl;
class CFixedEllipseCtrl;
class CEditorCtrl;

struct ParamInfo
{
	ParamInfo(long nID, double fL, double fH) : nParamID(nID), fLo(fL), fHi(fH){};
	long nParamID;
	double fLo;
	double fHi;
};


struct ClassInfo
{
	ClassInfo()
	{
		guidClassKey = INVALID_KEY;
	};
	~ClassInfo()
	{
		POSITION pos = listParams.GetHeadPosition();
		ParamInfo* pParam = listParams.GetNext(pos);
		if(pParam) delete pParam;
	};
	GUID	guidClassKey;
	CTypedPtrList<CPtrList, ParamInfo*> listParams;
};

void CopyParams(ICalcObjectContainerPtr in, ICalcObjectContainerPtr out);


//[ag]1. classes

class CActionObjectClassifyByLimits : public CFilterBase
{
	DECLARE_DYNCREATE(CActionObjectClassifyByLimits)
	GUARD_DECLARE_OLECREATE(CActionObjectClassifyByLimits)
public:
	CActionObjectClassifyByLimits();
	virtual ~CActionObjectClassifyByLimits();
public:
	virtual bool InvokeFilter();
};


class CActionObjectFilter : public CFilterBase
{
	DECLARE_DYNCREATE(CActionObjectFilter)
	GUARD_DECLARE_OLECREATE(CActionObjectFilter)
public:
	CActionObjectFilter();
	virtual ~CActionObjectFilter();
public:
	virtual bool InvokeFilter();
	virtual bool Invoke();
};


class CActionAutoAssignClasses : public CFilterBase
{
	DECLARE_DYNCREATE(CActionAutoAssignClasses)
	GUARD_DECLARE_OLECREATE(CActionAutoAssignClasses)
public:
	CActionAutoAssignClasses();
	virtual ~CActionAutoAssignClasses();
public:
	virtual bool InvokeFilter();
	virtual bool DoUndo()
	{
		m_changes.DoUndo(m_punkTarget);
		return CFilterBase::DoUndo();
	};
	virtual bool DoRedo()
	{
		m_changes.DoRedo(m_punkTarget);
		return CFilterBase::DoRedo();
	};
protected:
	CObjectListUndoRecord	m_changes;	//undo
};

class CActionObjectsSetClass : public CFilterBase
{
	DECLARE_DYNCREATE(CActionObjectsSetClass)
	GUARD_DECLARE_OLECREATE(CActionObjectsSetClass)
public:
	CActionObjectsSetClass();
	virtual ~CActionObjectsSetClass();
public:
	virtual bool InvokeFilter();
};

class CActionObjectsMerge : public CFilterBase
{
	DECLARE_DYNCREATE(CActionObjectsMerge)
	GUARD_DECLARE_OLECREATE(CActionObjectsMerge)


	BOOL TestNewObjects(IUnknown* punkLists[], int nObjLists, int nSkipOL, IUnknown* punkObj, int nVerType);
	BOOL TestExist(IUnknown* punkObj, CObjectListWrp &ObjLstCur, CObjectListWrp &ObjLstTest, int nVerType);
	void SaveObjectInfo(IUnknown* punkObj, CObjectListWrp &ObjLstCur, IUnknown* punkObjSrc);
public:
	CActionObjectsMerge();
	virtual ~CActionObjectsMerge();
public:
	virtual bool InvokeFilter();
};

class CActionObjectsRemove : public CFilterBase
{
	DECLARE_DYNCREATE(CActionObjectsRemove)
	GUARD_DECLARE_OLECREATE(CActionObjectsRemove)
public:
	CActionObjectsRemove();
	virtual ~CActionObjectsRemove();
public:
	virtual bool InvokeFilter();
};

class CActionObjectsSplit : public CFilterBase
{
	DECLARE_DYNCREATE(CActionObjectsSplit)
	GUARD_DECLARE_OLECREATE(CActionObjectsSplit)
public:
	CActionObjectsSplit();
	virtual ~CActionObjectsSplit();
public:
	virtual bool InvokeFilter();
	virtual bool DoUndo()
	{
		m_changes.DoUndo(m_punkTarget);
		return CFilterBase::DoUndo();
	};
	virtual bool DoRedo()
	{
		m_changes.DoRedo(m_punkTarget);
		return CFilterBase::DoRedo();
	};
protected:
	CObjectListUndoRecord	m_changes;	//undo
};

class CActionObjectsMakeLight : public CFilterBase
{
	DECLARE_DYNCREATE(CActionObjectsMakeLight)
	GUARD_DECLARE_OLECREATE(CActionObjectsMakeLight)
public:
	CActionObjectsMakeLight();
	virtual ~CActionObjectsMakeLight();
public:
	virtual bool InvokeFilter();
};

class CActionObjectMultiLine : public CActionObjectBase
{
	DECLARE_DYNCREATE(CActionObjectMultiLine)
	GUARD_DECLARE_OLECREATE(CActionObjectMultiLine)

ENABLE_UNDO();
public:
	CActionObjectMultiLine();
	virtual ~CActionObjectMultiLine();

public:
	virtual void DoDraw( CDC &dc );
	virtual bool Initialize();
	virtual bool DoChangeObjects();

	virtual bool DoLButtonDown( CPoint point );
	virtual bool DoStartTracking( CPoint point );
	virtual bool DoFinishTracking( CPoint point );
	virtual bool DoTrack( CPoint point );
	virtual bool DoLButtonDblClick( CPoint pt );
	virtual bool DoMouseMove( CPoint pt );
protected:
	void SetLastPoint( CPoint point );
	void Invalidate();

	bool	m_bFinalizeAfterFirstClick;
	CWalkFillInfo	*m_pwfi;
	CArray<CPoint, CPoint&>	m_points;
	int			m_nLineWidth;
	UINT		m_nIDCursor;
};

class CActionObjectLine : public CActionObjectMultiLine
{
	DECLARE_DYNCREATE(CActionObjectLine)
	GUARD_DECLARE_OLECREATE(CActionObjectLine)
public:
	CActionObjectLine()
	{
		m_bFinalizeAfterFirstClick = true;
		m_nIDCursor = IDC_LINE;
	}
public:
};

class CActionRecalcOnChangeCalibr : public CActionBase
{
	DECLARE_DYNCREATE(CActionRecalcOnChangeCalibr)
	GUARD_DECLARE_OLECREATE(CActionRecalcOnChangeCalibr)
public:
	CActionRecalcOnChangeCalibr();
	virtual ~CActionRecalcOnChangeCalibr();

public:
	virtual bool Invoke();
	void StoreCalibration( IUnknown *punk, GUID &guid, double fCalibr, bool bReplace );
};

class CActionObjectEditor : public CActionObjectBase
{
	DECLARE_DYNCREATE(CActionObjectEditor)
	GUARD_DECLARE_OLECREATE(CActionObjectEditor)

public:
	CActionObjectEditor();
	virtual ~CActionObjectEditor();

public:
	virtual bool Initialize();
	virtual void Finalize();

	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoRButtonDown( CPoint pt );
	virtual bool DoRButtonUp( CPoint pt ){return m_bUseRButton;};

	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );

	virtual bool DoChangeObjects();
	virtual void DoDraw( CDC &dc );
	virtual bool DoUpdateSettings();

public:
	void MergeObjects( CWalkFillInfo *pwfi );
	void SeparateObjects( CWalkFillInfo *pwfi );
	void MeasureFillInfo( CWalkFillInfo *pwfi, CRect rectBounds = NORECT );	

	void _InitObject( CWalkFillInfo *pwfi, CRect rectObject );
protected:
	void	_InvalidateTargetRect( CRect rect = NORECT );
	void	_AddLineSegment( CPoint pt );
	void	_InitClippingRgn();
protected:
	CImageWrp		m_imagePaint;
	CWalkFillInfo	*m_pwfi;
	CPen			m_penDivider;
	CPoint			m_pointLast;
	long			m_nLineWidth;
	CEditorCtrl		*m_pctrl;
	bool			m_bSeparate;
	bool			m_bUseRButton;
	CRgn*			m_pRgn;
	COLORREF		m_colDrawing;

private:
	void	clear_clipping_region();
	/*bool	is_phased(IUnknown* unk);
	void	update_phases_info(IUnknown* pList);*/

	
};

class CActionObjectEditorSpec : public CActionObjectEditor
{
	DECLARE_DYNCREATE(CActionObjectEditorSpec)
	GUARD_DECLARE_OLECREATE(CActionObjectEditorSpec)

public:
	CActionObjectEditorSpec();

public:
	virtual bool Initialize();
	virtual bool DoRButtonDown( CPoint pt ) {return false;}
	virtual bool DoLButtonDblClick( CPoint pt ) {return false;};
};

class CActionObjectRectDirect : public CActionObjectBase
{
	DECLARE_DYNCREATE(CActionObjectRectDirect)
	GUARD_DECLARE_OLECREATE(CActionObjectRectDirect)

public:
	CActionObjectRectDirect();
	virtual ~CActionObjectRectDirect();

public:
	virtual bool Initialize();
	virtual void StoreCurrentExecuteParams();

	virtual bool DoChangeObjects();

	virtual void DoDraw( CDC &dc );

	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoMouseMove( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
protected:
	CPoint	m_point1;
	CPoint	m_point2;
	int		m_nWidth;

	enum Stages
	{
		defineNothing,
		defineLength,
		defineWidth,
	};

	Stages	m_stagedef;

};

class CActionObjectRectFixed : public CActionObjectBase
{
	DECLARE_DYNCREATE(CActionObjectRectFixed)
	GUARD_DECLARE_OLECREATE(CActionObjectRectFixed)

public:
	CActionObjectRectFixed();
	virtual ~CActionObjectRectFixed();

public:
	virtual bool Initialize();
	virtual void Finalize();
	virtual void StoreCurrentExecuteParams();
	virtual bool DoChangeObjects();
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoUpdateSettings();
	void	UpdateCalibration();
	CRect	CalcPixelRect();
	CSize	CalcPixelSize(){return CalcPixelRect().Size();}
protected:
	double	m_left, m_top, m_width, m_height, m_calibr;
	bool	m_bCalibr;

	CFixedRectCtrl	*m_pctrl;
};


class CActionObjectEllipseDirect : public CActionObjectRectDirect
{
	DECLARE_DYNCREATE(CActionObjectEllipseDirect)
	GUARD_DECLARE_OLECREATE(CActionObjectEllipseDirect)

public:
	CActionObjectEllipseDirect();
	virtual ~CActionObjectEllipseDirect();

public:
	virtual bool Initialize();

	virtual bool DoChangeObjects();

	virtual void DoDraw( CDC &dc );
	virtual bool DoLButtonUp( CPoint pt );

	void CalcEllipse( CPoint *ppt );
};

class CActionObjectEllipseFixed : public CActionObjectRectFixed
{
	DECLARE_DYNCREATE(CActionObjectEllipseFixed)
	GUARD_DECLARE_OLECREATE(CActionObjectEllipseFixed)

public:
	CActionObjectEllipseFixed();
	virtual ~CActionObjectEllipseFixed();

public:
	virtual void StoreCurrentExecuteParams();
	virtual bool Initialize();
	virtual bool DoChangeObjects();
	virtual bool DoUpdateSettings();
};

class CActionObjectFreeLine : public CActionObjectBase
{
	DECLARE_DYNCREATE(CActionObjectFreeLine)
	GUARD_DECLARE_OLECREATE(CActionObjectFreeLine)

public:
	CActionObjectFreeLine();
	virtual ~CActionObjectFreeLine();
public:
	virtual bool Initialize();
	virtual bool DoChangeObjects();
	virtual bool DoUpdateSettings();

	virtual bool DoRButtonDown( CPoint pt );
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	virtual bool DoLButtonDblClick( CPoint pt );
	virtual bool DoMouseMove( CPoint pt );
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual void DoDraw( CDC &dc );
protected:
	Contour	*m_pcontour;
	bool	m_bTracking;
	FreeMode	m_nFreeMode;
};

class CActionObjectSpline : public CActionObjectBase
{
	DECLARE_DYNCREATE(CActionObjectSpline)
	GUARD_DECLARE_OLECREATE(CActionObjectSpline)
	BOOL m_bFinished;

public:
	CActionObjectSpline();
	virtual ~CActionObjectSpline();

public:
	virtual bool Initialize();
	virtual void Finalize();

	virtual bool DoChangeObjects();
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoLButtonDblClick( CPoint pt );
	virtual bool DoMouseMove( CPoint pt );
	virtual bool DoSetCursor( CPoint point );

	virtual void DoDraw( CDC &dc );
protected:
	void RedrawBoxes( bool bOnlyLast = false );
	bool IsCycled();

	HCURSOR	m_hCursourCycled;

	Contour	*m_pcontour;
	bool	m_bDrawBoxes;
};

class CActionObjectEllipse : public CActionObjectBase
{
	DECLARE_DYNCREATE(CActionObjectEllipse)
	GUARD_DECLARE_OLECREATE(CActionObjectEllipse)

public:
	CActionObjectEllipse();
	virtual ~CActionObjectEllipse();

public:
	virtual bool Initialize();
	virtual bool DoChangeObjects();
	virtual void StoreCurrentExecuteParams();
protected:
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual void DoDraw( CDC &dc );

	void DrawPoint( CDC &dc, CPoint point );
protected:
	CPoint	m_point1, m_point2;
	CPoint	m_pointCenter;
	long	m_nRadius;
};

class CActionObjectRect : public CActionObjectBase
{
	DECLARE_DYNCREATE(CActionObjectRect)
	GUARD_DECLARE_OLECREATE(CActionObjectRect)
public:
	CActionObjectRect();
	virtual ~CActionObjectRect();
public:
	virtual bool Initialize();
	virtual void Finalize();
	virtual bool DoChangeObjects();
	virtual void StoreCurrentExecuteParams();
protected:
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual void DoDraw( CDC &dc );
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	void CheckController();
protected:
	CRect	m_rectNewObject;

	CRectCtrl	*m_pctrl;

};

class CActionObjectMagickStick : public CActionObjectBase
{
	DECLARE_DYNCREATE(CActionObjectMagickStick)
	GUARD_DECLARE_OLECREATE(CActionObjectMagickStick)
public:
	CActionObjectMagickStick();
	virtual ~CActionObjectMagickStick();
public:
	virtual bool Invoke();
	virtual bool Initialize();
	virtual bool DoChangeObjects();
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoUpdateSettings();	

	void CheckAddArea( int x, int y, int x1, int y1, int nNearX , int nNearY );
protected:
	CPoint	m_point;
	color	m_colorDelta;
	bool	m_bFillHoles;
//cache information
	int		m_cx, m_cy, m_colors;
	CPoint	*m_ppoints;
	byte	*m_pbyteMark;
	color	**m_prows;
	int		m_nPointCount;

	CRect	m_rect;

	bool	m_bDynamicBrightness;	
	
	void GetSettings();
	//void TestPoint( int x, int y );
};

class CActionCalcMeasResult : public CActionBase
{
	DECLARE_DYNCREATE(CActionCalcMeasResult)
	GUARD_DECLARE_OLECREATE(CActionCalcMeasResult)

public:
	CActionCalcMeasResult();
	virtual ~CActionCalcMeasResult();

public:
	IUnknown* GetActiveList(LPCTSTR szObjectType);
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();

protected:
	CObjectListWrp	m_listObjects;		// active object list
	CCompManager	m_manGroup;			// manager for groups
};

class CActionObjectScissors : public CActionObjectEditor
{
	DECLARE_DYNCREATE(CActionObjectScissors)
	GUARD_DECLARE_OLECREATE(CActionObjectScissors)
public:
	CActionObjectScissors();
	virtual ~CActionObjectScissors();


	virtual void DoDraw( CDC &dc );
	virtual bool Initialize();
	virtual bool DoChangeObjects();

	virtual bool DoMouseMove( CPoint pt );
	virtual bool DoLButtonDown( CPoint point );
	virtual bool DoLButtonUp( CPoint point );
	virtual bool DoStartTracking( CPoint point );
	virtual bool DoFinishTracking( CPoint point );
	virtual bool DoTrack( CPoint point );
	virtual bool DoRButtonDown( CPoint pt ){return CActionObjectBase::DoRButtonDown(pt); }; // проскакиваем тут сразу на 2 класса вверх по иерархии - нам не требуется перекрытое в ObjectEditor поведение
	virtual bool DoRButtonUp( CPoint pt ){return CActionObjectBase::DoRButtonUp(pt); }; // проскакиваем тут сразу на 2 класса вверх по иерархии - нам не требуется перекрытое в ObjectEditor поведение

	virtual bool IsAvaible();

protected:
	CRect		m_rcHotLine;
	Contour*	m_pContour;
	bool		m_bTrackNow;
	COLORREF	m_crColor;
};

// [vanek] - 01.12.2003

typedef _list_t<IUnknownPtr> SPUNK_LIST;

inline void spunk_list_free( SPUNK_LIST* p )
{
    delete p;
}

class CActionJoinAllObjects : public CFilterBase
{
	DECLARE_DYNCREATE(CActionJoinAllObjects)
	GUARD_DECLARE_OLECREATE(CActionJoinAllObjects)

public:
	CActionJoinAllObjects();
	virtual ~CActionJoinAllObjects();

	virtual bool InvokeFilter();

protected:

	BOOL	join_objects_by_class( IUnknown *punkObjListIn, IUnknown *punkObjListOut, int &nNotify,
									CString strClassifierName, BOOL bInitContours, long lClass = -2 );

	BOOL	init( IUnknown *punkObjList, CString strClassifierName, long lClass, int *pnObjNum, 
					LPRECT lprc_result, BSTR *pbstrCCName, int *pnPaneNum );

	BOOL	init_class_list( IUnknown *punkObjList, CString strClassifierName, BOOL bUseGroupByClass );

	//_list_t<IUnknownPtr>	m_list_childs;

	//_list_map_t<int, IUnknownPtr, cmp_spunk> m_lmap_object_to_class;

	_list_map_t<SPUNK_LIST*, long/*class*/, cmp_long, spunk_list_free> m_map_class_to_objects;
};

class CActionSetListZOrder : public CActionBase
{
	DECLARE_DYNCREATE(CActionSetListZOrder)
	GUARD_DECLARE_OLECREATE(CActionSetListZOrder)

public:
	CActionSetListZOrder();
	virtual ~CActionSetListZOrder();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};




#endif //__objactions_h__
