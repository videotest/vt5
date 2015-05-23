#pragma once
#include "Sigmoid.h"

#define RF(x) if(!(x)) { dbg(); return 0; };
void dbg();

class CMatrix;

class CVector
{
public:
	CVector(int n=0);
	~CVector(void);
	void Alloc(int n);
	void Free();
	bool Set(CVector &arg); //присваивание
	bool Sub(CVector &V1, CVector &V2); //this=V1-V2
	bool MulMV(CMatrix &M, CVector &V); //this=M*V; false - ошибка
	bool MulVM(CVector &V, CMatrix &M); //this=V*M; false - ошибка
	bool CalcSigmoid(CSigmoid &S, CVector &V); // this[i]=S(V[i])
	bool MulSigmoidDer(CSigmoid &S, CVector &V1, CVector &V2); // this[i]=S'(V1[i])*V2[i]
	int m_size;
	double *m_ptr;
};

class CMatrix
{
public:
	CMatrix(int ny=0, int nx=0);
	~CMatrix(void);
	void Alloc(int ny, int nx);
	void Free();
	int m_nx,m_ny;
	double *m_ptr;
	double &elem(int y, int x) { return m_ptr[y*m_nx+x]; };
	bool TensMul(CVector &v1, CVector &v2); //M=[v1*v2] (тензорное произведение); false - ошибка
	bool CorrectByTensMul(CVector &v1, CVector &v2, double coef); //M = M+c*[v1*v2]
	bool RandomFill(double vmin, double vmax); //заполнить случайными числами от vmin до vmax
};
