#ifndef __convert_h__
#define __convert_h__

//[ag]1. classes

class CActionConvertImage : public CFilterBase
{
	DECLARE_DYNCREATE(CActionConvertImage)
	GUARD_DECLARE_OLECREATE(CActionConvertImage)

public:
	CActionConvertImage();
	virtual ~CActionConvertImage();

public:
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa );	//return true if argument should be deleted from NamedData
	virtual IUnknown *GetObjectArgument( const char *szType, int nCurrentPos, IDataContext2 *punkContext );
};

class CActionToGrayAvail : public CActionBase
{
	DECLARE_DYNCREATE(CActionToGrayAvail)
	GUARD_DECLARE_OLECREATE(CActionToGrayAvail)
public:
	CActionToGrayAvail();
	virtual ~CActionToGrayAvail();

	virtual bool IsAvaible();
	virtual bool Invoke();
};

#endif //__convert_h__
