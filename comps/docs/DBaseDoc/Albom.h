#if !defined(AFX_ALBOM_H__0F97C5DB_8B45_4AD7_B1C2_1BC6E2019B64__INCLUDED_)
#define AFX_ALBOM_H__0F97C5DB_8B45_4AD7_B1C2_1BC6E2019B64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Albom.h : header file
//
#include "OleServInt.h"

class CAlbomClassFactory;

/////////////////////////////////////////////////////////////////////////////
// CAlbomServerFactory command target

class CAlbomServerFactory : public CCmdTarget
{
	DECLARE_DYNCREATE(CAlbomServerFactory)
	ENABLE_MULTYINTERFACE();

	CAlbomServerFactory();           // protected constructor used by dynamic creation

	bool CanUseDBRouter();
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlbomServerFactory)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CAlbomServerFactory();

	// Generated message map functions
	//{{AFX_MSG(CAlbomServerFactory)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CAlbomServerFactory)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAlbomServerFactory)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()


public:
	BEGIN_INTERFACE_PART(ServerFactory, IServerFactory)
		com_call OpenFactory();
		com_call CloseFactory();
		com_call GetRegisterInfo( CLSID* pCLSID, BSTR* pbstrComponentName, BSTR* pbstrProgID, BSTR* pbstrProgIDVer );

		com_call GetObjectsCount( long* plCount );
		com_call GetObjectRefCount( long lObjectIndex, long* plCount );
		com_call GetServerLocks( long* plCount );

	END_INTERFACE_PART(ServerFactory);

	CAlbomClassFactory* m_pCF;
	DWORD				m_dwCF;
};


class CAlbomDB;
/////////////////////////////////////////////////////////////////////////////
class CAlbomClassFactory : public CCmdTarget
{
	friend class CAlbomServerFactory;	
	friend class CAlbomServerFactory::XServerFactory;

public:
	static const GUID m_guid;
public:
	CAlbomClassFactory( );
	~CAlbomClassFactory();


	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(ClassFactory2, IClassFactory)
        com_call CreateInstance( IUnknown* pUnkOuter, REFIID riid, void** ppvObject);
        com_call LockServer( BOOL fLock);
		/*
        com_call GetLicInfo( LICINFO* pLicInfo);
        com_call RequestLicKey( DWORD dwReserved, BSTR* pBstrKey);
        com_call CreateInstanceLic( 
						IUnknown* pUnkOuter,
			            IUnknown* pUnkReserved,
			            REFIID riid,
						BSTR bstrKey,
						void** ppvObj);
						*/
	END_INTERFACE_PART(ClassFactory2);
	
	virtual void OnFinalRelease();

protected:	

	CArray<CAlbomDB*,CAlbomDB*> m_arObjects;

public:
	void OnObjectKill( CAlbomDB* pObject );
	DWORD	m_dwLockCount;
					   

};

/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// CAlbomDB command target

class CAlbomDB : public CCmdTarget
{
protected:
	CAlbomClassFactory* m_pClassFactory;
	//DECLARE_DYNCREATE(CAlbomDB)
public:
	CAlbomDB( );           // protected constructor used by dynamic creation
	void SetFactory( CAlbomClassFactory* pClassFactory  );
	virtual ~CAlbomDB( );
// Attributes
public:

protected:

//Init
	bool OpenLastDBaseDoc();

//some test helpers
	bool IsAppValid();

	CString GetDBFileName();

//helpers
	IDBaseDocumentPtr	GetDBaseDoc( bool bReportError = false );
	ISelectQueryPtr		GetQuery( bool bReportError = false );	
	bool				IsQueryOpen( ISelectQueryPtr ptrQ );


//Report error
	void ReportCantFindDBaseDoc();
	void ReportCantFindQuery();

//Bring app to top
	void BringApp2Top();


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlbomDB)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL


	// Generated message map functions
	//{{AFX_MSG(CAlbomDB)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	//DECLARE_OLECREATE(CAlbomDB)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAlbomDB)
	afx_msg long GetFieldType(long iFieldNum);
	afx_msg BSTR GetFieldName(long iFieldNum);
	afx_msg long GetFieldCount();
	afx_msg VARIANT GetFieldValue(long iFieldNum);
	afx_msg BOOL ShowQueryDialog(long hWndParent);
	afx_msg BOOL SetFieldValue(long iFieldNum, const VARIANT FAR& var);
	afx_msg long GetRecordsCount();
	afx_msg long GetCurrentRecordNum();
	afx_msg BOOL MoveFirst();
	afx_msg BOOL MoveNext();
	afx_msg BOOL MovePrev();
	afx_msg BOOL MoveLast();
	afx_msg BOOL AddRecord();
	afx_msg BOOL DeleteCurrentRecord();
	afx_msg BOOL SetVisible(BOOL bVisible);
	afx_msg BOOL IsVisible();
	afx_msg BOOL PasteField(long iFieldNum);
	afx_msg BOOL OpenLastShape();
	afx_msg BOOL OpenFile(LPCTSTR szPathName);
	afx_msg void UpdateView();
	afx_msg long GetActiveField();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	/*
protected:
	IServerFactoryPtr m_ptrServerFactory;
	*/
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALBOM_H__0F97C5DB_8B45_4AD7_B1C2_1BC6E2019B64__INCLUDED_)
