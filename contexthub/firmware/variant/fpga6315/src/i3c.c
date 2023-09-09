

#include <csp_common.h>
#include <i3c.h>
#include <util.h>

static const struct I3cCfg mI3cConfigs[] = {
    [0] = {
        .sfrBase    = I3C_CMGP_BASE_ADDRESS,
        .speed      = 12500000,
        .xferMode   = I3C_MODE_POLLING,
        .irq        = SYSREG1_CMGP_IRQn,
    }
};

const struct I3cCfg *getI3cCfg(uint8_t busId)
{
    if (busId >= ARRAY_SIZE(mI3cConfigs))
        return NULL;

    return &mI3cConfigs[busId];
}

DECLARE_IRQ_HANDLERS(0);
