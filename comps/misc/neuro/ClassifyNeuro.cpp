#include "stdafx.h"
#include "ClassifyNeuro.h"

#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"
#include "class_utils.h"
#include "inifile.h"
#include "matrix.h"

_ainfo_base::arg CClassifyNeuroInfo::s_pargs[] =
{
	{"ClassifyInfo",szArgumentTypeString, "", true, false },
	{"ObjectList",	szArgumentTypeObjectList, 0, true, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg CTeachNeuroInfo::s_pargs[] =
{
	{"LearningInfo",szArgumentTypeString, "", true, false },
	{"ClassifyInfo",szArgumentTypeString, "", true, false },
	{0, 0, 0, false, false },
};

#define RSF(x) if(!(x)) { dbg(); return 0; };

static char *CutToken(char *pBuf, char cDelimiter)
{	// отрезает слово -возвращает указатель на то, что после разделител€,
	// а вместо разделител€ ставит '\0'
	// если наткнемс€ на CR/LF - оформим его, как конец строки
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
{	// подгружает серию чисел, разделенных зап€тыми, из строки
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
{	// подгружает серию чисел, разделенных зап€тыми, из строки
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
#define BIGBUFSIZE 65536
#define BIGBUFTHRESHOLD 60000

static bool DoClassify(IUnknownPtr punkObjList, char *sz_ini)
{
	sptrINamedDataObject2 sptrN(punkObjList); // Object list
	if(sptrN==0) return 0;

	CNet net;
	CIniFile ini;
	ini.Open(sz_ini);
	RF (net.LoadIni(&ini));

	int nclass = net.m_Y[net.m_nLayers].m_size;
	int nkeys = net.m_Y[0].m_size;

	if (nclass == 0 || nkeys == 0) return 0; 

	_ptr_t2<int> keys(nkeys);	

	_ptr_t2<char> big_buf(BIGBUFSIZE);
	ini.GetVal("Classifier","Keys",big_buf,BIGBUFTHRESHOLD);
	RF( nkeys == GetIntArray(big_buf,keys,nkeys,' ') );

	smart_alloc(in_shift,double,nkeys);
	smart_alloc(in_scale,double,nkeys);

	ini.GetVal("Calibration","Shift",big_buf,BIGBUFTHRESHOLD);
	RF( nkeys == GetDoubleArray(big_buf,in_shift,nkeys,' ') );

	ini.GetVal("Calibration","Scale",big_buf,BIGBUFTHRESHOLD);
	RF( nkeys == GetDoubleArray(big_buf,in_scale,nkeys,' ') );

	ini.GetVal("Calibration","OutScale",big_buf,BIGBUFTHRESHOLD);
	long lOutScale = atoi(big_buf);

	ini.Close();

	long lObjCount=0;
	sptrN->GetChildsCount(&lObjCount);

	CVector in;
	in.Alloc(nkeys);

	//double GarbageThreshold=GetValueDouble
	//	(GetAppUnknown(), "\\CellClassify", "GarbageThreshold", 5.0 );

	TPOS pos;
	sptrN->GetFirstChildPosition((TPOS*)&pos);
	while (pos)
	{
		TPOS pos1 = pos;
		IUnknownPtr sptr;
		sptrN->GetNextChild((TPOS*)&pos, &sptr);
		ICalcObjectPtr sptrObj = ICalcObjectPtr(sptr);
		if(sptrObj!=0) //ну мало ли объекты какие-нибудь не такие...
		{
			for (int j = 0; j < nkeys; j++)
			{
				double val=0;
				sptrObj->GetValue( keys[j], &val );
				in.m_ptr[j] = (val-in_shift[j])/max(in_scale[j],1e-100);
			}
			CVector *out = net.Process(in);

			int i_best=0;
			for (int i = 0; i < nclass; i++)
			{
				if(out->m_ptr[i]>out->m_ptr[i_best]) i_best=i;
			}
			//if(out->m_ptr[i_best]<0.0) i_best=-1;
			if(i_best>=0) i_best/=lOutScale;
			set_object_class(sptrObj, i_best);
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
CClassifyNeuro::CClassifyNeuro()
{}

CClassifyNeuro::~CClassifyNeuro()
{}


bool CClassifyNeuro::InvokeFilter()
{
	// Find object list
	IUnknownPtr punkObjList(GetDataArgument());
	if(punkObjList==0) return false;

	_bstr_t iniName=GetArgString("ClassifyInfo");
	if(iniName==_bstr_t(""))
	{ //им€ инишки не задано - берем из shell.data
		char	sz_ini[MAX_PATH];
		local_filename(sz_ini, "Classes.ini", sizeof(sz_ini)); //получим им€ файла ClassifyInfo.ini
		strcpy( sz_ini, (const char*)::GetValueString(GetAppUnknown(), "\\Classes", "ClassFile", sz_ini ));
		iniName=sz_ini;	
	}

	{
		CIniFile ini;
		ini.Open(iniName);
		char sz_tmp[128];
		//ѕроверить, что инишка - нашего типа
		ini.GetVal("General","ClassifyProgID",sz_tmp, sizeof(sz_tmp));
		RF(_stricmp(sz_tmp,"Neuro.NeuroClassifier")==0);
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



/////////////////////////////////////////////////////////////////////////////
CTeachNeuro::CTeachNeuro()
{}

CTeachNeuro::~CTeachNeuro()
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

HRESULT CTeachNeuro::DoInvoke()
{
	_bstr_t iniName=GetArgString("ClassifyInfo");
	if(iniName==_bstr_t(""))
	{ //им€ инишки не задано - берем из shell.data
		char	sz_ini[MAX_PATH];
		local_filename(sz_ini, "ClassifyInfo.net", sizeof(sz_ini)); //получим им€ файла ClassifyInfo.ini
		strcpy( sz_ini, (const char*)::GetValueString(GetAppUnknown(), "\\Classes", "ClassFile", sz_ini ));
		iniName=sz_ini;	
	}

	CIniFile ini;
	ini.Open(iniName);
	char sz_tmp[128];
	//ѕроверить, что инишка - нашего типа
	ini.GetVal("General","ClassifyProgID",sz_tmp, sizeof(sz_tmp));
	RF(_stricmp(sz_tmp,"Neuro.NeuroClassifier")==0);

	_bstr_t iniLearning=GetArgString("LearningInfo");
	if(iniLearning==_bstr_t(""))
	{ //им€ инишки не задано - берем из ClassifyInfo
		char	sz_ini2[MAX_PATH];
		ini.GetVal("Classes","LearningInfo",sz_ini2, sizeof(sz_ini2));
		iniLearning=sz_ini2;
		if(iniLearning==_bstr_t(""))
		{ //им€ инишки и там не задано - берем из shell.data
			char	sz_ini2[MAX_PATH];
			local_filename(sz_ini2, "LearningInfo.ini", sizeof(sz_ini2)); //получим им€ файла LearningInfo.ini
			strcpy( sz_ini2, (const char*)::GetValueString(GetAppUnknown(), "\\Classes", "LearningInfo", sz_ini2 ));
			iniLearning=sz_ini2;	
			//ini.SetVal("Classes","LearningInfo",sz_ini);
		}
		def_filename( sz_ini2, "Classes", short_filename(iniLearning), sizeof(sz_ini2) );
		iniLearning = sz_ini2;
	}

	long lHiddenNeurons = 9;
	ini.GetVal("NeuroSettings","HiddenNeurons",sz_tmp, sizeof(sz_tmp));
	if(sz_tmp[0]!=0) lHiddenNeurons = atoi(sz_tmp);
	lHiddenNeurons = min(1024, max(1,lHiddenNeurons));

	int nMaxEpoch = 1000;
	ini.GetVal("NeuroSettings","MaxEpoch",sz_tmp, sizeof(sz_tmp));
	if(sz_tmp[0]!=0) nMaxEpoch = atoi(sz_tmp);
	nMaxEpoch = min(1000000, max(10,nMaxEpoch));

	long lOutScale = 1;
	ini.GetVal("NeuroSettings","OutScale",sz_tmp, sizeof(sz_tmp));
	if(sz_tmp[0]!=0) lOutScale = atoi(sz_tmp);
	lOutScale = min(8, max(1,lOutScale));


	//long lHiddenNeurons = ::GetValueInt(GetAppUnknown(), "\\neuro", "HiddenNeurons", 9);
	//long lOutScale = ::GetValueInt(GetAppUnknown(), "\\neuro", "OutScale", 1);

	CFile f( fopen(iniLearning,"r") );
	RF(f);
	smart_alloc(buf,char,BIGBUFSIZE); RF(buf!=0);
	smart_alloc(keys,int,MAXKEYS); RF(keys!=0);
	char section[128];
	_ptr_t2 <CVector> in;
	_ptr_t2 <CVector> out;
	CVector out_count;

	int npat=0, ninp=0, nout=0;
	while(1)
	{	//пока хоть что-то читаетс€
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
			if(0==_stricmp("ClassCount", name))
			{
				nout=atoi(val);
				out_count.Alloc(nout);
				for(int j=0; j<nout; j++) out_count.m_ptr[j] = 0;
			}
			int n = atoi(name)-1;
			if(n>=0 && n<npat)
			{
				if(in.size()!=npat) in.alloc(npat);
				if(out.size()!=npat) out.alloc(npat);

				in[n].Alloc(ninp);
				RF(ninp==in[n].m_size);
				out[n].Alloc(nout*lOutScale);
				RF(nout*lOutScale==out[n].m_size);

				char *val1 = CutToken(val,':');
				int nclass = atoi(val)-1;
				int nn=GetDoubleArray(val1,in[n].m_ptr,in[n].m_size);
				RF(nn==ninp);
				if(nclass>=0 && nclass<nout)
				{ // увеличим счетчик дл€ класса
					out_count.m_ptr[nclass] ++ ;
				}
				
				for(int j=0; j<nout*lOutScale; j++) out[n].m_ptr[j] = ((j/lOutScale)==nclass) ? 1 : -1;
			}
		}
	}
	f.Close();
	out_count.m_size += 0;

	int npat1=npat;
	smart_alloc(in_min,double,ninp);
	smart_alloc(in_max,double,ninp);
	smart_alloc(in_avg,double,ninp);
	smart_alloc(in_dis,double,ninp);
	for(int j=0; j<ninp; j++)
	{
		in_max[j] = -1e200;
		in_min[j] = 1e200;
		in_avg[j] = 0;
		in_dis[j] = 0;
	}

	for(int i=0; i<npat; i++)
	{	// произведем калибровку, попутно проверим список на пустые/неверные строки
        RF(in[i].m_size==ninp);
        RF(out[i].m_size==nout*lOutScale);
		for(int j=0; j<ninp; j++)
		{
			if(in_max[j]<in[i].m_ptr[j]) in_max[j] = in[i].m_ptr[j];
			if(in_min[j]>in[i].m_ptr[j]) in_min[j] = in[i].m_ptr[j];
			in_avg[j] += in[i].m_ptr[j];
			in_dis[j] += sqr(in[i].m_ptr[j]);
		}
	}

	smart_alloc(in_shift,double,ninp);
	smart_alloc(in_scale,double,ninp);
	for(int j=0; j<ninp; j++)
	{
		in_avg[j] /= max(npat,1);
		in_dis[j] /= max(npat,1);
		in_dis[j] = sqrt(max(1e-20,in_dis[j] - sqr(in_avg[j])));
		//in_shift[j] = in_avg[j];
		//in_scale[j] = in_dis[j];
		in_shift[j] = (in_min[j]+in_max[j])/2;
		in_scale[j] = (in_max[j]-in_min[j])/2;
		in_scale[j] = max(in_scale[j], 1e-100);
	}

	for(int i=0; i<npat; i++)
	{	// отнормируем данные
		for(int j=0; j<ninp; j++)
		{
			in[i].m_ptr[j] = (in[i].m_ptr[j]-in_shift[j]) / in_scale[j];
		}
	}

	//переупор€дочим
	{	CVector tmp_in(ninp), tmp_out(nout);
		for(int i=0; i<npat; i++)
		{
			int j = rand() % npat;
			RF( tmp_in.Set(in[i]) );
			RF( tmp_out.Set(out[i]) );
			RF( in[i].Set(in[j]) );
			RF( out[i].Set(out[j]) );
			RF( in[j].Set(tmp_in) );
			RF( out[j].Set(tmp_out) );
		}
	}

	CNet net(ninp,lHiddenNeurons,nout*lOutScale);

	//int nMaxEpoch = 1000;
	int nNotifyFreq=max(nMaxEpoch/100,1);
	StartNotification(nMaxEpoch+nNotifyFreq,1,1);
	for(int epoch=0; epoch<nMaxEpoch; epoch+=nNotifyFreq)
	{
		double d = 0.1;
		if(epoch>=200*nMaxEpoch/1000) d=0.1;
		if(epoch>=700*nMaxEpoch/1000) d=0.1;
		if(epoch==200*nMaxEpoch/1000 || epoch==400*nMaxEpoch/1000 || epoch==600*nMaxEpoch/1000)
		{
			net.Learn(in, out, npat, 0.2, 1);
		}
		net.Learn(in, out, npat, d, nNotifyFreq);
		Notify(epoch+nNotifyFreq);
	}
	FinishNotification();

	//net.Save(".\\test.net");
	//CIniFile ini;
	//ini.Open(iniName);
	net.SaveIni(&ini);

	int buf_pos=0;
	for(int j=0; j<ninp; j++)
	{
		buf_pos += sprintf(buf+buf_pos, "%i ", keys[j]);
		RF(buf_pos<BIGBUFTHRESHOLD);
	}
	ini.SetVal("Classifier","Keys",buf);

	// сдублируем ту же инфу в ћаксовом формате
	buf_pos=0;
	buf_pos += sprintf(buf+buf_pos, "%i", keys[0]);
	RF(buf_pos<BIGBUFTHRESHOLD);
	for(int j=1; j<ninp; j++)
	{
		buf_pos += sprintf(buf+buf_pos, ",%i", keys[j]);
		RF(buf_pos<BIGBUFTHRESHOLD);
	}
	ini.SetVal(ID_SECT_KEY_INFO,ID_ENTR_KEYS_ENUM,buf);

	buf_pos=0;
	for(int j=0; j<ninp; j++)
	{
		buf_pos += sprintf(buf+buf_pos, "%g ", in_shift[j]);
		RF(buf_pos<BIGBUFTHRESHOLD);
	}
	ini.SetVal("Calibration","Shift",buf);

	buf_pos=0;
	for(int j=0; j<ninp; j++)
	{
		buf_pos += sprintf(buf+buf_pos, "%g ", in_scale[j]);
		RF(buf_pos<BIGBUFTHRESHOLD);
	}
	ini.SetVal("Calibration","Scale",buf);

	_itoa(lOutScale, buf, 100);
	ini.SetVal("Calibration","OutScale",buf);

	// ƒл€ совместимости с ћаксом
	ini.SetVal("General","ClassifyName","Neural Classifier (MLP)");
	ini.SetVal("General","ClassifyProgID","Neuro.NeuroClassifier");
    

	ini.Close();

	return true;
}


//////////////////////////////////////////////////////////////////////
CNeuroClassifier::CNeuroClassifier(void)
{
}

CNeuroClassifier::~CNeuroClassifier(void)
{
}

IUnknown *CNeuroClassifier::DoGetInterface( const IID &iid )
{
	if( iid == IID_IClassifyParams )
		return (IClassifyParams *)this;
	else if( iid == IID_IClassifyProxy )
		return (IClassifyProxy *)this;

	return ComObjectBase::DoGetInterface( iid );
}

HRESULT CNeuroClassifier::Start()
{
	long lClassCount = 0;
	GetClasses( 0, &lClassCount );

	if( !lClassCount )
		return S_FALSE;

	m_Classes.alloc(lClassCount);

	if( m_Classes==0 )
		return S_FALSE;

	long *pClasses=m_Classes;
	GetClasses( &pClasses, &lClassCount );

	long lKeyCount = 0;
	GetKeys( 0, &lKeyCount );

	if( !lKeyCount )
		return S_FALSE;

	m_Params.alloc(lKeyCount);

	if( m_Params==0 )
		return S_FALSE;

	long *pParams=m_Params;
	GetKeys( &pParams, &lKeyCount );
	
	return __super::Start();
}

HRESULT CNeuroClassifier::Load( /*[in]*/ BSTR bstrFile )
{
	__super::Load(bstrFile);

	CIniFile ini;
	ini.Open(_bstr_t(bstrFile));
	RF (m_net.LoadIni(&ini));

	int nclass = m_net.m_Y[m_net.m_nLayers].m_size;
	int nkeys = m_net.m_Y[0].m_size;

	if (nclass == 0 || nkeys == 0) return 0; 

	_ptr_t2<int> keys(nkeys);	

	_ptr_t2<char> big_buf(BIGBUFSIZE);
	ini.GetVal("Classifier","Keys",big_buf,BIGBUFTHRESHOLD);
	RF( nkeys == GetIntArray(big_buf,keys,nkeys,' ') );

	m_in_shift.alloc(nkeys);
	m_in_scale.alloc(nkeys);

	ini.GetVal("Calibration","Shift",big_buf,BIGBUFTHRESHOLD);
	RF( nkeys == GetDoubleArray(big_buf,m_in_shift,nkeys,' ') );

	ini.GetVal("Calibration","Scale",big_buf,BIGBUFTHRESHOLD);
	RF( nkeys == GetDoubleArray(big_buf,m_in_scale,nkeys,' ') );

	ini.GetVal("Calibration","OutScale",big_buf,BIGBUFTHRESHOLD);
	m_lOutScale = atoi(big_buf);

	ini.Close();

	CVector in;
	in.Alloc(nkeys);

	//double GarbageThreshold=GetValueDouble
	//	(GetAppUnknown(), "\\CellClassify", "GarbageThreshold", 5.0 );

	return S_OK;
}

HRESULT CNeuroClassifier::Process( /*[in]*/ IUnknown *punkObject )
{
	/*
	XObjectItem item;
	item.sptrObject = punkObject;
	item.guidKey = ::GetKey( punkObject );
	*/

	ICalcObjectPtr sptrObject = punkObject;

	if( sptrObject == 0 )
		return S_FALSE;

	int nParams=m_Params.size();
	if(nParams<1) return S_FALSE;
	smart_alloc(fParamsValues,double,nParams);
	for(int i=0; i<nParams; i++)
	{
		fParamsValues[i]=0;
		sptrObject->GetValue(m_Params[i],&(fParamsValues[i]));
	}

	long lClass=-1;
	RealProcess(nParams, fParamsValues, punkObject, &lClass);

	XObjectItem* pitem = new XObjectItem;
	pitem->sptrObject = punkObject;
	pitem->lClass = lClass;
	pitem->guidKey = ::GetKey( punkObject );
	m_arObjectList.set( pitem, &pitem->guidKey );

	//m_arObjectList.add_tail( item );

	return S_OK;
}

bool CNeuroClassifier::RealProcess(
		/*in*/ long nParams, double *pfParams, IUnknown *punkObject,
		/*out*/ long *plClass
		)
{
	if(m_net.m_nLayers<1) return false;

	int nclass = m_net.m_Y[m_net.m_nLayers].m_size;
	int nkeys = m_net.m_Y[0].m_size;
	if(nkeys!=nParams) return false;

	CVector in;
	in.Alloc(nkeys);

	for (int j = 0; j < nkeys; j++)
	{
		in.m_ptr[j] = (pfParams[j]-m_in_shift[j])/max(m_in_scale[j],1e-100);
	}
	CVector *out = m_net.Process(in);

	int i_best=0;
	for (int i = 0; i < nclass; i++)
	{
		if(out->m_ptr[i]>out->m_ptr[i_best]) i_best=i;
	}

	//if(out->m_ptr[i_best]<0.0) i_best=-1;
	if(i_best>=0) i_best /= m_lOutScale;
	*plClass=i_best;
	return true;
}

HRESULT CNeuroClassifier::Teach()
{
	_bstr_t iniName="";//GetArgString("ClassifyInfo");
	if(iniName==_bstr_t(""))
	{ //им€ инишки не задано - берем из shell.data
		char	sz_ini[MAX_PATH];
		local_filename(sz_ini, "ClassifyInfo.net", sizeof(sz_ini)); //получим им€ файла ClassifyInfo.ini
		strcpy( sz_ini, (const char*)::GetValueString(GetAppUnknown(), "\\Classes", "ClassFile", sz_ini ));
		iniName=sz_ini;	
	}

	_bstr_t iniLearning="";//GetArgString("LearningInfo");
	if(iniLearning==_bstr_t(""))
	{ //им€ инишки не задано - берем из shell.data
		char	sz_ini[MAX_PATH];
		local_filename(sz_ini, "LearningInfo.ini", sizeof(sz_ini)); //получим им€ файла LearningInfo.ini
		strcpy( sz_ini, (const char*)::GetValueString(GetAppUnknown(), "\\Classes", "LearningInfo", sz_ini ));
		iniLearning=sz_ini;	
	}

	CIniFile ini;
	ini.Open(iniName);
	char sz_tmp[128];
	//ѕроверить, что инишка - нашего типа
	ini.GetVal("General","ClassifyProgID",sz_tmp, sizeof(sz_tmp));
	RSF(_stricmp(sz_tmp,"Neuro.NeuroClassifier")==0);

	long lHiddenNeurons = 9;
	ini.GetVal("NeuroSettings","HiddenNeurons",sz_tmp, sizeof(sz_tmp));
	if(sz_tmp[0]!=0) lHiddenNeurons = atoi(sz_tmp);
	lHiddenNeurons = min(1024, max(1,lHiddenNeurons));

	long lOutScale = 1;
	ini.GetVal("NeuroSettings","OutScale",sz_tmp, sizeof(sz_tmp));
	if(sz_tmp[0]!=0) lOutScale = atoi(sz_tmp);
	lOutScale = min(8, max(1,lOutScale));


	//long lHiddenNeurons = ::GetValueInt(GetAppUnknown(), "\\neuro", "HiddenNeurons", 9);
	//long lOutScale = ::GetValueInt(GetAppUnknown(), "\\neuro", "OutScale", 1);

	CFile f( fopen(iniLearning,"r") );
	RSF(f);
	smart_alloc(buf,char,BIGBUFSIZE); RSF(buf!=0);
	smart_alloc(keys,int,MAXKEYS); RSF(keys!=0);
	char section[128];
	_ptr_t2 <CVector> in;
	_ptr_t2 <CVector> out;
	CVector out_count;

	int npat=0, ninp=0, nout=0;
	while(1)
	{	//пока хоть что-то читаетс€
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
			if(0==_stricmp("ClassCount", name))
			{
				nout=atoi(val);
				out_count.Alloc(nout);
				for(int j=0; j<nout; j++) out_count.m_ptr[j] = 0;
			}
			int n = atoi(name)-1;
			if(n>=0 && n<npat)
			{
				if(in.size()!=npat) in.alloc(npat);
				if(out.size()!=npat) out.alloc(npat);

				in[n].Alloc(ninp);
				RSF(ninp==in[n].m_size);
				out[n].Alloc(nout*lOutScale);
				RSF(nout*lOutScale==out[n].m_size);

				char *val1 = CutToken(val,':');
				int nclass = atoi(val)-1;
				int nn=GetDoubleArray(val1,in[n].m_ptr,in[n].m_size);
				RSF(nn==ninp);
				if(nclass>=0 && nclass<nout)
				{ // увеличим счетчик дл€ класса
					out_count.m_ptr[nclass] ++ ;
				}

				
				for(int j=0; j<nout*lOutScale; j++) out[n].m_ptr[j] = ((j/lOutScale)==nclass) ? 1 : -1;
			}
		}
	}
	f.Close();
	out_count.m_size += 0;

	int npat1=npat;
	smart_alloc(in_min,double,ninp);
	smart_alloc(in_max,double,ninp);
	smart_alloc(in_avg,double,ninp);
	smart_alloc(in_dis,double,ninp);
	for(int j=0; j<ninp; j++)
	{
		in_max[j] = -1e200;
		in_min[j] = 1e200;
		in_avg[j] = 0;
		in_dis[j] = 0;
	}

	for(int i=0; i<npat; i++)
	{	// произведем калибровку, попутно проверим список на пустые/неверные строки
        RSF(in[i].m_size==ninp);
        RSF(out[i].m_size==nout*lOutScale);
		for(int j=0; j<ninp; j++)
		{
			if(in_max[j]<in[i].m_ptr[j]) in_max[j] = in[i].m_ptr[j];
			if(in_min[j]>in[i].m_ptr[j]) in_min[j] = in[i].m_ptr[j];
			in_avg[j] += in[i].m_ptr[j];
			in_dis[j] += sqr(in[i].m_ptr[j]);
		}
	}

	smart_alloc(in_shift,double,ninp);
	smart_alloc(in_scale,double,ninp);
	for(int j=0; j<ninp; j++)
	{
		in_avg[j] /= max(npat,1);
		in_dis[j] /= max(npat,1);
		in_dis[j] = sqrt(max(1e-20,in_dis[j] - sqr(in_avg[j])));
		//in_shift[j] = in_avg[j];
		//in_scale[j] = in_dis[j];
		in_shift[j] = (in_min[j]+in_max[j])/2;
		in_scale[j] = (in_max[j]-in_min[j])/2;
		in_scale[j] = max(in_scale[j], 1e-100);
	}

	for(int i=0; i<npat; i++)
	{	// отнормируем данные
		for(int j=0; j<ninp; j++)
		{
			in[i].m_ptr[j] = (in[i].m_ptr[j]-in_shift[j]) / in_scale[j];
		}
	}

	//переупор€дочим
	{	CVector tmp_in(ninp), tmp_out(nout);
		for(int i=0; i<npat; i++)
		{
			int j = rand() % npat;
			RSF( tmp_in.Set(in[i]) );
			RSF( tmp_out.Set(out[i]) );
			RSF( in[i].Set(in[j]) );
			RSF( out[i].Set(out[j]) );
			RSF( in[j].Set(tmp_in) );
			RSF( out[j].Set(tmp_out) );
		}
	}

	CNet net(ninp,lHiddenNeurons,nout*lOutScale);

	int nMaxEpoch=1000;
	int nNotifyFreq=50;
	//StartNotification(nMaxEpoch+1,1,1);
	for(int epoch=0; epoch<nMaxEpoch; epoch+=nNotifyFreq)
	{
		double d = 0.1;
		if(epoch>=200) d=0.1;
		if(epoch>=700) d=0.1;
		if(epoch==200 || epoch==400 || epoch==600)
		{
			net.Learn(in, out, npat, 0.2, 1);
		}
		net.Learn(in, out, npat, d, nNotifyFreq);
		//Notify(epoch+nNotifyFreq);
	}
	//FinishNotification();

	//net.Save(".\\test.net");
	//CIniFile ini;
	//ini.Open(iniName);
	net.SaveIni(&ini);

	int buf_pos=0;
	for(int j=0; j<ninp; j++)
	{
		buf_pos += sprintf(buf+buf_pos, "%i ", keys[j]);
		RSF(buf_pos<BIGBUFTHRESHOLD);
	}
	ini.SetVal("Classifier","Keys",buf);

	// сдублируем ту же инфу в ћаксовом формате
	buf_pos=0;
	buf_pos += sprintf(buf+buf_pos, "%i", keys[0]);
	RSF(buf_pos<BIGBUFTHRESHOLD);
	for(int j=1; j<ninp; j++)
	{
		buf_pos += sprintf(buf+buf_pos, ",%i", keys[j]);
		RSF(buf_pos<BIGBUFTHRESHOLD);
	}
	ini.SetVal(ID_SECT_KEY_INFO,ID_ENTR_KEYS_ENUM,buf);

	buf_pos=0;
	for(int j=0; j<ninp; j++)
	{
		buf_pos += sprintf(buf+buf_pos, "%g ", in_shift[j]);
		RSF(buf_pos<BIGBUFTHRESHOLD);
	}
	ini.SetVal("Calibration","Shift",buf);

	buf_pos=0;
	for(int j=0; j<ninp; j++)
	{
		buf_pos += sprintf(buf+buf_pos, "%g ", in_scale[j]);
		RSF(buf_pos<BIGBUFTHRESHOLD);
	}
	ini.SetVal("Calibration","Scale",buf);

	_itoa(lOutScale, buf, 100);
	ini.SetVal("Calibration","OutScale",buf);

	// ƒл€ совместимости с ћаксом
	ini.SetVal("General","ClassifyName","Neural Classifier (MLP)");
	ini.SetVal("General","ClassifyProgID","Neuro.NeuroClassifier");
    

	ini.Close();

	return S_OK;
}
