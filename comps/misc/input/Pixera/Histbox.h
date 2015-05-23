#if !defined(__HistBox_H__)
#define __HistBox_H__
// HistBox.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CHistBox window

class CHistBox : public CStatic
{
public:
	enum HistoShowMethod
	{
		hsmNone = 0,
		hsmBri = 1,
		hsmRed = 2,
		hsmGreen = 4,
		hsmBlue = 8,
		hsmAll = 15
	};
protected:
	unsigned m_method;
// Construction
public:
	CHistBox();

	unsigned GetHSM() {return m_method;}
	void SetHSM(unsigned hsm)
	{
		m_method = hsm;
		Invalidate();
	}

	BOOL CanDrawBri(){return ((m_method&hsmBri) != 0);}
	BOOL CanDrawBlue(){return ((m_method&hsmBlue) != 0)&&m_fTrueColor;}
	BOOL CanDrawGreen(){return ((m_method&hsmGreen) != 0)&&m_fTrueColor;}
	BOOL CanDrawRed(){return ((m_method&hsmRed) != 0)&&m_fTrueColor;}

	BOOL CanDrawRow()
	{
		return (int)CanDrawBri()+
			(int)CanDrawBlue()+
			(int)CanDrawGreen()+
			(int)CanDrawRed() == 1;
	}



	void Calc(CRect rc, POINT *ptGray, POINT *ptRed, POINT *ptGreen, POINT *ptBlue);
	void InitHisto(LPBITMAPINFOHEADER lpbi);
	void OnDraw(CDC *pDC);

// Attributes
public:
	BOOL m_fTrueColor;
	unsigned m_nMaxValue;
	unsigned m_DataGray[256];
	unsigned m_DataRed[256];
	unsigned m_DataGreen[256];
	unsigned m_DataBlue[256];
	int m_nMax,m_nMin,m_nAvr;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHistBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHistBox)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void DrawRows( CDC *pDC, POINT *ppts, COLORREF crFinish );
	void DrawPts( CDC *pDC, POINT *ppts, COLORREF crFinish );
	void Validate();
};

/////////////////////////////////////////////////////////////////////////////


#endif