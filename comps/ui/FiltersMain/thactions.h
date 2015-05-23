#ifndef __thactions_h__
#define __thactions_h__

#include "imgfilterbase.h"

//[ag]1. classes

class CActionMeasureBin : public CFilterBase
{
	DECLARE_DYNCREATE(CActionMeasureBin)
	GUARD_DECLARE_OLECREATE(CActionMeasureBin)

public:
	CActionMeasureBin();
	virtual ~CActionMeasureBin();

public:
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa ){return false;};
};

class CActionMeasurePhases : public CFilterBase
{
	DECLARE_DYNCREATE(CActionMeasurePhases)
	GUARD_DECLARE_OLECREATE(CActionMeasurePhases)

public:
	CActionMeasurePhases();
	virtual ~CActionMeasurePhases();

public:
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa ){return false;};
};

class CActionMeasurePhasesLight : public CFilterBase
{
	DECLARE_DYNCREATE(CActionMeasurePhasesLight)
	GUARD_DECLARE_OLECREATE(CActionMeasurePhasesLight)

public:
	CActionMeasurePhasesLight();
	virtual ~CActionMeasurePhasesLight();

public:
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa ){return false;};
};


class CActionThresholdingGrayBin : public CFilterBase
{
	DECLARE_DYNCREATE(CActionThresholdingGrayBin)
	GUARD_DECLARE_OLECREATE(CActionThresholdingGrayBin)

public:
	CActionThresholdingGrayBin();
	virtual ~CActionThresholdingGrayBin();

public:
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa ){return false;};

};


class CActionExtractPhase : public CThresholdingActionBase
{
	DECLARE_DYNCREATE(CActionExtractPhase)
	GUARD_DECLARE_OLECREATE(CActionExtractPhase)
public:
	CActionExtractPhase();
	~CActionExtractPhase();

	virtual bool MakeBinaryImage();
	virtual COLORREF GetFillColor();
protected:
	int		m_nClass;
	color	**m_ppcolors;
	DWORD*	m_pdwLoRanges;
	DWORD*	m_pdwHiRanges;
};


class CActionAddRemoveColorInterval : public CInteractiveActionBase
{
	ENABLE_MULTYINTERFACE();
	DECLARE_DYNCREATE(CActionAddRemoveColorInterval)
	GUARD_DECLARE_OLECREATE(CActionAddRemoveColorInterval)

	ENABLE_SETTINGS();
public:
	CActionAddRemoveColorInterval();
	virtual ~CActionAddRemoveColorInterval();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();

	virtual bool Initialize();
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual void DoDraw( CDC &dc );

	void GetColorsInfo(long* pnCount, DWORD** ppdwColors);
protected:
	Contour	*m_pcontour;
	bool	m_bTracking;
	DWORD*	m_pColors;
	long	m_nCount;


	BEGIN_INTERFACE_PART(Colors, IProvideColors)
		com_call GetColorsInfo(long* pnCount, DWORD** ppdwColors);
	END_INTERFACE_PART(Colors)

	DECLARE_INTERFACE_MAP();
};

class CActionAddRemoveColor : public CInteractiveActionBase
{
	ENABLE_MULTYINTERFACE();

	DECLARE_DYNCREATE(CActionAddRemoveColor)
	GUARD_DECLARE_OLECREATE(CActionAddRemoveColor)
	ENABLE_SETTINGS();
public:
	CActionAddRemoveColor();
	virtual ~CActionAddRemoveColor();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
	virtual bool ExecuteSettings( CWnd *pwndParent );

	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	
	virtual bool Initialize();


	BEGIN_INTERFACE_PART(Colors, IProvideColors)
		com_call GetColorsInfo(long* pnCount, DWORD** ppdwColors);
	END_INTERFACE_PART(Colors)

	DECLARE_INTERFACE_MAP();

	void GetColorsInfo(long* pnCount, DWORD** ppdwColors);

private:
	long	m_nX;
	long	m_nY;
	bool	m_bInit;
	DWORD*	m_pColors;
	long	m_nCount;
};


class CActionThresholdingGray : public CThresholdingActionBase
{
	DECLARE_DYNCREATE(CActionThresholdingGray)
	GUARD_DECLARE_OLECREATE(CActionThresholdingGray)
public:
	CActionThresholdingGray();
	~CActionThresholdingGray();
	virtual bool MakeBinaryImage();
protected:
	byte	*m_pbuff;
	color	**m_ppcolors;
};

////////////////////////-----------------------------------
//CActionThresholdingGrayBinAuto

namespace gray_histo
{
typedef struct _h_Peak
{
	UINT pos;//position in histogramm
	ULONG value;//value of peak
}
HPEAK;

typedef CArray<HPEAK> CHistoArray ;
//class working with gray images only
class CHistogramm
{
#ifdef _DEBUG
BOOL bCreated;
#endif
protected:
	CArray<ULONG> histo; 
	/*CHistoArray data;*/
	//CHistoArray maximums;
	//CHistoArray limits;
	UINT size;
	int Algorithm;
	ULONG pixCount;//quantity of meaning (without masked ones) pixels 
	ULONG dataCount;//here it is 256
	UINT currentLimit;
	int nFrom,nTo;
	BOOL useLightObjects;
	float fUseFract;	//area for cutting histogramm edges

	
public:
	CHistogramm(){
#ifdef _DEBUG
		bCreated = FALSE;
#endif
	};
	~CHistogramm(){};
	//Create must been called before any other method
	bool Create(TCHAR* tcsSectName, BOOL);
	//UINT GetNextLimit();
	//fill histogramm with image data row by row
	void AddData(byte* imageRow,//original image row
				byte* rowMask,	//mask for it
				UINT cx);		//row size
	//reduct meanless data
	void Equalize();
	//bool FindMaxMins();

	void SmoothData(/*CArray<ULONG>& dst, */int nMask);	
	void CalculatePixCount();
	int IterativeThreshold();
	int OtsuThreshold();
	int Enthropy();
	int CalculateLocalPixCount(int begin,int end);
	int  CallAlgorithm();
private:
	double mean(int beg, int end, double freq);
	double freq(int beg, int end);
	double variance(int beg, int end, double mean);

};
}

class CActionThresholdingGrayBinAuto: public CFilterBase
{
	DECLARE_DYNCREATE(CActionThresholdingGrayBinAuto)
	GUARD_DECLARE_OLECREATE(CActionThresholdingGrayBinAuto)

public:
	CActionThresholdingGrayBinAuto();
	virtual ~CActionThresholdingGrayBinAuto();

public:
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa ){return false;};


};
///////////////////////////////////CActionThresGrayNoImage --end

///CActionThresGrayMute////////////////////////////////////////////
class CActionThresGrayMute: public CActionBase
{
	DECLARE_DYNCREATE(CActionThresGrayMute)
	GUARD_DECLARE_OLECREATE(CActionThresGrayMute)

public:
	CActionThresGrayMute(){};
	virtual ~CActionThresGrayMute(){};

public:
	virtual bool Invoke();

};
///CActionThresGrayMute --end////////////////////////////////////////////

#endif //__thactions_h__
