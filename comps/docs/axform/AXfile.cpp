// AXfile.cpp: implementation of the CImageFileFilterBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Axfile.h"
#include "resource.h"
#include "afxpriv2.h"
#include "docmiscint.h"

byte char_htobyte( char ch )
{
	if( ch >= '0' && ch <= '9' )
		return (byte)ch-'0';
	else
	if( ch >= 'A' && ch <= 'F' )
		return (byte)ch-'A'+10;
	else
	if( ch >= 'a' && ch <= 'f' )
		return (byte)ch-'a'+10;
	else
		return 0;

}

byte str_htobyte( const char *p )
{
	return (char_htobyte( *p ) << 4) |char_htobyte( *(p+1) );
}

/////////////////////////////////////////////////////////////////////////////
// CAXFileFilter

IMPLEMENT_DYNCREATE(CAXFileFilter, CCmdTargetEx)

// {4E093841-D7EC-11d3-A094-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CAXFileFilter, "FileFilters.AXFileFilter", 
0x4e093841, 0xd7ec, 0x11d3, 0xa0, 0x94, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CAXFileFilter::CAXFileFilter()
{
	_OleLockApp( this );
	m_strDocumentTemplate = szDocumentScript;
	m_strFilterName.LoadString( IDS_AX_FILTERNAME );
	m_strFilterExt = _T(".form");
	AddDataObjectType(szArgumenAXForm);
}

CAXFileFilter::~CAXFileFilter()
{
	_OleUnlockApp( this );
	m_mapFileNameToTitle.RemoveAll( );
}

const char	c_szAX[] = "[AxForm]";
const char	c_szControls[] = "[AXControls]";
const char	c_szScript[] = "[Script]";

bool CAXFileFilter::ReadFile( const char *pszFileName )
{
	//try to use new file filter
	HANDLE	hFile = ::CreateFile( pszFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if( hFile == INVALID_HANDLE_VALUE )
		return false;

	char	szSign[20];
	DWORD	dwRead = 0;
	::ReadFile( hFile, &szSign, strlen( c_szAX ), &dwRead, 0 ); 
	
	bool	bReadText = false;
	if( !strncmp( szSign, c_szAX, strlen( c_szAX ) ) )
		bReadText = true;

	::CloseHandle( hFile );


	//create a form
	CString strType;
	if(m_sptrINamedData)
		strType = GetDataObjectType(0);	
	else
		strType = szTypeAXForm;
	IUnknown	*punk = CreateNamedObject( strType );
	if( !punk )return false;
	IActiveXFormPtr	ptrAX( punk );
	punk->Release();
	// get object name from path and check it's exists
	CString	strObjName = ::GetObjectNameFromPath(pszFileName);
	_bstr_t bstrName(strObjName);
	long	NameExists = 0;

	if(m_sptrINamedData)
		m_sptrINamedData->NameExists(bstrName, &NameExists);

//////////////////////////////////////////////////////////////////////////////////////////////////
//read text file
	if( bReadText )
	{
		long	lVersion = 0;
		CStringArrayEx	strs;
		strs.ReadFile( pszFileName );

		strs.LockSection( c_szAX );

		const char *p;

		if( ! (p = strs.FindEntry( "Version" ) ) )return false;
		::sscanf( p, "%d", &lVersion );
		
		if( p = strs.FindEntry( "Title" ) )
		{
			_bstr_t	bstrTitle( p );

/********* Maxim [10.06.02] ***********************************************************************/			
			CString str( p );
			if( str[0] == '%' && str[str.GetLength() - 1] == '%' )
			{
				m_mapFileNameToTitle.SetAt( pszFileName, str );	// vanek - 18.09.2003
				str = str.Right( str.GetLength() - 1 );
				str = str.Left( str.GetLength() - 1 );
	
				IFormNamesDataPtr ptrFrmNms = ::GetAppUnknown();

				if( ptrFrmNms != 0 )
				{
					BSTR	bstrName = 0;

					if( ptrFrmNms->GetFormName( _bstr_t( str ), &bstrName ) == S_OK )
					{
						bstrTitle = bstrName;
						::SysFreeString( bstrName );
					}
				}
			}
/********* Maxim [10.06.02] ***********************************************************************/			

			ptrAX->SetTitle( bstrTitle );
		}

		if( p = strs.FindEntry( "Size" ) )
		{
			CSize	size( 400, 300 );
			::sscanf( p, "%d, %d", &size.cx, &size.cy );
			ptrAX->SetSize( size );
		}

		strs.UnlockSection();

		int 
		idx = strs.LockSection( c_szControls );

		while( idx != -1 )
		{
			int	idxNext = -1;
			int	i = idx+1;
			
			while( i < strs.GetSize() )
			{
				if( strs[i].IsEmpty() )
				{
					idxNext = i;
					break;
				}
				i++;
			}


			int	nID = -1;
			_bstr_t	bstrProgID;
			_bstr_t	bstrName;
			CRect	rect = CRect( 20, 20, 300, 200 );
			DWORD	dwStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP;
			byte	*pdata = 0;
			_bstr_t	bstrType = "ActiveXCtrl";
			long	nLen = 0;	

			if( p = strs.FindEntry( "Name", idx, idxNext ) )
				bstrName = p;
			if( p = strs.FindEntry( "ProgID", idx, idxNext ) )
				bstrProgID = p;
			if( p = strs.FindEntry( "ClassType", idx, idxNext ) )
				bstrType = p;
			if( p = strs.FindEntry( "Rect", idx, idxNext ) )
				::sscanf( p, "%d, %d, %d, %d", &rect.left, &rect.top, &rect.right, &rect.bottom );
			if( p = strs.FindEntry( "Style", idx, idxNext ) )
				sscanf( p, "%x", &dwStyle );
			if( p = strs.FindEntry( "ID", idx, idxNext ) )
				sscanf( p, "%d", &nID );

			if( p = strs.FindEntry( "Data", idx, idxNext ) )
			{
				sscanf( p, "%d", &nLen );
				if( nLen )
				{
					pdata = new byte[nLen];
					if( pdata )
					{
						char	*p1 = strchr( (char*)p, ' ' );
						if( p1 )p1++;

						long	n = 0;
						while( n < nLen )
						{
							pdata[n] = str_htobyte( p1 );

							p1 += 3;
							n++;

						}
					}
				}
			}

			if( bstrProgID.length() )
			{
				IUnknown	*punkAX = ::CreateTypedObject( bstrType );
				if( punkAX )
				{
					IActiveXCtrlPtr	ptrCtrl = punkAX;
					punkAX->Release();

					INamedDataObject2Ptr	ptrN( ptrCtrl );
					ptrN->SetParent( ptrAX, 0 );

					ptrCtrl->SetProgID( bstrProgID );
					ptrCtrl->SetRect( rect );
					ptrCtrl->SetStyle( dwStyle );
					ptrCtrl->SetControlID( nID );
					ptrCtrl->SetName( bstrName );

					if( pdata )
						ptrCtrl->SetObjectData( pdata, nLen );
				}
			}
			idx = idxNext;

			if( pdata )delete pdata;
		}

		strs.UnlockSection();


		idx = strs.LockSection( c_szScript );

		if( idx != -1 )
		{
			IUnknown	*punkScript = ::CreateTypedObject( szArgumentTypeScript );
			if( punkScript )
			{
				IScriptDataObjectPtr	ptrS( punkScript );
				punkScript->Release();

				int	c = 0;
				for (int i = (int)strs._GetStartPos() + 1; i < strs._GetEndPos(); i++)
				{
					_bstr_t	bstrLine( strs[i] );

					ptrS->InsertString( bstrLine, c );
					c++;
				}

				ptrAX->SetScript( ptrS );
			}
		}
		if (!NameExists && ptrAX != 0 )
			::SetObjectName( ptrAX, strObjName );
	}
	else
	try
	{
		if(CheckInterface(ptrAX, IID_ISerializableObject))
		{
			CFile	file( pszFileName, CFile::modeRead );
			sptrISerializableObject	sptrO( ptrAX );
			{

				SerializeParams	params;
				ZeroMemory( &params, sizeof( params ) );

				CArchive		ar( &file, CArchive::load );
				CArchiveStream	ars( &ar );
				sptrO->Load( &ars, &params );
			}
		}

		// if new name not exists allready
		if (!NameExists && ptrAX != 0 )
		{
			// set this name to object
			INamedObject2Ptr sptrObj(ptrAX);
			sptrObj->SetName(bstrName);
		}
	}
	catch(CException* e)
	{
		e->ReportError();
		e->Delete();
		if(punk)
			punk->Release();
		return false;
	}

	INamedDataObject2Ptr	ptrNamed( ptrAX );
	ptrNamed->SetModifiedFlag( false );

	return true;
}

bool CAXFileFilter::WriteFile( const char *pszFileName )
{
	IUnknown* punk = 0;
	try
	{
		// convert object type to BSTR
		_bstr_t bstrType( GetDataObjectType(0) );

		// get active object in which type == m_strObjKind
		if (!(m_bObjectExists && m_sptrAttachedObject != NULL) && m_sptrIDataContext != NULL)
			m_sptrIDataContext->GetActiveObject( bstrType, &punk );
		else
		{
			punk = (IUnknown*)m_sptrAttachedObject.GetInterfacePtr();
			punk->AddRef();
		}
	
		if( !punk )
			return false;

		IUnknownPtr	ptrObject = punk;
		punk->Release();


		bool	bTextStore = true;

		if( bTextStore )
		{
			CStringArrayEx	strs;
			CString			str;
			long			lVersion = 1;

			strs.Add( c_szAX );
			str.Format( "Version=%d", lVersion );
			strs.Add( str );

			IActiveXFormPtr	ptrAX( ptrObject );
			CSize	size;
			ptrAX->GetSize( &size );
			str.Format( "Size=%d, %d", size.cx, size.cy );
			strs.Add( str );

			// vanek - 18.09.2003
			CString	strTitle( _T("") );
			if( !m_mapFileNameToTitle.Lookup( pszFileName, strTitle ) )
			{
				BSTR	bstr;
				ptrAX->GetTitle( &bstr );
				if( bstr )
				{
					strTitle = bstr;
					::SysFreeString( bstr );
					bstr = 0;
				}
			}
            str = _T("Title=") + strTitle;

			strs.Add( str );

			strs.Add( "" );
			strs.Add( c_szControls );

			INamedDataObject2Ptr	ptrList( ptrObject );
			POSITION	lpos = 0;

			ptrList->GetFirstChildPosition( &lpos );

			while( lpos )
			{
				
				IUnknown	*punkCtrl = 0;
				ptrList->GetNextChild( &lpos, &punkCtrl );
				if( !punkCtrl )continue;
				IActiveXCtrlPtr	ptrCtrl( punkCtrl );
				punkCtrl->Release();

				BSTR	bstrName = 0;
				ptrCtrl->GetName( &bstrName );
				CString	strName = bstrName;
				::SysFreeString( bstrName );
				str.Format( "Name=%s", (const char*)strName );
				strs.Add( str );
				

				BSTR	bstr = 0;
				ptrCtrl->GetProgID( &bstr );
				CString	strProgID = bstr;
				::SysFreeString( bstr );
				str.Format( "ProgID=%s", (const char*)strProgID );
				strs.Add( str );

				RECT	rect;
				ptrCtrl->GetRect( &rect );
				str.Format( "Rect=%d, %d, %d, %d", rect.left, rect.top, rect.right, rect.bottom );
				strs.Add( str );


				DWORD	dwStyle = 0;
				ptrCtrl->GetStyle( &dwStyle );
				str.Format( "Style=%0Xh", dwStyle );
				strs.Add( str );
					 
				UINT	nID = 0;
				ptrCtrl->GetControlID( &nID );
				str.Format( "ID=%d", nID );
				strs.Add( str );

				byte	*pdata = 0;
				DWORD	dwDataSize = 0;
				ptrCtrl->GetObjectData( 0, &dwDataSize );
				pdata = new byte[dwDataSize];
				ptrCtrl->GetObjectData( &pdata, &dwDataSize );
				ptrCtrl->SetObjectData( pdata, dwDataSize );

				char	*psz = new char[20+dwDataSize*3];
				sprintf( psz, "Data=%d", dwDataSize );
				char	*p = psz+strlen( psz );
				for( int n = 0; n < dwDataSize; n++, p+=3 )
					::sprintf( p, " %02x", pdata[n] );
				strs.Add( psz );
				delete pdata;
				delete psz;

				strs.Add( "" );	
			}

			strs.Add( "" );
			strs.Add( c_szScript );

			IUnknown	*punkScript = 0;
			ptrAX->GetScript( &punkScript );
			if( !punkScript )return false;
			IScriptDataObjectPtr	ptrS( punkScript );
			punkScript->Release();
			BSTR	bstrScript  = 0;
			ptrS->GetFullText( &bstrScript );
			str = bstrScript;
			::SysFreeString( bstrScript );
			strs.Add( str );

			strs.WriteFile( pszFileName );
		}
		else
			if(CheckInterface(ptrObject, IID_ISerializableObject))
			{
				CFile	file( pszFileName, CFile::modeCreate|CFile::modeWrite );
				sptrISerializableObject	sptrO( ptrObject );
				{
					SerializeParams	params;
					ZeroMemory( &params, sizeof( params ) );

					CArchive		ar( &file, CArchive::store );
					CArchiveStream	ars( &ar );
					sptrO->Store( &ars, &params );
				}
			}
		INamedDataObject2Ptr	ptrNamed( ptrObject );
		ptrNamed->SetModifiedFlag( false );
	}
	catch(CException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	return true;
}

bool CAXFileFilter::BeforeReadFile()
{
	return CFileFilterBase::BeforeReadFile();
}

bool CAXFileFilter::AfterReadFile()
{
	return CFileFilterBase::AfterReadFile();
}


/*

*/