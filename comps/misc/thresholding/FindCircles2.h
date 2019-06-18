#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#pragma once

/////////////////////////////////////////////////////////////////////////////
class CFindCircles2 : public CFilter,
					public _dyncreate_t<CFindCircles2>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired()			{return false;};
	virtual bool CanDeleteArgument( CFilterArgument *pa );
	virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images
protected:
	IImage3Ptr m_sptrSrc; // ������� �����������
	INamedDataObject2Ptr m_sptrList; // ����� - ������ ��������

	color** m_ppSrc; // ������� ������
	byte** m_ppBin; // ������� ������ - binary
	float** m_ppDist; // ���������� �� ����
	int** m_ppSeed; // ����� ������� ���������

	color** m_ppDst; // ���������
	int m_cx, m_cy; // ������� �����������
	int m_nRadius; // ������������ ������ ������� �����������

	const int static m_na=16;
	const int static m_nr=16;
	double m_co[m_na], m_si[m_na];
	double m_rr[m_nr];
	int m_dx[m_na*m_nr], m_dy[m_na*m_nr];

	bool m_bSmoothContour; // ���������� �� ������� (������� ����������� �����)
	bool m_bCorrectByBin; // �������������� �� ������� �� binary
	
protected:
	// ���� ������� ��� �������������� ������� � ������ � �������
	inline double j2r(int j) { return 2.0 + j*(m_nRadius-2.0)/(m_nr-1); };
	inline int r2j(double r) { return int( (r-2.0)*(m_nr-1)/(m_nRadius-2.0) + 0.5 ); };

	void CalcDistFromBinBound(); // ���������� ������� ���������� �� ���� Binary
	double CheckCircle(int x, int y, int j); // ��������� ����� (x,y) �� ������� ���������� ������� rr[j]; ���������� "��������"
	double FindSpot(int x, int y, int* jj); // ����� ��������� �����
	double FindSpot2(int x, int y, int* jj_max, int* jj_wanted, int* jj, double wanted_dev=1.0, int *px=0, int *py=0); // ����� ��������� �����,
	// ��� ���� �� ����������� ������������� �������� ������ � �� ����������� ������������
	// ���� ������ px � py - ������������ ���������� ���������� ������ �����
	double SmoothContour(int x, int y, int* jj, int nArcLength, int nHandleLength);
	// ������������ ������ - ������� ������������ ����� �� ����� ��������
	INamedDataObject2Ptr CreateObject(int x, int y, int* jj);
};

//////////////////////////////////////////////////////////////////////

class CFindCircles2Info : public _ainfo_t<ID_ACTION_FINDCIRCLES2, _dyncreate_t<CFindCircles2>::CreateObject, 0>,
							public _dyncreate_t<CFindCircles2Info>
{
	route_unknown();
public:
	CFindCircles2Info()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


