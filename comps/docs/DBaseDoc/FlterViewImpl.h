#if !defined(AFX_FLTERVIEWIMPL_H__15C91244_8226_4076_B474_06705415BAD9__INCLUDED_)
#define AFX_FLTERVIEWIMPL_H__15C91244_8226_4076_B474_06705415BAD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlterViewImpl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFlterViewImpl

class CFlterViewImpl : public CImplBase
{
// Construction
protected:
	CFlterViewImpl();
	virtual ~CFlterViewImpl();

	BEGIN_INTERFACE_PART(FilterVw, IDBFilterView)		
		com_call GetActiveField( BSTR* pbstrTable, BSTR* pbstrField );
		com_call GetActiveFieldValue( BSTR* pbstrTable, BSTR* pbstrField, BSTR* pbstrValue );
	END_INTERFACE_PART(FilterVw);

protected:	
	virtual bool GetActiveField( CString& strTable, CString& strField ) = 0;
	virtual bool GetActiveFieldValue( CString& strTable, CString& strField, CString& strValue ) = 0;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLTERVIEWIMPL_H__15C91244_8226_4076_B474_06705415BAD9__INCLUDED_)
