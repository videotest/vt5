
/*inline void CImageFilterBase::_CheckMinMax( color colorVal, bool bAdd )
{
	if( bAdd )
	{
		bool	bNew = m_pCurrentHist[colorVal] == 0;
		m_pCurrentHist[colorVal]++;

		if( bNew )
		{
			int	c = -1;

			if( colorVal >= m_nCurrentMin && colorVal <= m_nCurrentMax )
			{
				int	c = m_nCurrentMin;
				while( true )
				{
					int	colorNext = m_pCurrentJumpUp[c];
					if( c < colorVal && (colorNext > colorVal ) )
					{
						m_pCurrentJumpUp[colorVal] = colorNext;
						m_pCurrentJumpUp[c] = colorVal;

						ASSERT(m_pCurrentJumpUp[c]>c||m_pCurrentJumpUp[c]==-1);
						ASSERT(m_pCurrentJumpUp[colorVal]>colorVal||m_pCurrentJumpUp[colorVal]==-1);

						m_pCurrentJumpDown[colorVal] = c;
						if( colorNext != -1 )
							m_pCurrentJumpDown[colorNext] = colorVal;
						else
							m_nCurrentMax = colorVal;
						break;
					}
					c = colorNext;
				}
			}
			else
			{
				if( colorVal > m_nCurrentMax )
				{
					if( m_nCurrentMax != -1 )
					{
						m_pCurrentJumpUp[m_nCurrentMax] = colorVal;
						m_pCurrentJumpDown[colorVal] = m_nCurrentMax;
					}
					else
					{
						m_nCurrentMin = colorVal;
					}
					m_nCurrentMax = colorVal;
				}
				else
				{
					if( m_nCurrentMin != -1 )
					{
						m_pCurrentJumpUp[colorVal] = m_nCurrentMin;
						m_pCurrentJumpDown[m_nCurrentMin] = colorVal;
					}
					else
					{
						m_nCurrentMax = colorVal;
					}
					m_nCurrentMin = colorVal;
				}
			}
		}
	}
	else
	{
		m_pCurrentHist[colorVal]--;
		bool	bRemove = m_pCurrentHist[colorVal] == 0;

		if( bRemove )
		{
			int	colorPrev = m_pCurrentJumpDown[colorVal];
			int	colorNext = m_pCurrentJumpUp[colorVal];

			if( colorPrev !=-1 )
				m_pCurrentJumpUp[colorPrev] = colorNext;
			else
			{
				m_nCurrentMin = colorNext;
			}

			if( colorNext !=-1 )
				m_pCurrentJumpDown[colorNext] = colorPrev;
			else
			{
				m_nCurrentMax = colorPrev;
			}

			m_pCurrentJumpDown[colorVal] = 
			m_pCurrentJumpUp[colorVal] = -1;
		}
	}
//	_check();
}*/

/*inline void CImageFilterBase::_CheckMedian( color colorVal, bool bAdd )
{
	if( bAdd )
	{
		if( m_pCurrentHist[colorVal] > m_pCurrentHist[m_nCurrentMedian] )
			m_nCurrentMedian = colorVal;
	}
	else
	{
		if( colorVal == m_nCurrentMedian )
		{
			int nCurrentMedianVal = m_pCurrentHist[m_nCurrentMedian];

			int	c = m_nCurrentMin;

			while( c != -1 )
			{
				int	nCurrentMedianVal = m_pCurrentHist[c];

				if( m_pCurrentHist[c] > nCurrentMedianVal )
				{
					m_nCurrentMedian = c;
					nCurrentMedianVal = m_pCurrentHist[c];
				}

				c = m_pCurrentJumpUp[c];
			}
		}
	}
}*/


