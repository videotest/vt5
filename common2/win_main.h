#ifndef __win_main_h__
#define __win_main_h__

#include "window5.h"
#include "impl_message.h"
#include "\vt5\awin\misc_list.h"
#include <atlstdthunk.h>


#pragma pack(push,1)
struct _thunk
{
	DWORD   m_mov;          // mov dword ptr [esp+0x4], pThis (esp+0x4 is hWnd)
	DWORD   m_this;         //
	BYTE    m_jmp;          // jmp WndProc
	DWORD   m_relproc;      // relative jmp
};
#pragma pack(pop)

class CWndProcThunk
{
public:
	ATL::CStdCallThunk m_thunk;
	void Init(WNDPROC proc, void* pThis)
	{
		//m_thunk.m_mov = 0x042444C7;  //C7 44 24 0C
		//m_thunk.m_this = (DWORD)pThis;
		//m_thunk.m_jmp = 0xe9;
		//m_thunk.m_relproc = (int)proc - ((int)this+sizeof(m_thunk));
		//// write block from data cache and
		////  flush from instruction cache
		//FlushInstructionCache(GetCurrentProcess(), &m_thunk, sizeof(m_thunk));
		BOOL b=m_thunk.Init((DWORD_PTR)proc, pThis);
	}
};

#define szDefClassName				"DefCommon2WindowClass"
#define szAxContainerWinClassName	"Common2AxDialog"


class CWinImpl : public CMessageFilterImpl,
					public IWindow2
{
public:
	CWinImpl();
	virtual ~CWinImpl();
public:
	virtual void Subclass( HWND hWnd );
	virtual void Detach();

	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
	virtual IUnknown	*DoGetWinInterface( const IID &iid );
//IWindow
	com_call CreateWnd( HANDLE hwndParent, RECT rect, DWORD dwStyles, UINT nID );
	com_call DestroyWindow();
	com_call GetHandle( HANDLE *phWnd );

	com_call HelpHitTest( POINT pt, BSTR *pbstrHelpFileName, DWORD *pdwTopicID, DWORD *pdwHelpCommand );
	com_call PreTranslateMsg( MSG *pmsg, BOOL *pbReturn );

	com_call GetWindowFlags( DWORD *pdwFlags );
	com_call SetWindowFlags( DWORD dwFlags );

	com_call PreTranslateAccelerator( MSG *pmsg );
//IWindow2
	com_call AttachMsgListener( IUnknown *punk );	//unknown should support IID_IMsgListener
	com_call DetachMsgListener( IUnknown *punk );
	com_call AttachDrawing( IUnknown *punk );		//unknown should support IID_IDrawObject
	com_call DetachDrawing( IUnknown *punk );
//message cracker prototipes
	/*WM_CREATE*/			virtual LRESULT	DoCreate( CREATESTRUCT *pcs )				{return 0;};
	/*WM_DESTROY*/			virtual void DoDestroy()									{};
	/*WM_PAINT*/			virtual void DoPaint()										{};
	/*WM_ERASEBACKRGND**/	virtual LRESULT DoEraseBackgrnd( HDC hDC )					{return 0;}
	/*WM_COMMAND*/			virtual void DoCommand( UINT nCmd )							{};
	/*WM_NOTIFY*/			virtual LRESULT DoNotify( UINT nID, NMHDR *phdr )			{return 0;}

	HWND	hwnd()	{return m_hwnd;}

	
protected:
	WNDPROC			m_pDefHandler;
	ATL::CStdCallThunk m_thunk;

	HWND			m_hwnd;
	DWORD			m_dwWindowFlags;
	char			m_szClassName[255];
	_list_t<IUnknownPtr> m_ptrsMsgListeners;
};

LRESULT __stdcall InitWindowProc( HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam );
LRESULT __stdcall WorkWindowProc( HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam );
LRESULT __stdcall InitAxWindowProc( HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam );
INT_PTR __stdcall InitDlgProc( HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam );


class CWndClass : public WNDCLASSEX
{
public:
	CWndClass( const char *pszName, WNDPROC pproc = InitWindowProc );
	bool Register();
public:
	bool	IsRegistred()	const		{return m_bRegistred;}
	const char *GetClassName()	const	{return m_szClassName;}
protected:
	bool	m_bRegistred;
	char	m_szClassName[255];
public:
	CWndClass	*m_pnext;
};




#endif // __win_main_h__