#ifndef __data_ui_actions_h__
#define __data_ui_actions_h__


//////////////////////////////////////////////////////////////////////
//
//	Class CActionChooseObject
//
//////////////////////////////////////////////////////////////////////
class CActionChooseObject : public CActionBase
{
	DECLARE_DYNCREATE(CActionChooseObject)
	GUARD_DECLARE_OLECREATE(CActionChooseObject)

public:
	CActionChooseObject();
	virtual ~CActionChooseObject();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

//////////////////////////////////////////////////////////////////////
//
//	Class CObjectChooser
//
//////////////////////////////////////////////////////////////////////
class CObjectChooser : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CObjectChooser);
protected:
	CObjectChooser();

	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(Combo,IUIComboBox)
		com_call OnInitList( HWND hWnd );
		com_call OnSelChange();
		com_call OnDetachWindow();
	END_INTERFACE_PART(Combo)

	HWND m_hwnd_combo;
};



#endif//__data_ui_actions_h__