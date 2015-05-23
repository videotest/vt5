#pragma once


// CXPBarMgr

class CXPBarMgr : public CControlBar
{
	DECLARE_DYNAMIC(CXPBarMgr)

public:
	CXPBarMgr();
	virtual ~CXPBarMgr();
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

	HWND m_hWndXPbar;
	int  m_nWidth;

	void SetXPBarPos();

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz) {return CSize(m_nWidth,32767);}
protected:
	DECLARE_MESSAGE_MAP()
};


