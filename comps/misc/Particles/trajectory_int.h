#ifndef __trajectory_int_h__
#define __trajectory_int_h__

#include "defs.h"

#ifndef com_call
#define com_call		virtual HRESULT __stdcall
#endif //com_call


const char sz_trajectory_type[] = "Trajectory";

// пока просто зарезервируем
interface ITrajectory : public IUnknown
{
	com_call GetObjNum(long * plNum)=0;
	com_call SetObjNum(long lNum)=0;
};

declare_interface(ITrajectory, "05E0BD58-F395-425a-8F3D-92B46A040D2F");

#endif //__trajectory_int_h__