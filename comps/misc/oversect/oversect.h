// oversect.h : main header file for the OVERSECT DLL
//

#if !defined(AFX_OVERSECT_H__1F053454_8578_4A09_A85C_85899C773107__INCLUDED_)
#define AFX_OVERSECT_H__1F053454_8578_4A09_A85C_85899C773107__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// COversectApp
// See oversect.cpp for the implementation of this class
//

class COversectApp : public CWinApp
{
public:
	COversectApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COversectApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(COversectApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


interface IOverlaySection : public IUnknown
{
	com_call SetPoints( POINT point1, POINT point2 ) = 0;
	com_call GetPoints( POINT *ppoint1, POINT *ppoint2 ) = 0;
	com_call SetDirection( DWORD dwDir ) = 0;	//0 - free, 1 - horz, 2 - vert
};

declare_interface(IOverlaySection, "FC42CAAB-BE64-4255-89E8-C4F184EF3FF0")

/////
class COverlaySection : public CDataObjectBase, public CDrawObjectImpl
{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(COverlaySection);
	GUARD_DECLARE_OLECREATE(COverlaySection);
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();
public:
	COverlaySection();
	virtual ~COverlaySection();

	DWORD GetNamedObjectFlags(){	return nofHasData|nofStoreByParent;}
	bool	SerializeObject( CStreamEx &ar, SerializeParams *pparams );
	void	DoDraw( CDC &dc );
	void	AttachImage();


	BEGIN_INTERFACE_PART(OvrSect, IOverlaySection)
		com_call SetPoints( POINT point1, POINT point2 );
		com_call GetPoints( POINT *ppoint1, POINT *ppoint2 );
		com_call SetDirection( DWORD dwDir );
	END_INTERFACE_PART(OvrSect)
	BEGIN_INTERFACE_PART(Rect, IRectObject)
		com_call SetRect(RECT rc);
		com_call GetRect(RECT* prc);
		com_call HitTest( POINT	point, long *plHitTest );
		com_call Move( POINT point );
	END_INTERFACE_PART(Rect)

	CPoint		m_point1;
	CPoint		m_point2;
	CImageWrp	m_image;
	CRect		m_rect;
	DWORD		m_dwDir;
	CSize		m_sizeImage;
};

#define szTypeOverlaySection	"OverlaySection"
/////////////////////////////////////////////////////////////////////////////

//[ag]1. classes

class CActionOverlaySectionFree : public CInteractiveActionBase
{
	DECLARE_DYNCREATE(CActionOverlaySectionFree)
	GUARD_DECLARE_OLECREATE(CActionOverlaySectionFree)

ENABLE_UNDO();
public:
	CActionOverlaySectionFree();
	virtual ~CActionOverlaySectionFree();

public:
	virtual bool Invoke();
	virtual bool Initialize();

	virtual void DoDraw( CDC &dc );
	virtual bool DoStartTracking( CPoint point );
	virtual bool DoTrack( CPoint point );
	virtual bool DoFinishTracking( CPoint point );
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();

	virtual void AjustPointLocations( bool bFirstPoint );
	virtual void Invalidate();
	virtual bool IsAvaible();
protected:
	UINT	m_nIDCursor;

	IOverlaySectionPtr	m_ptrSection;
	IDrawObjectPtr		m_ptrSectionDraw;
	CSize				m_sizeImage;

	CObjectListUndoRecord	m_changes;

	CPoint	m_point1, m_point2;
	DWORD	m_dwDir;
};

class CActionOverlaySectionVert : public CActionOverlaySectionFree
{
	DECLARE_DYNCREATE(CActionOverlaySectionVert)
	GUARD_DECLARE_OLECREATE(CActionOverlaySectionVert)
public:
	CActionOverlaySectionVert()
	{		m_nIDCursor = IDC_SECTION_VERT;m_dwDir=2;	}
	virtual void AjustPointLocations( bool bFirstPoint );
};

class CActionOverlaySectionHorz : public CActionOverlaySectionFree
{
	DECLARE_DYNCREATE(CActionOverlaySectionHorz)
	GUARD_DECLARE_OLECREATE(CActionOverlaySectionHorz)
public:
	CActionOverlaySectionHorz()
	{		m_nIDCursor = IDC_SECTION_HORZ;	m_dwDir = 1; }
	virtual void AjustPointLocations( bool bFirstPoint );
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OVERSECT_H__1F053454_8578_4A09_A85C_85899C773107__INCLUDED_)
