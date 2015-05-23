#if !defined(AFX_CHROMOACTIONS_H__387595A5_2E70_440B_A7FA_24DE4891CB97__INCLUDED_)
#define AFX_CHROMOACTIONS_H__387595A5_2E70_440B_A7FA_24DE4891CB97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChromoActions.h : header file
//

#include "carioint.h"
#include "ndata_api.h"

/////////////////////////////////////////////////////////////////////////////
class CActionShowCarioView : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowCarioView)
	GUARD_DECLARE_OLECREATE(CActionShowCarioView)

public:
	virtual bool Invoke();
	virtual CString GetViewProgID();
	virtual bool IsAvaible();
	virtual bool IsChecked();
	virtual IUnknownPtr get_view();
	virtual CarioViewMode get_view_mode(){	return cvmPreview;}
};

/////////////////////////////////////////////////////////////////////////////
class CActionShowCarioViewDesign : public CActionShowCarioView
{
	DECLARE_DYNCREATE(CActionShowCarioViewDesign)
	GUARD_DECLARE_OLECREATE(CActionShowCarioViewDesign)	
	virtual CarioViewMode get_view_mode(){	return cvmDesign;}
};

/////////////////////////////////////////////////////////////////////////////
class CActionShowCarioAnalize : public CActionShowCarioView
{
	DECLARE_DYNCREATE(CActionShowCarioAnalize)
	GUARD_DECLARE_OLECREATE(CActionShowCarioAnalize)	
	
public:
	virtual CString GetViewProgID();
	virtual CarioViewMode get_view_mode(){	return cvmCellar;}
};

/////////////////////////////////////////////////////////////////////////////
//add delete line & cell
/////////////////////////////////////////////////////////////////////////////
class CActionCarioEditBase : public CActionBase
{
protected:
	CActionCarioEditBase(){}
	virtual ~CActionCarioEditBase(){}

public:
	virtual void OnAttachTarget( IUnknown *punkTarget, bool bAttach );
	ICarioViewPtr get_view();
	virtual bool IsAvaible();

	int		get_line_count();	
	bool	set_line_at_pos( int nPos );

	int		get_cell_count( int nLine );
	bool	set_cell_at_pos( int nLine, int nPos );

	bool	delete_line( int nLine );
	bool	delete_cell( int nLine, int nCell );

	int		get_active_line();
	int		get_active_cell();	

	virtual bool Invoke();
	virtual bool InvokeEdit() = 0;

};

/////////////////////////////////////////////////////////////////////////////
class CActionInsertCarioLine : public CActionCarioEditBase
{
	CActionInsertCarioLine(){}

	DECLARE_DYNCREATE(CActionInsertCarioLine)
	GUARD_DECLARE_OLECREATE(CActionInsertCarioLine)

public:
	virtual bool IsAvaible();
	virtual bool InvokeEdit();
};

/////////////////////////////////////////////////////////////////////////////
class CActionAppendCarioLine : public CActionCarioEditBase
{
	CActionAppendCarioLine(){}

	DECLARE_DYNCREATE(CActionAppendCarioLine)
	GUARD_DECLARE_OLECREATE(CActionAppendCarioLine)		

public:
	virtual bool IsAvaible();
	virtual bool InvokeEdit();
};


/////////////////////////////////////////////////////////////////////////////
class CActionInsertCarioCell : public CActionCarioEditBase
{
	CActionInsertCarioCell(){}

	DECLARE_DYNCREATE(CActionInsertCarioCell)
	GUARD_DECLARE_OLECREATE(CActionInsertCarioCell)		

public:
	virtual bool IsAvaible();
	virtual bool InvokeEdit();
};

/////////////////////////////////////////////////////////////////////////////
class CActionAppendCarioCell : public CActionCarioEditBase
{
	CActionAppendCarioCell(){}

	DECLARE_DYNCREATE(CActionAppendCarioCell)
	GUARD_DECLARE_OLECREATE(CActionAppendCarioCell)		

public:
	virtual bool IsAvaible();
	virtual bool InvokeEdit();
};

/////////////////////////////////////////////////////////////////////////////
class CActionDeleteCarioLine : public CActionCarioEditBase
{
	CActionDeleteCarioLine(){}

	DECLARE_DYNCREATE(CActionDeleteCarioLine)
	GUARD_DECLARE_OLECREATE(CActionDeleteCarioLine)		

public:
	virtual bool IsAvaible();
	virtual bool InvokeEdit();
};

/////////////////////////////////////////////////////////////////////////////
class CActionDeleteCarioCell : public CActionCarioEditBase
{
	CActionDeleteCarioCell(){}

	DECLARE_DYNCREATE(CActionDeleteCarioCell)
	GUARD_DECLARE_OLECREATE(CActionDeleteCarioCell)		

public:
	virtual bool IsAvaible();
	virtual bool InvokeEdit();
};

/////////////////////////////////////////////////////////////////////////////
class CActionDeleteChromosome : public CActionBase
{
	CActionDeleteChromosome(){}

	DECLARE_DYNCREATE(CActionDeleteChromosome)
	GUARD_DECLARE_OLECREATE(CActionDeleteChromosome)		

public:
	virtual bool IsAvaible();
	virtual bool Invoke();
};

/////////////////////////////////////////////////////////////////////////////
class CActionCarioBuild : public CActionBase
{
	DECLARE_DYNCREATE(CActionCarioBuild)
	GUARD_DECLARE_OLECREATE(CActionCarioBuild)

public:
	virtual bool Invoke();
	virtual bool IsAvaible();

	IUnknown*		get_object_list();
};

/////////////////////////////////////////////////////////////////////////////
class CActionShowChromoIdiogramm : public CActionBase
{
	DECLARE_DYNCREATE(CActionShowChromoIdiogramm)
	GUARD_DECLARE_OLECREATE(CActionShowChromoIdiogramm)
	ENABLE_UNDO();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

/////////////////////////////////////////////////////////////////////////////
class CActionShowAllChromoIdiogramm : public CActionBase
{
	DECLARE_DYNCREATE(CActionShowAllChromoIdiogramm)
	GUARD_DECLARE_OLECREATE(CActionShowAllChromoIdiogramm)
	ENABLE_UNDO();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();
protected:
	bool _next_state( bool bChange = true );
};


/////////////////////////////////////////////////////////////////////////////
struct _object_params
{
	IUnknownPtr punkObject;
	double fAngle, fZoom;
	CString UpdatePath;

	void operator=( _object_params &param )
	{
		fAngle = param.fAngle;
		punkObject = param.punkObject;
		fZoom = param.fZoom;
		UpdatePath = param.UpdatePath;
	}
};

/*
#include <vector>
template <class Type> class TUndoStack
{
	std::vector<Type*>m_UndoObject;
	std::vector<Type*>m_RedoObject;
	int m_nCurrentID;

public:
	TUndoStack()  { m_nCurrentID = -1; }
	~TUndoStack() {	empty(); }

	void add( Type *pNew, bool bUndo )  
	{
		if( m_nCurrentID != -1 && m_nCurrentID != m_UndoObject.size() - 1 )
			empty( m_nCurrentID );

		if( m_UndoObject.size() == m_nCurrentID || m_nCurrentID == -1 )
			m_nCurrentID++;

		if( bUndo )
			m_UndoObject.push_back( pNew );
		else
			m_RedoObject.push_back( pNew );
		
	}

	bool left( Type *lpVal )
	{
		if( m_nCurrentID < 0 )
			return false;

		if( !lpVal )
			return false;

		*lpVal = *m_UndoObject[m_nCurrentID--];
		return true;
	}

	bool right( Type *lpVal )
	{
		if( m_nCurrentID < m_RedoObject.size() )
			return false;

		if( !lpVal )
			return false;

		*lpVal = *m_RedoObject[++m_nCurrentID];
		return true;
	}
	void empty( int nFrom = -1, int nTo = -1 )
	{
		m_nCurrentID = -1;
		int nSz = m_UndoObject.size();

		if( nFrom == -1)
			nFrom = 0;

		if( nTo == -1)
			nTo = nSz;

		for( int i = nFrom; i < nTo; i++ )
		{	
			delete m_UndoObject[i];
			delete m_RedoObject[i];
		}

		m_UndoObject.erase( m_UndoObject.begin() + nFrom, m_UndoObject.begin() + nTo);
		m_RedoObject.erase( m_UndoObject.begin() + nFrom, m_UndoObject.begin() + nTo);
	}
};
*/
/////////////////////////////////////////////////////////////////////////////
class CActionSetChromoZoom : public CInteractiveActionBase
{
	DECLARE_DYNCREATE(CActionSetChromoZoom)
	GUARD_DECLARE_OLECREATE(CActionSetChromoZoom)
	ENABLE_UNDO();

	CActionSetChromoZoom();	
	_object_params m_paramOld, m_paramNew;
	BOOL m_bTrack;
public:

	virtual bool Initialize();	

	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );

	virtual void DoDraw( CDC &dc );

	virtual bool Invoke();
	virtual bool IsAvaible();	
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );

protected:
	CPoint	m_ptStart;
	CPoint	m_ptFinish;
	bool activate_chromosome( CPoint pt );

protected:

	COLORREF	m_clrLine;
	int			m_nArrowSize;
	int			m_nLineSize;
	CRect		get_rect();
	void		invalidate_rect( bool bUpdateWindow );
	void _fill_undo_redo( bool bUndo = true );
	void _undo( _object_params &param );
	bool DoRedo();
	bool DoUndo();
};

/////////////////////////////////////////////////////////////////////////////
class CActionSetChromoAngle : public CInteractiveActionBase
{
	DECLARE_DYNCREATE(CActionSetChromoAngle)
	GUARD_DECLARE_OLECREATE(CActionSetChromoAngle)
	ENABLE_UNDO();

	IUnknownPtr m_sptrList;
	CActionSetChromoAngle();
	virtual ~CActionSetChromoAngle();

//	TUndoStack<_object_params> m_UndoRedoStack;
	_object_params m_paramOld, m_paramNew;
	
public:
	virtual bool Initialize();	

	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoLButtonDown( CPoint pt );

	virtual void Terminate();

	//virtual void DoDraw( CDC &dc );	

	virtual bool Invoke();
	virtual bool IsAvaible();	
	
	bool activate_chromosome( CPoint pt );
	bool DoRedo();
	bool DoUndo();
protected:
	void _fill_undo_redo( bool bUndo = true );
	double	m_fNewAngle;
	double	get_chromo_angle( );
	bool	set_chromo_angle( double fAngle );
	bool	mirror_chromo( );

	CPoint	m_pointCenter;
	double	m_fAngleStart;
//	CRect	get_text_rect();
//	void	invalidate_text_rect( bool bUpdateWindow );
	void _undo( _object_params &param );

};

#include "chromosome.h"
class CActionSetChromoMirror : public CInteractiveActionBase
{
	bool m_bUndoState, m_bRedoState;

	IChromosomePtr m_sptrChromo;

	DECLARE_DYNCREATE(CActionSetChromoMirror)
	GUARD_DECLARE_OLECREATE(CActionSetChromoMirror)
	ENABLE_UNDO();

	IUnknownPtr m_sptrList;
	CActionSetChromoMirror();	
public:

	virtual bool Initialize();	

	virtual bool DoLButtonDown( CPoint pt );

	virtual bool Invoke();
	virtual bool IsAvaible();	
	bool DoRedo();
	bool DoUndo();

protected:
	bool activate_chromosome( CPoint pt );
};
/****************************************************************************/
/////////////////////////////////////////////////////////////////////////////
class CActionShowAllChromoBrightness : public CActionBase
{
	DECLARE_DYNCREATE(CActionShowAllChromoBrightness)
	GUARD_DECLARE_OLECREATE(CActionShowAllChromoBrightness)
public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();
protected:
	bool _next_state( bool bChange = true );
};

class CActionShowAllChromoRatio : public CActionBase
{
	DECLARE_DYNCREATE(CActionShowAllChromoRatio)
	GUARD_DECLARE_OLECREATE(CActionShowAllChromoRatio)
public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();
protected:
	bool _next_state( bool bChange = true );
};

class CActionShowAllChromoAmplification : public CActionBase
{
	DECLARE_DYNCREATE(CActionShowAllChromoAmplification)
	GUARD_DECLARE_OLECREATE(CActionShowAllChromoAmplification)
public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();
protected:
	bool _next_state( bool bChange = true );
};

class CActionShowAllChromoObjects : public CActionBase
{
	DECLARE_DYNCREATE( CActionShowAllChromoObjects )
	GUARD_DECLARE_OLECREATE( CActionShowAllChromoObjects )
public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();
protected:
	bool _next_state( bool bChange = true );
};

class CActionShowAllChromoAddLines : public CActionBase
{
	DECLARE_DYNCREATE( CActionShowAllChromoAddLines )
	GUARD_DECLARE_OLECREATE( CActionShowAllChromoAddLines )
public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();
protected:
	bool _next_state( bool bChange = true );
};

class CActionAddToAnalize : public CActionBase
{
	CNDataCell m_undoState, m_redoState;
	CString m_section;
	DECLARE_DYNCREATE(CActionAddToAnalize)
	GUARD_DECLARE_OLECREATE(CActionAddToAnalize)
	ENABLE_UNDO();
public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	bool DoRedo();
	bool DoUndo();
protected:
	CString _get_key( IUnknown *punk );
	bool _refresh_data( bool bUndo );
};

class CActionAddToAnalize2 : public CActionBase
{
	CNDataCell m_undoState, m_redoState;
	CString m_section;
	DECLARE_DYNCREATE(CActionAddToAnalize2)
	GUARD_DECLARE_OLECREATE(CActionAddToAnalize2)
	ENABLE_UNDO();
public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	bool DoRedo();
	bool DoUndo();
protected:
	CString _get_key( IUnknown *punk );
	bool _refresh_data( bool bUndo );
};

class CActionAddToAnalize3 : public CActionBase
{
	CNDataCell m_undoState, m_redoState;
	CString m_section;
	DECLARE_DYNCREATE(CActionAddToAnalize3)
	GUARD_DECLARE_OLECREATE(CActionAddToAnalize3)
	ENABLE_UNDO();
public:
	virtual bool Invoke();
	bool DoRedo();
	bool DoUndo();
protected:
	CString _get_key( IUnknown *punk );
	bool _refresh_data( bool bUndo );
};


class CActionSetChromoClass : public CActionBase
{
	DECLARE_DYNCREATE(CActionSetChromoClass)
	GUARD_DECLARE_OLECREATE(CActionSetChromoClass)
	ENABLE_UNDO();

	CString m_sOldPath, m_sNewPath;
	int n_OldClass, n_NewClass;
	IUnknownPtr m_sptrList;

public:
	virtual bool Invoke();
	bool DoRedo();
	bool DoUndo();
	virtual bool IsAvaible();
protected:
	void _swap_section(CString strFrom, CString strTo, int nClass);
};

class CActionSetEditableIdio : public CActionBase
{
	DECLARE_DYNCREATE(CActionSetEditableIdio)
	GUARD_DECLARE_OLECREATE(CActionSetEditableIdio)
public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionRotateIdio : public CActionBase
{
	DECLARE_DYNCREATE(CActionRotateIdio)
	GUARD_DECLARE_OLECREATE(CActionRotateIdio)
public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHROMOACTIONS_H__387595A5_2E70_440B_A7FA_24DE4891CB97__INCLUDED_)
