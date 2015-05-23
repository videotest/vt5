#include "StdAfx.h"
#include "Math.h"
#include "Aphine.h"



CRect CAphineParams::CalcSize( CRect rc )
{
	CRect	rect;

//расчет координат четырех точек
	double	fx1, fx2, fx3, fx4;
	double	fy1, fy2, fy3, fy4;

	fx1 = ( rc.left-ptCenter.x )*::cos( fAngle )*fKx+( rc.top-ptCenter.y )*::sin( fAngle )*fKy+ptCenterNew.x;
	fy1 = -( rc.left-ptCenter.x )*::sin( fAngle )*fKx+( rc.top-ptCenter.y )*::cos( fAngle )*fKy+ptCenterNew.y;
	fx2 = ( rc.right-ptCenter.x )*::cos( fAngle )*fKx+( rc.top-ptCenter.y )*::sin( fAngle )*fKy+ptCenterNew.x;
	fy2 = -( rc.right-ptCenter.x )*::sin( fAngle )*fKx+( rc.top-ptCenter.y )*::cos( fAngle )*fKy+ptCenterNew.y;
	fx3 = ( rc.left-ptCenter.x )*::cos( fAngle )*fKx+( rc.bottom-ptCenter.y )*::sin( fAngle )*fKy+ptCenterNew.x;
	fy3 = -( rc.left-ptCenter.x )*::sin( fAngle )*fKx+( rc.bottom-ptCenter.y )*::cos( fAngle )*fKy+ptCenterNew.y;
	fx4 = ( rc.right-ptCenter.x )*::cos( fAngle )*fKx+( rc.bottom-ptCenter.y )*::sin( fAngle )*fKy+ptCenterNew.x;
	fy4 = -( rc.right-ptCenter.x )*::sin( fAngle )*fKx+( rc.bottom-ptCenter.y )*::cos( fAngle )*fKy+ptCenterNew.y;

	rect.left = int( min( min( fx1, fx2 ), min( fx3, fx4 ) ) );
	rect.right = rect.left+int( max( max( fx1, fx2 ), max( fx3, fx4 ) ) - min( min( fx1, fx2 ), min( fx3, fx4 ) ) );
	rect.top = int( min( min( fy1, fy2 ), min( fy3, fy4 ) ) );
	rect.bottom = rect.top+int( max( max( fy1, fy2 ), max( fy3, fy4 ) ) - min( min( fy1, fy2 ), min( fy3, fy4 ) ) );

	return rect;
}

/******* Maxim 16.04.2002 **************************/ 
void Rotate(CPoint *Ret,CPoint *Cent, float Angle)
{
	float Vector[] = {Ret->x - Cent->x,0,Ret->y - Cent->y,1};

	float MatrixY[][4] = {(float)cos(Angle), 0, (float)(-sin(Angle)), 0,
						0, 1, 0, 0,
						(float)sin(Angle), 0, (float)cos(Angle), 0,
						0, 0, 0, 1};
	float End[4];

	{
		for(int i = 0; i<4; i += 2)
			End[i] = Vector[0]*MatrixY[0][i]+
					 Vector[1]*MatrixY[1][i]+
					 Vector[2]*MatrixY[2][i]+
					 Vector[3]*MatrixY[3][i];
	}

	Ret->x = End[0]  + Cent->x;
	Ret->y = End[2]  + Cent->y;
}
/***************************************************/ 

BOOL DoAphineTransform( CImageWrp &imgNew, CImageWrp &img, CAphineParams &params, void (*pfnCallBack)(int) )
{
	
	double	a11, a12, a13,
		a21, a22, a23;

	double	fx, fy;
	double	d11, d12, d21, d22;
	int		x, y, xs, ys, xs1, ys1, c;
	int		cx = imgNew.GetWidth();
	int		cy = imgNew.GetHeight();
	int		colors = imgNew.GetColorsCount();

	color	*p1, *p2;

	a11 = cos( -params.fAngle )/params.fKx;
	a12 = sin( -params.fAngle )/params.fKx;
	a13 = -a11*params.ptCenterNew.x-a12*params.ptCenterNew.y+params.ptCenter.x;

	a21 = -sin( -params.fAngle )/params.fKy;
	a22 = cos( -params.fAngle )/params.fKy;
	a23 = -a21*params.ptCenterNew.x-a22*params.ptCenterNew.y+params.ptCenter.y;

	CRect	rectSource = img.GetRect();
	CRect	rectDest = imgNew.GetRect();

	color	**ppcolorsCache = new color*[rectSource.Height()*colors];
	byte	**ppbyteCache = new byte*[rectSource.Height()];

	for( int n = 0; n < rectSource.Height(); n++ ) 
	{
		ppbyteCache[n] = img.GetRowMask( n );
		for( c = 0; c < colors; c++ ) 
			ppcolorsCache[n*colors+c] = img.GetRow( n, c );
	}

	CPoint ptCent;

	ptCent.x = params.ptCenterNew.x - (rectDest.right  - rectSource.right) / 2.;
	ptCent.y = params.ptCenterNew.y - (rectDest.bottom - rectSource.bottom)/2;

	for( y = rectDest.top; y<rectDest.bottom; y++ )
	{
		for( c = 0; c < colors; c++ )
		{
			color	*pd = imgNew.GetRow( y-rectDest.top, c );
			byte	*pdb = imgNew.GetRowMask( y-rectDest.top );

			for( x = rectDest.left; x<rectDest.right; x++ ) 	
			{
/******* Maxim 16.04.2002 **************************/
				fx = a11 * x + a12 * y + a13;
				fy = a21 * x + a22 * y + a23;
/******* Maxim 16.04.2002 **************************
				CPoint pt;
				pt.x = x - (rectDest.right  - rectSource.right) / 2.;
				pt.y = y - (rectDest.bottom - rectSource.bottom)/2;

				Rotate( &pt, &ptCent, -params.fAngle );
				fx = pt.x ;
				fy = pt.y ;
/******* Maxim 16.04.2002 **************************/ 

				xs = int( floor(fx) );
				ys = int( floor(fy) );
				xs1 = xs+1;
				ys1 = ys+1;

				if( xs < rectSource.left )xs++;
				if( ys < rectSource.top )ys++;
				if( xs1 >= rectSource.right )xs1--;
				if( ys1 >= rectSource.bottom )ys1--;

				pdb[x-rectDest.left] = 0;

				if( !rectSource.PtInRect( CPoint( xs, ys ) ) )
					continue;

				d11 = (fx-xs)*(fy-ys);
				d12 = (1-fx+xs)*(fy-ys);
				d21 = (fx-xs)*(1-fy+ys);
				d22 = (1-fx+xs)*(1-fy+ys);

				ys -= rectSource.top;
				ys1 -= rectSource.top;
				xs -= rectSource.left;
				xs1 -= rectSource.left;

				{
					pdb[x-rectDest.left] = ppbyteCache[ys][xs];
				}

				if( pdb[x-rectDest.left] )
				{
					p1 = ppcolorsCache[ys*colors+c];
					p2 = ppcolorsCache[ys1*colors+c];
					pd[x-rectDest.left] = (color)(d11*p2[xs1]+d12*p2[xs]+
											d21*p1[xs1]+d22*p1[xs]+.5);
				}
			}

			if( pfnCallBack!=NULL )
				pfnCallBack( y-rectDest.top );
		}
	}

	delete ppcolorsCache;
	delete ppbyteCache;

	return TRUE;

}

BOOL DoAphineTransformFast( CImageWrp &imgNew, CImageWrp &img, CAphineParams &params, void (*pfnCallBack)(int) )
{
	double	a11, a12, a13,
		a21, a22, a23;

	double	fx, fy;
	int		x, y, xs, ys, c;
	int		cx = imgNew.GetWidth();
	int		cy = imgNew.GetHeight();
	int		colors = imgNew.GetColorsCount();


	a11 = cos( -params.fAngle )/params.fKx;
	a12 = sin( -params.fAngle )/params.fKx;
	a13 = -a11*params.ptCenterNew.x-a12*params.ptCenterNew.y+params.ptCenter.x;

	a21 = -sin( -params.fAngle )/params.fKy;
	a22 = cos( -params.fAngle )/params.fKy;
	a23 = -a21*params.ptCenterNew.x-a22*params.ptCenterNew.y+params.ptCenter.y;

	CRect	rectSource = img.GetRect();
	CRect	rectDest = imgNew.GetRect();

	rectSource -= rectSource.TopLeft();
	
	for( y = 0; y<cy; y++ )
	{
		for( c = 0; c < colors; c++ )
		{
			color	*pd = imgNew.GetRow( y-rectDest.top, c )-rectDest.left;

			for( x = 0; x<cx; x++ ) 	
			{
				fx = a11*x+a12*y+a13;
				fy = a21*x+a22*y+a23;

				xs = int( (fx+.5) );
				ys = int( (fy+.5) );

				if( !rectSource.PtInRect( CPoint( xs, ys ) ) )
					continue;

				color	*p = img.GetRow( ys-rectSource.top, c )-rectSource.left;

				pd[x] = p[xs];
			}

			if( pfnCallBack!=NULL )
				pfnCallBack( y );
		}
	}
	return TRUE;
}


