#pragma once

#define MIN_WORKSMODE	0
#define MAX_WORKSMODE	1	

#define IS_WORKSMODE(mode)	(mode >= MIN_WORKSMODE) && (mode <= MAX_WORKSMODE)

#define WKM_MEASUSER	0	// works with measurements users params
#define	WKM_STATUSER	1	// works with statistics users params