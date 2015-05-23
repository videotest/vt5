#ifndef __ARRANGE_DLG_H__
#define __ARRANGE_DLG_H__

#include "win_dlg.h"
#include "resource.h"
#include "misc_list.h"


class CArrangeDlg : public ComObjectBase,
					public CDialogImpl,
					public _dyncreate_t<CArrangeDlg>
{
	route_unknown();
public:
	CArrangeDlg(IUnknown* punkClassList);
	~CArrangeDlg();
	virtual bool DoInitDialog();
	virtual UINT idd() {return IDD_DIALOG_ARRANGE_CLASSES;}
	virtual void DoCommand( UINT nCmd );


protected:
	INamedDataObject2Ptr	m_sptrClassList;
	_list_t2<GuidKey*>		m_listClassesKeys;
};

#endif