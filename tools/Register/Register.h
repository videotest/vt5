// Register.h : main header file for the REGISTER application
//

#if !defined(AFX_REGISTER_H__55B46B20_53A3_4517_83DB_C078C490C0B2__INCLUDED_)
#define AFX_REGISTER_H__55B46B20_53A3_4517_83DB_C078C490C0B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols



/////////////////////////////////////////////////////////////////////////////
// CRegisterApp:
// See Register.cpp for the implementation of this class
//
extern UINT WM_FIND_APP;

extern "C" _declspec(dllexport) void STDAPICALLTYPE SetData(long * plData, long * plSize);
extern "C" _declspec(dllexport) void STDAPICALLTYPE GetData(long * plData, long * plSize);
extern "C" _declspec(dllexport) void STDAPICALLTYPE InitData(long * plData, long * plSize);

extern "C" _declspec(dllexport) void GuardGetNSKInfo(DWORD*);
extern "C" _declspec(dllexport) void GuardGetAppName(char*, LONG*);
extern "C" _declspec(dllexport) void GuardGetCompanyName(char * szCompanyName, long * plSize);
extern "C" _declspec(dllexport) void GuardGetSuffix(char * szSuffix, long * plSize);

extern "C" _declspec(dllexport) void GuardSetNSKInfo(DWORD*);
extern "C" _declspec(dllexport) void GuardSetAppName(const char *);
extern "C" _declspec(dllexport) void GuardSetCompanyName(const char * szCompanyName);
extern "C" _declspec(dllexport) void GuardSetSuffix(const char * szSuffix);
extern "C" _declspec(dllexport) IUnknown * GuardGetAppUnknown(bool bAddRef = false);


#include <AfxTempl.h>

struct CRegisterVersion
{
	CString			strProgramName;
	CString			strPath;
	CString			strSuffix;
	CStringArray	arrFiles;

	CRegisterVersion()
	{
		arrFiles.RemoveAll();
	}
	~CRegisterVersion()
	{
		arrFiles.RemoveAll();
	}

};

#include "TransTable.h"

class CRegisterApp : public CWinApp,
					public CMapInterfaceImpl
{
// guard data
	CString				m_strSuffix;
	CTranslateTable *	m_pTable;
	GUID 				m_innerclsid;
	DWORD				m_dwImito;

	CString				m_strGuardAppName;
	CString				m_strGuardCompanyName;
	DWORD				m_dwGuardKeyID;

	CRegisterVersion	m_regData;

public:
	CRegisterApp();
	~CRegisterApp();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRegisterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_INTERFACE_MAP()
	BEGIN_INTERFACE_PART(Guard, IVTApplication)
		com_call InitApp( BYTE* pBuf, DWORD dwData );
		// return ptr on KeyGuid, ptr on TranslationTable, szSuffix, imito
		com_call GetData(DWORD * pKeyGUID, BYTE ** pTable, BSTR * pbstrSuffix, DWORD * pdwImito);
		// return ptr on KeyGuid, ptr on TranslationTable, szSuffix, imito
		com_call SetData(DWORD * pKeyGUID, BYTE ** pTable, BSTR * pbstrSuffix, DWORD * pdwImito);
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

public:	//attributes
	void GuardGetNSKInfo(DWORD * pdwKeyId)
	{	
		if (pdwKeyId) *pdwKeyId = m_dwGuardKeyID;
	}
	void GuardSetNSKInfo(DWORD * pdwKeyId)
	{
		if (pdwKeyId) m_dwGuardKeyID = *pdwKeyId;
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

	bool LoadRegisterData(CRegisterVersion & regData);
	bool SaveRegisterData(CRegisterVersion & regData);

protected:
	bool ReadGuardInfo(LPCTSTR szFile);
	void DefaultInit();

	virtual IUnknown* GetInterfaceHook( const void* p )
	{	return get_fast_interface( (const GUID*)p );	}
	virtual IUnknown* raw_get_interface( const GUID* piid )
	{	return GetInterface( piid );					}
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTER_H__55B46B20_53A3_4517_83DB_C078C490C0B2__INCLUDED_)
