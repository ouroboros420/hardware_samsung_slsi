### Mailbox Device Driver

### Defines
```c
enum cipc_region {
	CIPC_REG_SRAM_BASE,
	CIPC_REG_IPC_BASE,
	CIPC_REG_CIPC_BASE,
	/* common ipc for chub & ap */
	CIPC_REG_AP2CHUB,
	CIPC_REG_EVT_AP2CHUB,
	CIPC_REG_DATA_AP2CHUB,
	CIPC_REG_CHUB2AP,
	CIPC_REG_EVT_CHUB2AP,
	CIPC_REG_DATA_CHUB2AP,
	CIPC_REG_DATA_CHUB2AP_BATCH,
	/* common ipc for chub & abox */
	CIPC_REG_ABOX2CHUB, /* 10 */
	CIPC_REG_EVT_ABOX2CHUB,
	CIPC_REG_DATA_ABOX2CHUB_AUD,
	CIPC_REG_DATA_ABOX2CHUB, /* prox data */
	CIPC_REG_CHUB2ABOX,
	CIPC_REG_EVT_CHUB2ABOX,
	CIPC_REG_DATA_CHUB2ABOX, /* gyro data */
	/* common ipc for chub & gnss */
	CIPC_REG_GNSS2CHUB,
	CIPC_REG_EVT_GNSS2CHUB,
	CIPC_REG_DATA_GNSS2CHUB,

	CIPC_REG_CHUB2GNSS,
	CIPC_REG_EVT_CHUB2GNSS,
	CIPC_REG_DATA_CHUB2GNSS,
	CIPC_REG_MAX,
};
```
```c
enum cipc_user_id {
	CIPC_USER_AP2CHUB,
	CIPC_USER_CHUB2AP,
#ifdef CIPC_DEF_USER_ABOX
	CIPC_USER_ABOX2CHUB,
	CIPC_USER_CHUB2ABOX,
#endif
#ifdef CIPC_DEF_USER_GNSS
	CIPC_USER_GNSS2CHUB,
	CIPC_USER_CHUB2GNSS,
#endif
	CIPC_USER_MAX,
};
```
---

### Types
```c
struct cipc_evt_buf {
	unsigned int evt;
	unsigned int value;	/* cipc for value passing */
	unsigned int mb_irq;
	unsigned int status;
};
```
---

### Functions
```c
void *cipc_read_data(enum cipc_region reg, unsigned int *len)
```
**DESCRIPTION**<br>
Read IPC data.<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| reg | ipc region id to read data |
| len | length of the recived data |

<br><br>

```c
int cipc_write_data(enum cipc_region reg, void *tx, unsigned int length)
```
**DESCRIPTION**<br>
Write IPC data.<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| reg | ipc region id to write data |
| tx | buffer to write |
| length | size to write |

<br><br>

```c
struct cipc_evt_buf *cipc_get_evt(enum cipc_region reg)
```

**DESCRIPTION**<br>
Read IPC event. <br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| reg | ipc region id to read evt |

<b><br>

```c
int cipc_add_evt(enum cipc_region reg, unsigned int evt)
```
**DESCRIPTION**<br>
Write a IPC event. <br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| reg | ipc region id to write evt |
| evt | a event to write |

<br><br>
---
