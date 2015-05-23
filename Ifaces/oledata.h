#ifndef __OleData_h__
#define __OleData_h__


#include "defs.h"
#include "oledlg.h"

#define szOleItemStateChange	"OleItemStateChange"

#define szTypeOleItem "OleObject"


enum OleCreationType
{
	octUndefine = 0,
	octLinkToFile = 1,
	octInsertFromFile = 2,
	octCreateNewItem = 3,
	octCreateFromClipboard = 4,
};

enum OleItemState
{
	iaNotDefined = 0,
	iaLoaded = 1,
	iaFullActivate = 2,
	iaFullDeactivate = 3,
	iaUIDeactivate = 4,
};

enum OleObjectType
{
	ootNotDefined = 0,		//Not init
	ootOleObject = 1,		//Such as media clip || wav
	ootInPlaceActivate = 2,	//PaintBrush
	ootAxDocument = 3,		//Word, Excel
};



interface IOleObjectData : public IUnknown
{
	com_call CreateItem( OLEUIINSERTOBJECT* pio ) = 0;
	com_call CreateFromClipboard( ) = 0;	
	com_call GetItemInfo( BSTR* bstrFileName, short* pCreationType, BSTR* bstrAppProgID ) = 0;	
};

interface IOleObjectUI : public IUnknown
{
	com_call Activate( HWND hWndView ) = 0;
	com_call Deactivate( HWND hWndView ) = 0;

	com_call ActivateUI( HWND hWndView ) = 0;
	com_call DeactivateUI( HWND hWndView ) = 0;

	com_call OnSize( int cx, int cy ) = 0;

	com_call IsObjectCreated( BOOL* pbCreated ) = 0;
	com_call CanProcessDrawig( HWND hWnd, BOOL* pbProcess ) = 0;
																		  //max GetScrollSize
	com_call DrawMeta( int nDrawAspect, HWND hWnd, HDC hdc, RECT rcTarget, POINT ptOffset, SIZE sizeClip ) = 0;

	com_call GetInPlaceEditHwnd( HWND* phWnd ) = 0;
	com_call GetExtent( SIZEL* psizel ) = 0;

	com_call GetState( short* pnState ) = 0;

	com_call GetScrollSize( SIZE* pSize, HWND hWnd ) = 0;

	//call by ShellFrame
	com_call OnFrameResizeBorder( RECT rcBorder ) = 0;
	com_call OnFrameActivate( BOOL bActivate ) = 0;

	//Run verb
	com_call DoVerb( UINT uiVerb ) = 0;

	com_call ForceUpdate() = 0;

	com_call OnOwnerActivate( ) = 0;
	

};


//0, 2, and 4 to reflect the number of menu elements it provided in the File, View, and Window menu groups. 
//1, 3, and 5 to indicate the number of menu items in its Edit, Object, and Help 

#define OLE_ACTION_PREFIX "OleAction_" 

#define OLE_MENU_SIZE	1

#define OLE_SECTION "\\OleClient"
#define OLE_FILE "\\OleClient\\File"
#define ID_OLE_FILE		0

#define OLE_EDIT "\\OleClient\\Edit"
#define ID_OLE_EDIT		1

#define OLE_VIEW "\\OleClient\\View"
#define ID_OLE_VIEW		2

#define OLE_OBJECT "\\OleClient\\Object"
#define ID_OLE_OBJECT	3

#define OLE_WINDOW "\\OleClient\\Window"
#define ID_OLE_WINDOW	4

#define OLE_HELP "\\OleClient\\Help"
#define ID_OLE_HELP		5


interface IOleFrame : public IUnknown
{
	com_call GetMainHwnd( HWND* phWndMain ) = 0;
    com_call ActivateOleObject( IUnknown* punkOleObject, HWND* phwndFrame ) = 0;
	com_call CreateStdOLEMenu( IUnknown* punkOleObjectData, IUnknown* punkOleObject ) = 0;	
	com_call DeactivateOleObject( IUnknown* punkOleObject ) = 0;
	//Get menu wanna see in main frame when ole object activate
    com_call GetMenu( HMENU* phMenu, BSTR* pbstrCaption/*[size - 6]*/ ) = 0;
	//Get space whithout doockbars
	com_call GetPseudoClientRect( RECT* prect, BOOL bIncludeBorders ) = 0;
	//Set space exclude OLE APP toolbars etc
	com_call SetOleBorders( LPCBORDERWIDTHS pBW ) = 0;
	//GetAccel info
	com_call GetAccelInfo( HACCEL* phaccel, UINT* pcAccelEntries ) = 0;
};


interface IOleView : public IUnknown
{
	com_call GetHwnd( HWND* phwnd ) = 0;
};





declare_interface( IOleObjectData, "1477457A-FB6B-47ff-837A-EDF67AAEA382" );
declare_interface( IOleObjectUI, "63815304-2877-4b0e-9847-BD509A3E84C2" );
declare_interface( IOleFrame, "4803A969-9FDB-43bf-81F6-1DB37B9954EE" );
declare_interface( IOleView, "41370C19-15E3-4f07-A9EB-C7EFC6DF91AC" );




#endif// __OleData_h__
