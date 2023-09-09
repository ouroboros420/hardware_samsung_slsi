### RTC Timer Device Driver

### Defines
```c
#define RTC_REPEAT_OFF 0
#define RTC_REPEAT_ON  1
```

---

### Types
---

### Functions

```c
uint64_t rtcGetTimeStampNS(void)
```
**DESCRIPTION**<br>
Get current timestamp in nanosecond<br>

**PARAMETERS**<br>

**RETURN VALUE** <br>
It returns current timestamp.

<br>
<br>


```c
uint64_t rtcGetTimeStampUS(void)
```
**DESCRIPTION**<br>
Get current timestamp in microsecond<br>

**PARAMETERS**<br>

**RETURN VALUE** <br>
It returns current timestamp.

<br>
<br>

```c
void rtcSetTickTime(IN uint32_t sec, IN uint32_t mSec, IN uint32_t uSec,
                     OUT void (*callback) (void), uint8_t repeat)
```
**DESCRIPTION**<br>
Set RTC Tick timer.<br>
The timer will be expired after sec + mSec + uSec.

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| sec | Second |
| mSec | milisecond |
| uSec | microsecond |
| callback | Point of function that will be executed when the timer is expired |
| repeat | Repeat option<br> 0 : Not repeat<br> 1 : Repeat |

**RETURN VALUE**<br>


<br>
<br>

```c
uint64_t rtcGetCurrentTickTime(void)
```
**DESCRIPTION**<br>
Get elapsed time<br>

**PARAMETERS**<br>

**RETURN VALUE**<br>
It returns elapsed time after start in microsecond order(us)


<br>
<br>

```c
void rtcStartTick(void)
```
**DESCRIPTION**<br>
Start Timer<br>

**PARAMETERS**<br>

**RETURN VALUE**<br>


<br>
<br>

```c
void rtcStopTick(void)
```
**DESCRIPTION**<br>
Stop Timer<br>

**PARAMETERS**<br>

**RETURN VALUE**<br>


<br>
<br>


```c
void rtcSetTickHandler(void (*callback)(void));
```
**DESCRIPTION**<br>
 Register RTC Tick interrupt handler<br>

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| callback | Pointer of function that will be executed when the timer expires|

**RETURN VALUE**<br>


<br>
<br>

---
