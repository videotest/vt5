#if !defined(__Utils_H__)
#define __Utils_H__

inline int Range(int n, int nMin, int nMax) {return n < nMin ? nMin : n > nMax ? nMax : n;}
inline double RangeD(double d, double dMin, double dMax)
	{return d < dMin ? dMin : d > dMax ? dMax : d;}

inline int Bright ( int b, int g, int r )	//функция для получения яркости
{
	return (r*60+g*118+b*22)/200;
}

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

class CManageFlag
{
	bool m_bPrevFlag;
	bool *m_pbFlag;
public:
	CManageFlag(bool *pbFlag, bool bSet)
	{
		m_pbFlag = pbFlag;
		m_bPrevFlag = *m_pbFlag;
		*m_pbFlag = bSet;
	}
	~CManageFlag()
	{
		*m_pbFlag = m_bPrevFlag;
	}
};


#endif