#ifndef __editorint_h__
#define __editorint_h__

interface IMarker : public IUnknown
{
	com_call SetCoordsLen( int x, int y, int nLength ) = 0;
	com_call SetLength( double fLength ) = 0;
};

// vk
interface IMarker2 : public IMarker
{
	com_call SetCoordsLenPrec( int x, int y, int nLength, int nPrecision ) = 0;
	com_call GetMaxLength( int nWidth, double *pnLength ) = 0;
};
// end vk 
interface IMarker3 : public IMarker2
{
	com_call SetCoordsEnd(int x, int y, bool bInit) = 0;
	com_call SetBorderMarker(int width, int height) = 0;
};

enum ArrowFlags
{
	afFirstArrow = 1<<0,
	afFirstCircle = 1<<1,
	afLastArrow = 1<<2,
	afLastCircle = 1<<3,
};


enum DifferencesFlags
{
	dfBinary = 1<<0,
	dfInvertColor = 1<<1,
};


interface IArrow : public IUnknown
{
	com_call SetCoords( POINT pointStart, POINT pointEnd ) = 0;
	com_call GetCoords( POINT *ppointStart, POINT *ppointEnd ) = 0;
	com_call SetDrawFlags( DWORD dwFlags ) = 0;
	com_call GetDrawFlags( DWORD *pdwFlags ) = 0;
	com_call SetColor( COLORREF	cr ) = 0;
	com_call GetColor( COLORREF *pcr ) = 0;
	com_call SetSize( int nSize, int nWidth ) = 0;
	com_call GetSize( int *pnSize, int *pnWidth ) = 0;
};

interface IEditorAction : public IUnknown
{
	com_call GetImage( IUnknown **ppunkImage ) = 0;
	com_call GetDifferences( void ***pppDifferences, int *pnStart, int *pnCount, DWORD* pdwFlags ) = 0;
	com_call SetDifferences( void **ppDifferences, RECT rect, DWORD dwFlags ) = 0;
};

interface IObjectAction : public IUnknown
{
	com_call GetFirstObjectPosition(LONG_PTR *plpos) = 0;
	com_call GetNextObject(IUnknown **ppunkObject, LONG_PTR *plpos) = 0;
	com_call AddObject( IUnknown *punkObject ) = 0;
};

interface ISyncEditManager : public IUnknown
{
	com_call GetEnabled( DWORD *pdwEnable ) = 0;
	com_call SetEnabled( DWORD dwEnable ) = 0;
};

interface IChooseColor : public IUnknown
{
	com_call SetLButtonColor(OLE_COLOR color) = 0;
	com_call SetRButtonColor(OLE_COLOR color) = 0;
};

interface IFontPropPage : public IUnknown
{
	com_call GetFont(LOGFONT*	pLogFont, int* pnAlign) = 0;
	com_call GetColorsAndStuff(OLE_COLOR* pcolorText, OLE_COLOR* pcolorBack, BOOL* pbBack, BOOL* pbArrow) = 0;
	com_call SetFont(LOGFONT*	pLogFont, int nAlign) = 0;
	com_call SetColorsAndStuff(OLE_COLOR colorText, OLE_COLOR colorBack, BOOL bBack, BOOL bArrow) = 0;
};

interface ICommentObj : public IUnknown
{
	com_call GetText(BSTR *pbstrText) = 0;
	com_call SetText(BSTR bstrText) = 0;
	com_call GetCoordinates(RECT* prcText, POINT* pPtArrow, BOOL bFull = 0) = 0;
	com_call SetCoordinates(RECT rcText, POINT ptArrow) = 0;
	com_call SetEditMode(BOOL bEdit) = 0;
	com_call LockDraw(BOOL bLock) = 0;
	com_call UpdateObject() = 0;
};



declare_interface( IMarker, "549607FF-5E75-4afd-9438-C5189FB9B531" );
// vk
declare_interface( IMarker2, "E7E26C89-F340-4258-AA8F-2E78266B0081" );
// end vk
declare_interface( IMarker3, "65D9FE11-1CA4-4900-9B38-25D96B9BA6D7" );
declare_interface( IArrow, "58F83196-07E5-44c9-BC45-81AEDFB3A4FB" );
declare_interface( IEditorAction, "98BDA055-329D-4691-A9C2-6B953BA7D60B" );
declare_interface( IObjectAction, "B8057435-3D59-4174-9F7B-7F6D4EA67D4E" );
declare_interface( ISyncEditManager, "D615B4A5-8164-4994-9FFC-34DE04305FA7" );
declare_interface( IChooseColor, "D43B64E1-A19C-11d3-A539-0000B493A187" );
declare_interface( IFontPropPage, "C4B48981-A8B6-11d3-A547-0000B493A187" );
declare_interface( ICommentObj, "D7BD3FC1-935F-11d3-A52D-0000B493A187" );


#endif //__editorint_h__