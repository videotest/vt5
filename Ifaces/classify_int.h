#pragma once

#define IDCP_READONLY ( 1 << 0 )

#define ID_SECT_GENERAL "General"
#define ID_ENTR_CLASSIFY_NAME "ClassifyName"
#define ID_ENTR_CLASSIFY_PROGID "ClassifyProgID"

#define ID_SECT_CLASSES_INFO "Classes"
#define ID_ENTR_CLASS_COUNT   "ClassCount"

#define ID_SECT_KEY_INFO "KeysInfo"
#define ID_ENTR_KEYS_ENUM   "KeysEnum"

#define ID_SECT_KEY_DATA "KeyData:"

#define ID_SECT_CLASSIFIER_INFO "ClassifierInfo"

struct CLASSINFO
{
	char szName[255];
	COLORREF clColor;

	CLASSINFO()
	{
		clColor = 0;
	}
	CLASSINFO(  const CLASSINFO &info  )
	{
		*this = info;
	}

	void operator=( const CLASSINFO &info )
	{
		::strncpy( szName, info.szName, MAX_PATH );
		clColor = info.clColor;
	}
	bool operator==( const CLASSINFO &info )
	{
		if( ::strcmp( szName, info.szName ) != 0 )
			return false;

		if( clColor != info.clColor )
			return false;

		return true;
	}
};

interface IClassifyParams : public IUnknown
{
	com_call SetName( /*[in] */BSTR bstrName ) = 0;
	com_call GetName(/*[out] */BSTR *pbstrName ) = 0;

	com_call SetClasses( /*[in] */long *plClasses , /*[in]*/ long lSz ) = 0;
	com_call GetClasses( /*[out] */long **pplClasses , /*[out]*/ long *plSz ) = 0;
	com_call RemoveClasses() = 0;

	com_call SetClassParams( /*[in]*/ long lClass, /*[in]*/ CLASSINFO pfParams ) = 0;
	com_call GetClassParams( /*[in]*/ long lClass, /*[out]*/CLASSINFO *ppParams ) = 0;

	com_call SetKeys( /*[in] */long *plKeys , /*[in]*/ long lSz ) = 0;
	com_call GetKeys( /*[out] */long **pplKeys , /*[out]*/ long *plSz ) = 0;
	com_call RemoveKeys() = 0;

	com_call GetKeyParams( /*[in] */long lClass ,/*[in]*/ long lKey, /*[out]*/ double **ppfParams, /*[out]*/ long *plSz ) = 0;
	com_call SetKeyParams( /*[in] */long lClass ,/*[in]*/ long lKey, /*[in]*/ double *pfParams, /*[in]*/ long plSz ) = 0;
};

interface IClassifyProxy : public IUnknown
{
	com_call Load( /*[in]*/ BSTR bstrFile ) = 0;
	com_call Store( /*[in]*/ BSTR bstrFile ) = 0;
						 
	com_call Start() = 0;
	com_call Process( /*[in]*/ IUnknown *punkObject ) = 0;
	com_call Finish() = 0;
	com_call GetObjectClass( /*[in]*/ IUnknown *punkObject, /*out*/ long *plClass ) = 0;

	com_call GetFlags( /*[out]*/ DWORD *pdwFlag ) = 0;
	com_call SetFlags( /*[in]*/ DWORD dwFlag ) = 0;
};

interface IClassifyProxy2 : public IClassifyProxy
{
	com_call Teach() = 0;
};

interface IClassifyProxy3 : public IClassifyProxy2
{
	com_call Teach(IUnknown* punkObjectList) = 0;
};

// vanek - 31.10.2003
interface IClassifyInfoProxy : public IUnknown
{
	com_call PrepareObjectDescrByClass( /*[in]*/ IUnknown *punkObject, /*[in]*/ long lClass ) = 0;
	com_call GetObjectDescrByClass( /*[out]*/ BSTR *pbstrDescription ) = 0;
};


declare_interface( IClassifyParams, "C2868EE1-87DC-4826-BFD6-17F8F1CEF3ED" );
declare_interface( IClassifyProxy,  "843A0DA3-9100-4938-B8F9-2128CE975EE0" );
declare_interface( IClassifyProxy2,  "EA4CC334-803D-4d59-BEEB-821DFE892DED" );
declare_interface( IClassifyInfoProxy,  "28190888-B43F-4523-A81B-CE02ED2201AA" );
declare_interface( IClassifyProxy3,  "3A0D75B7-C795-46f7-9561-3E329F4D34C2" );