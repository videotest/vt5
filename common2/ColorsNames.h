#pragma once

class CColorsNames : public IUnknown
{
	CStringW _sSection;
public:
	struct ColorName{
		ColorName(){}
		ColorName(COLORREF clr, CString Name):_clr(clr), _Name(Name){}
		COLORREF _clr;
		CString _Name;
	};
private:
	std::map<int,ColorName> _mapDscrClass;
public:
	virtual ~CColorsNames(){}
	void UpdateColorsNames(const char *szClassifierShortName);
	const char *get_class_name(const long lpos);
	const COLORREF get_class_color(const long lpos);
	bool testClass(const long lpos);
	bool ParseClassifier(const char *szClassifierName=0);
	bool LoadColorsNames(INamedData* pNamedData=0);
	bool Save();
	bool SetClasses(const std::vector<long>& aVecClasses, INamedData* pNamedData=0);
	bool RestoreByClassifier(const std::vector<long>& aVecClasses);
};

namespace{
	const CComBSTR CLASSIFIER(L"\\Classifier");
	const CComBSTR CLASSIFIER_CLASSES(L"\\Classifier\\Classes");
}
