#ifndef __IPC_OTHER_H__
#define __IPC_OTHER_H__

void ipc_abox_init(void);
void mailboxABOX_IRQHandler(void);
void mailboxABOXHandleIRQ(int evt, void *priv);

#endif
