

#ifndef __MAILBOX_GNSS_DRV_H__
#define __MAILBOX_GNSS_DRV_H__

#include <csp_common.h>

#if defined(MBGNSS_REQUIRED)
void mailboxGnssDrvGenInterupt(unsigned int event);
void mailboxGnssDrvClrInterupt(unsigned int event);
void mailboxGnssDrvhandleIrq(unsigned int event);
void mailboxGnssDrvEnable(void);
void mailboxGnssDrvSetIsr(int event, void *isr);
void mailboxGnssDrvClrIsr(int event);
void mailboxGnssDrvMaskIrq(int irq);
void mailboxGnssDrvUnmaskIrq(int irq);
#endif

#endif

