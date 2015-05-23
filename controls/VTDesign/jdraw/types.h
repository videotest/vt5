
#ifndef __TYPES_H__
#define __TYPES_H__

enum BorderType { 
	btEdgeNone			= 0,
	btEdgeRaised		= 1, 
	btEdgeSunken		= 2, 
	btEdgeEtched		= 3,
	btEdgeBump			= 4
};

enum FlipType { 
	ftNormal			= 0, 
	ftPositive			= 1, 
	ftNegative			= 2
};

enum PositionType
{
	ptLeft		= 0,
	ptRight		= 1,
	ptTop		= 2,
	ptBottom	= 3
};

enum AlignType{
	atLeft		= 0,
	atCenter	= 1,
	atRight		= 2
};

enum AlignHorizontal{
	ahLeft		= 0,
	ahCenter	= 1,
	ahRight		= 2
};

enum AlignVertical{
	avTop		= 0,
	avCenter	= 1,
	avBottom	= 2
};

enum ArrowType { 
	none				= 0, 
	pureArrow			= 1, 
	openArrow			= 2,
	stealthArrow		= 3, 
	diamondArrow		= 4, 
	ovalArrow			= 5
};

enum SystemType { 
	stDate				= 0, 
	stDateTime			= 1, 
	stPageNum			= 2
};


enum DashStyle {
	DL_SOLID			= 0, 
	DL_DASH				= 1, 
	DL_DOT				= 2, 
	DL_DASHDOT			= 3, 
	DL_DASHDOTDOT		= 4, 
	DL_DASHDOTDOTDOT	= 5, 
	DL_DASH_GAP			= 6, 
	DL_DOT_GAP			= 7, 
	DL_DASHDOT_GAP		= 8, 
	DL_DASHDOTDOT_GAP	= 9, 
	DL_DASHDOTDOTDOT_GAP= 10
}; 

#endif // __TYPES_H__
