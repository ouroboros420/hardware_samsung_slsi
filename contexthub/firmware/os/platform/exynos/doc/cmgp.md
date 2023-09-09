## CMGP(Common GPIO) Device Driver

### Defines
---

### Types
  
```c
  typedef enum {
      CMGP_IRQ_GROUP_USI,
      CMGP_IRQ_GROUP_I2C,
      CMGP_IRQ_GROUP_ADC,
      CMGP_IRQ_GROUP_GPIO,
      CMGP_IRQ_GROUP_MAX
  }CmgpIrqGroupType;
  
  typedef struct {
      CmgpIrqType         irq;
      CmgpIrqGroupType    irqGroup;
      uint32_t            irqSource;
  }CmgpIrqInfoType;
```
&nbsp;&nbsp;&nbsp;&nbsp;CmgpIrqType is enum value of each interrupt. The number of interrupts is different for each product.

---

### Functions
```c
void cmgpInit(void);
```
**DESCRIPTION**<br>
Initialize CMGP block
<br>
<br>

```c
void cmgpEnableInterrupt(uint32_t intNum);
```
**DESCRIPTION**<br>
Enable interrupt corresponding to intNum.<br>

**PARAMETERS**<br>
*intNum* - interrupt number.<br>
<br>
<br>

```c
void cmgpDisableInterrupt(uint32_t intNum);
```
**DESCRIPTION**<br>
Disable interrupt corresponding to intNum.<br>

**PARAMETERS**<br>
*intNum* - interrupt number.<br>
<br>
<br>

```c
void cmgpSetIrqPriority(uint32_t intNum, uint32_t priority)
```
**DESCRIPTION**<br>
Set interrupt priority for interrupt of intNum.<br>

**PARAMETERS**<br>
*intNum* - interrupt number.<br>
*priority* - interrupt priority (0~15). Lower value has the higher priority.<br>
<br>
<br>

```c
void cmgp_IRQHandler(void);
```
**DESCRIPTION**<br>
CMGP Interrupt Hander<br>
