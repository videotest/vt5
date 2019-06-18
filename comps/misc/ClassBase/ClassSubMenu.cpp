#include "classsubmenu.h"
#include "misc_utils.h"
#include "core5.h"
#include "data5.h"
#include "docview5.h"
#include "class_utils.h"

CClassSubMenu::CClassSubMenu(void)
: m_bShowUnknown(false)
{
	m_uiStartItemID = -1;
}

CClassSubMenu::~CClassSubMenu(void)
{
}

//////////////////////////////////////////////////////////////////////
HRESULT CClassSubMenu::SetStartItemID( UINT uiItemID )
{
	m_uiStartItemID = uiItemID;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CClassSubMenu::SetSingleObjectName( BSTR bstrObjectName )
{
	//!!! ISendToManagerPtr ptrS2( ::GetAppUnknown() );
	//!!! ptrS2->SetSingleObjectName( bstrObjectName );	

	return S_OK;
}



//////////////////////////////////////////////////////////////////////
HRESULT CClassSubMenu::GetFirstItemPos( long* plPos )
{
	if(plPos==0) return E_INVALIDARG;


	m_bShowUnknown = ::GetPrivateProfileInt("Classes", "AppointUnknownClass", 0, full_classifiername(NULL));
	long lClass=0;
	*plPos = lClass+1;
	
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CClassSubMenu::GetNextItem(	
										UINT* puiFlags, 
										UINT* puiItemID, 
										BSTR* pbstrText, 							
										UINT* puiParentID,
										long* plPos 
										)
{
	if(plPos==0) return E_INVALIDARG;

	long nClassCount = class_count();
	long lClass = *plPos-1;
	(*plPos)++;
	if(m_bShowUnknown){
		if(lClass>=nClassCount)
		{
			*plPos=0;
			lClass=-1;
		}
	}else{
		if(lClass>=nClassCount-1)
		{
			*plPos=0;
		}
	}
	_bstr_t bstrClassName = get_class_name(lClass);
	*puiFlags		= MF_STRING | MF_ENABLED;
	*puiItemID		= m_uiStartItemID + lClass + 1;
	*pbstrText		= SysAllocString( bstrClassName );
	*puiParentID	= -1;
	
	return S_OK;
}

static void set_object_class_with_undo(long lclass, bool bDeselect=false)
{
	char sz_cl[60];
	_itoa(lclass, sz_cl, 10);
	if(bDeselect) strcat(sz_cl,",\"\", 1"); //если  надо - задеселектить
	::ExecuteAction("SetClass",sz_cl,0);
}

//////////////////////////////////////////////////////////////////////
HRESULT CClassSubMenu::OnCommand( UINT uiCmd )
{
	long lClass = uiCmd - m_uiStartItemID - 1;
	set_object_class_with_undo(lClass);

	return S_OK;
}
