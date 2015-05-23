#pragma once



class CSelectBinaryPP: public CPropertyPageBase
{

	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CSelectBinaryPP);
	GUARD_DECLARE_OLECREATE(CSelectBinaryPP);
	
	IDataContext2* GetDataContext();
public:
	CSelectBinaryPP(CWnd* pParent = NULL);   // standard constructor
	~CSelectBinaryPP();
	enum { IDD = IDD_PROPPAGE_SELECTPLANE };
	virtual BOOL OnInitDialog();
	virtual void DoLoadSettings();	
protected:
	CArray<GuidKey> m_keys;
	void OnSelChanged();

	DECLARE_MESSAGE_MAP()
private:
	int compare(GuidKey k1,  GuidKey k2);
	int find_pos(GuidKey key);
};