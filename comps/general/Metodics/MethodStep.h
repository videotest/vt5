#pragma once
#include <comutil.h>
#include "method_int.h"
#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_map.h"
#include "PropBagImpl.h"
#define SIMPLE_TYPES_DEFINED
#include "\vt5\awin\misc_list.h"

//#include "MethodDoer.h"
class CMethodDoer; // ��������������� ���������� ������ - ����������� � CMethod::m_xMethodChanges

bool TestMethod(); // ������������ �������� ������. ������� � ����������, ��� �����

struct tagMethodStepNotification
{
	long lHint;
	long lPos;
};

enum MethodStepFlags
{
	msfHasPropPage	= 1<<0,	// � ������ (�������� ������������� ��� ������)
	msfShowPropPage	= 1<<1, // ���������� �����
	msfChecked		= 1<<2, // ��� ��������?
	msfUndead		= 1<<3, // ������ ������� ���� ���
	msfStateless	= 1<<4, // � ���� ��� ������ ��������� � ���� - ������ ��� � ����������� (��� Message Step)
	msfInteractive	= 1<<5, // ��� �������������
	msfWriting		= 1<<6, // ��� ��������� � �������� ������ (�� ��� ����������). ����� ��� ������ ����� ��������.
};

bool RecursiveAddEntry2( INamedData* ptrDst, INamedData* ptrSrc );
bool RecursiveUpdateEntry2( INamedData* ptrDst, INamedData* ptrSrc );
// ��������� �������� ���� ������ � ptrDst

class CMethodStep
{
public:
	_bstr_t				m_bstrActionName; // ��� ����� (�� ���� ����� ��������)
	_bstr_t				m_bstrUserName; // ���������������� ��� ����� (��������������)
	_bstr_t				m_bstrForcedName; // ���, ������������� ���������� �� �������. ���� �� "" - �� ��������� �� ���� �� �������� � �������� � m_bstrUserName (������ ��������� ����� m_bstrActionName);

	INamedDataPtr		m_ptrNamedData; // ��������� NamedData - ������ ��, ��� �������������� �� ������ ����
	DWORD				m_dwFlags;

	bool				m_bSkipData;   // ���� true - �� ���������� �����. ��������
	// ������������ ������ ��� �����������

	CMethodStep(); // ����������� �� ���������
	CMethodStep(CMethodStep& x); // ����������� �����������
	CMethodStep& operator= (CMethodStep& x); // ���������� ������������
	void CopyFrom(CMethodStep& x); // ����������� - � ������ ������ bHasParams, bHasData
	void StoreToNamedData(IUnknown* punkND, const char *pszSection);
	void LoadFromNamedData(IUnknown* punkND, const char *pszSection);
	bool ApplyToShellData(); // �������� ��� ��������� ���� � shell.data
	_bstr_t GetScript(bool bWithSetValue=false);
};
