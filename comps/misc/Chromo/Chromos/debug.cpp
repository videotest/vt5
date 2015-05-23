#include "StdAfx.h"
#include "Debug.h"
#include "Settings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString g_sFileName;

void dprintf(char * szFormat, ...)
{
	static bool bCalled = false;
	if (!bCalled)
	{
		if (g_sFileName.IsEmpty())
			g_sFileName = _T("c:\\chromos.log");
		remove(g_sFileName);
		bCalled = true;
	}
	if (g_Debug == 1)
	{
		FILE *f = fopen(g_sFileName, "at");
		if (!f)
			f = fopen(g_sFileName, "wt");
		if (!f) return;
		va_list va;
		va_start(va, szFormat);
		vfprintf(f, szFormat, va);
		va_end(va);
		fclose(f);
	}
}

void Dump_Axis(LPCTSTR pszFirst, IChromoAxis2 *pAxis)
{
	if (g_Debug != 1) return;
	dprintf("\n%s\n", pszFirst);
	FPOINT fpOffs = pAxis->GetOffset();
	int nScale = pAxis->GetScale();
	dprintf("Offset = (%f,%f), Scale = %d\n", fpOffs.x, fpOffs.y, nScale);
	int nSize = pAxis->GetSize();
	AxisType Type = pAxis->GetType();
	if (Type == IntegerAxis)
	{
		POINT *pt = (POINT *)pAxis->GetData();
		for (int i = 0; i < nSize; i++)
			dprintf("(%d,%d)\n", pt[i].x, pt[i].y);
	}
	else
	{
		FPOINT *pt = (FPOINT *)pAxis->GetData();
		for (int i = 0; i < nSize; i++)
			dprintf("(%f,%f)\n", pt[i].x, pt[i].y);
	}
	dprintf("\n");
}

void Dump_Interval_Image(LPCTSTR pszFirst, INTERVALIMAGE *pIntImg)
{
	if (g_Debug != 1) return;
	dprintf("\n%s\n", pszFirst);
	dprintf("Offset = (%d,%d), Size = (%d,%d),  nRows = %d\n", pIntImg->ptOffset.x, pIntImg->ptOffset.y,
		pIntImg->szInt.cx, pIntImg->szInt.cy, pIntImg->nrows);
	for (int i = 0; i < pIntImg->nrows; i++)
	{
		dprintf("%d: ", i);
		for (int j = 0; j < pIntImg->prows[i].nints; j++)
			dprintf("(%d-%d) ", pIntImg->prows[i].pints[j].left, pIntImg->prows[i].pints[j].right);
		dprintf("\n");
	}
}

