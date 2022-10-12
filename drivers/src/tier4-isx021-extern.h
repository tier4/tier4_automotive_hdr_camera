/*
 * Copyright (c) 2018-2020, NVIDIA Corporation.  All rights reserved.
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
 * <b>MAX9296 API: For Maxim Integrated MAX9295 deserializer.</b>
 *
 * @b Description: Defines the functions for other modules
 *  ISX021 Sensor driver.
 */

#ifndef __TIER4_ISX021_EXTREN_H__
#define __TIER$_ISX021_EXTERN_H__

/**
 * @brief  mutex lock for sensor.
 *
 *
 * @param  none
 *
 * @return  N/A
 */
void tier4_isx021_sensor_mutex_lock(void);

/**
 * @brief  mutex unlock for sensor.
 *
 *
 * @param   none
 *
 * @return  N/A
 */
void tier4_isx021_sensor_mutex_unlock(void);


/** @} */

#endif  /* __TIER4_ISX021_EXTERN_H__ */
