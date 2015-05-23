#pragma once
#include <list>
#include <stack>
using namespace std;



class CSceletonPoint
{
	~CSceletonPoint();
	int x,y;
	list<CSceletonPoint*> childs;
	CSceletonPoint* parent;
public:
	friend class CSceleton;
};

typedef list<CSceletonPoint*> SceletonTree;

class CSceleton
{
	byte* m_image;
	int* m_imageInt;
	Contour* m_c;
	int m_cx, m_cy;
	void process();
	void processSKIZ();
	void step1();
	void step2();
	bool clean();
	stack<byte*> m_vDel;
	inline bool _t01(char *row, char *rowUp, char *rowDown);
public:
	CSceleton(void);
	~CSceleton(void);
	void AttachData(byte* image, CSize size);
	int AttachDataSKIZ(Contour* c, byte* image, CSize size, POINT offset, int threshold =3);
	void GetSceleton(byte* image);
	void GetSceletonAsTree(SceletonTree& st);
	void GetLongestPath(list<POINT>& lpts);
	long GetLongestPathSize();
};
