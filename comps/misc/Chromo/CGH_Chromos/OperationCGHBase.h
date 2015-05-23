// OperationCGHBase.h: interface for the COperationCGHBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPERATIONCGHBASE_H__EE44C46D_2C0A_4232_84C2_3FBFF967BEB8__INCLUDED_)
#define AFX_OPERATIONCGHBASE_H__EE44C46D_2C0A_4232_84C2_3FBFF967BEB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COperationBase: public CInteractiveActionBase
{
	CObjectListUndoRecord	m_changes;	//undo
public:
	ENABLE_UNDO();
	COperationBase()	{ m_nImageW = m_nImageH = 0; }

	virtual ~COperationBase();
	HRESULT  StoreDestImage( CString strName );

	HRESULT  CreateDestByImage( CString strDestName, CString strSrcName, CString strConvName, int nDefPanesCount = -1 )
		{ return CreateDestByImage( strDestName, GetContextImage( strSrcName ), strConvName, nDefPanesCount ); }
	HRESULT  CreateDestByImage( CString strDestName, IUnknownPtr sptrSource, CString strConvName, int nDefPanesCount = -1 );

	HRESULT  ReCreateDest(bool bRemoveFromDoData = true);
	
	HRESULT CopyImagePane( CString strSrcName, int nPaneID, int nPaneIDFrom, int xOffset, int yOffset ) { return CopyImagePane( GetContextImage( strSrcName ), nPaneID, nPaneIDFrom, xOffset, yOffset ); }
	HRESULT CopyImagePane( IUnknownPtr sptrSource, int nPaneID, int nPaneIDFrom, int xOffset, int yOffset );
	HRESULT ClearPane( int nPaneID );
	HRESULT CopyImagePanesEx( IUnknownPtr sptrSource );

	IUnknown *GetContextImage(CString cName); // Without AddRef
	HRESULT StoreToContext();
	HRESULT StoreToNamedData( const _bstr_t &bstrSect, const _bstr_t &bstrKey, const _variant_t &var );

	virtual bool Initialize();

	virtual bool DoUndo() { m_changes.DoUndo( m_punkTarget ); return true; }
	virtual bool DoRedo() {	m_changes.DoRedo( m_punkTarget ); return true; }

	HRESULT RemoveFromContext( IUnknownPtr pUnk );
	CString GetCCName( IUnknownPtr sptrImage );
protected:
	int m_nImageW, m_nImageH;
	IUnknownPtr m_sptrImage, m_sptrPrevImg;
	HRESULT _chech_cc( IUnknown *punkImage, CString ColorName = "GRAY", bool bShowMessage = true );
	HRESULT _copy_named_data( IUnknownPtr sptr, IUnknownPtr sptrOld );
};

#endif // !defined(AFX_OPERATIONCGHBASE_H__EE44C46D_2C0A_4232_84C2_3FBFF967BEB8__INCLUDED_)
