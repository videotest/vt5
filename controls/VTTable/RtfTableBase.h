#pragma once


// CRtfTableBase

class CRtfTableBase : public CRichEditCtrl
{
	DECLARE_DYNAMIC(CRtfTableBase)

public:
	CRtfTableBase();
	virtual ~CRtfTableBase();

	// override
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
#ifndef _UNICODE
	CString GetSelText() const;
private:
	BOOL		m_bUnicodeCtrl;
#endif

public:
	void SetRTF(CString sRTF);
	CString GetRTF();

protected:
	DECLARE_MESSAGE_MAP()
private:
	HINSTANCE	m_hinstanse_ctrl;

	BOOL		_init_ctrl( );
	BOOL		_deinit_ctrl( );

	static DWORD CALLBACK CBStreamIn(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
	static DWORD CALLBACK CBStreamOut(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb);

public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


