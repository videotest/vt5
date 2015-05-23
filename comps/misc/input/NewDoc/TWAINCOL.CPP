#include "stdafx.h"
#include "TwainCol.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



static float FIX32ToFloat (TW_FIX32 fix32)
{
	float 	floater;
	floater = (float)fix32.Whole + (float)(fix32.Frac / 65536.0);
	return floater;
}

static TW_FIX32 FloatToFix32 (float floater)
{
	TW_FIX32 Fix32_value;
	TW_INT32 value = (TW_INT32) (floater * 65536.0 + 0.5);
	Fix32_value.Whole = (TW_INT16)(value >> 16);
	Fix32_value.Frac = (TW_UINT16)(value & 0x0000ffffL);
	return (Fix32_value);
}

IMPLEMENT_DYNAMIC(CTwainNotImplException,CException)

CTwainNotImplException::CTwainNotImplException(const char *pszName)
{
	m_sFeature = pszName;
}

int CTwainNotImplException::ReportError(UINT nType /*= MB_OK*/, UINT nMessageID /*= 0*/)
{
	char szBuff[50];
	wsprintf(szBuff, "This feature of TWAIN protocol not implemented : %s",
		(const char *)m_sFeature);
	return AfxMessageBox(szBuff, nType);
}


CTwainContainer::CTwainContainer()
{
	m_hContainer = NULL;
}

CTwainContainer::~CTwainContainer()
{
	GlobalFree(m_hContainer);
}

CTwainOneValue::CTwainOneValue(float Value, int nType /*= TWTY_UINT16*/)
{
	m_hContainer = GlobalAlloc(GHND, sizeof(TW_ONEVALUE));
	pTW_ONEVALUE pval = (pTW_ONEVALUE)GlobalLock(m_hContainer);
	if (pval->ItemType == TWTY_FIX32)
	{
		pval->ItemType = TWTY_FIX32;
		(TW_FIX32&)(pval->Item) = FloatToFix32(Value);
	}
	else
	{
		pval->ItemType = nType;
		pval->Item = (TW_UINT32)Value;
	}
	GlobalUnlock(m_hContainer);
}

float CTwainOneValue::GetValue()
{
	pTW_ONEVALUE pval = (pTW_ONEVALUE)GlobalLock(m_hContainer);
	float Item;
	if (pval->ItemType == TWTY_FIX32)
		Item = FIX32ToFloat((TW_FIX32&)pval->Item);
	else
		Item = (float)pval->Item;
	GlobalUnlock(m_hContainer);
	return Item;
}

CTwainEnumeration::CTwainEnumeration(int nNumOfItems, float nCurrent,
	float nDefault, int nType /*== TWTY_UINT16*/)
{
	int nTypeSiz;
	switch (nType)
	{
	case TWTY_INT8:
		nTypeSiz = sizeof(TW_INT8);
		break;
	case TWTY_INT16:
		nTypeSiz = sizeof(TW_INT16);
		break;
	case TWTY_INT32:
		nTypeSiz = sizeof(TW_INT32);
		break;
	case TWTY_UINT8:
		nTypeSiz = sizeof(TW_UINT8);
		break;
	case TWTY_UINT16:
		nTypeSiz = sizeof(TW_UINT16);
		break;
	case TWTY_UINT32:
		nTypeSiz = sizeof(TW_UINT32);
		break;
	case TWTY_BOOL:
		nTypeSiz = sizeof(TWTY_BOOL);
		break;
	case TWTY_FIX32:
		nTypeSiz = sizeof(TWTY_FIX32);
		break;
	}
	m_hContainer = GlobalAlloc(GHND, sizeof(TW_ENUMERATION) +
		nTypeSiz * nNumOfItems);
	pTW_ENUMERATION pval = (pTW_ENUMERATION)GlobalLock(m_hContainer);
	pval->ItemType = nType;
	pval->NumItems = nNumOfItems;
	pval->CurrentIndex = (TW_UINT32)nCurrent;
	pval->DefaultIndex = (TW_UINT32)nDefault;
	GlobalUnlock(m_hContainer);
}

float CTwainEnumeration::GetValue(pTW_ENUMERATION pval, int index)
{
	float ret;
	switch (pval->ItemType)
	{
	case TWTY_INT8:
		ret = ((TW_INT8 *)(pval->ItemList))[index];
		break;
	case TWTY_INT16:
		ret = ((TW_INT16 *)(pval->ItemList))[index];
		break;
	case TWTY_INT32:
		ret = (float)((TW_INT32 *)(pval->ItemList))[index];
		break;
	case TWTY_UINT8:
		ret = ((TW_UINT8 *)(pval->ItemList))[index];
		break;
	case TWTY_UINT16:
		ret = ((TW_UINT16 *)(pval->ItemList))[index];
		break;
	case TWTY_UINT32:
		ret = (float)((TW_UINT32 *)(pval->ItemList))[index];
		break;
	case TWTY_BOOL:
		ret = ((TW_BOOL *)(pval->ItemList))[index];
		break;
	case TWTY_FIX32:
		ret = FIX32ToFloat(((TW_FIX32 *)(pval->ItemList))[index]);
		break;
	}
	return ret;
}

void CTwainEnumeration::SetValue(pTW_ENUMERATION pval, int index,
	float value)
{
	switch (pval->ItemType)
	{
	case TWTY_INT8:
		((TW_INT8 *)(pval->ItemList))[index] = (TW_INT8)value;
		break;
	case TWTY_INT16:
		((TW_INT16 *)(pval->ItemList))[index] = (TW_INT16)value;
		break;
	case TWTY_INT32:
		((TW_INT32 *)(pval->ItemList))[index] = (TW_INT32)value;
		break;
	case TWTY_UINT8:
		((TW_UINT8 *)(pval->ItemList))[index] = (TW_UINT8)value;
		break;
	case TWTY_UINT16:
		((TW_UINT16 *)(pval->ItemList))[index] = (TW_UINT16)value;
		break;
	case TWTY_UINT32:
		((TW_UINT32 *)(pval->ItemList))[index] = (TW_UINT32)value;
		break;
	case TWTY_BOOL:
		((TW_BOOL *)(pval->ItemList))[index] = (TW_BOOL)value;
		break;
	case TWTY_FIX32:
		((TW_FIX32 *)(pval->ItemList))[index] = FloatToFix32(value);
		break;
	}
}

int CTwainEnumeration::GetNumOfItems()
{
	pTW_ENUMERATION pval = (pTW_ENUMERATION)GlobalLock(m_hContainer);
	int ret = pval->NumItems;
	GlobalUnlock(m_hContainer);
	return ret;
}

int CTwainEnumeration::GetCurrent()
{
	pTW_ENUMERATION pval = (pTW_ENUMERATION)GlobalLock(m_hContainer);
	int ret = pval->CurrentIndex;
	GlobalUnlock(m_hContainer);
	return ret;
}

int CTwainEnumeration::GetDefault()
{
	pTW_ENUMERATION pval = (pTW_ENUMERATION)GlobalLock(m_hContainer);
	int ret = pval->DefaultIndex;
	GlobalUnlock(m_hContainer);
	return ret;
}

float CTwainEnumeration::GetAt(int index)
{
	pTW_ENUMERATION pval = (pTW_ENUMERATION)GlobalLock(m_hContainer);
	ASSERT(index < (int)pval->NumItems);
	float ret = GetValue(pval,index);
	GlobalUnlock(m_hContainer);
	return ret;
}

void CTwainEnumeration::SetAt(int index, float value)
{
	pTW_ENUMERATION pval = (pTW_ENUMERATION)GlobalLock(m_hContainer);
	ASSERT(index < (int)pval->NumItems);
	SetValue(pval,index,value);
	GlobalUnlock(m_hContainer);
}

int CTwainEnumeration::Find(float value)
{
	pTW_ENUMERATION pval = (pTW_ENUMERATION)GlobalLock(m_hContainer);
	int ret = -1;
	for ( int i = 0; i < (int)pval->NumItems; i++)
	{
		if (GetValue(pval,i) == value)
		{
			ret = i;
			break;
		}
	}
	GlobalUnlock(m_hContainer);
	return ret;
}

int CTwainEnumeration::FindMoreOrEqual(float value)
{
	pTW_ENUMERATION pval = (pTW_ENUMERATION)GlobalLock(m_hContainer);
	int ret = -1;
	unsigned nMinVal = 0xFFFFFFFF;
	for ( int i = 0; i < (int)pval->NumItems; i++)
	{
		if (GetValue(pval,i) >= value && GetValue(pval,i) <= nMinVal)
		{
			ret = i;
			nMinVal = (unsigned)GetValue(pval,i);
		}
	}
	GlobalUnlock(m_hContainer);
	return ret;
}

int CTwainEnumeration::FindMax()
{
	pTW_ENUMERATION pval = (pTW_ENUMERATION)GlobalLock(m_hContainer);
	int ret = -1;
	if (!pval->NumItems)
		return -1;
	int Found = (int)GetValue(pval,0);
	for ( int i = 1; i < (int)pval->NumItems; i++)
	{
		float Value = GetValue(pval,i);
		if (Value > Found)
		{
			ret = i;
			Found = (int)Value;
			break;
		}
	}
	GlobalUnlock(m_hContainer);
	return ret;
}

inline float GetValue(int nType, DWORD dwData)
{
	float ret;
	switch (nType)
	{
	case TWTY_INT8:
		ret = (TW_INT8)dwData;
		break;
	case TWTY_INT16:
		ret = (TW_INT16)dwData;
		break;
	case TWTY_INT32:
		ret = (float)(TW_INT32)dwData;
		break;
	case TWTY_UINT8:
		ret = (TW_UINT8)dwData;
		break;
	case TWTY_UINT16:
		ret = (TW_UINT16)dwData;
		break;
	case TWTY_UINT32:
		ret = (float)(TW_UINT32)dwData;
		break;
	case TWTY_BOOL:
		ret = (TW_BOOL)dwData;
		break;
	case TWTY_FIX32:
		ret = FIX32ToFloat((TW_FIX32&)dwData);
		break;
	}
	return ret;
}

inline void SetValue(int nType, DWORD &dwDest, float Value)
{
	switch (nType)
	{
	case TWTY_INT8:
		dwDest = (TW_INT8)Value;
		break;
	case TWTY_INT16:
		dwDest = (TW_INT16)Value;
		break;
	case TWTY_INT32:
		dwDest = (TW_INT32)Value;
		break;
	case TWTY_UINT8:
		dwDest = (TW_UINT8)Value;
		break;
	case TWTY_UINT16:
		dwDest = (TW_UINT16)Value;
		break;
	case TWTY_UINT32:
		dwDest = (TW_UINT32)Value;
		break;
	case TWTY_BOOL:
		dwDest = (TW_BOOL)Value;
		break;
	case TWTY_FIX32:
		(TW_FIX32&)dwDest = FloatToFix32(Value);
		break;
	}
}

CTwainRange::CTwainRange(float fMin, float fMax, float fStep, float fCurrent,
	float fDefault, int nType /*= TWTY_FIX32*/)
{
	m_hContainer = GlobalAlloc(GHND, sizeof(TW_RANGE));
	pTW_RANGE pval = (pTW_RANGE)GlobalLock(m_hContainer);
	pval->ItemType = nType;
	SetValue(nType,pval->MinValue,fMin);
	SetValue(nType,pval->MaxValue,fMax);
	SetValue(nType,pval->StepSize,fStep);
	SetValue(nType,pval->DefaultValue,fDefault);
	SetValue(nType,pval->CurrentValue,fCurrent);
	GlobalUnlock(m_hContainer);
}

float CTwainRange::GetMin()
{
	pTW_RANGE pval = (pTW_RANGE)GlobalLock(m_hContainer);
	float value = GetValue(pval->ItemType,pval->MinValue);
	GlobalUnlock(m_hContainer);
	return value;
}

float CTwainRange::GetMax()
{
	pTW_RANGE pval = (pTW_RANGE)GlobalLock(m_hContainer);
	float value = GetValue(pval->ItemType,pval->MaxValue);
	GlobalUnlock(m_hContainer);
	return value;
}

float CTwainRange::GetStep()
{
	pTW_RANGE pval = (pTW_RANGE)GlobalLock(m_hContainer);
	float value = GetValue(pval->ItemType,pval->StepSize);
	GlobalUnlock(m_hContainer);
	return value;
}

float CTwainRange::GetCurrent()
{
	pTW_RANGE pval = (pTW_RANGE)GlobalLock(m_hContainer);
	float value = GetValue(pval->ItemType,pval->CurrentValue);
	GlobalUnlock(m_hContainer);
	return value;
}

float CTwainRange::GetDefault()
{
	pTW_RANGE pval = (pTW_RANGE)GlobalLock(m_hContainer);
	float value = GetValue(pval->ItemType,pval->DefaultValue);
	GlobalUnlock(m_hContainer);
	return value;
}

float CTwainRange::FindMoreOrEqual(float value)
{
	pTW_RANGE pval = (pTW_RANGE)GlobalLock(m_hContainer);
	float MinValue = GetValue(pval->ItemType,pval->MinValue);
	float MaxValue = GetValue(pval->ItemType,pval->MaxValue);
	float StepValue = GetValue(pval->ItemType,pval->StepSize);
	GlobalUnlock(m_hContainer);
	if (value < MinValue) return MinValue;
	if (value > MaxValue) return MaxValue;
	float ret = (float)(ceil((value-MinValue)/StepValue)*StepValue);
	return ret;
}

CTwainArray::CTwainArray(int nNumOfItems, int nType/*= TWTY_UINT16*/)
{
	int nTypeSiz;
	switch (nType)
	{
	case TWTY_INT8:
		nTypeSiz = sizeof(TW_INT8);
		break;
	case TWTY_INT16:
		nTypeSiz = sizeof(TW_INT16);
		break;
	case TWTY_INT32:
		nTypeSiz = sizeof(TW_INT32);
		break;
	case TWTY_UINT8:
		nTypeSiz = sizeof(TW_UINT8);
		break;
	case TWTY_UINT16:
		nTypeSiz = sizeof(TW_UINT16);
		break;
	case TWTY_UINT32:
		nTypeSiz = sizeof(TW_UINT32);
		break;
	case TWTY_BOOL:
		nTypeSiz = sizeof(TWTY_BOOL);
		break;
	case TWTY_FIX32:
		nTypeSiz = sizeof(TWTY_FIX32);
		break;
	}
	m_hContainer = GlobalAlloc(GHND, (sizeof(TW_ARRAY) + nTypeSiz * nNumOfItems));
	pTW_ARRAY pval = (pTW_ARRAY)GlobalLock(m_hContainer);
	pval->NumItems = nNumOfItems;
	pval->ItemType = nType;
	GlobalUnlock(m_hContainer);
}

float CTwainArray::GetValue(pTW_ARRAY pval, int index)
{
	float ret;
	switch (pval->ItemType)
	{
	case TWTY_INT8:
		ret = ((TW_INT8 *)(pval->ItemList))[index];
		break;
	case TWTY_INT16:
		ret = ((TW_INT16 *)(pval->ItemList))[index];
		break;
	case TWTY_INT32:
		ret = (float)((TW_INT32 *)(pval->ItemList))[index];
		break;
	case TWTY_UINT8:
		ret = ((TW_UINT8 *)(pval->ItemList))[index];
		break;
	case TWTY_UINT16:
		ret = ((TW_UINT16 *)(pval->ItemList))[index];
		break;
	case TWTY_UINT32:
		ret = (float)((TW_UINT32 *)(pval->ItemList))[index];
		break;
	case TWTY_BOOL:
		ret = ((TW_BOOL *)(pval->ItemList))[index];
		break;
	case TWTY_FIX32:
		ret = FIX32ToFloat(((TW_FIX32 *)(pval->ItemList))[index]);
		break;
	}
	return ret;
}

void CTwainArray::SetValue(pTW_ARRAY pval, int index, float value)
{
	switch (pval->ItemType)
	{
	case TWTY_INT8:
		((TW_INT8 *)(pval->ItemList))[index] = (TW_INT8)value;
		break;
	case TWTY_INT16:
		((TW_INT16 *)(pval->ItemList))[index] = (TW_INT16)value;
		break;
	case TWTY_INT32:
		((TW_INT32 *)(pval->ItemList))[index] = (TW_INT32)value;
		break;
	case TWTY_UINT8:
		((TW_UINT8 *)(pval->ItemList))[index] = (TW_UINT8)value;
		break;
	case TWTY_UINT16:
		((TW_UINT16 *)(pval->ItemList))[index] = (TW_UINT16)value;
		break;
	case TWTY_UINT32:
		((TW_UINT32 *)(pval->ItemList))[index] = (TW_UINT32)value;
		break;
	case TWTY_BOOL:
		((TW_BOOL *)(pval->ItemList))[index] = (TW_BOOL)value;
		break;
	case TWTY_FIX32:
		((TW_FIX32 *)(pval->ItemList))[index] = FloatToFix32(value);
		break;
	}
}

int CTwainArray::GetNumOfItems()
{
	pTW_ARRAY pval = (pTW_ARRAY)GlobalLock(m_hContainer);
	int ret = pval->NumItems;
	GlobalUnlock(m_hContainer);
	return ret;
}

float CTwainArray::GetAt(int index)
{
	pTW_ARRAY pval = (pTW_ARRAY)GlobalLock(m_hContainer);
	float ret = GetValue(pval,index);
	GlobalUnlock(m_hContainer);
	return ret;
}

void CTwainArray::SetAt(int index, float value)
{
	pTW_ARRAY pval = (pTW_ARRAY)GlobalLock(m_hContainer);
	SetValue(pval,index,value);
	GlobalUnlock(m_hContainer);
}

CTwainCapability::CTwainCapability()
{
	m_pContainer = NULL;
	memset(&m_twCapability, 0, sizeof(m_twCapability));
}

CTwainCapability::~CTwainCapability()
{
	delete m_pContainer;
}

CTwainCapability::CTwainCapability(int Cap, int ConType /*= TWON_DONTCARE16*/,
	CTwainContainer *pContainer /*= NULL*/)
{
	Create(Cap,ConType,pContainer);
}

void CTwainCapability::Create(int Cap, int ConType /*= TWON_DONTCARE16*/,
	CTwainContainer *pContainer /*= NULL*/)
{
	ASSERT(ConType==TWON_DONTCARE16&&pContainer==NULL||
		ConType!=TWON_DONTCARE16&&pContainer!=NULL);
	m_pContainer = pContainer;
	m_twCapability.Cap = Cap;
	m_twCapability.ConType = ConType;
	m_twCapability.hContainer = pContainer ? pContainer->GetHandle() :
		NULL;
}

void CTwainCapability::CreateContainer()
{
	Check();
	switch (m_twCapability.ConType)
	{
	case TWON_ONEVALUE:
		m_pContainer = new CTwainOneValue(m_twCapability.hContainer);
		break;
	case TWON_ARRAY:
		m_pContainer = new CTwainArray(m_twCapability.hContainer);
		break;
	case TWON_ENUMERATION:
		m_pContainer = new CTwainEnumeration(m_twCapability.hContainer);
		break;
	case TWON_RANGE:
		m_pContainer = new CTwainRange(m_twCapability.hContainer);
		break;
	default:
		ASSERT(FALSE);
	};
}

BOOL CTwainCapability::IsEmpty()
{
	return m_twCapability.ConType == TWON_DONTCARE16;
}

void CTwainCapability::Check()
{
	if (GetConType() == TWON_DONTCARE16)
		throw new CTwainNotImplException("Return code 0, but "
			"capability is empty");
	if (IsEmpty())
		throw new CTwainNotImplException("Return code 0, but "
			"capability is empty");
}

