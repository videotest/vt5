#ifndef __misc_utils_h__
#define __misc_utils_h__

#include "defs.h"
#include "window5.h"
#include "misc_classes.h"
#include "image5.h"
#include "misc5.h"
#include "data5.h"

#include <ActivScp.h>
#include <ComDef.h>
#include "script_int.h"

#define ToColor( c )	color( max( 0, min( 65535, c ) ) )
namespace misc_utils
{
	inline int Bright ( int b, int g, int r )	//функция для получения яркости
	{
		return (r*60+g*118+b*22)/200;
	}
}
using namespace misc_utils;

// Замена для некоторых стандартных функций,
// которые в либе MS реализованы с ошибкой
// (для русских букв возвращают мусор, да еще и с ассертом)
inline int __cdecl isdigit_ru(int c)
{ return isdigit(c & 255); }

inline int __cdecl isalpha_ru(int c)
{ return isalpha(c & 255); }

inline int __cdecl isalnum_ru(int c)
{ return isalnum(c & 255); }

inline int __cdecl isspace_ru(int c)
{ return isspace(c & 255); }

inline int __cdecl ispunct_ru(int c)
{ return ispunct(c & 255); }


//return the running interactive action
IUnknown *GetCurrentInteractiveAction();


//find the property page by name
IOptionsPage *FindPage( IPropertySheet *pSheet, const GUID &clsidPage, int *pnPos = 0 );
//find the property sheet dock bar
IPropertySheet *FindPropertySheet();

//get the key of the parent object
GuidKey GetKey( IUnknown *punk );
//get the key of the parent object
GuidKey GetParentKey( IUnknown *punk );
//get the flags of named object
DWORD	GetObjectFlags( IUnknown *punkObject );
//return tre object name
_bstr_t	GetName( IUnknown *punk );
//generate the unique name for data argument
_bstr_t	GenerateNameForArgument( const BSTR bstrArgName, IUnknown *punkDocData );
//set the name for object
void SetName( IUnknown *punk, const BSTR bstrName );
//create object with specified type
IUnknown	*CreateTypedObject( const BSTR bstrType );
//get the object type
_bstr_t	GetObjectType( IUnknown *punkObject );

//find object in data
IUnknown *GetObjectByKey( IUnknown *punkFrom, const GUID &guid );
//find child objects, stored by parent
IUnknown* GetChildObjectByKey(IUnknown* punkParent, GuidKey lKey);
//find object in data, include child objects, stored by parent
IUnknown* GetObjectByKeyEx(IUnknown* punkData, GuidKey lKey);
//return object using name
IUnknown *GetObjectByName( IUnknown *punkFrom, const BSTR bstrObject, const BSTR bstrType );
void DeleteObject( IUnknown *punkFrom, IUnknown *punkObject );
//return path to the object in NamedData
_bstr_t GetObjectPath( IUnknown *punkData, IUnknown *punkObj );
//delete an entry or section
bool DeleteEntry( IUnknown *punkDoc, const char *pszEntry );
//return the active object from data context
IUnknown *GetActiveObjectFromContext( IUnknown *punkContext, const char *szObjectType );
//return the active object from document
IUnknown *GetActiveObjectFromDocument( IUnknown *punkDoc, const char *szObjectType );



//convert to colorspace
void GetColors( IImage2 *pImage, color *pcolors, COLORREF cr );
//get any value from NamedData
_variant_t GetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault );
//get double value from NamedData
double GetValueDouble(IUnknown *punkDoc, const char *pszSection, const char *pszEntry, double fDefault);
//get integer value from NamedData
long GetValueInt( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lDefault );
//get string value from NamedData
_bstr_t GetValueString( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszDefault );
//get pointer value from NamedData. You should free it using "delete"
byte *GetValuePtr( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long *plSize/* = 0*/ );
//set pointer value to NamedData
void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, byte *ptr, long lSize );
//set double value to NamedData
void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, double fValue );
//set integer value to NamedData
void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lValue );
//set string value to NamedData
void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszValue );
//set any value to NamedData
void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault );
//set the color value to NamedData
void SetValueColor( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF cr );
//get the color value from NamedData
COLORREF GetValueColor( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF crDefault );
//safe change var type
HRESULT  _VarChangeType( VARIANT &var, VARTYPE lType, VARIANT *psrc = 0 );


IUnknown *GetAppUnknown();
DWORD ReportError( UINT	nIDS, ... );

inline bool CheckInterface( IUnknown *punkn, const GUID guid )
{
	if( punkn )
	{
		IUnknown	*ptest = 0;

		if( punkn->QueryInterface( guid, (void**)&ptest ) ==S_OK )
		{
			ptest->Release();
			return true;
		}
		return false;
	}
	else
		return false;
}

//find the component with given interface in component's tree
IUnknown *_GetOtherComponent( IUnknown *punkThis, const IID iid );

//Get color convertor by name;whithout addref
IUnknown* GetCCByName(const char* pszName);
_bstr_t GetCCName(IUnknown *pImage);


//coordinate mapping
#define INIT_ZOOM_SCROLL( psite )		\
	double	fZoom;						\
	_point	pointScroll;				\
	psite->GetZoom( &fZoom );			\
	psite->GetScrollPos( &pointScroll );

/*_point _client( const _point &src, IScrollZoomSite *psite );
_rect _client( const _rect &src, IScrollZoomSite *psite );
_point _window( const _point &src, IScrollZoomSite *psite );
_rect _window( const _rect &src, IScrollZoomSite *psite );*/
/////////////////////////////////////////////////////////////////////////////////////////
//coordinate mapping
inline _point _client( const _point &src, IScrollZoomSite *psite )
{
	INIT_ZOOM_SCROLL( psite );
	return _point( 
				int( (src.x+pointScroll.x)/fZoom ), 
				int( (src.y+pointScroll.y)/fZoom ) );
}

inline _point _window( const _point &src, IScrollZoomSite *psite )
{
	INIT_ZOOM_SCROLL( psite );
	return _point( 
				int( src.x*fZoom+.5-pointScroll.x ), 
				int( src.y*fZoom+.5-pointScroll.y ) );
}

inline _rect _client( const _rect &src, IScrollZoomSite *psite )
{
	INIT_ZOOM_SCROLL( psite );
	return _rect( 
				int( (src.left+pointScroll.x)/fZoom ), 
				int( (src.top+pointScroll.x)/fZoom ), 
				int( (src.right+pointScroll.x)/fZoom ), 
				int( (src.bottom+pointScroll.y)/fZoom ) );
}

inline _rect _window( const _rect &src, IScrollZoomSite *psite )
{
	INIT_ZOOM_SCROLL( psite );
	return _rect( 
				int( src.left*fZoom+.5-pointScroll.x ), 
				int( src.top*fZoom+.5-pointScroll.y ),
				int( src.right*fZoom+.5-pointScroll.x ), 
				int( src.bottom*fZoom+.5-pointScroll.y ) );
}


int VTMessageBox( LPCSTR lpText, LPCSTR lpCaption, UINT uType );
int VTMessageBox( UINT uiResID, HMODULE hInst, LPCSTR lpCaption, UINT uType );

class CHourglass
{
	 HCURSOR         m_hCur;
public:
	CHourglass();
	virtual ~CHourglass();

};

bstr_t LoadStringFromStream( IStream *pStream );
bool StoreStringToStream( IStream *pStream, bstr_t& bstr );


bool HelpDisplayTopic( const char *pszFileName, const char *pszPrefix, const char *pszName, const char *pszWinType );
bool HelpDisplay( const char *pszFileName );


inline char *local_filename( char *psz_dest, const char *psz_src, int cb )
{
	GetModuleFileName( 0, psz_dest, cb );
	strcpy( strrchr( psz_dest, '\\' )+1, psz_src );
	return psz_dest;
}
//get image pane count
int GetImagePaneCount( IUnknown *punkImage );
bool ExecuteScript( BSTR bstrScript, const char* psz_name = 0, bool bFromActionManager = false, GuidKey key = GuidKey(), DWORD dwFlags = 0);
//execute action
bool ExecuteAction( const char *szActionName, const char *szActionparam = 0, DWORD dwFlags = 0 );

//Write BSTR
HRESULT WriteBSTR( IStream* pi_stream, BSTR bstr );
//Read BSTR
HRESULT ReadBSTR( IStream* pi_stream, BSTR* pbstr );

//Display win 32 error
void DislayWin32Error( DWORD dw_error, HWND hwnd, const char* psz_title );

bool RecursiveWriteEntry( INamedData* ptrSrc, INamedData* ptrTarget, LPCSTR pszEntry, bool bOvewrite );
bool CopyObjectNamedData( IUnknown* punk_src, IUnknown* punk_target, LPCSTR pszEntry, bool bOvewrite );
void CopyPropertyBag(  IUnknown* punk_src, IUnknown* punk_target, bool bOvewrite );

#endif // __misc_utils_h__