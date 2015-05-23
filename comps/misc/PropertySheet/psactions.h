#ifndef __psactions_h__
#define __psactions_h__

//[ag]1. classes

class CActionShowOptions : public CActionBase
{
	DECLARE_DYNCREATE(CActionShowOptions)
	GUARD_DECLARE_OLECREATE(CActionShowOptions)

public:
	CActionShowOptions();
	virtual ~CActionShowOptions();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};


#endif //__psactions_h__
