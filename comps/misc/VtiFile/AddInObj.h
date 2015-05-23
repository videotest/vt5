#ifndef __AddInObj_h__
#define __AddInObj_h__


//implemeneted in VideoTesT
interface IObjHint :public IUnknown
{
	virtual HRESULT __stdcall GetActions( DWORD *pdwAction ) = 0;
	virtual HRESULT __stdcall GetRect( RECT *prect ) = 0;
	virtual HRESULT __stdcall GetPos( int *piPos ) = 0;
	virtual HRESULT __stdcall GetManual( BOOL *pbManual ) = 0;
	virtual HRESULT __stdcall GetCode1( DWORD *pwdCode1 ) = 0;
	virtual HRESULT __stdcall GetCode2( DWORD *pwdCode2 ) = 0;
	virtual HRESULT __stdcall GetRecreateIcon( BOOL *pbRecreateIcon ) = 0;
	virtual HRESULT __stdcall GetRecalcSize( BOOL *pbRecalcSize ) = 0;
};

interface IObjArrayHint : public IUnknown
{
	virtual HRESULT __stdcall GetObjArray( IUnknown **parr ) = 0;
};

interface IImageHint : public IUnknown
{
	virtual HRESULT __stdcall GetImage( LPBITMAPINFOHEADER *ppbi ) = 0;
	virtual HRESULT __stdcall GetBinImage( LPBITMAPINFOHEADER *ppbi ) = 0;

	virtual HRESULT __stdcall IsImgChanged( BOOL *pbIs ) = 0;
	virtual HRESULT __stdcall IsBinChanged( BOOL *pbIs ) = 0;

	virtual HRESULT __stdcall GetRect( RECT *prc ) = 0;
};
//data exchange interfaces 

interface IContour: public IUnknown
{
public:
	virtual HRESULT __stdcall Size( int *pisize ) = 0;
	virtual HRESULT __stdcall MakeEmpty() = 0;
	virtual HRESULT __stdcall IsEmpty( BOOL *pbIs ) = 0;

	virtual HRESULT __stdcall GetPoint( int idx, POINT *pt ) = 0;
	virtual HRESULT __stdcall SetPoint( int idx, POINT pt )= 0;
	virtual HRESULT __stdcall AddPoint( POINT pt ) = 0;
	virtual HRESULT __stdcall AddPointDirect( POINT pt ) = 0;
	virtual HRESULT __stdcall AddPointLine( POINT pt ) = 0;
};

interface IContourArray : public IUnknown
{
public:
	virtual HRESULT __stdcall Size( int *pisize ) = 0;

	virtual HRESULT __stdcall GetContour( int idx, IUnknown **pcntr ) = 0;
	virtual HRESULT __stdcall SetContour( int idx, IUnknown **pcntr ) = 0;
	virtual HRESULT __stdcall AddContour( IUnknown *pcntr ) = 0;

	virtual HRESULT __stdcall CreateNewContour( IUnknown **pcntr ) = 0;
};

interface IObject: public IUnknown
{
public:
	virtual HRESULT __stdcall SetVal( WORD wKey, double fValue ) = 0;
	virtual HRESULT __stdcall GetVal( WORD wKey, double *pfValue ) = 0;

	virtual HRESULT __stdcall GetContourArray( IUnknown **parray ) = 0;
	virtual HRESULT __stdcall SetContourArray( void *parray ) = 0;

	virtual HRESULT __stdcall GetBinBI( LPBITMAPINFOHEADER *ppbi ) = 0;
	virtual HRESULT __stdcall GetImgBI( LPBITMAPINFOHEADER *ppbi ) = 0;
	virtual HRESULT __stdcall GetIconBI( LPBITMAPINFOHEADER *ppbi ) = 0;
	virtual HRESULT __stdcall SetBinBI( LPBITMAPINFOHEADER ppbi ) = 0;
	virtual HRESULT __stdcall SetImgBI( LPBITMAPINFOHEADER ppbi ) = 0;


	virtual HRESULT __stdcall GetObjAngle( double *pf ) = 0;
	virtual HRESULT __stdcall SetObjAngle( double f ) = 0;

	virtual HRESULT __stdcall GetCurrentClbr( double *pfByX, double *pfByY ) = 0;

	virtual HRESULT __stdcall IsManual( BOOL *pbIs ) = 0;
	virtual HRESULT __stdcall GetClass( int *piClass ) = 0;
	virtual HRESULT __stdcall GetRect( LPRECT prect ) = 0;
	virtual HRESULT __stdcall SetClass( int iKey, BOOL bDirect ) = 0;

	//some global support
	virtual HRESULT __stdcall GetParamCount( int *picount ) = 0;
	virtual HRESULT __stdcall GetParamKey( int ipos, WORD *pwKey ) = 0;
	virtual HRESULT __stdcall GetParamName( int ipos, BSTR *pbstr ) = 0;
	virtual HRESULT __stdcall IsParamEnabled( WORD wKey ) = 0;

	virtual HRESULT __stdcall SetUserData( BSTR lpcszName, LPCVARIANT pcvarData ) = 0;
	virtual HRESULT __stdcall GetUserData( BSTR lpcszName, LPVARIANT pvarData ) = 0;
};

interface IObject2 : public IObject
{
public:
	virtual HRESULT __stdcall CloneObject(IObject2 **ppCopy, BOOL bCopyContent) = 0;
	virtual HRESULT __stdcall FreeObject() = 0;
	virtual HRESULT __stdcall MoveTo(int x, int y) = 0;
};

interface IObject3 : public IObject2
{
public:
	virtual HRESULT __stdcall GetObjectCode(__int64 *pnCode) = 0;
};

interface IObjArray : public IUnknown
{
public:
	virtual HRESULT __stdcall Size( int *pisize ) = 0;
	virtual HRESULT __stdcall GetObject( int idx, IUnknown **pobj ) = 0;
	virtual HRESULT __stdcall SetObject( int idx, void *pobj ) = 0;
	virtual HRESULT __stdcall AddObject( int idx, void *pobj ) = 0;
};

// Caution!!! These function can be used only to manipulate objects, created in vt4.exe !!!
interface IObjArray2 : public IObjArray
{
public:
	virtual HRESULT __stdcall SetObject2(int idx, IObject2 *pobj) = 0; // GetObject and IObject2::CloneObject give 
	virtual HRESULT __stdcall AddObject2(IObject2 *pobj) = 0; // IUnknown(IObject)*. It's difficult cast it to void*
	virtual HRESULT __stdcall CloneArray(IObjArray **pCopy, BOOL bCopyContent) = 0; // Make objarray same type and with same content
	virtual HRESULT __stdcall AddArray(IObjArray *pAdd) = 0; // Add copies of all objects
	virtual HRESULT __stdcall FreeArray() = 0;
};

interface IDocData: public IUnknown
{
public:
	virtual HRESULT __stdcall GetObjArray( IUnknown **parr ) = 0;
	virtual HRESULT __stdcall GetBinBI( LPBITMAPINFOHEADER *ppbi ) = 0;
	virtual HRESULT __stdcall GetImgBI( LPBITMAPINFOHEADER *ppbi ) = 0;

	virtual HRESULT __stdcall GetSelectedObject( int *pi ) = 0;
	virtual HRESULT __stdcall SetSelectedObject( int i ) = 0;

	virtual HRESULT __stdcall GetClassCount( int *pipos ) = 0;
	virtual HRESULT __stdcall GetClassName( int ipos, BSTR *pbstr ) = 0;
	
//support 
//	"Gallery bk"
//	"Binary bk"
//	"Binary"
//	"Contour"
	virtual HRESULT __stdcall GetVtColor( BSTR bstr, DWORD *pdw ) = 0;
};

const char	szColorGalBk[] = "Gallery bk",
	szColorBinBk[] = "Binary bk",
	szColorBinFr[] = "Binary",
	szColorCont[] = "Contour";
	
interface IDocData2: public IDocData
{
public:
	virtual HRESULT __stdcall SetObjArray( IUnknown *parr, BOOL bDoMeas, BOOL bRecreateBinImage, BSTR bstrOpName ) = 0;
	virtual HRESULT __stdcall SetBinBI( LPBITMAPINFOHEADER lpbi, BSTR bstrOpName ) = 0;
	virtual HRESULT __stdcall SetImgBI( LPBITMAPINFOHEADER lpbi, BSTR bstrOpName ) = 0;
	virtual HRESULT __stdcall PrepareUndo( unsigned nFlags, BSTR bstrOpName, DWORD *pdwIndex ) = 0;
	virtual HRESULT __stdcall MakeUndo( DWORD dwIndex ) = 0;
	virtual HRESULT __stdcall ReleaseUndo( DWORD dwIndex ) = 0;
	virtual HRESULT __stdcall GetApplication( IUnknown **ppApplication ) = 0;
	virtual HRESULT __stdcall GetAutoUpdateFlag( BOOL *pb ) = 0;
	virtual HRESULT __stdcall SetAutoUpdateFlag( BOOL b ) = 0;
};

//implemented in CustomView! dll
interface IViewData: public IUnknown
{
public:
//attach to documant
	virtual HRESULT _stdcall AttachDocumentData( IUnknown *pdata ) = 0;
//some virtuals
	virtual HRESULT _stdcall  OnCreate( HANDLE hWnd ) = 0;
	virtual HRESULT _stdcall  OnDestroy() = 0;
	virtual HRESULT _stdcall  OnInitialUpdate( IUnknown *punknwn ) = 0;
//zooming
	virtual HRESULT _stdcall  ZoomIn() = 0;
	virtual HRESULT _stdcall  ZoomOut() = 0;
	virtual HRESULT _stdcall  SetZoom( double fZoom ) = 0;
	virtual HRESULT _stdcall  GetZoom( double *pfZoom ) = 0;

	virtual HRESULT _stdcall  OnHScroll( UINT nPos ) = 0;
	virtual HRESULT _stdcall  OnVScroll( UINT nPos ) = 0;
	virtual HRESULT _stdcall  GetSize( SIZE *psize ) = 0;
//mouse
	virtual HRESULT _stdcall  OnLButtonDown( POINT pt, DWORD dwFlags ) = 0;
	virtual HRESULT _stdcall  OnLButtonUp( POINT pt, DWORD dwFlags ) = 0;
	virtual HRESULT _stdcall  OnLButtonDblClick( POINT pt, DWORD dwFlags ) = 0;
	virtual HRESULT _stdcall  OnRButtonDown( POINT pt, DWORD dwFlags ) = 0;
	virtual HRESULT _stdcall  OnRButtonUp( POINT pt, DWORD dwFlags ) = 0;
	virtual HRESULT _stdcall  OnRButtonDblClick( POINT pt, DWORD dwFlags ) = 0;
	virtual HRESULT _stdcall  OnMouseMove( POINT pt, DWORD dwFlags ) = 0;
//keyboard
	virtual HRESULT _stdcall  OnKeyDown( UINT nKey ) = 0;
	virtual HRESULT _stdcall  OnKeyUp( UINT nKey ) = 0;
	virtual HRESULT _stdcall  OnChar( UINT nChar ) = 0;
//message and command function
	virtual HRESULT _stdcall  OnCommand( UINT nCmd ) = 0;
	virtual HRESULT _stdcall  OnMessage(UINT message, WPARAM wParam, LPARAM lParam) = 0;
//drawing and printing
	virtual HRESULT _stdcall  OnDraw( HDC hdc ) = 0;
	virtual HRESULT _stdcall  OnPrint( HDC hdc, RECT *prect ) = 0;
//updaticn mech
	virtual HRESULT _stdcall  OnUpdate( IUnknown *punknown ) = 0;
	virtual HRESULT _stdcall  OnTimer( UINT nIDEvent ) = 0;
};

/*interface IView: public IUnknown
{
public:
	virtual HRESULT __stdcall Update( IUnknown * ) = 0;
};*/

interface IDocumentData: public IUnknown
{
//storage
public:
	virtual HRESULT _stdcall  DefaulInit() = 0;
	virtual HRESULT _stdcall  StoreToStorage( IStream *pstream ) = 0;
	virtual HRESULT _stdcall  ReadFromStorage( IStream *pstream ) = 0;
	virtual HRESULT _stdcall  OnUpdate( IUnknown *punknown ) = 0;
//view
public:
	virtual HRESULT _stdcall  CreateViewData( void **pData ) = 0;
};


interface IAppData: public IUnknown
{
//storage
public:														
	virtual HRESULT _stdcall  DefaulInit() = 0;
	virtual HRESULT _stdcall  StoreToStorage( IStream *pstream ) = 0;
	virtual HRESULT _stdcall  ReadFromStorage( IStream *pstream ) = 0;
public:
	virtual HRESULT _stdcall  GetModuleName( char **ppname ) = 0;

	virtual HRESULT _stdcall  CreateDocumentData( void **pdata ) = 0;
	virtual HRESULT _stdcall  OnUpdate( IUnknown *punknown ) = 0;
};

// {BACF8390-C48C-11d1-B8CC-00403338C443}
extern GUID IID_IAppData;
// {BACF8391-C48C-11d1-B8CC-00403338C443}
extern GUID IID_IDocumentData;
// {BACF8392-C48C-11d1-B8CC-00403338C443}
extern GUID IID_IViewData;
// {BACF8393-C48C-11d1-B8CC-00403338C443}
extern GUID IID_IObject;

// {BACF8394-C48C-11d1-B8CC-00403338C443}
extern GUID IID_IObjArray;
// {BACF8395-C48C-11d1-B8CC-00403338C443}
extern GUID IID_IContour;
// {BACF8396-C48C-11d1-B8CC-00403338C443}
extern GUID IID_IContourArray;
// {BACF8397-C48C-11d1-B8CC-00403338C443}
extern GUID IID_IObjHint;
// {BACF8398-C48C-11d1-B8CC-00403338C443}
extern GUID IID_IObjArrayHint;
// {BACF8399-C48C-11d1-B8CC-00403338C443}
extern GUID IID_IImageHint;
// {BACF839A-C48C-11d1-B8CC-00403338C443}
extern GUID IID_IDocData;
// {BACF839B-C48C-11d1-B8CC-00403338C443}
//extern GUID IID_IView;
// {B8A8A0B1-489F-11D3-BC33-000000000000}
extern GUID IID_IDocData2;
// {B8A8A0B5-489F-11D3-BC33-000000000000}
extern GUID IID_IObjArray2;
// {B8A8A0B6-489F-11D3-BC33-000000000000}
extern GUID IID_IObject2;
// {89E76716-7F25-4272-A748-32809D374C2A}
extern GUID IID_IObject3;


//interfaces ØÂ


#endif //__AddInObj_h__