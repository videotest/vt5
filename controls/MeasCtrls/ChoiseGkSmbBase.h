#pragma once


// CChoiseGkSmbBase

class CChoiseGkSmbBase : public CComboBox
{
	DECLARE_DYNAMIC(CChoiseGkSmbBase)

public:
	CChoiseGkSmbBase();
	virtual ~CChoiseGkSmbBase();

protected:
	//afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM);
	afx_msg void MeasureItem ( LPMEASUREITEMSTRUCT lpMeasureItemStruct );
	afx_msg void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct);

	DECLARE_MESSAGE_MAP()

public:
	BYTE	m_btCharSet;
	long	m_lFirstChar;
	long	m_lLastChar;

};


