#if !defined(AFX_TYPEINFOMGR_H__C49F7852_39FA_11D3_87C6_0000B493FF1B__INCLUDED_)
#define AFX_TYPEINFOMGR_H__C49F7852_39FA_11D3_87C6_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TypeInfoMgr.h : header file
//

#include "compManager.h"

/////////////////////////////////////////////////////////////////////////////
// CTypeInfoManager command target
// inmplements IDataTypeManager2 interface
// this class hold info about type of named objects (as ScriptDocObject etc.)
// later it will be more detailed description
class CTypeInfoManager :	public CCmdTargetEx,
							public CCompManager
{
	DECLARE_DYNCREATE(CTypeInfoManager)
	GUARD_DECLARE_OLECREATE(CTypeInfoManager)
	ENABLE_MULTYINTERFACE()

protected:
	CTypeInfoManager();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTypeInfoManager)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTypeInfoManager();

	virtual void DeInit()
	{
		CCompManager::DeInit();
	}


	// Generated message map functions
	//{{AFX_MSG(CTypeInfoManager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CTypeInfoManager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
//	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	
	BEGIN_INTERFACE_PART(TypeInfo, IDataTypeInfoManager)
		com_call GetTypesCount( long *pnCount );
		com_call GetType( long index, BSTR *pbstrType );
		com_call GetTypeInfo( BSTR bstrType, INamedDataInfo **ppunkObj );
		com_call GetTypeInfo( long index, INamedDataInfo **ppunkObj );
	END_INTERFACE_PART(TypeInfo);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TYPEINFOMGR_H__C49F7852_39FA_11D3_87C6_0000B493FF1B__INCLUDED_)
