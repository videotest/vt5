#include "stdafx.h"
#include "misc_utils.h"
#include "misc5.h"
#include "data5.h"
#include "stdio.h"
#include "com_main.h"
#include "helper.h"
#include "core5.h"
#include "image5.h"
#include "docview5.h"
#include "action5.h"
#include "nameconsts.h"
#include "misc_list.h"
#include "PropBag.h"
#include "alloc.h"

#include "dump_util.h"
#define SIMPLE_TYPES_DEFINED
#include "\vt5\awin\misc_dbg.h"

class time_test2
{
public:
	time_test2( const _char *psz=0, bool report=true )
	{
		m_psz = _tcsdup( psz );
		m_dw = 0;
		m_dw_last = ::GetTickCount();
		m_report = report;
		m_calls = 0;
	}
	~time_test2()
	{
		if( m_report )
			report();
		if( m_psz )
			::free( m_psz );
	}

	void start()
	{
		m_dw -= GetTickCount();
		m_calls++;
	}

	void stop()
	{
		DWORD dw = GetTickCount();
		m_dw += dw;
		if(dw-m_dw_last>5000)
		{ // каждые 5 секунд - промежуточный итог
			m_dw_last = dw;
			report();
		}
	}

	void report()
	{
		DWORD	dw = m_dw;

		if( m_psz )_trace_file( sz_profiler_log, _T("[%s:%d] %d (id=%x)"), m_psz, m_calls, dw, this );
		else _trace_file( sz_profiler_log, _T("[unknown:%d] %d (id=%x)"), m_calls, dw, this );
	}

public:
	DWORD	m_dw;
	DWORD	m_dw_last;
	bool	m_report;
	_char	*m_psz;
	int		m_calls;
};

class time_test2_trigger
{
public:
	time_test2_trigger(time_test2* p) : m_p(p)
	{
		m_p->start();
	};

	~time_test2_trigger()
	{
		m_p->stop();
	};

	time_test2* m_p;
};

class CSaveSetupSection
{ // класс, занимающийся исключительно тем, что сохраняет текущую секцию, а при смерти - восстанавливает ее
public:
	CSaveSetupSection(IUnknown* punkND)
	{
		m_sptrND = punkND;
		if(m_sptrND!=0) m_sptrND->GetCurrentSection(&m_bstrSection.GetBSTR());
	}
	~CSaveSetupSection()
	{
		if(m_sptrND!=0) m_sptrND->SetupSection(m_bstrSection);
	}
private:
	INamedDataPtr m_sptrND;
	_bstr_t m_bstrSection;
};

HRESULT  _VarChangeType( VARIANT &var, VARTYPE lType, VARIANT *psrc )
{
	if( !psrc )
		psrc = &var;

	long lSrcType = psrc->vt;

	if( lSrcType == VT_R4 && lType == VT_BSTR )
	{
		char	sz[100];
		sprintf( sz, "%f", psrc->fltVal );
		_bstr_t	bstr = sz;
		VariantClear( &var );
		var.vt = VT_BSTR;
		var.bstrVal = bstr.copy();
		return S_OK;
	}
	if( lSrcType == VT_R8 && lType == VT_BSTR )
	{
		char	sz[100];
		sprintf( sz, "%g", psrc->dblVal );
		_bstr_t	bstr = sz;
		VariantClear( &var );
		var.vt = VT_BSTR;
		var.bstrVal = bstr.copy();
		return S_OK;
	}

	if( lSrcType == VT_BSTR && lType == VT_R4 )
	{
		_bstr_t str = psrc->bstrVal;
		VariantClear( &var );
		if( sscanf( str, "%f", &var.fltVal ) != 1 )
			return E_INVALIDARG;
		var.vt = VT_R4;
		return S_OK;
	}
	if( lSrcType == VT_BSTR && lType == VT_R8 )
	{
		_bstr_t str = psrc->bstrVal;
		VariantClear( &var );
		if( sscanf( str, "%lf", &var.dblVal ) != 1 )
			return E_INVALIDARG;
		var.vt = VT_R8;
		return S_OK;
	}

	return VariantChangeType( &var, psrc, 0, lType );
}



GuidKey GetKey( IUnknown *punk )
{
	if( !punk )
		return INVALID_KEY;
	INumeredObject	*punkNum = 0;
	if( punk->QueryInterface( IID_INumeredObject, (void**)&punkNum ) != S_OK )
		return INVALID_KEY;

	GuidKey	key;
	punkNum->GetKey( &key );
	punkNum->Release();

	return key;
}

GuidKey GetParentKey( IUnknown *punk )
{
	IUnknown	*punkParent = 0;

	INamedDataObject2Ptr	ptrNamedDataObject( punk );
	if( ptrNamedDataObject == 0 )
		return INVALID_KEY;

	ptrNamedDataObject->GetParent( &punkParent );
//[AY] 05.02.2002 - IRooted removed
/*	IRootedObjectPtr	ptrRooted( punk );
	if( ptrRooted == 0 )
		return INVALID_KEY;
	
	ptrRooted->GetParent( &punkParent );*/
	if( !punkParent )
		return INVALID_KEY;

	GuidKey	key = GetKey( punkParent );
	punkParent->Release();

	return key;
}

//return the object parent
IUnknown *GetParentUnk( IUnknown *punkO ) // without addref
{
	IUnknown	*punkParent = 0;

	INamedDataObject2Ptr	ptrNamedDataObject( punkO );
	if( ptrNamedDataObject == 0 )
		return 0;

	ptrNamedDataObject->GetParent( &punkParent );

	if( punkParent )
		punkParent->Release();

	return punkParent;
}


DWORD	GetObjectFlags( IUnknown *punkObject )
{
	if( !punkObject )
		return 0;
	INamedDataObject2	*punkNamedDataObject = 0;

	if( punkObject->QueryInterface( IID_INamedDataObject2, (void **)&punkNamedDataObject ) != S_OK )
		return 0;

	DWORD	dwFlags = 0;
	punkNamedDataObject->GetObjectFlags( &dwFlags );
	punkNamedDataObject->Release();

	return dwFlags;
}

_bstr_t	GetName( IUnknown *punk )
{
	if( !punk )
		return "";
	INamedObject2	*punkNamedObject = 0;
	if( punk->QueryInterface( IID_INamedObject2, (void **)&punkNamedObject ) != S_OK )
		return "";

	BSTR	bstr = 0;
	punkNamedObject->GetName( &bstr );
	punkNamedObject->Release();

	_bstr_t	ret = bstr;

	if( bstr )
		::SysFreeString( bstr );	bstr = 0;	

	return ret;
}

void SetName( IUnknown *punk, const BSTR bstrName )
{
	if( !punk )
		return;
	INamedObject2	*punkNamedObject = 0;
	if( punk->QueryInterface( IID_INamedObject2, (void **)&punkNamedObject ) != S_OK )
		return;

	punkNamedObject->SetName( bstrName );
	punkNamedObject->Release();
}

static int get_tail_num(const char *s)
{ // получить написанный в конце строки номер
	int	nPos = strlen(s)-1;
	while( nPos>0 && isdigit_ru(s[nPos-1]) ) nPos--; // нашли начало номера
	return atoi(s+nPos);
};

static void set_tail_num(char *s, int n)
{ // проставить номер (внимание! строка может удлиниться - нужно место в буфере)
	unsigned	nPos = strlen(s);
	while( nPos>0 && isdigit_ru(s[nPos-1]) ) s[--nPos]='0'; // нашли начало номера, попутно забили его нулями
	int len=0;
	for(int n1=n; n1!=0; n1/=10) len++; // len - число цифр в номере, 0 и больше
	nPos = max(nPos, strlen(s)-len);
	if(n) _itoa(n, s + nPos, 10);
};

static bool str_eq_wo_tail_num(const char *s1, const char *s2)
{ // сравнить две строки без учета номеров; true - одинаковы
	while( *s1 && *s1==*s2 ) { s1++; s2++; }
	while( *s1 ) if( !isdigit_ru(*(s1++)) ) return false;
	while( *s2 ) if( !isdigit_ru(*(s2++)) ) return false;
	return true;
};

_bstr_t	GenerateNameForArgument( const BSTR bstrArgName, IUnknown *punkDocData )
{
	_bstr_t	strArgName = bstrArgName;

	if( !punkDocData )
		return "";

	IDataTypeManager	*punkDataTypeManager = 0;
	if( punkDocData->QueryInterface( IID_IDataTypeManager, (void**)&punkDataTypeManager ) != S_OK )
		return "";

	long	lTypesCount = 0, lType;
	punkDataTypeManager->GetTypesCount( &lTypesCount );

	int	nMaxNum = get_tail_num(strArgName);

	for( lType = 0; lType < lTypesCount; lType++ )
	{
		LONG_PTR	lpos  = 0;
		punkDataTypeManager->GetObjectFirstPosition( lType, &lpos  );

		while( lpos  )
		{
			IUnknown	*punkObject = 0;
			punkDataTypeManager->GetNextObject( lType, &lpos , &punkObject );

			_bstr_t	strName = ::GetName( punkObject );
			punkObject->Release();

			if( str_eq_wo_tail_num( strName, strArgName ) )
				nMaxNum = max(nMaxNum, get_tail_num(strName)+1);
		}
	}

	smart_alloc(sz, char, strArgName.length()+20);
	strcpy(sz, strArgName);
	set_tail_num(sz, nMaxNum);

	if (punkDataTypeManager)
		punkDataTypeManager->Release();

	return _bstr_t(sz);
}

IUnknown *GetAppUnknown()
{
	return App::application();
}

typedef INamedDataInfo* (*GTI)(BSTR);
GTI g_pfn = 0;

IUnknown	*CreateTypedObject( const BSTR bstrType )
{
	//_bstr_t	bstrType( szObjectType );
	/*
	IDataTypeInfoManager	*punkTypeInfoMan = 0;

	IUnknown	*punkApp = GetAppUnknown();
	if( punkApp->QueryInterface( IID_IDataTypeInfoManager, (void**)&punkTypeInfoMan ) != S_OK )
		return 0;

	INamedDataInfo	*pTypeInfo = 0, *p;
	

	long	nCount, n;
	punkTypeInfoMan->GetTypesCount( &nCount );

	_bstr_t	strType = bstrType;

	for( n = 0; n < nCount; n++ )
	{
		punkTypeInfoMan->GetTypeInfo( n, &p );

		INamedObject2Ptr	ptrN( p );
		BSTR	bstr;
		ptrN->GetName( &bstr );

		_bstr_t	str = bstr;
		::SysFreeString( bstr );

		if( str == strType )
		{
			pTypeInfo = p;
			break;
		}

		p->Release();
	}

	punkTypeInfoMan->Release();
	*/

	if( !g_pfn )
	{
		HINSTANCE hshell = GetModuleHandle("shell.exe");
		if( hshell )
			g_pfn = (GTI)GetProcAddress( hshell, "VTGetTypeInfo" );
	}
	
	if( !g_pfn )
		return 0;

	INamedDataInfo* pTypeInfo = g_pfn( bstrType );

	if( !pTypeInfo )
		return 0;

	IUnknown	*punkObject = 0;

	pTypeInfo->CreateObject( &punkObject );
	pTypeInfo->Release();

	return punkObject;
}

IUnknown *GetObjectByKey( IUnknown *punkFrom, const GUID &guid )
{
	IDataTypeManagerPtr	ptrDataType( punkFrom );

	if (ptrDataType == 0)
		return 0;

	long	nTypesCounter = 0;
	ptrDataType->GetTypesCount( &nTypesCounter );

	for( long nType = 0; nType < nTypesCounter; nType ++ )
	{
		IUnknown	*punkObj = 0;
		LONG_PTR	lpos = 0;

		ptrDataType->GetObjectFirstPosition( nType, &lpos );

		while( lpos )
		{
			punkObj = 0;
			ptrDataType->GetNextObject( nType, &lpos, &punkObj );

			if( ::GetKey( punkObj ) == guid )
				return punkObj;

			punkObj->Release();
		}
	}

	return 0;
}

IUnknown *GetObjectByName( IUnknown *punkFrom, const BSTR bstrObject, const BSTR bstrType )
{
	_bstr_t	bstrObjectName = bstrObject;
	sptrIDataTypeManager	ptrDataType( punkFrom );

	long	nTypesCounter = 0;
	ptrDataType->GetTypesCount( &nTypesCounter );

	for( long nType = 0; nType < nTypesCounter; nType ++ )
	{
		BSTR	bstrTypeName = 0;
		ptrDataType->GetType( nType, &bstrTypeName );

		_bstr_t	str( bstrTypeName, false );

		if( bstrType && str != _bstr_t(bstrType) )
			continue;

		IUnknown	*punkObj = 0;
		LONG_PTR	lpos = 0;

		ptrDataType->GetObjectFirstPosition( nType, &lpos );

		while( lpos )
		{
			punkObj = 0;
			ptrDataType->GetNextObject( nType, &lpos, &punkObj );

			if( ::GetName( punkObj ) == bstrObjectName )
				return punkObj;

			//INamedDataObject2Ptr	ptrN( punkObj );
			punkObj->Release();

			//paul 14.01.2003
			/*
			LPOS lpos = 0;
			ptrN->GetFirstChildPosition( &lpos );

			while( lpos )
			{
				IUnknown	*punkObject = 0;
				ptrN->GetNextChild( &lpos, &punkObject );

				if( ::GetName( punkObject ) == bstrObjectName )
					return punkObject;

				punkObject->Release();
			}
			*/
		}
	}

	return 0;
}

//GetObjectPath helper
_bstr_t	_get_object_path( IUnknown *punkDD, const char *sz, const GuidKey lKeyTest )
{
	sptrINamedData	sptrD( punkDD );

	_bstr_t	bstrPath = sz;

	sptrD->SetupSection( bstrPath );

	long nCount = 0;
	sptrD->GetEntriesCount( &nCount );

	StringArray	sa;

	long nObj;
	for( nObj = 0; nObj < nCount; nObj++ )
	{
		BSTR	bstrName = 0;

		_variant_t	var;

		sptrD->GetEntryName( nObj, &bstrName );
		sptrD->GetValue( bstrName, &var );

		_bstr_t	str( bstrName, false );

		sa.Add( _bstr_t(sz)+"\\"+str );

		if( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH )
			if( ::GetKey( var.punkVal ) == lKeyTest )
				return sa[nObj];
	}

	for( nObj = 0; nObj < nCount; nObj++ )
	{
		_bstr_t	str = _get_object_path( punkDD, sa[nObj], lKeyTest );

		if( str.length() )
			return str;
	}



	return "";
}


//return path to the object in NamedData
_bstr_t GetObjectPath( IUnknown *punkData, IUnknown *punkObj )
{
	return _get_object_path( punkData, 0, ::GetKey( punkObj ) );
}

//return path to the object in NamedData
_bstr_t GetObjectPath( IUnknown *punkData, IUnknown *punkObj, const char *sz )
{
	return _get_object_path( punkData, sz, ::GetKey( punkObj ) );
}


//delete an entry or section
bool DeleteEntry( IUnknown *punkDoc, const char *pszEntry )
{
	INamedDataPtr	ptrData( punkDoc );
	_bstr_t	bstrEntry( pszEntry );
	
	return (FAILED(ptrData->DeleteEntry( bstrEntry ))) ? false : true;
}


void DeleteObject( IUnknown *punkFrom, IUnknown *punkObject )//delete the specified object from NamedData
{
	_bstr_t	bstrPath = ::GetObjectPath( punkFrom, punkObject );
	if( bstrPath.length() == 0 )return;
	::DeleteEntry( punkFrom, bstrPath );
}

//get the color value from NamedData
COLORREF GetValueColor( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF crDefault )
{
	int	r = GetRValue( crDefault );
	int	g = GetGValue( crDefault );
	int	b = GetBValue( crDefault );

	char	sz[20];
	sprintf( sz, "(%d, %d, %d)", r, g, b );

	_bstr_t strValue = GetValueString( punkDoc, pszSection, pszEntry, sz );

	if( !strValue.length() )
		return crDefault;

	if( ::sscanf( strValue, "(%d, %d, %d)", &r, &g, &b ) == 3 )
	{
		return RGB( r, g, b );
	}

	SetValueColor( punkDoc, pszSection, pszEntry, crDefault );
	return crDefault;
}


time_test2 ttGetValue2("::GetValue2");
//get any value from NamedData
_variant_t GetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault )
{
	//time_test2_trigger tt(&ttGetValue2);
	_variant_t	var;// = varDefault;
	INamedData	*punkNamedData = 0;
	if( !punkDoc||punkDoc->QueryInterface( IID_INamedData, (void**)&punkNamedData ) != S_OK )
		return varDefault;

	_bstr_t	bstrSection( pszSection ), bstrEntry( pszEntry );

	CSaveSetupSection tmpsec(punkNamedData); // сохраним, какая секция была засетаплена
	punkNamedData->SetupSection( bstrSection );
	punkNamedData->GetValue( bstrEntry, &var );
	if( var.vt == VT_EMPTY )
	{
		punkNamedData->SetValue( bstrEntry, varDefault );
		punkNamedData->Release();
		return varDefault;
	}

	punkNamedData->Release();

	return var;
}

//get double value from NamedData
double GetValueDouble(IUnknown *punkDoc, const char *pszSection, const char *pszEntry, double fDefault)
{
	_variant_t	var(fDefault);
	var = ::GetValue(punkDoc, pszSection, pszEntry, var);

	if( _VarChangeType( var, VT_R8 )!= S_OK )
		return fDefault; 
	return var.dblVal;
}
//get integer value from NamedData
long GetValueInt( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lDefault )
{
	_variant_t	var(lDefault);
	var = ::GetValue(punkDoc, pszSection, pszEntry, var);

	if( _VarChangeType( var, VT_I4 )!= S_OK )
		return lDefault; 
	return var.lVal;
}

//get string value from NamedData
_bstr_t GetValueString( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszDefault )
{
	_variant_t	var( pszDefault );
	var = ::GetValue( punkDoc, pszSection, pszEntry, var );

	if( _VarChangeType( var, VT_BSTR )!= S_OK )
		return pszDefault; 
	return var.bstrVal;
}

//get pointer value from NamedData. You should free it using "delete"
byte *GetValuePtr( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long *plSize/* = 0*/ )
{
	_variant_t	var;
	var = ::GetValue( punkDoc, pszSection, pszEntry, var );

	if( var.vt == VT_EMPTY )
		return 0;

	long	lSize;
	byte	*pbyte = (byte*)var.lVal;
	if( !pbyte )return 0;

	lSize = *(long*)pbyte;
	byte	*ptr = new byte[lSize];
	memcpy( ptr, pbyte+sizeof( lSize ), lSize );

	if( plSize )*plSize = lSize;
	return ptr;
	
}

time_test2 ttSetValue2("::SetValue2");

//set pointer value to NamedData
void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, byte *ptr, long lSize )
{
	//time_test2_trigger tt(&ttSetValue2);
	//kill old
	_variant_t	var;
	var = ::GetValue( punkDoc, pszSection, pszEntry, var );

	if( var.vt == VT_BITS )
	{
		byte	*pbyte = (byte*)var.lVal;
		if(pbyte)	
			delete pbyte;
	}
	

	byte	*pdata = new byte[sizeof( lSize )+lSize];
	*(long*)pdata = lSize;
	memcpy( pdata+4, ptr, lSize );

	var = _variant_t((long)pdata);
	var.vt = VT_BITS;
	//var.pbVal = pdata;
	::SetValue(punkDoc, pszSection, pszEntry, var);
}
//set double value to NamedData
void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, double fValue )
{
	_variant_t	var(fValue);
	::SetValue(punkDoc, pszSection, pszEntry, var);
}
//set integer value to NamedData
void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lValue )
{
	_variant_t	var( lValue );
	::SetValue( punkDoc, pszSection, pszEntry, var );
}
//set string value to NamedData
void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszValue )
{
	_variant_t	var( pszValue );
	::SetValue( punkDoc, pszSection, pszEntry, var );
}
//set any value to NamedData
void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault )
{
	_variant_t	var = varDefault;

	INamedData	*punkNamedData = 0;
	if( !punkDoc||punkDoc->QueryInterface( IID_INamedData, (void**)&punkNamedData ) != S_OK )
		return;

	_bstr_t	bstrSection( pszSection ), bstrEntry( pszEntry );

	CSaveSetupSection tmpsec(punkNamedData); // сохраним, какая секция была засетаплена
	punkNamedData->SetupSection( bstrSection );
	punkNamedData->SetValue( bstrEntry, var );
	punkNamedData->Release();
}
//set the color value to NamedData
void SetValueColor( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF cr )
{
	int	r = GetRValue( cr );
	int	g = GetGValue( cr );
	int	b = GetBValue( cr );

	char	sz[30];
	sprintf( sz, "(%d, %d, %d)", r, g, b );
	SetValue( punkDoc, pszSection, pszEntry, sz );
}


DWORD ReportError( UINT	nIDS, ... )
{
	HWND	hwnd = 0;
	char	sz[255];

	::LoadString( App::handle(), nIDS, sz, 255 );

	IApplicationPtr	ptrA( App::application() );
	ptrA->GetMainWindowHandle( &hwnd );

	return ::MessageBox( hwnd, sz, "VideoTesT Message", MB_OK|MB_ICONEXCLAMATION );
}

//find the component with given interface in component's tree
IUnknown *_GetOtherComponent( IUnknown *punkThis, const IID iid )
{
	if( CheckInterface( punkThis, iid ) )
	{
		punkThis->AddRef();
		return punkThis;
	}

	IUnknown *punk = 0;
	if( CheckInterface( punkThis, IID_ICompManager ) )
	{
		ICompManagerPtr	sptr( punkThis );
		sptr->GetComponentUnknownByIID( iid, &punk );

		if( punk )
			return punk;
	}

	if( CheckInterface( punkThis, IID_IRootedObject ) )
	{
		IRootedObjectPtr	sptrR( punkThis );

		IUnknown	*punkParent = 0;

		sptrR->GetParent( &punkParent );

		punk = _GetOtherComponent( punkParent, iid );

		if( punkParent )
			punkParent->Release();
	}

	return punk;
}

//Get color convertor by name;whithout addref
IUnknown* GetCCByName(const char* pszName)
{
	if( !pszName )
		return 0;
	IUnknown* punk;
	punk = _GetOtherComponent(GetAppUnknown(), IID_IColorConvertorManager);
	ICompManagerPtr sptrCM(punk);
	punk->Release();
	if (sptrCM == 0)
		return 0;
	int numCvtrs;
	sptrCM->GetCount(&numCvtrs);
	BSTR bstr=0;
	sptrIColorConvertor2 sptrCC;
	bool bFind = false;
	for(int i = 0; i < numCvtrs; i++)
	{
		sptrCM->GetComponentUnknownByIdx(i, &punk);
		sptrCC = punk;
		punk->Release();
		sptrCC->GetCnvName(&bstr);
		_bstr_t bstrCName( bstr );		
		::SysFreeString(bstr);

		
		if (!_stricmp( pszName, (const char*)bstrCName ) )
		{
			bFind = true;
			break;
		}
	}
	if (!bFind)
	{
		sptrCM->GetComponentUnknownByIID(IID_IColorConvertor, &punk);
		sptrCC = punk;
		punk->Release();
	}
	return sptrCC;
}


//convert to colorspace
void GetColors( IImage2 *pImage, color *pcolors, COLORREF cr )
{
	if( !pImage )return;

	IUnknown	*punkCC = 0;
	pImage->GetColorConvertor( &punkCC );
	if( !punkCC )return;
	IColorConvertor5Ptr	ptrCC( punkCC );
	punkCC->Release();

	int	colors = ::GetImagePaneCount( pImage ), c;

	color	**ppcolor = new color*[colors];
	for( c = 0; c < colors; c++ )
		ppcolor[c] = &pcolors[c];
	byte	bytesDIB[3];
	bytesDIB[0] = GetBValue( cr );
	bytesDIB[1] = GetGValue( cr );
	bytesDIB[2] = GetRValue( cr );

	ptrCC->ConvertDIBToImageEx( bytesDIB, ppcolor, 1, true, colors);
	
	delete ppcolor;
}


//find object in data, include child objects, stored by parent
IUnknown* GetObjectByKeyEx(IUnknown* punkData, GuidKey lKey)
{	
	IDataTypeManagerPtr	ptrDataType(punkData);

	if (ptrDataType == 0)
		return 0;

	long	nTypesCounter = 0;
	ptrDataType->GetTypesCount(&nTypesCounter);

	// for all types
	for (long nType = 0; nType < nTypesCounter; nType++)
	{
		LONG_PTR lpos = 0;

		ptrDataType->GetObjectFirstPosition(nType, &lpos);

		while (lpos)
		{
			IUnknown * punkObj = 0;
			// get next object
			ptrDataType->GetNextObject(nType, &lpos, &punkObj);
			if (!punkObj)
				continue;

			// check this object and its children(if any) for its key
			IUnknown *punk = ::GetChildObjectByKey(punkObj, lKey);

			// if finded - return it
			if (punk)
			{
				if (::GetKey(punk) != ::GetKey(punkObj))
					punkObj->Release();

				return punk;
			}
			// to next object 
			punkObj->Release();
		}
	}
	// nothing finded
	return 0;
}

IUnknown* GetChildObjectByKey(IUnknown* punkParent, GuidKey lKey)
{
	if (lKey == INVALID_KEY || !punkParent)
		return 0;

	// if this object has same key as we need 
	if (::GetKey(punkParent) == lKey)
		// return it
		return punkParent;

	// parent must be to have INamedDataObject2 interface
	if (!CheckInterface(punkParent, IID_INamedDataObject2))
		return 0;

	sptrINamedDataObject2 sptr = punkParent;
	if (sptr == 0)
		return 0;

	// if has no children 
	long lCount = 0;
	if (SUCCEEDED(sptr->GetChildsCount(&lCount)) && !lCount)
		// return 
		return 0;

	// for all children
	LONG_PTR lPos = 0;
	if (FAILED(sptr->GetFirstChildPosition((POSITION*)&lPos)))
		return 0; 

	while (lPos)
	{
		IUnknown * punkObj = 0;
		sptr->GetNextChild((POSITION*)&lPos, &punkObj);
		if (!punkObj)
			continue;
	
		// check this object and its children(if any) has needed key
		IUnknown *punk = ::GetChildObjectByKey(punkObj, lKey);

		// if finded - return it
		if (punk)
		{
			if (::GetKey(punk) != ::GetKey(punkObj))
				punkObj->Release();

			return punk;
		}
		// to next object 
		punkObj->Release();
	}
	// nothing finded
	return 0;
}

//return the active object from data context
IUnknown *GetActiveObjectFromContext( IUnknown *punkContext, const char *szObjectType )
{
	if( !punkContext )return 0;
	_bstr_t	bstrType = szObjectType;
	sptrIDataContext	sptrContext( punkContext );

	IUnknown	*punkResult = 0;

	sptrContext->GetActiveObject( bstrType, &punkResult );

	return punkResult;
}

//return the active object from document
IUnknown *GetActiveObjectFromDocument( IUnknown *punkDoc, const char *szObjectType )
{
	IDocumentSitePtr ptrD(punkDoc);

	if (ptrD == 0)
		return 0;

	IUnknownPtr sptrView;
	ptrD->GetActiveView(&sptrView);

	IDataContext2Ptr sptrC = sptrView == 0 ? punkDoc : sptrView;
	if (sptrC == 0)
		return 0;

	IUnknown * punkObj = 0;
	if (!lstrlen(szObjectType))
		sptrC->GetLastActiveObject(0, &punkObj);
	else
	{
		_bstr_t	bstrType = szObjectType;
		sptrC->GetActiveObject(bstrType, &punkObj);
	}

	return punkObj;
}


_bstr_t	GetObjectType( IUnknown *punk )
{
	BSTR	bstr = 0;
	INamedDataObjectPtr	ptrNamed( punk );
	ptrNamed->GetType( &bstr );

	_bstr_t	bstrType = bstr;
	::SysFreeString( bstr );

	return bstrType;
}


//find the property sheet dock bar
IPropertySheet *FindPropertySheet()
{
	IApplicationPtr	ptrApp( App::application() );
	IUnknown	*punkMainWnd = ::_GetOtherComponent( ptrApp, IID_IMainWindow );
	IMainWindowPtr	ptrMainWnd = punkMainWnd;
	if( !punkMainWnd )return 0;
	punkMainWnd->Release();

	POSITION	lpos = 0;
	ptrMainWnd->GetFirstDockWndPosition( &lpos );
	while( lpos )
	{
		IUnknown *punkDockBar = 0;
		ptrMainWnd->GetNextDockWnd( &punkDockBar, &lpos );

		IPropertySheetPtr	ptrSheet = punkDockBar;
		if( punkDockBar )
			punkDockBar->Release();

		if( ptrSheet != 0 )
			return ptrSheet.Detach();
	}
	return 0;
}

//find the property page by name
IOptionsPage *FindPage( IPropertySheet *pSheet, const GUID &clsidPage, int *pnPos )
{
	if( pSheet == 0 )return 0;

	int	nCount;
	pSheet->GetPagesCount( &nCount );
	IUnknown	*punkPage = 0;
	if( pnPos )*pnPos = -1;

	for( int n = 0; n < nCount; n++ )
	{
		pSheet->GetPage( n, &punkPage );

		IOptionsPagePtr	ptrPage = punkPage;
		IPersistPtr		ptrPersist = punkPage;
		punkPage->Release();

		if( ptrPage != 0 && ptrPersist != 0 )
		{
			CLSID clsid;
			ptrPersist->GetClassID( &clsid );

			if( clsid == clsidPage )
			{
				if( pnPos )*pnPos = n;
				return ptrPage.Detach();
			}
		}
	}

	return 0;
}


//return the running interactive action
IUnknown *GetCurrentInteractiveAction()
{
	IUnknown	*punkAM = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
	if( !punkAM )	return 0;
	IActionManagerPtr	ptrAM = punkAM;
	punkAM->Release();

	IUnknown	*punkAction = 0;
	ptrAM->GetRunningInteractiveAction( &punkAction );

	return punkAction;
}


int VTMessageBox( LPCSTR lpText,LPCSTR lpCaption, UINT uType )
{
	HWND hWnd = NULL;
	IApplicationPtr ptrApp( ::GetAppUnknown() );
	if( ptrApp != 0 )
		ptrApp->GetMainWindowHandle( &hWnd );

	if( !lpCaption )
	{
		char sz_caption[1024];	sz_caption[0] = 0;
		::LoadString( app::handle(), 0xE000, sz_caption, sizeof(sz_caption) );
		return MessageBox( hWnd, lpText, sz_caption, uType );
	}
	
	return MessageBox( hWnd, lpText, lpCaption, uType );
	
}

int VTMessageBox( UINT uiResID, HMODULE hInst, LPCSTR lpCaption, UINT uType )
{
	char szString[1024];
	if( !::LoadString( hInst, uiResID, szString, 1024 ) )
	{
		dbg_assert( false );
		return -1;
	}

	return VTMessageBox( szString, lpCaption, uType );	
}



CHourglass::CHourglass()
{
	m_hCur = SetCursor(LoadCursor(NULL, IDC_WAIT));
}

CHourglass::~CHourglass()
{
	SetCursor(m_hCur);
}


bstr_t LoadStringFromStream( IStream *pStream )
{
	if(!pStream)
	{
		dbg_assert( false );
		return "";
	}

	ULONG nRead = 0;

	ULONG uStrLen = 0;
	pStream->Read( &uStrLen, sizeof(uStrLen), &nRead );
	
	char* pszBuf = new char[uStrLen];
	pStream->Read( pszBuf, uStrLen, &nRead );
	bstr_t bstr = pszBuf;
	
	if( pszBuf ) delete pszBuf; pszBuf = 0;

	return bstr;
}


bool StoreStringToStream( IStream *pStream, bstr_t& bstr )
{
	if(!pStream)
	{
		dbg_assert( false );
		return false;
	}	

	ULONG nWritten = 0;

	char* pszBuf = (char*)bstr;
    ULONG uBufLen = 0;
	if( pszBuf )
		uBufLen = strlen( pszBuf ) + 1;

	pStream->Write( &uBufLen, sizeof(ULONG), &nWritten );				
	pStream->Write( pszBuf, uBufLen, &nWritten );				

	return true;

}

int GetImagePaneCount( IUnknown *punkImage )
{
	if( !punkImage )
		return -1;

	IImage4Ptr sptrImage = punkImage;

	if( sptrImage == 0 )
		return -1;

	int nCount = -1;
	
	sptrImage->GetPanesCount( &nCount );
	return nCount;
}


//execute the specified action
bool ExecuteAction( const char *szActionName, const char *szActionparam, DWORD dwFlags )
{
	IUnknown	*punk = ::_GetOtherComponent( ::GetAppUnknown(), IID_IActionManager );

	sptrIActionManager sptrAM( punk );

	if( punk )
		punk->Release();

	if( sptrAM == 0 )
		return false;

	_bstr_t	bstrActionName = szActionName;
	_bstr_t	bstrExecuteParams = szActionparam;

	return (SUCCEEDED(sptrAM->ExecuteAction( bstrActionName, bstrExecuteParams, dwFlags ))) ? true : false;
}

//Write BSTR
HRESULT WriteBSTR( IStream* pi_stream, BSTR bstr )
{
	if( !pi_stream )	return E_FAIL;
	UINT uiLen = 0;
	if( bstr )
		uiLen = SysStringByteLen( bstr );

	ULONG nWrite = 0;
	HRESULT hr = pi_stream->Write( &uiLen, (ULONG)sizeof(uiLen), &nWrite );
	if( hr != S_OK )	return hr;

	if( !bstr )
		return hr;

	if( uiLen > 0 )
		hr = pi_stream->Write( (BYTE*)bstr, (ULONG)uiLen, &nWrite );

	return hr;	
}
//Read BSTR
HRESULT ReadBSTR( IStream* pi_stream, BSTR* pbstr )
{
	if( !pi_stream || !pbstr )	return E_FAIL;

	*pbstr = 0;
	ULONG nRead = 0;
	UINT uiLen = 0;
	HRESULT hr = pi_stream->Read( &uiLen, sizeof(uiLen), &nRead );
	if( hr != S_OK )	return hr;

	if( uiLen > 0 )
	{			
		BYTE *pBuf = new BYTE[uiLen];
		if( !pBuf )		return E_OUTOFMEMORY;		
		hr = pi_stream->Read( pBuf, uiLen, &nRead );
		if( hr != S_OK )
		{
			delete [] pBuf;	pBuf = 0;
			return hr;
		}
		*pbstr = SysAllocStringByteLen( (LPCSTR)pBuf, uiLen );
		delete [] pBuf;	pBuf = 0;
	}
	return hr;
}

//////////////////////////////////////////////////////////////////////
//Display win 32 error
void DislayWin32Error( DWORD dw_error, HWND hwnd, const char* psz_title )
{
	if( dw_error == 0 )
		return;

	LPVOID lpMsgBuf = 0;
	FormatMessage( 
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					0,
					dw_error,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR) &lpMsgBuf,
					0,
					0
					);

	::MessageBox( hwnd, (LPCTSTR)lpMsgBuf, psz_title, MB_ICONERROR );

	LocalFree( lpMsgBuf );
}

struct	ScriptRuntimeInfo
{
	GuidKey m_lOwnerKey;
	SCRIPTTHREADID	stid;
};

class CScriptRuntimeList : public _list_t2< ScriptRuntimeInfo* >
{
public:
	~CScriptRuntimeList()
	{
		TPOS pos = head();
		while( pos ) 
		{
			ScriptRuntimeInfo *p = next( pos );
			delete p;
		}
	}
};

CScriptRuntimeList	m_listScripts;

//execute the specified script
bool ExecuteScript( BSTR bstrScript, const char* psz_name, bool bFromActionManager, GuidKey lOwnerKey, DWORD dwFlags )
{
	class XAppManLock
	{
		bool	m_bOldScriptFlag;
	public:
		XAppManLock( bool bLock )
		{
			m_bOldScriptFlag = true;
			if( !bLock )
				return;
			IUnknown *punkAM = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
			sptrIActionManager	sptrAM(punkAM);
			if (sptrAM == 0)
				return;
			punkAM->Release();

			m_bOldScriptFlag = sptrAM->IsScriptRunning() == S_OK;
			sptrAM->SetScriptRunningFlag( TRUE );
		}

		~XAppManLock()
		{
			if( m_bOldScriptFlag )
				return;
			IUnknown *punkAM = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
			sptrIActionManager	sptrAM(punkAM);
			if (sptrAM == 0)
				return;
			punkAM->Release();

			sptrAM->SetScriptRunningFlag( m_bOldScriptFlag );
		}
	};

	XAppManLock	lock( !bFromActionManager );

//test the string
	if( !::SysStringLen( bstrScript ) )
		return false;
//get the ActiveScript
	IUnknown	*punkScript = 0;
	IUnknown	*papp = GetAppUnknown();
	sptrIApplication	sptrA( papp );

	sptrA->GetActiveScript( &punkScript );
	if( !punkScript )	return false;

	IScriptEnginePtr ptr_se( punkScript );
	if( punkScript )
		punkScript->Release();	punkScript = 0;

	if( ptr_se == 0 )	return false;

	IUnknown* punk_as = 0;
	IUnknown* punk_asp = 0;

	ptr_se->GetActiveScript( &punk_as );
	ptr_se->GetActiveScriptParse( &punk_asp );


 	IActiveScriptPtr		sptrS = punk_as;
	IActiveScriptParsePtr	sptrP = punk_asp;	

	if( punk_as )	
		punk_as->Release();		punk_as = 0;

	if( punk_asp )	
		punk_asp->Release();	punk_asp = 0;
	

	if( sptrS == 0 || sptrP == 0 )
		return false;


	EXCEPINFO   ei;
	DWORD dwContext = 0;

	static int sCount = 0;
	sCount++;
	//TRACE( "Before ParseScriptText %d\n", sCount );

	{
		static int qq = 0;
		qq++;
		char sz_buf[100];
		if( !psz_name )
			sprintf( sz_buf, "Script%d", qq );
		else
			strcpy( sz_buf, psz_name );

		IDebugHostPtr ptr_debugger( ptr_se );
		if( ptr_debugger )
			ptr_debugger->OnRunScript( bstrScript, _bstr_t( sz_buf ), _bstr_t( "" ), sptrS, 0 );
	}

	if( sptrP->ParseScriptText( bstrScript, NULL, NULL, NULL, dwContext, 0, 0L, NULL, &ei ) )
		return false;
	sCount--;
	//TRACE( "After ParseScriptText\n" );

	if( sptrS->SetScriptState( SCRIPTSTATE_CONNECTED ) )
		return false;

	SCRIPTTHREADID	stid;
	sptrS->GetCurrentScriptThreadID( &stid );

	if( lOwnerKey != INVALID_KEY )
	{
		ScriptRuntimeInfo	*psri = new ScriptRuntimeInfo;

		psri->m_lOwnerKey = lOwnerKey;
		sptrS->GetCurrentScriptThreadID( &psri->stid );

		m_listScripts.insert_before( psri );
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool RecursiveWriteEntry( INamedData* ptrSrc, INamedData* ptrTarget, LPCSTR pszEntry, bool bOvewrite )
{
	if( !ptrSrc || !ptrTarget || !pszEntry )
		return false;

	_bstr_t bstrPath( pszEntry );

	// set section
	if( S_OK != ptrSrc->SetupSection( bstrPath ) )
		return false;
	
	// get count of entries
	long EntryCount = 0;
	ptrSrc->GetEntriesCount( &EntryCount );

	// for all entries
	for( int i = 0;i<(int)EntryCount;i++ )
	{
		BSTR bstr = NULL;
		// get entry name
		
		ptrSrc->GetEntryName( i, &bstr );
		_bstr_t bstrEntryName = bstr;
		if(0>=bstrEntryName.length())
			continue;
		::SysFreeString( bstr );	bstr = 0;
		
		// format a full path for entry
		_bstr_t bstrPathNew;
		int nLen = bstrPath.length();

		if( nLen > 0 && ((char*)bstrPath)[nLen-1] == '\\' )
			bstrPathNew = bstrPath + bstrEntryName;
		else
			bstrPathNew = bstrPath + "\\" + bstrEntryName;

		// get value
		_variant_t var;
		ptrSrc->GetValue( bstrPathNew, &var );

		bool bCanSetValue = true;
		if( !bOvewrite )
		{
			_variant_t _var;
			ptrTarget->GetValue( bstrPathNew, &_var );

			bCanSetValue = ( _var.vt == VT_EMPTY );				
		}
			
		if( bCanSetValue )
		{
			ptrTarget->SetupSection( bstrPathNew );
			ptrTarget->SetValue( bstrPathNew, var );
		}
		
		// if this entry has children => we want to walk to them
		if( EntryCount > 0 )
			RecursiveWriteEntry( ptrSrc, ptrTarget, bstrPathNew, bOvewrite );

		// for next entry on this level restore Section
		ptrSrc->SetupSection( bstrPath );
	}
		
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CopyObjectNamedData( IUnknown* punk_src, IUnknown* punk_target, LPCSTR pszEntry, bool bOvewrite )
{
	INamedDataPtr ptr_src_nd( punk_src );
	if( ptr_src_nd == 0 )
		return false;

	INamedDataPtr ptr_target_nd( punk_target );
	if( ptr_target_nd == 0 )
	{
		INamedDataObject2Ptr ptr_target_ndo2( punk_target );
		if( ptr_target_ndo2 )				
			ptr_target_ndo2->InitAttachedData();
		
		ptr_target_nd = punk_target;
	}

	if( ptr_target_nd == 0 )
		return false;
		
	return RecursiveWriteEntry( ptr_src_nd, ptr_target_nd, pszEntry, bOvewrite );
}

void CopyPropertyBag(  IUnknown* punk_src, IUnknown* punk_target, bool bOvewrite )
{
	INamedPropBagPtr sptrNPGSrc(punk_src);
	INamedPropBagPtr sptrNPGTrg(punk_target);
	long lpos = 0;
	sptrNPGSrc->GetFirstPropertyPos(&lpos);
	while (lpos)
	{
		_variant_t var;
		_bstr_t bstrName;
		sptrNPGSrc->GetNextProperty(bstrName.GetAddress(), &var, &lpos);
		if (!bOvewrite)
		{
			_variant_t var1;
			sptrNPGTrg->GetProperty(bstrName, &var1);
			if (var1.vt != VT_EMPTY)
				continue;
		}
		sptrNPGTrg->SetProperty(bstrName, var);
	}
}


_bstr_t GetCCName(IUnknown *pImage)
{
	IImage3Ptr pimg = pImage;

	if( pimg == 0 )
		return _bstr_t("");


	IUnknown	*punkCC1 = 0;
	pimg->GetColorConvertor(&punkCC1);

	if (punkCC1 == 0)
		return _bstr_t("");

	IColorConvertor2Ptr sptrCC1 = punkCC1;
	punkCC1->Release();

	if (sptrCC1 == 0)
		return _bstr_t("");
	
	BSTR bstr;
	sptrCC1->GetCnvName(&bstr);
	_bstr_t bstrN1(bstr);
	::SysFreeString(bstr);
	return bstrN1;
}
