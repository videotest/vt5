#if !defined(__Lock_H__)
#define __Lock_H__

class CLock
{
public:
	bool *m_pbLockVar;
	bool m_bPrev;
	CLock(bool *pbLockVar)
	{
		m_bPrev = *pbLockVar;
		m_pbLockVar = pbLockVar;
		*pbLockVar = true;
	};
	~CLock()
	{
		*m_pbLockVar = m_bPrev;
	}
};

#endif