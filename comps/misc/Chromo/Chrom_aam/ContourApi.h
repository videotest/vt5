#if !defined(AFX_ContourApi_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_ContourApi_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#include "stdafx.h"
#include "misc_utils.h"

void ContourSetPoint( Contour *pcntr, int nNumberPoint, int x, int y);
void ContourAddPoint( Contour *pcntr, int x, int y, bool bAddSamePoints = true);

#endif // !defined(AFX_ContourApi_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
