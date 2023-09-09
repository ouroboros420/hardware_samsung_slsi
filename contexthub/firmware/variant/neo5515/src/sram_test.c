#include <csp_common.h>
#include <csp_printf.h>

unsigned int pattern[] = {
    0x00000000,
    0x0000FFFF,
    0x00FF00FF,
    0x0F0F0F0F,
    0x33333333,
    0x55555555,
    0xFFFFFFFF
};

static int march_test(unsigned int start, int size, unsigned int pat)
{
    unsigned int wr, rd;
    int i;

    // Increase
    wr = pat;
    for (i = 0 ; i < size ; i += 4) {
        *(unsigned int*)(start + i) = wr;
        wr = ~wr;
    }

    for (i = 0 ; i < size ; i += 4) {
        rd = *(unsigned int*)(start + i);
        if (rd != wr) {
            CSP_PRINTF_INFO("CHUB sram test I FAIL (0x%08x : 0x%08x --> 0x%08x)\n", start + i, wr, rd);
            return -1;
        }
        wr = ~wr;
    }

    // Decrease
    wr = pat;
    for (i = size - 4 ; i >= 0 ; i -= 4) {
        *(unsigned int*)(start + i) = wr;
        wr = ~wr;
    }

    for (i = size - 4 ; i >= 0 ; i -= 4) {
        rd = *(unsigned int*)(start + i);
        if (rd != wr) {
            CSP_PRINTF_INFO("CHUB sram test D FAIL (0x%08x : 0x%08x --> 0x%08x)\n", start + i, wr, rd);
            return -1;
        }
        wr = ~wr;
    }

    return 0;
}

int sram_test(unsigned int start, unsigned int size);
int sram_test(unsigned int start, unsigned int size)
{
    int ret;
    unsigned int i;

    CSP_PRINTF_INFO("chub sram test: 0x%x %d\n", start, size);
    for (i = 0 ; i < sizeof(pattern)/sizeof(pattern[0]) ; i++) {
        ret = march_test(start, size, pattern[i]);
        if (ret) {
            return ret;
        }
    }

    return 0;
}
