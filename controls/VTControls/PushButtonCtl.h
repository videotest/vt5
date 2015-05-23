#if !defined(AFX_PUSHBUTTONCTL_H__54A0BF93_9A92_11D3_B1C5_EB19BDBABA39__INCLUDED_)
#define AFX_PUSHBUTTONCTL_H__54A0BF93_9A92_11D3_B1C5_EB19BDBABA39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Dib.h"
#include <afxcmn.h>
#include "\vt5\comps\misc\glib\glib.h"

// PushButtonCtl.h : Declaration of the CPushButtonCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CPushButtonCtrl : See PushButtonCtl.cpp for implementation.

class CPushButtonCtrl : public COleControl
{
	DECLARE_DYNCREATE(CPushButtonCtrl)

#ifndef FOR_HOME_WORK
	DECLARE_INTERFACE_MAP();
#endif


// Constructor
public:
	CPushButtonCtrl();	

#ifndef FOR_HOME_WORK
	BEGIN_INTERFACE_PART(ActiveXCtrl, IVtActiveXCtrl)
		com_call SetSite( IUnknown *punkVtApp, IUnknown *punkSite );
		com_call GetName( BSTR *pbstrName );	//such as Editor1
	END_INTERFACE_PART(ActiveXCtrl)
#endif  


#ifndef FOR_HOME_WORK
protected:
	IUnknownPtr	m_ptrSite;
	IUnknownPtr	m_ptrApp;
#endif
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPushButtonCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();
	virtual void Serialize(CArchive& ar);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CPushButtonCtrl();

	DECLARE_OLECREATE_EX(CPushButtonCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CPushButtonCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CPushButtonCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CPushButtonCtrl)		// Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnOcmDrawItem( WPARAM wParam, LPARAM lParam );
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	CString m_BMPFileName;
	
	//CFontHolder m_fontHolder;
	//afx_msg void OnBMPFileNameChanged();

	 
	


// Message maps
	//{{AFX_MSG(CPushButtonCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		   
// Dispatch maps
	//{{AFX_DISPATCH(CPushButtonCtrl)	
	short m_buttonWidth;
	afx_msg void OnButtonWidthChanged();
	short m_buttonHeight;
	afx_msg void OnButtonHeightChanged();
	BOOL m_autoSize;
	afx_msg void OnAutoSizeChanged();
	short m_buttonDepth;
	afx_msg void OnButtonDepthChanged();
	BOOL m_drawBorder;
	afx_msg void OnDrawBorderChanged();
	BOOL m_flatButton;
	afx_msg void OnFlatButtonChanged();
	BOOL m_defaultButton;
	afx_msg void OnDefaultButtonChanged();
	BOOL m_threeStateButton;
	afx_msg void OnThreeStateButtonChanged();
	BOOL m_mouseInPicColor;
	afx_msg void OnMouseInPicColorChanged();
	BOOL m_mouseOutPicColor;
	afx_msg void OnMouseOutPicColorChanged();
	BOOL m_mouseOutShadow;
	afx_msg void OnMouseOutShadowChanged();
	BOOL m_buttonPressedPicColor;
	afx_msg void OnButtonPressedPicColorChanged();
	BOOL m_buttonPressedShadow;
	afx_msg void OnButtonPressedShadowChanged();
	BOOL m_mouseInShadow;
	afx_msg void OnMouseInShadowChanged();
	BOOL m_drawFocusRect;
	afx_msg void OnDrawFocusRectChanged();
	BOOL m_stretch;
	afx_msg void OnStretchChanged();
	CString m_buttonText;
	afx_msg void OnButtonTextChanged();
	short m_textAlign;
	afx_msg void OnTextAlignChanged();
	short m_offsetTextFromBMP;
	afx_msg void OnOffsetTextFromBMPChanged();
	BOOL m_drawFigures;
	afx_msg void OnDrawFiguresChanged();
	short m_figureType;
	afx_msg void OnFigureTypeChanged();
	short m_figureSize;
	afx_msg void OnFigureSizeChanged();
	OLE_COLOR m_colorTransparent;
	afx_msg void OnColorTransparentChanged();
	OLE_COLOR m_colorFigure;
	afx_msg void OnColorFigureChanged();
	OLE_COLOR m_colorFillFigure;
	afx_msg void OnColorFillFigureChanged();
	BOOL m_fillFigure;
	afx_msg void OnFillFigureChanged();
	OLE_COLOR m_activeTextColor;
	afx_msg void OnActiveTextColorChanged();
	OLE_COLOR m_inactiveTextColor;
	afx_msg void OnInactiveTextColorChanged();
	BOOL m_innerRaised;
	afx_msg void OnInnerRaisedChanged();
	BOOL m_innerSunken;
	afx_msg void OnInnerSunkenChanged();
	BOOL m_outerRaised;
	afx_msg void OnOuterRaisedChanged();
	BOOL m_outerSunken;
	afx_msg void OnOuterSunkenChanged();
	short m_figureMaxSize;
	afx_msg void OnFigureMaxSizeChanged();
	short m_figureBound;
	afx_msg void OnFigureBoundChanged();
	BOOL m_advancedShadow;
	afx_msg void OnAdvancedShadowChanged();
	BOOL m_darkShadow;
	afx_msg void OnDarkShadowChanged();
	afx_msg BSTR GetBMPFileName();
	afx_msg void SetBMPFileName(LPCTSTR lpszNewValue);
	afx_msg BOOL GetDisabled();
	afx_msg void SetDisabled(BOOL bNewValue);
	afx_msg BSTR GetActionName();
	afx_msg void SetActionName(LPCTSTR lpszNewValue);
	afx_msg BOOL GetRunAction();
	afx_msg void SetRunAction(BOOL bNewValue);
	afx_msg BOOL GetAutoRepeat();
	afx_msg void SetAutoRepeat(BOOL bNewValue);
	afx_msg BOOL ResetPressedState();
	afx_msg BOOL SetPressedState();
	afx_msg BOOL IsPressed();
	afx_msg BOOL SetAutoSize();
	afx_msg BOOL ReloadBitmap();
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg void SetFocus();
	afx_msg void SetDrawFocusedRect(BOOL bNewValue);
	afx_msg BOOL GetDrawFocusedRect();
	afx_msg void SetSemiFlat(BOOL bNewValue);
	afx_msg BOOL GetSemiFlat();
	afx_msg void CopyToClipboard();
	afx_msg void PasteFromClipboard();
	afx_msg BOOL GetUseSystemFont();
	afx_msg void SetUseSystemFont(BOOL bNewValue);
	afx_msg BOOL GetShowToolTip();
	afx_msg void SetShowToolTip(BOOL bNewValue);
	afx_msg BSTR GetToolTip();
	afx_msg void SetToolTip(LPCTSTR lpszNewValue);
	afx_msg SHORT GetLayoutTextAlign(void);
	afx_msg void SetLayoutTextAlign(SHORT newVal);
	afx_msg long GetButtonLeft( );
	afx_msg void SetButtonLeft(long lLeft);
	afx_msg long GetButtonTop( );
	afx_msg void SetButtonTop(long lTop);
	//}}AFX_DISPATCH
	afx_msg LRESULT OnSetCheck( WPARAM wCheck, LPARAM );

	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CPushButtonCtrl)
	void FireClick()
		{FireEvent(DISPID_CLICK,EVENT_PARAM(VTS_NONE));}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CPushButtonCtrl)
	dispidBMPFileName = 1L,
	dispidButtonWidth = 2L,
	dispidButtonHeight = 3L,
	dispidAutoSize = 4L,
	dispidButtonDepth = 5L,
	dispidDrawBorder = 6L,
	dispidFlatButton = 7L,
	dispidDefaultButton = 8L,
	dispidThreeStateButton = 9L,
	dispidMouseInPicColor = 10L,
	dispidMouseOutPicColor = 11L,
	dispidMouseOutShadow = 12L,
	dispidButtonPressedPicColor = 13L,
	dispidButtonPressedShadow = 14L,
	dispidMouseInShadow = 15L,
	dispidDisabled = 38L,
	dispidDrawFocusRect = 16L,
	dispidStretch = 17L,
	dispidButtonText = 18L,
	dispidTextAlign = 19L,
	dispidOffsetTextFromBMP = 20L,
	dispidDrawFigures = 21L,
	dispidFigureType = 22L,
	dispidFigureSize = 23L,
	dispidColorTransparent = 24L,
	dispidColorFigure = 25L,
	dispidColorFillFigure = 26L,
	dispidFillFigure = 27L,
	dispidActiveTextColor = 28L,
	dispidInactiveTextColor = 29L,
	dispidInnerRaised = 30L,
	dispidInnerSunken = 31L,
	dispidOuterRaised = 32L,
	dispidOuterSunken = 33L,
	dispidFigureMaxSize = 34L,
	dispidFigureBound = 35L,
	dispidAdvancedShadow = 36L,
	dispidDarkShadow = 37L,
	dispidResetPressedState = 39L,
	dispidSetPressedState = 40L,
	dispidIsPressed = 41L,
	dispidSetAutoSize = 42L,
	dispidReloadBitmap = 43L,
	dispidActionName = 44L,
	dispidRunAction = 45L,		
	dispidAutoRepeat = 46L,
	dispidVisible = 47L,
	dispidSetFocus = 48L,
	dispidDrawFocusedRect = 49L,
	dispidSemiFlat = 50L,
	dispidCopyToClipboard = 51L,
	dispidPasteFromClipboard = 52L,
	dispidUseSystemFont = 53L,
	dispidShowToolTip = 54L,
	dispidToolTip = 55L,
	dispidLayoutTextAlign = 56L,
	dispidGetButtonLeft = 57L,
	dispidSetButtonLeft = 58L,
	dispidGetButtonTop = 59L,
	dispidSetButtonTop = 60L,
	//}}AFX_DISP_ID
	};

protected:
	OLE_COLOR	GetActiveBgColor();
	OLE_COLOR	GetInactiveBgColor();
	OLE_COLOR	GetActiveFgColor();
	OLE_COLOR	GetInactiveFgColor();

	CSize		m_sizeOriginBitmap;
	
	/*
	OLE_COLOR	m_ColorTransparent;
	OLE_COLOR	m_ColorFigureFill;
	OLE_COLOR	m_ColorFigure;
	*/
	
	HBITMAP		m_hBitmapMono;
	HBITMAP		m_hBitmapColor;
	HBITMAP		m_hBitmapMonoDither;
	HBITMAP		m_hBitmapColorDither;
	HBITMAP		m_hBitmapDisable;
	HBITMAP		m_hBitmapColorShadow;
	HBITMAP		m_hBitmapMonoShadow;
	

	BOOL		m_MouseOnButton;
	BOOL		m_bPushed;
	BOOL		m_bDisabled;
	CDib		m_DIB;
	CString		m_strPrevFileName;


	CString		m_strActionName;
	BOOL		m_bRunAction;
	
	BOOL		m_bAutoRepeat;

	void	_MakeBitmaps( );
	void	_FreeBMPs();
	void	RunAction();

	UINT m_nTimerID;
	UINT m_nMouseTimer;
	void RunTimer();
	void StopTimer();

	bool	m_bSemiFlat;	
	BOOL	m_bDrawFocusedRect;

	COLORREF GetDitherColor();

	BOOL	m_bUseSystemFont;

	// tooltips
	CToolTipCtrl	m_ToolTip;
	BOOL			m_bShowToolTip;
	CString			m_sToolTipText;
	
	void	_CreateToolTip( );
	void	_ReCreateToolTip( );
	//

	// LayoutTextAlign
	short	m_LayoutTextAlign;	// 0 - left (default), 1 - center , 2 - right
	UINT	getDTLayoutTextAlign();

	// vanek : drawing disabled icons - 27.02.2005
	CImageDrawer	m_imgdrawer;

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


// This class implements a memory Device Context
class CMemDC : public CDC
{
public:

    // constructor sets up the memory DC
    CMemDC(CDC* pDC) : CDC()
    {
        ASSERT(pDC != NULL);

        m_pDC = pDC;
        m_pOldBitmap = NULL;
        m_bMemDC = !pDC->IsPrinting();
              
        if (m_bMemDC)    // Create a Memory DC
        {
            pDC->GetClipBox(&m_rect);
            CreateCompatibleDC(pDC);
            m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
            m_pOldBitmap = SelectObject(&m_bitmap);
            SetWindowOrg(m_rect.left, m_rect.top);
        }
        else        // Make a copy of the relevent parts of the current DC for printing
        {
            m_bPrinting = pDC->m_bPrinting;
            m_hDC       = pDC->m_hDC;
            m_hAttribDC = pDC->m_hAttribDC;
        }
    }
    
    // Destructor copies the contents of the mem DC to the original DC
    ~CMemDC()
    {
        if (m_bMemDC) 
        {    
            // Copy the offscreen bitmap onto the screen.
            m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
                          this, m_rect.left, m_rect.top, SRCCOPY);

            //Swap back the original bitmap.
            SelectObject(m_pOldBitmap);
        } else {
            // All we need to do is replace the DC with an illegal value,
            // this keeps us from accidently deleting the handles associated with
            // the CDC that was passed to the constructor.
            m_hDC = m_hAttribDC = NULL;
        }
    }

    // Allow usage as a pointer
    CMemDC* operator->() {return this;}
        
    // Allow usage as a pointer
    operator CMemDC*() {return this;}

private:
    CBitmap  m_bitmap;      // Offscreen bitmap
    CBitmap* m_pOldBitmap;  // bitmap originally found in CMemDC
    CDC*     m_pDC;         // Saves CDC passed in constructor
    CRect    m_rect;        // Rectangle of drawing area.
    BOOL     m_bMemDC;      // TRUE if CDC really is a Memory DC.	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PUSHBUTTONCTL_H__54A0BF93_9A92_11D3_B1C5_EB19BDBABA39__INCLUDED)
