#ifndef __IPC_OTHER_H__
#define __IPC_OTHER_H__
#if 1//def CIPC_DEF_USER_ABOX
void ipc_abox_init(void);
void mailboxABOX_IRQHandler(void);
void mailboxABOXHandleIRQ(int evt, void *priv);
#else
#define ipc_abox_init(void) (void)0
#define mailboxABOXHandleIRQ(a, b) (void)0
#define mailboxABOX_IRQHandler(void) (void)0
#endif
#endif
