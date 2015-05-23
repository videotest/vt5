///////////////////////////////////////////////////////////////////////
// CellRange.h: header file
//
// MFC Grid Control - interface for the CCellRange class.
//
// Written by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2000. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.10+
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CELLRANGE_H__F86EF761_725A_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_CELLRANGE_H__F86EF761_725A_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// The code contained in this file is based on the original
// WorldCom Grid control written by Joe Willcoxson,
//      mailto:chinajoe@aol.com
//      http://users.aol.com/chinajoe
#include "ControlDef.h"

control_export_class CCellID
{    
// Attributes
public:
    int row, col;

// Operations
public:
    CCellID(int nRow = -1, int nCol = -1) : row(nRow), col(nCol) {}

    int IsValid() const { return (row >= 0 && col >= 0); }
    int operator==(const CCellID& rhs)    { return (row == rhs.row && col == rhs.col); }
    int operator!=(const CCellID& rhs)    { return !operator==(rhs); }
};

control_export_class CCellRange
{ 
public:
    
    CCellRange(int nMinRow = -1, int nMinCol = -1, int nMaxRow = -1, int nMaxCol = -1)
    {
        Set(nMinRow, nMinCol, nMaxRow, nMaxCol);
    }

    void Set(int nMinRow = -1, int nMinCol = -1, int nMaxRow = -1, int nMaxCol = -1);
    
    int  IsValid() const;
    int  InRange(int row, int col) const;
    int  InRange(const CCellID& cellID) const;
    int  Count() { return (m_nMaxRow - m_nMinRow + 1) * (m_nMaxCol - m_nMinCol + 1); }
    
    CCellID  GetTopLeft() const;
    CCellID  GetBottomRight() const;
    CCellRange  Intersect(const CCellRange& rhs) const;
    
    int  GetMinRow() const {return m_nMinRow;}
    int  Top() const {return m_nMinRow;}
    void SetMinRow(int minRow) {m_nMinRow = minRow;}
    void Top(int minRow){m_nMinRow = minRow;}
    
    int  GetMinCol() const {return m_nMinCol;}
    int  Left() const {return m_nMinCol;}
    void SetMinCol(int minCol) {m_nMinCol = minCol;}
    void Left(int minCol) {m_nMinCol = minCol;}
    
    int  GetMaxRow() const {return m_nMaxRow;}
    int  Bottom() const {return m_nMaxRow;}
    void SetMaxRow(int maxRow) {m_nMaxRow = maxRow;}
    void Bottom(int maxRow) {m_nMaxRow = maxRow;}
    
    int  GetMaxCol() const {return m_nMaxCol;}
    int  Right() const {return m_nMaxCol;}
    void SetMaxCol(int maxCol) {m_nMaxCol = maxCol;}
    void Right(int maxCol) {m_nMaxCol = maxCol;}

    int  GetRowSpan() const {return m_nMaxRow - m_nMinRow + 1;}
    int  GetColSpan() const {return m_nMaxCol - m_nMinCol + 1;}
    
    int  operator==(const CCellRange& rhs);
    int  operator!=(const CCellRange& rhs);
    
protected:
    int m_nMinRow;
    int m_nMinCol;
    int m_nMaxRow;
    int m_nMaxCol;
};

inline void CCellRange::Set(int minRow, int minCol, int maxRow, int maxCol)
{
     m_nMinRow = minRow;
     m_nMinCol = minCol;
     m_nMaxRow = maxRow;
     m_nMaxCol = maxCol;
}

inline int CCellRange::operator==(const CCellRange& rhs)
{
     return ((m_nMinRow == rhs.m_nMinRow) && (m_nMinCol == rhs.m_nMinCol) &&
             (m_nMaxRow == rhs.m_nMaxRow) && (m_nMaxCol == rhs.m_nMaxCol));
}

inline int CCellRange::operator!=(const CCellRange& rhs)
{
     return !operator==(rhs);
}

inline int CCellRange::IsValid() const
{
     return (m_nMinRow >= 0 && m_nMinCol >= 0 && m_nMaxRow >= 0 && m_nMaxCol >= 0 &&
             m_nMinRow <= m_nMaxRow && m_nMinCol <= m_nMaxCol);
}

inline int CCellRange::InRange(int row, int col) const
{
     return (row >= m_nMinRow && row <= m_nMaxRow && col >= m_nMinCol && col <= m_nMaxCol);
}

inline int CCellRange::InRange(const CCellID& cellID) const
{
     return InRange(cellID.row, cellID.col);
}

inline CCellID CCellRange::GetTopLeft() const
{
     return CCellID(m_nMinRow, m_nMinCol);
}

inline CCellID CCellRange::GetBottomRight() const
{
     return CCellID(m_nMaxRow, m_nMaxCol);
}

inline CCellRange CCellRange::Intersect(const CCellRange& rhs) const
{
     return CCellRange(max(m_nMinRow,rhs.m_nMinRow), max(m_nMinCol,rhs.m_nMinCol),
                       min(m_nMaxRow,rhs.m_nMaxRow), min(m_nMaxCol,rhs.m_nMaxCol));
}


#include <AfxTempl.h>

typedef CArray <CCellRange, CCellRange&> CRangeArray;
typedef CList <CCellRange, CCellRange&>	CRangeList;

control_export_class CSelection
{
public:
	CSelection();
	CSelection(const CSelection & sel)
	{	*this = sel;	}
	virtual ~CSelection();

	CSelection& operator =(const CSelection & sel);

	CSelection operator+(const CSelection & sel)  const;
	CSelection operator+(CCellRange & range)  const;
	CSelection operator+(const CCellID & cell)  const;
	CSelection operator-(const CSelection & sel)  const;
	CSelection operator-(CCellRange & range)  const;
	CSelection operator-(const CCellID & cell)  const;
	void operator+=(const CSelection & sel);
	void operator+=(CCellRange & range);
	void operator+=(const CCellID & cell);
	void operator-=(const CSelection & sel);
	void operator-=(CCellRange & range);
	void operator-=(const CCellID & cell);

	bool AddRange(CCellRange & range);
	bool RemoveRange(CCellRange & range);
	void Sort();

	CSelection Intersect(CCellRange & range);
	CSelection Intersect(CSelection & sel);
	void ChangeEdges(const CCellRange & range);

	CRangeList& GetRanges()
	{	return m_list;	}
	bool IsEmpty()
	{	return m_list.GetCount() == 0;	}
	void SetEmpty()
	{	m_list.RemoveAll();	m_nLeft = m_nTop = m_nRight = m_nBottom = -1;	}

	bool InRange(const CCellID & cellID) const;
	bool InRange(const CCellRange & range) const;

	int GetMinRow() const {return m_nTop;}
	int Top() const {return m_nTop;}
	
	int GetMinCol() const {return m_nLeft;}
	int Left() const {return m_nLeft;}
	
	int GetMaxRow() const {return m_nBottom;}
	int Bottom() const {return m_nBottom;}
	
	int GetMaxCol() const {return m_nRight;}
	int Right() const {return m_nRight;}

protected:
	CRangeList	m_list;
	int			m_nLeft, m_nTop, m_nRight, m_nBottom;

};


#endif // !defined(AFX_CELLRANGE_H__F86EF761_725A_11D1_ABBA_00A0243D1382__INCLUDED_)
