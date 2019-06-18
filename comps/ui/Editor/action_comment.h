#ifndef __ACTION_COMMENT_H__
#define __ACTION_COMMENT_H__

#define hotPointTypeCount	3
enum hotPointType
{
	hptArrowMove = 1,
	hptTextMove,
	hptTextEdit
};

#include "scriptnotifyint.h"
class CEditComment;
class CActionEditComment : public CInteractiveActionBase
{
	bool m_bMoveOutside, m_bPureOutsideComment;
	DECLARE_DYNCREATE(CActionEditComment)
	GUARD_DECLARE_OLECREATE(CActionEditComment)

ENABLE_UNDO();
public:
	CActionEditComment();
	virtual ~CActionEditComment();

	virtual bool DoUndo();
	virtual bool DoRedo();
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();

	virtual bool DoRButtonDown( CPoint pt );
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoRButtonUp( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoSetCursor( CPoint point );
	virtual bool CanStartTracking( CPoint pt );

	virtual bool DoUpdateSettings();

	virtual bool Initialize();
	virtual void Finalize();

	virtual bool IsRequiredInvokeOnTerminate();
	
	virtual void DoDraw(CDC &dc );
	virtual bool _Begin();
	virtual void _End();

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	virtual bool DoTerminate();		//this virtual called when user terminate the action
									//return true if target window should be repainted

	void _SetHotPointsArray(IUnknown* punkExceptThis = 0);
	void _KillHotPointsArray();
	int _DeterminateHotPt(CPoint pt, bool bSetParams);
	void _ModifyCurrentObj(CPoint pt);
	bool _IsPtInEditControl(CPoint pt);
	void _OnTrack(CPoint pt);

	void _ResetEditControl();
	void _ResetPunkEditedObj();
	void InvalidateObjectRect();

	virtual void DoActivateObjects( IUnknown *punkObject )
	{			Terminate();};

	IUnknown *CreateCommentObject();
	void DeleteCommentObject( IUnknown *punkObject );
	IUnknown	*GetActiveList();	//return the active object list


	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(DrAction, IObjectAction)
		com_call GetFirstObjectPosition( long *plpos );
		com_call GetNextObject( IUnknown **ppunkObject, long *plpos );
		com_call AddObject( IUnknown *punkObject );
	END_INTERFACE_PART(DrAction)

	BEGIN_INTERFACE_PART( ScriptNotify, IScriptNotifyListner )
		com_call ScriptNotify( BSTR bstrNotifyDesc, VARIANT varValue );
	END_INTERFACE_PART( ScriptNotify )


	bool _fill_comment_properties( IUnknown* punk );
	void _update_arrow();
	

public:
	byte			m_pColors[3];
	CRect			m_rcText;
	CRect			m_rcPrevText;
	CPoint			m_ptArrow;
	CPoint			m_ptPrevText;
	long			m_nID;
	CString			m_strText;
	bool			m_bUseBackColor;
	bool			m_bDrawArrow;
	bool			m_bEditText;
	bool			m_bEditPlacement;
	bool			m_bReEditText;
	CEditComment*	m_pEditComment;
	int				m_editedObj;
	hotPointType    m_hotType;
	bool			m_bPreventChangeCursor;
	bool			m_bEventRegistered;

	bool			m_bFromScript;

	// undo/redo support
	/*CPoint			m_ptUndoText;
	CPoint			m_ptUndoArrow;
	CString			m_strUndoText;
	CPoint			m_ptRedoText;
	CPoint			m_ptRedoArrow;
	CString			m_strRedoText;*/
	CPoint			m_ptUndo;
	CPoint			m_ptRedo;
	CString			m_strUndo;
	CString			m_strRedo;
	bool			m_bFinalized;
	bool			m_bCancel;

	

	CObjectListWrp			m_objects;
	CObjectListUndoRecord	m_changes;	//undo
	IUnknown				*m_punkDocument;
	//CPtrList				m_listAddedObjects;
	CTypedPtrArray<CPtrArray, CRect*> m_pHotPoints;

	IUnknown*				m_punkEditedObj;


	CRect					m_rcTargetImage;
	IUnknownPtr				m_ptrComment;
};




#endif //__ACTION_COMMENT_H__