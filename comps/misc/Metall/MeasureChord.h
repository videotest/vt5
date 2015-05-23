#pragma once
#include "\vt5\common2\action_main.h"
#include "resource.h"
#include "image5.h"

class CMeasureChord :
	public CAction,
	public _dyncreate_t<CMeasureChord>
{
public:
	route_unknown();
	CMeasureChord(void);
	~CMeasureChord(void);
	com_call DoInvoke();
	virtual bool IsAllArgsRequired()	{return false;};
	IUnknown *GetContextObject(_bstr_t cName, _bstr_t cType);
	int CalcThreshold(color **srcRows, byte **maskRows, int cx, int cy);
};

//---------------------------------------------------------------------------
class CMeasureChordInfo : 
public _ainfo_t<ID_ACTION_MEASURECHORD, _dyncreate_t<CMeasureChord>::CreateObject, 0>,
public _dyncreate_t<CMeasureChordInfo>
{
	route_unknown();
public:
	CMeasureChordInfo() { }
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};
