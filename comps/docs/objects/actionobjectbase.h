#ifndef __actionobjectbase_h__
#define __actionobjectbase_h__

#include "editorint.h"

enum FreeMode
{
	fmUndef = -1,
	fmClick = 0,
	fmDblClick = 1,
};
enum DefineMode
{
	dmUndef = -1,
	dmNormal = 0,
	dmAdd = 1,
	dmSub = 2,
	dmSep = 3
};


//base class for object actions
class CActionObjectBase : public CInteractiveActionBase
{
	ENABLE_UNDO();
	void update_phases_info(IUnknown* pList);
	bool is_phased(IUnknown* unk);
public:
	CActionObjectBase();
	~CActionObjectBase();
protected:
//"abstract" part
	virtual bool DoChangeObjects() = 0;
	
protected:
//overrided virtuals
	virtual bool Initialize();
	virtual void Finalize();
	virtual bool Invoke();
	virtual bool DoRedo();
	virtual bool DoUndo(); 
	virtual bool DoUpdateSettings();
	virtual bool DoLButtonDown( CPoint pt );
	virtual void DoActivateObjects( IUnknown *punkObject )		{Terminate();}
	virtual bool IsCompatibleTarget();
	virtual bool DoSetCursor( CPoint point );

	virtual bool IsAvaible();
	virtual bool CanActivateObjectOnLbutton();
	virtual bool DoRButtonDown( CPoint pt )
	{ 
			_activate_object( pt );
			return __super::DoRButtonDown( pt );
	}

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	void _activate_object( CPoint pt );
	void _get_img_rect(IUnknown *pImage,RECT &rc);

public:
//helpers
	virtual IUnknown *CreateMeasureObject();
	virtual void DeleteMeasureObject( IUnknown *punkObject );
	virtual bool DoChangePhases(CWalkFillInfo* pWFI, bool bClear = false);
	IUnknown	*GetActiveList();	//return the active object list
	IUnknown	*GetActiveImage();	//return the active image

	void	ProcessObjectList();
	void	ProcessObjectListSub();
	void	ActivateObject( CPoint pt );
	void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
protected:
	void _MapObjectMaskToInfo( CObjectWrp &object, CWalkFillInfo &info, DefineMode mode );
	void _AttachMaskToObject( CObjectWrp &object, CWalkFillInfo &info );
	void _CreateIntersectList( CWalkFillInfo *pwfi );
	void _DestroyIntersectList();
	bool SetObjectRemoveDelay(bool bDelay);
	bool CalculatePhases();
protected:
	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(DrAction, IObjectAction)
		com_call GetFirstObjectPosition(LONG_PTR *plpos);
		com_call GetNextObject(IUnknown **ppunkObject, LONG_PTR *plpos);
		com_call AddObject( IUnknown *punkObject );
	END_INTERFACE_PART(DrAction)

protected:
	CObjectListWrp			m_objects;
	CImageWrp				m_image;

protected:
	CObjectListUndoRecord	m_changes;	//undo
	IUnknown				*m_punkDocument;
	CPtrList				m_listAddedObjects;
	CPtrList				m_listToDocument;

	DefineMode				m_nDefineMode;
	bool					m_bNoProcessOperations;
	bool					m_bObjectActivated;
	int						m_nMinSize;

	long					m_nOldSelectionType;	
	CPtrList				m_listIntersect;	//list of "intersect" object
	CRect					m_rectInterest;
	HCURSOR					m_hcurAdd, m_hcurSub;
	bool					m_bPhased;
	int						m_nPhaseEdit;
	bool					m_binvoke_return;
};
struct OBJINF
{
	OBJINF() : bChanged(false), rowMask(0) {};

	bool bChanged;
	CObjectWrp objectIn;
	CObjectWrp objectOut;
	CImageWrp imageIn;
	CImageWrp imageOut;
	byte* rowMask;
};


#endif // __actionobjectbase_h__