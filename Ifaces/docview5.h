#ifndef __docview5_h__
#define __docview5_h__


enum DocTemplateFlags
{
	dtfSingleDocumentOnly = 1<<0,
	dtfReopenLastAtStartup = 1<<1,
	dtfAutoSave = 1<<2,
	dtfSDIWindow = 1<<3
};

interface IDocTemplate : public IUnknown
{
	com_call GetTemplateFlags( DWORD *pdwFlags ) = 0;

	com_call GetDocTemplString( DWORD dwCode, BSTR *pbstr ) = 0;
	com_call GetDocResource( HICON *phIcon ) = 0;
	com_call GetSplitterProgId( BSTR *pbstr ) = 0;
};

interface IShellDocTemplate : public IUnknown
{
	com_call GetFileFilterByFile(BSTR bstr, IUnknown **punk) = 0;
};


enum DocFlags
{
	dfHasOwnFormat = 1,
	dfHasOwnUndoProcessing = 0,
};

interface IDocument : public IUnknown
{
	com_call GetDocFlags( DWORD *pdwFlags ) = 0;

	com_call Init( IUnknown *punkDocSite, IUnknown *punkDocTempl, IUnknown *punkApp ) = 0;

	com_call CreateNew() = 0;
	com_call LoadNativeFormat( BSTR bstr ) = 0;
	com_call SaveNativeFormat( BSTR bstr ) = 0;
};

interface IDocumentSite : public IUnknown
{
	com_call GetDocumentTemplate( IUnknown **punk ) = 0;
	com_call GetFirstViewPosition( long *plPos ) = 0;
	com_call GetNextView( IUnknown **ppunkView, long *plPos ) = 0;
	com_call GetActiveView( IUnknown **ppunkView ) = 0;
	com_call GetPathName( BSTR *pbstrPathName ) = 0;
	com_call SetActiveView( IUnknown *punkView ) = 0;
	com_call GetDocType( BSTR *pbstrDocType ) = 0;
	com_call SaveDocument( BSTR bstrFileName ) = 0;
};

interface IDocumentSite2 : public IDocumentSite
{
	// If path name is empty, this function will not return title in pbstrPathName.
	// Title will be returned in pbstrTitle.
	com_call GetPathName2( BSTR *pbstrPathName, BSTR *pbstrTitle ) = 0;
	// During save operation this function will return new file name
	// (GetPathName and GetPathName2 will return old file name).
	com_call GetSavingPathName( BSTR *pbstrPathName ) = 0;
	// Disable close document.
	com_call LockDocument( BOOL bLock ) = 0;
	com_call IsDocumentLocked( BOOL *pbLock ) = 0;
};

enum MatchView
{
	mvNone = 0,
	mvPartial = 1<<0,
	mvFull = 1<<1,
	mvFullWithBinary = 1<<2
};

enum ObjectFlags
{
	ofNormal = 0,
	ofOnlyOneChild = 1,
	ofAllSelected = 2	// [vanek] SBT:1261 view represent data's from all selected objects - 21.12.2004
};

interface IView : public IUnknown
{
	com_call Init( IUnknown *punkDoc, IUnknown *punkSite ) = 0;
	com_call GetDocument( IUnknown **ppunkDoc ) = 0;
	com_call OnUpdate( BSTR bstr, IUnknown *punkObject ) = 0;
	com_call GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch ) = 0;
	//drag-drop extension

	com_call GetObjectFlags( BSTR bstrObjectType, DWORD *pdwFlags /*ObjectFlags*/) = 0;
	com_call DoDragDrop( DWORD *pdwDropEffect ) = 0;
	//com_call GetSelectedObject( DWORD dwFlags, IUnknown **ppunkObject ) = 0;
	//com_call SetSelectedObject( IUnknown *punkObject, CPoint point );
	com_call GetMultiFrame(IUnknown **ppunkObject, BOOL bDragFrame) = 0;
	com_call MoveDragFrameToFrame() = 0;

	com_call GetDroppedDataObject(IUnknown** punkDO) = 0;
	com_call OnActivateView( BOOL bActivate, IUnknown *punkOtherView ) = 0;

	com_call GetFirstVisibleObjectPosition( long *plpos ) = 0;
	com_call GetNextVisibleObject( IUnknown ** ppunkObject, long *plPos ) = 0;
};


interface IViewSite : public IUnknown
{
	com_call GetFrameWindow( IUnknown **ppunkFrame ) = 0;
	com_call InitView( IUnknown *punkView ) = 0;
	com_call UpdateObjectInfo() = 0;
};

interface IMultiSelection  : public IUnknown
{
	com_call SelectObject( IUnknown* punkObj, BOOL bAddToExist ) = 0;
	com_call UnselectObject( IUnknown* punkObj ) = 0;
	com_call GetObjectsCount(DWORD* pnCount) = 0;
	com_call GetObjectByIdx(DWORD nIdx, IUnknown** ppunkObj) = 0;
	com_call GetFlags(DWORD* pnFlags) = 0;
	com_call SetFlags(DWORD nFlags) = 0;
	com_call GetFrameOffset(POINT* pptOffset) = 0;
	com_call SetFrameOffset(POINT ptOffset) = 0;
	com_call GetObjectOffsetInFrame(IUnknown* punkObject, POINT* pptOffset) = 0;
	com_call RedrawFrame() = 0;
	com_call EmptyFrame() = 0;
	com_call SyncObjectsRect() = 0;
};

//the view should implement this interface if it want to support
//previewing of actions work
interface IPreviewSite : public IUnknown
{
	com_call InitPreview( IUnknown *punkLong ) = 0;
	com_call AddPreviewObject( IUnknown *punkObject ) = 0;
	com_call ProgressPreview( int nStage, int nProgress ) = 0;
	com_call TerminatePreview() = 0;
	com_call DeInitPreview() = 0;

	com_call CanThisObjectBeDisplayed( IUnknown *punk, BOOL *pbCan ) = 0;
};

interface IEasyPreview : public IUnknown
{
	com_call AttachPreviewData( IUnknown *punkND ) = 0;
	com_call PreviewReady() = 0;
};


enum ViewMode{
	vmDesign = 0,
	vmPreview = 1,
	vmPrinting = 2,
	vmNoInfo = 3,

};


interface IReportView : public IUnknown
{
	com_call GetMode( int* pnMode ) = 0;
	com_call EnterDesignMode() = 0;
	com_call EnterPreviewMode( DWORD dwFlags ) = 0;
	com_call EnterPrintMode( ) = 0;	
	com_call UpdateView() = 0;
	com_call FitToPage() = 0;
	com_call FitToPageWidth() = 0;
	com_call ScrollToPage( int nPageNum, RECT rcTarget) = 0;
	com_call SetTargetDocument( IUnknown* punkDocSite ) = 0;
	com_call GetPageCount( int* pnPageCount ) = 0;	
	com_call LeavePreviewMode( ) = 0;
	com_call OnPrinterSettingsChange() = 0;
	com_call DisconnectNotifyController( BOOL bDisconnect ) = 0;
	com_call SetReportSourceDocument( IUnknown* punkDoc ) = 0;
};

interface IReportView2 : public IReportView
{
	com_call Draw( HDC hDC ) = 0;
	com_call GetPageRect( HDC hDC, RECT *pRect ) = 0;
};
interface IReportPageView : public IUnknown
{
	com_call DrawPage( HDC hDC, int idx, float zoom ) = 0;
	com_call GetZoom(float* zoom)=0;
};

interface IPrintView : public IUnknown
{
	com_call GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX ) = 0;
	com_call GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY ) = 0;;
	com_call Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags ) = 0;
};

enum PrintViewFlags
{
	PrintView_CanSaveToImage = 1,
};

interface IPrintView2 : public IPrintView
{
	com_call GetPrintFlags(DWORD *pdwFlags) = 0;
};

interface IContextView : public IUnknown
{
	com_call GetView(IUnknown ** ppunkView) = 0;
	com_call SetView(IUnknown * punkView) = 0;
	com_call SetReadOnlyMode(BOOL bMode) = 0;
	com_call GetReadOnlyMode(BOOL * pbMode) = 0;
};

interface IViewDispatch : public IUnknown
{
	com_call GetDispIID( GUID* pguid ) = 0;
};

enum ViewFlags
{
	View_AutoActivateObjects = 1, // View itself activates needed objects
};

interface IView2 : public IView
{
	com_call GetViewFlags(DWORD *pdwViewFlags) = 0;
};

declare_interface(IViewDispatch, "61C26370-44F6-4983-B9CF-74C0709F22EE" );


declare_interface( IContextView, "C5FD2322-077C-4289-9217-DD3E98831AA6" );
declare_interface( IDocTemplate, "51CE67A3-2220-11d3-A5D6-0000B493FF1B" );
declare_interface( IShellDocTemplate, "DFFD83B1-75BF-11d3-A4FD-0000B493A187" );
declare_interface( IDocument, "51CE67A4-2220-11d3-A5D6-0000B493FF1B" );
declare_interface( IDocumentSite, "51CE67A5-2220-11d3-A5D6-0000B493FF1B" );
declare_interface( IView, "949B93F1-225E-11d3-A5D6-0000B493FF1B" );
declare_interface( IViewSite, "949B93F2-225E-11d3-A5D6-0000B493FF1B" );
declare_interface( IMultiSelection, "0EF5EA83-C9CC-11d3-A087-0000B493A187" );
declare_interface( IPreviewSite, "D0369A70-A3A5-47c8-8EA6-C7AEC4594CA0" );
declare_interface( IReportView, "DA70C27F-0DEF-473d-8E97-BB37BE5F05B5" );
declare_interface( IPrintView, "CBFC7A8A-3F9F-47bd-B1A1-BC95B0BB94D0" );
declare_interface( IEasyPreview, "101AFB9F-30D7-4e60-89B8-14A143F188A9" );
declare_interface( IReportPageView, "8BFC4A51-6CC1-45fc-9998-27F9A3981991"); 
declare_interface( IReportView2, "806DF734-B74C-405b-BFC3-087D66D52557" );
declare_interface( IPrintView2, "91F8D9B0-909B-4605-A8C0-5AD3869017FE");
declare_interface( IView2, "60DEDFBB-0F3E-4017-9A86-F75B0DE0CF9F" );
declare_interface( IDocumentSite2, "295D2E10-97A8-459c-9804-DAF37EF8691C" );
#endif //__docview5_h__