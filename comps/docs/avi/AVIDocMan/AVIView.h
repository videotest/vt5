#if !defined(__AVIView_H__)
#define __AVIView_H__

#include "Common.h"
#include "ActiveMovieWnd.h"
#include "VT5AVI.h"
#include "VTAviInfo.h"
#include "AVIFileCapture.h"

#define ID_AMCONTROL1             1


class CAVIView : public CViewBase
//				public CPreviewSiteImpl////, public CMouseImpl
{
	ENABLE_MULTYINTERFACE();
	DECLARE_DYNCREATE(CAVIView)
	PROVIDE_GUID_INFO( )

	CActiveMovieWnd m_wndAM;
	// For capture using AVIFile
	CAVIFileCapture m_AVIFileCap;
	// Supplementary information when captured using VT4 or VT5
	VTVideoInfo m_VideoInfo; // Will be filled by nulls if no such info.
	BOOL m_bPauseThenOpened; // Pause afeter opening document
	GuidKey m_lImageDocKey; // Document, in which to store captured images
	BOOL m_bImageDocKeyInited; // m_lImageDocKey member is valid
	bool m_bScheduleNext;
	BOOL m_bEnableErrorMsg;
	CTypedPtrList<CPtrList, IUnknown*>m_listObjects;
	void ClearList();
// Construction
public:
	CAVIView();

// Attributes
public:

// Operations
public:
	virtual void AttachActiveObjects();	//attach the active image
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAVIView)
	public:
	virtual void OnFinalRelease();
	////virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAVIView();
	
//	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, const _variant_t var );
	// Generated message map functions
protected:
	//{{AFX_MSG(CAVIView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnOpenCompleteActivemoviecontrol1();
	afx_msg void OnPositionChangeActivemoviecontrol1(double oldPosition, double newPosition);
	afx_msg void OnStateChangeActivemoviecontrol1(long oldState, long newState);
	afx_msg void OnDisplayModeChangeActivemoviecontrol1();
	afx_msg void OnErrorActivemoviecontrol1(short SCode, LPCTSTR Description, LPCTSTR Source, BOOL FAR* CancelDisplay);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CAVIView)
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAVIView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(VideoView, IVideoView)
		com_call FirstFrame();
		com_call IsFirstFrame(BOOL *pbFirstFrame);
		com_call PreviousFrame();
		com_call NextFrame();
		com_call LastFrame();
		com_call IsLastFrame(BOOL *pbLastFrame);
		com_call Capture(IUnknown *punkImage, bool bStepRight);
		com_call SetImageDocKey(GUID lDocKey);
		com_call GetImageDocKey(GUID *plDocKey, BOOL *pbDocKeyInited);
	END_INTERFACE_PART(VideoView)


	DWORD GetMatchType( const char *szType );
	virtual POSITION GetFisrtVisibleObjectPosition();
	virtual IUnknown * GetNextVisibleObject( POSITION &rPos );

	////virtual IUnknown *GetTarget(){return GetControllingUnknown();}
	//virtual bool DoStartTracking( CPoint pt );
	//virtual bool DoTrack( CPoint pt );
	//virtual bool DoFinishTracking( CPoint pt );
	//virtual bool DoLButtonUp( CPoint pt );
	////virtual bool CanStartTracking( CPoint pt );

};


#endif
