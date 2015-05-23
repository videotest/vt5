#include "stdafx.h"
#include "skeleton.h"

int skel1(byte *pbuf, byte *pbuf1, int cx, int cy, int dir)
{
	int nChanges=0;
	int dirs[]={ -cx, -cx+1, 1, cx+1, cx, cx-1, -1, -cx-1 };
	int n1=dirs[(dir+7)%8], n2=dirs[dir], n3=dirs[(dir+1)%8]; 
    for(int y=1; y<cy-1; y++)
	{
		byte *p = pbuf+y*cx+1, *p1 = pbuf1+y*cx+1;
	    for(int x=1; x<cx-1; x++, p++, p1++)
		{
			if(p[0] && p[n1] && p[n2] && p[n3] && !p[-n1] && !p[-n2] && !p[-n3])
			{
				*p1=0;
				nChanges++;
			}
		}
	}
	if(nChanges) memcpy(pbuf, pbuf1, cx*cy);
	return nChanges;
}

int skeleton(byte *pbuf, byte *pbuf1, int cx, int cy)
{   // один шаг скелетонизации, вернул 0 - ничего не сделали
	// buf - рабочие данные, buf1 - вспомогательные

	// Вообще говоря, далеко не оптимально, зато просто...

	if(cx<3 || cy<3) return 0; // защита

	int nChanges=0;

	nChanges += skel1(pbuf, pbuf1, cx, cy, 4);
	nChanges += skel1(pbuf, pbuf1, cx, cy, 0);
	nChanges += skel1(pbuf, pbuf1, cx, cy, 2);
	nChanges += skel1(pbuf, pbuf1, cx, cy, 6);
	nChanges += skel1(pbuf, pbuf1, cx, cy, 1);
	nChanges += skel1(pbuf, pbuf1, cx, cy, 3);
	nChanges += skel1(pbuf, pbuf1, cx, cy, 5);
	nChanges += skel1(pbuf, pbuf1, cx, cy, 7);

	return nChanges;
}