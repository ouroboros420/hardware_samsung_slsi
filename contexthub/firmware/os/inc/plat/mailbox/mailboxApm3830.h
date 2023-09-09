#if defined(MBAPM_REQUIRED)

#include <csp_common.h>

#define APM_EVT_RCO_DIVIDING 1

void mailboxApmWaitAck(uint32_t event);
void mailboxApmWriteEvent(enum mailbox_event event);
void mailboxApmEnable(void);
int32_t mailboxApmGetSleepReady(void);
void mailboxAPM_IRQHandler(void);

#endif
