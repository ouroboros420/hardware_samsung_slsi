## USI Device Driver

### Defines
```c
#define USI_CHUB0                   0
#define USI_I2C_CHUB0               1
#define USI_CHUB1                   2
#define USI_I2C_CHUB1               3
#define USI_CHUB2                   4
#define USI_I2C_CHUB2               5
```
* <I>Note: The number of USI channels is different for each product</I>

---

### Types

```c
typedef enum {
    USI_PROTOCOL_UART = 0x1,
    USI_PROTOCOL_SPI  = 0x2,
    USI_PROTOCOL_I2C  = 0x4,
    USI_PROTOCOL_MAX
} UsiProtocolType;
```

---

### Functions
```c
int32_t usiOpen(IN uint32_t port, IN UsiProtocolType protocol)
```
**DESCRIPTION**<br>
Open USI port<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| port | USI channel number. |
| protocol | Select one of the follouing functions of USI<br>USI_PROTOCOL_UART<br>USI_PROTOCOL_SPI<br>USI_PROTOCOL_I2C |

**RETURN VALUE**<br>
It returns 0 when success, and returns not zero value when there is any error.

<br>
<br>

```c
int32_t usiSetProtocol(IN uint32_t port, IN UsiProtocolType protocol);
```
**DESCRIPTION**<br>
Set the function of USI.<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| port | USI channel number. |
| protocol | Select one of the follouing functions of USI<br>USI_PROTOCOL_UART<br>USI_PROTOCOL_SPI<br>USI_PROTOCOL_I2C |

**RETURN VALUE**<br>
It returns 0 when success, and returns not zero value when there is any error.

<br>
<br>

```c
UsiProtocolType usiGetProtocol(IN uint32_t port);
```
**DESCRIPTION**<br>
Get the current configured function of USI<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| port | USI channel number. |

**RETURN VALUE**<br>
It returns function of USI.<br>
- USI_PROTOCOL_UART
- USI_PROTOCOL_SPI
- USI_PROTOCOL_I2C 

<br>
<br>

