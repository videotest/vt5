#ifndef __FindCells_h__
#define __FindCells_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "action_filter.h"
//#include "FilterMorphoBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"

/////////////////////////////////////////////////////////////////////////////
// CFilterNegative class

class CFindCells : public CAction,
	public CUndoneableActionImpl,
	public CLongOperationImpl,
	public _dyncreate_t<CFindCells>
{

#define CSize SIZE

public:
	route_unknown();
	virtual IUnknown *DoGetInterface( const IID &iid );

	CFindCells();
	~CFindCells();

	com_call DoInvoke();
	// virtual bool InvokeFilter();
	// virtual bool CanDeleteArgument( CFilterArgument *pa ){return false;};

private:
	IUnknown *GetContextObject(_bstr_t bstrName, _bstr_t bstrType);
	void Smoothing( BYTE *p_arg, BYTE *p_res , int cx , int cy, int Mask);
	bool SelectCell(color **src, byte **dst, int x0, int y0, CSize size, RECT *dstRect);
	void ExpandDarkArea(color **src, byte **dst, CSize size, int tolerance);
	void ExpandSelectedArea(color **src, byte **dst, CSize size, int tolerance);
	void SelectByRadius(color **src, byte **dst, CSize size, int tolerance, RECT *dstRect);
	void SelectByRadius2(color **src, byte **dst, CSize size, int tolerance, RECT *dstRect);
	void DeleteSmallSegments(byte **dst, CSize size);
	void CalcHist(color **src, int cx, int cy, int *hist);
	void CalcBack2(int *hist, color &back1, color &back2, color &threshold);
	void LoadIni();
	void Calibrate(color **src, byte **bin, int cx, int cy);
	//void DilateColor(BYTE **sel, int cx, int cy, int mask, byte cFore,  byte cForeNew, byte cBack);

	int m_SmoothSize;
	int m_EvenSize;
	int m_FindAreaSize;
	int m_FindThreshold;
	bool m_bShowCross;
	
	int m_NucleiTolerance;
	int m_NucleiAddBlack;
	int m_CitoplasmaTolerance;
	int m_MinNucleiArea;
	int m_MinNucleiSplitter;
	int m_NucleiCloseSize;
	int m_MaxCellRadius;
	int m_CellCircleRadius;
	int m_FramePosX;
	int m_FramePosY;
	int m_MinCellDistance;
	bool m_TestOnly; // только проверить наличие ядер (для быстрого теста на наличие лейкоцитов), не вносить ничего в список

	int *px, *py; //кольцевой буфер координат точек
	int m_PointBufSize;
	void CheckBufSize(); //проверить размер буфера под точки, если близки к пределу - увеличить.

	int nInserted; //сколько точек добавлено в список
	int nInserted0; //сколько точек было добавлено в список в начале выделения
	int nDone; //сколько точек обработано
	bool m_bCellTooBig;
	double colorSum; //сумма цветов по добавленным точкам
	double colorSumR;
	double colorSumG;
	double colorSumB;
	double colorSumBack; //сумма цветов по добавленным точкам
	int nBackCount;
	int gradLimit;
	int fillColor;

	color backR1;
	color backG1;
	color backB1;

	color backR2;
	color backG2;
	color backB2;

	_point m_ptNucleusCenter; // центр последнего найденного ядра (заполняется SelectByRadius)
}; 

//---------------------------------------------------------------------------
class CFindCellsInfo : public _ainfo_t<ID_ACTION_FINDCELLS, _dyncreate_t<CFindCells>::CreateObject, 0>,
public _dyncreate_t<CFindCellsInfo>
{
        route_unknown();
public:
        CFindCellsInfo()
        {
        }
        arg *args() {return s_pargs;}
        virtual _bstr_t target()  {return "anydoc";}
        static arg s_pargs[];
};


#endif //__FindCells_h__
