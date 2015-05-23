#pragma once

namespace smooth
{
#define OUTER 64
#define INNER 32

	class CContour: public CWalkFillInfo
	{
		CFilterBase* filter;
		IBinaryImage* bi;
		RECT  GetRect(IBinaryImage* bin);
		Contour* redraw(Contour* c);
		int cx,cy;
		int maskSize;
		void FillContour(byte **dst, int cx, int cy, ContourPoint *pPoints, int n, byte color);
	public:
		CContour(CFilterBase* f, IBinaryImage* bin, int maskSize);
		~CContour();


		// производит сглаживание контуров
		void Gauss(void);
		bool Build();
		void Fill();
		void GetRow(byte** row, int y);
		void MarkContourAvoidConnect(Contour* pcntr);



	};
	inline bool operator ==(ContourPoint p1, ContourPoint p2)
	{
		return (p1.x==p2.x && p1.y == p2.y);
	}
}


class CSmoothContourBin: public CFilterBase
{
	DECLARE_DYNCREATE(CSmoothContourBin)
	GUARD_DECLARE_OLECREATE(CSmoothContourBin)

public:
	CSmoothContourBin(void);
	virtual ~CSmoothContourBin(void);

	public:
	virtual bool InvokeFilter();
	//BT 5359: virtual bool CanDeleteArgument( CFilterArgument *pa ){return false;};
};
