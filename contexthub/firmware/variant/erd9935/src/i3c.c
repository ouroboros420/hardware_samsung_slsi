#include <csp_common.h>
#include <plat/i3c/i3c.h>
#include <util.h>

static const struct I3cCfg mI3cConfigs[] = {
    [0] = {
        .busId          = I3C_CHUB0,
        .sfrBase        = I3C0_BASE_ADDRESS,
        .clkOpenDrain   = 3000000,
        .clkPushPull    = 6000000,
        .xferMode       = I3C_MODE_POLLING,
        .irq            = I3C_CHUB0_IRQn,
    }
};

const struct I3cCfg *getI3cCfg(uint8_t busId)
{
    int i;

    for(i = 0; i < ARRAY_SIZE(mI3cConfigs); i++) {
        if (mI3cConfigs[i].busId == busId)
            return &mI3cConfigs[i];
    }

    return NULL;
}

DECLARE_IRQ_HANDLERS(0);
