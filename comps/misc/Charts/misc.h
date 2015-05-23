#pragma once

#include <vector>

template <class TKey, class TData> class _map_t2
{
	std::vector< TKey > m_Keys;
	std::vector< TData * > m_Data;
	public:

	~_map_t2()
	{
		clear();
	}

	TData &operator[]( TKey key )
	{
		int nSz =  (int)m_Keys.size();
		for( int i = 0; i < nSz; i++ )
			if( m_Keys[i] == key )
				break;
		
		if( i == nSz )
		{
			m_Keys.push_back( key );

			m_Data.push_back( new TData );
			return *( *( m_Data.end() - 1 ) );
		}
		return *m_Data[i];
	}
	TData &ByIndex( int i )
	{
		return *m_Data[i];
	}
	TKey GetKey( int i )
	{
		return m_Keys[i];
	}
	void erase( TKey key )
	{
		size_t nSz =  m_Keys.size();
		for( size_t i = 0; i < nSz; i++ )
			if( m_Keys[i] == key )
				break;

		if( i != nSz )
		{
			m_Keys.erase( m_Keys.begin() + i );

			delete m_Data[i];
			m_Data.erase( m_Data.begin() + i );
		}
	}
	int size()
	{
		return (int)m_Keys.size();
	}
	void clear()
	{
		m_Keys.clear();

		typedef std::vector< TData * >::iterator IT;
		for( IT it = m_Data.begin(); it != m_Data.end(); ++it )
			delete *it;

		m_Data.clear();
	}
};