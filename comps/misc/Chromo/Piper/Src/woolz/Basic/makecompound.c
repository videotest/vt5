/*
 * makecompound.c		Jim Piper		6 December 1988
 *
 * make compound objects
 *
 * MODIFICATIONS
 * -------------
 * 07-11-90 jimp	Bug-fix - must initialise co->o in modes 1,3.
 * 23-10-90 jimp	Object linkcounts not yet in IRS system -
 *			on #ifdef OBJECT_LINKCOUNTS
 * 23-10-90 jimp	Inconsistency between this file and wstruct.h,
 *			confusing types 80 and 81, corrected.
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * allocate a struct compounda.  Various mode-switched behaviour:
 * mode==1:	Allocate empty array space for n objects.
 * mode==2:	Array is input parameter ol; link.
 * mode==3:	Array is input parameter ol; allocate space and copy,
 *		incrementing object linkcounts.
 */
struct compounda *makecompounda(type,mode,n,ol,otype)
int type, mode, n, otype;
struct object **ol;
{
	struct compounda *co, *Calloc();
	int i;

	if (type != 80 && type != 81) {
		fprintf(stderr,"Makecompounda: illegal type %d\n",type);
		return(NULL);
	}

	/*
	 * If appropriate, check objects have suitable type
	 */
	if (type == 80) {
		switch(mode) {
			case 2:
			case 3:
				for (i=0; i<n; i++) {
					if (ol[i] != NULL && ol[i]->type != otype) {
						fprintf(stderr,"Makecompounda: illegal otype %d (%d)\n",ol[i]->type,otype);
						return(NULL);
					}
				}
				break;
			default:
				break;
		}
	}

	/*
	 * Allocate space and fill if appropriate
	 */
	switch (mode) {
		default:
		case 1:
			co = Calloc(1,sizeof(struct compounda)+n*sizeof(struct object *));
			co->o = (struct object **) (co+1);
			break;
		case 2:
			co = Calloc(1,sizeof(struct compounda));
			co->o = ol;
			break;
		case 3:
			co = Calloc(1,sizeof(struct compounda)+n*sizeof(struct object *));
			co->o = (struct object **) (co+1);
			for (i=0; i<n; i++) {
				co->o[i] = ol[i];
#ifdef OBJECT_LINKCOUNTS
				if (ol[i] != NULL)
					ol[i]->linkcount++;
#endif OBJECT_LINKCOUNTS
			}
			break;
	}
	co->n = n;
	co->type = type;
	if (type == 80)
		co->otype = otype;
	co->linkcount = 1;
	return(co);
}
