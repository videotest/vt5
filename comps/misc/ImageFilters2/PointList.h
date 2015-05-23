// PointList.h: interface for the CPointList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTLIST_H__9B7AD5E4_2B59_11D5_8F84_008048FD42FE__INCLUDED_)
#define AFX_POINTLIST_H__9B7AD5E4_2B59_11D5_8F84_008048FD42FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPointList
{
public:
	__forceinline void DelPoint(int color, int index)
  {
    int pr=prev[index];
    if(pr<0) first[color]=next[index];
    else     next[pr]=next[index];
    int nx=next[index];
    if(nx<0) last[color]=prev[index];
    else prev[nx]=prev[index];
  }
	__forceinline void AddPoint(int color, int index)
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
	void CreateNew( int cx, int cy );
	void Free();
	int * first;
	int * last;
	int * next;
	int * prev;
	CPointList();
	virtual ~CPointList();

};

#endif // !defined(AFX_POINTLIST_H__9B7AD5E4_2B59_11D5_8F84_008048FD42FE__INCLUDED_)
