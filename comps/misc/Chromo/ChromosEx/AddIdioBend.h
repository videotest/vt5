#pragma once

#include "idio_int.h"
#include "\vt5\awin\misc_list.h"
#include "carioimpl.h"

class CIdioEditUndoRedoImpl
{
	struct XInfo
	{
		IUnknownPtr m_sptrParent;
		double *m_pPos, *m_pPos2;
		int *m_pcolors, *m_pflags;
		_bstr_t *m_pbstr_number;
		long m_lCount;
		bool bFilled;

		double *m_pLength, *m_pCentr;

		XInfo()
		{
			m_pPos = m_pPos2 = 0;
			m_pcolors = m_pflags = 0;
			m_pbstr_number = 0;	
			m_lCount = 0;
			m_pLength = m_pCentr = 0;
			bFilled = false;
		}
		~XInfo()
		{
			if( m_pPos )
				delete []m_pPos;
			if( m_pPos2 )
				delete []m_pPos2;
			if( m_pcolors )
				delete []m_pcolors;
			if( m_pflags )
				delete []m_pflags;
			if( m_pbstr_number )
				delete []m_pbstr_number;

			if( m_pLength )
				delete []m_pLength;

			if( m_pCentr )
				delete []m_pCentr;
		};
	};
	_list_t<XInfo *> m_listUndoInfo;
	_list_t<XInfo *> m_listRedoInfo;
public:
	virtual ~CIdioEditUndoRedoImpl()
	{
		for( TPOS lPos = m_listUndoInfo.head(); lPos; lPos = m_listUndoInfo.next( lPos ) )
		{
			XInfo *p = m_listUndoInfo.get( lPos );
			if( p )
				delete p;
		}
		m_listUndoInfo.clear();

		for (TPOS lPos = m_listRedoInfo.head(); lPos; lPos = m_listRedoInfo.next(lPos))
		{
			XInfo *p = m_listRedoInfo.get( lPos );
			if( p )
				delete p;
		}
		m_listRedoInfo.clear();
	}
	void FillUndoInfo( IUnknown *punkPar, IUnknown *punkIdio )
	{
		XInfo *m_pInfo = new XInfo;
		m_pInfo->m_sptrParent = punkPar;

		_fill_info( m_pInfo, punkIdio );
		m_listUndoInfo.insert_after( m_pInfo );
	}
	void FillRedoInfo( IUnknown *punkPar, IUnknown *punkIdio )
	{
		XInfo *m_pInfo = new XInfo;
		m_pInfo->m_sptrParent = punkPar;

		_fill_info( m_pInfo, punkIdio );
		m_listRedoInfo.insert_after( m_pInfo );
	}
private:
	void _fill_info( XInfo *pInfo, IUnknown *punkIdio )
	{
		if( pInfo->bFilled )
			return;

		pInfo->bFilled = true;
		IIdioBendsPtr ptrBends = punkIdio;

		if( ptrBends != 0 )
		{
			long lCount = 0;
			ptrBends->GetBengsCount( &lCount );

			if( lCount > 0 )
			{
				pInfo->m_lCount = lCount;
				pInfo->m_pPos		 = new double[lCount];
				pInfo->m_pPos2		 = new double[lCount];
				pInfo->m_pcolors		 = new int[lCount];
				pInfo->m_pflags		 = new int[lCount];
				pInfo->m_pbstr_number = new _bstr_t[lCount];

				for( int i = 0; i < lCount; i++ )
					ptrBends->GetBendParams( i, &pInfo->m_pPos[i], &pInfo->m_pPos2[i], &pInfo->m_pcolors[i], pInfo->m_pbstr_number[i].GetAddress(), &pInfo->m_pflags[i] );
			}
			
			IIdiogramPtr sptrIdio = ptrBends;

			pInfo->m_pLength = new double;
			pInfo->m_pCentr = new double;

			sptrIdio->get_params( pInfo->m_pLength, pInfo->m_pCentr );

		}
	}
	IUnknownPtr get_idio( XInfo *pInfo )
	{
		IMeasureObjectPtr ptrMeasure( pInfo->m_sptrParent );
		if( ptrMeasure == 0 )	return 0;

		INamedDataObject2Ptr ptrNDO( ptrMeasure );
		if( ptrNDO == 0 )		return 0;

		POSITION lPosCromo = 0;
		ptrNDO->GetFirstChildPosition( &lPosCromo );
		while( lPosCromo )
		{
			IUnknown* punkChromo = 0;
			ptrNDO->GetNextChild( &lPosCromo, &punkChromo );
			if( !punkChromo )
				continue;

			IIdiogramPtr ptr_idio( punkChromo );
			punkChromo->Release();	punkChromo = 0;

			if( ptr_idio != 0 )
			{
				unsigned uFlag = 0;
				ptr_idio->get_flags( &uFlag );

				if( uFlag == IDIT_EDITABLE )
					return ptr_idio;
			}
		}
		return 0;
	}
	void _make_undo_redo( XInfo *pInfo )
	{
		IIdioBendsPtr ptrBend = get_idio( pInfo );
		
		if( ptrBend != 0 )
		{
			ptrBend->Remove( -1 );
			for( int i = 0; i < pInfo->m_lCount; i++ )
				ptrBend->Add( pInfo->m_pPos[i], pInfo->m_pPos2[i], pInfo->m_pcolors[i], pInfo->m_pbstr_number[i], pInfo->m_pflags[i] );
		}

		IIdiogramPtr sptrIdio = ptrBend;
		sptrIdio->set_params( *pInfo->m_pLength, *pInfo->m_pCentr );

	}
protected:
	void do_undo() 
	{ 
		for (TPOS lPos = m_listUndoInfo.head(); lPos; lPos = m_listUndoInfo.next(lPos))
		{
			XInfo *p = m_listUndoInfo.get( lPos );
			_make_undo_redo( p ); 
		}
	}
	void do_redo() 
	{ 
		for (TPOS lPos = m_listRedoInfo.head(); lPos; lPos = m_listRedoInfo.next(lPos))
		{
			XInfo *p = m_listRedoInfo.get( lPos );
			_make_undo_redo( p ); 
		}
	}
};

class CActionIdioEditBase : public CInteractiveActionBase
{
public:
	CActionIdioEditBase()
	{
		m_fZoom = 1.0;
		m_bPopUpMenu = false;
	}
protected:
	CPoint _correct( CPoint pt );
	CRect  _correct( CRect pt );
	void   _get_zoom();
	bool   _activate_chromosome( CPoint pt );
	void _correct_numbers( IIdioBendsPtr sptrBend, CString strNew, CString strPrev, int nMain, bool bSkipTargetBend = false );
	CString _incr_change( CString str, int nPart, int nDelta );
	void _get_parts( CString str, CString &sA, CString &sB, CString &sC, CString &sD );
	CString  _set_parts( CString sA, CString sB, CString sC, CString sD );
protected:
	double m_fZoom;
	bool m_bPopUpMenu;
	POINT m_ptScroll;
	CPoint m_ptFirst, m_ptSecond, m_ptMice;
};

class CActionAddIdioBend : public CIdioEditUndoRedoImpl, public CActionIdioEditBase
{
	HCURSOR m_hCursor, m_hcurOld, m_hDelZoneCursor;
	bool m_bLBtnDown, m_bDelZone, m_bSputnic;
	int m_nOffset;
	double m_fHotZone;
	CMenu *m_pMenu;
	CMenu *m_pMenu2;
	CMenu m_mnuMain;
	CMenu m_mnuMain2;

	bool m_bSkipInvoke;

	CRect m_rcOld;

	DECLARE_DYNCREATE(CActionAddIdioBend)
	GUARD_DECLARE_OLECREATE(CActionAddIdioBend)
	ENABLE_UNDO();

	CActionAddIdioBend(void);
	~CActionAddIdioBend(void);
public:
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	virtual bool DoRButtonDown( CPoint pt );
	virtual bool DoRButtonUp( CPoint pt );
	virtual bool DoMouseMove(CPoint point);
	virtual BOOL OnListenMessage( MSG * pmsg, LRESULT *plResult );

	virtual bool Initialize();	
	virtual bool Invoke();
	virtual bool IsAvaible();	

	bool DoRedo();
	bool DoUndo();

	void DoDraw(CDC &dc);
	static double _to_chromo( IUnknown *punkTarget, int nLine, int nCell, int nChromo, double x, double y, double *pfLen = 0, double *pY = 0, FPoint *pPt1 = 0, FPoint *pPt2 = 0, int *pnIndex = 0 );
};

class CActionAddIdioBend2 : public CActionBase, public CIdioEditUndoRedoImpl
{
	DECLARE_DYNCREATE(CActionAddIdioBend2)
	GUARD_DECLARE_OLECREATE(CActionAddIdioBend2)
	ENABLE_UNDO();

	CActionAddIdioBend2(void);
	~CActionAddIdioBend2(void);
public:
	virtual bool Invoke();
	bool DoRedo();
	bool DoUndo();
};

#include <vector>
#include "\vt5\awin\win_dlg.h"
class CActionFillAndNumberIdioBend : public CIdioEditUndoRedoImpl, public CActionIdioEditBase
{
	class XColorMenu : public CMenu  
	{
		std::vector<CBitmap*>m_cBitmaps;
		std::vector<HBITMAP>m_hBitmaps;
	public:
		XColorMenu();
		virtual ~XColorMenu();
		void CreateMenu( IUnknown *punkTarget );
		void Destroy();
	protected:
		HBITMAP _make_icon( HBRUSH hBrush );
	} *m_pMenu;

	bool m_bPopUpMenu;
	DECLARE_DYNCREATE( CActionFillAndNumberIdioBend )
	GUARD_DECLARE_OLECREATE( CActionFillAndNumberIdioBend )
	ENABLE_UNDO();

	CActionFillAndNumberIdioBend();
	~CActionFillAndNumberIdioBend();
public:
	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoRButtonUp( CPoint pt );

	virtual BOOL OnListenMessage( MSG * pmsg, LRESULT *plResult );
	virtual bool Initialize();	

	virtual bool Invoke();
	virtual bool IsAvaible();	

	bool DoRedo();
	bool DoUndo();
protected:
};
