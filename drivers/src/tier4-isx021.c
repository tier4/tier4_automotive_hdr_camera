/*
 * isx021.c - isx021 sensor driver
 *
 * Copyright (c) 2022, TIERIV INC.  All rights reserved.
 * Copyright (c) 2018-2021, NVIDIA CORPORATION.  All rights reserved.
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

#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/module.h>

#include <linux/device.h>

#include <linux/seq_file.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>

#include "tier4-max9295.h"
#include "tier4-max9296.h"
#include "tier4-gmsl-link.h"
#include <media/tegracam_core.h>
#include "tier4-isx021-ctrl.h"
#include "tier4-isx021.h"

#define NO_ERROR									(0)

#define TIME_120_MILISEC							(120000)
#define TIME_121_MILISEC							(121000)


#define ISX021_DEFAULT_FRAME_LENGTH    	(1125)

enum {
	ISX021_MODE_1920X1280_CROP_30FPS,
	ISX021_MODE_START_STREAM,
	ISX021_MODE_STOP_STREAM,
};

static const int tier4_isx021_30fps[] = {
	30,
};
static const struct camera_common_frmfmt tier4_isx021_frmfmt[] = {
	{{1920, 1280}, tier4_isx021_30fps, 1, 0, ISX021_MODE_1920X1280_CROP_30FPS},
};

const struct of_device_id tier4_isx021_of_match[] = {
	{ .compatible = "nvidia,tier4_isx021",},
	{ },
};

MODULE_DEVICE_TABLE(of, tier4_isx021_of_match);

// If you add new ioctl VIDIOC_S_EXT_CTRLS function, please add new CID to the following table.
// and define the CID number in  nvidia/include/media/tegra-v4l2-camera.h

static const u32 ctrl_cid_list[] = {
	TEGRA_CAMERA_CID_GAIN,
	TEGRA_CAMERA_CID_EXPOSURE,
	TEGRA_CAMERA_CID_EXPOSURE_SHORT,
	TEGRA_CAMERA_CID_FRAME_RATE,
	TEGRA_CAMERA_CID_HDR_EN,
//	TEGRA_CAMERA_CID_DISTORTION_CORRECTION,
};

struct tier4_isx021 {
	struct 	i2c_client				*i2c_client;
	const 	struct i2c_device_id 	*id;
	struct 	v4l2_subdev				*subdev;
	struct 	device					*ser_dev;
	struct 	device					*dser_dev;
	struct 	gmsl_link_ctx			g_ctx;
	u32								frame_length;
	struct 	camera_common_data		*s_data;
	struct 	tegracam_device			*tc_dev;
	bool 							fsync_mode;
	bool 							distortion_correction;
	bool							auto_exposure;
};

static const struct regmap_config tier4_sensor_regmap_config = {
	.reg_bits = 16,
	.val_bits = 8,
	.cache_type = REGCACHE_RBTREE,
};


static int trigger_mode ;
module_param(trigger_mode, int, 0644);

static struct mutex serdes_lock__;

int tier4_isx021_write_reg(struct camera_common_data *s_data, u16 addr, u8 val);
int tier4_isx021_read_reg(struct camera_common_data *s_data, u16 addr, u8 *val);
/* --------------------------------------------------------------------------- */

static int tier4_isx021_set_fsync_trigger_mode(struct tier4_isx021 *priv)
{
	int err = 0;
	struct device *dev = priv->s_data->dev;
	
	err = tier4_max9296_setup_gpi(priv->dser_dev);
	if ( err ) {
		dev_err(dev, "[%s] :tier4_max9296_setup_gpi() failed\n", __func__);
		return err;
	}

	err = tier4_max9295_setup_gpo(priv->ser_dev);
	if ( err ) {
		dev_err(dev, "[%s] : tier4_max9295_setup_gpo() failed\n", __func__);
		return err;
	}

	err = tier4_isx021_ctrl_set_fsync_trigger_mode(priv->s_data);
	if ( err ) {
		dev_err(dev, "[%s] : err = 0x%0X : tier4_isx021_ctrl_setup() failed\n", __func__, err);
		return err;
	} else {
		dev_info(dev, "[%s] : tier4_isx021_ctrl_setup() succeeded\n", __func__);
	}

	return err;
}
/* --------------------------------------------------------------------------- */

static int tier4_isx021_gmsl_serdes_setup(struct tier4_isx021 *priv)
{
	int 			err 	= 0;
	int 			des_err = 0;
	struct device 	*dev;

	if (!priv || !priv->ser_dev || !priv->dser_dev || !priv->i2c_client) {
		return -EINVAL;
	}

	dev = &priv->i2c_client->dev;

	mutex_lock(&serdes_lock__);

	/* For now no separate power on required for serializer device */

	tier4_max9296_power_on(priv->dser_dev);

	/* setup serdes addressing and control pipeline */


	err = tier4_max9296_setup_link(priv->dser_dev, &priv->i2c_client->dev);

	if (err) {
		dev_err(dev, "[%s] : GMSL deserializer link config failed\n", __func__);
		goto error;
	}


	err = tier4_max9295_setup_control(priv->ser_dev);

	/* proceed even if ser setup failed, to setup deser correctly */
	if (err) {
		dev_err(dev, "[%s] : GMSL serializer setup failed\n", __func__);
		goto error;
	}

	des_err = tier4_max9296_setup_control(priv->dser_dev, &priv->i2c_client->dev);

	if (des_err) {
		dev_err(dev, "[%s] : GMSL deserializer : setup failed\n", __func__);
		err = des_err;
	}

error:
	mutex_unlock(&serdes_lock__);
	return err;
}

/* --------------------------------------------------------------------------- */

static void tier4_isx021_gmsl_serdes_reset(struct tier4_isx021 *priv)
{
	mutex_lock(&serdes_lock__);

	/* reset serdes addressing and control pipeline */
	tier4_max9295_reset_control(priv->ser_dev);

	tier4_max9296_reset_control(priv->dser_dev, &priv->i2c_client->dev);

	tier4_max9296_power_off(priv->dser_dev);

	mutex_unlock(&serdes_lock__);
}

/* --------------------------------------------------------------------------- */

static int tier4_isx021_power_on(struct camera_common_data *s_data)
{
	int 							err 	= 0;
	struct camera_common_power_rail *pw 	= s_data->power;
	struct camera_common_pdata 		*pdata 	= s_data->pdata;
	struct device 					*dev 	= s_data->dev;

	dev_dbg(dev, "[%s] : power on\n", __func__);

	if (pdata && pdata->power_on) {

		err = pdata->power_on(pw);

		if (err) {
			dev_err(dev, "[%s] :  failed.\n", __func__);
		}
	   	else
		{
			pw->state = SWITCH_ON;
		}
		return err;
	}

	pw->state = SWITCH_ON;

	return err;
}

/* --------------------------------------------------------------------------- */

static int tier4_isx021_power_off(struct camera_common_data *s_data)
{
	int 							err 	= 0;
	struct camera_common_power_rail *pw 	= s_data->power;
	struct camera_common_pdata 		*pdata 	= s_data->pdata;
	struct device 					*dev 	= s_data->dev;

	if (pdata && pdata->power_off) {

		err = pdata->power_off(pw);

		if (!err) {
			goto power_off_done;
		}
		else
		{
			dev_err(dev, "[%s] : power off failed.\n", __func__);
		}
		return err;
	}

power_off_done:
	pw->state = SWITCH_OFF;

	return err;
}

/* --------------------------------------------------------------------------- */

static int tier4_isx021_power_get(struct tegracam_device *tc_dev)
{
	struct 	camera_common_power_rail 	*pw 	= tc_dev->s_data->power;
	int 								err		= 0;

	pw->state = SWITCH_OFF;

	return err;
}

/* --------------------------------------------------------------------------- */

static int tier4_isx021_power_put(struct tegracam_device *tc_dev)
{
	if (unlikely(!tc_dev->s_data->power)) {
		return -EFAULT;
	}

	return NO_ERROR;
}

/* --------------------------------------------------------------------------- */

static int tier4_isx021_set_group_hold(struct tegracam_device *tc_dev, bool val)
{
	volatile int err = 0;

	return err;
}

/* --------------------------------------------------------------------------- */

static int tier4_isx021_set_gain(struct tegracam_device *tc_dev, s64 val)
{

	int err = 0;
	struct camera_common_data *s_data = tc_dev->s_data;
//	struct device *dev = tc_dev->dev;

	err = tier4_isx021_ctrl_set_gain(s_data, val);

	if (err) {
		dev_err(tc_dev->dev, "[%s] : GAIN control error\n", __func__);
	}

	return err;
}

/* ------------------------------------------------------------------------- */

static int tier4_isx021_set_frame_rate(struct tegracam_device *tc_dev, s64 val)
{

	struct tier4_isx021 *priv = (struct tier4_isx021 *)tegracam_get_privdata(tc_dev);

	/* fixed 10fps */
	priv->frame_length = ISX021_DEFAULT_FRAME_LENGTH;

	return NO_ERROR;
}

/* ------------------------------------------------------------------------- */

static int tier4_isx021_set_auto_exposure(struct tegracam_device *tc_dev)
{

	int err	= 0;
	struct camera_common_data *s_data = tc_dev->s_data;
	struct tier4_isx021 *priv = (struct tier4_isx021 *)tegracam_get_privdata(tc_dev);

	err = tier4_isx021_ctrl_set_auto_exposure(s_data);

	if (err) {
		dev_err(&priv->i2c_client->dev,	"[%s] : Setting Auto exposure mode failed.\n", __func__);
	}

	return err;
}

/* ------------------------------------------------------------------------- */

static int tier4_isx021_set_exposure(struct tegracam_device *tc_dev, s64 val)
{

	int err = 0;
	struct camera_common_data *s_data = tc_dev->s_data;
	struct tier4_isx021 *priv = (struct tier4_isx021 *)tegracam_get_privdata(tc_dev);
	
	err = tier4_isx021_ctrl_set_exposure(s_data, val);

	if (err) {
		dev_err(&priv->i2c_client->dev,	"[%s] : Setting Exposure Time failed.\n", __func__);
	}

	return err;
}

// --------------------------------------------------------------------------------------
//  Enable Distortion Coreection
// --------------------------------------------------------------------------------------
static int tier4_isx021_enable_distortion_correction(struct tegracam_device *tc_dev, bool val)
{
	int err = 0;
	struct camera_common_data *s_data = tc_dev->s_data;

	err = tier4_isx021_ctrl_enable_distortion_correction(s_data, val);

	return err;
}
// --------------------------------------------------------------------------------------
//  If you add new ioctl VIDIOC_S_EXT_CTRLS function, 
//  please add the new memeber and the function at the following table.

static struct tegracam_ctrl_ops tier4_isx021_ctrl_ops = {
	.numctrls 			= ARRAY_SIZE(ctrl_cid_list),
	.ctrl_cid_list 		= ctrl_cid_list,
	.set_gain 			= tier4_isx021_set_gain,
	.set_exposure 		= tier4_isx021_set_exposure,
	.set_exposure_short = tier4_isx021_set_exposure,
	.set_frame_rate 	= tier4_isx021_set_frame_rate,
	.set_group_hold 	= tier4_isx021_set_group_hold,
//	.set_distortion_correction 	= tier4_isx021_enable_distortion_correction,
};

// --------------------------------------------------------------------------------------

static struct camera_common_pdata *tier4_isx021_parse_dt(struct tegracam_device *tc_dev)
{
	struct device 				*dev 				= tc_dev->dev;
	struct device_node 			*node 				= dev->of_node;
	struct camera_common_pdata 	*board_priv_pdata;
	const struct of_device_id 	*match;
	int err;

	if (!node) {
		return NULL;
	}

	match = of_match_device(tier4_isx021_of_match, dev);

	if (!match) {
		dev_err(dev, "[%s] : Failed to find matching dt id\n", __func__);
		return NULL;
	}

	board_priv_pdata = devm_kzalloc(dev, sizeof(*board_priv_pdata), GFP_KERNEL);

	err = of_property_read_string(node, "mclk", &board_priv_pdata->mclk_name);

	if (err) {
		dev_err(dev, "[%s] : mclk not in DT\n", __func__);
	}

	return board_priv_pdata;
}


/* --------------------------------------------------------------------------- */
/*   tier4_isx021_set_mode() can not be needed. But it remains for compatiblity */

static int tier4_isx021_set_mode(struct tegracam_device *tc_dev)
{

	volatile int err 	= 0;

	return err;
}

/* --------------------------------------------------------------------------- */

static int tier4_isx021_start_streaming(struct tegracam_device *tc_dev)
{
	struct tier4_isx021 	*priv = (struct tier4_isx021 *)tegracam_get_privdata(tc_dev);
	struct device 	*dev  = tc_dev->dev;
	int 			err;

	/* enable serdes streaming */

	err = tier4_max9295_setup_streaming(priv->ser_dev);

	if (err) {
		goto exit;
	}

	err = tier4_max9296_setup_streaming(priv->dser_dev, dev);

	if (err) {
		dev_err(dev, "[%s] : Setup Streaming failed\n", __func__);
		goto exit;
	}

	err = tier4_max9295_control_sensor_power_seq(priv->ser_dev);

	if (err) {
		dev_err(dev, "[%s] : Powerup Camera Sensor failed\n", __func__);
		goto exit;
	}

	if( priv->auto_exposure == true )
	{
		err = tier4_isx021_set_auto_exposure(tc_dev);
	}

	if (err) {
		dev_err(dev, "[%s] : Setting Digital Gain to default value failed\n", __func__);
		goto exit;
	}

	if ( priv->distortion_correction == true ) {

		err = tier4_isx021_enable_distortion_correction(tc_dev, true);

		if (err) {
			dev_err(dev, "[%s] : Enabling Distortion Correction  failed\n", __func__);
			goto exit;
		}
		msleep(20);
	}

	dev_info(dev, "[%s] : trigger_mode = %d\n", __func__,  trigger_mode );

	if ( trigger_mode == 1 ) {
		priv->fsync_mode = true;
		dev_info(dev, "[%s] : fsync-mode will be set to 1\n", __func__ );
	}

	dev_info(dev, "[%s] : fsync-mode = %d\n", __func__,  priv->fsync_mode );

	if ( priv->fsync_mode == true ) {

		err = tier4_isx021_set_fsync_trigger_mode(priv);

		if (err) {
			dev_err(dev, "[%s] : Setting camera sensor to Fsync triggered mode failed\n", __func__);
			goto exit;
		}
		msleep(20);
	}

	err = tier4_max9296_start_streaming(priv->dser_dev, dev);

	if (err) {
		dev_err(dev, "[%s] : tier4_max9296_start_stream() failed\n", __func__);
		return err;
	}

	dev_info(dev, "[%s] :  Camera start stream succeeded\n", __func__);

	return NO_ERROR;

exit:

	dev_err(dev, "[%s] :  Camera start stream failed\n", __func__);

	return err;
}

/* --------------------------------------------------------------------------- */

static int tier4_isx021_stop_streaming(struct tegracam_device *tc_dev)
{
	struct device 	*dev  	= tc_dev->dev;
	struct tier4_isx021 	*priv 	= (struct tier4_isx021 *)tegracam_get_privdata(tc_dev);
	int 			err		= 0;

	/* disable serdes streaming */
	err = tier4_max9296_stop_streaming(priv->dser_dev, dev);

	if (err) {
		return err;
	}

	return NO_ERROR;
}

/* --------------------------------------------------------------------------- */

static struct camera_common_sensor_ops tier4_isx021_common_ops = {
	.numfrmfmts 	 = ARRAY_SIZE(tier4_isx021_frmfmt),
	.frmfmt_table 	 = tier4_isx021_frmfmt,
	.power_on 		 = tier4_isx021_power_on,
	.power_off 		 = tier4_isx021_power_off,
	.write_reg 		 = tier4_isx021_write_reg,
	.read_reg 		 = tier4_isx021_read_reg,
	.parse_dt 		 = tier4_isx021_parse_dt,
	.power_get 		 = tier4_isx021_power_get,
	.power_put 		 = tier4_isx021_power_put,
	.set_mode 		 = tier4_isx021_set_mode,
	.start_streaming = tier4_isx021_start_streaming,
	.stop_streaming  = tier4_isx021_stop_streaming,
};

/* --------------------------------------------------------------------------- */

static int tier4_isx021_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	dev_dbg(&client->dev, "%s:\n", __func__);

	return NO_ERROR;
}

/* --------------------------------------------------------------------------- */

static const struct v4l2_subdev_internal_ops tier4_isx021_subdev_internal_ops = {
	.open = tier4_isx021_open,
};

/* --------------------------------------------------------------------------- */

static int tier4_isx021_board_setup(struct tier4_isx021 *priv)
{
	struct tegracam_device 		*tc_dev 		= priv->tc_dev;
	struct device 				*dev 			= tc_dev->dev;
	struct device_node 			*node 			= dev->of_node;
	struct device_node 			*ser_node;
	struct i2c_client 			*ser_i2c 		= NULL;
	struct device_node 			*dser_node;
	struct i2c_client 			*dser_i2c 		= NULL;
	struct device_node 			*gmsl;
	int 						value 			= 0xFFFF;
	const char 					*str_value;
	const char 					*str_value1[2];
	int  						i;
	int 						err;

	err = of_property_read_u32(node, "reg", &priv->g_ctx.sdev_reg);

	if (err < 0) {
		dev_err(dev, "[%s] : reg not found\n", __func__);
		goto error;
	}

	err = of_property_read_u32(node, "def-addr", &priv->g_ctx.sdev_def);

	if (err < 0) {
		dev_err(dev, "[%s] : def-addr not found\n", __func__);
		goto error;
	}

	err = of_property_read_u32(node, "reg_mux", &priv->g_ctx.reg_mux);

	if (err < 0) {
		dev_err(dev, "[%s] : reg_mux not found\n", __func__);
		goto error;
	}

	err = of_property_read_string(node, "fsync-mode", &str_value);

	if (err < 0) {
		dev_err(dev, "[%s] : No fsync-mode found\n", __func__);
		goto error;
	}
	
	if (!strcmp(str_value, "true")) {
		priv->fsync_mode = true;
	} else {
		priv->fsync_mode = false;
	}

	err = of_property_read_string(node, "distortion-correction", &str_value);

	if (err < 0) {
		dev_err(dev, "[%s] : No distortion-correction found\n", __func__);
		goto error;
	}

	if (!strcmp(str_value, "true")) {
		priv->distortion_correction = true;
	} else {
		priv->distortion_correction = false;
	}

	err = of_property_read_string(node, "auto-exposure", &str_value);

	if (err < 0) {
		dev_err(dev, "[%s] : Inavlid Exposure mode.\n", __func__);
		goto error;
	}

	if (!strcmp(str_value, "true")) {
		priv->auto_exposure = true;
	} else {
		priv->auto_exposure = false;
	}

	ser_node = of_parse_phandle(node, "nvidia,gmsl-ser-device", 0);

	if (ser_node == NULL) {
		dev_err(dev, "[%s] : Missing %s handle\n", __func__, "nvidia,gmsl-ser-device");
		goto error;
	}

	err = of_property_read_u32(ser_node, "reg", &priv->g_ctx.ser_reg);

	if (err < 0) {
		dev_err(dev, "[%s] : Serializer reg not found\n", __func__);
		goto error;
	}

	ser_i2c = of_find_i2c_device_by_node(ser_node);

	of_node_put(ser_node);

	if (ser_i2c == NULL) {
		dev_err(dev, "[%s] : Missing Serializer Dev Handle\n", __func__);
		goto error;
	}
	if (ser_i2c->dev.driver == NULL) {
		dev_err(dev, "[%s] : Missing serializer driver\n", __func__);
		goto error;
	}

	priv->ser_dev = &ser_i2c->dev;

	dser_node = of_parse_phandle(node, "nvidia,gmsl-dser-device", 0);

	if (dser_node == NULL) {
		dev_err(dev, "[%s] : Missing %s handle\n", __func__, "nvidia,gmsl-dser-device");
		goto error;
	}

	dser_i2c = of_find_i2c_device_by_node(dser_node);

	of_node_put(dser_node);

	if (dser_i2c == NULL) {
		dev_err(dev, "[%s] : Missing deserializer dev handle\n", __func__);
		goto error;
	}

	if (dser_i2c->dev.driver == NULL) {
		dev_err(dev, "[%s] : Missing deserializer driver\n", __func__);
		goto error;
	}

	priv->dser_dev = &dser_i2c->dev;

	/* populate g_ctx from DT */

	gmsl = of_get_child_by_name(node, "gmsl-link");

	if (gmsl == NULL) {
		dev_err(dev, "[%s] : Missing GMSL-Link device node\n", __func__);
		err = -EINVAL;
		goto error;
	}

	err = of_property_read_string(gmsl, "dst-csi-port", &str_value);

	if (err < 0) {
		dev_err(dev, "[%s] : No dst-csi-port found\n", __func__);
		goto error;
	}

	priv->g_ctx.dst_csi_port = 	(!strcmp(str_value, "a")) ? GMSL_CSI_PORT_A : GMSL_CSI_PORT_B;

	err = of_property_read_string(gmsl, "src-csi-port", &str_value);

	if (err < 0) {
		dev_err(dev, "[%s] : No src-csi-port found\n", __func__);
		goto error;
	}

	priv->g_ctx.src_csi_port = 	(!strcmp(str_value, "a")) ? GMSL_CSI_PORT_A : GMSL_CSI_PORT_B;

	err = of_property_read_string(gmsl, "csi-mode", &str_value);

	if (err < 0) {
		dev_err(dev, "[%s] : No csi-mode found\n", __func__);
		goto error;
	}

	if (!strcmp(str_value, "1x4")) {
		priv->g_ctx.csi_mode = GMSL_CSI_1X4_MODE;
	} else if (!strcmp(str_value, "2x4")) {
		priv->g_ctx.csi_mode = GMSL_CSI_2X4_MODE;
	} else if (!strcmp(str_value, "4x2")) {
		priv->g_ctx.csi_mode = GMSL_CSI_4X2_MODE;
	} else if (!strcmp(str_value, "2x2")) {
		priv->g_ctx.csi_mode = GMSL_CSI_2X2_MODE;
	} else {
		dev_err(dev, "[%s] :Invalid csi-mode\n", __func__);
		goto error;
	}

	err = of_property_read_string(gmsl, "serdes-csi-link", &str_value);

	if (err < 0) {
		dev_err(dev, "[%s] : No serdes-csi-link found\n", __func__);
		goto error;
	}

	priv->g_ctx.serdes_csi_link = (!strcmp(str_value, "a")) ?
									GMSL_SERDES_CSI_LINK_A : GMSL_SERDES_CSI_LINK_B;

	err = of_property_read_u32(gmsl, "st-vc", &value);

	if (err < 0) {
		dev_err(dev, "[%s] : No st-vc info\n", __func__);
		goto error;
	}

	priv->g_ctx.st_vc = value;

	err = of_property_read_u32(gmsl, "vc-id", &value);

	if (err < 0) {
		dev_err(dev, "[%s] : No vc-id info\n", __func__);
		goto error;
	}

	priv->g_ctx.dst_vc = value;

	err = of_property_read_u32(gmsl, "num-lanes", &value);

	if (err < 0) {
		dev_err(dev, "[%s] : No num-lanes info\n", __func__);
		goto error;
	}

	priv->g_ctx.num_csi_lanes = value;

	priv->g_ctx.num_streams = of_property_count_strings(gmsl, "streams");

	if (priv->g_ctx.num_streams <= 0) {
		dev_err(dev, "[%s] : No streams found\n", __func__);
		err = -EINVAL;
		goto error;
	}

	for (i = 0; i < priv->g_ctx.num_streams; i++) {

		of_property_read_string_index(gmsl, "streams", i, &str_value1[i]);

		if (!str_value1[i]) {
			dev_err(dev, "[%s] : Invalid Stream Info\n", __func__);
			goto error;
		}

		if (!strcmp(str_value1[i], "raw12")) {
			priv->g_ctx.streams[i].st_data_type = GMSL_CSI_DT_RAW_12;
		} else if (!strcmp(str_value1[i], "yuv8")) {
			priv->g_ctx.streams[i].st_data_type = GMSL_CSI_DT_YUV_8;
		} else if (!strcmp(str_value1[i], "embed")) {
			priv->g_ctx.streams[i].st_data_type = GMSL_CSI_DT_EMBED;
		} else if (!strcmp(str_value1[i], "ued-u1")) {
			priv->g_ctx.streams[i].st_data_type = GMSL_CSI_DT_UED_U1;
		} else {
			dev_err(dev, "[%s] : Invalid stream data type\n", __func__);
			goto error;
		}
	}


	priv->g_ctx.s_dev = dev;

	return NO_ERROR;

error:
	dev_err(dev, "[%s] : Board Setup failed\n", __func__);
	return err;
}

/* --------------------------------------------------------------------------- */

static int tier4_isx021_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct device 			*dev 	= &client->dev;
	struct device_node 		*node 	= dev->of_node;
	struct tegracam_device 	*tc_dev;
	struct tier4_isx021 	*priv;
	int err;


	dev_info(dev, "[%s] : Probing V4L2 Sensor.\n", __func__);

	if (!IS_ENABLED(CONFIG_OF) || !node) {
		return -EINVAL;
	}

	priv = devm_kzalloc(dev, sizeof(struct tier4_isx021), GFP_KERNEL);

	if (!priv) {
		dev_err(dev, "[%s] : Unable to allocate Memory!\n", __func__);
		return -ENOMEM;
	}

	tc_dev = devm_kzalloc(dev, sizeof(struct tegracam_device), GFP_KERNEL);

	if (!tc_dev) {
		return -ENOMEM;
	}

	priv->i2c_client = tc_dev->client = client;

	tc_dev->dev = dev;

	strncpy(tc_dev->name, "isx021", sizeof(tc_dev->name));

	tc_dev->dev_regmap_config 	= &tier4_sensor_regmap_config;
	tc_dev->sensor_ops 			= &tier4_isx021_common_ops;
	tc_dev->v4l2sd_internal_ops = &tier4_isx021_subdev_internal_ops;
	tc_dev->tcctrl_ops 			= &tier4_isx021_ctrl_ops;

	err = tegracam_device_register(tc_dev);

	if (err) {
		dev_err(dev, "[%s] : Tegra Camera Driver Registration failed\n", __func__);
		return err;
	}

	priv->tc_dev = tc_dev;
	priv->s_data = tc_dev->s_data;
	priv->subdev = &tc_dev->s_data->subdev;

	tegracam_set_privdata(tc_dev, (void *)priv);

	err = tier4_isx021_board_setup(priv);

	if (err) {
		dev_err(dev, "[%s] : Board Setup failed\n", __func__);
		return err;
	}

	/* Pair sensor to serializer dev */

	err = tier4_max9295_sdev_pair(priv->ser_dev, &priv->g_ctx);

	if (err) {
		dev_err(&client->dev, "[%s] : GMSL Ser Pairing failed\n", __func__);
		return err;
	}

	/* Register sensor to deserializer dev */

	err = tier4_max9296_sdev_register(priv->dser_dev, &priv->g_ctx);

	if (err) {
		dev_err(&client->dev, "[%s] : GMSL Deserializer Register failed\n", __func__);
		return err;
	}

	/*
	 * gmsl serdes setup
	 *
	 * Sensor power on/off should be the right place for serdes
	 * setup/reset. But the problem is, the total required delay
	 * in serdes setup/reset exceeds the frame wait timeout, looks to
	 * be related to multiple channel open and close sequence
	 * issue (#BUG 200477330).
	 * Once this bug is fixed, these may be moved to power on/off.
	 * The delays in serdes is as per guidelines and can't be reduced,
	 * so it is placed in probe/remove, though for that, deserializer
	 * would be powered on always post boot, until 1.2v is supplied
	 * to deserializer from CVB.
	 */

	err = tier4_isx021_gmsl_serdes_setup(priv);

	if (err) {
		dev_err(&client->dev,"[%s] : GMSL Serdes setup failed\n", __func__);
		return err;
	}

	err = tegracam_v4l2subdev_register(tc_dev, true);

	if (err) {
		dev_err(dev, "[%s] : Tegra Camera Subdev Registration failed\n", __func__);
		return err;
	}

	dev_info(&client->dev, "Detected ISX021 sensor\n");

	err = tier4_isx021_ctrl_set_response_mode(priv->s_data);
	if(err) {
		dev_err(dev, "[%s]: Failed set i2c response mode\n", __func__);
	}
	
  return NO_ERROR;
}

/* --------------------------------------------------------------------------- */

static int tier4_isx021_remove(struct i2c_client *client)
{
	struct camera_common_data 	*s_data = to_camera_common_data(&client->dev);
	struct tier4_isx021 		*priv 	= (struct tier4_isx021 *)s_data->priv;

	tier4_isx021_gmsl_serdes_reset(priv);

	tegracam_v4l2subdev_unregister(priv->tc_dev);

	tegracam_device_unregister(priv->tc_dev);

	return NO_ERROR;
}


static const struct i2c_device_id tier4_isx021_id[] = {
	{ "tier4_isx021", 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, tier4_isx021_id);

static struct i2c_driver tier4_isx021_i2c_driver = {
	.driver = {
		.name 			= "tier4_isx021",
		.owner 			= THIS_MODULE,
		.of_match_table = of_match_ptr(tier4_isx021_of_match),
	},
	.probe 		= tier4_isx021_probe,
	.remove 	= tier4_isx021_remove,
	.id_table 	= tier4_isx021_id,
};

/* --------------------------------------------------------------------------- */

static int __init tier4_isx021_init(void)
{
	mutex_init(&serdes_lock__);

	printk("ISX021 Sensor Driver for ROScube: %s\n", BUILD_STAMP);

	return i2c_add_driver(&tier4_isx021_i2c_driver);
}

/* --------------------------------------------------------------------------- */

static void __exit tier4_isx021_exit(void)
{
	mutex_destroy(&serdes_lock__);

	i2c_del_driver(&tier4_isx021_i2c_driver);
}

module_init(tier4_isx021_init);
module_exit(tier4_isx021_exit);

MODULE_DESCRIPTION("TIERIV Automotive HDR Camera driver");
MODULE_AUTHOR("K.Iwasaki");
MODULE_AUTHOR("Y.Fujii");
MODULE_LICENSE("GPL v2");
