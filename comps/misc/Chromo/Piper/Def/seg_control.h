/*
 * seg_control.h	Jim Piper	3 August 1987
 *
 *  (C)  M E D I C A L   R E S E A R C H   C O U N C I L   1987
 *
 * Structure definition for variables used to connect the
 * interactive segmentation stuff.   This should permit easier
 * splitting of the relevant modules into int pieces
 *
 * It is intended to hang this structure from "kcontrol"
 *
 * MODIFICATIONS
 * -------------
 * 24-09-87	jimp	CUT, ENTRY, etc (SI->state definitions) moved here
 * 18-08-87	jimp	Rubber-banding switch
 */

struct seg_interact {
	int initialised;
	int numchroms;	/* the number of CHROMOSOMES displayed */
	int objnum;	/* current object (probably kcont->curr_obj) */
	int eggno;	/* component count */
	int splitnum;	/* component count */
	int axchrom;	/* current chromosome in manual overlap split */
	int state;	/* state of splitting..... */
	int jselect;	/* object for joining */
	int jobject;	/* and another..... */
	struct ivector arc;		/* most recent piece of ... */
	struct object *polyobj;		/* the splitting polygon */
	struct pframe *framewas;	/* when frames are swapped */
	struct chromosome *objl[50];	/* objects */
	struct object *boundlist[50];	/* boundaries */
	int autosplit_toggle;
	int windowstyle;
	int wind_scale;
	int blobs_toggle;
	int numbers_toggle;
	int noise_toggle;
	int rubber_segment;
	int rubber_overlap;
	int rubber_straight;
};

/*
 * kcont->SI->state bit definitions
 */
#define ENTRY 1
#define CUT 2
#define TOUCH	4
#define OVLP	8
#define STRT	16

