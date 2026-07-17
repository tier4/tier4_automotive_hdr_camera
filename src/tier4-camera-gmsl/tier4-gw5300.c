/*
 * tier4_gw5300.c - tier4_gw5300 ISP driver
 *
 * Copyright (c) 2022-2023, TIER IV Inc.  All rights reserved.
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

#include <linux/module.h>
#include <linux/string.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <media/camera_common.h>

#include "tier4-gmsl-link.h"
#include "tier4-gw5300.h"

struct tier4_gw5300 {
	struct i2c_client *i2c_client;
	struct tier4_gmsl_link_ctx g_client;
	struct mutex lock;
	/* primary ISP properties */
	__u32 def_addr;
	const char *compatible;
	enum tier4_camera_type cam_type;
};

#define MAX_CHANNEL_NUM 8

#define NO_ERROR 0

// #define SHOW_I2C_WRITE_MSG

/* count channel,the max MAX_CHANNEL_NUM*/
static __u32 channel_count_gw5300;

static struct tier4_gw5300 *prim_priv__[MAX_CHANNEL_NUM];

/*
 * C2
 */

static u8 master_30fps[] = { 0x33, 0x47, 0x0B, 0x00, 0x00, 0x00,
			     0x12, 0x00, 0x80, 0x03, 0x00, 0x00,
			     0x00, 0x50, 0x00, 0x00, 0x00, 0x6A };

static u8 master_10fps[] = { 0x33, 0x47, 0x0B, 0x00, 0x00, 0x00,
			     0x12, 0x00, 0x80, 0x03, 0x00, 0x00,
			     0x00, 0x1E, 0x00, 0x00, 0x00, 0x38 };

static u8 slave_10fps[] = { 0x33, 0x47, 0x0B, 0x00, 0x00, 0x00,
			    0x12, 0x00, 0x80, 0x03, 0x00, 0x00,
			    0x00, 0x28, 0x00, 0x00, 0x00, 0x42 };

static u8 master_20fps[] = { 0x33, 0x47, 0x0B, 0x00, 0x00, 0x00,
			     0x12, 0x00, 0x80, 0x03, 0x00, 0x00,
			     0x00, 0x5A, 0x00, 0x00, 0x00, 0x74 };

static u8 slave_20fps[] = { 0x33, 0x47, 0x0B, 0x00, 0x00, 0x00,
			    0x12, 0x00, 0x80, 0x03, 0x00, 0x00,
			    0x00, 0x5F, 0x00, 0x00, 0x00, 0x79 };

static u8 slave_30fps[] = { 0x33, 0x47, 0x0B, 0x00, 0x00, 0x00,
			    0x12, 0x00, 0x80, 0x03, 0x00, 0x00,
			    0x00, 0x55, 0x00, 0x00, 0x00, 0x6F };

static u8 master_10fps_slow[] = { 0x33, 0x47, 0x0B, 0x00, 0x00, 0x00,
				  0x12, 0x00, 0x80, 0x00, 0x00, 0x00,
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x17 };

static u8 slave_10fps_slow[] = { 0x33, 0x47, 0x0B, 0x00, 0x00, 0x00,
				 0x12, 0x00, 0x80, 0x03, 0x00, 0x00,
				 0x00, 0x4B, 0x00, 0x00, 0x00, 0x65 };

/*
 * C2-MP
 */

static u8 c2mp_master_10fps[] = {
				0x33, 0x47, 0x15, 0x00, 0x00, 0x00, 0xe0, 0x00,
				0x80, 0x01, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x70, 0x00, 0x02, 0x03, 0xB0
			};

static u8 c2mp_slave_10fps[] = {
				0x33, 0x47, 0x15, 0x00, 0x00, 0x00, 0xe0, 0x00,
				0x80, 0x01, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x70, 0x00, 0x02, 0x03, 0xB0
			};

static u8 c2mp_master_20fps[] = {
				0x33, 0x47, 0x15, 0x00, 0x00, 0x00, 0xe0, 0x00,
				0x80, 0x01, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0xD8, 0x0B, 0xB8, 0x00, 0x02, 0x03, 0xC4
			};

static u8 c2mp_slave_20fps[] = {
				0x33, 0x47, 0x15, 0x00, 0x00, 0x00, 0xe0, 0x00,
				0x80, 0x01, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0xB8, 0x00, 0x02, 0x03, 0xC4
			};

static u8 c2mp_master_30fps[] = {
				0x33, 0x47, 0x15, 0x00, 0x00, 0x00, 0xe0, 0x00,
				0x80, 0x01, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xD0, 0x00, 0x02, 0x03, 0x00
			};

static u8 c2mp_slave_30fps[] = {
				0x33, 0x47, 0x15, 0x00, 0x00, 0x00, 0xe0, 0x00,
				0x80, 0x01, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xD0, 0x00, 0x02, 0x03, 0x00
			};

/*
 * C3 camera mode support
 */
struct mode_command {
	u8 *command;
	size_t len;
	unsigned int delay_ms;
};

#define MODE_CMD(cmd, delay)                                             \
	{                                                                \
		.command = (cmd), .len = sizeof(cmd), .delay_ms = delay, \
	}

#define NUM_VA_ARGS(type, ...) (sizeof((type[]) { __VA_ARGS__ }) / sizeof(type))

#define MODE_SEQ(h_line_time_ns, ...)                                 \
	{                                                             \
		.commands = (struct mode_command[]){ __VA_ARGS__ },   \
		.len = NUM_VA_ARGS(struct mode_command, __VA_ARGS__), \
		.h_line_ns = (h_line_time_ns)                         \
	}

struct mode_sequence {
	struct mode_command *commands;
	size_t len;
	u32 h_line_ns;
};

/*
 * C3 preset modes
 */

static u8 c3_master_5fps_cmd[] = { 0x33, 0x47, 0xb,  0x0, 0x0, 0x0,
				   0x12, 0x0,  0x80, 0x0, 0x0, 0x0,
				   0x0,	 0x0,  0x0,  0x0, 0x0, 0x17 };

/* mode 0 */
static struct mode_sequence c3_master_5fps_seq =
	MODE_SEQ(40000, MODE_CMD(c3_master_5fps_cmd, 0));

/* mode 4 */
static u8 c3_master_20fps_cmd[] = { 0x33, 0x47, 0xb,  0x0, 0x0, 0x0,
				    0x12, 0x0,	0x80, 0x3, 0x0, 0x0,
				    0x0,  0x4b, 0x0,  0x0, 0x0, 0x65 };
static struct mode_sequence c3_master_20fps_seq =
	MODE_SEQ(20000, MODE_CMD(c3_master_20fps_cmd, 0));

static u8 c3_master_20fps_ebd_cmd[] = { 0x33, 0x47, 0xb,  0x0, 0x0, 0x0,
					0x12, 0x0,  0x80, 0x3, 0x0, 0x0,
					0x0,  0x50, 0x0,  0x0, 0x0, 0x6a };

/* mode 10 */
static struct mode_sequence c3_master_20fps_ebd_seq =
	MODE_SEQ(20000, MODE_CMD(c3_master_20fps_ebd_cmd, 0));

static u8 c3_master_30fps_cmd[] = { 0x33, 0x47, 0xb,  0x0, 0x0, 0x0,
				    0x12, 0x0,	0x80, 0x3, 0x0, 0x0,
				    0x0,  0x1e, 0x0,  0x0, 0x0, 0x38 };

/* mode 6 */
static struct mode_sequence c3_master_30fps_seq =
	MODE_SEQ(13890, MODE_CMD(c3_master_30fps_cmd, 0));

static u8 c3_master_30fps_ebd_cmd[] = { 0x33, 0x47, 0xb,  0x0, 0x0, 0x0,
					0x12, 0x0,  0x80, 0x3, 0x0, 0x0,
					0x0,  0x28, 0x0,  0x0, 0x0, 0x42 };

/* mode 12 */
static struct mode_sequence c3_master_30fps_ebd_seq =
	MODE_SEQ(13890, MODE_CMD(c3_master_30fps_ebd_cmd, 0));

/*
 * C3 derived modes
 */
static u8 c3_regmap_ffff[] = { 0x33, 0x47, 0x15, 0x0,  0x0,  0x0, 0xe0,
			       0x0,  0x80, 0x1,	 0x0,  0x0,  0x0, 0x34,
			       0x0,  0x0,  0x0,	 0xff, 0xff, 0x0, 0x0,
			       0x0,  0x0,  0x0,	 0x0,  0x2,  0x1, 0x25 };

static u8 c3_standby_mode[] = { 0x33, 0x47, 0x15, 0x0, 0x0,  0x0, 0xe0,
				0x0,  0x80, 0x1,  0x0, 0x0,  0x0, 0x34,
				0x0,  0x0,  0x0,  0x4, 0x1b, 0x0, 0x0,
				0xff, 0x0,  0x0,  0x0, 0x2,  0x1, 0x45 };

static u8 c3_drive_mode_sel[] = { 0x33, 0x47, 0x15, 0x0,  0x0, 0x0, 0xe0,
				  0x0,	0x80, 0x1,  0x0,  0x0, 0x0, 0x34,
				  0x0,	0x0,  0x0,  0x40, 0x0, 0x0, 0x0,
				  0x1,	0x0,  0x0,  0x0,  0x2, 0x1, 0x68 };

static u8 c3_master_sync_mode[] = { 0x33, 0x47, 0x15, 0x0,  0x0, 0x0, 0xe0,
				    0x0,  0x80, 0x1,  0x0,  0x0, 0x0, 0x34,
				    0x0,  0x0,	0x0,  0x41, 0x0, 0x0, 0x0,
				    0x5,  0x0,	0x0,  0x0,  0x2, 0x1, 0x6d };

static u8 c3_sensor_streaming[] = { 0x33, 0x47, 0x15, 0x0, 0x0,	 0x0, 0xe0,
				    0x0,  0x80, 0x1,  0x0, 0x0,	 0x0, 0x34,
				    0x0,  0x0,	0x0,  0x4, 0x1b, 0x0, 0x0,
				    0x5c, 0x0,	0x0,  0x0, 0x2,	 0x1, 0xa2 };

static u8 c3_master_sync_mode_2[] = { 0x33, 0x47, 0x15, 0x0, 0x0,  0x0, 0xe0,
				      0x0,  0x80, 0x1,	0x0, 0x0,  0x0, 0x34,
				      0x0,  0x0,  0x0,	0x4, 0x1b, 0x0, 0x0,
				      0xa3, 0x0,  0x0,	0x0, 0x2,  0x1, 0xe9 };

/* mode 1 */
static struct mode_sequence c3_slave_5fps_seq = MODE_SEQ(
	40000, MODE_CMD(c3_master_5fps_cmd, 5000),

	MODE_CMD(c3_regmap_ffff, 500), MODE_CMD(c3_standby_mode, 500),
	MODE_CMD(c3_drive_mode_sel, 500), MODE_CMD(c3_master_sync_mode, 500),
	MODE_CMD(c3_sensor_streaming, 500),
	MODE_CMD(c3_master_sync_mode_2, 500));

static u8 c3_20_to_10fps_cmd_1[] = {
	0x33, 0x47, 0x15, 0x0,	0x0, 0x0, 0xe0, 0x0,  0x80, 0x1,
	0x0,  0x0,  0x0,  0x34, 0x0, 0x0, 0x0,	0x50, 0x97, 0x0,
	0x0,  0xc4, 0x0,  0x0,	0x0, 0x2, 0x1,	0xd2,
};

static u8 c3_20_to_10fps_cmd_2[] = {
	0x33, 0x47, 0x15, 0x0,	0x0, 0x0, 0xe0, 0x0,  0x80, 0x1,
	0x0,  0x0,  0x0,  0x34, 0x0, 0x0, 0x0,	0x51, 0x97, 0x0,
	0x0,  0x09, 0x0,  0x0,	0x0, 0x2, 0x1,	0x18,
};

/* mode 2 */
static struct mode_sequence c3_master_10fps_seq =
	MODE_SEQ(20000, MODE_CMD(c3_master_20fps_cmd, 5000),

		 MODE_CMD(c3_20_to_10fps_cmd_1, 2000),
		 MODE_CMD(c3_20_to_10fps_cmd_2, 500));

/* mode 3 */
static struct mode_sequence c3_slave_10fps_seq = MODE_SEQ(
	20000, MODE_CMD(c3_master_20fps_cmd, 5000),

	MODE_CMD(c3_20_to_10fps_cmd_1, 2000),
	MODE_CMD(c3_20_to_10fps_cmd_2, 500),

	MODE_CMD(c3_regmap_ffff, 500), MODE_CMD(c3_standby_mode, 500),
	MODE_CMD(c3_drive_mode_sel, 500), MODE_CMD(c3_master_sync_mode, 500),
	MODE_CMD(c3_sensor_streaming, 500),
	MODE_CMD(c3_master_sync_mode_2, 500));

/* mode 5 */
static struct mode_sequence c3_slave_20fps_seq = MODE_SEQ(
	20000, MODE_CMD(c3_master_20fps_cmd, 5000),

	MODE_CMD(c3_regmap_ffff, 500), MODE_CMD(c3_standby_mode, 500),
	MODE_CMD(c3_drive_mode_sel, 500), MODE_CMD(c3_master_sync_mode, 500),
	MODE_CMD(c3_sensor_streaming, 500),
	MODE_CMD(c3_master_sync_mode_2, 500));

/* mode 7 */
static struct mode_sequence c3_slave_30fps_seq = MODE_SEQ(
	13890, MODE_CMD(c3_master_30fps_cmd, 5000),

	MODE_CMD(c3_regmap_ffff, 500), MODE_CMD(c3_standby_mode, 500),
	MODE_CMD(c3_drive_mode_sel, 500), MODE_CMD(c3_master_sync_mode, 500),
	MODE_CMD(c3_sensor_streaming, 500),
	MODE_CMD(c3_master_sync_mode_2, 500));

/* mode 8 */
static struct mode_sequence c3_master_10fps_ebd_seq =
	MODE_SEQ(20000, MODE_CMD(c3_master_20fps_ebd_cmd, 5000),

		 MODE_CMD(c3_20_to_10fps_cmd_1, 2000),
		 MODE_CMD(c3_20_to_10fps_cmd_2, 500));

/* mode 9 */
static struct mode_sequence c3_slave_10fps_ebd_seq = MODE_SEQ(
	20000, MODE_CMD(c3_master_20fps_ebd_cmd, 5000),

	MODE_CMD(c3_20_to_10fps_cmd_1, 2000),
	MODE_CMD(c3_20_to_10fps_cmd_2, 500),

	MODE_CMD(c3_regmap_ffff, 500), MODE_CMD(c3_standby_mode, 500),
	MODE_CMD(c3_drive_mode_sel, 500), MODE_CMD(c3_master_sync_mode, 500),
	MODE_CMD(c3_sensor_streaming, 500),
	MODE_CMD(c3_master_sync_mode_2, 500));

/* mode 11 */
static struct mode_sequence c3_slave_20fps_ebd_seq = MODE_SEQ(
	20000, MODE_CMD(c3_master_20fps_ebd_cmd, 5000),

	MODE_CMD(c3_20_to_10fps_cmd_1, 2000),
	MODE_CMD(c3_20_to_10fps_cmd_2, 500),

	MODE_CMD(c3_regmap_ffff, 500), MODE_CMD(c3_standby_mode, 500),
	MODE_CMD(c3_drive_mode_sel, 500), MODE_CMD(c3_master_sync_mode, 500),
	MODE_CMD(c3_sensor_streaming, 500),
	MODE_CMD(c3_master_sync_mode_2, 500));

/* mode 13 */
static struct mode_sequence c3_slave_30fps_ebd_seq = MODE_SEQ(
	13890, MODE_CMD(c3_master_30fps_ebd_cmd, 5000),

	MODE_CMD(c3_20_to_10fps_cmd_1, 2000),
	MODE_CMD(c3_20_to_10fps_cmd_2, 500),

	MODE_CMD(c3_regmap_ffff, 500), MODE_CMD(c3_standby_mode, 500),
	MODE_CMD(c3_drive_mode_sel, 500), MODE_CMD(c3_master_sync_mode, 500),
	MODE_CMD(c3_sensor_streaming, 500),
	MODE_CMD(c3_master_sync_mode_2, 500));

static struct mode_sequence c3_mode_seqs[GW5300_MODE_MAX];

struct map_ctx {
	u8 dt;
	u16 addr;
	u8 val;
	u8 st_id;
};

// -------------------------------------------------------------------

static int tier4_gw5300_send_and_recv_msg(struct device *dev, u8 *wdata,
					  int wdata_size, u8 *rdata,
					  int rdata_size)
{
	int err = 0;
	struct i2c_msg msg[2];
	struct tier4_gw5300 *priv = dev_get_drvdata(dev);

	msg[0].addr = priv->i2c_client->addr;
	msg[0].flags = 0; // I2C Write
	msg[0].len = wdata_size;
	msg[0].buf = wdata;

	msg[1].addr = priv->i2c_client->addr;
	msg[1].flags = I2C_M_RD; // I2C Read
	msg[1].len = rdata_size;
	msg[1].buf = rdata;

#ifdef SHOW_I2C_WRITE_MSG
	// sending data
	dev_info(dev, "[Debug] Sending %d bytes:\n", wdata_size);
	print_hex_dump(KERN_INFO, "TX_DATA: ", DUMP_PREFIX_OFFSET, 16, 1,
		       wdata, wdata_size, false);
#endif

	// transfer data
	err = i2c_transfer(priv->i2c_client->adapter, msg, 2);

	if (err <= 0) {
		dev_err(dev,
			"[%s] : i2c_transer send message failed. %d: slave addr = 0x%x\n",
			__func__, err, msg[0].addr);
	} else {
#ifdef SHOW_I2C_WRITE_MSG
		// receiving data
		dev_info(dev, "[Debug] Received %d bytes:\n", rdata_size);
		print_hex_dump(KERN_INFO, "RX_DATA: ", DUMP_PREFIX_OFFSET, 16, 1,
			       rdata, rdata_size, false);
#endif
	}

	return err; //  the total number of bytes to have been sent or recived
}

// -------------------------------------------------------------------

static int tier4_gw5300_c3_send_and_recv_msg(struct device *dev, u8 *wdata,
					     int wdata_size, u8 *rdata,
					     int rdata_size)
{
	return tier4_gw5300_send_and_recv_msg(dev, wdata, wdata_size, rdata,
					      rdata_size);
}

static int
tier4_gw5300_mode_seq_send_and_recv_msg(struct device *dev,
					struct mode_sequence *mode_seq,
					u8 *rdata, int rdata_size)
{
	int i;
	int err = 0;

	for (i = 0; i < mode_seq->len; ++i) {
		struct mode_command *cmd = &mode_seq->commands[i];

		err = tier4_gw5300_send_and_recv_msg(
			dev, cmd->command, cmd->len, rdata, rdata_size);
		msleep(cmd->delay_ms);
		if (err < 0) {
			dev_err(dev, "%s: Failed to send a command[%d]: %d\n",
				__func__, i, err);
			break;
		}
	}

	return err;
}

// -------------------------------------------------------------------

uint8_t calcCheckSum(const uint8_t *data, size_t size)
{
	uint8_t result = 0;
	size_t i = 0;

	for (i = 0; i < size; i++)
		result += data[i];
	return result;
}

static int tier4_gw5300_param_byte_width(u8 param_type)
{
	switch (param_type) {
	case TIER4_GW5300_PARAM_UINT8:
	case TIER4_GW5300_PARAM_INT8:
		return 1;
	case TIER4_GW5300_PARAM_UINT16:
	case TIER4_GW5300_PARAM_INT16:
		return 2;
	case TIER4_GW5300_PARAM_UINT32:
	case TIER4_GW5300_PARAM_INT32:
	case TIER4_GW5300_PARAM_FLOAT:
		return 4;
	default:
		return -EINVAL;
	}
}

/*
 * Write a single scalar ISP parameter. Mirrors write_parameter() in the
 * isp_parameter_bridge: a 0x72 "set parameter" command carrying the value as
 * @width little-endian bytes, terminated by a modulo-256 checksum.
 */
int tier4_gw5300_isp_set_param(struct device *dev, u8 spec_id, u8 context,
			       u16 param_id, u8 param_type, u32 value)
{
	/* header(9) + payload(8) + value(<=4) + checksum(1) */
	u8 packet[22];
	u8 buf[6];
	int width = tier4_gw5300_param_byte_width(param_type);
	int data_size;
	int len = 0;
	int i;

	if (width < 0) {
		dev_err(dev, "%s: unsupported param_type %u\n", __func__,
			param_type);
		return -EINVAL;
	}

	data_size = 11 + width;

	packet[len++] = 0x33;
	packet[len++] = 0x47;
	packet[len++] = data_size & 0xFF;
	packet[len++] = (data_size >> 8) & 0xFF;
	packet[len++] = 0x00;
	packet[len++] = 0x00;
	packet[len++] = 0x72;
	packet[len++] = 0x00;
	packet[len++] = 0x80;

	packet[len++] = spec_id;
	packet[len++] = context;
	packet[len++] = param_id & 0xFF;
	packet[len++] = (param_id >> 8) & 0xFF;
	packet[len++] = 0x01;
	packet[len++] = 0x00;
	packet[len++] = width & 0xFF;
	packet[len++] = (width >> 8) & 0xFF;
	for (i = 0; i < width; i++)
		packet[len++] = (value >> (8 * i)) & 0xFF;

	packet[len] = calcCheckSum(packet, len);
	len++;

	dev_info(dev,
		 "%s: spec_id=%u context=%u param_id=0x%04x type=%u width=%d value=0x%08x\n",
		 __func__, spec_id, context, param_id, param_type, width, value);

	msleep(20);
	return tier4_gw5300_send_and_recv_msg(dev, packet, len, buf,
					      sizeof(buf));
}
EXPORT_SYMBOL(tier4_gw5300_isp_set_param);

/*
 * Read a single scalar ISP parameter. Mirrors read_parameter() in the
 * isp_parameter_bridge: a 0x70 read request returns a message id in its ACK,
 * then a 0x51 query is polled until the result is ready, and @width
 * little-endian bytes of the value are returned in *value.
 */
int tier4_gw5300_isp_get_param(struct device *dev, u8 spec_id, u8 context,
			       u16 param_id, u8 param_type, u32 *value)
{
	u8 req[19]; /* header(9) + payload(9) + checksum(1) */
	u8 ack[6];
	u8 query[11]; /* query(10) + checksum(1) */
	u8 resp[24];
	int width = tier4_gw5300_param_byte_width(param_type);
	int len = 0;
	int i, ret, retry;
	u8 msg_id;
	u32 v = 0;

	if (width < 0) {
		dev_err(dev, "%s: unsupported param_type %u\n", __func__,
			param_type);
		return -EINVAL;
	}
	if (!value)
		return -EINVAL;

	/* Phase 1: 0x70 read request -> 6-byte ACK carrying the message id. */
	req[len++] = 0x33;
	req[len++] = 0x47;
	req[len++] = 0x0C;
	req[len++] = 0x00;
	req[len++] = 0x00;
	req[len++] = 0x00;
	req[len++] = 0x70;
	req[len++] = 0x00;
	req[len++] = 0x80;
	req[len++] = spec_id;
	req[len++] = 0x00;
	req[len++] = 0x00;
	req[len++] = 0x00;
	req[len++] = param_id & 0xFF;
	req[len++] = (param_id >> 8) & 0xFF;
	req[len++] = 0x00;
	req[len++] = 0x00;
	req[len++] = context;
	req[len] = calcCheckSum(req, len);
	len++;

	msleep(20);
	ret = tier4_gw5300_send_and_recv_msg(dev, req, len, ack, sizeof(ack));
	if (ret < 0)
		return ret;
	if (ack[1] != 0x41 || ack[4] != 0x01) {
		dev_err(dev, "%s: read ACK failed for param 0x%04x\n", __func__,
			param_id);
		return -EIO;
	}
	msg_id = ack[3];

	/* Phase 2: 0x51 query, polled until the result status is ready. */
	len = 0;
	query[len++] = 0x33;
	query[len++] = 0x51;
	query[len++] = 0x07;
	query[len++] = msg_id;
	query[len++] = 0x00;
	query[len++] = 0x00;
	query[len++] = 0x08;
	query[len++] = 0x00;
	query[len++] = 0x00;
	query[len++] = 0x00;
	query[len] = calcCheckSum(query, len);
	len++;

	for (retry = 0; retry < 5; retry++) {
		ret = tier4_gw5300_send_and_recv_msg(dev, query, len, resp,
						     sizeof(resp));
		if (ret < 0)
			return ret;

		/* resp[8]: 0x02 = ready, 0x01 = pending, otherwise error. */
		if (resp[8] == 0x02) {
			for (i = 0; i < width; i++)
				v |= (u32)resp[15 + i] << (8 * i);
			*value = v;
			dev_info(dev,
				 "%s: param_id=0x%04x type=%u value=0x%08x\n",
				 __func__, param_id, param_type, v);
			return 0;
		}
		if (resp[8] != 0x01) {
			dev_err(dev, "%s: read status 0x%02x for param 0x%04x\n",
				__func__, resp[8], param_id);
			return -EIO;
		}
		msleep(50);
	}

	dev_err(dev, "%s: read timed out for param 0x%04x\n", __func__,
		param_id);
	return -ETIMEDOUT;
}
EXPORT_SYMBOL(tier4_gw5300_isp_get_param);

/*
 * Type operations for the private "ISP Parameter" compound control, whose
 * value is a struct tier4_gw5300_isp_param. The V4L2 core copies the value
 * to/from userspace generically (elem_size bytes); these ops only need to
 * implement equal/init/log/validate.
 */
static bool tier4_gw5300_isp_param_equal(const struct v4l2_ctrl *ctrl, u32 idx,
					 union v4l2_ctrl_ptr ptr1,
					 union v4l2_ctrl_ptr ptr2)
{
	size_t off = idx * ctrl->elem_size;

	return !memcmp(ptr1.p_const + off, ptr2.p_const + off, ctrl->elem_size);
}

static void tier4_gw5300_isp_param_init(const struct v4l2_ctrl *ctrl, u32 idx,
					union v4l2_ctrl_ptr ptr)
{
	memset(ptr.p + idx * ctrl->elem_size, 0, ctrl->elem_size);
}

static void tier4_gw5300_isp_param_log(const struct v4l2_ctrl *ctrl)
{
	const struct tier4_gw5300_isp_param *p = ctrl->p_cur.p;

	pr_cont("param_id=0x%04x type=%u value=0x%08x", le16_to_cpu(p->param_id),
		p->param_type, le32_to_cpu(p->value));
}

static int tier4_gw5300_isp_param_validate(const struct v4l2_ctrl *ctrl,
					   u32 idx, union v4l2_ctrl_ptr ptr)
{
	const struct tier4_gw5300_isp_param *p = ptr.p + idx * ctrl->elem_size;

	if (p->param_type >= TIER4_GW5300_PARAM_TYPE_MAX)
		return -EINVAL;

	return 0;
}

const struct v4l2_ctrl_type_ops tier4_gw5300_isp_param_type_ops = {
	.equal = tier4_gw5300_isp_param_equal,
	.init = tier4_gw5300_isp_param_init,
	.log = tier4_gw5300_isp_param_log,
	.validate = tier4_gw5300_isp_param_validate,
};
EXPORT_SYMBOL(tier4_gw5300_isp_param_type_ops);

int tier4_gw5300_set_integration_time_on_aemode(struct device *dev,
						u32 h_line_ns,
						u32 max_integration_time,
						u32 min_integration_time)
{
	u8 buf[6];
	int ret = 0;

	u8 cmd_integration_max[22] = {
		0x33, 0x47, 0x0f, 0x00, 0x00, 0x00, 0x55, 0x00,
		0x80, 0x05, 0x00, 0x15, 0x00, 0x01, 0x00, 0x04,
		0x00, 0x70, 0x03, 0x00, 0x00, 0x00
	}; // val = 0x70, 0x03, 0x00, 0x00
	u8 cmd_integration_min[20] = {
		0x33, 0x47, 0x0d, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
		0x00, 0x21, 0x00, 0x01, 0x00, 0x02, 0x00, 0x70, 0x03, 0x00
	}; //val = 0x70, 0x03

	const size_t max_val_pos = 17;
	const size_t min_val_pos = 17;
	uint32_t min_line =
		DIV_ROUND_CLOSEST(min_integration_time * 1000, h_line_ns);
	uint32_t max_line =
		DIV_ROUND_CLOSEST(max_integration_time * 1000, h_line_ns);
	uint8_t b1 = max_line & 0xFF;
	uint8_t b2 = (max_line >> 8) & 0xFF;
	uint8_t b3 = 0;
	uint8_t b4 = 0;

	dev_info(
		dev,
		"%s: integration time(%u[ns/H line]): max=(%d[us], %d[line]) min=(%d[us], %d[line])\n",
		__func__, h_line_ns, max_integration_time, max_line,
		min_integration_time, min_line);

	cmd_integration_max[max_val_pos] = b1;
	cmd_integration_max[max_val_pos + 1] = b2;
	cmd_integration_max[max_val_pos + 2] = b3;
	cmd_integration_max[max_val_pos + 3] = b4;
	cmd_integration_max[sizeof(cmd_integration_max) - 1] =
		calcCheckSum(cmd_integration_max, sizeof(cmd_integration_max));

	b1 = min_line & 0xFF;
	b2 = (min_line >> 8) & 0xFF;

	cmd_integration_min[min_val_pos] = b1;
	cmd_integration_min[min_val_pos + 1] = b2;

	cmd_integration_min[sizeof(cmd_integration_min) - 1] =
		calcCheckSum(cmd_integration_min, sizeof(cmd_integration_min));

	msleep(20);
	ret += tier4_gw5300_send_and_recv_msg(dev, cmd_integration_max,
					      sizeof(cmd_integration_max), buf,
					      sizeof(buf));
	msleep(20);
	ret += tier4_gw5300_send_and_recv_msg(dev, cmd_integration_min,
					      sizeof(cmd_integration_min), buf,
					      sizeof(buf));

	return ret;
}

int tier4_gw5300_c2_set_integration_time_on_aemode(struct device *dev,
						   int trigger_mode,
						   u32 max_integration_time,
						   u32 min_integration_time)
{
	struct tier4_gw5300 *priv = dev_get_drvdata(dev);

	size_t h_line_ns = 12500;

	if (priv->cam_type == TIER4_CAMERA_TYPE_STANDARD) {
		h_line_ns = 12500;
		if (trigger_mode == GW5300_MASTER_MODE_10FPS_SLOW ||
			trigger_mode == GW5300_SLAVE_MODE_10FPS_SLOW)
			h_line_ns = 50000;
	} else if (priv->cam_type == TIER4_CAMERA_TYPE_MP) {
		h_line_ns = 16666;
	}

	return tier4_gw5300_set_integration_time_on_aemode(
		dev, h_line_ns, max_integration_time, min_integration_time);
}
EXPORT_SYMBOL(tier4_gw5300_c2_set_integration_time_on_aemode);

int tier4_gw5300_c3_set_integration_time_on_aemode(struct device *dev,
						   int trigger_mode,
						   u32 max_integration_time,
						   u32 min_integration_time)
{
	return tier4_gw5300_set_integration_time_on_aemode(
		dev, c3_mode_seqs[trigger_mode].h_line_ns, max_integration_time,
		min_integration_time);
}
EXPORT_SYMBOL(tier4_gw5300_c3_set_integration_time_on_aemode);

int tier4_gw5300_set_readout_delay(struct device *dev, int readout_delay_us, u32 h_line_ns)
{
	int ret = 0;
	int i;
	u8 buf[6];
	int delay_in_line;
	u8 cmd[28] = {
		0x33, 0x47, 0x15, 0x00, 0x00, 0x00, 0xe0, 0x00,
		0x80, 0x01, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00,
		0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x02, 0x01, 0x00
	};
	u8 regs[3] = {0xfd, 0xfe, 0xff};
	u8 vals[3];

	if (readout_delay_us < 0 || readout_delay_us > 100000)
		return -EINVAL;

	if (h_line_ns == 0)
		return -EINVAL;

	delay_in_line = DIV_ROUND_CLOSEST(readout_delay_us * 1000, h_line_ns);

	vals[0] = (delay_in_line >> 0) & 0xff;
	vals[1] = (delay_in_line >> 8) & 0xff;
	vals[2] = (delay_in_line >> 16) & 0xff;

	for (i = 0; i < 3; i++) {
		cmd[17] = regs[i];
		cmd[21] = vals[i];
		cmd[27] = 0;
		cmd[27] = calcCheckSum(cmd, 28);

		ret = tier4_gw5300_send_and_recv_msg(dev, cmd, sizeof(cmd), buf, sizeof(buf));
		if (ret < 0)
			return -EINVAL;
		usleep_range(100000, 110000);
	}

	return 0;
}
EXPORT_SYMBOL(tier4_gw5300_set_readout_delay);

// ------------------------------------------------------------------

int tier4_gw5300_set_distortion_correction(struct device *dev, bool val)
{
	int ret = 0;
	u8 buf[6];
	u8 cmd_dwp_on[] = { 0x33, 0x47, 0x06, 0x00, 0x00, 0x00, 0x4d,
			    0x00, 0x80, 0x04, 0x00, 0x01, 0x52 };
	u8 cmd_dwp_off[] = { 0x33, 0x47, 0x03, 0x00, 0x00,
			     0x00, 0x45, 0x00, 0x80, 0x42 };

	if (val) {
		ret += tier4_gw5300_send_and_recv_msg(
			dev, cmd_dwp_on, sizeof(cmd_dwp_on), buf, sizeof(buf));
	} else {
		ret += tier4_gw5300_send_and_recv_msg(dev, cmd_dwp_off,
						      sizeof(cmd_dwp_off), buf,
						      sizeof(buf));
	}
	return ret;
}
EXPORT_SYMBOL(tier4_gw5300_set_distortion_correction);

// ------------------------------------------------------------------

int tier4_gw5300_c3_set_distortion_correction(struct device *dev, bool val)
{
	int ret = 0;
	u8 buf[6];
	u8 cmd_dwp_on[] = { 0x33, 0x47, 0x6,  0x00, 0x00, 0x00, 0x4d,
			    0x00, 0x80, 0x04, 0x00, 0x01, 0x52 };
	u8 cmd_dwp_off[] = { 0x33, 0x47, 0x3,  0x00, 0x00,
			     0x00, 0x45, 0x00, 0x80, 0x042 };

	if (val) {
		ret += tier4_gw5300_c3_send_and_recv_msg(
			dev, cmd_dwp_on, sizeof(cmd_dwp_on), buf, sizeof(buf));
	} else {
		ret += tier4_gw5300_c3_send_and_recv_msg(dev, cmd_dwp_off,
							 sizeof(cmd_dwp_off),
							 buf, sizeof(buf));
	}
	return ret;
}
EXPORT_SYMBOL(tier4_gw5300_c3_set_distortion_correction);

// ------------------------------------------------------------------

int tier4_gw5300_c3_set_auto_exposure(struct device *dev, bool val)
{
	int ret = 0;
	u8 buf[6];
	u8 cmd_auto_exp_on[] = { 0x33, 0x47, 0x0C, 0x00, 0x00, 0x00, 0x55,
				 0x00, 0x80, 0x05, 0x00, 0x07, 0x00, 0x01,
				 0x00, 0x01, 0x00, 0x00, 0x69 };
	u8 cmd_auto_exp_off[] = { 0x33, 0x47, 0x0C, 0x00, 0x00, 0x00, 0x55,
				  0x00, 0x80, 0x05, 0x00, 0x07, 0x00, 0x01,
				  0x00, 0x01, 0x00, 0x01, 0x6A };

	dev_info(dev, "%s: auto exposure is %s\n", __func__,
		 val ? "on" : "off");

	if (val) {
		ret += tier4_gw5300_c3_send_and_recv_msg(
			dev, cmd_auto_exp_on, sizeof(cmd_auto_exp_on), buf,
			sizeof(buf));
	} else {
		ret += tier4_gw5300_c3_send_and_recv_msg(
			dev, cmd_auto_exp_off, sizeof(cmd_auto_exp_off), buf,
			sizeof(buf));
	}
	return ret;
}
EXPORT_SYMBOL(tier4_gw5300_c3_set_auto_exposure);

// ------------------------------------------------------------------

int tier4_gw5300_check_device(struct device *dev, u8 *rdata, int rdata_size)
{
	int err = 0;
	struct i2c_msg msg[2];
	struct tier4_gw5300 *priv = dev_get_drvdata(dev);

	msg[0].addr = priv->i2c_client->addr;
	msg[0].flags = I2C_M_RD; // I2C Read
	msg[0].len = rdata_size;
	msg[0].buf = rdata;

	err = i2c_transfer(priv->i2c_client->adapter, msg, 1);

	if (err <= 0) {
		dev_err(dev,
			"[%s] : i2c_transer send message failed. : slave addr = 0x%x\n",
			__func__, msg[0].addr);
	} else {
		err = NO_ERROR;
	}

	return err;
}
EXPORT_SYMBOL(tier4_gw5300_check_device);

int tier4_gw5300_prim_slave_addr(struct tier4_gmsl_link_ctx *g_ctx)
{
	if (!g_ctx) {
		dev_err(&prim_priv__[channel_count_gw5300 - 1]->i2c_client->dev,
			"[%s] : Failed. g_ctx is null\n", __func__);
		return -1;
	}

	g_ctx->sdev_isp_def = prim_priv__[channel_count_gw5300 - 1]->def_addr;

	return 0;
}
EXPORT_SYMBOL(tier4_gw5300_prim_slave_addr);

int tier4_gw5300_setup_sensor_mode(struct device *dev, int sensor_mode)
{
	int err = 0;
	u8 buf[6];
	struct tier4_gw5300 *priv = dev_get_drvdata(dev);

	memset(buf, 0x00, 6);

	if (priv->cam_type == TIER4_CAMERA_TYPE_STANDARD) {
		// for C2/C3 camera
		switch (sensor_mode) {
		case GW5300_MASTER_MODE_10FPS:
			err = tier4_gw5300_send_and_recv_msg(dev, master_10fps,
								sizeof(master_10fps), buf,
								sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Master mode 10fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Master mode 10fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		case GW5300_SLAVE_MODE_10FPS:
			err = tier4_gw5300_send_and_recv_msg(dev, slave_10fps,
								sizeof(slave_10fps), buf,
								sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Slave mode 10fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Slave mode 10fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		case GW5300_MASTER_MODE_20FPS:
			err = tier4_gw5300_send_and_recv_msg(dev, master_20fps,
								sizeof(master_20fps), buf,
								sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Master mode 20fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Master mode 20fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		case GW5300_SLAVE_MODE_20FPS:
			err = tier4_gw5300_send_and_recv_msg(dev, slave_20fps,
								sizeof(slave_20fps), buf,
								sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Slave mode 20fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Slave mode 20fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		case GW5300_MASTER_MODE_30FPS:
			err = tier4_gw5300_send_and_recv_msg(dev, master_30fps,
								sizeof(master_30fps), buf,
								sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Master mode 30fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Master mode 30fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		case GW5300_SLAVE_MODE_30FPS:
			err = tier4_gw5300_send_and_recv_msg(dev, slave_30fps,
								sizeof(slave_30fps), buf,
								sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Slave mode 30fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Slave mode 30fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		case GW5300_MASTER_MODE_10FPS_SLOW:
			err = tier4_gw5300_send_and_recv_msg(dev, master_10fps_slow,
								sizeof(master_10fps_slow),
								buf, sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Slow clock Master mode 10fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Slow clock Master mode 10fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		case GW5300_SLAVE_MODE_10FPS_SLOW:
			err = tier4_gw5300_send_and_recv_msg(dev, slave_10fps_slow,
								sizeof(slave_10fps_slow),
								buf, sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Slow clock Slave mode 10fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Slow clock Slave mode 10fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		default:
			break;
		}
	} else if (priv->cam_type == TIER4_CAMERA_TYPE_MP) {
		// for C2/C3MP camera
		switch (sensor_mode) {
		case GW5300_MASTER_MODE_10FPS:
			err = tier4_gw5300_send_and_recv_msg(dev, c2mp_master_10fps,
								sizeof(c2mp_master_10fps), buf,
								sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Master mode 10fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Master mode 10fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		case GW5300_SLAVE_MODE_10FPS:
			err = tier4_gw5300_send_and_recv_msg(dev, c2mp_slave_10fps,
								sizeof(c2mp_slave_10fps), buf,
								sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Slave mode 10fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Slave mode 10fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		case GW5300_MASTER_MODE_20FPS:
			err = tier4_gw5300_send_and_recv_msg(dev, c2mp_master_20fps,
								sizeof(c2mp_master_20fps), buf,
								sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Master mode 20fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Master mode 20fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		case GW5300_SLAVE_MODE_20FPS:
			err = tier4_gw5300_send_and_recv_msg(dev, c2mp_slave_20fps,
								sizeof(c2mp_slave_20fps), buf,
								sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Slave mode 20fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Slave mode 20fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		case GW5300_MASTER_MODE_30FPS:
			err = tier4_gw5300_send_and_recv_msg(dev, c2mp_master_30fps,
								sizeof(c2mp_master_30fps), buf,
								sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Master mode 30fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Master mode 30fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		case GW5300_SLAVE_MODE_30FPS:
			err = tier4_gw5300_send_and_recv_msg(dev, c2mp_slave_30fps,
								sizeof(c2mp_slave_30fps), buf,
								sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Slave mode 30fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Slave mode 30fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		case GW5300_MASTER_MODE_10FPS_SLOW:
		case GW5300_SLAVE_MODE_10FPS_SLOW:
			dev_warn(dev,
				 "[%s] : Slow shutter speed modes are not supported for C2 MP. set 10fps mode\n",
				__func__);
			err = tier4_gw5300_send_and_recv_msg(dev, c2mp_slave_10fps,
						sizeof(c2mp_slave_10fps), buf,
						sizeof(buf));
			if (err < 0) {
				dev_err(dev,
					"[%s] : Setting up Slave mode 10fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				goto error;
			} else if (err == 0) { // it means that 0 message has been sent.
				dev_err(dev,
					"[%s] : Setting up Slave mode 10fps failed. %d message has been sent to gw5300.\n",
					__func__, err);
				err = -999;
				goto error;
			} else {
				err = 0;
			}
			break;
		default:
			break;
		}
	}
error:

	return err;
}
EXPORT_SYMBOL(tier4_gw5300_setup_sensor_mode);

// -----------   for C3 camera   ----------

int tier4_gw5300_c3_setup_sensor_mode(struct device *dev, int sensor_mode)
{
	int err = 0;
	u8 buf[6];

	memset(buf, 0x00, 6);

	switch (sensor_mode) {
	case GW5300_MASTER_MODE_10FPS:
	case GW5300_SLAVE_MODE_10FPS:
	case GW5300_MASTER_MODE_20FPS:
	case GW5300_SLAVE_MODE_20FPS:
	case GW5300_MASTER_MODE_30FPS:
	case GW5300_SLAVE_MODE_30FPS:
	case GW5300_MASTER_MODE_5FPS:
	case GW5300_SLAVE_MODE_5FPS:
	case GW5300_MASTER_MODE_10FPS_EBD:
	case GW5300_SLAVE_MODE_10FPS_EBD:
	case GW5300_MASTER_MODE_20FPS_EBD:
	case GW5300_SLAVE_MODE_20FPS_EBD:
	case GW5300_MASTER_MODE_30FPS_EBD:
	case GW5300_SLAVE_MODE_30FPS_EBD:
		err = tier4_gw5300_mode_seq_send_and_recv_msg(
			dev, &c3_mode_seqs[sensor_mode], buf, sizeof(buf));
		if (err < 0) {
			dev_err(dev,
				"[%s] : Setting up %s failed. %d message has been sent to gw5300.\n",
				__func__, gw5300_mode_name[sensor_mode], err);
			goto error;
		} else if (err == 0) { // it means that 0 message has been sent.
			dev_err(dev,
				"[%s] : Setting up %s failed. %d message has been sent to gw5300.\n",
				__func__, gw5300_mode_name[sensor_mode], err);
			err = -999;
			goto error;
		} else {
			err = 0;
		}
		break;
	default:
		break;
	}

error:

	return err;
}
EXPORT_SYMBOL(tier4_gw5300_c3_setup_sensor_mode);


static u8 c2_pseudo_error_0[] = {
	0x33, 0x47, 0x15, 0x00, 0x00, 0x00, 0xe0, 0x00,
	0x80, 0x01, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00,
	0x00, 0xde, 0xc0, 0x00, 0x00, 0x01, 0x00, 0x00,
	0x00, 0x02, 0x01, 0x00
};

static u8 c2_pseudo_error_1[] = {
	0x33, 0x47, 0x15, 0x00, 0x00, 0x00, 0xe0, 0x00,
	0x80, 0x01, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00,
	0x00, 0xdf, 0xc0, 0x00, 0x00, 0x01, 0x00, 0x00,
	0x00, 0x02, 0x01, 0x00
};

static u8 c2_pseudo_error_2[] = {
	0x33, 0x47, 0x15, 0x00, 0x00, 0x00, 0xe0, 0x00,
	0x80, 0x01, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00,
	0x00, 0x90, 0xc0, 0x00, 0x00, 0x01, 0x00, 0x00,
	0x00, 0x02, 0x01, 0x00
};

static u8 c2_pseudo_error_3[] = {
	0x33, 0x47, 0x15, 0x00, 0x00, 0x00, 0xe0, 0x00,
	0x80, 0x01, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00,
	0x00, 0xa8, 0xc0, 0x00, 0x00, 0x01, 0x00, 0x00,
	0x00, 0x02, 0x01, 0x00
};

static u8 c2_pseudo_error_4[] = {
	0x33, 0x47, 0x15, 0x00, 0x00, 0x00, 0xe0, 0x00,
	0x80, 0x01, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00,
	0x00, 0xc0, 0xc0, 0x00, 0x00, 0x01, 0x00, 0x00,
	0x00, 0x02, 0x01, 0x00
};

int tier4_gw5300_c2_test_hw_fault(struct device *dev, bool enable)
{
	int err;
	u8 buf[6];

	if (!enable) {
		c2_pseudo_error_4[sizeof(c2_pseudo_error_4) - 1] = 0;
		c2_pseudo_error_4[sizeof(c2_pseudo_error_4) - 1] =
			calcCheckSum(c2_pseudo_error_4, sizeof(c2_pseudo_error_4));

		msleep(20);
		err = tier4_gw5300_send_and_recv_msg(dev, c2_pseudo_error_4,
				sizeof(c2_pseudo_error_4), buf, sizeof(buf));
		return err < 0 ? err : 0;
	}

	c2_pseudo_error_0[sizeof(c2_pseudo_error_0) - 1] = 0;
	c2_pseudo_error_1[sizeof(c2_pseudo_error_1) - 1] = 0;
	c2_pseudo_error_2[sizeof(c2_pseudo_error_2) - 1] = 0;
	c2_pseudo_error_3[sizeof(c2_pseudo_error_3) - 1] = 0;

	c2_pseudo_error_0[sizeof(c2_pseudo_error_0) - 1] =
		calcCheckSum(c2_pseudo_error_0, sizeof(c2_pseudo_error_0));
	c2_pseudo_error_1[sizeof(c2_pseudo_error_1) - 1] =
		calcCheckSum(c2_pseudo_error_1, sizeof(c2_pseudo_error_1));
	c2_pseudo_error_2[sizeof(c2_pseudo_error_2) - 1] =
		calcCheckSum(c2_pseudo_error_2, sizeof(c2_pseudo_error_2));
	c2_pseudo_error_3[sizeof(c2_pseudo_error_3) - 1] =
		calcCheckSum(c2_pseudo_error_3, sizeof(c2_pseudo_error_3));

	msleep(20);
	err = tier4_gw5300_send_and_recv_msg(dev, c2_pseudo_error_0,
			sizeof(c2_pseudo_error_0), buf, sizeof(buf));
	if (err < 0)
		return err;

	msleep(20);
	err = tier4_gw5300_send_and_recv_msg(dev, c2_pseudo_error_1,
			sizeof(c2_pseudo_error_1), buf, sizeof(buf));
	if (err < 0)
		return err;

	msleep(20);
	err = tier4_gw5300_send_and_recv_msg(dev, c2_pseudo_error_2,
			sizeof(c2_pseudo_error_2), buf, sizeof(buf));
	if (err < 0)
		return err;

	msleep(20);
	err = tier4_gw5300_send_and_recv_msg(dev, c2_pseudo_error_3,
			sizeof(c2_pseudo_error_3), buf, sizeof(buf));

	return err < 0 ? err : 0;
}
EXPORT_SYMBOL(tier4_gw5300_c2_test_hw_fault);

const struct of_device_id tier4_gw5300_of_match[] = {
	{
		.compatible = "nvidia,tier4_gw5300",
	},
	{
		.compatible = "nvidia,tier4mp_gw5300",
	},
	{},
};
MODULE_DEVICE_TABLE(of, tier4_gw5300_of_match);

static int tier4_gw5300_probe(struct i2c_client *client,
			      const struct i2c_device_id *id)
{
	struct tier4_gw5300 *priv;
	int err = 0;
	struct device_node *node = client->dev.of_node;
	const struct of_device_id *match;

	dev_info(&client->dev, "[%s] : probing GW5300 ISP\n", __func__);

	priv = devm_kzalloc(&client->dev, sizeof(*priv), GFP_KERNEL);
	priv->i2c_client = client;

	match = of_match_device(tier4_gw5300_of_match, &client->dev);
	if (!match) {
		dev_err(&client->dev, "[%s] : Failed to match device with dt id\n", __func__);
		return -ENODEV;
	}
	priv->compatible = match->compatible;

	if (strstr(priv->compatible, "tier4mp_")) {
		priv->cam_type = TIER4_CAMERA_TYPE_MP;
		dev_info(&client->dev, "[%s] : Matched TIER IV MP compatible: %s\n", __func__, priv->compatible);
	} else {
		priv->cam_type = TIER4_CAMERA_TYPE_STANDARD;
		dev_info(&client->dev, "[%s] : Matched TIER IV compatible: %s\n", __func__, priv->compatible);
	}

	mutex_init(&priv->lock);

	if (of_get_property(node, "is-prim-isp", NULL)) {
		if (prim_priv__[channel_count_gw5300] &&
		    channel_count_gw5300 >= MAX_CHANNEL_NUM) {
			dev_err(&client->dev,
				"[%s] : prim-isp already exists\n", __func__);
			return -EEXIST;
		}

		err = of_property_read_u32(node, "reg", &priv->def_addr);
		if (err < 0) {
			dev_err(&client->dev, "[%s] : reg not found\n",
				__func__);
			return -EINVAL;
		}

		prim_priv__[channel_count_gw5300] = priv;
		channel_count_gw5300++;
	}

	dev_set_drvdata(&client->dev, priv);

	/* dev communication gets validated when GMSL link setup is done */
	dev_info(&client->dev, "[%s] :  Probing succeeded\n", __func__);

	return err;
}

static int tier4_gw5300_remove(struct i2c_client *client)
{
	struct tier4_gw5300 *priv;

	if (channel_count_gw5300 > 0)
		channel_count_gw5300--;

	if (client != NULL) {
		priv = dev_get_drvdata(&client->dev);
		mutex_destroy(&priv->lock);
	}

	return 0;
}

static const struct i2c_device_id tier4_gw5300_id[] = {
	{ "tier4_gw5300", 0 },
	{},
};
MODULE_DEVICE_TABLE(i2c, tier4_gw5300_id);

static struct i2c_driver tier4_gw5300_i2c_driver = {
	.driver = {
		.name = "tier4_gw5300",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(tier4_gw5300_of_match),
	},
	.probe = tier4_gw5300_probe,
	.remove = tier4_gw5300_remove,
	.id_table = tier4_gw5300_id,
};

static int __init tier4_gw5300_init(void)
{
	pr_info("ISP Driver for TIERIV Camera.\n");

	c3_mode_seqs[GW5300_MASTER_MODE_5FPS] = c3_master_5fps_seq;
	c3_mode_seqs[GW5300_SLAVE_MODE_5FPS] = c3_slave_5fps_seq;

	c3_mode_seqs[GW5300_MASTER_MODE_10FPS] = c3_master_10fps_seq;
	c3_mode_seqs[GW5300_MASTER_MODE_10FPS_EBD] = c3_master_10fps_ebd_seq;
	c3_mode_seqs[GW5300_SLAVE_MODE_10FPS] = c3_slave_10fps_seq;
	c3_mode_seqs[GW5300_SLAVE_MODE_10FPS_EBD] = c3_slave_10fps_ebd_seq;

	c3_mode_seqs[GW5300_MASTER_MODE_20FPS] = c3_master_20fps_seq;
	c3_mode_seqs[GW5300_MASTER_MODE_20FPS_EBD] = c3_master_20fps_ebd_seq;
	c3_mode_seqs[GW5300_SLAVE_MODE_20FPS] = c3_slave_20fps_seq;
	c3_mode_seqs[GW5300_SLAVE_MODE_20FPS_EBD] = c3_slave_20fps_ebd_seq;

	c3_mode_seqs[GW5300_MASTER_MODE_30FPS] = c3_master_30fps_seq;
	c3_mode_seqs[GW5300_MASTER_MODE_30FPS_EBD] = c3_master_30fps_ebd_seq;
	c3_mode_seqs[GW5300_SLAVE_MODE_30FPS] = c3_slave_30fps_seq;
	c3_mode_seqs[GW5300_SLAVE_MODE_30FPS_EBD] = c3_slave_30fps_ebd_seq;

	return i2c_add_driver(&tier4_gw5300_i2c_driver);
}

static void __exit tier4_gw5300_exit(void)
{
	i2c_del_driver(&tier4_gw5300_i2c_driver);
}

module_init(tier4_gw5300_init);
module_exit(tier4_gw5300_exit);

MODULE_DESCRIPTION("GW5300 ISP driver tier4_gw5300");
MODULE_AUTHOR("Kohji Iwasaki");
MODULE_LICENSE("GPL v2");
