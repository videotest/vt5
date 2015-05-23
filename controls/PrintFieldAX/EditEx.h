#pragma once

#define EDITEX_TYPE_NUMERIC				0x00000000
#define	EDITEX_TYPE_TEXT				0x00000001
#define EDITEX_TYPE_MASKS				0x0000000F

#define EDITEX_NUMERIC_TYPE_INTEGER		0x00000000
#define EDITEX_NUMERIC_TYPE_DOUBLE		0x00000100
#define EDITEX_NUMERIC_TYPE_MASK		0x00000F00


// CEditEx

class CEditEx : public CEdit
{
	DECLARE_DYNAMIC(CEditEx)

	DWORD m_dwFlags;
	BOOL  m_bPaste;
	CString m_strPrevText;

public:
	CEditEx( DWORD dwFlags = 0 );
	virtual ~CEditEx();

	BOOL ValidateChar( UINT nChar );

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnSetText( WPARAM, LPARAM );
	afx_msg LRESULT OnPaste( WPARAM, LPARAM );
	afx_msg void OnChanged();
};


