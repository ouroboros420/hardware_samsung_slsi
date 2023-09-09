
#if defined(MBAPM_REQUIRED)

#ifndef __MAILBOX_APM_DRV_H__
#define __MAILBOX_APM_DRV_H__

#include <csp_common.h>

typedef void (*mailboxApmRxCB) (void *cookie, void *Rxbuf, int err);

void mailboxApmEnable(void);
int32_t mailboxApmWrite(const void *buf, uint32_t size);
int32_t mailboxApmRead(void *buf, uint32_t size, mailboxRxCB callback,
                       void *cookie);
void mailboxApmWriteEvent(enum mailbox_event event);
int32_t mailboxApmGetSleepReady(void);
void handleApmRxMessage(int irq_num);
void mailboxApm2Chub_IRQHandler(void);
#endif
#endif
