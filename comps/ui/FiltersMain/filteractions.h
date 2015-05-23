#ifndef __filteractions_h__
#define __filteractions_h__

#include "imgfilterbase.h"

//[ag]1. classes

class CActionEqualContrast : public CFilterBase
{
	DECLARE_DYNCREATE(CActionEqualContrast)
	GUARD_DECLARE_OLECREATE(CActionEqualContrast)

public:
	CActionEqualContrast();
	virtual ~CActionEqualContrast();
public:
	virtual bool CanDeleteArgument( CFilterArgument *pa );
	virtual bool InvokeFilter();
};

class CActionEqualHists : public CFilterBase
{
	DECLARE_DYNCREATE(CActionEqualHists)
	GUARD_DECLARE_OLECREATE(CActionEqualHists)

public:
	CActionEqualHists();
	virtual ~CActionEqualHists();

public:
	virtual bool InvokeFilter();
};

class CActionUnsharpenMask : public CImageFilterBase
{
	DECLARE_DYNCREATE(CActionUnsharpenMask)
	GUARD_DECLARE_OLECREATE(CActionUnsharpenMask)

public:
	CActionUnsharpenMask();
	virtual ~CActionUnsharpenMask();

public:
	virtual bool InvokeFilter();
	virtual void DoDrawMaskShape( CDC *pdc, int xCenter, int yCenter, int nMaskSize );
};

class CActionExtractPhases : public CFilterBase
{
	DECLARE_DYNCREATE(CActionExtractPhases)
	GUARD_DECLARE_OLECREATE(CActionExtractPhases)

public:
	CActionExtractPhases();
	virtual ~CActionExtractPhases();

public:
	virtual bool InvokeFilter();

protected:
	color	**m_ppcolorscr;
	color	**m_ppcolordest;
	DWORD** m_ppHiIntervals;
	DWORD** m_ppLoIntervals;
	DWORD** m_pClassColors;
	long	m_nClasses;
};



class CActionKircsh : public CImageFilterBase
{
	DECLARE_DYNCREATE(CActionKircsh)
	GUARD_DECLARE_OLECREATE(CActionKircsh)

public:
	CActionKircsh();
	virtual ~CActionKircsh();

public:
	virtual bool InvokeFilter();
protected:
	virtual bool IsAvaible(){ return CImageFilterBase::IsAvaible();}
};

class CActionLocalEqualize : public CImageFilterBase
{
	DECLARE_DYNCREATE(CActionLocalEqualize)
	GUARD_DECLARE_OLECREATE(CActionLocalEqualize)

public:
	CActionLocalEqualize();
	virtual ~CActionLocalEqualize();

public:
	virtual bool InvokeFilter();
protected:
	virtual bool IsAvaible(){ return CImageFilterBase::IsAvaible();}
};

class CActionEqualize : public CImageFilterBase
{
	DECLARE_DYNCREATE(CActionEqualize)
	GUARD_DECLARE_OLECREATE(CActionEqualize)

public:
	CActionEqualize();
	virtual ~CActionEqualize();

public:
	virtual bool InvokeFilter();
	virtual void InitImageResult();
protected:
	CImageWrp	m_imageBigSource;
	int			m_cxBig, m_cyBig;
protected:
	virtual bool IsAvaible(){ return CImageFilterBase::IsAvaible();}
};

class CActionLaplas : public CImageFilterBase
{
	DECLARE_DYNCREATE(CActionLaplas)
	GUARD_DECLARE_OLECREATE(CActionLaplas)

public:
	CActionLaplas();
	virtual ~CActionLaplas();

public:
	virtual bool InvokeFilter();
protected:
	virtual bool IsAvaible(){ return CImageFilterBase::IsAvaible();}
};

class CActionAverage : public CImageFilterBase
{
	DECLARE_DYNCREATE(CActionAverage)
	GUARD_DECLARE_OLECREATE(CActionAverage)

public:
	CActionAverage();
	virtual ~CActionAverage();

public:
	virtual bool InvokeFilter();
protected:
	virtual bool IsAvaible(){ return CImageFilterBase::IsAvaible();}
};

class CActionMedian : public CImageFilterBase
{
	DECLARE_DYNCREATE(CActionMedian)
	GUARD_DECLARE_OLECREATE(CActionMedian)

public:
	CActionMedian();
	virtual ~CActionMedian();

public:
	virtual bool InvokeFilter();
protected:
	virtual bool IsAvaible(){ return CImageFilterBase::IsAvaible();}
};

class CActionSharping : public CImageFilterBase
{
	DECLARE_DYNCREATE(CActionSharping)
	GUARD_DECLARE_OLECREATE(CActionSharping)

public:
	CActionSharping();
	virtual ~CActionSharping();

public:
	virtual bool InvokeFilter();
protected:
	virtual bool IsAvaible(){ return CImageFilterBase::IsAvaible();}
};

class CActionBriContrast : public CFilterBase
{
	DECLARE_DYNCREATE(CActionBriContrast)
	GUARD_DECLARE_OLECREATE(CActionBriContrast)

public:
	CActionBriContrast();
	virtual ~CActionBriContrast();

public:
	virtual bool InvokeFilter();
};

class CActionEmboss : public CFilterBase
{
	DECLARE_DYNCREATE(CActionEmboss)
	GUARD_DECLARE_OLECREATE(CActionEmboss)

public:
	CActionEmboss();
	virtual ~CActionEmboss();
public:
	virtual bool InvokeFilter();
};

class CActionNegative : public CFilterBase
{
	DECLARE_DYNCREATE(CActionNegative)
	GUARD_DECLARE_OLECREATE(CActionNegative)

public:
	CActionNegative();
	virtual ~CActionNegative();
public:
	virtual bool InvokeFilter();
};


class CActionBinaryExtractPhases : public CFilterBase
{
	DECLARE_DYNCREATE(CActionBinaryExtractPhases)
	GUARD_DECLARE_OLECREATE(CActionBinaryExtractPhases)

public:
	CActionBinaryExtractPhases();	
	~CActionBinaryExtractPhases();	

public:	
	virtual void DoFinalizePreview();
	//virtual void DoInitializePreview();
	virtual bool InvokeFilter();	

	bool CreateImageMask();
	void DestroyImageMaskAndRects();
	void DestroyHelpersArray();
	
	bool ProcessImage( );
	IUnknown* GetSource();

protected:	
	//return true if argument should be deleted from NamedData
	virtual bool CanDeleteArgument( CFilterArgument *pa );	

protected:

	byte	*m_pbyteMark;
	CRect* m_prcVImage;

	color	**m_ppcolorscr;
	color	**m_ppcolordest;
	DWORD** m_ppHiIntervals;
	DWORD** m_ppLoIntervals;
	DWORD** m_pClassColors;
	long	m_nClasses;

};




#endif //__filteractions_h__
