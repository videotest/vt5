/*
 *      W T C K L I M S . H  --  woolz checker test limits
 *
 *
 *  Written: Bruce D Pinnington
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems
 *
 *  Date:    5th November 1987
 *
 *	The following constants are those used by the woolz object check routine
 *	to verify likely acceptable ranges. These may need to be tuned with
 *	experience to prevent incorrect exceptions being raised.
 *
 * Modifications:
 *
 *	12 Aug 1988		CAS		Reset line limits to be less tight
 *	17 Nov 1987		BDP		Added new chord length check 
 *	12 Nov 1987		BDP		Changed pdom vertex count max to 4k cos of effects
 *							of iuspace
 *
 */
#define CKIDMNLCMAX		100				/* idom linkcount maximum   */
#define CKPDMNVXMAX		4096			/* polygon max vertex count */
#define CKVDMNLCMAX		100				/* vdom linkcount maximum	*/
#define CKCDMNCHMAX		1024			/* cvhdom max no of chords	*/
#define CKCHRDLENMAX	8 *	1024		/* max chord length, scaled by 8*/
#define CKHDMNPTMAX		1024			/* hdom max no of points		*/
#define CKTDMNSTMAX		1024			/* max string length for tdom	*/
#define CKLINEMIN		-2048			/* min value for line coords	*/
#define CKLINEMAX		2048			/* max value for line coords	*/
#define CKKOLMIN		-2048			/* min column coordinate value	*/
#define CKKOLMAX		2048			/* max column coordinate value	*/
#define CKLINEFLMIN		-2048.0			/* min line for floating pt coords	*/
#define CKLINEFLMAX		2048.0			/* max line for floating pt coords	*/
#define CKKOLFLMIN		-2048.0			/* min column fp coordinate	*/
#define CKKOLFLMAX		2048.0			/* max column fp coordinate */


