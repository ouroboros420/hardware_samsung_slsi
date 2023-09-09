## SPI Device Driver

### Defines
---

### Types

```c
struct SpiDeviceState {
    struct SpiDevice dev;

    const struct SpiPacket *packets;
    size_t n;
    size_t currentBuf;
    struct SpiMode mode;
    uint16_t tid;

    SpiCbkF rxTxCallback;
    void *rxTxCookie;

    SpiCbkF finishCallback;
    void *finishCookie;

    int err;
};

```

---

### Functions
```c
int spiMasterRequest(uint8_t busId, struct SpiDevice **dev);
```
**DESCRIPTION**<br>
Request SPI Master channel.<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| busId | Spi channel number. |
| dev | Pointer to a spi information of type SpiDevice. |


<br>
<br>

```c
int spiMasterRxTx(struct SpiDevice *dev,
                  spi_cs_t cs,
                  const struct SpiPacket packets[],
                  size_t n,
                  const struct SpiMode *mode,
                  SpiCbkF callback,
                  void *cookie);
```
**DESCRIPTION**<br>
Read and Transfer SPI message.<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| dev | Pointer to a spi information of type SpiDevice. |
| cs | Chip Select number. |
| packets | Pointer to a spi message array of type SpiPacket. |
| n | The number of messages. |
| SpiMode | Pointer to a spi mode information of type SpiMode. |
| callback | Pointer to the callback routing to be called after spi is done. |
| cookie | A void pointer which is used as parameter for the callback function. |

<br>
<br>

```c
int spiMasterRelease(struct SpiDevice *dev);
```
**DESCRIPTION**<br>
Release SPI master channel.<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| dev | Pointer to a spi information of type SpiDevice. |

<br><br>

