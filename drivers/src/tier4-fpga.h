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
 * @param  [in] frequency   fsync frequency in Hz ( 1 to 255 )
 *
 * @return  0 for success, or -1 otherwise.
 */

int tier4_fpga_set_fsync_signal_frequency(struct device *dev, int des_number, int freequency);

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

#endif /* __TIER4_FPGA_H__ */
