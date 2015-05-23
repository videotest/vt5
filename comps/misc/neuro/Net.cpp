#include "StdAfx.h"
#include "net.h"

#include "Windows.h"
#include "\vt5\awin\profiler.h"
#include "\vt5\common\alloc.h"

CNet::CNet(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8)
{
	int n[MAXLAYERS]={n0, n1, n2, n3, n4, n5, n6, n7, n8, 0};
	m_nLayers=-1;
	if(n[0]) Create(n);
}

bool CNet::Create(int nNeurons[MAXLAYERS])
{
	int *n=nNeurons;
	for(int i=0; i<=MAXLAYERS; i++)
	{
		if(n[i]==0) break; //последний параметр
		m_nLayers=i;
		m_Y[i].Alloc(n[i]);
		m_S[i].Alloc(n[i]);
		m_E[i].Alloc(n[i]);
		m_Delta[i].Alloc(n[i]);
	}
	for(int i=1; i<=m_nLayers; i++)
	{
		m_W[i].Alloc(n[i],n[i-1]);
	}
	RandomFill(); // начальное состояние - хаотическое
	return true;
}

CNet::~CNet(void)
{
}

CVector* CNet::Process(CVector &in)
{
	RF(in.m_size == m_Y[0].m_size); //проверим соответствие размерностей

	CSigmoid sigmoid;

	RF(m_Y[0].Set(in)); //вход
	for(int n=1; n<=m_nLayers; n++)
	{
		m_S[n].MulMV(m_W[n],m_Y[n-1]); // S[n]=W[n]*Y[n-1]
		m_Y[n].CalcSigmoid(sigmoid, m_S[n]); // Y[i]=f(S[i])
		m_S[n]=m_S[n];
	}

	return &m_Y[m_nLayers];
}

bool CNet::RandomFill()
{
	// проинициализируем матрицы весов
	for(int n=1; n<=m_nLayers; n++) m_W[n].RandomFill(-1,1);
	m_nEpoch=0;
	m_fErr=1;
	return true;
}

bool CNet::Learn(CVector *pin, CVector *pout, int nCount, double fSpeed, int nEpoch)
{
	TIME_TEST("net.Learn");

	int n0=m_Y[0].m_size;

	for(int k=0; k<nCount; k++)
	{	//проверим размерности
		CVector *in=pin+k, *out=pout+k;
		int n1=in->m_size, n2=out->m_size;
		RF(n0==n1 && m_Y[m_nLayers].m_size==n2); //проверим соответствие размерностей
	}

	CSigmoid sigmoid;

	//основной цикл
	for(int epoch=0; epoch<nEpoch; epoch++)
	{
		double err=0;
		double cerr=0; //ошибки классификации
		for(int k=0; k<nCount; k++)
		{
			CVector *in=pin+k, *out=pout+k;

			Process(*in); // обработаем

			m_E[m_nLayers].Sub(m_Y[m_nLayers],*out); //E[N]=...
			for(int n=m_nLayers; n>0; n--)
			{
				m_Delta[n].MulSigmoidDer(sigmoid,m_S[n],m_E[n]); //Delta[i]=f'(S[i])*E[i]
				m_E[n-1].MulVM(m_Delta[n],m_W[n]); // E[n-1]=Delta[n]*W[n]
				m_W[n].CorrectByTensMul(m_Delta[n],m_Y[n-1],-fSpeed); //W[n]=W[n]-speed*Delta[n]*Y[n-1]
			}

			//test error
			double err1=0;
			for(int j=0; j<m_E[m_nLayers].m_size; j++)
			{
				double e = m_E[m_nLayers].m_ptr[j];
				err1 += e*e;
			}
			err1 /= m_E[m_nLayers].m_size;
			err += sqrt(err1);

			int c2=-1;
			double m2=-1;
			for(int j=0; j<m_Y[m_nLayers].m_size; j++)
			{
				if(m_Y[m_nLayers].m_ptr[j]>m2)
				{
					c2=j;
					m2=m_Y[m_nLayers].m_ptr[j];
				};
			}
			if(out->m_ptr[c2]<0.5) cerr += 1;
		}
		err /= nCount;
		cerr /= nCount;

		m_nEpoch++;
		m_fErr = err;

		if(m_nEpoch%10==0)
		{
			//printf("epoch %i: error %e / %e\n",epoch,err,cerr);

			char	sz[200];
			::sprintf( sz, "epoch %i: error %e / %e\n",m_nEpoch,err,cerr);
			::OutputDebugString( sz );
		}
	}

	return true;
}

bool CNet::Test(CVector *pin, CVector *pout, int nCount, double *perr, double *pcerr)
{
	RF(pin!=0 && pout!=0 && nCount>0);

	int n0=m_Y[0].m_size;

	for(int k=0; k<nCount; k++)
	{	//проверим размерности
		CVector *in=pin+k, *out=pout+k;
		int n1=in->m_size, n2=out->m_size;
		RF(n0==n1 && m_Y[m_nLayers].m_size==n2); //проверим соответствие размерностей
	}

	double err=0;
	double cerr=0; //ошибки классификации
	for(int k=0; k<nCount; k++)
	{
		CVector *in=pin+k, *out=pout+k;

		Process(*in); // обработаем

		//test error
		double err1=0;
		for(int j=0; j<m_E[m_nLayers].m_size; j++)
		{
			double e = m_E[m_nLayers].m_ptr[j];
			err1 += e*e;
		}
		err1 /= m_E[m_nLayers].m_size;
		err += sqrt(err1);

		int c2=-1;
		double m2=-1;
		for(int j=0; j<m_Y[m_nLayers].m_size; j++)
		{
			if(m_Y[m_nLayers].m_ptr[j]>m2)
			{
				c2=j;
				m2=m_Y[m_nLayers].m_ptr[j];
			};
		}
		if(out->m_ptr[c2]<0.5) cerr += 1;
	}
	err /= nCount;
	cerr /= nCount;

	if(perr) *perr = err;
	if(pcerr) *pcerr = cerr;

	return true;
}

#define BIGBUFSIZE 65536
#define BIGBUFTHRESHOLD 60000

bool CNet::SaveIni(CIniFile *pIni)
{
	RF(pIni);

	_ptr_t2<char> name_buf(256);
	_ptr_t2<char> big_buf(BIGBUFSIZE);
	int big_buf_pos=0;

	_itoa(m_nLayers, big_buf, 16);
	pIni->SetVal("Network","Layers",big_buf);

	big_buf_pos=0;
	for (int i=0; i<=m_nLayers; i++)
	{
		big_buf_pos += sprintf(big_buf+big_buf_pos, "%i ", m_Y[i].m_size);
		RF(big_buf_pos<BIGBUFTHRESHOLD);
	}
	if(big_buf_pos>0 && ' '==big_buf[big_buf_pos-1])
		big_buf[big_buf_pos-1]=0;
	pIni->SetVal("Network","Neurons",big_buf);

	for (int i=1; i<=m_nLayers; i++)
	{
		//fprintf(f, "# Layer %i\n", i);
		for(int y=0; y<m_W[i].m_ny; y++)
		{
			big_buf_pos=0;
			for(int x=0; x<m_W[i].m_nx; x++)
			{
				big_buf_pos += sprintf(big_buf+big_buf_pos, "%g ", m_W[i].elem(y,x));
				RF(big_buf_pos<BIGBUFTHRESHOLD);
			}
			sprintf(name_buf,"Data_%i_%i",i,y);
			if(big_buf_pos>0 && ' '==big_buf[big_buf_pos-1])
				big_buf[big_buf_pos-1]=0;
			pIni->SetVal("Network",name_buf,big_buf);
		}
	}

	return true;
}

bool CNet::Save(char *filename)
{
	CIniFile ini;
	ini.Open(filename);
	SaveIni(&ini);
	ini.Close();
	return true;
}

bool CNet::LoadIni(CIniFile *pIni)
{
	RF(pIni);

	_ptr_t2<char> name_buf(256);
	_ptr_t2<char> big_buf(BIGBUFSIZE);

	int nLayers=0, nNeurons[MAXLAYERS]={0};

	pIni->GetVal("Network","Layers",big_buf,BIGBUFTHRESHOLD);
	nLayers = atoi(big_buf);
	RF(nLayers>=1 && nLayers<=MAXLAYERS);

	pIni->GetVal("Network","Neurons",big_buf,BIGBUFTHRESHOLD);
	char *buf=big_buf;
	for (int i=0; i<=nLayers; i++)
	{
		while(*buf==' ' || *buf=='\t') buf++;
		RF(*buf!=0);
		char *buf1=buf;
		while(*buf1!=' ' && *buf1!='\t' && buf1!=0) buf1++;
		//*buf1=0;
		nNeurons[i]=atoi(buf);
		buf=buf1+1;
	}
	Create(nNeurons);

	for (int i=1; i<=m_nLayers; i++)
	{
		for(int y=0; y<m_W[i].m_ny; y++)
		{
			sprintf(name_buf,"Data_%i_%i",i,y);
			pIni->GetVal("Network",name_buf,big_buf,BIGBUFTHRESHOLD);
			char *buf=big_buf;
			for(int x=0; x<m_W[i].m_nx; x++)
			{
				while(*buf==' ' || *buf=='\t') buf++;
				RF(*buf!=0);
				char *buf1=buf;
				while(*buf1!=' ' && *buf1!='\t' && buf1!=0) buf1++;
				//*buf1=0;
				m_W[i].elem(y,x)=atof(buf);
				buf=buf1+1;
			}
			sprintf(name_buf,"Data_%i_%i",i,y);
			pIni->SetVal("Network",name_buf,big_buf);
		}
	}

	return true;
}

bool CNet::Load(char *filename)
{
	CIniFile ini;
	ini.Open(filename);
	LoadIni(&ini);
	ini.Close();
	return true;
}