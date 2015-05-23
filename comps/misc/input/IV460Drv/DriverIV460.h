#if !defined(AFX_DriverIV460_H__43437A22_09A0_11D4_A98A_FA6524668D51__INCLUDED_)
#define AFX_DriverIV460_H__43437A22_09A0_11D4_A98A_FA6524668D51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DriverIV460.h : header file
//

#include "DriverBase.h"
#include "PreviewDlg.h"
#include "Input.h"
#if !defined(__IV4_H_INCLUDED__)
#include "iv4.h"
#define __IV4_H_INCLUDED__
#endif

/////////////////////////////////////////////////////////////////////////////
// CDriverIV460 command target

class CDriverIV460 : public CDriverBase, public CInputPreviewImpl, public CInputWndOverlayImpl,
	public IDialogSite
{
	DECLARE_DYNCREATE(CDriverIV460)
	ENABLE_MULTYINTERFACE()

	CRect m_rcOvrSrc,m_rcOvrDst;
	HWND m_hwndOvr;
	IV4CameraConfig m_camcfg;

//	bool m_bOnGrab;
	int  m_nGrabCount;
	int  m_nBoardInit;
	BOOL m_bCameraChanged;
	BOOL m_bSettingsChanged;

	// Construction
public:
	CDriverIV460();
	virtual ~CDriverIV460();

// Attributes
public:

// Operations
public:
	bool InitializeBoard();
	void InitializeCamera();
	LPBITMAPINFOHEADER GrabImage(BOOL bConvert = FALSE, BOOL bGrayScale = FALSE);
	void ResetSettingsToDefault();
	virtual LPBITMAPINFOHEADER OnInputImage(BOOL bUseSettings);

// Overrides
	// Support of preview
	virtual bool OnBeginPreview(IInputPreviewSite *pSite);
	virtual void OnEndPreview(IInputPreviewSite *pSite);
	virtual void OnSetValue(int nDev, LPCTSTR lpName, VARIANT var);
	virtual VARIANT OnGetValue(int nDev, LPCTSTR lpName);
	virtual void OnGetSize(short &cx, short &cy);
	virtual void OnDrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst);
	// Support of window overlay
	virtual bool OnInitOverlay();
	virtual void OnDeinitOverlay();
	virtual void OnGetOvrSize(short &cx, short &cy);
	virtual bool OnStartOverlay(HWND hwndDraw, LPRECT lprcSrc, LPRECT lprcDst);
	virtual void OnStopOverlay(HWND hwndDraw);
	virtual void OnSetDstRect(HWND hwndDraw, LPRECT lprcDst);
	virtual void OnSetSrcRect(HWND hwndDraw, LPRECT lprcSrc);
	// Input functions
	virtual void OnInputImage(IUnknown *pUnkImg, IUnknown *punkTarget, int nDevice, BOOL bDialog);
	virtual bool OnInputVideoFile(CString &sVideoFileName, int nDevice, int nMode);
	virtual bool OnExecuteSettings(HWND hwndParent, IUnknown *punkTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput);
	virtual void OnGetPreviewFreq(DWORD &dwFreq);
	
	// Functions from IDialogSite
	virtual int GetButtonsNumber() {return 2;}
	virtual void GetButtonInfo(int nButton, CButtonInfo *pButtonInfo);
	virtual void ButtonPress(int nButtonId, ISettingsSite *pSettingsSite, IRedrawHook **ppRedrawHook);

	// Functions for IDriver2
	virtual void OnUpdateSizes();
	virtual void ReinitPreview();
	virtual void OnSetValueIntPrv(int nCurDev, LPCTSTR strSection, LPCTSTR strEntry, int nValue);
	virtual void OnSetValueStringPrv(int nCurDev, LPCTSTR strSection, LPCTSTR sEntry, LPCTSTR sValue);
	virtual void OnSetValueDoublePrv(int nCurDev, LPCTSTR strSection, LPCTSTR sEntry, double dValue);
	virtual int OnGetBoardInt(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, int nDef);
	virtual CString OnGetBoardString(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, LPCTSTR sValue);
	virtual double OnGetBoardDouble(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, double dValue);
	virtual void OnCancelChanges();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriverIV460)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDriverIV460)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CDriverIV460)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CDriverIV460)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVERMV500_H__43437A22_09A0_11D4_A98A_FA6524668D51__INCLUDED_)
