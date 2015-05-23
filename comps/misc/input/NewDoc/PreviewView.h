#if !defined(__PreviewView_H__)
#define __PreviewView_H__

#include "Input.h"
#include "PropBagImpl.h"

//sergey 04/10/06
#include "Reposition.h"
//end

class CIPreviewView;
class CDrawBase
{
public:
	virtual ~CDrawBase() {};
	virtual bool OnCreate(CIPreviewView *pView) {return true;};
	virtual void OnDestroy(CIPreviewView *pView) {};
	virtual void OnPaint(CIPreviewView *pView, CDC *pDC) {};
	virtual void OnTimer(CIPreviewView *pView, UINT idTimer) {};
	virtual void OnSize(CIPreviewView *pView) {}
};

class CDrawPreview : public CDrawBase, public IInputPreviewSite
{
	
	sptrIInputPreview m_sptrIP;
	sptrIInputPreview2 m_sptrIP2;
	CIPreviewView *m_pView;
	int m_nDev;
	DWORD m_dwRefCount;
	bool m_bDrawPeriod;

	// function for work with both interfaces, IInputPreview and IInputPreview2
	bool  AnyInterface() {return m_sptrIP!= 0 || m_sptrIP2 != 0;}
	bool  BeginPreview();
	void  EndPreview();
	DWORD GetFreq();
	bool  GetSize(CSize &sz);
	void  DrawRect(CDC *pDC, CRect rcSrc, CRect rcDst);
public:
	//sergey 04/10/06
	IInputPreview3Ptr m_sptrPrv;
	CSize m_szPrv;
	void WindowToImage(CPoint &pt,CRect rcClient1);
	void DoLButtonDown(UINT nFlags, CPoint point,CRect rcClient1);
	void DoLButtonUp(UINT nFlags, CPoint point,CRect rcClient1);
	void DoMouseMove(UINT nFlags, CPoint point,CRect rcClient1);
	bool DoSetCursor(UINT nFlags, CPoint point,CRect rcClient1);
	//end
	CDrawPreview();
	virtual ~CDrawPreview();
	virtual bool OnCreate(CIPreviewView *pView);
	virtual void OnDestroy(CIPreviewView *pView);
	virtual void OnPaint(CIPreviewView *pView, CDC *pDC);
	virtual void OnTimer(CIPreviewView *pView, UINT idTimer);

	STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj) {return S_FALSE;}
	STDMETHOD_(ULONG,AddRef)() {return 0;}
	STDMETHOD_(ULONG,Release)() {return 0;}
	com_call Invalidate();
	com_call OnChangeSize();
};

class CDrawWnd : public CDrawBase
{
	sptrIInputWindow m_sptrIW;
	long m_hwnd;
public:
	CDrawWnd();
	virtual ~CDrawWnd();
	virtual bool OnCreate(CIPreviewView *pView);
	virtual void OnDestroy(CIPreviewView *pView);
	virtual void OnSize(CIPreviewView *pView);
};

/////////////////////////////////////////////////////////////////////////////
// CIPreviewView window

class CIPreviewView : public CViewBase, public CNamedPropBagImpl,public CRepositionManager//,public CDrawWnd//sergey 04/10/06
{
	ENABLE_MULTYINTERFACE();
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CIPreviewView)

	CDrawBase *m_pDraw;
	//sergey 04/10/06
	CDrawPreview	m_Image;
	//end
// Construction
public:
	CIPreviewView();

// Attributes
public:

// Operations
public:
	bool InitPreview();
	void DeinitPreview();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPreviewView)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIPreviewView();

	virtual LPUNKNOWN GetInterfaceHook(const void* p)
	{
		if (*(IID*)p == IID_INamedPropBag || *(IID*)p == IID_INamedPropBagSer)
			return (INamedPropBagSer*)this;
		else return __super::GetInterfaceHook(p);
	}

	// Generated message map functions
protected:
	//{{AFX_MSG(CIPreviewView)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();

	//serey 04/10/06
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//end
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CIPreviewView)
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CIPreviewView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()


	POSITION GetFisrtVisibleObjectPosition() {return 0;}
	IUnknown * GetNextVisibleObject( POSITION &rPos ) {return NULL;}
};

/////////////////////////////////////////////////////////////////////////////









#endif


