// DBaseDoc.h : main header file for the DBaseDoc DLL
//

#if !defined(AFX_DBaseDoc_H__0182AFB4_FB2A_42FE_BC57_26348D3BCD6E__INCLUDED_)
#define AFX_DBaseDoc_H__0182AFB4_FB2A_42FE_BC57_26348D3BCD6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "resource.h"		// main symbols


class _CTableInfo;

#define TABLEINFO_ARR		CArray<_CTableInfo*, _CTableInfo*>
#define TABLEINFO_ARR_PTR	CArray<_CTableInfo*, _CTableInfo*>*




#define return_and_assert(value) {ASSERT(false);return value;}


#define IS_OK(f)		VERIFY(SUCCEEDED(f))

#define ConvertToBoolean(BIGBOOL) (BIGBOOL==TRUE?true:false)
#define ConvertToBOOL(SMALLBOOL) (SMALLBOOL==true?TRUE:FALSE)

#define _START_CATCH		\
try{					\

#define _START_CATCH_REPORT	_START_CATCH

#define _END_CATCH(x)	\
}						\
catch(...)	\
{						\
	return x;			\
}						\

#define _END_CATCH_REPORT(x)	\
}						\
catch(_com_error &e)	\
{						\
	dump_com_error(e);	\
	return x;			\
}						\


bool IsOwnDataObject( _variant_t var );

bool GetDBaseDocument( sptrIDBaseDocument& spIDBDoc );

bool ReportError( CString strMessage, IUnknown* pUnk, const IID& iid, CString strSQL = "" );

CString GetDBaseConfigtDir();
BOOL WriteDirectory(CString dd);
void dump_com_error( _com_error &e );

//bool GetObjectTypesSupportByView( IUnknown* pUnkView, CArray<CString, CString>& arrTypes );
//bool GenerateTumbnailFromView( IUnknown* pUnkView, sptrIDBaseDocument spDBDoc );

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

FieldType GetFieldType( sptrIDBaseDocument spDBDoc, CString strTable, CString strField );
CString GetFieldCaption( sptrIDBaseDocument spDBDoc, CString strTable, CString strField );

CWnd* GetMainFrameWnd();


bool IsAvailableFieldInQuery( IUnknown* punkQuery, CString strTable, CString strField );


bool GetTableFieldFromSection(CString strSection, CString& strTable, CString& strField, bool bTestFullPath );


_variant_t _GetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault );
long _GetValueInt( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lDefault );
CString _GetValueString( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszDefault );
void _SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lValue );
void _SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszValue );
void _SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault );


IUnknown* GetFilterPropPage();

bool RefreshFilterPropPage();

void GetTableFieldFromPath( CString strPath, CString& strTable, CString& strField  );
CString GenerateFieldPath( CString strTable, CString strField, bool bAddPath = false );

IUnknown* GetObjectFromContextByPos( IUnknown* punkDataContext, _bstr_t bstrType, int nPos );

bool DrawThumbnail( IUnknown* punkObject, CDC* pDC, CRect rcTarget, CString strComment );

CString GetFieldVTObjectType( IUnknown* punkNamedData, CString strTable, CString strField );


void CreateRecordPaneInfo();
void DestroyRecordPaneInfo();
void UpdateRecordPaneInfo( IUnknown* punkQuery/* can be 0, then get active query from active doc*/);

bool IsAccessDBase( IUnknown* punkDoc );

CString _MakeLower( CString str );


bool FillTableInfoFromDoc( IUnknown* punkDoc, TABLEINFO_ARR_PTR pArr );

CString GenerateUniqFieldName( IUnknown* punkDoc, CString strTable, FieldType ft );
/////////////////////////////////////////////////////////////////////////////
// CDBaseDocApp
// See DBaseDoc.cpp for the implementation of this class
//

bool ActivateDBObjectInContext( CString strTable, CString strField, IUnknown* punkContext );

ADO::CursorLocationEnum GetCursorLocation( CString strSQL );

bool GetActiveFieldFromActiveView( CString& strTable, CString& strField, CString& strValue );

class CDBaseDocApp : public CWinApp
{
public:
	CDBaseDocApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBaseDocApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDBaseDocApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

bool SetModifiedFlagToDoc( IUnknown* punkDoc );
bool SetModifiedFlagToObj( IUnknown* punkObj );


bool ForceAppIdleUpdate();


bool IsSQLReservedWord( const char* psz_word );
bool SetListoxAutoExtent( CListBox* plistbox );

#define IMPLEMENT_HELP( str_topic )\
void OnHelp()\
{\
	HelpDisplay( str_topic );\
}\

//compact
bool CompactMDB( const char* psz_file_name );
//save DataBase document
bool SaveDBaseDocument( IUnknown* punk_doc );

/////////////////////////////////////////////////////////////////////////////
//Users info
class user_info
{
public:
	CString	m_str_user;
	CString	m_str_machine;
};
bool operator ==(const user_info&, const user_info&);
bool GetUsersInfo( CArray<user_info,user_info&> *parr_users );

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBaseDoc_H__0182AFB4_FB2A_42FE_BC57_26348D3BCD6E__INCLUDED_)
