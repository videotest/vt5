#pragma once
#include "method_int.h"

BOOL	get_method_by_name(BSTR bstr_mtd_name, IMethodMan *pmtd_man, IUnknown **ppunk_mtd, TPOS *plpos_mtd = 0);
HRESULT _invoke_func( IDispatch *pDisp, BSTR bstrFuncName, VARIANT* pargs, int nArgsCount, VARIANT* pvarResult );
_variant_t _exec_script_function( LPCTSTR lpctstrFunction, long nArgsCount=0, _variant_t *pvarArgs=0 );
void FireScriptEvent(LPCTSTR lpszEventName, int nParams=0, VARIANT *pvarParams=0);
DWORD	get_action_flags( BSTR bstr_name, DWORD dwDefault=0 );
bool IsInteractiveAction(char *pszName);
bool IsInteractiveRunning();
_bstr_t GetActionGroup(char *pszName);

extern _map_t<int, const char*, cmp_string> _tracers_map; // карта трейсеров - со счетчиками

class _dbg_tracer
{
public:
	_dbg_tracer(const char *name, const char *param=0) // предполагается, что *name - литерал, копию заводить не будем
	{
		m_pszName = name;
		TPOS pos = _tracers_map.find(m_pszName);
		if(pos) _tracers_map.set( _tracers_map.get(pos) + 1, m_pszName );
		else _tracers_map.set( 1, m_pszName );
		//_trace( "/----{ ", param );
		_trace( "", param );
		m_nDepth++;
	}
	~_dbg_tracer()
	{
		m_nDepth--;
		//_trace( "\\----} " );
		TPOS pos = _tracers_map.find(m_pszName);
		if(pos) _tracers_map.set( _tracers_map.get(pos) - 1, m_pszName );
		else _tracers_map.set( 0, m_pszName );
	}
	void _trace(char *prefix, const char *param=0)
	{
		static char sz[256];
		char* p = sz;
		for(int i=0; i<m_nDepth; i++)
		{
			*(p++) = '|';
			*(p++) = ' ';
			if(i>=18 && m_nDepth>=20)
			{
				*(p++) = '.';
				*(p++) = '.';
				*(p++) = '.';
				*(p++) = ' ';
				break;
			}
		}
		strcpy(p, prefix);
		p += strlen(p);
		strncpy(p, m_pszName, 100);
		p += strlen(p);

		TPOS pos = _tracers_map.find(m_pszName);
		if(pos)
		{
			sprintf( p, "[%i]", _tracers_map.get(pos) );
			p += strlen(p);
		}

		if(param)
		{
			*(p++) = ' ';
			strcpy(p, param);
			p += strlen(p);
		}

		sprintf(p,"\n");
		p += strlen(p);

		_trace_file_text("trace_aam", sz);
	}
	const char *m_pszName;
	static int m_nDepth;
};
