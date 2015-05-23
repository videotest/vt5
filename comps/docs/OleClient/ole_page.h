#ifndef __ole_page_h__
#define __ole_page_h__

#include "win_page.h"
#include "com_main.h"
#include "misc_utils.h"
#include "resource.h"
#include "impl_help.h"


class COlePropertyPage : public ComObjectBase, 							
							public CPageImpl, 
							public CHelpInfoImpl,
							public IPersist,
							public CNamedObjectImpl,
							public _dyncreate_t<COlePropertyPage>
{
	virtual UINT idd()
	{	return IDD_OLE_PROP_PAGE;}
public:
	COlePropertyPage();
	~COlePropertyPage();
protected:
	route_unknown();

	IUnknown *DoGetInterface( const IID &iid );

	com_call GetClassID( CLSID *pclsid );
//
	virtual bool DoInitDialog();
	virtual void DoCommand( UINT nCmd );	

};


#endif//__ole_page_h__
