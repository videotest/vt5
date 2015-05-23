#pragma once

#include <vector>
#include <string>

class CSimilarityHelper
{
public:
	CSimilarityHelper(void);
	~CSimilarityHelper(void);

	void Init(IUnknown* punkList); // прочитать все двнные из списка
	void DeInit();

	void ProcessObject(IUnknown* punkObject); // обсчитать все для объекта; должен быть уже вызван Init

	int GetSimilar(); // вернуть номер наиболее похожего
	int GetSimilarInClass(int nClass); // вернуть номер наиболее похожего в данном классе
	int GetSimilarInOwnClass(); // вернуть номер наиболее похожего в собственном классе

	int GetClassSimilar(); // вернуть номер класса наиболее похожего

	double _GetParamDistanceSqrByIndex(int iParam, int jObject); // вернуть "расстояние" по указанному параметру до данного объекта
	// никаких проверок корректности не ведется!
	double GetParamDistance(int nParam, int jObject); // вернуть "расстояние" по указанному параметру до данного объекта
	double GetParamDistance(int nParam); // вернуть "расстояние" по указанному параметру до лучшего объекта

	void WriteObjectReport(const wchar_t* wszFileName); // сохранить отчет по объекту

	void ProcessObjectError(); // обсчитать для объекта статистику значимости параметров;
	// должен быть уже вызван Process

	void TestObjectList(IUnknown* punkList); // протестировать на Object List -
	// чтобы оценить точность классификации и важность параметров
	void TestObjectList2(IUnknown* punkList); // протестировать на Object List -
	// чтобы оценить точность классификации и важность параметров
	void WriteTestReport(const wchar_t* wszFileName); // сохранить отчет по тестированию

	double CalcError(IUnknown* punkList); // посчитать ошибку по данному OL

	double GetError2(); // получить ошибку, которая была при TestObjectList2
	double Improve2(); // один шаг оптимизации, возвращает, на сколько прооптимизировали; только после TestObjectList2

protected:
	long nClasses, nObjects, nParams;

	// исходные данные с Object List
	vector<long> params;
	vector<wstring> param_names;
	vector<double> params_weight;
	vector<double> values;
	vector<long> classes;
	vector<double> classes_weight;

	// промежуточные данные - статистика
	vector<int> num;
	vector<double>avg;
	vector<double> disp;
	vector<int> num_total;
	vector<double> disp_total;

	// исходные данные с объекта
	vector<double> values_obj;
	int nObjectClass;

	// результаты по объекту
	vector<double> classes_best_d; // лучшие расстояния по классам
	vector<int> classes_best_j; // лучшие объекты по классам
	int best_class; // лучший класс
	int best_j; // лучший объект

	// статистика по куче объектов
	vector<int> error_matrix; // матрица ошибок, [nClasses*nClasses], на диагонали - правильно классифицированные
	// значения - error_matrix[old_class*nClasses+new_class]

	// статистика по TestObjectList2
	double current_error; // текущая ошибка
	vector<double> params_error; // ошибки для разных параметров, [nParams]

	// Степень проиниченности
	bool bInited; // проиничен сам хелпер
	bool bProcessed; // был обработан объект - есть результаты по нему
	bool bErrorInited; // есть хоть какие-то данные ProcessObjectError
	bool bTestObjectList2Done; // есть данные TestObjectList2
};
