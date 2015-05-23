/*
 *      W O O L Z _ E R R O R        Error exit defined by user
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
 *  Date:    3rd March 1987
 *
 *  Modifications
 *
 *	 1 Dec 1988		CAS		woolz_exit really exits if no user routine setup
 *							Can't use woolzsig generally - most programs don't
 *							know aboutit..
 *	22 Nov 1988		dcb		woolz_exit now has an additional parameter to
 *							indicate where it was called from. Note that:
 *							1) woolz_exit must be called with this parameter
 *							2) The woolz exit function must also cope
 *	18 Nov 1988		SCG		modified woolz_exit to return the error status
 *	18 Nov 1988		dcb		woolz_exit() outputs message string, & kills process
 *							with signal WOOLZSIG, instead of just exiting.
 */

#include <stdio.h>
# include <wtcheck.h>

static int (*woolz_error)() = NULL ;

/*
 *
 *		S E T _ W O O L Z _ E X I T
 *
 */
set_woolz_exit(exit_fn)
int (*exit_fn)();
{
	woolz_error=exit_fn;
}

/*
 *
 *		W O O L Z _ E X I T
 *
 */
woolz_exit(errno, called_from)
int errno;
char *called_from;
{
	if (woolz_error == NULL) {
		fprintf(stdout, "Woolz error %s, called from\n",
			woolz_err_msg(errno), called_from);
		exit(errno);
	} else {
		return(woolz_error(errno, called_from));
	}
}

