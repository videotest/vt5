#ifndef __MANUAL_OBJECTS_H
#define __MANUAL_OBJECTS_H


#include "Classes5.h"
#include "Measure5.h"
#include "ClassBase.h"
#include "DataBase.h"
#include "ObListWrp.h"
#include "Factory.h"
#include "calibrint.h"


struct tMeasPoint
{
	double fX;
	double fY;
};

typedef struct tMeasPoint TMeasPoint;

struct tExchangeMeasData
{
	DWORD		dwObjectType; // type of manual meas object
	long		lNumPoints;
	TMeasPoint* pPoints;
};

typedef struct tExchangeMeasData TExchangeMeasData;

typedef CArray <TMeasPoint, TMeasPoint& > CMeasPointArray;

class CManualMeasObject :	public  CDataObjectBase,
							public  CDrawObjectImpl

{
	DECLARE_DYNCREATE(CManualMeasObject);
	GUARD_DECLARE_OLECREATE(CManualMeasObject);
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();
	PROVIDE_GUID_INFO();

public:
	CManualMeasObject();
	~CManualMeasObject();

	BEGIN_INTERFACE_PART(ManualObj, IManualMeasureObject2)
		com_call GetParamInfo(long * plParamKey, DWORD * pdwType);
		com_call SetParamInfo(long * plParamKey, DWORD * pdwType);
		com_call GetObjectType(DWORD * pdwType);
		com_call SetObjectType(DWORD dwType);

		com_call SetValue(double fVal);
		com_call GetValue(double * pfVal);
		com_call CalcValue(double * pfVal);

		com_call ExchangeData( unsigned long * pdwData, long bRead );
		com_call UpdateParent();

		//paul : next 2 func. should be killed as posible
		com_call  StartStopMeasure ( long bStart );
		com_call  IsProcessMeasure ( long * pbRun );

		com_call SetOffset(POINT pt);
		com_call CrossPoint( struct tagPOINT pt, long * pbReturn );

		com_call GetParametersCount(long *plParamsCount);
		com_call SetParametersCount(long lParamsCount);
		com_call GetParamKey(long lParam, long *plParamKey);
		com_call SetParamKey(long lParam, long lParamKey);
		com_call SetValue2(long lParam, double fVal);
		com_call GetValue2(long lParam, double *pfVal);
		com_call CalcAllValues();
	END_INTERFACE_PART(ManualObj);

	BEGIN_INTERFACE_PART(Clone, IClonableObject)
		com_call Clone(IUnknown** ppunkCloned);
	END_INTERFACE_PART(Clone);

	BEGIN_INTERFACE_PART(Rect, IRectObject)
		com_call SetRect(RECT rc);
		com_call GetRect(RECT* prc);
		com_call HitTest( POINT	point, long *plHitTest );
		com_call Move( POINT point );
	END_INTERFACE_PART(Rect)

	BEGIN_INTERFACE_PART(Notify, INotifyObject2)
		com_call NotifyCreate();
		com_call NotifyDestroy();
		com_call NotifyActivate( bool bActivate );
		com_call NotifySelect( bool bSelect );
		com_call NotifyDataListChanged(int nCode, IUnknown *punkHint);
	END_INTERFACE_PART(Notify)

	BEGIN_INTERFACE_PART(Calibr, ICalibr)
		com_call GetCalibration( double *pfPixelPerMeter, GUID *pguidC );
		com_call SetCalibration( double fPixelPerMeter, GUID * pguidC );
	END_INTERFACE_PART(Calibr)



	virtual IUnknown* Clone();

//overrided virtuals
	virtual void OnSetParent( IUnknown *punkNewParent );
	virtual DWORD GetNamedObjectFlags();
	virtual bool  SerializeObject( CStreamEx &ar, SerializeParams *pparams );

	virtual bool  GetValue(double & rfVal);
	virtual bool  SetValue(double fVal);
	virtual bool  CalcValue(double & rfVal);
	virtual bool  CalcSetValues();
	virtual bool  ExchangeData( unsigned long * pdwData, long bRead );

	virtual bool GetData(TExchangeMeasData * pData);
	virtual bool SetData(TExchangeMeasData * pData);
	virtual void SerializeData(CStreamEx & ar);

	virtual bool CrossZone(CPoint & pt);


// draw impl
	virtual void  DoDraw(CDC &dc);
	virtual CRect DoGetRect();

protected:
	bool CalcAngle2Line(double & rfVal);
	bool CalcAngle3Point(double & rfVal);

	bool CalcLength(double & rfVal);
	bool CalcCount(double & rfVal);
	bool CalcSpline(double & rfVal);
	bool CalcRadius(double & rfVal);

	bool CalcRadius2Point(double & rfVal);
	bool CalcRadius3Point(double & rfVal);
	bool CalcCycleRect(CRect & rect, CPoint & ptc);

	void DrawLine(CDC & dc);
	void DrawBrokenLine(CDC & dc);
	void DrawFreeLine(CDC & dc);
	void DrawSpline(CDC & dc);
	void DrawCycle3(CDC & dc);
	void DrawCycle2(CDC & dc);
	void DrawAngle2Line(CDC & dc);
	void DrawAngle3Point(CDC & dc);
	void DrawCount(CDC & dc);

	void DrawCross(CDC & dc, const CPoint & pt);

	double  CalcAngles(double & rfA1, double & rfA2);
	bool	CalcDrawRectLine(CRect & rc);
	bool	CalcDrawRectSpline(CRect & rc);
	bool	CalcDrawRectCycle3(CRect & rc);
	bool	CalcDrawRectCycle2(CRect & rc);
	bool	CalcDrawRectAngle3Point(CRect & rc);

	CRect	CalcTextRect();

	bool SetOffset(CPoint pt); // offset data during drag'n'drop
	void UpdateParent();

	CFont	*create_font(	bool bZoom, 
							long lfontsize = 15 //  in logical units - [vanek] BT2000: 3560
						); 
	CString	get_number();

	void CalcParallelLinesMatrix(IUnknown *punkNewParent, IUnknown *punkExclude);
	void ResetParallelLinesKeys(IUnknown *punkNewParent);
protected:
	CPoint			m_ptOffset;
	long			m_lParamKey;
	DWORD			m_dwParamType;
	DWORD			m_dwObjType;
	double			m_fValue;
	CArray<long,long&> m_arrKeys;
	CArray<double,double&> m_arrValues;

	CMeasPointArray m_arrPoints;

	double	m_fCalibr;
	double	m_fXOffset;
	double	m_fYOffset;

	int	m_nCrossOffset;
	int	m_nArcSize;
	int	m_nSensZoneLen;
	bool	m_bCalculated;

	CPoint	m_pt1, m_pt2;	//for object title
	CString	m_str;
	GuidKey	m_guidC;
public:
	static const char *c_szType;
};

int		TransformValue(double fVal, double fCalibr, double fOffset = 0.);
double	TransformValue(int nVal, double fCalibr, double fOffset = 0.);

#endif// __MANUAL_OBJECTS_H
