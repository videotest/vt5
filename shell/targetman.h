#ifndef __TargetMan_h__
#define __TargetMan_h__

struct XUnk
{
	IUnknown	*punk;
	bool		bEnable;
};

enum TargetNotifyCodes
{
	tncActivate,
	tncAdd,
	tncRemove
};

class CTargetManager : public CCmdTargetEx
{
protected:
	
	class XTargetEntry		//target entry class - for each target type
	{
		
		CString		m_strTypeName;
		CPtrList	m_listTargets;
	public:
		XTargetEntry( const char *sz );	
		virtual ~XTargetEntry();
	public:
		IUnknown *GetActiveTarget();			//return the active (head-pos)target, if exist
												
		POSITION GetFirstTargetPosition();			//walking target list
		XUnk *GetNextTarget( POSITION &pos );	//return IUnknown without AddRef
		POSITION FindTargetPos( IUnknown *punk );	//find the specified terget in target list. return IUnknown without AddRef

		bool AddTarget( IUnknown *punk );
		bool RemoveTarget( IUnknown *punk );
		bool ActivateTarget( IUnknown *punk );
		bool EnableTarget( IUnknown *punk, bool bEnable );

		
		const char *GetName(){return m_strTypeName;}
	};

	CPtrList	m_listTargetEntries;
public:
	CTargetManager();
	virtual ~CTargetManager();

	
	void Init();	//empty the current target state and register "app" terget
	void DeInit();	//de-initialize tagert manager and free memory
public:
	POSITION	GetFisrtTargetEntryPosition();				//TargetEntry list navigation
	XTargetEntry	*GetNextTargetEntry( POSITION &pos );

	XTargetEntry	*GetTargetEntry( const char *szTargetName );
public:
	void ActivateTarget( IUnknown *punk );
	IUnknown *GetTarget( const char *szName );
	IUnknown *GetTarget( GuidKey nKey );

	//several names separate by  "\n"
	void RegisterTarget( IUnknown *punk, const char *szTargName );
	void UnRegisterTarget( IUnknown *punk );
	void EnableTarget( IUnknown *punk, bool bEnable );
	void NotifyApplication( IUnknown *punk, int nCode );
protected:
	DECLARE_INTERFACE_MAP();

	BEGIN_INTERFACE_PART(Target, IActionTargetManager)
		com_call RegisterTarget( IUnknown *punkTarget, BSTR bstr );
		com_call UnRegisterTarget( IUnknown *punkTarget );
		com_call ActivateTarget( IUnknown *punkTarget );
		com_call GetTarget( BSTR bstr, IUnknown **punkTarget );
		com_call GetTargetByKey( GUID lKey, IUnknown **ppunk );
    END_INTERFACE_PART(Target)

	CMapStringToPtr	m_mapTargetCache;
};

extern CTargetManager	g_TargetManager;

#endif //__TargetMan_h__