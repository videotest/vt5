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
{	// отрезает слово -возвращает указатель на то, что после разделителя,
	// а вместо разделителя ставит '\0'
	// если наткнемся на CR/LF - оформим его, как конец строки
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
{	// подгружает серию чисел, разделенных запятыми, из строки
	// возвращает, сколько чисел прочли
	for(int n=0; (n<nMaxVals) && (*pBuf!=0); n++)
	{
		char *p = pBuf;
		pBuf = CutToken(pBuf, cDelimiter);
        pVal[n]=atoi(p);
	}
	return n;
}

static int GetDoubleArray(char *pBuf, double *pVal, int nMaxVals, char cDelimiter=',')
{	// подгружает серию чисел, разделенных запятыми, из строки
	// возвращает, сколько чисел прочли
	for(int n=0; (n<nMaxVals) && (*pBuf!=0); n++)
	{
		char *p = pBuf;
		pBuf = CutToken(pBuf, cDelimiter);
        pVal[n]=atof(p);
	}
	return n;
}



/////////////////////////////////////////////////////////////////////////////
// классы "хромосомы" для преобразования

// преобразование одного параметра
class CTranslation
{
public:
	enum CKind {tkNone=0, tkShift=1, tkScale=2, tkShiftScale=3};
	CKind m_kind;
	double m_fShift;
	double m_fScale;
	int m_nFirst, m_nSecond;
	// 0..npat-1, 0..nObjects-1  -  номера объектов, для которых эти параметры должны совпадать

	CTranslation()
	{ m_kind=tkNone; m_fShift=0; m_fScale=1; };
};

// преобразование всех параметров
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

	LONG_PTR lPos = 0;
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
	{	//пока хоть что-то читается
		if( ! fgets(buf,BIGBUFSIZE,f) ) break;

		char *name=buf;
		char *val=CutToken(buf,'='); //val - то, что после '=', и ограничим name

		if('['==name[0])
		{	// если метка секции - запомнить
			strncpy(section,name,sizeof(section));
			name[0]=0;
		}

		if(0==_stricmp("[LearningInfo]", section))
		{
			if(0==_stricmp("ObjectsCount", name)) npat=atoi(val);
			if(0==_stricmp("Keys", name)) ninp=GetIntArray(val,keys,MAXKEYS);
			if(0==_stricmp("ClassCount", name)) nClasses=atoi(val);
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
	{	// произведем калибровку, попутно проверим список на пустые/неверные строки
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
	{	// отнормируем данные
		for(int j=0; j<ninp; j++)
		{
			//in[i].m_ptr[j] = (in[i].m_ptr[j]-in_shift[j]) / in_scale[j];
		}
	}}

	//переупорядочим
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
	// Фильтрация данных: сделаем так, что первые nclassess элементов - средние по классам
	if(0) if(npat>nClasses)
	{
		smart_alloc(class_counts, int, nClasses);
		for(int k=0; k<nClasses; k++) class_counts[k] = 0;
		for(int i=0; i<npat; i++)
		{
			int k = in_classes[i];
			class_counts[k]++;

			if(in_classes[k] != k)
			{ // если еще не заполнили соответствующую запись - меняем местами
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
			{ // если идем по незадействованным строкам - прибавляем их к первой
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

	TPOS pos = 0; long nObj = 0;
	sptrList->GetFirstChildPosition(&pos);
	while (pos && nObj<nObjects)
	{	// переберем объекты, переложим данные в массив
		in_new[nObj].Alloc(ninp);
		RF(ninp==in_new[nObj].m_size);

		IUnknownPtr sptr;
		sptrList->GetNextChild(&pos, &sptr);

		ICalcObjectPtr sptrCalc(sptr);
		RF(sptrCalc!=0);
		for(int nParam=0; nParam<ninp; nParam++)
		{
			sptrCalc->GetValue(keys[nParam], in_new[nObj].m_ptr+nParam);
		}
		nObj++;
	}

	// В этот момент keys[ninp] - номера ключей
	// inp[npat][ninp] - старая выборка
	// inp_new[nObjects][ninp] - новая выборка

	CMultiTranslation trans(ninp);
	CMultiTranslation trans2(ninp);

	StartNotification(nPasses);

	double q_best = 1e200; // вначале все плохо;

	int i = rand() % npat;
	int j = rand() % nObjects;
	bool bThereWasImprovement=false;

	for(int iPass=0; iPass<nPasses; iPass++)
	{ // подбор наиболее подходящего сдвига - покоординатный спуск?
		Notify(iPass);

		// сбэкапим трансляцию
		{for(int k=0; k<ninp; k++)
			trans2[k] = trans[k];}

		// попробуем изменить трансляцию;
		if(!bThereWasImprovement)
		{ // если не было улучшения - берем новую пару, иначе пытаемся взять еще ключи по последней
			i = rand() % npat;
			j = rand() % nObjects;
		}
		//int k = rand() % ninp;
		for(int k=0; k<ninp; k++)
		{
			if( rand() % 15 ) continue; // бОльшую часть параметров не трогаем - варьируем только несколько штук

			if (rand()%2) trans[k].m_kind = CTranslation::tkScale;
			else trans[k].m_kind = CTranslation::tkShift;

			if(iPass==0) 
				trans[k].m_kind = CTranslation::tkNone; // на первом проходе только запомним разницу без сдвигов

			if(trans[k].m_kind == CTranslation::tkScale)
			{
				if( fabs(in[i].m_ptr[k])*1e-2 >= fabs(in_new[j].m_ptr[k]) )
				{ // если не получается масштабирование - то все-таки юзать сдвиг
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

		// оценка качества
		double q = 0;
		// тормозной алгоритм - сравнение всех со всеми
		int npat_used=0;
		for(int i=0; i<npat; i++)
		{ // для каждого объекта из входной выборки найдем самый близкий в выходной
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
			{ // совсем убогая точка - отбросить ее, что ли...
				r_min = ninp;
			}
			q += r_min;
			npat_used ++;
		}

		if(!npat_used) q = 1e200; // если не сравнивали - ложный проход
		else q /= npat_used;

		if( (iPass%100==0) || q<q_best )
		{ // отчет на каждом сотом шаге или при улучшениях
			char sz[200];
			sprintf(sz, "trans q: %e (%e)\n", q, q_best);
			OutputDebugString(sz);
		}

		if(q>=q_best)
		{ // вернем предыдущее состояние хромосомы
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

	// собственно масштабирование:
	{
		TPOS pos = 0; long nObj = 0;
		sptrList->GetFirstChildPosition(&pos);
		while (pos && nObj<nObjects)
		{	// переберем объекты, переложим данные в массив
			IUnknownPtr sptr;
			sptrList->GetNextChild(&pos, &sptr);

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


