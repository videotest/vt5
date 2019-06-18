#include "stdafx.h"
#include "AdjustParams.h"

#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"
#include "class_utils.h"
#include "inifile.h"
#include "matrix.h"
#include "object5.h"
#include "docview5.h"

_ainfo_base::arg CAdjustParamsInfo::s_pargs[] =
{
	{"LearningInfo",szArgumentTypeString, "", true, false },
	{"Passes",	szArgumentTypeInt, "5000", true, false },
	{"ObjectList",	szArgumentTypeObjectList, 0, true, true },
	{0, 0, 0, false, false },
};

#define RSF(x) if(!(x)) { dbg(); return 0; };

static char *CutToken(char *pBuf, char cDelimiter)
{	// �������� ����� -���������� ��������� �� ��, ��� ����� �����������,
	// � ������ ����������� ������ '\0'
	// ���� ��������� �� CR/LF - ������� ���, ��� ����� ������
	char *pRet=pBuf;
	while (*pRet)
	{
		if(*pRet=='\r' || *pRet=='\n')
		{
			*pRet=0;
			break;
		}
		if(*pRet == cDelimiter)
		{
			*pRet=0;
			pRet++;
			break;
		}
		pRet++;
	}
	return pRet;
}

static int GetIntArray(char *pBuf, int *pVal, int nMaxVals, char cDelimiter=',')
{	// ���������� ����� �����, ����������� ��������, �� ������
	// ����������, ������� ����� ������
	for(int n=0; (n<nMaxVals) && (*pBuf!=0); n++)
	{
		char *p = pBuf;
		pBuf = CutToken(pBuf, cDelimiter);
        pVal[n]=atoi(p);
	}
	return n;
}

static int GetDoubleArray(char *pBuf, double *pVal, int nMaxVals, char cDelimiter=',')
{	// ���������� ����� �����, ����������� ��������, �� ������
	// ����������, ������� ����� ������
	for(int n=0; (n<nMaxVals) && (*pBuf!=0); n++)
	{
		char *p = pBuf;
		pBuf = CutToken(pBuf, cDelimiter);
        pVal[n]=atof(p);
	}
	return n;
}



/////////////////////////////////////////////////////////////////////////////
// ������ "���������" ��� ��������������

// �������������� ������ ���������
class CTranslation
{
public:
	enum CKind {tkNone=0, tkShift=1, tkScale=2, tkShiftScale=3};
	CKind m_kind;
	double m_fShift;
	double m_fScale;
	int m_nFirst, m_nSecond;
	// 0..npat-1, 0..nObjects-1  -  ������ ��������, ��� ������� ��� ��������� ������ ���������

	CTranslation()
	{ m_kind=tkNone; m_fShift=0; m_fScale=1; };
};

// �������������� ���� ����������
class CMultiTranslation
{
public:
	_ptr_t2<CTranslation> m_Translations;
	CMultiTranslation(int nkeys) : m_Translations(nkeys) {};
	CTranslation& operator[](int nIndex) { return m_Translations[nIndex]; };
};

/////////////////////////////////////////////////////////////////////////////
CAdjustParams::CAdjustParams()
{}

CAdjustParams::~CAdjustParams()
{}

#define BIGBUFSIZE 65536
#define BIGBUFTHRESHOLD 60000
#define MAXKEYS 1024

static double sqr(double x) {return x*x;}

class CFile
{
public:
	CFile(FILE *f) : m_file(f) {};
	~CFile() { Close(); };
	void Close() { if(m_file) { fclose(m_file); m_file=0; } }
	FILE *m_file;
	operator FILE*() { return m_file; };
};

//extract object name from Unknown
static _bstr_t GetObjectName( IUnknown *punk )
{
	INamedObject2Ptr	sptr( punk );
	if (sptr == 0) return "";

	BSTR	bstr = 0;
	if (FAILED(sptr->GetName(&bstr)) || !bstr)
		return "";

	_bstr_t	bstrRes(bstr);
	::SysFreeString(bstr);

	return bstrRes;
}

IUnknown* CAdjustParams::GetContextObject(_bstr_t bstrName, _bstr_t bstrType)
{
	IDataContext3Ptr ptrC = m_ptrTarget;

	if( bstrName==_bstr_t("") )
	{
		IUnknown *punkImage = 0;
		ptrC->GetActiveObject( bstrType, &punkImage );
		//if( punkImage )
			//punkImage->Release();

		return punkImage;
	}

	long lPos = 0;
	ptrC->GetFirstObjectPos( bstrType, &lPos );
	
	while( lPos )
	{
		IUnknown *punkImage = 0;
		ptrC->GetNextObject( bstrType, &lPos, &punkImage );

		if( punkImage )
		{
			if( GetObjectName( punkImage ) == bstrName )
			{
				//if( punkImage )
					//punkImage->Release();
				return punkImage;
			}

			punkImage->Release();
		}
	}
	return 0;
}

HRESULT CAdjustParams::DoInvoke()
{
	if (m_ptrTarget==0) return false;
	IDocumentPtr doc(m_ptrTarget);
	if(doc==0) return S_FALSE;

	int nPasses = GetArgLong("Passes");

	_bstr_t bstrObjectList=GetArgString("ObjectList");
	IUnknown *punk2 = GetContextObject(bstrObjectList, szTypeObjectList);
	INamedDataObject2Ptr sptrList(punk2);
	if (punk2!=0) punk2->Release();

	if( sptrList==NULL)
		return S_FALSE;

	_bstr_t iniLearning=GetArgString("LearningInfo");

	CFile f( fopen(iniLearning,"r") );
	RF(f);
	smart_alloc(buf,char,BIGBUFSIZE); RF(buf!=0);
	smart_alloc(keys,int,MAXKEYS); RF(keys!=0);
	char section[128];
	_ptr_t2 <CVector> in;
	_ptr_t2 <int> in_classes;

	int npat=0, ninp=0;
	int nClasses=0;
	while(1)
	{	//���� ���� ���-�� ��������
		if( ! fgets(buf,BIGBUFSIZE,f) ) break;

		char *name=buf;
		char *val=CutToken(buf,'='); //val - ��, ��� ����� '=', � ��������� name

		if('['==name[0])
		{	// ���� ����� ������ - ���������
			strncpy(section,name,sizeof(section));
			name[0]=0;
		}

		if(0==stricmp("[LearningInfo]", section))
		{
			if(0==stricmp("ObjectsCount", name)) npat=atoi(val);
			if(0==stricmp("Keys", name)) ninp=GetIntArray(val,keys,MAXKEYS);
			if(0==stricmp("ClassCount", name)) nClasses=atoi(val);
			int n = atoi(name)-1;
			if(n>=0 && n<npat)
			{
				if(in.size()!=npat) in.alloc(npat);
				if(in_classes.size()!=npat) in_classes.alloc(npat);

				in[n].Alloc(ninp);
				RF(ninp==in[n].m_size);

				char *val1 = CutToken(val,':');
				int nclass = atoi(val)-1;
				int nn=GetDoubleArray(val1,in[n].m_ptr,in[n].m_size);
				RF(nn==ninp);

                in_classes[n] = nclass;
			}
		}
	}
	f.Close();

	int npat1=npat;
	smart_alloc(in_min,double,ninp);
	smart_alloc(in_max,double,ninp);
	smart_alloc(in_avg,double,ninp);
	smart_alloc(in_dis,double,ninp);
	{for(int j=0; j<ninp; j++)
	{
		in_max[j] = -1e200;
		in_min[j] = 1e200;
		in_avg[j] = 0;
		in_dis[j] = 0;
	}}

	{for(int i=0; i<npat; i++)
	{	// ���������� ����������, ������� �������� ������ �� ������/�������� ������
        RF(in[i].m_size==ninp);
		for(int j=0; j<ninp; j++)
		{
			if(in_max[j]<in[i].m_ptr[j]) in_max[j] = in[i].m_ptr[j];
			if(in_min[j]>in[i].m_ptr[j]) in_min[j] = in[i].m_ptr[j];
			in_avg[j] += in[i].m_ptr[j];
			in_dis[j] += sqr(in[i].m_ptr[j]);
		}
	}}

	smart_alloc(in_shift,double,ninp);
	smart_alloc(in_scale,double,ninp);
	{for(int j=0; j<ninp; j++)
	{
		in_avg[j] /= max(npat,1);
		in_dis[j] /= max(npat,1);
		in_dis[j] = sqrt(max(1e-20,in_dis[j] - sqr(in_avg[j])));
		//in_shift[j] = in_avg[j];
		//in_scale[j] = in_dis[j];
		in_shift[j] = (in_min[j]+in_max[j])/2;
		in_scale[j] = (in_max[j]-in_min[j])/2;
		in_scale[j] = max(in_scale[j], 1e-100);
	}}

	{for(int i=0; i<npat; i++)
	{	// ����������� ������
		for(int j=0; j<ninp; j++)
		{
			//in[i].m_ptr[j] = (in[i].m_ptr[j]-in_shift[j]) / in_scale[j];
		}
	}}

	//��������������
	{	CVector tmp_in(ninp);
		for(int i=0; i<npat; i++)
		{
			int j = rand() % npat;
			RF( tmp_in.Set(in[i]) );
			RF( in[i].Set(in[j]) );
			RF( in[j].Set(tmp_in) );
		}
	}

	//!!!!!!!!!!!!
	// ���������� ������: ������� ���, ��� ������ nclassess ��������� - ������� �� �������
	if(0) if(npat>nClasses)
	{
		smart_alloc(class_counts, int, nClasses);
		for(int k=0; k<nClasses; k++) class_counts[k] = 0;
		for(int i=0; i<npat; i++)
		{
			int k = in_classes[i];
			class_counts[k]++;

			if(in_classes[k] != k)
			{ // ���� ��� �� ��������� ��������������� ������ - ������ �������
				in_classes[i] = in_classes[k];
				in_classes[k] = k;
				for(int j=0; j<ninp; j++)
				{
					double tmp = in[i].m_ptr[j];
					in[i].m_ptr[j] = in[k].m_ptr[j];
					in[k].m_ptr[j] = tmp;
				}
			}

			k = in_classes[i];

			if(i>=nClasses)
			{ // ���� ���� �� ����������������� ������� - ���������� �� � ������
				for(int j=0; j<ninp; j++)
				{
					in[k].m_ptr[j] +=
						(in[i].m_ptr[j]+in[k].m_ptr[j])/class_counts[k];
				}
			}
		}
		npat = nClasses;
	}

	long nObjects;
	sptrList->GetChildsCount(&nObjects);

	_ptr_t2 <CVector> in_new(nObjects);

	long pos = 0, nObj = 0;
	sptrList->GetFirstChildPosition((long*)&pos);
	while (pos && nObj<nObjects)
	{	// ��������� �������, ��������� ������ � ������
		in_new[nObj].Alloc(ninp);
		RF(ninp==in_new[nObj].m_size);

		IUnknownPtr sptr;
		sptrList->GetNextChild((long*)&pos, &sptr);

		ICalcObjectPtr sptrCalc(sptr);
		RF(sptrCalc!=0);
		for(int nParam=0; nParam<ninp; nParam++)
		{
			sptrCalc->GetValue(keys[nParam], in_new[nObj].m_ptr+nParam);
		}
		nObj++;
	}

	// � ���� ������ keys[ninp] - ������ ������
	// inp[npat][ninp] - ������ �������
	// inp_new[nObjects][ninp] - ����� �������

	CMultiTranslation trans(ninp);
	CMultiTranslation trans2(ninp);

	StartNotification(nPasses);

	double q_best = 1e200; // ������� ��� �����;

	int i = rand() % npat;
	int j = rand() % nObjects;
	bool bThereWasImprovement=false;

	for(int iPass=0; iPass<nPasses; iPass++)
	{ // ������ �������� ����������� ������ - �������������� �����?
		Notify(iPass);

		// �������� ����������
		{for(int k=0; k<ninp; k++)
			trans2[k] = trans[k];}

		// ��������� �������� ����������;
		if(!bThereWasImprovement)
		{ // ���� �� ���� ��������� - ����� ����� ����, ����� �������� ����� ��� ����� �� ���������
			i = rand() % npat;
			j = rand() % nObjects;
		}
		//int k = rand() % ninp;
		for(int k=0; k<ninp; k++)
		{
			if( rand() % 15 ) continue; // ������� ����� ���������� �� ������� - ��������� ������ ��������� ����

			if (rand()%2) trans[k].m_kind = CTranslation::tkScale;
			else trans[k].m_kind = CTranslation::tkShift;

			if(iPass==0) 
				trans[k].m_kind = CTranslation::tkNone; // �� ������ ������� ������ �������� ������� ��� �������

			if(trans[k].m_kind == CTranslation::tkScale)
			{
				if( fabs(in[i].m_ptr[k])*1e-2 >= fabs(in_new[j].m_ptr[k]) )
				{ // ���� �� ���������� ��������������� - �� ���-���� ����� �����
					trans[k].m_kind = CTranslation::tkShift;
				}
				else
				{
					trans[k].m_fScale = in[i].m_ptr[k] / in_new[j].m_ptr[k];
					trans[k].m_fShift = 0.0;
				}
			};

			if(trans[k].m_kind == CTranslation::tkShift)
			{
				trans[k].m_fScale = 1.0;
				trans[k].m_fShift = in[i].m_ptr[k] - in_new[j].m_ptr[k];
			};
		}

		// ������ ��������
		double q = 0;
		// ��������� �������� - ��������� ���� �� �����
		int npat_used=0;
		for(int i=0; i<npat; i++)
		{ // ��� ������� ������� �� ������� ������� ������ ����� ������� � ��������
			if(i>50) continue;

			double r_min = 1e100;
			for(int j=0; j<nObjects; j++)
			{
				double r=0;
				for(int k=0; k<ninp; k++)
				{
					double val = in_new[j].m_ptr[k]*trans[k].m_fScale + trans[k].m_fShift;
					r += sqr( (in[i].m_ptr[k]-val) / in_scale[k] );
				}
				if(r<r_min) r_min=r;
			}
			if(r_min>ninp)
			{ // ������ ������ ����� - ��������� ��, ��� ��...
				r_min = ninp;
			}
			q += r_min;
			npat_used ++;
		}

		if(!npat_used) q = 1e200; // ���� �� ���������� - ������ ������
		else q /= npat_used;

		if( (iPass%100==0) || q<q_best )
		{ // ����� �� ������ ����� ���� ��� ��� ����������
			char sz[200];
			sprintf(sz, "trans q: %e (%e)\n", q, q_best);
			OutputDebugString(sz);
		}

		if(q>=q_best)
		{ // ������ ���������� ��������� ���������
			for(int k=0; k<ninp; k++)
				trans[k] = trans2[k];
			bThereWasImprovement = false;
		}
		else
		{
			q_best = q;
			bThereWasImprovement = true;
		}
	}

	// ���������� ���������������:
	{
		long pos = 0, nObj = 0;
		sptrList->GetFirstChildPosition((long*)&pos);
		while (pos && nObj<nObjects)
		{	// ��������� �������, ��������� ������ � ������
			IUnknownPtr sptr;
			sptrList->GetNextChild((long*)&pos, &sptr);

			ICalcObjectPtr sptrCalc(sptr);
			RF(sptrCalc!=0);
			for(int nParam=0; nParam<ninp; nParam++)
			{
				double val = in_new[nObj].m_ptr[nParam]*trans[nParam].m_fScale + trans[nParam].m_fShift;
				double val2 = 0;
				sptrCalc->GetValue(keys[nParam], &val2);
				sptrCalc->SetValue(keys[nParam], val);
			}
			nObj++;
		}
	}

	FinishNotification();

	return true;
}


