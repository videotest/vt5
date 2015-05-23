#ifndef __actions_h__
#define __actions_h__

#include "action_main.h"
#include "action_interactive.h"
#include "resource.h"
#include "morphobase.h"
#include "bitbase.h"
#include <vector>
#include <queue>

#define IDS_ACTION_TEST	1


class CBinCleanV4 : public CMorphoFilter,
			   	  public _dyncreate_t<CBinCleanV4>
{
public:
	route_unknown();
public:
	
	CBinCleanV4();
	virtual bool InvokeFilter();
};


class CBinCleanV4Info : public _ainfo_t<ID_ACTION_BIN_CLEAN_V4, _dyncreate_t<CBinCleanV4>::CreateObject, 0>,
					   public _dyncreate_t<CBinCleanV4Info>
{
	route_unknown();
public:
	CBinCleanV4Info()
	{
		return;
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CBinMedian : public CMorphoFilter,
			   	  public _dyncreate_t<CBinMedian>
{
public:
	route_unknown();
public:
	
	CBinMedian();
	virtual bool InvokeFilter();
};


class CBinMedianInfo : public _ainfo_t<ID_ACTION_BIN_MEDIAN, _dyncreate_t<CBinMedian>::CreateObject, 0>,
					   public _dyncreate_t<CBinMedianInfo>
{
	route_unknown();
public:
	CBinMedianInfo()
	{
		return;
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


namespace _kir
{
template<class DATA>
class CCyclicList
{
public:
	struct list_item
	{
		DATA item;
		list_item* next;
		list_item* prev;
	};
private:
	int count;
	list_item* head;
	list_item* current;
	list_item* tail;
	bool encycled;
public:
	CCyclicList()	
	{	
		head = tail =0;	
		count = 0;
		encycled =false;
	};
	~CCyclicList()	
	{	
		while(count)
		{
			list_item* temp = head->next;
			delete head;
			head = temp;
			count--;
		}
	};
	void Add(DATA data)
	{
		if(head == 0)
		{
			head = new list_item();
			tail = head;
			head->next =0;
			head->prev =0;
			current = head;

		}
		else
		{
			tail->next = new list_item();
			list_item* temp = tail;
			tail = tail->next;
			tail->next=0;
			tail->prev = temp;

		}
		tail->item = data;
		count++;
	};

	void ShiftHead(int n =1)
	{
		while (n)
		{
			if(n>0)	{head = head->next;n--;}
			else {head = head->prev;n++;}
			
		}
	};
	DATA Previous()
	{

		return current->prev->item;

		
	}

	DATA Next()
	{

		if(current->next==0)
		{
			DebugBreak();
		}
		return current->next->item;

	}
	DATA GetData()
	{
		return current->item;
	}
	void SetData(DATA data)
	{
		if(!count) return;
		current->item = data;
	}

	void Encycle()
	{
		if(!count) return;
		tail->next = head;
		head->prev = tail;
		encycled =true;
		
	};
	void Decycle()
	{
		tail->next = 0;
		head->prev = 0;
		encycled =false;
	}

	bool isEncycled()
	{
		return encycled;
	}
	void Shift()
	{		
		 current = current->next;
		 current=current;
	}
	DATA GetNthFromCurrent(int pos)
	{
		list_item* temp = current;
		while(pos)
		{
			if(pos>0)
			{
				if(temp->next)  temp = temp->next;
				pos--;
			}
			else 
			{
				if(temp->prev)	temp = temp->prev;				
				pos++;
			}
		}
		return temp->item;
	}
	void SetCurrent(int n =0)
	{
		if(!head) return;
		list_item* temp = head;
		while (n)
		{
			if(n>0)	{temp = temp->next; n--;}
			else {temp = temp->prev; n++;}
		}
		current = temp;
	}
	void Delete()
	{
		if(count==0) return;
		if(count==1) 
		{
			delete head;
			count=0;
			return;
		}
		if(current==head) { head=head->next; }
		if(current==tail) { tail=tail->prev; }
		(current->prev)->next = current->next;
		(current->next)->prev = current->prev;
		list_item* temp = current;
		
		current = current->prev;
		
		delete temp;
		count--;
	}
	
	void DeleteTail()
	{
		if(!count) return;
		list_item* temp =tail;
		tail=tail->prev;
		count--;
		if(encycled) Encycle();
		delete temp;
	}

	int size()
	{
		return count;
	}



};

class CRoundMask
{
	typedef CCyclicList<byte*> RowSet;
	int cx,cy;		//image size
	int maskSize;
	byte** rows;
	RowSet destRows;
	RowSet srcRows;
					/* 
					the destRow & srcRows lists will contain pointers to currently selected image rows
					and will be cyclically filled with ones while moving top to down  
					*/
	

	int x,y;		//current position
	bool notify;
	IBinaryImage* source, *dest;
	byte* iMask;
	byte CheckPoint(int i,int j);
public:
	CRoundMask();
	bool Init(IBinaryImage* source, IBinaryImage* dest, UINT maskSize);
	~CRoundMask();

	bool Shift();
	bool Stamp(byte mark = 0xff);
};
using namespace std;
typedef vector<short> short_array;
typedef vector<byte*> pbyte_array;
typedef vector<short*> pshort_array;
typedef vector<UINT> uint_array;
typedef struct tagPix
{
	byte* ptr;
}PIX;

#define WSHED 0
#define MASK -2
#define INIT -1
#define FICT_PIX 0

class CWatershed
{
	bool e_mask;
	int disp;
	byte* src;//input image
	short* srcCopy;//copy of input image
	int cx,cy;//image dimensions
	short* imgDist;//map of distances
	byte** sortedSrc;//sorted by gray value image
	uint_array levelMap;//map of sortedSrc
	pshort_array util; //multi-purpose storage for operations with images
	bool init;
	int currentLabel;//current marker 
	int currentDist;//current distance from IZ centers
	int erode();//one step erosion
	int currLevel;//current level 
	int currPosition;//position in the images
	WORD hMax,hMin;//max & min brightness level
	queue<byte*> fifo;//queue with "points of interest"
    void AddLevel(byte* b, short* copy, int& count);//forming  image sorted by level
	UINT pixNumber;//number of masked pixels at image 
	bool pix_in_area(byte*& b,byte* p,int i, int y);//get or set pixel b near p at 'src' image 
	//next methods must be used only after completed Erode() - because they are also using srcCopy array
	short& im_out(/*in*/byte* p);//set or get assotiated with p-pixel of sorted image pixel of output image
	short& im_dist(/*in*/byte* p);//set or get assotiated with p-pixel of sorted image pixel of distances image
	void NormalizeOutput();
	short* out_neighbour(short* p,int i, int j);
public:
	void Init(IBinaryImage* bin);//must be called once before any method
	CWatershed::CWatershed();
	CWatershed::~CWatershed();
	void FindWatershed();//main routine
	void CopyRow(byte* dest, int y);//copiing resulting image row to the buffer  
	void Erode();//total erosion with many erode() calls
};

#ifdef _DEBUG
#define CHECK_INIT {if(!init)	{\
								MessageBox(0,"Need be initiated first!","",MB_ICONERROR | MB_OK);\
								DebugBreak();\
								}					\
								}
#else 
#define CHECK_INIT
#endif 

#define MARKER -2
#define MARKER0 -3
#define OUTER 2 
#define INNER 4
#define CHECKED 1

class CContour
{
#ifdef _DEBUG
	bool init;
#endif 
	short marker;
	int maskSize;
	int cx,cy;
	typedef CCyclicList<short*> Contour;
	typedef vector<Contour*> Contours;
	Contours contours;
	Contours cOut;
	short* imOut(short *p);
	short *image, *imageOut;
	float* gauss;
	POINT* area;
	byte* currentPoint;
	Contour* currentContour;
	bool follow_contour(short* begin);
	bool check_point(int i,int j, short* prePoint);
	inline bool isPointInArea(short* p, short* areaP);
	inline bool isBorder(int x,int y){return x==0 || y==0 || x==cx-1 || y==cy-1;};
	bool isBorder(short* p);
	bool isContour(short* p);
	void turn(int& i,int& j);//turn vector (i,j) by 90 counterclockwise
	int fill_row(short* row, int begin, short fill);
	void prefill();
	void repair(Contour*);
	void redraw(Contour*);
	void shift_and_fill(int x, int y, short* point);
	void fill_contour(short mark, short* begin);
	short* im(short *p);
	bool zero(short*);
	Contour* find_contour(short mark);
	short detect_cmark();
	
public:
	CContour();
	~CContour();
	bool Init(IBinaryImage* binImage, int maskSize = 3);
	void Gauss();
	void Build();
	void GetRow(short*& row, int n);
	void Fill();

private:
	// recursively remove contours point which are dead ended 
	void normalize(void);
};

}//namespace _kir

class CBinErode : public CMorphoFilter,
			   	  public _dyncreate_t<CBinErode>
{
public:
	route_unknown();
public:
	
	CBinErode();
	virtual bool InvokeFilter();
};


class CBinErodeInfo : public _ainfo_t<ID_ACTION_BIN_ERODE, _dyncreate_t<CBinErode>::CreateObject, 0>,
					   public _dyncreate_t<CBinErodeInfo>
{
	route_unknown();
public:
	CBinErodeInfo()
	{
		return;
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CBinDilate : public CMorphoFilter,
			   	  public _dyncreate_t<CBinDilate>
{
public:
	route_unknown();
public:
	
	CBinDilate();
	virtual bool InvokeFilter();
};


class CBinDilateInfo : public _ainfo_t<ID_ACTION_BIN_DILATE, _dyncreate_t<CBinDilate>::CreateObject, 0>,
					   public _dyncreate_t<CBinDilateInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CBinOpen : public CMorphoFilter,
			   	  public _dyncreate_t<CBinOpen>
{
public:
	route_unknown();
public:
	
	CBinOpen();
	virtual bool InvokeFilter();
};


class CBinOpenInfo : public _ainfo_t<ID_ACTION_BIN_OPEN, _dyncreate_t<CBinOpen>::CreateObject, 0>,
					   public _dyncreate_t<CBinOpenInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CBinClose : public CMorphoFilter,
			   	  public _dyncreate_t<CBinClose>
{
public:
	route_unknown();
public:
	
	CBinClose();
	virtual bool InvokeFilter();
};


class CBinCloseInfo : public _ainfo_t<ID_ACTION_BIN_CLOSE, _dyncreate_t<CBinClose>::CreateObject, 0>,
					   public _dyncreate_t<CBinCloseInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CBinNot : public CBitFilter,
			   	  public _dyncreate_t<CBinNot>
{
public:
	route_unknown();
public:
	
	CBinNot(){m_nBitOperationType = botNot;};
};


class CBinNotInfo : public _ainfo_t<ID_ACTION_BIN_NOT, _dyncreate_t<CBinNot>::CreateObject, 0>,
					   public _dyncreate_t<CBinNotInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CBinOr : public CBitFilter,
			   	  public _dyncreate_t<CBinOr>
{
public:
	route_unknown();
public:
	
	CBinOr(){m_nBitOperationType = botOr;};
};

class CBinOrInfo : public _ainfo_t<ID_ACTION_BIN_OR, _dyncreate_t<CBinOr>::CreateObject, 0>,
					   public _dyncreate_t<CBinOrInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CBinAnd : public CBitFilter,
			   	  public _dyncreate_t<CBinAnd>
{
public:
	route_unknown();
public:
	
	CBinAnd(){m_nBitOperationType = botAnd;};
};

class CBinAndInfo : public _ainfo_t<ID_ACTION_BIN_AND, _dyncreate_t<CBinAnd>::CreateObject, 0>,
					   public _dyncreate_t<CBinAndInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CBinXor : public CBitFilter,
			   	  public _dyncreate_t<CBinXor>
{
public:
	route_unknown();
public:
	
	CBinXor(){m_nBitOperationType = botXor;};
};

class CBinXorInfo : public _ainfo_t<ID_ACTION_BIN_XOR, _dyncreate_t<CBinXor>::CreateObject, 0>,
					   public _dyncreate_t<CBinXorInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CBinCustomLogic : public CFilter,
			   	  public _dyncreate_t<CBinCustomLogic>
{
public:
	route_unknown();
protected:
	virtual bool InvokeFilter();
};

class CBinCustomLogicInfo : public _ainfo_t<ID_ACTION_BIN_CUSTOM_LOGIC, _dyncreate_t<CBinCustomLogic>::CreateObject, 0>,
					   public _dyncreate_t<CBinCustomLogicInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CBinReconstructFore : public CFilter,
			   	  public _dyncreate_t<CBinReconstructFore>
{
public:
	route_unknown();
protected:
	virtual bool InvokeFilter();
	
};

class CBinReconstructForeInfo : public _ainfo_t<ID_ACTION_BIN_RECONSTRUCT_FORE, _dyncreate_t<CBinReconstructFore>::CreateObject, 0>,
					   public _dyncreate_t<CBinReconstructForeInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CBinReconstructBack : public CFilter,
			   	  public _dyncreate_t<CBinReconstructBack>
{
public:
	route_unknown();
protected:
	virtual bool InvokeFilter();
	
};

class CBinReconstructBackInfo : public _ainfo_t<ID_ACTION_BIN_RECONSTRUCT_BACK, _dyncreate_t<CBinReconstructBack>::CreateObject, 0>,
					   public _dyncreate_t<CBinReconstructBackInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CBinDeleteSmallObjects : public CFilter,
			   	  public _dyncreate_t<CBinDeleteSmallObjects>
{
public:
	route_unknown();
protected:
	virtual bool InvokeFilter();
	void FillSmallSegments(byte **buf, int cx, int cy, byte fill_color);
	int m_nMinSize; // минимальный размер оставл€емых объектов
	int m_nMinSize2; // объекты от m_nMinSize до m_nMinSize2 провер€ем по форме
	double m_fFormThreshold;
	int m_nFillValue; // байт дл€ заливки
	int m_nFlags; // битова€ маска:
	// бит 0 Ц удал€ть не мелкие объекты, а большие
	// бит 1 Ц в промежуточном диапазоне (от MinSize до MinSize2) удал€ть не округлые объекты, а выт€нутые
	// бит 2 - работать не по линейному размеру, а по площади (MinSize и MinSize2 - площади)
};

class CBinDeleteSmallObjectsInfo : public _ainfo_t<ID_ACTION_BIN_DELETE_SMALL_OBJECTS, _dyncreate_t<CBinDeleteSmallObjects>::CreateObject, 0>,
					   public _dyncreate_t<CBinDeleteSmallObjectsInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CBinMove : public CInteractiveAction, 
				 public CUndoneableActionImpl,
			   	 public _dyncreate_t<CBinMove>
{
public:
	route_unknown();
public:
	CBinMove();
	virtual ~CBinMove();

	virtual IUnknown *DoGetInterface( const IID &iid );
	virtual bool Initialize();
	virtual void DeInit();
	virtual bool Finalize();

protected:

	virtual bool DoStartTracking( _point point );
	virtual bool DoFinishTracking( _point point );
	virtual bool DoTrack( _point point );
	virtual bool DoLButtonDown( _point point );
	virtual bool DoLButtonDblClick( _point point );
	virtual bool DoChar( int nChar, bool bKeyDown );

	com_call DoInvoke();
	com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
	com_call GetRect( RECT *prect );

	void MoveTo( _point point );

private:
	_point	m_ptOrigOffset;
	_point	m_ptNewOffset;
	_rect	m_rectActiveImage;
	IBinaryImagePtr m_image;
};

class CBinMoveInfo : public _ainfo_t<ID_ACTION_BIN_MOVE, _dyncreate_t<CBinMove>::CreateObject, 0>,
					   public _dyncreate_t<CBinMoveInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};

// MorphDivision

class CBinMorphDivision : public CMorphoFilter,
			   	  public _dyncreate_t<CBinMorphDivision>
{
public:
	route_unknown();
public:

	CBinMorphDivision();
	virtual bool InvokeFilter();
};

class CBinMorphDivisionInfo : public _ainfo_t<ID_ACTION_BIN_MORPH_DIVISION, _dyncreate_t<CBinMorphDivision>::CreateObject, 0>,
					   public _dyncreate_t<CBinMorphDivisionInfo>
{
	route_unknown();
public:
	CBinMorphDivisionInfo()
	{
		return;
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


///////////////////////
//MorphDivision2
//////////////////////
class CBinMorphDivision2 : public CMorphoFilter,
			   	  public _dyncreate_t<CBinMorphDivision2>
{
public:
	route_unknown();
public:

	CBinMorphDivision2();
	virtual bool InvokeFilter();
};
class CBinMorphDivision2Info : public _ainfo_t<ID_ACTION_BIN_MORPH_DIVISION2, _dyncreate_t<CBinMorphDivision2>::CreateObject, 0>,
					   public _dyncreate_t<CBinMorphDivision2Info>
{
	route_unknown();
public:
	CBinMorphDivision2Info()
	{
		return;
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};
////////////////////////////
///SmoothContour
///////////////////////////
class CSmoothContour:	public CFilter, 
						public _dyncreate_t<CSmoothContour>
{
public:
	route_unknown();

	CSmoothContour(){};
	~CSmoothContour(){};
	bool InvokeFilter();
};

class CSmoothContourInfo : public _ainfo_t<ID_ACTION_SMOOTH_CONTOUR, _dyncreate_t<CSmoothContour>::CreateObject, 0>,
					   public _dyncreate_t<CSmoothContour>
{
	route_unknown();
public:
	CSmoothContourInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif //__actions_h__