#include "stdafx.h"

#include "StreamEx.h"
#include "StreamUtils.h"
#include "Utils.h"
#include "\vt5\AWIN\profiler.h"


//////////////////////////////////////////////////////////////////////
CStreamEx::CStreamEx( IStream* pStream, bool bLoading )
{
	m_ptrStream = pStream;
	m_bLoading	= bLoading;
}

//////////////////////////////////////////////////////////////////////
CStreamEx::~CStreamEx()
{

}

//////////////////////////////////////////////////////////////////////
bool CStreamEx::Verify()
{
	if( m_ptrStream == 0 )
	{
		ASSERT( false );
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
CStreamEx::operator IStream*() const
{
	return m_ptrStream;
}

//////////////////////////////////////////////////////////////////////
BOOL CStreamEx::IsLoading()
{
	return m_bLoading ? TRUE : FALSE;
}

//////////////////////////////////////////////////////////////////////
BOOL CStreamEx::IsStoring()
{
	return !IsLoading();
}

//////////////////////////////////////////////////////////////////////
UINT CStreamEx::Read(void* lpBuf, UINT nMax)
{
	if( !Verify() )
		return -1;


	ULONG nRead = 0;
	HRESULT hr = m_ptrStream->Read( lpBuf, (ULONG)nMax, &nRead );
	if( hr != S_OK )
		CFileException::ThrowOsError( ::GetLastError() );

	ASSERT( nMax >= nRead );

	if( nRead != nMax )
		CFileException::ThrowErrno( CFileException::endOfFile );		

	return (UINT)nRead;
}

//////////////////////////////////////////////////////////////////////
void CStreamEx::Write(const void* lpBuf, UINT nMax)
{
	if( !Verify() )
		return;

	ULONG nWrite = 0;
	HRESULT hr = m_ptrStream->Write( lpBuf, (ULONG)nMax, &nWrite );
	if( hr != S_OK )
		CFileException::ThrowOsError( ::GetLastError() );

	ASSERT( nWrite == nMax );
}

//////////////////////////////////////////////////////////////////////
void CStreamEx::Flush()
{

}


// insertion operations
//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator<<(BYTE by)
{
	Verify();

	Write( (void*)&by, sizeof(BYTE) );	

	return *this;
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator<<(WORD w)
{
	Verify();

	Write( (void*)&w, sizeof(WORD) );	

	return *this;
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator<<(LONG l)
{
	Verify();

	Write( (void*)&l, sizeof(LONG) );

	return *this;
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator<<(DWORD dw)
{
	Verify();

	Write( (void*)&dw, sizeof(DWORD) );	

	return *this;
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator<<(float f)
{
	Verify();

	Write( (void*)&f, sizeof(float) );

	return *this;
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator<<(double d)
{
	Verify();

	Write( (void*)&d, sizeof(double) );	

	return *this;
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator<<(int i)
{
	Verify();

	return CStreamEx::operator<<((LONG)i);
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator<<(short w)
{
	Verify();

	return CStreamEx::operator<<((WORD)w);
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator<<(char ch)
{
	Verify();

	return CStreamEx::operator<<((BYTE)ch);
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator<<(unsigned u)
{
	Verify();

	return CStreamEx::operator<<((LONG)u);
}


// extraction operations
//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator>>(BYTE& by)
{
	Verify();

	Read( (void*)&by, sizeof(BYTE) );	

	return *this;
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator>>(WORD& w)
{
	Verify();

	Read( (void*)&w, sizeof(WORD) );		

	return *this;
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator>>(DWORD& dw)
{
	Verify();

	Read( (void*)&dw, sizeof(DWORD) );

	return *this;
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator>>(LONG& l)
{
	Verify();

	Read( (void*)&l, sizeof(LONG) );	

	return *this;
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator>>(float& f)
{
	Verify();

	Read( (void*)&f, sizeof(float) );		

	return *this;
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator>>(double& d)
{
	Verify();

	Read( (void*)&d, sizeof(double) );

	return *this;
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator>>(int& i)
{
	Verify();
	return CStreamEx::operator>>((LONG&)i);
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator>>(short& w)
{
	Verify();
	return CStreamEx::operator>>((WORD&)w);
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator>>(char& ch)
{
	Verify();
	return CStreamEx::operator>>((BYTE&)ch);
}

//////////////////////////////////////////////////////////////////////
CStreamEx& CStreamEx::operator>>(unsigned& u)
{
	Verify();	
	return CStreamEx::operator>>((LONG&)u);
}

//////////////////////////////////////////////////////////////////////
UINT _ReadStringLength(CStreamEx& ar)
{
	DWORD nNewLen;

	// attempt BYTE length first
	BYTE bLen;
	ar >> bLen;

	if (bLen < 0xff)
		return bLen;

	// attempt WORD length
	WORD wLen;
	ar >> wLen;
	if (wLen == 0xfffe)
	{
		// UNICODE string prefix (length will follow)
		return (UINT)-1;
	}
	else if (wLen == 0xffff)
	{
		// read DWORD of length
		ar >> nNewLen;
		return (UINT)nNewLen;
	}
	else
		return wLen;

}

//////////////////////////////////////////////////////////////////////
std_dll CStreamEx& operator<<(CStreamEx& ar, const CString& string)
{
	if (string.GetLength() < 255)
	{
		ar << (BYTE)string.GetLength();
	}
	else if (string.GetLength() < 0xfffe)
	{
		ar << (BYTE)0xff;
		ar << (WORD)string.GetLength();
	}
	else
	{
		ar << (BYTE)0xff;
		ar << (WORD)0xffff;
		ar << (DWORD)string.GetLength();
	}
	ar.Write( (LPCTSTR)string, string.GetLength()*sizeof(TCHAR));
	return ar;
}


//////////////////////////////////////////////////////////////////////
std_dll CStreamEx& operator>>(CStreamEx& ar, CString& string)
{
	int nConvert = 0;   // if we get UNICODE, convert

	UINT nNewLen = _ReadStringLength(ar);
	if (nNewLen == (UINT)-1)
	{
		nConvert = 1 - nConvert;
		nNewLen = _ReadStringLength(ar);
		ASSERT(nNewLen != -1);
	}

	// set length of string to new length
	UINT nByteLen = nNewLen;
	nByteLen += nByteLen * nConvert;    // bytes to read
	if (nNewLen == 0)
		string.GetBufferSetLength(0);
	else
		string.GetBufferSetLength((int)nByteLen+nConvert);

	// read in the characters
	if (nNewLen != 0)
	{
		ASSERT(nByteLen != 0);

		// read new data	
		if (ar.Read( string.GetBuffer( nByteLen ), nByteLen) != nByteLen)
			AfxThrowArchiveException(CArchiveException::endOfFile);

		string.ReleaseBuffer();

		/*
		// convert the data if as necessary
		if (nConvert != 0)
		{
			CStringData* pOldData = string.GetData();
			LPWSTR lpsz = (LPWSTR)string.m_pchData;
			lpsz[nNewLen] = '\0';    // must be NUL terminated
			string.Init();   // don't delete the old data
			string = lpsz;   // convert with operator=(LPWCSTR)
			CString::FreeData(pOldData);
		}
		*/
	}
	return ar;
}

//////////////////////////////////////////////////////////////////////
std_dll CStreamEx &operator<<(CStreamEx& ar, GuidKey &g)
{	
	long	*pl = (long*)&g;
	ar<<pl[0]<<pl[1]<<pl[2]<<pl[3];
	return ar;
}

//////////////////////////////////////////////////////////////////////
std_dll CStreamEx &operator>>(CStreamEx& ar, GuidKey &g)
{	
	long	*pl = (long*)&g;
	ar>>pl[0]>>pl[1]>>pl[2]>>pl[3];
	return ar;
}

//////////////////////////////////////////////////////////////////////
std_dll CStreamEx& operator<<(CStreamEx& ar, SIZE size)
{ 
	ar.Write( (void*)&size, sizeof(SIZE)); return ar;
}

//////////////////////////////////////////////////////////////////////
std_dll CStreamEx& operator<<(CStreamEx& ar, POINT point)
{ 
	ar.Write( (void*)&point, sizeof(POINT)); return ar;
}

//////////////////////////////////////////////////////////////////////
std_dll CStreamEx& operator<<(CStreamEx& ar, const RECT& rect)
{ 
	ar.Write( (void*)&rect, sizeof(RECT)); return ar;
}

//////////////////////////////////////////////////////////////////////
std_dll CStreamEx& operator>>(CStreamEx& ar, SIZE& size)
{ 
	ar.Read( (void*)&size, sizeof(SIZE)); return ar;
}

//////////////////////////////////////////////////////////////////////
std_dll CStreamEx& operator>>(CStreamEx& ar, POINT& point)
{ 
	ar.Read( (void*)&point, sizeof(POINT)); return ar;
}

//////////////////////////////////////////////////////////////////////
std_dll CStreamEx& operator>>(CStreamEx& ar, RECT& rect)
{ 
	ar.Read( (void*)&rect, sizeof(RECT)); return ar;
}


//////////////////////////////////////////////////////////////////////
//
//
//	New serialize helpers
//
//
//////////////////////////////////////////////////////////////////////

std_dll DWORD GetStreamPos( IStream* pStream )
{	

	if( !pStream )
		return -1;

	LARGE_INTEGER	uiPos;
	::ZeroMemory( &uiPos, sizeof(LARGE_INTEGER) );
	
	ULARGE_INTEGER	uiCurPos;	
	::ZeroMemory( &uiCurPos, sizeof(ULARGE_INTEGER) );

	if( S_OK != pStream->Seek( uiPos, STREAM_SEEK_CUR, &uiCurPos ) )
		return -1;

	return uiCurPos.LowPart;	
}

//////////////////////////////////////////////////////////////////////
std_dll bool StreamSeek( IStream* pStream, DWORD dwOrigin, long lOffset )
{
	LARGE_INTEGER	uiPos = {0};
	uiPos.LowPart = lOffset;
	
	if( S_OK != pStream->Seek( uiPos, dwOrigin, 0 ) )
	{
		ASSERT( 0 );
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
std_dll bool WriteObject( IStream* pStream, IUnknown* punkObj, ObjectStreamInfo* pInfo, bool bForceWriteAsText, SerializeParams *pparams )
{
	if( !pStream )
		return false;	

	DWORD dwStreamFirstPos = GetStreamPos( pStream );
	if( dwStreamFirstPos == -1 )
		return false;

	IPersistPtr ptrPersist( punkObj );
	if( ptrPersist == 0 )
	{
		ASSERT( false );
		return false;
	}

	GUID guidExternal = INVALID_KEY;
	ptrPersist->GetClassID( &guidExternal );

	GUID guidInternal = ::GetInternalGuid( guidExternal );
	if( guidInternal == INVALID_KEY )
		return false;

	bool bSerializeAsText = ( CheckInterface( punkObj, IID_ITextObject ) && bForceWriteAsText );

	if( !bSerializeAsText && !CheckInterface( punkObj, IID_ISerializableObject ) )
		return false;
	

	ObjectStreamInfo osi;
	::ZeroMemory( &osi, sizeof(ObjectStreamInfo) );

	osi.dwVersion		= 1;
	osi.clsidObj		= guidInternal;	
	osi.dwStructSize	= sizeof(ObjectStreamInfo);
	osi.dwStartPos		= dwStreamFirstPos + sizeof(ObjectStreamInfo);
	osi.bSerializeAsText= bSerializeAsText ? TRUE : FALSE;

	CString strObjectType = ::GetObjectKind( punkObj );
	if( strObjectType.GetLength() > 29 )
		return false;

	strcpy( osi.szObjectType, (LPCSTR)strObjectType );

    // ADO Stream не понимает seek за пределы потока - надо просто писать	
	//if( !StreamSeek( pStream, STREAM_SEEK_CUR, sizeof( ObjectStreamInfo ) ) )
	//{
	//	StreamSeek( pStream, STREAM_SEEK_SET, dwStreamFirstPos );
	//	return false;
	//}

	CStreamEx stremex( pStream, false );
	stremex.Write( &osi, sizeof(ObjectStreamInfo) );

	if( bSerializeAsText )
	{
		sptrITextObject	sptrT( punkObj );
		BSTR	bstrText = 0;
		if( S_OK != sptrT->GetText( &bstrText ) )
			return false;

		CString	strText = bstrText;
		::SysFreeString( bstrText );	bstrText = 0;
		
		stremex << strText;
	}
	else
	{
		ISerializableObjectPtr	ptrS( punkObj );
		if( ptrS == 0 )
			return false;

		if( S_OK != ptrS->Store( pStream, pparams ) )
			return false;
	}

	DWORD dwNewPos = GetStreamPos( pStream );
	if( dwNewPos == -1 )
		return false;

	osi.dwObjectSize	= dwNewPos - osi.dwStartPos;

	if( !StreamSeek( pStream, STREAM_SEEK_SET, dwStreamFirstPos ) )
		return false;

	stremex.Write( &osi, sizeof(ObjectStreamInfo) );

	if(	!StreamSeek( pStream, STREAM_SEEK_SET, dwNewPos ) )
		return false;


	if( pInfo )
		memcpy( pInfo, &osi, sizeof(ObjectStreamInfo) );

	return true;
}

//////////////////////////////////////////////////////////////////////
std_dll bool ReadObject( IStream* pStream, IUnknown** ppunkObj, ObjectStreamInfo* pInfo, SerializeParams *pparams )
{
	PROFILE_TEST( "common.dll\\utils.cpp\\ReadObject" );

	if( !pStream )
		return false;

	CStreamEx stremex( pStream, true );

	ObjectStreamInfo osi;
	::ZeroMemory( &osi, sizeof(ObjectStreamInfo) );
	
	stremex.Read( &osi, sizeof(ObjectStreamInfo) );

	if( pInfo )memcpy( pInfo, &osi, sizeof(ObjectStreamInfo) );

	DWORD dwStreamPos = GetStreamPos( pStream );	

	if( dwStreamPos == -1 || dwStreamPos != osi.dwStartPos )
	{
		ASSERT( false );
		return false;
	}


	GUID guidExternal = ::GetExternalGuid( osi.clsidObj );
	if( guidExternal == INVALID_KEY )
		return false;

	CString strObjectType = osi.szObjectType;

	

	INamedDataInfo	*pTypeInfo = 0;
	{
	//PROFILE_TEST( "common.dll\\utils.cpp\\ReadObject->FindTypeInfo" );
	pTypeInfo = ::FindTypeInfo( _bstr_t( strObjectType ) );
	}
	if( pTypeInfo == 0 )
		return false;

	INamedDataInfoPtr ptrNDI = pTypeInfo;
	pTypeInfo->Release(); pTypeInfo = 0;


	IUnknown* punkObject = 0;
	HRESULT hres = E_FAIL;
	{
		//PROFILE_TEST( "stream_ex::CoCreateInstance" )
		hres = ::CoCreateInstance( guidExternal, 0,  1, IID_IUnknown, (void**)&punkObject );
	}
	if( hres != S_OK || punkObject == 0 )
		return false;

	INamedDataObjectPtr ptrNO( punkObject );
	punkObject->Release();	punkObject = 0;

	if( ptrNO == 0 )
		return false;

	{
	//PROFILE_TEST( "common.dll\\utils.cpp\\ReadObject->SetTypeInfo" );
	ptrNO->SetTypeInfo( ptrNDI );
	}

	if( osi.bSerializeAsText )
	{
		ITextObjectPtr ptrT( ptrNO );
		if( ptrT == 0 )
			return false;

		CString		strText;
		stremex >> strText;
		
		if( S_OK != ptrT->SetText( _bstr_t( strText ) ) )
		{
			return false;	
		}		
	}
	else
	{
		//PROFILE_TEST( "common.dll\\utils.cpp\\ReadObject->Load" );

		ISerializableObjectPtr ptrS( ptrNO );
		if( S_OK != ptrS->Load( pStream, pparams ) )
			return false;
	}

	dwStreamPos = GetStreamPos( pStream );

	if( dwStreamPos == -1 || dwStreamPos != osi.dwStartPos + osi.dwObjectSize )
	{
		ASSERT( false );
		return false;
	}


	ptrNO->AddRef();
	*ppunkObj = ptrNO;

	return true;
}

std_dll bool SafeReadObject( IStream* pStream, IUnknown** ppunkObj, LPCSTR szObjectName, bool* pbRecoverSeekSucceded, SerializeParams *pparams )
{
	ObjectStreamInfo osi;
	::ZeroMemory( &osi, sizeof(ObjectStreamInfo) );

	if( ReadObject( pStream, ppunkObj, &osi, pparams ) )
		return true;

	if( szObjectName )
	{
		STATSTG st = {0};
		pStream->Stat( &st, STATFLAG_DEFAULT );
		
		CString strMessage = "Safe read object failed. File name:";
		strMessage += CString( st.pwcsName );
		strMessage += ". Object name:";
		strMessage += CString( szObjectName );
		strMessage += "\n";					

		GetLogFile()<<strMessage;
	}

	if( osi.dwObjectSize <= 0 )
		return false;

	bool b = StreamSeek( pStream, STREAM_SEEK_SET, osi.dwStartPos + osi.dwObjectSize );

	if( pbRecoverSeekSucceded )
		*pbRecoverSeekSucceded = b;

	return false;
}

