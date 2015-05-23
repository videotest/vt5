#include "stdafx.h"
#include "ScriptView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum {
	Language_Cpp,
	Language_Vb
};

static int gnCrystalEditLanguage = Language_Vb;

//	C++ keywords (MSVC5.0 + POET5.0)
static LPTSTR s_apszCppKeywordList[] =
{
	_T("__asm"),
	_T("enum"),
	_T("__multiple_inheritance"),
	_T("template"),
	_T("auto"),
	_T("__except"),
	_T("__single_inheritance"),
	_T("this"),
	_T("__based"),
	_T("explicit"),
	_T("__virtual_inheritance"),
	_T("thread"),
	_T("bool"),
	_T("extern"),
	_T("mutable"),
	_T("throw"),
	_T("break"),
	_T("false"),
	_T("naked"),
	_T("true"),
	_T("case"),
	_T("__fastcall"),
	_T("namespace"),
	_T("try"),
	_T("catch"),
	_T("__finally"),
	_T("new"),
	_T("__try"),
	_T("__cdecl"),
	_T("float"),
	_T("operator"),
	_T("typedef"),
	_T("char"),
	_T("for"),
	_T("private"),
	_T("typeid"),
	_T("class"),
	_T("friend"),
	_T("protected"),
	_T("typename"),
	_T("const"),
	_T("goto"),
	_T("public"),
	_T("union"),
	_T("const_cast"),
	_T("if"),
	_T("register"),
	_T("unsigned"),
	_T("continue"),
	_T("inline"),
	_T("reinterpret_cast"),
	_T("using"),
	_T("__declspec"),
	_T("__inline"),
	_T("return"),
	_T("uuid"),
	_T("default"),
	_T("int"),
	_T("short"),
	_T("__uuidof"),
	_T("delete"),
	_T("__int8"),
	_T("signed"),
	_T("virtual"),
	_T("dllexport"),
	_T("__int16"),
	_T("sizeof"),
	_T("void"),
	_T("dllimport"),
	_T("__int32"),
	_T("static"),
	_T("volatile"),
	_T("do"),
	_T("__int64"),
	_T("static_cast"),
	_T("wmain"),
	_T("double"),
	_T("__leave"),
	_T("__stdcall"),
	_T("while"),
	_T("dynamic_cast"),
	_T("long"),
	_T("struct"),
	_T("xalloc"),
	_T("else"),
	_T("main"),
	_T("switch"),
	_T("interface"),
	//	Added by a.s.
	_T("persistent"),
	_T("_persistent"),
	_T("transient"),
	_T("depend"),
	_T("ondemand"),
	_T("transient"),
	_T("cset"),
	_T("useindex"),
	_T("indexdef"),
	NULL
};

//	VBScript keywords
static LPTSTR s_apszVbKeywordList[] =
{
	_T("If"),
	_T("Function"),
	_T("Else"),
	_T("End"),
	_T("For"),
	_T("Next"),
	_T("While"),
	_T("Wend"),
	_T("SMDoMenu"),
	_T("GetAttrType"),
	_T("GetAttrName"),
	_T("GetAttrValString"),
	_T("GetAttrValFloat"),
	_T("GetAttrValInt"),
	_T("GetAttrValBool"),
	_T("GetAttrValEnumInt"),
	_T("GetAttrValEnumString"),
	_T("GetClassId"),
	_T("GetGeoType"),
	_T("SetAttrValString"),
	_T("SetAttrValInt"),
	_T("SetAttrValFloat"),
	_T("SetAttrValBool"),
	_T("SetAttrValEnumString"),
	_T("SetAttrValEnumInt"),
	_T("CreateVerifyItem"),
	_T("VerifyCardinalities"),
	_T("As"),
	_T("Abs"),
	_T("AppActivate"),
	_T("Asc"),
	_T("Atn"),
	_T("Beep"),
	_T("Call"),
	_T("CDbl"),
	_T("ChDir"),
	_T("ChDrive"),
	_T("CheckBox"),
	_T("Chr"),
	_T("CInt"),
	_T("CLng"),
	_T("Close"),
	_T("Const"),
	_T("Cos"),
	_T("CreateObject"),
	_T("CSng"),
	_T("CStr"),
	_T("CVar"),
	_T("CurDir"),
	_T("Date"),
	_T("Declare"),
	_T("Dialog"),
	_T("Dim"),
	_T("Dir"),
	_T("DlgEnable"),
	_T("DlgText"),
	_T("DlgVisible"),
	_T("Do"),
	_T("Double"),
	_T("Loop"),
	_T("End"),
	_T("EOF"),
	_T("Erase"),
	_T("Exit"),
	_T("Exp"),
	_T("FileCopy"),
	_T("FileLen"),
	_T("Fix"),
	_T("For"),
	_T("To"),
	_T("Step"),
	_T("Next"),
	_T("Format"),
	_T("Function"),
	_T("GetObject"),
	_T("Global"),
	_T("GoSub"),
	_T("Return"),
	_T("GoTo"),
	_T("Hex"),
	_T("Hour"),
	_T("If"),
	_T("Then"),
	_T("Else"),
	_T("Input"),
	_T("InputBox"),
	_T("InStr"),
	_T("Int"),
	_T("IsDate"),
	_T("IsEmpty"),
	_T("IsNull"),
	_T("IsNumeric"),
	_T("Kill"),
	_T("LBound"),
	_T("LCase"),
	_T("LCase$"),
	_T("Left"),
	_T("Left$"),
	_T("Len"),
	_T("Let"),
	_T("Line"),
	_T("Input#"),
	_T("Log"),
	_T("Long"),
	_T("Mid"),
	_T("Minute"),
	_T("MkDir"),
	_T("Month"),
	_T("MsgBox"),
	_T("Name"),
	_T("Now"),
	_T("Oct"),
	_T("On"),
	_T("Error"),
	_T("Open"),
	_T("Option"),
	_T("Base"),
	_T("Print"),
	_T("Print"),
	_T("Rem"),
	_T("Right"),
	_T("RmDir"),
	_T("Rnd"),
	_T("Second"),
	_T("Seek"),
	_T("Seek"),
	_T("Select Case"),
	_T("SendKeys"),
	_T("Set"),
	_T("Shell"),
	_T("Sin"),
	_T("Space"),
	_T("Sqr"),
	_T("Static"),
	_T("Stop"),
	_T("Str"),
	_T("StrComp"),
	_T("String"),
	_T("StringFunction"),
	_T("Sub"),
	_T("Tan"),
	_T("Text"),
	_T("TextBox"),
	_T("Time"),
	_T("TimeSerial"),
	_T("TimeValue"),
	_T("Trim"),
	_T("LTrim"),
	_T("RTrim"),
	_T("Type"),
	_T("UBound"),
	_T("UCase"),
	_T("Val"),
	_T("VarType"),
	_T("While"),
	_T("Wend"),
	_T("With"),
	_T("Write"),
	_T("Year"),
	NULL
};

static BOOL SetParserLanguage( int nLanguage)
{
	gnCrystalEditLanguage = Language_Vb;
}


static BOOL IsCppKeyword(LPCTSTR pszChars, int nLength)
{
	for (int L = 0; s_apszCppKeywordList[L] != NULL; L ++)
	{
		if (strncmp(s_apszCppKeywordList[L], pszChars, nLength) == 0
				&& s_apszCppKeywordList[L][nLength] == 0)
			return TRUE;
	}
	return FALSE;
}

static BOOL IsVbKeyword(LPTSTR pszChars, int nLength)
{
	for (int L = 0; s_apszVbKeywordList[L] != NULL; L ++)
	{
		if (strnicmp(s_apszVbKeywordList[L], pszChars, nLength) == 0
			&& s_apszVbKeywordList[L][nLength] == 0) {

#ifndef _NOAUTOCAPITALIZEKEYWORD
			if ( strncmp(s_apszVbKeywordList[L], pszChars, nLength)) {
				
				LPTSTR pszBuffer = pszChars;
				
				for ( int i=0; i<nLength; i++)
					*pszBuffer++ = s_apszVbKeywordList[L][i];
			
			}
#endif 
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL IsKeyword(LPTSTR pszChars, int nLength)
{
	switch ( gnCrystalEditLanguage)
	{
		case Language_Cpp:
			return IsCppKeyword( pszChars, nLength);

		case Language_Vb:
			return IsVbKeyword( pszChars, nLength);

		default:
			ASSERT( FALSE); 
	}

	return FALSE;
}


static BOOL IsCppNumber(LPCTSTR pszChars, int nLength)
{
	if (nLength > 2 && pszChars[0] == '0' && pszChars[1] == 'x')
	{
		for (int I = 2; I < nLength; I++)
		{
			if (isdigit_ru(pszChars[I]) || (pszChars[I] >= 'A' && pszChars[I] <= 'F') ||
										(pszChars[I] >= 'a' && pszChars[I] <= 'f'))
				continue;
			return FALSE;
		}
		return TRUE;
	}
	if (! isdigit_ru(pszChars[0]))
		return FALSE;
	for (int I = 1; I < nLength; I++)
	{
		if (! isdigit_ru(pszChars[I]) && pszChars[I] != '+' &&
			pszChars[I] != '-' && pszChars[I] != '.' && pszChars[I] != 'e' &&
			pszChars[I] != 'E')
			return FALSE;
	}
	return TRUE;
}

#define DEFINE_BLOCK(pos, colorindex)	\
	ASSERT((pos) >= 0 && (pos) <= nLength);\
	if (pBuf != NULL)\
	{\
		if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){\
		pBuf[nActualItems].m_nCharPos = (pos);\
		pBuf[nActualItems].m_nColorIndex = (colorindex);\
		nActualItems ++;}\
	}

#define COOKIE_COMMENT			0x0001
#define COOKIE_PREPROCESSOR		0x0002
#define COOKIE_EXT_COMMENT		0x0004
#define COOKIE_STRING			0x0008
#define COOKIE_CHAR				0x0010

DWORD CScriptView::ParseLine(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
	int nLength = GetLineLength(nLineIndex);
	if (nLength <= 0)
		return dwCookie & COOKIE_EXT_COMMENT;

	LPTSTR pszChars = (LPTSTR)GetLineChars(nLineIndex);
	BOOL bFirstChar = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
	BOOL bRedefineBlock = TRUE;

	BOOL bDecIndex  = FALSE;
	int nIdentBegin = -1;
	for (int I = 0; ; I++)
	{
		if (bRedefineBlock)
		{
			int nPos = I;
			if (bDecIndex)
				nPos--;
			if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
			{
				DEFINE_BLOCK(nPos, COLORINDEX_COMMENT);
			}
			else
			if (dwCookie & (COOKIE_CHAR | COOKIE_STRING))
			{
				DEFINE_BLOCK(nPos, COLORINDEX_STRING);
			}
			else
			if (dwCookie & COOKIE_PREPROCESSOR)
			{
				DEFINE_BLOCK(nPos, COLORINDEX_PREPROCESSOR);
			}
			else
			{
				DEFINE_BLOCK(nPos, COLORINDEX_NORMALTEXT);
			}
			bRedefineBlock = FALSE;
			bDecIndex      = FALSE;
		}

		if (I == nLength)
			break;

		if (dwCookie & COOKIE_COMMENT)
		{
			DEFINE_BLOCK(I, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_COMMENT;
			break;
		}

		//	String constant "...."
		if (dwCookie & COOKIE_STRING)
		{
			if (pszChars[I] == '"' && (I == 0 || pszChars[I - 1] != '\\'))
			{
				dwCookie &= ~COOKIE_STRING;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		//	Char constant '..'
		if (dwCookie & COOKIE_CHAR)
		{
			if (pszChars[I] == '\'' && (I == 0 || pszChars[I - 1] != '\\'))
			{
				dwCookie &= ~COOKIE_CHAR;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		//	Extended comment /*....*/
		if (dwCookie & COOKIE_EXT_COMMENT)
		{
			if (I > 0 && pszChars[I] == '/' && pszChars[I - 1] == '*')
			{
				dwCookie &= ~COOKIE_EXT_COMMENT;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		if (( gnCrystalEditLanguage == Language_Cpp && I > 0 && pszChars[I] == '/'&& pszChars[I - 1] == '/')
			|| (gnCrystalEditLanguage == Language_Cpp && (pszChars[I] == '\'' || (pszChars[I] == 'R' && pszChars[I +1] == 'e' && pszChars[I +2] == 'm'))))
		{
			DEFINE_BLOCK(I, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_COMMENT;
			break;
		}

		//	Preprocessor directive #....
		if (dwCookie & COOKIE_PREPROCESSOR)
		{
			if (I > 0 && pszChars[I] == '*' && pszChars[I - 1] == '/')
			{
				DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
				dwCookie |= COOKIE_EXT_COMMENT;
			}
			continue;
		}

		//	Normal text
		if (pszChars[I] == '"')
		{
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_STRING;
			continue;
		}
		if (pszChars[I] == '\'')
		{
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_CHAR;
			continue;
		}
		if (I > 0 && pszChars[I] == '*' && pszChars[I - 1] == '/')
		{
			DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_EXT_COMMENT;
			continue;
		}


		if (bFirstChar)
		{
			if (pszChars[I] == '#')
			{
				DEFINE_BLOCK(I, COLORINDEX_PREPROCESSOR);
				dwCookie |= COOKIE_PREPROCESSOR;
				continue;
			}
			if (!isspace_ru(pszChars[I]))
				bFirstChar = FALSE;
		}

		if (pBuf == NULL)
			continue;	//	We don't need to extract keywords,
						//	for faster parsing skip the rest of loop

		if (isalnum_ru(pszChars[I]) || pszChars[I] == '_' || pszChars[I] == '.')
		{
			if (nIdentBegin == -1)
				nIdentBegin = I;
		}
		else
		{
			if (nIdentBegin >= 0)
			{
				if (IsKeyword(pszChars + nIdentBegin, I - nIdentBegin))
				{
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
				}
				else
				if (IsCppNumber(pszChars + nIdentBegin, I - nIdentBegin))
				{
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
				}
				bRedefineBlock = TRUE;
				bDecIndex = TRUE;
				nIdentBegin = -1;
			}
		}
	}

	if (nIdentBegin >= 0)
	{
		if (IsKeyword(pszChars + nIdentBegin, I - nIdentBegin))
		{
			DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
		}
		else
		if (IsCppNumber(pszChars + nIdentBegin, I - nIdentBegin))
		{
			DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
		}
	}

	if (pszChars[nLength - 1] != '\\')
		dwCookie &= COOKIE_EXT_COMMENT;
	return dwCookie;
}
