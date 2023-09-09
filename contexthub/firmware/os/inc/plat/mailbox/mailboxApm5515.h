#include <mailboxDrv.h>

#define APM_MSG_CM55_RESET_RELEASE (0x1 << 1)
#define APM_MSG_DPU_ON             (0x1 << 2)
#define APM_MSG_DPU_OFF            (0x0 << 2)
#define APM_MSG_GPIO_RETENTION_ON  (0x4)
#define APM_MSG_GPIO_RETENTION_OFF (0x5)
#define APM_MSG_MIF_ONLY_ON        (0x6)
#define APM_MSG_MIF_ONLY_OFF       (0x7)
#define APM_RESP_ERROR       (0xF)

struct APMOps {
    void (*resetRelease)(void);
    void (*dramOn)(void);
    void (*dramOff)(void);
    void (*dpuOn)(void);
    void (*dpuOff)(void);
    void (*gpioRetentionOn)(void);
    void (*gpioRetentionOff)(void);
    void (*mifOn)(void);
    void (*mifOff)(void);
    void (*error)(void);
};

typedef void (*mailboxApmRxCB) (void *cookie, void *Rxbuf, int err);

void mailboxApmEnable(void);
void mailboxApmWriteEvent(enum mailbox_event event);
void mailboxApmMIFEnable(bool enableDRAM);
void mailboxApmMIFDisable(bool enableDRAM);
void mailboxApmCM55ResetRelease(void);
void mailboxApmDPUEnable(void);
void mailboxApmDPUDisable(void);
void mailboxApmGPIORetentionEnable(void);
void mailboxApmGPIORetentionDisable(void);
void mailboxApmCM55ResetRelease(void);
int32_t mailboxApmGetSleepReady(void);
void mailboxApmSetOps(struct APMOps *p);
void mailboxAPMSetTimerHandler(void (*handler)(void));
void mailboxAPM_IRQHandler(void);
