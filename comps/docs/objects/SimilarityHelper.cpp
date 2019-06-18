#include "stdafx.h"
#include "SimilarityHelper.h"
#include "\vt5\common2\class_utils.h"
#include <math.h>
#include <fstream>
#include <algorithm>

CSimilarityHelper::CSimilarityHelper(void) :
nClasses(0), nObjects(0), nParams(0),
bInited(false), bProcessed(false), bErrorInited(false),
bTestObjectList2Done(false)
{
}

CSimilarityHelper::~CSimilarityHelper(void)
{
}

void CSimilarityHelper::Init(IUnknown* punkList)
{
	bInited = false;
	bProcessed = false;
	bErrorInited = false;
	bTestObjectList2Done = false;

	INamedDataObject2Ptr sptrL(punkList);
	if(sptrL==0) return; // неудача - фигню передали
	ICalcObjectContainerPtr sptrC(punkList);
	if(sptrC==0) return; // неудача - фигню передали

	nClasses = class_count();
	if(nClasses<=0) return; // неудача - фигня в классификаторе

	sptrL->GetChildsCount(&nObjects);
	sptrC->GetParameterCount(&nParams);

	params.resize(nParams);
	param_names.resize(nParams);
	params_weight.resize(nParams);
	values.resize(nParams*nObjects);
	values_obj.resize(nParams);
	classes.resize(nObjects);
	classes_weight.resize(nClasses);

	long lpos = 0;
	sptrC->GetFirstParameterPos( &lpos );
	char szBuf[100];
	for(int i=0; i<nParams; i++)
	{
		ParameterContainer	*p = 0;
		sptrC->GetNextParameter( &lpos, &p );
		params[i] = p->pDescr->lKey;
		param_names[i] = p->pDescr->bstrName; // копируем - не вполне эффективно, да хрен с ним
		values_obj[i] = 1e222; // >1e200 - ошибочное число, в расчете не участвует
		GetPrivateProfileString( "ParamsWeight", CString(p->pDescr->bstrName), "",
			szBuf, sizeof(szBuf), full_classifiername(0) );
		if(szBuf[0]) params_weight[i] = atof(szBuf);
		else WritePrivateProfileString( "ParamsWeight", CString(p->pDescr->bstrName),
			"1.0", full_classifiername(0) ); // Сразу воткнем обратно - на случай, если не было прописано
	}

	char szNumBuf[10];
	for(int k=0; k<nClasses; k++)
	{
		_itoa(k, szNumBuf, 10);
		GetPrivateProfileString( "ClassesWeight", szNumBuf, "",
			szBuf, sizeof(szBuf), full_classifiername(0) );
		classes_weight[k]=1.0;
		if(szBuf[0]) classes_weight[k] = atof(szBuf);
		else WritePrivateProfileString( "ClassesWeight", szNumBuf,
			"1.0", full_classifiername(0) ); // Сразу воткнем обратно - на случай, если не было прописано

	}

	long pos=0;
	sptrL->GetFirstChildPosition(&pos);
	for(int j=0; j<nObjects; j++)
	{
		classes[j] = -1;
		for(int i=0; i<nParams; i++) values[j*nParams+i] = 1e222; // >1e200 - ошибочное число, в расчете не участвует
		IUnknownPtr ptr;
		sptrL->GetNextChild(&pos, &ptr);
		ICalcObjectPtr sptrChild = ptr;
		if (sptrChild != 0)
		{
			classes[j] = get_object_class(sptrChild);
			if(classes[j]<0 || classes[j]>=nClasses) classes[j]=-1; // подстраховка для кривых номеров классов
			for(int i=0; i<nParams; i++)
			{
				values[j*nParams+i] = 1e222; // >1e200 - ошибочное число, в расчете не участвует
				sptrChild->GetValue(params[i], &values[j*nParams+i]);
			}
		}
	}

	// На данный момент все данные в массивах: номера параметров в params[i],
	// значения для списка - в values[j*nParams+i]
	// классы списка - в classes[j]

	num.resize(nClasses*nParams);
	avg.resize(nClasses*nParams);
	disp.resize(nClasses*nParams);
	num_total.resize(nParams);
	disp_total.resize(nParams);

	for(int i=0; i<nParams; i++)
	{
		for(int k=0; k<nClasses; k++)
		{
			num[k*nParams+i] = 0;
			avg[k*nParams+i] = 0;
			disp[k*nParams+i] = 0;
		}
		num_total[i] = 0;
		disp_total[i] = 0;
	}

	for(int i=0; i<nParams; i++)
	{
		// расчет средних
		for(int j=0; j<nObjects; j++)
		{
			if(values[j*nParams+i]>1e200)
				continue; // ошибочное значение
			int k = classes[j];
			if(k<0 || k>=nClasses) continue;
			avg[k*nParams+i] = ( avg[k*nParams+i] * num[k*nParams+i] + values[j*nParams+i] ) / (num[k*nParams+i]+1);
			num[k*nParams+i]++;
			num_total[i]++;
		}

		// расчет дисперсий
		for(int j=0; j<nObjects; j++)
		{
			int k = classes[j];
			if(k<0 || k>=nClasses) continue;
			if(values[j*nParams+i]>1e200)
				continue; // ошибочное значение
			double df = values[j*nParams+i] - avg[k*nParams+i];
			disp[k*nParams+i] += df*df / num[k*nParams+i] ;
			disp_total[i] = disp_total[i] + df*df / num_total[i];
		}
	}

	bInited = true;
}

void CSimilarityHelper::ProcessObject(IUnknown* punkObject)
{ // обсчитать все для объекта; должен быть уже вызван Init
	bProcessed = false;

	ASSERT(bInited);
	if(!bInited) return; // не проинитили

	// загрузим параметры объекта в массив
	ICalcObject2Ptr sptrObj = punkObject;
	if(sptrObj==0) return; // error - no object

	nObjectClass = get_object_class(sptrObj);

	for(int i=0; i<nParams; i++)
	{
		values_obj[i] = 1e222; // >1e200 - ошибочное число, в расчете не участвует
		sptrObj->GetValue(params[i], &values_obj[i]);
	}

	// Сравнение
	classes_best_d.resize(nClasses);
	classes_best_j.resize(nClasses);
	for(int k=0; k<nClasses; k++)
	{
		classes_best_d[k] = 1e200;
		classes_best_j[k] = -1;
	}

	// Найдем минимумы по классам
	for(int j=0; j<nObjects; j++)
	{
		int k = classes[j];
		if(k<0 || k>=nClasses) continue;
		double d = 0;
		for(int i=0; i<nParams; i++)
		{
			if(values[j*nParams+i]>1e200)
				continue; // ошибочное значение
			if(values_obj[i]>1e200)
				continue; // ошибочное значение

			double df = values_obj[i] - values[j*nParams+i];
			df *= params_weight[i];
			double scale = disp_total[i];
			if(k>=0 && k<nClasses) scale = disp[k*nParams+i];
			if(scale<=0) scale = 1e-100;
			d += df*df/scale;
		}
		d *= classes_weight[k];
		if( d < classes_best_d[k] )
		{
			classes_best_d[k] = d;
			classes_best_j[k] = j;
		}
	}

	// найдем лучший класс
	best_class=0;
	for(int k=1; k<nClasses; k++)
		if(classes_best_d[k] < classes_best_d[best_class]) best_class = k;

	// найдем лучший объект
	best_j = classes_best_j[best_class];

	// Поиск второго по близости класса
	double best_d2 = 1e200;
	for(int k=0; k<nClasses; k++)
	{
		if ( k == best_class ) continue;
		best_d2 = min(best_d2, classes_best_d[k]);
	}
	double ratio_d = 1.0;
	if(best_d2>0) ratio_d = classes_best_d[best_class] / best_d2;

	::SetValue(::GetAppUnknown(), "\\Objects\\SimilarObject", "Distance", classes_best_d[best_class]);
	::SetValue(::GetAppUnknown(), "\\Objects\\SimilarObject", "Accuracy", 1 - ratio_d);

	bProcessed = true;
}

int CSimilarityHelper::GetSimilar()
{ // вернуть номер наиболее похожего
	if(!bProcessed) return -1;

	return best_j;
}

int CSimilarityHelper::GetSimilarInClass(int nClass)
{ // вернуть номер наиболее похожего в данном классе
	if(!bProcessed) return -1;

	if(nClass<0 || nClass>=nClasses)
		return -1;

	return classes_best_j[nClass];
}

int CSimilarityHelper::GetSimilarInOwnClass()
{ // вернуть номер наиболее похожего в собственном классе
	if(!bProcessed) return -1;

	return GetSimilarInClass(nObjectClass);
}

int CSimilarityHelper::GetClassSimilar()
{ // вернуть номер класса наиболее похожего
	if(!bProcessed) return -1;

	return best_class;
}

inline double CSimilarityHelper::_GetParamDistanceSqrByIndex(int iParam, int jObject)
{	// вернуть "расстояние" по указанному параметру
	// без учета params_weight;

	double d = 0;

	if(values[jObject*nParams+iParam]>1e200)
		return 1e200; // ошибочное значение
	if(values_obj[iParam]>1e200)
		return 1e200; // ошибочное значение

	double df = values_obj[iParam] - values[jObject*nParams+iParam];
	double scale = disp_total[iParam];
	scale = disp[classes[jObject]*nParams+iParam];
	if(scale<=0) scale = 1e-100;
	d += df*df/scale;

	d *= classes_weight[classes[jObject]];

	return d;
}

double CSimilarityHelper::GetParamDistance(int nParam, int jObject)
{ // вернуть "расстояние" по указанному параметру
	// без учета params_weight[iParam]
	if(!bProcessed) return 1e200;
	if(jObject<0 || jObject>=nObjects) return 1e200;
	if(classes[jObject]<0 || classes[jObject]>=nClasses) return 1e200;

	// найдем наш параметр
	int iParam = -1; // индекс в таблице
	{
		for(int i=0; i<nParams; i++)
			if(params[i] == nParam) iParam = i;
	}

	if(iParam<0) return 1e200; // ошибочное значение - нет такого параметра

	double d = _GetParamDistanceSqrByIndex(iParam, jObject);
	if(d>1e199) return d; // error

	return sqrt(d);
}

double CSimilarityHelper::GetParamDistance(int nParam)
{ // вернуть "расстояние" по указанному параметру
	if(!bProcessed) return 1e200;

	return GetParamDistance(nParam, best_j);
}

void CSimilarityHelper::WriteObjectReport(const wchar_t* wszFileName)
{ // сохранить отчет по объекту
	if(!bProcessed) return;

	FILE* f = _wfopen(wszFileName, L"w, ccs=UTF-8");
	fputws(L"<html>", f);
	fputws(L"<head>", f);
    fputws(L"<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; CHARSET=utf-8\">", f);
    fputws(L"<TITLE>Таблица расстояний объекта по параметрам</TITLE>", f);
	fputws(L"</head>", f);
	fputws(L"<body>\n\r", f);

	fwprintf(f, L"<TABLE BORDER=1 BORDERCOLOR=\"#000000\" CELLPADDING=1 CELLSPACING=0>\n");

	// ----------------

	fwprintf(f, L"<TR>\n");
	fwprintf(f, L"<TD>#</TD>\n");
	fwprintf(f, L"<TD>Параметр</TD>\n");
	fwprintf(f, L"<TD>Значение</TD>\n");
	fwprintf(f, L"<TD>Значение в ближайшем эталоне</TD>\n");
	fwprintf(f, L"<TD>Расстояние по параметру</TD>\n");
	fwprintf(f, L"<TD>Значение в ближайшем эталоне своего класса</TD>\n");
	fwprintf(f, L"<TD>Расстояние по параметру</TD>\n");
	fwprintf(f, L"</TR>\n");

	// ----------------

	for(int i=0; i<nParams; i++)
	{
		if(params_weight[i]<1e-2) continue;
		fwprintf(f, L"<TR>\n");
		fwprintf(f, L"<TD>%i</TD>\n", params[i]);
		fwprintf(f, L"<TD>%s</TD>\n", param_names[i].c_str());
		if(values_obj[i]>1e200) // нет значений параметра
			fwprintf(f, L"<TD>-</TD>\n");
		else // вывести значение параметра
			fwprintf(f, L"<TD>%g</TD>\n", values_obj[i]);
		if(best_j<0) // нет лучшего эталона
		{
			fwprintf(f, L"<TD>-</TD>\n", values[best_j*nParams+i]);
			fwprintf(f, L"<TD>-</TD>\n");
		}
		else // вывести значение в ближайшем эталоне и расстояние до него
		{
			fwprintf(f, L"<TD>%g</TD>\n", values[best_j*nParams+i]);
			double d = GetParamDistance(params[i], best_j);
			if(d>2.0)
				fwprintf(f, L"<TD BGCOLOR=#FF8080>%g</TD>\n", d);
			else if(d>1.0)
				fwprintf(f, L"<TD BGCOLOR=#FFFF60>%g</TD>\n", d);
			else if(d>0.5)
				fwprintf(f, L"<TD BGCOLOR=FFFFE0>%g</TD>\n", d);
			else
				fwprintf(f, L"<TD>%g</TD>\n", d);
		}
		if( nObjectClass<0 || nObjectClass>=nClasses || classes_best_j[nObjectClass]<0 )
		{ // класс объекта плохой или нет лучшего эталона в этом классе
			fwprintf(f, L"<TD>-</TD>\n");
			fwprintf(f, L"<TD>-</TD>\n");
		}
		else
		{ // вывести значение в ближайшем эталоне своего класса и расстояние до него
			fwprintf(f, L"<TD>%g</TD>\n", values[classes_best_j[nObjectClass]*nParams+i]);
			double d = GetParamDistance(params[i], classes_best_j[nObjectClass]);
			if(d>2.0)
				fwprintf(f, L"<TD BGCOLOR=#FF8080>%g</TD>\n", d);
			else if(d>1.0)
				fwprintf(f, L"<TD BGCOLOR=#FFFF60>%g</TD>\n", d);
			else if(d>0.5)
				fwprintf(f, L"<TD BGCOLOR=FFFFE0>%g</TD>\n", d);
			else
				fwprintf(f, L"<TD>%g</TD>\n", d);
		}
		fwprintf(f, L"</TR>\n");
	}

	fwprintf(f, L"</TABLE>\n");

	fputws(L"</body>", f);
	fputws(L"</html>", f);
	fclose(f);
}

void CSimilarityHelper::ProcessObjectError()
{	// обсчитать для объекта статистику значимости параметров;
	// должен быть уже вызван Process
	if(!bProcessed) return;
	if(nObjectClass<0 || nObjectClass>=nClasses || best_class<0)
		return;

	if(!bErrorInited)
	{
		bErrorInited = true;

		// проинитим матрицу ошибок
		error_matrix.resize(nClasses*nClasses);
		for(int i=0; i<nClasses*nClasses; i++)
			error_matrix[i] = 0;
	}

	error_matrix[nObjectClass*nClasses+best_class] ++;
}


void CSimilarityHelper::TestObjectList(IUnknown* punkList)
{	// протестировать на Object List -
	// чтобы оценить точность классификации и важность параметров

	ASSERT(bInited);
	if(!bInited) return; // не проинитили

	INamedDataObject2Ptr sptrL(punkList);
	if(sptrL==0) return; // неудача - фигню передали

	long pos=0;
	sptrL->GetFirstChildPosition(&pos);
	while(pos)
	{
		IUnknownPtr ptr;
		sptrL->GetNextChild(&pos, &ptr);
		ProcessObject(ptr);
		ProcessObjectError();
	}
}

double CSimilarityHelper::CalcError(IUnknown* punkList)
{
	ASSERT(bInited);
	if(!bInited) return 1e200; // не проинитили

	INamedDataObject2Ptr sptrL(punkList);
	if(sptrL==0) return 1e200; // неудача - фигню передали

	int nError=0, nTotal=0;

	long pos=0;
	sptrL->GetFirstChildPosition(&pos);
	while(pos)
	{
		IUnknownPtr ptr;
		sptrL->GetNextChild(&pos, &ptr);
		ProcessObject(ptr);
		if(!bProcessed) continue;

		if(nObjectClass!=best_class) nError++;
		nTotal++;
	}

	return double(nError)/nTotal;
}


void CSimilarityHelper::TestObjectList2(IUnknown* punkList)
{	// протестировать на Object List -
	// чтобы оценить важность параметров
	// и выбрать, какой из них лучше добавить, а какой - удалить

	ASSERT(bInited);
	if(!bInited) return; // не проинитили

	bTestObjectList2Done = false;
	params_error.resize(nParams);

	for(int iParam=0; iParam<nParams; iParam++)
	{
		double tmp = params_weight[iParam];
		if(tmp > 1e-5)
			params_weight[iParam] = 0; // параметр был включен - выключим
		else
			params_weight[iParam] = 1; // параметр был выключен - включим

		params_error[iParam] = CalcError(punkList);

		params_weight[iParam] = tmp;
	}
	// уже нашли лучший параметр для включения и для выключения

	current_error = CalcError(punkList);

	bTestObjectList2Done = true;
}

void CSimilarityHelper::WriteTestReport(const wchar_t* wszFileName)
{ // сохранить отчет по тестированию

	FILE* f = _wfopen(wszFileName, L"w, ccs=UTF-8");
	fputws(L"<html>", f);
	fputws(L"<head>", f);
    fputws(L"<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; CHARSET=utf-8\">", f);
    fputws(L"<TITLE>Таблица статистики</TITLE>", f);
	fputws(L"</head>", f);
	fputws(L"<body>\n\r", f);

	if(bErrorInited)
	{
		// Поиск актуальных столбцов и строк
		vector<bool> exclude_rows(nClasses, true);
		// где true - исключен и столбец, и строка

		int nError=0, nTotal=0;

		for(int k=0; k<nClasses; k++)
		{
			for(int i=0; i<nClasses; i++)
			{
				if(error_matrix[k*nClasses+i])
				{
					nTotal += error_matrix[k*nClasses+i];
					if(i!=k) nError += error_matrix[k*nClasses+i];
					exclude_rows[k] = false;
					exclude_rows[i] = false;
				}
			}
		}

		int nHideEmptyClasses = GetValueInt( ::GetAppUnknown(), "\\SimilarityHelper", "HideEmptyClasses", 0 );
		if(!nHideEmptyClasses) // если не прятать - во всех exclude_rows будет false
			for(int i=0; i<nClasses; i++)
				exclude_rows[i] = false;



		fwprintf(f, L"<TABLE BORDER=1 BORDERCOLOR=\"#000000\" CELLPADDING=1 CELLSPACING=0>\n");

		// ----------------

		fwprintf(f, L"<TR>\n");
		fwprintf(f, L"<TD> </TD>\n");
		for(int i=0; i<nClasses; i++)
		{
			if(exclude_rows[i]) continue;
			fwprintf(f, L"<TD>%s</TD>\n", _bstr_t(get_class_name(i)).GetBSTR());
		}
		fwprintf(f, L"</TR>\n");

		// ----------------
		for(int k=0; k<nClasses; k++)
		{
			if(exclude_rows[k]) continue;

			fwprintf(f, L"<TR>\n");
			fwprintf(f, L"<TD>%s</TD>\n", _bstr_t(get_class_name(k)).GetBSTR());
			for(int i=0; i<nClasses; i++)
			{
				if(exclude_rows[i]) continue;

				COLORREF color = 0xFFFFFF;
				if(error_matrix[k*nClasses+i]==0)
				{
					color = (i==k) ? 0xF0F0F0 : 0xFFFFFF;
				}
				else
				{
					color = (i==k) ? 0xE8FFE8 : 0xFFF0F0;
				}
				fwprintf(f, L"<TD BGCOLOR=#%6X>%i</TD>\n", color, error_matrix[k*nClasses+i]);
			}
			fwprintf(f, L"</TR>\n");
		}
		// ----------------

		fwprintf(f, L"</TABLE>\n");

		fwprintf(f, L"<P>Total Errors: %i/%i. (%1.2f%%)</P>\n",
			nError, nTotal, nError*100.0/nTotal);
	}

	if(bTestObjectList2Done)
	{
		fwprintf(f, L"<P>Рекомендации по включению/выключению параметров:\n");

		vector< pair<double,int> > tmp(nParams);
		for(int i=0; i<nParams; i++)
		{
			tmp[i].first = params_error[i];
			tmp[i].second = i;
		}
		sort(tmp.begin(), tmp.end());

		for(int i=0; i<nParams; i++)
		{
			int iParam = tmp[i].second;
			double err = tmp[i].first;
			if(err>current_error) break;

			if(params_weight[iParam]<1e-5)
			{
				fwprintf(f, L"<BR><FONT COLOR=#00C000>+%s (%1.2f%%)</FONT>\n",
					param_names[iParam].c_str(),
					(err-current_error)*100.0);
			}
			else
			{
				fwprintf(f, L"<BR><FONT COLOR=#C06060>-%s (%1.2f%%)</FONT>\n",
					param_names[iParam].c_str(),
					(err-current_error)*100.0);
			}
		}
		fwprintf(f, L"</P>\n");
	}

	fputws(L"</body>", f);
	fputws(L"</html>", f);
	fclose(f);
}

double CSimilarityHelper::GetError2()
{ // получить ошибку, которая была при TestObjectList2
	if(!bTestObjectList2Done) return 1.0; // если не тестили - 100% ошибки

	return current_error;
}

double CSimilarityHelper::Improve2()
{ // один шаг оптимизации, возвращает, на сколько прооптимизировали; только после TestObjectList2

	if(!bTestObjectList2Done) return 0.0; // если не тестили - не прооптимизируем

	vector< pair<double,int> > tmp(nParams);
	for(int i=0; i<nParams; i++)
	{
		tmp[i].first = params_error[i];
		tmp[i].second = i;
	}
	sort(tmp.begin(), tmp.end());

	if( tmp[0].first>current_error )
		return 0.0; // если никак не прооптимизировать - нафиг

	for(int n=1; n<nParams; n++)
		if( tmp[n].first != tmp[0].first ) break;
	// теперь n: tmp[0]=...=tmp[n-1]

	int iParam = tmp[rand()%n].second; // номер оптимизируемого параметра

	if(params_weight[iParam] > 1e-5)
	{
		params_weight[iParam] = 0; // параметр был включен - выключим
		WritePrivateProfileString( "ParamsWeight", CString(param_names[iParam].c_str()),
			"0", full_classifiername(0) );
	}
	else
	{
		params_weight[iParam] = 1; // параметр был выключен - включим
		WritePrivateProfileString( "ParamsWeight", CString(param_names[iParam].c_str()),
			"1", full_classifiername(0) );
	}


	params_error[iParam] = current_error*2-params_error[iParam]; // примерная прикидка - чтобы хоть кое-как несколько вызовов подряд работало

	return current_error - tmp[0].first;
}
