/*
 * Copyright (c) 2022, Tier4  All rights reserved.
 *
 */

#include "tier4-gmsl-link.h"

/**
 * @file
 * <b>FPGA API: For Tier4 FPGA.</b>
 *
 * @b Description: Defines elements used to set up and use a
 *  FPGA.
 */

#ifndef __TIER4_FPGA_H__
/**
 * \defgroup Tier4 FPGA
 *
 * Defines the interface used to control the TIER4_FPGA ISP.
 *
 */

#define __TIER4_FPGA_H__

#define FPGA_REG_MODE_ADDR  0x04
#define FPGA_MODE_FREE_RUN  0xF0  // camera master mode, default 30 FPS
#define FPGA_MODE_FSYNC     0xFF  // camera slave mode, triggered by FSYNC signal

#define FPGA_REG_FSYNC_TRIG_ADDR  0x05
#define FPGA_FSYNC_MANUAL         0x00  // manually triggered by external GPIO
#define FPGA_FSYNC_AUTO           0xF0  // automatically triggered by FPGA itself

#define FPGA_FSYNC_MODE_DISABLE 0
#define FPGA_FSYNC_MODE_AUTO    1
#define FPGA_FSYNC_MODE_MANUAL  2

#define NO_ERROR 0

#define FPGA_REG_FREQ_BASE_ADDR   0x08

#define FPGA_SLAVE_MODE_10FPS 10
#define FPGA_SLAVE_MODE_30FPS 30

/**
 *  Enable fpga generate fsync signal for slave mode
 *
 * @param  [in] dev   pointer to fpga device.
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_fpga_enable_generate_fsync_signal(struct device *dev);

/**
 *  Disable fpga generate fsync signal for slave mode
 *
 * @param  [in] dev   pointer to fpga device.
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_fpga_disable_generate_fsync_signal(struct device *dev);

/**
 *  set frequency of the fsync signal pulse
 *
 * @param  [in] dev         pointer to fpga device structure.
 * @param  [in] des_number  des number ( 1 to 4 )
 * @param  [in] trigger_mode  sensor master/slave mode with fps
 *
 * @return  0 for success, or -1 otherwise.
 */

/**
 *  get which fsync mode is used
 *
 * @param  [in] dev   pointer to fpga device.
 *
 * @return  0 for disabled, 1 for enable auto mode, 2 for enable manual mode
 */

int tier4_fpga_get_fsync_mode(void);

/**
 *  Enable fpga generate fsync signal for slave mode
 *
 * @return  0 for disabled, or 1 enabled.
 */

int tier4_fpga_enable_fsync_mode(struct device *dev);

/**
 *  Disable fpga generate fsync signal for slave mode
 *
 * @param  [in] dev   pointer to fpga device.
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_fpga_disable_fsync_mode(struct device *dev);

/**
 *  set fpga to automatically generate fsync signal
 *
 * @param  [in] dev   pointer to fpga device.
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_fpga_set_fsync_auto_trigger(struct device *dev);

/**
 *  set fpga to manually generate fsync signal
 *
 * @param  [in] dev   pointer to fpga device.
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_fpga_set_fsync_manual_trigger(struct device *dev);

/**
 *  set frequency of the fsync signal pulse
 *
 * @param  [in] dev         pointer to fpga device structure.
 * @param  [in] des_number  des number ( 0 to 3 )
 * @param  [in] trigger_mode sensor master/slave mode with fps (  C1: 0 to 1  C2: 0 to 5)
 * @param  [in] sensor_id   which sensor (21, 490, 728) is chosen
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_fpga_set_fsync_signal_frequency(struct device *dev, int des_number, int trigger_mode, __u32 sensor_id);

/**
 *  check access to FPGA.
 *
 * @param  [in] dev   pointer to fpga device structure.
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_fpga_check_access(struct device *dev);

/**
 *  get fpga slave address
 *
 * @param  [in] dev         pointer to fpga device structure.
 * @param  [in/out] g_ctx   pointer to global context structure
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_fpga_get_slave_addr(struct device *dev, struct tier4_gmsl_link_ctx *g_ctx);

/**
 * @brief power on deserializer by writing i2c commands to FPGA
 * 
 * @param dev           pointer to fpga device structure.
 * @param des_number    deserializer number ( 0 to 3 )
 * @return 0 for success, or -1 otherwise.
 */
int tier4_fpga_power_on_deserializer(struct device *dev, int des_number);

/**
 * @brief power off deserializer by writing i2c commands to FPGA
 * 
 * @param dev           pointer to fpga device structure.
 * @param des_number    deserializer number ( 0 to 3 )
 * @return 0 for success, or -1 otherwise. 
 */
int tier4_fpga_power_off_deserializer(struct device *dev, int des_number);

#endif /* __TIER4_FPGA_H__ */
