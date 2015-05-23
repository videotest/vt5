#if !defined(AFX_EXPONENTSLIDER_H__82678116_8273_411B_BB52_A5C7D0AF3E4D__INCLUDED_)
#define AFX_EXPONENTSLIDER_H__82678116_8273_411B_BB52_A5C7D0AF3E4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExponentSlider.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExponentSlider window

class CExponentSlider : public CSliderCtrl
{
// Construction
	CUIntArray m_arr;
	int IntPosByExtPos(int nIntVal);
public:
	CExponentSlider();

// Attributes
public:

// Operations
public:
	void SetRange(int nMin, int nMax, BOOL bRedraw = FALSE);
	void SetPos(int nPos);
	int  GetPos();

	int NextValue(int nPos, int nDelta = 1);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExponentSlider)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExponentSlider();

	// Generated message map functions
protected:
	//{{AFX_MSG(CExponentSlider)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPONENTSLIDER_H__82678116_8273_411B_BB52_A5C7D0AF3E4D__INCLUDED_)
