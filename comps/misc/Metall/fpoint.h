#pragma once



struct FPOINT
{
	double x,y;
	FPOINT() {}
	FPOINT(POINT pt)
	{
		this->x = pt.x;
		this->y = pt.y;
	}
	FPOINT(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
	FPOINT(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};

struct FRECT
{
	double left,right,top,bottom;
	FRECT() {}
	FRECT(double left, double right, double top, double bottom)
	{
		this->left = left;
		this->right = right;
		this->top = top;
		this->bottom = bottom;
	}
	FRECT(RECT rc)
	{
		this->left = rc.left;
		this->right = rc.right;
		this->top = rc.top;
		this->bottom = rc.bottom;
	}
};