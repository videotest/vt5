#if !defined __filter_base_h
#define __filter_base_h


/////////////////////////////////////////////////////////////////////////////
class CFilterCondition
{
public:
	CFilterCondition();
	~CFilterCondition();	
	
	CString m_strTable;
	CString m_strField;
	CString m_strCondition;	

	CString m_strValue;		
	CString m_strValueTo;

	BOOL	m_bANDunion;

	int		m_nLeftBracketCount;
	int		m_nRightBracketCount;

	CString GetAsTableField();	
	void Serialize( CArchive& ar );

	CString GetAsCaption( IUnknown* punkDoc );

	bool	StoreToData( IUnknown* punkData, CString strSection );
	bool	LoadFromData( IUnknown* punkData, CString strSection );
	

	CString GetFilterConditionForOrganizer( IUnknown* punkDoc, bool bInsUnion, bool bInsBracket = true );
	CString GetFilterConditionForSQL( IUnknown* punkDoc, bool bInsUnion);
private:
	CString GetConditionAsString( IUnknown* punkDoc, bool bInsUnion, bool bForSQL, bool bInsBracket = true );
};

/////////////////////////////////////////////////////////////////////////////
class CQueryFilter
{
public:
	CQueryFilter();
	~CQueryFilter();
	
	CString m_strName;
	CArray<CFilterCondition*, CFilterCondition*> m_arrCondition;	

	CString GetFilterForSQL( IUnknown* punkDoc );
	CString GetFilterForOrganizer( IUnknown* punkDoc, CString strQueryName );

	void Serialize( CArchive& ar );

	bool	StoreToData( IUnknown* punkData, CString strSection );
	bool	LoadFromData( IUnknown* punkData, CString strSection );

	bool	m_bModifiedFlag;

	
	bool CopyFrom( CQueryFilter* pSourceFilter );
	/*
	bool CopyFromDataFilter( IUnknown* punkQuery, int nFilterIndex, BOOL bWorkingFilter );
	bool StoreToDataFilter( IUnknown* punkQuery, int nFilterIndex, BOOL bWorkingFilter );
	*/

	void DeInit();
	
};

class CFilterHolder
{
public:
	CFilterHolder();
	~CFilterHolder();

//Filter operation
public:
	int  GetFilterCount();
	
	CString GetFilterName( int nIndex );
	CQueryFilter* GetFilter( CString strName );

	bool AddFilter( CString strFilterName );
	bool EditFilter( CString strFilterName, CString strNewFilterName );
	bool DeleteFilter( CString strFilterName );
	
	
	bool IsFilterNameUnic( CString strFilterName );
	bool IsValidFilter( CString strFilter, bool bReportError );


	CString GetFilterForSQL( IUnknown* punkDoc, CString strFilterName );
	CString GetFilterForOrganizer( IUnknown* punkDoc, CString strFilterName, CString strQueryName );
	

public:
	int  GetFilterConditionCount( CString strFilterName );
	bool AddFilterCondition( CString strFilterName,
							CString strTable, CString strField,
							CString strCondition, CString strValue,
							CString strValueTo, BOOL bANDUnion,
							int nLeftBracketCount, int nRightBracketCount
							);	
	bool GetFilterCondition( CString strFilterName, int nConditionIndex,
							CString& strTable, CString& strField,
							CString& strCondition, CString& strValue,
							CString& strValueTo, BOOL& bANDUnion,
							int& nLeftBracketCount, int& nRightBracketCount
							);
	bool EditFilterCondition( CString strFilterName, int nConditionIndex,
							CString strTable, CString strField,
							CString strCondition, CString strValue,
							CString strValueTo, BOOL bANDUnion,
							int nLeftBracketCount, int nRightBracketCount
							);	
	
	bool DeleteFilterCondition( CString strFilterName, int nConditionIndex );

	CString GetFilterConditionForOrganizer( IUnknown* punkDoc, CString strFilterName, int nConditionIndex );	


//FilterView support
	bool AddConditionFromView( CString strTable, CString strField, CString strValue );

//Filter selection
	CString GetActiveFilter();
	bool SetActiveFilter( CString strActiveFilter );

	bool SaveWorkingFilter( );	

protected:
	CString m_strCustomFilterName, m_strNoneFilterName;

public:
	WorkingFilterStatus GetWorkingFilterStatus();
	void SetWorkingFilterStatus( WorkingFilterStatus wfs );
	CString m_strFilterNameWorkingFilterCreateFrom;

	void DeInit();

protected:

	void InitFilters();	
	void Serialize( CArchive& ar );

	CQueryFilter m_workFilter;

	WorkingFilterStatus m_WorkingFilterStatus;
	CString  m_strActiveFilter;
	CArray<CQueryFilter*, CQueryFilter*> m_arrFilters;
	
	void ResetWorkingFilter();	

	
public:
	bool GetFilterInOrganizerMode();
	void SetFilterInOrganizerMode( bool bFilterInOrganizerMode );
protected:
	bool m_bFilterInOrganizerMode;
	

public:
	bool GetIsActiveFilterApply();
	void SetIsActiveFilterApply( bool bApply );
	
protected:
	bool m_bApplyFilter;


public:
	bool StoreToData( IUnknown* punkData );
	bool LoadFromData( IUnknown* punkData );


		
};


#endif //__filter_base_h