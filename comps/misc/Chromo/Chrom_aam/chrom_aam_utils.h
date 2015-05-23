// SelectByAxisFilter.h: interface for the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////
#include "action_filter.h"
//#include "Filter.h"
#include "image5.h"
#include "misc_utils.h"
#include "dpoint.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "action_interactive.h"
#include "Chromosome.h"

#if !defined(AFX_chrom_aam_utils_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_chrom_aam_utils_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

IUnknown * FindComponentByName(IUnknown *punkThis, const IID iid, LPCTSTR szName);
void CalcObject(IUnknown* punkCalc, IUnknown* punkImage, bool bSetParams, ICompManager *pGroupManager );
void ConditionalCalcObject(IUnknown* punkCalc, IUnknown* punkImage, bool bSetParams, ICompManager *pGroupManager );

void FireEvent( IUnknown *punkCtrl, const char *pszEvent, IUnknown *punkFrom, IUnknown *punkData, void *pdata, long cbSize );

void DoExecute(LPCTSTR pActionName, LPCTSTR pParams);

class _fpoint : public FPOINT
{
public:
	_fpoint(int x, int y)
	{
		this->x = x;
		this->y = y;
	};
	_fpoint(double x, double y)
	{
		this->x = x;
		this->y = y;
	};
	_fpoint(POINT pt)
	{
		x = pt.x;
		y = pt.y;
	}
	_fpoint(FPOINT pt)
	{
		x = pt.x;
		y = pt.y;
	}
	_fpoint operator=(FPOINT pt)
	{
		x = pt.x;
		y = pt.y;
		return *this;
	}
	_fpoint operator=(_point pt)
	{
		x = pt.x;
		y = pt.y;
		return *this;
	}
	bool operator==(FPOINT pt)
	{
		return x==pt.x&&y==pt.y;
	}
	operator _point()
	{
		return _point((int)x,(int)y);
	}
};
#define INVCOORD  -10000
#define INVPOINT  _point(INVCOORD,INVCOORD)
#define INVFPOINT _fpoint(INVCOORD,INVCOORD)
#define INVANGLE  -10000.

#endif // !defined(AFX_chrom_aam_utils_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
