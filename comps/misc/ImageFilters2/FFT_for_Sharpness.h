#pragma once

#include "\vt5\comps\misc\fft\cwlib\cwp.h"
#pragma comment( lib, "cwlib.lib" )

#include <math.h>
#include "general.h"
#include "Sharpness.h"
 
class CFFT_for_Sharpness
{
public:
	CFFT_for_Sharpness(void);
	~CFFT_for_Sharpness(void);
	void FFT_T(complex *Data, ulong Len, int Dir);
	void FFT2(CImg &img, long H, long W, int Dir);
};
