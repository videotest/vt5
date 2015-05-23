#pragma once

#include "galleryviewbase.h"
#include "carioint.h"
#include "idiodbase_int.h"
/*
class CGalleryIdioItem : public GallerySpace::CGalleryItemBase
{
	IUnknownPtr m_sptrIdio;
	long m_lClass; 
public:
	CGalleryIdioItem( IUnknownPtr sptr )
	{
		m_sz.cx = ::GetValueInt( ::GetAppUnknown(), CARIO_VIEW_SHARED, KARYO_IDIOWIDTH, 20 );
		m_sz.cy = ::GetValueInt( ::GetAppUnknown(), CARIO_VIEW_SHARED, KARYO_IDIOHEIGHT, 45 );

		m_sptrIdio = sptr;

		m_lClass = -1;
	}
	IUnknownPtr GetObject() { return m_sptrIdio; }
	void DoDraw( CDC *pDc, bool bSelected, CWnd *pWnd );

	void SetClass( long lClass ) { m_lClass = lClass; }
	long GetClass() { return m_lClass; }

	void Destroy()
	{
		delete this;
	}
};

class CIdioDBView : public GallerySpace::CGalleryViewBase
{
//	IIdioDBasePtr m_sptrDB;
	GUARD_DECLARE_OLECREATE(CIdioDBView)
	DECLARE_DYNCREATE(CIdioDBView)
	PROVIDE_GUID_INFO( )
	ENABLE_MULTYINTERFACE();
	int m_nIdioWidth, m_nIdioHeight;
	bool m_bDrag;

public:
	CIdioDBView();
// Operations
public:
	//{{AFX_VIRTUAL(CIdioDBView)
	public:
	virtual void OnFinalRelease();
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIdioDBView();
	virtual DWORD GetMatchType( const char *szType );
	void DrawCell( CDC *pDc, const CRect &rc );
	void OnEditCtrlChange(CString str);
	virtual IUnknown *GetObjectByPoint( CPoint point );
	virtual void DoDrawFrameObjects( IUnknown *punkObject, CDC &dc, CRect rectInvalidate, BITMAPINFOHEADER *pbih, byte *pdibBits, ObjectDrawState state );
protected:
	virtual POSITION GetFisrtVisibleObjectPosition();
	virtual IUnknown * GetNextVisibleObject( POSITION &rPos );
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam );

	//{{AFX_MSG(CIdioDBView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CIdioDBView)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	virtual CWnd *GetWindow(){return this;}
	virtual void OnActivateObject( IUnknown *punkDataObject );
	virtual void set_layout();
	void attach_data();
	void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
protected:
	void text_layout(void);
};
*/