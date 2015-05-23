#include "StdAfx.h"
#include "ScriptGenerator.h"



CCmdScriptGenerator::CCmdScriptGenerator(wchar_t *szFormat, ...)
{
    wchar_t buf[256];
    va_list va;
    va_start(va, szFormat);
    vswprintf(buf, szFormat, va);
    va_end(va);
	m_bstrCmd = buf;
}
