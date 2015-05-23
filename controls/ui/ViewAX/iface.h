#pragma once

interface IViewAXPropBag : public IUnknown
{
	com_call SetProperty( BSTR bstrName, VARIANT var ) = 0;
	com_call GetProperty( BSTR bstrName, VARIANT* pvar ) = 0;
};
declare_interface( IViewAXPropBag, "00251CCA-D000-4a2b-B729-848ABD7006BF" );
