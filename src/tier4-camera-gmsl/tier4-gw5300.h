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

#include <linux/device.h>
#include <media/v4l2-ctrls.h>

#define GW5300_MASTER_MODE_10FPS 0
#define GW5300_SLAVE_MODE_10FPS 1
#define GW5300_MASTER_MODE_20FPS 2
#define GW5300_SLAVE_MODE_20FPS 3
#define GW5300_MASTER_MODE_30FPS 4
#define GW5300_SLAVE_MODE_30FPS 5
#define GW5300_SLAVE_MODE_10FPS_SLOW 6
#define GW5300_MASTER_MODE_10FPS_SLOW 7
#define GW5300_MASTER_MODE_5FPS 8
#define GW5300_SLAVE_MODE_5FPS 9
#define GW5300_MASTER_MODE_10FPS_EBD 10
#define GW5300_SLAVE_MODE_10FPS_EBD 11
#define GW5300_MASTER_MODE_20FPS_EBD 12
#define GW5300_SLAVE_MODE_20FPS_EBD 13
#define GW5300_MASTER_MODE_30FPS_EBD 14
#define GW5300_SLAVE_MODE_30FPS_EBD 15
#define GW5300_MODE_MAX 16

const char *const gw5300_mode_name[] = {
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

int tier4_gw5300_c2_set_integration_time_on_aemode(struct device *dev,
						   int trigger_mode,
						   u32 max_integration_time,
						   u32 min_integration_time);

int tier4_gw5300_c3_set_integration_time_on_aemode(struct device *dev,
						   int trigger_mode,
						   u32 max_integration_time,
						   u32 min_integration_time);

int tier4_gw5300_set_readout_delay(struct device *dev, int readout_delay_us, u32 h_line_ns);

int tier4_gw5300_set_distortion_correction(struct device *dev, bool val);

int tier4_gw5300_c3_set_distortion_correction(struct device *dev, bool val);

int tier4_gw5300_c3_set_auto_exposure(struct device *dev, bool val);

int tier4_gw5300_check_device(struct device *dev, u8 *rdata, int rdata_size);

int tier4_gw5300_c2_test_hw_fault(struct device *dev, bool enable);

/*
 * GW5300 ISP scalar parameter access (see the isp_parameter_bridge spec).
 *
 * spec_id / context identify the parameter spec and the ISP context; they are
 * fixed for the supported ISP firmware (matching the bridge defaults).
 */
#define TIER4_GW5300_ISP_SPEC_ID 5
#define TIER4_GW5300_ISP_CONTEXT 0

/* Scalar parameter types; the value determines the on-wire byte width. */
enum tier4_gw5300_param_type {
	TIER4_GW5300_PARAM_UINT8 = 0,
	TIER4_GW5300_PARAM_INT8,
	TIER4_GW5300_PARAM_UINT16,
	TIER4_GW5300_PARAM_INT16,
	TIER4_GW5300_PARAM_UINT32,
	TIER4_GW5300_PARAM_INT32,
	TIER4_GW5300_PARAM_FLOAT,
	TIER4_GW5300_PARAM_TYPE_MAX,
};

/*
 * Value of the "ISP Parameter" V4L2 control. This is a driver-private
 * compound control type (see tier4_gw5300_isp_param_type_ops); the control's
 * value is one of these structs. Multi-byte fields are little-endian, and
 * @value holds the raw bits (an int or an IEEE-754 float, per @param_type).
 */
struct tier4_gw5300_isp_param {
	__le16 param_id;
	__u8 param_type; /* enum tier4_gw5300_param_type */
	__u8 reserved;
	__le32 value;
} __packed;

/*
 * Private V4L2 compound control type for struct tier4_gw5300_isp_param. The id
 * is in the vendor range (>= V4L2_CTRL_COMPOUND_TYPES, clear of the upstream
 * compound types) and is paired with tier4_gw5300_isp_param_type_ops.
 */
#define TIER4_GW5300_CTRL_TYPE_ISP_PARAM 0xf000

extern const struct v4l2_ctrl_type_ops tier4_gw5300_isp_param_type_ops;

int tier4_gw5300_isp_set_param(struct device *dev, u8 spec_id, u8 context,
			       u16 param_id, u8 param_type, u32 value);

int tier4_gw5300_isp_get_param(struct device *dev, u8 spec_id, u8 context,
			       u16 param_id, u8 param_type, u32 *value);

int tier4_gw5300_set_reverse(struct device *dev, int v_reverse, int h_reverse);

#endif /* __TIER4_GW5300_H__ */
