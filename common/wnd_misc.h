#ifndef __wnd_misc_h__
#define __wnd_misc_h__

#include "wndbase.h"

#define LAST_VIEW_SECTION	"\\LastSettings\\View"
#define LAST_VIEW_PROGID	"ProgID"

#define OBJECT_COUNT_PANE_SECTION	"\\ObjectCountPane"

// {DFEF56CD-CF25-4707-B8D3-2E044475D623}
static const GUID guidObjectCount = 
{ 0xdfef56cd, 0xcf25, 0x4707, { 0xb8, 0xd3, 0x2e, 0x4, 0x44, 0x75, 0xd6, 0x23 } };


std_dll bool statusShowObjectCountPage( bool bShow );


class CObjectCountPage : public CStatusPaneBase
{
public:
	CObjectCountPage();
	~CObjectCountPage();

protected:
	//{{AFX_MSG(CObjectCountPage)
	afx_msg void OnPaint();	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:

	UINT_PTR m_nTimerID;
	INT_PTR m_nLastCount;

	HICON	m_hIcon;	
};


#endif//__wnd_misc_h__
