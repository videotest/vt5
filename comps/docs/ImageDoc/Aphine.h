#ifndef __APHINE_H__
#define __APHINE_H__

void Rotate(CPoint *Ret,CPoint *Cent, float Angle);
struct CAphineParams
{
	CPoint	ptCenter;	//Точка, относительно которой поизводится преобразование
	CPoint	ptCenterNew;//Ее положение на новом изображении
	double	fKx, fKy;	//Коэффициенты растяжения/сжатия по x и y
	double	fAngle;		//Угол поворота (0 - 2*PI)
	BOOL    bFillOutside;//Заполнять ли точки на новом изображении вне
	                     //региона.
public:
	CAphineParams()		//инициализация по умолчанию - ничего не делать
	{
		ptCenter = CPoint( 0, 0 );
		ptCenterNew = CPoint( 0, 0 );
		fKx = fKy = 1;
		fAngle = 0;
		bFillOutside = FALSE;
	}
						//Положение
	CRect CalcSize( CRect rc );
};

BOOL DoAphineTransform( CImageWrp &imgNew, CImageWrp &img, CAphineParams &params, void (*pfnCallBack)(int) = 0 );
BOOL DoAphineTransformFast( CImageWrp &imgNew, CImageWrp &img, CAphineParams &params, void (*pfnCallBack)(int) = 0 );

#endif //__APHINE_H__