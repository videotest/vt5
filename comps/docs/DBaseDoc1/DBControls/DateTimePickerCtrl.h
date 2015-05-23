#pragma once

// [vanek] BT2000: 3510 - 26.01.2004
// CDateTimePickerCtrl
class CDateTimePickerCtrl : public CDateTimeCtrl
{
	DECLARE_DYNAMIC(CDateTimePickerCtrl)

public:
	CDateTimePickerCtrl();
	virtual ~CDateTimePickerCtrl();

	void	SetReadOnly( BOOL bReadOnly ) { m_bReadOnly = bReadOnly;  }
	BOOL	GetReadOnly( ) { return m_bReadOnly; }

protected:
    DECLARE_MESSAGE_MAP()

	BOOL m_bReadOnly;
public:
	afx_msg BOOL OnDtnDropdown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};


