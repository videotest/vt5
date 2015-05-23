#include "action_filter.h"
#include "action_interactive.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"
#include "measure5.h"


#if !defined(AFX_ErectChrom_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_ErectChrom_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CErectChromFilter :
public CAction,
public CUndoneableActionImpl,
public CLongOperationImpl,
public _dyncreate_t<CErectChromFilter>
{
public:
	route_unknown();
public:
	CErectChromFilter();
	virtual ~CErectChromFilter();
	virtual IUnknown *DoGetInterface( const IID &iid );
	com_call DoInvoke();
	com_call DoUndo();
	com_call DoRedo();
	bool ErectOneChrom(IMeasureObjectPtr chrom);
protected:
	IUnknownPtr get_document();
	//virtual bool InvokeFilter();
private:
	_list_t2 <IUnknown*, FreeReleaseUnknown> m_undoI;
	_list_t2 <IUnknown*, FreeReleaseUnknown> m_undoC;
	_list_t2 <IUnknown*, FreeReleaseUnknown> m_undoO;
};

class CErectChromOn :
public CAction,
public CUndoneableActionImpl,
public _dyncreate_t<CErectChromOn>
{
public:
	route_unknown();
public:
	CErectChromOn();
	virtual ~CErectChromOn();
	virtual IUnknown *DoGetInterface( const IID &iid );
	com_call DoInvoke();
	com_call DoUndo();
	com_call DoRedo();
	com_call GetActionState( DWORD *pdwState );
	virtual bool IsAvaible();
protected:
	_list_t2 <IUnknown*, FreeReleaseUnknown> m_undoChrom;
	_list_t2 <IUnknown*, FreeReleaseUnknown> m_undoChromConv;
	IUnknownPtr m_ptrObjectList;
};

class CErectOneChromOnOff :
public CInteractiveAction,
public _dyncreate_t<CErectOneChromOnOff>
{
public:
	route_unknown();
public:
	CErectOneChromOnOff();
	virtual ~CErectOneChromOnOff();
	virtual IUnknown *DoGetInterface( const IID &iid );
	com_call DoInvoke();
	com_call GetActionState( DWORD *pdwState );
	virtual bool Initialize();
	virtual void DeInit();
	virtual bool Finalize();
	virtual bool IsAvaible();
	virtual bool DoLButtonDown( _point  point );
	_gdi_t<HCURSOR> m_hCurActive;
protected:
	bool activate_chromosome( _point  pt );
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
};


/////////////////////////////////////////////////////////////////////////////

class CErectChromOnInfo : public _ainfo_t<ID_ACTION_ERECTCHROMON, _dyncreate_t<CErectChromOn>::CreateObject, 0>,
public _dyncreate_t<CErectChromOnInfo>
{
	route_unknown();
public:
	CErectChromOnInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	//virtual _bstr_t target() {return "anydoc";}
	static arg	s_pargs[];
};

class CErectOneChromOnOffInfo : public _ainfo_t<ID_ACTION_ERECTONECHROMONOFF, _dyncreate_t<CErectOneChromOnOff>::CreateObject, 0>,
public _dyncreate_t<CErectOneChromOnOffInfo>
{
	route_unknown();
public:
	CErectOneChromOnOffInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	//virtual _bstr_t target() {return "anydoc";}
	static arg	s_pargs[];
};

class CErectChromInfo : public _ainfo_t<ID_ACTION_ERECTCHROM, _dyncreate_t<CErectChromFilter>::CreateObject, 0>,
public _dyncreate_t<CErectChromInfo>
{
	route_unknown();
public:
	CErectChromInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target() {return "anydoc";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_ErectChrom_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
