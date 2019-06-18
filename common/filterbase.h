#ifndef __filterbase_h__
#define __filterbase_h__

#include "actionbase.h"

//#include "list"

//using namespace std ;


class std_dll CFilterErrorInfo
{
};

class std_dll CFilterArgument
{
public:
	CString		m_strArgName;
	CString		m_strArgType;
	IUnknown	*m_punkArg;
	bool		m_bInArgument;
	bool		m_bNameDataChanged;
	GuidKey		m_lParentKey;
	bool		m_bVirtual;

	CFilterArgument()
	{
		m_punkArg = 0;
		m_bInArgument = true;
		m_bNameDataChanged = false;
		m_bVirtual = false;
	}

	CFilterArgument( const CFilterArgument& src)
	{
		m_strArgName = src.m_strArgName;
		m_strArgType = src.m_strArgType;
		m_punkArg = src.m_punkArg;
		m_bInArgument = src.m_bInArgument;
		m_bNameDataChanged = src.m_bNameDataChanged;
		m_lParentKey= src.m_lParentKey;
		m_bVirtual = src.m_bVirtual;

		if( m_punkArg )
			m_punkArg->AddRef();
	}


	virtual ~CFilterArgument()
	{
		if( m_punkArg )
			m_punkArg->Release();
		m_punkArg = 0;
	}
};


/*typedef list<CFilterArgument>			XArgumentList;
typedef list<CFilterArgument>::iterator	XArgumentIterator;*/

class std_dll CFilterBase :	public CActionBase,
							public CLongOperationImpl
{
	ENABLE_UNDO();
	ENABLE_MULTYINTERFACE();
public:
	CTypedPtrList<CPtrList, CFilterArgument*>	
							m_arguments;
	bool					m_bInitialized;
public:
	CFilterBase();
	virtual ~CFilterBase();

	bool IsPreviewMode()	const		{return m_bPreviewMode;}
protected:
//action's virtuals
	virtual bool DoUndo();
	virtual bool DoRedo();
	virtual bool IsAvaible();
	virtual bool Invoke();
//filter's virtuals - should be overrided in derived classes
	virtual bool InvokeFilter();	//here filter should preform work
	virtual bool CanDeleteArgument( CFilterArgument *pa );	//return true if argument should be deleted from NamedData
									//default implementation allways returns true
	virtual bool NeedActivateResult( CFilterArgument *pa );	//return true if result should be activated
	virtual IUnknown *GetObjectArgument( const char *szType, int nCurrentPos, IDataContext2 *punkContext );
	//we place all the created object to NamedData here
	virtual void DoInitializePreview();
	virtual void DoFinalizePreview();
	virtual bool StartNotification( int nCYCount, int nStageCount = 1, int nNotifyFreq = 10 );	
	virtual bool FinishNotification();

	virtual bool DoResetData();

	// override if need
	virtual CString GetBaseKeyParamName();
	virtual void MakeModified();


	virtual bool IsAllArgsRequired(){return true;};

	virtual bool IsObjectMatch(IUnknown* punkObj){return true;};


//implementation helpers
protected:
	virtual IUnknown *GetDataArgument( const char *szArgName = 0 );
	virtual IUnknown *GetDataResult( const char *szArgName = 0 );
//internal helpers
protected:
	bool _Init();
	bool _Init1();
	bool _DeInit();
	bool _PlaceArguments( bool bDirection = true, bool bRemoveOnly = false );
	IUnknown *_GetActiveContext( bool bAddRef = false );
	IUnknown *_CreateArgument( CFilterArgument *parg );

	void ContextNotification(long lCod);
	void _SetArgumentName( CFilterArgument *parg );

	POSITION _FindArgumentByKey(GuidKey key, bool bInArgs, bool bOutArgs);
	void _MakeFilterArgumentsArray(CArray<POSITION,POSITION&> &arrArgs);
protected:
	bool	m_bNotifyBegin;
	bool	m_bNotifyFinished;
	bool	m_bInsideInvokeFilter;
	bool	m_bLockChangeData;
	bool	m_bPreviewMode;

	// base & source keys & object's type
	GuidKey m_lBaseKey;
//	CString m_strBaseType;

	DECLARE_INTERFACE_MAP();

	BEGIN_INTERFACE_PART_EXPORT(Filter, IFilterAction2)
		com_call GetFirstArgumentPosition( long *pnPos );
		com_call GetNextArgument( IUnknown **ppunkDataObject, long *pnPosNext );
		com_call GetArgumentInfo( long lPos, BSTR *pbstrArgType, BSTR *pbstrArgName, BOOL *pbOut );
		com_call SetArgument( long lPos, IUnknown *punkDataObject, BOOL bDataChanged );
		com_call InitArguments();
		com_call LockDataChanging( BOOL bLock );
		com_call SetPreviewMode( BOOL bEnter );
		com_call CanDeleteArgument( IUnknown *punk, BOOL *pbCan );
		com_call NeedActivateResult( IUnknown *punk, BOOL *pbCan );
	END_INTERFACE_PART(Filter)

	
};



#endif //__filterbase_h__