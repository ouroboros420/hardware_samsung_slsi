
#if defined(MBAPM_REQUIRED)

#ifndef __MAILBOX_APM_DRV_H__
#define __MAILBOX_APM_DRV_H__

#include <csp_common.h>

typedef void (*mailboxApmRxCB) (void *cookie, void *Rxbuf, int err);

void mailboxApmDrvEnable(void);
int32_t mailboxApmDrvWrite(const void *buf, uint32_t size);
int32_t mailboxApmDrvRead(void *buf, uint32_t size, mailboxRxCB callback,
                       void *cookie);
void mailboxApmDrvWriteEvent(enum mailbox_event event);
int32_t mailboxApmDrvGetSleepReady(void);
void handleApmRxMessage(int irq_num);
#endif
#endif
