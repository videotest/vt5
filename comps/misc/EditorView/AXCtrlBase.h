#ifndef __axctrlbase_h__
#define __axctrlbase_h__

#include "editorviewdefs.h"
#include "StreamEx.h"

class DLL CAXCtrlBase : public CDataObjectBase
{
	DECLARE_DYNAMIC(CAXCtrlBase);
public:
	CAXCtrlBase();
	virtual ~CAXCtrlBase();

	DECLARE_INTERFACE_MAP();
public:
	

	bool IsUniqueName( CString strName );
	CString GenerateUniqueName( CString strOfferName );


protected:
	virtual DWORD GetNamedObjectFlags();	//return the object flags
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );

	BEGIN_INTERFACE_PART_EXP(Ctrl, IActiveXCtrl)
		com_call GetProgID( BSTR *pbstrProgID );
		com_call SetProgID( BSTR bstrProgID );
		com_call GetRect( RECT *prect );
		com_call SetRect( RECT rect );
		com_call GetStyle( DWORD *pdwStyle );
		com_call SetStyle( DWORD dwStyle );
		com_call GetControlID( UINT *pnID );
		com_call SetControlID( UINT nID );
		com_call GetObjectData( BYTE **ppbyte, DWORD *pdwMemSize );
		com_call SetObjectData( BYTE *pbyte, DWORD dwMemSize );
		com_call GetName( BSTR	*pbstrName );
		com_call SetName( BSTR	bstrName );
		com_call IsUniqueName( BSTR	bstrName, BOOL* pbUniqueName );
		com_call GenerateUniqueName( BSTR bstrOfferName, BSTR* pbstrNewName );
	END_INTERFACE_PART(Ctrl)

	DECLARE_DISPATCH_MAP();
	//{{AFX_DISPATCH(CAXCtrlBase)
	afx_msg void StoreCtrlData( LPDISPATCH lpCtrlDisp );
	//}}AFX_DISPATCH

protected:
	CString	m_strControlName;
	CString	m_strProgID;
	CRect	m_rect;
	UINT	m_nID;
	DWORD	m_dwStyle;
	DWORD	m_dwMemSize;
	byte	*m_pObjectData;

};




class DLL CAXFormBase : public CDataObjectListBase
{
	DECLARE_DYNAMIC(CAXFormBase);

	DECLARE_INTERFACE_MAP();
public:
	CAXFormBase();
	virtual ~CAXFormBase();
public:
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );
	virtual bool CanBeBaseObject();
	virtual bool IsModified();
	virtual void SetModifiedFlag( bool bSet = true );
protected:
	BEGIN_INTERFACE_PART_EXP(Form, IActiveXForm)
		com_call SetTitle( BSTR bstr );
		com_call GetTitle( BSTR *pbstr );
		com_call SetSize( SIZE size );
		com_call GetSize( SIZE *psize );

		com_call AutoUpdateSize();

		com_call GetScript( IUnknown **ppunkScript );
		com_call SetScript( IUnknown *punkScript );
	END_INTERFACE_PART(Form)

protected:
	CSize	m_size;
	CString	m_strTitle;

	sptrINamedDataObject2	m_sptrScript;
};

#endif// __axctrlbase_h__
