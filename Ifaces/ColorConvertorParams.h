#ifndef __ColorConvertorParams_h__
#define __ColorConvertorParams_h__

enum ColorConvertorParamsKind
{
	params_none=0,
	params_CGH
};

struct CColorConvertorParams
{
	DWORD size;
	ColorConvertorParamsKind kind;
};

class CColorConvertorParamsCGH : public CColorConvertorParams
{
public:
	DWORD dwViewType;
	// 0 - DAPI, 1 - FITC, 2 - TRITC, 3 - rgb, 4 - ratio
};

#endif // __ColorConvertorParams_h__
