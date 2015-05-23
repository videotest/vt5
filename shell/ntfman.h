#ifndef __ntfman_h__
#define __ntfman_h__

class CLongOperationListener
{
public:
	CLongOperationListener( IUnknown *punkLongOperation, CTypedPtrList<CPtrList, IUnknown*>& listPermanents );
	~CLongOperationListener();

	bool IsEmpty()	const {return m_listViews.GetCount()==0;}
public:
	void RouteMessage();
	void RouteTerminateMessage();

	CTypedPtrList<CPtrList, IUnknown*>	&GetList(){return m_listViews;}
protected:
	void _AddListenersToSite( IUnknown *punkSite );

	ILongOperationPtr	m_ptrLongOperation;
	CTypedPtrList<CPtrList, IUnknown*>	m_listViews;
};

class CNotifyManager : public CCmdTargetEx
{
public:
	CNotifyManager();
	~CNotifyManager();
public:	
	void ProcessMessage( IUnknown *punkOperation, LongOperationAction loa );

	void _AddOperation( IUnknown *punkOperation );
	void _RemoveOperation( IUnknown *punkOperation, bool bSuccess = true );
	POSITION	_GetOperationPos( IUnknown *punkOperation );



	DECLARE_INTERFACE_MAP();

	BEGIN_INTERFACE_PART(Man, IOperationManager2)
		com_call RegisterPermanent( IUnknown *punk );
		com_call UnRegisterPermanent( IUnknown *punk );
		com_call TerminateCurrentOperation(long lTerminateFlag);
	END_INTERFACE_PART(Man)

protected:
	CMapPtrToPtr	m_mapListeners;
	CTypedPtrList<CPtrList, CLongOperationListener*>	
					m_listListeners;
	CTypedPtrList<CPtrList, IUnknown*>
					m_listPermanents;
	long m_lTerminateFlag;

};

extern CNotifyManager	g_NtfManager;


#endif // __ntfman_h__