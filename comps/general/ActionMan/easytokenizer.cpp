#include "stdafx.h"
#include "easytokenizer.h"

CEasyTokenizer::CEasyTokenizer( const char	*psz )
{
	m_str = psz;
}

CEasyTokenizer::~CEasyTokenizer()
{
	for( int i = 0; i < m_arr.GetSize(); i++ )
		delete (COleVariant *)m_arr[i];
	m_arr.RemoveAll();
}

void CEasyTokenizer::Tokenize()
{
	SkipSpaces();

	while( !m_str.IsEmpty() )
	{
		int	idxTokenStart, idxTokenEnd;

		if( m_str[0] == '\"' )
		{
			idxTokenStart = 1;
			idxTokenEnd = m_str.Find( '\"', 1 );
			if( idxTokenEnd == -1 )
				idxTokenEnd = m_str.GetLength();
		}
		else
		{
			idxTokenStart = 0;
			idxTokenEnd = m_str.Find( ',' );

			if( idxTokenEnd == -1 )
				idxTokenEnd = m_str.GetLength();
		}

		CString	strToken = m_str.Mid( idxTokenStart, idxTokenEnd - idxTokenStart );
		
		if( m_str.GetLength()-idxTokenEnd-1 > 0 )
			m_str = m_str.Right( m_str.GetLength()-idxTokenEnd-1 );
		else
			m_str.Empty();

		COleVariant	*pvar = new COleVariant( strToken );
		m_arr.Add( pvar );

		SkipSpaces();
	}
}

void CEasyTokenizer::SkipSpaces()
{
	int	i = 0;
	while( i < m_str.GetLength() && (m_str[i] == ' '||m_str[i] == '\t') )i++;

	m_str = m_str.Right( m_str.GetLength() - i  );
}