#if !defined(__DebugObj_H__)
#define __DebugObj_H__

#if defined(_DEBUG)
struct debugobj {
	struct polygondomain *shortaxis;
	struct polygondomain *smoothshortaxis;
	struct polygondomain *longaxis;
	struct histogramdomain *prof0;
	struct histogramdomain *prof1;
	struct histogramdomain *sprof0;
	struct histogramdomain *sprof1;
	int skel;
	int *nvec;
};
#endif









#endif
