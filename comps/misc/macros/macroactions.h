#ifndef __macroactions_h__
#define __macroactions_h__

//[ag]1. classes

class CActionMacroRecord : public CActionBase
{
	DECLARE_DYNCREATE(CActionMacroRecord)
	GUARD_DECLARE_OLECREATE(CActionMacroRecord)

public:
	CActionMacroRecord();
	virtual ~CActionMacroRecord();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();
};


#endif //__macroactions_h__
