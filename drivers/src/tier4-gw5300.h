/*
 * Copyright (c) 2022, Tier4  All rights reserved.
 *
 */

/**
 * @file
 * <b>GW5300 API: For Tier4 GW5300 ISP.</b>
 *
 * @b Description: Defines elements used to set up and use a
 *  GW5300 ISP.
 */

#ifndef __TIER4_GW5300_H__
/**
 * \defgroup Tier4 GW5300 ISP
 *
 * Defines the interface used to control the TIER4_GW5300 ISP.
 *
 */

#define __TIER4_GW5300_H__

#define GW5300_MASTER_MODE_10FPS 0
#define GW5300_SLAVE_MODE_10FPS  1
#define GW5300_MASTER_MODE_20FPS 2
#define GW5300_SLAVE_MODE_20FPS  3
#define GW5300_MASTER_MODE_30FPS 4
#define GW5300_SLAVE_MODE_30FPS  5
#define GW5300_SLAVE_MODE_10FPS_SLOW  6
#define GW5300_MASTER_MODE_10FPS_SLOW 7
#define GW5300_MASTER_MODE_5FPS 8
#define GW5300_SLAVE_MODE_5FPS 9
#define GW5300_MASTER_MODE_10FPS_EBD 10
#define GW5300_SLAVE_MODE_10FPS_EBD  11
#define GW5300_MASTER_MODE_20FPS_EBD 12
#define GW5300_SLAVE_MODE_20FPS_EBD  13
#define GW5300_MASTER_MODE_30FPS_EBD 14
#define GW5300_SLAVE_MODE_30FPS_EBD  15
#define GW5300_MODE_MAX 16

const char * const gw5300_mode_name[] = {
    [GW5300_MASTER_MODE_10FPS] = "Master mode 10FPS",
    [GW5300_SLAVE_MODE_10FPS] = "Slave mode 10FPS",
    [GW5300_MASTER_MODE_20FPS] = "Master mode 20FPS",
    [GW5300_SLAVE_MODE_20FPS] = "Slave mode 20FPS",
    [GW5300_MASTER_MODE_30FPS] = "Master mode 30FPS",
    [GW5300_SLAVE_MODE_30FPS] = "Slave mode 30FPS",
    [GW5300_MASTER_MODE_10FPS_SLOW] = "Master mode 10FPS Slow",
    [GW5300_SLAVE_MODE_10FPS_SLOW] = "Slave mode 10FPS Slow",
    [GW5300_MASTER_MODE_5FPS] = "Master mode 5FPS",
    [GW5300_SLAVE_MODE_5FPS] = "Slave mode 5FPS",
    [GW5300_MASTER_MODE_10FPS_EBD] = "Master mode 10FPS with EBD",
    [GW5300_SLAVE_MODE_10FPS_EBD] = "Slave mode 10FPS with EBD",
    [GW5300_MASTER_MODE_20FPS_EBD] = "Master mode 20FPS with EBD",
    [GW5300_SLAVE_MODE_20FPS_EBD] = "Slave mode 20FPS with EBD",
    [GW5300_MASTER_MODE_30FPS_EBD] = "Master mode 30FPS with EBD",
    [GW5300_SLAVE_MODE_30FPS_EBD] = "Slave mode 30FPS with EBD",
};

/**
 * get gw5300 prim slave address.
 *
 * @param  [out]  g_ctx   global context.
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_gw5300_prim_slave_addr(struct tier4_gmsl_link_ctx *g_ctx);

/**
 * Sets up the ISP device for a specified sensor mode.
 *
 * @param  [in]  dev   The ISP device handle.
 * @param  [in]  sensor_mode The Sensor mode.
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_gw5300_setup_sensor_mode(struct device *dev, int sensor_mode);

/**
 * Sets up the ISP device in C3 camera for a specified sensor mode.
 *
 * @param  [in]  dev   The ISP device handle.
 * @param  [in]  sensor_mode The Sensor mode.
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_gw5300_c3_setup_sensor_mode(struct device *dev, int sensor_mode);

/**
 * Sets Max/Min integration time for AE mode.
 *
 * @param  [in]  dev   The ISP device handle.
 * @param  [in]  max integration time.
 * @param  [in]  min integration time.
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_gw5300_c2_set_integration_time_on_aemode(struct device *dev, int trigger_mode, u32 max_integration_time, u32 min_integration_time);

int tier4_gw5300_c3_set_integration_time_on_aemode(struct device *dev, int trigger_mode, u32 max_integration_time, u32 min_integration_time);

int tier4_gw5300_set_distortion_correction(struct device *dev, bool val);

int tier4_gw5300_c3_set_distortion_correction(struct device *dev, bool val);

int tier4_gw5300_c3_set_auto_exposure(struct device *dev, bool val);

int tier4_gw5300_check_device(struct device *dev, u8 *rdata, int rdata_size);

#endif /* __TIER4_GW5300_H__ */
