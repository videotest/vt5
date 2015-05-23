#if !defined(__MorphDivEx_H__)
#define __MorphDivEx_H__

class CActionMorphDivideEx : public CFilterBase
{
	DECLARE_DYNCREATE(CActionMorphDivideEx)
	GUARD_DECLARE_OLECREATE(CActionMorphDivideEx)

	void DoDivide(byte *pur, byte *pre, int cx, int cy, int iOffset, BOOL bModifiedAlg, BOOL bFast);
public:
	CActionMorphDivideEx();
	virtual ~CActionMorphDivideEx();

public:
	virtual bool InvokeFilter();
	CObjectListUndoRecord m_changes;
};

class CActionUltimateErosion : public CFilterBase
{
	DECLARE_DYNCREATE(CActionUltimateErosion)
	GUARD_DECLARE_OLECREATE(CActionUltimateErosion)

	void DoDivide(byte *pur, byte *pre, int cx, int cy, int iOffset, BOOL bModifiedAlg, BOOL bFast);
public:
	CActionUltimateErosion();
	virtual ~CActionUltimateErosion();

public:
	virtual bool InvokeFilter();
};





#endif
