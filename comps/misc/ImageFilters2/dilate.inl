	smart_alloc(buf, color, mask*2+1);
	color *buf0=buf+mask;
	smart_alloc(xx, int, mask*2+1);
	int *xx0=xx+mask;
	smart_alloc(yy, int, mask*2+1);
	int *yy0=yy+mask;

	int m2=mask/2;
	int left=m2, right=cx-m2;
	int top=m2, bottom=cy-m2;

	color **src, **dst;

	src=srcRows; dst=tmpRows;
	// проход по всем отрезкам по горизонтали
	for(int x0=m2; x0<cx+m2; x0+=mask)
	{
		if(x0>cx-1-m2) x0=cx-1-m2;
		for(int i=-mask; i<=mask; i++) 
			xx0[i]=max(0,min(cx-1,x0+i));
		// проход по всем строчкам внутри данного отрезка
		for(int y=0; y<cy; y++)
		{
			buf0[0] = src[y][xx0[0]];
			for(int i=1; i<=mask; i++)
			{
				buf0[i]=CMP(buf0[i-1],src[y][xx0[i]]);
				buf0[-i]=CMP(buf0[-i+1],src[y][xx0[-i]]);
			}
			for(int i=-m2; i<=m2; i++)
				dst[y][xx0[i]]=CMP(buf0[i-m2],buf0[i+m2]);
		}
	}

	src=tmpRows; dst=dstRows;
	// проход по всем отрезкам по вертикали
	for(int y0=m2; y0<cy+m2; y0+=mask)
	{
		if(y0>cy-1-m2) y0=cy-1-m2;
		for(int i=-mask; i<=mask; i++) yy0[i]=max(0,min(cy-1,y0+i));
		// проход по всем столбцам внутри данного отрезка
		for(int x=0; x<cx; x++)
		{
			buf0[0] = src[yy0[0]][x];
			for(int i=1; i<=mask; i++)
			{
				buf0[i]=CMP(buf0[i-1],src[yy0[i]][x]);
				buf0[-i]=CMP(buf0[-i+1],src[yy0[-i]][x]);
			}
			for(int i=-m2; i<=m2; i++) dst[yy0[i]][x]=CMP(buf0[i-m2],buf0[i+m2]);
		}
	}
