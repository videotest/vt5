// OperationCGHBase.cpp: implementation of the COperationBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OperationCGHBase.h"
#include "resource.h"
#include "misc_utils.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COperationBase::~COperationBase()
{

}

IUnknown * COperationBase::GetContextImage(CString cName)
{

	IDataContext3Ptr ptrC = m_punkTarget;
	_bstr_t	bstrType = szTypeImage;

	if( cName.IsEmpty() )
	{
		IUnknown *punkImage = 0;
		ptrC->GetActiveObject( bstrType, &punkImage );
		if( punkImage )
			punkImage->Release();

		return punkImage;
	}

	long lPos = 0;
	ptrC->GetFirstObjectPos( bstrType, &lPos );
	
	while( lPos )
	{
		IUnknown *punkImage = 0;
		ptrC->GetNextObject( bstrType, &lPos, &punkImage );

		if( punkImage )
		{
			if( GetObjectName( punkImage ) == cName )
			{
				if( punkImage )
					punkImage->Release();
				return punkImage;
			}

			punkImage->Release();
		}
	}
	return 0;
}

HRESULT COperationBase::StoreDestImage( CString strName )
{
	IUnknown *punkDoc = 0;
	IViewPtr	sptrV = m_punkTarget;

	sptrV->GetDocument( &punkDoc );
	IDocumentPtr	sptrD = punkDoc;

	if( !punkDoc )
		return false;
	punkDoc->Release(); punkDoc = 0;

	m_sptrImage = GetContextImage( strName );

	if( m_sptrImage == 0 )
		return S_FALSE;
	
	m_sptrPrevImg = m_sptrImage;
	
	IImage3Ptr sptrSrcImage = m_sptrImage;
	sptrSrcImage->GetSize( &m_nImageW, &m_nImageH );
	
	if( !m_nImageW || !m_nImageH )
		return S_FALSE;

	return m_sptrImage != 0 ? S_OK : S_FALSE;
}

HRESULT COperationBase::CreateDestByImage( CString strDestName, IUnknownPtr sptrSource,
	CString strConvName, int nDefPanesCount )
{
	if( sptrSource == 0 )
		return S_FALSE;

	IUnknown *punkDoc = 0;
	IViewPtr	sptrV = m_punkTarget;

	sptrV->GetDocument( &punkDoc );
	IDocumentPtr	sptrD = punkDoc;

	if( !punkDoc )
		return false;
	punkDoc->Release(); punkDoc = 0;


	IImage3Ptr sptrSrcImage = sptrSource;

	if( sptrSrcImage == 0 )
		return S_FALSE;

	m_sptrImage = ::CreateTypedObject( _bstr_t( szTypeImage ) );
	m_sptrImage->Release();

	if( m_sptrImage == 0 )
		return S_FALSE;

	CopyPropertyBag(sptrSrcImage, m_sptrImage, true);

	sptrSrcImage->GetSize( &m_nImageW, &m_nImageH );
	
	if( !m_nImageW || !m_nImageH )
		return S_FALSE;
	
	IImage3Ptr ptrImage = m_sptrImage;

	ptrImage->CreateImage( m_nImageW, m_nImageH, _bstr_t( strConvName ), ( _chech_cc( sptrSrcImage, strConvName, false ) == S_OK ) ?  ::GetImagePaneCount( sptrSrcImage ) : nDefPanesCount );
	ptrImage->InitRowMasks();

/*****************************************************************/
	int nSrcPanes = ::GetImagePaneCount( ptrImage  );
	for( int k = 0; k < nSrcPanes; k++ )
	{
		for( int j = 0; j < m_nImageH; j++ )
		{
			color *pDestColor = 0;
			ptrImage->GetRow( j, k, &pDestColor );
			for( int i = 0; i < m_nImageW; i++ )
				pDestColor[i] = 0;
		}
	}
/*****************************************************************/

	ptrImage->SetOffset( CPoint( 0, 0 ), true );

	if( !strDestName.IsEmpty()  )
		::SetObjectName( m_sptrImage, strDestName );
	else
	{
		strDestName = ::GetObjectName( sptrSrcImage );
		::SetObjectName( m_sptrImage, strDestName );
	}

	return S_OK;
}

HRESULT COperationBase::_chech_cc( IUnknown *punkImage, CString ColorName/* = "GRAY"*/, bool bShowMessage /* = true*/)
{
	IImage3Ptr ptrI = punkImage;

	if( ptrI == 0 )
		return S_FALSE;

	IUnknown	*punkCC = NULL;
	ptrI->GetColorConvertor( &punkCC );

	if( !punkCC )
		return S_FALSE;

	IColorConvertor2Ptr	ptrCC = punkCC;
	punkCC->Release(); punkCC = 0;

	BSTR bstr = 0;
	ptrCC->GetCnvName( &bstr );

	if( !bstr )
		return S_FALSE;

	if( _bstr_t( bstr ) != _bstr_t( ColorName ) )
	{
		if( bShowMessage )
			AfxMessageBox( IDS_CONVERT_ERROR, MB_OK | MB_ICONERROR );
		return S_FALSE;
	}

	return S_OK;
}

HRESULT COperationBase::CopyImagePane( IUnknownPtr sptrSource, int nPaneID, int nPaneIDFrom, int xOffset, int yOffset )
{
	IImage3Ptr ptrDest =  m_sptrImage;
	IImage3Ptr ptrSrc  =  sptrSource;

	bool bRGB = _chech_cc( sptrSource, "RGB", 0 ) == S_OK;

	int nSrcW, nSrcH;
	ptrSrc->GetSize(&nSrcW, &nSrcH);
	int nMinW = min(nSrcW,m_nImageW), nMinH = min(nSrcH,m_nImageH);

	for( int j = 0; j < m_nImageH; j++ )
	{
		color *pDestColor = 0;
		ptrDest->GetRow( j, nPaneID, &pDestColor );

		if( !pDestColor )
			break;

		if( j + yOffset >= 0 && j + yOffset < nMinH )
		{
			color *pSrcColor = 0;

			if( bRGB )
			{
				ptrSrc->GetRow( j + yOffset, 0, &pSrcColor );
				for( int z = 1; z < 3; z++ )
				{
					color *pColor = 0;
					ptrSrc->GetRow( j + yOffset, z, &pColor );

					for( int i = 0; i < nMinW; i++ )
						if( pColor[i] > pSrcColor[i] )
							pSrcColor[i] = pColor[i];
				}
			}
			else
				ptrSrc->GetRow( j + yOffset, nPaneIDFrom, &pSrcColor );
			
			for( int i = 0; i < m_nImageW; i++ )
			{
				pDestColor[i] = 0;
				if( i + xOffset >= 0 && i + xOffset < nMinW )
					pDestColor[i] += pSrcColor[i + xOffset];
			}
		}
		else
			for( int i = 0; i < m_nImageW; i++ )
				pDestColor[i] = 0;
	}

	return S_OK;
}

HRESULT COperationBase::StoreToContext()
{
	GuidKey key;

	INamedDataObject2Ptr sptrNDO = m_sptrPrevImg;

	if( sptrNDO != 0 )
		sptrNDO->GetBaseKey( &key );

	sptrNDO = m_sptrImage;

	if( sptrNDO != 0 )
		sptrNDO->SetBaseKey( &key );


	IUnknown *punkDoc = 0;
	IViewPtr	sptrV = m_punkTarget;

	sptrV->GetDocument( &punkDoc );
	IDocumentPtr	sptrD = punkDoc;

	if( !punkDoc )
		return S_FALSE;
		
	punkDoc->Release(); punkDoc = 0;
	m_changes.SetToDocData( sptrD, m_sptrImage );
	return S_OK;
}

HRESULT COperationBase::ReCreateDest(bool bRemoveFromDoData)
{
	IImage3Ptr sptrSrcImage = m_sptrImage;

	if( sptrSrcImage == 0 )
		return S_FALSE;

	m_sptrImage = ::CreateTypedObject( _bstr_t( szTypeImage ) );
	m_sptrImage->Release();

	if( m_sptrImage == 0 )
		return S_FALSE;
	
	sptrSrcImage->GetSize( &m_nImageW, &m_nImageH );
	
	if( !m_nImageW || !m_nImageH )
		return S_FALSE;
	
	IImage3Ptr ptrImage = m_sptrImage;

	ptrImage->CreateImage( m_nImageW, m_nImageH, _bstr_t( GetCCName( sptrSrcImage ) ), ::GetImagePaneCount( sptrSrcImage ) );
	ptrImage->InitRowMasks();

	CopyImagePanesEx( sptrSrcImage );
	CopyPropertyBag(sptrSrcImage, m_sptrImage, true);

	ptrImage->SetOffset( CPoint( 0, 0 ), true );

	::SetObjectName( m_sptrImage, ::GetObjectName( sptrSrcImage ) );

	IUnknown *punkDoc = 0;
	IViewPtr	sptrV = m_punkTarget;

	sptrV->GetDocument( &punkDoc );
	IDocumentPtr	sptrD = punkDoc;

	if( !punkDoc )
		return S_FALSE;

	_copy_named_data( m_sptrImage, sptrSrcImage );

	punkDoc->Release(); punkDoc = 0;
	if (bRemoveFromDoData)
		m_changes.RemoveFromDocData( sptrD, sptrSrcImage );

	GuidKey key;
	
	INumeredObjectPtr sptrN = sptrSrcImage;
	INumeredObjectPtr sptrN2 = m_sptrImage;
	
	sptrN->GetKey( &key );
	sptrN2->AssignKey( key );
	
	return S_OK;
}

HRESULT COperationBase::_copy_named_data( IUnknownPtr sptr, IUnknownPtr sptrOld )
{
	INamedDataPtr sptrNDO_Old = sptrOld;
	INamedDataPtr sptrNDO = sptr;

	if( sptrNDO == 0 )
	{
		INamedDataObject2Ptr ptrNDO = m_sptrImage;
		ptrNDO->InitAttachedData();
		sptrNDO = ptrNDO;
	}

//	return RecursiveWriteEntry( sptrNDO_Old, sptrNDO, KARYO_ACTIONROOT, 1 ) ? S_OK : S_FALSE;
	return RecursiveWriteEntry( sptrNDO_Old, sptrNDO, _T("\\"), 1 ) ? S_OK : S_FALSE;
}

HRESULT COperationBase::CopyImagePanesEx( IUnknownPtr sptrSource )
{
	IImage3Ptr ptrDest =  m_sptrImage;
	IImage3Ptr ptrSrc  =  sptrSource;

	int nSrcPanes = ::GetImagePaneCount( ptrSrc  );

	for( int nPaneID = 0; nPaneID < nSrcPanes; nPaneID++ )
	{
		for( int j = 0; j < m_nImageH; j++ )
		{
			color *pDestColor = 0;
			color *ptColor = 0;

			ptrDest->GetRow( j, nPaneID, &pDestColor );
			ptrSrc->GetRow(  j, nPaneID, &ptColor );
		
			for( int i = 0; i < m_nImageW; i++ )
				pDestColor[i] = ptColor[i];
		}
	}

	return S_OK;
}

HRESULT COperationBase::StoreToNamedData( const _bstr_t &bstrSect, const _bstr_t &bstrKey, const _variant_t &var )
{
	INamedDataObject2Ptr ptrNDO = m_sptrImage;
	if( ptrNDO == 0 )
		return S_FALSE;

	INamedDataPtr sptrNDO = m_sptrImage;

	if( sptrNDO == 0 )
	{
		ptrNDO->InitAttachedData();
		sptrNDO = ptrNDO;
	}

	sptrNDO->SetupSection( bstrSect );
	sptrNDO->SetValue( bstrKey, _variant_t( long( var.boolVal ) ) );

	return S_OK;
}

HRESULT COperationBase::ClearPane( int nPaneID )
{
	IImage3Ptr ptrDest =  m_sptrImage;

	int nSrcPanes = ::GetImagePaneCount( ptrDest );

	if( nPaneID >= nSrcPanes )
		return S_FALSE;

	for( int j = 0; j < m_nImageH; j++ )
	{
		color *pDestColor = 0;
		ptrDest->GetRow( j, nPaneID, &pDestColor );
	
		for( int i = 0; i < m_nImageW; i++ )
			pDestColor[i] = 0;
	}

	return S_OK;
}

bool COperationBase::Initialize()
{
	IUnknown	*punkDoc = 0;
	sptrIView	sptrV = m_punkTarget;
	sptrV->GetDocument( &punkDoc );
	sptrIDocument	sptrD = punkDoc;

	if( !punkDoc )
		return false;

	punkDoc->Release();

	m_lTargetKey = ::GetObjectKey( sptrD );
	return CInteractiveActionBase::Initialize();
}

HRESULT COperationBase::RemoveFromContext( IUnknownPtr pUnk )
{
	IUnknown *punkDoc = 0;
	IViewPtr	sptrV = m_punkTarget;

	sptrV->GetDocument( &punkDoc );
	IDocumentPtr	sptrD = punkDoc;

	if( !punkDoc )
		return S_FALSE;

	punkDoc->Release(); punkDoc = 0;
	m_changes.RemoveFromDocData( sptrD, pUnk );
	return S_OK;
}

CString COperationBase::GetCCName( IUnknownPtr sptrImage )
{
	IImage3Ptr sptrI = sptrImage;
	
	if( sptrI == 0 )
		return "";

	IUnknown *punkCC = 0;
	sptrI->GetColorConvertor( &punkCC );

	if( !punkCC )
		return "";

	IColorConvertor2Ptr sptrCC = punkCC;
	punkCC->Release();

	if( sptrCC == 0 )
		return "";

	BSTR bstr = 0;
	sptrCC->GetCnvName( &bstr );

	CString strRet = bstr;
	
	if( bstr )
		::SysFreeString( bstr );

	return strRet;
}
