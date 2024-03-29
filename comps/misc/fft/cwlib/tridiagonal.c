/* Copyright (c) Colorado School of Mines, 1995.*/
/* All rights reserved.                       */

/*********************** self documentation **********************/
/*****************************************************************************
TRIDIAGONAL - Functions to solve tridiagonal systems of equations Tu=r for u.

tridif		Solve a tridiagonal system of equations (float version)
tridid		Solve a tridiagonal system of equations (double version)
tripd		Solve a positive definite, symmetric tridiagonal system

******************************************************************************
Function Prototypes:
void tridif (int n, float a[], float b[], float c[], float r[], float u[]);
void tridid (int n, double a[], double b[], double c[], double r[], double u[]);
void tripd (float *d, float *e, float *b, int n);

******************************************************************************
tridif, tridid:
Input:
n		dimension of system
a		array[n] of lower sub-diagonal of T (a[0] ignored)
b		array[n] of diagonal of T
c		array[n] of upper super-diagonal of T (c[n-1] ignored)
r		array[n] of right-hand-side column vector

Output:
u		array[n] of solution (left-hand-side) column vector

*****************************************************************************
tripd:
Input:
d	array[n], the diagonal of A 
e	array[n], the superdiagonal of A
b	array[n], the rhs column vector of Ax=b

Output:
b	b is overwritten with the solution to Ax=b

******************************************************************************
Notes:
For example, a tridiagonal system of dimension 4 is specified as:

    |b[0]    c[0]     0       0  | |u[0]|     |r[0]|
    |a[1]    b[1]    c[1]     0  | |u[1]|  =  |r[1]|
    | 0      a[2]    b[2]    c[2]| |u[2]|     |r[2]|
    | 0       0      a[3]    b[3]| |u[3]|     |r[3]|

The tridiagonal matrix is assumed to be non-singular.

tripd:
Given an n-by-n symmetric, tridiagonal, positive definite matrix A and
 n-vector b, following algorithm overwrites b with the solution to Ax = b

******************************************************************************
Authors:  tridif, tridid: Dave Hale, Colorado School of Mines, 10/03/89
tripd: Zhenyue Liu, Colorado School of Mines, 1993
*****************************************************************************/
/**************** end self doc ********************************/

#include "cwp.h"

void tridif (int n, float a[], float b[], float c[], float r[], float u[])
/*****************************************************************************
Solve a tridiagonal linear system of equations Tu=r for u
(float version)
******************************************************************************
Input:
n		dimension of system
a		array[n] of lower sub-diagonal of T (a[0] ignored)
b		array[n] of diagonal of T
c		array[n] of upper super-diagonal of T (c[n-1] ignored)
r		array[n] of right-hand-side column vector

Output:
u		array[n] of solution (left-hand-side) column vector
******************************************************************************
Notes:
For example, a tridiagonal system of dimension 4 is specified as:

    |b[0]    c[0]     0       0  | |u[0]|     |r[0]|
    |a[1]    b[1]    c[1]     0  | |u[1]|  =  |r[1]|
    | 0      a[2]    b[2]    c[2]| |u[2]|     |r[2]|
    | 0       0      a[3]    b[3]| |u[3]|     |r[3]|

The tridiagonal matrix is assumed to be non-singular.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 10/03/89
*****************************************************************************/
{
	int j;
	float t,*w;
	
	w = (float*)malloc(n*sizeof(float));
	t = b[0];
	u[0] = r[0]/t;
	for (j=1; j<n; j++) {
		w[j] = c[j-1]/t;
		t = b[j]-a[j]*w[j];
		u[j] = (r[j]-a[j]*u[j-1])/t;
	}
	for (j=n-2; j>=0; j--)
		u[j] -= w[j+1]*u[j+1];
	free(w);
}

void tridid (int n, double a[], double b[], double c[], double r[], double u[])
/*****************************************************************************
Solve a tridiagonal linear system of equations Tu=r for u
(double version)
******************************************************************************
Input:
n		dimension of system
a		array[n] of lower sub-diagonal of T (a[0] ignored)
b		array[n] of diagonal of T
c		array[n] of upper super-diagonal of T (c[n-1] ignored)
r		array[n] of right-hand-side column vector

Output:
u		array[n] of solution (left-hand-side) column vector
******************************************************************************
Notes:
For example, a tridiagonal system of dimension 4 is specified as:

    |b[0]    c[0]     0       0  | |u[0]|     |r[0]|
    |a[1]    b[1]    c[1]     0  | |u[1]|  =  |r[1]|
    | 0      a[2]    b[2]    c[2]| |u[2]|     |r[2]|
    | 0       0      a[3]    b[3]| |u[3]|     |r[3]|

The tridiagonal matrix is assumed to be non-singular.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 10/03/89
*****************************************************************************/
{
	int j;
	double t,*w;
	
	w = (double*)malloc(n*sizeof(double));
	t = b[0];
	u[0] = r[0]/t;
	for (j=1; j<n; j++) {
		w[j] = c[j-1]/t;
		t = b[j]-a[j]*w[j];
		u[j] = (r[j]-a[j]*u[j-1])/t;
	}
	for (j=n-2; j>=0; j--)
		u[j] -= w[j+1]*u[j+1];
	free(w);
}

void tripd(float *d, float *e, float *b, int n)
/*****************************************************************************
Given an n-by-n symmetric, tridiagonal, positive definite matrix A and
 n-vector b, following algorithm overwrites b with the solution to Ax = b

*****************************************************************************
Input:
d[]	the diagonal of A 
e[]	the superdiagonal of A
b[]	the rhs of Ax=b

Output:
b[]	b[] is overwritten with the solution to Ax=b

*****************************************************************************
Notes:

Given an n-by-n symmetric, tridiagonal, positive definite matrix A and
 n-vector b, following algorithm overwrites b with the solution to Ax = b

*****************************************************************************
Author: Zhenyue Liu, Colorado School of Mines, 1993.
*****************************************************************************/
{
	int k; 
	float temp;
	
	/* decomposition */
	for(k=1; k<n; ++k){
           temp = e[k-1];
           e[k-1] = temp/d[k-1];
           d[k] -= temp*e[k-1];
	}

	/* substitution	*/
        for(k=1; k<n; ++k)  b[k] -= e[k-1]*b[k-1];
	
        b[n-1] /=d[n-1];
        for(k=n-1; k>0; --k)  b[k-1] = b[k-1]/d[k-1] - e[k-1]*b[k]; 
	
 }
