/*
 *      M O V E . C  --  Basic object moving stuff
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
 *	Copyright (c) and intellectual property rights Image Recognition Systems (1988)
 *
 *  Date:    15th August 1988
 *
 *  Modifications
 *
 */
#include <wstruct.h>

/*
 *	M O V E I D O M  --
 *
 */
moveidom(idom,x,y)
struct intervaldomain *idom;
int		x,y;
{
	idom->kol1 += x;
	idom->lastkl += x;
	idom->line1 += y;
	idom->lastln += y;
}

/*
 *	M O V E V D O M  --
 *
 */
movevdom(vdom,x,y)
struct	valuetable *vdom;
int		x, y;
{
	int		height;
	struct	valueline **vline;
	height = vdom->lastln - vdom->line1 + 1;
	vdom->line1 += y;
	vdom->lastln += y;
	switch (vdom->type) {
		case 1:
			vline = vdom->vtblines;
			while (height--) {
				(*vline)->vkol1 += x;
				(*vline)->vlastkl += x;
				vline++;
			}
			break;
		default:
			break;
	}
}

