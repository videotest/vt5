#include "stdafx.h"
#include "hook.h"

#if _MFC_VER < 0x0710
// Fixing bugs in mfc before 710, in 710 these bugs were fixed.
class COleControlFix : public COleControl{
public:
	// Data binding operations
	void BoundPropertyChanged(DISPID dispid);
	BOOL BoundPropertyRequestEdit(DISPID dispid);
};


// Calls to IPropertyNotifySink

void COleControlFix::BoundPropertyChanged(DISPID dispid)
{
	POSITION pos = m_xPropConnPt.GetStartPosition();
	LPPROPERTYNOTIFYSINK pPropNotifySink;

	while (pos != NULL)
	{
		pPropNotifySink =
			(LPPROPERTYNOTIFYSINK)m_xPropConnPt.GetNextConnection(pos);
		if(pPropNotifySink != NULL)
        	pPropNotifySink->OnChanged(dispid);
	}
}

BOOL COleControlFix::BoundPropertyRequestEdit(DISPID dispid)
{
	POSITION pos = m_xPropConnPt.GetStartPosition();
	LPPROPERTYNOTIFYSINK pPropNotifySink;

	while (pos != NULL)
	{
		pPropNotifySink =
			(LPPROPERTYNOTIFYSINK)m_xPropConnPt.GetNextConnection(pos);
		if (pPropNotifySink != NULL && pPropNotifySink->OnRequestEdit(dispid) != S_OK)
			return FALSE;
	}

	// All of the sinks said yes, so it's ok.
	return TRUE;
}

bool InstallMfcFixs()
{
	HMODULE hmod = ::GetModuleHandle( "mfc70d.dll" );
	{
		//old address
		void (COleControl::*  pint_old)(DISPID) = &COleControl::BoundPropertyChanged;	
		LPBYTE pfunc_addr_old = *(LPBYTE*)&pint_old;
		if(0xe9 == *pfunc_addr_old){
			pfunc_addr_old += *((int*)(pfunc_addr_old+1))  + 5;
		}
		if(0xFF == *pfunc_addr_old && 0x25 == *(pfunc_addr_old+1) ){
			int pb=*(int*)(pfunc_addr_old+2);
			pfunc_addr_old = (LPBYTE)(*(int*)pb);
		}
		//new address
		void (COleControlFix::*  pint_new)(DISPID) = &COleControlFix::BoundPropertyChanged;	
		LPVOID pfunc_addr_new = *(void**)&pint_new;

		if( hmod )
		{
			if( pfunc_addr_old )
				VERIFY( ::_install_function_hook( __GetOriginalCode, pfunc_addr_old, pfunc_addr_new ) );
		}
	}
	{
		//old address
		BOOL (COleControl::*  pint_old)(DISPID) = &COleControl::BoundPropertyRequestEdit;	
		LPBYTE pfunc_addr_old = *(LPBYTE*)&pint_old;
		if(0xe9 == *pfunc_addr_old){
			pfunc_addr_old += *((int*)(pfunc_addr_old+1))  + 5;
		}
		if(0xFF == *pfunc_addr_old && 0x25 == *(pfunc_addr_old+1) ){
			int pb=*(int*)(pfunc_addr_old+2);
			pfunc_addr_old = (LPBYTE)(*(int*)pb);
		}
		//new address
		BOOL (COleControlFix::*  pint_new)(DISPID) = &COleControlFix::BoundPropertyRequestEdit;	
		LPVOID pfunc_addr_new = *(void**)&pint_new;

		if( hmod )
		{
			if( pfunc_addr_old )
				VERIFY( ::_install_function_hook( __GetOriginalCode, pfunc_addr_old, pfunc_addr_new ) );
		}
	}
	return true;
}

#endif //_MFC_VER < 0x0710
