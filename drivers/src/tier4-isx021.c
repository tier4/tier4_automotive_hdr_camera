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

#include <linux/version.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/fs.h>

#include <linux/device.h>

#include <linux/seq_file.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,65)
#include <linux/kernel_read_file.h>
#endif

#include "tier4-max9295.h"
#include "tier4-max9296.h"
#include "tier4-gmsl-link.h"

#include "tier4-hw-model.h"
#include "tier4-fpga.h"

#include <media/tegracam_core.h>

#define USE_FIRMWARE

MODULE_SOFTDEP("pre: tier4_max9296");
MODULE_SOFTDEP("pre: tier4_max9295");
MODULE_SOFTDEP("pre: tier4_fpga");

// Register Address

#define	ISX021_SG_MODE_INTERNAL_SYNC				0

#define	ISX021_SG_MODE_INTERNAL_SYNC				0

#define ISX021_MIN_GAIN         					0
#define ISX021_MAX_GAIN         					48

#define ISX021_MIN_EXPOSURE_TIME   					0
#define ISX021_MID_EXPOSURE_TIME   					11010	// 11 milisecond
#define ISX021_MAX_EXPOSURE_TIME   					33000	// 33 milisecond

#define ISX021_GAIN_DEFAULT_VALUE					6

#define ISX021_DEFAULT_FRAME_LENGTH    				(1125)

// ---   Start of Register definition   ------------------

#define TIER4_ISX021_REG_0_ADDR    0
#define TIER4_ISX021_REG_1_ADDR    1
#define TIER4_ISX021_REG_2_ADDR    2
#define TIER4_ISX021_REG_3_ADDR    3
#define TIER4_ISX021_REG_4_ADDR    4
#define TIER4_ISX021_REG_5_ADDR    5
#define TIER4_ISX021_REG_6_ADDR    6
#define TIER4_ISX021_REG_7_ADDR    7
#define TIER4_ISX021_REG_8_ADDR    8
#define TIER4_ISX021_REG_9_ADDR    9
#define TIER4_ISX021_REG_10_ADDR    10
#define TIER4_ISX021_REG_11_ADDR    11
#define TIER4_ISX021_REG_12_ADDR    12
#define TIER4_ISX021_REG_13_ADDR    13

#define TIER4_ISX021_REG_14_ADDR    14

//   AE  mode

#define TIER4_ISX021_REG_15_ADDR    15
#define TIER4_ISX021_REG_16_ADDR    16
#define TIER4_ISX021_REG_17_ADDR    17
#define TIER4_ISX021_REG_18_ADDR    18

#define TIER4_ISX021_REG_19_ADDR    19
#define TIER4_ISX021_REG_20_ADDR    20
#define TIER4_ISX021_REG_21_ADDR    21
#define TIER4_ISX021_REG_22_ADDR    22

//  ME  mode

#define TIER4_ISX021_REG_23_ADDR    23
#define TIER4_ISX021_REG_24_ADDR    24
#define TIER4_ISX021_REG_25_ADDR    25
#define TIER4_ISX021_REG_26_ADDR    26

#define TIER4_ISX021_REG_27_ADDR    27
#define TIER4_ISX021_REG_28_ADDR    28
#define TIER4_ISX021_REG_29_ADDR    29
#define TIER4_ISX021_REG_30_ADDR    30

//  Analog Gain

#define TIER4_ISX021_REG_31_ADDR    31
#define TIER4_ISX021_REG_32_ADDR    32

#define TIER4_ISX021_REG_33_ADDR    33
#define TIER4_ISX021_REG_34_ADDR    34

#define TIER4_ISX021_REG_35_ADDR    35
#define TIER4_ISX021_REG_36_ADDR    36

#define TIER4_ISX021_REG_37_ADDR    37
#define TIER4_ISX021_REG_38_ADDR    38

#define TIER4_ISX021_REG_39_ADDR    39
#define TIER4_ISX021_REG_40_ADDR    40

#define TIER4_ISX021_REG_41_ADDR    41
#define TIER4_ISX021_REG_42_ADDR    42

#define TIER4_ISX021_REG_43_ADDR    43
#define TIER4_ISX021_REG_44_ADDR    44

#define TIER4_ISX021_REG_45_ADDR    45
#define TIER4_ISX021_REG_46_ADDR    46

// Exposure

#define TIER4_ISX021_REG_47_ADDR    47
#define TIER4_ISX021_REG_48_ADDR    48
#define TIER4_ISX021_REG_49_ADDR    49

#define TIER4_ISX021_REG_50_ADDR    50
#define TIER4_ISX021_REG_51_ADDR    51
#define TIER4_ISX021_REG_52_ADDR    52
#define TIER4_ISX021_REG_53_ADDR    53

#define TIER4_ISX021_REG_54_ADDR    54
#define TIER4_ISX021_REG_55_ADDR    55
#define TIER4_ISX021_REG_56_ADDR    56
#define TIER4_ISX021_REG_57_ADDR    57

#define TIER4_ISX021_REG_58_ADDR    58
#define TIER4_ISX021_REG_59_ADDR    59
#define TIER4_ISX021_REG_60_ADDR    60
#define TIER4_ISX021_REG_61_ADDR    61


#define TIER4_ISX021_REG_62_ADDR    62

#define TIER4_ISX021_REG_63_ADDR    63
#define TIER4_ISX021_REG_64_ADDR    64
#define TIER4_ISX021_REG_65_ADDR    65

#define TIER4_ISX021_REG_66_ADDR    66

//  AE mode

#define TIER4_ISX021_REG_67_ADDR    67

#define TIER4_ISX021_REG_68_ADDR    68
#define TIER4_ISX021_REG_69_ADDR    69

#define TIER4_ISX021_REG_70_ADDR    70

//  Digital Gain

#define TIER4_ISX021_REG_71_ADDR    71
#define TIER4_ISX021_REG_72_ADDR    72

// Distortion Corretion

#define TIER4_ISX021_REG_73_ADDR    73
#define TIER4_ISX021_REG_74_ADDR    74
#define TIER4_ISX021_REG_75_ADDR    75
#define TIER4_ISX021_REG_76_ADDR    76

#define TIER4_ISX021_REG_77_ADDR    77

#define TIER4_ISX021_REG_78_ADDR    78

#define TIER4_ISX021_REG_79_ADDR    79

// --- End of  Register definition ------------------------


#define	MAX_NUM_OF_REG								(100)

#define ISX021_AUTO_EXPOSURE_MODE					0x00
#define ISX021_AE_TIME_UNIT_MICRO_SECOND			0x03

#define BIT_SHIFT_8									8
#define BIT_SHIFT_16								16
#define MASK_1_BIT									0x1
#define MASK_4_BIT									0xF
#define MASK_8_BIT									0xFF

#define NO_ERROR									0

#define TIME_120_MILISEC							120000
#define TIME_121_MILISEC							121000

#define MAX_NUM_CAMERA								8

#define	FIRMWARE_BIN_FILE							"/lib/firmware/tier4-isx021.bin"

#define	FSYNC_FREQ_HZ								10

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
	u16								*firmware_buffer;
	int								es_number;
	struct 	device					*fpga_dev;
};

static const struct regmap_config tier4_sensor_regmap_config = {
	.reg_bits = 16,
	.val_bits = 8,
	.cache_type = REGCACHE_RBTREE,
};

struct st_priv {
	struct 	i2c_client 	*p_client;
	struct 	tier4_isx021 *p_priv;
	bool	sensor_ser_shutdown;
	bool	des_shutdown;
};

static struct st_priv wst_priv[MAX_NUM_CAMERA];

static int camera_channel_count = 0;

static int trigger_mode = 0xCAFE;
static int enable_auto_exposure = 0xCAFE;
static int enable_distortion_correction = 0xCAFE;
static int shutter_time_min = ISX021_MIN_EXPOSURE_TIME;
static int shutter_time_mid = ISX021_MID_EXPOSURE_TIME;
static int shutter_time_max = ISX021_MAX_EXPOSURE_TIME;

module_param(trigger_mode, int, S_IRUGO);
module_param(enable_auto_exposure, int, S_IRUGO | S_IWUSR);
module_param(enable_distortion_correction, int, S_IRUGO | S_IWUSR);
module_param(shutter_time_min, int, S_IRUGO | S_IWUSR);
module_param(shutter_time_mid, int, S_IRUGO | S_IWUSR);
module_param(shutter_time_max, int, S_IRUGO | S_IWUSR);

static struct mutex tier4_sensor_lock__;

void tier4_isx021_sensor_mutex_lock(void)
{
	mutex_lock(&tier4_sensor_lock__);
}
EXPORT_SYMBOL(tier4_isx021_sensor_mutex_lock);

void tier4_isx021_sensor_mutex_unlock(void)
{
	mutex_unlock(&tier4_sensor_lock__);
}
EXPORT_SYMBOL(tier4_isx021_sensor_mutex_unlock);

// -------------------------------

static char upper(char c){
    if('a' <= c && c <= 'z'){
        c = c - ('a' - 'A');
    }
    return c;
}

static void to_upper_string(char *out, const char *in){
    int i;

    i = 0;
    while(in[i] != '\0'){
        out[i] = upper(in[i]);
        i++;
    }
}

static inline int tier4_isx021_read_reg(struct camera_common_data *s_data, u16 addr, u8 *val)
{
	int err 	= 0;
	u32 reg_val = 0;
	struct tier4_isx021 *priv = (struct tier4_isx021 *)s_data->priv;
  u16 reg_addr = 0;

#ifdef USE_FIRMWARE
  reg_addr = priv->firmware_buffer[addr];
#else
  reg_addr = addr;
#endif

  err = regmap_read(s_data->regmap, reg_addr, &reg_val);

	*val = reg_val & 0xFF;

	if (err) {
		dev_err(s_data->dev, "[%s ] : ISX021 I2C Read failed. Address = 0x%04X\n", __func__, reg_addr);
	}

	return err;
}

static int tier4_isx021_write_reg(struct camera_common_data *s_data, u16 addr, u8 val)
{

	int 				err 	= 0;
	struct tier4_isx021 *priv = (struct tier4_isx021 *)s_data->priv;
  u16 reg_addr = 0;

#ifdef USE_FIRMWARE
  reg_addr = priv->firmware_buffer[addr];
#else
  reg_addr = addr;
#endif

  err = regmap_write(s_data->regmap, reg_addr, val);

	if (err) {
		dev_err(s_data->dev,  "[%s] : I2C write failed. Reg Address = 0x%04X  Data = 0x%02X\n", __func__, reg_addr, val);
	}

	return err;
}

static int tier4_isx021_write_reg_with_verify(struct camera_common_data *s_data, u16 addr, u8 val8)
{

	u32					r_val32;
	u8					r_val8;
	int 				err 	= 0;
	struct tier4_isx021 *priv = (struct tier4_isx021 *)s_data->priv;
  u16 reg_addr = 0;

#ifdef USE_FIRMWARE
  reg_addr = priv->firmware_buffer[addr];
#else
  reg_addr = addr;
#endif

	err = regmap_write(s_data->regmap, reg_addr, val8);

	usleep_range(10000,11000);

	err = regmap_read(s_data->regmap, reg_addr, &r_val32);

	r_val8 = r_val32 & 0xFF;

	if (err) {
		dev_err(s_data->dev,  "[%s] : Failed at I2C Read Reg. Reg Address[0x%04X]  Read Data[0x%02X]\n", __func__, reg_addr, r_val8);
	} else {
		if ( val8 != r_val8 ) {
			dev_err(s_data->dev,  "[%s] : Failed at I2C Reg Read and Verify. Reg Address[0x%04X], Expected Data[0x%02X], Actual Read Data[0x%02X]\n", __func__, reg_addr, val8, r_val8);
			err = -EINVAL;
		}
	}

	return err;
}

static int tier4_isx021_check_register_mode_set_f_lock(	struct camera_common_data *s_data)
{
  	int err = 0;
  	int iter = 0;
  	u8  val8, save_reg;
    struct device 	*dev	= s_data->dev;

 //   usleep_range(100000,110000);

    err = tier4_isx021_read_reg(s_data, TIER4_ISX021_REG_73_ADDR, &save_reg);
    if ( err ) {
      dev_err(dev, "[%s] : Failed to save Remap mode.\n", __func__);
      return err;
    }

    err = tier4_isx021_write_reg_with_verify(s_data, TIER4_ISX021_REG_73_ADDR, 0x02);

  	while(1) {

    	if ( iter > 10 ) {
        	return -1;
    	}

    	err = tier4_isx021_read_reg(s_data, TIER4_ISX021_REG_77_ADDR, &val8);

    	if ( !err ) {
      		if ( val8 == 0x53 ) {
        		break;
      		} else {
        		err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_77_ADDR, 0x53);
      		}
      		usleep_range(3000,3300);
    	}

  }

  err = tier4_isx021_write_reg_with_verify(s_data, TIER4_ISX021_REG_73_ADDR, save_reg);

  return err;
}

static int tier4_isx021_set_fsync_trigger_mode(struct tier4_isx021 *priv)
{
	int err 	= 0;

	struct camera_common_data 	*s_data = priv->s_data;
	struct device 				*dev	= s_data->dev;
	int							des_num = 0;

	if ( priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE ) {

		dev_info(dev, "[%s] : generate-fsync =%d\n", __func__, priv->g_ctx.fpga_generate_fsync );

		if ( priv->g_ctx.fpga_generate_fsync == false ) {

			err = tier4_fpga_disable_generate_fsync_signal(priv->fpga_dev);
			if ( err ) {
				dev_err(dev, "[%s] : Disabling FPGA generate fsync failed.\n", __func__);
				return err;
			}
		} else {

			err = tier4_fpga_enable_generate_fsync_signal(priv->fpga_dev);
			if ( err ) {
				dev_err(dev, "[%s] : Enabling FPGA generate fsync failed.\n", __func__);
				return err;
			}

			des_num = priv->g_ctx.reg_mux;

			err = tier4_fpga_set_fsync_signal_frequency(priv->fpga_dev, des_num, FSYNC_FREQ_HZ );
			if ( err ) {
				dev_err(dev, "[%s] : Setting the frequency of fsync genrated by FPGA failed.\n", __func__);
				return err;
			}
		}
	}

	err = tier4_max9296_setup_gpi(priv->dser_dev);

	if ( err ) {
		dev_err(dev, "[%s] : Failed in tier4_max9296_setup_gpi.\n", __func__);
		return err;
	}

	err = tier4_max9295_setup_gpo(priv->ser_dev);

	if ( err ) {
		dev_err(dev, "[%s] : Failed in tier4_max9295_setup_gpo.\n", __func__);
		return err;
	}

	// transit to Startup state

  err = tier4_isx021_check_register_mode_set_f_lock(s_data);
  if ( err ) {
    dev_err(dev, "[%s] : Time out Error occurred in tier4_isx021_check_register_mode_set_f_lock.\n", __func__);
    return err;
  }

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_62_ADDR, 0x00);

	if ( err ) {
		dev_err(dev, "[%s] : Failed tier4_isx021_write_reg.\n", __func__);
		return err;
	}

	usleep_range(TIME_120_MILISEC, TIME_121_MILISEC);

	// set SG_MODE_CTL to 0 for transition to FSYNC mode

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_63_ADDR, 0x02);		// set FSYNC mode

	if ( err ) {
			dev_err(dev, "[%s] : Failed tier4_isx021_write_reg.\n", __func__);
		return err;
	}

	usleep_range(TIME_120_MILISEC, TIME_121_MILISEC);

	// set SG_MODE_APL to 0 for transition to FSYNC mode

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_78_ADDR, 0x02);

	if ( err ) {
		dev_err(dev, "[%s] : Failed tier4_isx021_write_reg.\n", __func__);
		return err;
	}

	usleep_range(TIME_120_MILISEC, TIME_121_MILISEC);

	// Exit Startup state

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_62_ADDR, 0x80);

	if ( err ) {
			dev_err(dev, "[%s] : Failed tier4_isx021_write_reg.\n", __func__);
		return err;
	}

	usleep_range(TIME_120_MILISEC, TIME_121_MILISEC);

	return err;
}

/*--------------------------------------------------------------------------*/
static int tier4_isx021_set_response_mode(struct tier4_isx021 *priv)
{
	int err = 0;
	struct camera_common_data *s_data = priv->s_data;
	u8	r_val;

	usleep_range(100000, 110000);

  err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_62_ADDR, 0x00);
	if (err) {
		goto error_exit;
	}

	usleep_range(100000, 110000); 	// For ES3

	err = tier4_isx021_read_reg(s_data,TIER4_ISX021_REG_66_ADDR, &r_val);

	if ( r_val == 0x04 ) {
		priv->es_number = 2;
 	} else {
		priv->es_number = 3;
 	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_66_ADDR, 0x06);

	usleep_range(100000, 110000);	// For ES3
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_62_ADDR, 0x80);

error_exit:

  return err;
}


static int tier4_isx021_gmsl_serdes_setup(struct tier4_isx021 *priv)
{
	int 			err 	= 0;
	int 			des_err = 0;
	struct device 	*dev;

	if (!priv || !priv->ser_dev || !priv->dser_dev || !priv->i2c_client) {
		return -EINVAL;
	}

	dev = &priv->i2c_client->dev;


	/* For now no separate power on required for serializer device */

	tier4_max9296_power_on(priv->dser_dev);

	/* setup serdes addressing and control pipeline */


	err = tier4_max9296_setup_link(priv->dser_dev, &priv->i2c_client->dev);

	if (err) {
		dev_err(dev, "[%s] : Failed to configure GMSL deserializer link.\n", __func__);
		goto error;
	}


	err = tier4_max9295_setup_control(priv->ser_dev);

	/* proceed even if ser setup failed, to setup deser correctly */
	if (err) {
		dev_err(dev, "[%s] : Failed to setup GMSL serializer.\n", __func__);
		goto error;
	}

	des_err = tier4_max9296_setup_control(priv->dser_dev, &priv->i2c_client->dev);

	if (des_err) {
		dev_err(dev, "[%s] : Failed setup GMSL deserializer.\n", __func__);
		err = des_err;
	}
error:

	return err;
}

/*--------------------------------------------------------------------------*/

static void tier4_isx021_gmsl_serdes_reset(struct tier4_isx021 *priv)
{

	/* reset serdes addressing and control pipeline */
	tier4_max9295_reset_control(priv->ser_dev);

	tier4_max9296_reset_control(priv->dser_dev, &priv->i2c_client->dev, false );

	tier4_max9296_power_off(priv->dser_dev);

}

/*--------------------------------------------------------------------------*/

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
			dev_err(dev, "[%s] :  Failed power on.\n", __func__);
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

/*--------------------------------------------------------------------------*/

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
			dev_err(dev, "[%s] : Failed power off.\n", __func__);
		}
		return err;
	}

power_off_done:
	pw->state = SWITCH_OFF;

	return err;
}

/*--------------------------------------------------------------------------*/

static int tier4_isx021_power_get(struct tegracam_device *tc_dev)
{
	struct 	camera_common_power_rail 	*pw 	= tc_dev->s_data->power;
	int 								err		= 0;

	pw->state = SWITCH_OFF;

	return err;
}

/*--------------------------------------------------------------------------*/

static int tier4_isx021_power_put(struct tegracam_device *tc_dev)
{
	if (unlikely(!tc_dev->s_data->power)) {
		return -EFAULT;
	}

	return NO_ERROR;
}

/*--------------------------------------------------------------------------*/

static int tier4_isx021_set_group_hold(struct tegracam_device *tc_dev, bool val)
{
	volatile int err = 0;

	return err;
}

/*--------------------------------------------------------------------------*/

static int tier4_copy_reg_value(struct camera_common_data *s_data, u16 ae_addr, u16 me_addr)
{
	int err 	= 0;
	u8	val8 = 0;

	err = tier4_isx021_read_reg(s_data, ae_addr, &val8);

	if (err) {
		dev_err(s_data->dev, "[%s] : Failed tier4_isx021_read_reg, address = 0x%x\n", __func__, ae_addr);
	}

	err = tier4_isx021_write_reg(s_data, me_addr, val8);

	if (err) {
		dev_err(s_data->dev, "[%s] : Failed tier4_isx021_write_reg, address = 0x%x, val = 0x%x\n", __func__, me_addr, val8);
	}

	return err;
}

/*--------------------------------------------------------------------------*/

static int tier4_isx021_set_gain(struct tegracam_device *tc_dev, s64 val)
{

	int 						err = 0;

	s64							gain;
	u8							digital_gain_low_byte;
	u8							digital_gain_high_byte;

	struct camera_common_data 	*s_data 	= tc_dev->s_data;
	struct device 				*dev 		= tc_dev->dev;

	if ( val > ISX021_MAX_GAIN ) {
		val = ISX021_MAX_GAIN;
	} else if ( val < ISX021_MIN_GAIN ) {
		val = ISX021_MIN_GAIN;
	}

	dev_info(dev,"[%s] :Gain is set to %lld\n", __func__, val);

	gain = 10*val;

	digital_gain_low_byte = gain & 0xFF;

	digital_gain_high_byte = ( gain >> 8  ) & 0xFF;

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_68_ADDR, 0x01);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_69_ADDR, 0x01);

	if (err) {
		goto fail;
	}

	// Shutter 0 value

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_15_ADDR, TIER4_ISX021_REG_23_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_16_ADDR, TIER4_ISX021_REG_24_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_17_ADDR, TIER4_ISX021_REG_25_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_18_ADDR, TIER4_ISX021_REG_26_ADDR);

	if (err) {
		goto fail;
	}

	// Shutter 1 value

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_19_ADDR, TIER4_ISX021_REG_27_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_20_ADDR, TIER4_ISX021_REG_28_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_21_ADDR, TIER4_ISX021_REG_29_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_22_ADDR, TIER4_ISX021_REG_30_ADDR);

	if (err) {
		goto fail;
	}

	// Set Analog Gain for shutter 0

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_31_ADDR, digital_gain_low_byte);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_32_ADDR, digital_gain_high_byte);

	if (err) {
		goto fail;
	}

	// copy  AE Analog gain value to ME analog gain registers for shutter 1 to 3

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_35_ADDR, TIER4_ISX021_REG_37_ADDR);

	if (err) {
		goto fail;
	}


	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_36_ADDR, TIER4_ISX021_REG_38_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_39_ADDR, TIER4_ISX021_REG_41_ADDR);

	if (err) {
		goto fail;
	}


	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_40_ADDR, TIER4_ISX021_REG_42_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_43_ADDR, TIER4_ISX021_REG_45_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_44_ADDR, TIER4_ISX021_REG_46_ADDR);

	if (err) {
		goto fail;
	}

	msleep(100);		// For ES3

	// Change AE mode

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_67_ADDR, 0x03);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_71_ADDR, digital_gain_low_byte);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_72_ADDR, digital_gain_high_byte);

	if (err) {
		goto fail;
	}


	return NO_ERROR;

fail:
	dev_err(dev, "[%s] : GAIN control error\n", __func__);

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

	int 									err 		= 0;
	struct camera_common_data 				*s_data 	= tc_dev->s_data;

	// Change to Auto exposure mode

  msleep(100);		// For ES3

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_67_ADDR, ISX021_AUTO_EXPOSURE_MODE);


	// Set min exposure time unit
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_47_ADDR, ISX021_AE_TIME_UNIT_MICRO_SECOND );
	if (err) {
		goto fail;
	}

	// Set mid exposure time unit
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_48_ADDR, ISX021_AE_TIME_UNIT_MICRO_SECOND );
	if (err) {
		goto fail;
	}

	// Set max exposure time unit
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_49_ADDR, ISX021_AE_TIME_UNIT_MICRO_SECOND );
	if (err) {
		goto fail;
	}

	// Set min exposure time
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_50_ADDR, shutter_time_min & 0xFF);
	if (err) {
		goto fail;
	}
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_51_ADDR, (shutter_time_min >> 8) & 0xFF);
	if (err) {
		goto fail;
	}

	// Set mid exposure time
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_54_ADDR, shutter_time_mid & 0xFF);
	if (err) {
		goto fail;
	}
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_55_ADDR, (shutter_time_mid >> 8) & 0xFF);
	if (err) {
		goto fail;
	}

	// Set max exposure time
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_58_ADDR, shutter_time_max & 0xFF);
	if (err) {
		goto fail;
	}
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_59_ADDR, (shutter_time_max >> 8) & 0xFF);
	if (err) {
		goto fail;
	}

fail:

  return err;
}

/* ------------------------------------------------------------------------- */

static int tier4_isx021_set_exposure(struct tegracam_device *tc_dev, s64 val)
{

	int 									err 		= 0;

	u8										exp_time_byte0;
	u8										exp_time_byte1;
	u8										exp_time_byte2;
	u8										exp_time_byte3;
	struct tier4_isx021 					*priv 		= (struct tier4_isx021 *)tegracam_get_privdata(tc_dev);
	struct camera_common_data 				*s_data 	= tc_dev->s_data;

	//  unit for val is micro-second

	if ( val > ISX021_MAX_EXPOSURE_TIME ) {
		val = ISX021_MAX_EXPOSURE_TIME;
	} else if ( val < ISX021_MIN_EXPOSURE_TIME ) {
		val = ISX021_MIN_EXPOSURE_TIME;
	}

	exp_time_byte0 = val & 0xFF;

	exp_time_byte1 = ( val >> 8 ) & 0xFF;

	exp_time_byte2 = ( val >> 16 ) & 0xFF;

	exp_time_byte3 = ( val >> 24 ) & 0xFF;

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_48_ADDR, 0x03);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_49_ADDR, 0x03 );

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_54_ADDR, exp_time_byte0 );

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_55_ADDR, exp_time_byte1 );

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_56_ADDR, exp_time_byte2 );

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_57_ADDR, exp_time_byte3 );

	if (err) {
		goto fail;
	}

	// Shutter2

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_58_ADDR, exp_time_byte0 );

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_59_ADDR, exp_time_byte1 );

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_60_ADDR, exp_time_byte2 );

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_61_ADDR, exp_time_byte3 );

	if (err) {
		goto fail;
	}

	return NO_ERROR;

fail:
	dev_dbg(&priv->i2c_client->dev,	"[%s] : Failed to set Exposure time.\n", __func__);

	return err;
}
// --------------------------------------------------------------------------------------
//  Enable Distortion Coreection
// --------------------------------------------------------------------------------------
static int tier4_isx021_enable_distortion_correction(struct tegracam_device *tc_dev, bool is_enabled)
{
  int err = 0;
  u8 r_val = 0;

    err = tier4_isx021_read_reg(tc_dev->s_data,TIER4_ISX021_REG_73_ADDR, &r_val);
    if(r_val != 0x02 || r_val != 0x04){
      err = tier4_isx021_write_reg(tc_dev->s_data, TIER4_ISX021_REG_73_ADDR, 0x02);
    }

    err = tier4_isx021_write_reg(tc_dev->s_data, TIER4_ISX021_REG_62_ADDR, 0x00);

  	usleep_range(35000,36000);

    if(is_enabled){

      err = tier4_isx021_write_reg(tc_dev->s_data, TIER4_ISX021_REG_74_ADDR, 0x01);
      if ( err ) {
        dev_err(tc_dev->dev,"[%s] : Failed to enable Distortion Correction.", __func__);
        return err;
      }

      usleep_range(35000,36000);

      err = tier4_isx021_write_reg(tc_dev->s_data, TIER4_ISX021_REG_75_ADDR, 0x01);
      if ( err ) {
        dev_err(tc_dev->dev,"[%s] : Failed to enable Distortion Correction.", __func__);
        return err;
      }

    } else{

      dev_info(tc_dev->dev,"[%s] : Disabled Distortion Correction.", __func__);

      err = tier4_isx021_write_reg(tc_dev->s_data, TIER4_ISX021_REG_74_ADDR, 0x00);
      if ( err ) {
        dev_err(tc_dev->dev,"[%s] : Failed to disable Distortion Correction.", __func__);
        return err;
      }

      usleep_range(35000,36000);

      err = tier4_isx021_write_reg(tc_dev->s_data, TIER4_ISX021_REG_75_ADDR, 0x00);
      if ( err ) {
        dev_err(tc_dev->dev,"[%s] : Failed to disable Distortion Correction.", __func__);
        return err;
      }

    }

    usleep_range(35000,36000);
    err = tier4_isx021_write_reg(tc_dev->s_data, TIER4_ISX021_REG_62_ADDR, 0x80);

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
//	.set_distortion_correction 	= tier4_isx021_set_distortion_correction,
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

/*   tier4_isx021_set_mode() can not be needed. But it remains for compatiblity */

static int tier4_isx021_set_mode(struct tegracam_device *tc_dev)
{

	volatile int err 	= 0;

	return err;
}

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
		dev_err(dev, "[%s] : Failed to setup Streaming.\n", __func__);
		goto exit;
	}

	err = tier4_max9295_control_sensor_power_seq(priv->ser_dev, SENSOR_ID_ISX021, true);
	if (err) {
		dev_err(dev, "[%s] : Failed to powerup Camera Sensor.\n", __func__);
		goto exit;
	}
	if ( enable_auto_exposure == 1 ) {
		priv->auto_exposure = true;
		dev_info(dev, "[%s] : Parameter[enable_auto_exposure] = 1.\n", __func__ );
	}

	if( priv->auto_exposure == true )
	{
		err = tier4_isx021_set_auto_exposure(tc_dev);
		if (err) {
			dev_err(dev, "[%s] : Failed to enable Auto Exposure .\n", __func__);
			goto exit;
		} else {
			dev_info(dev, "[%s] : Enabled Auto Exposure.\n", __func__ );
		}
	} else {
		dev_info(dev, "[%s] : Disabled Auto Exposure.\n", __func__ );
	}
	if (err) {
		dev_err(dev, "[%s] : Failed to make Digital Gain set to the default value.\n", __func__);
	}

  if ( trigger_mode == 1 ) {
		priv->fsync_mode = true;
		dev_info(dev, "[%s] : Enabled Slave(fsync triggered) mode.\n", __func__ );
	}

	dev_info(dev, "[%s] : fsync-mode in DTB = %d\n", __func__,  priv->fsync_mode );

	if ( priv->fsync_mode == true ) {
		err = tier4_isx021_set_fsync_trigger_mode(priv);
		if (err) {
			dev_err(dev, "[%s] : Failed to put Camera sensor into Slave(fsync triggered) mode.\n", __func__);
			goto exit;
		} else {
			dev_dbg(dev, "[%s] : Put Camera sensor into Slave(fsync triggered) Mode.\n", __func__);
		}
	} else {
		dev_dbg(dev, "[%s] :Put Camera sensor into Master(free running) Mode.\n", __func__);
	}
  msleep(20);
	err = tier4_max9296_start_streaming(priv->dser_dev, dev);
	if (err) {
		dev_err(dev, "[%s] : tier4_max9296_start_stream() failed\n", __func__);
		return err;
	}


  if ( enable_distortion_correction == 1 ) {
    priv->distortion_correction = true;
    dev_info(dev, "[%s] : Prameter[enable_distortion_correction] = 1 .\n", __func__ );
  }
  err = tier4_isx021_enable_distortion_correction(tc_dev, priv->distortion_correction);
  if (err) {
    dev_err(dev, "[%s] : Failed to enable Distortion Correction.\n", __func__);
    goto exit;
  }
	dev_info(dev, "[%s] : Started Camera streaming.\n", __func__);

	return NO_ERROR;

exit:

	dev_err(dev, "[%s] : Failed to start Camera streaming.\n", __func__);

	return err;
}

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

static int tier4_isx021_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	dev_dbg(&client->dev, "%s:\n", __func__);

	return NO_ERROR;
}

static const struct v4l2_subdev_internal_ops tier4_isx021_subdev_internal_ops = {
	.open = tier4_isx021_open,
};

static const char *of_stdout_options;

static int tier4_isx021_board_setup(struct tier4_isx021 *priv)
{
	struct tegracam_device 		*tc_dev 		= priv->tc_dev;
	struct device 				*dev 			= tc_dev->dev;
	struct device_node 			*node 			= dev->of_node;
	struct device_node 			*ser_node;
	struct device_node 			*root_node;
	struct i2c_client 			*ser_i2c 		= NULL;
	struct device_node 			*dser_node;
	struct i2c_client 			*dser_i2c 		= NULL;
	struct device_node 			*fpga_node		= NULL;
	struct i2c_client 			*fpga_i2c 		= NULL;
	struct device_node 			*gmsl;
	int 						value 			= 0xFFFF;
	const char 					*str_value;
	const char 					*str_value1[2];
	int  						i;
	int 						err;
	const char					*str_model;
	char						upper_str_model[64];
	char						*str_err;

	root_node = of_find_node_opts_by_path("/", &of_stdout_options);
	err = of_property_read_string(root_node, "model", &str_model);
	if (err < 0) {
		dev_err(dev, "[%s] : model not found\n", __func__);
		goto error;
	}

	memset(upper_str_model, 0, 64);
	to_upper_string(upper_str_model, str_model);
	str_err = strstr(upper_str_model, STR_DTB_MODEL_NAME_ORIN);
 	priv->g_ctx.hardware_model = HW_MODEL_UNKNOWN;

	if ( str_err  ) {
		 priv->g_ctx.hardware_model = HW_MODEL_NVIDIA_ORIN_DEVKIT;
	}

	str_err = strstr(upper_str_model, STR_DTB_MODEL_NAME_XAVIER);
	if ( str_err  ) {
		 priv->g_ctx.hardware_model = HW_MODEL_NVIDIA_XAVIER_DEVKIT;
	}

	str_err = strstr(upper_str_model, STR_DTB_MODEL_NAME_ROSCUBE);
	if ( str_err  ) {
		 priv->g_ctx.hardware_model = HW_MODEL_ADLINK_ROSCUBE;
	}

	dev_info(dev, "[%s] : model=%s\n", __func__, str_model);
	dev_dbg(dev, "[%s] : hardware_model=%d\n", __func__, priv->g_ctx.hardware_model);

	if ( priv->g_ctx.hardware_model == HW_MODEL_UNKNOWN ) {
		dev_err(dev, "[%s] : Unknown Hardware Sysytem !\n", __func__);
		goto error;
	}

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


  	if(enable_distortion_correction == 0xCAFE){
    	// if not set kernel param, read device tree param
    	err = of_property_read_string(node, "distortion-correction", &str_value);
    	if (err < 0) {
      		dev_err(dev, "[%s] : No distortion-correction found. set enable_distortion-correction = true\n", __func__);
    	}else{
      		if (!strcmp(str_value, "true")) {
        		enable_distortion_correction = 1;
      		} else {
        		enable_distortion_correction = 0;
      		}
    	}
  	}
  	priv->distortion_correction = enable_distortion_correction != 0? true:false;

  	if(enable_auto_exposure == 0xCAFE){
    	// if not set kernel param, read device tree param
    	err = of_property_read_string(node, "auto-exposure", &str_value);
    	if (err < 0) {
      		dev_err(dev, "[%s] : No auto-exposure mode found. set enable_auto_exposure = true\n", __func__);
    	}else{
      		if (!strcmp(str_value, "true")) {
        		enable_auto_exposure = 1;
      		} else {
        		enable_auto_exposure = 0;
      		}
    	}
  	}

	priv->auto_exposure = enable_auto_exposure != 0? true: false;

#if 0
	priv->g_ctx.fpga_generate_fsync = false;

	if ( priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE ) {
		err = of_property_read_string(node, "fpga-generate-fsync", &str_value);
		if ( err < 0) {
			if ( err == -EINVAL ) {
				dev_info(dev, "[%s] : fpga-generate-fsync does not exist.\n", __func__);
			} else {
				dev_err(dev, "[%s]  : fpga-generate-fsync  is invalid .\n", __func__);
				goto error;
			}
		} else {
			if (!strcmp(str_value, "true")) {
				priv->g_ctx.fpga_generate_fsync = true;
			}
		}
	}
#endif

	// for Ser node

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

	// for Dser node

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

	if ( priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE ) {

	// for FPGA node

		fpga_node = of_parse_phandle(node, "nvidia,fpga-device", 0);

		if (fpga_node == NULL) {
			dev_err(dev, "[%s] : Missing %s handle\n", __func__, "nvidia,fpga-device");
			goto error;
		}

		err = of_property_read_u32(fpga_node, "reg", &priv->g_ctx.sdev_fpga_reg);

		if (err < 0) {
			dev_err(dev, "[%s] : FPGA reg not found\n", __func__);
			goto error;
		}

		fpga_i2c = of_find_i2c_device_by_node(fpga_node);

		of_node_put(fpga_node);

		if (fpga_i2c == NULL) {
			dev_err(dev, "[%s] : Missing FPGA Dev Handle\n", __func__);
			goto error;
		}
		if (fpga_i2c->dev.driver == NULL) {
			dev_err(dev, "[%s] : Missing FPGA driver\n", __func__);
			goto error;
		}
		priv->fpga_dev = &fpga_i2c->dev;
	}

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

static int tier4_isx021_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int 	err = 0;
	char 	*path = FIRMWARE_BIN_FILE;
	void	*firmware_buffer;

	struct 	device 			*dev 	= &client->dev;
	struct 	device_node 	*node 	= dev->of_node;
	struct 	tegracam_device *tc_dev;
	struct 	tier4_isx021 	*priv;

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,65)
  loff_t 	size;
#endif

  tier4_isx021_sensor_mutex_lock();

  dev_info(dev, "[%s] : Probing V4L2 Sensor.\n", __func__);

	if (!IS_ENABLED(CONFIG_OF) || !node) {
		err = -EINVAL;
		goto errret;
	}

	priv = devm_kzalloc(dev, sizeof(struct tier4_isx021), GFP_KERNEL);

	if (!priv) {
		dev_err(dev, "[%s] : Unable to allocate Memory!\n", __func__);
		err = -ENOMEM;
		goto errret;
	}

	tc_dev = devm_kzalloc(dev, sizeof(struct tegracam_device), GFP_KERNEL);

	if (!tc_dev) {
		err = -ENOMEM;
		goto errret;
	}

	priv->i2c_client = tc_dev->client = client;

	tc_dev->dev = dev;

	strncpy(tc_dev->name, "isx021", sizeof(tc_dev->name));

	tc_dev->dev_regmap_config 	= &tier4_sensor_regmap_config;
	tc_dev->sensor_ops 			= &tier4_isx021_common_ops;
	tc_dev->v4l2sd_internal_ops = &tier4_isx021_subdev_internal_ops;
	tc_dev->tcctrl_ops 			= &tier4_isx021_ctrl_ops;

	firmware_buffer = devm_kzalloc(dev, sizeof(u16)*MAX_NUM_OF_REG, GFP_KERNEL);

	if (!firmware_buffer) {
		dev_err(dev, "[%s] : Failed to allocate firmware buffer\n", __func__);
		err = -ENOMEM;
		goto errret;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,65)
	err = kernel_read_file_from_path(path, 0, &firmware_buffer, INT_MAX, NULL, READING_POLICY); //err is errono and number of bytes actually read
	if (!err) {
		dev_err(dev, "Failed loading %s with error %d\n", path, err);
#ifdef USE_FIRMWARE
    goto errret;
#endif
  }
#else
	err = kernel_read_file_from_path(path, &firmware_buffer, &size, INT_MAX, READING_FIRMWARE);		// size : number of bytes actually read
	if (err) {
		dev_err(dev, "Failed loading %s with error %d\n", path, err);
#ifdef USE_FIRMWARE
    goto errret;
#endif
	}
#endif

	priv->firmware_buffer = (u16 *)firmware_buffer;

	err = tegracam_device_register(tc_dev);

	if (err) {
		dev_err(dev, "[%s] : Failed Tegra Camera Driver Registration.\n", __func__);
		goto errret;
	}

	priv->tc_dev = tc_dev;
	priv->s_data = tc_dev->s_data;
	priv->subdev = &tc_dev->s_data->subdev;

	tegracam_set_privdata(tc_dev, (void *)priv);

	err = tier4_isx021_board_setup(priv);

	if (err) {
		dev_err(dev, "[%s] : Board Setup failed\n", __func__);
		goto errret;
	}

	/* Pair sensor to serializer dev */
	err = tier4_max9295_sdev_pair(priv->ser_dev, &priv->g_ctx);

	if (err) {
		dev_err(&client->dev, "[%s] : GMSL Ser Pairing failed\n", __func__);
		goto errret;
	}

	/* Register sensor to deserializer dev */
	err = tier4_max9296_sdev_register(priv->dser_dev, &priv->g_ctx);

	if (err) {
		dev_err(&client->dev, "[%s] : GMSL Deserializer Register failed\n", __func__);
		goto errret;
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
		dev_err(&client->dev,"[%s] : Failed GMSL Serdes setup.\n", __func__ );
		goto errret;
	}


	err = tegracam_v4l2subdev_register(tc_dev, true);
	if (err) {
		dev_err(dev, "[%s] : Failed Tegra Camera Subdev Registration.\n", __func__ );
		goto errret;
	}

  	tier4_isx021_sensor_mutex_unlock();

	err = tier4_isx021_set_response_mode(priv);
	if (err) {
		dev_warn(dev, "[%s] : Failed to set response mode.\n", __func__ );
	}

	dev_info(&client->dev, "Detected ISX021 sensor\n");

	if ( err ) {
		wst_priv[camera_channel_count].p_client = NULL;
		wst_priv[camera_channel_count].p_priv   = NULL;
	} else {
		wst_priv[camera_channel_count].p_client = client;
		wst_priv[camera_channel_count].p_priv   = priv;
	}

	wst_priv[camera_channel_count].sensor_ser_shutdown = false;
	wst_priv[camera_channel_count].des_shutdown = false;

errret:

  	tier4_isx021_sensor_mutex_unlock();

	camera_channel_count++;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,65)

	dev_info(&client->dev, "Detected ISX021 sensor\n");
	return NO_ERROR;
#else
	return err;
#endif
}

static int tier4_isx021_remove(struct i2c_client *client)
{
	struct camera_common_data 	*s_data = to_camera_common_data(&client->dev);
	struct tier4_isx021 		*priv 	= (struct tier4_isx021 *)s_data->priv;

	tier4_isx021_gmsl_serdes_reset(priv);

	tegracam_v4l2subdev_unregister(priv->tc_dev);

	tegracam_device_unregister(priv->tc_dev);

	return NO_ERROR;
}

static void tier4_isx021_shutdown(struct i2c_client *client)
{
	struct 	tier4_isx021 *priv = NULL;
	int 	i;

	tier4_isx021_sensor_mutex_lock();

	for( i = 0 ; i < camera_channel_count ; i++ ) {
		if ( client == wst_priv[i].p_client ) {
			priv =  wst_priv[i].p_priv;
			if ( i & 0x1 ) {	// Even number port ( i = port_number -1 )
				if ( wst_priv[i-1].p_client ) {  					 		// Is the camera connected to another port on the same Des ?
					if ( wst_priv[i-1].sensor_ser_shutdown ) {		 		//  Are the ISP and Ser on another port already shut down ?
						if ( wst_priv[i-1].des_shutdown == false ) { 		// Des is not shut down yet.
		 					wst_priv[i].sensor_ser_shutdown = true;
		 					wst_priv[i].des_shutdown = true;
						} else {	// The Des on the another port is already shut down. This is Error case.
	 						wst_priv[i].sensor_ser_shutdown = false; 		// Unable to shut down ISP, Ser and Des
	 						wst_priv[i].des_shutdown = false;
						}
					} else { // The camera ISP and Ser on another port are not shut down yet.
						if ( wst_priv[i-1].des_shutdown == false ) {		// Des is not shut down yet.
		 					wst_priv[i].sensor_ser_shutdown = true;			//  The ISP and Ser will be shut down.
		 					wst_priv[i].des_shutdown = false;				//  The Des won't be shut down.
						} else {	// Only Des on another port is already shut down. This is Error case.
	 						wst_priv[i].sensor_ser_shutdown = false;		// Unable to shut down ISP, Ser and Des
	 						wst_priv[i].des_shutdown = false;
						}
					}
				} else {  //   Camera is not connected to another port on the same Des. The case of Single camera connected to one Des .
	 				wst_priv[i].sensor_ser_shutdown = true;					//  The ISP and Ser will be shut down.
	 				wst_priv[i].des_shutdown = true;						//  The Des won't be shut down.
				}
			} else {	// Camera is connected to Odd number port
				if ( wst_priv[i+1].p_client ) { 							// Another camera is connected to another port on the same Des
					if ( wst_priv[i+1].sensor_ser_shutdown ) { 				// The ISP and Ser on another port are already shut down
						if ( wst_priv[i+1].des_shutdown == false ) {		// Des is not shut down yet.
		 					wst_priv[i].sensor_ser_shutdown = true;			// The ISP and Ser will be shut down.
		 					wst_priv[i].des_shutdown = true;				// The Des will be shut down.
						} else {	// Des is already shut down. This is Error case.
	 						wst_priv[i].sensor_ser_shutdown = false;		// Unable to shut down ISP, Ser and Des
	 						wst_priv[i].des_shutdown = false;
						}
					} else { // The ISPr and Ser on another port are not shut down yet.
						if ( wst_priv[i+1].des_shutdown == false ) {		// Des is not shut down yet.
		 					wst_priv[i].sensor_ser_shutdown = true;			//  The ISP and Ser will be shut down.
		 					wst_priv[i].des_shutdown = false;				//  The Des won't be shut down.
						} else {	// Only Des on another port is already shut down. This is Error case.
	 						wst_priv[i].sensor_ser_shutdown = false;		// Unable to shut down ISP, Ser and Des
	 						wst_priv[i].des_shutdown = false;
						}
					}
				} else { // Another camera is not connected with another port on the same des
	 				wst_priv[i].sensor_ser_shutdown = true;
	 				wst_priv[i].des_shutdown = true;
				}
			}
			break;
		}
	}

	if ( priv == NULL || i >= camera_channel_count ) {
		tier4_isx021_sensor_mutex_unlock();
		return;
	}

	if ( wst_priv[i].sensor_ser_shutdown ) {
		// Reset camera sensor
		tier4_max9295_control_sensor_power_seq(priv->ser_dev, SENSOR_ID_ISX021, false);

		// S/W Reset max9295
		tier4_max9295_reset_control(priv->ser_dev);
	}

	if ( wst_priv[i].des_shutdown ) {
		// S/W Reset max9296
		tier4_max9296_reset_control(priv->dser_dev, &client->dev, true );
	}

	tier4_isx021_sensor_mutex_unlock();

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
	.shutdown 	= tier4_isx021_shutdown,
	.id_table 	= tier4_isx021_id,
};

static int __init tier4_isx021_init(void)
{
	mutex_init(&tier4_sensor_lock__);

	printk(KERN_INFO "TIERIV Automotive HDR Camera driver : %s\n", BUILD_STAMP);

	return i2c_add_driver(&tier4_isx021_i2c_driver);
}

static void __exit tier4_isx021_exit(void)
{
	mutex_destroy(&tier4_sensor_lock__);

	printk(KERN_INFO "[%s]: tier4_isx021 driver is exitted.\n",__func__);

	i2c_del_driver(&tier4_isx021_i2c_driver);
}

module_init(tier4_isx021_init);
module_exit(tier4_isx021_exit);

MODULE_DESCRIPTION("TIERIV Automotive HDR Camera driver");
MODULE_AUTHOR("K.Iwasaki");
MODULE_AUTHOR("Y.Fujii");
MODULE_LICENSE("GPL v2");




