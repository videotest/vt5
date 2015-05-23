#if !defined(__PaintDC_H__)
#define __PaintDC_H__

class paint_dc
{
public:
	HDC hdc;
	HWND hwnd;
	PAINTSTRUCT	ps;	
	paint_dc(HWND hwnd)
	{
		this->hwnd = hwnd;
		hdc = ::BeginPaint(hwnd, &ps);
	}
	~paint_dc()
	{
		::EndPaint(hwnd, &ps);
	}
};

#endif