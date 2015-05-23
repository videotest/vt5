#if !defined(AFX_CARIOOBJ_H__A39C893B_4221_4E98_A182_F94100553E85__INCLUDED_)
#define AFX_CARIOOBJ_H__A39C893B_4221_4E98_A182_F94100553E85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CarioObj.h : header file
//

#include "carioimpl.h"

/////////////////////////////////////////////////////////////////////////////
// CCarioObj command target

/*
class CCarioObj :	public CDataObjectBase,
					public CDrawObjectImpl,
					public CCarioImpl
{
	DECLARE_DYNCREATE(CCarioObj)
	PROVIDE_GUID_INFO()
	GUARD_DECLARE_OLECREATE(CCarioObj)
	DECLARE_INTERFACE_MAP()

	ENABLE_MULTYINTERFACE();

	CCarioObj();           // protected constructor used by dynamic creation

	virtual void DoDraw( CDC &dc );
	virtual DWORD GetNamedObjectFlags();
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );

	BEGIN_INTERFACE_PART(Rect, IRectObject)
		com_call SetRect(RECT rc);
		com_call GetRect(RECT* prc);
		com_call HitTest( POINT	point, long *plHitTest );
		com_call Move( POINT point );
	END_INTERFACE_PART(Rect)

	BEGIN_INTERFACE_PART(MsgListener, IMsgListener)
		com_call OnMessage( MSG *pmsg, LRESULT *plReturn );
	END_INTERFACE_PART(MsgListener)
	

	inline virtual CarioMode	GetCarioMode();
	virtual IUnknownPtr			GetDataContextPtr();
	virtual IUnknownPtr			GetViewPtr();
	virtual HWND				GetHwnd();

	virtual CPoint				GetOffset();
	virtual CSize				GetSize();	
	virtual bool				CanSetClientSize();

	CRect						m_rectObject;
	bool						process_message( MSG *pmsg, LRESULT *plReturn );


// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCarioObj)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCarioObj();

	// Generated message map functions
	//{{AFX_MSG(CCarioObj)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CCarioObj)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()	
};
*/
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CARIOOBJ_H__A39C893B_4221_4E98_A182_F94100553E85__INCLUDED_)
