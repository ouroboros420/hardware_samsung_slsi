#include <mailboxDrv.h>


void mailboxApmWriteEvent(enum mailbox_event event);
int32_t mailboxApmGetSleepReady(void);
void mailboxApmReqDvsLvl(uint32_t lvl);
void mailboxApmEnable(void);
void mailboxApmReqDvsFreq(uint32_t targetFrequency);
void mailboxAPM_IRQHandler(void);
