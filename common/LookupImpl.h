#if !defined(AFX_LOOKUPIMPL_H__1A94E271_812A_451E_9EBF_0D5FF60FBA1D__INCLUDED_)
#define AFX_LOOKUPIMPL_H__1A94E271_812A_451E_9EBF_0D5FF60FBA1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LookupImpl.h : header file
//
#include "utils.h"
#include "image5.h"
/////////////////////////////////////////////////////////////////////////////
// CLookupImpl command target

class std_dll CLookupImpl : public CImplBase
{
public:
	CLookupImpl();
	virtual ~CLookupImpl();

	BEGIN_INTERFACE_PART_EXPORT(CnvLookup, IColorConvertorLookup)
		com_call SetLookupTable(byte* pTable, int cx, int cy);
		com_call KillLookup();
	END_INTERFACE_PART(CnvLookup);

protected:

	void SetLookupTable(byte* pTable, int cx, int cy);
	void DeleteLookup();

	inline byte _ColorAsByteR( color cr ){return m_pLookupTableR?m_pLookupTableR[cr]:(cr>>8);}
	inline byte _ColorAsByteG( color cr ){return m_pLookupTableG?m_pLookupTableG[cr]:(cr>>8);}
	inline byte _ColorAsByteB( color cr ){return m_pLookupTableB?m_pLookupTableB[cr]:(cr>>8);}

	byte*		m_pLookupTableR;
	byte*		m_pLookupTableG;
	byte*		m_pLookupTableB;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOOKUPIMPL_H__1A94E271_812A_451E_9EBF_0D5FF60FBA1D__INCLUDED_)
