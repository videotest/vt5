// shell.h : main header file for the SHELL application
//

#if !defined(AFX_SHELL_H__C48CE9BA_F947_11D2_A596_0000B493FF1B__INCLUDED_)
#define AFX_SHELL_H__C48CE9BA_F947_11D2_A596_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define WM_CLOSE_DOCK_BAR	WM_USER + 1000
extern UINT WM_FIND_APP;

extern "C" _declspec(dllexport) void STDAPICALLTYPE SetData(long * plData, long * plSize);
extern "C" _declspec(dllexport) void STDAPICALLTYPE GetData(long * plData, long * plSize);
extern "C" _declspec(dllexport) void STDAPICALLTYPE InitData(long * plData, long * plSize);

extern "C" _declspec(dllexport) void GuardGetNSKInfo(DWORD*);
extern "C" _declspec(dllexport) void GuardGetAppName(char*, LONG*);
extern "C" _declspec(dllexport) void GuardGetCompanyName(char * szCompanyName, long * plSize);

extern "C" _declspec(dllexport) void GuardSetNSKInfo(DWORD*);
extern "C" _declspec(dllexport) void GuardSetAppName(const char *);
extern "C" _declspec(dllexport) void GuardSetCompanyName(const char * szCompanyName);
extern "C" _declspec(dllexport) IUnknown * GuardGetAppUnknown(bool bAddRef = false);

CString		LanguageLoadCString( UINT ui_id );
		


/////////////////////////////////////////////////////////////////////////////
// CShellApp:
// See shell.cpp for the implementation of this class
//

#include "TransTable.h"
#include "wndmisc5.h"
#include "docmiscint.h"
#include "ScriptNotifyInt.h"

class CShellDocManager;

#include "\vt5\awin\misc_list.h"


class CShellApp : public CWinApp,
					public CCompManagerImpl,
					public CNumeredObjectImpl,
					public CMapInterfaceImpl
{
// guard data
	CString				m_strSuffix;
	CTranslateTable *	m_pTable;
	GUID 				m_innerclsid;
	DWORD				m_dwImito;

	CString				m_strGuardAppName;
	CString				m_strGuardCompanyName;
	
	class CScriptDesc
	{
		DWORD m_dwFlag;
		IUnknown *m_punkScript;

	public:
		CScriptDesc()
		{
			m_dwFlag = 0;
			m_punkScript = 0;
		}
		
		CScriptDesc( IUnknown *punk, DWORD dwFlag)
		{
			m_dwFlag = dwFlag;
			m_punkScript = punk;
			m_punkScript->AddRef();
		}

		CScriptDesc( const CScriptDesc &desc )
		{
			*this = desc;
		}
		
		~CScriptDesc()
		{
			if( m_punkScript )
				m_punkScript->Release();
		}
		
		void operator=( const CScriptDesc &desc)
		{
			m_dwFlag = desc.m_dwFlag;
			m_punkScript = desc.m_punkScript;
			m_punkScript->AddRef();
		}

		bool operator==( const CScriptDesc &desc)
		{
			if( m_dwFlag == desc.m_dwFlag && m_punkScript == desc.m_punkScript )
				return true;
			return false;
		}

		IUnknown *GetScript( bool bAddRef = false ) 
		{ 
			if( bAddRef )
				m_punkScript->Release();
			return m_punkScript; 
		}

		DWORD GetFlag() { return m_dwFlag; }
	};
	_list_t< CScriptDesc > m_arrScriptPUNK;
public:
	static DWORD		m_dwGuardKeyID;

protected:
	bool m_bCreateInstance;
	bool m_bGetClassObject;
	bool m_bCLSIDFromProgID;
	bool m_bProgIDFromCLSID;
#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
	bool m_bCreateInstanceEx;
#endif // DCOM

	HANDLE m_hErrorThread;
	ENABLE_MULTYINTERFACE();
public:
	CShellApp();
	~CShellApp();

	bool m_bDialog;

	virtual void OnFinalRelease();
	virtual BOOL IsIdleMessage(MSG* pMsg);
	virtual BOOL PumpMessage();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
		// Server object for document creation
	//{{AFX_MSG(CShellApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(App, IApplication)
		com_call GetActionManager( IUnknown **punk );
		com_call GetCommandManager( IUnknown **punk );
		com_call GetActiveScript( IUnknown **punk );
		com_call GetTargetManager( IUnknown **punk );
		com_call GetFirstDocTemplPosition( LONG_PTR *plPos );
		com_call GetNextDocTempl( LONG_PTR *plPos, BSTR *pbstrName, IUnknown **punk );
				//temporely IUnknow doesn/t used

		com_call GetFirstDocPosition( LONG_PTR lPosTemplate, LONG_PTR *plPosDoc );
		com_call GetNextDoc( LONG_PTR lPosTemplate, LONG_PTR *plPosDoc, IUnknown **punkDoc );

		com_call GetActiveDocument( IUnknown **plDoc );
		com_call ProcessMessage();
		com_call GetMainWindowHandle( HWND *phWnd );
		com_call GetMatchDocTemplate( BSTR bstrFileName, IUnknown **punk );
		com_call GetStatusBar( IUnknown **punk );
		com_call SetNestedMessageLoop( BOOL bSet, BOOL *pbOld );
		com_call IsNestedMessageLoop();
		com_call IdleUpdate();
	END_INTERFACE_PART(App)

	
	BEGIN_INTERFACE_PART(HwndHook, IHwndHook)
		com_call EnterHook( HWND hWnd );
		com_call LeaveHook( );
	END_INTERFACE_PART(HwndHook)
	HWND		m_hWndHook;


	BEGIN_INTERFACE_PART(Guard, IVTApplication)
		com_call InitApp( BYTE* pBuf, DWORD dwData );
		// return ptr on KeyGuid, ptr on TranslationTable, szSuffix, imito
		com_call GetData(DWORD * pKeyGUID, BYTE ** ppTable, BSTR * pbstrSuffix, DWORD * pdwImito);
		// return ptr on KeyGuid, ptr on TranslationTable, szSuffix, imito
		com_call SetData(DWORD * pKeyGUID, BYTE ** ppTable, BSTR * pbstrSuffix, DWORD * pdwImito);
		// get mode (register or not)
		com_call GetMode(BOOL * pbRegister);

		// Add guid to table
		com_call AddEntry(DWORD * pIntGUID, DWORD * pExtGUID, BSTR bstrIntProgID, DWORD dwData);
		// Remove guid from table
		com_call RemoveEntry(DWORD * pExtGUID, BSTR bstrIntProgID);
		// get inner guid from table by extern guid	: mode 0
		// get extern guid by inner guid			: mode 1
		com_call GetEntry(DWORD dwMode, DWORD * pSrcGUID, DWORD * pDstGUID, DWORD * pdwData);

		// Get External GUID from internal/external ProgID : mode 0
		// Get internal GUID from internal/external ProgID : mode 1
		com_call GetModeData(DWORD dwMode, DWORD * pGUID, BSTR bstrProgID);
		// translations	
		// get external ProgID from internal ProgID : mode 0
		// get internal ProgID from external ProgID : mode 1
		com_call GetDataValue(DWORD dwMode, BSTR bstrSrcProgID, BSTR * pbstrDstProgID);

		com_call GetNames(BSTR * pbstrAppName, BSTR * pbstrCompanyName);
		com_call GetRegKey(BSTR * pbstrRegKey);
	END_INTERFACE_PART(Guard)

	BEGIN_INTERFACE_PART(FormNames, IFormNamesData )
		com_call GetFormName( BSTR bstr_label, BSTR *bstr_name );
	END_INTERFACE_PART(FormNames)

	// IScriptHolder
	BEGIN_INTERFACE_PART(ScriptSite, IScriptSite )
		com_call RegisterScript( IUnknown *punk, DWORD dwFlags );
		com_call UnregisterScript( IUnknown *punk, DWORD dwFlags );
		com_call UnregisterAll();
		com_call Invoke( BSTR bstrFuncName, VARIANT* pargs, int nArgsCount, VARIANT* pvarResult, DWORD dwFlags   );

		com_call GetFirstPos(TPOS *dwPos);
		com_call GetNextPos(TPOS *dwPos, IUnknown **punk, DWORD *dwFlags);
	END_INTERFACE_PART( ScriptSite )

public:
	void FreeComponents();					//free all application components. 
											//Called in CMainFrame::OnClose for avoid deadlock
public:	//attributes
	CShellDocManager	*GetDocManager() const;	//return application document manager
	UINT	GetCurrentHelpCommand() const;		//return a current help command
	DWORD	GetCurrentTopicID() const;			//return a current topic id
	CRecentFileList	*GetRecentFileList()			{return m_pRecentFileList;}

	void GuardGetNSKInfo(DWORD * pdwKeyID)
	{	
		if (pdwKeyID) *pdwKeyID = m_dwGuardKeyID;
	}
	void GuardSetNSKInfo(DWORD * pdwKeyID)
	{
		if (pdwKeyID) m_dwGuardKeyID = *pdwKeyID;
	}
	
	void GuardGetAppName(char * szAppName, LONG * lpSize);
	void GuardSetAppName(const char * szAppName)
	{	m_strGuardAppName = szAppName;	}

	void GuardGetCompanyName(char * szCompanyName, long * plSize);
	void GuardSetCompanyName(const char * szCompanyName)
	{	m_strGuardCompanyName = szCompanyName;	}

	void GuardGetSuffix(char * szSuffix, long * plSize);
	void GuardSetSuffix(const char * szSuffix)
	{	m_strSuffix = szSuffix;	}



protected:
	bool ReadGuardInfo(LPCTSTR szFile);
	bool AddOwnComponents();
	HRESULT _invoke_script_func( IDispatch *pDisp, BSTR bstrFuncName, VARIANT* pargs, int nArgsCount, VARIANT* pvarResult );


protected:
	CAggrManager	m_aggrs;

	UINT	m_nCurrentHelpCommand;
	DWORD	m_dwCurrentHelpTopicID;


	IUnknown* m_pUnkThumbnailManager;
	BOOL		m_bNestedLoop;

	IActionManagerPtr	m_ptrAM;
	
	HANDLE				m_hmutex_app;

//OnIdle optimize
	long				m_last_idle_tick;
	LONG				m_last_idle_count;
public:
	void				OnTimerFire();
	void				ForceIdleUpdate();

	//cache interface
	virtual IUnknown* GetInterfaceHook( const void* p );
	virtual IUnknown* raw_get_interface( const GUID* piid );

protected:
	long m_nPumpMessageDepth; // AAM: глубина вложенности PumpMessage - чтобы отличить внешний цикл от внутренних вызовов ProcessMessages
};

extern CShellApp	theApp;
extern GUID IID_ITypeLibID;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHELL_H__C48CE9BA_F947_11D2_A596_0000B493FF1B__INCLUDED_)
