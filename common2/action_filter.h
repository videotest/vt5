#ifndef __action_filter_h__
#define __action_filter_h__

#include "action_main.h"
#include "action_undo.h"
#include "impl_long.h"


interface IDataContext2;


class CFilterArgument
{
public:
	_bstr_t		m_bstrArgName;
	_bstr_t		m_bstrArgType;
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

	/*CFilterArgument( const CFilterArgument& src)
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
	}*/


	virtual ~CFilterArgument()
	{
		if( m_punkArg )
			m_punkArg->Release();
		m_punkArg = 0;
	}
};


class CFilter :	public CAction,
					public CLongOperationImpl,
					public CUndoneableActionImpl,
					public IFilterAction2
{
public:
	CFilter();
	virtual ~CFilter();

	bool IsPreviewMode()	const		{return m_bPreviewMode;}
protected:
	virtual IUnknown *DoGetInterface( const IID &iid );

//action's virtuals
	com_call DoInvoke();
	com_call DoUndo();
	com_call DoRedo();
	com_call GetActionState( DWORD *pdwState );

	com_call GetFirstArgumentPosition( long *pnPos );
	com_call GetNextArgument( IUnknown **ppunkDataObject, long *pnPosNext );
	com_call GetArgumentInfo( long lPos, BSTR *pbstrArgType, BSTR *pbstrArgName, BOOL *pbOut );
	com_call SetArgument( long lPos, IUnknown *punkDataObject, BOOL bDataChanged );
	com_call InitArguments();
	com_call LockDataChanging( BOOL bLock );
	com_call SetPreviewMode( BOOL bEnter );
	com_call CanDeleteArgument( IUnknown *punk, BOOL *pbCan );
	com_call NeedActivateResult( IUnknown *punk, BOOL *pbCan );

	com_call AttachTarget( IUnknown *punkTarget );

	virtual bool InvokeFilter();
//filter's virtuals - should be overrided in derived classes
	
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
	virtual _bstr_t GetBaseKeyParamName();


	virtual bool IsAllArgsRequired(){return true;};

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
protected:
	bool	m_bNotifyBegin;
	bool	m_bNotifyFinished;
	bool	m_bInsideInvokeFilter;
	bool	m_bLockChangeData;
	bool	m_bPreviewMode;
	_list_ptr_t<CFilterArgument*, FreeEmpty>		
			m_arguments;
	bool	m_bInitialized;
	GuidKey m_lBaseKey;
};


bool IsPaneEnable( int nPane );

#endif //__action_filter_h__