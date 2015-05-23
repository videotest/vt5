/*
 *      M A C H D E F . H  --  Machine dependant type deffinutions
 *
 *
 *  Written: Clive A Stubbings
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems
 *
 *  Date:    7th October 1986
 *
 *  Modifications
 *
 *	 4 Apr 1989		dcb/SCG		Add SUN definitions
 *	 4 Mar 1987		CAS			Added definition of FOLD
 *
 */
#ifndef MACHDEF_H

#define mgetc getc
#define fbread fread
#define Malloc malloc
#define Calloc calloc
#define Free free

#ifdef OSK
typedef short	GREY;		/* pixel values */
typedef short	COORD;		/* integer coordinate */
typedef short	SMALL;		/* small integer value */

/* #define FOLD 0x80000		/* Processor memory fold */
#endif OSK

#ifdef SUN
typedef short GREY;		/* pixel values */
typedef short COORD;		/* integer coordinate */
typedef short SMALL;		/* small integer value */
#endif SUN

#ifdef VAX
typedef int GREY;		/* pixel values */
typedef int COORD;		/* integer coordinate */
typedef int SMALL;		/* small integer value */
#endif VAX

//#ifdef WIN32
typedef short	GREY;		/* pixel values */
typedef short	COORD;		/* integer coordinate, COORD conflicts with structure 'CCORD' in wincon.h  */
typedef short	SMALL;		/* small integer value */
#undef  stdin
#undef  stdout
#undef  stderr
#define stdin   fStdIn
#define stdout  fStdOut
#define stderr  fStdErr
extern  struct _iobuf *fStdIn;
extern  struct _iobuf *fStdOut;
extern  struct _iobuf *fStdErr;
//#endif WIN32

#define MACHDEF_H
#endif MACHDEF_H

