#ifndef __CPU_H__
#define __CPU_H__

#if !defined(SEOS)
uint64_t cpuIntsOff(void);
uint64_t cpuIntsOn(void);
void cpuIntsRestore(uint64_t state);
#endif
void cpuRegisterHandler(uint32_t irqNum, void (*handler)(void));

#endif
