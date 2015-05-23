#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"
#include "measure5.h"
#include "chromosome.h"
#include "dpoint.h"

class IErectCC: public ComObjectBase, public IChromoConvert
{
public:
	//construction/destruction
	IErectCC();
	virtual ~IErectCC();

	route_unknown();
	virtual IUnknown *DoGetInterface( const IID &iid );

	// During draw operation
	com_call CCGetAxisSize(IChromosome *pChromo, int nFlags /*ChromoAxisType*/, long *plSize, BOOL *pbIsManual);
	com_call CCGetAxis(IChromosome *pChromo, int nFlags /*ChromoAxisType*/, POINT *pAxisPoints);
	com_call CCGetCentromereCoord(IChromosome *pChromo, int nFlags/*ChromoAxisType*/, POINT *pptCen, POINT *pptDraw, BOOL *pbIsManual); // pptDraw can be NULL
	// During IChromosome::GetRotatedImage 
	com_call CCGetImage(IChromosome *pChromo, IUnknown **ppunk);
	com_call Recalc(IChromosome *pChromo);
	com_call Invalidate();

	IChromoConvertPtr m_sptrPrevCC;

private:
	IImage3Ptr m_img;
	_ptr_t2<_dpoint> m_faxis;
	_ptr_t2<_dpoint> m_faxis_long;
	_point m_ptCen;
	_point m_ptDraw[2];
};

//declare_interface(IErectCC, "5974F378-0CC4-4e84-B58E-807272214CBC")
