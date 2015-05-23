#ifndef __editoractions_h__
#define __editoractions_h__

#include "editorint.h"
#include "ImagePane.h"

#define hotPoint		8  //side of squre of hot points
#define ArrHeight		20
#define ArrAngle		25

//#define MaxLineWidth	50


enum DodgeBurnType
{
	dbtShadow,
	dbtMidtones,
	dbtHightlight
};


class CLinePreviewCtrl;


class CActionEditorBase : public CInteractiveActionBase
{
	ENABLE_UNDO();
	CObjectListUndoRecord	m_changes;	//undo
public:
	
protected:
	CActionEditorBase();
	virtual ~CActionEditorBase();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();

	//virtual bool OnRButtonDown( CPoint pt );
	//virtual bool OnLButtonDown( CPoint pt );
	virtual bool DoRButtonDown( CPoint pt );
	virtual bool DoRButtonUp( CPoint pt );
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	
	virtual bool DoSetCursor( CPoint point );

	virtual bool Initialize();
	virtual void Finalize();
	virtual void OnFinalize(){};
	virtual bool IsCompatibleTarget();
	//virtual void OnInitialize(){};

	virtual bool DoUpdateSettings();
	virtual void DoActivateObjects( IUnknown *punkObject )
	{	
		if(m_bWorkWithBinary)
		{
			BSTR bstrType;
			INamedDataObjectPtr	ptrNamed( punkObject );
			if(ptrNamed!=0)
			{
				ptrNamed->GetType( &bstrType );
				CString s( bstrType);
				SysFreeString(bstrType);
				if(s != szTypeBinaryImage && m_bTerminateOnActivateObj)
				{
					Terminate();
				}
				UpdatePropertyPages();
				return;
			}
			Terminate();
		}
		else if(m_bTerminateOnActivateObj)
			Terminate();
	};

	virtual void _DoDraw(){};
	bool _Begin(bool bCanUseBinary = true, long nDiffType = -1);
	virtual void OnBegin(){}; 
	virtual void _End(){};

	void _UpdateLinePreview();

	virtual bool DoTerminate();


	inline void RestoreImageFromChanges(CRect rc, bool bInvalidate = true);
	inline void MoveFillInfoToImage(CRect rc, color* pColors1, color* pColors2);
	void UndoImage(bool bDeleteArray);

	virtual bool DoButtonDown( CPoint pt ) {return true;}
	virtual bool DoButtonUp( CPoint pt ) {return true;}

protected:
	CString			m_strActionName;
	CWalkFillInfo*	m_pwfi;
	IUnknown*		m_punkImage;
	CRect			m_rect;
	CRect			m_rectInvalid;
	bool			m_bInteractive;
	color*			m_pColors;
	color*			m_pFillColors;
	bool			m_bUseBackColor;
	CPoint			m_imgOffset;
	int				m_nLineWidth;
	int				m_nLineWidth2; // A.M. BT4387
	bool			m_bUseWidth2; // Is action draws using m_nLineWidth2 by right button
	CTypedPtrArray<CPtrArray, FillInfo*> m_ptrFillInfo;
	CImagePaneCache m_ImagePaneCache; // for Redo
	
	CLinePreviewCtrl		*m_pctrl;

	bool			m_bWorkWithBinary;		//current m_ptrFillInfo type
	bool			m_bBinaryIsActive;		//if this member is false than we can't draw because binary image not active yet


	bool			m_bRightMouseButton; //for prevent trables with context menu

	bool			m_bTerminateOnActivateObj;
	BOOL			m_bOldModified;

	
	BEGIN_INTERFACE_PART(Info, IPaintInfo)
		com_call GetInvalidRect( RECT* pRect );
	END_INTERFACE_PART(Info);
	BEGIN_INTERFACE_PART(Edit, IEditorAction)
		com_call GetImage( IUnknown **ppunkImage );
		com_call GetDifferences( void ***pppDifferences, int *pnStart, int *pnCount, DWORD* pdwFlags );
		com_call SetDifferences( void **ppDifferences, RECT rect, DWORD dwFlags );
	END_INTERFACE_PART(Edit)

	
	DECLARE_INTERFACE_MAP();
};

void AttachArrayToImage(CTypedPtrArray<CPtrArray, FillInfo*> *pptrFillInfo, IUnknown* punkImage, CRect rcInvalid, color* pColors, color* pBackColor, CImagePaneCache &ImagePaneCache);
void AttachArrayToImageBin(CTypedPtrArray<CPtrArray, FillInfo*> *pptrFillInfo, IUnknown* punkImage, CRect rcInvalid, color* pColors, color* pBackColor);
void RestoreFromPixArray(CTypedPtrArray<CPtrArray, FillInfo*> *pptrFillInfo, IUnknown* punkImage, CRect rcInvalid, bool bDeleteArray = true);
void RestoreFromPixArrayBin(CTypedPtrArray<CPtrArray, FillInfo*> *pptrFillInfo, IUnknown* punkImage, CRect rcInvalid, bool bDeleteArray = true);

inline void CActionEditorBase::RestoreImageFromChanges(CRect rc, bool bInvalidate)
{
	rc.NormalizeRect();
	rc.InflateRect(m_nLineWidth, m_nLineWidth, m_nLineWidth, m_nLineWidth);

	if(m_bWorkWithBinary)
		RestoreFromPixArrayBin(&m_ptrFillInfo, m_punkImage, rc);
	else
		RestoreFromPixArray(&m_ptrFillInfo, m_punkImage, rc);

	if(bInvalidate)
	{
		CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
		rc.OffsetRect(m_imgOffset);
		rc = ConvertToWindow( m_punkTarget, rc);
		pwnd->InvalidateRect(rc);
	}
}

inline void CActionEditorBase::MoveFillInfoToImage(CRect rc, color* pColors1, color* pColors2)
{
	if(m_bWorkWithBinary)
		m_pwfi->AttachFillInfoToImageBin(&m_ptrFillInfo, m_punkImage, rc, pColors1, pColors2, m_punkTarget);
	else
	{
		m_pwfi->AttachFillInfoToImage(&m_ptrFillInfo, m_punkImage, rc, pColors1, pColors2, m_punkTarget);
		m_ImagePaneCache.InitPaneCache(m_punkImage, m_punkTarget);
		m_ImagePaneCache.FreeImage();
	}
	m_pwfi->ClearMasks();
	rc.OffsetRect(m_imgOffset);
	rc = ConvertToWindow( m_punkTarget, rc );
	CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
	pwnd->InvalidateRect(rc);
	pwnd->UpdateWindow();
}



//[ag]1. classes


class CActionDodgeBase : public CActionEditorBase
{
public:
	CActionDodgeBase();
	virtual ~CActionDodgeBase();

public:
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );

	virtual bool Initialize();
	virtual void OnFinalize();
	virtual bool DoUpdateSettings();

	virtual bool DoSetCursor( CPoint point );

	virtual void _DoDraw();
	virtual void OnBegin();


protected:
	virtual void Dodge(CRect rc) = 0;
	
	bool	m_bDrawMode;
	bool	m_bInitialized;
	bool	m_bTracking;
	long	m_nExposure;
	//bool	m_bSmooth;
	long	m_nDodgeBurnType;
	BOOL*	m_pbMask;
};

class CActionDodge : public CActionDodgeBase
{
	DECLARE_DYNCREATE(CActionDodge)
	GUARD_DECLARE_OLECREATE(CActionDodge)
	CActionDodge();
protected:
	virtual void Dodge(CRect rc);
	virtual bool DoRedo();
};

class CActionBurn : public CActionDodgeBase
{
	DECLARE_DYNCREATE(CActionBurn)
	GUARD_DECLARE_OLECREATE(CActionBurn)
	CActionBurn();
protected:
	virtual void Dodge(CRect rc);
	virtual bool DoRedo();
};


class CActionEditFloodFill : public CActionEditorBase
{
	DECLARE_DYNCREATE(CActionEditFloodFill)
	GUARD_DECLARE_OLECREATE(CActionEditFloodFill)

ENABLE_UNDO();
public:
	CActionEditFloodFill();
	virtual ~CActionEditFloodFill();

public:
//undo interface
	//virtual bool DoUndo();
	//virtual bool DoRedo();

	virtual bool Initialize();
	virtual void _DoDraw();
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoRButtonDown( CPoint pt );
	virtual bool DoMouseMove( CPoint pt );	
	virtual void _End();
	virtual bool DoSetCursor( CPoint point );
	virtual bool DoUpdateSettings();

};


class CActionEditPickColor : public CInteractiveActionBase
{
	DECLARE_DYNCREATE(CActionEditPickColor)
	GUARD_DECLARE_OLECREATE(CActionEditPickColor)
	ENABLE_SETTINGS();
public:
	CActionEditPickColor();
	virtual ~CActionEditPickColor();

public:
	virtual bool ExecuteSettings( CWnd *pwndParent );
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
  virtual bool IsCompatibleTarget();

	virtual bool DoRButtonDown( CPoint pt );
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoRButtonUp( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoMouseMove( CPoint pt );	

	

	virtual bool Initialize();
	virtual void Finalize();

private:
	void _SetPaneValue(CPoint pt);

//	bool GetColor( CPoint pt, COLORREF& dwRGB, color& H, color& L, color& S );

	long	m_nX;
	long	m_nY;
	bool	m_bByLButton;

	int				m_nPaneNum;


//	IUnknownPtr GetImagePtr();

};

class CActionEditAutoBackground : public CActionBase
{
	DECLARE_DYNCREATE(CActionEditAutoBackground)
	GUARD_DECLARE_OLECREATE(CActionEditAutoBackground)

public:
	CActionEditAutoBackground();
	virtual ~CActionEditAutoBackground();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionEditRect : public CActionEditorBase
{
	DECLARE_DYNCREATE(CActionEditRect)
	GUARD_DECLARE_OLECREATE(CActionEditRect)
public:
	CActionEditRect();
	virtual ~CActionEditRect();

public:
//	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoButtonDown( CPoint pt );

	virtual bool Initialize();

	virtual void _DoDraw();
	virtual void OnBegin();
	virtual void _End();

	virtual bool DoSetCursor( CPoint point );
	virtual bool DoUpdateSettings();


	//int		m_nLineWidth;
	bool	m_bNeedToFill;
	bool	m_bFillByDrawColor;
};

class CActionEditCircle : public CActionEditorBase
{
	DECLARE_DYNCREATE(CActionEditCircle)
	GUARD_DECLARE_OLECREATE(CActionEditCircle)

public:
	CActionEditCircle();
	virtual ~CActionEditCircle();

public:
//	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoButtonDown( CPoint pt );

	virtual bool Initialize();

	virtual void _DoDraw();
	virtual void OnBegin();
	virtual void _End();

	virtual bool DoSetCursor( CPoint point );
	virtual bool DoUpdateSettings();

	
	//int		m_nLineWidth;
	bool	m_bNeedToFill;
	bool	m_bFillByDrawColor;
};

class CActionEditSplainLine : public CActionEditorBase
{
	DECLARE_DYNCREATE(CActionEditSplainLine)
	GUARD_DECLARE_OLECREATE(CActionEditSplainLine)

public:
	CActionEditSplainLine();
	virtual ~CActionEditSplainLine();

public:
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoRButtonDown( CPoint pt );
	//virtual bool OnLButtonUp( CPoint pt );
	//virtual bool OnRButtonUp( CPoint pt );
	virtual bool DoLButtonDblClick( CPoint pt );
	virtual bool DoRButtonDblClick( CPoint pt );
	virtual bool DoMouseMove( CPoint pt );
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoSetCursor( CPoint point );

	virtual bool Initialize();
	virtual void OnFinalize();

	virtual bool DoTerminate();

	virtual bool IsRequiredInvokeOnTerminate();

	virtual bool DoUpdateSettings();

	virtual void _DoDraw();
	virtual void DoDraw( CDC &dc );
	virtual void OnBegin();
	virtual void _End();

	int		_DeterminateHotPt(CPoint pt);
	void	_ClearHotPoints(int nNumber = -1);

	//int		m_nLineWidth;
	bool	m_bDrawMode;
	bool	m_bInitialized;
	bool	m_bByBackColor;
	bool	m_bEditMode;
	bool	m_bRing;
	bool	m_bEditPlacement;
	int		m_nDraggingPt;
	CRect	m_rectOld;
	CRect	m_rcFirstPoint;
	CTypedPtrArray<CPtrArray, CPoint*> m_pHotPoints;
	Contour* m_pContour;
};

//enum FreeLineShapes {flshCircle = 1, flshSquare};

class CActionEditFreeLine : public CActionEditorBase
{
	DECLARE_DYNCREATE(CActionEditFreeLine)
	GUARD_DECLARE_OLECREATE(CActionEditFreeLine)

public:
	CActionEditFreeLine();
	virtual ~CActionEditFreeLine();

public:
	//virtual bool DoUndo();
	//virtual bool DoRedo();
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoRButtonDown( CPoint pt );
	virtual bool DoRButtonUp( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );

	virtual bool Initialize();
	virtual void OnFinalize();

	virtual bool DoSetCursor( CPoint point );
	
	virtual void _DoDraw();
	virtual void OnBegin();
	virtual void _End();

	//int		m_nLineWidth;
	bool	m_bDrawMode;
	bool	m_bInitialized;
	bool	m_bTracking;
//	FreeLineShapes m_flshForm;
	//CTimeTest m_timeTest;

	//CLinePreviewCtrl		*m_pctrl;
};

class CActionEditLine : public CActionEditorBase
{
	DECLARE_DYNCREATE(CActionEditLine)
	GUARD_DECLARE_OLECREATE(CActionEditLine)

public:
	CActionEditLine();
	virtual ~CActionEditLine();

public:
//	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoButtonDown( CPoint pt );
	virtual bool DoButtonUp( CPoint pt );

	virtual bool Initialize();

	virtual bool DoSetCursor( CPoint point );

	virtual void _DoDraw();
	virtual void OnBegin();
	virtual void _End();

	//int		m_nLineWidth;

};

class CActionPolyline : public CActionEditorBase
{
	DECLARE_DYNCREATE(CActionPolyline)
	GUARD_DECLARE_OLECREATE(CActionPolyline)

public:
	CActionPolyline();
	virtual ~CActionPolyline();

	virtual bool Initialize();
	virtual void Finalize();
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoRButtonDown( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoLButtonDblClick( CPoint pt );
	virtual bool DoRButtonDblClick( CPoint pt );
	virtual bool DoMouseMove( CPoint pt );
	virtual bool DoSetCursor( CPoint point );
	virtual void _DoDraw();

	void _add_point( CPoint &pt, bool bReplaceLast );

	bool IsFirstPoint()	{return m_pcontour->nContourSize == 2;	}
	bool IsActive()		{return m_pcontour->nContourSize != 0;	}
protected:
	Contour	*m_pcontour;
	CRect	m_rect_old;
};

class CActionBinEditSplainLine : public CActionEditSplainLine
{
	DECLARE_DYNCREATE(CActionBinEditSplainLine)
	GUARD_DECLARE_OLECREATE(CActionBinEditSplainLine)
public:
	CActionBinEditSplainLine(){m_strActionName = "BinEditSplainLine";m_bWorkWithBinary = true;};
};

class CActionBinEditFreeLine : public CActionEditFreeLine
{
	DECLARE_DYNCREATE(CActionBinEditFreeLine)
	GUARD_DECLARE_OLECREATE(CActionBinEditFreeLine)
public:
	CActionBinEditFreeLine()
	{
		m_strActionName = "BinEditFreeLine";
		m_bWorkWithBinary = true;
		m_bUseWidth2 = true;
	};

	virtual bool Initialize();
};

class CActionBinEditLine : public CActionEditLine
{
	DECLARE_DYNCREATE(CActionBinEditLine)
	GUARD_DECLARE_OLECREATE(CActionBinEditLine)
public:
	CActionBinEditLine(){m_strActionName = "BinEditLine";m_bWorkWithBinary = true;};
};

class CActionPolylineBin : public CActionPolyline
{
	DECLARE_DYNCREATE(CActionPolylineBin)
	GUARD_DECLARE_OLECREATE(CActionPolylineBin)
public:
	CActionPolylineBin(){m_strActionName = "BinEditPolyline";m_bWorkWithBinary = true;}
};


#endif //__editoractions_h__
