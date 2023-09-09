#include <mailboxOS.h>
#include <mailboxDrv.h>
#include <csp_common.h>
#include <csp_os.h>
#include <csp_printf.h>
#include <csp_assert.h>
#include <ipc_chub.h>
#include <pwrDrv.h>
#include <sysreg.h>
#include <mailboxApm.h>
#include <debug.h>

#if defined(SEOS)
#include <platform.h>
#include <nanohubCommand.h>
#include <hostIntf.h>
#include <cmsis.h>
#include <timer.h>
#include <algos/ap_hub_sync.h>
#if defined(HEAP_DEBUG)
#include <heapDebug.h>
#endif
#endif

#include <plat/rtc/rtc.h>
#include <cmu.h>
#include <pwm.h>
#include <plat/dfs/dfs.h>

#ifdef DEF_PERF_MONITOR
static bool mPerfMonitorEnabled;
static void perfMonitorResult(int final);
static bool mIdleMonitorAutoPrint;
static void perfMonitorEnable(bool on, bool auto_print)
{
    mIdleMonitorAutoPrint = auto_print;
    if (mPerfMonitorEnabled && !on)
        perfMonitorResult(1);

    mPerfMonitorEnabled = on;
}

#define CSP_LOQ_QUEUE_MAX (5000)

struct chub_prof_s {
    uint64_t time;
    enum chub_prof_id id;
};

struct prof_log {
    bool warp;
    int log_index;
    struct chub_prof_s log[CSP_LOQ_QUEUE_MAX];
};
struct prof_log pLog;

#define SENS_ON_OFF_CNT (4)
#define CHECK_ACCEL_UTIL (2)

int sensor_logging;

char chub_prof_name[chub_prof_max][16] = {
    "wfi_in",
    "wfi_out",
    "wfi2_in",
    "wfi2_out",
    "ipc_rd",
    "ipc_rd_out",
    "ipc_wt",
    "ipc_wt_out",
    "ipc_wt_evt",
    "ipc_wt_evt_out",
    "i2c_in",
    "i2c_out",
    "i2c_irq",
    "i2c_irq_out",
    "spi_in",
    "spi_out",
    "spi_irq",
    "spi_irq_out",
    /* acc */
    "acc_on_in",
    "acc_on_out",
    "acc_rate_in",
    "acc_rate_out",
    "acc_timer",
    "acc_cb",
    /* mag */
    "mag_on_in",
    "mag_on_out",
    "mag_rate_in",
    "mag_rate_out",
    "mag_timer",
    "mag_cb",
    /* gyro */
    "gyro_on_in",
    "gyro_on_out",
    "gyro_rate_in",
    "gyro_rate_out",
    "gyro_timer",
    "gyro_cb",
    /* baro */
    "baro_on_in",
    "baro_on_out",
    "baro_rate_in",
    "baro_rate_out",
    "baro_timer",
    "baro_cb",
    /* prox */
    "prox_on_in",
    "prox_on_out",
    "prox_rate_in",
    "prox_rate_out",
    "prox_timer",
    "prox_cb",
    "prox_isr",
    "prox_isr_cb",
    /* als */
    "als_on_in",
    "als_on_out",
    "als_rate_in",
    "als_rate_out",
    "als_timer",
    "als_cb",
    "als_isr",
    "als_isr_cb",
};

struct sens_prof {
    u64 poweron;
    u64 setrate;
    u64 poweroff;
    u64 read;
};

struct prof_time {
    char name[16];
    u64 lastest_t;
    u64 total_t;
};

enum {
    DEV_ID_WFI,
    DEV_ID_SPI,
    DEV_ID_I2C,
};

enum sensorEvtList {
    SENS_PROP_EVT_POWER_ON,
    SENS_PROP_EVT_SETRATE,
    SENS_PROP_EVT_CAL,
    SENS_PROP_EVT_FLUSH,
    SENS_PROP_EVT_MAX
};

enum chub_prof_sens_list {
    chub_prof_acc,
    chub_prof_gyro,
    chub_prof_mag,
    chub_prof_prox,
    chub_prof_als,
    chub_prof_baro,
};

struct chub_prof_result {
    /* duriations of each event */
    u64 total_t;
    u64 wfi_t;
    u64 spi_t;
    u64 i2c_t;
    u64 ipc_t;

    /* check interrupt mode */
    bool in_spi;
    bool in_i2c;
    bool in_wfi;
    bool in_wfi2;

    struct prof_time acc_t;
    struct prof_time gyro_t;
    struct prof_time mag_t;
    struct prof_time prox_t;
    struct prof_time als_t;
    struct prof_time baro_t;

    struct prof_time wfi;
    struct prof_time wfi2;
    struct prof_time spi;
    struct prof_time i2c;
    struct prof_time ipc_rd;
    struct prof_time ipc_wt;
    struct prof_time ipc_wt_evt;
};

struct chub_prof_result pRes = {
    .wfi = {.name = "wfi",},
    .wfi2 = {.name = "wfi2",},
    .spi = {.name = "spi",},
    .i2c = {.name = "i2c",},
    .ipc_rd = {.name = "ipc_rd",},
    .ipc_wt = {.name = "ipc_wt",},
    .ipc_wt_evt = {.name = "ipc_wt_evt",},
};

#define INVAL_INDEX (-1)

static inline void handleDebugLogEvt(int index)
{
    if (pLog.log[index].time)
        CSP_PRINTF_INFO("[ %4d: %4d %lld %s ]\n", index, pLog.log[index].id,
                        pLog.log[index].time,
                        chub_prof_name[pLog.log[index].id]);
    else
        CSP_PRINTF_INFO("%s: error\n", __func__);
}

static inline void chubPrefClearBuf(bool all)
{
    if (all) {
        /* clear debug info */
        sensor_logging = 0;
        pLog.log_index = 0;
        memset(&pRes, 0, sizeof(struct chub_prof_result));
        memset(&pLog, 0, sizeof(struct prof_log));
    } else {
        memset(&pRes, 0, sizeof(struct chub_prof_result));
    }
}

/* calcalrate the duriation of each event */
static void updateDevEvt(struct prof_time *dev, u64 time, bool in)
{
    if (in) {
        dev->lastest_t = time;
    } else {
        if (dev->lastest_t) {
            dev->total_t += (time - dev->lastest_t);
            dev->lastest_t = 0;
        }
    }
}

static void debugHandleProfEvt(enum chub_prof_id id, u64 time)
{
    if (pRes.in_wfi) {
        pRes.in_wfi = 0;
        updateDevEvt(&pRes.wfi, time, false);
    } else if (pRes.in_wfi2) {
        pRes.in_wfi2 = 0;
        updateDevEvt(&pRes.wfi2, time, false);
    }

    switch (id) {
        /* wfi */
    case chub_prof_wfi_in:
        pRes.in_wfi = 1;
        updateDevEvt(&pRes.wfi, time, true);
        break;
    case chub_prof_wfi_out:
        if (pRes.in_wfi) {
            pRes.in_wfi = 0;
            updateDevEvt(&pRes.wfi, time, false);
        }
        break;
    case chub_prof_wfi2_in:
        pRes.in_wfi2 = 1;
        updateDevEvt(&pRes.wfi2, time, true);
        break;
    case chub_prof_wfi2_out:
        if (pRes.in_wfi2) {
            pRes.in_wfi2 = 0;
            updateDevEvt(&pRes.wfi2, time, false);
        }
        break;
        /* drivers */
    case chub_prof_ipc_rd_in:
    case chub_prof_ipc_rd_out:
        updateDevEvt(&pRes.ipc_rd, time,
                     (id == chub_prof_ipc_rd_in) ? true : false);
        break;
    case chub_prof_ipc_wt_in:
    case chub_prof_ipc_wt_out:
        updateDevEvt(&pRes.ipc_wt, time,
                     (id == chub_prof_ipc_wt_in) ? true : false);
        break;
    case chub_prof_ipc_wt_evt_in:
    case chub_prof_ipc_wt_evt_out:
        updateDevEvt(&pRes.ipc_wt_evt, time,
                     (id == chub_prof_ipc_wt_evt_in) ? true : false);
        break;
    case chub_prof_spi_in:
    case chub_prof_spi_out:
        updateDevEvt(&pRes.spi, time, (id == chub_prof_spi_in) ? true : false);
        break;
    case chub_prof_spi_irq:
    case chub_prof_spi_irq_out:
        updateDevEvt(&pRes.spi, time, (id == chub_prof_spi_irq) ? true : false);
        break;
    case chub_prof_i2c_in:
    case chub_prof_i2c_out:
        updateDevEvt(&pRes.i2c, time, (id == chub_prof_i2c_in) ? true : false);
        break;
    case chub_prof_i2c_irq:
    case chub_prof_i2c_irq_out:
        updateDevEvt(&pRes.i2c, time, (id == chub_prof_i2c_irq) ? true : false);
        break;
        /* acc */
    case chub_prof_sens_acc_poweron:
    case chub_prof_sens_acc_setfreq:
        updateDevEvt(&pRes.acc_t, time, true);
        break;
    case chub_prof_sens_acc_poweron_done:
    case chub_prof_sens_acc_setfreq_done:
        updateDevEvt(&pRes.acc_t, time, false);
        break;
    case chub_prof_sens_acc_timer:
    case chub_prof_sens_acc_cb:
        break;
        /* mag */
    case chub_prof_sens_mag_poweron:
    case chub_prof_sens_mag_setfreq:
        updateDevEvt(&pRes.mag_t, time, true);
        break;
    case chub_prof_sens_mag_poweron_done:
    case chub_prof_sens_mag_setfreq_done:
        updateDevEvt(&pRes.mag_t, time, false);
        break;
    case chub_prof_sens_mag_timer:
    case chub_prof_sens_mag_cb:
        break;
        /* gyro */
    case chub_prof_sens_gyro_poweron:
    case chub_prof_sens_gyro_setfreq:
        updateDevEvt(&pRes.gyro_t, time, true);
        break;
    case chub_prof_sens_gyro_poweron_done:
    case chub_prof_sens_gyro_setfreq_done:
        updateDevEvt(&pRes.gyro_t, time, false);
        break;
    case chub_prof_sens_gyro_timer:
    case chub_prof_sens_gyro_cb:
        break;
        /* baro */
    case chub_prof_sens_baro_poweron:
    case chub_prof_sens_baro_setfreq:
        updateDevEvt(&pRes.baro_t, time, true);
        break;
    case chub_prof_sens_baro_poweron_done:
    case chub_prof_sens_baro_setfreq_done:
        updateDevEvt(&pRes.baro_t, time, false);
        break;
    case chub_prof_sens_baro_timer:
    case chub_prof_sens_baro_cb:
        break;
        /* prox */
    case chub_prof_sens_prox_poweron:
    case chub_prof_sens_prox_setfreq:
        updateDevEvt(&pRes.prox_t, time, true);
        break;
    case chub_prof_sens_prox_poweron_done:
    case chub_prof_sens_prox_setfreq_done:
        updateDevEvt(&pRes.prox_t, time, false);
        break;
    case chub_prof_sens_prox_isr:
    case chub_prof_sens_prox_isr_out:
    case chub_prof_sens_prox_timer:
    case chub_prof_sens_prox_cb:
        break;
        /* als */
    case chub_prof_sens_als_poweron:
    case chub_prof_sens_als_setfreq:
        updateDevEvt(&pRes.als_t, time, true);
        break;
    case chub_prof_sens_als_poweron_done:
    case chub_prof_sens_als_setfreq_done:
        updateDevEvt(&pRes.als_t, time, false);
        break;
    case chub_prof_sens_als_isr:
    case chub_prof_sens_als_isr_out:
    case chub_prof_sens_als_timer:
    case chub_prof_sens_als_cb:
    default:
        break;
    }
}

#define DEBUG_PRINT_LOOP

static void perfMonitorResult(int final)
{
    int i;
    int firstIndex;
    int lastIndex;
    u64 firstTime;
    u64 lastTime;
    u64 profTime;
    u64 wfi_ratio;
    u64 wfi2_ratio;
#if defined(DEF_PERF_MONITOR_IPC) || defined(DEF_PERF_MONITOR_SPI) || defined(DEF_PERF_MONITOR_I2C)
    u64 spi_ratio;
    u64 i2c_ratio;
    u64 ipc_ratio;
    u64 total_ipc_t;
    u64 total_dev_t;
#endif
#ifdef ENABLE_SENS_RATIO
    u64 acc_ratio;
    u64 gyro_ratio;
    u64 mag_ratio;
    u64 prox_ratio;
    u64 als_ratio;
    u64 baro_ratio;
    u64 total_sens_t;
#endif

#ifdef DEBUG_PRINT_LOOP
#if defined(DEF_PERF_MONITOR_IPC) || defined(DEF_PERF_MONITOR_SPI) || defined(DEF_PERF_MONITOR_I2C)
    int id;

    if (final) {
        for (i = pLog.log_index - 1; i > 0; i--) {
            id = pLog.log[i].id;
            if ((id != chub_prof_wfi_in) &&
                (id != chub_prof_wfi_out) &&
                (id != chub_prof_wfi2_in) && (id != chub_prof_wfi2_out))
                break;
        }
        lastIndex = i;
    } else {
        lastIndex = pLog.log_index - 1;
    }
#else
    if (final)
        lastIndex = pLog.log_index - 5;
    else
        lastIndex = pLog.log_index - 1;
#endif

    firstIndex = 0;

    for (i = firstIndex; i < lastIndex; i++)
        debugHandleProfEvt(pLog.log[i].id, pLog.log[i].time);
#else
    if (pLog.warp) {
        firstIndex = pLog.log_index;
        lastIndex =
            pLog.log_index ==
            0 ? (CSP_LOQ_QUEUE_MAX - 1) : (pLog.log_index - 1);
    } else {
        firstIndex = 0;
        lastIndex = pLog.log_index - 1;
    }
#endif

    /* skip first WFI */
    firstTime = pLog.log[firstIndex].time;
    lastTime = pLog.log[lastIndex].time;
    profTime = lastTime - firstTime;

    if ((firstTime == 0) || (firstTime >= lastTime))
        CSP_PRINTF_ERROR
            ("%s: Prof fail: no data: invalid index: %d, %d, %lld, %lld, log_index:%d\n",
             __func__, firstIndex, lastIndex, firstTime, lastTime,
             pLog.log_index);

    CSP_PRINTF_INFO("\n");
    CSP_PRINTF_INFO("[ IdleMon (%d~%d, (idx:%d,final:%d)) ]", firstIndex,
                    lastIndex, pLog.log_index, final);

    /* raw data dump */
    if (final) {
        if (pLog.log_index > CSP_LOQ_QUEUE_MAX) {
            CSP_PRINTF_ERROR("%s: error : index:%d\n", __func__,
                             pLog.log_index);
        } else {
            for (i = firstIndex; i < lastIndex; i++)
                handleDebugLogEvt(i);
        }
    }
#if defined(DEF_PERF_MONITOR_IPC) || defined(DEF_PERF_MONITOR_SPI) || defined(DEF_PERF_MONITOR_I2C)
    total_ipc_t =
        pRes.ipc_rd.total_t + pRes.ipc_wt.total_t + pRes.ipc_wt_evt.total_t;

    CSP_PRINTF_INFO("IPC: rd:%lld, wt:%lld, wt_evt:%lld = %lld\n",
                    pRes.ipc_rd.total_t, pRes.ipc_wt.total_t,
                    pRes.ipc_wt_evt.total_t, total_ipc_t);

    total_dev_t =
        pRes.wfi.total_t + pRes.wfi2.total_t + pRes.spi.total_t +
        pRes.i2c.total_t + total_ipc_t;

    CSP_PRINTF_INFO("DEV: wfi:%lld,%lld,spi:%lld,i2c:%lld,ipc:%lld = %lld\n",
                    pRes.wfi.total_t, pRes.wfi2.total_t, pRes.spi.total_t,
                    pRes.i2c.total_t, total_ipc_t, total_dev_t);
#endif

#if defined(ENABLE_SENS_RATIO)
    total_sens_t =
        pRes.acc_t.total_t + pRes.gyro_t.total_t + pRes.mag_t.total_t +
        pRes.prox_t.total_t + pRes.als_t.total_t + pRes.baro_t.total_t;
    CSP_PRINTF_INFO("[sen] %lld, %lld, %lld, %lld, %lld, %lld = %lld\n",
                    pRes.acc_t.total_t, pRes.gyro_t.total_t, pRes.mag_t.total_t,
                    pRes.prox_t.total_t, pRes.als_t.total_t,
                    pRes.baro_t.total_t, total_sens_t);
#endif

    CSP_PRINTF_INFO("Pro: %dMhz (%lld us=%lld(%d)~%lld(%d))\n",
                    (int)cmuGetCpuSpeed() / 1000000, profTime, firstTime,
                    firstIndex, lastTime, lastIndex);
    wfi_ratio = (pRes.wfi.total_t * 100) / profTime;
    wfi2_ratio = (pRes.wfi2.total_t * 100) / profTime;

    CSP_PRINTF_INFO("== Result ( %6lld us) ==\n", (profTime));
    CSP_PRINTF_INFO("| WFI : %2lld %% ( %6lld us )\n", wfi_ratio + wfi2_ratio,
                    (pRes.wfi.total_t + pRes.wfi2.total_t));

#if defined(DEF_PERF_MONITOR_IPC) || defined(DEF_PERF_MONITOR_SPI) || defined(DEF_PERF_MONITOR_I2C)
    spi_ratio = (pRes.spi.total_t * 100) / profTime;
    i2c_ratio = (pRes.i2c.total_t * 100) / profTime;
    ipc_ratio = (total_ipc_t * 100) / profTime;

    CSP_PRINTF_INFO("| SPI : %2lld %% ( %6lld us )\n", spi_ratio,
                    pRes.spi.total_t);
    CSP_PRINTF_INFO("| I2C : %2lld %% ( %6lld us )\n", i2c_ratio,
                    pRes.i2c.total_t);
    CSP_PRINTF_INFO("| IPC : %2lld %% ( %6lld us )\n", ipc_ratio, total_ipc_t);
#endif
#if defined(ENABLE_SENS_RATIO)
    acc_ratio = (pRes.acc_t.total_t * 100) / profTime;
    gyro_ratio = (pRes.gyro_t.total_t * 100) / profTime;
    mag_ratio = (pRes.mag_t.total_t * 100) / profTime;
    prox_ratio = (pRes.prox_t.total_t * 100) / profTime;
    als_ratio = (pRes.als_t.total_t * 100) / profTime;
    baro_ratio = (pRes.baro_t.total_t * 100) / profTime;

    CSP_PRINTF_INFO("== Sens Ratio (diff:%lld) ==\n", total_sens_t);
    CSP_PRINTF_INFO("| Acc : %lld \n", acc_ratio);
    CSP_PRINTF_INFO("| Gyro : %lld \n", gyro_ratio);
    CSP_PRINTF_INFO("| Mag : %lld \n", mag_ratio);
    CSP_PRINTF_INFO("| Prox : %lld\n", prox_ratio);
    CSP_PRINTF_INFO("| Als : %lld \n", als_ratio);
    CSP_PRINTF_INFO("| Baro : %lld \n", baro_ratio);
#endif

    if (final)
        chubPrefClearBuf(1);
    else
        chubPrefClearBuf(0);
}

enum chub_prof_sens_id {
    chub_prof_sens_acc = chub_prof_sens_acc_poweron,
    chub_prof_sens_gyro = chub_prof_sens_gyro_poweron,
    chub_prof_sens_mag = chub_prof_sens_mag_poweron,
    chub_prof_sens_als = chub_prof_sens_als_poweron,
    chub_prof_sens_prox = chub_prof_sens_prox_poweron,
    chub_prof_sens_baro = chub_prof_sens_baro_poweron,
};

void perfMonitorSetSensEvt(u32 sensType, enum chub_prof_sens_evt act)
{
    enum chub_prof_id id = 0;

    if ((sensType == SENS_TYPE_ACCEL) || (sensType == SENS_TYPE_ACCEL_UNCAL))
        id = chub_prof_sens_acc + act;
    else if ((sensType == SENS_TYPE_GYRO) || (sensType == SENS_TYPE_GYRO_UNCAL))
        id = chub_prof_sens_gyro + act;
    else if ((sensType == SENS_TYPE_MAG) || (sensType == SENS_TYPE_MAG_UNCAL))
        id = chub_prof_sens_mag + act;
    else if (sensType == SENS_TYPE_BARO)
        id = chub_prof_sens_baro + act;
    else if (sensType == SENS_TYPE_ALS)
        id = chub_prof_sens_als + act;
    else if (sensType == SENS_TYPE_PROX)
        id = chub_prof_sens_prox + act;

    osLog(LOG_DEBUG, "%s: type:%d, act:%d, id:%d\n", __func__, sensType, act,
          id);
    perfMonitorSetEvt(id);
}

void perfMonitorSetEvt(enum chub_prof_id id)
{
    struct chub_prof_s *log;
    bool wrap = 0;

    idleMonitorSetEvt(id);
    if (!mPerfMonitorEnabled)
        return;

#if !defined(DEF_PERF_MONITOR_IPC)
    if (id >= chub_prof_ipc_rd_in && id <= chub_prof_ipc_wt_evt_out)
        return;
#endif

#if !defined(DEF_PERF_MONITOR_SPI)
    if (id >= chub_prof_spi_in && id <= chub_prof_spi_in)
        return;
#endif

#if !defined(DEF_PERF_MONITOR_I2C)
    if (id >= chub_prof_i2c_in && id <= chub_prof_i2c_irq_out)
        return;
#endif

#if defined(DEF_PERF_MONITOR_IPC) || defined(DEF_PERF_MONITOR_SPI) || defined(DEF_PERF_MONITOR_I2C)
    /* skip first WFI */
    if 
(pLog.log_index == 0)
        if (id == chub_prof_wfi_in || id == chub_prof_wfi2_in
            || id == chub_prof_wfi_out || id == chub_prof_wfi2_out)
            return;
#endif

    log = &pLog.log[pLog.log_index];
    pLog.log_index = (pLog.log_index + 1) % CSP_LOQ_QUEUE_MAX;
    if (pLog.log_index == (CSP_LOQ_QUEUE_MAX - 1))
        wrap = 1;

    /* time stamp */
    log->time = getTimeStampUS();
    log->id = id;

    /* auto printout */
    if (wrap && mIdleMonitorAutoPrint)
        perfMonitorResult(0);
}
#else /* #ifdef DEF_PERF_MONITOR */
#define perfMonitorEnable(a, b)
#if defined(DEF_IDLE_MONITOR)
void perfMonitorSetEvt(enum chub_prof_id id)
{
    idleMonitorSetEvt(id);
}
#endif
#endif

#if defined(DEF_IPC_MONITOR) || defined(DEF_IDLE_MONITOR)
#ifdef DEF_IPC_MONITOR
extern int mNumIpcOut;
struct ipcMonitor {
	u64 preIpcNum;
};
struct ipcMonitor ipcMon;
#endif

#ifdef DEF_IDLE_MONITOR
struct idleMonitor {
    bool inWfi;
    u64 startWfiTime;
    u64 totalWfiTime;
    u64 perFreqWfiTime[DFS_FREQ_MAX];
    DfsFreq curFreq;
    u64 startTime;
    u64 wfiTime;
    u64 lastTotalMonTime;
    void *cbData;
    idleMonitorCbkF CB;
};
static struct idleMonitor idleMon;

static u64 idleMonitorGetRatio(void)
{
    return ((idleMon.totalWfiTime * 100) / idleMon.lastTotalMonTime);
}
#endif

struct monitorInfo {
	bool idleMonEnabled;
	bool ipcMonEnabled;
	bool perfTimerEnabled;
	uint32_t perfTimerId;
};
static struct monitorInfo monInfo;

typedef void (*idleMonitorCbkF) (void *data, u64 * idleRatio, u64 * totalUs);
static void perfTimerCallback(uint32_t timerId, void *cookie)
{
#ifdef DEF_IDLE_MONITOR
    u64 idleRatio;

    if (monInfo.idleMonEnabled) {
        idleMon.lastTotalMonTime = getTimeStampUS() - idleMon.startTime;
        idleRatio = idleMonitorGetRatio();
        CSP_PRINTF_INFO("[idleMon : %lld %% (wfi:%lld us / total:%lld us)]\n",
                        idleRatio, idleMon.totalWfiTime,
                        idleMon.lastTotalMonTime);
        idleMon.startTime = getTimeStampUS();
        idleMon.wfiTime = idleMon.totalWfiTime;
        if (idleMon.CB)
            idleMon.CB(idleMon.cbData, &idleMon.wfiTime,
                       &idleMon.lastTotalMonTime);
        idleMon.totalWfiTime = 0;
    }
#endif
#ifdef DEF_IPC_MONITOR
    if (monInfo.ipcMonEnabled) {
        CSP_PRINTF_INFO("[ipc Mon : ipc:%lld, total:%lld]\n",
                        (mNumIpcOut - ipcMon.preIpcNum), ipcMon.preIpcNum);
        mNumIpcOut = ipcMon.preIpcNum;
    }
#endif
}

static void perfTimerFistCallback(uint32_t timerId, void *cookie)
{
    int i;

    CSP_PRINTF_INFO("perfTimer: skip first: idle:%d(CB:0x%x), ipc:%d\n",
		monInfo.idleMonEnabled, (u32)idleMon.CB, monInfo.ipcMonEnabled);

    idleMon.totalWfiTime = 0;
    for (i = 0; i < DFS_FREQ_MAX; i++)
        idleMon.perFreqWfiTime[i] = 0;

	idleMon.startTime = getTimeStampUS();
	if (monInfo.ipcMonEnabled || (monInfo.idleMonEnabled && idleMon.CB))
	    monInfo.perfTimerId =
	        timTimerSet((uint64_t) PERF_TIMEOUT_NS, 0, 0, perfTimerCallback, NULL, false);
	else
		monInfo.perfTimerEnabled = 0;
}

static void perfTimerCallbackEnable(bool on)
{
    if (on && !monInfo.perfTimerEnabled) {
        monInfo.perfTimerEnabled = 1;
        timTimerSet((uint64_t) PERF_TIMEOUT_NS / 10,
                    0, 0, perfTimerFistCallback, NULL, true);
    } else {
        if (monInfo.perfTimerEnabled) {
			/* disable perfTimer if both monitors are off */
            if (!monInfo.ipcMonEnabled && !monInfo.idleMonEnabled) {
                monInfo.perfTimerEnabled = 0;
                if (monInfo.perfTimerId) {
                    timTimerCancel(monInfo.perfTimerId);
                    monInfo.perfTimerId = 0;
                }
            }
        }
    }
}

#ifdef DEF_IPC_MONITOR
static void ipcMonitorEnable(bool on)
{
    monInfo.ipcMonEnabled = on;
    if (on) {
        mNumIpcOut = ipcMon.preIpcNum = 0;
        perfTimerCallbackEnable(1);
    } else {
        perfTimerCallbackEnable(0);
    }
}
#else
#define ipcMonitorEnable(a) ((void)0);
#endif

#ifdef DEF_IDLE_MONITOR
#if defined (DFS_SUPPORT)
extern uint32_t mDfsCurrFreq;
#endif
void idleMonitorSetEvt(enum chub_prof_id id)
{
    bool in;
    u64 wfiTime;

    if (id == chub_prof_wfi_in || id == chub_prof_wfi2_in)
        in = true;
    else {
        in = false;
    }

    if (in) {
        idleMon.inWfi = 1;
        idleMon.startWfiTime = getTimeStampUS();
#if defined (DFS_SUPPORT)
        idleMon.curFreq = mDfsCurrFreq;
#endif
    } else if (!in && idleMon.inWfi) {
        idleMon.inWfi = 0;
        wfiTime = getTimeStampUS() - idleMon.startWfiTime;
        idleMon.totalWfiTime += wfiTime;
#if defined (DFS_SUPPORT)
        if (idleMon.curFreq != mDfsCurrFreq)
            CSP_PRINTF_ERROR("ERROR: %s: invalid cpuFreq changed %d -> %d\n",
                __func__, (u32)idleMon.curFreq, (u32)mDfsCurrFreq);
#endif
        idleMon.perFreqWfiTime[idleMon.curFreq] += wfiTime;
#ifdef DEF_DEBUG_MODE
        if ((id != chub_prof_wfi2_out) && (id != chub_prof_wfi_out))
            CSP_PRINTF_INFO("%s: wakeup by %d\n", __func__, id);
#endif
    }
}

u64 idleMonitorEnable(bool on, u32 timeUs, idleMonitorCbkF cbk, void *data)
{
    u64 idleRatio = 0;
	int i;
	u64 activeTime = 0;

    if (on && !monInfo.idleMonEnabled) {
        monInfo.idleMonEnabled = on;
        idleMon.startTime = getTimeStampUS();
        if (timeUs) {
            idleMon.CB = cbk;
            idleMon.cbData = data;
        }
		/* enable first Callback to clear previous idle time */
		perfTimerCallbackEnable(1);
    } else if (!on && monInfo.idleMonEnabled) {
        monInfo.idleMonEnabled = on;
        if (timeUs) {
			idleMon.CB = NULL;
			idleMon.cbData = NULL;
            perfTimerCallbackEnable(0);
        } else {
			idleMon.lastTotalMonTime = getTimeStampUS() - idleMon.startTime;
			idleRatio = idleMonitorGetRatio();
            for (i = 0; i < DFS_FREQ_MAX; i++)
                 if (idleMon.perFreqWfiTime[i]) {
                     CSP_PRINTF_INFO("idleMon:%d:idle:%lld\n", i, idleMon.perFreqWfiTime[i] / 1000);
                     activeTime += idleMon.perFreqWfiTime[i];
                 }
            CSP_PRINTF_INFO("idleMon:total:%lld,idle:%lld(%lld),actRatio:%lld%%\n",
                idleMon.lastTotalMonTime / 1000, idleMon.totalWfiTime / 1000,
                activeTime / 1000, idleRatio);
        }
		idleMon.lastTotalMonTime = 0;
		idleMon.startTime = 0;
		idleMon.totalWfiTime = 0;
    }
    return idleRatio;
}
#else
#define idleMonitorEnable(a, b, c, d) ((void)0);
#endif
#else   // #if defined(DEF_IPC_MONITOR) || defined(DEF_IDLE_MONITOR)
#define idleMonitorEnable(a, b, c, d)
#define ipcMonitorEnable(a)
#endif

#ifdef DEF_IPC_DEBUG
static void ipc_hang_test(enum ipc_debug_event event)
{
    struct cipc_evt *ipc_evt_c2a = cipc_get_base(CIPC_REG_EVT_CHUB2AP);
    struct cipc_evt *ipc_evt_a2c = cipc_get_base(CIPC_REG_EVT_AP2CHUB);
    struct cipc_data *ipc_buf_c2a = cipc_get_base(CIPC_REG_DATA_CHUB2AP);
    struct cipc_data *ipc_buf_a2c = cipc_get_base(CIPC_REG_DATA_AP2CHUB);
    struct ipc_logbuf *logbuf = ipc_get_base(IPC_REG_LOG);
    char tmpbuf[16];
    int val;
	struct ipc_info *ipc = ipc_get_info();

    CSP_PRINTF_INFO("%s: with %d\n", __func__, event);
    msleep(100);
    switch (event) {
    case IPC_DEBUG_UTC_HANG_IPC_C2A_FULL:
        ipc_evt_c2a->ctrl.qctrl.eq = 3;
        ipc_evt_c2a->ctrl.qctrl.dq = 4;
        CSP_PRINTF_INFO("%s: c2a ipcfull. sent event\n", __func__);
        val = cipc_add_evt(CIPC_REG_EVT_CHUB2AP, IRQ_EVT_C2A_DEBUG);
        CSP_PRINTF_INFO("%s: c2a send evt ipcfull: val:%d\n", __func__, val);
        break;
    case IPC_DEBUG_UTC_HANG_IPC_C2A_CRASH:
        ipc_evt_c2a->ctrl.qctrl.eq = 1000;
        ipc_evt_c2a->ctrl.qctrl.dq = 4;
        CSP_PRINTF_INFO("%s: c2a flash. sent event\n", __func__);
        val = cipc_add_evt(CIPC_REG_EVT_CHUB2AP, IRQ_EVT_C2A_DEBUG);
        CSP_PRINTF_INFO("%s: c2a send evt ipcfull: val:%d\n", __func__, val);
        break;
    case IPC_DEBUG_UTC_HANG_IPC_C2A_DATA_FULL:
        ipc_buf_c2a->ctrl.qctrl.eq = 3;
        ipc_buf_c2a->ctrl.qctrl.dq = 4;
        CSP_PRINTF_INFO("%s: ipc_buf c2a full. sent data\n", __func__);
        cipc_write_data(CIPC_REG_DATA_CHUB2AP, tmpbuf, sizeof(tmpbuf));
        break;
    case IPC_DEBUG_UTC_HANG_IPC_C2A_DATA_CRASH:
        ipc_buf_c2a->ctrl.qctrl.eq = 3;
        ipc_buf_c2a->ctrl.qctrl.dq = 1000;
        CSP_PRINTF_INFO("%s: ipc_buf c2a crash (dq:1000). sent data\n",
                        __func__);
        cipc_write_data(CIPC_REG_DATA_CHUB2AP, tmpbuf, sizeof(tmpbuf));
        break;
    case IPC_DEBUG_UTC_HANG_IPC_A2C_FULL:
        CSP_PRINTF_INFO("%s: a2c ipc full\n", __func__);
        ipc_evt_a2c->ctrl.qctrl.eq = 3;
        ipc_evt_a2c->ctrl.qctrl.dq = 4;
        break;
    case IPC_DEBUG_UTC_HANG_IPC_A2C_CRASH:
        CSP_PRINTF_INFO("%s: a2c ipc crash\n", __func__);
        ipc_evt_a2c->ctrl.qctrl.eq = 3;
        ipc_evt_a2c->ctrl.qctrl.dq = 1000;
        break;
    case IPC_DEBUG_UTC_HANG_IPC_A2C_DATA_FULL:
        CSP_PRINTF_INFO("%s: a2c data ipc full\n", __func__);
        ipc_buf_a2c->ctrl.qctrl.eq = 3;
        ipc_buf_a2c->ctrl.qctrl.dq = 4;
        break;
    case IPC_DEBUG_UTC_HANG_IPC_A2C_DATA_CRASH:
        CSP_PRINTF_INFO("%s: a2c data ipc crash\n", __func__);
        ipc_buf_a2c->ctrl.qctrl.eq = 3;
        ipc_buf_a2c->ctrl.qctrl.dq = 1000;
        break;
    case IPC_DEBUG_UTC_HANG_IPC_LOGBUF_EQ_CRASH:
        CSP_PRINTF_INFO("%s:EQ_CRASH\n", __func__);
        logbuf->eq = 0x10000;
        CSP_PRINTF_INFO("%s:EQ_CRASH: %d, %d\n", __func__, logbuf->eq,
                        logbuf->dq);
        break;
    case IPC_DEBUG_UTC_HANG_IPC_LOGBUF_DQ_CRASH:
        CSP_PRINTF_INFO("%s: DQ_CRASH\n", __func__);
        logbuf->dq = 0x10000;
        CSP_PRINTF_INFO("%s:DQ_CRASH: %d, %d\n", __func__, logbuf->eq,
                        logbuf->dq);
        break;
    case IPC_DEBUG_UTC_HANG_INVAL_INT:
        CSP_PRINTF_INFO
            ("%s: invalid interrupt generation to AP. clear c2a evt\n",
             __func__);
        memset(ipc_evt_c2a->data, 0, sizeof(ipc_evt_c2a->data));
        ipc_hw_gen_interrupt(ipc->mb_base, ipc->opp_mb_id, 1);
		ipc_hw_gen_interrupt(ipc->mb_base, ipc->opp_mb_id, 2);
		ipc_hw_gen_interrupt(ipc->mb_base, ipc->opp_mb_id, 3);
		ipc_hw_gen_interrupt(ipc->mb_base, ipc->opp_mb_id, 4);
        break;
    default:
        break;
    }
}

bool mLoopbackTest;
#define CIPC_TEST_SIZE (64)
#define IPC_TEST_MAGIC (0xbb00)
#include "ipc_common.h"
static bool mLoopbackTestChubAbox;

static void debugSetIPCLoopback(enum ipc_debug_event event)
{
    int reg_val = ipc_read_value(IPC_VAL_A2C_DEBUG2);

    if (event == (enum ipc_debug_event)IPC_DEBUG_UTC_IPC_TEST_START) {
        enum cipc_region reg = (enum cipc_region)(reg_val & ((1 << CIPC_TEST_BAAW_REQ_BIT) -1 ));

        CSP_PRINTF_INFO(">>>>>%s: test on, reg_val:%x (t: %lld)\n", __func__, reg_val, getTimeStampUS());
        if (reg == CIPC_REG_DATA_ABOX2CHUB || reg == CIPC_REG_DATA_CHUB2ABOX) {
           CSP_PRINTF_INFO(">>%s: aud2chub on reg:%x (t: %lld)\n",
               __func__, (int)reg, getTimeStampUS());
           mLoopbackTestChubAbox = 1;
           cipc_add_evt(CIPC_REG_EVT_CHUB2ABOX, CIPC_LOOPBACK_EVT);
           cipc_add_evt(CIPC_REG_EVT_ABOX2CHUB, CIPC_LOOPBACK_EVT);
        }
        mLoopbackTest = 1;
		CSP_PRINTF_INFO(">%s: call cipc_loopback_test (t: %lld)\n", __func__, getTimeStampUS());
		cipc_loopback_test(reg_val, 1);
    } else if (event == (enum ipc_debug_event)IPC_DEBUG_UTC_IPC_TEST_END) {
        mLoopbackTest = 0;
        if (mLoopbackTestChubAbox) {
            enum cipc_region reg = (enum cipc_region)(reg_val & ((1 << CIPC_TEST_BAAW_REQ_BIT) -1 ));

            msleep(1000);
            CSP_PRINTF_INFO(">>%s: aud2chub off reg:%x\n", __func__, (int)reg);
            mLoopbackTestChubAbox = 0;
            cipc_add_evt(CIPC_REG_EVT_CHUB2ABOX, CIPC_LOOPBACK_EVT);
            cipc_add_evt(CIPC_REG_EVT_ABOX2CHUB, CIPC_LOOPBACK_EVT);
            CSP_PRINTF_INFO(">>%s: aud2chub call client_callback reg:%x\n",
				__func__, (int)reg);
#ifdef IPC_DEF_IPC_TEST_CB
            cipc_add_evt(CIPC_REG_EVT_CHUB2ABOX, (CIPC_LOOPBACK_EVT + 1));
            cipc_add_evt(CIPC_REG_EVT_ABOX2CHUB, (CIPC_LOOPBACK_EVT + 1));
            cipc_write_data(CIPC_REG_DATA_CHUB2ABOX, "ipc_client_test", 32);
            cipc_write_data(CIPC_REG_DATA_ABOX2CHUB, "ipc_client_test", 32);
#endif
        }
        CSP_PRINTF_INFO(">>>>>%s: test off, reg_val:%x\n", __func__, reg_val);
    }
}

void debugHandler(void)
{
    enum ipc_debug_event event =
		(enum ipc_debug_event)ipc_read_value(IPC_VAL_A2C_DEBUG);
    struct ipc_info *ipc = ipc_get_info();
#if defined (DFS_SUPPORT)
    uint32_t val;
#endif
    volatile uint64_t wdt_val = 1;
    unsigned int wtcon, wtdat;

    CSP_PRINTF_INFO("%s: event:%d/%d\n", __func__, event, IPC_DEBUG_UTC_IPC_TEST_START);

    switch (event) {
    case IPC_DEBUG_UTC_WDT:
        wtcon = __raw_readl(WDT_BASE_ADDRESS);
        wtdat = __raw_readl(WDT_BASE_ADDRESS + 4);
        CSP_PRINTF_INFO("%s: wdt test enter:%lld, 0x%x, 0x%x, \n", __func__, (unsigned long long)wdt_val,
		         wtcon, wtdat);
        while(wdt_val > 0) {
	    wdt_val++;
	    if (wdt_val == 10000000) {
                wtcon = __raw_readl(WDT_BASE_ADDRESS);
                wtdat = __raw_readl(WDT_BASE_ADDRESS + 4);
                CSP_PRINTF_INFO("%s: wdt test reload:%lld, 0x%x, 0x%x\n", __func__, (unsigned long long)wdt_val,
		                 wtcon, wtdat);
                wdt_val = 1;
	    }
        };
        CSP_PRINTF_INFO("%s: wdt test bye:%lld\n", __func__, (unsigned long long)wdt_val);
        break;
    case IPC_DEBUG_UTC_STOP:
        ipcMonitorEnable(0);
        idleMonitorEnable(0, 0, NULL, NULL);
        perfMonitorEnable(0, 1);
        break;
    case IPC_DEBUG_UTC_CHECK_CPU_UTIL:
        ipcMonitorEnable(1);
        perfMonitorEnable(1, 1);
        break;
    case IPC_DEBUG_UTC_IDLE:
        idleMonitorEnable(1, 0, NULL, NULL);
        break;
    case IPC_DEBUG_UTC_IPC_TEST_START:
    case IPC_DEBUG_UTC_IPC_TEST_END:
        debugSetIPCLoopback(event);
        break;
    case IPC_DEBUG_DUMP_STATUS:
        CSP_PRINTF_INFO("%s: dump ipc\n", __func__);
        ipc_dump();
        break;
#if defined (DFS_SUPPORT)
    case IPC_DEBUG_DFS_GOVERNOR:
        val = ipc_read_value(IPC_VAL_A2C_DFS);
        CSP_PRINTF_INFO("%s: select dfs governor %lu\n", __func__, val);
        dfsSelectGovernor(val);
        break;
    case IPC_DEBUG_UTC_DFS:
        CSP_PRINTF_INFO("%s: dfs\n", __func__);
        dfsDebugPrint();
        break;
#endif
    case IPC_DEBUG_CLK_DIV:
        CSP_PRINTF_INFO("%s: clk req: %d\n", __func__, ipc_get_chub_clk());
        //cmuSetCpuClk(ipc_get_chub_clk());
#if defined (ALIVE_DVS_SUPPORT)
        mailboxApmReqDvsFreq(ipc_get_chub_clk());
#endif
	break;
    case IPC_DEBUG_UTC_ASSERT:
        CSP_PRINTF_INFO("%s: Assert\n", __func__);
        CSP_ASSERT(0);
        break;
    case IPC_DEBUG_UTC_FAULT:
        CSP_PRINTF_INFO("%s: Fault\n", __func__);
        CSP_FAULT(0);
        break;
    case IPC_DEBUG_UTC_HANG:
        CSP_PRINTF_INFO("%s: panic\n", __func__);
        CSP_PANIC(0);
        break;
    case IPC_DEBUG_UTC_HANG_ITMON:
        CSP_PRINTF_INFO("%s: itmon\n", __func__);
        memset((void *)0xfffffffe, 0, 1024);
        break;
    case IPC_DEBUG_UTC_REBOOT:
        CSP_PRINTF_INFO("%s: reboot\n", __func__);
        CSP_REBOOT(0);
        break;
    case IPC_DEBUG_UTC_HANG_IPC_C2A_FULL:
    case IPC_DEBUG_UTC_HANG_IPC_C2A_CRASH:
    case IPC_DEBUG_UTC_HANG_IPC_C2A_DATA_FULL:
    case IPC_DEBUG_UTC_HANG_IPC_C2A_DATA_CRASH:
    case IPC_DEBUG_UTC_HANG_IPC_A2C_FULL:
    case IPC_DEBUG_UTC_HANG_IPC_A2C_CRASH:
    case IPC_DEBUG_UTC_HANG_IPC_A2C_DATA_FULL:
    case IPC_DEBUG_UTC_HANG_IPC_A2C_DATA_CRASH:
    case IPC_DEBUG_UTC_HANG_IPC_LOGBUF_EQ_CRASH:
    case IPC_DEBUG_UTC_HANG_IPC_LOGBUF_DQ_CRASH:
    case IPC_DEBUG_UTC_HANG_INVAL_INT:
        ipc_hang_test(event);
        break;
    default:
        CSP_PRINTF_INFO("%s: event:%d doesn't supported.\n", __func__, event);
        break;
    }
    ipc_hw_gen_interrupt(ipc->mb_base, ipc->opp_mb_id, IRQ_NUM_CHUB_LOG);
}
#endif
