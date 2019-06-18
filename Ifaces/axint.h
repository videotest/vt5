#ifndef __axint_h__
#define __axint_h__

interface IActiveXForm : public IUnknown
{
	com_call SetTitle( BSTR bstr ) = 0;
	com_call GetTitle( BSTR *pbstr ) = 0;
	com_call SetSize( SIZE size ) = 0;
	com_call GetSize( SIZE *psize ) = 0;

	com_call AutoUpdateSize() = 0;

	com_call GetScript( IUnknown **ppunkScript ) = 0;
	com_call SetScript( IUnknown *punkScript ) = 0;
};

interface IActiveXCtrl : public IUnknown
{
	com_call GetProgID( BSTR *pbstrProgID ) = 0;
	com_call SetProgID( BSTR bstrProgID ) = 0;
	com_call GetRect( RECT *prect ) = 0;
	com_call SetRect( RECT rect ) = 0;
	com_call GetStyle( DWORD *pdwStyle ) = 0;
	com_call SetStyle( DWORD dwStyle ) = 0;
	com_call GetControlID( UINT *pnID ) = 0;
	com_call SetControlID ( UINT nID ) = 0;
	com_call GetObjectData( BYTE **ppbyte, DWORD *pdwMemSize ) = 0;
	com_call SetObjectData( BYTE *pbyte, DWORD dwMemSize ) = 0;
	com_call GetName( BSTR	*pbstrName ) = 0;
	com_call SetName( BSTR	bstrName ) = 0;
	com_call IsUniqueName( BSTR	bstrName, BOOL* pbUniqueName ) = 0;
	com_call GenerateUniqueName( BSTR bstrOfferName, BSTR* pbstrNewName ) = 0;
};

interface ILayoutView : public IUnknown
{
	com_call GetControlObject( IUnknown *punkAXCtrl, IUnknown **ppAX ) = 0;	//return ActiveX unknown by ActiveX data object unknwn	
	com_call GetFormOffset( POINT *ppoint ) = 0;
};

interface IVtActiveXCtrl : public IUnknown
{
	com_call SetSite( IUnknown *punkVtApp, IUnknown *punkSite ) = 0;
	com_call GetName( BSTR *pbstrName ) = 0;	//such as Editor1	
};

interface IVtActiveXCtrl2 : public IVtActiveXCtrl
{
	com_call SetDoc( IUnknown *punkDoc ) = 0;
	com_call SetApp( IUnknown *punkVtApp ) = 0;
};

interface IVtActiveXCtrl3 : public IVtActiveXCtrl2
{
	com_call OnOK( ) = 0;
	com_call OnApply( ) = 0;
	com_call OnCancel( ) = 0;
};


interface IAXInsertPropPage : public IUnknown
{
	com_call GetCurProgID( BSTR *pbstrProgID ) = 0;	
	com_call UnCheckCurrentButton() = 0;
	com_call CheckCurrentButton() = 0;
	com_call OnActivateView() = 0;
};

interface IAXSite : public IUnknown
{
	com_call ExecuteAction( BSTR bstrActionName, BSTR bstrActionParam, DWORD dwFlags ) = 0;
};



interface IReportCtrl : public IUnknown
{
	com_call IsViewCtrl( BOOL* bResult ) = 0;
	com_call GetDesignOwnerPage( int* pnPage ) = 0;
    com_call SetDesignOwnerPage( int nPage ) = 0;

	com_call GetDesignColRow( int* nCol, int* nRow ) = 0;
    com_call SetDesignColRow( int nCol, int nRow ) = 0;

	com_call GetDesignPosition( RECT *pRect ) = 0;
    com_call SetDesignPosition( RECT Rect ) = 0;

	com_call GetPrevFormPosition( RECT *pRect ) = 0;
    com_call SetPrevFormPosition( RECT Rect ) = 0;

	com_call GetPreviewColRow( int* nCol, int* nRow ) = 0;
    com_call SetPreviewColRow( int nCol, int nRow ) = 0;

	com_call GetPreviewPosition( RECT *pRect ) = 0;
    com_call SetPreviewPosition( RECT Rect ) = 0;

	com_call GetHorzScale( BOOL *pbHorzScale ) = 0;
    com_call SetHorzScale( BOOL bHorzScale  ) = 0;

	com_call GetVertScale( BOOL *pbVertScale ) = 0;
    com_call SetVertScale( BOOL bVertScale   ) = 0;

	com_call GetDPI( double *pfDPI ) = 0;
    com_call SetDPI( double fDPI   ) = 0;

	com_call GetZoom( double *pfZoom ) = 0;
    com_call SetZoom( double fZoom   ) = 0;

	com_call GetFlag( DWORD *pdwFlag ) = 0;
    com_call SetFlag( DWORD   dwFlag ) = 0;

	com_call GetCloneObject( BOOL *pbCloneObject ) = 0;
    com_call SetCloneObject( BOOL bCloneObject ) = 0;

};

interface IReportForm : public IUnknown
{

	com_call GetPaperName( BSTR *pbstrProgID ) = 0;
	com_call SetPaperName( BSTR bstrProgID ) = 0;

	com_call GetPaperOrientation(int* pnPaperOriantation) = 0;
    com_call SetPaperOrientation(int nPaperOriantation) = 0;

	com_call GetPaperSize( SIZE* pSize ) = 0;
	com_call SetPaperSize( SIZE size ) = 0;

	com_call GetPaperField( RECT *pRect ) = 0;
	com_call SetPaperField( RECT rect ) = 0;	
		
	com_call GetPaperAlign( RECT *pRect ) = 0;
	com_call SetPaperAlign( RECT rect ) = 0;	

	com_call GetMeasurementSystem(int* pnSystem) = 0;
    com_call SetMeasurementSystem(int nSystem) = 0;	

	com_call GetDesignCurentPage(int* pnCurPage) = 0;
    com_call SetDesignCurentPage(int nCurPage) = 0;

	com_call GetDesignPageCount(int* pnPageCount) = 0;
	com_call SetDesignPageCount(int pnPageCount) = 0;

	com_call GetMode(int* nMode) = 0;

	com_call GetSimpleMode(BOOL* pbSimpleMode) = 0;
	com_call SetSimpleMode(BOOL  bSimpleMode) = 0;	


	com_call GetUsePageNum(BOOL* pbUsePageNum) = 0;
	com_call SetUsePageNum(BOOL  bUsePageNum) = 0;	

	com_call GetVertPageNumPos(int* pnVertPos) = 0;
	com_call SetVertPageNumPos(int  nVertPos) = 0;	

	com_call GetHorzPageNumPos(int* pnHorzPos) = 0;
	com_call SetHorzPageNumPos(int  nHorzPos) = 0;	

	com_call GetPageNumFont(LOGFONT* pLFONT) = 0;
	com_call SetPageNumFont(LOGFONT  LFONT) = 0;	

	com_call SetMode(int nMode ) = 0;

	com_call GetBuildMode(int* pnMode) = 0;
	com_call SetBuildMode(int  nMode) = 0;	


};

		   

interface IVTPrintCtrl : public IUnknown
{
	com_call Draw( HDC hDC, SIZE sizeVTPixel ) = 0;
	com_call FlipHorizontal(  ) = 0;
	com_call FlipVertical(  ) = 0;
	com_call SetPageInfo( int nCurPage, int nPageCount ) = 0;
	com_call NeedUpdate( BOOL* pbUpdate ) = 0;
};


enum ObjectTransformation
{
	otGrowTwoSide,
	otGrowHorz,
	otGrowVert,
	otStretch
};

enum ObjectDefinition
{
	odActiveView,
	odActiveObject,
	odFixedObject
};

interface IViewCtrl : public IUnknown
{
	com_call GetApp(IUnknown** ppunkApp) = 0;
	com_call GetDoc(IUnknown** ppunkDoc) = 0;

	//Auto build mode
	com_call GetAutoBuild( BOOL* pbAutoBuild ) = 0;
	com_call SetAutoBuild( BOOL bAutoBuild ) = 0;

	com_call Build( BOOL* pbSucceded ) = 0;

	//size
	com_call GetDPI( double* pfDPI ) = 0;
	com_call SetDPI( double fDPI ) = 0;

	com_call GetZoom( double* pfZoom ) = 0;
	com_call SetZoom( double fZoom ) = 0;

	com_call GetObjectTransformation( short* pnObjectTransformation ) = 0;
	com_call SetObjectTransformation( short nObjectTransformation ) = 0;

	//active view
	com_call GetUseActiveView( BOOL* pbUseActiveView ) = 0;
	com_call SetUseActiveView( BOOL bUseActiveView ) = 0;

	com_call GetViewAutoAssigned( BOOL* pbViewAutoAssigned ) = 0;
	com_call SetViewAutoAssigned( BOOL bViewAutoAssigned ) = 0;

	com_call GetViewProgID( BSTR* pbstrProgID ) = 0;
	com_call SetViewProgID( BSTR bstrProgID ) = 0;

	//object list
	com_call GetFirstObjectPosition( long* plPos ) = 0;
	com_call GetNextObject( BSTR* pbstrObjectName, BOOL* pbActiveObject, 
							BSTR* bstrObjectType, long *plPos ) = 0;
	com_call InsertAfter( long lPos,  
							BSTR bstrObjectName, BOOL bActiveObject, 
							BSTR bstrObjectType ) = 0;
	com_call EditAt( long lPos,  
							BSTR bstrObjectName, BOOL bActiveObject, 
							BSTR bstrObjectType ) = 0;
	com_call RemoveAt( long lPos ) = 0;
	
	com_call DestroyView() = 0;
};




declare_interface( IActiveXForm, "96E3B927-EAD9-44de-8DDD-2B373AD97A72" );
declare_interface( IActiveXCtrl, "1FAC1F81-7A31-48f2-9C90-7ACB1920A091" );
declare_interface( ILayoutView, "B2FE136D-21F5-4245-AF21-038240F4A812" );
declare_interface( IVtActiveXCtrl, "0C5F2B56-D5A8-4aea-BF04-95D30495B47B" );
declare_interface( IAXInsertPropPage, "F855E011-D94C-11d3-A096-0000B493A187" );
declare_interface( IAXSite, "E700BD37-AF72-4e6e-9D90-11504568EC68" );
declare_interface( IReportForm, "21D6DF97-BBBA-404b-ABBF-6872B7DC0702" );
declare_interface( IReportCtrl, "050645BF-63EB-4eea-8A1C-382644ABCAB9" );
declare_interface( IVTPrintCtrl, "E69689EF-216B-42d3-B139-C033A6D758F2" );
declare_interface( IVtActiveXCtrl2, "BEEA90B3-57AE-4e28-9883-03FFA85D21FE" );
declare_interface( IVtActiveXCtrl3, "FB2B7AE2-ABC8-413a-A3FB-1A0E927A3E6C" );
declare_interface( IViewCtrl, "2AA4231E-03F3-4b76-B6D3-6AC5DE21AA04");

#endif // __axint_h__