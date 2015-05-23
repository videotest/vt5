#if !defined(__VcvDataArray_H__)
#define __VcvDataArray_H__

#include "Vcv.h"
#include "StdioFile.h"


class CVcvDataArray
{
	VcvChromoData *pArray;
	int nSize;
public:
	int nFeatures;
	long *plFeatKeys;
	CVcvDataArray();
	~CVcvDataArray();
	void Alloc(int nSize);
	void Add(int nSize);
	VcvChromoData &operator[](int i) {return pArray[i];}
	int GetSize() {return nSize;}
	VcvChromoData *GetData() {return pArray;}
	bool InitFeatures(ICalcObjectPtr sptrObj, IUnknown *punkObjectsList);
	void InitFeatures(int nFeats);
};

class _StatFile : public _StdioFile
{
public:
	_StatFile(char *pszName)
	{
		int nStat = ::GetValueInt(GetAppUnknown(), "Woolz", "Statistics", 1);
		if (nStat)
		{
			char szRptPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
			GetModuleFileName(NULL,szRptPath,_MAX_PATH);
			_splitpath(szRptPath,szDrive,szDir,NULL,NULL);
			_makepath(szRptPath,szDrive,szDir,pszName, NULL);
			try
			{
				Open(szRptPath, "wt");
			}
			catch(_StdioFileException *e)
			{
				e->ReportError();
				e->Delete();
			}
		}
	}
};


#endif
