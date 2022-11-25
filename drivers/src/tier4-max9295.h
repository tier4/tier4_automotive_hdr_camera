/*
 * Copyright (c) 2022, TIERIV INC.  All rights reserved.
 * Copyright (c) 2018, NVIDIA Corporation.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * <b>MAX9295 API: For Maxim Integrated MAX9295 serializer.</b>
 *
 * @b Description: Defines elements used to set up and use a
 *  Maxim Integrated MAX9295 serializer.
 */

#ifndef __TIER4_MAX9295_H__
#define __TIER4_MAX9295_H__

#include "tier4-gmsl-link.h"
/**
 * \defgroup tier4_max9295 MAX9295 serializer driver
 *
 * Controls the MAX9295 serializer module.
 *
 * @ingroup serdes_group
 * @{
 */

int tier4_max9295_setup_gpo(struct device *dev);

/**
 * @brief  Powers on a serializer device and performs the I2C overrides
 * for sensor and serializer devices.
 *
 * The I2C overrides include setting proxy I2C slave addresses for the devices.
 *
 * Before the client calls this function it must ensure that
 * the deserializer device is in link_ex exclusive link mode
 * by calling the deserializer driver's max9296_setup_link() function.
 *
 * @param  [in]  dev            The serializer device handle.
 *
 * @return  0 for success, or -1 otherwise.
 */
int tier4_max9295_setup_control(struct device *dev);

/**
 * Reverts I2C overrides and resets a serializer device.
 *
 * @param  [in]  dev            The serializer device handle.
 *
 * @return  0 for success, or -1 otherwise.
 */
int tier4_max9295_reset_control(struct device *dev);

/**
 * @brief  Pairs a sensor device with a serializer device.
 *
 * To be called by sensor client driver.
 *
 * @param  [in]  dev            The deserializer device handle.
 * @param  [in]  g_ctx          The @ref gmsl_link_ctx structure handle.
 *
 * @return  0 for success, or -1 otherwise.
 */
int tier4_max9295_sdev_pair(struct device *dev, struct gmsl_link_ctx *g_ctx);

/**
 * @brief Unpairs a sensor device from a serializer device.
 *
 * To be called by sensor client driver.
 *
 * @param  [in]  dev            The serializer device handle.
 * @param  [in]  s_dev          The sensor device handle.
 *
 * @return  0 for success, or -1 otherwise.
 */
int tier4_max9295_sdev_unpair(struct device *dev, struct device *s_dev);

/**
 * Power up the camera sensor from serializer.
 *
 * @param  [in]  dev            The serializer device handle.
 * @param  [in]  sensor_id      Sensor ID.  21 : ISX021 490 : IMX490
 * @param  [in]  power_on       Power on camera sensor : true    Power off camera sensor : 0
 *
 * @return  0 for success, or -1 otherwise.
 */
#if 0 
int tier4_max9295_power_up_camera_sensor(struct device *dev);
#else
int tier4_max9295_control_sensor_power_seq(struct device *dev, __u32 sensor_id, bool power_on);
#endif
/**
 * Pre-sets up the serializer device's internal pipeline for a specified
 * sensor/serializer pair.
 *
 * @param  [in]  dev            The serializer device handle.
 *
 * @return  0 for success, or -1 otherwise.
 */
int tier4_max9295_pre_setup_streaming(struct device *dev);

/**
 * Sets up the serializer device's internal pipeline for a specified
 * sensor/serializer pair.
 *
 * @param  [in]  dev            The serializer device handle.
 *
 * @return  0 for success, or -1 otherwise.
 */
int tier4_max9295_setup_streaming(struct device *dev);

#endif  // __TIER4_MAX9295_H__
