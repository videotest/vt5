#if !defined(AFX_DBASEENGINE_H__C640F856_BB17_47BA_B897_B6F82E18FCF4__INCLUDED_)
#define AFX_DBASEENGINE_H__C640F856_BB17_47BA_B897_B6F82E18FCF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBaseEngine.h : header file
//

class CDBaseDocument;
#include "types.h"



/////////////////////////////////////////////////////////////////////////////
// CDBaseEngine command target

class CDBaseEngine : public CCmdTarget
{
	DECLARE_DYNCREATE(CDBaseEngine)

	CDBaseDocument*		m_pDocument;
public:
	
	CDBaseEngine();
	CDBaseEngine(CDBaseDocument* pDocument);
	virtual ~CDBaseEngine();


	bool				OpenFromString( CString strConnectionString );
	bool				OpenAccessFile( CString strMDBFile, CString& strConnectionString );
	bool				OpenAdvancedConnection( CString& strConnectionString );
	bool				Close();

	bool				IsConnectionOK();
	bool				IsCatalogOK();
	bool				IsDirtyTableInfo();
	void				SetDirtyTableInfo( bool bDirty );

	bool				GetTablesInfo(CArray<_CTableInfo*, _CTableInfo*>** pArrTableInfo, bool bForceReread );

	IUnknown*			GetConnection();

	//DDL
	HRESULT				AddTable( BSTR bstrTableName );

	HRESULT				DeleteTable( BSTR bstrTableName );

	HRESULT				AddField(	BSTR bstrTableName, BSTR bstrFieldName, short nFieldType,
									BSTR bstrPrimaryTable, BSTR bstrPrimaryFieldName );

	HRESULT				SetFieldInfo(	BSTR bstrTableName, BSTR bstrFieldName, BSTR bstrUserName,
									BOOL bRequiredValue, BOOL bDefaultValue,
									BSTR bstrDefaultValue, BSTR bstrVTObjectType );

	HRESULT				DeleteField( BSTR bstrTableName, BSTR bstrFieldName );

	
	
protected:
	//Connection
	bool				CreateConnectionObject();
	ADO::_ConnectionPtr m_connPtr;	
	ADOX::_CatalogPtr	m_catalogPtr;



	bool				CreateTableInfoCache();
	void				DestroyTableInfoCache();	

	bool				m_bDirtyTableInfo;

	CString				m_str_conn;

	

	CArray<_CTableInfo*, _CTableInfo*> m_arrTableInfo;

public:
	static CString		GetFieldEntry( CString strTable, CString strField );
	static bool			CreateTableCacheOnCatalog( 
								IUnknown* punkCatalog, 
								IUnknown* punkNamedData,
								CArray<_CTableInfo*, _CTableInfo*>& arrTableInfo
								);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBASEENGINE_H__C640F856_BB17_47BA_B897_B6F82E18FCF4__INCLUDED_)
