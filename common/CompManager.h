#ifndef __CompMan_h__
#define __CompMan_h__

#include "defs.h"

//the stronge point of this class is implementing component array as 

class std_dll CCompManager 
{
protected:
	CPtrArray		m_ptrs;
	CStringArray	m_strs;
	CString		m_sGroupName;
	IUnknown	*m_punkParent;
	IUnknown	*m_punkOuterUnknown;
public:
	CCompManager( const char *szGroupName = 0, IUnknown *punkParent = 0, bool bLoadComponents = true );
	virtual ~CCompManager();
public:
	void AttachComponentGroup( const char *szGroupName );
	void SetRootUnknown( IUnknown *punkParent );
	void SetOuterUnknown( IUnknown *m_punkOuterUnknown );

	virtual bool Init();
	virtual void DeInit();

	void FreeComponents();
	bool LoadComponentsDefault();
	virtual void RemoveAllComponents();

public:
	int GetComponentCount();

	IUnknown *GetComponent( int iCompIdx, bool bAddRef = true );
	IUnknown *GetComponent( const IID iid, bool bAddRef = true );
	IUnknown *GetComponent( const char *szCompName, bool bAddRef = true );
	const char *GetComponentName( int nCompIdx );

	int AddComponent( IUnknown *punk );	//add component (don't register it
	void RemoveComponent( int idx );

	IUnknown *operator[](int idx);
protected:
	virtual void OnAddComponent( int idx ){};
	virtual void OnRemoveComponent( int idx ){};
	virtual void AfterRemoveComponent( int idx ){};
};

#define MAX_AGGR	10

class std_dll CAggrManager : public CCompManager
{
public:
	IUnknown	*m_pinterfaces[MAX_AGGR];
public:
	CAggrManager();
	virtual ~CAggrManager();

	virtual bool Init();
	virtual void DeInit();

	virtual void OnAddComponent( int idx );
	virtual void AfterRemoveComponent( int idx );
};

#endif // __CompMan_h__