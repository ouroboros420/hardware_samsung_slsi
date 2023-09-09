
#ifndef	__VDIS_INTERFACE_H__
#define	__VDIS_INTERFACE_H__

#define __cplusplus_
#ifdef __cplusplus_
extern "C" {
#endif

#define PROFILE_INITIALIZE			struct timeval ccTime, ppTime; \
									unsigned long timeUs;
#define PROFILE_START				gettimeofday(&ppTime, NULL);
#define PROFILE_END(x)				gettimeofday(&ccTime, NULL); \
									timeUs = (1000000+ccTime.tv_usec-ppTime.tv_usec)%1000000; \
									(x) = (int)(timeUs);

//#define __VDIS_DEBUG_
#ifdef __VDIS_DEBUG_
#define LOG_VS		CLOGD
#else
#define LOG_VS
#endif

#define ALIGNVDIS(value, base) (((value) + (base) - 1) & ~((base) - 1))

namespace android {

struct swVDis_info {
	void *mCtx;
	unsigned int nInWidth, nInHeight, nOutWidth, nOutHeight;
	int bRoll, bTrans, bRotate;
	int nBlankNum, nVDis_Delay, nVDis_Version, nTargetVersion;
	int nFrameNum;
};

//enum {JA,JF,OCR,VE,HLTE,TIZEN,LT033G,LT03LTE};
enum {K3G, KLTE, OCR, VE, KQ};	//2014_VDIS_enum

void *vsInit(int nInWidth, int nInHeight, int nOutWidth, int nOutHeight, int bRoll, int bTrans, int bRotate, int nBlankNum, int nVDIS_Delay, int nVDIS_Version, int nTargetVersion);
void vsParamFree(void *arg);
void VDIS_1_0(void *ptr, char *pSrc[2], char *pDst[2]);
void VDIS_2_0(void *ptr, char *pSrc[2], char *pDst[2], nsecs_t nTimestamp);
void vsSetOffset(void *arg, int nDisplayWidth, int nDisplayHeight, int *nOffset); // JF Preview offset
void vsSetZoomLevel(void *arg, int nZoomLevel);
void vsSetLux(void *arg, int nLux);
nsecs_t vsGetTimeStamp(void *arg);

};		// end namespace android

#ifdef __cplusplus_
}
#endif

#endif
