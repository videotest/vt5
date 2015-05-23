#include "stdafx.h"
#include "actions.h"
#include "resource.h"
#include "math.h"

#include "ImagePane.h"
#include "clone.h"
#include "clonedata.h"


//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionEven, CCmdTargetEx);

// olecreate 

// {6329E33B-C3F1-4ac7-AF5D-778D31DA6832}
GUARD_IMPLEMENT_OLECREATE(CActionEven, "Even.Even",
0x6329e33b, 0xc3f1, 0x4ac7, 0xaf, 0x5d, 0x77, 0x8d, 0x31, 0xda, 0x68, 0x32);

// guidinfo 

// {0B10E415-2617-4f2a-82A6-A38F064BEDDC}
static const GUID guidEven =
{ 0xb10e415, 0x2617, 0x4f2a, { 0x82, 0xa6, 0xa3, 0x8f, 0x6, 0x4b, 0xed, 0xdc } };


//[ag]6. action info

ACTION_INFO_FULL(CActionEven, IDS_ACTION_EVEN, -1, -1, guidEven);
//[ag]7. targets

ACTION_TARGET(CActionEven, szTargetAnydoc);
//[ag]9. implementation


ACTION_ARG_LIST(CActionEven)
	ARG_INT(_T("MaskSize"), 16)
	ARG_BOOL(_T("Contrast"), 0)
	ARG_BOOL(_T("EvenColor"), 0)
	ARG_BOOL(_T("Restriction"), 0)
	ARG_IMAGE(_T("Source"))
	RES_IMAGE(_T("Even"))
END_ACTION_ARG_LIST();


//////////////////////////////////////////////////////////////////////
//CActionEven implementation
CActionEven::CActionEven()
{
}

CActionEven::~CActionEven()
{
}

#pragma optimize("", off)

bool CActionEven::InvokeFilter()
{	
	//const WORD MAX_COLOR = 65535;
	const WORD MAX_COLOR = 255;

	long nMaskSize = GetArgumentInt("MaskSize");
	BOOL bContrast = GetArgumentInt("Contrast");
	BOOL bEvenColor = GetArgumentInt("EvenColor");
	BOOL bRestriction = GetArgumentInt("Restriction");


	CImageWrp	image( GetDataArgument() );

	CString strCC = image.GetColorConvertor();

	bool bUseHSB = (strCC != "RGB" && strCC != "GRAY" && bEvenColor == FALSE);

	long	colors = image.GetColorsCount();
	long	cx = image.GetWidth(), cy = image.GetHeight();

	
	CImageWrp	imageNew( GetDataResult() );
	imageNew.CreateCompatibleImage( image );

	CImageWrp	imageWork;
	if( !imageWork.CreateNew(cx, cy, bEvenColor==TRUE?strCC : "GRAY") )
		return false;


	CImageWrp	imageHSB;
	if( !imageHSB.CreateNew(cx, cy, "HSB") )
		return false;


	sptrIColorConvertor3 sptrCCScr = 0;
	sptrIColorConvertor3 sptrCCDest = 0;

	double*	pLab = 0;
	

	if(bUseHSB && strCC != "HSB")
	{
		IUnknown* punk = 0;
		image->GetColorConvertor(&punk);
		sptrCCScr = punk;
		punk->Release();
		imageHSB->GetColorConvertor(&punk);
		sptrCCDest = punk;
		punk->Release();

		pLab = new double[cx*3];
	}

	

	int proc = 0; 

	long x = 0, y = 0, c = 0, i = 0, j = 0;

	DWORD* spectr = new DWORD[MAX_COLOR+1];   

	long colorsWork = imageWork.GetColorsCount();

	StartNotification(cy, colorsWork+2/*+(colors>1?2:0)*/);

	color **pGrayCopy = new color*[cy];
	color **ppRowS = new color*[colors];
	color **ppRowD = new color*[colorsWork];

	color **ppRowImgNew = new color*[colors];

	
	


	for(y = 0; y < cy; y++)
	{
				
		for(c = 0; c < colors; c++)
		{
			ppRowS[c] = image.GetRow(y, c);
			if(bUseHSB && strCC != "HSB")
				ppRowImgNew[c] = imageHSB.GetRow(y, c);
			else
				ppRowImgNew[c] = imageNew.GetRow(y, c);
			if(bEvenColor==TRUE)
				ppRowD[c] = imageWork.GetRow(y, c);
		}
		if(bEvenColor==FALSE)
		{
			ppRowD[0] = imageWork.GetRow(y, 0);
			if(colors > 1 && !bUseHSB)
				pGrayCopy[y] = new color[cx];
		}


		if(!bUseHSB)
		{
			for(x = 0; x < cx; x++)
			{
				if(bEvenColor==TRUE)
				{
					for(c = 0; c < colors; c++)
					{
						color clr = (byte)(ppRowS[c][x]>>8);
						ppRowD[c][x] = clr;
						spectr[clr]++;
					}
				}
				else
				{
					color maxcolor = 0;
					color mincolor = MAX_COLOR;
					color clr = 0;
					for(c = 0; c < colors; c++)
					{
						clr = (byte)(ppRowS[c][x]>>8);
						maxcolor = max(clr, maxcolor);
						mincolor = min(clr, mincolor);

						ppRowImgNew[c][x] = clr;

						spectr[clr]++;
					}
					if(colors > 1)
						pGrayCopy[y][x] = ppRowD[0][x] = (byte)((maxcolor + mincolor)>>1);
					else
						ppRowD[0][x] = clr;
				}
			} 
		}
		else
		{
			if(strCC == "HSB")
			{
				for(c = 0; c < colors; c++)
				{
					memcpy(ppRowImgNew[c], ppRowS[c], cx*sizeof(color));
				}
			}
			else
			{
				sptrCCScr->ConvertImageToLAB(ppRowS, pLab, cx);
				sptrCCDest->ConvertLABToImage(pLab, ppRowImgNew, cx);
			}

			for(x = 0; x < cx; x++)
			{
				color clr = (byte)(ppRowImgNew[2][x]>>8);
				ppRowD[0][x] = clr;
				spectr[clr]++;
			}
		}

		
		Notify(y);
		
	}
	NextNotificationStage();

	delete ppRowD;

	


	/*double*		pLab;
	pLab = new double[cx*3];
	
	StartNotification( cy );
	IUnknown* punk;
	image->GetColorConvertor(&punk);
	sptrIColorConvertor3 sptrCCScr(punk);
	punk->Release();
	imageNew->GetColorConvertor(&punk);
	sptrIColorConvertor3 sptrCCDest(punk);
	punk->Release();

	color	**ppcolorscr = new color*[colors];
	color	**ppcolordest = new color*[colors];
	for( long y = 0; y < cy; y++ )
	{
		for( long c = 0; c  < colors; c++ )
		{
			ppcolorscr[c] = image.GetRow( y, c );
			ppcolordest[c] = imageNew.GetRow( y, c );
		}
		sptrCCScr->ConvertImageToLAB(ppcolorscr, pLab, cx);
		sptrCCDest->ConvertLABToImage(pLab, ppcolordest, cx);
		Notify( y );
	}

	delete pLab;
	delete ppcolorscr;
	delete ppcolordest;
	*/


	
	long mconst = nMaskSize;

	if (mconst <= 15 )
	{
		mconst = (long)((cx+cy)>>4);
	}

	if(mconst > min(cx,cy)>>1)
		mconst = min(cx,cy)>>1;
    
	if (mconst < 11) 
		mconst = 11;

	long mmconst = (long)mconst>>1; 

	nMaskSize = (int)mconst/10+1;
	if(nMaskSize < 7) 
		nMaskSize = 7;
	 
     
	long xms = (long)cx/mmconst+5;
	long yms = (long)cy/mmconst+5;

	long* simg0 =	new long[xms*yms+1];   
	long* simg1 =	new long[xms*yms+1];    
	long* simg01 =	new long[xms*yms+1];    
	long* simgs =	new long[xms*yms+1];

	ZeroMemory(simg01, (xms*yms+1)*sizeof(long));

	double* simgkof0 = new double[xms*yms+1];
	double* simgkof1 = new double[xms*yms+1];

	DWORD* gist = new DWORD[MAX_COLOR+1];   
	
	
	 	
	long gox = (long)(cx/mmconst-0.5)-1;
	long goy = (long)(cy/mmconst-0.5)-1;

	long cmax = 0;
	long cmin = 0;
	DWORD sum = 0;
	double kcontr = 1;
	long delta = 0;

	DWORD b1 = 0;
	DWORD b2 = 0;


	color* pMasks = new color[(cx-1)*(cy-1)];

	//if(bEvenColor==TRUE && colors > 1) 
	{
		proc = 100;

		
		long porog = (long)mconst*mconst/proc;
		if(porog < 3) 
			porog = 3;

		

		for(c = 0; c < colorsWork; c++)
		{
			//if(c != 0)
				//NextNotificationStage();

///////////////////////////////////////////////////////////////////////
			long y = 1;
			color* pCurRow1 = imageWork.GetRow(y-1, c);
			color* pCurRow2 = imageWork.GetRow(y, c);
			color* pCurRow3 = 0;
			for(y = 1; y < cy-1; y++)			   
			{
				long nRow = (cx-1)*(cy-y-1);

				if(y > 1)
				{
					pCurRow1 = pCurRow2;
					pCurRow2 = pCurRow3;
				}
				pCurRow3 = imageWork.GetRow(y+1, c);
				
				
				for (long x = 1; x < cx-1; x++)
				{
					sum = 0;
					for (long i = x-1; i < x+2; i++)
					{
						sum += pCurRow1[i] + pCurRow2[i] + pCurRow3[i];
					}
					
					pMasks[nRow + x] = (color)(sum/9);
				}

				Notify(y);
			}
			NextNotificationStage();
///////////////////////////////////////////////////////////////////////

			long nCounter = 0;				

			for (long j = 0; j < goy; j++)
			{
				long ym = (j+2)*mmconst;                              
				if (ym > cy-1) 
					ym = cy-1;

				long J2 = j*mmconst+1;

				for (long i = 0; i < gox; i++)
				{
					long iyms = i*yms;

					long xm = (i+2)*mmconst;     
					if (xm > cx-1) 
						xm = cx-1;		      
									 
					
					ZeroMemory(gist, (MAX_COLOR+1)*sizeof(DWORD));


					sum = 0;
					long i2, j2;
					nCounter = 0;
					long I2 = i*mmconst+1;

					for (j2 = J2; j2<ym+1; j2++)
					{
						long nJ = (cx-1)*(cy-j2-1);
						for (i2 = I2;i2<xm+1;i2++)
						{
							color colorMask = pMasks[nJ + i2];
							gist[colorMask]++;
							sum += colorMask;
							nCounter++;
						}
					}

					simgs[iyms+j]= (long)((double)sum/nCounter);

					DWORD a111 = 0;
					for (i2 = 0; i2 <= MAX_COLOR/3-1; i2++)
					{
						DWORD dwParam = gist[i2*3+1]+gist[i2*3+2]+gist[i2*3+ 3/*((i2==(MAX_COLOR/3-1))?2:3)*/];
						if (a111 < dwParam)
						{
							a111 = dwParam;
							simg01[iyms+j]= i2*3+1;   
						}
					}

					porog=(long)((double)((xm-i*mmconst)*(ym-J2 - 1))/(double)proc);

					if(porog<3) 
						porog=3;

					cmin = 0;
					cmax = MAX_COLOR;			 


					DWORD sum1 = 0;
					DWORD sum2 = 0;
					bool bNeedProcessSum1 = true;
					bool bNeedProcessSum2 = true;

					for (long i3 = 0; i3 <= MAX_COLOR; i3++)
					{
						sum1 += gist[i3];
						sum2 += gist[MAX_COLOR-i3];
						if (sum1 < porog && bNeedProcessSum1)
							cmin = i3;
						else
							bNeedProcessSum1 = false;

						if (sum2 < porog && bNeedProcessSum2)
							cmax = MAX_COLOR-i3;
						else
							bNeedProcessSum2 = false;

						if(!bNeedProcessSum1 && !bNeedProcessSum2)
							break;
					}   


					
					if(cmin > cmax)
						cmax = cmin = long((cmin+cmax)>>1);
						
					simg0[iyms+j]= cmin;   
					simg1[iyms+j]= cmax;      
				}
				//Notify(j);
			}

			
///////////////////////////////////////////////////////////////////////

			long kr0 = 0;
			long kr1 = 0;  
			long kr2 = 0;
			long a1 = 0;
			long a2 = 0;
			long a3 = 0;
			long a4 = 0;
		    
			DWORD sumA = 0;
			DWORD sumB = 0;
			DWORD sumC = 0;

			for (i = 0; i < gox-1; i++)
			{
				long iyms = i*yms;
				for (j = 0; j < goy-1; j++)	
				{
					a1 = simg0[iyms+j]-simg0[iyms+j+1];
					kr0=kr0+abs(a1);

				   	a2 = simg0[iyms+j]-simg0[(i+1)*yms+j];
					kr0=kr0+abs(a2); 
                        
					a3 = simg1[iyms+j]-simg1[iyms+j+1];
					kr1=kr1+abs(a3);

					a4 = simg1[iyms+j]-simg1[(i+1)*yms+j];
					kr1=kr1+abs(a4); 
             
					a3 = simg01[iyms+j]-simg01[iyms+j+1];
					kr2=kr2+abs(a3);

					a4 = simg01[iyms+j]-simg01[(i+1)*yms+j];
					kr2=kr2+abs(a4);

					sumA += simg1[iyms+j]; 
					sumB += simg01[iyms+j]; 
					sumC += simg0[iyms+j]; 
      			};	    
			}
			

				
			long* simg2 = simg0;
			sum = sumC;

			a4 = kr0;
			if(kr1 <= a4)
			{
				simg2 = simg1;
				a4 = kr1;

				sum = sumA;
			} 

			if(kr2 <= a4)
			{
				simg2 = simg01;
				a4 = kr2;

				sum = sumB;
			} 
///////////////////////////////////////////////////////////////////////
			
		                               
			/*nCounter = 0;
			sum = 0;
				
			for (j = 0; j < goy; j++)
			{
	    		for (i = 0; i < gox; i++)
				{
					sum += simg2[i*yms+j]; 
					nCounter++;
				}         
			}
			

			long sr = (long)((double)sum/(double)nCounter);
			*/
			long sr = sum/(goy*gox);


			nCounter = 0;
			sum = 0;
			long mincolor = MAX_COLOR;
			long maxcolor = 0;
			
			
			for (i = 0; i < gox; i++)
			{
				long iyms= i*yms;
	    		for (j = 0; j < goy; j++)	
				{
					long nsimg2 = simg2[iyms+j];
					sum += simgs[iyms+j] - nsimg2 + sr; 
					nCounter++;
					mincolor = min(mincolor, simg0[iyms+j]-nsimg2 + sr);
					maxcolor = max(maxcolor, simg1[iyms+j]-nsimg2 + sr); 
				}         
			}
			
			maxcolor = min(maxcolor, MAX_COLOR);
			mincolor = max(mincolor, 0);

			
			if(bEvenColor==FALSE)
			{
				kcontr = (double)MAX_COLOR/(maxcolor-mincolor);
				if (kcontr <= 0) kcontr = -kcontr;
				delta = mincolor*kcontr;
			}


			long sr_even = (long)((double)sum/nCounter);


			if(bRestriction == FALSE)
				sr_even = 0;			
			else
				sr_even=(long)(sr_even*kcontr)-delta;

			
			long dup = MAX_COLOR - sr_even;
			long ddown = sr_even;
		    long ddownij = 0;
			long dupij = 0;

			
			if (bContrast == TRUE)
			for (i = 0; i < gox; i++)
			{
				long iyms = i*yms;
	    		for (j = 0; j < goy; j++)
				{
					ddownij=sr_even-(simg0[iyms+j]-simg2[iyms+j]+sr)*kcontr+delta;
					if (ddownij<1) ddownij=1;

					dupij=(simg1[iyms+j]-simg2[iyms+j]+sr)*kcontr-delta-sr_even;
					if (dupij<1) dupij=1;

					if(bRestriction==TRUE)
					{
						simgkof1[iyms+j]=dup/(double)dupij;
						simgkof0[iyms+j]=ddown/(double)ddownij;
						
						if (simgkof1[iyms+j]<1) simgkof1[iyms+j] = 1;
						if (simgkof1[iyms+j]>3) simgkof1[iyms+j] = 3;
						if (simgkof0[iyms+j]<1) simgkof0[iyms+j] = 1;
						if (simgkof0[iyms+j]>3) simgkof0[iyms+j] = 3;

					}
					else
					{
						simgkof0[iyms+j]=MAX_COLOR/(double)((simg1[iyms+j]-simg0[iyms+j])*kcontr);
						simgkof1[iyms+j]=simg0[iyms+j];
            
						if (simgkof0[iyms+j]<1) simgkof0[iyms+j] = 1;
						if (simgkof0[iyms+j]>20) simgkof0[iyms+j] = 20; 
					}
				}
			}

			
///////////////////////////////////////////////////////////////////////
			double x1u,x2u,x3u,x4u,y1u,y2u,x1d,x2d,x3d,x4d,y1d,y2d;
			double kcontr1,kcontr2;
			long xw, yw, bx1, by1, bx2, by2; 		 
			long x1 = 0;
			long x2 = 0;
			long x3 = 0;
			long x4 = 0;
			long y1 = 0;
			long y2 = 0;
			long x = 0;
			/*int*/ y = 0;

			int type = 2; 
		                			
			for (j = 1; j < goy; j++)
			{
				for (i = 1; i < gox; i++)
				{      
					bx1=0;
					if (i<=1) bx1=(mmconst>>1)*type;
					bx2=0;
					if (i==gox-1) bx2=2*mmconst*type;
					by1=0;
					if (j<=1) by1=(mmconst>>1)*type;
					by2=0;
					if (j==goy-1) by2=2*mmconst*type;
              
					
					for (yw=0-by1;yw<mmconst+by2;yw++)
					{
						y=yw;
						if (y<0) y=0;
						if (y>mmconst) y=mmconst;
						long j2 =  (j-1)*mmconst+(mmconst/2)*type+yw;

						if (j2<cy && j2>=0)  
						{

						color* pRow = imageWork.GetRow(j2, c);

						for (xw=0-bx1;xw<mmconst+bx2;xw++)
						{
							x=xw;
							if (x<0) x=0;
							if (x>mmconst) x=mmconst;
							x1 = simg2[(i-1)*yms+j-1];
							x2 = simg2[i*yms+j-1];
							y1 = (x2-x1)*x/mmconst+x1;
								 
							x3 = simg2[(i-1)*yms+j];
							x4 = simg2[i*yms+j];
							y2 = (x4-x3)*x/mmconst+x3;

							if (bContrast == TRUE)
							{
								x1u = simgkof1[(i-1)*yms+j-1];
								x2u = simgkof1[i*yms+j-1];
								y1u = (x2u-x1u)*(double)x/mmconst+x1u;
									 
								x3u = simgkof1[(i-1)*yms+j];
								x4u = simgkof1[i*yms+j];
								y2u = (x4u-x3u)*(double)x/mmconst+x3u;

								x1d = simgkof0[(i-1)*yms+j-1];
								x2d = simgkof0[i*yms+j-1];
								y1d = (x2d-x1d)*(double)x/mmconst+x1d;
									 
								x3d = simgkof0[(i-1)*yms+j];
								x4d = simgkof0[i*yms+j];
								y2d = (x4d-x3d)*(double)x/mmconst+x3d;

							}

							
										
							long i2 =  (i-1)*mmconst+(mmconst/2)*type+xw; 
							            
							if (i2<cx) 
							if (i2>=0) 
							{                
								long a = pRow[i2] - (y2-y1)*y/mmconst - y1 + sr;
								a = a*kcontr-delta;

								if(bContrast == TRUE)
								{
									if(bRestriction == TRUE)
									{

										if (a>sr_even)
										{
											kcontr1=(y2u-y1u)*(double)y/mmconst+y1u;

											if (kcontr1<1) kcontr1 = 1;
											if (kcontr1>3) kcontr1 = 3;

		 									a=sr_even+((double)(a-sr_even)*kcontr1);
										}
										else
										{
											kcontr1=(y2d-y1d)*(double)y/mmconst+y1d;

											if (kcontr1<1) kcontr1 = 1;
											if (kcontr1>3) kcontr1 = 3;

											a=sr_even-((double)(sr_even-a)*kcontr1);
										}

									}
									else
									{
										 // контраст в каждой маске без ограничений

										kcontr2=(y2u-y1u)*(double)y/mmconst+y1u;
										kcontr1=(y2d-y1d)*(double)y/mmconst+y1d;
										long minbord=kcontr2-(y2-y1)*y/mmconst-y1+sr;
										minbord = minbord*kcontr-delta;
										a=(abs(a-minbord))*kcontr1;
									}
								}

								pRow[i2] = max(0, min(a, MAX_COLOR));
														 
							}  
						}
						}
					}
				}
				//Notify(j);
			}		

			
///////////////////////////////////////////////////////////////////////			
			
		}

		//FinishNotification();

		if(colors > 1)
		{
			//autocontrast!!
			proc=10000;

			color mincolor=0;
			color maxcolor=MAX_COLOR;			 

			long porog33=colors*cx*cy/proc;		 
			if (porog33<3) 
				porog33=3;

			for (i = 0; i <= MAX_COLOR; i++)
			{
				b1 += spectr[i];
				if (b1<porog33 ) 
					mincolor = i;

				b2 += spectr[MAX_COLOR-i];
				if (b2<porog33 ) 
					maxcolor = MAX_COLOR-i;
			} 
			

			kcontr = MAX_COLOR/(maxcolor-mincolor);

			if (kcontr <= 0)
			{
				kcontr = -kcontr;
			}

			delta = mincolor*kcontr;

			/*for (j=0; j < cy; j++)
			{                  
				for(c = 0; c < colors; c++)
				{
					color* pRow = imageNew.GetRow(j, c);
					for (i = 0; i < cx; i++)      
					{ 
						long a = (pRow[i]*kcontr)-delta;

						pRow[i] = max(0, min(MAX_COLOR, a));
					}
				}
				Notify(j);
			}
			NextNotificationStage();
			*/

		}

		
		if(bEvenColor==FALSE && colors > 1)
		{      

			if(!bUseHSB)
			{
				double *COS=new double[MAX_COLOR+1];
				double lum = 0;

				for (i = 0; i <= MAX_COLOR; i++)
					COS[i]=sin(3.1415*(double)i/MAX_COLOR)+0.01;

				for (j=0; j < cy; j++)
				{
					for(c = 0; c < colors; c++)
					{
						color* pRow = imageNew.GetRow(j, c);
						color* pWorkRow = imageWork.GetRow(j, 0);
						for (long i2=0; i2 < cx; i2++)
						{
							lum = (pWorkRow[i2]+delta)/kcontr;			 			 
							lum = max(0, min(lum, MAX_COLOR));
							double lum1=(COS[(int)lum]/COS[pGrayCopy[j][i2]]);

							hyper C = lum-(pGrayCopy[j][i2]-pRow[i2])*lum1;
							C = (C*kcontr)-delta;
							if(C < 0)
								TRACE0("\n");
							C = max(0, min(C, MAX_COLOR));

							pRow[i2] = ((color)C)<<8;
						}
					}
					Notify(j);

					delete pGrayCopy[j];
				}

				//NextNotificationStage();
				delete COS;
			}
			else
			{
				for(y = 0; y < cy; y++)
				{
					color *pRowS = imageWork.GetRow(y, 0);
					color *pRowD = 0;
					if(strCC != "HSB")
						pRowD = imageHSB.GetRow(y, 2);
					else
						pRowD = imageNew.GetRow(y, 2);
						

					for(x = 0; x < cx; x++)
					{
						long a = pRowS[x];
						if(colors > 1)
							a = (a*kcontr)-delta;
						a =max(0, min(MAX_COLOR, a));
						pRowD[x] = a<<8;
					}

					//convert to origin
					if(strCC != "HSB")
					{
						for(c = 0; c < colors; c++)
						{	
							ppRowS[c] = imageHSB.GetRow(y, c);
							ppRowImgNew[c] = imageNew.GetRow(y, c);
						}

						sptrCCDest->ConvertImageToLAB(ppRowS, pLab, cx);
						sptrCCScr->ConvertLABToImage(pLab, ppRowImgNew, cx);
					}

					Notify(y);
				}
				
			}
		}
		else
		{
			//imageWork -> imageNew
			for(y = 0; y < cy; y++)
			{
				for(c = 0; c < colors; c++)
				{
					color *pRowS = imageWork.GetRow(y, c);
					color *pRowD = imageNew.GetRow(y, c);
					//memcpy(pRowD, pRowS, cx*sizeof(color));

					for(x = 0; x < cx; x++)
					{
						long a = pRowS[x];
						if(colors > 1)
							a = (a*kcontr)-delta;

						a =max(0, min(MAX_COLOR, a));
						pRowD[x] = a<<8;
					}
				}
				Notify(y);
			}
			//NextNotificationStage();

		}
	


		
		
		
	}
	/*else
	{
	}*/

	

	delete simg0;
	delete simg1;    
	delete simg01;    
	delete simgs;
	delete simgkof0;
	delete simgkof1;
	delete gist;   
	delete spectr;   

	delete pMasks;

	delete pGrayCopy;

	delete ppRowS;
	delete ppRowImgNew;
	if(bUseHSB && strCC != "HSB")
		delete pLab;


	/*for(y = 0; y < cy; y++)
	{
		for(c = 0; c < colors; c++)
		{
			color *pRowD = imageNew.GetRow(y, c);

			//memcpy(pRowD, pRowS, cx*sizeof(color));
			for(x = 0; x < cx; x++)
			{
				long a = (pRowD[x]*kcontr)-delta;
				pRowD[x] = max(0, min(65535, a<<8));
			}
		}
		Notify(y);
	}*/

	FinishNotification();
	

	return true;
}




/*
void CTest1Func::DoWork( void **pargs, void *pres )
{

    int cheng = AfxGetApp()->GetProfileInt( "Settings", "cheng", 1 );

    iConst=m_iOffset;
	C8Image	*pimgbin = (C8Image	*)CImageBase::MakeFromBits( ((CDIBArgument*)pargs[0])->m_lpbi, FALSE );

	int	cx = pimgbin->GetCX();
	int	cy = pimgbin->GetCY();
	int ncolors = pimgbin->GetCols();	


	int Hx_max = cx;
	int Hy_max = cy;

        int q=1;//  = 1 or 3
		int opt=0;
	if (cx*2<ncolors) {q = 3;opt=1;};


    if (colorflag==1 && q==3) 
	{

	C8Image	*pimg = new C8Image( Hx_max , Hy_max, opt );
	C8Image	*pimgshow = new C8Image( Hx_max , Hy_max, opt );

    int	iRowMemSize = (cx*q+3)/4*4;
	int	iRowMemSizeh = (cx+3)/4*4;

	byte	*pur = (byte*)pimgbin->GetBI();
	pur+=(UINT)((byte*)(pimgbin->GetBI()->biSize+((q==1)?1024:0)));
	byte	*pury;

	byte	*psh = (byte*)pimgshow->GetBI();
	psh+=(UINT)((byte*)(pimgshow->GetBI()->biSize+((q==1)?1024:0)));
	byte	*pshy;


    int i2,j2;

    for (j2=0; j2 < cy; j2++)
	{
		pury = (byte*)(pur+j2*iRowMemSize);
    	pshy = (byte*)(psh+j2*iRowMemSize);
		for (i2=0; i2 < cx*q; i2++)
            pshy[i2] = pury[i2];
    }


	((CBINArgument*)pres)->m_lpbi = pimgshow->GetBI();
	pimgshow->SetFlag( TRUE );

	StartNotification( Hy_max*q );

	    int MAX = 0;






	int qi=0;
	int noti;


//  begin registred	     
    int proc;
	proc = AfxGetApp()->GetProfileInt( "Settings", "ContrOut", 500 );
	AfxGetApp()->WriteProfileInt( "Settings", "ContrOut", proc );
	proc = 100;
	int sr;                   
	long b1;
	int a;
	int a1;
	int a2;
	int a3;
	int a4; 
	DWORD sum;
	int i1;
	int j1; 
	int i3;
	int j3;
	int nal;
	int cmin;
	int cmax; 
	int xm;
	int ym;
	int x;
	int y;  
	int x1;
	int y1;
	int x2;
	int y2;  
	int x3;
	int x4;
	int maxcolor;
	int mincolor;

	
	
	int mconst;
    int mask;
	long kr0;
	long kr1;
	long kr2;
    long porog;
		 
	int xms;
	int yms;
	int *simg0;   
	int *simg1;
	int *simg01;
	int *simg2;

	DWORD *spectr = new DWORD[300];
	

    double kcontr;
		int delta ;
		int mmconst; 
		int bx1;
		int by1;
		int bx2;
		int by2; 		 

//  end registred	


	 maxcolor=0;
	 mincolor=255;



   noti=0;

	byte	*pre = (byte*)pimg->GetBI();
	pre+=(UINT)((byte*)(pimg->GetBI()->biSize+((q==1)?1024:0)));
	byte	*prey;



    for (j2=0; j2 < cy; j2++)
	{
			   pury = (byte*)(pur+j2*iRowMemSize);
   		   prey = (byte*)(pre+j2*iRowMemSize);
       for (i2=0; i2 < cx*q; i2++)
            prey[i2] = pury[i2];
    }




for (qi=0 ;qi<q;qi++)
{

	C8Image	*pimgh = new C8Image( Hx_max , Hy_max, 0 );


   	byte	*ph = (byte*)pimgh->GetBI();
	ph+=(UINT)((byte*)(pimgh->GetBI()->biSize+((1==1)?1024:0)));
	byte	*phy;

   noti=0;

    for (j2=1; j2 < cy-1; j2++)			   
	  {
       for (i2=1; i2 < cx-1; i2++)
	   {
	     sum=0;
         for (j3=j2-1; j3 < j2+2; j3++)			   
            for (i3=i2-1; i3 < i2+2; i3++)
              sum=sum+(*pimg)[j3][i3*q+qi];
          (*pimgh)[j2][i2]=(BYTE)(sum/9);
        }
     Notify(noti++);
       }





	 sr=0;b1;a=0;a1=0;a2=0;a3=0;a4=0; sum=0;i1=0;j1=0;i2=0;j2=0;  i3=0;j3=0;cmin=0;
	cmax=0; xm=0;ym=0;x=0;y=0;  x1=0;y1=0;x2=0;y2=0;  x3=0;x4=0;

	 nal=2;
	
	
	 mconst=(int)iConst;


     if (iConst <= 15 )
	 {mconst = (int) (cx+cy)/16;}

   
        i1 = (int)( min(cx,cy)/2);        

        if (mconst >= i1)
        {mconst = i1;}
         i1 = 0;
       

	if (mconst<=11) {mconst=11;}
	
	
	mmconst = (int)mconst/nal; 

	porog = (long)mconst*mconst/proc;
	if (porog<3) porog=3;

	 kr0=0;
	 kr1=0;
	 kr2=0;
	
	mask = (int)mconst/10+1;

	if (mask <= 7) 
		mask = 7;
	 
     
	 xms = (int)cx/mmconst+5;
	 yms = (int)cy/mmconst+5;
	 simg0 = new int[xms*yms+1];   
	 simg1 = new int[xms*yms+1];    
     simg01 = new int[xms*yms+1];    
	 int *simgs = new int[xms*yms+1];
	 double *simgkof0 = new double[xms*yms+1];
	 double *simgkof1 = new double[xms*yms+1];
	 	DWORD *gist = new DWORD[310];   
	
	 noti=0;
	
	
	int gox = (int)(cx/mmconst-0.5)-1;
	int goy = (int)(cy/mmconst-0.5)-1;
			
    	for (j1=0; j1 < goy; j1++)
			for (i1=0; i1 < gox; i1++)
	          {
				 xm = (i1+2)*mmconst;     
				 if (xm >= cx-1) xm = cx-1;		      
				 ym = (j1+2)*mmconst;                                
				  if (ym >= cy-1) ym = cy-1;
				 
				 cmax=0;
				 cmin=10000;
	//       BEGIN     IN    MASK              
				 for (i2 = 0; i2<300;i2++)
				 gist[i2]=0;

				 sum=0;
				 a1=0;	

				 for (j2 = j1*mmconst+1;j2<ym+1;j2++)
				 {
					phy = (byte*)(ph+(cy-j2-1)*iRowMemSizeh);
					 for (i2 = i1*mmconst+1;i2<xm+1;i2++)
					 {
							gist[phy[i2]]++;
							sum = sum + phy[i2];
							a1++;
					 }
				  }


		  // simgs
      
				  simgs[i1*yms+j1]= (int)((double)sum/(double)a1);

		  // simq01
				 DWORD a111 = 0;
				 for (i2 = 0; i2<84;i2++)
					 if (a111<gist[i2*3+1]+gist[i2*3+2]+gist[i2*3+3])
						   {
						   a111=gist[i2*3+1]+gist[i2*3+2]+gist[i2*3+3];
						   simg01[i1*yms+j1]= i2*3+1;   
						   }
				i2=84; 
				if (a111<gist[i2*3+1]+gist[i2*3+2]+gist[i2*3+2])
						   {
						   a111=gist[i2*3+1]+gist[i2*3+2]+gist[i2*3+2];
						   simg01[i1*yms+j1]= i2*3+1;   
						   }

		   // simq0 && simq1

				 porog=(long)((double)((xm-i1*mmconst)*(ym-j1*mconst))/(double)proc);
				 if (porog<3) porog=3;
				 b1 = 0;
				 cmin=0;
				 cmax=255;			 
				 for (i3=0;i3<256;i3++)
				 {
				   b1 = b1+gist[i3];
				   if (b1<porog ) cmin = i3;
				   else
				   i3=256;
				 }   

				 b1 = 0;
				 for (i3=255;i3>=0;i3--)
				 {
				   b1 = b1+gist[i3];
				   if (b1<porog ) cmax = i3;
				   else
				   i3=-1;
				 }
           
			   if (cmin>cmax)
			   {cmin=int((cmin+cmax)/2);cmax=cmin;} 

			   simg0[i1*yms+j1]= cmin;   
			   simg1[i1*yms+j1]= cmax;      

//       END     IN    MASK              
			 }
//  end first way			 



    cheng = AfxGetApp()->GetProfileInt( "Settings", "cheng", 1 );
	AfxGetApp()->WriteProfileInt( "Settings", "cheng", (int)1 );
    if (cheng!=1)
	{
	 EndNotification();
	 delete spectr;
     pimg->Free();
     delete pimg;
	delete simg0;
	delete simg1;
	delete simg01;
	delete simgs;
	delete simgkof0;
	delete simgkof1;
	delete gist;
	pimgh->Free();
    delete pimgh;




	return;
	}





  */







/*


///////////////////////////////////////////////////////////////
//cheng mask
//int type=1;
    int type=2; 

///////////////////////////////////////////////////////////////  
		   
		kr0 = 0;
		kr1 = 0;  
		kr2 = 0;
		    
	for (i1=0; i1 < gox-1; i1++)
		for (j1=0; j1 < goy-1; j1++)
		{
		    
		    a1 = (int) simg0[(int)(i1*yms+j1)]-simg0[(int)(i1*yms+j1+1)];
		    if (a1<=0 ) a1 = 0-a1;
			kr0=kr0+a1;
		   

            a2 = (int) simg0[(int)(i1*yms+j1)]-simg0[(int)((i1+1)*yms+j1)];
		    if (a2<=0 ) a2 = 0-a2;
            kr0=kr0+a2; 
            
            
		    a3 = (int) simg1[(int)(i1*yms+j1)]-simg1[(int)(i1*yms+j1+1)];
		    if (a3<=0 ) a3 = 0-a3;
            kr1=kr1+a3;

            a4 = (int) simg1[(int)(i1*yms+j1)]-simg1[(int)((i1+1)*yms+j1)];
		    if (a4<=0 ) a4 = 0-a4;
            kr1=kr1+a4; 


             
			a3 = (int) simg01[(int)(i1*yms+j1)]-simg01[(int)(i1*yms+j1+1)];
		    if (a3<=0 ) a3 = 0-a3;
			kr2=kr2+a3;

            a4 = (int) simg01[(int)(i1*yms+j1)]-simg01[(int)((i1+1)*yms+j1)];
		    if (a4<=0 ) a4 = 0-a4;  
			kr2=kr2+a4;


              
      	}	    
		    

		    
		   simg2 = simg0;
		   a4 = kr0;
		   if (kr1 <= a4)
		   {simg2 = simg1;
		   a4 = kr1;} 
		   if (kr2 <= a4)
		   {simg2 = simg01;
		   a4 = kr2;} 
		                        
		
		
		 bx1=0;
		 by1=0;
		 bx2=0;
		 by2=0; 
		                               
         sum=0;
		 a1=0;

		 for (i1=0; i1 < gox; i1++)
	    	for (j1=0; j1 < goy; j1++)
		    {
		      sum=sum+ simg2[i1*yms+j1]; 
			  a1++;
		    }                          
		 sr=(int)((double)sum/(double)a1);



         sum=0;
		 a1=0;
    	 mincolor=255;
		 maxcolor=0;			 

	
        for (i1=0; i1 < gox; i1++)
	    	for (j1=0; j1 < goy; j1++)
		    {
		      sum=sum+simgs[i1*yms+j1]-simg2[i1*yms+j1]+sr; 
			  a1++;
			  if (mincolor>simg0[i1*yms+j1]-simg2[i1*yms+j1]+sr)
			     mincolor=simg0[i1*yms+j1]-simg2[i1*yms+j1]+sr;
              if (maxcolor<simg1[i1*yms+j1]-simg2[i1*yms+j1]+sr)
			     maxcolor=simg1[i1*yms+j1]-simg2[i1*yms+j1]+sr; 
		    }                          

        if (maxcolor>255) maxcolor=255;
		if (mincolor<0) mincolor=0;


          kcontr=(double)1;
          delta=0;



        int sr_even=(int)((double)sum/(double)a1);


        if (supercontr==0)
		sr_even=(int)((double)(sr_even)*kcontr)-delta;
		else
		sr_even=0;


        int dup = 255-sr_even;
		int ddown = sr_even;
         int ddownij=0;
		 int dupij=0;
        for (i1=0; i1 < gox; i1++)
	    	for (j1=0; j1 < goy; j1++)
			{
			ddownij=sr_even-
			(int)((double)(simg0[i1*yms+j1]-simg2[i1*yms+j1]+sr)*kcontr)+delta;
			if (ddownij<1) ddownij=1;

			dupij=(int)((double)(simg1[i1*yms+j1]-simg2[i1*yms+j1]+sr)*kcontr)-delta
			-sr_even;
			if (dupij<1) dupij=1;

            if (supercontr==0)
			{
			
			simgkof1[i1*yms+j1]=(double)dup/((double)dupij);
			
			simgkof0[i1*yms+j1]=(double)ddown/((double)ddownij);
			
			if (simgkof1[i1*yms+j1]<1) simgkof1[i1*yms+j1]=(double)1;
			if (simgkof1[i1*yms+j1]>3) simgkof1[i1*yms+j1]=(double)3;
            if (simgkof0[i1*yms+j1]<1) simgkof0[i1*yms+j1]=(double)1;
			if (simgkof0[i1*yms+j1]>3) simgkof0[i1*yms+j1]=(double)3;
			}
			else
			{
			simgkof0[i1*yms+j1]=(double)255/((double)(simg1[i1*yms+j1]-simg0[i1*yms+j1])*kcontr);
			simgkof1[i1*yms+j1]=(double)simg0[i1*yms+j1];
            
			if (simgkof0[i1*yms+j1]<1) simgkof0[i1*yms+j1]=(double)1;
			if (simgkof0[i1*yms+j1]>20) simgkof0[i1*yms+j1]=(double)20;
			} 

			
			}



		  
		 noti=0;       
		
		double x1u,x2u,x3u,x4u,y1u,y2u,x1d,x2d,x3d,x4d,y1d,y2d;
        double kcontr1,kcontr2;
		int xw,yw;
		                		
		for (j1=1; j1 < goy; j1++)
		{
		   for (i1=1; i1 < gox; i1++)
              {      
            bx1=0;
            if (i1<=1) bx1=(int)(mmconst/2)*type;
            bx2=0;
            if (i1==gox-1) bx2=2*mmconst*type;
            by1=0;
            if (j1<=1) by1=(int)(mmconst/2)*type;
            by2=0;
            if (j1==goy-1) by2=2*mmconst*type;

              
              
           for (xw=0-bx1;xw<mmconst+bx2;xw++)
		   {
		 x=xw;
		 if (x<0) x=0;
         if (x>mmconst) x=mmconst;

		 x1 = simg2[(i1-1)*yms+j1-1];
		 x2 = simg2[i1*yms+j1-1];
		 y1 = (int)(x2-x1)*x/mmconst+x1;
		 
		 x3 = simg2[(i1-1)*yms+j1];
		 x4 = simg2[i1*yms+j1];
		 y2 = (int)(x4-x3)*x/mmconst+x3;

		 if (mflag>0)
	     {
		 x1u = simgkof1[(i1-1)*yms+j1-1];
		 x2u = simgkof1[i1*yms+j1-1];
		 y1u = (x2u-x1u)*(double)x/mmconst+x1u;
		 
		 x3u = simgkof1[(i1-1)*yms+j1];
		 x4u = simgkof1[i1*yms+j1];
		 y2u = (x4u-x3u)*(double)x/mmconst+x3u;

		 x1d = simgkof0[(i1-1)*yms+j1-1];
		 x2d = simgkof0[i1*yms+j1-1];
		 y1d = (x2d-x1d)*(double)x/mmconst+x1d;
		 
		 x3d = simgkof0[(i1-1)*yms+j1];
		 x4d = simgkof0[i1*yms+j1];
		 y2d = (x4d-x3d)*(double)x/mmconst+x3d;


		 }
		 
              for (yw=0-by1;yw<mmconst+by2;yw++)
		      {
			 
		     y=yw;
		     if (y<0) y=0;
             if (y>mmconst) y=mmconst;
		                
            i2 =  (i1-1)*mmconst+(int)(mmconst/2)*type+xw; 
            j2 =  (j1-1)*mmconst+(int)(mmconst/2)*type+yw;
            
            if (i2<cx) 
            if (j2<cy)  
            if (i2>=0) 
            if (j2>=0)  

            {                
              a = (*pimg)[j2][i2*q+qi]-(int)(y2-y1)*y/mmconst-y1+sr;
              a = (int)((double)a*kcontr)-delta;
              if (mflag>0)
			  {
             if (supercontr==0)
			{

			  if (a>sr_even)
			  {
                kcontr1=(y2u-y1u)*(double)y/mmconst+y1u;

                if (kcontr1<1) kcontr1=(double)1;
				if (kcontr1>3) kcontr1=(double)20;

		 		a=sr_even+(int)((double)(a-sr_even)*kcontr1);
			  }
			  else
			  {
			  kcontr1=(y2d-y1d)*(double)y/mmconst+y1d;

              if (kcontr1<1) kcontr1=(double)1;
			  if (kcontr1>3) kcontr1=(double)20;

			  a=sr_even-(int)((double)(sr_even-a)*kcontr1);
			  }

			 }
			 else
			 {
              // контраст в каждой маске без ограничений

				  kcontr2=(y2u-y1u)*(double)y/mmconst+y1u;
				  kcontr1=(y2d-y1d)*(double)y/mmconst+y1d;
				  int minbord=(int)kcontr2-(int)(y2-y1)*y/mmconst-y1+sr;
			      minbord = (int)((double)minbord*kcontr)-delta;
				  a=(int)(abs(a-minbord)*kcontr1);
			 }
			  }



		           if (a>255) a=255;		    
		            if (a<0) a=0;
			    (*pimg)[j2][i2*q+qi]=a;
				                         
		     }  
           }
		 }
		   

		}
        noti=noti+(int)j1*mmconst;	    
		Notify( noti );
		}  
		  
		  
	delete simg0;
	delete simg1;
	delete simg01;
	delete simgs;
	delete simgkof0;
	delete simgkof1;
	delete gist;
	pimgh->Free();
    delete pimgh;





    cheng = AfxGetApp()->GetProfileInt( "Settings", "cheng", 1 );
	AfxGetApp()->WriteProfileInt( "Settings", "cheng", (int)1 );
    if (cheng!=1)
	{
	 EndNotification();
	 delete spectr;
     pimg->Free();
     delete pimg;

	return;
	}





 }//end qi


   
   noti=0;
        if(q==3)
{
            proc=10000;
            for (i3 = 0 ;i3<256;i3++)
						{spectr[i3]=0;}
              a1=q*(cx-1);
			  a2=(cy-1);
               for (j2 = 0;j2<a2;j2++)
			   {
		           prey = (byte*)(pre+j2*iRowMemSize);
                  for (i2 = 0;i2<a1;i2++)		         
		                 spectr[prey[i2]]++;        		                		             		     
                }
		         

             b1 = 0;
			 mincolor=0;
			 maxcolor=255;			 
			 porog=(long)q*(cx)*(cy)/proc;		 
			 if (porog<3) porog=3;

			 for (i3=0;i3<256;i3++)
			 {
               b1 = b1+spectr[i3];
			   if (b1<porog ) mincolor = i3;
			  }   

             b1 = 0;
			 for (i3=0;i3<256;i3++)
			 {
               b1 = b1+spectr[255-i3];
			   if (b1<porog ) maxcolor = 255-i3;
			  }



        kcontr = (double)255/(maxcolor-mincolor);
		if (kcontr <= 0)
		{kcontr = -kcontr;}
		 delta = (int)(((double)mincolor)*kcontr);

            for (j2=0; j2 < cy; j2++)
                {                  
		          prey = (byte*)(pre+j2*iRowMemSize);
		              for (i2=0; i2 < cx*q; i2++)      
                           { 
                             a = (int)(((double)prey[i2])*kcontr)-delta;
					         if (a>255) a=255;		    
		                     if (a<0) a=0;
 			                 prey[i2]=a;      
                           }
                    }

}



 EndNotification();

	((CBINArgument*)pres)->m_lpbi = pimg->GetBI();
	pimg->SetFlag( TRUE );
    delete spectr;
    pimgshow->Free();
    delete pimgshow;
}







/*
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
else
{

	C8Image	*pimg = new C8Image( Hx_max , Hy_max, opt );
	C8Image	*pimgshow = new C8Image( Hx_max , Hy_max, opt );

    int	iRowMemSize = (cx*q+3)/4*4;
	int	iRowMemSizeh = (cx+3)/4*4;

	byte	*pur = (byte*)pimgbin->GetBI();
	pur+=(UINT)((byte*)(pimgbin->GetBI()->biSize+((q==1)?1024:0)));
	byte	*pury;

	byte	*psh = (byte*)pimgshow->GetBI();
	psh+=(UINT)((byte*)(pimgshow->GetBI()->biSize+((q==1)?1024:0)));
	byte	*pshy;


    int i2,j2;

    for (j2=0; j2 < cy; j2++)
	{
			   pury = (byte*)(pur+j2*iRowMemSize);
    		   pshy = (byte*)(psh+j2*iRowMemSize);
       for (i2=0; i2 < cx*q; i2++)
            pshy[i2] = pury[i2];
    }


	((CBINArgument*)pres)->m_lpbi = pimgshow->GetBI();
	pimgshow->SetFlag( TRUE );

	StartNotification( Hy_max*q );

	    int MAX = 0;



	int qi=0;
	int noti;


//  begin registred	     
    int proc;
	proc = AfxGetApp()->GetProfileInt( "Settings", "ContrOut", 500 );
	AfxGetApp()->WriteProfileInt( "Settings", "ContrOut", proc );
	proc = 100;
	int sr;                   
	long b1;
	int a;
	int a1;
	int a2;
	int a3;
	int a4; 
	DWORD sum;
	int i1;
	int j1; 
	int i3;
	int j3;
	int nal;
	int cmin;
	int cmax; 
	int xm;
	int ym;
	int x;
	int y;  
	int x1;
	int y1;
	int x2;
	int y2;  
	int x3;
	int x4;
	int maxcolor;
	int mincolor;

	
	
	int mconst;
    int mask;
	long kr0;
	long kr1;
	long kr2;
//	long kr3;
//    long kr4;
    long porog;
		 
//	int iCol ;
	int xms;
	int yms;
	int *simg0;   
	int *simg1;
	int *simg01;
	int *simg2;
	DWORD *gist = new DWORD[310];   
	DWORD *spectr = new DWORD[300];
	

    double kcontr;
		int delta ;
		int mmconst; 
		int bx1;
		int by1;
		int bx2;
		int by2; 		 

//  end registred	


	 maxcolor=0;
	 mincolor=255;


   double R,G,B,H,L,S;          // input RGB values 
   BYTE cMax,cMin;      // max and min RGB values 
   double  Rdelta,Gdelta,Bdelta; // intermediate value: % of spread from max
   double HLSMAX = 255;
   double RGBMAX = 255;


            double k05 = (double)0.5;
			double k425 = (double)42.5;

   noti=0;
    
	BYTE Rb,Gb,Bb;

	byte	*pre = (byte*)pimg->GetBI();
	pre+=(UINT)((byte*)(pimg->GetBI()->biSize+((q==1)?1024:0)));
	byte	*prey;


  if (q==3)
  {
      
		   for (j2=0; j2 < cy; j2++)
           {
		   pury = (byte*)(pur+j2*iRowMemSize);
		   prey = (byte*)(pre+j2*iRowMemSize);
		   
		   for (i2=0; i2 < cx*3; i2+=3)
            {                                   

 
            Rb = pury[i2+2];
			Gb = pury[i2+1];
            Bb = pury[i2];

			cMax = max( max(Rb,Gb), Bb);   
			cMin = min( min(Rb,Gb), Bb);
            
			prey[i2] = (BYTE)((cMax+cMin)/2);
            prey[i2+1] = prey[i2];


		   }
	  Notify( noti++ );
 
      }				 

  }
  else
  {
    for (j2=0; j2 < cy; j2++)
	{
			   pury = (byte*)(pur+j2*iRowMemSize);
    		   prey = (byte*)(pre+j2*iRowMemSize);
       for (i2=0; i2 < cx; i2++)
            prey[i2] = pury[i2];
    }
  }






	C8Image	*pimgh = new C8Image( Hx_max , Hy_max, 0 );

   	byte	*ph = (byte*)pimgh->GetBI();
	ph+=(UINT)((byte*)(pimgh->GetBI()->biSize+((1==1)?1024:0)));
	byte	*phy;


    for (j2=1; j2 < cy-1; j2++)			   
       for (i2=1; i2 < cx-1; i2++)
	   {
	     sum=0;
         for (j3=j2-1; j3 < j2+2; j3++)			   
            for (i3=i2-1; i3 < i2+2; i3++)
              sum=sum+(*pimg)[j3][i3*q];
          (*pimgh)[j2][i2]=(BYTE)(sum/9);
        }


    cheng = AfxGetApp()->GetProfileInt( "Settings", "cheng", 1 );
	AfxGetApp()->WriteProfileInt( "Settings", "cheng", (int)1 );
    if (cheng!=1)
	{
	 EndNotification();
	 delete gist;
	 delete spectr;
     pimg->Free();
     delete pimg;
   	pimgh->Free();
    delete pimgh;

	return;
	}






	 sr=0;                   
	 b1;
	 a=0;
	 a1=0;
	 a2=0;
	 a3=0;
	 a4=0; 
	 sum=0;
	 i1=0;
	 j1=0;
	 i2=0;
	 j2=0;  
	 i3=0;
	 j3=0;
	 nal=2;
	 cmin=0;
	 cmax=0; 
	 xm=0;
	 ym=0;
	 x=0;
	 y=0;  
	 x1=0;
	 y1=0;
	 x2=0;
	 y2=0;  
	 x3=0;
	 x4=0;
	
	
	 mconst=(int)iConst;


     if (iConst <= 15 )
	 {mconst = (int) (cx+cy)/16;}

   
        i1 = (int)( min(cx,cy)/2);        

        if (mconst>=i1)
        {mconst=i1;}

         i1 = 0;
       

	if (mconst<=11) {mconst=11;}
	
	
	mmconst = (int)mconst/nal; 

	porog = (long)mconst*mconst/proc;
	if (porog<3) porog=3;

	 kr0=0;
	 kr1=0;
	 kr2=0;
	
	mask = (int)mconst/10+1;
	if (mask<=7) 
	{mask=7;}
	 
     
	 xms = (int)cx/mmconst+5;
	 yms = (int)cy/mmconst+5;
	 simg0 = new int[xms*yms+1];   
	 simg1 = new int[xms*yms+1];    
     simg01 = new int[xms*yms+1];    
	 int *simgs = new int[xms*yms+1];
	 double *simgkof0 = new double[xms*yms+1];
	 double *simgkof1 = new double[xms*yms+1];
	
	 noti=0;
	
	
	int gox = (int)(cx/mmconst-0.5)-1;
	int goy = (int)(cy/mmconst-0.5)-1;
			
    	for (j1=0; j1 < goy; j1++)
			for (i1=0; i1 < gox; i1++)
	          {
		     xm = (i1+2)*mmconst;     
		     if (xm >= cx-1) xm = cx-1;		      
		     ym = (j1+2)*mmconst;                                
              if (ym >= cy-1) ym = cy-1;
		     
		     cmax=0;
		     cmin=10000;
//       BEGIN     IN    MASK              
			 for (i2 = 0; i2<300;i2++)
			 gist[i2]=0;

			 sum=0;
			 a1=0;	

			 for (j2 = j1*mmconst+1;j2<ym+1;j2++)
			 {
			    phy = (byte*)(ph+(cy-j2-1)*iRowMemSizeh);
		         for (i2 = i1*mmconst+1;i2<xm+1;i2++)
				 {
						gist[phy[i2]]++;
						sum = sum + phy[i2];
						a1++;
                 }
              }


      // simgs
      
      simgs[i1*yms+j1]= (int)((double)sum/(double)a1);

	  // simq01
			 DWORD a111 = 0;
             for (i2 = 0; i2<84;i2++)
				 if (a111<gist[i2*3+1]+gist[i2*3+2]+gist[i2*3+3])
				       {
					   a111=gist[i2*3+1]+gist[i2*3+2]+gist[i2*3+3];
				       simg01[i1*yms+j1]= i2*3+1;   
					   }
            i2=84; 
            if (a111<gist[i2*3+1]+gist[i2*3+2]+gist[i2*3+2])
				       {
					   a111=gist[i2*3+1]+gist[i2*3+2]+gist[i2*3+2];
				       simg01[i1*yms+j1]= i2*3+1;   
					   }

       // simq0 && simq1

             porog=(long)((double)((xm-i1*mmconst)*(ym-j1*mconst))/(double)proc);
			 if (porog<3) porog=3;
			 b1 = 0;
			 cmin=0;
			 cmax=255;			 
			 for (i3=0;i3<256;i3++)
			 {
               b1 = b1+gist[i3];
			   if (b1<porog ) cmin = i3;
			   else
			   i3=256;
			 }   

             b1 = 0;
			 for (i3=255;i3>=0;i3--)
			 {
               b1 = b1+gist[i3];
			   if (b1<porog ) cmax = i3;
			   else
			   i3=-1;
			 }
           
		   if (cmin>cmax)
		   {cmin=int((cmin+cmax)/2);cmax=cmin;} 

		   simg0[i1*yms+j1]= cmin;   
           simg1[i1*yms+j1]= cmax;      

//       END     IN    MASK              
			 }
//  end first way			 



    cheng = AfxGetApp()->GetProfileInt( "Settings", "cheng", 1 );
	AfxGetApp()->WriteProfileInt( "Settings", "cheng", (int)1 );
    if (cheng!=1)
	{
	 EndNotification();
	 delete spectr;
     pimg->Free();
     delete pimg;
	delete simg0;
	delete simg1;
	delete simg01;
	delete simgs;
	delete simgkof0;
	delete simgkof1;
	delete gist;
	pimgh->Free();
    delete pimgh;




	return;
	}




*/





/*

			 
///////////////////////////////////////////////////////////////
//cheng mask
//int type=1;
    int type=2; 
		   
		kr0 = 0;
		kr1 = 0;  
		kr2 = 0;
		    
	for (i1=0; i1 < gox-1; i1++)
		for (j1=0; j1 < goy-1; j1++)
		{
		    
		    a1 = (int) simg0[(int)(i1*yms+j1)]-simg0[(int)(i1*yms+j1+1)];
		    if (a1<=0 ) a1 = 0-a1;
			kr0=kr0+a1;
		   

            a2 = (int) simg0[(int)(i1*yms+j1)]-simg0[(int)((i1+1)*yms+j1)];
		    if (a2<=0 ) a2 = 0-a2;
            kr0=kr0+a2; 
            
            
		    a3 = (int) simg1[(int)(i1*yms+j1)]-simg1[(int)(i1*yms+j1+1)];
		    if (a3<=0 ) a3 = 0-a3;
            kr1=kr1+a3;

            a4 = (int) simg1[(int)(i1*yms+j1)]-simg1[(int)((i1+1)*yms+j1)];
		    if (a4<=0 ) a4 = 0-a4;
            kr1=kr1+a4; 


             
			a3 = (int) simg01[(int)(i1*yms+j1)]-simg01[(int)(i1*yms+j1+1)];
		    if (a3<=0 ) a3 = 0-a3;
			kr2=kr2+a3;

            a4 = (int) simg01[(int)(i1*yms+j1)]-simg01[(int)((i1+1)*yms+j1)];
		    if (a4<=0 ) a4 = 0-a4;  
			kr2=kr2+a4;


              
      	    };	    
		    

		    
		   simg2 = simg0;
		   a4 = kr0;
		   if (kr1 <= a4)
		   {simg2 = simg1;
		   a4 = kr1;} 
		   if (kr2 <= a4)
		   {simg2 = simg01;
		   a4 = kr2;} 
		                        
		
		
		 bx1=0;
		 by1=0;
		 bx2=0;
		 by2=0; 
		                               
         sum=0;
		 a1=0;

		 for (i1=0; i1 < gox; i1++)
	    	for (j1=0; j1 < goy; j1++)
		    {
		      sum=sum+ simg2[i1*yms+j1]; 
			  a1++;
		    }                          
		 sr=(int)((double)sum/(double)a1);



         sum=0;
		 a1=0;
    	 mincolor=255;
		 maxcolor=0;			 

	
        for (i1=0; i1 < gox; i1++)
	    	for (j1=0; j1 < goy; j1++)
		    {
		      sum=sum+simgs[i1*yms+j1]-simg2[i1*yms+j1]+sr; 
			  a1++;
			  if (mincolor>simg0[i1*yms+j1]-simg2[i1*yms+j1]+sr)
			     mincolor=simg0[i1*yms+j1]-simg2[i1*yms+j1]+sr;
              if (maxcolor<simg1[i1*yms+j1]-simg2[i1*yms+j1]+sr)
			     maxcolor=simg1[i1*yms+j1]-simg2[i1*yms+j1]+sr; 
		    }                          

        if (maxcolor>255) maxcolor=255;
		if (mincolor<0) mincolor=0;

        kcontr = (double)255/(maxcolor-mincolor);
		if (kcontr <= 0) kcontr = -kcontr;
		delta = (int)((double)mincolor*kcontr);

        int sr_even=(int)((double)sum/(double)a1);



        if (supercontr==0)
		sr_even=(int)((double)(sr_even)*kcontr)-delta;
		else
        sr_even=0;


        int dup = 255-sr_even;
		int ddown = sr_even;
         int ddownij=0;
		 int dupij=0;
        for (i1=0; i1 < gox; i1++)
	    	for (j1=0; j1 < goy; j1++)
			{
			ddownij=sr_even-
			(int)((double)(simg0[i1*yms+j1]-simg2[i1*yms+j1]+sr)*kcontr)+delta;
			if (ddownij<1) ddownij=1;

			dupij=(int)((double)(simg1[i1*yms+j1]-simg2[i1*yms+j1]+sr)*kcontr)-delta
			-sr_even;
			if (dupij<1) dupij=1;

			if (supercontr==0)
			{
			simgkof1[i1*yms+j1]=(double)dup/((double)dupij);
			simgkof0[i1*yms+j1]=(double)ddown/((double)ddownij);
			
			if (simgkof1[i1*yms+j1]<1) simgkof1[i1*yms+j1]=(double)1;
			if (simgkof1[i1*yms+j1]>3) simgkof1[i1*yms+j1]=(double)3;
            if (simgkof0[i1*yms+j1]<1) simgkof0[i1*yms+j1]=(double)1;
			if (simgkof0[i1*yms+j1]>3) simgkof0[i1*yms+j1]=(double)3;

			}
			else
			{
			simgkof0[i1*yms+j1]=(double)255/((double)(simg1[i1*yms+j1]-simg0[i1*yms+j1])*kcontr);
			simgkof1[i1*yms+j1]=(double)simg0[i1*yms+j1];
            
			if (simgkof0[i1*yms+j1]<1) simgkof0[i1*yms+j1]=(double)1;
			if (simgkof0[i1*yms+j1]>20) simgkof0[i1*yms+j1]=(double)20;
			}

			}



		  
		 noti=0;       
		
		double x1u,x2u,x3u,x4u,y1u,y2u,x1d,x2d,x3d,x4d,y1d,y2d;
        double kcontr1,kcontr2;
		int xw,yw;
		                		
		for (j1=1; j1 < goy; j1++)
		{
		   for (i1=1; i1 < gox; i1++)
              {      
            bx1=0;
            if (i1<=1) bx1=(int)(mmconst/2)*type;
            bx2=0;
            if (i1==gox-1) bx2=2*mmconst*type;
            by1=0;
            if (j1<=1) by1=(int)(mmconst/2)*type;
            by2=0;
            if (j1==goy-1) by2=2*mmconst*type;

              
              
           for (xw=0-bx1;xw<mmconst+bx2;xw++)
		   {
		 x=xw;
		 if (x<0) x=0;
         if (x>mmconst) x=mmconst;

		 x1 = simg2[(i1-1)*yms+j1-1];
		 x2 = simg2[i1*yms+j1-1];
		 y1 = (int)(x2-x1)*x/mmconst+x1;
		 
		 x3 = simg2[(i1-1)*yms+j1];
		 x4 = simg2[i1*yms+j1];
		 y2 = (int)(x4-x3)*x/mmconst+x3;

		 if (mflag>0)
	     {
		 x1u = simgkof1[(i1-1)*yms+j1-1];
		 x2u = simgkof1[i1*yms+j1-1];
		 y1u = (x2u-x1u)*(double)x/mmconst+x1u;
		 
		 x3u = simgkof1[(i1-1)*yms+j1];
		 x4u = simgkof1[i1*yms+j1];
		 y2u = (x4u-x3u)*(double)x/mmconst+x3u;

		 x1d = simgkof0[(i1-1)*yms+j1-1];
		 x2d = simgkof0[i1*yms+j1-1];
		 y1d = (x2d-x1d)*(double)x/mmconst+x1d;
		 
		 x3d = simgkof0[(i1-1)*yms+j1];
		 x4d = simgkof0[i1*yms+j1];
		 y2d = (x4d-x3d)*(double)x/mmconst+x3d;


		 }
		 
              for (yw=0-by1;yw<mmconst+by2;yw++)
		      {
			 
		     y=yw;
		     if (y<0) y=0;
             if (y>mmconst) y=mmconst;
		                
            i2 =  (i1-1)*mmconst+(int)(mmconst/2)*type+xw; 
            j2 =  (j1-1)*mmconst+(int)(mmconst/2)*type+yw;
            
            if (i2<cx) 
            if (j2<cy)  
            if (i2>=0) 
            if (j2>=0)  

            {                
              a = (*pimg)[j2][i2*q+qi]-(int)(y2-y1)*y/mmconst-y1+sr;
              a = (int)((double)a*kcontr)-delta;
              if (mflag>0)
			  {
            if (supercontr==0)
			{

			  if (a>sr_even)
			  {
                kcontr1=(y2u-y1u)*(double)y/mmconst+y1u;

                if (kcontr1<1) kcontr1=(double)1;
				if (kcontr1>3) kcontr1=(double)3;

		 		a=sr_even+(int)((double)(a-sr_even)*kcontr1);
			  }
			  else
			  {
			  kcontr1=(y2d-y1d)*(double)y/mmconst+y1d;

              if (kcontr1<1) kcontr1=(double)1;
			  if (kcontr1>3) kcontr1=(double)3;

			  a=sr_even-(int)((double)(sr_even-a)*kcontr1);
			  }
			}
			else
			{
				 // контраст в каждой маске без ограничений

  				  kcontr2=(y2u-y1u)*(double)y/mmconst+y1u;
				  kcontr1=(y2d-y1d)*(double)y/mmconst+y1d;
				  int minbord=(int)kcontr2-(int)(y2-y1)*y/mmconst-y1+sr;
			      minbord = (int)((double)minbord*kcontr)-delta;
				  a=(int)(abs(a-minbord)*kcontr1);

			} 

			  }

		           if (a>255) a=255;		    
		            if (a<0) a=0;
			    (*pimg)[j2][i2*q+qi]=a;
				                         
		     }  
           }
		 }
		   

		}
        noti=noti+(int)j1*mmconst;	    
		Notify( noti );
		}  
		  
		  
	delete simg0;
	delete simg1;
	delete simg01;
	delete simgs;
	delete simgkof0;
	delete simgkof1;
	delete gist;
	pimgh->Free();
    delete pimgh;





    cheng = AfxGetApp()->GetProfileInt( "Settings", "cheng", 1 );
	AfxGetApp()->WriteProfileInt( "Settings", "cheng", (int)1 );
    if (cheng!=1)
	{
	 EndNotification();
	 delete spectr;
     pimg->Free();
     delete pimg;

	return;
	}






	 RGBMAX=255;
	 HLSMAX=255;
	 double hue,sat,lum,lum1;
	 double  Magic1,Magic2; 
   
   noti=0;
    if (q==3)
  {      


   double *COS=new double[300];
   for (i1=0;i1<256;i1++)
       COS[i1]=sin(3.1415*(double)i1/255)+0.01;

          for (j2=0; j2 < cy; j2++)
		  {
		   pury = (byte*)(pur+j2*iRowMemSize);
		   prey = (byte*)(pre+j2*iRowMemSize);
		      for (i2=0; i2 < cx*3; i2+=3)
                {                  


            lum = (double)(prey[i2]+delta)/kcontr;			 			 
			 if (lum>255) lum=255;
			 if (lum<0) lum=0;
			
             lum1=(double)(COS[(int)lum]/COS[prey[i2+1]]);

            R = lum-(double)(prey[i2+1]-pury[i2+2])*lum1;
			G = lum-(double)(prey[i2+1]-pury[i2+1])*lum1;
            B = lum-(double)(prey[i2+1]-pury[i2])*lum1;
            
           if (R>255) R=255;if (R<0) R=0;if (G>255) G=255;if (G<0) G=0;
		   if (B>255) B=255;if (B<0) B=0;
               

            prey[i2+2] = (BYTE)R;
			prey[i2+1] = (BYTE)G;
            prey[i2] = (BYTE)B;   


  
	}
	}
	delete COS;
	}
	

        if(q==3)
{
            proc=10000;
            for (i3 = 0 ;i3<256;i3++)
						{spectr[i3]=0;}
              a1=q*(cx-1);
			  a2=(cy-1);
               for (j2 = 0;j2<a2;j2++)
			   {
		           prey = (byte*)(pre+j2*iRowMemSize);
                  for (i2 = 0;i2<a1;i2++)		         
		                 spectr[prey[i2]]++;        		                		             		     
                }
		         

             b1 = 0;
			 mincolor=0;
			 maxcolor=255;			 
			 porog=(long)q*(cx)*(cy)/proc;		 
			 if (porog<3) porog=3;

			 for (i3=0;i3<256;i3++)
			 {
               b1 = b1+spectr[i3];
			   if (b1<porog ) mincolor = i3;
			  }   

             b1 = 0;
			 for (i3=0;i3<256;i3++)
			 {
               b1 = b1+spectr[255-i3];
			   if (b1<porog ) maxcolor = 255-i3;
			  }



        kcontr = (double)255/(maxcolor-mincolor);
		if (kcontr <= 0)
		{kcontr = -kcontr;}
		 delta = (int)(((double)mincolor)*kcontr);

            for (j2=0; j2 < cy; j2++)
                {                  
		          prey = (byte*)(pre+j2*iRowMemSize);
		              for (i2=0; i2 < cx*q; i2++)      
                           { 
                             a = (int)(((double)prey[i2])*kcontr)-delta;
					         if (a>255) a=255;		    
		                     if (a<0) a=0;
 			                 prey[i2]=a;      
                           }
                    }


}


 EndNotification();

	((CBINArgument*)pres)->m_lpbi = pimg->GetBI();
	pimg->SetFlag( TRUE );
    delete spectr;
    pimgshow->Free();
    delete pimgshow;



}


}

  */