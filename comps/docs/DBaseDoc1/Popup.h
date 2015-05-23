#ifndef POPUP_H
#define POPUP_H


interface IPopupCtrlEvent;
interface IPopupCtrlReciever;


///////////////////////////////////////////////////////////////////////////////
class CPopup : public CMiniFrameWnd
{
public:
    // Return value of OnKeyDown, OnLButtonDown and OnLButtonUp callbacks
    enum
    {
        doNothing = 0,  // No action
        noSend    = 1,  // Do not send the current message to the control
        end       = 2,  // Hide popup normally
        abort     = 3,  // Hide popup with flag 'abort'
    };

	void ChildWannaDead();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPopup)
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL

    bool Display (CWnd* pControl, CWnd* pCaller, CRect& rcCaller, IPopupCtrlEvent* pEvents,
					IPopupCtrlReciever* pReciever );    

	bool NotifyReciever( const char* szEvent, CString strValue );

// Implementation
protected:
    CWnd* m_pCaller;
    CWnd* m_pControl;
    IPopupCtrlEvent* m_pEvents;	
    HWND m_hPrevParentCtrl;
    bool m_bDestroy;

protected:
    CPopup ();
    virtual ~CPopup () {};

    void EndPopup (bool bAbort = false, bool bSetFocus = true);

    // Generated message map functions
protected:
    //{{AFX_MSG(CPopup)
    afx_msg void OnActivate(UINT, CWnd*, BOOL);
    afx_msg void OnCancelMode();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
    DECLARE_DYNCREATE(CPopup)
};


///////////////////////////////////////////////////////////////////////////////
// {600029F0-14EE-11D3-941C-00105A66E1E2}
DEFINE_GUID(IID_IPopupCtrlEvent, 0x600029F0, 0x14EE, 0x11D3, 0x94, 0x1C, 0x00, 0x10, 0x5A, 0x66, 0xE1, 0xE2);

// {9B5C8F15-7603-4d33-A564-DECAB1C79894}
DEFINE_GUID(IID_IPopupCtrlReciever, 0x9b5c8f15, 0x7603, 0x4d33, 0xa5, 0x64, 0xde, 0xca, 0xb1, 0xc7, 0x98, 0x94);


interface IPopupCtrlEvent
{
    virtual bool OnInit () = 0;
    virtual void OnShow () = 0;
    virtual void OnHide (bool bAbort) = 0;
    virtual int OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags) = 0;
    virtual int OnLButtonDown (UINT nFlags, CPoint point) = 0;
    virtual int OnLButtonUp (UINT nFlags, CPoint point) = 0;
    virtual IPopupCtrlEvent* GetInterfaceOf (HWND hWnd) = 0;
	virtual void SetReciever (IPopupCtrlReciever* pReciever) = 0;
};


interface IPopupCtrlReciever
{
    virtual bool OnNotify( const char* szEvent,  CString strValue ) = 0;
	virtual bool OnNotify( const char* szEvent,  UINT nValue ) = 0;
};

/////////////////////////////////////////////////////////////////////////////
#endif // POPUP_H
