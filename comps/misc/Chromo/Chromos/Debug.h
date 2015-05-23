#if !defined(__Debug_H__)
#define __Debug_H__

#include "Chromosome.h"
#include "woolz.h"

void dprintf(char * szFormat, ...);
void Dump_Axis(LPCTSTR pszFirst, IChromoAxis2 *pAxis);
void Dump_Interval_Image(LPCTSTR pszFirst, INTERVALIMAGE *pIntImg);








#endif
