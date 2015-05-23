#if !defined __query_base_h
#define __query_base_h

#include "filterbase.h"
#include "types.h"

class CSortField
{
public:	
	CSortField()
	{
		m_bAcsending = true;
	}

	CString	m_strTable;
	CString	m_strField;

	bool	m_bAcsending;

	void Serialize( CStreamEx& ar )
	{
		long nVersion = 1;	
		if( ar.IsLoading() )
		{
			ar >> nVersion;
			ar >> m_strTable;
			ar >> m_strField;
			long l = 0;
			ar >> l;
			m_bAcsending = ( l == 1 );
		}
		else
		{
			ar << nVersion;
			ar << m_strTable;
			ar << m_strField;
			long l = ( m_bAcsending ? 1 : 0 );
			ar << l;			
		}
	}

};


class CGridField
{
public:	
	CGridField()
	{
		m_nWidth = 100;	
		m_nPos	 = -1;
	}

	void Serialize( CStreamEx& ar )
	{
		long nVersion = 1;	
		if( ar.IsLoading() )
		{
			ar >> nVersion;
			ar >> m_strTable;
			ar >> m_strField;
			ar >> m_nWidth;
			ar >> m_nPos;
		}
		else
		{
			ar << nVersion;
			ar << m_strTable;
			ar << m_strField;
			ar << m_nWidth;
			ar << m_nPos;
		}
	}

	CString	m_strTable;
	CString	m_strField;

	int m_nWidth;
	int m_nPos;
};

/////////////////////////////////////////////////////////////////////////////
class CBaseQuery
{	
public:
	CBaseQuery();
	~CBaseQuery();
	void DeInit();
	
	CString			m_strBaseTable;		
	BOOL			m_bSimpleQuery;
	CString			m_strSQL;

	void			Serialize( CStreamEx& ar );	
	
	CArray<CQueryField*, CQueryField*> m_arrFields;
	CArray<CSortField*, CSortField*> m_arrSortFields;
	CArray<CGridField*, CGridField*> m_arrGridFields;

	void DeleteSortFields();
	void DeleteGridFields();

	CString			GetSQL( IUnknown* punkFilterHolder );

};


/////////////////////////////////////////////////////////////////////////////
class CUndoQuery{
public:
	CString		m_strQueryName;		
	bool		m_bCanUseUndo;
	CBaseQuery	m_queryInfo;			
};



#endif //__filter_base_h
