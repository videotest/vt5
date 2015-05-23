#if !defined(__ChromoSubs_H__)
#define __ChromoSubs_H__

#include "woolz.h"

int _cindexa(INTERVALIMAGE *pinvls, int y);
int _cindexm(CImageWrp &pImgWrp, INTERVALIMAGE *pinvls, int y);
int _cindexl(POINT *ppt, int npts, int y);
double _cvdd(int *h, int l);
double _nssd(int *h, int l);
void _wdd(int *hdom, int npoints, short *v);
int *gradprof(int *hv, int npoints, int *pnpoints1);


#endif
