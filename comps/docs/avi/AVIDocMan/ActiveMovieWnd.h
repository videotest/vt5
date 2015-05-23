#if !defined(__ActiveMovieWnd_H__)
#define __ActiveMovieWnd_H__

#include "ActiveMovie3.h"

class CActiveMovieWnd : public CActiveMovie3
{
public:
	unsigned AMTimeToFrame(double dTime);
	double AMMidFrameToTime(unsigned nFrame);
	double AMBegFrameToTime(unsigned nFrame);
	unsigned AMGetPos();
	double AMGetTime();
	unsigned AMGetLength();
	void Capture(IUnknown *punkImage);
};


#endif