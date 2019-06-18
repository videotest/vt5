#ifndef __obj_types_h__
#define __obj_types_h__

#include "afxcoll.h"

CString GetViewNameByProgID( CString strViewProgID, bool bUserName );
CString GetViewProgIDByName( CString strViewName );
bool GetAvailableViewProgID( CStringArray& arViewList );
bool GetAllObjectTypes( CStringArray& arTypes, CStringArray& arUserNames );

inline CString GetViewNameByProgID( CString strViewProgID, bool bUserName )
{
	try
	{
		CString strResult;
		if( strViewProgID.IsEmpty() )
			return strResult;

		BSTR bstrProgID = strViewProgID.AllocSysString();
		IViewPtr sptrV( bstrProgID, 0, CLSCTX_INPROC_SERVER );
		::SysFreeString( bstrProgID );

		if( sptrV == NULL )
			return strResult;

		BSTR bstrName = 0;
		INamedObject2Ptr	sptrNO(sptrV);
		if( sptrNO == NULL )
			return strResult;

		if( bUserName )
			sptrNO->GetUserName(&bstrName);
		else
			sptrNO->GetName(&bstrName);

		strResult = bstrName;
		::SysFreeString(bstrName);

		return strResult;
	}
	catch(...)
	{
		return "";
	}

	return "";
}

////////////////////////////////////////////////////////////////////////////
inline CString GetViewProgIDByName( CString strViewName )
{
	CString strResult;
	CStringArray arrViewProgID;
	
	if( !GetAvailableViewProgID( arrViewProgID ) )
		return strResult;

	for( int i=0;i<arrViewProgID.GetSize();i++ )
	{
		if( GetViewNameByProgID( arrViewProgID[i], true ) == strViewName )
			strResult = arrViewProgID[i];

		if( GetViewNameByProgID( arrViewProgID[i], false ) == strViewName )
			strResult = arrViewProgID[i];
	}

	arrViewProgID.RemoveAll();

	return strResult;
}

////////////////////////////////////////////////////////////////////////////
inline DWORD GetViewMatchType( CString strViewProgID, CString strObjectType )
{
	DWORD dwMatch = mvNone;
	try
	{
		if( strViewProgID.IsEmpty() )
			return mvNone;

		BSTR bstrProgID = strViewProgID.AllocSysString();
		IViewPtr sptrV(bstrProgID, 0, CLSCTX_INPROC_SERVER);
		::SysFreeString(bstrProgID);

		if( sptrV == NULL )
			return mvNone;
		
		sptrV->GetMatchType( _bstr_t( (LPCSTR)strObjectType ), &dwMatch );
	}
	catch(...)
	{
		return mvNone;
	}


	return dwMatch;
}

////////////////////////////////////////////////////////////////////////////
inline bool GetAvailableViewProgID( CStringArray& arViewList )
{
	sptrIApplication sptrApp( ::GetAppUnknown() );
	if( sptrApp == NULL )
		return false;

	long pos = 0;
	sptrApp->GetFirstDocTemplPosition(&pos);
	while(pos)
	{
		BSTR bstrName = 0;
		sptrApp->GetNextDocTempl(&pos, &bstrName, 0);
		CString strTempl(bstrName);
		::SysFreeString(bstrName);
		
		//if(punkObj)
		{
			sptrApp->GetFirstDocTemplPosition(&pos);

			while(pos)
			{

				sptrApp->GetNextDocTempl(&pos, &bstrName, 0);
				strTempl = bstrName;
				::SysFreeString(bstrName);				
				
			
				CCompRegistrator	rv( strTempl+"\\"+szPluginView );
				long nComps = rv.GetRegistredComponentCount();
				for(long i = 0; i < nComps; i++)
				{
					CString strProgID = rv.GetComponentName(i);
					arViewList.Add( strProgID );
					/*
					try
					{
						if( !strProgID.IsEmpty() )							
						{
							BSTR bstrProgID = strProgID.AllocSysString();

							IViewPtr sptrV(bstrProgID, 0, CLSCTX_INPROC_SERVER);
							::SysFreeString(bstrProgID);						

							if( sptrV != NULL )
								arViewList.Add( strProgID );
						}						
					}
					catch(...)
					{
						
					}
					*/
				}
			}
		}
	}

	return true;	
}

////////////////////////////////////////////////////////////////////////////
inline bool GetAllObjectTypes( CStringArray& arTypes, CStringArray& arUserNames )
{
	sptrIApplication spApp( ::GetAppUnknown() );
	if( spApp == NULL )
		return false;
	
	
	sptrIDataTypeInfoManager	sptrT( spApp );
	if( sptrT == NULL )
		return false;

	long	nTypesCount = 0;
	sptrT->GetTypesCount( &nTypesCount );
	for( long nType = 0; nType < nTypesCount; nType++ )
	{

		INamedObject2Ptr	ptrNamed;

		{
			INamedDataInfo	*pTypeInfo = 0;
			sptrT->GetTypeInfo( nType, &pTypeInfo );
			BSTR	bstrC = 0;
			pTypeInfo->GetContainerType( &bstrC );
			CString	strCType = bstrC;
			bool	bEmpty = strCType.GetLength()==0;
			::SysFreeString( bstrC );

			ptrNamed = pTypeInfo;

			pTypeInfo->Release();	pTypeInfo = 0;			

			if( !bEmpty )continue;			
		}

		if( ptrNamed == 0 ) continue;

		BSTR	bstr = 0;

		ptrNamed->GetName( &bstr );
		arTypes.Add( bstr );
		::SysFreeString( bstr );	bstr = 0;

		ptrNamed->GetUserName( &bstr );
		arUserNames.Add( bstr );
		::SysFreeString( bstr );	bstr = 0;		
	}

	return true;

}


inline CString GetObjTypeByID( CString s )
{
	CString strRes;

	sptrIApplication sptrApp( ::GetAppUnknown() );
	if( sptrApp == NULL )
		return strRes;

	long pos = 0;
	sptrApp->GetFirstDocTemplPosition(&pos);
	while(pos)
	{
		BSTR bstrName = 0;
		sptrApp->GetNextDocTempl(&pos, &bstrName, 0);
		CString strTempl(bstrName);
		::SysFreeString(bstrName);
		
		sptrApp->GetFirstDocTemplPosition(&pos);

		while(pos)
		{
			sptrApp->GetNextDocTempl(&pos, &bstrName, 0);
			strTempl = bstrName;
			::SysFreeString(bstrName);				
							
			CCompRegistrator	rv( strTempl+"\\"+szPluginView );
			long nComps = rv.GetRegistredComponentCount();
			for(long i = 0; i < nComps; i++)
			{
				CString strProgID = rv.GetComponentName(i);
				if(strProgID==s)
					strRes=strTempl;
			}
		}
	}
	return strRes;	
}

#endif//__obj_types_h__