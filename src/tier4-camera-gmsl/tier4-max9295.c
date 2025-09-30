/*
 * tier4_max9295.c - tier4_max9295 GMSL Serializer driver
 *
 * Copyright (c) 2022-2023, TIERIV Inc.  All rights reserved.
 * Copyright (c) 2020, Leopard Imaging Inc.  All rights reserved.
 * Copyright (c) 2018-2020, NVIDIA CORPORATION.  All rights reserved.
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
#include <media/camera_common.h>

#include "tier4-max9295.h"
#include "tier4-gmsl-link.h"

#define MAX9295_SHOW_I2C_WRITE_MSG 0

/* register specifics */

#define MAX9295_DEV_ADDR 0x0000
#define MAX9295_PIPE_EN_ADDR 0x0002
#define MAX9295_CLK_OUTPUT_ADDR 0x0003
#define MAX9295_CTRL0_ADDR 0x0010

#define MAX9295_I2C4_ADDR 0x0044
#define MAX9295_I2C5_ADDR 0x0045

#define MAX9295_CFGV_VIDEO_Y_ADDR 0x0057
#define MAX9295_CFGV_VIDEO_Z_ADDR 0x005B

#define MAX9295_VIDEO_TX0_ADDR 0x0110
#define MAX9295_VIDEO_TX1_ADDR 0x0111
#define MAX9295_VIDEO_TX2_ADDR 0x0112
#define MAX9295_SRC_CTRL_ADDR 0x02BF
#define MAX9295_SRC_PWDN_ADDR 0x02BE

#define MAX9295_GPIO_3_CONFIG_MFP3_ADDR 0x02C7
#define MAX9295_GPIO_3_GPIO_TX_ID_ADDR 0x02C8
#define MAX9295_GPIO_3_GPIO_RX_ID_ADDR 0x02C9
#define MAX9295_GPIO_4_ADDR 0x02CA
#define MAX9295_GPIO_5_ADDR 0x02CD
#define MAX9295_GPIO_6_ADDR 0x02D0
#define MAX9295_GPIO_7_ADDR 0x02D3
#define MAX9295_GPIO_8_ADDR 0x02D6

#define MAX9295_CSI_PORT_SEL_ADDR 0x0308

#define MAX9295_START_PIPE_ADDR 0x0311
#define MAX9295_SEND_Z_PIPE_ADDR 0x0312
#define MAX9295_PIPE_X_DT_ADDR 0x0314
#define MAX9295_PIPE_Y_DT_ADDR 0x0316
#define MAX9295_PIPE_Z_DT_ADDR 0x0318
#define MAX9295_PIPE_U_DT_ADDR 0x031A
#define MAX9295_OVERRIDE_Z_PIPE_ADDR 0x031E

#define MAX9295_MIPI_RX0_ADDR 0x0330
#define MAX9295_MIPI_RX1_ADDR 0x0331
#define MAX9295_MIPI_RX2_ADDR 0x0332
#define MAX9295_MIPI_RX3_ADDR 0x0333

#define MAX9295_REF_VTG0_ADDR 0x03F0
#define MAX9295_SRC_OUT_RCLK_ADDR 0x03F1

#define MAX9295_REG570_ADDR 0x0570

#define MAX9295_STREAM_PIPE_UNUSED 0x22
#define MAX9295_CSI_MODE_1X4 0x00
#define MAX9295_CSI_MODE_2X2 0x03
#define MAX9295_CSI_MODE_2X4 0x06

#define MAX9295_CSI_PORT_B(num_lanes) (((num_lanes) << 4) & 0xF0)
#define MAX9295_CSI_PORT_A(num_lanes) ((num_lanes)&0x0F)

#define MAX9295_CSI_1X4_MODE_LANE_MAP1 0xE0
#define MAX9295_CSI_1X4_MODE_LANE_MAP2 0x04

#define MAX9295_CSI_2X4_MODE_LANE_MAP1 0xEE
#define MAX9295_CSI_2X4_MODE_LANE_MAP2 0xE4

#define MAX9295_CSI_2X2_MODE_LANE_MAP1 MAX9295_CSI_2X4_MODE_LANE_MAP1
#define MAX9295_CSI_2X2_MODE_LANE_MAP2 MAX9295_CSI_2X4_MODE_LANE_MAP2

#define MAX9295_ST_ID_0 0x0
#define MAX9295_ST_ID_1 0x1
#define MAX9295_ST_ID_2 0x2
#define MAX9295_ST_ID_3 0x3

#define MAX9295_PIPE_X_START_B 0x80
#define MAX9295_PIPE_Y_START_B 0x40
#define MAX9295_PIPE_Z_START_B 0x20
#define MAX9295_PIPE_U_START_B 0x10

#define MAX9295_PIPE_X_START_A 0x1
#define MAX9295_PIPE_Y_START_A 0x2
#define MAX9295_PIPE_Z_START_A 0x4
#define MAX9295_PIPE_U_START_A 0x8

#define MAX9295_START_PORT_A 0x10
#define MAX9295_START_PORT_B 0x20

#define MAX9295_CSI_LN2 0x1
#define MAX9295_CSI_LN4 0x3

#define MAX9295_EN_LINE_INFO 0x40

#define MAX9295_VID_TX_EN_X 0x10
#define MAX9295_VID_TX_EN_Y 0x20
#define MAX9295_VID_TX_EN_Z 0x40
#define MAX9295_VID_TX_EN_U 0x80

#define MAX9295_VID_INIT 0x3
#define MAX9295_SRC_RCLK 0x89

#define MAX9295_RESET_ALL 0x80
#define MAX9295_RESET_SRC 0x60
#define MAX9295_PWDN_GPIO 0x90

#define MAX9295_MAX_PIPES 0x4

struct tier4_max9295_client_ctx {
	struct tier4_gmsl_link_ctx *g_ctx;
	bool st_done;
};

struct tier4_max9295 {
	struct i2c_client *i2c_client;
	struct regmap *regmap;
	struct tier4_max9295_client_ctx g_client;
	struct mutex lock;
	/* primary serializer properties */
	__u32 def_addr;
	__u32 pst2_ref;

	atomic_t work_canceled;
	bool last_fault_state;
	struct timer_list hw_monitor_timer;
	struct work_struct hw_monitor_work;
	struct v4l2_subdev *subdev;
};

#define MAX_CHANNEL_NUM 8

/* count channel,the max MAX_CHANNEL_NUM*/
static __u32 channel_count_isx021;

static struct tier4_max9295 *prim_priv__[MAX_CHANNEL_NUM];

struct map_ctx {
	u8 dt;
	u16 addr;
	u8 val;
	u8 st_id;
};

#if 1

static int tier4_max9295_read_reg(struct device *dev, u16 addr, u8 *val)
{
    int err = 0;
    u32 reg_val = 0;
    struct tier4_max9295 *priv = dev_get_drvdata(dev);

    err = regmap_read(priv->regmap, addr, &reg_val);

    *val = reg_val & 0xFF;

    //dev_info(dev,  "[%s ] : Max9295 I2C Read at 0x%04X=[0x%02X].\n", __func__, addr, *val );

    return err;
}
#endif

static int tier4_max9295_write_reg(struct device *dev, u16 addr, u8 val)
{
	struct tier4_max9295 *priv = dev_get_drvdata(dev);
	int err = 0;
	//    u8 e;
	char str_bus_num[4], str_sl_addr[4];
	int len;

	memset(str_bus_num, 0, 4);
	memset(str_sl_addr, 0, 4);

	len = strlen(dev->kobj.name);

	if ((err == 0) && (dev != NULL)) {
		strncpy(str_bus_num, &dev->kobj.name[0], 2);
		strncpy(str_sl_addr, &dev->kobj.name[len - 2], 2);
	}

	err = regmap_write(priv->regmap, addr, val);

	if (err) {
		dev_err(dev,
			"[%s] : Max9295 I2C write failed Reg at 0x%04X:[0x%02X].\n",
			__func__, addr, val);
	}
#if MAX9295_SHOW_I2C_WRITE_MSG
	else {
		dev_info(
			dev,
			"[%s] : Max9295 I2C write register at 0x%04X:[0x%02X]\n",
			__func__, addr, val);
	}
#endif

	/* delay before next i2c command as required for SERDES link */

	usleep_range(100, 110);
	//msleep(100);
	//    tier4_max9295_read_reg(dev, addr, &e);
	usleep_range(100, 110);
	//msleep(100);

	return err;
}

int tier4_max9295_setup_streaming(struct device *dev)
{
	struct tier4_max9295 *priv = dev_get_drvdata(dev);
	int err = 0;
	u32 csi_mode;
	u32 lane_map1;
	u32 lane_map2;
	u32 port;
	u32 rx1_lanes;
	u32 st_pipe;
	u32 pipe_en;
	u32 port_sel = 0;
	struct tier4_gmsl_link_ctx *g_ctx;
	u32 i;
	u32 j;
	u32 st_en;

	struct map_ctx map_pipe_dtype[] = {
		{ GMSL_CSI_DT_YUV_8, MAX9295_PIPE_Z_DT_ADDR, 0x1E, // For YUV8
		  MAX9295_ST_ID_2 },
		{ GMSL_CSI_DT_UED_U1, MAX9295_PIPE_X_DT_ADDR,
		  0x30, // User defined embedded data type
		  MAX9295_ST_ID_0 },
		{ GMSL_CSI_DT_EMBED, MAX9295_PIPE_Y_DT_ADDR, 0x12,
		  MAX9295_ST_ID_1 },
	};

	mutex_lock(&priv->lock);

	if (!priv->g_client.g_ctx) {
		dev_err(dev, "[%s] : No sdev client found\n", __func__);
		err = -EINVAL;
		goto error;
	}

	if (priv->g_client.st_done) {
		dev_dbg(dev, "[%s] : Stream setup is already done\n", __func__);
		goto error;
	}

	g_ctx = priv->g_client.g_ctx;

	switch (g_ctx->csi_mode) {
	case GMSL_CSI_1X4_MODE:
		csi_mode = MAX9295_CSI_MODE_1X4;
		lane_map1 = MAX9295_CSI_1X4_MODE_LANE_MAP1;
		lane_map2 = MAX9295_CSI_1X4_MODE_LANE_MAP2;
		rx1_lanes = MAX9295_CSI_LN4;
		break;
	case GMSL_CSI_2X2_MODE:
		csi_mode = MAX9295_CSI_MODE_2X2;
		lane_map1 = MAX9295_CSI_2X2_MODE_LANE_MAP1;
		lane_map2 = MAX9295_CSI_2X2_MODE_LANE_MAP2;
		rx1_lanes = MAX9295_CSI_LN2;
		break;
	case GMSL_CSI_2X4_MODE:
		csi_mode = MAX9295_CSI_MODE_2X4;
		lane_map1 = MAX9295_CSI_2X4_MODE_LANE_MAP1;
		lane_map2 = MAX9295_CSI_2X4_MODE_LANE_MAP2;
		rx1_lanes = MAX9295_CSI_LN4;
		break;
	default:
		dev_err(dev, "[%s] : Invalid csi mode\n", __func__);
		err = -EINVAL;
		goto error;
	}

	port = (g_ctx->src_csi_port == GMSL_CSI_PORT_B) ?
		       MAX9295_CSI_PORT_B(rx1_lanes) :
		       MAX9295_CSI_PORT_A(rx1_lanes);

	tier4_max9295_write_reg(dev, MAX9295_MIPI_RX0_ADDR, csi_mode);
	tier4_max9295_write_reg(dev, MAX9295_MIPI_RX1_ADDR, port);
	tier4_max9295_write_reg(dev, MAX9295_MIPI_RX2_ADDR, lane_map1);
	tier4_max9295_write_reg(dev, MAX9295_MIPI_RX3_ADDR, lane_map2);

	for (i = 0; i < g_ctx->num_streams; i++) {
		struct tier4_gmsl_stream *g_stream = &g_ctx->streams[i];

		g_stream->st_id_sel = GMSL_ST_ID_UNUSED;

		for (j = 0; j < ARRAY_SIZE(map_pipe_dtype); j++) {
			if (map_pipe_dtype[j].dt == g_stream->st_data_type) {
				/*
         * TODO:
         * 1) Remove link specific overrides, depends
         * on #2.
         * 2) Add support for vc id based stream sel
         * overrides TX_SRC_SEL. would be useful in
         * using same mappings in all ser devs.
         */
				if (g_ctx->serdes_csi_link ==
				    GMSL_SERDES_CSI_LINK_B) {
					map_pipe_dtype[j].addr += 2;
					map_pipe_dtype[j].st_id += 1;
				}

				g_stream->st_id_sel = map_pipe_dtype[j].st_id;

				st_en = (map_pipe_dtype[j].addr ==
					 MAX9295_PIPE_X_DT_ADDR) ?
						0xC0 :
						0x40;

				tier4_max9295_write_reg(
					dev, map_pipe_dtype[j].addr,
					(st_en | map_pipe_dtype[j].val));
			}
		}
	}

	for (i = 0; i < g_ctx->num_streams; i++)
		if (g_ctx->streams[i].st_id_sel != GMSL_ST_ID_UNUSED)
			port_sel |= (1 << g_ctx->streams[i].st_id_sel);

	if (g_ctx->src_csi_port == GMSL_CSI_PORT_B) {
		st_pipe = (MAX9295_PIPE_X_START_B | MAX9295_PIPE_Y_START_B |
			   MAX9295_PIPE_Z_START_B | MAX9295_PIPE_U_START_B);
		port_sel |= (MAX9295_EN_LINE_INFO | MAX9295_START_PORT_B);
	} else {
		st_pipe = MAX9295_PIPE_X_START_A | MAX9295_PIPE_Y_START_A |
			  MAX9295_PIPE_Z_START_A | MAX9295_PIPE_U_START_A;
		port_sel |= (MAX9295_EN_LINE_INFO | MAX9295_START_PORT_A);
	}

	pipe_en =
		(MAX9295_VID_TX_EN_X | MAX9295_VID_TX_EN_Y |
		 MAX9295_VID_TX_EN_Z | MAX9295_VID_TX_EN_U | MAX9295_VID_INIT);

	tier4_max9295_write_reg(dev, MAX9295_START_PIPE_ADDR, st_pipe);
	tier4_max9295_write_reg(dev, MAX9295_CSI_PORT_SEL_ADDR, port_sel);

	tier4_max9295_write_reg(dev, MAX9295_PIPE_EN_ADDR, pipe_en);

	priv->g_client.st_done = true;

error:
	mutex_unlock(&priv->lock);
	return err;
}
EXPORT_SYMBOL(tier4_max9295_setup_streaming);

// ------------------------------------------------------------------

int tier4_max9295_control_sensor_power_seq(struct device *dev, __u32 sensor_id,
					   bool power_on)
{
	struct tier4_max9295 *priv = dev_get_drvdata(dev);
	struct tier4_gmsl_link_ctx *g_ctx;
	int err = 0;

	g_ctx = priv->g_client.g_ctx;
	
#if 1
	
	msleep(200);

	if (power_on == true) { // power up camera sensor

		err += tier4_max9295_write_reg(dev, MAX9295_GPIO_8_ADDR, 0x00);

		msleep(100);

		if (sensor_id == SENSOR_ID_ISX021) {
			err += tier4_max9295_write_reg(dev, MAX9295_GPIO_5_ADDR,
						       0x04);
			msleep(100);
			err += tier4_max9295_write_reg(dev, MAX9295_GPIO_4_ADDR,
						       0x10);
			msleep(100);
		} else if (sensor_id == SENSOR_ID_IMX728) {
			err += tier4_max9295_write_reg(dev, MAX9295_GPIO_4_ADDR,
						       0x10);
			msleep(100);
		}
		err += tier4_max9295_write_reg(dev, MAX9295_GPIO_8_ADDR, 0x10);
	} else { // power down camera sensor
		err += tier4_max9295_write_reg(dev, MAX9295_GPIO_8_ADDR, 0x00);
		msleep(100);
		if (sensor_id == SENSOR_ID_ISX021) {
			err += tier4_max9295_write_reg(dev, MAX9295_GPIO_5_ADDR,
						       0x04);
			msleep(100);
			err += tier4_max9295_write_reg(dev, MAX9295_GPIO_4_ADDR,
						       0x00);
			msleep(100);
		} else if (sensor_id == SENSOR_ID_IMX728) {
			err += tier4_max9295_write_reg(dev, MAX9295_GPIO_4_ADDR,
						       0x00);
			msleep(100);
		}
	}

	if (err) {
		dev_err(dev,
			"[%s] : Power on/off Camera Sensor or ISP failed.\n",
			__func__);
	}

	msleep(50);
#endif
	return err;
}
EXPORT_SYMBOL(tier4_max9295_control_sensor_power_seq);

void tier4_max9295_set_v4l2_subdev(struct device *dev,
					struct v4l2_subdev *subdev)
{
	struct tier4_max9295 *priv = dev_get_drvdata(dev);

	mutex_lock(&priv->lock);
	priv->subdev = subdev;
	mutex_unlock(&priv->lock);
}
EXPORT_SYMBOL(tier4_max9295_set_v4l2_subdev);

void tier4_max9295_unset_v4l2_subdev(struct device *dev)
{
	struct tier4_max9295 *priv = dev_get_drvdata(dev);

	mutex_lock(&priv->lock);
	priv->subdev = NULL;
	mutex_unlock(&priv->lock);
}
EXPORT_SYMBOL(tier4_max9295_unset_v4l2_subdev);

int tier4_max9295_setup_gpo(struct device *dev)
{
	int err = 0;

	dev_info(dev, "[%s] : Set up max9295 for fsync trigger mode \n",
		 __func__);

	err += tier4_max9295_write_reg(dev, MAX9295_GPIO_3_CONFIG_MFP3_ADDR,
				       0x04);
	err += tier4_max9295_write_reg(dev, MAX9295_GPIO_3_GPIO_TX_ID_ADDR,
				       0x00);
	err += tier4_max9295_write_reg(dev, MAX9295_GPIO_3_GPIO_RX_ID_ADDR,
				       0x06);

	return err;
}
EXPORT_SYMBOL(tier4_max9295_setup_gpo);

int tier4_max9295_setup_control(struct device *dev)
{
	struct tier4_max9295 *priv = dev_get_drvdata(dev);
	int err = 0;
	struct tier4_gmsl_link_ctx *g_ctx;
	u32 offset1 = 0;
	u32 offset2 = 0;
	u32 i;
	int retry = 2;

	u8 i2c_ovrd[] = {
		0x6B, 0x10, 0x73, 0x11, 0x7B, 0x30, 0x83, 0x30, 0x93,
		0x30, 0x9B, 0x30, 0xA3, 0x30, 0xAB, 0x30, 0x8B, 0x30,
	};

	u8 addr_offset[] = {
		0x80, 0x00, 0x00, 0x84, 0x00, 0x01,
		0xC0, 0x02, 0x02, 0xC4, 0x02, 0x03,
	};

	mutex_lock(&priv->lock);

	g_ctx = priv->g_client.g_ctx;

	if (prim_priv__[g_ctx->reg_mux] == NULL) {
		err = -EINVAL;
		goto error;
	}
	/* update address reassingment */
	err = tier4_max9295_write_reg(&prim_priv__[g_ctx->reg_mux]->i2c_client->dev,
				MAX9295_DEV_ADDR, (g_ctx->ser_reg << 1));
	while (err && retry--) {
		dev_err(dev, "[%s] : Failed to set serializer reg addr, retry=%d\n", __func__, retry);
		msleep(100);
		err = tier4_max9295_write_reg(&prim_priv__[g_ctx->reg_mux]->i2c_client->dev,
				MAX9295_DEV_ADDR, (g_ctx->ser_reg << 1));
	}

	msleep(100);

	if (g_ctx->serdes_csi_link == GMSL_SERDES_CSI_LINK_A) {
		err = tier4_max9295_write_reg(dev, MAX9295_CTRL0_ADDR, 0x21);
	} else {
		err = tier4_max9295_write_reg(dev, MAX9295_CTRL0_ADDR, 0x22);
	}

	/* check if serializer device exists */
	if (err) {
		dev_err(dev, "[%s]: Ser device not found\n", __func__);
		goto error;
	}
	/* delay to settle link */
	msleep(100);

	/* Set RCLKOUT soruce to the reference PLL clock*/
	err = tier4_max9295_write_reg(dev, MAX9295_CLK_OUTPUT_ADDR, 0x03);

	/* No need?*/
	usleep_range(10000, 11000);

	/* PLL setting & Reset PLL */
	err = tier4_max9295_write_reg(dev, MAX9295_REF_VTG0_ADDR, 0x5A);
	usleep_range(10000, 11000);
	//  err = tier4_max9295_write_reg(dev, MAX9295_REG570_ADDR, 0x0C);

	for (i = 0; i < ARRAY_SIZE(addr_offset); i += 3) {
		if ((g_ctx->ser_reg << 1) == addr_offset[i]) {
			offset1 = addr_offset[i + 1];
			offset2 = addr_offset[i + 2];
			break;
		}
	}

	if (i == ARRAY_SIZE(addr_offset)) {
		dev_err(dev, "[%s] : Invalid ser slave address\n", __func__);
		err = -EINVAL;
		goto error;
	}

	for (i = 0; i < ARRAY_SIZE(i2c_ovrd); i += 2) {
		/* update address overrides */
		i2c_ovrd[i + 1] += (i < 4) ? offset1 : offset2;

		/* i2c passthrough2 must be configured once for all devices */
		if ((i2c_ovrd[i] == 0x8B) &&
		    prim_priv__[g_ctx->reg_mux]->pst2_ref)
			continue;

		tier4_max9295_write_reg(dev, i2c_ovrd[i], i2c_ovrd[i + 1]);
	}

	/* dev addr pass-through2 ref */

	prim_priv__[g_ctx->reg_mux]->pst2_ref++;

	tier4_max9295_write_reg(dev, MAX9295_I2C4_ADDR, (g_ctx->sdev_reg << 1));
	tier4_max9295_write_reg(dev, MAX9295_I2C5_ADDR, (g_ctx->sdev_def << 1));

	tier4_max9295_write_reg(dev, MAX9295_SRC_PWDN_ADDR, MAX9295_PWDN_GPIO);
	tier4_max9295_write_reg(dev, MAX9295_SRC_CTRL_ADDR, MAX9295_RESET_SRC);
	tier4_max9295_write_reg(dev, MAX9295_SRC_OUT_RCLK_ADDR,
				MAX9295_SRC_RCLK);

	/* PLL setting & Enable PLL */
	tier4_max9295_write_reg(dev, MAX9295_REF_VTG0_ADDR, 0x59);

	g_ctx->serdev_found = true;

error:

	mutex_unlock(&priv->lock);

	return err;
}
EXPORT_SYMBOL(tier4_max9295_setup_control);

int tier4_max9295_reset_control(struct device *dev)
{
	struct tier4_max9295 *priv = dev_get_drvdata(dev);
	int err = 0;
	
	mutex_lock(&priv->lock);
	if (!priv->g_client.g_ctx) {
		dev_err(dev, "[%s] : No sdev client found\n", __func__);
		err = -EINVAL;
		goto error;
	}

	prim_priv__[priv->g_client.g_ctx->reg_mux]->pst2_ref--;

	priv->g_client.st_done = false;

	tier4_max9295_write_reg(
		dev, MAX9295_DEV_ADDR,
		(prim_priv__[priv->g_client.g_ctx->reg_mux]->def_addr << 1));	
	
	tier4_max9295_write_reg(
		&prim_priv__[priv->g_client.g_ctx->reg_mux]->i2c_client->dev,
		MAX9295_CTRL0_ADDR, MAX9295_RESET_ALL);

error:
	mutex_unlock(&priv->lock);
	return err;
}
EXPORT_SYMBOL(tier4_max9295_reset_control);

int tier4_max9295_sdev_pair(struct device *dev,
			    struct tier4_gmsl_link_ctx *g_ctx)
{
	struct tier4_max9295 *priv;
	int err = 0;

	if (!dev || !g_ctx || !g_ctx->s_dev) {
		dev_err(dev, "[%s]: Invalid input params\n", __func__);
		return -EINVAL;
	}

	priv = dev_get_drvdata(dev);
	mutex_lock(&priv->lock);
	if (priv->g_client.g_ctx) {
		dev_err(dev, "[%s] : Device already paired\n", __func__);
		err = -EINVAL;
		goto error;
	}
	priv->g_client.st_done = false;

	priv->g_client.g_ctx = g_ctx;

error:
	mutex_unlock(&priv->lock);
	return 0;
}
EXPORT_SYMBOL(tier4_max9295_sdev_pair);

int tier4_max9295_sdev_unpair(struct device *dev, struct device *s_dev)
{
	struct tier4_max9295 *priv = NULL;
	int err = 0;

	if (!dev || !s_dev) {
		dev_err(dev, "[%s] : Invalid input params\n", __func__);
		return -EINVAL;
	}

	priv = dev_get_drvdata(dev);

	mutex_lock(&priv->lock);

	if (!priv->g_client.g_ctx) {
		dev_err(dev, "[%s] : Device is not paired\n", __func__);
		err = -ENOMEM;
		goto error;
	}

	if (priv->g_client.g_ctx->s_dev != s_dev) {
		dev_err(dev, "[%s] : Invalid device\n", __func__);
		err = -EINVAL;
		goto error;
	}

	priv->g_client.g_ctx = NULL;
	priv->g_client.st_done = false;

error:
	mutex_unlock(&priv->lock);
	return err;
}
EXPORT_SYMBOL(tier4_max9295_sdev_unpair);


static ssize_t
mfp7_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	u8 val;
	int err;
	struct tier4_max9295 *priv = dev_get_drvdata(dev);

	mutex_lock(&priv->lock);
	err = tier4_max9295_read_reg(dev, MAX9295_GPIO_7_ADDR, &val);
	mutex_unlock(&priv->lock);

	return err ? err : sprintf(buf, "%x\n", val);
}

static ssize_t
mfp7_store(struct device *dev, struct device_attribute *attr,
			const char *buf, size_t count)
{
	long long res;
	int err;
	struct tier4_max9295 *priv = dev_get_drvdata(dev);

	err = kstrtoull(buf, 16, &res);
	if (err)
		return err;

	mutex_lock(&priv->lock);
	err = tier4_max9295_write_reg(dev, MAX9295_GPIO_7_ADDR, res);
	mutex_unlock(&priv->lock);

	return err ? err : count;
}
static DEVICE_ATTR_RW(mfp7);

static void hw_monitor_timer_callback(struct timer_list *timer)
{
	struct tier4_max9295 *priv =
		container_of(timer, struct tier4_max9295, hw_monitor_timer);

	if (!atomic_read(&priv->work_canceled))
		schedule_work(&priv->hw_monitor_work);
}

static void tier4_max9295_check_sensor_error(struct work_struct *work)
{
	u8 val;
	int err = 0;
	char camera_id[10 + V4L2_DEVICE_NAME_SIZE] = {0};
	char major[32], minor[32];
	char fusa_hw_fault[32];
	char *envp[] = {
		camera_id,
		major,
		minor,
		fusa_hw_fault,
		NULL
	};
	bool fault_state;

	struct tier4_max9295 *priv =
		container_of(work, struct tier4_max9295, hw_monitor_work);
	struct device *dev = &priv->i2c_client->dev;
	struct v4l2_subdev *subdev = priv->subdev;

	if (atomic_read(&priv->work_canceled))
		return;

	mod_timer(&priv->hw_monitor_timer, jiffies + (HZ / 10));

	if (!subdev)
		return;

	mutex_lock(&priv->lock);
	err = tier4_max9295_read_reg(dev, MAX9295_GPIO_7_ADDR, &val);
	if (err) {
		dev_err(dev, "failed to read the MFP7 pin: %d\n", err);
		mutex_unlock(&priv->lock);
		return;
	}

	fault_state = !(val & 0x08);
	if (priv->last_fault_state == fault_state) {
		mutex_unlock(&priv->lock);
		return;
	}

	priv->last_fault_state = fault_state;
	mutex_unlock(&priv->lock);

	dev_err_ratelimited(dev, "hardware fault of the image sensor %s\n",
			fault_state ? "detected" : "cleared");

	snprintf(camera_id, sizeof camera_id, "CAMERA_ID=%s", subdev->name);

	if (!subdev->devnode) {
		dev_warn(dev, "subdev->devnode is NULL\n");
		return;
	}
	
	snprintf(major, sizeof major, "SUBDEV_MAJOR=%d",
			MAJOR(subdev->devnode->dev.devt));

	if (!subdev->devnode->cdev) {
		dev_warn(dev, "subdev->devnode->cdev is NULL\n");
		return;
	}

	snprintf(minor, sizeof minor, "SUBDEV_MINOR=%d",
			MINOR(subdev->devnode->cdev->dev));
	snprintf(fusa_hw_fault, sizeof fusa_hw_fault, "FUSA_HW_FAULT=%d",
			fault_state);
	kobject_uevent_env(&subdev->v4l2_dev->dev->kobj, KOBJ_CHANGE, envp);
}

static struct regmap_config tier4_max9295_regmap_config = {
	.reg_bits = 16,
	.val_bits = 8,
	.cache_type = REGCACHE_NONE,
};

static int tier4_max9295_probe(struct i2c_client *client,
			       const struct i2c_device_id *id)
{
	struct tier4_max9295 *priv;
	int err = 0;
	struct device_node *node = client->dev.of_node;

	dev_info(&client->dev, "[%s] : probing GMSL Serializer\n", __func__);

	priv = devm_kzalloc(&client->dev, sizeof(*priv), GFP_KERNEL);
	dev_set_drvdata(&client->dev, priv);
	priv->i2c_client = client;
	priv->regmap = devm_regmap_init_i2c(priv->i2c_client,
					    &tier4_max9295_regmap_config);
	if (IS_ERR(priv->regmap)) {
		dev_err(&client->dev, "[%s] : devm_regmap_init() failed: %ld\n",
			__func__, PTR_ERR(priv->regmap));
		return -ENODEV;
	}

	mutex_init(&priv->lock);

	if (of_get_property(node, "is-prim-ser", NULL)) {
		if (prim_priv__[channel_count_isx021] &&
		    channel_count_isx021 >= MAX_CHANNEL_NUM) {
			dev_err(&client->dev,
				"[%s] : prim-ser already exists\n", __func__);
			return -EEXIST;
		}

		err = of_property_read_u32(node, "reg", &priv->def_addr);
		if (err < 0) {
			dev_err(&client->dev, "[%s] : reg not found\n",
				__func__);
			return -EINVAL;
		}

		if (priv == NULL) {
			dev_err(&client->dev, "[%s] : priv is NULL\n",
				__func__);
			return err;
		}

		prim_priv__[channel_count_isx021] = priv;

		channel_count_isx021++;
	}

	device_create_file(&client->dev, &dev_attr_mfp7);

	atomic_set(&priv->work_canceled, 0);
	priv->last_fault_state = false;
	INIT_WORK(&priv->hw_monitor_work, tier4_max9295_check_sensor_error);

	timer_setup(&priv->hw_monitor_timer, hw_monitor_timer_callback,
			TIMER_PINNED | TIMER_IRQSAFE);
	mod_timer(&priv->hw_monitor_timer, jiffies + (HZ / 10));

	/* dev communication gets validated when GMSL link setup is done */
	dev_info(&client->dev, "[%s] :  Probing succeeded\n", __func__);

	return err;
}

static int tier4_max9295_remove(struct i2c_client *client)
{
	struct tier4_max9295 *priv = dev_get_drvdata(&client->dev);

	atomic_set(&priv->work_canceled, 1);
	del_timer_sync(&priv->hw_monitor_timer);
	cancel_work_sync(&priv->hw_monitor_work);

	device_remove_file(&client->dev, &dev_attr_mfp7);

	if (channel_count_isx021 > 0)
		channel_count_isx021--;

	mutex_destroy(&priv->lock);

	return 0;
}

static void tier4_max9295_shutdown(struct i2c_client *client)
{
	struct tier4_max9295 *priv = dev_get_drvdata(&client->dev);

	atomic_set(&priv->work_canceled, 1);
	del_timer_sync(&priv->hw_monitor_timer);
	cancel_work_sync(&priv->hw_monitor_work);

	device_remove_file(&client->dev, &dev_attr_mfp7);

	if (channel_count_isx021 > 0)
		channel_count_isx021--;

	mutex_destroy(&priv->lock);
}


static const struct i2c_device_id tier4_max9295_id[] = {
	{ "tier4_max9295", 0 },
	{},
};

const struct of_device_id tier4_max9295_of_match[] = {
	{
		.compatible = "nvidia,tier4_max9295",
	},
	{},
};
MODULE_DEVICE_TABLE(of, tier4_max9295_of_match);
MODULE_DEVICE_TABLE(i2c, tier4_max9295_id);

static struct i2c_driver tier4_max9295_i2c_driver = {
    .driver = {
        .name = "tier4_max9295",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(tier4_max9295_of_match),
    },
    .probe = tier4_max9295_probe,
    .remove = tier4_max9295_remove,
    .shutdown = tier4_max9295_shutdown,
    .id_table = tier4_max9295_id,
};

static int __init tier4_max9295_init(void)
{
	printk(KERN_INFO "MAX9295 Driver for TIER4 C1 camera : %s\n",
	       BUILD_STAMP);

	return i2c_add_driver(&tier4_max9295_i2c_driver);
}

static void __exit tier4_max9295_exit(void)
{
	i2c_del_driver(&tier4_max9295_i2c_driver);
}

module_init(tier4_max9295_init);
module_exit(tier4_max9295_exit);

MODULE_DESCRIPTION("GMSL Serializer driver tier4_max9295");
MODULE_AUTHOR("Originaly NVIDIA Corporation");
MODULE_AUTHOR("K.Iwasaki");
MODULE_AUTHOR("Y.Fujii");
MODULE_LICENSE("GPL v2");
