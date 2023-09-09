#if defined(MBAPM_REQUIRED)

#include <csp_os.h>
#include <csp_common.h>
#include <cmsis.h>
#include <mailbox.h>
#include <mailboxDrv.h>
#include <mailboxApm.h>
#include <ipc_hw.h>

int32_t mailboxApmGetSleepReady(void)
{
    return 1;
}

void mailboxApmWaitAck(uint32_t event)
{
    uint32_t intsr1;
    while(1) {
        intsr1 = ipc_hw_read_int_status_reg((void *)MAILBOX_APM_BASE_ADDRESS, IPC_DST_MB1, 0);
        if (intsr1 == 0)
            continue;

        ipc_hw_clear_int_pend_reg((void *)MAILBOX_APM_BASE_ADDRESS, IPC_DST_MB1, 0);
        break;
    }
}

void mailboxApmWriteEvent(enum mailbox_event event)
{
    uint32_t sr0;

#if defined(SUPPORT_RCO_DIVIDING)
    sr0 = event;

    ipc_hw_write_shared_reg((void *)MAILBOX_APM_BASE_ADDRESS, sr0, SR_0);
    ipc_hw_gen_interrupt((void *)MAILBOX_APM_BASE_ADDRESS, IPC_SRC_MB0, 0);
#endif
}

void mailboxApmEnable(void) {
    return;
}

void mailboxAPM_IRQHandler(void) {
    uint32_t intState;
    intState = ipc_hw_read_int_status_reg_all((void *)MAILBOX_APM_BASE_ADDRESS, IPC_DST_MB1);
    ipc_hw_clear_all_int_pend_reg((void *)MAILBOX_APM_BASE_ADDRESS, IPC_DST_MB1);
}
#endif
