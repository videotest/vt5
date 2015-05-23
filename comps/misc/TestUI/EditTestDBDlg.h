#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "test_man.h"


// CEditTestDBDlg dialog

#include "\vt5\common\resize_dlg.h"
#include "\vt5\AWIN\ctrl_groupbox.h"

class CEditTestDBDlg : public CDialog, public resizable_dlg_impl< CEditTestDBDlg >
{
	DECLARE_DYNAMIC(CEditTestDBDlg)

	void DoInitTestsWindow(HTREEITEM hti, ITestManager *pTestManager, long lParentPos);
	void InitTestsWindow();
	RECT m_rect_wnd;
public:
	CEditTestDBDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditTestDBDlg();

// Dialog Data
	enum { IDD = IDD_EDIT_TEST_DB };

	struct XItemInfoEx
	{
		XItemInfoEx();
		~XItemInfoEx();

		long lparent_pos;
		long lpos;
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	BEGIN_DLGRESIZE_MAP( CCompareImagesDlg )
		DLGRESIZE_CONTROL( IDC_TREE1		, DLG_SIZE_Y )

		DLGRESIZE_CONTROL( IDC_EDIT_EXEC_COND			,	DLG_SIZE_X )
		DLGRESIZE_CONTROL( IDC_EDIT_START_CONDITIONS	,	DLG_SIZE_X )
		DLGRESIZE_CONTROL( IDC_EDIT_TEST_SEQUENCE		,	DLG_SIZE_X | DLG_SIZE_Y )
		DLGRESIZE_CONTROL( IDC_EDIT_FINAL_TEST_SEQUENCE	,	DLG_SIZE_X )
		DLGRESIZE_CONTROL( IDC_EDIT_INFO				,	DLG_SIZE_X )

		DLGRESIZE_CONTROL( IDC_BUTTON_EXEC_COND					, DLG_MOVE_X )
		DLGRESIZE_CONTROL( IDC_BUTTON_START_CONDITIONS			, DLG_MOVE_X  )
		DLGRESIZE_CONTROL( IDC_BUTTON_TEST_SEQUENCE				, DLG_MOVE_X )
		DLGRESIZE_CONTROL( IDC_BUTTON_FINAL_TEST_SEQUENCE		, DLG_MOVE_X )

		DLGRESIZE_CONTROL( IDC_BUTTON_EDIT_FINAL_TEST_SEQUENCE	, DLG_MOVE_X  )
		DLGRESIZE_CONTROL( IDC_BUTTON_EDIT_TEST_SEQUENCE		, DLG_MOVE_X )

		DLGRESIZE_CONTROL( IDC_STATIC_GR1		, DLG_SIZE_X )
		DLGRESIZE_CONTROL( IDC_STATIC_GR2		, DLG_SIZE_X )

		DLGRESIZE_CONTROL( IDC_STATIC_CMB		, DLG_SIZE_X )
///////////////////////////////////////

		DLGRESIZE_CONTROL( IDC_STATIC_T4	, DLG_MOVE_Y )
		DLGRESIZE_CONTROL( IDC_EDIT_FINAL_TEST_SEQUENCE	,	DLG_MOVE_Y )
		DLGRESIZE_CONTROL( IDC_BUTTON_EDIT_FINAL_TEST_SEQUENCE	, DLG_MOVE_Y )
		DLGRESIZE_CONTROL( IDC_BUTTON_FINAL_TEST_SEQUENCE		, DLG_MOVE_Y )

		DLGRESIZE_CONTROL( IDC_STATIC_GR2		, DLG_MOVE_Y )
		DLGRESIZE_CONTROL( IDC_BUTTON_SAVE_ENV					, DLG_MOVE_Y )
		DLGRESIZE_CONTROL( IDC_STATIC_ENV_INFO	, DLG_MOVE_Y )
		DLGRESIZE_CONTROL( IDC_CHECK_LOAD_STATE	, DLG_MOVE_Y  )
		DLGRESIZE_CONTROL( IDC_CHECK_LOAD_SDC	,  DLG_MOVE_Y )
		DLGRESIZE_CONTROL( IDC_STATIC_T5	, DLG_MOVE_Y )
		DLGRESIZE_CONTROL( IDC_EDIT_INFO	, DLG_MOVE_Y )
		DLGRESIZE_CONTROL( IDC_STATIC_CMB	, DLG_MOVE_Y  )



/*
///////////////////////////////////////////////////////////////////////////////
		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_EDIT_EXEC_COND			,	DLG_MOVE_X | DLG_SIZE_Y )
			DLGRESIZE_CONTROL( IDC_EDIT_START_CONDITIONS	,	DLG_MOVE_X | DLG_MOVE_Y | DLG_SIZE_Y )
			DLGRESIZE_CONTROL( IDC_EDIT_TEST_SEQUENCE		,	DLG_MOVE_X | DLG_MOVE_Y | DLG_SIZE_Y )
			DLGRESIZE_CONTROL( IDC_EDIT_FINAL_TEST_SEQUENCE	,	DLG_MOVE_X | DLG_MOVE_Y | DLG_SIZE_Y )
			DLGRESIZE_CONTROL( IDC_EDIT_INFO				,	DLG_MOVE_X | DLG_MOVE_Y | DLG_SIZE_Y )
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_EDIT_EXEC_COND			,	0 ) // fake item
			DLGRESIZE_CONTROL( IDC_EDIT_START_CONDITIONS	,	0 ) // fake item
			DLGRESIZE_CONTROL( IDC_EDIT_TEST_SEQUENCE		,	DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_EDIT_FINAL_TEST_SEQUENCE	,	DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_EDIT_INFO				,	DLG_MOVE_Y )
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_EDIT_EXEC_COND			,	0 ) // fake item
			DLGRESIZE_CONTROL( IDC_EDIT_START_CONDITIONS	,	0 ) // fake item
			DLGRESIZE_CONTROL( IDC_EDIT_TEST_SEQUENCE		,	0 ) // fake item
			DLGRESIZE_CONTROL( IDC_EDIT_FINAL_TEST_SEQUENCE	,	0 ) // fake item
			DLGRESIZE_CONTROL( IDC_EDIT_INFO				,	DLG_MOVE_Y )
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_EDIT_EXEC_COND			,	0 )	 // fake item
			DLGRESIZE_CONTROL( IDC_EDIT_START_CONDITIONS	,	0 )	 // fake item
			DLGRESIZE_CONTROL( IDC_EDIT_TEST_SEQUENCE		,	0 )	 // fake item
			DLGRESIZE_CONTROL( IDC_EDIT_FINAL_TEST_SEQUENCE	,	0 ) // fake item
			DLGRESIZE_CONTROL( IDC_EDIT_INFO				,	DLG_MOVE_Y )
		END_DLGRESIZE_GROUP()
///////////////////////////////////////////////////////////////////////////////
		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_BUTTON_EXEC_COND					, DLG_MOVE_X )
			DLGRESIZE_CONTROL( IDC_BUTTON_START_CONDITIONS			, DLG_MOVE_X | DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_BUTTON_TEST_SEQUENCE				, DLG_MOVE_X | DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_BUTTON_FINAL_TEST_SEQUENCE		, DLG_MOVE_X | DLG_MOVE_Y )

			DLGRESIZE_CONTROL( IDC_BUTTON_FINAL_TEST_SEQUENCE		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_BUTTON_EXEC_COND					, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_BUTTON_START_CONDITIONS			, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_BUTTON_TEST_SEQUENCE				, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_BUTTON_FINAL_TEST_SEQUENCE		, DLG_MOVE_Y )

			DLGRESIZE_CONTROL( IDC_BUTTON_FINAL_TEST_SEQUENCE		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_BUTTON_EXEC_COND					, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_BUTTON_START_CONDITIONS			, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_BUTTON_EDIT_TEST_SEQUENCE		, DLG_MOVE_X | DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_BUTTON_EDIT_FINAL_TEST_SEQUENCE	, DLG_MOVE_X | DLG_MOVE_Y )

			DLGRESIZE_CONTROL( IDC_BUTTON_FINAL_TEST_SEQUENCE		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_BUTTON_EXEC_COND					, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_BUTTON_START_CONDITIONS			, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_BUTTON_EDIT_TEST_SEQUENCE		, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_BUTTON_EDIT_FINAL_TEST_SEQUENCE	, DLG_MOVE_Y )

			DLGRESIZE_CONTROL( IDC_BUTTON_FINAL_TEST_SEQUENCE		, 0 ) // fake item
		END_DLGRESIZE_GROUP()
//////////////////////////////////////////////////////////////////////////

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, DLG_MOVE_X | DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()
//////////////////////////////////////////////////////////////////////////
		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_BUTTON_SAVE_ENV					, DLG_MOVE_X | DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_BUTTON_SAVE_ENV					, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_BUTTON_SAVE_ENV					, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_BUTTON_SAVE_ENV					, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()
//////////////////////////////////////////////////////////////////////////
		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_STATIC_ENV_INFO	, DLG_MOVE_X | DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_STATIC_ENV_INFO	, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_STATIC_ENV_INFO	, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_STATIC_ENV_INFO	, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()
//////////////////////////////////////////////////////////////////////////
		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_CHECK_LOAD_STATE	, DLG_MOVE_X | DLG_MOVE_Y  )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_CHECK_LOAD_STATE	, DLG_MOVE_Y  )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_CHECK_LOAD_STATE	, DLG_MOVE_Y  )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_CHECK_LOAD_STATE	, DLG_MOVE_Y  )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()
//////////////////////////////////////////////////////////////////////////
		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_CHECK_LOAD_SDC	, DLG_MOVE_X | DLG_MOVE_Y   )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_CHECK_LOAD_SDC	, DLG_MOVE_Y   )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_CHECK_LOAD_SDC	, DLG_MOVE_Y   )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_CHECK_LOAD_SDC	, DLG_MOVE_Y   )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()
//////////////////////////////////////////////////////////////////////////
		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_STATIC_CMB		, DLG_MOVE_X | DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_STATIC_CMB		, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_STATIC_CMB		, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_STATIC_CMB		, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
			DLGRESIZE_CONTROL( IDC_STATIC_GR2		, 0 ) // fake item
		END_DLGRESIZE_GROUP()
//////////////////////////////////////////////////////////////////////////

/*

			DLGRESIZE_CONTROL( IDC_STATIC_T1	, DLG_MOVE_X )
			DLGRESIZE_CONTROL( IDC_STATIC_T2	, DLG_MOVE_X | DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_T3	, DLG_MOVE_X | DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_T3	, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_T4	, DLG_MOVE_X | DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_T4	, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_T4	, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_T5	, DLG_MOVE_X | DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_T5	, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_T5	, DLG_MOVE_Y )
			DLGRESIZE_CONTROL( IDC_STATIC_T5	, DLG_MOVE_Y )
*/
	END_DLGRESIZE_MAP()

	IMPLEMENT_RESIZE( m_hWnd );


	virtual BOOL OnInitDialog();
	CTreeCtrl m_TestsTree;
	CEdit	m_EditConds,
			m_EditTestSequence,
			m_EditFinalTestSequence,
			m_EditAddInfo,
			m_EditExecConds;

	HTREEITEM	AddTreeItem( HTREEITEM hti_parent, CString sName, long lparent_pos, long lpos);
	BOOL		DeleteTreeItem( HTREEITEM hti );

	void		UpdateTreeItemIcon( HTREEITEM hti );
	void		SynchronizeTestItemWithCtrls( TVITEM *pst_tvitem );
	void		SynchronizeCtrlsWithTestItem( TVITEM *pst_tvitem );
	BOOL		GenerateUniqueTestName( HTREEITEM hti_parent, CString sbase_name, CString *psUniqueTestName );
	HTREEITEM	FindTestItemInTree( HTREEITEM hti_parent, long lpos_parent_test, long lpos_test );
	HTREEITEM	FindItemByName( HTREEITEM hti_parent, CString str_item_name, BOOL bsearch_in_subitems );

	void		SetSelectedTestItem( long lpos_parent, long lpos );
	void		GetSelectedTestItem( long *plpos_parent, long *plpos );
	void		UpdateSelectedTest();

	void		_update_test_info_ctrls( ITestItem *pti );

protected:
	ITestManagerPtr m_sptrTM;
    CImageList m_ilTreeIcons;
	CMap< int, int, HBITMAP, HBITMAP>  m_mapIdToBitmap;
	BOOL	m_bChangeConds, 
			m_bChangeTestSequence,
			m_bChangeTestSequenceFinal,
			m_bChangeAddInfo,		  // [vanek] SBT: 795 - 12.03.2004
			m_bprevent_ok_cancel,
			m_bChangeExecConds;		// [vanek] : support executing conditions - 24.08.2004

	XItemInfoEx	m_selected_test;

	group_box_ctrl m_gr1;
	group_box_ctrl m_gr2;
public:
	void	OnAddNewTest( HTREEITEM hti_parent );
	void	OnDeleteTest( HTREEITEM hti );
	void	OnRenameTest( HTREEITEM hti_rename );
	void	OnCopyTest( HTREEITEM hti );
	void	OnPasteTest( HTREEITEM hti_paste );

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnNMRclickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnDeleteitemTree1(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnEnChangeEditStartConditions();
	afx_msg void OnEnChangeEditTestSequence();
	afx_msg void OnEnChangeEditFinalTestSequence();
	afx_msg void OnEnChangeEditInfo();

	afx_msg void OnEnMaxTextEditStartConditions();
	afx_msg void OnEnMaxTextEditTestSequence();
	afx_msg void OnEnMaxTextEditFinalTestSequence();
	afx_msg void OnEnMaxTextEditInfo();

	afx_msg void OnBnClickedButtonStartConditions();
	afx_msg void OnBnClickedButtonTestSequence();
	afx_msg void OnBnClickedButtonFinalTestSequence();
	afx_msg void OnBnClickedButtonEditTestSequence();
	afx_msg void OnBnClickedButtonEditFinalTestSequence();
	afx_msg void OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBeginlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnKeydownTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSaveEnv();
	afx_msg void OnTvnGetInfoTipTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonExecCond();
	afx_msg void OnEnChangeEditExecCond();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
};
