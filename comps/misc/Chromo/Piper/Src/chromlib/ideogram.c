/*
 *       I D E O G R A M . C  --  Draw chromosome ideograms
 *
 *
 *  Written: Smaragda S Georgiou
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems (1988)
 *
 *  Date:    7th October 1988
 *
 *  Modifications:
 *  13 Mar 1989		SEAJ		Made skipword static to avoid name clashes.
 *  31 Oct 1988     SMG			Renamed Idiogram to Ideogram and removed the
 *								case of removing bands from Putband fn.
 *	27 Aug 1988		CAS			Removed i/o -> return status
 *								Changed colours to work better with app luts
 *	27 Aug 1988		CAS/SMG		Hook into application dirs etc
 *	26 Aug 1988		SMG			working and shows chromosomes with reverse shadings
 *								at centromere
 */
#include <stdio.h>
#include <ideogram.h>

#define MAXLINE 256

/* levels of grey colors */
#define OUTLINE 247
static int greycolor [10] ={ -2, -1, 0, 0, 23, 39, 71, 119, 175, 247 };
static int sat_greycolor [10] = { -2, -1, 0, 0, 7, 15, 23, 47, 95, 247 };

/* corner indentations */
static int corners[3] [6] = { 
					{1,0,0,0,0,0},
					{4,2,1,0,0,0},
					{6,3,2,1,0,0}
				};
static int tip_corners[3][3] = {
						{0,1,3},
						{1,2,4},
						{1,2,4}
				};

static int count, sign, skip;

static int satellite ;			/* flag for satellire existence */
static int pix_x,pix_y;			/* coordinates */
static int start;				/* starting position for facilitating the
								   reading of data from the database */		
static float capture ; 			/* temporary value for storage and 
								   manipulation */

static char str1[3],str2[3];	/* temporary arrays for string assignments */

static FILE *debug = NULL;

/*
 * forward declarations of functions
 */
static char *skipword(), *skipband();

/*
 *	I D E O G R A M _ D E B U G  --
 *
 */
ideogram_debug(chan)
FILE *chan;
{
	debug = chan;
}

/*
 *  G E T _ I D E O G R A M ---
 * 			This function reads chromosome data from the given file
 * 			ie. get data lines; compare the first to the entered chromosome
 * 			and if they match, select the band and read rest of the data.
 */
struct chrom_data *
get_ideogram(fp,chromosome,band_type)
FILE *fp;
char chromosome[];
int band_type;
{
	char *b;
	char buffer[MAXLINE];   	
	struct chrom_data *chrom_ptr = NULL;

	while (fgets (buffer,MAXLINE,fp)) {
		buffer[strlen(buffer) - 1] = '\0';	/* buffer line terminator */
		b = buffer;
		if ( *buffer == '\0' ) continue ;	/* skip any empty lines */
    	while ( *b == ' ' ) ++b;			/* skip starting blanks */

		if ( !strncmp (chromosome, b, strlen(chromosome)) ) {
	      /* allocate structure if chromosome found */
		 	if ((chrom_ptr = (struct chrom_data *) 
				Calloc(1, sizeof(struct chrom_data))) == NULL)
					return(NULL);
		    strcpy (chrom_ptr->chrom_id,chromosome);
			sign = P;
			count = 0;	/* a counter for the chromosome arms */
			chrom_ptr->p_length = 0;
			chrom_ptr->q_length = 0;
			select_band (fp,chrom_ptr,band_type,buffer);
			readbands (fp,chrom_ptr,buffer) ;
			break;			/* read no more lines */
		} else {			/* chromosome not found yet, skip chromosome data */
			skipchrom (fp,buffer,chromosome);
		}
	}
	return(chrom_ptr);
}

/*
 * S E L E C T _ B A N D ---
 *
 * 		read the buffer line containing the widths;
 * 		assign the band_type and the width of the required band;
 *      start points are given to indicate the data position;
 *		skip indicates the number of words and spaces to be skipped
 *		in order to move on to the next band
 */	
 
select_band (fp, chrom_ptr,band_type,buffer)
FILE *fp;
struct chrom_data *chrom_ptr;
int band_type;
char buffer[];   	
{	
	 int i;
	 char *ptr;
	 
    fgets (buffer,MAXLINE,fp);             
	buffer[strlen(buffer)-1] = '\0';
			    
	switch (band_type) {
	case P900 :
		chrom_ptr->bandtype = 900;
	 	chrom_ptr->width = atoi (buffer);
	 	start = 4;
		break;
	case P600 :
		chrom_ptr->bandtype = 600;
    	ptr = skipband (buffer,2);
	 	chrom_ptr->width = atoi (ptr);
	 	skip = 2;
	 	start = 14;
		break;
	case P300 :			
		chrom_ptr->bandtype = 300;
	 	ptr = skipband (buffer,3);
	 	chrom_ptr->width = atoi (ptr);
	 	skip = 4;
	 	start = 26;	 	
		break;
	}
  }
  
/*
 * R E A D B A N D S  ---
 *
 *		read data lines and create new nodes making assignments until 
 *		the occurence of a (-1) which indicates that the arm data
 *		have been finished;
 */
readbands (fp,chrom_ptr,buffer)
struct chrom_data *chrom_ptr;
FILE *fp;
char buffer[];   	
{
	char *ptr;
	double atof();
    int firsttime = 1;	
	struct band_data *band_ptr,*pband_ptr;

	while  (fgets(buffer,MAXLINE,fp)) {
		buffer[strlen(buffer)-1] = '\0';
		if (atoi(buffer) == -1)  {
			end_of_band_data (fp,chrom_ptr,band_ptr,buffer);
			if (count == 2)
				break;		/* since both arms'data have been read */
			else 
				continue;	/* read Q arm data */
		}	
		else {	
		 /* allocate space for band_data structure */
			band_ptr = (struct band_data *) Calloc(1, sizeof(struct
						band_data));
			strcpy (band_ptr->chrom_id,chrom_ptr->chrom_id);
          	band_ptr->bandtype = chrom_ptr->bandtype;
          	band_ptr->width = chrom_ptr->width;

          	band_ptr->arm = sign;
			band_ptr->length = ((float) atof(buffer)) + 0.49 ;
	        
		  /* move the ptr to read the density and check for grey values */	
			ptr = buffer + start;
			band_ptr->density = greycheck (band_ptr,atoi(ptr));
			
		  /* move the ptr to read the capture, if any */
		    ptr = buffer + start + 3;	
   			if  (*ptr == ' '){
				/* combine band lines if captions do not exist,
				 * by adding up the band_lengths, free the current node 
				 * and get the next line */
				
				   pband_ptr->length = pband_ptr->length + band_ptr->length;
   			       Free(band_ptr);
				   continue;
 			}
 			else {	
				 /* convert the float number into a string and split */
			       capture = (float) atof(ptr);
			       convert (capture,str1,str2);
		   	       strcpy (band_ptr->major_name,str1);
				   strcpy (band_ptr->minor_name,str2);
	              		
				/* complete the structure data and start off the next node */
 				   if (firsttime==1) {
 					/* reset the head pointer */
	 		           chrom_ptr->head = band_ptr ;
 		           	   band_ptr->previous = band_ptr;
					   firsttime = 0;
  	 			   }
 				   else {
 					 /* if not firsttime */
			 			band_ptr->previous = pband_ptr;
 						pband_ptr->next = band_ptr;
  				   }
			}

			pband_ptr = band_ptr;
		}
	}
	band_ptr = chrom_ptr->head;
	while (band_ptr) {
		if (band_ptr->arm == P)
  			chrom_ptr->p_length = chrom_ptr->p_length + band_ptr->length; 
		if (band_ptr->arm == Q)  		 		
	  		chrom_ptr->q_length = chrom_ptr->q_length + band_ptr->length;
	  	band_ptr = band_ptr->next;
	 }
}

/*
 * E N D _ O F _ B A N D _ D A T A ---
 *
 *		this routine is called twice for each chromosome;
 *		at the end of the P arm, count becomes 1 and thus the check for
 * 		the existence of satellites and the change in the sign;
 *		at the end of the Q arm, count becomes 2, to be used in the
 *		calling function for reading no more lines.
 */
end_of_band_data (fp,chrom_ptr,band_ptr,buffer)
FILE *fp;
struct chrom_data *chrom_ptr;
struct band_data *band_ptr;
char buffer[];
{
	++count;
	if (count == 1 ) {
   		fgets (buffer,MAXLINE,fp);
		if ( (satellite = atoi(buffer)) != -1) 
			read_sat_data (chrom_ptr,buffer);
		sign = Q;
	}
    /* at this point the function returns in the readbands 
     * and continues to get more lines and create more nodes */
}

/*
 * R E A D _ S A T _ D A T A ---
 *
 * 		Satellite data consist of length, width and density;
 *		the database width is not being used because it does not correspond
 *		with the template width; template or chromosome width is used instead;
 */
read_sat_data (chrom_ptr,buffer)
struct chrom_data *chrom_ptr;
char buffer[]; 	
{
	char *ptr;
	
	chrom_ptr->sat_length = atoi (buffer); 
	ptr = skipword (buffer);

	chrom_ptr->sat_width = chrom_ptr->width; 		
		
 /* since the pointer is not used to assign data, skip twice */	
	ptr = skipword (ptr);
	ptr = skipword (ptr);

	if ( atoi (ptr) < -1 )
		chrom_ptr->sat_grey = -1;
	else
		if 	( atoi (ptr) > 7 )	
			chrom_ptr->sat_grey = 7;
		else				
			chrom_ptr->sat_grey = atoi (ptr);
}
			
/*
 * D R A W _ I D E O G R A M  ---
 *		
 *		draws the P arm, then the satellite if it exists, then the Q arm
 */
draw_ideogram (chrom_ptr,x_orig,y_orig,scale)  
struct chrom_data *chrom_ptr;
int x_orig, y_orig;
int scale;
{ 	
	struct band_data *band_ptr;
	int x,y;
	
	x = x_orig ;

	put_bands (chrom_ptr,P,x,y_orig,scale);
 /* draw the satellite if it exists */	
	if ( satellite != -1 )
		draw_sat (x,pix_y+scale*2,chrom_ptr->sat_length,chrom_ptr->sat_width,
		    		chrom_ptr->sat_grey,scale,chrom_ptr->bandtype);
	put_bands (chrom_ptr,Q,x,y_orig-scale,scale);
}

/*
 * P U T _ B A N D S ---
 *
 *		draws the chromosome arm that corresponds to the arm passed 
 *		as an argument, starting at the original y_position;
 */
 put_bands (chrom_ptr,arm,x,y_orig,scale)
 struct chrom_data *chrom_ptr;
 int arm;
 int x,y_orig;
 int scale;
 {
 	int i,l,m,width,bandtype;
	int diff,left_edge,pix_r;
    struct band_data *band_ptr; 	
 	int t = 0;			/* counter to decide for the tips outline */
	int j = 1;			/* counter for the total arm length */
	int indent = 0;		/* number of pixels to indent */
	
 	
	band_ptr = chrom_ptr->head;		
	
 	switch (chrom_ptr->bandtype) {
 		case 900 :
 			bandtype = 1;
 		case 600 :
 			bandtype = 2;
 		case 300 :
 			bandtype = 3;
 		}

 	pix_y = y_orig;

 	while (band_ptr) {
		if  (band_ptr->arm == arm)  {

			left_edge = x - scale * band_ptr->width /2;
			width = band_ptr->width;
			i = 1;					/* counter for each band's length */
			while ( i <= band_ptr->length ) {

			 /* calculate centromere width */				
				if (j < 7) {
					indent = corners[bandtype-1][j-1];
					width = band_ptr->width-indent;
				}
			 /* calculate Q tip width */				
				if ((j >= chrom_ptr->q_length - 2) &&
					 ( j<= chrom_ptr->q_length) && (arm == Q)) {
					indent = tip_corners[bandtype-1][t];
		 			width = band_ptr->width-indent;
 					t++;
				}
			 /* calculate P tip width */				
				if ((j >= chrom_ptr->p_length - 2) && 
					(j<= chrom_ptr->p_length) && (arm == P)) {
					indent = tip_corners[bandtype-1][t];
	 				width = band_ptr->width-indent;
	 				t++;
 				}

			 /* new egdes */
				pix_x = x - scale * width /2;
				pix_r = x + scale * width /2;

				diff = pix_x - left_edge;
				
			 /* case of centromere,tips and bands with the below densities 
			  *  draw half pixels */
				if  ((diff != 0) || (j <7) 
					|| (band_ptr->density == -1) 
					|| (band_ptr->density == -2) ) { 
				
					moveto (pix_x , pix_y);
            		pixbegin (8,width*2,1,scale/2,scale);
						
					for (l=1+indent; l<=width*2+indent; l++) {
						pixel(greycells(band_ptr,l,j));
					}
					pixend();
				 /* draw outline using half pixels for middle centromere */	
					if (j==1) {
						pixbegin (8,2,1,scale/2,scale);
						moveto (pix_x-3*scale/2,pix_y);
						pixel (OUTLINE);
						moveto (pix_r+scale,pix_y);
						pixel (OUTLINE);
						pixend ();
					}
				} else {
					moveto (pix_x , pix_y);
					pixbegin (8,width,1,scale,scale);
					for (l=1; l<=width; l++) {
						pixel(greycells(band_ptr,l,j));
					}
					pixend();
				}				
			 /* draw left and right outline */
				pixbegin (8,2,1,scale,scale);
				moveto (pix_x-scale,pix_y);
				pixel (OUTLINE);
				moveto  (pix_r,pix_y);
				pixel (OUTLINE);
				pixend ();

			 /* draw horizontal outline for tips */
				if (t>2) {
					pixbegin (8,width*2+1,1,scale/2,scale);
					moveto (pix_x-scale/2 , pix_y+scale*arm);
					for ( m=1; m<=width*2+1; m++ )
						pixel (OUTLINE);
					pixend ();
				}
				pix_y = scale * arm + pix_y;
				i++;   
				j++;
			}
		}
		band_ptr = band_ptr->next ;
	}
}

/*
 * D R A W _ S A T ---
 *
 */
draw_sat (x,y,length,width,grey_color,scale,bandtype)
int x,y,length,width,grey_color,scale,bandtype;
{
	int i,l,k,w,m,n;
	int diff,left_edge,pix_r;
	
	int indent = 0;
	
 	switch (bandtype) {
 		case 900 :
 			bandtype = 1;
 		case 600 :
 			bandtype = 2;
 		case 300 :
 			bandtype = 3;
 		}
 /* draw the two separating lines between chromosome body and satellite */ 		

	moveto (x-scale*width/2+scale*width/4,y );
	pixbegin (8,width/2,1,scale,scale);
	for ( m=1; m<=width/2; m++ )
		pixel (OUTLINE);
	pixend ();

   	moveto (x-scale*width/2+scale*width/4,y+scale*2 );
	pixbegin (8,width/2,1,scale,scale);
	for ( n=1; n<=width/2; n++ )
		pixel (OUTLINE);
	pixend ();
   	
    pix_y = y + scale * 4; 
	left_edge = x - scale * width /2;
	i = 1;
	while ( i <= length ) {
		w = width;	
		if  (i <= 3){ 
			indent = tip_corners[bandtype-1][3-i];
		 	w =  width-indent;
		 	if (i==1) {
	 		 /* draw bottom horisontal outline for satellite */	
		 		moveto (x-scale*w/2, pix_y);
				pixbegin (8,w,1,scale,scale);
				for (l=1; l<=w; l++)
					pixel (OUTLINE);
				pixend ();
		 		pix_y = pix_y+scale;
		 	} 
	 	}
	 	else
	 		if (i >= length-2)  {
				indent = tip_corners[bandtype-1][i-(length-2)];
			 	w =  width-indent;
	 	}
		pix_x = x - scale * w /2;
		pix_r = x + scale * w /2;
		diff = pix_x - left_edge;
				
		if  (diff != 0) {
			moveto (pix_x , pix_y);
       		pixbegin (8,w*2,1,scale/2,scale);
						
			for (l=1+indent; l<=w*2+indent; l++) {
				pixel(satel_color(i,l,grey_color));
			}
			pixend();
		} else { 
			moveto (pix_x, pix_y);
			pixbegin (8,width,1,scale,scale);
			for (k=1; k<=width; k++) {
				pixel(satel_color(i,k,grey_color));
			}
			pixend();
		}				
      /* draw left and right outline */
		pixbegin (8,2,1,scale,scale);
		moveto (pix_x-scale,pix_y);
		pixel (OUTLINE);
		moveto  (pix_r,pix_y);
		pixel (OUTLINE);
		pixend ();

		pix_y = scale + pix_y;
		i++;
	}
 /* draw top horisontal outline for satellite */	
	moveto (pix_x,pix_y);
	pixbegin (8,w,1,scale,scale);
	for (l=1; l<=w; l++)
		pixel (OUTLINE);
	pixend ();

}
 
/*
 * G R E Y C E L L S ---
 *
 */
greycells (band_ptr,col,row)
struct band_data *band_ptr;
int col,row;
{
	int x, grey;
	
   	grey = greycolor[band_ptr->density+2];

	if ( grey == -1 ) {
		if (band_ptr->arm == P ) 
			x = ( 3 * row + col - 4) % 4;
		if (band_ptr->arm == Q)
			x = (col + row ) % 4;
		if ( ( x == 0) || ( x == 1) )
		 	grey = 64;
	}
	if ( grey == -2 ) {
		if ( band_ptr->arm == P ) {
			x = ( 2 * row + col - 3 ) % 8;
			if ( (x >= 4) && (x<=7 ) )
				grey = 36;
		}
		if ( band_ptr->arm == Q ) {
			x = (2 * row * band_ptr->arm + col - 1) % 8;
			if ( x < 0 ) 
				x = x + 8;
			if ( (x >= 4) && (x <= 7) )
				grey = 36;
		}	
	}
	return(grey);
}
 
/*
 * S A T E L _ C O L O R ---
 */
satel_color (row,col,density)
int row,col,density;
{
	int		x, grey;
	
	grey = greycolor[density+2];
 /* only one density shading for satellites */	
	if ( grey == -1 ) {
		x = ( 3 * row + col - 4) % 4;
		if ( ( x == 0) || ( x == 1) )
		 	grey = 64;
	}
	return(grey);
} 
 
/*
 *  L A B E L _ I D E O G R A M ---
 *
 *		puts labels and separating lines for different part of chromosomes
 */
label_ideogram (chrom_ptr,x_orig,y_orig,scale)
struct chrom_data *chrom_ptr;
int x_orig,y_orig,scale;
{
	struct band_data *band_ptr,*line();
	char str[2];
	
	pix_x = x_orig - scale * chrom_ptr->width;
	pix_y = y_orig;
	
 /* draw the centromere line first */
	draw_line ();
	
	band_ptr = chrom_ptr->head;
	while ( band_ptr ) {
		band_ptr = line ( band_ptr->major_name[0],band_ptr->arm,band_ptr,
					chrom_ptr,scale);
		if (band_ptr == NULL) break;	
		if (( band_ptr->previous->arm == P) && (band_ptr->arm == Q)) {

    	 /* draw the p-arm line and print 'p' */
			draw_line ();
			moveto ( pix_x-450, (y_orig+pix_y)/2 );	
			tsize (scale*10,scale*10);
			str[0] = 'p';
			str[1] = '\n';
			text (str);
					
			pix_y = y_orig;
		}
	}
 /* draw the q-arm line and print 'q' */	
	draw_line ();
	moveto ( pix_x-450, (y_orig+pix_y)/2 );	
	tsize (scale*10,scale*10);
	str[0] = 'q';
	str[1] = '\n';
	text (str);
}

/*
 * L I N E ---
 *			decides where to put the separating line 
 *			for bands with different caption numbers
 */
struct band_data *
line (l,arm,band_ptr,chrom_ptr,scale)
char l;
int arm;
struct band_data *band_ptr;
struct chrom_data *chrom_ptr;
int scale;
{
	int y_orig;
	char str[2];
	
	y_orig = pix_y;	
	str[0] = l;
	str[1] = '\n';

 /* when in the same caption group, increment y_position */
	while (( band_ptr->major_name[0] == l) && 
			 (band_ptr->arm == arm) && (band_ptr != NULL)) {

		pix_y = pix_y +  scale * band_ptr->length * band_ptr->arm;		
		put_caption (band_ptr,pix_x,pix_y,scale);
		band_ptr = band_ptr->next;
	}
	
 /* include the satellite, if it exists in the P arm */	
	if ( (satellite != -1) && (band_ptr->arm == Q) 
		&& (band_ptr->previous->arm == P) ) 
		pix_y = pix_y + scale * chrom_ptr->sat_length + scale*6;
		
 /* print caption name in the middle of the range */		
	moveto ( pix_x-250, (y_orig+pix_y)/2-scale*5/2 );
	tsize (scale*5,scale*5);
	text (str);
 /* draw a line to separate different caption groups */
	moveto ( pix_x-300, pix_y );
	lineto ( pix_x, pix_y );
	
	return (band_ptr);
}	

/*
 * P U T _ C A P T I O N ---
 */
put_caption ( band_ptr,pix_x,pix_y,scale)
struct band_data *band_ptr;
int pix_x,pix_y,scale;
{
	char string[6];
	 
		if (band_ptr->length >= 4) {
				
			if (band_ptr->arm == P)
				moveto (pix_x -150, 
					pix_y-scale*band_ptr->length );
			else
				moveto (pix_x -150, 
					pix_y+scale*band_ptr->length/2 );
	
			if (band_ptr->minor_name[0] == '\0')
				sprintf (string,"%c",band_ptr->major_name[1]);
			else
				sprintf (string,"%c.%s",band_ptr->major_name[1],band_ptr->minor_name);
			tsize (40,40);		
			text (string);
		}
		if ( ( band_ptr->length < 4) && (band_ptr->previous->length < 4 )
			&& (band_ptr->next != NULL) )
				if (band_ptr->next->length > 4 ) {
					if (band_ptr->arm == P)
						moveto (pix_x -150, 
							pix_y-scale*band_ptr->length );
					else
						moveto (pix_x -150, 
							pix_y+scale*band_ptr->length/2 );
	
					if (band_ptr->minor_name[0] == '\0')
						sprintf (string,"%c",band_ptr->major_name[1]);
					else
						sprintf (string,"%c.%s",band_ptr->major_name[1],band_ptr->minor_name);							
					tsize (40,40);		
					text (string);
				}
}
/*
 * D R A W _ L I N E ---
 *
 */ 
draw_line ()
{
	moveto ( pix_x-450, pix_y );
	lineto ( pix_x, pix_y );
}

/*
 * S K I P W O R D ---
 *			skips a word and the blanks until the next piece of data
 */
static char *
skipword (b)
char *b;
{
	while((*b != NULL) && (*b != ' ') && (*b != '\t'))
				b++;
	while((*b != NULL) && ((*b == ' ') || (*b == '\t')))
				b++;
	return(b);
}   	 

/*
 * S K I P B A N D ---
 *			skipping one or two sets of data to skip one or two bands
 */
char *skipband(b,i)
int i;
char *b;
{
	int j;
	
	for (j=1;j<=i;j++)
		b = skipword (b);

	return (b);
}

/*
 *	S K I P C H R O M ---
 *			skips a complete chromosome's data, by reading data lines
 *			until a terminator is encountered (-1)
 */
skipchrom (fp,b,id)
char *b;
FILE *fp;
char id[];
{
 	int temp =0 ;

/*do {*/
 	while (temp != -1) {
 		fgets (b,MAXLINE,fp);
 		temp = atoi (b);
 	}
  /* read the next line; either -1 or satellite data */	
	fgets (b,MAXLINE,fp);
	temp = atoi (b);
 	do {
		fgets (b,MAXLINE,fp);
 		temp = atoi (b);
	} while (temp != -1); 	
/*
 *	fgets (b,MAXLINE,fp);
 *	fprintf (stdout,"buffer : %s\n",b);
 *	if  ( strncmp ( b,id,strlen(id))== 0 ){
 *		b = b--;
 *		fprintf (stdout,"buffer : %s\n",b);
 *		break;
 *	}
 *}while ( strncmp ( b,id,strlen(id))!= 0 );
 */
 }
 
 	  	
/*
 * F R E E _ I D E O G R A M  ---
 *		 return the pointers to the memory (free) 
 *		 starting from the head pointer
 */
free_ideogram(chrom_ptr)
struct chrom_data *chrom_ptr;
{
	struct band_data *band_ptr,*pband_ptr;
	
	band_ptr = chrom_ptr->head;
	while ( band_ptr->next != NULL ) {
		pband_ptr = band_ptr->next;
		Free(band_ptr);
		band_ptr = pband_ptr;
	}
}

/*
 * G R E Y C H E C K ---
 *			keeps the density in the range of (-2,7)
 */
greycheck (band_ptr,density)
struct band_data *band_ptr;
int density;
{
	if ( density < -2) 
		density = 1;
	if ( density > 7 )
		density = 7 ;
	return ( density );
}

/*
 * C O N V E R T ---
 *			converts a decimal number into two strings;
 *			the first will be the part before the decimal poinrt;
 *			the second, the part after the decimal point, if not zero.
 */
convert (decimal,s1,s2)
float decimal;
char *s1,*s2;
{
	char string[6];
	char *str;
	int i;
	
	sprintf (string,"%5.2f",decimal);
	string[5] = '\0';
    str = string;
		    while (1) {
		     /* copy characters to the first string until 
		      * the '.' or a null character occurs */ 	
		    	if ((*str == '.') || (*str == '\0'))
		    		break;
		    	else {
			    	*s1 = *str;
			    	++str;
		    		++s1;
				}
		    }
		    *s1 = '\0';
		 /* if there was a decimal point, copy the decimal part 
		  * into the second string, excluding the zeroes */
		    if (*str == '.') {
		     	++str;
			  	for (i=0;i<=2;i++) {
		    		if ( *str != '0' ){		
		    			*s2 = *str;
		   		 		++str;
		    			++s2;
		    		}
		    	}
		    }
		   	*s2 = '\0';
}
