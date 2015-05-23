#ifndef __imgactions_h__
#define __imgactions_h__

struct CAphineParams;
//[ag]1. classes


//paul12.04.2002

#define DECLARE_AVAIBLE() \
bool IsAvaible() \
{ \
	CChechAvailable avail; \
	return avail.IsAvaible(); \
} \

struct CChechAvailable
{
	bool IsAvaible();
};


bool fill_background_color( IUnknown* punk_image );

class CCropRectCtrl;
class CActionCrop : public CInteractiveActionBase,
					public CLongOperationImpl
{
	ENABLE_UNDO();
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();
	DECLARE_DYNCREATE(CActionCrop)
	GUARD_DECLARE_OLECREATE(CActionCrop)

public:
	CActionCrop();
	virtual ~CActionCrop();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();
	virtual bool IsRequiredInvokeOnTerminate(){return false;}

	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoUpdateSettings();

	virtual bool Initialize();	
	virtual void Finalize();

	virtual bool DoUndo();
	virtual bool DoRedo();


protected:
	void StoreCurrentExecuteParams();
	bool CanDoCrop();
	void _CreateController(CSize sizeContr);

	CCropRectCtrl*	m_pctrl;
	CSize			m_size;
	CPoint			m_point;
	CObjectListUndoRecord	m_changes;
	IUnknownPtr		m_ptrImage;
};

class CActionLongUndoBase : public CActionBase,
							public CLongOperationImpl
{
	ENABLE_UNDO();
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();
public:
	virtual bool DoUndo();
	virtual bool DoRedo();

	CObjectListUndoRecord	m_changes;
};

class CActionBorders : public CFilterBase
{
	DECLARE_DYNCREATE(CActionBorders)
	GUARD_DECLARE_OLECREATE(CActionBorders)

public:
	CActionBorders();
	virtual ~CActionBorders();

public:
	virtual bool InvokeFilter();

	color** m_ppColors;
	color	m_colors;
};

class CActionClone : public CActionLongUndoBase
{
	DECLARE_DYNCREATE(CActionClone)
	GUARD_DECLARE_OLECREATE(CActionClone)

public:
	CActionClone();
	virtual ~CActionClone();

public:
	virtual bool Invoke();
//	virtual bool IsAvaible(); 
	// Max : 2706
	DECLARE_AVAIBLE()
};

class CActionExpansion : public CActionLongUndoBase
{
	DECLARE_DYNCREATE(CActionExpansion)
	GUARD_DECLARE_OLECREATE(CActionExpansion)
public:
	CActionExpansion();
	virtual ~CActionExpansion();

public:
	virtual bool Invoke();
	virtual bool IsAvaible(); 
		
	CImageWrp				m_imageSource;
	CImageWrp**				m_pimagesDest;
	long					m_nImages;
};

class CActionMerge : public CFilterBase
{
	DECLARE_DYNCREATE(CActionMerge)
	GUARD_DECLARE_OLECREATE(CActionMerge)

public:
	CActionMerge();
	virtual ~CActionMerge();
public:
	virtual bool InvokeFilter();
	// vk begin
	virtual bool IsAvaible();
	// vk end

	CImageWrp**		m_pImagesSource;
	CImageWrp		m_imageDest;
	int				m_nImages;
};

class CTransformBase2 : public CFilterBase
{
public:
	CTransformBase2();
	~CTransformBase2();
public:
	virtual void DoTransform(CImageWrp& image, CImageWrp& imageTarget){};
	virtual bool SetParams(CAphineParams* pparams) = 0;

	virtual bool InvokeFilter();
	virtual CRect GetRect(CRect rcSrc){return rcSrc;}; 
protected:
	color**	m_ppcolorscr;
	color***	m_ppcolordest;
	byte**		m_ppmask;
	long		m_colors;
	bool		m_bAphineTransform;
	bool		m_bCopyMask;
};

class CActionRotate270 : public CTransformBase2
{
	DECLARE_DYNCREATE(CActionRotate270)
	GUARD_DECLARE_OLECREATE(CActionRotate270)
public:
	CActionRotate270();

	virtual void DoTransform(CImageWrp& image, CImageWrp& imageTarget);
	virtual bool SetParams(CAphineParams* pparams);
	virtual CRect GetRect(CRect rcSrc){return CRect(rcSrc.left, rcSrc.top, rcSrc.left + rcSrc.Height(), rcSrc.top + rcSrc.Width());}; 
};


class CActionRotate180 : public CTransformBase2
{
	DECLARE_DYNCREATE(CActionRotate180)
	GUARD_DECLARE_OLECREATE(CActionRotate180)

public:
	CActionRotate180();
	virtual void DoTransform(CImageWrp& image, CImageWrp& imageTarget);
	virtual bool SetParams(CAphineParams* pparams);
};

class CActionRotate90 : public CTransformBase2
{
	DECLARE_DYNCREATE(CActionRotate90)
	GUARD_DECLARE_OLECREATE(CActionRotate90)
public:
	CActionRotate90();

	virtual void DoTransform(CImageWrp& image, CImageWrp& imageTarget);
	virtual bool SetParams(CAphineParams* pparams);
	virtual CRect GetRect(CRect rcSrc){return CRect(rcSrc.left, rcSrc.top, rcSrc.left + rcSrc.Height(), rcSrc.top + rcSrc.Width());}; 
};

class CActionMirrorVert : public CTransformBase2
{
	DECLARE_DYNCREATE(CActionMirrorVert)
	GUARD_DECLARE_OLECREATE(CActionMirrorVert)

public:
	CActionMirrorVert();
	virtual void DoTransform(CImageWrp& image, CImageWrp& imageTarget);
	virtual bool SetParams(CAphineParams* pparams);
	// Max : 2706
	DECLARE_AVAIBLE()
};

class CActionMirrorHorz : public CTransformBase2
{
	DECLARE_DYNCREATE(CActionMirrorHorz)
	GUARD_DECLARE_OLECREATE(CActionMirrorHorz)

public:
	CActionMirrorHorz();
	virtual void DoTransform(CImageWrp& image, CImageWrp& imageTarget);
	virtual bool SetParams(CAphineParams* pparams);
	// Max : 2706
	DECLARE_AVAIBLE()
};

class CActionRotate : public CTransformBase2
{
	DECLARE_DYNCREATE(CActionRotate)
	GUARD_DECLARE_OLECREATE(CActionRotate)

public:
	CActionRotate();
public:
	virtual bool SetParams(CAphineParams* pparams);
};

class CActionResize : public CTransformBase2
{
	DECLARE_DYNCREATE(CActionResize)
	GUARD_DECLARE_OLECREATE(CActionResize)

public:
	CActionResize();

public:
	virtual bool SetParams(CAphineParams* pparams);
};

class CActionImageTransform : public CActionBase
{
	DECLARE_DYNCREATE(CActionImageTransform)
	GUARD_DECLARE_OLECREATE(CActionImageTransform)

ENABLE_UNDO();
public:
	CActionImageTransform();
	virtual ~CActionImageTransform();

public:
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
//update interface
	virtual bool IsAvaible();
protected:
	void _UpdateObject();
protected:
	CObjectListUndoRecord	m_changes;
	bool		m_bObject;
	CImageWrp	m_imageSource, m_imageDest;
	CObjectWrp	m_object;

};

class CActionShowObjectViewBase : public CActionShowViewBase
{
protected:
	virtual void AfterInvoke()
	{
		::SetValue(GetAppUnknown(), "General", "LastUsedObjectView", _variant_t(GetViewProgID()));
	};
};

class CActionShowBinaryViewBase : public CActionShowViewBase
{
	protected:
	virtual void AfterInvoke()
	{
		::SetValue(GetAppUnknown(), "General", "LastUsedBinaryView", _variant_t(GetViewProgID()));
	};
};

class CActionShowImage : public CActionShowObjectViewBase
{
	DECLARE_DYNCREATE(CActionShowImage)
	GUARD_DECLARE_OLECREATE(CActionShowImage)
public:
	virtual CString GetViewProgID();
};

class CActionShowMasks : public CActionShowObjectViewBase
{
	DECLARE_DYNCREATE(CActionShowMasks)
	GUARD_DECLARE_OLECREATE(CActionShowMasks)

public:
	virtual CString GetViewProgID();
};

class CActionShowObjects : public CActionShowObjectViewBase
{
	DECLARE_DYNCREATE(CActionShowObjects)
	GUARD_DECLARE_OLECREATE(CActionShowObjects)

public:
	virtual CString GetViewProgID();
};

class CActionShowPseudo : public CActionShowObjectViewBase
{
	DECLARE_DYNCREATE(CActionShowPseudo)
	GUARD_DECLARE_OLECREATE(CActionShowPseudo)
public:
	virtual CString GetViewProgID();
};

class CActionShowSource : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowSource)
	GUARD_DECLARE_OLECREATE(CActionShowSource)
public:
	virtual CString GetViewProgID();
};

class CActionShowBinaryFore : public CActionShowBinaryViewBase
{
	DECLARE_DYNCREATE(CActionShowBinaryFore)
	GUARD_DECLARE_OLECREATE(CActionShowBinaryFore)
public:
	virtual CString GetViewProgID();
};

class CActionShowBinaryBack : public CActionShowBinaryViewBase
{
	DECLARE_DYNCREATE(CActionShowBinaryBack)
	GUARD_DECLARE_OLECREATE(CActionShowBinaryBack)
public:
	virtual CString GetViewProgID();
};

class CActionShowBinary : public CActionShowBinaryViewBase
{
	DECLARE_DYNCREATE(CActionShowBinary)
	GUARD_DECLARE_OLECREATE(CActionShowBinary)
public:
	virtual CString GetViewProgID();
};

class CActionShowBinaryContour : public CActionShowBinaryViewBase
{
	DECLARE_DYNCREATE(CActionShowBinaryContour)
	GUARD_DECLARE_OLECREATE(CActionShowBinaryContour)
public:
	virtual CString GetViewProgID();
};

class CActionShowView : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowView)
	GUARD_DECLARE_OLECREATE(CActionShowView)
public:
	virtual CString GetViewProgID();
};

class CActionShowPhase : public CActionShowBinaryViewBase
{
	DECLARE_DYNCREATE(CActionShowPhase)
	GUARD_DECLARE_OLECREATE(CActionShowPhase)
public:
	virtual CString GetViewProgID();
};

class CActionShowCalibr : public CActionBase
{
	DECLARE_DYNCREATE(CActionShowCalibr)
	GUARD_DECLARE_OLECREATE(CActionShowCalibr)
protected:
	virtual bool Invoke();
	virtual bool IsChecked();
	virtual bool IsAvaible();
};

class CActionCipher : public CFilterBase
{
	DECLARE_DYNCREATE(CActionCipher)
	GUARD_DECLARE_OLECREATE(CActionCipher)

public:
	CActionCipher();
	virtual ~CActionCipher();
public:
	virtual bool InvokeFilter();
};

#endif //__imgactions_h__
