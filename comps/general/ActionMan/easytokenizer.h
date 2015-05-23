#ifndef __easytoken_h__
#define __easytoken_h__

class CEasyTokenizer 
{
	CPtrArray	m_arr;
	CString		m_str;
public:
	CEasyTokenizer( const char	*psz );
	~CEasyTokenizer();

	void Tokenize();
	CPtrArray	&GetVars()	{return m_arr;};

protected:
	void SkipSpaces();

};

#endif //__easytoken_h__