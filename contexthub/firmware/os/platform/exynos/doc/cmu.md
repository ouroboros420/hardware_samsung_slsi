### CMU Device Driver

### Defines
---

### Types
```c
typedef enum {
    CMU_CLK_OSCCLK_RCO = 0,
    CMU_CLK_CLK_RCO,
    CMU_CLK_RTCCLK,
    CMU_CLK_OUTPUT_CMUAPM,
    CMU_CLK_OUTPUT_CPU,
    CMU_CLK_CHUB_TIMER,
    CMU_CLK_CHUB_BUS,
    CMU_CLK_CHUB_I2C,
    CMU_CLK_CHUB_USI00,
    CMU_CLK_CHUB_USI01,
    CMU_CLK_CHUB_USI02,
    CMU_CLK_CHUB_WDT,

    CMU_CLK_CMGP_I2C,
    CMU_CLK_CMGP_USI00,
    CMU_CLK_CMGP_USI01,
    CMU_CLK_CMGP_USI02,
    CMU_CLK_CMGP_USI03,

    CMU_CLK_I2C,
    CMU_CLK_I2C00 = CMU_CLK_I2C,
    CMU_CLK_I2C01,
    CMU_CLK_I2C02,
    CMU_CLK_I2C03,
    CMU_CLK_I2C04,
    CMU_CLK_I2C05,

    CMU_CLK_I2C06,
    CMU_CLK_I2C07,
    CMU_CLK_I2C08,
    CMU_CLK_I2C09,
    CMU_CLK_I2C10,
    CMU_CLK_I2C11,
    CMU_CLK_I2C12,
    CMU_CLK_I2C13,

    CMU_MAX_IP_TYPE
} CmuIpType;
```
---

### Functions
```c
void cmuInit(uint32_t mainclk)
```
**DESCRIPTION**<br>
Initialize CMU settings. 

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| mainclk | Source clock from DLL |

<br>
<br>

```c
uint32_t cmuGetSpeed(IN CmuIpType ip)
```
**DESCRIPTION**<br>
Get clk speed of cmuIPs.

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| ip | IP number from CmuIpType |

<br>
<br>

```c
bool cmuSetDivider(IN CmuIpType ip, IN uint32_t divider)
```
**DESCRIPTION**<br>
Set divider of cmu IPs.

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| ip | IP number from CmuIpType |
| divider | divider value |

<br>
<br>


```c
uint32_t cmuGetCpuSpeed(void)
```
**DESCRIPTION**<br>
Get clk speed of CPU

<br>
<br>

---
