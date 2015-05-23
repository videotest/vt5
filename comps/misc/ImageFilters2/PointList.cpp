// PointList.cpp: implementation of the CPointList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PointList.h"
#include "image5.h"


#define MaxColor color(-1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPointList::CPointList()
{
  first=last=next=prev=NULL;
}

CPointList::~CPointList()
{
  Free();
}

void CPointList::Free()
{
  if(first) delete[] first;
  if(last) delete[] last;
  if(next) delete[] next;
  if(prev) delete[] prev;
}

void CPointList::CreateNew(int cx, int cy)
{
	Free();
	first = new int[MaxColor+1L];
	last  = new int[MaxColor+1L];
	next  = new int[cx*cy];
	prev  = new int[cx*cy];
  for (int c=0; c<=MaxColor; c++)
  {
    first[c]=-1; //no link
    last[c] =-1;  //no link
  }
}

/*
void CPointList::AddPoint(int color, int index)
{
  if(first[color]<0) //список пуст
  {
    first[color]=last[color]=index;
    next[index]=prev[index]=-1;
  }
  else
  {
    next[last[color]]=index;
    prev[index]=last[color];
    last[color]=index;
    next[index]=-1;
  }
}
*/

/*
void CPointList::DelPoint(int color, int index)
{
  int pr=prev[index];
  if(pr<0) first[color]=next[index];
  else     next[pr]=next[index];
  int nx=next[index];
  if(nx<0) last[color]=prev[index];
  else prev[nx]=prev[index];
}
*/