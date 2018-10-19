#include "stdafx.h"
#include "common.h"
#include "afxpriv2.h"
#include "thumbnail.h"
#include "scriptdocint.h"
#include "nameconsts.h"
#include "aliaseint.h"

#include "compressint.h"
#include "string"
#include "editorint.h"

#include "StreamEx.h"
#include "StreamUtils.h"

//#include <ActivScp.h>
//#include <ComDef.h>
#include "script_int.h"
#include "\vt5\AWIN\profiler.h"
#include "\vt5\AWIN\misc_string.h"
#include "\vt5\AWIN\misc_map.h"
#include "\vt5\awin\misc_dbg.h"

#import "msado15.dll" rename_namespace("ADO") rename("EOF", "ADOEOF")

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

//global flags
bool g_bActionRegistrationEnabled = true;

////////////CLogFile
CLogFile g_log;
//IUnknown *g_punkApp = 0;	//application Unknown (thin reference)

std_dll CLogFile &GetLogFile()
{
	return g_log;
}

CLogFile::CLogFile()
{
	m_bInit = false;
	m_lLoggingLevel = 0;
}

void CLogFile::Init()
{
	if( m_bInit )
		return;

	m_bInit = true;

	char	szDrv[_MAX_DRIVE], szDir[_MAX_PATH], szName[_MAX_FNAME];
	char	szPath[_MAX_PATH];


	::GetModuleFileName( ::AfxGetInstanceHandle(), szPath, _MAX_PATH );
	::GetLongPathName( szPath, szPath, _MAX_PATH ) ;
	::_splitpath( szPath, szDrv, szDir, szName, 0 );
	DWORD	dwType, dwDataSize;

	::_makepath( szPath, szDrv, szDir, 0, 0 );
	
	HKEY	hKey = 0;
	LONG 
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,  _T("Software\\VideoTest\\Settings"), 0, KEY_ALL_ACCESS, &hKey );

	if( lResult )
	{
		RegCreateKeyEx(HKEY_CURRENT_USER,  _T("Software\\VideoTest\\Settings"), 	0, NULL, REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS, NULL, &hKey, NULL );
	}


	dwDataSize = sizeof( m_lLoggingLevel );
	dwType = REG_DWORD;

	lResult = ::RegQueryValueEx( 	hKey,	"Logging",
		0,	&dwType,	(BYTE*)&m_lLoggingLevel,	&dwDataSize );

	if( lResult )
	{
		m_lLoggingLevel = 0;
		::RegSetValueEx( hKey, "Logging", 0, REG_DWORD,
				(BYTE*)&m_lLoggingLevel, sizeof( m_lLoggingLevel ) );
	}

	dwDataSize = sizeof( szPath );
	dwType = REG_SZ;

	lResult = ::RegQueryValueEx( 	hKey,	"LoggingPath",
		0,	&dwType,	(BYTE*)&szPath[0],	&dwDataSize );

	if( lResult )
	{
		::RegSetValueEx( hKey, "LoggingPath", 0, REG_SZ,
				(BYTE*)&szPath[0], sizeof( szPath ) );
	}

	::RegCloseKey( hKey );

	::_splitpath( szPath, szDrv, szDir, 0, 0 );
	::_makepath( szPath, szDrv, szDir, szName, "log" );

	m_sFileName = szPath;

	if( m_lLoggingLevel )
	{
		CFileException pError;
		m_file.Open(m_sFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText, &pError);
		if (pError.m_cause == CFileException::none)
		{
			Write("start", m_sFileName);
		}
		else
		{
			pError.ReportError();
			exit(1);
		}
	}
}

CLogFile::~CLogFile()
{
	Init();
	if( m_lLoggingLevel )
		Write( "finish", m_sFileName );
}

void CLogFile::Exception( CException *pe )
{
	Init();

	if( pe )
	{
		char	sz[1000];
		pe->GetErrorMessage( sz, 1000 );
		
		if(  m_lLoggingLevel  >= 1 )
		{
			Write( "! MS Exception: ", sz );
			DumpStack();
		}
	}
	else
	{
		if(  m_lLoggingLevel  >= 1 )
		{
			Write( "! Generic Exception, possible GPF" );
			DumpStack();
		}
	}
}

void CLogFile::Add( const char *szFunc )
{
	Init();

	//m_arr.Add( szFunc );

	if( m_lLoggingLevel  >= 2 )
		Write( " enter: ", szFunc, m_arr.GetSize() );
}

void CLogFile::RemoveLast()
{
	Init();

	if(  m_lLoggingLevel  >= 2 && m_arr.GetSize() > 0 )
		Write( " leave: ", m_arr[m_arr.GetSize()-1], m_arr.GetSize() );

	if( m_arr.GetSize() )
		m_arr.RemoveAt( m_arr.GetSize()-1 );
}

void CLogFile::DumpStack()
{
	Init();

	if(  m_lLoggingLevel  == 0  )
		return;
	Write( "?call stack begin", "***" );
	
	for( int i = m_arr.GetSize()-1; i >= 0; i-- )
		Write( "call:", m_arr[i] );
	Write( "call: ", "<<external caller>>" );
	Write( "?call stack end", "***" );
	Write( "" );
}

void CLogFile::WriteRaw( const char *sz )
{
	if(  m_lLoggingLevel  == 0  )
		return;
	Init();
	m_file.WriteString( sz );
	m_file.Flush();
}


void CLogFile::Write( const char *szCaller, const char *szFunc, int iLevel )
{
	if(  m_lLoggingLevel  == 0  )
		return;
	Init();

	CString	s1;

	for( int i = 0; i < iLevel; i++ )
		s1+="  ";

	CString	s;

	if( szFunc )
		s.Format( "%s %s %s", szCaller, (LPCSTR)s1, szFunc );
	else
		s = szCaller;

	s += "\n";

	m_file.WriteString( s );
	m_file.Flush();
}

CLogFile &operator << (CLogFile &log, double f)
{
	CString	s;
	s.Format( "%lf", f );
	log.WriteRaw( s );

	return log;
}

CLogFile &operator << (CLogFile &log, const char *sz)
{
	log.WriteRaw( sz );

	return log;
}

CLogFile &operator << (CLogFile &log, int i)
{
	CString	s;
	s.Format( "%d", i );
	log.WriteRaw( s );

	return log;
}

CLogFile &operator << (CLogFile &log, const CRect rc)
{
	CString	s;
	s.Format( "( %d, %d )( %d, %d )", rc.left, rc.top, rc.right, rc.bottom );
	log.WriteRaw( s );

	return log;
}

/////////////////////////////////////////////////////////////////////////////////////////
///CStringArrayEx
CStringArrayEx::CStringArrayEx()
{
	m_nStartSection = -1;
	m_nEndSection = -1;
}

int	CStringArrayEx::LockSection( const char *pszSection )
{
	bool	b1st = true;
	m_nStartSection = -1;
	m_nEndSection = -1;

	for( int i = 0; i < GetSize(); i++ )
	{
		if( b1st )
		{
			if( !strcmp( GetAt( i ), pszSection ) )
			{
				m_nStartSection = i;
				b1st = false;
			}
		}
		else
		{
			const char	*psz = GetAt( i );
			if( strlen( psz ) )
				if( *psz == '[' )
					m_nEndSection = i;
		}
	}

	return m_nStartSection;
}

void CStringArrayEx::UnlockSection()
{
	m_nStartSection = -1;
	m_nEndSection = -1;
}

const char *CStringArrayEx::FindEntry( const char *pszEntry, int idxStart, int idxEnd )
{
	int	l = strlen( pszEntry );
	int	nStart = (idxStart == -1)?_GetStartPos():idxStart;
	int	nEnd = (idxEnd == -1)?_GetEndPos():idxEnd;

	for( int i = nStart; i < nEnd; i++ )
	{
		const char *psz = GetAt( i );
		
		if( !strncmp( pszEntry, psz, l ) )
			return psz+l+1;
	}

	return 0;
}

CString CStringArrayEx::GetTotalText()
{
	CString	str;
	for( int i = _GetStartPos(); i < _GetEndPos(); i++ )
	{
		if( !str.IsEmpty() )
			str += "\n";
		str += GetAt( i );
	}
	return str;
}

INT_PTR	CStringArrayEx::Find( const char *szString, int iPos )
{
	int	nStart = (iPos == -1)?_GetStartPos():iPos;
	int	nEnd = _GetEndPos();

	for( int i = nStart; i < nEnd; i++ )
	{
		if( GetAt( i ) == szString )
			return i;
	}

	return Add( szString );
}

void CStringArrayEx::SkipComments( const char chComment )
{
	for( int i = GetSize()-1; i >= 0; i-- )
	{
		CString	str = (*this)[i];

		int	idx = str.Find( chComment );

		if( idx != -1 )
			str = str.Left( idx );

		if( str.IsEmpty() )
			RemoveAt( i );
		else
			(*this)[i] = str;
	}
}

void CStringArrayEx::ReadFile( CStdioFile *pfile )
{
	_try(CStringArrayEx, ReadFile)
	{
		ASSERT( pfile );

		//RemoveAll();
		SetSize( 0, 50 );

		DWORD	dwLength = (DWORD)pfile->GetLength();
		char	*pszFile = new char[dwLength+1];
		if( pszFile )
		{
			dwLength = pfile->Read( pszFile, dwLength );
			*(pszFile+dwLength) = 0;
			char	*p = pszFile, *p0 = pszFile;

			while( true )
			{
				while( dwLength && *p != '\n' && *p != '\r' ){p++;dwLength--;}
				if( dwLength )
				{
					CString	str( p0, (BYTE*)p-(BYTE*)p0 );
					Add( str );
				}
				else
				{
					CString	str( p0 );
					Add( str );
				}

				if( !dwLength )break;
				if( *p == '\r' ){p++;dwLength--;}
				if( !dwLength )break;
				if( *p == '\n' ){p++;dwLength--;}
				if( !dwLength )break;
				
				p0=p;
			}

			delete pszFile;
		}
		else
		{
			CString	s;
			for( ;; )
			{
				if( !pfile->ReadString( s ) )
					break;
				Add( s );
			}
		}
	}
	_catch_report;
}

void CStringArrayEx::WriteFile( CStdioFile *pfile )
{
	_try(CStringArrayEx, WriteFile)
	{
		ASSERT( pfile );

		for( int i = 0; i < GetSize(); i++ )
			pfile->WriteString( GetAt( i )+"\n" );
	}
	_catch_report;
}


void CStringArrayEx::ReadFile( const char *szFileName )
{
	CStdioFile	file( szFileName, CFile::modeRead|CFile::typeText );
	ReadFile( &file );
}

void CStringArrayEx::WriteFile( const char *szFileName )
{
	CStdioFile	file( szFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeText );
	WriteFile( &file );

}

CString CStringArrayEx::operator[](int nIndex) const
{
	if( nIndex >= GetSize() )
		return "";

	return GetAt( nIndex );
}

CString& CStringArrayEx::operator[](int nIndex)
{
	if( nIndex >= GetSize() )
		SetSize( nIndex+1 );
	return ElementAt( nIndex );
}



///////////////////////////////////////////////////////////////////////////////
//
// generate hash key for object from object's key (GUID)
template<> std_dll UINT AFXAPI HashKey(GUID& key)
{
	return key.Data1;
}
//
//template<> std_dll UINT AFXAPI HashKey<GuidKey> (GuidKey & key)
//{
////	GUID g = key;
//	return key.Data1;
//}



///////////////////////////////////////////////////////////////////////////////
//some misc functions
std_dll void FireEvent( IUnknown *punkCtrl, const char *pszEvent, IUnknown *punkFrom, IUnknown *punkData, void *pdata, long cbSize )
{
	INotifyControllerPtr	sptr( punkCtrl );

	if( sptr== 0 )
	{
//проверка на то, что data не в документе, а в объекте
		INamedDataObject2Ptr	ptrND( punkCtrl );
		if( ptrND != 0 )
		{
			IUnknown *punkNamedData = 0;
			ptrND->GetData( &punkNamedData );
			if( punkNamedData )
			{
				FireEvent( punkNamedData, pszEvent, punkFrom, punkData, pdata, cbSize );
				punkNamedData->Release();
			}
		}
		return;
	}

	_bstr_t	bstrEvent( pszEvent );
	sptr->FireEvent( bstrEvent, punkData, punkFrom, pdata, cbSize );
}

std_dll void FireEventNotify( IUnknown *punkCtrl, const char *pszEvent, IUnknown *punkFrom, IUnknown *punkData, long code )
{
	FireEvent( punkCtrl, pszEvent, punkFrom, punkData, &code, sizeof( code ) );
}

// fire event with exculude event source 
std_dll void FireEvent2(IUnknown *punkCtrl, const char *pszEvent, IUnknown *punkFrom, IUnknown *punkData, void *pdata, long cbSize, IUnknown * punkEvListener)
{
	_try(common, FireEvent2)
	{
		INotifyController2Ptr	sptr(punkCtrl);

		if (sptr == 0)
			return;

		_bstr_t		bstrEvent(pszEvent);

		bool bRestore = false;

		if (CheckInterface(punkEvListener, IID_IEventListener))
		{
			sptr->AddSource(punkEvListener);
			bRestore = true;
		}

		sptr->FireEvent(bstrEvent, punkData, punkFrom, pdata, cbSize );

		if (bRestore)
			sptr->RemoveSource(punkEvListener);
	}
	_catch;
}

//CImplBase
std_dll IUnknown *_GetOtherComponent( IUnknown *punkThis, const IID iid )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

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

std_dll IUnknown * FindComponentByName(IUnknown *punkThis, const IID iid, LPCTSTR szName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strName(szName);
	CString strCompName;

	if (CheckInterface(punkThis, iid))
	{
		strCompName = ::GetObjectName(punkThis);
		if (strCompName == strName)
		{
			punkThis->AddRef();
			return punkThis;
		}
	}

	IUnknown *punk = 0;
	if (CheckInterface(punkThis, IID_ICompManager))
	{
		ICompManagerPtr	sptr(punkThis);
		int nCount = 0;
		sptr->GetCount(&nCount);
		for (int i = 0; i < nCount; i++)
		{
			sptr->GetComponentUnknownByIdx(i, &punk);

			if (punk)
			{
				if (CheckInterface(punk, iid))
				{
					strCompName = ::GetObjectName(punk);
					if (strCompName == strName)
						return punk;
				}
				punk->Release();
				punk = 0;
			}
		}
	}

	if (CheckInterface(punkThis, IID_IRootedObject))
	{
		IRootedObjectPtr	sptrR(punkThis);
		IUnknown	*punkParent = 0;

		sptrR->GetParent(&punkParent);

		punk = FindComponentByName(punkParent, iid, strName);

		if (punkParent)
			punkParent->Release();
	}

	return punk;
}

//return temporaly or permanent window object, associated with given unknown.
std_dll CWnd *GetWindowFromUnknown( IUnknown *punk )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !punk )
		return 0;

	if( !CheckInterface( punk, IID_IWindow ) )
		return 0;

	HWND	hwnd = 0;

	sptrIWindow	sptrW( punk );
	sptrW->GetHandle( (HANDLE*)&hwnd );

	if( !hwnd )
		return 0;

	return CWnd::FromHandle( hwnd );
}

//set the color value to NamedData
std_dll void SetValueColor( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF cr )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int	r = GetRValue( cr );
	int	g = GetGValue( cr );
	int	b = GetBValue( cr );

	CString	strDef;
	strDef.Format( "(%d, %d, %d)", r, g, b );
	SetValue( punkDoc, pszSection, pszEntry, strDef );
}

//get the color value from NamedData
std_dll COLORREF GetValueColor( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF crDefault )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int	r = GetRValue( crDefault );
	int	g = GetGValue( crDefault );
	int	b = GetBValue( crDefault );

	CString	strDef;
	strDef.Format( "(%d, %d, %d)", r, g, b );

	CString	strValue = GetValueString( punkDoc, pszSection, pszEntry, strDef );

	if( !strValue.GetLength() )
		return crDefault;

	if( ::sscanf( strValue, "(%d, %d, %d)", &r, &g, &b ) == 3 )
	{
		return RGB( r, g, b );
	}

	SetValueColor( punkDoc, pszSection, pszEntry, crDefault );
	return crDefault;
}

//set the colors value from NamedData for fhases extract
std_dll void SetValuePhase( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF crBegin,  COLORREF crEnd, COLORREF crChange)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int	rBegin = GetRValue( crBegin );
	int	gBegin = GetGValue( crBegin );
	int	bBegin = GetBValue( crBegin );

	int	rEnd = GetRValue( crEnd );
	int	gEnd = GetGValue( crEnd );
	int	bEnd = GetBValue( crEnd );

	int	rChange = GetRValue( crChange );
	int	gChange = GetGValue( crChange );
	int	bChange = GetBValue( crChange );

	CString	strDef;
	strDef.Format( "[(%d, %d, %d) - (%d, %d, %d)] <-> (%d, %d, %d)", rBegin, gBegin, bBegin, rEnd, gEnd, bEnd, rChange, gChange, bChange );
	SetValue( punkDoc, pszSection, pszEntry, strDef );
}

//get the colors value from NamedData for fhases extract
std_dll void GetValuePhase( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF& crBegin,  COLORREF& crEnd, COLORREF& crChange )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int	rBegin = GetRValue( crBegin );
	int	gBegin = GetGValue( crBegin );
	int	bBegin = GetBValue( crBegin );

	int	rEnd = GetRValue( crEnd );
	int	gEnd = GetGValue( crEnd );
	int	bEnd = GetBValue( crEnd );

	int	rChange = GetRValue( crChange );
	int	gChange = GetGValue( crChange );
	int	bChange = GetBValue( crChange );

	CString	strDef;
	strDef.Format( "[(%d, %d, %d) - (%d, %d, %d)] <-> (%d, %d, %d)", rBegin, gBegin, bBegin, rEnd, gEnd, bEnd, rChange, gChange, bChange );

	CString	strValue = GetValueString( punkDoc, pszSection, pszEntry, strDef );

	if( !strValue.GetLength() )
		return;

	if( ::sscanf( strValue, "[(%d, %d, %d) - (%d, %d, %d)] <-> (%d, %d, %d)", &rBegin, &gBegin, &bBegin, &rEnd, &gEnd, &bEnd, &rChange, &gChange, &bChange ) == 9 )
	{
		crBegin = RGB( rBegin, gBegin, bBegin );
		crEnd = RGB( rEnd, gEnd, bEnd );
		crChange = RGB( rChange, gChange, bChange );
	}
}


//get double value from NamedData
std_dll double GetValueDouble(IUnknown *punkDoc, const char *pszSection, const char *pszEntry, double fDefault)
{
//	try
	{
		_variant_t	var(fDefault);
		var = ::GetValue(punkDoc, pszSection, pszEntry, var);

		_VarChangeType( var, VT_R8);
		return var.dblVal;
	}
	//catch (...)
	//{
	//	return fDefault;
	//}
}

//get integer value from NamedData
std_dll long GetValueInt( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lDefault )
{
//	try
	{
		_variant_t	var( lDefault );
		var = ::GetValue( punkDoc, pszSection, pszEntry, var );

		_VarChangeType( var, VT_I4 );

		if (var.vt == VT_I4)
			return var.lVal;
		else
			return lDefault;
	}
	//catch( ... )
	//{
	//	return lDefault;
	//}
}
//get __int64 value from NamedData
std_dll __int64 GetValueInt64( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, __int64 lDefault )
{
	//	try
	{
		_variant_t	var( lDefault );
		var = ::GetValue( punkDoc, pszSection, pszEntry, var );

		_VarChangeType( var, VT_I8 );

		if (var.vt == VT_I8)
			return var.lVal;
		else
			return lDefault;
	}
	//catch( ... )
	//{
	//	return lDefault;
	//}
}

//get string value from NamedData
std_dll CString GetValueString( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszDefault )
{
//	try
	{
		_variant_t	var( pszDefault );
		var = ::GetValue( punkDoc, pszSection, pszEntry, var );

		
		_VarChangeType( var, VT_BSTR );

		return CString( var.bstrVal );
	}
	//catch( ... )
	//{
	//	return CString( pszDefault );
	//}
}

time_test2 ttGetValue1("::GetValue");

//get any value from NamedData
std_dll _variant_t GetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault )
{
	//_trace_file("GetValue.log", "%s\\%s", pszSection, pszEntry);
	// time_test2_trigger tt(&ttGetValue1);

	_variant_t	var;// = varDefault;
	if( !CheckInterface( punkDoc, IID_INamedData ) )
		return varDefault;

	_bstr_t	bstrSection( pszSection ), bstrEntry( pszEntry );

	sptrINamedData	sptrData( punkDoc );
	CSaveSetupSection tmpsec(sptrData); // сохраним, какая секция была засетаплена
	sptrData->SetupSection( bstrSection );
	sptrData->GetValue( bstrEntry, &var );
	if( var.vt == VT_EMPTY )
	{
		sptrData->SetValue( bstrEntry, varDefault );
		return varDefault;
	}

	return var;
}

//get pointer value from NamedData. You should free it using "delete"
std_dll byte *GetValuePtr( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long *plSize/* = 0*/ )
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

time_test2 ttSetValue1("::SetValue");

//set pointer value to NamedData
std_dll void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, byte *ptr, long lSize )
{
	//time_test2_trigger tt(&ttSetValue1);

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

	TRACE("0x%x allocated\n", pdata);

	var = _variant_t((long)pdata);
	var.vt = VT_BITS;
	//var.pbVal = pdata;
	::SetValue(punkDoc, pszSection, pszEntry, var);
}
//set double value to NamedData
std_dll void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, double fValue )
{
	_variant_t	var(fValue);
	::SetValue(punkDoc, pszSection, pszEntry, var);
}

//set integer value to NamedData
std_dll void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lValue )
{
	_variant_t	var( lValue );
	::SetValue( punkDoc, pszSection, pszEntry, var );
}

//set __int64 value to NamedData
std_dll void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, __int64 lValue )
{
	_variant_t	var( lValue );
	::SetValue( punkDoc, pszSection, pszEntry, var );
}

//set string value to NamedData
std_dll void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszValue )
{
	_variant_t	var( pszValue );
	::SetValue( punkDoc, pszSection, pszEntry, var );
}

//set any value to NamedData
std_dll void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault )
{
	_variant_t	var = varDefault;
	if( !CheckInterface( punkDoc, IID_INamedData ) )
		return;

	_bstr_t	bstrSection( pszSection ), bstrEntry( pszEntry );

	sptrINamedData	sptrData( punkDoc );
	CSaveSetupSection tmpsec(sptrData); // сохраним, какая секция была засетаплена
	sptrData->SetupSection( bstrSection );
	sptrData->SetValue( bstrEntry, var );
}

//return entries counter in given section
std_dll long GetEntriesCount( IUnknown *punkDoc, const char *pszSection )
{
	if( !CheckInterface( punkDoc, IID_INamedData ) )
		return 0;

	_bstr_t	bstrSection( pszSection );

	sptrINamedData	sptrData( punkDoc );
	CSaveSetupSection tmpsec(sptrData); // сохраним, какая секция была засетаплена
	sptrData->SetupSection( bstrSection );

	long	nCounter = 0;
	sptrData->GetEntriesCount( &nCounter );

	return nCounter;
}


//return the entry name in given section
std_dll CString GetEntryName( IUnknown *punkDoc, const char *pszSection, long nPos )
{
	if( !CheckInterface( punkDoc, IID_INamedData ) )
		return "";

	_bstr_t	bstrSection( pszSection );

	sptrINamedData	sptrData( punkDoc );
	CSaveSetupSection tmpsec(sptrData); // сохраним, какая секция была засетаплена
	sptrData->SetupSection( bstrSection );

	BSTR	bstrName;
	sptrData->GetEntryName( nPos, &bstrName );

	CString	str( bstrName );
	::SysFreeString( bstrName );

	return str;
}

//delete an entry or section
std_dll bool DeleteEntry( IUnknown *punkDoc, const char *pszEntry )
{
	if( !CheckInterface( punkDoc, IID_INamedData ) )
		return false;;

	_bstr_t	bstrEntry( pszEntry );
	sptrINamedData	sptrData( punkDoc );
	
	return (FAILED(sptrData->DeleteEntry( bstrEntry ))) ? false : true;
}



//create the full pathname from filename (add path to executable module)
std_dll CString MakeAppPathName( const char *szFileName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	char szPath[_MAX_PATH], szDrv[_MAX_DRIVE], szDir[_MAX_DIR], szFile[_MAX_FNAME], szExt[_MAX_EXT];

	::GetModuleFileName( AfxGetInstanceHandle(), szPath, sizeof( szPath ) );
	::GetLongPathName( szPath, szPath, _MAX_PATH ) ;

	::_splitpath( szPath, szDrv, szDir, 0, 0 );
	::_splitpath( szFileName, 0, 0, szFile, szExt );
	::_makepath( szPath, szDrv, szDir, szFile, szExt );

	return szPath;
}

//create the full filename from filename and filepath
std_dll CString MakeFillFileName( const char *szPath, const char *szFileName )
{
	char szPathRes[_MAX_PATH], szDrv[_MAX_DRIVE], szDir[_MAX_DIR], szFile[_MAX_FNAME], szExt[_MAX_EXT];

	::_splitpath( szPath, szDrv, szDir, 0, 0 );
	::_splitpath( szFileName, 0, 0, szFile, szExt );
	::_makepath( szPathRes, szDrv, szDir, szFile, szExt );

	return szPathRes;
}

/*
//initialize global pointer to the application unknwon
extern "C" std_dll void InitAppUnknown( IUnknown *punkApp )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (g_punkApp)
		g_punkApp->Release();
//	ASSERT( !g_punkApp );	//should be called only once
	g_punkApp = punkApp;
	if (g_punkApp)
		g_punkApp->AddRef();
//	ASSERT( g_punkApp );	//punk can't be zero
}
*/


typedef INamedDataInfo* (*GTI)(BSTR);
GTI g_pfn = 0;

std_dll INamedDataInfo	*FindTypeInfo( BSTR	bstrType )
{
	PROFILE_TEST( "common.dll\\utils.cpp\\FindTypeInfo" );

	if( !g_pfn )
	{
		HINSTANCE hshell = GetModuleHandle("shell.exe");
		if( hshell )
		{
			g_pfn = (GTI)GetProcAddress( hshell, "VTGetTypeInfo" );
			return g_pfn( bstrType );
		}
	}
	else	
		return g_pfn( bstrType );

	return 0;
}


//create an instance of specified type
std_dll IUnknown *CreateTypedObject( const char *szObjectType )
{
	//PROFILE_TEST( "common.dll\\utils.cpp\\CreateTypedObject()" )

	ASSERT( GetAppUnknown(false) );

	_bstr_t	bstrType( szObjectType );
	INamedDataInfo	*pTypeInfo = FindTypeInfo( bstrType );
	
	if( pTypeInfo == 0 )
	{
		LOCAL_RESOURCE;
		AfxMessageBox( IDS_NOTYPEINFO );
		return 0;
	}

	IUnknown	*punkObject = 0;

	pTypeInfo->CreateObject( &punkObject );
	pTypeInfo->Release();

	return punkObject;
}

std_dll CString CreateUniqueName(IUnknown *punkData, const char *szObjectBase )
{
	INamedDataPtr	ptrData( punkData );
	int idx;
	for( idx = strlen( szObjectBase )-1; idx > 0; idx-- )
	{
		if( szObjectBase[idx] < '0' || szObjectBase[idx] > '9' )
			break;
	}

	int	nStart = 1;
	const char	*pnum = szObjectBase+idx+1;
	if( *pnum )sscanf( pnum, "%d", &nStart );

	char	sz[200];
	idx = min( sizeof( sz )-20, idx+1 );
	strncpy( sz, szObjectBase, idx );
	sz[idx]=0;

	for( int i = nStart;; i++ )
	{
		::sprintf( sz+idx, "%d", i );

		_bstr_t	bstr( sz );
		long	lExist = 0;
		ptrData->NameExists( bstr, &lExist );
		if( !lExist )
			return sz;
	}
}

std_dll HRESULT GenerateUniqueNameForObject( IUnknown *punkObject, IUnknown *punkData )
{
	ASSERT(punkObject!=0);
	ASSERT(punkData!=0);

	BSTR	bstrTemplate;

	INamedObject2Ptr	ptrNamed( punkObject );
	if(ptrNamed==0) return E_FAIL; // 29.09.2005 build 91, SBT 1442 - из-за какого-то изврата вызываем GenerateUniqueNameForObject(0)

	ptrNamed->GetName( &bstrTemplate );

	{
		INamedDataPtr	ptrData( punkData );
		if(ptrData==0) return E_FAIL;
		long	lExist = 0;
		ptrData->NameExists( bstrTemplate, &lExist );
		if( !lExist )
		{
			SysFreeString( bstrTemplate );
			return S_OK;
		}
	}

	CString	strTemplate = bstrTemplate;
	::SysFreeString( bstrTemplate );

	if( strTemplate.IsEmpty() )
	{
		IUnknown	*punkTI = 0;
		INamedDataObjectPtr	ptrNamedDataObject( punkObject );
		if(ptrNamedDataObject==0) return E_FAIL; // 29.09.2005 build 91, SBT 1442 - из-за какого-то изврата вызываем GenerateUniqueNameForObject(0)

		ptrNamedDataObject->GetDataInfo( &punkTI );

		INamedObject2Ptr	ptrNamedObject( punkTI );
		if(ptrNamedObject==0) return E_FAIL;
		punkTI->Release();
		ptrNamedObject->GetUserName( &bstrTemplate );

		strTemplate = bstrTemplate;
		::SysFreeString( bstrTemplate );
	}

	_bstr_t	bstrName = CreateUniqueName( punkData, strTemplate );
	ptrNamed->SetName( bstrName );

	return S_OK;
}





//find object in namedData by name and return it
std_dll IUnknown *FindObjectByName(IUnknown *punkData, const char *szName)
{
	sptrIDataTypeManager	sptrM(punkData);
	if (sptrM == 0 || !lstrlen(szName))
		return 0;

	long	nTypesCounter = 0;
	sptrM->GetTypesCount(&nTypesCounter);
	// for all types in documentA
	for (long nType = 0; nType < nTypesCounter; nType++)
	{
		LONG_PTR	lpos = 0;
		// for all objects in type
		sptrM->GetObjectFirstPosition(nType, &lpos);

		while (lpos )
		{
			// get next object
			IUnknown	*punkParent = 0;
			sptrM->GetNextObject(nType, &lpos, &punkParent);
			// try find object by name
			IUnknown *punk = FindChildByName(punkParent, szName);
			// if object is found
			if (punk)
			{
				// and this object is not parent object
				if (::GetObjectKey(punk) != ::GetObjectKey(punkParent))
					punkParent->Release();

				return punk;
			}
			// continue
			punkParent->Release();
		}
	}

	return 0;
}

//find child object in namedData by name and return it
std_dll IUnknown *FindChildByName(IUnknown *punkParent, const char *szName)
{
	if (!punkParent || !lstrlen(szName))
		return 0;

	if (::GetObjectName(punkParent) == CString(szName))
		return punkParent;

	INamedDataObject2Ptr sptrParent = punkParent;
	if (sptrParent == 0)
		return 0;

	POSITION lPos = 0;
	sptrParent->GetFirstChildPosition(&lPos);

	while (lPos)
	{
		IUnknown *punkChild = 0;
		sptrParent->GetNextChild(&lPos, &punkChild);
		if (!punkChild)
			continue;
	
		IUnknown * punk = FindChildByName(punkChild, szName);
		if (punk)
		{
			if (::GetObjectKey(punk) != ::GetObjectKey(punkChild))
				punkChild->Release();
			return punk;
		}
		// continue to next child
		punkChild->Release();
	}
	return 0;
}

//extract object kind from Unknown
std_dll CString GetObjectKind( IUnknown *punkObj )
{
	sptrINamedDataObject	sptrO( punkObj );

	BSTR	bstrKind = 0;
	if (sptrO == 0 || FAILED(sptrO->GetType( &bstrKind )) || !bstrKind)
		return "";
	
	CString	strKind = bstrKind;
	::SysFreeString( bstrKind );

	return strKind;
}

//return key by request interface INumberedObect. If key can't be obtained, return -1 (NOKEY)
std_dll GuidKey GetObjectKey( IUnknown *punk )
{
	GuidKey key;

	INumeredObject * punkNum = 0;
	if (!punk || FAILED(punk->QueryInterface(IID_INumeredObject, (void**)&punkNum)) || !punkNum)
		return key;

	punkNum->GetKey( &key );
	punkNum->Release();

	return key;
}

//extract object name from Unknown
std_dll CString GetObjectName( IUnknown *punk )
{
	INamedObject2Ptr	sptr( punk );
	if (sptr == 0)
		return "";

	BSTR	bstr = 0;
	if (FAILED(sptr->GetName(&bstr)) || !bstr)
		return "";

	CString	str(bstr);
	::SysFreeString(bstr);

	return str;
}

//assign new name to the object
std_dll bool SetObjectName( IUnknown *punkObj, const char *szNewName )
{
	_bstr_t	bstrNewName = szNewName;
	INamedObject2Ptr	sptr( punkObj );
	return (sptr != 0 && SUCCEEDED(sptr->SetName( bstrNewName ))) ? true : false;
}



//execute the specified action
std_dll bool ExecuteAction( const char *szActionName, const char *szActionparam, DWORD dwFlags )
{
	IUnknown	*punk = _GetOtherComponent( GetAppUnknown(false), IID_IActionManager );

	sptrIActionManager sptrAM( punk );

	if( punk )
		punk->Release();

	if( sptrAM == 0 )
		return false;

	_bstr_t	bstrActionName = szActionName;
	_bstr_t	bstrExecuteParams = szActionparam;

	return (SUCCEEDED(sptrAM->ExecuteAction( bstrActionName, bstrExecuteParams, dwFlags ))) ? true : false;
}

// return action name by Resource ID
std_dll CString GetActionNameByResID(UINT nResID)
{
	CString strRes, strName;

	strRes.LoadString(nResID);
	int pos = strRes.Find("\n", 0);
	strName = strRes.Left(pos);

	return strName;
}



//return the application's unknown pointer
extern "C" std_dll IApplication* GetAppUnknown( bool bAddRef )
{
	static IApplication	*papp = 0;

	if( !papp ) 
	{
		IUnknown * punkApp = 0;
		typedef IUnknown* (*PGUARDGETAPPUNKNOWN)(bool bAddRef/* = false*/);
		HMODULE hModule = GetModuleHandle( 0 );
		if (hModule)
		{
			PGUARDGETAPPUNKNOWN pfn = (PGUARDGETAPPUNKNOWN)GetProcAddress(hModule, "GuardGetAppUnknown");
			if (pfn)
				punkApp = pfn(false);
		}
		if( punkApp )
			punkApp->QueryInterface( IID_IApplication, (void**)&papp );
		if( !papp )
			return 0;
		papp->Release();
	}

	if( bAddRef )papp->AddRef();
	return papp;
}


//return the document by document key
std_dll IUnknown *GetDocByKey( GuidKey lDocKey )
{
	sptrIApplication sptrA( GetAppUnknown() );

	LONG_PTR	lPosTemplate = 0;
	
	if (FAILED(sptrA->GetFirstDocTemplPosition( &lPosTemplate )))
		return 0;

	while( lPosTemplate )
	{
		LONG_PTR	lPosDoc = 0;

		if (SUCCEEDED(sptrA->GetFirstDocPosition( lPosTemplate, &lPosDoc )))
		{

			while( lPosDoc )
			{
				IUnknown *punkDoc = 0;
				if (FAILED(sptrA->GetNextDoc( lPosTemplate, &lPosDoc, &punkDoc )) || !punkDoc)
					continue;

				ASSERT( punkDoc );

				if( GetObjectKey( punkDoc ) == lDocKey )
					return punkDoc;

				if (punkDoc)
					punkDoc->Release();
			}
		}

		sptrA->GetNextDocTempl( &lPosTemplate, 0, 0 );
	}

	return 0;
}

//return typeinfo by name
std_dll IUnknown *GetTypeByName( const char *szType, bool bAddRef )
{
	_bstr_t	bstrType = szType;
	IUnknown	*punkType = FindTypeInfo( bstrType );

	if( punkType && !bAddRef )
		punkType->Release();

	return punkType;
}


//find object in namedData by name and return it
std_dll IUnknown *GetObjectByName( IUnknown *punkData, const char *szName, const char *szType )
{
	sptrIDataTypeManager	sptrM( punkData );
	if( sptrM == 0 )
		return 0;

	long	nTypesCounter = 0;
	sptrM->GetTypesCount( &nTypesCounter );

	for( long nType = 0; nType < nTypesCounter; nType ++ )
	{
		BSTR	bstrTypeName = 0;
		sptrM->GetType( nType, &bstrTypeName );

		CString	str = bstrTypeName;

		::SysFreeString( bstrTypeName );

		if( szType && str != szType )
			continue;

		IUnknown	*punkObj = 0;
		LONG_PTR	lpos = 0;

		sptrM->GetObjectFirstPosition( nType, &lpos );

		while( lpos )
		{
			punkObj = 0;
			sptrM->GetNextObject( nType, &lpos, &punkObj );

			if( ::GetObjectName( punkObj ) == szName )
				return punkObj;
            
			//CObjectListWrp	list( punkObj );
			punkObj->Release();

			/*
			POSITION	pos = list.GetFirstObjectPosition();

			while( pos )
			{
				IUnknown	*punkObject = list.GetNextObject( pos );

				if( ::GetObjectName( punkObject ) == szName )
					return punkObject;

				punkObject->Release();
			}
			*/

			
		}
	}

	return 0;
}

//find object in namedData by name and return it
std_dll IUnknown *GetObjectByKey( IUnknown *punkData, GuidKey lKey )
{
	PROFILE_TEST( "common::utils::GetObjectByKey" )

	sptrIDataTypeManager	sptrM( punkData );

	if (sptrM == 0)
		return 0;

	long	nTypesCounter = 0;
	sptrM->GetTypesCount( &nTypesCounter );

	for( long nType = 0; nType < nTypesCounter; nType ++ )
	{
		IUnknown	*punkObj = 0;
		LONG_PTR	lpos = 0;

		sptrM->GetObjectFirstPosition( nType, &lpos );

		while( lpos )
		{
			punkObj = 0;
			sptrM->GetNextObject( nType, &lpos, &punkObj );

			if( ::GetObjectKey( punkObj ) == lKey )
				return punkObj;

			punkObj->Release();
		}
	}

	return 0;
}


/////running script state
struct	ScriptRuntimeInfo
{
	GuidKey m_lOwnerKey;
	SCRIPTTHREADID	stid;
};

class CScriptRuntimeList : public CTypedPtrList<CPtrList, ScriptRuntimeInfo*>
{
public:
	~CScriptRuntimeList()
	{
		POSITION	pos = GetHeadPosition();
		while( pos ) delete GetNext( pos );
	}
};

CScriptRuntimeList	m_listScripts;

//terminater the scripts with given thread id
std_dll void TerminateScripts( const GuidKey lKeyToTerminate )
{
	IUnknown	*punkScript = 0;	
	sptrIApplication	sptrA( GetAppUnknown() );

	sptrA->GetActiveScript( &punkScript );
	if( !punkScript )	return;

	IScriptEnginePtr ptr_se( punkScript );
	if( punkScript )
		punkScript->Release();	punkScript = 0;

	if( ptr_se == 0 )	return;

	IUnknown* punk_as = 0;
	ptr_se->GetActiveScript( &punk_as );
 	IActiveScriptPtr		sptrAS = punk_as;
	if( punk_as )	
		punk_as->Release();		punk_as = 0;

	POSITION pos = m_listScripts.GetHeadPosition();

	while( pos )
	{
		POSITION	posCur = pos;
		ScriptRuntimeInfo	*psri = m_listScripts.GetNext( pos );

		if( psri->m_lOwnerKey == lKeyToTerminate )
		{
			EXCEPINFO	ei;
			ZeroMemory( &ei, sizeof( ei ) );
			SCRIPTTHREADSTATE sts = SCRIPTTHREADSTATE_NOTINSCRIPT;


			call( sptrAS->GetScriptThreadState( psri->stid, &sts  ) );
			call( sptrAS->InterruptScriptThread( psri->stid, 0, 0  ) );
			call( sptrAS->GetScriptThreadState( psri->stid, &sts  ) );

			delete psri;

			m_listScripts.RemoveAt( posCur );
		}
	}
}
//execute the specified script
std_dll bool ExecuteScript( BSTR bstrScript, const char* psz_name, bool bFromActionManager, GuidKey lOwnerKey, DWORD dwFlags )
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

	/*if( lOwnerKey != INVALID_KEY )
	{
		ScriptRuntimeInfo	*psri = new ScriptRuntimeInfo;

		psri->m_lOwnerKey = lOwnerKey;
		sptrS->GetCurrentScriptThreadID( &psri->stid );

		m_listScripts.AddHead( psri );
	}*/

	return true;
}

//delete the specified object from NamedData
std_dll bool DeleteObject( IUnknown *punkData, IUnknown *punkObjToDelete )
{
	CString	str = GetObjectPath( punkData, punkObjToDelete );

	if( str.IsEmpty() )return false;
	return ::DeleteEntry( punkData, str );
}

//rename the specified object
std_dll bool RenameObject( IUnknown *punkData, IUnknown *punkObjToRename, const char *szName )
{
	punkObjToRename->AddRef();

	INamedDataPtr sptrData = punkData;
	IUnknownPtr sptrContext;
	if (sptrData)
		sptrData->GetActiveContext(&sptrContext);

	CContextLockUpdate lock(sptrContext, true);

	CString	strOldPath = GetObjectPath( punkData, punkObjToRename );
	
	if (!::DeleteEntry( punkData, strOldPath ))
		return false;

	_variant_t	var( punkObjToRename );

	if (!::SetObjectName( punkObjToRename, szName ))
		return false;

	punkObjToRename->Release();

	sptrINamedData	sptrD( punkData );
	sptrD->SetValue( 0, _variant_t( punkObjToRename ) );

	return true;
}


//GetObjectPath helper
CString	_get_object_path( IUnknown *punkDD, const char *sz, const GuidKey lKeyTest )
{
	sptrINamedData	sptrD( punkDD );

	_bstr_t	bstrPath = sz;

	sptrD->SetupSection( bstrPath );

	long nCount = 0;
	sptrD->GetEntriesCount( &nCount );

	CStringArray	sa;

	for( long nObj = 0; nObj < nCount; nObj++ )
	{
		BSTR	bstrName = 0;

		_variant_t	var;

		sptrD->GetEntryName( nObj, &bstrName );
		sptrD->GetValue( bstrName, &var );

		CString	str = bstrName;
		::SysFreeString( bstrName );

		sa.Add( CString(sz)+"\\"+str );

		if( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH )
			if( ::GetObjectKey( var.punkVal ) == lKeyTest )
				return sa[nObj];
	}

	for( nObj = 0; nObj < nCount; nObj++ )
	{
		CString	str = _get_object_path( punkDD, sa[nObj], lKeyTest );

		if( !str.IsEmpty() )
			return str;
	}



	return "";
}


//return path to the object in NamedData
std_dll CString GetObjectPath( IUnknown *punkData, IUnknown *punkObj )
{
	CString	str = _get_object_path( punkData, 0, ::GetObjectKey( punkObj ) );
	return str;
}

//return path to the object in NamedData
std_dll CString GetObjectPath( IUnknown *punkData, IUnknown *punkObj, const char *sz )
{
	CString	str = _get_object_path( punkData, sz, ::GetObjectKey( punkObj ) );
	return str;
}

std_dll bool GetDataDump(LPCTSTR szFileName, IUnknown *punkData)
{
	CString strEnd("\n");
	CString strNum("%x = %d");
	CStdioFile	file;

	try
	{
		if (!CheckInterface(punkData, IID_INamedData))
			return false;

		sptrINamedData	sptrD(punkData);

		if (!szFileName || !lstrlen(szFileName))
			return false;

		CFileException fe;
		if (!file.Open(szFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite , &fe))
			AfxThrowFileException(fe.m_cause, fe.m_lOsError, fe.m_strFileName);

		file.SeekToEnd();

		CArchive ar(&file, CArchive::store);

		CTime t = CTime::GetCurrentTime();

		ar << t;
		ar << strEnd;

		CString str;
		str.Format(strNum, punkData, punkData);
		str = "INamedData :" + str + strEnd;
		ar << str;

		sptrIDataTypeManager sptrT(sptrD);

		long nCount = 0; 
		sptrT->GetTypesCount(&nCount);

		for (long i = 0; i < nCount; i++)
		{
			BSTR bstr = 0;
			sptrT->GetType(i, &bstr);
			
			CString strType = bstr;
			::SysFreeString(bstr);

			ar << strType + strEnd;
			
			LONG_PTR	lpos = 0;
			sptrT->GetObjectFirstPosition(i, &lpos);

			while (lpos)
			{
				IUnknown *punkObj = 0;
				sptrT->GetNextObject(i, &lpos, &punkObj);

				if (!punkObj)
					continue;
				
				CString strPath = ::GetObjectPath(sptrT, punkObj);
				CString strName = ::GetObjectName(punkObj);
				CString strPath2 = ::GetObjectPathByName(sptrT, NULL, strName);
				
				str.Format(strNum, punkObj, punkObj);

				punkObj->Release();

				strPath += ";" + strPath2 + CString(":ObjectName:") + strName + CString(":val=") + str;

				ar << strPath + strEnd;
			}
			ar << CString("============================ \n");
		}
		ar << CString("data end ============================ \n");
		ar.Flush();
//		file.Close();
	}
	catch( ... )
	{
		return false;
	}
	return true;
}

// get object path  by name only
CString GetObjectPathByName(IUnknown * pNamedData, LPCTSTR szpath, LPCTSTR szObjName)
{
	IUnknown*	punk = NULL;
	CString		strRes;

	try 
	{
		if (!CheckInterface(pNamedData, IID_INamedData))
			return "";

		sptrINamedData sptrData(pNamedData);

		CString strFindName(szObjName);
		// check strObjName valid; if not return NULL (object with empty name naver exists)
		if (strFindName.IsEmpty())
			return "";

		CString CurrentPath((!szpath) ? "" : szpath);
		_bstr_t bstrPath(CurrentPath);

		// set section
		sptrData->SetupSection(bstrPath);

		// get count of entries
		long EntryCount = 0;
		sptrData->GetEntriesCount(&EntryCount);
	
		// for all entries
		for (int  i = 0; i < (int)EntryCount; i++)
		{
			_try(common, SubGetObjectByName)
			{
				BSTR bstr = NULL;
				// get entry name
				sptrData->GetEntryName(i, &bstr);

				// destructor will release system memory from bstr
				_bstr_t bstrName(bstr, false);
				// format a full path for entry
				_bstr_t bstrPathNew = bstrPath + "\\" + bstrName;
		
				// get value
				_variant_t var;
				sptrData->GetValue(bstrPathNew, &var);

				// check for IUnknown
				if (var.vt == VT_UNKNOWN)
				{
					// it's must be namedObject
					punk = (IUnknown*)var;
					// get object name 
					CString strObjectName = ::GetObjectName(punk);
					
					// compare object's  name with Looking to  name
					if (strObjectName == strFindName)
					{
						strRes = (LPCTSTR)bstrPathNew;
						return strRes; 
					}

					// else release this pointer and look to again
					punk->Release();
					punk = NULL;
						
				} // IUnknown
				
				// if this entry has children => we want to walk throught them
				if (EntryCount > 0)
				{
					strRes = GetObjectPathByName((IUnknown*)sptrData.GetInterfacePtr(), bstrPathNew, strFindName);
					if (!strRes.IsEmpty())
						return strRes;
				}

				// for next entry on this level restore Section
				sptrData->SetupSection(bstrPath);

			}
			_catch // for all exceptions
			{
				continue;
			}
		} // for all entries

	}
	catch (...)
	{
		return "";
	}
	return "";
}


inline int _floor( double x )
{	return (int(x) == x)?int(x):int(x)+1;	}
inline int _ceil( double x )
{	return int(x);	}



//get zoom of zoom site
std_dll double GetZoom( IUnknown  *punk )
{
	sptrIScrollZoomSite	sptrS( punk );
	if( sptrS == 0 )return 1;

	double	fZoom = 1;
	if (sptrS)
		sptrS->GetZoom( &fZoom );

	return fZoom;
}

//get scroll position
std_dll CPoint GetScrollPos( IUnknown  *punk )
{
	sptrIScrollZoomSite	sptrS( punk );
	if( sptrS == 0 )return CPoint( 0, 0 );

	CPoint point( 0, 0 );

	if( sptrS )
		sptrS->GetScrollPos( &point );

	return point;
}

//coordinate mapping - screen to client
std_dll CRect ConvertToClient( IUnknown *punkSSite, CRect rc )
{
	double fZoom = GetZoom( punkSSite );
	CPoint ptScroll = GetScrollPos( punkSSite );

	rc.left = _floor( (rc.left+ptScroll.x) / fZoom );//_AjustValueEx( 1./fZoom, (rc.left+ptScroll.x) / fZoom );
	rc.right = _ceil( (rc.right+ptScroll.x) / fZoom );
	rc.top = _floor( (rc.top+ptScroll.y) / fZoom );
	rc.bottom = _ceil( (rc.bottom+ptScroll.y) / fZoom );

	return rc;
}

std_dll CPoint ConvertToClient( IUnknown *punkSSite, CPoint pt )
{
	double fZoom = GetZoom( punkSSite );
	CPoint ptScroll = GetScrollPos( punkSSite );

	pt.x = int( (pt.x+ptScroll.x) / fZoom );
	pt.y = int( (pt.y+ptScroll.y) / fZoom );

	return pt;
}

std_dll CSize ConvertToClient( IUnknown *punkSSite, CSize size )
{
	double fZoom = GetZoom( punkSSite );

	size.cx = int( size.cx / fZoom );
	size.cy = int( size.cy / fZoom );

	return size;
}

//coordinate mapping - client to screen
std_dll CRect ConvertToWindow( IUnknown *punkSSite, CRect rc )
{
	double fZoom = GetZoom( punkSSite );
	CPoint ptScroll = GetScrollPos( punkSSite );

	/*if( fZoom >= 1 )
	{
		rc.left = _floor( rc.left*fZoom-ptScroll.x );
		rc.right = _ceil( rc.right*fZoom-ptScroll.x );
		rc.top = _floor( rc.top*fZoom-ptScroll.y );
		rc.bottom = _ceil( rc.bottom*fZoom-ptScroll.y );
	}
	else*/
	{
		rc.left = _ceil( rc.left*fZoom-ptScroll.x );
		rc.right = _floor( rc.right*fZoom-ptScroll.x );
		rc.top = _ceil( rc.top*fZoom-ptScroll.y );
		rc.bottom = _floor( rc.bottom*fZoom-ptScroll.y );
	}

	return rc;
}

std_dll CPoint ConvertToWindow( IUnknown *punkSSite, CPoint pt )
{
	double fZoom = GetZoom( punkSSite );
	CPoint ptScroll = GetScrollPos( punkSSite );

	pt.x = _AjustValue( fZoom, pt.x * fZoom - ptScroll.x );
	pt.y = _AjustValue( fZoom, pt.y * fZoom - ptScroll.y );

	return pt;
}

std_dll CSize ConvertToWindow( IUnknown *punkSSite, CSize size )
{
	double fZoom = GetZoom( punkSSite );

	size.cx = _floor( size.cx*fZoom );
	size.cy = _floor( size.cy*fZoom );

	return size;
}

// copy source menu to dst menu
std_dll HMENU CopyMenu(HMENU hSrc)
{
	CMenu menu, *pMenu = NULL;
	UINT nDefItem = -1;

	try
	{
		CMenu* pMenu = CMenu::FromHandle (hSrc);

		if (!pMenu)
			return NULL;
	
		int nCount = (int)pMenu->GetMenuItemCount();

		if (nCount == -1)
			return NULL;
		
		menu.CreatePopupMenu();

		for (int i = 0; i < nCount; i ++)
		{
			HMENU hSubMenu = NULL;

			CString strText;

			pMenu->GetMenuString(i, strText, MF_BYPOSITION);
			UINT uiCmd = pMenu->GetMenuItemID(i);
			UINT uiState = pMenu->GetMenuState(i, MF_BYPOSITION);

			switch (uiCmd)
			{
			case 0:
				if (!menu.InsertMenu(i, MF_BYPOSITION | MF_SEPARATOR))
					AfxThrowUserException();
				break;

			case -1:
				hSubMenu = CopyMenu(pMenu->GetSubMenu(i)->GetSafeHmenu());
				if (!menu.InsertMenu(i, MF_BYPOSITION | MF_STRING | MF_POPUP | uiState, (UINT_PTR)hSubMenu, strText))
					AfxThrowUserException();

				if (hSubMenu)
					::DestroyMenu(hSubMenu);

				hSubMenu = NULL;
				
				break;

			default:
				if (!menu.InsertMenu(i, MF_BYPOSITION | MF_STRING | uiState, uiCmd, strText))
					AfxThrowUserException();
				break;
			}// switch
		}

	
		nDefItem = pMenu->GetDefaultItem(GMDI_USEDISABLED, FALSE);
		if (nDefItem != -1)
			menu.SetDefaultItem(nDefItem, FALSE);
		
	}
	catch (...)
	{
		menu.DestroyMenu();
		return NULL;
	}
	
	HMENU hMenu = menu.Detach();
	return hMenu;
}

// copy source menu to dst menu
std_dll bool AppendMenu(HMENU & hDst, HMENU hSrc)
{
	UINT	nDefItem = -1;
	CMenu	menu;

	try
	{
		// create dst menu copy
		CMenu * pMenu = CMenu::FromHandle(hSrc);
		if (!pMenu)
			return false;
	
		menu.m_hMenu = ::CopyMenu(hDst);
		if (menu.m_hMenu == 0)
			return false;

		int nCount = (int)pMenu->GetMenuItemCount();
		int nOffset = (int)menu.GetMenuItemCount();

		if (nCount == -1 || nOffset == -1)
			AfxThrowUserException();
		
		for (int i = 0; i < nCount; i ++)
		{
			HMENU hSubMenu = NULL;

			CString strText;

			pMenu->GetMenuString(i, strText, MF_BYPOSITION);
			UINT uiCmd = pMenu->GetMenuItemID(i);
			UINT uiState = pMenu->GetMenuState(i, MF_BYPOSITION);

			switch (uiCmd)
			{
			case 0:
				if (!menu.InsertMenu(nOffset + i, MF_BYPOSITION | MF_SEPARATOR))
					AfxThrowUserException();
				break;

			case -1:
				hSubMenu = CopyMenu(pMenu->GetSubMenu(i)->GetSafeHmenu());
				if (!menu.InsertMenu(nOffset + i, MF_BYPOSITION | MF_STRING | MF_POPUP | uiState, (UINT_PTR)hSubMenu, strText))
					AfxThrowUserException();

				if (hSubMenu)
					::DestroyMenu(hSubMenu);

				hSubMenu = NULL;
				
				break;

			default:
				if (!menu.InsertMenu(nOffset + i, MF_BYPOSITION | MF_STRING | uiState, uiCmd, strText))
					AfxThrowUserException();
				break;
			}// switch
		}

		nDefItem = pMenu->GetDefaultItem(GMDI_USEDISABLED, FALSE);
		if (nDefItem != -1)
			menu.SetDefaultItem(nDefItem, FALSE);
		
	}
	catch (...)
	{
		menu.DestroyMenu();
		return false;
	}
	// replace menu
	::DestroyMenu(hDst);
	hDst = menu.Detach();
	return true;
}

// copy menu skipping submenu which commands contain uiSkip
std_dll HMENU CopyMenuSkipSubMenu(HMENU hSrc, UINT uiSkip, UINT& nPosition)
{
	CMenu menu, *pMenu = NULL;
	UINT nDefItem = -1;

	try
	{
		CMenu* pMenu = CMenu::FromHandle (hSrc);

		if (!pMenu)
			return NULL;
	
		int nCount = (int)pMenu->GetMenuItemCount();

		if (nCount == -1)
			return NULL;
		
		menu.CreatePopupMenu();
	
		nPosition=-1;
		for (int i = 0; i < nCount; i ++)
		{
			HMENU hSubMenu = NULL;

			CString strText;

			pMenu->GetMenuString(i, strText, MF_BYPOSITION);
			UINT uiCmd = pMenu->GetMenuItemID(i);
			UINT uiState = pMenu->GetMenuState(i, MF_BYPOSITION);

			switch (uiCmd)
			{
			case 0:
				if (!menu.InsertMenu(i, MF_BYPOSITION | MF_SEPARATOR))
					AfxThrowUserException();
				break;

			case -1:
			{
				hSubMenu = CopyMenu(pMenu->GetSubMenu(i)->GetSafeHmenu());
				if (!menu.InsertMenu(i, MF_BYPOSITION | MF_STRING | MF_POPUP | uiState, (UINT_PTR)hSubMenu, strText))
					AfxThrowUserException();

				if (hSubMenu)
					::DestroyMenu(hSubMenu);

				hSubMenu = NULL;
				
				break;
			}
			default:
				if(uiSkip==uiCmd){
					nPosition=i;
				}else{
					if (!menu.InsertMenu(i, MF_BYPOSITION | MF_STRING | uiState, uiCmd, strText))
						AfxThrowUserException();
				}
				break;
			}// switch
		}

	
		nDefItem = pMenu->GetDefaultItem(GMDI_USEDISABLED, FALSE);
		if (nDefItem != -1)
			menu.SetDefaultItem(nDefItem, FALSE);
		
	}
	catch (...)
	{
		menu.DestroyMenu();
		return NULL;
	}
	
	HMENU hMenu = menu.Detach();
	return hMenu;
}


std_dll IUnknown *FindObjectByKeyEx(GuidKey lKey)
{
	//try to locate data object with specified key in all documents
	sptrIApplication sptrApp(::GetAppUnknown(false));

	LONG_PTR lPosTempl = 0; 
	LONG_PTR lPosDoc = 0;
	sptrApp->GetFirstDocTemplPosition(&lPosTempl);

	while (lPosTempl)
	{
		sptrApp->GetFirstDocPosition(lPosTempl, &lPosDoc);

		while (lPosDoc)
		{
			IUnknownPtr	ptrData;
			sptrApp->GetNextDoc(lPosTempl, &lPosDoc, &ptrData);
			IUnknown *punkObject = ::GetObjectByKeyEx(ptrData, lKey);

			if (punkObject)
				return punkObject;
		}
		sptrApp->GetNextDocTempl(&lPosTempl, 0, 0);
	}
	return ::GetObjectByKeyEx(GetAppUnknown(), lKey);
}


//return the object from document. Find in all avaible documents
IUnknown *FindObjectByKey( GuidKey lKey )
{
	//try to locate data object with specified key in all documents

	sptrIApplication	sptrApp( ::GetAppUnknown() );

	LONG_PTR	lPosTempl, lPosDoc;

	sptrApp->GetFirstDocTemplPosition( &lPosTempl );

	while( lPosTempl )
	{
		sptrApp->GetFirstDocPosition( lPosTempl, &lPosDoc );

		while( lPosDoc )
		{
			IUnknownPtr	ptrData;
			sptrApp->GetNextDoc( lPosTempl, &lPosDoc, &ptrData );
			IUnknown *punkObject =::GetObjectByKey( ptrData, lKey );

			if( punkObject )
			{
				return punkObject;
			}
		}
		sptrApp->GetNextDocTempl( &lPosTempl, 0, 0 );
	}
	return ::GetObjectByKey( GetAppUnknown(), lKey );
}

IStreamPtr CreateMemoryStream()
{
	IDispatchPtr pDispStream;//(__uuidof(ADO::_Stream));
	HRESULT hr=pDispStream.CreateInstance(L"ADODB.Stream");//,0,CLSCTX_INPROC_SERVER);
//	adoStream->Mode = ADO::adModeReadWrite; //read/write mode
	DISPID dispId;
	LPWSTR pOpen=L"Open";
	hr=pDispStream->GetIDsOfNames(IID_NULL,&pOpen,1,0,&dispId);
	DISPPARAMS dispParams={0};
	_variant_t	var( 0l);
	hr=pDispStream->Invoke(dispId,IID_NULL,0,DISPATCH_METHOD,&dispParams,&var,NULL,NULL);

	ADO::_StreamPtr adoStream=pDispStream;
	if( adoStream == 0 ){
		::MessageBox(NULL,_T("ADODB stream wasn't created!"),_T("Error"),MB_ICONWARNING);
		return hr;
	}
	adoStream->Type = ADO::adTypeBinary ;//- Set it as binary
	_ASSERTE(!hr);
	ADO::StreamTypeEnum streamType;
	hr=adoStream->get_Type(&streamType);

	return adoStream;
}

//clone the data object
IUnknown *CloneObject( IUnknown *punk )
{
//get object kind
	CString	strObjectKind = ::GetObjectKind( punk );
	IUnknown	*punkNew = ::CreateTypedObject( strObjectKind );

//if object support serializable interface, serialize old and deserialize new object

	if( CheckInterface( punk, IID_ISerializableObject ) )
	{
		ASSERT( CheckInterface( punkNew, IID_ISerializableObject ) );
		//CMemFile	file;
		
		IStreamPtr ptrStream = CreateMemoryStream();

		if( ptrStream==0 )
			return 0;

		sptrISerializableObject	sptrO( punk );
		sptrISerializableObject	sptrN( punkNew );


		SerializeParams	params;
		ZeroMemory( &params, sizeof( params ) );
		params.flags = sf_DetachParent;

//serialize
		{
			sptrO->Store( ptrStream, &params );

		}

		::StreamSeek( ptrStream, 0, 0 );

//de-serialize
		{
			sptrN->Load( ptrStream, &params );
		}
	}
	
	return punkNew;
}

//if object has parent, return true
std_dll bool HasParentObject( IUnknown *punk )
{
	IUnknown	*punkParent = 0;

	sptrINamedDataObject2	sptrN( punk );
	sptrN->GetParent( &punkParent );

	if( !punkParent )
		return false;

	punkParent->Release();

	return true;

}

std_dll bool ObjectProperties(IUnknown *punk)
{
	if (!CheckInterface(punk, IID_INamedDataObjectProps))
		return false;

	sptrINamedDataObjectProps sptr(punk);
	

	return (FAILED(sptr->CreatePropSheet())) ? false : true;
}

// get filter unknown what can work object of 'Type' type
std_dll IUnknown* GetFilterByType(LPCTSTR szType)
{
	CString		strType(szType);

	_try (common, GetFilterByType)
	{
		if (strType.IsEmpty())
			return NULL;

		// get Application
		IUnknown * punk = ::GetAppUnknown();
		sptrIApplication sptrApp(punk);

		if (!punk)
			return NULL;
		else 
			punk->Release();

		// for all document templates
		LONG_PTR nPos = 0;
		sptrApp->GetFirstDocTemplPosition(&nPos);

		while (nPos)
		{
			BSTR		bstrTemplName;
			// get doc template and it's attributes
			sptrApp->GetNextDocTempl(&nPos, &bstrTemplName, NULL);

			CString strTemplName = bstrTemplName;
			::SysFreeString(bstrTemplName);

			// fill manager of filters
			CCompManager manFilters(strTemplName + CString("\\") + CString(szPluginExportImport), sptrApp);

			for (int i = 0; i < manFilters.GetComponentCount(); i++)
			{
				IUnknown* punk = manFilters.GetComponent(i, true);
				if (!punk)
					continue;

				if (CheckFilterSupportType(strType, punk))
					return punk;
			} // for all filters
		} // for doc templates
	}
	_catch
	{
		return NULL;
	}
	return NULL;
}

// check filter support object's type
std_dll bool CheckFilterSupportType(LPCTSTR szType, IUnknown * punk)
{	
	CString strType(szType);

	if (strType.IsEmpty())
		return false;

	if (!CheckInterface(punk, IID_IFileFilter2))
		return false;

	sptrIFileFilter2 sptr(punk);

	int nCount = 0;
	// get number of supported objects' types
	sptr->GetObjectTypesCount(&nCount);
	// for all types
	for (int i = 0; i < nCount; i++)
	{
		// get type
		BSTR bstrType = 0;
		sptr->GetObjectType(i, &bstrType);
		
		CString str = bstrType;
		::SysFreeString(bstrType);
		// compare with parameters
		if (strType == str)
			return true;
	}
	// 
	return false;
}

// return fileFilter that may be used for open/saeve with requested file
std_dll IUnknown* GetFilterByFile(LPCTSTR szFileName)
{
	CDocTemplate::Confidence	conf = CDocTemplate::noAttempt, confTest = CDocTemplate::noAttempt;
	_bstr_t		bstrFileName = szFileName;
	IUnknown *	pIFilter = 0;

	CDocument *pdocDummy = 0;

	bool	bNativeFormat = true;

	_try (common, GetFilterByType)
	{
		// get Application
		IUnknown * punk = ::GetAppUnknown();
		sptrIApplication sptrApp(punk);

		if (punk)
			punk->Release();
		else
			return NULL;

		// for all document templates
		LONG_PTR nPos = 0;
		sptrApp->GetFirstDocTemplPosition(&nPos);

		while (nPos)
		{
			BSTR		bstrTemplName;
			// get doc template and it's attributes
			sptrApp->GetNextDocTempl(&nPos, &bstrTemplName, NULL);

			CString strTemplName = bstrTemplName;
			::SysFreeString(bstrTemplName);

			// fill manager of filters
			CCompManager manFilters(strTemplName + CString("\\") + CString(szPluginExportImport), sptrApp);

			for (int i = 0; i < manFilters.GetComponentCount(); i++)
			{
				sptrIFileFilter2  spFilter(manFilters.GetComponent(i, false));

				spFilter->MatchFileType(bstrFileName, (DWORD*)&confTest);

				if (confTest > conf)
				{
					bNativeFormat = false;
					if (pIFilter)
						pIFilter->Release();

					pIFilter = (IUnknown*)spFilter.GetInterfacePtr();
					pIFilter->AddRef();
					conf = confTest;
				}
			} // for all filters
		} // for doc templates
	}
	_catch
	{
		return NULL;
	}

	if (bNativeFormat)
		return 0;

	return pIFilter;
}

// get object name extention from full path
std_dll CString GetFileExtention(LPCTSTR szPath)
{
	CString strExt(_T(""));

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_PATH];
	TCHAR fname[_MAX_PATH];

	_tsplitpath(szPath, drive, dir, fname, strExt.GetBuffer(_MAX_PATH));
	strExt.ReleaseBuffer();

	return strExt;
}

// Get Object name from full path
std_dll CString GetObjectNameFromPath(LPCTSTR szPath)
{
	CString strName(_T(""));

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_PATH];
	TCHAR ext[_MAX_PATH];

	_tsplitpath(szPath, drive, dir, strName.GetBuffer(_MAX_PATH), ext);
	strName.ReleaseBuffer();

	return strName;
}

//return true if dynamic action registration enabled
std_dll bool IsDynamicActionRegistrationEnabled()
{
	return g_bActionRegistrationEnabled;
}

//manage  the action registration state
std_dll void EnableActionRegistartion( bool bEnable )
{
	g_bActionRegistrationEnabled = bEnable;
}

//find the better view for given object type
std_dll CString FindMostMatchView( const char *szType, DWORD &dwMatch )
{
	sptrIApplication	sptrA( GetAppUnknown() );

	LONG_PTR	lDocTemplPos;
	_bstr_t	bstrType = szType;

	sptrA->GetFirstDocTemplPosition( &lDocTemplPos );

	DWORD	dwMaxMatch = mvNone;
	CString	strProgID;

	while( lDocTemplPos )
	{
		BSTR	bstrTemplName;
		sptrA->GetNextDocTempl( &lDocTemplPos, &bstrTemplName, 0 );
		CString	strTemplName( bstrTemplName );
		::SysFreeString( bstrTemplName );

		strTemplName=strTemplName+"\\"+szPluginView;

		CCompManager		m( strTemplName );
		CCompRegistrator	r( strTemplName );

		for( int nView=0; nView < m.GetComponentCount(); nView ++ )
		{
			sptrIView	sptrV( m.GetComponent( nView, false ) );

			DWORD	dwMatch;
			sptrV->GetMatchType( bstrType, &dwMatch );

			if( dwMatch > dwMaxMatch )
			{
				dwMaxMatch = dwMatch;
				strProgID = m.GetComponentName( nView );
			}
		}
	}

	dwMatch = dwMaxMatch;
	return strProgID;
}

//determinate is this CC name valid (we have such CC)
std_dll bool IsCCValid(const char* pszName)
{
	if( !pszName )
		return false;
	IUnknown* punk;
	punk = _GetOtherComponent(GetAppUnknown(), IID_IColorConvertorManager);
	ICompManagerPtr sptrCM(punk);
	punk->Release();
	if (sptrCM == 0)
		return false;
	int numCvtrs;
	sptrCM->GetCount(&numCvtrs);
	BSTR bstr;
	sptrIColorConvertor2 sptrCC;
	bool bFind = false;
	for(int i = 0; i < numCvtrs; i++)
	{
		sptrCM->GetComponentUnknownByIdx(i, &punk);
		sptrCC = punk;
		punk->Release();
		sptrCC->GetCnvName(&bstr);
		CString strName(bstr);
		::SysFreeString(bstr);
		if (strName == pszName)
		{
			bFind = true;
			break;
		}
	}
	if (!bFind)
	{
		return false;
	}
	return true;
}


std_dll IUnknown* GetCCByName(const char* pszName, bool bfirst_if_fail)
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
	BSTR bstr;
	sptrIColorConvertor2 sptrCC;
	bool bFind = false;
	for(int i = 0; i < numCvtrs; i++)
	{
		sptrCM->GetComponentUnknownByIdx(i, &punk);
		sptrCC = punk;
		punk->Release();
		sptrCC->GetCnvName(&bstr);
		CString strName(bstr);
		::SysFreeString(bstr);
		if (!strName.CompareNoCase( pszName ) )
			return sptrCC;
	}

	sptrCC = 0;

	if (!bFind && bfirst_if_fail )
	{
		sptrCM->GetComponentUnknownByIID( IID_IColorConvertor, &punk);
		sptrCC = punk;
		punk->Release();
	}
	return sptrCC;
}


//return the active dataobject of document
IUnknown *GetActiveDataObject( IUnknown *punkDoc )
{
	//1. Get the active context
	sptrIDocumentSite	sptrDS( punkDoc );
	if( sptrDS == 0 )	return 0;
	IUnknown	*punkV = 0;
	sptrDS->GetActiveView( &punkV );
	sptrIView	sptrV( punkV );
	sptrIDataContext	sptrC( punkV );

	if( punkV )punkV->Release();
	if( sptrC == 0 || sptrV == 0 )
		return 0;

	//2. get the better type for this view
	
	DWORD	dwMaxMatch = mvNone;
	IUnknown	*punkDataObject = 0;
	sptrIDataTypeInfoManager	sptrM( GetAppUnknown(false) );
	

	long	nTypesCount = 0;

	sptrM->GetTypesCount( &nTypesCount );
	
	for( long nType = 0; nType < nTypesCount; nType++ )
	{
		BSTR	bstrType = 0;
		DWORD	dwCurrentMatch = 0;

		INamedDataInfo *pTypeInfo = 0;
		sptrM->GetTypeInfo( nType, &pTypeInfo );
		_bstr_t bstrString = (const char *)GetObjectName( pTypeInfo );
		pTypeInfo->Release();

		sptrV->GetMatchType( bstrString, &dwCurrentMatch );

		if( dwCurrentMatch > dwMaxMatch )
		{
			IUnknown	*punkObject = 0;
			sptrC->GetActiveObject( bstrString, &punkObject );

			if( punkObject )
			{
				if( punkDataObject )
					punkDataObject->Release();
				punkDataObject = punkObject;

				dwMaxMatch = dwCurrentMatch;
			}
		}
	}

	return punkDataObject;
}

//return the clipboard format identifier
std_dll UINT GetClipboardFormat()
{
	const char	*szClipboardFormat=_T("VT5UNIFIED");
	static UINT	clipboardFormat = 0;

	if( !clipboardFormat )
		clipboardFormat = ::RegisterClipboardFormat( szClipboardFormat );
	return clipboardFormat;
}


static CMap<DWORD, DWORD, CString, LPCTSTR> s_mapClipboardFormats;

void _InitClipboardMap()
{
	CString	strObjectType;
	sptrIDataTypeInfoManager	sptrT(GetAppUnknown());

	long	nTypesCount = 0;
	sptrT->GetTypesCount( &nTypesCount );

	for( long nType = 0; nType < nTypesCount; nType++ )
	{
		BSTR	bstr = 0;
		INamedDataInfo *pTypeInfo;
		sptrT->GetTypeInfo( nType, &pTypeInfo );
		strObjectType = ::GetObjectName( pTypeInfo );
		pTypeInfo->Release();

		IUnknown	*punkO = ::CreateTypedObject( strObjectType );
		IClipboardPtr sptrClipbd(punkO);
		if(punkO)
			punkO->Release();
		if(sptrClipbd != 0)
		{
			long boolSupportPaste = FALSE;
			sptrClipbd->IsSupportPaste(&boolSupportPaste);
			if(boolSupportPaste)
			{
				long nPos = 0;
				sptrClipbd->GetFirstFormatPosition(&nPos);
				while(nPos)
				{
					UINT nRegFormat = 0;
					sptrClipbd->GetNextFormat(&nRegFormat, &nPos);
					if(nRegFormat)
						s_mapClipboardFormats.SetAt(nRegFormat, strObjectType);
				}
			}
		}
	}
}
//return the type of object that supports specified clipboard format  
std_dll CString GetObjectTypeByClipboardFormat(UINT nFormat)
{
	CString	strObjectType;
	if(s_mapClipboardFormats.IsEmpty())
	{
		_InitClipboardMap();
	}

	if(s_mapClipboardFormats.Lookup(nFormat, strObjectType))
		return strObjectType;
	else
		return "";
}

std_dll POSITION GetFirstClipboardFormatPos()
{
	if(s_mapClipboardFormats.IsEmpty())
	{
		_InitClipboardMap();
	}

	return s_mapClipboardFormats.GetStartPosition();
	
}

std_dll void GetClipboardFormatByPos(POSITION& pos, UINT& nFormat, CString& strType)
{
	s_mapClipboardFormats.GetNextAssoc(pos, (ULONG&)nFormat, strType);
}


//store the object data to hglobal
std_dll HGLOBAL StoreObjectToHandle( IUnknown *punkDataObject, CPoint point )
{
	//CTimeTest	test( true, "StoreObjectToHandle" );
	try
	{

		IStream* pStream = 0;
		CreateStreamOnHGlobal( 0, FALSE, &pStream );

		if( pStream == 0 )
			return 0;

		IStreamPtr ptrStream = pStream;
		pStream->Release();	pStream = 0;

		CStreamEx ar( ptrStream, false );


		/*
		CSharedFile file;
		CArchive ar( &file,CArchive::store );
		*/

		CString strObjectName = ::GetObjectName( punkDataObject );
		CString	strObjectType = ::GetObjectKind( punkDataObject );

		ar<<strObjectName;
		ar<<strObjectType;
		ar.Flush();

		{
			//CArchiveStream	stream( &ar );

			//IUnknown	*punkParentObject = 0;
			sptrINamedDataObject2	sptrN( punkDataObject );

			DWORD	dwFlags = 0;
			sptrN->GetObjectFlags( &dwFlags );

			bool bVirtual  = ( dwFlags & nofHasData ) == 0;

			/*sptrN->GetParent( &punkParentObject );

			if( bVirtual )
				sptrN->SetParent( 0, sdfCopyParentData );
			else
				sptrN->SetParent( 0, apfNone );*/

			if( bVirtual )
				sptrN->StoreData( sdfCopyParentData );

			SerializeParams	params;
			ZeroMemory( &params, sizeof( params ) );

			sptrISerializableObject	sptrS( punkDataObject );
			sptrS->Store( ptrStream, &params );

			if( bVirtual )
				sptrN->StoreData( apfAttachParentData );

			/*if( bVirtual )
				sptrN->SetParent( punkParentObject, apfAttachParentData );
			else
				sptrN->SetParent( punkParentObject, apfNone );
			if( punkParentObject )
				punkParentObject->Release();*/
		}

		ar<<point.x;
		ar<<point.y;

		//ar.Close();
		//file.Flush();

		HGLOBAL h = 0;
		GetHGlobalFromStream( ptrStream, &h );


		return h;
	}
	catch (COleException* pEx)
	{
		//TRACE("OLE exception: %x\r\n",	pEx->m_sc);
		pEx->Delete();
		return 0;
	}
	catch (CArchiveException* pEx)
	{
		//TRACE("Archive exception\r\n");
		pEx->Delete();
		return 0;
	}
}

//store multi frame to hglobal
#include "image5.h"
std_dll HGLOBAL StoreMultiFrameObjectsToHandle( IUnknown *punkMF, CPoint point, long* pnSize, SerializeParams *pparams )
{
	sptrIMultiSelection	sptrMF(punkMF);
	if(sptrMF == 0)
		return 0;
	try
	{

		IStream* pStream = 0;
		CreateStreamOnHGlobal( 0, FALSE, &pStream );

		if( pStream == 0 )
			return 0;

		CStreamEx ar( pStream, false );

		ar<<CString(szMultiFrameSignature);
		ar<<point.x;
		ar<<point.y;

		sptrISerializableObject	sptrS( punkMF );
		sptrS->Store( pStream, pparams );

		HGLOBAL h = 0;
		GetHGlobalFromStream( pStream, &h );

		if(pnSize)
			*pnSize = GlobalSize( h );

		

		if( pStream )
			pStream->Release();	pStream = 0;

		return h;


		/*
		CSharedFile file;
		CArchive ar( &file,CArchive::store );

		ar<<CString(szMultiFrameSignature);
		ar<<point.x;
		ar<<point.y;
		ar.Flush();
		
		CArchiveStream	stream( &ar );
		sptrISerializableObject	sptrS( punkMF );
		sptrS->Store( &stream );

		ar.Close();
		file.Flush();

		if(pnSize)
			*pnSize = file.GetLength();

		DWORD dwLen = file.GetLength();

		return file.Detach();
		*/
	}
	catch (COleException* pEx)
	{
		//TRACE("OLE exception: %x\r\n",	pEx->m_sc);
		pEx->Delete();
		return 0;
	}
	catch (CArchiveException* pEx)
	{
		//TRACE("Archive exception\r\n");
		pEx->Delete();
		return 0;
	}
}


std_dll void RestoreMultiFrameFromHandle( HGLOBAL hObj, IUnknown* punkMF,  CPoint *ppoint, SerializeParams *pparams )
{
	sptrIMultiSelection	sptrMF(punkMF);
	if(sptrMF == 0)
		return;
	try
	{
		
		IStream* pStream = 0;
		CreateStreamOnHGlobal( hObj, FALSE, &pStream );

		if( pStream == 0 )
			return;

		CStreamEx stream( pStream, true );
		
		CString strSignature;
		stream>>strSignature;
		ASSERT( strSignature == szMultiFrameSignature );
		stream>>ppoint->x;
		stream>>ppoint->y;

		sptrISerializableObject	sptrS( punkMF );
		sptrS->Load( pStream, pparams );


		if( pStream )
		{
			pStream->Release();
		}
		
		return;
		/*
		
		CSharedFile globFile;
		globFile.SetHandle( hObj, FALSE );
		CArchive ar (&globFile,CArchive::load);

		DWORD dwLen = globFile.GetLength();

		CString strSignature;
		ar>>strSignature;
		ASSERT(strSignature == szMultiFrameSignature);
		ar>>ppoint->x;
		ar>>ppoint->y;
		ar.Flush();
		
		CArchiveStream	stream( &ar );
		sptrISerializableObject	sptrS( punkMF );
		sptrS->Load( &stream );

		ar.Close();
		globFile.Detach();
		*/
		
	}
	catch (COleException* pEx)
	{
		//TRACE("OLE exception: %x\r\n",	pEx->m_sc);
		pEx->Delete();
		return;
	}
	catch (CArchiveException* pEx)
	{
		//TRACE("Archive exception\r\n");
		pEx->Delete();
		return;
	}
}

std_dll IUnknown *RestoreObjectFromHandle( HGLOBAL hObj, CPoint *ppoint )
{
	try
	{
		/*
		CSharedFile globFile;
		globFile.SetHandle( hObj );
		CArchive ar (&globFile,CArchive::load);
		*/

		IStream* pStream = 0;
		CreateStreamOnHGlobal( hObj, FALSE, &pStream );

		if( pStream == 0 )
			return 0;

		CStreamEx ar( pStream, true );


		CString	strObjectType, strObjectName;

		ar>>strObjectName;
		ar>>strObjectType;
		ar.Flush();
		
		IUnknown	*punkO = ::CreateTypedObject( strObjectType );

		if( !punkO )return 0;

		CString	strOldName = ::GetObjectName( punkO );

		{
			//CArchiveStream	stream( &ar );
			SerializeParams	params;
			ZeroMemory( &params, sizeof( params ) );

			sptrISerializableObject	sptrS( punkO );
			sptrS->Load( pStream, &params );

			_bstr_t	bstrObjectName = strObjectName;

			::SetObjectName( punkO, 0 );
			INumeredObjectPtr	sptrN( punkO );
			sptrN->GenerateNewKey( 0 );
		}

		if( ppoint )
		{
			ar>>ppoint->x;
			ar>>ppoint->y;
		}

		if( pStream )
		{
			pStream->Release();
		}


		return punkO;
	}
	catch (COleException* pEx)
	{
		//TRACE("OLE exception: %x\r\n",	pEx->m_sc);
		pEx->Delete();
		return 0;
	}
	catch (CArchiveException* pEx)
	{
		//TRACE("Archive exception\r\n");
		pEx->Delete();
		return 0;
	}
}

//it's execute advanced file open/save dialog
std_dll BOOL ExecuteFileDialog(BOOL bOpenFileDialog, OPENFILENAME& ofn, const char *pszSection )
{
	IUnknown *punk = ::_GetOtherComponent( GetAppUnknown(), IID_IOpenFileDialog );
	if( !punk )
	{
		AfxMessageBox( "FileOpen.dll not registred" );
		return FALSE;
	}

	sptrIOpenFileDialog sptr(punk);
	punk->Release();
	if(sptr == 0)
		return FALSE;


	if( pszSection  )
	{
		_bstr_t		bstr = pszSection;
		sptr->SetDefaultSection( bstr );
	}
	else
		sptr->SetDefaultSection( 0 );
	

	CFileDialog dlgFile(bOpenFileDialog, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, 0/*_FILE_OPEN_SIZE_*/ );

	ofn.Flags |= OFN_EXPLORER | OFN_ENABLEHOOK;
	ofn.lpfnHook = dlgFile.m_ofn.lpfnHook;
	ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;

	sptr->SetOFNStruct(&ofn);

	//if(bOpenFileDialog)
	//	sptr->SetCurExtension(_bstr_t(strExt));
	
	
	sptrIApplication sptrApp(GetAppUnknown());
	sptrIActionManager sptrActMan;
	if (sptrApp != 0)
	{
		sptrApp->GetActionManager(&punk);
		sptrActMan = punk;
		punk->Release();
	}
	if (sptrActMan != 0)
		sptrActMan->EnableManager(FALSE);
	AfxGetApp()->OnIdle(0);

	HRESULT hr = sptr->Execute(bOpenFileDialog ? offOpen : offSave);

	if (sptrActMan != 0)
		sptrActMan->EnableManager(TRUE);

	//if(bOpenFileDialog)
	//{
	//	BSTR bstr = 0;
	//	sptr->GetCurExtension(&bstr);
	//	strExt = bstr;
	//	::SysFreeString(bstr);
	//}

	sptr = 0;

	return hr == S_OK;
}

//return the active parent object from context
std_dll IUnknown *GetActiveParentObjectFromContext( IUnknown *punkContext, const char *szObjectType )
{
	IUnknown *punk = ::GetActiveObjectFromContext( punkContext, szObjectType );
	if( !punk )return 0;

	while( GetParentKey( punk ) != INVALID_KEY )
	{
		INamedDataObject2Ptr	ptrN( punk );
		
		punk->Release();
		ptrN->GetParent( &punk );
	}

	return punk;
}
//return the active object from data context
std_dll IUnknown *GetActiveObjectFromContext( IUnknown *punkContext, const char *szObjectType )
{
	if( !punkContext )return 0;
	_bstr_t	bstrType = szObjectType;
	sptrIDataContext	sptrContext( punkContext );

	if( sptrContext == 0 )
		return 0;

	IUnknown	*punkResult = 0;
	sptrContext->GetActiveObject( bstrType, &punkResult );

	return punkResult;
}

//return the active object from document
std_dll IUnknown *GetActiveObjectFromDocument( IUnknown *punkDoc, const char *szObjectType )
{
	sptrIDocumentSite sptrD(punkDoc);

	if (sptrD == 0)
		return 0;

	IUnknownPtr sptrView;
	sptrD->GetActiveView(&sptrView);

	IDataContext2Ptr sptrC = sptrView == 0 ? punkDoc : sptrView;
	if (sptrC == 0)
		return 0;

	IUnknown * punkObj = 0;
	if (!szObjectType || !lstrlen(szObjectType))
		sptrC->GetLastActiveObject(0, &punkObj);
	else
	{
		_bstr_t	bstrType = szObjectType;
		sptrC->GetActiveObject(bstrType, &punkObj);
	}

	return punkObj;
}

std_dll bool ChangeBaseKeyForDocument(IUnknown * punkDoc, GuidKey OldKey, GuidKey NewKey)
{
	return false;
/*
	INamedDataPtr sptrD = punkDoc;
	if (sptrD == 0)
		return false;

	IUnknown * punkC = 0;
	sptrD->GetActiveContext(&punkC);
	if (!punkC)
		return false;

	IDataContext2Ptr sptrC = punkC;
	punkC->Release();
	
	if (sptrC == 0 || OldKey == INVALID_KEY || NewKey == INVALID_KEY)
		return false;

	if (OldKey == NewKey)
		return true;

	// get types count
	int nTypeCount = 0;
	if (FAILED(sptrC->GetObjectTypeCount(&nTypeCount)))
		return false;

	bool bRet = true;
	// for all types in context
	for (int i = 0; i < nTypeCount; i++)
	{
		BSTR bstrtype = 0;
		if (FAILED(sptrC->GetObjectTypeName(i, &bstrtype)) || !bstrtype)
		{
			bRet = false;
			continue;
		}
		_bstr_t bstrType(bstrtype, false);

		// get object's count for given type
		LPOS lPos = 0;
		if (FAILED(sptrC->GetFirstObjectPos(bstrType, &lPos)) || !lPos)
		{
			bRet = false;
			continue;
		}

		// for all objects w/ given type
		while (lPos)
		{
			IUnknown * punkObject = 0;
			// get next object
			if (FAILED(sptrC->GetNextObject(bstrType, &lPos, &punkObject)) || !punkObject)
			{
				bRet = false;
				continue;
			}

			INamedDataObject2Ptr sptrObj = punkObject;
			punkObject->Release();

			if (sptrObj == 0)
			{
				bRet = false; // must never occure
				continue;
			}
			
			GuidKey OldBaseKey;
			GuidKey OldSrcKey;
			// get old base and source keys
			if (FAILED(sptrObj->GetBaseSourceKey(&OldBaseKey, &OldSrcKey)))
			{
				bRet = false; // must never occure
				continue;
			}
			// get object_key
			GuidKey ObjKey = ::GetObjectKey(sptrObj);

			// if object is old base object -> continue
			if (OldKey == ObjKey)
				continue;

			// for new base object we need to set source key
			if (NewKey == ObjKey)
			{
				sptrObj->SetBaseSourceKey(0, &OldKey);
				continue;
			}

			// set new base_obj_key if needed
			if (OldKey == OldBaseKey)
			{
				sptrObj->SetBaseSourceKey(&NewKey, 0);
				::FireEvent(punkDoc, szEventActiveContextChange, sptrObj,
							sptrC, &_variant_t((LONG)CHANGE_BASE_OBJECT));
			}


			// set new sorc_obj_key if needed
			if (OldKey == OldSrcKey)
				sptrObj->SetBaseSourceKey(0, &NewKey);
		}
	}
	return bRet;
*/
}

//store the specified data object to the archive
std_dll void StoreDataObjectToArchive( CStreamEx &ar, IUnknown *punkObject, SerializeParams *pparams )
{
	sptrINamedDataObject2	sptrN( punkObject );

	DWORD	dwFlags = 0;
	sptrN->GetObjectFlags( &dwFlags );

	//bool	bStoreByParent = dwFlags & nofStoreByParent


	//store the common object info
	long	lVersion = 2;
	CString	strName = ::GetObjectName( punkObject );
	ar<<lVersion;
	ar<<strName;

	GuidKey guidObject = ::GetObjectKey( punkObject );
	ar<<guidObject;
	
	::WriteObject( (IStream*)ar, punkObject, 0, false, pparams );	
	

	return;
}

//load the data object from the archive
std_dll IUnknown *LoadDataObjectFromArchive( CStreamEx &ar, SerializeParams *pparams )
{
	//store the common object info
	long	lVersion;
	CString	strType;
	CString	strName;

	ar>>lVersion;

	if( lVersion >= 2 )
	{
		ar>>strName;
		GuidKey guidObject;
		ar>>guidObject;

		IUnknown* punk = 0;
		if( !::SafeReadObject( (IStream*)ar, &punk, (LPCSTR)strName, 0, pparams ) )
		{
			return 0;
		}

		return punk;
	}
	else
	{
		ar>>strType;
		ar>>strName;

		if( lVersion != 1 )
			return 0;

		//can we store serializible object or text object

		long	lFormat = 0;
		ar>>lFormat;

		if( lFormat != 1 && lFormat != 2 )
			return 0;



		IUnknown *punkObject = ::CreateTypedObject( strType );
		if( !punkObject )
			return 0;

		if( lFormat == 1 )	//binary
		{
			//CArchiveStream	ars( &ar );

			sptrISerializableObject	sptrS( punkObject );
			sptrS->Load( (IStream*)ar, pparams );
		}
		else				//text
		{
			sptrITextObject	sptrT( punkObject );

			CString		strText;
			ar>>strText;
			BSTR	bstrText = strText.AllocSysString();
			sptrT->SetText( bstrText );
			::SysFreeString( bstrText );
		}

		return punkObject;
	}

	return 0;
}

//return true oif object stored by parent
std_dll bool IsObjectStoredByParent( IUnknown *punkChild )
{
	sptrINamedDataObject2	sptrN( punkChild );
	if( sptrN == 0 )	return true;
	DWORD	dwFlags = 0;
	sptrN->GetObjectFlags( &dwFlags  );

	return (dwFlags & nofStoreByParent) == nofStoreByParent;
}

//return the propertysheet dockbar unknown 
std_dll IUnknown *GetPropertySheet()
{
	//1. get the main window
	IUnknown	*punkMainWnd = ::_GetOtherComponent( ::GetAppUnknown(), IID_IMainWindow );
	sptrIMainWindow	sptrM( punkMainWnd );
	if( !punkMainWnd )
		return 0;
	punkMainWnd->Release();


	//find the properties window between dockbars
	POSITION lPos =0;
	sptrM->GetFirstDockWndPosition( &lPos );
	IUnknown	*punkDockBar;

	while( lPos )
	{
		sptrM->GetNextDockWnd( &punkDockBar, &lPos );

		if( CheckInterface( punkDockBar, IID_IPropertySheet ) )
			break;

		if( punkDockBar )
			punkDockBar->Release();
		punkDockBar = 0;
	}

	//if no properties window avaible
	return punkDockBar;
}

//return the property page by title
std_dll IUnknown *GetPropertyPageByTitle( const char *szPageTitle, long *plPageNo )
{
	IUnknown	*punkDockBar = ::GetPropertySheet();
	if( !punkDockBar )
		return 0;

	ICompManagerPtr	sptrCM( punkDockBar );
	if( punkDockBar )
		punkDockBar->Release();

	//find page from component manager

	int	nPagesCount = 0;
	sptrCM->GetCount( &nPagesCount );

	for( long nPage = 0; nPage < nPagesCount; nPage++ )
	{
		IUnknown	*punkPage = 0;
		sptrCM->GetComponentUnknownByIdx( nPage, &punkPage );

		if( !punkPage )
			continue;

		INamedObject2Ptr	ptrO( punkPage );
		if( ptrO == 0 )
		{
			//ASSERT( false );
			continue;
		}

		BSTR	bstrPageName = 0;
		ptrO->GetName(  &bstrPageName );
		CString	strName = bstrPageName;

		::SysFreeString( bstrPageName );

		if( strName == szPageTitle )
		{
			if( plPageNo )
				*plPageNo = nPage;

			return punkPage;
		}

		punkPage->Release();
	}

	return 0;
}

//return the ColorChooser dockbar unknown 
std_dll IUnknown *GetColorChooser()
{
	//1. get the main window
	IUnknown	*punkMainWnd = ::_GetOtherComponent( ::GetAppUnknown(), IID_IMainWindow );
	sptrIMainWindow	sptrM( punkMainWnd );
	if( punkMainWnd )
		punkMainWnd->Release();


	//find the properties window between dockbars
	POSITION	lPos =0;
	sptrM->GetFirstDockWndPosition( &lPos );
	IUnknown	*punkDockBar;

	while( lPos )
	{
		sptrM->GetNextDockWnd( &punkDockBar, &lPos );

		if( CheckInterface( punkDockBar, IID_IChooseColor ) )
			break;

		if( punkDockBar )
			punkDockBar->Release();
		punkDockBar = 0;
	}

	//if no properties window avaible
	return punkDockBar;
}


/////////////////////////////////////////////////////////////////////////////////////////
//mouse controller helpers implementation

//return the mouse controller pointer
std_dll IUnknown	*GetMouseController()
{
	IUnknown	*punk = ::GetAppUnknown();
	if( ::CheckInterface( punk, IID_IMouseController ) )
		return punk;
	return 0;
}

//register view as target
std_dll void RegisterMouseCtrlTarget( IUnknown *punkView )
{
	sptrIMouseController	sptrC( ::GetMouseController() );
	if( sptrC != 0 )
		sptrC->RegisterMouseCtrlTarget( punkView );
}

//unregister view as target
std_dll void UnRegisterMouseCtrlTarget( IUnknown *punkView )
{
	sptrIMouseController	sptrC( ::GetMouseController() );
	if( sptrC != 0 )
		sptrC->UnRegisterMouseCtrlTarget( punkView );
}

//dispatch wm_mousemove to controller
std_dll void DispatchMouseToCtrl( IUnknown *punkView, POINT point )
{
	sptrIMouseController	sptrC( ::GetMouseController() );
	if( sptrC != 0 )
		sptrC->DispatchMouseToCtrl( punkView, point );
}

//draw the controller from wm_paint
std_dll void RepaintMouseCtrl( IUnknown *punkView, HDC hDC, bool bDraw )
{
	sptrIMouseController	sptrC( ::GetMouseController() );
	if( sptrC != 0 )
		sptrC->RepaintMouseCtrl( punkView, hDC, bDraw );
}

//check on wm_timer
std_dll void CheckMouseInWindow( IUnknown *punkView )
{
	sptrIMouseController	sptrC( ::GetMouseController() );
	if( sptrC != 0 )
		sptrC->CheckMouseInWindow( punkView );
}

//install the draw handler
std_dll void InstallController( IUnknown *punkCtrl )
{
	sptrIMouseController	sptrC( ::GetMouseController() );
	if( sptrC != 0 )
		sptrC->InstallController( punkCtrl );
}

//remove the draw handler
std_dll void UnInstallController( IUnknown *punkCtrl )
{
	sptrIMouseController	sptrC( ::GetMouseController() );
	if( sptrC != 0 )
		sptrC->UnInstallController( punkCtrl );
}

std_dll void _MyCheckMemory( bool bAlloc )
{
	static byte	*ptrDummy[1000];
	static bool	bAllocated = false;

	if( bAllocated )
	{
		for( int i = 0; i < 1000; i++ )
		{
			for( int n = 0; n < 222; n++ )
			{
				ASSERT( ptrDummy[i][n] == 222 );
			}
			delete ptrDummy[i];
		}
		bAllocated = false;
	}

	if( bAlloc )
	{
		for( int i = 0; i < 1000; i++ )
		{
			ptrDummy[i] = new byte[222];
			memset( ptrDummy[i], 222, 222 );
		}
		bAllocated = true;
	}
}

//return the object parent key
std_dll GuidKey GetParentKey( IUnknown *punkO )
{
	if( !punkO )
		return INVALID_KEY;

	sptrINamedDataObject2	sptrN( punkO );
	IUnknown	*punkParent = 0;

	sptrN->GetParent( &punkParent );

	GuidKey lKey = ::GetObjectKey( punkParent );

	if( punkParent )
		punkParent->Release();

	return lKey;
}

//return the object parent
IUnknown *GetParentUnk( IUnknown *punkO ) // without addref
{
	if( !punkO )
		return 0;

	sptrINamedDataObject2	sptrN( punkO );
	IUnknown	*punkParent = 0;

	sptrN->GetParent( &punkParent );

	if( punkParent )
		punkParent->Release();

	return punkParent;
}

//return the object flags
std_dll DWORD GetObjectFlags( IUnknown *punkO )
{
	DWORD	dwFlags = 0;
	if( !punkO )
		return dwFlags;

	sptrINamedDataObject2	sptrN( punkO );
	sptrN->GetObjectFlags( &dwFlags );

	return dwFlags;
}

//send the update hint for the object
std_dll void UpdateDataObject( IUnknown *punkD, IUnknown *punkO )
{
	::FireEvent( punkD, szEventChangeObject, punkO );
//	INamedDataPtr sptrData = punkD;
//	if (sptrData != 0 && punkO)
//		sptrData->NotifyContexts(ncReplaceObject, punkO, punkO, 0);
}

//restore ActiveX control state from persist stream, specified by DataObject
std_dll void RestoreContainerFromDataObject( IUnknown *punkCtrlObject, IUnknown *punkCtrl )
{
	if( !punkCtrl )
		return;

	IPersistStreamInitPtr	sptr( punkCtrl );
	if (sptr == 0)
		return;

	DWORD	dwDataSize = 0;

	sptrIActiveXCtrl	sptrC( punkCtrlObject );
	sptrC->GetObjectData( 0, &dwDataSize );

	if( !dwDataSize )
	{
		::StoreContainerToDataObject( punkCtrlObject, punkCtrl );
		return;
	}

	byte	*pbyte = new byte[dwDataSize];
	sptrC->GetObjectData( &pbyte, &dwDataSize );

	{
		CMemFile	file( pbyte, dwDataSize );
		CArchive	ar( &file, CArchive::load );
		CArchiveStream	ars( &ar );

		sptr->Load( &ars );
	}

	delete pbyte;
}

//store ActiveX control state to persist stream, specified by DataObject
std_dll void StoreContainerToDataObject( IUnknown *punkCtrlObject, IUnknown *punkCtrl )
{
	IPersistStreamInitPtr	sptr( punkCtrl );
	if (sptr == 0)
		return;

	CMemFile	file;

	{
		CArchive	ar( &file, CArchive::store );
		CArchiveStream	ars( &ar );

		sptr->Save( &ars, true );
	}

	DWORD	dwDataSize = (DWORD)file.GetLength();
	byte	*pbyte = file.Detach();


	sptrIActiveXCtrl	sptrC( punkCtrlObject );
	sptrC->SetObjectData( pbyte, dwDataSize );

	free( pbyte );
}


std_dll IUnknown* GetObjectByKeyEx(IUnknown* punkData, GuidKey lKey)
{	
	PROFILE_TEST( "common::utils::GetObjectByKeyEx" )

	sptrIDataTypeManager	sptrM(punkData);

	if (sptrM == 0)
		return 0;

	long	nTypesCounter = 0;
	sptrM->GetTypesCount(&nTypesCounter);

	// for all types
	for (long nType = 0; nType < nTypesCounter; nType++)
	{
		LONG_PTR lpos = 0;

		sptrM->GetObjectFirstPosition(nType, &lpos);

		while (lpos)
		{
			IUnknown * punkObj = 0;
			// get next object
			sptrM->GetNextObject(nType, &lpos, &punkObj);
			if (!punkObj)
				continue;

			// check this object and its children(if any) for its key
			IUnknown *punk = ::GetChildObjectByKey(punkObj, lKey);

			// if finded - return it
			if (punk)
			{
				if (::GetObjectKey(punk) != ::GetObjectKey(punkObj))
					punkObj->Release();

				return punk;
			}

			//[ay]
			//притворимся, что у object есть своя named data
			punk = GetObjectByKeyEx( punkObj, lKey );
			if (punk)
			{
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

std_dll IUnknown* GetChildObjectByKey(IUnknown* punkParent, GuidKey lKey)
{
	if (lKey == INVALID_KEY || !punkParent)
		return 0;

	// if this object has same key as we need 
	if (::GetObjectKey(punkParent) == lKey)
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
	POSITION lPos = 0;
	if (FAILED(sptr->GetFirstChildPosition(&lPos)))
		return 0; 

	while (lPos)
	{
		IUnknown * punkObj = 0;
		sptr->GetNextChild(&lPos, &punkObj);
		if (!punkObj)
			continue;
	
		// check this object and its children(if any) has needed key
		IUnknown *punk = ::GetChildObjectByKey(punkObj, lKey);

		// if finded - return it
		if (punk)
		{
			if (::GetObjectKey(punk) != ::GetObjectKey(punkObj))
				punkObj->Release();

			return punk;
		}
		// to next object 
		punkObj->Release();
	}
	// nothing finded
	return 0;
}

std_dll CString GetDocTypesString(IUnknown *punkDoc)
{
	CString strRes;
	_try(Common, GetDocTypesString)
	{
		if (!CheckInterface(punkDoc, IID_IDataTypeManager))
			return "";

		IDataTypeManagerPtr sptr = punkDoc;

		long nCount = 0;
		sptr->GetTypesCount(&nCount);
		for (long i = 0; i < nCount; i++)
		{
			BSTR bstr = 0;
			if (SUCCEEDED(sptr->GetType(i, &bstr)))
			{
				CString str = bstr;
				::SysFreeString(bstr);
				strRes += str + _T('\n');
			}
		}
	}
	_catch
	{
		return strRes;
	}
	return strRes;
}

std_dll CString GetDocTypesStringFromView(IUnknown *punkView)
{
	sptrIView sptrV(punkView);
	if (sptrV == 0)
		return "";

	CString strRes;
	_try(Common, GetDocTypesStringFromView)
	{
		IDataTypeInfoManagerPtr	sptrM(GetAppUnknown(false));
		if (sptrM == 0)
			return "";
		
		long nCount = 0;
		sptrM->GetTypesCount(&nCount);
		for (int i = 0; i < nCount; i++)
		{
			INamedDataInfo *pTypeInfo = 0;
			
			if (FAILED(sptrM->GetTypeInfo(i, &pTypeInfo)))
				continue;

			CString str = GetObjectName( pTypeInfo );
			pTypeInfo->Release();

			if (str.IsEmpty())
				continue;

			_bstr_t bstrType(str);
			DWORD dwMatch = 0;
			if (FAILED(sptrV->GetMatchType(bstrType, &dwMatch)) || dwMatch == mvNone)
				continue;
		
			strRes += str + _T('\n');
		}
	}
	_catch
	{
		return strRes;
	}
	return strRes;
}

//return the active object from data context one of supported types
std_dll IUnknown* GetActiveObjectFromContextEx(IUnknown *punkContext)
{
	sptrIDataContext2	sptrContext(punkContext);
	sptrIView			sptrView(punkContext);

	IUnknown *punkDoc = 0;

	CString strTypes;
	if (sptrView != 0)
	{
		sptrView->GetDocument(&punkDoc);
		if (punkDoc)
		{
			// get doc types
			strTypes = GetDocTypesString(punkDoc);
			punkDoc->Release();
		}
		else
		{

			strTypes = GetDocTypesStringFromView(punkContext);
		}
	}
//	if (strTypes.IsEmpty())	
//		return 0;

	int nLast = strTypes.Find(_T('\n'));
	if (strTypes.GetLength() && strTypes[strTypes.GetLength() - 1] != _T('\n'))
		strTypes += _T('\n');

	CStringArray arr;
	int nFirst = 0;
	while (nLast != -1)
	{
		CString str = strTypes.Mid(nFirst, nLast - nFirst);
		arr.Add(str);
		nFirst = nLast + 1;
		nLast = strTypes.Find(_T('\n'), nFirst);
	}

	strTypes.Empty();
	for (int i = 0; i < arr.GetSize(); i++)
	{
		DWORD dwMatch = 0;
		_bstr_t bstrType = arr[i];
		sptrView->GetMatchType(bstrType, &dwMatch);
		MatchView eMatch = (MatchView)dwMatch;

		if (dwMatch > mvNone)
			strTypes += arr[i] + _T('\n');
	}

//	if (strTypes.IsEmpty())	
//		return 0;

	IUnknown	*punkResult = 0;

	_bstr_t bstrTypes = strTypes;
	sptrContext->GetLastActiveObject(bstrTypes, &punkResult);

	return punkResult;
}

// change object selection and send notification about it
std_dll void NotifyObjectSelect(IUnknown *punk, bool bSelect)
{
	if (!CheckInterface(punk, IID_INamedDataObject2))
		return;
	
	INamedDataObject2Ptr sptrN = punk;

	IUnknown *punkParent = 0;
	if (FAILED(sptrN->GetParent(&punkParent)) || !punkParent)
		return;

	INamedDataObject2Ptr sptrParent = punkParent;
	if (punkParent)
		punkParent->Release();

	if (!CheckInterface(sptrParent, IID_IDataObjectList))
		return;

	TPOS lPos = 0;
	if (bSelect)
	{
		if (FAILED(sptrN->GetObjectPosInParent(&lPos)) || !lPos)
			return;
	}
	sptrParent->SetActiveChild(lPos);

}

// change object selection and send notification about it
std_dll void NotifyObjectSelect(IUnknown *punk, IUnknown * punkEvListener, bool bSelect)
{
	if (!CheckInterface(punk, IID_INamedDataObject2))
		return;
	
	INamedDataObject2Ptr sptrN = punk;
	// get parent object
	IUnknown *punkParent = 0;
	if (FAILED(sptrN->GetParent(&punkParent)) || !punkParent)
		return;

	INamedDataObject2Ptr	sptrParent = punkParent;
	IDataObjectListPtr		sptrList = punkParent;
	
	if (punkParent)
		punkParent->Release();

	// forbid notification
	if (sptrList != 0)sptrList->LockObjectUpdate(true);
	TPOS lPos = 0;
	if (bSelect)
		sptrN->GetObjectPosInParent(&lPos);

	sptrParent->SetActiveChild(lPos);
	// permit notification
	if (sptrList != 0)sptrList->LockObjectUpdate(false);

// send notification
	// get NamedData	
	IUnknown *punkNamedData = 0;
	sptrParent->GetData(&punkNamedData);

	if (punkNamedData)
	{
		ChildNotifyCodes	code = cncActivate;
		::FireEvent2(punkNamedData, szEventChangeObjectList, sptrParent, punk, &code, sizeof( code ), punkEvListener);
		punkNamedData->Release();
	}

	//update
	HWND	hwnd = 0;
	IApplicationPtr	ptrA( GetAppUnknown() );
	ptrA->GetMainWindowHandle( &hwnd );

	::PostMessage( hwnd, (WM_USER+791), 0, 0 );
}

//////////////////////////////////////////////////////////classes
/*std_dll COLORREF	GetClassColor( long lClass )
{
	return ::GetValueColor( GetAppUnknown(), szPluginClasses, szUnkClassColor, RGB(255, 255, 0));
}

std_dll long GetClassName( long lClass, char *psz, size_t cb )
{
	::GetValueString( GetAppUnknown(), szPluginClasses, szUnkClassName, "Unknown" );
	return 1;
}

std_dll long GetObjectClass( ICalcObject *pcalc )
{
	return -1;
}

std_dll void SetObjectClass( ICalcObject *pcalc, long lClass )
{
}


std_dll COLORREF GetObjectClassColor(ICalcObject *pcalc)
{
	long	lClass = ::GetObjectClass( pcalc );
	return GetClassColor( lClass );
}

std_dll CString GetObjectClassName(ICalcObject *pcalc)
{
	CString	str;
	long	lClass = ::GetObjectClass( pcalc );
	
	GetClassName( lClass, str.GetBuffer( 200 ), 200 );
	return str;
}*/

// return param name by its type (for manual measure params)
std_dll CString GetParamNameByParamType(DWORD dwParamType)
{
	CString strResult = _T("");

	switch (dwParamType)
	{
		case etLinear:
			strResult = _T("Linear");
			break;

		case etAngle:
			strResult = _T("Angle");
			break;

		case etCounter:
			strResult = _T("Counter");
			break;

		default:
			strResult = _T("Undefined");
			break;
	}
	return strResult;
}


/*
// Load calibrX & calibrY
std_dll void GetCalibrValues(double * pfCalibrX, double * pfCalibrY, long *plType)
{
	if (pfCalibrX)
		*pfCalibrX = ::GetValueDouble(GetAppUnknown(false), szCalibration, szCalibrX, 1.);

	if (pfCalibrY)
		*pfCalibrY = ::GetValueDouble(GetAppUnknown(false), szCalibration, szCalibrY, 1.);

	if (plType)
		*plType = ::GetValueInt(GetAppUnknown(false), szCalibration, szMeasUnit, 0);
}

// Save calibrX & calibrY
std_dll void SetCalibrValues(double * pfCalibrX, double * pfCalibrY, long *plType)
{
	if (pfCalibrX)
		::SetValue(GetAppUnknown(false), szCalibration, szCalibrX, *pfCalibrX);

	if (pfCalibrY)
		::SetValue(GetAppUnknown(false), szCalibration, szCalibrY, *pfCalibrY);

	if (plType)
		::SetValue(GetAppUnknown(false), szCalibration, szMeasUnit, (long)*plType);
}

// Load OffsetX & OffsetY
std_dll void GetOffsetValues(double * pfOffsetX, double * pfOffsetY)
{
	if (pfOffsetX)
		*pfOffsetX = ::GetValueDouble(GetAppUnknown(false), szCalibration, szOffsetX, 0.);

	if (pfOffsetY)
		*pfOffsetY = ::GetValueDouble(GetAppUnknown(false), szCalibration, szOffsetY, 0.);
}

// Save OffsetX & OffsetY
std_dll void SetOffsetValues(double * pfOffsetX, double * pfOffsetY)
{
	if (pfOffsetX)
		::SetValue(GetAppUnknown(false), szCalibration, szOffsetX, *pfOffsetX);

	if (pfOffsetY)
		::SetValue(GetAppUnknown(false), szCalibration, szOffsetY, *pfOffsetY);
}*/

/*
//Сдвиг активного параметра:
std_dll bool ShiftActiveParam(IUnknown * punkList)
{
	if (!CheckInterface(punkList, IID_IMeasParamGroup2))
		return false;

	IMeasParamGroup2Ptr sptrGroup = punkList;
	CObjectListWrp		wrpObjList(punkList);

	if (sptrGroup == 0 || wrpObjList == 0)
		return false;

//прочитать флаг сдвига NextParam 1 or 0
//{ т.е. сдвигать на след параметр или на следующий объект }
	EShiftMode eShiftMode = (EShiftMode)::GetValueInt(::GetAppUnknown(false), 
													  szManualMeasure, szShiftMode, 
													  (long)esmNextParam);
	bool bNeedShiftObject = false;
	// mode 0 == shift to next param
	// if next param == current active => shift activity to next object 
	if (eShiftMode == esmNextParam)
	{
		long lActivePos = 0;
		sptrGroup->GetActiveParamPos(&lActivePos);

		// find next manual param
		long lNewPos = FindNextManualParam(wrpObjList);
		if (lNewPos == lActivePos)
		{
			sptrGroup->SetActiveParamPos(lNewPos);
			bNeedShiftObject = true;
		}
		else
			sptrGroup->SetActiveParamPos(lNewPos);
	}
	// mode 1 == shift activity to next object
	// if next object is not exists we need to set activity to nothing
	if (eShiftMode == esmNextObject || bNeedShiftObject)
	{
		POSITION ActivePos = wrpObjList.GetCurPosition();
		POSITION Pos = ActivePos;

		IUnknown *punkNext = wrpObjList.GetNextObject(Pos);
		
		if (punkNext)
		{
			wrpObjList.SetCurPosition(Pos);
			punkNext->Release();
		}
		else
		{
			Pos = wrpObjList.GetFirstObjectPosition();		//get the first object position
			POSITION PrevPos = Pos;
			punkNext =  wrpObjList.GetNextObject(Pos);	//get the object at the specified position and move the POS

			if (!punkNext)
				wrpObjList.SetCurPosition(0);

			else if (ActivePos == PrevPos)
			{
				punkNext->Release();
				wrpObjList.SetCurPosition(0);
			}
			else if (PrevPos)
			{
				punkNext->Release();
				wrpObjList.SetCurPosition(PrevPos);
			}
		}
	}

	return true;
}

// поиск следующего параметра
std_dll long FindNextManualParam(IUnknown * punkList)
{
	_try(common, FindNextManualParam)
	{
		CPtrArray arr;
		IMeasParamGroup2Ptr sptr = punkList;
		if (sptr == 0)
			return 0;

		long lActivePos = 0;
		sptr->GetActiveParamPos(&lActivePos);
		LPOS lPos = lActivePos;
	// try to find next manual param
		while (lPos)
		{
			sptr->GetNextPos(&lPos);
			if (!lPos)
				break;

			BOOL bManual = false;
			sptr->IsManualParam(lPos, &bManual);

			if (bManual)
				return lPos;

		}

		// from begin
		sptr->GetFirstPos(&lPos);
		while (lPos)
		{
			if (!lPos || lPos == lActivePos)
				return lActivePos;

			BOOL bManual = false;
			sptr->IsManualParam(lPos, &bManual);

			if (bManual)
				return lPos;

			sptr->GetNextPos(&lPos);
		}
	}
	_catch
	{
		return 0;
	}
	return 0;
}
*/

/*#include "ClassBase.h"

// return param name by its type (for manual measure params)
std_dll CString GetParamNameByParamType(DWORD dwParamType)
{
	CString strResult = _T("");

	switch (dwParamType)
	{
	case CCalcParamInfo::etRegular:
		strResult = _T("Regular");
		break;

	case CCalcParamInfo::etLinear:
		strResult = _T("Linear");
		break;

	case CCalcParamInfo::etAngle:
		strResult = _T("Angle");
		break;

//	case CCalcParamInfo::etRadius:
//		strResult = _T("Radius");
//		break;
//
	case CCalcParamInfo::etCounter:
		strResult = _T("Counter");
		break;

	case CCalcParamInfo::etUndefine:
		strResult = _T("Undefined");
		break;
	}
	return strResult;
}
// get name of calibr units 
std_dll CString GetCalibrUnitName(LPCTSTR szGroupMgr, long lParamKey, long lMeasUnit)
{
	CString strOut = _T("Ed");
	CString strGroupMgr = szGroupMgr;
	if (strGroupMgr.IsEmpty())
		return strOut;

	IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IParamGroupManager, strGroupMgr);
	if (!punk)
		return _T("Ed");

	IParamGroupManagerPtr sptrM = punk;
	punk->Release();

	if (sptrM == 0)
		return strOut;

	_try(common, GetCalibrUnitName)
	{
		// check manager contains groups
		int nCount = 0;
		sptrM->GetGroupCount(&nCount);
		if (!nCount)
			return strOut;

		LPOS lPos = 0;
		sptrM->GetFirstPos(&lPos);

		int nNextPos = 0;
		
		// for all groups
		while (lPos)
		{
			// get next group
			IUnknown *punkGroup = 0;
			if (FAILED(sptrM->GetNextGroup(&lPos, &punkGroup)) || !punkGroup)
				continue;

			IMeasParamGroupPtr sptrGroup = punkGroup;
			punkGroup->Release();

			if (sptrGroup == 0)
				continue;

			// try to get unit name for paramKey from group
			BSTR bstrUnitName = 0;
			if (FAILED(sptrGroup->GetParamUnitName(lParamKey, lMeasUnit, &bstrUnitName)) || !bstrUnitName)
				continue;

			//if found ->return 
			strOut = bstrUnitName;
			::SysFreeString(bstrUnitName);

			return strOut;
		} // for all groups
	}
	_catch;
	return _T("Ed");
}*/

// notify_param_data_struct create new and init
std_dll TParamNotifyData * CreatePNData(UINT uCode, long lKey, IUnknown * punk)
{
	TParamNotifyData* pData = (TParamNotifyData*)::CoTaskMemAlloc(sizeof(TParamNotifyData));
	if (pData)
	{
		pData->uCode = uCode;
		pData->lParamKey = lKey;
		pData->punkMgr = punk;
		if (pData->punkMgr)
			pData->punkMgr->AddRef();
	}
	return pData;
}
// delete notify_param_data_struct
std_dll void DeletePNData(TParamNotifyData * pData)
{
	if (pData)
	{
		if (pData->punkMgr)
			pData->punkMgr->Release();

		::CoTaskMemFree((LPVOID)pData);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// helper function that is provide help to restore relation between prev object key and new object key
// after named data loaded from file
#include <AfxTempl.h>
/*CMap <long, long, long, long> __mapKeyCache__;
static bool __bmapKeyCacheInited__ = false;

std_dll void InitKeyCache()
{
	if( __bmapKeyCacheInited__ )
		ReleaseKeyCache();
	__mapKeyCache__.InitHashTable(200);
	__bmapKeyCacheInited__ = true;
}

std_dll void ReleaseKeyCache()
{
	__mapKeyCache__.RemoveAll();
	__bmapKeyCacheInited__ = false;
}

std_dll bool SetKeyToCache(long lOldKey, long lNewKey)
{
	if (!__bmapKeyCacheInited__)
		return false;

	long lKey = -1;
	if (__mapKeyCache__.Lookup(lOldKey, lKey) && lKey != lNewKey)
	{
		ASSERT(false);
		return false;
	}
	__mapKeyCache__.SetAt(lOldKey, lNewKey);
	return true;
}

std_dll long GetKeyFromCache(long lOldKey)
{
	ASSERT(__bmapKeyCacheInited__);
	if (!__bmapKeyCacheInited__)
		return -1;

	long lKey = -1;
	if (lOldKey == -1)
		return -1;

	if (!__mapKeyCache__.Lookup(lOldKey, lKey))
	{
//		ASSERT(false);
		lKey = -1;
	}
	return lKey;
}*/

//////////////////////////////////////////////////////////////////////////////////////////////////////


void _PreformCopy( INamedData *pdest, INamedData *psrc, const char *szSection )
{
	_bstr_t	bstrSection = szSection;
	psrc->SetupSection( bstrSection );
	pdest->SetupSection( bstrSection );

	long	nCount, nEntry;
	psrc->GetEntriesCount( &nCount );

	for( nEntry = 0; nEntry < nCount; nEntry++ )
	{
		BSTR	bstrName = 0;
		psrc->GetEntryName( nEntry, &bstrName );
		_variant_t	var;
		psrc->GetValue( bstrName, &var );

		if( var.vt == VT_BITS )
		{
			byte	*pdata = (byte*)var.lVal;
			long	lSize = *(long*)pdata;
			byte	*pdata1 = new byte[sizeof( lSize )+lSize];
			//*(long*)pdata1 = lSize;
			memcpy( pdata1, pdata, lSize+sizeof( lSize ) );

			var = _variant_t((long)pdata1);
			var.vt = VT_BITS;
		}
		pdest->SetValue( bstrName, var );

		CString	str( szSection );

		if( str != "\\" )
			str += "\\";
		str += bstrName;

		_PreformCopy( pdest, psrc, str );
		
		psrc->SetupSection( bstrSection );
		pdest->SetupSection( bstrSection );
	}
}
//copy named data
std_dll void CopyNamedData( IUnknown *punkDest, IUnknown *punkSource )
{
	INamedDataPtr	ptrDataDest( punkDest );
	INamedDataPtr	ptrDataSource( punkSource );

	::DeleteEntry( ptrDataDest, 0 );

	_PreformCopy( ptrDataDest, ptrDataSource, "\\" ); 
}


//init the NamedData attached to DataObject 
std_dll void InitAttachedData( IUnknown *punkObject )
{
	INamedDataObject2Ptr	ptr( punkObject );
	ptr->InitAttachedData();

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

//generate the unique name for data argument
std_dll CString	GenerateNameForArgument( CString strArgName, IUnknown *punkDocData )
{
	IDataTypeManagerPtr	ptrManager( punkDocData );

	long	lTypesCount = 0, lType;
	ptrManager->GetTypesCount( &lTypesCount );

	int	nMaxNum = get_tail_num(strArgName);

	for( lType = 0; lType < lTypesCount; lType++ )
	{
		LONG_PTR	lpos = 0;
		ptrManager->GetObjectFirstPosition( lType, &lpos );

		while( lpos )
		{
			IUnknown	*punkObject = 0;
			ptrManager->GetNextObject( lType, &lpos, &punkObject );

			CString	strName = ::GetObjectName( punkObject );
			punkObject->Release();

			if( str_eq_wo_tail_num( strName, strArgName ) )
				nMaxNum = max(nMaxNum, get_tail_num(strName)+1);
		}
	}

	CString str1=strArgName;
	set_tail_num( str1.GetBuffer(strArgName.GetLength()+20), nMaxNum );
	str1.ReleaseBuffer();

	return str1;
}

//return the document from view
std_dll IUnknown *GetDocumentFromView( IUnknown *punkView )
{
	if( !punkView )return 0;
	IViewPtr	ptrView( punkView );
	IUnknown	*punkDoc = 0;
	ptrView->GetDocument( &punkDoc );

	return punkDoc;
}

std_dll void _VarChangeType( VARIANT &var, long lType, VARIANT *psrc )
{
	if( !psrc )
		psrc = &var;

	long lSrcType = psrc->vt;

	if( lSrcType == VT_R4 && lType == VT_BSTR )
	{
		char	sz[100];
		sprintf( sz, "%g", (double)psrc->fltVal );
		ATL::CComVariant comVar=sz;
		comVar.Detach(&var);
		return;
	}
	if( lSrcType == VT_R8 && lType == VT_BSTR )
	{
		char	sz[100];
		sprintf( sz, "%g", psrc->dblVal );
		ATL::CComVariant comVar = sz;
		comVar.Detach(&var);
		return;
	}

	if( lSrcType == VT_BSTR && lType == VT_R4 )
	{
		CString	str = psrc->bstrVal;

		char* p = (char*)strchr( str, ',' );
		if( p )*p = '.';
		float fltVal;
		sscanf( str, "%f", &fltVal );
		ATL::CComVariant comVar = fltVal;
		comVar.Detach(&var);
		return;
	}
	if( lSrcType == VT_BSTR && lType == VT_R8 )
	{
		CString	str = psrc->bstrVal;

		char* p = (char*)strchr( str, ',' );
		if( p )*p = '.';
		double dblVal;
		sscanf( str, "%lf", &dblVal );
		ATL::CComVariant comVar = dblVal;
		comVar.Detach(&var);
		return;
	}

	HRESULT hr=VariantChangeType( &var, psrc, 0, (VARTYPE)lType );
//	if(FAILED(hr))
//		throw _com_error(hr);
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//	IMPORTANT
//
//	See detail info in \vt5\ifaces\thumbnail.h
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

/////////////////////////////////////////////////////////////////////////////
//Serialize thumbnail. 
/////////////////////////////////////////////////////////////////////////////
long const nThumbnailVersion = 1;
std_dll bool ThumbnailStore( CArchive& ar, BYTE* pbi )
{
	try
	{
		DWORD dwSize = ::ThumbnailGetSize( pbi );

		ar << nThumbnailVersion;
		ar << dwSize;
		
		if( dwSize < sizeof(BITMAPINFOHEADER) + ((BYTE)sizeof(TumbnailInfo)) )
			return false;

		ar.Write( pbi, dwSize );

	}
	catch( ... )
	{
		return false;
	}
	return true;
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//	IMPORTANT
//
//	See detail info in \vt5\ifaces\thumbnail.h
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
std_dll bool ThumbnailLoad( CArchive& ar, BYTE** ppbi )
{
	try
	{
		DWORD dwSize = -1;
		
		long nVersion;

		ar >> nVersion;
		ar >> dwSize;
		if( dwSize < 1 )
			return false;

		*ppbi = new BYTE[dwSize];
		ar.Read( *ppbi, dwSize );
	}
	catch( ... )
	{
		return false;
	}
	return true;
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//	IMPORTANT
//
//	See detail info in \vt5\ifaces\thumbnail.h
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/////////////////////////////////////////////////////////////////////////////
std_dll CSize ThumbnailGetRatioSize( CSize sizeTarget, CSize sizeThumbnail )
{	
	sizeTarget.cx = max(sizeTarget.cx, 1);
	sizeTarget.cy = max(sizeTarget.cy, 1);
	sizeThumbnail.cx = max(sizeThumbnail.cx, 1);
	sizeThumbnail.cy = max(sizeThumbnail.cy, 1);
	double fTargetWidth		= (double)sizeTarget.cx;
	double fTargetHeight	= (double)sizeTarget.cy;
	double fTargetZoom		= fTargetWidth / fTargetHeight;

	double fThumbnailWidth	= (double)sizeThumbnail.cx;
	double fThumbnailHeight	= (double)sizeThumbnail.cy;	
	double fThumbnailZoom	= fThumbnailWidth / fThumbnailHeight;



	int nWidth, nHeight;
	
	nWidth = (int)fTargetWidth;
	nHeight = (int)fTargetHeight;
	
	if( (double)nWidth / fThumbnailZoom > fTargetHeight )
	{		
		nWidth = (int)( fTargetHeight * fThumbnailZoom );
		nHeight = (int)( nWidth / fThumbnailZoom );//fTargetHeight;
	}

	if( (double)nHeight * fThumbnailZoom > fTargetWidth )
	{
		nHeight = (int)( fTargetWidth / fThumbnailZoom );
		nWidth = (int)( nHeight * fThumbnailZoom );//fTargetWidth;
	}

	return CSize( nWidth, nHeight );
}	

/*
std_dll CSize ThumbnailGetRatioSize( CSize sizeThumbnail, CSize sizeImage )
{
	CSize sizeReturn;
	double fThumbnailWidth	= (double)sizeThumbnail.cx;
	double fThumbnailHeight	= (double)sizeThumbnail.cy;
	
	double fThumbnailZoom	= fThumbnailWidth / fThumbnailHeight;

	double fImageWidth		= (double)sizeImage.cx;
	double fImageHeight		= (double)sizeImage.cy;

	double fImageZoom		= fImageWidth / fImageHeight;		


	if( fImageZoom > 1.0 ) //image width > image height
	{
		sizeReturn = CSize ( fThumbnailWidth, fThumbnailWidth / fImageZoom );
	}
	else//image width < image height
	{
		sizeReturn = CSize ( fThumbnailHeight * fImageZoom, fThumbnailHeight );
	}

	return sizeReturn;
}	
*/
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//	IMPORTANT
//
//	See detail info in \vt5\ifaces\thumbnail.h
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/////////////////////////////////////////////////////////////////////////////

class DrawDib
{
public:
	DrawDib()
	{		m_hDrawDib = ::DrawDibOpen();	}
	~DrawDib()
	{		::DrawDibClose( m_hDrawDib );	}
	operator HDRAWDIB()
	{	return m_hDrawDib;}
protected:
	HDRAWDIB	m_hDrawDib;
};
DrawDib	DrawDibInstance;


// Draw thumbnail
std_dll bool ThumbnailDraw( BYTE* pbi, CDC* pDC, CRect rcTarget, IUnknown* punkDataObject )
{

	 if( pbi == NULL )
		return false;

	BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)pbi;

	BYTE* pDibBits = pbi + ((BYTE)sizeof(BITMAPINFOHEADER)) + ((BYTE)sizeof(TumbnailInfo));			


	if( pDC == NULL )
		return false;

	TumbnailInfo* pti = (TumbnailInfo*) ( (BYTE*)pbi + (BYTE)sizeof(BITMAPINFOHEADER) );	


	CRect rcDibOutput;


	CSize sizeOutput = ::ThumbnailGetRatioSize( 							
							CSize( rcTarget.Width(), rcTarget.Height() ),
							CSize( pBitmapInfoHeader->biWidth, pBitmapInfoHeader->biHeight)
							);

	if( pti->dwFlags & tfVarStretch )
	{
		//Do nothing
	}
	else
	if( pti->dwFlags & tfCanStretch2_1 ) // only 2:1, || 1:1
	{
		if( rcTarget.Width() > pBitmapInfoHeader->biWidth * 2 && 
			rcTarget.Height() > pBitmapInfoHeader->biHeight * 2 )
		{			
			sizeOutput.cx = pBitmapInfoHeader->biWidth * 2;
			sizeOutput.cy = pBitmapInfoHeader->biHeight * 2;

		}
		else
		if( rcTarget.Width() > pBitmapInfoHeader->biWidth && 
			rcTarget.Height() > pBitmapInfoHeader->biHeight )
		{
			sizeOutput.cx = pBitmapInfoHeader->biWidth;
			sizeOutput.cy = pBitmapInfoHeader->biHeight;
		}
		
	}


	rcDibOutput.left	= rcTarget.left + (rcTarget.Width() - sizeOutput.cx) / 2;
	rcDibOutput.top		= rcTarget.top  + (rcTarget.Height() - sizeOutput.cy) / 2;

	rcDibOutput.right	= rcDibOutput.left + sizeOutput.cx;
	rcDibOutput.bottom	= rcDibOutput.top  + sizeOutput.cy;

	
	int nDevCaps = ::GetDeviceCaps(pDC->GetSafeHdc(), TECHNOLOGY);
	if (nDevCaps == DT_RASPRINTER || nDevCaps == DT_METAFILE)
	{
		StretchDIBits(pDC->GetSafeHdc(), rcDibOutput.left, rcDibOutput.top, rcDibOutput.Width(), rcDibOutput.Height(),
			0, 0, pBitmapInfoHeader->biWidth, pBitmapInfoHeader->biHeight, pDibBits,
			(BITMAPINFO*)pBitmapInfoHeader, DIB_RGB_COLORS, SRCCOPY);
	}
	else
	{
		int nMode = pDC->GetMapMode();

		double fZoomX = 1, fZoomY = 1;
		
		if( nMode == MM_ANISOTROPIC )
		{
			CSize szW = pDC->GetWindowExt(), szV = pDC->GetViewportExt();

			fZoomX = szV.cx / double( szW.cx );
			fZoomY = szV.cy / double( szW.cy );
		}
		pDC->SetMapMode( MM_TEXT );

		// works only in MM_TEXT mode!!!
		BOOL bRes = ::DrawDibDraw( 
				DrawDibInstance, *pDC, 
				rcDibOutput.left * fZoomX, rcDibOutput.top * fZoomY, rcDibOutput.Width() * fZoomX, rcDibOutput.Height() * fZoomY,
				(BITMAPINFOHEADER*)pBitmapInfoHeader, pDibBits, 
				0, 0, pBitmapInfoHeader->biWidth, pBitmapInfoHeader->biHeight, DDF_HALFTONE );
		
		pDC->SetMapMode( nMode );	
	}

	if( punkDataObject )
	{
		
		IThumbnailSurfacePtr ptrTS( punkDataObject );
		if( ptrTS )
		{
			ptrTS->Draw( pbi, pDC->m_hDC, 
				rcTarget, CSize( pBitmapInfoHeader->biWidth, pBitmapInfoHeader->biHeight),
				punkDataObject
				);
		}
		

		INamedDataObject2Ptr ptrDO( punkDataObject );
		if( ptrDO )
		{
			IUnknown* punkDoc = 0;			
			ptrDO->GetData( &punkDoc );
			if( punkDoc )
			{
				IThumbnailSurfacePtr ptrTS( punkDoc );
				if( ptrTS )
				{
					ptrTS->Draw( pbi, pDC->m_hDC, 
						rcTarget, CSize( pBitmapInfoHeader->biWidth, pBitmapInfoHeader->biHeight),
						punkDataObject
						);
				}

				punkDoc->Release();	punkDoc = 0;
			}
		}
	}
	


//	VERIFY( bRes );

	//TRACE( "h= %d w= %d (%d %d)\n", rcDibOutput.Width(), rcDibOutput.Height(), pBitmapInfoHeader->biWidth, pBitmapInfoHeader->biHeight );


	//pDC->FrameRect( &rcTarget, &CBrush( RGB(255,0,0) ) );

	//pDC->FrameRect( &rcDibOutput, &CBrush( RGB(0,0,255) ) );


/*	
	 if( pbi == NULL )
		return false;

	BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)pbi;

	BYTE* pDibBits = pbi + ((BYTE)sizeof(BITMAPINFOHEADER)) + ((BYTE)sizeof(TumbnailInfo));

	if( pDC == NULL )
		return false;


	CRect rcDibOutput;

	CSize sizeOutput = ::ThumbnailGetRatioSize( 							
							CSize( rcTarget.Width()+1, rcTarget.Height()+1 ),
							CSize( pBitmapInfoHeader->biWidth, pBitmapInfoHeader->biHeight)														
							);

	rcDibOutput.left	= rcTarget.left + (rcTarget.Width() + 1 - sizeOutput.cx) / 2;
	rcDibOutput.top		= rcTarget.top  + (rcTarget.Height() + 1 - sizeOutput.cy) / 2;

	rcDibOutput.right	= rcDibOutput.left + sizeOutput.cx - 1;
	rcDibOutput.bottom	= rcDibOutput.top  + sizeOutput.cy - 1;


//todo - use drawdibdraw!!!
	int nResult = ::StretchDIBits(pDC->m_hDC,			// hDC
							   rcDibOutput.left,        // DestX
							   rcDibOutput.top,         // DestY
							   rcDibOutput.Width(),     // nDestWidth
							   rcDibOutput.Height(),    // nDestHeight
							   0,						// SrcX
							   0,						// SrcY
							   pBitmapInfoHeader->biWidth,			// wSrcWidth
							   pBitmapInfoHeader->biHeight,			// wSrcHeight
							   pDibBits,                // lpBits
							   (BITMAPINFO*)pBitmapInfoHeader,                     // lpBitsInfo
							   DIB_RGB_COLORS,          // wUsage
							   SRCCOPY);                // dwROP

	ASSERT( nResult != GDI_ERROR );
	

	
*/
	return true;
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//	IMPORTANT
//
//	See detail info in \vt5\ifaces\thumbnail.h
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/////////////////////////////////////////////////////////////////////////////
// Get Render Object from Data Object
std_dll IUnknown* GetRenderObject( IUnknown* pUnkDataObject )
{
	sptrIThumbnailManager spTM( ::GetAppUnknown() );
	if( spTM == NULL )
		return NULL;

	IUnknown* punkRender = NULL;
	spTM->GetRenderObject( pUnkDataObject, &punkRender );

	return punkRender;
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//	IMPORTANT
//
//	See detail info in \vt5\ifaces\thumbnail.h
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/////////////////////////////////////////////////////////////////////////////
// Get Thumbnail Size
std_dll DWORD ThumbnailGetSize( BYTE* pbi )
{
	if( pbi == NULL )
		return 0;

	BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)pbi;

	return ( ( ( pBitmapInfoHeader->biWidth * 3 + 3 ) / 4) * 4 * pBitmapInfoHeader->biHeight + 
		sizeof(BITMAPINFOHEADER) + sizeof(TumbnailInfo) );
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//	IMPORTANT
//
//	See detail info in \vt5\ifaces\thumbnail.h
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/////////////////////////////////////////////////////////////////////////////
// Allocate thumbnail
std_dll bool ThumbnailAllocate( BYTE** ppbi, CSize sizeThumbnail )
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*ppbi = NULL;
	BITMAPINFOHEADER BitmapInfoHeader;
	::ZeroMemory( &BitmapInfoHeader, sizeof( BitmapInfoHeader ) );
	BitmapInfoHeader.biWidth	= sizeThumbnail.cx;
	BitmapInfoHeader.biHeight	= sizeThumbnail.cy;

	DWORD dwSize = ::ThumbnailGetSize( (BYTE*)&BitmapInfoHeader );
	if( dwSize < sizeof(BITMAPINFOHEADER) + ((BYTE)sizeof(TumbnailInfo)) )
	{
		return false;
	}
	
	*ppbi = new BYTE[ dwSize ];
	memset(*ppbi, 0, dwSize);

	return true;
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//	IMPORTANT
//
//	See detail info in \vt5\ifaces\thumbnail.h
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/////////////////////////////////////////////////////////////////////////////
// Free thumbnail
std_dll bool ThumbnailFree( BYTE* pbi )
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( pbi == NULL )
		return false;

	delete[] pbi;
	return true;
}
/////////////////////////////////////////////////////////////////////////////


//return the SyncManager component unknown
std_dll IUnknown *GetSyncManager()
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ISyncManager );
	if( punk )return punk;
	IApplicationPtr	ptrA( GetAppUnknown() );
	IUnknown	*punkD;
	ptrA->GetActiveDocument(&punkD );
	if( !punkD )return 0;
	punk = _GetOtherComponent( punkD, IID_ISyncManager );
	punkD->Release();

	return punk;
}


std_dll void SetBaseObject( IUnknown *punk, IUnknown *punkBase, IUnknown *punkDocument )
{
	if( punk == 0 || punkBase == 0 )
		return;
	INamedDataObject2Ptr ptrBase = punkBase;

//only parent object can be base
	IUnknown	*punkParent = 0;
	ptrBase->GetParent( &punkParent );

	if( punkParent )
	{
		ptrBase = punkParent;
		punkParent->Release();
	}

	BOOL bFlag = false;
	ptrBase->IsBaseObject(&bFlag);

	if (bFlag)
	{
		GuidKey BaseKey;
		INamedDataObject2Ptr ptrN = punk;
		if( SUCCEEDED( ptrBase->GetBaseKey(&BaseKey)) && BaseKey != INVALID_KEY )
			ptrN->SetBaseKey(&BaseKey);
	}
}

std_dll CString GetDirectory( const char *pszRecord, const char *pszFolder )
{
	char	szPathName[_MAX_PATH];

	::GetModuleFileName( ::AfxGetInstanceHandle(), szPathName, _MAX_PATH );
	::GetLongPathName( szPathName, szPathName, _MAX_PATH ) ;

	char	*pszLast =strrchr( szPathName, '\\' );
	ASSERT( pszLast );
	*pszLast = 0;
	strcat( szPathName, "\\" );
	strcat( szPathName, pszFolder );
	strcat( szPathName, "\\" );

	CString strFileName = 
	::GetValueString( ::GetAppUnknown(), _T("Paths"), pszRecord, szPathName );

	return strFileName;
}

//helpers

std_dll void MemToText( byte *ptr, long nLength, CStringArrayEx &sa )
{
	CString	strLine;

	char szLongString[255];

	int	iChPos = 0;

	for( int i = 0; i < nLength; i++ )
	{
		iChPos += ::sprintf( szLongString+iChPos, "%02X ", ptr[i] );

		if( iChPos > 90 || i == nLength-1 )
		{
			szLongString[iChPos]=0;
			sa.Add( szLongString );
			iChPos = 0;
		}
	}

	sa.Add( END_OF_MLS );
}

std_dll byte *TextToMem( CStringArrayEx &sa, int &nPos, long &nLength )
{
//1. Get buffer size

	nLength = 0;
	for( int i = nPos; i < sa.GetSize() && sa[i] != END_OF_MLS; i++ )
		nLength += sa[i].GetLength()/3;

	byte	*ptr = new byte[nLength];
	long	nCount = 0;

	for( i = nPos; i < sa.GetSize() && sa[i] != END_OF_MLS; i++ )
	{
		const	char *sz = sa[i];
		int	nLineLen = ::strlen( sz );

		for( int j = 0; j <nLineLen; j+=3 )
		{
			int	nVal;
			::sscanf( sz+j, "%x", &nVal );
			ptr[nCount++] = (byte)nVal;
		}
	}

	nPos = i;

	return ptr;
}


extern "C" std_dll void GetRegCompanyName(char * szCompanyName, long * plSize)
{
	HMODULE hModule = GetModuleHandle(0);
	if (hModule)
	{
		typedef void (*GCN)(char *, long *);
		GCN pfn = (GCN)GetProcAddress(hModule, "GuardGetCompanyName");
		if (pfn)
			pfn(szCompanyName, plSize);
	}
}

extern "C" std_dll void GetRegAppName(char * szAppName, long * plSize)
{
	HMODULE hModule = GetModuleHandle(0);
	if (hModule)
	{
		typedef void (*GAN)(char *, long *);
		GAN pfn = (GAN)GetProcAddress(hModule, "GuardGetAppName");
		if (pfn)
			pfn(szAppName, plSize);
	}
}

extern "C" std_dll void GetRegProgKey(char * szRegKey, long * plSize)
{
	CString strRet("Software\\");
	CString strAppName(DEF_APP_NAME);
	CString strCompanyName(DEF_COMPANY_NAME);

	long lSize = 0;
	GetRegCompanyName(0, &lSize);
	if (lSize)
	{
		GetRegCompanyName(strCompanyName.GetBuffer(lSize + 1), &lSize);
		strCompanyName.ReleaseBuffer();
	}

	GetRegAppName(0, &lSize);
	if (lSize)
	{
		GetRegAppName(strAppName.GetBuffer(lSize + 1), &lSize);
		strAppName.ReleaseBuffer();
	}
	strRet += strCompanyName + _T("\\") + strAppName + _T("\\add-ins");
	if (plSize)
		*plSize = strRet.GetLength();

	if (AfxIsValidAddress(szRegKey, strRet.GetLength() + 1))
		lstrcpy(szRegKey, strRet);
}

//find the type aliase
std_dll CString GetTypeAliase( const char *pszType )
{
	IAliaseManPtr	ptrMan( GetAppUnknown() );
	if( ptrMan != 0 )
	{
		bstr_t	bstrType = pszType;
		BSTR	bstrA;





		if( ptrMan->GetObjectString( bstrType, &bstrA ) == S_OK )
		{
			CString	str = bstrA;
			::SysFreeString( bstrA );

			return str;
		}


	}
	return pszType;
}

//find the type using aliase
std_dll CString FindTypeByAliase( const char *pszAliase )
{
	IDataTypeInfoManagerPtr	ptrMan( GetAppUnknown() );

	long	nCount, n;
	ptrMan->GetTypesCount( &nCount );

	for( n = 0; n < nCount; n++ )


	{
		INamedDataInfo	*pTypeInfo = 0;
		ptrMan->GetTypeInfo( n, &pTypeInfo );

		INamedObject2Ptr	ptrN( pTypeInfo );
		pTypeInfo->Release();

		BSTR	bstr;
		ptrN->GetUserName( &bstr );








		CString	str = bstr;
		::SysFreeString( bstr );


		if( str == pszAliase )
			return ::GetObjectName( ptrN );
	}
	return pszAliase;
}


//AjustViewForObject - change view type according object, show object
std_dll void AjustViewForObject( IUnknown *punkView, IUnknown *punkObject, DWORD dwFlags )
{
	CString	strObjectType = ::GetObjectKind( punkObject );
	_bstr_t	bstrObjectType = strObjectType;
	// find appropriate view  for object
	IViewPtr		ptrV( punkView );
	IDataContextPtr	ptrContext( punkView );

	DWORD	dwMatch = mvNone;
	// check current view can show loaded object
	if (ptrV != 0)
	{
		if( (dwFlags & avf_Force) == 0 )
			ptrV->GetMatchType(bstrObjectType, &dwMatch );

		// if active view can not show loaded object
		// try to find appropriate view
		if (dwMatch == mvNone)
		{
			//find the most match view
			_bstr_t bstrNewViewProgID = ::FindMostMatchView(bstrObjectType, dwMatch);
			//replace unmatch view
			if (dwMatch > mvNone)
			{
				//get the frame
				IUnknown *		punkFrame = 0;
				// get view site 
				sptrIViewSite	ptrVS(ptrV);
				// get frame window pointer
				ptrVS->GetFrameWindow(&punkFrame);
				sptrIFrameWindow	sptrF(punkFrame) ;
				if (punkFrame)
					punkFrame->Release();

				//get the splitter
				IUnknown * punkSplitter = 0;
				sptrF->GetSplitter(&punkSplitter);
				sptrISplitterWindow	sptrS(punkSplitter);
				if (punkSplitter)
					punkSplitter->Release();

				//find the current active view in splitter
				int	nRowCount, nColCount;
				int	nRowFound = -1, nColFound = -1;

				sptrS->GetRowColCount(&nRowCount, &nColCount);
				
				for (int nCol = 0; nCol < nColCount; nCol++)
				{
					for (int nRow = 0; nRow < nRowCount; nRow++)
					{
						IUnknown *	punkTryView = 0;
						sptrS->GetViewRowCol(nRow, nCol, &punkTryView);

						if (GetObjectKey(punkTryView) == ::GetObjectKey(ptrV))
						{
							// founded
							nRowFound = nRow;
							nColFound = nCol;
						}
						punkTryView->Release();
					}
				}
				//change view!
				
				if (nRowFound != -1 && nColFound != -1)
				{
					ptrV = 0;
					ptrVS = 0;
					ptrContext = 0;
					// change view type
					sptrS->ChangeViewType(nRowFound, nColFound, bstrNewViewProgID);
					// get new view unknown
					IUnknown *	punkTryView = 0;
					sptrS->GetViewRowCol(nRowFound, nColFound, &punkTryView);

					ptrContext = punkTryView;
					if (punkTryView)
						punkTryView->Release();

					if (ptrContext != 0)
						ptrContext->SetActiveObject( bstrObjectType, punkObject, aofActivateDepended|aofSkipIfAlreadyActive );
				}
			}
		}
	}
}


COleObjectFactory * FindNextComponent(COleObjectFactory * pf, AFX_MODULE_STATE * pModuleState = 0);
COleObjectFactory * FindFirstComponent(AFX_MODULE_STATE * pModuleState = 0);

// {ffffffff-ffff-ffff-ffff-ffffffffffff}
static GUID cmp_guid1 = { 0xffffffff, 0xffff, 0xffff, { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };
// {00000000-0000-0000-0000-000000000000}
static GUID cmp_guid2= { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

class COleObjectFactoryWrp : public COleObjectFactory
{
public:
	LPCTSTR GetProgID()	{	return m_lpszProgID;	}
	CLSID & GetCLSID()	{	return m_clsid;			}
};

std_dll bool NeedComponentInfo(REFCLSID guid1, REFIID guid2)
{
	return GuidKey(guid1) == GuidKey(cmp_guid1) && GuidKey(guid2) == GuidKey(cmp_guid2);
}

std_dll HRESULT GetComponentInfo(LPVOID * ppv)
{
	__GuardComponentInfo * pInfo = *((__GuardComponentInfo**)ppv);
	if (!pInfo || pInfo->nSize != sizeof(__GuardComponentInfo))
		return E_INVALIDARG;
		
	pInfo->bstrName = 0;
	pInfo->guid = GUID_NULL;
	pInfo->bIsAction = false;

	COleObjectFactoryWrp * pFact = 0;

	if (!pInfo->lpos)
	{
		return E_FAIL;
	}
	else if (pInfo->lpos == (DWORD)-1) // get first component
	{
		pFact = (COleObjectFactoryWrp*)FindFirstComponent(AfxGetModuleState());
		pInfo->lpos = (DWORD_PTR)FindNextComponent(pFact, AfxGetModuleState());
	}
	else // get next component
	{
		pFact = (COleObjectFactoryWrp*)pInfo->lpos;
		pInfo->lpos = (DWORD_PTR)FindNextComponent(pFact, AfxGetModuleState());
	}

	if (!pFact)
	{
		pInfo->lpos = 0;
		return E_FAIL;
	}

	CString strName(pFact->GetProgID());
	pInfo->bstrName = strName.AllocSysString();
	pInfo->guid = pFact->GetClassID(); 
	pInfo->bIsAction = strName.Find(ACTION_NAME_PREFIX) != -1;

	return S_OK;
}


COleObjectFactory * FindFirstComponent(AFX_MODULE_STATE * pModuleState)
{
	if (!pModuleState)
		pModuleState = AfxGetModuleState();

	for (COleObjectFactory * pFactory = pModuleState->m_factoryList; 
		 pFactory != NULL; 
		 pFactory = pFactory->m_pNextFactory)
	{
		if (pFactory)
			return pFactory;
	}

	// register extension DLL factories
	for (CDynLinkLibrary * pDLL = pModuleState->m_libraryList; 
		 pDLL != NULL; 
		 pDLL = pDLL->m_pNextDLL)
	{
		for (pFactory = pDLL->m_factoryList; 
			 pFactory != NULL; 
			 pFactory = pFactory->m_pNextFactory)
		{
			if (pFactory)
				return pFactory;
		}
	}
	return 0;
}

COleObjectFactory * FindNextComponent(COleObjectFactory * pf, AFX_MODULE_STATE * pModuleState)
{
	if (!pf)
		return 0;

	bool bFounded = false;

	if (!pModuleState)
		pModuleState = AfxGetModuleState();

	for (COleObjectFactory * pFactory = pModuleState->m_factoryList; 
		 pFactory != NULL; 
		 pFactory = pFactory->m_pNextFactory)
	{
		if (!bFounded)
		{
			if (pf == pFactory)
				bFounded = true;
		}
		else
		{
			if (pFactory)
				return pFactory;
		}
	}

	// register extension DLL factories
	for (CDynLinkLibrary * pDLL = pModuleState->m_libraryList; 
		 pDLL != NULL; 
		 pDLL = pDLL->m_pNextDLL)
	{
		for (pFactory = pDLL->m_factoryList; 
			 pFactory != NULL; 
			 pFactory = pFactory->m_pNextFactory)
		{
			if (!bFounded)
			{
				if (pf == pFactory)
					bFounded = true;
			}
			else
			{
				if (pFactory)
					return pFactory;
			}
		}
	}

	return 0;
}

std_dll void CalcObject(IUnknown* punkCalc, IUnknown* punkImage, bool bSetParams, ICompManager *pGroupManager )
{
	if( !pGroupManager )
	{
		IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IMeasureManager, szMeasurement);
		if( !punk )return;
		ICompManagerPtr	ptrManager( punk );
		punk->Release();
		pGroupManager = ptrManager;
	}

	IDataObjectListPtr ptrObjectList;

	if( bSetParams )
	{
		INamedDataObject2Ptr ptrObject(punkCalc);
		IUnknown* punkParent = 0;
		ptrObject->GetParent(&punkParent);
		ptrObjectList = punkParent;
		if( punkParent )punkParent->Release();
		if( ptrObjectList == 0 )return;
	}

	int	nCount;
	pGroupManager->GetCount( &nCount );
	for( int idx = 0; idx < nCount; idx++ )
	{
		IUnknown	*punk = 0;
		pGroupManager->GetComponentUnknownByIdx( idx, &punk );
		IMeasParamGroupPtr	ptrG( punk );
		if( punk )punk->Release();
		if( ptrG == 0 )continue;

		if( bSetParams )
			ptrG->InitializeCalculation( ptrObjectList );
		ptrG->CalcValues( punkCalc, punkImage );
		if( bSetParams )
			ptrG->FinalizeCalculation();
	}

	if( bSetParams )
		ptrObjectList->UpdateObject( punkCalc );
}

IUnknown	*g_punkSourceObject = 0;

std_dll void SetSourceObject( IUnknown *punkObject )
{
	g_punkSourceObject = punkObject;
}

std_dll IUnknown *FindSourceObject( IUnknown *punkData, IUnknown *punkObject )
{
	g_punkSourceObject = 0;
	::FireEventNotify( GetAppUnknown(), szEventNoDataObject, punkData, punkObject, ncRemoveObject );
	if( g_punkSourceObject )g_punkSourceObject->AddRef();
	return g_punkSourceObject;
}


std_dll bool SetAppropriateView(IUnknown* punkTarget, DWORD dwType)
{
	DWORD dwFlags = 0;
	{
		IImageViewPtr sptrIV(punkTarget);
		if(sptrIV != 0)sptrIV->GetImageShowFlags(&dwFlags);
	}

	CString strDesireViewType;
	if((dwType & avtSource) == avtSource)
		strDesireViewType = szSourceViewProgID;
	else if((dwType & avtObject) == avtObject)
		strDesireViewType = ::GetValueString(GetAppUnknown(), "General", "LastUsedObjectView", szImageViewProgID);
	else if((dwType & avtBinary) == avtBinary)
		strDesireViewType = ::GetValueString(GetAppUnknown(), "General", "LastUsedBinaryView", szBinaryForeViewProgID);
		

	IViewSitePtr sptrV(punkTarget);
	if(sptrV == 0)return false;
	GUID	key = ::GetObjectKey( punkTarget );

	IUnknown* punkFrame = 0;
	sptrV->GetFrameWindow(&punkFrame);
	IFrameWindowPtr	sptrF(punkFrame) ;
	if (punkFrame)punkFrame->Release();
	sptrV  = 0;
	if(sptrF == 0)return false;

	IUnknown * punkSplitter = 0;
	sptrF->GetSplitter(&punkSplitter);
	ISplitterWindowPtr	sptrS(punkSplitter);
	if (punkSplitter)punkSplitter->Release();
	sptrF = 0;

	if(sptrS == 0)return false;

	//find the current active view in splitter
	int	nRowCount, nColCount;
	int	nRowFound = -1, nColFound = -1;
	bool bFounded = false;
	sptrS->GetRowColCount(&nRowCount, &nColCount);
	
	for (int nCol = 0; nCol < nColCount; nCol++)
	{
		for (int nRow = 0; nRow < nRowCount; nRow++)
		{
			IUnknown *	punkTryView = 0;
			sptrS->GetViewRowCol(nRow, nCol, &punkTryView);
			GUID	keyTest = GetObjectKey( punkTryView );
			if( punkTryView )punkTryView->Release();

			if ( keyTest == key )
			{
				nRowFound = nRow;
				nColFound = nCol;
				bFounded = true;
				break;
			}
		}
		if(bFounded)
			break;
	}

	//change view type
	if (nRowFound != -1 && nColFound != -1)
	{
		BSTR bstrViewType = 0;
		sptrS->GetViewTypeRowCol(nRowFound, nColFound, &bstrViewType);
		CString strCurViewType(bstrViewType);
		::SysFreeString(bstrViewType);
		if(strDesireViewType != strCurViewType)
		{
			IUnknown *punkAM = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
			IActionManagerPtr	ptrAM( punkAM );
			punkAM->Release();
			
			BOOL	bOldLock = false;
			ptrAM->LockInitInteractive( TRUE, &bOldLock );
			sptrS->ChangeViewType(nRowFound, nColFound, _bstr_t(strDesireViewType));
			ptrAM->LockInitInteractive( bOldLock, 0 );
		}

		return true;
	}

	return false;
}


CPtrList	g_listLock;
struct	PtrLockInfo
{
	std::string	strFile;
	int			nLine;
	CCmdTarget	*pcmdTarget;
};

POSITION	_OleFindLockInfoDbg( CCmdTarget *ptarget )
{
	POSITION	pos = g_listLock.GetHeadPosition();
	while( pos )
	{	
		PtrLockInfo	*pinfo = (PtrLockInfo	*)g_listLock.GetAt( pos );
		if( pinfo->pcmdTarget == ptarget )
			return pos;
		g_listLock.GetNext( pos );
	}
	return 0;
}



std_dll void _OleLockAppDbg( CCmdTarget *ptarget, int nLine, const char *pszFile )
{
	AfxOleLockApp();

	PtrLockInfo	*p = new PtrLockInfo;
	p->nLine = nLine;
	p->strFile = pszFile;
	p->pcmdTarget = ptarget;

	g_listLock.AddTail( p );
}

std_dll void _OleUnlockAppDbg( CCmdTarget *ptarget )
{
	POSITION	pos = _OleFindLockInfoDbg( ptarget );
	ASSERT( pos );

	PtrLockInfo	*pinfo = (PtrLockInfo	*)g_listLock.GetAt( pos );
	delete pinfo;
	g_listLock.RemoveAt( pos );

	AfxOleUnlockApp();
}

std_dll void _OleTraceLeaks()
{
	POSITION	pos = g_listLock.GetHeadPosition();
	while( pos )
	{
		PtrLockInfo	*pinfo = (PtrLockInfo	*)g_listLock.GetNext( pos );
		char	sz[255];
		sprintf( sz, "Ole object found file %s{%d}\n", pinfo->strFile.c_str(), pinfo->nLine );
		OutputDebugString( sz );
	}
}

std_dll IUnknown *GetActiveMeasureObject( INamedDataObject2Ptr	ptrNamedObject )
{
	if( ptrNamedObject == 0 )return 0;
	IUnknown	*punk = 0;
	ptrNamedObject->GetParent( &punk );
	if( !punk )return 0;
	ICalcObjectContainerPtr	ptrCont = punk;
	punk->Release();

	if( ptrCont == 0 )return 0;
	LONG_PTR lpos; long lkey;
	ptrCont->GetCurentPosition( &lpos, &lkey );

	ptrNamedObject->GetFirstChildPosition((POSITION*)&lpos);

	while( lpos )
	{
		IUnknown *punk = 0;
		ptrNamedObject->GetNextChild((POSITION*)&lpos, &punk);
		if( !punk )continue;
		IManualMeasureObjectPtr	ptrM = punk;
		punk->Release();

		if( ptrM == 0 )continue;
		
		long	lTestKey = -1;
		ptrM->GetParamInfo( &lTestKey, 0 );
		if( lTestKey == lkey )
			return ptrM.Detach();
	}
	return 0;
}


std_dll GUID GetInternalGuid( GUID guidExternal )
{
	GUID guidInternal = INVALID_KEY;	
	IVTApplicationPtr ptrApp( ::GetAppUnknown() );
	if( ptrApp == 0 )
	{
		ASSERT( false );
		return guidInternal; 
	}
	
	if( S_OK != ptrApp->GetEntry(0, (DWORD*)&guidExternal, (DWORD*)&guidInternal, 0) )
	{
		ASSERT( false );
		guidInternal = INVALID_KEY;		
		return guidInternal; 
	}					

	return guidInternal;
}

std_dll GUID GetExternalGuid( GUID guidInternal )
{
	//PROFILE_TEST( "common.dll\\utils.cpp\\GetExternalGuid" );

	GUID guidExternal = INVALID_KEY;
	IVTApplicationPtr ptrApp( ::GetAppUnknown() );

	if( ptrApp == 0 )
	{
		ASSERT( false );
		return guidExternal; 
	}

	if( S_OK != ptrApp->GetEntry( 1, (DWORD*)&guidInternal, (DWORD*)&guidExternal, 0 ) )
	{
		ASSERT( false );
		guidExternal = INVALID_KEY;		
		return guidExternal;		
	}

	return guidExternal;
}

std_dll IUnknown* FindMeasGroup( GUID clsidGroup )
{
	IUnknown *punk = 0;
	punk = FindComponentByName( GetAppUnknown(false), IID_IMeasureManager, szMeasurement );
	if( !punk )
		return 0;

	ICompManagerPtr ptrGroupMan( punk );
	punk->Release();	punk = 0;

	if( ptrGroupMan == 0 )
		return 0;

	int	nCount = 0;
	ptrGroupMan->GetCount( &nCount );	


	for( int idx = 0; idx < nCount; idx++ )
	{
		punk = 0;
		ptrGroupMan->GetComponentUnknownByIdx( idx, &punk );

		if( !punk )
			continue;

		IPersistPtr ptrPersist( punk );
		punk->Release();	punk = 0;

		if( ptrPersist == 0 )
			continue;

		//get external CLSID
		CLSID clsid;
		::ZeroMemory( &clsid, sizeof(CLSID) );
		ptrPersist->GetClassID( &clsid );

		if( clsidGroup == clsid )
		{
			ptrPersist->AddRef();
			return (IUnknown*)ptrPersist;
		}
	}

	return 0;
}


std_dll bool SyncSplitterView( IUnknown* punkViewFocused, SyncViewData* pData )
{
	if( !pData )
		return false;

	if( pData->dwStructSize != sizeof(SyncViewData) )
		return false;

	if( !pData->punkObject2Activate )
		return false;


	IViewPtr ptrViewFocused( punkViewFocused );
	if( ptrViewFocused == 0 )
		return false;

	IUnknown* punkDoc = 0;
	ptrViewFocused->GetDocument( &punkDoc );

	if( !punkDoc )
		return false;

	IDocumentSitePtr ptrDS( punkDoc );
	punkDoc->Release();	punkDoc = 0;

	if( ptrDS == 0 )
		return false;

	GUID guidFocusedView = ::GetObjectKey( ptrViewFocused );

	_bstr_t bstrObjType = ::GetObjectKind( pData->punkObject2Activate );

	TPOS lPos = 0;
	ptrDS->GetFirstViewPosition( &lPos );

	bool	bProcess = false;
	
	while( lPos )
	{
		IUnknown* punkView = 0;
		ptrDS->GetNextView( &punkView, &lPos );
		if( !punkView )
			continue;

		IViewPtr ptrV( punkView );
		punkView->Release();	punkView = 0;

		if( ptrV == 0 )
			continue;

		if( ::GetObjectKey( ptrV ) == guidFocusedView )
			continue;

		if( pData->bOnlyViewInterfaceMatch && pData->iidViewMatch != INVALID_KEY )
		{
			if( !CheckInterface( ptrV, pData->iidViewMatch ) )
				continue;
		}

		DWORD	dwMatchType = 0;
		ptrV->GetMatchType( bstrObjType, &dwMatchType );


		if( dwMatchType >= mvPartial )
		{
			IDataContext2Ptr ptrDC( ptrV );
			if( ptrDC == 0 )
				continue;

			if( S_OK == ptrDC->SetObjectSelect( pData->punkObject2Activate, 1 ) )
			{
				bProcess = true;

				if( !pData->bSyncAllView )
					return true;
			}			
		}
		else if( pData->bCanChangeViewType )
		{
			AjustViewForObject( ptrV, pData->punkObject2Activate );

			bProcess = true;

			if( !pData->bSyncAllView )
				return true;			
		}		
	}

	return true;
}

std_dll bool HelpDisplay( const char *pszFileName )
{
	if( !pszFileName )
		return false;

	char	sz[MAX_PATH];
	::GetModuleFileName( AfxGetApp()->m_hInstance, sz, sizeof( sz ) );
	::GetLongPathName( sz, sz, sizeof(sz) );
	char	szChm[MAX_PATH];
	::_splitpath( sz, 0, 0, szChm, 0 );

	return HelpDisplayTopic( szChm, "", pszFileName, "$GLOBAL_main" );
}

static CString _GetTemplateString(BOOL bOpenFileDialog, LPCTSTR lpstrSubSec)
{
	CString sSec = _T("\\FileOpenDialog");
	if (lpstrSubSec != NULL)
		sSec += CString(_T("\\")) + lpstrSubSec;
	CString strFilterTemplate;
	if (bOpenFileDialog)
		strFilterTemplate = ::GetValueString( GetAppUnknown(), sSec, "OpenFilterTemplate", "" );
	else
		strFilterTemplate = ::GetValueString( GetAppUnknown(), sSec, "SaveFilterTemplate", "" );
	if (strFilterTemplate.IsEmpty())
		strFilterTemplate = ::GetValueString( GetAppUnknown(), sSec, "FilterTemplate", "" );
	return strFilterTemplate;
}

std_dll BOOL IsGoodExt( CString &strExt, BOOL bOpenFileDialog, LPCTSTR lpstrSubSec )
{

	if( strExt.IsEmpty() )
		return true;


	BOOL bUseFilterTemplate = ::GetValueInt( GetAppUnknown(), "\\FileOpenDialog", "UseTemplate", 0 );
	
	if( bUseFilterTemplate )
	{
		CString strFilterTemplate;
		if (lpstrSubSec == 0)
			strFilterTemplate = _GetTemplateString(bOpenFileDialog, NULL);
		else
		{
			strFilterTemplate = _GetTemplateString(bOpenFileDialog, lpstrSubSec);
			if (strFilterTemplate.IsEmpty())
				strFilterTemplate = _GetTemplateString(bOpenFileDialog, NULL);
		}
		/*if (bOpenFileDialog)
			strFilterTemplate = ::GetValueString( GetAppUnknown(), "\\FileOpenDialog", "OpenFilterTemplate", "" );
		else
			strFilterTemplate = ::GetValueString( GetAppUnknown(), "\\FileOpenDialog", "SaveFilterTemplate", "" );
		if (strFilterTemplate.IsEmpty())
			strFilterTemplate = ::GetValueString( GetAppUnknown(), "\\FileOpenDialog", "FilterTemplate", "" );*/

		if( strFilterTemplate.IsEmpty() )
			return true;

		if( strExt.Find( "\n", 0 ) == -1 )
		{
			if( strFilterTemplate.Find( strExt.Right( strExt.GetLength() - 1 ), 0 ) == - 1 )
				return false;
		}
		else
		{
			CString ResultStr = "";
			int nSz = strExt.GetLength();
			int nBrID = -1;
			for(int i = 0; i < nSz;i++ )
			{
				if( strExt[i] == '\n' )
				{
					CString Res;
					
					if( nBrID >= 0 )
					{
						Res = strExt.Mid( nBrID+1, i-nBrID-1 );
//						Res = strExt.Right( nBrID );
//						Res = Res.Left( i - nBrID );
					}
					else
						Res = strExt.Left( i );
					
					nBrID = i;

					if( strFilterTemplate.Find( Res.Right( Res.GetLength() - 1 ), 0 ) != - 1 )
					{
						if( !ResultStr.IsEmpty() )
							ResultStr += "\n" + Res;
						else
							ResultStr = Res;
					}
				}
			}

			{
				CString Res;
				
				if( nBrID >= 0 )
					Res = strExt.Right( strExt.GetLength() - nBrID - 1 );
				else
					Res = strExt.Left( i );
				
				nBrID = i;

				if( strFilterTemplate.Find( Res.Right( Res.GetLength() - 1 ), 0 ) != - 1 )
				{
					if( !ResultStr.IsEmpty() )
						ResultStr += "\n" + Res;
					else
						ResultStr = Res;
				}
			}


			if( ResultStr.IsEmpty() )
				return false;

			strExt = ResultStr;

		}
	}

	return true;

}

std_dll CString MakeAllFilesStringForDocument(LPCTSTR lpstrDocName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strFilterName;
	strFilterName.Format(IDS_ALLFILTER_NAME_FORMAT, lpstrDocName);
	return strFilterName;
}
