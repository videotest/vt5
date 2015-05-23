#ifndef __idio_actions_h__
#define __idio_actions_h__

#include "action_main.h"
#include "action_undo.h"
#include "resource.h"

class action_insert : public CAction, 
	public CUndoneableActionImpl,
	public CCustomizableActionImpl

{
	route_unknown();
public:
	
	~action_insert();
	static void *create_instance()	{return new action_insert();}
protected:
	virtual IUnknown *DoGetInterface( const IID &iid )
	{
		if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this; 
		else if( iid == IID_ICustomizableAction )return (ICustomizableAction*)this; 
		return CAction::DoGetInterface( iid );
	}					  

	com_call DoInvoke();
	com_call CreateSettingWindow( HWND hWnd );
	com_call GetActionState(DWORD *pdwState);

	IUnknown *get_object();

public:
	_list_t2<IIdiogram*>	m_insert;
};

class action_insert_info : public 
_ainfo_t<IDS_INSERT_IDIO, 
	action_insert::create_instance, 
	0, 0>
{
	route_unknown();
	static void *create_instance()	{return new action_insert_info();}
	virtual arg		*args()			
	{
		static arg	arguments[] = 
		{
			{0, 0, 0, 0, 0},
		};
		return arguments;		
	}
	virtual _bstr_t target()		{return "anydoc";}

};

// {C662CA7C-97DA-45e9-97B4-FE7B3D475369}
static const GUID clsis_action_insert = 
{ 0xc662ca7c, 0x97da, 0x45e9, { 0x97, 0xb4, 0xfe, 0x7b, 0x3d, 0x47, 0x53, 0x69 } };


#endif //__idio_actions_h__