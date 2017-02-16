#pragma once

#include "\vt5\common2\com_unknown.h"
#include "\vt5\awin\misc_list.h"
#include "resource.h"

#include "test_man.h"
#include "test_defs.h"

#include "data_main.h"

#include "idispatchimpl.h"

#import "testman.tlb" no_namespace raw_interfaces_only named_guids 

#include <atlstr.h>
#include "/vt5/awin/misc_list.h"
#include "PropBagImpl.h"

#define FUNC_CMP_NAME			"test_array_item_cmp"
#define	FUNC_CMP_BEGIN			"Function " FUNC_CMP_NAME "( old_val, new_val ) \n"
#define	FUNC_CMP_END			"\nEnd Function"

class CTestDataArray : public CObjectBase, 
				  public ITestDataArray,
				  public CNamedPropBagImpl,
				  public IDispatchImpl< ITestDataArrayDisp >
{
public:
	class  XTestArrayItemInfo
	{	 
	public:
	    _variant_t	m_varData;
        _bstr_t		m_bstrName;
        _bstr_t		m_bstrScriptCmp; // выражение на языке VBScript для сравнения 

		XTestArrayItemInfo(void);
		~XTestArrayItemInfo(void);

		bool Compare( _variant_t var_val );
		HRESULT	Serialize( IStream *pStream, bool bstoring );
	};

public:
	CTestDataArray(void);
	~CTestDataArray(void);

	route_unknown();
	virtual IUnknown *DoGetInterface( const IID &iid );

	// interface ITestDataArray
	com_call AddItem( /*[in]*/ VARIANT vData, /*[in]*/ BSTR bstrName);
	com_call GetItem( /*[in]*/ UINT nID, /*[out]*/ VARIANT *lpvRetVal);
	com_call GetItemCount( /*[out]*/ long *lplRet);
	com_call RemoveItem( /*[in]*/ UINT nID);
	com_call RemoveAll();
	com_call GetItemName( /*[in]*/ UINT nID, /*[out]*/ BSTR *pbstrName);
	com_call SetItemCmpExpr( /*[in]*/ UINT nID, /*[in]*/ BSTR bstrCmpExpr);
	com_call GetItemCmpExpr( /*[in]*/ UINT nID, /*[out]*/ BSTR *pbstrCmpExpr);
	com_call CompareItem( /*[in]*/ UINT nID, /*[in]*/ VARIANT vCmpData, /*[out]*/ long *plEqual );

    /////////////////////////////////////////////////  
	//IPersist helper
	virtual GuidKey GetInternalClassID(){ return clsidTestDataArray; }

	/////////////////////////////////////////////////
	//ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );

private:
	static void _free_test_item_info( XTestArrayItemInfo *pdata ) { delete pdata; }
	_list_t<XTestArrayItemInfo *, _free_test_item_info> m_items;

	TPOS FindItem(UINT nID);
};

//////////////////////////////////////////////////////////////////////
//
// class CTestDataArrayInfo
//
//////////////////////////////////////////////////////////////////////
class CTestDataArrayInfo : public CDataInfoBase,
					public _dyncreate_t<CTestDataArrayInfo>
{
public:
	CTestDataArrayInfo()	: CDataInfoBase( clsidTestDataArray, szTypeTestDataArray, 0, IDI_TESTDATAARRAY )
	{	}
};
