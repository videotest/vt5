#include "StdAfx.h"
#include "resource.h"
#include "LearningInfo.h"
#include "misc_calibr.h"
#include "misc_templ.h"
#include "data5.h"
#include "Classes5.h"
#include "misc_utils.h"
#include "class_utils.h"
#include "MessageException.h"
#include "StdioFile.h"
#include "Object5.h"
#include <math.h>

#define LEARNINGINFO "LearningInfo"
#define LEARNINGINFO1 "[LearningInfo]"
const int nMaxBuf = 64000;

static double __GetValue(ICalcObject *pObj, long lKey)
{
	double dVal;
	HRESULT hr = pObj->GetValue(lKey, &dVal);
	if (FAILED(hr))
	{
		BOOL bMessages = ::GetValueInt(::GetAppUnknown(), "Classification", "Messages", 0);
		if (bMessages)
			throw new CMessageException(IDS_PARAMETER_ABSENT);
		else
			dVal = 0.;
	}
	return dVal;
}

static void __Exchange(int &n1, int &n2)
{
	int t = n1;
	n1 = n2;
	n2 = t;
}

static void __Exchange(_bstr_t &bstr1, _bstr_t &bstr2)
{
	_bstr_t bstr = bstr1;
	bstr1 = bstr2;
	bstr2 = bstr;
}

static IUnknownPtr FindComponentByName(IUnknown *punkThis, const IID iid, _bstr_t strName)
{
	_bstr_t strCompName;
	if (CheckInterface(punkThis, iid))
	{
		strCompName = GetName(punkThis);
		if (strCompName == strName)
			return punkThis;
	}
	IUnknownPtr punk;
	if (CheckInterface(punkThis, IID_ICompManager))
	{
		ICompManagerPtr	sptr(punkThis);
		int nCount = 0;
		sptr->GetCount(&nCount);
		for (int i = 0; i < nCount; i++)
		{
			sptr->GetComponentUnknownByIdx(i, &punk);
			if (punk != 0)
			{
				if (CheckInterface(punk, iid))
				{
					strCompName = ::GetName(punk);
					if (strCompName == strName)
						return punk;
				}
			}
		}
	}
	if (CheckInterface(punkThis, IID_IRootedObject))
	{
		IRootedObjectPtr	sptrR(punkThis);
		IUnknownPtr punkParent;
		sptrR->GetParent(&punkParent);
		punk = FindComponentByName(punkParent, iid, strName);
	}
	return punk;
}

CFeaturesArray::CFeaturesArray()
{
	nFeatures = 0;
	plFeatKeys = NULL;
}

CFeaturesArray::~CFeaturesArray()
{
	if (plFeatKeys)
		delete plFeatKeys;
}

void CFeaturesArray::Deinit()
{
	if (plFeatKeys)
		delete plFeatKeys;
	plFeatKeys = NULL;
	nFeatures = 0;
}

bool CFeaturesArray::InitFeatByStr(char *pszString)
{
	char *buff = pszString;
	// 1. Calc Features
	int nFeatures = 0;
	while (1)
	{
		char *p = strchr(buff, ',');
		if (!p)
			break;
		nFeatures++;
		buff = p+1;
	}
	if (nFeatures)
		nFeatures++;
	else
	{ // if no commas in string , string may be empty or contain one integer
		if (buff[0] && (isdigit_ru(buff[0]) || buff[0]=='-' && isdigit_ru(buff[1])))
			nFeatures++;
	}
	if (nFeatures == 0)
		return false;
	// 2. Read features from string
	_ptr_t2<long> FeatBuff(nFeatures);
	buff = pszString;
	int nFeatures1 = 0;
	while (1)
	{
		long nKey = atol(buff);
		FeatBuff[nFeatures1] = nKey;
		char *p = strchr(buff, ',');
		if (!p)
			break;
		nFeatures1++;
		if (nFeatures1 >= nFeatures)
			break;
		buff = p+1;
	}
	plFeatKeys = new long[nFeatures];
	memcpy(plFeatKeys, FeatBuff, nFeatures*sizeof(long));
	this->nFeatures = nFeatures;
	return true;
}

bool CFeaturesArray::InitFromString(char *pszString)
{
	char *buff = pszString;
	// 1. Calc Features
	int nFeatures = 0;
	while (1)
	{
		char *p = strchr(buff, ',');
		if (!p)
			break;
		nFeatures++;
		buff = p+1;
	}
	if (nFeatures)
		nFeatures++;
	else
	{ // if no commas in string , string may be empty or contain one integer
		if (buff[0] && (isdigit_ru(buff[0]) || buff[0]=='-' && isdigit_ru(buff[1])))
			nFeatures++;
	}
	if (nFeatures == 0)
		return false;
	// 2. Read features from string
	_ptr_t2<long> FeatBuff(nFeatures);
	buff = pszString;
	int nFeatures1 = 0;
	while (1)
	{
		long nKey = atol(buff);
		FeatBuff[nFeatures1] = nKey;
		char *p = strchr(buff, ',');
		if (!p)
			break;
		nFeatures1++;
		if (nFeatures1 >= nFeatures)
			break;
		buff = p+1;
	}
	plFeatKeys = new long[nFeatures];
	memcpy(plFeatKeys, FeatBuff, nFeatures*sizeof(long));
	this->nFeatures = nFeatures;
	return true;
}

struct CKeyInfo
{
	char *pszName;
	long lKey;
	bool bEnable;
	CKeyInfo()
	{
		pszName = NULL;
		lKey = -1;
		bEnable = false;
	}
	~CKeyInfo()
	{
		delete pszName;
	}
	void SetName(char *p)
	{
		delete pszName;
		pszName = new char[strlen(p)+1];
		strcpy(pszName, p);
	}
};

bool CFeaturesArray::InitFromClassFile(const char *pszClassFile)
{
	bool bFromClassFile = false;
	_ptr_t2<CKeyInfo> Keys;
	int nEnabled = 0;
	// 1. try init from classfile
	const char *pszSection = "Classification\\Parameters";
	_ptr_t2<char> pBuff(60000);
	if (GetPrivateProfileString(pszSection, NULL, NULL, pBuff, pBuff.size(), pszClassFile))
	{
		bFromClassFile = true;
		// Calculate number of entries in section
		int nParams = 0;
		for (char *p = pBuff; ; p++)
		{
			if (*p == 0)
			{
				nParams++;
				if (p[1] == 0)
					break;
			}
		}
		Keys.alloc(nParams);
		// Read entries and calculate enabled
		nParams = 0;
		for (char *p = pBuff, *pPrev = pBuff; ; p++)
		{
			if (*p == 0)
			{
				Keys[nParams].SetName(pPrev);
				BOOL bEnable = GetPrivateProfileInt(pszSection, pPrev, FALSE, pszClassFile);
				Keys[nParams].bEnable = bEnable?true:false;
				if (bEnable)
					nEnabled++;
				nParams++;
				pPrev = p+1;
				if (p[1] == 0)
					break;
			}
		}
	}
	else // 2. try to get parameters from shell.data
	{
		INamedDataPtr sptrNDApp(GetAppUnknown());
		sptrNDApp->SetupSection(_bstr_t(pszSection));
		long nEntries = 0;
		sptrNDApp->GetEntriesCount(&nEntries);
		Keys.alloc(nEntries);
		for (long i = 0; i < nEntries; i++)
		{
			BSTR bstr = 0;
			sptrNDApp->GetEntryName(i, &bstr);
			_bstr_t bstr1(bstr, false);
			Keys[i].SetName(bstr1);
			BOOL bEnable = ::GetValueInt(GetAppUnknown(), pszSection, bstr1, FALSE);
			Keys[i].bEnable = bEnable?true:false;
			if (bEnable)
				nEnabled++;
		}
		if (nEntries == 0)         // if neither class file nor shell.data contains parametrs,
			bFromClassFile = true; // write its templates into class file.
	}
	if (nEnabled == 0)
		throw new CMessageException(IDS_NO_PARAMETERS);
	// 3. Parameters obtained. Now get it keys and check for existance.
	IUnknownPtr punk(FindComponentByName(GetAppUnknown(), IID_IMeasureManager, szMeasurement));
	ICompManagerPtr	ptrManager(punk);
	int	nCount,iGroup;
	ptrManager->GetCount(&nCount);
	for (int iParam = 0; iParam < Keys.size(); iParam++)
	{
		if (!Keys[iParam].bEnable) continue;
		_bstr_t bstrName(Keys[iParam].pszName);
		bool bFound = false;
		for (iGroup = 0; iGroup < nCount; iGroup++)
		{
			IUnknownPtr punk;
			ptrManager->GetComponentUnknownByIdx(iGroup, &punk);
			IMeasParamGroupPtr	ptrG(punk);
			if (ptrG == 0) continue;
			LONG_PTR lPos = 0;
			if (FAILED(ptrG->GetFirstPos(&lPos))) continue;
			while (lPos)
			{
				ParameterDescriptor *pDescr = NULL;
				if (SUCCEEDED(ptrG->GetNextParam(&lPos, &pDescr)) && pDescr)
				{
					if (wcscmp(pDescr->bstrName, bstrName) == 0)
					{
						Keys[iParam].lKey = pDescr->lKey;
						bFound = true;
					}
				}
			}
		}
		if (!bFound)
			return false;
	}
	// 4. Write 0 for values which doesn't exists in class file or shell.data (only params
	// enabled in section [measurement\\Parameters]).
	for (iGroup = 0; iGroup < nCount; iGroup++)
	{
		IUnknownPtr punk;
		ptrManager->GetComponentUnknownByIdx(iGroup, &punk);
		IMeasParamGroupPtr	ptrG(punk);
		if (ptrG == 0) continue;
		LONG_PTR lPos = 0;
		if (FAILED(ptrG->GetFirstPos(&lPos))) continue;
		while (lPos)
		{
			ParameterDescriptor *pDescr = NULL;
			if (SUCCEEDED(ptrG->GetNextParam(&lPos, &pDescr)) && pDescr)
			{
				if (pDescr->bstrName && pDescr->lEnabled)
				{
					bool bFound = false;
					for (iParam = 0; iParam < Keys.size(); iParam++)
					{
						_bstr_t bstrName(Keys[iParam].pszName);
						if (wcscmp(bstrName, pDescr->bstrName) == 0)
						{
							bFound = true;
							break;
						}
					}
					if (!bFound)
					{
						char *p = _com_util::ConvertBSTRToString(pDescr->bstrName);
						if (bFromClassFile)
							WritePrivateProfileString(pszSection, p, "0", pszClassFile);
						else
							SetValue(GetAppUnknown(), pszSection, p, 0L);
						delete p;
					}
				}
			}
		}
	}
	// 5. Allocate array.
	plFeatKeys = new long[nEnabled];
	int n = 0;
	for (int iParam = 0; iParam < Keys.size(); iParam++)
	{
		if (Keys[iParam].bEnable)
			plFeatKeys[n++] = Keys[iParam].lKey;
	}
	nFeatures = nEnabled;
	return true;
}

#if 0

bool CFeaturesArray::InitFeatures(IUnknown *punkObjectsList)
{
	if (plFeatKeys) return true;
	ICalcObjectContainerPtr sptrCOCObjList(punkObjectsList);
	// Calculate total number of features.
	long nFeatAll = 0;
	sptrCOCObjList->GetParameterCount(&nFeatAll);
	_ptr_t2<long> arrAllFeats(nFeatAll);
	_ptr_t2<_bstr_t> arrAllFeatNames(nFeatAll);
	int nFeatEn = 0; // we will count number of features enabled for classification
	long pos;
	long key;
	struct ParameterContainer *pParamDef;
	sptrCOCObjList->GetFirstParameterPos(&pos);
	while(pos)
	{
		sptrCOCObjList->GetNextParameter(&pos, &pParamDef);
		if (!pParamDef || !pParamDef->pDescr)
			continue;
		key = pParamDef->pDescr->lKey;
		_bstr_t bstrParamName = pParamDef->pDescr->bstrName;
		int nEnabled = _GetClassifierInt("Classification\\Parameters", bstrParamName, 0, m_bstrLearnInfo);
		if (nEnabled)
			arrAllFeats[nFeatEn++] = key;
	}
	if (nFeatEn == 0)
		return false;
	plFeatKeys = new long[nFeatEn];
	for (int i = 0; i < nFeatEn; i++)
		plFeatKeys[i] = arrAllFeats[i];
	nFeatures = nFeatEn;
	return true;
}

bool CFeaturesArray::InitFeatures(_bstr_t bstrKeys, IUnknown *punkObjectsList)
{
	char *buff = bstrKeys;
	// 1. Calc Features
	int nFeatures = 0;
	while (1)
	{
		char *p = strchr(buff, ',');
		if (!p)
			break;
		nFeatures++;
		buff = p+1;
	}
	if (nFeatures)
		nFeatures++;
	else
	{ // if no commas in string , string may be empty or contain one integer
		if (buff[0] && (isdigit_ru(buff[0]) || buff[0]=='-' && isdigit_ru(buff[1])))
			nFeatures++;
	}
	if (nFeatures == 0)
		if (punkObjectsList)
			return InitFeatures(punkObjectsList);
		else
			return false;
	// 2. Read features from string
	_ptr_t2<long> FeatBuff(nFeatures);
	buff = bstrKeys;
	int nFeatures1 = 0;
	while (1)
	{
		long nKey = atol(buff);
		FeatBuff[nFeatures1] = nKey;
		char *p = strchr(buff, ',');
		if (!p)
			break;
		nFeatures1++;
		if (nFeatures1 >= nFeatures)
			break;
		buff = p+1;
	}
	plFeatKeys = new long[nFeatures];
	memcpy(plFeatKeys, FeatBuff, nFeatures*sizeof(long));
	this->nFeatures = nFeatures;
	return true;
}
#endif

_bstr_t CFeaturesArray::ConvertToStr()
{
	_bstr_t bstr;
	for (int i = 0; i < nFeatures; i++)
	{
		char buff[50];
		ltoa(plFeatKeys[i], buff, 10);
		bstr += buff;
		if (i < nFeatures-1)
			bstr += ",";
	}
	return bstr;
}


CVcvDataArray::CVcvDataArray()
{
	pArray = NULL;
	nAlloc = nSize = m_nPreallocSize = 0;
}

CVcvDataArray::~CVcvDataArray()
{
	for (int i = 0; i < nSize; i++)
	{
		delete pArray[i].pdValues;
	}
	delete pArray;
}

void CVcvDataArray::Prealloc(int nAdded)
{
	bool bAdd = pArray!=NULL;
	if (bAdd)
		nAlloc += nAdded;
	else
		nAlloc = nAdded;
	VcvChromoData *p = new VcvChromoData[nAlloc];
	memset(p, 0, nAlloc*sizeof(VcvChromoData));
	if (bAdd)
		memcpy(p, pArray, nSize*sizeof(VcvChromoData));
	else
		nSize = 0;
	delete pArray;
	pArray = p;
}

int CVcvDataArray::AddObjects(int nObjects)
{
	if (nAlloc <= nSize)
		Prealloc(max(nObjects,m_nPreallocSize));
	for (int i = 0; i < nObjects; i++)
	{
		pArray[nSize].pdValues = new double[nFeatures];
		for (int j = 0; j < nFeatures; j++)
			pArray[nSize].pdValues[j] = 0.;
		nSize++;
	}
	return nSize-1;
}

bool CVcvDataArray::InitObjectsAndFeatures(int nObjects, _bstr_t bstrKeys)
{
	Deinit();
	if (!InitFromString(bstrKeys))
		return false;
	AddObjects(nObjects);
	return true;
}

void CVcvDataArray::Deinit()
{
	for (int i = 0; i < nSize; i++)
	{
		delete pArray[i].pdValues;
	}
	delete pArray;
	nAlloc = nSize = 0;
	pArray = NULL;
	CFeaturesArray::Deinit();
}

CLearningInfo::CLearningInfo(const char *pszClassFile, IUnknown *punkObjectsList, CLearningInfoState *pSaveForUndo)
{
	InitFilesPaths(pszClassFile);
	m_nClassesCount = -1;
	m_bAddObjectsMode = 0;
	if (punkObjectsList)
		BeginAddObjects(punkObjectsList,pSaveForUndo);
}

CLearningInfo::~CLearningInfo()
{
	if (m_bAddObjectsMode)
		EndAddObjects();
	else
		m_FeaturesArray.Deinit();
}

void CLearningInfo::InitFilesPaths(const char *pszClassFile)
{
	// Make path to learning info
	char szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME],szExt[_MAX_EXT],szPath[_MAX_PATH],szPath1[_MAX_PATH];
	local_filename(szPath1, "classes\\LearningInfo.ini", _MAX_PATH);
	_GetClassifierString("Classes","LearningInfo",szPath1,szPath,_MAX_PATH,pszClassFile);
	if (pszClassFile)
	{
		_splitpath(szPath,szDrive,szDir,szFName,szExt);
		_splitpath(pszClassFile,szDrive[0]?0:szDrive,szDir[0]?0:szDir,0,0);
		_makepath(szPath,szDrive,szDir,szFName,szExt[0]?szExt:".ini");
		m_bstrClassInfo = pszClassFile;
	}
	m_bstrLearnInfo = szPath;
}

static bool _ReadLine(FILE *f, _bstr_t &bstrKey, _bstr_t &bstrValue, bool &bSecName)
{
	_ptr_t2<char> buf(nMaxBuf);
	buf.zero();
	int n = 0,c;
	do
	{
		c = fgetc(f);
		if (c == EOF)
			return false;
	}
	while (c == '\n' || c == '\r');
	do
	{
		if (n < nMaxBuf)
			buf[n++] = (char)c;
		c = fgetc(f);
	}
	while (c != '\n' && c != '\r' && c != EOF);
	if (buf[0] == '[')
	{ // Section name
		char *p = strchr(buf, ']');
		if (p)
		{
			p[1] = 0;
			bstrKey = buf;
			bSecName = true;
			return true;
		}
	}
	bSecName = false; // Key value
	char *p = strchr(buf, '=');
	if (p) p++[0] = 0;
	bstrKey = buf;
	if (p) bstrValue = p;
	return true;
}


bool CLearningInfo::ReadKeyAndValue(FILE *f, _bstr_t &bstrKey, _bstr_t &bstrValue)
{
	bool bSecName,bRead;
	do
	{
		bRead = _ReadLine(f, bstrKey, bstrValue, bSecName);
		if (bRead && bSecName)
			m_bstrCurSec = bstrKey;
	}
	while (bRead && (bSecName || bstrKey.length() == 0));
	return bRead;
	return false;
}

_bstr_t CLearningInfo::MakeObjStr(int nObj)
{
	VcvChromoData &Obj = m_FeaturesArray[nObj];
	_bstr_t bstrObjStr;
	_ptr_t2<char> buff(255);
	_itoa(Obj.nClass+1, buff, 10); // Class number will be from 1
	bstrObjStr += (char *)buff;
	bstrObjStr += ":";
	for (int i = 0; i < m_FeaturesArray.nFeatures; i++)
	{
		double dVal = Obj.pdValues[i];
		if (fabs(dVal) < 0.1)
			sprintf(buff, "%e", dVal);
		else
			sprintf(buff, "%.3f", dVal);
		bstrObjStr += (char *)buff;
		if (i < m_FeaturesArray.nFeatures-1)
			bstrObjStr += ",";
	}
	return bstrObjStr;
}


void CLearningInfo::ParseObject(_bstr_t bstrObjStr, int nObject)
{
	const char *buf = bstrObjStr;
	VcvChromoData *pData = &m_FeaturesArray[nObject];
	pData->nClass = atoi(buf)-1;
	char *p1 = (char*)strchr(buf, ':')+1;
	int iFeat = 0;
	while (p1 && iFeat < m_FeaturesArray.nFeatures)
	{
		char *p2;
		pData->pdValues[iFeat++] = strtod(p1, &p2);
		p1 = strchr(p1, ',')+1;
	}
}

static bool EnumMissedParams(_ptr_t2<long> &MissedParams, int nEnable = -1)
{
	IUnknownPtr punk(FindComponentByName(GetAppUnknown(), IID_IMeasureManager, szMeasurement));
	ICompManagerPtr	ptrManager(punk);
	int	nCount;
	ptrManager->GetCount(&nCount);
	for (int iMiss = 0; iMiss < MissedParams.size(); iMiss++)
	{
		bool bFound = false;
		for (int iGroup = 0; iGroup < nCount; iGroup++)
		{
			IUnknownPtr punk;
			ptrManager->GetComponentUnknownByIdx(iGroup, &punk);
			IMeasParamGroupPtr	ptrG(punk);
			if (ptrG == 0) continue;
			LONG_PTR lPos = 0;
			if (SUCCEEDED(ptrG->GetPosByKey(MissedParams[iMiss], &lPos)) && lPos)
			{
				ParameterDescriptor *pDescr = NULL;
				if (SUCCEEDED(ptrG->GetNextParam(&lPos, &pDescr)) && pDescr)
				{
					bFound = true;
					if (nEnable > -1)
						pDescr->lEnabled = nEnable;
				}
			}
		}
		if (!bFound) return false;
	}
	return true;
}

void CalcObjectList(IUnknown* punkObjList, bool bSetParams, ICompManager *pGroupManager)
{
	ICompManagerPtr	ptrManager(pGroupManager);
	if (ptrManager == 0)
	{
		IUnknownPtr punk(FindComponentByName(GetAppUnknown(), IID_IMeasureManager, szMeasurement));
		ptrManager = punk;
		if (ptrManager == 0) return;
	}
	sptrINamedDataObject2 sptrNDO2Obj(punkObjList);
	IDataObjectListPtr ptrObjectList(punkObjList);
	int	nCount;
	ptrManager->GetCount(&nCount);
	for( int idx = 0; idx < nCount; idx++ )
	{
		IUnknownPtr punk;
		ptrManager->GetComponentUnknownByIdx( idx, &punk );
		IMeasParamGroupPtr	ptrG(punk);
		if (ptrG == 0)continue;
		if( bSetParams )
			ptrG->InitializeCalculation( ptrObjectList );

		TPOS pos = 0;
		sptrNDO2Obj->GetFirstChildPosition(&pos);
		while (pos)
		{
			IUnknownPtr punkObj;
			sptrNDO2Obj->GetNextChild(&pos, &punkObj);
			ICalcObjectPtr sptrCO(punkObj);
			IMeasureObjectPtr sptrMO(punkObj);
			if (sptrCO == 0 || sptrMO == 0) continue;
			IUnknownPtr punkImage;
			sptrMO->GetImage(&punkImage);
			ptrG->CalcValues(sptrCO, punkImage);
		}
		if( bSetParams )
			ptrG->FinalizeCalculation();
	}
}


bool CLearningInfo::FindMissedParams(IUnknown *punkObjList, _ptr_t2<long> &MissedParams)
{
	ICalcObjectContainerPtr sptrCOCObjList(punkObjList);
	int nMissedParams = 0;
	for (int i = 0; i < m_FeaturesArray.nFeatures; i++)
	{
		long lKey = m_FeaturesArray.plFeatKeys[i];
		ParameterContainer *pCont;
		if (FAILED(sptrCOCObjList->ParamDefByKey(lKey, &pCont)) || pCont == NULL || pCont->pDescr == NULL)
			nMissedParams++;
	}
	if (nMissedParams == 0) return false;
	MissedParams.alloc(nMissedParams);
	int n = 0;
	for (i = 0; i < m_FeaturesArray.nFeatures; i++)
	{
		long lKey = m_FeaturesArray.plFeatKeys[i];
		ParameterContainer *pCont;
		if (FAILED(sptrCOCObjList->ParamDefByKey(lKey, &pCont)) || pCont == NULL || pCont->pDescr == NULL)
			MissedParams[n++] = lKey;
	}
	return true;
}


bool CLearningInfo::BeginAddObjects(IUnknown *punkObjectsList, CLearningInfoState *pSaveForUndo)
{
	// 1. Read LearnInfo
	bool bEmpty = false;
	try
	{
		bEmpty = !ReadData();
	}
	catch(_StdioFileException *e)
	{
		e->Delete();
		bEmpty = true; 
	}
	if (bEmpty)
	{
		// Empty teach database. Initialize keys list from classify info or shell.data.
		if (!m_FeaturesArray.InitFromClassFile(m_bstrClassInfo))
			throw new CMessageException(IDS_PARAMETER_ABSENT);
		long lObjects;
		sptrINamedDataObject2 sptrNDO2Obj(punkObjectsList);
		sptrNDO2Obj->GetChildsCount(&lObjects);
		m_FeaturesArray.SetPreallocSize(lObjects);
		m_nClassesCount = m_nObjectsCount = 0;
	}
	// 2. Check object list for compatibility
	_ptr_t2<long> MissedKeys;
	if (FindMissedParams(punkObjectsList,MissedKeys))
	{
		if (!EnumMissedParams(MissedKeys))
			throw new CMessageException(IDS_PARAMETER_ABSENT);
		EnumMissedParams(MissedKeys, 1);
		CalcObjectList(punkObjectsList,true,0);
		EnumMissedParams(MissedKeys, 0);
	}
	// 3. Save information for undo
	if (pSaveForUndo)
	{
		pSaveForUndo->m_nObjectsCount = m_nObjectsCount;
		pSaveForUndo->m_nClassesCount = m_nClassesCount;
		pSaveForUndo->m_bstrClassFile = m_bstrClassInfo;
		pSaveForUndo->m_bstrLearnInfo = m_bstrLearnInfo;
		// pSaveForUndo->m_arrObjectStrings will be filled on redo
	}
	m_bAddObjectsMode = m_bAddObjectsOk = true;
	return true;
}

void CLearningInfo::EndAddObjects()
{
	if (m_FeaturesArray.GetSize() && m_bAddObjectsOk)
		WriteData();
	m_FeaturesArray.Deinit();
	m_bAddObjectsMode = false;
}

bool CLearningInfo::AddObjectToLearnInfo(IUnknown *punkObj)
{
	if (!m_bAddObjectsOk) return false;
	ICalcObjectPtr sptrCO(punkObj);
	int nClass = get_object_class(sptrCO); // Class from 0...
	if (nClass <= -1 || nClass >= class_count()) return false; // Unknown object
	m_bAddObjectsOk = false;
	int nObj = m_FeaturesArray.AddObjects();
	m_FeaturesArray[nObj].nClass = nClass;
	for (int i = 0; i < m_FeaturesArray.nFeatures; i++)
		m_FeaturesArray[nObj].pdValues[i] = __GetValue(sptrCO, m_FeaturesArray.plFeatKeys[i]);
	m_nObjectsCount = nObj+1;
	m_nClassesCount = max(m_nClassesCount, m_FeaturesArray[nObj].nClass+1);
	m_bAddObjectsOk = true;
	return true;
}

bool CLearningInfo::ReadData(bool bThrowExc)
{
	try 
	{
	_StdioFile F(m_bstrLearnInfo, "rt");
	setvbuf(F.f,NULL,_IOFBF, 16384);
	_bstr_t bstrKeys;
	m_nClassesCount = 0;
	m_nObjectsCount = 0;
	_bstr_t bstrKey;
	_bstr_t bstrValue;
	while (ReadKeyAndValue(F.f, bstrKey, bstrValue))
	{
		if (m_bstrCurSec.length() > 0 && stricmp(m_bstrCurSec, LEARNINGINFO1) == 0)
		{
			if (bstrKey == _bstr_t("Keys"))
				bstrKeys = bstrValue;
			else if (bstrKey == _bstr_t("ClassCount"))
				m_nClassesCount = atoi(bstrValue);
			else if (bstrKey == _bstr_t("ObjectsCount"))
				m_nObjectsCount = atoi(bstrValue);
		}
	}
	if (m_nObjectsCount == 0)
		return false;
	fseek(F.f,0L,SEEK_SET);
	m_bstrCurSec = "";
	if (!m_FeaturesArray.InitObjectsAndFeatures(m_nObjectsCount, bstrKeys))
		return false;
	while (ReadKeyAndValue(F.f, bstrKey, bstrValue))
	{
		if (m_bstrCurSec.length() > 0 && stricmp(m_bstrCurSec, LEARNINGINFO1) == 0)
		{
			int n = atoi(bstrKey)-1;
			if (n >= 0 && n < m_nObjectsCount)
				ParseObject(bstrValue, n);
		}
	}
	}
	catch(_StdioFileException *e)
	{
		if (bThrowExc)
			throw;
		delete e;
		m_nObjectsCount = m_nClassesCount = 0;
		return false;
	}
	return true;
}


void CLearningInfo::WriteData()
{
	_StdioFile F(m_bstrLearnInfo, "wt");
	fprintf(F.f, "%s\n", LEARNINGINFO1);
	_bstr_t bstrKeys = m_FeaturesArray.ConvertToStr();
	fprintf(F.f, "Keys=%s\n", (const char *)bstrKeys);
	fprintf(F.f, "ClassCount=%d\n", m_nClassesCount);
	fprintf(F.f, "ObjectsCount=%d\n", m_nObjectsCount);
	for (int i = 0; i < m_nObjectsCount; i++)
	{
		_bstr_t bstrObj(MakeObjStr(i));
		fprintf(F.f, "%d=%s\n", i+1, (const char *)bstrObj);
	}
}

void CLearningInfo::Empty(CLearningInfoState &LearningInfoState)
{
	bool bEmpty = !ReadData(false);
	if (bEmpty)
	{
		LearningInfoState.m_nObjectsCount = LearningInfoState.m_nClassesCount = 0;
		LearningInfoState.m_arrObjectStrings.free();
	}
	else
	{
		LearningInfoState.m_nObjectsCount = m_nObjectsCount;
		LearningInfoState.m_nClassesCount = m_nClassesCount;
		LearningInfoState.m_arrObjectStrings.alloc(m_nObjectsCount);
		for (int i = 0; i < m_nObjectsCount; i++)
			LearningInfoState.m_arrObjectStrings[i] = MakeObjStr(i);
		LearningInfoState.m_keys = m_FeaturesArray.ConvertToStr();
	}
	m_nObjectsCount = m_nClassesCount = 0;
	m_FeaturesArray.Deinit();
	WriteData();
}

void CLearningInfo::DoUndoRedo(CLearningInfoState &LearningInfoState, bool bUndo)
{
	if (LearningInfoState.m_bAddObjects && bUndo || !LearningInfoState.m_bAddObjects && !bUndo)
	{ // Objects was added during operation and it will be undone or objects was removed and operation will be redone
		int n = m_nObjectsCount-LearningInfoState.m_nObjectsCount;
		LearningInfoState.m_arrObjectStrings.alloc(n);
		for (int i = 0; i < n; i++)
			LearningInfoState.m_arrObjectStrings[i] = MakeObjStr(LearningInfoState.m_nObjectsCount+i);
	}
	else
	{ // Objects was removed during operation and it will be undone or objects was added and action will be redone
		if (m_nObjectsCount == 0) // Classifier was emptied during operation
		{
			if (LearningInfoState.m_keys.length() > 0) // If wasn't empty before action
				m_FeaturesArray.InitFromString(LearningInfoState.m_keys);
			LearningInfoState.m_keys = "";
		}
		int n = LearningInfoState.m_nObjectsCount-m_nObjectsCount; // Removed objects will be restored
		m_FeaturesArray.AddObjects(n);
		for (int i = 0; i < n; i++)
			ParseObject(LearningInfoState.m_arrObjectStrings[i], m_nObjectsCount+i);
	}
	__Exchange(m_nObjectsCount, LearningInfoState.m_nObjectsCount);
	__Exchange(m_nClassesCount, LearningInfoState.m_nClassesCount);
	__Exchange(m_bstrClassInfo, LearningInfoState.m_bstrClassFile);
	__Exchange(m_bstrLearnInfo, LearningInfoState.m_bstrLearnInfo);
	if (m_nObjectsCount == 0 && (LearningInfoState.m_bAddObjects&&bUndo||!LearningInfoState.m_bAddObjects&&!bUndo))
		LearningInfoState.m_keys = m_FeaturesArray.ConvertToStr();
}

void CLearningInfoState::DoUndoRedo(bool bUndo)
{
	CLearningInfo LearnInfo(0);
	LearnInfo.m_bstrClassInfo = m_bstrClassFile;
	LearnInfo.m_bstrLearnInfo = m_bstrLearnInfo;
	bool bEmpty = !LearnInfo.ReadData(false);
	_bstr_t bstrMsg;
	if (bUndo)
		bstrMsg = "Undo not possible";
	else
		bstrMsg = "Redo not possible";
	if (m_bAddObjects && bUndo || !m_bAddObjects && !bUndo)
	{
		if (bEmpty || LearnInfo.m_nObjectsCount < m_nObjectsCount ||
			LearnInfo.m_nClassesCount < m_nClassesCount)
			throw new CMessageException("Undo not possible");
	}
	else
	{
		_MessageBoxCaption MsgBoxCaption;
		if (LearnInfo.m_nObjectsCount > m_nObjectsCount ||
			LearnInfo.m_nClassesCount > m_nClassesCount)
			VTMessageBox("Redo not possible", MsgBoxCaption, MB_OK|MB_ICONEXCLAMATION);
	}
	LearnInfo.DoUndoRedo(*this,bUndo);
	LearnInfo.WriteData();
}




