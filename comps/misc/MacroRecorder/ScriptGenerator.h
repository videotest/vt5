#pragma once

class CScriptGenerator
{
public:
	virtual _bstr_t MakeLine() = 0;
};

class CCmdScriptGenerator : public CScriptGenerator
{
public:
	CCmdScriptGenerator(wchar_t *szFormat, ...);
	_bstr_t m_bstrCmd;
	_bstr_t MakeLine() {return m_bstrCmd;}
};


