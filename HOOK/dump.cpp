// dump.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "_pdb.h"
#include "_hook.h"
#include "utils.h"

#pragma comment( lib, "Dbghelp.lib" )

//start time
DWORD g_dw_start_tick = 0;
time_t g_time_start;

MapHeapInfo g_map_heap;

//template<class key, class val> class pmap : public map<key,val*>
//{
//public:
//	~pmap(){
//		clear();
//	}
//	void clear(){
//		for(const_iterator it=begin(); it!=end(); ++it) delete it->second;
//		map<key,val*>::clear();
//	}
//	val*& operator[](key parKey){
//		std::pair<iterator,bool> ib=insert(value_type(parKey,(val*)NULL));
//		if(!ib.second)
//			delete ib.first->second;
//		return ib.first->second;
//	}
//	val* operator()(const key parKey){
//		iterator it=find(parKey);
//		if(it!=end())
//			return it->second;
//		else
//			return (val*)0;
//	}
//};

long g_block_mem_hook = false;
bool g_bRunMemTrace = false;

//map cs
CRITICAL_SECTION	g_cs_map;// = {0};

void ProcessAttach(HANDLE hModule)
	{
//		::LoadLibrary( "oleaut32.dll" );		

		//awin
		static module _module( (HMODULE)hModule );
		
		//save start time
		g_dw_start_tick = ::GetTickCount();
		time( &g_time_start );

		//read min alloc size
		g_block_mem_hook = !get_startup_gather_state();

		//read max stack size
		g_nmax_stack_size = get_stack_size();
		if( g_nmax_stack_size < 0 )	g_nmax_stack_size = 0;
		if( g_nmax_stack_size > MAX_STACK_SIZE )	g_nmax_stack_size = MAX_STACK_SIZE;
		set_stack_size( g_nmax_stack_size );		

}

void ProcessDetach(void)
{
//		::DeleteCriticalSection( &g_cs_map );		
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved)
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		ProcessAttach((HMODULE)hModule );
		//install hooks
		install_hook( 0 );		

	}
	else if( ul_reason_for_call == DLL_PROCESS_DETACH )
	{
		deinstall_hook( 0 );
	}

	return TRUE;
}

#ifdef _AGNITUM_

#include "\agnitum\ifaces\op_plugin.h"


//interface guid
// {A7632336-1A5B-4cff-9799-F98150D6955F}
static const GUID IFACE_DUMP	= 
{ 0xa7632336, 0x1a5b, 0x4cff, { 0x97, 0x99, 0xf9, 0x81, 0x50, 0xd6, 0x95, 0x5f } };

//plugin guid
// {9E160C34-188C-428a-9A56-73695CF33AA5}
static const GUID ID_OPCODE_DUMP = 
{ 0x9e160c34, 0x188c, 0x428a, { 0x9a, 0x56, 0x73, 0x69, 0x5c, 0xf3, 0x3a, 0xa5 } };


class dump_plugin : public outpost_plugin
{
public:	
	dump_plugin()
	{
		m_lref = 1;
	}
	virtual ~dump_plugin()
	{		
	}

	//просто добавляет reference на объект. аналон windows
	virtual long add_ref()
	{	
		::InterlockedIncrement( &m_lref ); 
		return m_lref;
	}
	//уменьшает счетчик ссылок объекта. если он 0, то убиваем сам объект
	virtual long release()
	{
		::InterlockedDecrement( &m_lref );
		if( !m_lref )
			delete this;
		return m_lref;
	}
	//используется для прослушивания общих сообшений о событии в системе
	virtual long entry_point( const GUID &from_code, long notify_code, void *ptr, long cb )
	{
		if( from_code == ID_OPCODE_SYSTEM )
		{
			if( notify_code == NC_PLUGIN_INIT )
			{

			}
			else if( notify_code == NC_PLUGIN_DEINIT )
			{

			}
			else if( notify_code == NC_PLUGIN_DEINIT )
			{

			}
			else if( notify_code == NC_PLUGIN_GETCODE )
			{
				*(GUID*)ptr = ID_OPCODE_DUMP;
				return 1;
			}
		}
		return 0;
	}
	//возвратить интерфейс в опро
	virtual void *get_interface( const GUID &iface ){
		if( iface == IFACE_DUMP )
			return this;
		return 0;
	}

	long m_lref;

};

//as engine simple plugin
//////////////////////////////////////////////////////////////////////
EXTERN_C __declspec( dllexport ) outpost_plugin* OutpostGetPlugin()
{
	dump_plugin* p = new dump_plugin;
	return (outpost_plugin*)p;
}

//AS driver high level plugin
//////////////////////////////////////////////////////////////////////
extern "C" __declspec( dllexport )
int OutpostPlugInEntry(HWND hOutpostWnd, HKEY hKeyOutpost, _char *pszOutpostRegPath, _char *pszProgramPath) 
{
	return 1;
}
extern "C" __declspec( dllexport ) void _OutpostPlugInUnLoad(void) 
{	
}
extern "C" __declspec( dllexport ) void OutpostPlugInUnLoad(void) 
{
}


#endif _AGNITUM_