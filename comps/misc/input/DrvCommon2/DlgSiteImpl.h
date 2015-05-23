#pragma once


#include "input.h"


class CDriverDialogSite : public IDriverDialogSite
{
public:
	IUnknown *DoGetInterface( const IID &iid )
	{
		if (iid == IID_IDriverDialogSite)
			return (IDriverDialogSite *)this;
		else
			return NULL;
	}

	com_call OnInitDialog(IDriverDialog *pDlg) {return E_NOTIMPL;}
	com_call OnInitDlgSize(IDriverDialog *pDlg) {return E_NOTIMPL;}
	com_call OnCmdMsg(int nCode, UINT nID, long lHWND) {return E_NOTIMPL;}
	com_call GetControlInfo(int nID, VALUEINFOEX2 *pValueInfoEx2)
		 {return E_NOTIMPL;}
	com_call OnSetControlInt(int nIDCtrl, int nValue)  {return E_NOTIMPL;}
	com_call OnUpdateCmdUI(HWND hwnd, int nId) {return E_NOTIMPL;}
	com_call OnCloseDialog(IDriverDialog *pDlg) {return E_NOTIMPL;}
	com_call OnDelayedRepaint(HWND hwndDlg, IDriverDialog *pDlg) {return E_NOTIMPL;}
};
