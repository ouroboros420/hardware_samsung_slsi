#include <csp_common.h>
#include <cpu.h>
#include <cmsis.h>
#include <plat/cpu/cpu.h>

#if !defined(SEOS)
uint64_t cpuIntsOff(void)
{
    uint32_t state;

    asm volatile (
        "mrs %0, PRIMASK    \n"
        "cpsid i            \n"
        :"=r"(state)
    );

    return state;
}

uint64_t cpuIntsOn(void)
{
    uint32_t state;

    asm volatile (
        "mrs %0, PRIMASK    \n"
        "cpsie i            \n"
        :"=r"(state)
    );

    return state;
}

void cpuIntsRestore(uint64_t state)
{

    asm volatile(
        "msr PRIMASK, %0   \n"
        ::"r"((uint32_t)state)
    );
}
#endif

void cpuRegisterHandler(uint32_t irqNum, void (*handler)(void))
{
    uint32_t vectorBase = SCB->VTOR;

    vectorBase = vectorBase + 0x40 + (irqNum * 4);
    if((uint32_t)handler % 2)
        *(uint32_t *)vectorBase = (uint32_t)handler;
    else
        *(uint32_t *)vectorBase = (uint32_t)handler + 1;
}
