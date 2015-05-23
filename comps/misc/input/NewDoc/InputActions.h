#ifndef __InputActions_H__
#define __InputActions_H__

#include "common.h"
#include "NotifySpec.h"

//[ag]1. classes

class CActionDoInputImage : public CActionBase
{
	DECLARE_DYNCREATE(CActionDoInputImage)
	GUARD_DECLARE_OLECREATE(CActionDoInputImage)

	IUnknownPtr m_punkRes;
	INotifyPlacePtr m_sptrNty;
	IUnknownPtr m_punkUndoObj;

public:
	CActionDoInputImage();
	virtual ~CActionDoInputImage();

protected:
	virtual bool DoUndo();
	virtual bool DoRedo();
public:
	ENABLE_SETTINGS();
	ENABLE_UNDO();
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
	virtual bool IsAvaible();
};

class CActionInputImage : public CActionBase
{
	DECLARE_DYNCREATE(CActionInputImage)
	GUARD_DECLARE_OLECREATE(CActionInputImage)

public:
	CActionInputImage();
	virtual ~CActionInputImage();

public:
	ENABLE_SETTINGS();
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
};

class CActionInputVideo : public CActionBase
{
	DECLARE_DYNCREATE(CActionInputVideo)
	GUARD_DECLARE_OLECREATE(CActionInputVideo)

public:
	CActionInputVideo();
	virtual ~CActionInputVideo();

public:
	virtual bool Invoke();
};

class CActionShowPreview : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowPreview)
	GUARD_DECLARE_OLECREATE(CActionShowPreview)
public:
	virtual CString GetViewProgID();
};


#endif //__InputActions_H__
