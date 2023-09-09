
struct BlVecTable {
    /* cortex requirements */
    uint32_t    blStackTop;
    void        (*blEntry)(void);
    void        (*blNmiHandler)(void);
    void        (*blHardFaultHandler)(void);
    void        (*blMmuFaultHandler)(void);
    void        (*blBusFaultHandler)(void);
    void        (*blUsageFaultHandler)(void);
    void        (*reservedHandler7)(void);
    void        (*reservedHandler8)(void);
    void        (*reservedHandler9)(void);
    void        (*reservedHandler10)(void);
    void        (*blSvcCallHandler)(void);
    void        (*reservedHandler12)(void);
    void        (*reservedHandler13)(void);
    void        (*blPendSvHandler)(void);
    void        (*blSysTickHandler)(void);

    /* soc requirements */
    void        (*isr0)(void);
    void        (*isr1)(void);
    void        (*isr2)(void);
    void        (*isr3)(void);
    void        (*isr4)(void);
    void        (*isr5)(void);
    void        (*isr6)(void);
    void        (*isr7)(void);
    void        (*isr8)(void);
    void        (*isr9)(void);
    void        (*isr10)(void);
    void        (*isr11)(void);
    void        (*isr12)(void);
    void        (*isr13)(void);
    void        (*isr14)(void);
    void        (*isr15)(void);
    void        (*isr16)(void);
    void        (*isr17)(void);
    void        (*isr18)(void);
    void        (*isr19)(void);
    void        (*isr20)(void);
    void        (*isr21)(void);
    void        (*isr22)(void);
    void        (*isr23)(void);
    void        (*isr24)(void);
    void        (*isr25)(void);
    void        (*isr26)(void);
};
