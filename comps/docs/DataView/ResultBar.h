#if !defined(AFX_RESULTBAR_H__C945A969_862F_4459_8B0D_DA3BFBCE29DC__INCLUDED_)
#define AFX_RESULTBAR_H__C945A969_862F_4459_8B0D_DA3BFBCE29DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResultBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResGridCtrl window
#include "DataViewDef.h"

#include "VTGridCtrl.h"
#include "GridCell.h"

#include <AfxTempl.h>

class CGridViewBase;
class CResultBar;

struct SValue
{
	double	fValue;
	CRect	rcRect;
	bool	bEmpty;
	CString	strFormat;

public:
	SValue()	
	{	fValue = 0; rcRect.SetRectEmpty();	bEmpty = true; }
	SValue(const SValue & rVal)
	{	fValue = rVal.fValue; rcRect = rVal.rcRect;	bEmpty = rVal.bEmpty;strFormat=rVal.strFormat; }
	SValue & operator = (const SValue & rVal)
	{	fValue = rVal.fValue; rcRect = rVal.rcRect;	bEmpty = rVal.bEmpty; strFormat=rVal.strFormat; return *this;	}

};


typedef CArray <SValue, SValue &> CValueArray;
typedef CArray <double, double &> CDoubleArray;

class CResultParam
{
public:
	CResultParam(long lKey = -1, LPCTSTR szName = 0)
	{	
		m_lParamKey = lKey; 
		m_strName	= szName;	
		m_arrValue.RemoveAll();
	}
	CResultParam(const CResultParam & rp)
	{	
		m_lParamKey = rp.m_lParamKey;	
		m_strName	= rp.m_strName;	
		m_arrValue.RemoveAll();
		m_arrValue.Copy(rp.m_arrValue);
	}
	CResultParam& operator = (const CResultParam & rp)
	{	
		m_lParamKey = rp.m_lParamKey;	
		m_strName	= rp.m_strName;	
		m_arrValue.RemoveAll();
		m_arrValue.Copy(rp.m_arrValue);
		return *this;	
	}

	void	SetParamRect(int nIndex, CRect & cellRect);
	CRect	GetParamRect(int nIndex);

	void	SetParamValue(int nIndex, double fVal);
	double  GetParamValue(int nIndex);

	void SetParamFormat(int nIndex, const char *pszFormat );
	const char *GetParamFormat(int nIndex);


	bool	IsValueEmpty(int nIndex);
	void	SetValueEmpty(int nIndex, bool bEmpty);

	CValueArray &	GetValues()	{	return m_arrValue;	}

	bool CalcValue(int nIndex, CDoubleArray & arr);

public:
	CString m_strName;
	long	m_lParamKey;

	CValueArray	m_arrValue;
};


typedef CArray <CResultParam, CResultParam &> CResParamArray;

/////////////////////////////////////////////////////////////////////////////
// CResultBar window

class CResultBar : public CStatic
{
	DECLARE_DYNAMIC(CResultBar)
// Construction
public:
	CResultBar();
	virtual ~CResultBar();
	virtual void Clear();
	BOOL RegisterWindowClass();
	void Show(bool bShow = true);
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResultBar)
	public:
	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual bool Init();
	virtual bool LoadState();
	virtual bool SaveState();
	virtual bool LoadParamInfo();
	void	SetSection(LPCTSTR szSection)
	{	m_strSection = szSection;	}

	virtual bool UpdateParam( long pos, ParameterContainer *pc );
	virtual bool Recalc();
	virtual bool UpdateParams();
	virtual bool UpdateParams(INamedDataObject2Ptr ptrNamed);
	CResParamArray		&GetParamArray(){return m_arrParams;};

	void UpdateSize();
	void CalcDefCellHeight();

	void	SetDefRowName(long lKey, LPCTSTR szName, bool bNeedUpdate = false);
	CString GetDefRowName(long lKey);

	bool CalcStats( int nPos, CDoubleArray & arr, const char *pszFmt );


	virtual CRect CalcGridRect();
	virtual CRect CalcParamNameRect();

	virtual int		GetHeight();
	int				GetDefCellHeight();

	virtual void Draw(CDC *pDC);
	virtual void DrawParam(CDC *pDC, CRect & rc, CString strFormat, double fValue, 
							bool bEmpty = true, int nOffset = 0, int nLast = 0);
	virtual bool SyncParamColumn();
	void	SetColumnWidth(int nCol, int nWidth);


public:
	void ParamRect(CDC *pDC, CRect & rc, CString strFormat, double fValue, bool bEmpty = true);
	void CalcOffset(CDC *pDC, int nIndex, int & nOffset, int & nLast);
	bool IsDrawLine() {return m_bDrawLine;}
	void PasteParams( IStream* pi_stream, int nstart_tabs_count );

protected:
	CResParamArray		m_arrParams;
	CRect				m_ParamRc;
	CRect				m_GridRc;
	int					m_nTextMargin;

	INamedDataObject2Ptr	m_objects;
	CGridViewBase *			m_pView;


	int	 m_nDefCellHeight;
	bool m_bDrawLine;

	CString	m_strSection;
	bool m_bShowNumber;
	bool m_bShowSum;
	bool m_bShowAver;
	bool m_bShowDev;
	bool m_bShowAbsDev;
	bool m_bShowVar;

	bool	m_bEnabled;

	// Generated message map functions
protected:
	//{{AFX_MSG(CResultBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESULTBAR_H__C945A969_862F_4459_8B0D_DA3BFBCE29DC__INCLUDED_)
