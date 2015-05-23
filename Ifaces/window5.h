#ifndef __window5_h__
#define __window5_h__

#include "defs.h"

enum WindowFlags
{
	wfPlaceInDialog = 1 << 0,
	wfZoomSupport = 1 << 1,
	wfSupportFitDoc2Scr = 1 << 2,
};

interface ISplitterTabbed : public IUnknown
{
	com_call AddView(BSTR bstrName) = 0;
	com_call RemoveActiveView() = 0;
	com_call RenameView(BSTR bstrNewName) = 0;
	com_call SaveState(IUnknown* punkND) = 0;
	com_call LoadState(IUnknown* punkND) = 0;
	com_call GetActiveViewName(BSTR* pbstrName) = 0;
};

interface IScrollZoomSite: public IUnknown
{
	com_call GetScrollBarCtrl( DWORD sbCode, HANDLE *phCtrl ) = 0;
	com_call GetClientSize( SIZE *psize ) = 0;
	com_call SetClientSize( SIZE sizeNew ) = 0;
	com_call GetScrollPos( POINT *pptPos ) = 0;
	com_call SetScrollPos( POINT ptPos ) = 0;
	com_call GetVisibleRect( RECT *pRect ) = 0;
	com_call EnsureVisible( RECT rect ) = 0;
	com_call GetZoom( double *pfZoom ) = 0;
	com_call SetZoom( double fZoom ) = 0;
	com_call SetScrollOffsets( RECT rcOffsets ) = 0;
	com_call SetAutoScrollMode( BOOL bSet ) = 0;
	com_call GetAutoScrollMode( BOOL* pbSet ) = 0;
	com_call LockScroll( BOOL bLock ) = 0;
	com_call UpdateScroll( WORD sbCode ) = 0;
};

interface IScrollZoomSite2: public IScrollZoomSite
{
	com_call GetFitDoc( BOOL* pbFitDoc ) = 0;
	com_call SetFitDoc( BOOL bFitDoc ) = 0;
};

interface IWindow: public IUnknown
{
	com_call CreateWnd( HANDLE hwndParent, RECT rect, DWORD dwStyles, UINT nID ) = 0;
	com_call DestroyWindow() = 0;
	com_call GetHandle( HANDLE *phWnd ) = 0;

	com_call HelpHitTest( POINT pt, BSTR *pbstrHelpFileName, DWORD *pdwTopicID, DWORD *pdwHelpCommand ) = 0;
	com_call PreTranslateMsg( MSG *pmsg, BOOL *pbReturn ) = 0;

	com_call GetWindowFlags( DWORD *pdwFlags ) = 0;
	com_call SetWindowFlags( DWORD dwFlags ) = 0;

	com_call PreTranslateAccelerator( MSG *pmsg ) = 0;	//S_FALSE - continue processing
};

interface IWindow2 : public IWindow
{
	com_call AttachMsgListener( IUnknown *punk ) = 0;	//unknown should support IID_IMsgListener
	com_call DetachMsgListener( IUnknown *punk ) = 0;
	com_call AttachDrawing( IUnknown *punk ) = 0;		//unknown should support IID_IDrawObject
	com_call DetachDrawing( IUnknown *punk ) = 0;
};

interface IMsgListener : public IUnknown
{
	com_call OnMessage( MSG *pmsg, LRESULT *plReturn ) = 0;
};

interface ISplitterWindow : public IUnknown
{
	com_call GetActiveView( IUnknown **punkView ) = 0;
	com_call ChangeViewType( int nRow, int nCol, BSTR bstrViewType ) = 0;
	com_call GetRowColCount( int *pnRows, int *pnCols ) = 0;
	com_call GetViewRowCol( int	nRow, int nCol, IUnknown **ppunkView ) = 0;
	com_call GetViewTypeRowCol( int	nRow, int nCol, BSTR *pbstrViewType ) = 0;
	com_call Split( int nRowCount, int nColCount ) = 0;

	com_call GetActivePane( int *piRow, int *piCol ) = 0;
	com_call GetScrollBarCtrl( IUnknown *punkView, DWORD sbCode, HANDLE *phCtrl ) = 0;

	//recalc splitter layout for optimal view
	com_call OptimizeSplitter() = 0;
};

interface IFrameWindow: public IUnknown
{
	com_call CreateView( UINT nID, BSTR bstrProgID, IUnknown **ppunkView ) = 0;
	com_call DestroyView( IUnknown	*punkView ) = 0;

	com_call GetActiveView( IUnknown **ppunkView ) = 0;

	com_call GetDocument( IUnknown **ppunkDoc ) = 0;
	com_call GetSplitter( IUnknown **ppunkSplitter ) = 0;

	com_call RecalcLayout() = 0;

	com_call Maximize() = 0;
	com_call Restore() = 0;
};

interface IFrameWindow2: public IFrameWindow
{
	// Locked flag is implemented to prevent destruction or hiding view during modal dialog 
	// (e.g. print preview). 
	com_call GetLockedFlag(BOOL *pbLocked) = 0;
	com_call SetLockedFlag(BOOL bLocked) = 0;
};

interface IMainWindow: public IUnknown
{
	//com_call ResetPagesList() = 0;
	//com_call AddSettingsPage( IUnknown *punk ) = 0;
	com_call EnterSettingsMode(HWND* hwndSettings, LONG nHeight) = 0;
	com_call LeaveSettingsMode() = 0;
	
	com_call GetFirstDockWndPosition( POSITION *plPosition ) = 0;
	com_call GetNextDockWnd(IUnknown **ppunkDock, POSITION *plPosition) = 0;
	com_call CreateDockBar(IUnknown* punk, UINT nID, HWND* pHWND) = 0;
	com_call KillDockBar(BSTR bstrName) = 0;

	com_call GetMDIClientWnd(HWND* pHwnd) = 0;

	com_call GetFirstToolbarPosition(POSITION *plPosition) = 0;
	com_call GetNextToolbar(HWND *phwnd, POSITION *plPosition) = 0;
	com_call ShowControlBar( HWND hwnd, long bShow ) = 0;
	com_call CreateDockBarEx(IUnknown* punk, UINT nID, HWND* pHWND, DWORD dwFlags/*1 - not float now*/) = 0;
	com_call IsHelpMode( BOOL *pbool ) = 0;
};

interface IDockableWindow : public IUnknown
{
	com_call GetSize( SIZE *psize, BOOL bVert ) = 0;
	com_call GetDockSide( DWORD	*pdwSite ) = 0;
	com_call GetFixed(BOOL *pbFixed) = 0; 
	com_call OnShow() = 0;
	com_call OnHide() = 0;
};


enum PaneSide
{
	psLeft = 1,
	psRight
};

interface IStatusPane : public IUnknown
{
	com_call GetWidth( int *pWidth ) = 0;
	com_call GetSide( DWORD *pdwSide ) = 0;
};

interface IOptionsPage : public IUnknown
{
	com_call GetSize( SIZE *psize ) = 0;
	com_call GetCaption( BSTR *pbstrCaption ) = 0;
	com_call OnSetActive() = 0;
	com_call LoadSettings() = 0;
	com_call StoreSettings() = 0;
};

interface ITextPage : public IUnknown
{
	com_call SetText( BSTR bstrText ) = 0;
};

interface IZoomPage : public IUnknown
{
	com_call Update( DWORD dwOptions ) = 0;
};

interface ISyncManager : public IUnknown
{
	com_call SetSyncMode( BOOL bEnterSyncMode, BOOL bRelative, BOOL bSyncZoom, IUnknown *punkSyncTargetDoc ) = 0;
	com_call GetSyncMode( BOOL *pbSyncMode, BOOL *pbSyncZoom, BOOL *pbRelative) = 0;
	com_call SetLockSyncMode(BOOL bLock) = 0;
	com_call SetSyncContext( BOOL bSync ) = 0;
	com_call GetSyncContext( BOOL *pbSync ) = 0;
};

interface IUserInterface : public IUnknown
{
	com_call GetFirstComboButton(POSITION *plPosition) = 0;
	com_call GetNextComboButton(HWND* phWnd, IUnknown** ppunkInfo, POSITION *plPosition) = 0;
};







declare_interface( IWindow, "51CE679F-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( IMainWindow, "51CE67A1-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( IFrameWindow, "51CE67A2-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( IDockableWindow, "51CE67B1-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( ISplitterWindow, "C3289811-2583-11d3-A5DA-0000B493FF1B" )
declare_interface( IScrollZoomSite, "51CE67A0-2220-11d3-A5D6-0000B493FF1B" )
declare_interface( IScrollZoomSite2, "1499BE71-295C-4b46-8640-C58EC20F83F1" )
declare_interface( IWindow2, "17889A01-48E4-11d3-A616-0090275995FE" )
declare_interface( IMsgListener, "17889A02-48E4-11d3-A616-0090275995FE" )
declare_interface( IStatusPane, "1362B111-8AE6-11d3-A523-0000B493A187" )
declare_interface( IOptionsPage, "CB6CA761-809D-11d3-A510-0000B493A187" )
declare_interface( ITextPage, "E8988901-C36D-11d3-A080-0000B493A187" )
declare_interface(ISyncManager, "011C002B-582E-4c72-A9E8-4F51A519D0A3");
declare_interface(IUserInterface, "B19D9E7E-0F7B-4dfa-A009-E8F7F9D9AEB1");
declare_interface(IZoomPage, "5161D125-ECDC-414c-8881-01D6C1C278CE");
declare_interface( ISplitterTabbed, "1D645D88-5ADF-11D3-9999-0000B493A187");
declare_interface( IFrameWindow2, "001D9E5F-A8CE-412d-8705-0554EAEE8DEC" )





#endif //__window5_h__