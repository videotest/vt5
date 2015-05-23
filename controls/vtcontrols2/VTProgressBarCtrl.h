#pragma once

// VTProgressBarCtrl.h : Declaration of the CVTProgressBarCtrl ActiveX Control class.


// CVTProgressBarCtrl : See VTProgressBarCtrl.cpp for implementation.

class CVTProgressBarCtrl : public COleControl
{
	DECLARE_DYNCREATE(CVTProgressBarCtrl)

// Constructor
public:
	CVTProgressBarCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// Implementation
protected:
	~CVTProgressBarCtrl();

	DECLARE_OLECREATE_EX(CVTProgressBarCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTProgressBarCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTProgressBarCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTProgressBarCtrl)		// Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
		/*dispidShowPercent = 5,*/	dispidSmooth = 4,	dispidPos = 3,		dispidMax = 2,		dispidMin = 1
	};
private:
	long	m_lMin,	m_lMax, m_lPos;
	BOOL	m_bSmooth, m_bShowPercent;

	void	InitCurrState( );
	CString get_output_text( );
	
protected:
	LONG GetMin(void);
	void SetMin(LONG newVal);
	LONG GetMax(void);
	void SetMax(LONG newVal);
	LONG GetPos(void);
	void SetPos(LONG newVal);
	BOOL GetSmooth();
	void SetSmooth(BOOL bNewValue);
	/*BOOL GetShowPercent();
	void SetShowPercent(BOOL bNewValue);*/
public:
	virtual void Serialize(CArchive& ar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

