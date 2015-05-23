#ifndef __fftactions_h__
#define __fftactions_h__

//[ag]1. classes

#define DECLARE_AVAIBLE() \
bool IsAvaible() \
{ \
	CChechAvailable avail; \
	return avail.IsAvaible(); \
} \

#define DECLARE_AVAIBLE2() \
bool IsAvaible() \
{ \
	CChechAvailable avail; \
	return avail.IsAvaible( "Gray" ); \
} \


struct CChechAvailable
{
	bool IsAvaible( CString strConvertor = "Complex" );
};

class CActionFourierHilight  : public CFilterBase
{
	DECLARE_DYNCREATE(CActionFourierHilight)
	GUARD_DECLARE_OLECREATE(CActionFourierHilight)
	ENABLE_UNDO();
public:
	CActionFourierHilight();
	virtual ~CActionFourierHilight();
public:
//update interface
	virtual bool InvokeFilter();
	virtual void ChangeColor( color *pdest, color *psrc );
	//virtual bool IsAvaible();
	//virtual bool DoUndo();
	//virtual bool DoRedo();
	DECLARE_AVAIBLE();
	//CObjectListUndoRecord	m_changes;
};

class CActionFourierRemove : public CActionFourierHilight
{
	DECLARE_DYNCREATE(CActionFourierRemove)
	GUARD_DECLARE_OLECREATE(CActionFourierRemove)
public:
	virtual void ChangeColor( color *pdest, color *psrc );
	DECLARE_AVAIBLE();
};


class CActionFourierBack : public CFilterBase
{
	DECLARE_DYNCREATE(CActionFourierBack)
	GUARD_DECLARE_OLECREATE(CActionFourierBack)

public:
	CActionFourierBack();
	virtual ~CActionFourierBack();
	DECLARE_AVAIBLE();

public:
	//virtual IUnknown *GetObjectArgument( const char *szType, int nCurrentPos, IDataContext2 *punkContext );
	virtual bool InvokeFilter();
//update interface
	//virtual bool IsAvaible();
	//virtual bool IsChecked();
};

class CActionFourier : public CFilterBase//, public INotify
{
	DECLARE_DYNCREATE(CActionFourier)
	GUARD_DECLARE_OLECREATE(CActionFourier)

public:
	CActionFourier();
	virtual ~CActionFourier();
public:
	//virtual void Notify( int n );
	//virtual IUnknown *GetObjectArgument( const char *szType, int nCurrentPos, IDataContext2 *punkContext );
	virtual bool InvokeFilter();
//update interface
	DECLARE_AVAIBLE2();

	//virtual bool IsAvaible();
	//virtual bool IsChecked();
};

class CActionPreformFFT : public CActionBase
{
	DECLARE_DYNCREATE(CActionPreformFFT)
	GUARD_DECLARE_OLECREATE(CActionPreformFFT)

public:
	CActionPreformFFT();
	virtual ~CActionPreformFFT();

public:
	virtual bool Invoke();
//update interface
//	virtual bool IsAvaible();
	DECLARE_AVAIBLE();
	virtual bool IsChecked();
};


#endif //__fftactions_h__
