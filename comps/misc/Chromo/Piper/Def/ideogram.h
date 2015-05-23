/*
 *       I D E O G R A M . H  --  Header for chromosome ideograms
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
 *  Modifications
 *
 */

#define P 1			/* arm definitions */
#define Q -1
#define P900 1
#define P600 2
#define P300 3

struct chrom_data {			/* data for a chromosome */
	char chrom_id[3];
	int width;
	int bandtype;
	int sat_length;
	int sat_width;
	int sat_grey;
	int p_length;
	int q_length;
	struct band_data *head;
}; 	
struct band_data {			/* data for a chromosome band */
	int bandtype;
	int width;
	int density;
	int length;
	int arm ;
	char chrom_id[3];
	char major_name[3];
	char minor_name[3];
	struct band_data *next, *previous;
};

struct chrom_data *get_ideogram();

