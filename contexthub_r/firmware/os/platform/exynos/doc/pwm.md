### PWM Timer Device Driver

### Defines
```c
#define LOW   0
#define HIGH  1
```

---

### Types
```c
 typedef enum {
    PWM_TIMER_NUM_MIN,
    PWM_TIMER0 = PWM_TIMER_NUM_MIN,
    PWM_TIMER1,
    PWM_TIMER2,
    PWM_TIMER3,
#if (SUPPORT_PWM_TIMER4)
    PWM_TIMER4,
#endif
    PWM_TIMER_NUM_MAX
} PwmTimerNumType;

typedef enum {
    PWM_TIMER_MODE_ONESHOT,
    PWM_TIMER_MODE_INTERVAL,
    PWM_TIMER_MODE_MAX
} PwmTimerModeType;
```
---

### Functions
```c
int pwmInit( PwmTimerNumType timer, uint32_t uSecond,
             PwmTimerModeType mode, uint32_t intEn )
```
**DESCRIPTION**<br>
Config PWM Timer<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| timer | Number of PWM Timer <br> PWM_TIMER4 may not be supported according to the product |
| uSecond | Period of timer. Unit is micro second(us)|
| mode | Timer mode : Oneshot or Continuous mode <br>(PWM_TIMER_MODE_ONESHOT, PWM_TIMER_MODE_INTERVAL)|
| intEn | Enable or Disable Interrupt |

**RETURN VALUE** <br>
It returns 0 when success, or return not zero value when there is an error while configuration.

<br>
<br>


```c
int pwmEnableInterrut( PwmTimerNumType timer )
```
**DESCRIPTION**<br>
Enable Interrupt<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| timer | Number of PWM Timer |

**RETURN VALUE** <br>
It returns 0 when success, or return not zero value when there is an error while configuration.

<br>
<br>

```c
int pwmDisableInterrupt( PwmTimerNumType timer )
```
**DESCRIPTION**<br>
Disable Interrupt<br>

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| timer | Number of PWM Timer |

**RETURN VALUE**<br>
It returns 0 when success, or return not zero value when there is an error while configuration.


<br>
<br>

```c
int pwmStartTimer( PwmTimerNumType timer )
```
**DESCRIPTION**<br>
Start Timer<br>

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| timer | Number of PWM Timer |

**RETURN VALUE**<br>
It returns 0 when success, or return not zero value when there is an error while configuration.


<br>
<br>

```c
int pwmStopTimer( PwmTimerNumType timer )
```
**DESCRIPTION**<br>
Stop Timer<br>

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| timer | Number of PWM Timer |

**RETURN VALUE**<br>
It returns 0 when success, or return not zero value when there is an error while configuration.


<br>
<br>

```c
int pwmSetTime( PwmTimerNumType timer, uint32_t uSecond )
```
**DESCRIPTION**<br>
Change period of Timer<br>

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| timer | Number of PWM Timer |
| uSecond | Expiration time in microsecond(us) |

**RETURN VALUE**<br>
It returns 0 when success, or return not zero value when there is an error while configuration.


<br>
<br>


```c
int pwmSetDuty( PwmTimerNumType timer, uint32_t duty )
```
**DESCRIPTION**<br>
 Change Duty Ratio in a timer period<br>

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| timer | Number of PWM Timer |
| duty | Duty Ratio. (1 < duty < 100)|

**RETURN VALUE**<br>
It returns 0 when success, or return not zero value when there is an error while configuration.


<br>
<br>


```c
int pwmSetMode( PwmTimerNumType timer, PwmTimerModeType mode )
```
**DESCRIPTION**<br>
Change Timer Mode. (OneShot or Continuous mode)<br>

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| timer | Number of PWM Timer |
| mode | Timer mode : Oneshot or Continuous mode <br>(PWM_TIMER_MODE_ONESHOT, PWM_TIMER_MODE_INTERVAL)|


**RETURN VALUE**<br>
It returns 0 when success, or return not zero value when there is an error while configuration.


<br>
<br>


```c
int pwmSetDefaultLevel( PwmTimerNumType timer, uint32_t level )
```
**DESCRIPTION**<br>
Set Default level of PWM output signal<br>

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| timer | Number of PWM Timer |
| level | Level of signal. (LOW, HIGH)|

**RETURN VALUE**<br>
It returns 0 when success, or return not zero value when there is an error while configuration.


<br>
<br>

```c
int pwmEnableInterrupt(PwmTimerNumType timer )
```
**DESCRIPTION**<br>
Enable Interrupt<br>

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| timer | Number of PWM Timer |

**RETURN VALUE**<br>
It returns 0 when success, or return not zero value when there is an error while configuration.


<br>
<br>

```c
int pwmDisableInterrupt(PwmTimerNumType timer )
```
**DESCRIPTION**<br>
Disable Interrupt<br>

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| timer | Number of PWM Timer |

**RETURN VALUE**<br>
It returns 0 when success, or return not zero value when there is an error while configuration.


<br>
<br>

```c
uint32_t pwmGetCount( PwmTimerNumType timer)
```
**DESCRIPTION**<br>
Get current count value. <br>
The timer expires when the counter value is 0.<br>

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| timer | Number of PWM Timer |

**RETURN VALUE**<br>
It returns 0 when success, or return not zero value when there is an error while configuration.


<br>
<br>

```c
int pwmClose( PwmTimerNumType timer )
```
**DESCRIPTION**<br>
Close Timer<br>

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| timer | Number of PWM Timer |

**RETURN VALUE**<br>
It returns 0 when success, or return not zero value when there is an error while configuration.


<br>
<br>


```c
void pwmSetHandler(PwmTimerNumType timer, void (*handler)(void))
```
**DESCRIPTION**<br>
Register timer interrrupt handler<br>

**PARAMETERS**<br>
| Parameter | Description |
| --- | ---|
| timer | Number of PWM Timer |
| handler | Pointer of handler function|


<br><br>


---
