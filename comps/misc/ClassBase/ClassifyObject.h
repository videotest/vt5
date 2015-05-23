#pragma once

#include "action_filter.h"
#include "misc_utils.h"
#include "classify_int.h"

#include <atlstr.h>
#include "nameconsts.h"
#include "resource.h"

#include "classifyactionsbase.h"

#include "\vt5\awin\misc_ptr.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
class CActionClassifyObjectAll : public CAction, 
							  public CClassifyActionsBase,
							  public CUndoneableActionImpl,
							  public _dyncreate_t<CActionClassifyObjectAll>
{
protected:
	ICalcObjectPtr *m_ptrObject_;
	long *m_lClass_;
	long *m_undo_lClass_;
	_ptr_t<bool> m_bManualFlags;
	_ptr_t<bool> m_bUndoManualFlags;
	int m_nCount;
	int m_nSkipped;
public:
	route_unknown();
	CActionClassifyObjectAll()	
	{ 
		m_ptrObject_ = 0;
		m_lClass_ = 0;
		m_undo_lClass_ = 0;

		m_nCount = 0;
	}
	~CActionClassifyObjectAll()
	{
		if( m_ptrObject_ )
			delete []m_ptrObject_;

		if( m_lClass_ )
			delete []m_lClass_;

		if( m_undo_lClass_ )
			delete []m_undo_lClass_;
	}
public:
	com_call DoInvoke();
	com_call DoUndo();
	com_call DoRedo();

	com_call GetActionState( DWORD *pdwState );
	IUnknown* DoGetInterface( const IID &iid )
	{
		if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
		return __super::DoGetInterface( iid );
	}
protected:
	bool _is_avaible();
	CString _get_value_string( CString strFileName, CString strSection, CString strKey )
	{
		TCHAR szT[4096];
		DWORD dw = ::GetPrivateProfileString( strSection, strKey,
			"", szT, sizeof( szT ) / sizeof( szT[0] ), strFileName );
		if( dw > 0 )
			return szT;
		return "";
	}
}; 
//---------------------------------------------------------------------------
class CActionClassifyObjectAllInfo : public _ainfo_t<ID_ACTION_CLASSIFY_OBJECT_ALL, _dyncreate_t<CActionClassifyObjectAll>::CreateObject, 0>,
public _dyncreate_t<CActionClassifyObjectAllInfo>
{
public:
    route_unknown();
	CActionClassifyObjectAllInfo() {}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};
/////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SUPPORT_COMPOSITE
#define SUPPORT_COMPOSITE

class CSupportComposite
{
	int m_nPurpose;
	ICompositeObjectPtr m_ptrComp;
	bool m_bComp;
public:
	CSupportComposite(IUnknown* unk)
	{
		m_bComp = false;
		m_ptrComp = unk;

		if(m_ptrComp)
		{		
			long bComp;
			m_ptrComp->IsComposite(&bComp);
			if(!bComp)
			{
				m_ptrComp->SetPurpose(0);
				return;
			}
			m_bComp = bComp!=0;
			m_ptrComp->BuildTree(-1);
			m_ptrComp->GetPurpose(&m_nPurpose);
			m_ptrComp->SetPurpose(1);
		}
	};
	~CSupportComposite()
	{
		if(m_ptrComp) m_ptrComp->SetPurpose(m_nPurpose);
	};
	ICompositeObject* operator->()
	{
		return m_ptrComp;
	}
	bool IsComposite()
	{
		return m_bComp;
	}
	operator INamedDataObject2*()
	{
		INamedDataObject2Ptr ndo(m_ptrComp);
		return ndo;
	}

	
};
#endif //SUPPORT_COMPOSITE