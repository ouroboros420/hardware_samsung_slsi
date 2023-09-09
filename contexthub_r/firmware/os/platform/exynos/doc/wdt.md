## WDT Device Driver

### Defines
```c
#define WDT_DEFAULT_TIMEOUT     30000 // 30000ms
```

---

### Types

---

### Functions
```c
void wdtInit(void)
```
**DESCRIPTION**<br>
Watchdog Timer Init<br>
Watchdog timer runs with WDT_DEFAULT_TIMEOUT value after init.
If want to change timeout value, call wdtSetTime() after calling this function.

**PARAMETERS**<br>
**RETURN VALUE**<br>

<br>
<br>

```c
void wdtEnable(void)
```
**DESCRIPTION**<br>
Start Watchdog timer<br>

**PARAMETERS**<br>
**RETURN VALUE**<br>

<br>
<br>

```c
void wdtDisable(void)
```
**DESCRIPTION**<br>
Stop Watchdog Timer<br>

**PARAMETERS**<br>
**RETURN VALUE**<br>

<br>
<br>

```c
void wdtSetTime(uint32_t time_msec)
```
**DESCRIPTION**<br>
Set expiration time in milisecond<br>

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| time_msec | Expiration time |

**RETURN VALUE**<br>

<br>
<br>

```c
void wdtPing(void)
```
**DESCRIPTION**<br>
Restart Watchdog Timer to prevent expiration<br>

**PARAMETERS**<br>
**RETURN VALUE**<br>

<br>
<br>

```c
void wdtEnableInterrupt(void)
```
**DESCRIPTION**<br>
Enable Interrupt<br>

**PARAMETERS**<br>
**RETURN VALUE**<br>

<br>
<br>

```c
void wdtDisableInterrupt(void)
```
**DESCRIPTION**<br>
Disable Interrupt<br>

**PARAMETERS**<br>
**RETURN VALUE**<br>

<br>
<br>

