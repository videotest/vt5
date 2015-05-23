#include "StdAfx.h"
#include <stdio.h>
#include <math.h>

void mfsd (double *a,int n,double eps,int *ier);
/*
 * parameters :
 *	a	upper triangular part of given symmetric positive definite
 *		n by n coefficient matrix.  On return, contains the resultant
 *		upper triangular matrix.  Both are stored column-wise.
 *	n	matrix dimension
 *	eps	an input constant which is used as relative tolerance for
 *		test on loss of significance.
 *	ier	resulting error parameter coded as follows :
 *			0	no error
 *			-1	no result because of wrong input parameter n
 *				or because some radicand is non-positive
 *				(matrix a is not positive definite, possibly
 *				due to loss of significance)
 *			k	warning which indicates loss of significance.
 *				The radicand formed at factorization step
 *				k+1 was still positive but no longer greater
 *				than abs(eps*a(k+1,k+1)).
 *	sqrtdet	the square-root of the determinant of a.
 * remarks:
 *	The procedure gives a result if n and all calculated radicands
 *	are positive.
 * method :
 *	solution using factorization by function mfsd.
 */
void sinv(double *a,int n,double eps,int *ier,double *sqrtdet)
{
	double din, work;
	int i,j,k,l,lhor,lver,min,kend,lanf,ipiv,ind;
	/*
	 * factorize a 
	 * a = transpose(t) * t
	 */
	mfsd(a,n,eps,ier);
	if (*ier < 0)
		return;
	/*
	 * compute square root of determinant as product of
	 * diagonal elements of "a"
	 */
	*sqrtdet = 1.0;
	k = 0;
	for (i=1; i<=n; i++) {
		*sqrtdet *= a[k];
		k += i+1;
	}
	/*
	 * invert upper triangular matrix t
	 * prepare inversion loop
	 */
	ipiv = n * (n+1)/2;
	ind = ipiv;
	/*
	 * initialise inversion loop
	 */
	for (i=1; i<=n; i++) {
		din = 1.0/a[ipiv-1];
		a[ipiv-1] = din;
		min = n;
		kend = i-1;
		lanf = n - kend;
		if (kend > 0) {
			j = ind;
			/*
		 	* initialise row loop
		 	*/
			for (k=1; k<=kend; k++) {
				work = 0.0;
				min--;
				lhor = ipiv;
				lver = j;
				/*
			 	* start inner loop
			 	*/
				for (l=lanf; l<=min; l++) {
					lver++;
					lhor += l;
					work += (a[lver-1] * a[lhor-1]);
				}
				a[j-1] = -work*din;
				j -= min;
			}
		}
		ipiv -= min;
		ind--;
	}
	/*
	 * calculate inverse(a) by means of inverse(t)
	 * inverse(a) = inverse(t) * transpose(inverse(t))
	 * initialise multiplication loop
	 */
	for (i=1; i<=n; i++) {
		ipiv += i;
		j = ipiv;
		/* initialise the row loop */
		for (k=i; k<=n; k++) {
			work = 0.0;
			lhor = j;
			/* start inner loop */
			for (l=k; l<=n; l++) {
				lver = lhor + k - i;
				work += (a[lhor-1] * a[lver-1]);
				lhor += l;
			}
			a[j-1] = work;
			j += k;
		}
	}
}





/*
 * factor a symmetric positive definite matrix.
 * parameters:
 *	a	upper triangular part of the given symmetric positive
 *		definite matrix, dimension n, stored columnwise.  On
 *		return contains the resultant upper trianglar matrix.
 *	n	dimension of matrix
 *	eps	an input constant used as relative tolerance on loss
 *		of significance
 *	ier	resulting error parameter coded as follows :
 *			0	no error
 *			-1	no result because of wrong input parameter n
 *				or because some radicand is non-positive
 *				(matrix a is not positive definite, possibly
 *				due to loss of significance)
 *			k	warning which indicates loss of significance.
 *				The radicand formed at factorization step
 *				k+1 was still positive but no longer greater
 *				than abs(eps*a(k+1,k+1)).
 * remarks:
 *	The procedure gives a result if n and all calculated radicands
 *	are positive.
 *	the product of the returned diagonal terms is equal to the
 *	square-root of the determinant of the given matrix.
 * method:
 *	solution is done using the square-root method of Cholesky.
 *	The given matrix is represented as a product of two triangular
 *	matrices, where the left hand factor is the transpose of the
 *	returned right hand factor.
 */
void mfsd (double *a,int n,double eps,int *ier)
{
	double dpiv,dsum,tol;
	int i,k,l,ind,lend,lanf,lind,kpiv;
	/* test input parameter n */
	if (n<1) {
		*ier = -1;
		return;
	} else
		*ier = 0;

	/* initialise diagonal loop */
	kpiv = 0;
	for (k=1; k<=n; k++) {
		kpiv += k;
		ind = kpiv;
		lend = k-1;

		/* calculate tolerance */
		tol = eps * a[kpiv-1];
		if (tol < 0.0)
			tol = -tol;

		/* start factorization loop over k-th row */
		for (i=k; i<=n; i++) {
			dsum = 0.0;
			if (lend != 0) {
				/* start inner loop */
				for (l=1; l<=lend; l++) {
					lanf = kpiv - l;
					lind = ind - l;
					dsum += (a[lanf-1] * a[lind-1]);
				}
			}
			/* transform element a(ind) */
			dsum = a[ind-1] - dsum;
			if (i == k) {
				/*
				 * test for negative pivot element and
				 * for loss of significance
				 */
				if (dsum <= tol) {
					if (dsum <= 0.0) {
						*ier = -1;
						return;
					} else if (*ier <= 0)
						*ier = k-1;
				}
				/* compute pivot element */
				dpiv = sqrt(dsum);
				a[kpiv-1] = dpiv;
				dpiv = 1.0/dpiv;
			} else
				a[ind-1] = dsum*dpiv;
			ind += i;
		}
	}
}
