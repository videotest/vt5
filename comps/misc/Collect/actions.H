//[ag]1. classes
#include <queue>
class CActionCollectByMasks : public CFilterBase
{
	DECLARE_DYNCREATE(CActionCollectByMasks)
	GUARD_DECLARE_OLECREATE(CActionCollectByMasks)

public:
	CActionCollectByMasks();
	virtual ~CActionCollectByMasks();

public:
	virtual bool IsAvaible();
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired(){return false;};
	virtual bool IsObjectMatch(IUnknown* punkObj)
	{
		return GetParentKey(punkObj) == INVALID_KEY;
	};


};

class CActionCollectByPoints : public CFilterBase
{
	DECLARE_DYNCREATE(CActionCollectByPoints)
	GUARD_DECLARE_OLECREATE(CActionCollectByPoints)

public:
	CActionCollectByPoints();
	virtual ~CActionCollectByPoints();

public:
	virtual bool IsAvaible();
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired(){return false;};
	virtual bool IsObjectMatch(IUnknown* punkObj)
	{
		return GetParentKey(punkObj) == INVALID_KEY;
	};
};


class CActionCollectByDisp : public CFilterBase
{
	DECLARE_DYNCREATE(CActionCollectByDisp)
	GUARD_DECLARE_OLECREATE(CActionCollectByDisp)

public:
	CActionCollectByDisp();
	virtual ~CActionCollectByDisp();

public:
	virtual bool IsAvaible();
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired(){return false;};
	virtual bool IsObjectMatch(IUnknown* punkObj)
	{
		return GetParentKey(punkObj) == INVALID_KEY;
	};
};

class CActionAlignImages : public CFilterBase
{
	DECLARE_DYNCREATE(CActionAlignImages)
	GUARD_DECLARE_OLECREATE(CActionAlignImages)
public:
	CActionAlignImages();
	virtual ~CActionAlignImages();


	virtual bool IsAvaible();
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired(){return false;};
	virtual bool CanDeleteArgument(){return false;};



};

class Quadrant
{
	int m_cx,m_cy, m_step, m_size;
	float* m_quads;
	float GetAlignSum(Quadrant& q, POINT offset);
public:
	Quadrant(){m_quads=0;};
	~Quadrant(){delete[]m_quads;};
	void set_size(int cx, int cy, int s);
	void create_by_parent(Quadrant* p);
	inline float& get(int y,int x);
	friend class CSignature;
};

class CSignature
{
public:
	CSignature();
	~CSignature();
	void Init(IUnknown* image);
	void Build(IUnknown* image, double shift =.1);
	POINT Find(CSignature& s, int dx, int dy);
	POINT FindQuad(CSignature& s, int dx, int dy);
	
private:
	int* m_hsums;
	int* m_vsums;
	int m_cx, m_cy;
	Quadrant* m_Qs;
	int m_pow;
	CImageWrp m_Image;
};

struct MEMO
{
	GUID guid;
	int offsetX;
	int offsetY;
	int number;
};
class Memorizer: public MEMO
{
public:
	Memorizer(IUnknown* pUnkND);
	Memorizer(POINT offset, int number, IUnknown* pUnkND, GUID& guid);
	~Memorizer();
	bool	ReadNamedData();
	void	WriteNamedData();
	void	EmptyNamedData();
private:
	INamedDataPtr nd;
};

static BOOL SubstituteByAlignedImages(CImageWrp* images, int& count, RECT* rects);
static void CopyImage(CImageWrp& dest, CImageWrp& src, POINT p);
static void CreateDutyImage(CImageWrp* sources, CImageWrp& duty, RECT* rects, int count);
static color get_owner_color(int x, int y, color c, CImageWrp* images , RECT*  rects, int count);

typedef int(*CompareFn)(void*,void*); 

template<class T>
class MaskZone
{
	struct MZ
	{
		MZ(){m_Index =-1;};
		RECT m_rect;
		int m_Index;
		T value;
	};
public:
	MaskZone(){currInd =0;m_pmzOwners =0;m_size=0;};
	~MaskZone()	{
		if(m_pmzOwners)
		delete[] m_pmzOwners;
		m_count=0;
	};
	void InitCell(RECT r, int i, T v );	
	void operator ++(){currInd++;};
	void sort();
	void Init(int(c)(void*,void*), int count);
	int GetBest(int x, int y);
private:
	MZ* m_pmzOwners;
	static int m_count;

	static CompareFn m_pFnCompare;
	int currInd;
	int m_size;
};

static int compareInt(void* a,void* b)
{
	return *((int*)a)-*((int*)b);
}
static int compareDWORD(void* a, void* b)
{
	return *((DWORD*)a)-*((DWORD*)b);
}
struct PXL
{
	int x,y;
	int c;
};
using namespace std;
class Filler
{
	color m_mark;
	CImageWrp m_pImage;
	queue<PXL> pixels;
	int m_cx,m_cy;
public:
	Filler(CImageWrp& img, color mark=1);
	~Filler(){};
	void AddToQueue(int x, int y, int c);
	void Process();
	void get_around(int x, int y, int c);
};
