#include <stdio.h>
#include <api.h>

void FillVTImageRow(Image *image, unsigned int _y, unsigned short** pPane, unsigned short numPanes)
{
	register union { unsigned short w; struct { unsigned char l, h; }; } w = { 0 };
	unsigned ecx = 0; 

	if (0 > _y)
		return;
	if (0 == image->pixels)
		return;

	if (0 == UncondenseImage(image))
		return;

	if (!(_y < image->rows))
		return;

	if (image->columns <= 0)
		return;


	if (numPanes>4)
		numPanes = 4;
	for (; ecx < image->columns; ++ecx)
	{
		RunlengthPacket* p = image->pixels + image->columns *_y + ecx;

		switch (numPanes){
		case 4:
			w.h = *((unsigned char*)p + 4);
			pPane[3][ecx] = w.w;
		case 3:
			w.h = p->blue;
			pPane[2][ecx] = w.w;
		case 2:
			w.h = p->green;
			pPane[1][ecx] = w.w;
		case 1:
			w.h = p->red;
			pPane[0][ecx] = w.w;
		default:;
		}
	}
}

void FillMagickImageRow(Image *image, unsigned int _y, unsigned short** pPane, unsigned short numPanes)
{

	if (0 > _y)
		return;
	if (0 == image->pixels)
		return;

	if (0 == UncondenseImage(image))
		return;

	if (!(_y < image->rows))
		return;

	if (image->columns > 0)
	{
		union _pane { unsigned short w; struct { unsigned char l, h; }; };
		unsigned ecx = 0; 

		if (numPanes>4)
			numPanes = 4;
		for (; ecx < image->columns; ++ecx)
		{
			RunlengthPacket* pack = image->pixels + image->columns *_y + ecx;

			switch (numPanes){
			case 4:
				*((unsigned char*)pack + 4) = ((union _pane*)&pPane[3][ecx])->h;
			case 3:
				pack->blue	= ((union _pane*)&pPane[2][ecx])->h;
			case 2:
				pack->green	= ((union _pane*)&pPane[1][ecx])->h;
			case 1:
				pack->red	= ((union _pane*)&pPane[0][ecx])->h;
			default:;
			}
		}
	}
	image->class = DirectClass;
}
