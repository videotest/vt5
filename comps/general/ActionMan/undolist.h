#ifndef __undolist_h__
#define __undolist_h__

#include "undoint.h"

#include "\vt5\awin\misc_list.h"


struct	undo_info
{
	_bstr_t	caption;
	_list_t<IUnknown*>	actions;
};

inline void free_undo_info( undo_info *p )
{
	if( !p )return;
	for (TPOS lpos = p->actions.head(); lpos; lpos = p->actions.next(lpos))
		p->actions.get( lpos )->Release();

	delete p;
}

class CUndoList : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CUndoList);
	GUARD_DECLARE_OLECREATE(CUndoList);
public:
	CUndoList();
	~CUndoList();
protected:
	void ClearUndoList();
	void ClearRedoList();
	void Check();
	void CheckUndoCount();
protected:
	DECLARE_INTERFACE_MAP();

	BEGIN_INTERFACE_PART(UndoList, IUndoList2)
		com_call DoUndo();
		com_call DoRedo();
		com_call GetUndoStepsCount( int *pnStepsCount );
		com_call GetRedoStepsCount( int *pnStepsCount );
		com_call AddAction( IUnknown *punkAction );
		com_call RemoveAction( IUnknown *punkAction );
		com_call GetLastUndoAction( IUnknown **ppunkAction );
		com_call ClearUndoRedoList( );		
		com_call BeginGroup( BSTR bstr );
		com_call EndGroup( long code );
		com_call ClearRedoList( );
		com_call GetEnableUndo( BOOL* pbenable );
		com_call SetEnableUndo( BOOL benable );

		com_call BeginGroup2( BSTR bstr, long lStepsBack ); // начать группу, включив в нее акции из lStepsBack последних групп
		com_call GetUndoAction( int nUndoStepPos, int nActionPos, IUnknown **ppunkAction ); // получить указатель на акцию в undo-листе
		// nUndoStepPos - номер шага Undo, 0 - последний
		// nActionPos - номер акции в шаге, 0 - последн€€
		com_call GetUndoActionName( int nUndoStepPos, int nActionPos, BSTR *pbstrActionName); // получить им€ акции в undo-листе
		com_call DeleteLastUndoAction( );
		com_call EndGroup2( BSTR bstr,  long code );
	END_INTERFACE_PART(UndoList);

	_list_t	<undo_info*>	m_listUndoActions;
	_list_t	<undo_info*>	m_listRedoActions;

	undo_info				*m_pgroup;
	IActionManagerPtr		m_ptrActionManager;
	BOOL					m_benable_undo;
};

#endif //__undolist_h__