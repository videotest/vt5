#pragma once

// ------------- тип значения, живущего в куче (для сортировки) -------------
class CTimeValue
{
public:
	int x;
	int y;
	double u;
};

// ------------- Min Heap -------------
// хранит кучу значений, позволяет мгновенно получать наименьшее из них,
// добавлять значения, удалять наименьшее
// (по сути - очередь с приоритетами)
class CHeap
{
public:
	inline CHeap();
	inline ~CHeap();
	inline CTimeValue *GetFirst();
	inline void DeleteFirst();
	inline void Insert(int x, int y, double u);

protected:
	inline void PushToTail(int i);
	inline void PushToHead(int i);
	CTimeValue *m_pValues;
	int m_nCount;
	int m_nMaxCount;
};

CHeap::CHeap()
{
	m_nCount = 0;
	m_nMaxCount = 64;
	m_pValues = new CTimeValue[m_nMaxCount];
}

CHeap::~CHeap()
{
	delete[] m_pValues;
}

CTimeValue *CHeap::GetFirst()
{
	if(m_nCount<=0) return 0;
	return m_pValues;
}

void CHeap::DeleteFirst()
{
	if(m_nCount<=0) return;

	m_nCount--;
	m_pValues[0] = m_pValues[m_nCount];
	PushToTail(0);
}

void CHeap::Insert(int x, int y, double u)
{
	if(m_nCount>=m_nMaxCount)
	{
		m_nMaxCount = m_nCount*2;
		CTimeValue* p = new CTimeValue[m_nMaxCount];
		for(int i=0; i<m_nCount; i++)
			p[i] = m_pValues[i];
		delete[] m_pValues;
		m_pValues = p;
	}
	m_pValues[m_nCount].x = x;
	m_pValues[m_nCount].y = y;
	m_pValues[m_nCount].u = u;
	m_nCount++;
	PushToHead(m_nCount-1);
}

void CHeap::PushToTail(int i)
{
	int child = i*2+1; // left child
	if(child>=m_nCount) return; // добрались до конца дерева - нет чайлдов
	if(child+1<=m_nCount) // есть и правый child
	{
		if( m_pValues[child+1].u < m_pValues[child].u )
			child++; // выберем меньшего из двух чилдов
	}
	if( m_pValues[i].u > m_pValues[child].u )
	{ // если больше, чем чилд - поменяемся с ним
		CTimeValue tmp = m_pValues[i];
		m_pValues[i] = m_pValues[child];
		m_pValues[child] = tmp;
		PushToTail(child); // и протолкнуть его и дальше вглубь
	} // можно переделать на итеративный алгоритм
}

void CHeap::PushToHead(int i)
{
	if(i==0) return;
	int parent = (i-1)/2;
	if( m_pValues[i].u < m_pValues[parent].u )
	{ // если чилд меньше парента - поменяемся с ним
		CTimeValue tmp = m_pValues[i];
		m_pValues[i] = m_pValues[parent];
		m_pValues[parent] = tmp;
		PushToHead(parent); // и протолкнуть его и дальше вглубь
	}
}

/////////////////////////////////////////////////////////////////////////////
// _fast_march_t - шаблон для реализации Fast Marching
// 

// В классе T должны быть определены методы
// double GetV(int x, int y, int x_from, int y_from); // вычислить v в точке;
// может иметь побочные эффекты (вызывается только перед расползанием из from в (x,y)
// void OnAddPoint(int x, int y, double dist, double v);
// этот вызов дергают после добавления очередной точки
// на нем можно, к примеру, апдейтить среднее значение цвета по области
// Лучше эти две функции сделать инлайнами
// Задача _fast_march_t - вычислить для всех точек
// dist = min интеграла GetV по пути
// по всем доступным путям от зародышей

// ------------- реализация Fast Marching -------------
template<class T>
class _fast_march_t : public T
{
public:
	_fast_march_t();
	~_fast_march_t();
	bool Init(int cx, int cy); // возвращает false, если не проинитились
	inline void AddPoint(int x, int y, double dist=0);
	bool Process(); // вычислить m_pDist; возвращает false, если не проинитились

	double *m_pDist;
private:
	int m_cx,m_cy;
	CHeap m_Heap;
};

template<class T>
_fast_march_t<T>::_fast_march_t()
{
	m_cx=0;
	m_cy=0;
	m_pDist = 0;
}

template<class T>
_fast_march_t<T>::~_fast_march_t()
{
	delete[] m_pDist;
}

template<class T>
bool _fast_march_t<T>::Init(int cx, int cy)
{
	m_cx=cx;
	m_cy=cy;
	m_pDist = new double[cx*cy];
	if(!m_pDist) return false;
	int n=cx*cy;
	for(int i=0; i<n; i++) m_pDist[i] = 1e200;
	return true;
}

template<class T>
void _fast_march_t<T>::AddPoint(int x, int y, double dist)
{
	m_Heap.Insert(x,y,dist);
	m_pDist[y*m_cx+x]=dist;
}

// dist = min интеграла GetV по пути
// по всем доступным путям от зародышей
template<class T>
bool _fast_march_t<T>::Process()
{
	if(m_cx==0 || m_cy==0 || m_pDist==0) return false;
	CTimeValue* p;
	while( p = m_Heap.GetFirst() )
	{
		int x_from=p->x, y_from = p->y;

		if( x_from<2 || x_from>=m_cx-2 || y_from<2 || y_from>=m_cy-2 )
		{
			m_Heap.DeleteFirst();
			continue;
		}
		int dx=1, dy=0;
		do
		{
			int x_new = x_from+dx, y_new = y_from+dy;
			if( m_pDist[y_new*m_cx+x_new]>1e199 )
			{
				double v = GetV(x_new,y_new, x_from, y_from);

				double u1 = m_pDist[y_from*m_cx+x_from];
				double u2 = min(m_pDist[(y_new+dx)*m_cx+x_new-dy],m_pDist[(y_new-dx)*m_cx+x_new+dy]);
				if(u2>=u1+v) u2=u1+v;
				if(u1>=u2+v) u1=u2+v;
				double u21 = (u2-u1)/2;
				double d = sqrt( v*v/2 - u21*u21 );
				double u = (u1+u2)/2 + d;

				if(u<m_pDist[y_new*m_cx+x_new])
				{
					m_pDist[y_new*m_cx+x_new] = u;

					if(x_new>0 && x_new<m_cx-1 && y_new>0 && y_new<m_cy-1)
					{
						AddPoint(x_new,y_new,u);
						OnAddPoint(x_new,y_new,u,v);
					}
				}
			}

			int tmp=dx; dx=-dy; dy=tmp; // поворот на 90 градусов
		}
		while(dx!=1);
		m_Heap.DeleteFirst();
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Вспомогательные классы для Fast Marching -
// их можно подставлять в качестве T для _fast_march_t<T>

// ------------- тупой Point Processor - для Erode/Dilate -------------
class CSimplePointProcessor
{
public:
	inline double GetV(int x, int y, int x_from, int y_from)
	{ return 1.0; };
	inline void OnAddPoint(int x, int y, double dist, double v)
	{ };
};

// ------------- продвинутый Point Processor - для Magic Stick -------------
class CColorLikenessPointProcessor
{
public:
	double m_fTolerance;

	CColorLikenessPointProcessor()
	{
		m_ptrImage = 0;
		m_pRows = 0;
		m_pSum = 0;
		m_pAvg = 0;
		m_count = 0.0;
		m_fTolerance = 1.0;
	};
	~CColorLikenessPointProcessor()
	{
		delete[] m_pRows;
		delete[] m_pSum;
		delete[] m_pAvg;
	};
	void SetupImage(IImage4* pImage)
	{
		m_ptrImage = pImage;
		pImage->GetSize(&m_cx,&m_cy);
		pImage->GetPanesCount(&m_nPanes);
		m_pRows = new color* [ m_nPanes * m_cy ];
		m_pSum = new double[m_nPanes];
		m_pAvg = new double[m_nPanes];
		for(int pane=0; pane<m_nPanes; pane++)
		{
			m_pSum[pane] = m_pAvg[pane] = 0.0;
			for(int y=0; y<m_cy; y++)
			{
				pImage->GetRow(y, pane, & (m_pRows[pane*m_cy+y]) );				
			}
		}
		m_count = 0.0;
	};

public:
	inline double GetV(int x, int y, int x_from, int y_from)
	{
		double s = 0.0;
		for(int pane=0; pane<m_nPanes; pane++)
		{
			double d = m_pRows[pane*m_cy+y][x];
			d -= m_pAvg[pane];
			s += d*d;
		}
		s = s/m_fTolerance;
		return 1+s; // приближенное представление exp(s) = 1 + s/1! + s^2/2! + s^3/3! + ...
	};
	inline void OnAddPoint(int x, int y, double dist, double v)
	{
		m_count += 1/v;
		for(int pane=0; pane<m_nPanes; pane++)
		{
			m_pSum[pane] += m_pRows[pane*m_cy+y][x]/v;
			m_pAvg[pane] = m_pSum[pane] / m_count;
		}
	};
private:
	int m_cx,m_cy,m_nPanes;
	color **m_pRows;
	double *m_pSum, *m_pAvg; // счетчики 
	double m_count;
	IImage4Ptr m_ptrImage; // на всякий случай
};

// ------------- Point Processor для многофазного изображения -------------
class CPhasesColorLikenessPointProcessor
{
public:
	double m_fTolerance;

	CPhasesColorLikenessPointProcessor()
	{
		m_ptrImage = 0;
		m_pRows = 0;
		m_pBinRows = 0;
		m_pSum = 0;
		m_pAvg = 0;
		m_pCount = 0;
		m_fTolerance = 1.0;
	};
	~CPhasesColorLikenessPointProcessor()
	{
		delete[] m_pRows;
		delete[] m_pBinRows;
		delete[] m_pSum;
		delete[] m_pAvg;
		delete[] m_pCount;
	};
	void SetupImages(IImage4* pImage, IBinaryImage* pBin)
	{
		m_ptrImage = pImage;
		m_ptrBin = pBin;
		pImage->GetSize(&m_cx,&m_cy);
		pImage->GetPanesCount(&m_nPanes);
		m_pRows = new color* [ m_nPanes * m_cy ];
		m_pBinRows = new byte* [ m_cy ];
		m_pSum = new double[256*m_nPanes];
		m_pAvg = new double[256*m_nPanes];
		m_pCount = new double[256];
		for(int pane=0; pane<m_nPanes; pane++)
		{
			for(int phase=0; phase<256; phase++)
				m_pSum[pane*256+phase] = m_pAvg[pane*256+phase] = 0.0;
			for(int y=0; y<m_cy; y++)
				pImage->GetRow(y, pane, & (m_pRows[pane*m_cy+y]) );
		}
		for(int y=0; y<m_cy; y++)
			pBin->GetRow( &(m_pBinRows[y]), y, false );
		for(int phase=0; phase<256; phase++)
			m_pCount[phase] = 0.0;
	};

public:
	inline double GetV(int x, int y, int x_from, int y_from)
	{
		int phase = m_pBinRows[y_from][x_from];
		m_pBinRows[y][x] = phase;
		double s = 0.0;
		for(int pane=0; pane<m_nPanes; pane++)
		{
			double d = m_pRows[pane*m_cy+y][x];
			d -= m_pAvg[pane*256+phase];
			s += d*d;
		}
		s = s/m_fTolerance;
		return 1+s; // приближенное представление exp(s) = 1 + s/1! + s^2/2! + s^3/3! + ...
	};
	inline void OnAddPoint(int x, int y, double dist, double v)
	{
		int phase = m_pBinRows[y][x];
		m_pCount[phase] += 1/v;
		for(int pane=0; pane<m_nPanes; pane++)
		{
			m_pSum[pane*256+phase] += m_pRows[pane*m_cy+y][x]/v;
			m_pAvg[pane*256+phase] = m_pSum[pane*256+phase] / m_pCount[phase];
		}
	};
private:
	int m_cx,m_cy,m_nPanes;
	color **m_pRows;
	byte **m_pBinRows;
	double *m_pSum, *m_pAvg; // счетчики (256 фаз * число пан)
	double *m_pCount; // (для 256 фаз)
	IImage4Ptr m_ptrImage; // на всякий случай
	IBinaryImagePtr m_ptrBin; // на всякий случай
};

