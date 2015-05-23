#ifndef _ax_ctrl_misc_h
#define _ax_ctrl_misc_h

#include "core5.h"
#include "misc5.h"

interface IAXCtrlDataSite : public IUnknown
{
	com_call SetData( IUnknown* punk_data ) = 0;
	com_call GetData( IUnknown** ppunk_data ) = 0;
};

declare_interface( IAXCtrlDataSite, "A068C5E6-7FD9-4b87-9F95-D35D1ABA4CF8" )


#define szAxCtrlPropChange	"AxCtrlPropChange"

#define DECLARE_AX_DATA_SITE(class_name)							\
	IUnknownPtr	m_ptr_data;											\
BEGIN_INTERFACE_PART(AXCtrlDataSite, IAXCtrlDataSite)				\
	com_call SetData( IUnknown* punk_data );						\
	com_call GetData( IUnknown** ppunk_data );						\
END_INTERFACE_PART(AXCtrlDataSite)									\



#define IMPLEMENT_AX_DATA_SITE(class_name)							\
HRESULT class_name::XAXCtrlDataSite::SetData( IUnknown* punk_data )	\
{																	\
	METHOD_PROLOGUE_EX(class_name, AXCtrlDataSite)					\
	if( pThis->m_ptr_data )											\
		pThis->m_ptr_data = 0;										\
																	\
	pThis->m_ptr_data = punk_data;									\
																	\
	return S_OK;													\
}																	\
HRESULT class_name::XAXCtrlDataSite::GetData( IUnknown** ppunk_data )\
{																	\
	METHOD_PROLOGUE_EX(class_name, AXCtrlDataSite)					\
	if( !ppunk_data )												\
		return E_POINTER;											\
																	\
	*ppunk_data = pThis->m_ptr_data;								\
																	\
	if( pThis->m_ptr_data )											\
		pThis->m_ptr_data->AddRef();								\
																	\
	return S_OK;													\
}																	\
IMPLEMENT_UNKNOWN(class_name, AXCtrlDataSite)						\


inline void FireCtrlPropChange( IUnknown* punk_ctrl_disp )
{
	if( !punk_ctrl_disp )
		return;

	typedef  IUnknown* (*PGET_APP_UNKNOWN)(bool bAddRef /*= false*/);
	PGET_APP_UNKNOWN pGetAppUnknown	= 0;
	IUnknown* punk_app				= 0;

	HINSTANCE hDll_Common = GetModuleHandle("common.dll");

	if (hDll_Common)
	{
		pGetAppUnknown = (PGET_APP_UNKNOWN)GetProcAddress(hDll_Common, "GetAppUnknown"); 
		if (pGetAppUnknown)
			punk_app = pGetAppUnknown(false);
	}


	IApplicationPtr ptr_app = punk_app;
	if( ptr_app == 0 )		return;

	IUnknown* punk_doc = 0;
	ptr_app->GetActiveDocument( &punk_doc );
	if( !punk_doc )			return;	

	INotifyControllerPtr ptr_nc( punk_doc );
	punk_doc->Release();

	if( ptr_nc == 0 )		return;

	IAXCtrlDataSitePtr ptr_ds = punk_ctrl_disp;
	if( ptr_ds == 0 )		return;

	IUnknown* punk_data = 0;
	ptr_ds->GetData( &punk_data );
	if( !punk_data )		return;
	ptr_nc->FireEvent( _bstr_t(szAxCtrlPropChange), punk_data, ptr_nc, 0, 0 );
	punk_data->Release();	punk_data = 0;
}

inline void FirePropExchange(COlePropertyPage* pctrl )
{
	if( !pctrl )			return;

	ULONG u = 0;
	LPDISPATCH* pdisp = pctrl->GetObjectArray( &u );
	if( !pdisp || !u ) 				return;
	
	FireCtrlPropChange( pdisp[0] );
}




#endif _ax_ctrl_misc_h