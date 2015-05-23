#pragma once


// CCustomStatic

class CCustomStatic : public CStatic
{
	DECLARE_DYNAMIC(CCustomStatic)
	bool m_bPushed, m_bOver;
public:
	CCustomStatic();
	virtual ~CCustomStatic();

protected:
	DECLARE_MESSAGE_MAP()
public:
	bool SetState(bool bState, int nType );
	afx_msg void OnPaint();
};


