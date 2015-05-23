// CellRange.cpp : implementation file
//
// MFC Grid Control - Grid cell base class

#include "stdafx.h"
#include "CellRange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


bool Divide(CCellRange & range, CCellRange & inner, CRangeArray& arr);

CSelection::CSelection()
{	
	m_list.RemoveAll();	
	m_nLeft = m_nTop = m_nRight = m_nBottom = -1;
}

CSelection::~CSelection()
{
	m_list.RemoveAll();
	m_nLeft = m_nTop = m_nRight = m_nBottom = -1;
}


CSelection& CSelection::operator = (const CSelection & sel)
{	
	m_nTop = sel.m_nTop;
	m_nLeft = sel.m_nLeft;
	m_nRight = sel.m_nRight;
	m_nBottom = sel.m_nBottom;

	m_list.RemoveAll();
	for (POSITION pos  = sel.m_list.GetHeadPosition(); pos != NULL; )
	{
		CCellRange range = sel.m_list.GetNext(pos);
		m_list.AddTail(range);
	}

	return *this;
}


CSelection CSelection::operator+(const CSelection & sel)  const
{
	CSelection s(*this);
	s += sel;
	return s;
}

CSelection CSelection::operator+(CCellRange & range)  const
{
	CSelection s(*this);
	s += range;
	return s;
}

CSelection CSelection::operator+(const CCellID & cell)  const
{
	CSelection s(*this);
	CCellRange range(cell.row, cell.col, cell.row, cell.col);
	s += range;
	return s;
}

CSelection CSelection::operator-(const CSelection & sel)  const
{
	CSelection s(*this);
	s -= sel;
	return s;
}

CSelection CSelection::operator-(CCellRange & range)  const
{
	CSelection s(*this);
	s -= range;
	return s;
}

CSelection CSelection::operator-(const CCellID & cell)  const
{
	CSelection s(*this);
	CCellRange range(cell.row, cell.col, cell.row, cell.col);
	s -= range;
	return s;
}


void CSelection::operator+=(const CSelection & sel)
{
	bool bRet = true;
	for (POSITION pos = sel.m_list.GetHeadPosition(); pos != NULL; )
	{
		CCellRange range = sel.m_list.GetNext(pos);
		bRet = AddRange(range) && bRet;
	}
	VERIFY(bRet == true);
}

void CSelection::operator+=(CCellRange & range)
{
	bool bRet = AddRange(range);
	VERIFY(bRet == true);
}

void CSelection::operator+=(const CCellID & cell)
{
	CCellRange range(cell.row, cell.col, cell.row, cell.col);
	bool bRet = AddRange(range);
	VERIFY(bRet == true);
}

void CSelection::operator-=(const CSelection & sel)
{
	bool bRet = true;
	for (POSITION pos = sel.m_list.GetHeadPosition(); pos != NULL; )
	{
		CCellRange range = sel.m_list.GetNext(pos);
		bRet = RemoveRange(range) && bRet;
	}
	VERIFY(bRet == true);
}

void CSelection::operator-=(CCellRange & range)
{
	bool bRet = RemoveRange(range);
	VERIFY(bRet == true);
}

void CSelection::operator-=(const CCellID & cell)
{
	CCellRange range(cell.row, cell.col, cell.row, cell.col);
	bool bRet = RemoveRange(range);
	VERIFY(bRet == true);
}


void CSelection::ChangeEdges(const CCellRange & range)
{
	if (!range.IsValid())
		return;

	m_nLeft = (m_nLeft < range.Left() && m_nLeft >= 0) ? m_nLeft : range.Left();
	m_nTop = (m_nTop < range.Top() && m_nTop >= 0) ? m_nTop : range.Top();
	m_nRight = (m_nRight > range.Right()) ? m_nRight : range.Right();
	m_nBottom = (m_nBottom > range.Bottom()) ? m_nBottom : range.Bottom();
}

bool CSelection::AddRange(CCellRange & newrange)
{
	if (!newrange.IsValid())
		return true;

	// 1 list is empty -> simply add
	if (!m_list.GetCount())
	{
		m_list.AddTail(newrange);
		// update edges for first range
		m_nLeft = newrange.Left();
		m_nTop = newrange.Top();
		m_nRight = newrange.Right();
		m_nBottom = newrange.Bottom();
		return true;
	}

	// 2 new range is outside -> simply add
	if (m_nRight < newrange.Left() || m_nLeft > newrange.Right() ||
		m_nTop > newrange.Bottom() || m_nBottom < newrange.Top())
	{
		m_list.AddTail(newrange);
		// and update edges if need
		ChangeEdges(newrange);
		Sort();
		return true;
	}

	for (POSITION pos = m_list.GetHeadPosition(); pos != NULL; )
	{
		POSITION posRange = pos;
		// get next source range
		CCellRange srcrange = m_list.GetNext(pos);
		// if it's invalid ->remove it
		if (!srcrange.IsValid())
		{
			m_list.RemoveAt(posRange);
			continue;
		}
		// get intersect range
		CCellRange inter = srcrange.Intersect(newrange);
		// if it's invalid
		if (!inter.IsValid())
			continue;

	// now intersect_range is valid 
		// 3 and new_range is inside in one of already selected ranges
		if (inter == newrange)
		{
			// update edges if need
			ChangeEdges(newrange);
			// -> do nothing 
			return true;
		}

		// 4. if intersect == source range -> 
		// we need exclude this source range from list
		if (inter == srcrange)
		{
			m_list.RemoveAt(posRange);
			continue;
		}

		// 5. partial intersect
		if (srcrange.Count() < newrange.Count())
		{
			// source_range area is smaller than newrange area
			// we need replace source_range to set of ranges exclude intersect 

			// create array of ranges that is not include intersect range
			CRangeArray arr;
			Divide(srcrange, inter, arr);
			
			// and insert this array before replaced source range
			for (int nCnt = 0; nCnt < arr.GetSize(); nCnt++)
			{
				m_list.InsertBefore(posRange, arr[nCnt]);
				// update limits
				ChangeEdges(arr[nCnt]);
			}
			arr.RemoveAll();
			// remove source_range
			m_list.RemoveAt(posRange);
		}
		else
		{
			// source_range area greater than newrange area
			// we need divide new range to set of ranges exclude intersect 
			CRangeArray arr;
			Divide(newrange, inter, arr);

			// and add them to Selection as usually and return 
			bool bRet = true;
			for (int nCnt = 0; nCnt < arr.GetSize(); nCnt++)
				bRet = AddRange(arr[nCnt]) && bRet;

			arr.RemoveAll();

			return bRet;
		}
	} // for all ranges in selection

	// if we are here ->we need add new_range to list
	m_list.AddTail(newrange);
	ChangeEdges(newrange);
	Sort();

	return true;
}

bool CSelection::RemoveRange(CCellRange & remove)
{
	if (!remove.IsValid())
		return true;

	m_nLeft = m_nTop = m_nRight = m_nBottom = -1;

	for (POSITION pos = m_list.GetHeadPosition(); pos != NULL; )
	{
		POSITION posRange = pos;
		// get next source range
		CCellRange range = m_list.GetNext(pos);
		// if it's invalid ->remove it
		if (!range.IsValid())
		{
			m_list.RemoveAt(posRange);
			continue;
		}
		// get intersect range
		CCellRange inter = remove.Intersect(range);
		// if it's invalid
		if (!inter.IsValid())
		{
			// update limits
			ChangeEdges(range);
			continue;
		}

	// now intersect is valid

		// if intersect != source range 
		// we need replace it with set of ranges excludes intersect range
		if (inter != range)
		{
			// create array of ranges that is not include intersect range
			CRangeArray arr;
			Divide(range, inter, arr);
			
			// and insert this array before replaced source range
			for (int nCnt = 0; nCnt < arr.GetSize(); nCnt++)
			{
				m_list.InsertBefore(posRange, arr[nCnt]);
				// update limits
				ChangeEdges(arr[nCnt]);
			}
		}
		// else intersect == source range
		// we shall simply remove source range from list

		// remove intersected range
		m_list.RemoveAt(posRange);
	} // for all source ranges
	Sort();
	return true;
}

CSelection CSelection::Intersect(CCellRange & range)
{
	CSelection sel;

	for (POSITION pos = m_list.GetHeadPosition(); pos != NULL; )
	{
		CCellRange inner = range.Intersect(m_list.GetNext(pos));
		if (inner.IsValid())
			sel.AddRange(inner);
	}
	return sel;
}


CSelection CSelection::Intersect(CSelection & sel)
{
	CSelection ret;
	ret.SetEmpty();
	
	if (sel.IsEmpty())
		return ret;

	if (IsEmpty())
		return ret;

	for (POSITION pos = sel.GetRanges().GetHeadPosition(); pos != NULL; )
	{
		CSelection s = Intersect(sel.GetRanges().GetNext(pos));
		if (!s.IsEmpty())
		{
			for (POSITION p = s.GetRanges().GetHeadPosition(); p != NULL; )
				ret.AddRange(s.GetRanges().GetNext(p));
		}
	}
	return ret;
}



bool CSelection::InRange(const CCellID & cellID) const
{
	if (m_nRight < cellID.col || m_nLeft > cellID.col ||
		m_nTop > cellID.row || m_nBottom < cellID.row)
		return false;

	for (POSITION pos = m_list.GetHeadPosition(); pos != NULL; )
	{
		if (m_list.GetNext(pos).InRange(cellID))
			return true;
	}
	return false;
}

bool CSelection::InRange(const CCellRange & range) const
{
	if (m_nRight < range.Left() || m_nLeft > range.Right() ||
		m_nTop > range.Bottom() || m_nBottom < range.Top())
		return false;

	for (POSITION pos = m_list.GetHeadPosition(); pos != NULL; )
	{
		if (m_list.GetNext(pos).Intersect(range).IsValid())
			return true;
	}
	return false;
}

void CSelection::Sort()
{
	if (m_list.GetCount() < 2)
		return;

	// from list to array
	CRangeArray arr;
	for (POSITION pos = m_list.GetHeadPosition(); pos != NULL; )
	{
		CCellRange range = m_list.GetNext(pos);
		if (!range.IsValid())
			continue;
		arr.Add(range);
	}
	// sort array by byble method ;-)

	int nPos = 0;
	for (int i = 0; i < arr.GetSize() - 1; i++)
	{
		nPos = i;
		for (int j = i; j < arr.GetSize(); j++)
		{
			// find  top-left
			if ((arr[nPos].Top() > arr[j].Top()) ||
				(arr[nPos].Top() == arr[j].Top() && arr[nPos].Left() > arr[j].Left()))
			{
				nPos = j;
			}
		}
		if (nPos != i)
		{
			CCellRange temp = arr[i];
			arr[i] = arr[nPos]; 
			arr[nPos] = temp;
		}
	}

	// from array to list
	m_list.RemoveAll();
	m_nLeft = m_nTop = m_nRight = m_nBottom = -1;

	for (i = 0; i < arr.GetSize(); i++)
	{
		CCellRange range = arr[i];
		if (!range.IsValid())
			continue;

		ChangeEdges(range);
		m_list.AddTail(range);
	}
	arr.RemoveAll();
}

bool Divide(CCellRange & range, CCellRange & inner, CRangeArray& arr)
{
	arr.RemoveAll();
	if (!inner.IsValid() || !range.IsValid())
		return false;

	// 1
	CCellRange tl(range.Top(), range.Left(), inner.Top() - 1, range.Right());
	if (tl.IsValid())
		arr.Add(tl);
	// 2
	CCellRange ml(inner.Top(), range.Left(), inner.Bottom(), inner.Left() - 1);
	if (ml.IsValid())
		arr.Add(ml);
	// 3
//	arr.Add(inner);
	// 4
	CCellRange mr(inner.Top(), inner.Right() + 1, inner.Bottom(), range.Right());
	if (mr.IsValid())
		arr.Add(mr);
	// 5
	CCellRange br(inner.Bottom() + 1, range.Left(), range.Bottom(), range.Right());
	if (br.IsValid())
		arr.Add(br);

	return true;
}


