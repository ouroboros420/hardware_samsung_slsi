#ifndef _EXYNOS_MAILBOX_H_
#define _EXYNOS_MAILBOX_H_

#include <mailboxDrv.h>

void mailboxEnable(void);
int32_t mailboxRead(void *buf, uint32_t size, mailboxRxCB callback,
                    void *cookie);
int32_t mailboxWrite(const void *buf, uint32_t size);
void mailboxWriteEvent(enum mailbox_event event, uint32_t val);
#if defined(MBAPM_REQUIRED)
int32_t mailboxGetPowerMode(void);
void mailboxSetPowerMode(bool bEn);
#endif
#endif
