/*
 * Copyright (c) 2022, TIERIV INC.  All rights reserved.
 */

/**
 * @file
 * <b>ISX021 register access control API : For ISX021 sensor.</b>
 *
 * @b Description: Defines elements used to set up and use a
 *  Maxim Integrated MAX9295 serializer.
 */

#ifndef __TIER4_ISX021_CTRL_H__
#define __TIER4_ISX021_CTRL_H__

//#include <media/tegracam_core.h>
#include "tier4-isx021.h"

int tier4_isx021_ctrl_set_response_mode(void *p_s_data);

/**
 * \defgroup tier4_isx021_ctrl driver
 *
 * Set trigger mode for ISX021 sensor module.
 *
 * @ingroup isx021_ctrl_group
 * @{
 */

int tier4_isx021_ctrl_set_fsync_trigger_mode(void *p_s_data);

/**
 * \defgroup tier4_isx021_ctrl driver
 *
 * Set gain for the ISX021 sensor module.
 *
 * @ingroup isx021_ctrl_group
 * @{
 */

int tier4_isx021_ctrl_set_gain(void *p_s_data, s64 val);

/**
 * \defgroup tier4_isx021_ctrl driver
 *
 * Set Auto Exposure for the ISX021 sensor module.
 *
 * @ingroup isx021_ctrl_group
 * @{
 */

int tier4_isx021_ctrl_set_auto_exposure(void *p_s_data);

/**
 * \defgroup tier4_isx021_ctrl driver
 *
 * Set Exposure for the ISX021 sensor module.
 *
 * @ingroup isx021_ctrl_group
 * @{
 */

int tier4_isx021_ctrl_set_exposure(void *p_s_data, s64 val);

/**
 * \defgroup tier4_isx021_ctrl driver
 *
 * Enable correct dsitortion for the ISX021 sensor module.
 *
 * @ingroup isx021_ctrl_group
 * @{
 */

int tier4_isx021_ctrl_enable_distortion_correction(void *p_s_data, bool val);

#endif  // __TIER4_ISX021_CTRL_H__
