#if !defined(AFX_VTLIST_H__29353517_1286_4037_B03C_3A545CA7E056__INCLUDED_)
#define AFX_VTLIST_H__29353517_1286_4037_B03C_3A545CA7E056__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTList.h : Declaration of the CVTList ActiveX Control class.

#include "ListBaseCtrl.h"



//#include ".\..\..\ifaces\axint.h"
/////////////////////////////////////////////////////////////////////////////
// CVTList : See VTList.cpp for implementation.

class CBaseCell;

#include "\vt5\awin\misc_list.h"
class CVTList : public COleControl
{
	CHeaderMan m_Man;
	long m_lVersion;
	bool m_bUseExtHeader;
	bool m_bLock;
	bool m_listDraw;
	DECLARE_SERIAL(CVTList)	
#ifndef FOR_HOME_WORK
	DECLARE_INTERFACE_MAP();
#endif
protected:
	struct XHeaderData
	{
		CString strName;
		int nAssignment;
		int lWidth;

		CHeaderCtrl *pHeaderLow;
		int nFrom;

		int nGrCount;
		int nGrIndex;

		CString strForienName;

		XHeaderData()
		{
			nAssignment	= 0;
			lWidth			= 100;

			pHeaderLow	= 0;
			nFrom			= -1;

			nGrCount		= 1;
			nGrIndex = 0;
		}

		static void _cleaner( XHeaderData *pData )
		{
			delete pData;
		}
	};

	typedef _list_t<XHeaderData *, XHeaderData::_cleaner> typeHD;

	static void _list_header_cleaner( typeHD *pData )
	{
		delete pData;
	}

	typedef _list_t<typeHD *, _list_header_cleaner > typeHL;
	
	typeHL m_listHeaders;
	typeHD m_listHeadersMain;

	static void _list_header_ctrl_cleaner( CListHeaderCtrl *pData )
	{
		delete []pData;
	}

	_list_t<CListHeaderCtrl *, _list_header_ctrl_cleaner > m_lstExtHeaderCtls;

	void SetDefaultValues();
	virtual void Serialize(CArchive& ar);	

	BEGIN_INTERFACE_PART(ActiveXCtrl, IVtActiveXCtrl3)
		com_call SetSite( IUnknown *punkVtApp, IUnknown *punkSite );
		com_call GetName( BSTR *pbstrName );	//such as Editor1
		com_call SetDoc( IUnknown *punkDoc );
		com_call SetApp( IUnknown *punkVtApp );
		com_call OnOK( );
		com_call OnApply( );
		com_call OnCancel( );
	END_INTERFACE_PART(ActiveXCtrl)

public:	

	CListBaseCtrl m_listCtrl;

   BOOL WriteToShellData(CBaseCell* pCell, datatype_e datatype);
   BOOL RestoreFromShellData();

// Constructor
public:
	CVTList();
	virtual void GetClientRect(LPRECT lpRect);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTList)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();
	//}}AFX_VIRTUAL

public:
	CStringArray m_sNameColumn;
	HWND hwndLV;
	//BOOL m_hideHeader;
	CToolTipCtrl *m_ToolTipCtrl;

// Implementation
protected:
	~CVTList();
	void UpdateControl(BOOL b_repaint = TRUE);

	DECLARE_OLECREATE_EX(CVTList)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTList)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTList)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTList)		// Type name and misc status
	
	
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

// Message maps
	//{{AFX_MSG(CVTList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTList)
	BOOL m_useGrid;
	afx_msg void OnUseGridChanged();
	BOOL m_useFullRowSelect;
	afx_msg void OnUseFullRowSelectChanged();
	BOOL m_AutoUpdate;
	afx_msg void OnAutoUpdateChanged();
	BOOL m_enabled;
	afx_msg void OnEnabledChanged();
	CString m_strName;
	afx_msg void OnNameChanged();
	afx_msg BOOL GetInnerRaised();
	afx_msg void SetInnerRaised(BOOL bNewValue);
	afx_msg BOOL GetInnerSunken();
	afx_msg void SetInnerSunken(BOOL bNewValue);
	afx_msg BOOL GetOuterRaised();
	afx_msg void SetOuterRaised(BOOL bNewValue);
	afx_msg BOOL GetOuterSunken();
	afx_msg void SetOuterSunken(BOOL bNewValue);
	afx_msg OLE_COLOR GetColumnBackColor(short nColumn);
	afx_msg OLE_COLOR GetColumnForeColor(short nColumn);
	afx_msg BSTR GetColumnCaption(short nColumn);
	afx_msg short GetColumnType(short nColumn);
	afx_msg short GetColumnAlign(short nColumn);
	afx_msg short GetColumnCount();
	afx_msg short GetColumnWidth(short nColumn);
	afx_msg VARIANT GetColumnFont(short nColumn);
	afx_msg void SetColumnAlign(short nColumn, short nAlign);
	afx_msg void SetColumnBackColor(short nColumn, OLE_COLOR color);
	afx_msg void SetColumnForeColor(short nColumn, OLE_COLOR color);
	afx_msg void SetColumnType(short nColumn, short nType);
	afx_msg void SetColumnWidth(short nColumn, short nWidth);
	afx_msg void SetColumnCaption(short nColumn, LPCTSTR pstrCaption);
	afx_msg void SetColumnFont(short nColumn, const VARIANT FAR& pFont);
	afx_msg void AddColumn(short nColumn);
	afx_msg void DeleteColumn(short nColumn);
	afx_msg short GetDisplayLogicalAsText(short nColumn);
	afx_msg void SetDisplayLogicalAsText(short nColumn, short nValue);
	afx_msg short GetCurentColumn();
	afx_msg short GetRowCount();
	afx_msg short IsColumnUseIcon(short nColumn);
	afx_msg void SetColumnUseIcon(short nColumn, short nValue);
	afx_msg BSTR GetCellAsString(short nColumn, short nRow);
	afx_msg void DeleteRow(short nRow);
	afx_msg void AddRow(short nRow);
	afx_msg void SetIcon(short nColumn, short nRow, LPCTSTR strFileName);
	afx_msg void SetColumnLogicalIcon(short nColumn, LPCTSTR pbStrFileName, short b_forValue);
	afx_msg short CurColumnLeft();
	afx_msg short CurColumnRight();
	afx_msg void SetColumnReadOnly(short nColumn, short nValue);
	afx_msg short IsColumnReadOnly(short nColumn);
	afx_msg void SetCellValue(short nColumn, short nRow, LPCTSTR pbStrValue);
	afx_msg BSTR GetKeyValue(short nColumn, short nRow);
	afx_msg void SetKeyValue(short nColumn, short nRow, LPCTSTR pbStrKey);
	afx_msg BSTR GetCellKey(short nColumn, short nRow);
	afx_msg BSTR GetCellAsSubject(short nColumn, short nRow);
	afx_msg OLE_COLOR GetCellAsColor(short nColumn, short nRow);
	afx_msg short GetCellAsLogical(short nColumn, short nRow);
	afx_msg BSTR GetCellAsValue(short nColumn, short nRow);
	afx_msg void StoreToData();
	afx_msg short GetCurentRow();
	afx_msg void SetCurentRow(short nRow);
	afx_msg void SetCurentCol(short nCol);
	afx_msg void RestoreFromData();
	afx_msg BOOL GetDisabled();
	afx_msg void SetDisabled(BOOL bNewValue);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg void SetFocus();
	afx_msg OLE_COLOR GetEditBackColor();
	afx_msg void SetEditBackColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetEditTextColor();
	afx_msg void SetEditTextColor(OLE_COLOR nNewValue);
	afx_msg void ForceEditCell( short nColumn, short nRow );
	afx_msg void SetCellAsLogical(short nColumn, short nRow, short bValue);
	afx_msg OLE_COLOR GetListBkColor();
	afx_msg void SetListBkColor(OLE_COLOR nNewValue);	

	afx_msg	OLE_COLOR GetActiveCellBackColor( );
	afx_msg	void SetActiveCellBackColor(OLE_COLOR nNewValue);	
	afx_msg	OLE_COLOR GetActiveCellTextColor( );
	afx_msg	void SetActiveCellTextColor(OLE_COLOR nNewValue);	

	afx_msg BOOL GetUseSystemFont();
	afx_msg void SetUseSystemFont(BOOL bNewValue);
	afx_msg void SetCellValueColor(short nColumn, short nRow, OLE_COLOR color);

	afx_msg void SetCellValueColorRGB( short nColumn, short nRow, LPCTSTR strRGBColor ); // ( r, g, b)
	afx_msg BSTR GetCellAsColorRGB( short nColumn, short nRow );

	afx_msg void ClearCombo( short nColumn, short nRow );
	afx_msg BOOL AddComboString( short nColumn, short nRow, LPCTSTR pbStrValue );
	afx_msg BOOL RemoveComboItem( short nColumn, short nRow, short nIndex );
	
	BOOL m_bEnableResizeColumns; // [vanek] BT2000: 3542 - 26.12.2003
	afx_msg void OnEnableResizeColumnsChanged();

	afx_msg short GetCellTypeShading(short nColumn, short nRow);
	afx_msg void SetCellTypeShading(short nColumn, short nRow, short typeShading);
	afx_msg short GetCountTypeShading();

public:
	afx_msg BOOL GetUseSimpleFontType();
	afx_msg void SetUseSimpleFontType(BOOL bNewValue);

	afx_msg OLE_COLOR GetCellBackColor(short nColumn, short nRow );
	afx_msg OLE_COLOR GetCellForeColor(short nColumn, short nRow );

	afx_msg void SetCellBackColor(short nColumn, short nRow, OLE_COLOR color);
	afx_msg void SetCellForeColor(short nColumn, short nRow, OLE_COLOR color);

	afx_msg void ClearExtHeaders();
	afx_msg void SetUseExtHeader( short bUse );
	afx_msg long GetUseExtHeader();
	afx_msg long GetExtHeadersSize();

	afx_msg void SetExtHeaderData( short nLev, short nItem, LPCTSTR pbstrName, short nAssignment, short nWidth, short nDel );
	afx_msg BSTR GetExtHeaderName( short nLev, short nItem );
	afx_msg long GetExtHeaderAssignment( short nLev, short nItem );
	afx_msg long GetExtHeadersColumnSize( short nLev );
	afx_msg long GetExtHeaderWidth( short nLev, short nItem );

	afx_msg void SetHideHeader(BOOL val);
	afx_msg void SetHideBorder(BOOL val);
	afx_msg BOOL GetHideHeader();
	afx_msg BOOL GetHideBorder();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
								   
	afx_msg void AboutBox();

public:
	BOOL UseGrid(){
		return m_useGrid;
	}

	BOOL UseFullRowSelect(){
		return m_useFullRowSelect;
	}

	BOOL IsAutoUpdate(){
		return m_AutoUpdate;
	}

	// [vanek] BT2000: 3542 - 26.12.2003
	BOOL IsEnableResizeColumns(){
		return m_bEnableResizeColumns;
	}

	BOOL _GetUseSystemFont(){ return GetUseSystemFont(); }
	void _create_header_ctrls();
	void _create_assignment_map();
	void _apply_header_resize( CHeaderCtrl *pCtrl, int iItem, int nWidth, bool bAsgnHeader, bool InreaseChW, bool InternalCall, bool MainCall );
	BOOL _can_be_resizable( CHeaderCtrl *pCtrl, int iItem, int nDir );
	int _calc_header_groups( CHeaderCtrl *pCtr, int nitem, int nIndex, int nMaxCount, CString strMainName, int nParentAssignment   );

protected:
	BOOL				m_bInnerRaised;
	BOOL				m_bInnerSunken;
	BOOL				m_bOuterRaised;
	BOOL				m_bOuterSunken;

    BOOL				m_bUseSystemFont;
	BOOL				m_bUseSimpleFontType;

	BOOL				m_HideHeader;
	BOOL				m_HideBorder;
	BOOL				flagHideHeader;
	int					m_widthCtrlList;
	WNDPROC				wpOrigEditProc;
	int					m_zeroColumnWidth;
	void SendWM_SIZE();
   
public:


// Event maps
	//{{AFX_EVENT(CVTList)
	void FireCurentColumnChange(short nColumn)
		{FireEvent(eventidCurentColumnChange,EVENT_PARAM(VTS_I2), nColumn);}
	void FireCurentRowChange(short nRow)
		{FireEvent(eventidCurentRowChange,EVENT_PARAM(VTS_I2), nRow);}
	void FireDataChange(short nColumn, short nRow)
		{FireEvent(eventidDataChange,EVENT_PARAM(VTS_I2  VTS_I2), nColumn, nRow);}
	void FireOnDeleteRow(short nRow)
		{FireEvent(eventidOnDeleteRow,EVENT_PARAM(VTS_I2), nRow);}
	void FireDblClick(short nColumn, short nRow)
		{FireEvent(eventidDblClick,EVENT_PARAM(VTS_I2  VTS_I2), nColumn, nRow);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CVTList)
	dispidUseGrid = 1L,
	dispidUseFullRowSelect = 2L,
	dispidAutoUpdate = 3L,
	dispidEnabled = 4L,
	dispidName = 5L,
	dispidInnerRaised = 6L,
	dispidInnerSunken = 7L,
	dispidOuterRaised = 8L,
	dispidOuterSunken = 9L,
	dispidGetColumnBackColor = 10L,
	dispidGetColumnForeColor = 11L,
	dispidGetColumnCaption = 12L,
	dispidGetColumnType = 13L,
	dispidGetColumnAlign = 14L,
	dispidGetColumnCount = 15L,
	dispidGetColumnWidth = 16L,
	dispidGetColumnFont = 17L,
	dispidSetColumnAlign = 18L,
	dispidSetColumnBackColor = 19L,
	dispidSetColumnForeColor = 20L,
	dispidSetColumnType = 21L,
	dispidSetColumnWidth = 22L,
	dispidSetColumnCaption = 23L,
	dispidSetColumnFont = 24L,
	dispidAddColumn = 25L,
	dispidDeleteColumn = 26L,
	dispidGetDisplayLogicalAsText = 27L,
	dispidSetDisplayLogicalAsText = 28L,
	dispidGetCurentColumn = 29L,
	dispidGetRowCount = 30L,
	dispidIsColumnUseIcon = 31L,
	dispidSetColumnUseIcon = 32L,
	dispidGetCellAsString = 33L,
	dispidDeleteRow = 34L,
	dispidAddRow = 35L,
	dispidSetIcon = 36L,
	dispidSetColumnLogicalIcon = 37L,
	dispidCurColumnLeft = 38L,
	dispidCurColumnRight = 39L,
	dispidSetColumnReadOnly = 40L,
	dispidIsColumnReadOnly = 41L,
	dispidSetCellValue = 42L,
	dispidGetKeyValue = 43L,
	dispidSetKeyValue = 44L,
	dispidGetCellKey = 45L,
	dispidGetCellAsSubject = 46L,
	dispidGetCellAsColor = 47L,
	dispidGetCellAsLogical = 48L,
	dispidGetCellAsValue = 49L,
	dispidStoreToData = 50L,
	dispidGetCurentRow = 51L,
	dispidSetCurentRow = 52L,
	dispidSetCurentCol = 53L,
	dispidRestoreFromData = 54L,
	dispidDisabled = 55L,
	dispidVisible = 56L,
	dispidSetFocus = 57L,
	dispidEditBackColor = 58L,
	dispidEditTextColor = 59L,
	dispidForceEditCell = 60L,
	dispidSetCellAsLogical = 61L,
	dispidListBkColor = 62L,
	dispidUseSystemFont = 63L,

	dispidSetCellValueColor = 64L,
	dispidSetCellValueColorRGB = 65L,
	dispidGetCellAsColorRGB = 66L,

	dispidClearCombo = 67L,
	dispidAddComboString = 68L,
	dispidRemoveComboItem = 69L,

	dispidUseSimpleFontType = 70L,

	dispidActiveCellBackColor = 71L,
	dispidActiveCellTextColor = 72L,
	dispidEnableResizeColumns = 73L,
	
	eventidCurentColumnChange = 1L,
	eventidCurentRowChange = 2L,
	eventidDataChange = 3L,
	eventidOnDeleteRow = 4L,
	eventidDblClick = 5L,

	dispidGetCellBackColor = 74L,
	dispidGetCellForeColor = 75L,
	dispidSetCellBackColor = 76L,
	dispidSetCellForeColor = 77L,

	dispidClearExtHeaders = 78L,
	dispidSetUseExtHeader = 79L,
	dispidGetUseExtHeader = 80L,
	dispidGetExtHeadersSize = 81L,

	dispidSetExtHeaderData = 82L,
	dispidGetExtHeaderName = 83L,
	dispidGetExtHeaderAssignment = 84L,
	dispidGetExtHeadersColumnSize = 85L,
	dispidGetExtHeaderWidth = 86L,

	dispidSetHideHeader = 87L,
	dispidSetHideBorder = 88L,
	dispidGetHideHeader = 89L,
	dispidGetHideBorder = 90L,

	dispidSetCellTypeShading = 91L,
	dispidGetCellTypeShading = 92L,
	//}}AFX_DISP_ID
	};
	

#ifndef FOR_HOME_WORK
	//IAXSitePtr	
	IUnknownPtr	m_ptrSite;
	IUnknownPtr	m_ptrApp;
#endif
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//---------  For tolltip -------------
	BOOL OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	//------------------------------------

public:
	afx_msg void OnDestroy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTLIST_H__29353517_1286_4037_B03C_3A545CA7E056__INCLUDED)
