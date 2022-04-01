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
 * <b>ISX021 register access control API : For ISX021 sensor.</b>
 *
 * @b Description: Defines elements used to set up and use a
 *  Sony ISX021 Camera Sensor.
 */

#ifndef __TIER4_ISX021_H__
#define __TIER4_ISX021_H__

//#include <media/gmsl-link.h>
//#include "tier4-gmsl-link.h"
//#include <media/tegracam_core.h>

/**
 * \defgroup tier4_isx021_driver
 *
 * Read ISX021 register.
 *
 * @ingroup isx021_group
 * @{
 */
void micro_sec_sleep(unsigned int u_time);
/**
 * \defgroup tier4_isx021_driver
 *
 * sleep for micro seconds
 *
 * @ingroup isx021_group
 * @{
 */

int tier4_isx021_read_reg_wrapper(void *p_s_data, u16 reg_addr, u8 *val);

/**
 * \defgroup tier4_isx021_driver
 *
 * Write value to ISX021 register.
 *
 * @ingroup isx021_group
 * @{
 */
int tier4_isx021_write_reg_wrapper(void *p_s_data, u16 reg_addr, u8 val);

#endif  // __TIER4_ISX021_H__
