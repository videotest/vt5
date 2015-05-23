#if !defined(__Docable_H__)
#define __Docable_H__

#include <ComDef.h>
#include "Defs.h"
#include "window5.h"


class CDocableWindowImplm : public IDockableWindow
{
protected:
	CSize m_sz;
	DWORD m_dwDockSide;
public:
	CDocableWindowImplm()
	{
		m_sz = CSize(100,100);
		m_dwDockSide = AFX_IDW_DOCKBAR_BOTTOM;
	}
	com_call GetSize( SIZE *psize, BOOL bVert )
	{
		*psize = m_sz;
		return S_OK;
	}
	com_call GetDockSide(DWORD *pdwSite)
	{
		*pdwSite = m_dwDockSide;
		return S_OK;
	}
	com_call GetFixed(BOOL *pbFixed)
	{
		*pbFixed = FALSE;
		return S_OK;
	}
	com_call OnShow()
	{
		return S_OK;
	}
};



class CWindowImplm : public IWindow
{
protected:
	CWnd *m_pWnd;
public:
	CWindowImplm();
	com_call CreateWnd(HANDLE hwndParent, RECT rect, DWORD dwStyles, UINT nID );
	com_call DestroyWindow();
	com_call GetHandle( HANDLE *phWnd );
	com_call HelpHitTest( POINT pt, BSTR *pbstrHelpFileName, DWORD *pdwTopicID, DWORD *pdwHelpCommand );
	com_call PreTranslateMsg( MSG *pmsg, BOOL *pbReturn );
	com_call GetWindowFlags( DWORD *pdwFlags );
	com_call SetWindowFlags( DWORD dwFlags );
	com_call PreTranslateAccelerator( MSG *pmsg );	//S_FALSE - continue processing

};








#endif
