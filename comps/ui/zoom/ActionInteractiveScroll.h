// ActionInteractiveScroll.h: interface for the CActionInteractiveScroll class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIONINTERACTIVESCROLL_H__D6AB2ADF_A502_4A6D_AA3F_105DE8F0D11E__INCLUDED_)
#define AFX_ACTIONINTERACTIVESCROLL_H__D6AB2ADF_A502_4A6D_AA3F_105DE8F0D11E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CActionInteractiveScroll : public CInteractiveActionBase  
{
	DECLARE_DYNCREATE(CActionInteractiveScroll)
	GUARD_DECLARE_OLECREATE(CActionInteractiveScroll)
	
	CPoint m_ptStart;
	POINT m_StartScrollPos;
	bool m_bMPressed;
	bool m_bInvertScrollVertDirection, m_bInvertScrollHorzDirection; 
	HCURSOR m_hcurCatch, m_hcurFree, m_hcurOld;
public:
	CActionInteractiveScroll();
	virtual ~CActionInteractiveScroll();

	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoMouseMove( CPoint pt );
	virtual bool DoRButtonDown( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	virtual bool Initialize();
	virtual bool IsAvaible();
	virtual void Finalize();
protected:
	void _scroll_delta( CPoint ptDelta );
	void _activate_object( CPoint pt );
	void _get_img_rect(IUnknown *pImage,RECT &rc);

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	CPoint _convert_point( CPoint pt );
};

#endif // !defined(AFX_ACTIONINTERACTIVESCROLL_H__D6AB2ADF_A502_4A6D_AA3F_105DE8F0D11E__INCLUDED_)
