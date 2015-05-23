#ifndef __sewpage_h__
#define __sewpage_h__

#include "win_page.h"
#include "com_main.h"
#include "misc_utils.h"
#include "resource.h"
#include "sew_actions.h"
#include "ax_events.h"
#include "impl_help.h"


#import "stdole2.tlb" rename_namespace("OcxSpace") exclude("GUID", "DISPPARAMS", "EXCEPINFO", \
	"OLE_XPOS_PIXELS", "OLE_YPOS_PIXELS", "OLE_XSIZE_PIXELS", "OLE_YSIZE_PIXELS", "OLE_XPOS_HIMETRIC", \
	"OLE_YPOS_HIMETRIC", "OLE_XSIZE_HIMETRIC", "OLE_YSIZE_HIMETRIC", "OLE_XPOS_CONTAINER", \
	"OLE_YPOS_CONTAINER", "OLE_XSIZE_CONTAINER", "OLE_YSIZE_CONTAINER", "OLE_HANDLE", "OLE_OPTEXCLUSIVE", \
	"OLE_CANCELBOOL", "OLE_ENABLEDEFAULTBOOL", "FONTSIZE", "OLE_COLOR", \
	"IUnknown", "IDispatch", "IEnumVARIANT", "IFont", "IPicture")

//ViewAX button referense
#import <VTControls.ocx> rename_namespace("OcxSpace") 

using namespace OcxSpace; 



class CSewPropertyPage : public ComObjectBase, 
							public CAxEventMap,
							public CPageImpl, 
							public CHelpInfoImpl,
							public IPersist,
							public _dyncreate_t<CSewPropertyPage>
{
	virtual UINT idd()
	{	return IDD_SEW_PAGE;}
public:
	CSewPropertyPage();
	~CSewPropertyPage();
protected:
	route_unknown();

	IUnknown *DoGetInterface( const IID &iid );

	com_call GetClassID( CLSID *pclsid );
//
	virtual bool DoInitDialog();
	virtual void DoCommand( UINT nCmd );
	virtual LRESULT DoNotify( UINT nID, NMHDR *phdr );
	virtual CWinImpl *GetTarget();

	void DoChangeOverlayMode( UINT nShowMode );

	virtual void UpdateControls();
	virtual void ReturnFocus();

	void OnClickCopy();
	void OnClickAdd();
	void OnClickSub();
	void OnClickNext();

	IInteractiveSewAction	*GetAction();
protected:
	DWORD	m_dwMode;

	_DPushButtonPtr	m_ptrButtonCopy;
	_DPushButtonPtr	m_ptrButtonNew;
	_DPushButtonPtr	m_ptrButtonAdd;
	_DPushButtonPtr	m_ptrButtonSub;
};

#endif //__sewpage_h__