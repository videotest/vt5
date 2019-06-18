#ifndef __popupint_h__
#define __popupint_h__

interface ISubMenu : public IUnknown
{	
	com_call SetStartItemID( UINT uiItemID) = 0;
	com_call SetSingleObjectName( BSTR bstrObjectName ) = 0;
	com_call GetFirstItemPos( long* plPos ) = 0;
	com_call GetNextItem(	
			UINT* puiFlags, 
			UINT* puiItemID, 
			BSTR* pbstrText,			
			UINT* puiParentID,
			long* plPos ) = 0;	
	com_call OnCommand( UINT uiCmd ) = 0;	
};


declare_interface( ISubMenu, "1247D9BB-EDF7-43f9-972F-3A14DC66FBAC");

#endif __popupint_h__
