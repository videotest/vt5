#pragma once
#include <vector>
using namespace std;

struct CLASSINFO
{
	TCHAR* name;
	COLORREF color;
};

class CCompositeEditorPage : public CPropertyPageBase
{
	PROVIDE_GUID_INFO( )
		DECLARE_DYNCREATE(CCompositeEditorPage)
	GUARD_DECLARE_OLECREATE(CCompositeEditorPage);

public:
	CCompositeEditorPage();
	~CCompositeEditorPage();
	enum { IDD = IDD_COMPOSITE_EDITOR_PP };
	virtual BOOL OnInitDialog();

protected:

	CComboBox m_cmbClass;
	void OnSelChanged();

	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
private:
	void read_classfile();
	CLASSINFO* strData; 
	int classCount;
	RECT itemRect;

	DECLARE_MESSAGE_MAP()
};

class CPhaseEditorPage : public CPropertyPageBase
{
	PROVIDE_GUID_INFO( )
		DECLARE_DYNCREATE(CPhaseEditorPage)
	GUARD_DECLARE_OLECREATE(CPhaseEditorPage);

public:
	CPhaseEditorPage();
	~CPhaseEditorPage();
	enum { IDD = IDD_PHASE_EDITOR_PP };
	virtual BOOL OnInitDialog();

protected:

	CComboBox m_cmbClass;
	void OnSelChanged();

	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
private:
	void read_classfile();
	RECT itemRect;
	vector<CLASSINFO> strData; 
	int classCount;
	void clear_strData();
	DECLARE_MESSAGE_MAP()
};