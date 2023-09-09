#ifndef _EXYNOS_MAILBOX_H_
#define _EXYNOS_MAILBOX_H_

#include <mailboxDrv.h>

void mailboxEnable(void);
void mailboxWriteEvent(enum mailbox_event event, uint32_t val);
#if defined(MBAPM_REQUIRED)
int32_t mailboxGetPowerMode(void);
void mailboxSetPowerMode(bool bEn);
#endif
#endif
