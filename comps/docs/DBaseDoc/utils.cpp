#include "stdafx.h"
#include "DBaseDoc.h"
#include "ErrorDlg.h"
#include "constant.h"
#include "thumbnail.h"
#include "types.h"
#include "ErrorDlg.h"
#include "dbmacros.h"
#include <odbcinst.h>

/////////////////////////////////////////////////////////////////////////////
bool GetDBaseDocument( sptrIDBaseDocument& spIDBDoc )
{
	ASSERT(spIDBDoc == NULL );

	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA == NULL )
		return false;
	
	IUnknown* punk = NULL;
	sptrA->GetActiveDocument( &punk );
	if( punk == NULL )
		return false;

	spIDBDoc = punk;

	punk->Release();
	
	if( spIDBDoc != NULL )
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////
CString GetDBaseConfigtDir()
{
	CString	strPathDef = ::MakeAppPathName( "DBaseConfig" )+"\\";
	CString strPath = ::_GetValueString( GetAppUnknown(), "\\Paths", "DBaseConfig", 
						(const char*)strPathDef );
	
	if( strPath.GetLength() > 1 )
	{
		CString strDir = strPath.Left( strPath.GetLength() - 1 );
		WriteDirectory( strDir );
	}	

	return strPath;
}

/////////////////////////////////////////////////////////////////////////////
BOOL WriteDirectory(CString dd)
{


	HANDLE		fFile;					// File Handle
	WIN32_FIND_DATA fileinfo;			// File Information Structure
	CStringArray	m_arr;				// CString Array to hold Directory Structures
	BOOL tt;							// BOOL used to test if Create Directory was successful
	int x1 = 0;							// Counter
	CString tem = "";					// Temporary CString Object

	// Before we go to a lot of work.  
	// Does the file exist

	fFile = FindFirstFile(dd,&fileinfo);

	// if the file exists and it is a directory
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		//  Directory Exists close file and return
		FindClose(fFile);
		return TRUE;
	}

	m_arr.RemoveAll();					// Not really necessary - Just habit
	for(x1=0;x1<dd.GetLength();x1++)	// Parse the supplied CString Directory String
	{									
		if(dd.GetAt(x1) != '\\')		// if the Charachter is not a \ 
			tem += dd.GetAt(x1);		// else add character to Temp String
		else
		{
			m_arr.Add(tem);				// if the Character is a \ Add the Temp String to the CString Array
			tem += "\\";				// Now add the \ to the temp string
		}
		if(x1 == dd.GetLength()-1)		// If we reached the end of the file add the remaining string
			m_arr.Add(tem);
	}


	// Close the file
	FindClose(fFile);
	
	// Now lets cycle through the String Array and create each directory in turn
	for(x1 = 1;x1<m_arr.GetSize();x1++)
	{
		tem = m_arr.GetAt(x1);
		tt = CreateDirectory(tem,NULL);

		// If the Directory exists it will return a false
		if(tt)
			SetFileAttributes(tem,FILE_ATTRIBUTE_NORMAL);
		// If we were successful we set the attributes to normal
	}
	m_arr.RemoveAll();
	//  Now lets see if the directory was successfully created
	fFile = FindFirstFile(dd,&fileinfo);

	// if the file exists and it is a directory
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		//  Directory Exists close file and return
		FindClose(fFile);
		return TRUE;
	}
	else
	{
		FindClose(fFile);
		return FALSE;
	}
}
/*
/////////////////////////////////////////////////////////////////////////////
bool GetObjectTypesSupportByView( IUnknown* pUnkView, CArray<CString, CString>& arrTypes )
{	

	sptrIView spView( pUnkView );
	if( spView == NULL )
		return false;

	sptrIDataContext2 spDC2( pUnkView );
	if( spDC2 == NULL )
		return false;

	int nObjectTypeCount = 0;
	spDC2->GetObjectTypeCount( &nObjectTypeCount );
	for( int nCurType=0;nCurType<nObjectTypeCount;nCurType++ )
	{
		BSTR bstrType;
		spDC2->GetObjectType( nCurType, &bstrType );

		DWORD dwMatch;
		spView->GetMatchType( bstrType, &dwMatch );			

		if( (MatchView)dwMatch == mvFull )
		{
			arrTypes.Add( bstrType );
		}

		::SysFreeString( bstrType );
	}

	return true;

}
*/

/////////////////////////////////////////////////////////////////////////////
bool IsOwnDataObject( _variant_t var )
{	
	return true;

	if( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH )
	{
		IUnknown	*punkObj = var.punkVal;
		if( punkObj == NULL )
			return false;

		if( CheckInterface(punkObj, IID_IActiveXForm) )		
			return true;		
		else		
		if( CheckInterface(punkObj, IID_IActiveXCtrl) )			
			return true;
		else
		if( CheckInterface(punkObj, IID_IQueryObject) )			
				return true;
		if( CheckInterface(punkObj, IID_IReportForm) )			
				return true;
		if( CheckInterface(punkObj, IID_IReportCtrl) )			
				return true;

		return false;
	}
	
	//for numbers, strings ... etc
	return true;
}

/*
/////////////////////////////////////////////////////////////////////////////
bool GenerateTumbnailFromView( IUnknown* pUnkView, sptrIDBaseDocument spDBDoc )
{

	sptrIDataContext2 spDC2( pUnkView );
	if( spDC2 == NULL )
		return false;

	if( spDBDoc == NULL )
		return false;

	IUnknown* pUnkQuery = NULL;
	spDBDoc->GetActiveQueryUnknown( &pUnkQuery );
	if( pUnkQuery == NULL )
		return false;

	sptrISelectQuery spQuery( pUnkQuery );
	pUnkQuery->Release();
	if( spQuery == NULL )
		return false;


	CArray<CString, CString> arrTypes;
	if( ::GetObjectTypesSupportByView( pUnkView, arrTypes ) )
	{		

		for( int i=0;i<arrTypes.GetSize();i++ )
		{
			_bstr_t bstrType( arrTypes[i] );

			IUnknown* pUnkObject = NULL;
			spDC2->GetActiveObject( bstrType, &pUnkObject );
			if( pUnkObject )
			{
				if( !IsOwnDataObject( pUnkObject ) )
				{
					sptrINamedObject2 spNO2( pUnkObject );
					if( spNO2 )
					{
						BSTR bstrName;
						spNO2->GetName( &bstrName );
						CString strName = bstrName;
						::SysFreeString( bstrName );
						//TRACE("\n"+strName);
						CString strTable, strField;
						int nPoint = strName.Find( "." );
						if( nPoint != -1 )
						{
							strTable = strName.Left( nPoint );
							strField = strName.Right( strName.GetLength() - nPoint - 1 );

							if( !strTable.IsEmpty() && !strField.IsEmpty() )
							{
								spQuery->GenerateTumbnail( pUnkView ,
										_bstr_t( (LPCTSTR)strTable ),
										_bstr_t( (LPCTSTR)strField )
												);
							}
						}								
					}
				}
				
				pUnkObject->Release();
			}
		}
	}
	
	arrTypes.RemoveAll();

	return true;
}
*/
/////////////////////////////////////////////////////////////////////////////
FieldType GetFieldType( sptrIDBaseDocument spDBDoc, CString strTable, CString strField )
{

	IDBaseStructPtr ptrS( spDBDoc );
	if( ptrS == NULL )
		return ftNotSupported;
	

	short nFieldType = -1;		

	if(  S_OK != ptrS->GetFieldInfo(	_bstr_t( strTable ), _bstr_t( strField ), 
										0, &nFieldType,
										0, 0, 0, 0, 0, 0 )
		) 
		return ftNotSupported;


	return (FieldType)nFieldType;
}

/////////////////////////////////////////////////////////////////////////////
CString GetFieldCaption( sptrIDBaseDocument spDBDoc, CString strTable, CString strField )
{
	IDBaseStructPtr ptrS( spDBDoc );
	if( ptrS == NULL )
		return "";
	

	BSTR bstrCaption = 0;

	if(  S_OK != ptrS->GetFieldInfo(	_bstr_t( strTable ), _bstr_t( strField ), 
										&bstrCaption, 0,
										0, 0, 0, 0, 0, 0 )
		) 
		return "";


	CString strCaption = bstrCaption;

	if( bstrCaption )
		::SysFreeString( bstrCaption );

	return strCaption;

}

/////////////////////////////////////////////////////////////////////////////
CWnd* GetMainFrameWnd()
{
	sptrIApplication spApp( ::GetAppUnknown() );
	if( spApp )
	{
		HWND hWnd;
		spApp->GetMainWindowHandle( &hWnd );
		if( ::IsWindow( hWnd ) )
		{
			return CWnd::FromHandle( hWnd );
		}

	}

	return NULL;	
}


/////////////////////////////////////////////////////////////////////////////
bool IsAvailableFieldInQuery( IUnknown* punkQuery, CString strTable, CString strField )
{
	sptrIQueryObject spQuery( punkQuery );

	if( spQuery == NULL )
		return false;

		
	int nFieldsCount = 0;
	spQuery->GetFieldsCount( &nFieldsCount );
	for( int i=0;i<nFieldsCount;i++ )
	{
		BSTR bstrTable = 0;
		BSTR bstrField = 0;
		if( S_OK != spQuery->GetField( i, 0, &bstrTable, &bstrField ) )
			continue;

		bool bFound = false;
		if( bstrTable == strTable && bstrField == strField )
			bFound = true;

		if( bstrTable )
			::SysFreeString( bstrTable );	bstrTable = 0;

		if( bstrField )
			::SysFreeString( bstrField );	bstrField = 0;

		if( bFound )
			return true;
		
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
bool GetTableFieldFromString(CString strSection, CString& strTable, CString& strField )
{
	bool bWorkWithField = true;
	bool bEnough = false;

	if( strSection.GetLength() < 3 )
		return false;

	for( int i=strSection.GetLength()-1;i>=0;i-- )
	{
		if( bEnough )
			continue;
		if( strSection[i] == '.')
		{
			bWorkWithField = false;
			continue;
		}

		if( strSection[i] == '\\')
		{
			bEnough = true;
			continue;
		}

		 if( bWorkWithField )
		 {
			strField += strSection[i];
		 }
		 else
		 {
			strTable += strSection[i];
		 }
	}

	strTable.MakeReverse();
	strField.MakeReverse();

	if( strTable.IsEmpty() || strField.IsEmpty() )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
IUnknown* GetFilterPropPage()
{
	CString strInsPage;
					
	IUnknown* punkPage = 0;	
	GetPropertyPageByProgID( szFilterPropPageProgID, &punkPage, 0 );
	
	return punkPage;
}

/////////////////////////////////////////////////////////////////////////////
bool RefreshFilterPropPage()
{
	IUnknown* punkFilterPropPage = NULL;
	punkFilterPropPage = ::GetFilterPropPage();
	if( punkFilterPropPage == NULL )
		return false;

	
	sptrIDBFilterPage  sptrPP( punkFilterPropPage );
	punkFilterPropPage->Release();

	if( sptrPP ==NULL )
		return false;

	sptrPP->BuildAppearance();	

	return true;

}

/////////////////////////////////////////////////////////////////////////////
void GetTableFieldFromPath( CString strPath, CString& strTable, CString& strField  )
{
	bool bWorkWithField = true;
	bool bEnough = false;

	for( int i=strPath.GetLength()-1;i>=0;i-- )
	{
		if( bEnough )
			continue;
		if( strPath[i] == '.')
		{
			bWorkWithField = false;
			continue;
		}

		if( strPath[i] == '\\')
		{
			bEnough = true;
			continue;
		}

		 if( bWorkWithField )
		 {
			strField += strPath[i];
		 }
		 else
		 {
			strTable += strPath[i];
		 }
	}

	strTable.MakeReverse();
	strField.MakeReverse();
}

/////////////////////////////////////////////////////////////////////////////
CString GenerateFieldPath( CString strTable, CString strField, bool bAddPath )
{
	CString strResult;
	strResult.Format( "%s.%s", (LPCSTR)strTable, (LPCSTR)strField );
	if( bAddPath )
	{
		CString strTemp = CString(SECTION_DBASEFIELDS) + CString("\\");
		strTemp += strResult;		
		strResult = strTemp;
	}

	return strResult;
}

/////////////////////////////////////////////////////////////////////////////
IUnknown* GetObjectFromContextByPos( IUnknown* punkDataContext, _bstr_t bstrType, int nPos )
{
	if( punkDataContext == NULL )
		return NULL;

	sptrIDataContext2 spDC( punkDataContext );
	if( spDC == NULL )
		return NULL;

	long nObjectCount = -1;
	spDC->GetObjectCount( bstrType, &nObjectCount );
	if( nPos < 0 || nPos >= nObjectCount )
		return false;

	LONG_PTR lPos = NULL;
	int nCounter = 0;
	
	spDC->GetFirstObjectPos( bstrType, &lPos );
	while( lPos )
	{
		IUnknown* punkObject = NULL;
		spDC->GetNextObject( bstrType, &lPos, &punkObject );
		

		if( nCounter == nPos )
			return punkObject;

		if( punkObject )
			punkObject->Release();

		nCounter++;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

bool DrawThumbnail( IUnknown* punkObject, CDC* pDC, CRect rcTarget, CString strComment )
{
	if( punkObject == NULL )
		return false;

	sptrIThumbnailManager spThumbMan( GetAppUnknown() );
	if( spThumbMan == NULL )
		return false;


	IUnknown* punkRenderObject = NULL;

	spThumbMan->GetRenderObject( punkObject, &punkRenderObject );
	if( punkRenderObject == NULL )
		return false;
			   

	sptrIRenderObject spRenderObject( punkRenderObject );
	punkRenderObject->Release();

	if( spRenderObject == NULL ) 
		return false;


	BYTE* pbi = NULL;
	
	short nSupported;
	spRenderObject->GenerateThumbnail( punkObject, 24, 0, 0, 
				CSize(rcTarget.Width(), rcTarget.Height() ), 
				&nSupported, &pbi );

	if( pbi )
	{		
		pDC->FillRect( &rcTarget, &CBrush(RGB(255,255,255)) );
		::ThumbnailDraw( pbi, pDC, 
				CRect(	rcTarget.left, rcTarget.top,
						rcTarget.right, rcTarget.bottom
						), 0
					);
		::ThumbnailFree( pbi );

		pDC->TextOut( rcTarget.left, rcTarget.top, strComment );

		pbi = NULL;
	}

	return true;

}
/////////////////////////////////////////////////////////////////////////////
CString GetFieldVTObjectType( IUnknown* punkNamedData, CString strTable, CString strField )
{
	CString strEntry;
	strEntry.Format( "%s.%s", (LPCTSTR)strTable, (LPCTSTR)strField );
	CString strVTObjectType;

	strTable.MakeLower();
	strField.MakeLower();
	
	strVTObjectType = ::_GetValueString( punkNamedData, SECTION_FIELD_VTOBJECT_TYPE, strEntry, "" );

	return strVTObjectType;
}

//get integer value from NamedData
long _GetValueInt( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lDefault )
{
	try
	{
		_variant_t	var( lDefault );
		var = ::_GetValue( punkDoc, pszSection, pszEntry, var );

		var.ChangeType( VT_I4 );

		return var.lVal;
	}
	catch( ... )
	{
		return lDefault;
	}
}

//get string value from NamedData
CString _GetValueString( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszDefault )
{
	try
	{
		_variant_t	var( pszDefault );
		var = ::_GetValue( punkDoc, pszSection, pszEntry, var );

		
		var.ChangeType( VT_BSTR );

		return CString( var.bstrVal );
	}
	catch( ... )
	{
		return CString( pszDefault );
	}
}

//get any value from NamedData
_variant_t _GetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault )
{
	_variant_t	var = varDefault;
	if( !CheckInterface( punkDoc, IID_INamedData ) )
		return var;	

	sptrINamedData	sptrData( punkDoc );
	CString strPath;
	strPath.Format( "%s\\%s", (LPCTSTR)pszSection, (LPCTSTR)pszEntry );
	_bstr_t	bstrPath( strPath );
	sptrData->GetValue( bstrPath, &var );	
	return var;
}




//set integer value to NamedData
void _SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lValue )
{
	_variant_t	var( lValue );
	::_SetValue( punkDoc, pszSection, pszEntry, var );
}

//set string value to NamedData
void _SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszValue )
{
	_variant_t	var( pszValue );
	::_SetValue( punkDoc, pszSection, pszEntry, var );
}

//set any value to NamedData
void _SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault )
{
	_variant_t	var = varDefault;
	if( !CheckInterface( punkDoc, IID_INamedData ) )
		return;

	sptrINamedData	sptrData( punkDoc );

	CString strPath;
	strPath.Format( "%s\\%s", (LPCTSTR)pszSection, (LPCTSTR)pszEntry );
	_bstr_t	bstrPath( strPath );
	sptrData->SetValue( bstrPath, var );

}


void dump_com_error( _com_error &e )
{
	CErrorsDialog dlg;
	dlg.InitFromComError( e );
	dlg.DoModal();

	/*
	CString strError;
	char szErrorBuf[1024];
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	
	sprintf(szErrorBuf, "Error\n");
	strError += szErrorBuf;
	sprintf(szErrorBuf, "Code = %08lx\n", e.Error());
	strError += szErrorBuf;
	sprintf(szErrorBuf, "Code meaning = %s", e.ErrorMessage());
	strError += szErrorBuf;
	sprintf(szErrorBuf, "Source = %s\n", (LPCSTR) bstrSource);
	strError += szErrorBuf;
	sprintf(szErrorBuf, "Description = %s\n", (LPCSTR) bstrDescription);
	strError += szErrorBuf;
	*/

	
	/*
	sptrIDBaseDocument spIDBDoc;

	if( GetDBaseDocument( spIDBDoc ) )
	{

		IUnknown* punkConn = NULL;
		spIDBDoc->GetConnection( &punkConn );
		
		_ConnectionPtr conn = punkConn;
		if( punkConn )
		{
			punkConn->Release();		
		}

		if( conn )
		{
			ErrorsPtr Errors = conn->Errors;
			if( Errors )
			{
				CString strErrors;
				CString strAddError;

				int nCount = Errors->GetCount();
				for( int i=0;i<nCount;i++ )
				{
					ErrorPtr error = Errors->GetItem( _variant_t( (long)i ) );
					if( error == NULL )
						continue;

					strAddError.Format( "\
						Description - %s,\
						Number - %d,\
						Source - %s,\
						HelpFile - %s,\
						HelpContext - %d,\
						SQLState - %s,\
						NativeError - %d\
						",
						(LPCSTR)error->Description,
						(int)error->Number,
						(LPCSTR)error->Source,
						(LPCSTR)error->HelpFile,
						(int)error->HelpContext,
						(LPCSTR)error->SQLState,
						(int)error->NativeError
						);					

					strErrors += strAddError;
					strErrors += "\n";			
				}

				strError += "Extended:\n";
				strError += strErrors;
			}
		}
	}
	*/

	//AfxMessageBox( strError, MB_OK | MB_ICONSTOP );
	
}



////////////////////////////////////////////////////////////////////////////////////////////
bool GetTableFieldFromSection(CString strSection, CString& strTable, CString& strField, bool bTestFullPath )
{
	if( bTestFullPath )
	{
		CString strDBasefields = SECTION_DBASEFIELDS;
		strDBasefields.Delete( 0 );// kill '//'

		if( strSection.Find( strDBasefields ) == -1 )
			return false;
	}


	bool bWorkWithField = true;
	bool bEnough = false;

	for( int i=strSection.GetLength()-1;i>=0;i-- )
	{
		if( bEnough )
			continue;
		if( strSection[i] == '.')
		{
			bWorkWithField = false;
			continue;
		}

		if( strSection[i] == '\\')
		{
			bEnough = true;
			continue;
		}

		 if( bWorkWithField )
		 {
			strField += strSection[i];
		 }
		 else
		 {
			strTable += strSection[i];
		 }
	}

	strTable.MakeReverse();
	strField.MakeReverse();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool IsAccessDBase( IUnknown* punkDoc )
{
	IDBConnectionPtr ptrDB( punkDoc );
	if( ptrDB == 0 )
	{
		ASSERT( false );
		return false;
	}

	BSTR bstr = 0;
	ptrDB->GetMDBFileName( &bstr );

	CString str = bstr;

	if( bstr )
		::SysFreeString( bstr );

	if( !str.IsEmpty() )
		return true;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool FillTableInfoFromDoc( IUnknown* punkDoc, TABLEINFO_ARR_PTR pArr )
{
	if( !pArr )
		return false;

	IDBaseStructPtr	ptrDBS( punkDoc );
	if( ptrDBS == 0 )
		return false;

	int nCount = 0;
	ptrDBS->GetTableCount( &nCount );
	for( int i=0;i<nCount;i++ )
	{		
		BSTR bstr = 0;
		if( S_OK != ptrDBS->GetTableName( i, &bstr ))
			continue;
		_bstr_t bstrTable( bstr );

		if( bstr )
			::SysFreeString( bstr );	bstr = 0;

		_CTableInfo* pti = new _CTableInfo( CString( (LPCSTR)bstrTable ) );
		pArr->Add( pti );

		int nFieldCount = 0;
		if( S_OK != ptrDBS->GetFieldCount( bstrTable, &nFieldCount ) )
			continue;
		
		for( int j=0;j<nFieldCount;j++ )
		{
			if( S_OK != ptrDBS->GetFieldName( bstrTable, j, &bstr ) )
				continue;
			_bstr_t bstrField = bstr;

			if( bstr )
				::SysFreeString( bstr );	bstr = 0;


			BSTR bstrUserName			= 0; 
			short nFieldType			= 0; 
			BSTR bstrPrimaryTable		= 0; 
			BSTR bstrPrimaryFieldName	= 0; 
			BOOL bRequiredValue			= 0; 
			BOOL bDefaultValue			= 0; 
			BSTR bstrDefaultValue		= 0; 
			BSTR bstrVTObjectType		= 0; 

			if( S_OK != ptrDBS->GetFieldInfo( bstrTable, bstrField, 
								&bstrUserName, &nFieldType,
								&bstrPrimaryTable, &bstrPrimaryFieldName,
								&bRequiredValue, &bDefaultValue,
								&bstrDefaultValue, &bstrVTObjectType
				) )
				continue;


			_CFieldInfo* pfi = new _CFieldInfo;
			pti->AddFieldInfo( pfi );

			pfi->m_strFieldName			= (LPSTR)bstrField;
			pfi->m_strUserName			= bstrUserName;
			pfi->m_FieldType			= (FieldType)nFieldType;
			
			pfi->m_strPrimaryTable		= bstrPrimaryTable;
			pfi->m_strPrimaryFieldName	= bstrPrimaryFieldName;

			pfi->m_bRequiredValue		= bRequiredValue;
			pfi->m_bDefaultValue		= bDefaultValue;
			pfi->m_strDefaultValue		= bstrDefaultValue;
			pfi->m_strVTObjectType		= bstrVTObjectType;
			pfi->m_bCanEditStructure	= FALSE;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
CString GenerateUniqFieldName( IUnknown* punkDoc, CString strTable, FieldType ft )
{
	
	CString strPrefix;

	if( ft == ftDigit )
		strPrefix.LoadString( IDS_NEW_FIELD_DIGIT );
	else if( ft == ftString )
		strPrefix.LoadString( IDS_NEW_FIELD_STRING );
	else if( ft == ftDateTime )
		strPrefix.LoadString( IDS_NEW_FIELD_DATE_TIME );
	else if( ft == ftVTObject )
	{
		strPrefix.LoadString( IDS_NEW_FIELD_BINARY );
	}
	else
		return "";

	IDBaseStructPtr ptrDBS( punkDoc );
	if( ptrDBS == 0 )
		return strPrefix;

	CStringArray strFields;

	
	_bstr_t bstrTable( (LPCSTR)strTable );

	int nCount = 0;
	ptrDBS->GetFieldCount( bstrTable, &nCount );
	for( int i=0;i<nCount;i++ )
	{
		BSTR bstrField = 0;
		if( S_OK == ptrDBS->GetFieldName( bstrTable, i, &bstrField ) )
		{
			strFields.Add( bstrField );

			if( bstrField )
				::SysFreeString( bstrField );	bstrField = 0;
		}
	}

	CString strFieldName;
	int nIndex = 1;	

	while( true )	
	{
		strFieldName.Format( "%s%d", strPrefix, nIndex );

		CString str1 = strFieldName;		str1.MakeLower();

		bool bFound = false;
		for( int i=0;i<strFields.GetSize();i++ )
		{
			CString str2 = strFields[i];	str2.MakeLower();
			if( str1 == str2 )
			{
				bFound = true;
				break;
			}
		}

		if( !bFound )
			return strFieldName;
		else
			nIndex++;
	}

}


////////////////////////////////////////////////////////////////////////////////////////////
bool ActivateDBObjectInContext( CString strTable, CString strField, IUnknown* punkContext )
{
	IDataContext2Ptr ptrDC = punkContext;
	if( ptrDC == 0 )
		return false;

	IDBaseStructPtr ptrDB( ptrDC );
	if( ptrDB == 0 )
	{
		IViewPtr ptrView( ptrDC );
		if( ptrView )
		{
			IUnknown* punkDoc = 0;
			ptrView->GetDocument( &punkDoc );

			if( punkDoc )
			{
				ptrDB = punkDoc;
				punkDoc->Release();	punkDoc = 0;
			}
		}
	}

	if( ptrDB == 0 )
		return false;

	BSTR bstrFieldType = 0;
	if( S_OK == ptrDB->GetFieldInfo( _bstr_t( strTable ), _bstr_t( strField ), 0, 0, 0, 0, 0, 0, 0, &bstrFieldType ) )
	{
		CString strType = bstrFieldType;
		if( !strType.IsEmpty() )
		{
			CString strObjectName;
			strObjectName.Format( "%s.%s", (LPCSTR)strTable, (LPCSTR)strField );
			
			IUnknown* punk = 0;
			punk = ::GetObjectByName( ptrDB, strObjectName, strType );

			if( punk )
			{
				ptrDC->UnselectAll( bstrFieldType );
				ptrDC->SetObjectSelect( punk, 1 );

				punk->Release();				
				
				if( bstrFieldType )
					::SysFreeString( bstrFieldType );	bstrFieldType = 0;

				return true;
			}
		}			
		
		if( bstrFieldType )
			::SysFreeString( bstrFieldType );	bstrFieldType = 0;		
	}

	return false;

}


////////////////////////////////////////////////////////////////////////////////////////////
ADO::CursorLocationEnum GetCursorLocation( CString strSQL )
{
	long nUseServerCursor = ::GetValueInt( GetAppUnknown(), SHELL_DATA_DB_SECTION,
								SHELL_DATA_DB_USE_SERVER_CURSOR, 
								1 );

	strSQL.MakeLower();

	//int nIndex = strSQL.Find( " order " );
	
	if( nUseServerCursor == 0/* || nIndex != -1*/ )
		return ADO::adUseClient;
	else
		return ADO::adUseServer;			

}

////////////////////////////////////////////////////////////////////////////////////////////
bool GetActiveFieldFromActiveView( CString& strTable, CString& strField, CString& strValue )
{
	IApplicationPtr ptrApp( GetAppUnknown() );
	IUnknown* punk = 0;
	ptrApp->GetActiveDocument( &punk );
	if( punk )
	{
		IDocumentSitePtr ptrDoc( punk );
		punk->Release();	punk = 0;

		if( ptrDoc )
		{
			ptrDoc->GetActiveView( &punk );

			if( punk )
			{
				IDBFilterViewPtr ptrView( punk );
				punk->Release();	punk = 0;

				if( ptrView )
				{
					BSTR bstrTable, bstrField, bstrValue;
					bstrTable = bstrField = bstrValue = 0;
					ptrView->GetActiveField( &bstrTable, &bstrField );

					ptrView->GetActiveFieldValue( 0, 0, &bstrValue );					
					

					strTable = bstrTable;
					strField = bstrField;
					strValue = bstrValue;


					if( bstrTable )
						::SysFreeString( bstrTable );	bstrTable = 0;

					if( bstrField )
						::SysFreeString( bstrField );	bstrField = 0;


					if( bstrValue )
						::SysFreeString( bstrValue );	bstrValue = 0;

					return true;
				}
			}
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool SetModifiedFlagToDoc( IUnknown* punkDoc )
{
	IFileDataObject2Ptr ptrFDO2;

	if( punkDoc == 0 )
	{
		IApplicationPtr ptrApp( ::GetAppUnknown() );
		if( ptrApp )
		{
			IUnknown* punk = 0;
			ptrApp->GetActiveDocument( &punk );
			if( punkDoc )
			{
				ptrFDO2 = punk;
				punk->Release();	punk = 0;
			}
		}
	}
	else
		ptrFDO2 = punkDoc;

	if( ptrFDO2 == 0 )
		return false;

	ptrFDO2->SetModifiedFlag( TRUE );


	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool SetModifiedFlagToObj( IUnknown* punkObj )
{
	INamedDataObject2Ptr ptrNDO( punkObj );

	if( ptrNDO == 0 )
		return false;

	ptrNDO->SetModifiedFlag( TRUE );

	//set modified flag to doc
	IUnknown* punkDoc = 0;
	ptrNDO->GetData( &punkDoc );
	if( punkDoc )
	{
		IFileDataObject2Ptr ptrFDO2 = punkDoc;
		punkDoc->Release();	punkDoc = 0;

		if( ptrFDO2 )
		{
			ptrFDO2->SetModifiedFlag( TRUE );
		}
	}

	return true;
}



////////////////////////////////////////////////////////////////////////////////////////////
bool ForceAppIdleUpdate()
{
	IApplicationPtr ptrApp( ::GetAppUnknown() );
	if( ptrApp == 0 )
		return false;

	return ( S_OK == ptrApp->IdleUpdate() );
}

char* szSQLReserved[] = 
{
	"ABSOLUTE",
	"DECIMAL",
	"IS ROWS",
	"ACTION",
	"DECLARE",
	"ISOLATION",
	"SCHEMA",
	"ADD",
	"DEFAULT",
	"JOIN",
	"SCROLL",
	"ALL",
	"DEFERRABLE",
	"KEY",
	"SECOND",
	"ALLOCATE",
	"DEFERRED",
	"LANGUAGE",
	"SECTION",
	"ALTER",
	"DELETE",
	"LAST",
	"SELECT",
	"AND",
	"DESCRIBE",
	"LEADING",
	"SESSION",
	"ANY",
	"DESC",
	"LEFT",
	"SESSION_USER",
	"ARE",
	"DESCRIPTOR",
	"LEVEL",
	"SET",
	"AS",
	"DIAGNOSTICS",
	"LIKE",
	"SIZE",
	"ASC",
	"DISCONNECT",
	"LOCAL",
	"SMALLINT",
	"ASSERTION",
	"DISTINCT",
	"LOWER",
	"SOME",
	"AT",
	"DOMAIN",
	"MATCH",
	"SQL",
	"AUTHORIZATION",
	"DOUBLE",
	"MAX",
	"SQLCODE",
	"AVG",
	"DROP",
	"MIN",
	"SQLERROR",
	"BEGIN",
	"ELSE",
	"MINUTE",
	"SQLSTATE",
	"BETWEEN",
	"END",
	"MODULE",
	"SUBSTRING",
	"BIT",
	"END-EXEC",
	"MONTH",
	"SUM",
	"BIT_LENGTH",
	"ESCAPE",
	"NAMES",
	"SYSTEM_USER",
	"BOTH",
	"EXCEPT",
	"NATIONAL",
	"TABLE",
	"BY",
	"EXCEPTION",
	"NATURAL",
	"TEMPORARY",
	"CASCADE",
	"EXEC",
	"NCHAR",
	"THEN",
	"CASCADED",
	"EXECUTE",
	"NEXT",
	"TIME",
	"CASE",
	"EXISTS",
	"NO",
	"TIMESTAMP",
	"CAST",
	"EXTERNAL",
	"NOT",
	"TIMEZONE_HOUR",
	"CATALOG",
	"EXTRACT",
	"NULL",
	"TIMEZONE_MINUTE",
	"CHAR",
	"FALSE",
	"NULLIF",
	"TO",
	"CHARACTER",
	"FETCH",
	"NUMERIC",
	"TRAILING",
	"CHAR_LENGTH",
	"FIRST",
	"OCTET_LENGTH",
	"TRANSACTION",
	"CHARACTER_LENGTH",
	"FLOAT",
	"OF",
	"TRANSLATE",
	"CHECK",
	"FOR",
	"ON",
	"TRANSLATION",
	"CLOSE",
	"FOREIGN",
	"ONLY",
	"TRIM",
	"COALESCE",
	"FOUND",
	"OPEN",
	"TRUE",
	"COLLATE",
	"FROM",
	"OPTION",
	"UNION",
	"COLLATION",
	"FULL",
	"OR",
	"UNIQUE",
	"COLUMN",
	"GET",
	"ORDER",
	"UNKNOWN",
	"COMMIT",
	"GLOBAL",
	"OUTER",
	"UPDATE",
	"CONNECT",
	"GO",
	"OUTPUT",
	"UPPER",
	"CONNECTION",
	"GOTO",
	"OVERLAPS",
	"USAGE",
	"CONSTRAINT",
	"GRANT",
	"PARTIAL",
	"USER",
	"CONSTRAINTS",
	"GROUP",
	"POSITION",
	"USING",
	"CONTINUE",
	"HAVING",
	"PRECISION",
	"VALUE",
	"CONVERT",
	"HOUR",
	"PREPARE",
	"VALUES",
	"CORRESPONDING",
	"IDENTITY",
	"PRESERVE",
	"VARCHAR",
	"COUNT",
	"IMMEDIATE",
	"PRIMARY",
	"VARYING",
	"CREATE",
	"IN",
	"PRIOR",
	"VIEW",
	"CROSS",
	"INDICATOR",
	"PRIVILEGES",
	"WHEN",
	"CURRENT",
	"INITIALLY",
	"PROCEDURE",
	"WHENEVER",
	"CURRENT_DATE",
	"INNER",
	"PUBLIC",
	"WHERE",
	"CURRENT_TIME",
	"INPUT",
	"READ",
	"WITH",
	"CURRENT_TIMESTAMP",
	"INSENSITIVE",
	"REAL",
	"WORK",
	"CURRENT_USER",
	"INSERT",
	"REFERENCES",
	"WRITE",
	"CURSOR",
	"INT",
	"RELATIVE",
	"YEAR",
	"DATE",
	"INTEGER",
	"RESTRICT",
	"ZONE",
	"DAY",
	"INTERSECT",
	"REVOKE",
	"DEALLOCATE",
	"INTERVAL",
	"RIGHT",
	"DEC",
	"INTO",
	"ROLLBACK",
	"IMAGE",
	0,
};


bool IsSQLReservedWord( const char* psz_word )
{
	int i = 0;
	while( szSQLReserved[i] != 0 )
	{
		if( !_stricmp( psz_word, szSQLReserved[i] ) )
			return true;

		i++;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool SetListoxAutoExtent( CListBox* plistbox )
{
	if( !( plistbox && plistbox->GetSafeHwnd() ) )
		return false;

	CString str;
	CSize   sz;
	int     dx=0;
	CDC*    pDC = plistbox->GetDC();
	if( !pDC )
		return false;

	for( int i=0;i<plistbox->GetCount();i++ )
	{
	   plistbox->GetText( i, str );
	   sz = pDC->GetTextExtent(str);

	   if (sz.cx > dx)
		  dx = sz.cx;
	}
	
	plistbox->ReleaseDC( pDC );

	plistbox->SetHorizontalExtent( dx );

	return true;
}

//get error descr
CString GetWin32ErrorDescr( DWORD dw_error )
{
	LPVOID lpMsgBuf = 0;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		(DWORD)dw_error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);

	CString str_result = _T("");

	if( lpMsgBuf )
	{
		str_result = (LPCTSTR)lpMsgBuf;	
		LocalFree( lpMsgBuf );
	}

	return str_result;
}

//compact
bool CompactMDB( const char* psz_file_name )
{	    
	if( !psz_file_name )	return false;

	char szDriver[] = "Microsoft Access Driver (*.mdb)";

	CString str_connect = "REPAIR_DB=";
	str_connect += "\"";
	str_connect += psz_file_name;
	str_connect += "\"";

	BOOL bres = SQLConfigDataSource( 0, ODBC_ADD_DSN, szDriver, (const char*)str_connect );

	return ( bres == TRUE );	
}

//save DataBase document
bool SaveDBaseDocument( IUnknown* punk_doc )
{
	IDBaseDocumentPtr spDBDoc = punk_doc;
	if( spDBDoc == 0 )	return false;

	HRESULT hr = S_FALSE;
	
	BOOL bReadOnly = FALSE;
	spDBDoc->IsReadOnly( &bReadOnly );
	if( !bReadOnly )
	{
		IDocumentSitePtr ptr_ds( spDBDoc );
		if( ptr_ds )
		{
			BSTR bstr_name = 0;
			ptr_ds->GetPathName( &bstr_name );
			if( bstr_name )
			{
				hr = ptr_ds->SaveDocument( bstr_name );
				::SysFreeString( bstr_name );
			}
		}
	}

	return ( hr == S_OK );
}