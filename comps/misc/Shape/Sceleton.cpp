#include "StdAfx.h"
#include "sceleton.h"
#include <queue>


static int log2(UINT m)
{
	int n =0;
	while(m) 
	{
		n++;
		m = m>>1;
	}
	return n;
}

CSceleton::CSceleton(void)
{
	m_c =0;
	m_imageInt =0;
}

CSceleton::~CSceleton(void)
{
	ContourDelete(m_c);
	delete []m_imageInt;
}

void CSceleton::AttachData(byte* image, CSize size)
{
	m_cx = size.cx+2;
	m_cy = size.cy+2;
	int n = m_cx*m_cy;
	m_image = new byte[n];
	ZeroMemory(m_image, n);

	for(int y=0;y<size.cy;y++)
	{
		memcpy(m_image + (y+1)*m_cx+1, image + y*size.cx, size.cx);
	}
	byte* temp;
	for(y=1;y<m_cy;y++)
	{	
		temp = m_image + m_cx*y +1;
		for(int x=1;x<m_cx;x++,temp++)
		{
			if(*temp) *temp = 1;
		}
	}

	process();

	for(int y=0;y<size.cy;y++)
	{
		memcpy(image + y*size.cx, m_image + (y+1)*m_cx+1, size.cx);
	}
	delete[] m_image;
}

static void offset_contour(Contour* cDest , const Contour* cSource, POINT offset)
{
	ZeroMemory(cDest, sizeof(Contour));
	for(int i=0;i<cSource->nContourSize; i++)
	{
		ContourAddPoint(cDest,cSource->ppoints[i].x - offset.x, cSource->ppoints[i].y - offset.y);
	}
}

int CSceleton::AttachDataSKIZ(Contour* c, byte* image, CSize size, POINT offset, int threshold)
{
	m_c = ContourCreate();
	offset_contour(m_c, c, offset);
	m_cx = size.cx+2;
	m_cy = size.cy+2;
	int n = m_cx*m_cy;
	delete []m_imageInt;
	m_imageInt = new int[n];
	ZeroMemory(m_imageInt, n*sizeof(int));
	int s =0;
	for(int y=1;y<size.cy+1;y++)
	{
		byte* row = image + size.cx*(y-1); 
		int* rowTest = m_imageInt + y*m_cx +1;
		for(int x=0;x<size.cx;x++, row++,rowTest++)
		{
			if(*row) 
			{
				*rowTest = -1;
				s++;
			}
		}		
	}
	/*
	threshold = s/m_c->nContourSize/3 + 1; /*log2sqrt((double)s)/20+1;*/
	processSKIZ();

	/*ZeroMemory(image, size.cx*size.cy);*/
	n=0;
	for(int y=1;y<m_cy-1;y++)
	{
		int* row = m_imageInt + y*m_cx +1;
		int* rowUp = m_imageInt + (y-1)*m_cx +1;
		int* rowDown = m_imageInt + (y+1)*m_cx +1;
		byte* rowOut = image + (y-1)*size.cx;

		for(int x=1;x<m_cx-1; x++, row++,rowUp++, rowDown++, rowOut++)
		{
			/*ASSERT(*row!=-1);*/
			if(*row ==0 || *rowOut==0) continue;
			int nMax =0;
			int k =0;
			if( *(rowUp-1) && (k =(*row - *(rowUp-1)))>nMax) nMax = k;
			if( *(rowUp) && (k =(*row - *(rowUp)))>nMax) nMax = k;
			if( *(rowUp+1) && (k =(*row - *(rowUp+1)))>nMax) nMax = k;
			if( *(rowDown-1) && (k =(*row - *(rowDown-1)))>nMax) nMax = k;
			if( *(rowDown) && (k =(*row - *(rowDown)))>nMax) nMax = k;
			if( *(rowDown+1) && (k =(*row - *(rowDown+1)))>nMax) nMax = k;
			if( *(row-1) && (k =(*row - *(row-1)))>nMax) nMax = k;
			if( *(row+1) && (k =(*row - *(row+1)))>nMax) nMax = k;
			if(nMax > m_c->nContourSize/2) nMax = m_c->nContourSize-k;
            *rowOut = nMax>threshold?255:0;
			if(*rowOut) n++;
		}		
	}
	return n;
}

void CSceleton::process()
{
	while(1)
	{
		step1();
		if(clean()) break;
		step2();
		if(clean()) break;
	}
}

void CSceleton::processSKIZ()
{
	ContourPoint* pts = m_c->ppoints;
	queue<int*> prp;
	for(int i=0; i<m_c->nContourSize; i++, pts++)//labeling contour points
	{
		int* p = m_imageInt + (pts->y+1)*m_cx + pts->x + 1;
		*p = i+1;
		prp.push(p);
	}

	while(!prp.empty())
	{
		int *p = prp.front();
		prp.pop();

		/*pp = p - m_cx -1;
		if(*pp==-1)
		{
			prp.push(pp);
			*pp = *p;
		}*/

		int *pp = p - m_cx;
		if(*pp==-1)
		{
			prp.push(pp);
			*pp = *p;
		}

		/*pp = p - m_cx + 1;
		if(*pp==-1)
		{
			prp.push(pp);
			*pp = *p;
		}*/

		pp = p - 1;
		if(*pp==-1)
		{
			prp.push(pp);
			*pp = *p;
		}

		pp = p + 1;
		if(*pp==-1)
		{
			prp.push(pp);
			*pp = *p;
		}

		/*pp = p + m_cx - 1;
		if(*pp==-1)
		{
			prp.push(pp);
			*pp = *p;
		}

		pp = p + m_cx + 1;
		if(*pp==-1)
		{
			prp.push(pp);
			*pp = *p;
		}*/

		pp = p + m_cx;
		if(*pp==-1)
		{
			prp.push(pp);
			*pp = *p;
		}
	}

}

bool CSceleton::_t01(char *row, char *rowUp, char *rowDown)
{

	byte T =0;
	if(*(rowUp+1)-*rowUp == 1) T++;
	if(*(row+1)==1 && !*(rowUp+1) == 1) T++;
	if(*(rowDown+1)- *(row+1) == 1) T++;
	if(*(rowDown)- *(rowDown+1) == 1) T++;
	if(*(rowDown-1)-*(rowDown) == 1) T++;
	if(*(row-1)- *(rowDown-1) == 1) T++;
	if(*(rowUp-1)- *(row-1) == 1) T++;
	if(*(rowUp)-*(rowUp-1) == 1) T++;
	return T == 1;
}

void CSceleton::step1()
{
	byte *row,*rowUp,*rowDown;
	for(int y =1;y<m_cy-1;y++)
	{
		row = m_image + m_cx*y+1;
		rowUp = m_image + m_cx*(y-1) +1;
		rowDown =  m_image + m_cx*(y+1) +1;
		for(int x =1; x<m_cx-1; x++, row++, rowUp++, rowDown++)
		{

			if(!*row) continue;
			byte s = *rowUp + *(rowUp+1)+ *(row+1) + *(rowDown+1) +  *(rowDown) + *(rowDown-1) + *(row-1) + *(rowUp-1);
			if(s>=2 && s<=6)
			{
				if(_t01((char*)row, (char*)rowUp, (char*)rowDown))
				{
					if(*rowUp * *(row+1) * *rowDown ==0)
					{
						if(*(row+1) * *rowDown * *(row-1) ==0)
						{
							m_vDel.push(row);
						}
					}
				}
			}
		}
	}
}

void CSceleton::step2()
{
	byte *row,*rowUp,*rowDown;
	for(int y =1;y<m_cy-1;y++)
	{
		row = m_image + m_cx*y;
		rowUp = m_image + m_cx*(y-1) +1;
		rowDown =  m_image + m_cx*(y+1) +1;
		for(int x =1; x<m_cx-1; x++, row++, rowUp++, rowDown++)
		{

			if(!*row) continue;
			byte s = *rowUp + *(rowUp+1)+ *(row+1) + *(rowDown+1) +  *(rowDown) + *(rowDown-1) + *(row-1) + *(rowUp-1);
			if(s>=2 && s<=6)
			{
				if(_t01((char*)row, (char*)rowUp, (char*)rowDown))
				{
					if(*rowUp * *(row+1) * *(row-1) ==0)
					{
						if(*(rowUp) * *rowDown * *(row-1) ==0)
						{
							m_vDel.push(row);
						}
					}
				}
			}
		}
	}
}

bool CSceleton::clean()
{
	if(m_vDel.empty()) return true;
	while(!m_vDel.empty())
	{
		byte* b = m_vDel.top();
		*b =0;
		m_vDel.pop();
	}
	return false;
}




CSceletonPoint::~CSceletonPoint()
{

}

