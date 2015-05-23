#ifndef __dragdrop_h__
#define __dragdrop_h__

class __declspec(dllexport) CViewBase;
class CFrame;
#include "mousebase.h"

class export_data CDataSource : public COleDataSource//,					  public CMouseImpl
{
	//ENABLE_MULTYINTERFACE();
public:
	CDataSource();			//construct the Data Source object
	virtual ~CDataSource();	//destroy the Data source object
public:
	virtual void Init( CViewBase* pview, CFrame*  pOwnerFrame );
	virtual void DeInit();		//de-init
	virtual bool DoEnterMode(CPoint point);

	virtual void OnFinalRelease();
	//virtual bool CanStartTracking( CPoint pt );
//overrided
	//virtual IUnknown *GetTarget();
protected:
	CViewBase*	m_pviewTarget;
	CFrame*		m_pownerFrame;
};

class export_data CDropTarget : public COleDropTarget
{
public:
	CDropTarget();
	virtual ~CDropTarget();

	IUnknown* m_punkDroppedDataObject;
public:
	virtual void Init( CViewBase* pview);
	virtual void DeInit();
	virtual void OnFinalRelease();
protected:
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
	virtual DROPEFFECT OnDragScroll(CWnd* pWnd, DWORD dwKeyState,
		CPoint point);

protected:
	CViewBase	*m_pviewTarget;
	sptrINamedDataObject	m_sptrDragObject;
	CPoint		m_pointDragOffset;
	UINT		m_cf;
	DWORD		m_dwSaveDragFlags;
	
};

#endif // __dragdrop_h__