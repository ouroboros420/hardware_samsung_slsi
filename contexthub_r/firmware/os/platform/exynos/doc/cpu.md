### I2C Device Driver

### Defines

### Types

### Functions
```c
void cpuRegisterHandler(uint32_t irqNum, void (*handler)(void))
```
**DESCRIPTION**<br>
Register IRQ Hander for the specified IRQ.<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| irqNum | Interrupt number. <br> It is described in [chip specific header file](./csp.md). |
| handler | Pointer of IRQ handler |

<br><br>