
/*
 * Copyright (C) 2017 Samsung Electronics Co., Ltd.
 *
 * Contact: Boojin Kim <boojin.kim@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEF_IPC_DEBUG
void debugHandler(void);
#else
#define debugHandler() ((void)0)
#endif

#ifdef DEF_PERF_MONITOR
enum chub_prof_sens_evt {
    chub_prof_sens_evt_poweron,
    chub_prof_sens_evt_poweron_done,
    chub_prof_sens_evt_setfreq,
    chub_prof_sens_evt_setfreq_done,
    chub_prof_sens_evt_timer,
    chub_prof_sens_evt_cb,
    chub_prof_sens_evt_isr,
    chub_prof_sens_evt_isr_out,
};
void perfMonitorSetSensEvt(u32 sensType, enum chub_prof_sens_evt act);
#endif

enum chub_prof_id {
    chub_prof_wfi_in,
    chub_prof_wfi_out,
    chub_prof_wfi2_in,
    chub_prof_wfi2_out,
    chub_prof_ipc_rd_in,
    chub_prof_ipc_rd_out,
    chub_prof_ipc_wt_in,
    chub_prof_ipc_wt_out,
    chub_prof_ipc_wt_evt_in,
    chub_prof_ipc_wt_evt_out,
    chub_prof_i2c_in,
    chub_prof_i2c_out,
    chub_prof_i2c_irq,
    chub_prof_i2c_irq_out,
    chub_prof_spi_in,
    chub_prof_spi_out,
    chub_prof_spi_irq,
    chub_prof_spi_irq_out,
    chub_prof_gpio_isr,
    chub_prof_ipc_isr,
#ifdef DEF_PERF_MONITOR
    /* acc */
    chub_prof_sens_acc_poweron,
    chub_prof_sens_acc_poweron_done =
        chub_prof_sens_acc_poweron + chub_prof_sens_evt_poweron_done,
    chub_prof_sens_acc_setfreq =
        chub_prof_sens_acc_poweron + chub_prof_sens_evt_setfreq,
    chub_prof_sens_acc_setfreq_done =
        chub_prof_sens_acc_poweron + chub_prof_sens_evt_setfreq_done,
    chub_prof_sens_acc_timer =
        chub_prof_sens_acc_poweron + chub_prof_sens_evt_timer,
    chub_prof_sens_acc_cb = chub_prof_sens_acc_poweron + chub_prof_sens_evt_cb,
    /* mag */
    chub_prof_sens_mag_poweron,
    chub_prof_sens_mag_poweron_done =
        chub_prof_sens_mag_poweron + chub_prof_sens_evt_poweron_done,
    chub_prof_sens_mag_setfreq =
        chub_prof_sens_mag_poweron + chub_prof_sens_evt_setfreq,
    chub_prof_sens_mag_setfreq_done =
        chub_prof_sens_mag_poweron + chub_prof_sens_evt_setfreq_done,
    chub_prof_sens_mag_timer =
        chub_prof_sens_mag_poweron + chub_prof_sens_evt_timer,
    chub_prof_sens_mag_cb = chub_prof_sens_mag_poweron + chub_prof_sens_evt_cb,
    /* gyro */
    chub_prof_sens_gyro_poweron,
    chub_prof_sens_gyro_poweron_done =
        chub_prof_sens_gyro_poweron + chub_prof_sens_evt_poweron_done,
    chub_prof_sens_gyro_setfreq =
        chub_prof_sens_gyro_poweron + chub_prof_sens_evt_setfreq,
    chub_prof_sens_gyro_setfreq_done =
        chub_prof_sens_gyro_poweron + chub_prof_sens_evt_setfreq_done,
    chub_prof_sens_gyro_timer =
        chub_prof_sens_gyro_poweron + chub_prof_sens_evt_timer,
    chub_prof_sens_gyro_cb =
        chub_prof_sens_gyro_poweron + chub_prof_sens_evt_cb,
    /* baro */
    chub_prof_sens_baro_poweron,
    chub_prof_sens_baro_poweron_done =
        chub_prof_sens_baro_poweron + chub_prof_sens_evt_poweron_done,
    chub_prof_sens_baro_setfreq =
        chub_prof_sens_baro_poweron + chub_prof_sens_evt_setfreq,
    chub_prof_sens_baro_setfreq_done =
        chub_prof_sens_baro_poweron + chub_prof_sens_evt_setfreq_done,
    chub_prof_sens_baro_timer =
        chub_prof_sens_baro_poweron + chub_prof_sens_evt_timer,
    chub_prof_sens_baro_cb =
        chub_prof_sens_baro_poweron + chub_prof_sens_evt_cb,

    /* prox */
    chub_prof_sens_prox_poweron,
    chub_prof_sens_prox_poweron_done =
        chub_prof_sens_prox_poweron + chub_prof_sens_evt_poweron_done,
    chub_prof_sens_prox_setfreq =
        chub_prof_sens_prox_poweron + chub_prof_sens_evt_setfreq,
    chub_prof_sens_prox_setfreq_done =
        chub_prof_sens_prox_poweron + chub_prof_sens_evt_setfreq_done,
    chub_prof_sens_prox_timer =
        chub_prof_sens_prox_poweron + chub_prof_sens_evt_timer,
    chub_prof_sens_prox_cb =
        chub_prof_sens_prox_poweron + chub_prof_sens_evt_cb,
    chub_prof_sens_prox_isr =
        chub_prof_sens_prox_poweron + chub_prof_sens_evt_isr,
    chub_prof_sens_prox_isr_out =
        chub_prof_sens_prox_poweron + chub_prof_sens_evt_isr_out,

    /* als */
    chub_prof_sens_als_poweron,
    chub_prof_sens_als_poweron_done =
        chub_prof_sens_als_poweron + chub_prof_sens_evt_poweron_done,
    chub_prof_sens_als_setfreq =
        chub_prof_sens_als_poweron + chub_prof_sens_evt_setfreq,
    chub_prof_sens_als_setfreq_done =
        chub_prof_sens_als_poweron + chub_prof_sens_evt_setfreq_done,
    chub_prof_sens_als_timer =
        chub_prof_sens_als_poweron + chub_prof_sens_evt_timer,
    chub_prof_sens_als_cb = chub_prof_sens_als_poweron + chub_prof_sens_evt_cb,
    chub_prof_sens_als_isr =
        chub_prof_sens_als_poweron + chub_prof_sens_evt_isr,
    chub_prof_sens_als_isr_out =
        chub_prof_sens_als_poweron + chub_prof_sens_evt_isr_out,
    chub_prof_max
#endif
};

typedef void (*idleMonitorCbkF) (void *data, u64 *idleUs, u64 *totalUs);
#define PERF_TIMEOUT_NS (1000000000)
#define PERF_TIMEOUT_US (PERF_TIMEOUT_NS / 1000)

#ifdef DEF_IDLE_MONITOR
u64 idleMonitorEnable(bool on, u32 timerUs, idleMonitorCbkF cbk, void *data);
void idleMonitorSetEvt(enum chub_prof_id id);
#else
#define idleMonitorEnable(a, b, c, d)
#define idleMonitorSetEvt(a)
#endif

#if defined(DEF_PERF_MONITOR) || defined(DEF_IDLE_MONITOR)
void perfMonitorSetEvt(enum chub_prof_id id);
#else
#define perfMonitorSetEvt(a) ((void)0)
#endif
#endif
