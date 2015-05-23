#include "stdafx.h"
#include "transformcolor1.h"
#include "misc_utils.h"
#include <math.h>

#include "nameconsts.h"
#include "ImagePane.h"
#include "binimageint.h"
#include "core5.h"
#include "alloc.h"
#include "stdio.h"

/////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CTransformColor1Info::s_pargs[] = 
{
	{"Flags",		szArgumentTypeInt, "0", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"TransformColor1",szArgumentTypeImage, 0, false, true },
	{0, 0, 0, false, false },
};

static bool MatrixSet1(double* m, int n)
{ // m2 := E
    for(int i=0; i<n*n; i++) m[i] = 0;
    for(int i=0; i<n; i++) m[i*n+i] = 1;
	return true;
}

static bool MatrixCpy(double* m2, double* m1, int n)
{ // m2 := m1
    for(int i=0; i<n*n; i++) m2[i] = m1[i];
	return true;
}

static bool MatrixTrans(double* m2, double* m1, int n)
{ // m2 := m1^T, допустимо транспонирование на месте
	for(int i=0; i<n; i++)
	{
		for(int j=0; j<=i; j++)
		{
			double tmp = m1[i*n+j];
			m2[i*n+j] = m1[j*n+i];
			m2[j*n+i] = tmp;
		}
	}
	return true;
}

static bool MatrixMulConst(double* m, int n, double f)
{ // m := f*m
    for(int i=0; i<n*n; i++) m[i] *= f;
}

static bool MatrixMul(double* m1, double* m2, double* m3, int n)
{	// m1 := m2*m3
	for(int i=0; i<n; i++)
	{
		for(int j=0; j<n; j++)
		{
			m1[i*n+j] = 0;
			for(int k=0; k<n; k++)
				m1[i*n+j] += m2[i*n+k]*m3[k*n+j];
		}
	}

	return true;
}

static bool MatrixDiv(double* m2, double* m1, int n)
{	// m2 := m2*m1^-1 // алгоритм без выбора ведущего элемента!!! И вообще не оптимизирован нифига - лишь бы попроще
	// m1 портится (превращается в единичную)
	// и вообще я не уверен, умножаем на m2^-1 слева или справа - ну да пофиг, матрицы у нас симметричные
	for(int i=0; i<n; i++) // идем по строкам
	{
		if(fabs(m1[i*n+i])<1e-20) return false; // ошибка
		double d = 1.0/m1[i*n+i];
		for(int k=0; k<n; k++)
		{ // сделаем ведущий элемент = 1
			m1[i*n+k] *= d;
			m2[i*n+k] *= d;
		}
		for(int j=i+1; j<n; j++) // обрабатываем все строки ниже текущей
		{
			double d = m1[j*n+i];
			for(int k=0; k<n; k++)
			{ // вычтем из j-строки i-строку, умноженную на m1[j,i] (обнулим i-столбец)
				m1[j*n+k] -= m1[i*n+k]*d;
				m2[j*n+k] -= m2[i*n+k]*d;
			}
		}
	}

	// обратный ход метода гаусса
	for(int i=n-1; i>=0; i--) // идем по диагонали
	{
		for(int j=0; j<i; j++) // обрабатываем все строки выше текущей
		{
			double d = m1[j*n+i]; // элемент, который будем обнулять
			for(int k=0; k<n; k++)
			{ // вычтем из j-строки i-строку, умноженную на m1[j,i] (обнулим i-столбец)
				m1[j*n+k] -= m1[i*n+k]*d;
				m2[j*n+k] -= m2[i*n+k]*d;
			}
		}
	}

	return true;
}

static double inline sqr(double x)
{ return x*x; }

bool CTransformColor1::InvokeFilter()
{
	IImage3Ptr	ptrSrc( GetDataArgument() );
	IImage3Ptr	ptrDst( GetDataResult() );
	long nFlags = GetArgLong("Flags");
			
	if( ptrSrc == NULL || ptrDst==NULL )
		return false;

	bool bDisableCreate = (nFlags & 16) != 0; // запрет на создание результирующего изображения - только подсчет статистики

	int cx,cy;
	ptrSrc->GetSize(&cx,&cy);
	int nPanes = ::GetImagePaneCount( ptrSrc );

	if(!bDisableCreate)
	{	
		ICompatibleObjectPtr ptrCO( ptrDst );
		if( ptrCO == NULL ) return false;
		if( S_OK != ptrCO->CreateCompatibleObject( ptrSrc, NULL, 0 ) )
			return false;
	}

	try
	{
		StartNotification( cy, 2 );

		smart_alloc(m, double, nPanes*nPanes); // матрица инерции
		smart_alloc(m2, double, nPanes*nPanes); // корень из матрицы инерции (для старого изобр-я)
		smart_alloc(v, double, nPanes);	// вектор средних цветов (для старого изобр-я)
		smart_alloc(m3, double, nPanes*nPanes);
		smart_alloc(m4, double, nPanes*nPanes);

		smart_alloc(src, color*, nPanes);
		smart_alloc(dst, color*, nPanes);

		if(nFlags&1) // 1 - не считать цвета, а взять из shell.data
		{
			int nSrcPanes = ::GetValueInt(::GetAppUnknown(), "\\TransformColor", "SrcPanes", 0);
			if( nPanes!=nSrcPanes) // Защита от дурака - если разное количество пан, работать не можем
				return false;

			char sz[100];
			for( int i=0; i<nPanes; i++ )
			{
				for( int j=0; j<nPanes; j++ )
				{
					sprintf(sz, "a_%u_%u", i, j);					
					m2[i*nPanes+j] = 
						::GetValueDouble(::GetAppUnknown(), "\\TransformColor", sz, i==j ? 1 : 0 );
				}
			}
			for( int i=0; i<nPanes; i++ )
			{
				sprintf(sz, "u_%u", i);					
				v[i] = 
					::GetValueDouble(::GetAppUnknown(), "\\TransformColor", sz, 32768);
			}
		}
		else
		{ // обсчитать матрицу инерции, извлечь корень из нее
			for(int i=0; i<nPanes*nPanes; i++) 
				m[i] = 0;
			for(int i=0; i<nPanes; i++) 
				v[i] = 0;

			int nModel = ::GetValueInt(::GetAppUnknown(), "\\TransformColor", "Model", 0 );
			// Номер принятой для работы модели распределения цвета:
			// 0 - модель эллипсоида в пространстве RGB (по умолчанию)
			// 1 - модель двух ведущих цветов
			// 2 - модель светлого фона (диагональная матрица, фиксированный черный)

			if(nModel==2)
			{ // 2 - модель светлого фона (диагональная матрица, фиксированный черный)
				smart_alloc(c, double, nPanes);
				smart_alloc(s, double, nPanes);
				double cnt = 0;
				for(int j=0; j<nPanes; j++)
				{
					c[j] = 0;
					s[j] = c[j] * cnt;
				}

				for( int y=0; y<cy; y++ )
				{
					byte *pm;
					ptrSrc->GetRowMask( y, &pm );
					for( int nPane=0; nPane<nPanes; nPane++ )
						ptrSrc->GetRow(y, nPane, &src[nPane]);

					for(int x=0; x<cx; x++)
					{
						if( pm[x] >= 128 )
						{
							double r=0, r1=0;
							for( int i=0; i<nPanes; i++ )
							{
								r += sqr(src[i][x]);
							}
							r = sqrt(r);
							double w = exp(r/(256*16)); // примем характерное изменение цвета в 16 единиц
							cnt += w;
							for( int i=0; i<nPanes; i++ )
							{
								s[i] += w*src[i][x];
							}
						}
					}
					for(int i=0; i<nPanes; i++)
					{
						c[i] = s[i]/cnt;
					}
					Notify(y);
				}

				// построим диагональную матрицу
				bool bFixedBlack = ::GetValueInt(::GetAppUnknown(), "\\TransformColor", "FixedBlack", 0 ) != 0;
				for( int i=0; i<nPanes; i++ )
				{
					v[i] = 0;
					for( int j=0; j<nPanes; j++ )
					{
						m[i*nPanes+j] = 0;
					}
					m[i*nPanes+i] = sqr(c[i]/32768); // на всякий случай добавка к диагонали, как от легкого некореллированного по каналам шума - для устойчивости
				}

			}
			else if(nModel==1)
			{ // 1 - модель двух ведущих цветов
				smart_alloc(c0, double, nPanes);
				smart_alloc(c1, double, nPanes);
				smart_alloc(s0, double, nPanes);
				smart_alloc(s1, double, nPanes);
				double cnt0 = cx; // ну, якобы уже столько точек было - для затравки
				double cnt1 = cx;
				for(int j=0; j<nPanes; j++)
				{
					c0[j] = 0;
					c1[j] = 65535;
					s0[j] = c0[j] * cnt0;
					s1[j] = c1[j] * cnt1;
				}

				smart_alloc(y_i, int, cy); // для перестановки строк
				for(int y=0; y<cy; y++) y_i[y] = y;
				for(int y=0; y<cy; y++)
				{ // подготовим перестановку
					int i = rand() % cy;
					int tmp = y_i[i];
					y_i[i] = y_i[y];
					y_i[y] = tmp;
				}

				for( int y=0; y<cy; y++ )
				{
					byte *pm;
					ptrSrc->GetRowMask( y, &pm );
					for( int nPane=0; nPane<nPanes; nPane++ )
						ptrSrc->GetRow(y, nPane, &src[nPane]);

					double r=0;
					for(int i=0; i<nPanes; i++)
						r += sqr(double(c1[i]) - c0[i]);
					r = sqrt(r);

					for(int x=0; x<cx; x++)
					{
						if( pm[x] >= 128 )
						{
							double r0=0, r1=0;
							for( int i=0; i<nPanes; i++ )
							{
								double c = src[i][x];
								r0 += sqr(c - c0[i]);
								r1 += sqr(c - c1[i]);
							}
							r0 = sqrt(r0);
							r1 = sqrt(r1);
							double w0 = r1/(r+r0);
							double w1 = r0/(r+r1);
							cnt0 += w0;
							cnt1 += w1;
							for( int i=0; i<nPanes; i++ )
							{
								s0[i] += w0*src[i][x];
								s1[i] += w1*src[i][x];
							}
						}
					}
					for(int i=0; i<nPanes; i++)
					{
						c0[i] = s0[i]/cnt0;
						c1[i] = s1[i]/cnt1;
					}
					Notify(y);
				}

				bool bFixedBlack = ::GetValueInt(::GetAppUnknown(), "\\TransformColor", "FixedBlack", 0 ) != 0;
				for( int i=0; i<nPanes; i++ )
				{
					v[i] = (c0[i]+c1[i]) / 2;
					for( int j=0; j<nPanes; j++ )
					{
						//m[i*nPanes+j] = c0[i]*c0[j] + c1[i]*c1[j];
						m[i*nPanes+j] = 0;
					}
					//m[i*nPanes+i] += sqr(8*256); // на всякий случай добавка к диагонали, как от легкого некореллированного по каналам шума - для устойчивости
					m[i*nPanes+i] = sqr((c0[i]+c1[i])/16384); // на всякий случай добавка к диагонали, как от легкого некореллированного по каналам шума - для устойчивости
				}
			}
			else
			{ // 0 - модель эллипсоида в пространстве RGB (по умолчанию)
				long n=0;
				for( int y=0; y<cy; y++ )
				{
					byte *pm;
					ptrSrc->GetRowMask( y, &pm );
					for( int nPane=0; nPane<nPanes; nPane++ )
						ptrSrc->GetRow(y, nPane, &src[nPane]);
					for(int x=0; x<cx; x++)
					{
						if( *pm >= 128 )
						{
							for( int i=0; i<nPanes; i++ )
							{
								v[i] += *src[i];
								for( int j=0; j<nPanes; j++ )
								{
									m[i*nPanes+j] += double(*src[i]) * double(*src[j]);
								}
							}
							n++; // счетчик пикселей
						}
						for( int i=0; i<nPanes; i++ ) src[i]++;
						pm++;
					}
					Notify(y);
				}
				// приведем все к средним
				for( int i=0; i<nPanes; i++ )
				{
					v[i] /= n;
					for( int j=0; j<nPanes; j++ )
					{
						m[i*nPanes+j] /= n;
					}
				}
			}


			bool bFixedBlack = ::GetValueInt(::GetAppUnknown(), "\\TransformColor", "FixedBlack", 0 ) != 0;
			// перейдем к ковариациям
			for( int i=0; i<nPanes; i++ )
			{
				for( int j=0; j<nPanes; j++ )
				{
					if(!bFixedBlack)
						m[i*nPanes+j] -= v[i]*v[j];
					m[i*nPanes+j] /= 16384*16384;
				}
			}
			if(bFixedBlack)
			{
				for( int i=0; i<nPanes; i++ ) 
					v[i] = 0;
			}

			// заполним матрицу m2 (будущий корень из m) как единичную
			MatrixSet1(m2, nPanes);

			for(int pass=0; pass<20; pass++)
			{ // собственно извлечение корня
				MatrixCpy(m3, m, nPanes); // m3 = m
				MatrixTrans(m4, m2, nPanes); // m4 = m2^T
				MatrixDiv(m3, m4, nPanes); // m3 = m/(m2^T), m4=0
				for(int i=0; i<nPanes*nPanes; i++) m2[i] = ( m2[i] + m3[i] ) / 2;
			}
		}

		smart_alloc(v2, double, nPanes);
		for( int i=0; i<nPanes; i++ )
			v2[i] = 32768;
		MatrixSet1(m4, nPanes); // по умолчанию выходная матрица = E

		if(nFlags&2) // 2 - выходная матрица из shell.data
		{
			int nDstPanes = ::GetValueInt(::GetAppUnknown(), "\\TransformColor", "DstPanes", 0);
			if( nPanes!=nDstPanes ) // Защита от дурака - если разное количество пан, работать не можем
				return false;

			char sz[100];
			for( int i=0; i<nPanes; i++ )
			{
				for( int j=0; j<nPanes; j++ )
				{
					sprintf(sz, "b_%u_%u", i, j);					
					m4[i*nPanes+j] = 
						::GetValueDouble(::GetAppUnknown(), "\\TransformColor", sz, m4[i*nPanes+j] );
				}
			}
			for( int i=0; i<nPanes; i++ )
			{
				sprintf(sz, "v_%u", i);					
				v2[i] = 
					::GetValueDouble(::GetAppUnknown(), "\\TransformColor", sz, 32768);
			}
		}

		MatrixCpy(m3, m2, nPanes);
		MatrixTrans(m4, m4, nPanes);
		MatrixDiv(m4, m3, nPanes); // m4 = выходн/входн
		MatrixTrans(m4, m4, nPanes);

		NextNotificationStage();

		if( !bDisableCreate ) // только если надо создавать выходное изображение
		{
		for( int y=0; y<cy; y++ )
		{
			byte *pm;
			ptrSrc->GetRowMask( y, &pm );
			for( int nPane=0; nPane<nPanes; nPane++ )
			{
				ptrSrc->GetRow(y, nPane, &src[nPane]);
				ptrDst->GetRow(y, nPane, &dst[nPane]);
			}
			for(int x=0; x<cx; x++)
			{
				if( *pm >= 128 )
				{
					for( int i=0; i<nPanes; i++ )
					{
						double s=0;
						for( int j=0; j<nPanes; j++ )
						{
							s += m4[i*nPanes+j] * (*src[j]-v[j]);
						}
						s = v2[i] + s;
						s = max(0,min(65535,s));
						*dst[i] = color(s);
					}
				}
				pm++;
				for( int i=0; i<nPanes; i++ ) src[i]++;
				for( int i=0; i<nPanes; i++ ) dst[i]++;
			}
			Notify(y);
		}
		}

		if(nFlags&4) // 4 - сохранять текущие цвета как "старые" (u,a)
		{
			char sz[100];
			for( int i=0; i<nPanes; i++ )
			{
				for( int j=0; j<nPanes; j++ )
				{
					sprintf(sz, "a_%u_%u", i, j);					
					::SetValue(::GetAppUnknown(), "\\TransformColor", sz, m2[i*nPanes+j]);
				}
			}
			for( int i=0; i<nPanes; i++ )
			{
				sprintf(sz, "u_%u", i);
				::SetValue(::GetAppUnknown(), "\\TransformColor", sz, v[i]);
			}
			::SetValue(::GetAppUnknown(), "\\TransformColor", "SrcPanes", long(nPanes));
		}
		if(nFlags&8) // 8 - сохранять текущие цвета как "новые" (v,b)
		{
			char sz[100];
			for( int i=0; i<nPanes; i++ )
			{
				for( int j=0; j<nPanes; j++ )
				{
					sprintf(sz, "b_%u_%u", i, j);					
					::SetValue(::GetAppUnknown(), "\\TransformColor", sz, m2[i*nPanes+j]);
				}
			}
			for( int i=0; i<nPanes; i++ )
			{
				sprintf(sz, "v_%u", i);
				::SetValue(::GetAppUnknown(), "\\TransformColor", sz, v[i]);
			}
			::SetValue(::GetAppUnknown(), "\\TransformColor", "DstPanes", long(nPanes));
		}

		if(bDisableCreate)
			return false; // если запрещено создание - притворимся, что фильтр не сработал

		FinishNotification();
	}
	catch( ... )
	{
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////

