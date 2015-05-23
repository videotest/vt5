/*
 *      C H R O M  _ E R R O R        Error exit defined by user
 *
 *
 *  Written: Graham John Page
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems
 *
 *  Date:    5rd March 1987
 *
 *  Modifications
 *
 */

#include <stdio.h>

static int (*chrom_error)() = NULL ;

/*
 *
 *		S E T _ C H R O M _ E X I T
 *
 */
 
set_chrom_exit(exit_fn)
int (*exit_fn)();
{
	chrom_error=exit_fn;
}

/*
 *
 *		C H R O M _ E X I T
 *
 */
 
chrom_exit(errno)
int errno;
{
	if (chrom_error == NULL) {
		exit(errno);
	} else {
		chrom_error(errno);
	}
}
