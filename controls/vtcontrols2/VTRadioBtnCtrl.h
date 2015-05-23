#pragma once

// VTRadioBtnCtrl.h : Declaration of the CVTRadioBtnCtrl ActiveX Control class.


// CVTRadioBtnCtrl : See VTRadioBtnCtrl.cpp for implementation.

class CVTRadioBtnCtrl : public COleControl
{
	DECLARE_DYNCREATE(CVTRadioBtnCtrl)

// Constructor
public:
	CVTRadioBtnCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual void Serialize(CArchive& ar);

// Implementation
protected:
	~CVTRadioBtnCtrl();

	DECLARE_OLECREATE_EX(CVTRadioBtnCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTRadioBtnCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTRadioBtnCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTRadioBtnCtrl)		// Type name and misc status

// Message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
		eventidOnChangeCheck = 1L,		

		dispidEnableRButton = 13L,
		dispidDisableRButton = 12L,
		dispidEnableMask = 11L,
		dispidEvenlySpaced = 10L,
		dispidVisible = 9L,
		dispidItemChecked = 8L,
		dispidGetItem = 7L,	
		dispidReCreate = 6L,
		dispidDeleteAll = 5L,
		dispidGetItemsCount = 4L,
		dispidDeleteItem = 3L,
		dispidAddItem = 2L,
	};
private:
	CStringArray	m_saItems;			// массив подписей для кнопок
	long			m_lItemChecked;		// индекс выбранной кнопки
	HWND			*m_haRBtns;			// массив handl-ов кнопок
	long			m_lRBCount;			// количество созданных кнопок
	BOOL			m_bVisible;			// видимость кнопок
    long			m_lLayout;			// размещение кнопок в контроле
	BOOL			m_bEvenlySpaced;	// кнопки на равном растоянии друг от друга
	DWORD			m_lEnableMask;		// маска контролов, младший бит - первый контрол


	bool	CreateButtons( );					// создание кнопок
	bool	DeleteAllButtons( );				// уничтожение всех кнопок
	HWND	GetItemHwnd( long iItemIndex );		// get handle window of item
	long	GetItemIndex( HWND hwnd );			// get item index of button
	void	SetButtonsOptions( DWORD nFlags );
	
	enum ButtonsOptions
	{	
		boCheck		= 1 << 0,
		boVisible	= 1 << 1,
		boEnable	= 1 << 2,		
	};

	// [vanek] BT2000:3849 - 18.06.2004
	enum ButtonsLayouts
	{
        bltVertical		=	0,
        bltHorizontal	=	1,	// на будующее :)
	};
	void	CalcLayout( );
	
	void	SetModifiedFlagAndRefresh( );		
	
// Dispatch 
protected:
	LONG AddItem(LPCTSTR NewValue);		// add new item
	LONG DeleteItem(LONG lIndex);		// delete item
	LONG GetItemsCount(void);			// get number of items
	void DeleteAll(void);				// delete all items
	void ReCreate(void);				// recreate buttons
	BSTR GetItem(LONG lIndex);			// get item
	LONG GetItemChecked(void);			// get checked index item
	void SetItemChecked(LONG newVal);	// set checked index item
	BOOL GetVisible(void);				// get visible
	void SetVisible(BOOL newVal);		// set visible
	BOOL GetEvenlySpaced(void);			// get EvenlySpaced
	void SetEvenlySpaced(BOOL newVal);	// set EvenlySpaced
	LONG GetEnableMask(void);			// get mask of enabled controls, 1 - enabled, 0 -disabled
	void SetEnableMask(LONG newVal);	// set mask of enabled controls, 1 - enabled, 0 -disabled
	BOOL DisableRButton(LONG lIndex);	// set disabled index item
	BOOL EnableRButton(LONG lIndex);	// set enabled index item


public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	void OnChangeCheck(LONG lItem)
	{
		FireEvent(eventidOnChangeCheck, EVENT_PARAM(VTS_I4), lItem);
	}
public:
	virtual void OnEnabledChanged();
};

