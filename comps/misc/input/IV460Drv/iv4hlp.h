#if !defined(__IV4Hlp_H__)
#define __IV4Hlp_H__

#if !defined(__IV4_H_INCLUDED__)
#include "iv4.h"
#define __IV4_H_INCLUDED__
#endif
#include "input.h"

int IV4HlpInitializeCamera(unsigned nCamera, LPCTSTR lpIniPath, IV4CameraConfig *pCfg = NULL);
int IV4HlpPostInitCamera(IV4CameraConfig &CamCfg, IDriver2 *pDrv);
BOOL IV4HlpIsMonoCamera(IV4CameraConfig *pCamCfg);
BOOL IV4HlpIsRGBCamera(IV4CameraConfig *pCamCfg);


#endif