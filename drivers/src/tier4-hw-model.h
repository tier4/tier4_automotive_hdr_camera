/*
 * Copyright (c) 2022, TIERIV INC.  All rights reserved.
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
 * <b>Definition for Hardware Model Information </b>
 *
 * @b Description: Defines hardware model
 */

#ifndef __TIER4_HW_MODEL_H__
#define __TIER4_HW_MODEL_H__

#define	STR_DTB_MODEL_NAME_ORIN			"ORIN"
#define	STR_DTB_MODEL_NAME_XAVIER		"JETSON-AGX"
#define	STR_DTB_MODEL_NAME_ROSCUBE		"ROSCUBE"

#define	HW_MODEL_NVIDIA_ORIN_DEVKIT		1
#define	HW_MODEL_NVIDIA_XAVIER_DEVKIT	2
#define	HW_MODEL_ADLINK_ROSCUBE			3
#define	HW_MODEL_UNKNOWN				0

#endif // __TIER4_HW_MODEL_H__
