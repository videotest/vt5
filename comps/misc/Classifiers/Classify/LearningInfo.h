#if !defined(__LearningInfo_H__)
#define __LearningInfo_H__

#include "Vcv.h"
#include "misc_utils.h"
#include "array.h"
#include "Ver30.h"
#include "Profile.h"

class CFeaturesArray
{
	bool InitFeatByStr(char *psz);
public:
	int nFeatures;
	long *plFeatKeys;

	CFeaturesArray();
	~CFeaturesArray();
	bool InitFromString(char *pszString); // Changes string
	bool InitFromClassFile(const char *pszClassFile);
//	bool InitFeatures(IUnknown *punkObjectsList);
//	bool InitFeatures(_bstr_t bstrKeys, IUnknown *punkObjectsList);
	void Deinit();
	_bstr_t ConvertToStr();
};

class CVcvDataArray : public CFeaturesArray
{
	VcvChromoData *pArray;
	int nSize,nAlloc,m_nPreallocSize;
	void Prealloc(int nAdded);
public:
	CVcvDataArray();
	~CVcvDataArray();
	void Deinit();

	// Make array for nObjects and Feature keys from bstrKeys. All data will be allocated and
	// zeroed. Returns false if no keys in string.
	bool InitObjectsAndFeatures(int nObjects, _bstr_t bstrKeys);
	int  AddObjects(int nObjects = 1);
	void SetPreallocSize(int nPreallocSize) {m_nPreallocSize = nPreallocSize;};

//	void Init(IUnknown *punkObjectsList); // Features from object list
//	bool Init(int nObjectsCount, _bstr_t bstrKeys); // features fromfile
	VcvChromoData &operator[](int i) {return pArray[i];}
	int GetSize() {return nSize;}
	VcvChromoData *GetData() {return pArray;}
};

struct VcvParamsHld : public VcvParams
{
	VcvParamsHld(const char *pszClassifierFile = 0)
	{
		if (IsVersion3x())
			nFeatureMethod = _GetClassifierInt("Woolz", "FeatureSelectionMethod", 3, pszClassifierFile);
		else
			nFeatureMethod = 3;
		switch (nFeatureMethod)
		{
		case 1:
			nFeatures = _GetClassifierInt("Woolz", "Features", 1, pszClassifierFile);
			dMaxcor = _GetClassifierDouble("Woolz", "Maxcor", 0., pszClassifierFile);
			break;
		case 2:
			nFeatures = _GetClassifierInt("Woolz", "Features", 1, pszClassifierFile);
			break;
		case 3:
			if (IsVersion3x())
				nFeatures = _GetClassifierInt("Woolz", "Features", 1, pszClassifierFile);
			else
				nFeatures = 3;
			pnExplicitFeatArray = new int[nFeatures];
			for (int i = 0; i < nFeatures; i++)
			{
				char szBuff[50];
				sprintf(szBuff, "Param%d", i+1);
				pnExplicitFeatArray[i] = _GetClassifierInt("Woolz", szBuff, i, pszClassifierFile);
			}
			break;
		}
		nStat = _GetClassifierInt("Woolz", "Statistics", 1, pszClassifierFile);
		if (nStat)
		{
			char szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
			GetModuleFileName(NULL,szRptPath,_MAX_PATH);
			_splitpath(szRptPath,szDrive,szDir,NULL,NULL);
			_makepath(szRptPath,szDrive,szDir,"train.txt", NULL);
		}
		if (pszClassifierFile)
		{
			pszClassFile = new char[strlen(pszClassifierFile)+1];
			strcpy(pszClassFile, pszClassifierFile);
		}
		else
			pszClassFile = 0;
	}
	~VcvParamsHld()
	{
		if (nFeatureMethod == 3)
			delete pnExplicitFeatArray;
		delete pszClassFile;
	}
};


interface ICalcObject;
class CLearningInfoState;
class CLearningInfo
{
	void InitFilesPaths(const char *pszClassFile);
	bool FindMissedParams(IUnknown *punkObjList, _ptr_t2<long> &MissedParams);
public:
	_bstr_t m_bstrLearnInfo,m_bstrClassInfo;
	CVcvDataArray m_FeaturesArray;
	int m_nClassesCount;
	int m_nObjectsCount; // Can be != m_FeaturesArray.nSize. If m_nObjectsCount == 0 then LearnInfo assumed to be empty.
	_bstr_t m_bstrKeys;
	_bstr_t m_bstrCurSec; // ReadKeyAndValue saves section here
	bool m_bAddObjectsMode,m_bAddObjectsOk;

	_bstr_t MakeObjStr(int nObj);
	void ParseObject(_bstr_t bstrObjStr, int nObject);
	bool ReadKeyAndValue(FILE *f, _bstr_t &bstrKey, _bstr_t &bstrValue);

	CLearningInfo(const char *pszClassFile, IUnknown *punkObjectsList = NULL, CLearningInfoState *pSaveForUndo = NULL);
	~CLearningInfo();
	bool BeginAddObjects(IUnknown *punkObjectsList, CLearningInfoState *pSaveForUndo = NULL);
	void EndAddObjects();
	bool AddObjectToLearnInfo(IUnknown *punkObj); // Makes m_nObjectsCount == m_FeaturesArray.nSize
	bool ReadData(bool bThrowExc = true);
	void WriteData();
	void Empty(CLearningInfoState &LearningInfoState);
	void DoUndoRedo(CLearningInfoState &LearningInfoState, bool bUndo);
};


class CLearningInfoState
{
public:
	CLearningInfoState(bool bAddObjects = true)
	{
		m_bAddObjects = bAddObjects;
	}
	int m_nObjectsCount;
	int m_nClassesCount;
	bool m_bAddObjects;
	_array_t<_bstr_t> m_arrObjectStrings; // for redo
	_bstr_t m_keys;
	_bstr_t m_bstrClassFile,m_bstrLearnInfo;
	void DoUndoRedo(bool bUndo);
//	void DoRedo();
};


#endif 