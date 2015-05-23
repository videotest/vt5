#if !defined(__StatusPane_H__)
#define __StatusPane_H__

#include "impl_misc.h"
#include "win_main.h"
#include "FocusInt.h"

class CStatusPaneFocus : public ComObjectBase, public CWinImpl, public CNamedObjectImpl,
	public IStatusPane, public IFocusPane
{
	HFONT m_ft;
	int m_nFocus;
	bool m_bAuto;
	int m_nTimer;
public:
	route_unknown();
	CStatusPaneFocus()
	{
		m_ft = NULL;
		m_nTimer = 0;
		m_nFocus = 0;
		m_bAuto = false;
	}
	virtual IUnknown *DoGetInterface(const IID &iid);
	virtual void DoPaint();
	virtual LRESULT	DoMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);
	com_call GetWidth(int *pWidth);
	com_call GetSide(DWORD *pdwSide);
	com_call SetFocusIndicator(bool bAuto, int nFocus);
	com_call GetFocusIndicator(bool *pbAuto, int *pnFocus);
};



#endif