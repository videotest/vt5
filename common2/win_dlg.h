#ifndef __win_dlg_h__
#define __win_dlg_h__

#include "win_main.h"
#include "ax_host.h"

class CDialogImpl: public CWinImpl
{
public:
	CDialogImpl();
	~CDialogImpl();
public:
//IWindow
	com_call CreateWnd( HANDLE hwndParent, RECT rect, DWORD dwStyles, UINT nID );
	com_call DestroyWindow();

	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
	virtual void DoPaint();
	virtual bool DoInitDialog()		{return true;};
	virtual void Subclass( HWND hWnd );
	virtual void DoCommand( UINT nCmd );

	int DoModal();
	
	virtual UINT idd() = 0;
protected:
	bool LoadTemplate();
	void FreeTemplate();
protected:
	bool			m_bInsideDialogMessage;

	DLGTEMPLATE		*m_pTemplate;
	DLGTEMPLATE		*m_pDialogTemplateSrc;
	byte			*m_pInitData;
	HANDLE			m_hTemplate;
	HANDLE			m_hInitData;
};


#endif //__win_dlg_h__
