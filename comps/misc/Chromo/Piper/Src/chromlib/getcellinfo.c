/*
 * getcellinfo.c
 * allocate space for cell information list
 * fill information in list by interacting with operator
 */

#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>

extern FILE *in;
struct cellid  *getinformation()
{
	struct cellid *cellid;
	struct cellplist *p;
	char c;
	int i;

	in = fopen("/dev/tty","r");
	p = (struct cellplist *) Calloc(sizeof(struct cellplist),1);
	p->size = sizeof(struct cellplist);
	cellid = (struct cellid *) makemain(110,NULL,NULL,p,NULL);
	fprintf(stderr,"Please give cell information\n");
	fprintf(stderr,"slide number ?  ");
	fscanf(in,"%s",p->slideid);
	fprintf(stderr,"date of culture ? ");	
	fscanf(in,"%s",p->doc);
	fprintf(stderr,"type of stain ? ");
	fscanf(in,"%s",p->stain);
	while ((c = getc(in)) != '\n')
		;
	fprintf(stderr,"operator's name ? ");
	i=0;
        while ((c = getc(in)) != '\n')
		p->operatorid[i++] = c;
	p->operatorid[i] = '\0';
	fclose(in);
	return(cellid);
}
