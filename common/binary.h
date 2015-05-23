#ifndef __BINARY_H__
#define __BINARY_H__

#include "utils.h"
#include "cmnbase.h"
#include "binimageint.h"

class std_dll	CBinImageWrp : public IBinaryImagePtr
{
public:
	CBinImageWrp( IUnknown *punk  = 0, bool bAddRef = true );

public:
	void Attach( IUnknown *punk, bool bAddRef = true );
	IUnknown *operator =(IUnknown *);


	bool CreateNew( int cx, int cy );	//create a new image
	int GetWidth();			//image width
	int GetHeight();		//image height
	void Free();
};

#endif //__BINARY_H__