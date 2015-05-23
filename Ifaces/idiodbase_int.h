#pragma once
/*
interface IIdioDBase : public IUnknown
{
	com_call AddItem( IUnknown *punkIdio ) = 0;
	com_call AddItemAfter( IUnknown *punkIdio, long lID ) = 0;
	com_call GetItem( long lID, IUnknown **punkIdio ) = 0;

	com_call SetItemText( long lID, BSTR bstr ) = 0;
	com_call GetItemText( long lID, BSTR *bstr ) = 0;

	com_call GetCount( long *lCount ) = 0;
	com_call Remove( long lID ) = 0;
	com_call RemoveAll() = 0;
};
*/
/*
interface IIdioDBaseStorage : public IUnknown
{
	com_call ReadFile( BSTR bstr )  = 0;
	com_call WriteFile( BSTR bstr ) = 0;
};


declare_interface( IIdioDBase,		  "34F5E4D5-1C86-4523-962A-395B4FA859D0" );
declare_interface( IIdioDBaseStorage, "2EAF755B-4394-41cd-A4C7-AAB114D67E4D" );
*/

interface IIdioDBaseView : public IUnknown
{
	com_call SelectByText( BSTR bstr ) = 0;
	com_call GetActiveObject( IUnknown **punk ) = 0;
	com_call EnableWindowMessages( BOOL *pbEnable ) = 0;
	com_call EnableContextMenu( BOOL bEnable ) = 0;
};
declare_interface( IIdioDBaseView,	  "313A159E-3B08-4d15-9647-337F7A185895" );
