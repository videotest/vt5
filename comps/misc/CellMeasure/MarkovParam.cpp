#include "StdAfx.h"
#include "markovparam.h"
#include "math.h"

CMarkovParamCalculator::CMarkovParamCalculator(void)
{
	m_nLevelsPhases = 0;
	for(int i=0; i<256; i++)
		m_LevelNum[i] = i * nEqualColors / 256;
}

CMarkovParamCalculator::~CMarkovParamCalculator(void)
{
}

void CMarkovParamCalculator::CalcLevels(color **ppc, byte **ppm, int cx, int cy, int phase)
{ // Рассчитать уровни для equalize
	//{
	//	char s[200];
	//	sprintf(s,"CalcLevels(%d)", phase);
	//	MessageBox(0,s,"CMarkovParamCalculator",0);
	//}
	m_nLevelsPhases = phase;
	// обсчитаем гистограмму
	int phase_min = phase & 255;
	int phase_max = phase_min + (phase>>8)&255;
	for(int i=0; i<256; i++) m_Hist[i]=0;
	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(ppm[y][x]>=phase_min && ppm[y][x]<=phase_max)
			{
				m_Hist[ ppc[y][x] >> 8 ] ++;
			}
		}
	}

	int sum=0;
	for(int i=0; i<256; i++) sum += m_Hist[i];
	sum = sum+1;

	// а теперь вычислим номера уровней
	int s=0;
	for(int i=0; i<256; i++)
	{
		m_LevelNum[i] = s * nEqualColors / sum;
		ASSERT(m_LevelNum[i]<nEqualColors);
		s += m_Hist[i];
	}
}

void CMarkovParamCalculator::CalcMatrix(color **ppc, byte **ppm, int cx, int cy, int phase)
{	// Расчет матрицы смежности
	// Предполагается, что уровни заранее посчитаны
	//{
	//	char s[200];
	//	sprintf(s,"CalcMatrix(%d)", phase);
	//	MessageBox(0,s,"CMarkovParamCalculator",0);
	//}
	int phase_min = phase & 255;
	int phase_max = phase_min + (phase>>8)&255;

	const int nn=nEqualColors*nEqualColors;
	for(int i=0; i<nn; i++) m_Matrix[i]=0;

	double fStep = ::GetValueDouble( ::GetAppUnknown(), "\\CellMeasGroup", "MarkovianStep", 1.0);
	int h = max(1, int(fStep));
	int h2 = max(1, int(fStep/sqrt(2.0)));

	static int dx[] = { 1, 1, 0,-1,-1,-1, 0, 1, 1};
	static int dy[] = { 0, 1, 1, 1, 0,-1,-1,-1, 0};
	static int dx1[] = { 1, 1, 0,-1,-1,-1, 0, 1, 1};
	static int dy1[] = { 0, 1, 1, 1, 0,-1,-1,-1, 0};

	dx[0] = h; dy[0] = 0;
	dx[1] = h2; dy[1] = h2;
	dx[2] = 0; dy[2] = h;
	dx[3] = -h2; dy[3] = h2;

	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(ppm[y][x]>=phase_min && ppm[y][x]<=phase_max)
			{
				int c1 = m_LevelNum[ ppc[y][x]>>8 ];
				ASSERT(c1>=0 && c1<nEqualColors);
				for(int i=0; i<8; i++)
				{ // делаю не оптимально, но надежно
					int x2=x+dx[i], y2=y+dy[i];
					if(x2>=0 && x2<cx && y2>=0 && y2<cy &&
						ppm[y2][x2]>=phase_min && ppm[y2][x2]<=phase_max)
					{
						int c2 = m_LevelNum[ ppc[y2][x2]>>8 ];
						// ----------
						// добавляем код для max/min по отрезку
						/*
						int c2_min=c2, c2_max=c2;
						int hh = max(abs(dx[i]),abs(dy[i]));
						for(int k=1; k<hh; k++)
						{
							int x3=x+dx1[i]*k, y3=y+dy1[i]*k;
                            if(ppm[y3][x3]>=phase_min && ppm[y3][x3]<=phase_max)
							{
								int c3 = m_LevelNum[ ppc[y3][x3]>>8 ];
								c2_min = min(c2_min,c3);
								c2_max = max(c2_min,c3);
							}
						}
						// А теперь возьмем в качестве c2 наиболее удаленную от c точку - как там этот фильтр называется...
						c2 = (c2_max-c1>c1-c2_min) ? c2_max : c2_min;
						*/
						// ---------- конец кода для max/min

						ASSERT(c2>=0 && c2<nEqualColors);
						m_Matrix[c1*nEqualColors+c2] ++ ;
					}
				}
			}
		}
	}

	/*
	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(ppm[y][x]>=phase_min && ppm[y][x]<=phase_max)
			{
				int c1 = m_LevelNum[ ppc[y][x]>>8 ];
				ppc[y][x] = c1*65535/(nEqualColors-1);
			}
		}
	}
	*/
}

void CMarkovParamCalculator::CalcParams()
{ // посчитать параметры по уже готовой матрице
	//{
	//	char s[200];
	//	sprintf(s,"CalcParams");
	//	MessageBox(0,s,"CMarkovParamCalculator",0);
	//}
	m_fInertia = m_fEnergy = m_fEntropy = m_fCorrelation = m_fHomogenity = 0;

	double sum=0;
	double mi=0, mj=0; // средние значения i,j
	double mi2=0, mj2=0; // стандартные отклонения i,j
	for(int i=0; i<nEqualColors; i++)
	{
		for(int j=0; j<nEqualColors; j++)
		{
			double w = m_Matrix[i*nEqualColors+j];
			sum += w;
			mi += w*i;
			mj += w*j;
			mi2 += w*i*i;
			mj2 += w*j*j;
		}
	}
	sum=max(1,sum);
	mi/=sum; mj/=sum;
	mi2/=sum; mj2/=sum;

	mi2 = sqrt(max(0,mi2-mi*mi));
	mj2 = sqrt(max(0,mj2-mj*mj));

	// собственно расчет параметров
	for(int i=0; i<nEqualColors; i++)
	{
		for(int j=0; j<nEqualColors; j++)
		{
			double w = m_Matrix[i*nEqualColors+j]/sum; // сразу вероятность - чтоб потом не париться
			m_fInertia += w*(i-j)*(i-j);
			m_fEnergy += w*w;
			if(w>0) m_fEntropy += - w * log(w);
			m_fCorrelation += w*(i-mi)*(j-mj);
			m_fHomogenity += w / (1 + (i-j)*(i-j));
		}
	}
	m_fCorrelation /= max(1e-10,mi2);
	m_fCorrelation /= max(1e-10,mj2);
}
