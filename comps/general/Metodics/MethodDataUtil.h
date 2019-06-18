#pragma once
// ������� ��� ������������� � ���������:

enum LoadStateFlags
{
	lssSwitchViews = 1<<0, // ����������� �����?
	lssActivateObjects = 1<<1, // �������������� ������� � ���������?
	lssRestoreZoom = 1<<2, // ��������������� zoom � scroll pos ����.
	lssActiveViewOnly = 1<<3, // ������� ������ ��������� ��������� ����, �� �������� �������� ������.
};

// ���������� ��������� (���� �������� ����/���������) � NamedData
void StoreSplitter(const char *szSection, IUnknown* punkNamedData, bool bSimplified = false);
void LoadSplitter(const char *szSection, IUnknown* punkNamedData, DWORD dwFlags);

// �������� ���� (� named data) � ���������
// ������ ������ � ��������� ������, ������ ��������� CR �/��� LF
// true, ���� szName ��������� � ����� �� �������� ��� ���������
// ������ ������-�������
bool CompareWildcards(const char* szName, const char* szWild);

// ��������������� �-�
inline long cmp_len_CString( const CString psz1, const CString psz2 )
{ // ��� ���������� ����� �� \\Paths: ����� ������� � ������, ����� ����� - �� ��������
	long n = psz1.GetLength() - psz2.GetLength();
	if(n) return n;
	return psz2.CompareNoCase( psz1 );
}

typedef _list_map_t<CString, CString, cmp_len_CString> CSubstMap;


// ����������: ������������� ���, ����� pszDisabledKeys, �� ����� NamedData
// � ������; ��� ���� ��������� ��� ���� � ������������ � pSubstMap (���
// ������, ������������ � ������ pSubstMap �������� �� �������� pSubstMap).
bool RecursiveAddEntry( INamedData* ptrDst, INamedData* ptrSrc, LPCSTR pszEntry,
					   char* pszDisabledKeys, CSubstMap* pSubstMap );

// ������ shell.data, �� ����������� DisabledKeys � � ��������
// ���� ����� �� [Paths] �� �������
bool ImportShellData( IUnknown* punkSrc, char* pszDisabledKeys );

// ������ shell.data � ������ MergeKeys � � ��������
// ���� ����� �� [Paths] �� �������
bool ImportShellData2( IUnknown* punkSrc );
