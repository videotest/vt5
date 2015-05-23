#pragma once



struct Contour;
struct CntrPoint:
	public ContourPoint
{
	double s;
	double chainCode;
};
struct CntrPointF
{
	int s;
	double value;
};
class ContourArray:
	public CArray<CntrPoint>
{
public:
	CntrPoint& GetAt(int i);
	CntrPoint& operator[](int i);
};
class CChCodedContour
{
	ContourArray m_points;
	CArray<double> m_graphic; 
	int m_nContourSize;
	int m_nExtent;
	void modify();
	int get_angle_code(POINT pt1, POINT pt2);
	int get_chain_code(ContourPoint p1, ContourPoint p2);
	void resolve_discontinuities();
	void resolve_protrusions();
	void resolve_path();
	void conv(CArray<double>* in1, CArray<double>* in2, CArray<double>* out);
	void expand_contour(CArray<double>& out);
	void gauss(CArray<double>& out, int k, int n =0);
	void filter(CArray<double>& arr, int k, int n);
	void diff(CArray<double>& out);
	/*Contour* expand_contour(Contour* c);*/
public:
	CChCodedContour(IUnknown* object, int nExt =30);
	CChCodedContour(Contour* c, int nExt);
	~CChCodedContour(void);
	int GetContourSize(){return m_nContourSize;};
	void Smooth(double sigma);
	void Derivative();

	void Build();	
	CArray<double>& GetArray(){return m_graphic;};

};
