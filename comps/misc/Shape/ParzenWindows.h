#pragma once
class CStringX;
struct PRZPT
{
	double value;
};
class ParamEst:
	public CArray<PRZPT>
{
	double m_weight;
	double m_Min;
	int m_key;
public:
	double& Weight(){return m_weight;};
	double& Min(){return m_Min;};
	int& Key(){return m_key;};
};
class ParzenWindows
{
	CArray<ParamEst> m_params;
	CList<double> m_gauss;
	void calc_gauss();
	int m_nN;
	int m_nD;
	double m_h;
	/*double m_Min;*/
	/*void to_mat(ParamEst& pe, int n);*/
public:
	ParzenWindows(void);
	~ParzenWindows(void);
	//построение плотности вероятности для каждого параметра
	void AttachDescriptors(const CArray<CStringX>& descr);
	double EstimateDescriptor(const double d, int key);
};
