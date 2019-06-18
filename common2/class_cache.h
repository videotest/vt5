#pragma once
#include "class_utils.h"
#include <string>
#include <vector>
using namespace std;

class CClassCache
{
public:
	CClassCache(char *szClassifierName=0)
	{
		set_classifier(szClassifierName);
	}

	void set_classifier(char *szClassifierName=0)
	{
		str_short = ::short_classifiername(szClassifierName);
		str_full = ::full_classifiername(str_short.c_str());

		class_count = ::class_count(str_short.c_str());
		ASSERT(class_count>=0 && class_count<=1000);
		class_count = min(1000, max(0, class_count)); // классов от 0 до 1000 - просто на всякий случай

		class_name_unknown = ::get_class_name(-1);
		class_color_unknown = ::get_class_color(-1);

		class_names.resize(class_count);
		class_colors.resize(class_count);
		for(int i=0; i<class_count; i++)
		{ // TODO: лучше бы переделать - чтобы не вызывать много раз вычислении short и full имен друг из друга
			class_names[i] = ::get_class_name(i, str_short.c_str());
			class_colors[i] = ::get_class_color(i, str_short.c_str());
		}
	};

	inline long get_object_class( ICalcObject *pcalc)
	{
		return ::get_object_class(pcalc, str_short.c_str());
	};

	inline string get_class_name(long lpos)
	{
		return (lpos>=0 && lpos<class_count) ? class_names[lpos] : class_name_unknown;
	};

	inline COLORREF get_class_color(long lpos)
	{
		return (lpos>=0 && lpos<class_count) ? class_colors[lpos] : class_color_unknown;
	};

	//inline COLORREF get_smooth_class_color(long lpos, long nClassCount);
	inline long	get_class_count()
	{
		return class_count;
	};

protected:
	string str_full; //classifier_name;
	string str_short; //classifier_name;
	int class_count;
	vector<string> class_names;
	vector<COLORREF> class_colors;
	string class_name_unknown;
	COLORREF class_color_unknown;
};