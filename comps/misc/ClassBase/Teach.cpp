#include "stdafx.h"
#include "Teach.h"
#include "misc_str.h"
#include "statistics.h"
#include "core5.h"


/**************************************************************************************************/
_ainfo_base::arg CActionTeachObjectAllInfo::s_pargs[] =
{
	{"ClassFile", szArgumentTypeString, "", true, false },
	{ "ObjectName", szArgumentTypeString, "", true, false },
	{0, 0, 0, false, false },
};

static _bstr_t MakeClassifierName(_bstr_t bstrClassFile)
{
	CString tmp = short_filename( (char *)bstrClassFile );
	CString strClassFile = full_classifiername( tmp.GetBuffer( tmp.GetLength() ) );
	tmp.ReleaseBuffer();
	bstrClassFile = strClassFile;
	return bstrClassFile;
}

HRESULT CActionTeachObjectAll::DoInvoke()
{
	_bstr_t bstrClassFile = GetArgString( "ClassFile" );
	bstrClassFile = MakeClassifierName(bstrClassFile);
	if( !bstrClassFile.length() )
		bstrClassFile = ::GetValueString( ::GetAppUnknown(), "\\Classes", "ClassFile", "" );
	CString strProgID = _get_value_string( (char*)bstrClassFile, ID_SECT_GENERAL, ID_ENTR_CLASSIFY_PROGID );
	if( strProgID.IsEmpty() )
		return S_FALSE;
	IUnknownPtr sptrUnk( strProgID, 0, CLSCTX_INPROC_SERVER );
	if( sptrUnk == 0 )
		return S_FALSE;
	IClassifyProxy2Ptr sptrProxy = sptrUnk;
	if( sptrProxy == 0 )
		return S_FALSE;
	if( FAILED( sptrProxy->Load( bstrClassFile ) ) )
		return S_FALSE;

	_bstr_t bstrObjectName = GetArgString( "ObjectName" );
	IUnknown *punkObjList = 0;

	IApplicationPtr app(m_ptrTarget);
	IUnknown* pDoc =0;
	app->GetActiveDocument(&pDoc);
	if(pDoc==0) return S_FALSE;

	if( bstrObjectName.length() )
		punkObjList = ::GetObjectByName( pDoc, bstrObjectName, 0 );

	if( !punkObjList )
		punkObjList = ::GetActiveObjectFromDocument( pDoc, szTypeObjectList );

	if( !punkObjList )
		punkObjList = ::GetActiveObjectFromDocument( pDoc, szTypeStatTable );

	pDoc->Release();

	if( !punkObjList )
		return S_FALSE;


	IClassifyProxy3Ptr proxy3(sptrProxy);
	if( proxy3->Teach(punkObjList) != S_OK )
		return S_FALSE;

	if( sptrProxy->Store( bstrClassFile ) != S_OK )
		return S_FALSE;
	return S_OK;
}
