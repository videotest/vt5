#if !defined(AFX_SIMPLEDIB_H__4F0F9101_383A_4C06_87A7_901901853858__INCLUDED_)
#define AFX_SIMPLEDIB_H__4F0F9101_383A_4C06_87A7_901901853858__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimpleDib.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSimpleDib window


class CDibWrapper
{
// Construction
public:
	CDibWrapper();
	virtual ~CDibWrapper();

	bool CreateFromDataObject( IUnknown* punkDataObject, CSize sizeDIB );
	bool NeedReCreate( IUnknown* punkDataObject, CSize sizeDIB );
	
	bool Draw( CDC* pDC, CRect rcTarget, bool bDrawBorderAroundDib );

	bool IsWasCreated();
	
	void DeInit();

protected:	
	BYTE *m_pDIB;	

public:	
	CSize GetDIBSize();	

	bool Store( IStream* pStream );
	bool Load( IStream* pStream );

	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEDIB_H__4F0F9101_383A_4C06_87A7_901901853858__INCLUDED_)
