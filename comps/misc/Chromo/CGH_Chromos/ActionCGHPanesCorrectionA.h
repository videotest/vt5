// ActionCGHPanesCorrectionA.h: interface for the CActionPanesCorrectionA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIONCGHPANESCORRECTIONA_H__6DE1A4B9_8314_4672_BADF_D0F8DA6BF642__INCLUDED_)
#define AFX_ACTIONCGHPANESCORRECTIONA_H__6DE1A4B9_8314_4672_BADF_D0F8DA6BF642__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OperationCGHBase.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
const int nEqual = 5;
class CActionPanesCorrectionBase : public COperationBase
{
	template <class Type> class TPtrKiller 
	{
		Type *m_ptr;
	public:
		TPtrKiller( Type *ptr ) { m_ptr = ptr; }
	   ~TPtrKiller() { delete []m_ptr; }
	};

	template <class Type> class TPtrKiller2
	{
		Type **m_ptr;
		int m_nSz;
	public:
	   TPtrKiller2( Type **ptr, int nSz )
	   {
		   m_ptr = ptr;
		   m_nSz = nSz;
	   }
	   ~TPtrKiller2()
		{
		   for( int i = 0; i < m_nSz; i++ )
			   delete []m_ptr[i];
		   delete []m_ptr;
		}
	};
public:
	CActionPanesCorrectionBase() {}
	virtual ~CActionPanesCorrectionBase() {}
	bool IsAvaible() { return true; }
	bool Initialize() {	COperationBase::Initialize(); return true; }
};
///////////////////////////////////////////////////////////////////////////////////////////////////
class CActionPanesCorrectionA : public CActionPanesCorrectionBase, public CLongOperationImpl  
{
	DECLARE_DYNCREATE( CActionPanesCorrectionA )
	GUARD_DECLARE_OLECREATE( CActionPanesCorrectionA )
public:
	CActionPanesCorrectionA();
	virtual ~CActionPanesCorrectionA();

	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();

public:
	virtual void Finalize();
	bool Initialize();
	bool Invoke();
protected:
	void _combine();
	void _get_offset( int nPaneID,int **pXDensity, int **pYDensity );
	void _get_density( int **pXDensity, int **pYDensity, long **pSrcImage );
};
///////////////////////////////////////////////////////////////////////////////////////////////////
class CActionPanesCorrectionM : public CActionPanesCorrectionBase
{
	DECLARE_DYNCREATE( CActionPanesCorrectionM )
	GUARD_DECLARE_OLECREATE( CActionPanesCorrectionM )

	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();

	BOOL m_bDrag;
	HCURSOR m_hcurCatch, m_hcurFree, m_hcurOld;
	CPoint m_ptStart;
	int m_nActivePane;

	int m_xOffsets[32];
	int m_yOffsets[32];
public:
	CActionPanesCorrectionM();
	virtual ~CActionPanesCorrectionM();
public:
	virtual void Finalize();
	bool Initialize();
	bool Invoke();

	bool DoMouseMove(CPoint point);
	bool DoLButtonUp( CPoint pt );
	bool DoLButtonDown( CPoint pt );
	void DoDraw( CDC &dc );
	bool DoVirtKey( UINT nVirtKey );

	BEGIN_INTERFACE_PART( ScriptNotify, IScriptNotifyListner )
		com_call ScriptNotify( BSTR bstrNotifyDesc, VARIANT varValue );
	END_INTERFACE_PART( ScriptNotify )

protected:
	void _invalidate();
	void _get_img_rect( CRect &rc );
	HRESULT _fire_ch_event(  CString strNotify );
};
///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_ACTIONCGHPANESCORRECTIONA_H__6DE1A4B9_8314_4672_BADF_D0F8DA6BF642__INCLUDED_)
