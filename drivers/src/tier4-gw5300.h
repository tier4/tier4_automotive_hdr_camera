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

#define GW5300_SLAVE_MODE_10FPS 1
#define GW5300_MASTER_MODE_10FPS 2
#define GW5300_MASTER_MODE_30FPS 3
#define GW5300_SLAVE_MODE_20FPS 4
#define GW5300_MASTER_MODE_20FPS 5
#define GW5300_SLAVE_MODE_30FPS 6

/**
 * get gw5300 prim slave address.
 *
 * @param  [out]  g_ctx   global context.
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_gw5300_prim_slave_addr(struct gmsl_link_ctx *g_ctx);

/**
 * Sets up the ISP device for a specified sensor mode.
 *
 * @param  [in]  dev   The ISP device handle.
 * @param  [in]  sensor_mode The Sensor mode.
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_gw5300_setup_sensor_mode(struct device *dev, int sensor_mode);

// int tier4_gw5300_check_device(struct device *dev, u8 *wdata, int wdata_size
//								, u8 *rdata, int rdata_size, u32 target_addr );

int tier4_gw5300_check_device(struct device *dev, u8 *rdata, int rdata_size);

#endif /* __TIER4_GW5300_H__ */
