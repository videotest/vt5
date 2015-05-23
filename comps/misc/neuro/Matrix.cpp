#include "StdAfx.h"
#include "Matrix.h"

void dbg()
{
	return;
}

CVector::CVector(int n)
{
	m_size=0;
	m_ptr=0;
	Alloc(n);
}

CVector::~CVector(void)
{
	Free();
}

void CVector::Alloc(int n)
{
	Free();
	m_ptr = new double[n];
	if(m_ptr!=0)
	{
		m_size=n;
	}
	return;
}

void CVector::Free()
{
	if(m_ptr) delete [] m_ptr;
	m_ptr = 0;
	m_size=0;
}

bool CVector::Set(CVector &arg)
{	//присваивание
	int n=m_size;
	if(arg.m_size!=n) return false;
	double *pa=arg.m_ptr;
	for(int x=0; x<n; x++) m_ptr[x] = pa[x];
	return true;
}

bool CVector::Sub(CVector &V1, CVector &V2)
{	//this=V1-V2
	RF(V1.m_size==m_size && V2.m_size==m_size); //проверим размерности

	for(int x=0; x<m_size; x++) m_ptr[x] = V1.m_ptr[x]-V2.m_ptr[x];
	return true;
}

bool CVector::MulMV(CMatrix &M, CVector &V)
{	//this=M*V; false - ошибка
	int ny=m_size, nx=V.m_size;
	RF(ny==M.m_ny && nx==M.m_nx);

	double *pr=m_ptr, *pa=V.m_ptr;
	for(int y=0; y<ny; y++)
	{
		pr[y]=0;
		for(int x=0; x<nx; x++) pr[y] += pa[x]*M.elem(y,x);
	}
	return true;
}

bool CVector::MulVM(CVector &V, CMatrix &M)
{	//this=V*M; false - ошибка
	int nx=m_size, ny=V.m_size;
	RF(ny==M.m_ny && nx==M.m_nx);

	double *pr=m_ptr, *pa=V.m_ptr;
	for(int x=0; x<nx; x++)
	{
		pr[x]=0;
		for(int y=0; y<ny; y++) pr[x] += pa[y]*M.elem(y,x);
	}
	return true;
}

bool CVector::CalcSigmoid(CSigmoid &S, CVector &V)
{	// this[i]=S(V[i]);
	RF(V.m_size==m_size);
	double *pr=m_ptr, *pa=V.m_ptr;
	for(int i=0; i<m_size; i++) pr[i] = S.Value(pa[i]);
	return true;
}

bool CVector::MulSigmoidDer(CSigmoid &S, CVector &V1, CVector &V2)
{	// this[i]=S'(V1[i])*V2[i]
	RF(V1.m_size==m_size && V2.m_size==m_size);
	double *pr=m_ptr, *pa1=V1.m_ptr, *pa2=V2.m_ptr;
	for(int i=0; i<m_size; i++) pr[i] = S.Derivative(pa1[i])*pa2[i];
	return true;
}

CMatrix::CMatrix(int ny, int nx)
{
	m_nx=0;
	m_ny=0;
	m_ptr=0;
	Alloc(ny,nx);
}

CMatrix::~CMatrix(void)
{
	Free();
}

void CMatrix::Alloc(int ny, int nx)
{
	Free();
	m_ptr = new double[nx*ny];
	if(m_ptr!=0)
	{
		m_nx=nx;
		m_ny=ny;
	}
	return;
}

void CMatrix::Free()
{
	if(m_ptr) delete [] m_ptr;
	m_ptr = 0;
	m_nx=0;
	m_ny=0;
}

bool CMatrix::TensMul(CVector &v1, CVector &v2)
{	//M=v1*v2 (тензорное произведение)
	if(v1.m_size!=m_ny) return false; // v1 - строки
	if(v2.m_size!=m_nx) return false; // v2 - столбцы

	double *p1=v1.m_ptr, *p2=v2.m_ptr;
	for(int y=0; y<m_ny; y++)
		for(int x=0; x<m_nx; x++) elem(y,x) = p1[y]*p2[x];

	return true;
}

bool CMatrix::CorrectByTensMul(CVector &v1, CVector &v2, double coef)
{ //M = M+c*[v1*v2]
	if(v1.m_size!=m_ny) return false; // v1 - строки
	if(v2.m_size!=m_nx) return false; // v2 - столбцы

	double *p1=v1.m_ptr, *p2=v2.m_ptr;
	for(int y=0; y<m_ny; y++)
		for(int x=0; x<m_nx; x++) elem(y,x) += coef*p1[y]*p2[x];

	return true;
}

bool CMatrix::RandomFill(double vmin, double vmax)
{	//заполнить случайными числами от vmin до vmax
	for(int y=0; y<m_ny; y++)				  
		for(int x=0; x<m_nx; x++)
		{
			elem(y,x) = (rand()%4096)/4096.0*(vmax-vmin)+vmin;
		}
	return true;
}
