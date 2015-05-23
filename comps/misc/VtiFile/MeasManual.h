#ifndef __MeasManual_H__
#define __MeasManual_H__

#include "ObjMease.h"

#define MARKER_SIZE	5	//Размер указателя
#define ARC_SIZE	15	//Размер дуги при измерении угла

class CObjHandyBase;	//Базовый класс для объектов измерения

class CObjHandyBase: public CObjMeasure
{
	DECLARE_DYNAMIC(CObjHandyBase);
protected:
	CScrollView	*m_pView;	//Временный указатель на View
	double	m_fZoom;		//Увеличение
public:
	CObjHandyBase();	//Конструктор

	
public:
	virtual void OnBeginMeasure(CScrollView *, double iZoom);	//Начать измерения
	virtual void OnEndMeasure();					//Завершить измерения
	void Redraw();				//Перерисовать

	virtual double	GetVal() = 0;
public:
	virtual void OnLButtonDown(CPoint);			//Обработчики сообщений
	virtual void OnLButtonUp(CPoint);
	virtual void OnMouseMove(CPoint);
	virtual void OnLButtonDblClk(CPoint pt){};
	virtual BOOL OnSetCursor(CPoint pt) {return FALSE;};
	virtual void DoPaintInt(CDC &theDC, int iType );
	virtual void DoPaint( CDC &theDC, double iZoom, int iType, BOOL bActive );
	virtual BOOL GetObjType(){ return OBJ_HANDY; };
	virtual CRect CalcInvalidateRect() {return GetRect();}
	virtual BOOL OnBeginMove(CPoint pt, CPoint ptBmp, int MoveMode,
		double fZoom, CView *pView);
	virtual void OnMove(CPoint ptStart, CPoint ptEnd, double fZoom);
	virtual BOOL OnEndMove(CPoint pt, CPoint ptBmpStart, double fZoom,
		CRect &rcInv1, CRect &rcInv2, CView *pView);
public:
	virtual BOOL IsReady(){return FALSE;};	//Готовы - ли данные
protected:
	static void DrawMarker(CDC &theDC, CPoint pt);//Нарисовать маркер
	virtual HRESULT __stdcall IsManual( BOOL *pbIs )
	{ *pbIs = true;return S_OK;}
};

///////////////////////////////////////////////////////////////////////////////
//Описание класса CObjLength - объект измерения длинны
class CObjFreeLength : public CObjHandyBase
{
	DECLARE_SERIAL(CObjFreeLength);
protected:
	CContour	m_cntr;
	BOOL	m_bReady;
	double	m_fStoredLen;
public:
	CObjFreeLength();	//Конструктор

	virtual void Serialize( CArchive &ar );
public:
	virtual void OnLButtonDown(CPoint);			//Обработчики сообщений
	virtual void OnMouseMove(CPoint);
	virtual void DoPaintInt(CDC &theDC, int iType );
	virtual void Attach( CObjNative & );
	virtual void AttachData( CObjNative & );
public:
	virtual void DoMeasure();
	virtual BOOL IsReady(){return m_bReady;}//Готовность данных
	virtual double	GetVal();
	virtual void OnLButtonDblClk(CPoint pt);
	virtual BOOL IsPointInObject( CPoint pt, BOOL bActive = FALSE );

	virtual void OnEndMeasure();					//Завершить измерения
//	virtual BOOL IsPointInObject( CPoint pt, BOOL bActive = FALSE );
};


///////////////////////////////////////////////////////////////////////////////
//Описание класса CObjLength - объект измерения длинны
class CObjLength : public CObjHandyBase
{
	DECLARE_SERIAL(CObjLength);
protected:
	CPoint	m_ptStart, m_ptEnd;
	int		m_iPointDefined;
public:
	CObjLength();	//Конструктор

	virtual void Serialize( CArchive &ar );
public:
	virtual void OnLButtonDown(CPoint);			//Обработчики сообщений
	virtual void OnMouseMove(CPoint);
	virtual void DoPaintInt(CDC &theDC, int iType );
public:
	virtual void DoMeasure();
	virtual BOOL IsReady(){return m_iPointDefined == 2;}//Готовность данных
	virtual double	GetVal();

	virtual void OnEndMeasure();					//Завершить измерения
	virtual void Attach( CObjNative & );
	virtual void AttachData( CObjNative & );
	virtual BOOL IsPointInObject( CPoint pt, BOOL bActive = FALSE );

	double GetDX();
	double GetDY();
};

class CObjXYRatio: public CObjLength
{
	DECLARE_SERIAL(CObjXYRatio);
public:
	virtual void DoPaintInt(CDC &theDC, int iType );
	virtual void OnMouseMove(CPoint);
	virtual double GetVal();
	virtual void DoMeasure();
	virtual void Attach( CObjNative & );
	virtual void AttachData( CObjNative & );
};


///////////////////////////////////////////////////////////////////////////////
//Описание класса CObjAngle - объект измерения угла
class CObjAngle : public CObjHandyBase
{
	DECLARE_SERIAL(CObjAngle);
protected:
	CPoint	m_pt1, m_pt2, m_pt3;
	int		m_iPointDefined;
public:
	CObjAngle();	//Конструктор

	virtual void Serialize( CArchive &ar );
public:
	virtual void OnLButtonDown(CPoint);			//Обработчики сообщений
	virtual void OnMouseMove(CPoint);
	virtual void DoPaintInt(CDC &theDC, int iType );
	virtual void DoMeasure();
public:
	virtual BOOL IsReady(){return m_iPointDefined == 3;}//Готовность данных
	virtual void Attach( CObjNative & );
	virtual void AttachData( CObjNative & );
	virtual BOOL IsPointInObject( CPoint pt, BOOL bActive = FALSE );
protected:
	double Calc(double &dA1, double &dA2);	//a1, a2- углы с горизонталью
	virtual double	GetVal();
};


///////////////////////////////////////////////////////////////////////////////
//Описание класса CObjRadius - объект измерения радиуса
class CObjRadius : public CObjAngle
{
	DECLARE_SERIAL(CObjRadius);
public:
	virtual void DoPaintInt(CDC &theDC, int iType );
	virtual void DoMeasure();
	virtual BOOL IsPointInObject( CPoint pt, BOOL bActive = FALSE );
protected:
	BOOL Calc(CPoint &ptCenter, double &r);	
	virtual double	GetVal();
	virtual void Attach( CObjNative & );
	virtual void AttachData( CObjNative & );
};



#endif //__MeasManual_H__