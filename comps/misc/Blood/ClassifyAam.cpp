#include "stdafx.h"
#include "ClassifyAam.h"
#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"
#include "class_utils.h"

_ainfo_base::arg CClassifyAamInfo::s_pargs[] =
{
	{"ClassifyInfo",szArgumentTypeString, "", true, false },
	{"ObjectList",	szArgumentTypeObjectList, 0, true, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg CLearnAamInfo::s_pargs[] =
{
	{"ClassifyInfo",szArgumentTypeString, "", true, false },
	{"ObjectList",	szArgumentTypeObjectList, 0, true, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg CLearnLinkAamInfo::s_pargs[] =
{
	{"ClassifyInfo",szArgumentTypeString, "", true, false },
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////
CClassifyAam::CClassifyAam()
{}

CClassifyAam::~CClassifyAam()
{}

static void ReadData(char *sz_ini, int nclass, int nkeys,
					 long *keys, double *count, double *avg, double *disp)
{
	smart_alloc(big_buf,char,64000);

	GetPrivateProfileString("ClassifyInfo", "Keys", "", big_buf, 64000, sz_ini);
	{
		char *s=big_buf;
		for(int i=0;i<nkeys;i++)
		{
			sscanf(s, "%i",keys+i);
			s=strchr(s,',')+1;
		}
	}

	for(int i = 0; i < nclass; i++)
	{
		char sz_cl[10];
		_itoa(i+1, sz_cl, 10);
		GetPrivateProfileString("ClassifyInfo", sz_cl, "", big_buf, 64000, sz_ini);
		char *s=big_buf;

		count[i]=atof(s);
		s=strchr(s,',')+1;

		for(int j=0;j<nkeys;j++)
		{
			s=strchr(s,'(')+1;
			avg[i*nkeys+j]=atof(s);
			s=strchr(s,',')+1;
			disp[i*nkeys+j]=atof(s);
			s=strchr(s,')')+1;
			s=strchr(s,',')+1;
		}
		for (j = 0; j < nkeys; j++) disp[i*nkeys+j]=max(disp[i*nkeys+j],1e-10);
	}
}

static void WriteData(char *sz_ini, int nclass, int nkeys,
					 long *keys, double *count, double *avg, double *disp)
{
	smart_alloc(big_buf,char,64000);

	WritePrivateProfileSection("ClassifyInfo", "", sz_ini); //очистим секцию

	_itoa(nclass,big_buf,10);
	WritePrivateProfileString("ClassifyInfo", "ClassCount", big_buf, sz_ini);

	_itoa(nkeys,big_buf,10);
	WritePrivateProfileString("ClassifyInfo", "KeyCount", big_buf, sz_ini);

	{
		char *buf=big_buf;
		for(int j=0;j<nkeys;j++)
		{
			if(j>0) buf+=sprintf(buf,",");
			buf+=sprintf(buf,"%d",keys[j]);
		}
		WritePrivateProfileString("ClassifyInfo", "Keys", big_buf, sz_ini);
	}

	for(int i=0;i<nclass;i++)
	{
		char *buf=big_buf;
		buf+=sprintf(buf,"%f",count[i]);
		for (int j = 0; j < nkeys; j++)
		{
			buf+=sprintf(buf,", (%g,%g)",avg[i*nkeys+j],disp[i*nkeys+j]);
		}
		char sz_cl[10];
		_itoa(i+1, sz_cl, 10);
		WritePrivateProfileString("ClassifyInfo", sz_cl, big_buf, sz_ini);
	}
}

static void AvgToSum(char *sz_ini, int nclass, int nkeys,
					 long *keys, double *count, double *avg, double *disp)
{  //перейти от средних значений и дисперсий к суммам и суммам квадратов
	for(int i = 0; i < nclass; i++)
	{
		for(int j=0;j<nkeys;j++)
		{
			int n=i*nkeys+j;
			disp[n]+=avg[n]*avg[n];
			avg[n]*=count[i];
			disp[n]*=count[i];
		}
	}
}

static void SumToAvg(char *sz_ini, int nclass, int nkeys,
					 long *keys, double *count, double *avg, double *disp)
{  //перейти от сумм и сумм квадратов к средним значениям и дисперсиям
	for(int i = 0; i < nclass; i++)
	{
		if(count[i]>0.1)
		{
			for(int j=0;j<nkeys;j++)
			{
				int n=i*nkeys+j;
				avg[n]/=count[i];
				disp[n]/=count[i];
				disp[n]-=avg[n]*avg[n];
			}
		}
	}
}


static void DoClassify(IUnknownPtr punkObjList, char *sz_ini)
{
	sptrINamedDataObject2 sptrN(punkObjList); // Object list
	if(sptrN==0) return;

	int nclass = ::GetPrivateProfileInt("ClassifyInfo", "ClassCount", 0, sz_ini);
	int nkeys = ::GetPrivateProfileInt("ClassifyInfo", "KeyCount", 0, sz_ini);
	if (nclass == 0 || nkeys == 0) return; 

	_ptr_t2<long> keys(nkeys);	
	_ptr_t2<double> count(nclass);
	_ptr_t2<double> avg(nclass*nkeys);
	_ptr_t2<double> disp(nclass*nkeys);
	_ptr_t2<double> sum(nclass);

	ReadData(sz_ini, nclass, nkeys, keys, count, avg, disp);

	double GarbageThreshold=GetValueDouble
		(GetAppUnknown(), "\\CellClassify", "GarbageThreshold", 5.0 );

	long pos;
	sptrN->GetFirstChildPosition((long*)&pos);
	while (pos)
	{
		long pos1 = pos;
		IUnknownPtr sptr;
		sptrN->GetNextChild((long*)&pos, &sptr);
		ICalcObjectPtr sptrObj = ICalcObjectPtr(sptr);
		if(sptrObj!=0) //ну мало ли объекты какие-нибудь не такие...
		{
			for (int i = 0; i < nclass; i++) sum[i]=0;
			for (int j = 0; j < nkeys; j++)
			{
				double val;
				sptrObj->GetValue( keys[j], &val );
				for (int i = 0; i < nclass; i++)
				{
					double d=val-avg[i*nkeys+j];
					sum[i]+=d*d/disp[i*nkeys+j];
				}
			}
			int i_best=0;
			for (i = 0; i < nclass; i++)
			{
				if(sum[i]<sum[i_best]) i_best=i;
			}
			if(sum[i_best]>GarbageThreshold*nclass) i_best=-1;
			set_object_class(sptrObj, i_best);
		}
	}
}

static void DoLearn(IUnknownPtr punkObjList, char *sz_ini)
{
	sptrINamedDataObject2 sptrN(punkObjList); // Object list
	if(sptrN==0) return;

	ICalcObjectContainerPtr	sptrC(punkObjList);
	if(sptrC==0) return; //убедимся, что это список с параметрами

	int nclass = ::class_count();

	long nkeys=0;
	sptrC->GetParameterCount(&nkeys);

	_ptr_t2<long> keys(nkeys);	
	_ptr_t2<double> count(nclass);
	_ptr_t2<double> avg(nclass*nkeys);
	_ptr_t2<double> disp(nclass*nkeys);

	long lPos; sptrC->GetFirstParameterPos(&lPos);
	for (int i = 0; i < nkeys; i++)
	{
		ParameterContainer	*pc = 0;
		sptrC->GetNextParameter(&lPos, &pc);
		keys[i]=pc->pDescr->lKey;
	}

	{for(int i=0; i<nclass; i++) count[i]=0;}
	{for(int i=0; i<nclass*nkeys; i++) avg[i]=0;}
	{for(int i=0; i<nclass*nkeys; i++) disp[i]=0;}

	_ptr_t2<double> sum(nclass);

	AvgToSum(sz_ini, nclass, nkeys, keys, count, avg, disp);

	long pos;
	sptrN->GetFirstChildPosition((long*)&pos);
	while (pos)
	{
		long pos1 = pos;
		IUnknownPtr sptr;
		sptrN->GetNextChild((long*)&pos, &sptr);
		ICalcObjectPtr sptrObj = ICalcObjectPtr(sptr);
		if(sptrObj!=0) //ну мало ли объекты какие-нибудь не такие...
		{
			int i=::get_object_class(sptrObj);
			if(i>=0 && i<nclass)
			{
				for (int j = 0; j < nkeys; j++)
				{
					double val;
					sptrObj->GetValue( keys[j], &val );
					avg[i*nkeys+j]+=val;
					disp[i*nkeys+j]+=val*val;
				}
				count[i]++;
			}
		}
	}
	SumToAvg(sz_ini, nclass, nkeys, keys, count, avg, disp);

	WriteData(sz_ini, nclass, nkeys, keys, count, avg, disp);
}

static void DoLink(char *sz_ini)
{
	int nfiles = ::GetPrivateProfileInt("Sources", "SourcesCount", 0, sz_ini);
	if(nfiles<1) return;

	char sz_ini2[MAX_PATH];
	GetPrivateProfileString("Sources", "Source0", "", sz_ini2, MAX_PATH, sz_ini);
	if(!sz_ini2) return;
	if(!*sz_ini2) return; //первое имя должно быть задано

	int nclass = ::GetPrivateProfileInt("ClassifyInfo", "ClassCount", 0, sz_ini2);
	int nkeys = ::GetPrivateProfileInt("ClassifyInfo", "KeyCount", 0, sz_ini2);

	_ptr_t2<long> keys(nkeys);	
	_ptr_t2<double> count(nclass);
	_ptr_t2<double> avg(nclass*nkeys);
	_ptr_t2<double> disp(nclass*nkeys);

	_ptr_t2<long> keys2(nkeys);	
	_ptr_t2<double> count2(nclass);
	_ptr_t2<double> avg2(nclass*nkeys);
	_ptr_t2<double> disp2(nclass*nkeys);

	//вначале настроимся по первому исходнику
	ReadData(sz_ini2, nclass, nkeys, keys, count, avg, disp); 
	AvgToSum(sz_ini2, nclass, nkeys, keys, count, avg, disp);

	for(int n=1; n<nfiles; n++)
	{
		char sz_num[64];
		sprintf(sz_num,"Source%d",n);
		GetPrivateProfileString("Sources", sz_num, "", sz_ini2, MAX_PATH, sz_ini);

		if(!sz_ini2) continue;
		if(!*sz_ini2) continue; //скипаем отсутствующие файлы
		int nclass2 = ::GetPrivateProfileInt("ClassifyInfo", "ClassCount", 0, sz_ini2);
		int nkeys2 = ::GetPrivateProfileInt("ClassifyInfo", "KeyCount", 0, sz_ini2);

		if(nclass2!=nclass || nkeys2!=nkeys) continue;

		ReadData(sz_ini2, nclass2, nkeys2, keys2, count2, avg2, disp2); 

		bool bWrongKeys=false;
		for(int j=0; j<nkeys; j++)
		{
			if(keys[j]!=keys2[j]) bWrongKeys = true; 
		}
		if(bWrongKeys) continue; //skip, если ключи не совпадают

		AvgToSum(sz_ini2, nclass2, nkeys2, keys2, count2, avg2, disp2);
		for(int i = 0; i < nclass; i++)
		{
			for(int j=0;j<nkeys;j++)
			{
				int n=i*nkeys+j;
				avg[i*nkeys+j]+=avg2[i*nkeys+j];
				disp[i*nkeys+j]+=disp2[i*nkeys+j];
			}
			count[i]+=count2[i];
		}
		
	}

	SumToAvg(sz_ini, nclass, nkeys, keys, count, avg, disp);
	WriteData(sz_ini, nclass, nkeys, keys, count, avg, disp);
}

bool CClassifyAam::InvokeFilter()
{
	// Find object list
	IUnknownPtr punkObjList(GetDataArgument());
	if(punkObjList==0) return false;

	_bstr_t iniName=GetArgString("ClassifyInfo");
	if(iniName==_bstr_t(""))
	{ //имя инишки не задано - берем из shell.data
		char	sz_ini[MAX_PATH];
		local_filename(sz_ini, "ClassifyInfo.ini", sizeof(sz_ini)); //получим имя файла ClassifyInfo.ini
		strcpy( sz_ini, (const char*)::GetValueString(GetAppUnknown(), "\\Classes", "ClassifyInfo", sz_ini ));
		iniName=sz_ini;	
	}

	// Process classification itself.
	DoClassify(punkObjList, iniName);

	// Now all OK. Redraw document.
	INamedDataObject2Ptr NDO(punkObjList);
	IUnknown *punk = NULL;
	HRESULT hr = NDO->GetData(&punk);
	if (SUCCEEDED(hr) && punk)
	{
		long l = cncReset;
		INotifyControllerPtr sptr(punk);
		sptr->FireEvent(_bstr_t(szEventChangeObjectList), NULL, NDO, &l, 0);
		punk->Release();
	}
	SetModified(true);
	return true;
}



//-------------- Learn action ---------------
/////////////////////////////////////////////////////////////////////////////
CLearnAam::CLearnAam()
{}

/////////////////////////////////////////////////////////////////////////////
CLearnAam::~CLearnAam()
{}

bool CLearnAam::InvokeFilter()
{
	// Find object list
	IUnknownPtr punkObjList(GetDataArgument());
	if(punkObjList==0) return false;

	_bstr_t iniName=GetArgString("ClassifyInfo");
	if(iniName==_bstr_t(""))
	{ //имя инишки не задано - берем из shell.data
		char	sz_ini[MAX_PATH];
		local_filename(sz_ini, "ClassifyInfo.ini", sizeof(sz_ini)); //получим имя файла ClassifyInfo.ini
		strcpy( sz_ini, (const char*)::GetValueString(GetAppUnknown(), "\\Classes", "ClassifyInfo", sz_ini ));
		iniName=sz_ini;	
	}
	
	// Process learning itself.
	DoLearn(punkObjList, iniName);
	return true;
}


//-------------- LearnLink action ---------------
/////////////////////////////////////////////////////////////////////////////
CLearnLinkAam::CLearnLinkAam()
{}

/////////////////////////////////////////////////////////////////////////////
CLearnLinkAam::~CLearnLinkAam()
{}

HRESULT CLearnLinkAam::DoInvoke()
{
	_bstr_t iniName=GetArgString("ClassifyInfo");
	if(iniName==_bstr_t(""))
	{ //имя инишки не задано - берем из shell.data
		char	sz_ini[MAX_PATH];
		local_filename(sz_ini, "ClassifyInfo.ini", sizeof(sz_ini)); //получим имя файла ClassifyInfo.ini
		strcpy( sz_ini, (const char*)::GetValueString(GetAppUnknown(), "\\Classes", "ClassifyInfo", sz_ini ));
		iniName=sz_ini;
	}
	DoLink(iniName);

	return S_OK;
}
