/*
 * isx021.c - isx021 sensor driver
 *
 * Copyright (c) 2022-2023, TIERIV INC.  All rights reserved.
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

#include "linux/types.h"
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

// ---------------------------------------------------------------

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 65)
#include <linux/kernel_read_file.h>
#endif

#include <media/tegracam_core.h>

#include "tier4-fpga.h"
#include "tier4-gmsl-link.h"
#include "tier4-hw-model.h"
#include "tier4-max9295.h"
#include "tier4-max9296.h"

#define USE_FIRMWARE

//#define USE_I2C_TRANSFER
#define USE_I2C_READ_REG

#undef SHOW_I2C_READ_MSG

#define SHOW_I2C_WRITE_MSG
//#undef  SHOW_I2C_WRITE_MSG

#undef USE_CHECK_MODE_SEL

#undef USE_EMBEDDED_METADAT_HEIGHT_IN_DTB

MODULE_SOFTDEP("pre: tier4_max9296");
MODULE_SOFTDEP("pre: tier4_max9295");
MODULE_SOFTDEP("pre: tier4_fpga");

// Register Address

#define ISX021_SG_MODE_INTERNAL_SYNC 0

#define ISX021_SG_MODE_INTERNAL_SYNC 0

#define ISX021_MIN_GAIN 0
#define ISX021_MAX_GAIN 48

#define ISX021_MIN_EXPOSURE_TIME 0
#define ISX021_MID_EXPOSURE_TIME 11010 // 11 milisecond
#define ISX021_MAX_EXPOSURE_TIME 33000 // 33 milisecond

#define ISX021_GAIN_DEFAULT_VALUE 6

#define ISX021_DEFAULT_FRAME_LENGTH (1125)

// ---   Start of Register definition   ------------------

#define TIER4_ISX021_REG_0_ADDR 0
#define TIER4_ISX021_REG_1_ADDR 1
#define TIER4_ISX021_REG_2_ADDR 2
#define TIER4_ISX021_REG_3_ADDR 3
#define TIER4_ISX021_REG_4_ADDR 4
#define TIER4_ISX021_REG_5_ADDR 5
#define TIER4_ISX021_REG_6_ADDR 6
#define TIER4_ISX021_REG_7_ADDR 7
#define TIER4_ISX021_REG_8_ADDR 8
#define TIER4_ISX021_REG_9_ADDR 9
#define TIER4_ISX021_REG_10_ADDR 10
#define TIER4_ISX021_REG_11_ADDR 11
#define TIER4_ISX021_REG_12_ADDR 12
#define TIER4_ISX021_REG_13_ADDR 13

#define TIER4_ISX021_REG_14_ADDR 14

//   AE  mode

#define TIER4_ISX021_REG_15_ADDR 15
#define TIER4_ISX021_REG_16_ADDR 16
#define TIER4_ISX021_REG_17_ADDR 17
#define TIER4_ISX021_REG_18_ADDR 18

#define TIER4_ISX021_REG_19_ADDR 19
#define TIER4_ISX021_REG_20_ADDR 20
#define TIER4_ISX021_REG_21_ADDR 21
#define TIER4_ISX021_REG_22_ADDR 22

//  ME  mode

#define TIER4_ISX021_REG_23_ADDR 23
#define TIER4_ISX021_REG_24_ADDR 24
#define TIER4_ISX021_REG_25_ADDR 25
#define TIER4_ISX021_REG_26_ADDR 26

#define TIER4_ISX021_REG_27_ADDR 27
#define TIER4_ISX021_REG_28_ADDR 28
#define TIER4_ISX021_REG_29_ADDR 29
#define TIER4_ISX021_REG_30_ADDR 30

//  Analog Gain

#define TIER4_ISX021_REG_31_ADDR 31
#define TIER4_ISX021_REG_32_ADDR 32

#define TIER4_ISX021_REG_33_ADDR 33
#define TIER4_ISX021_REG_34_ADDR 34

#define TIER4_ISX021_REG_35_ADDR 35
#define TIER4_ISX021_REG_36_ADDR 36

#define TIER4_ISX021_REG_37_ADDR 37
#define TIER4_ISX021_REG_38_ADDR 38

#define TIER4_ISX021_REG_39_ADDR 39
#define TIER4_ISX021_REG_40_ADDR 40

#define TIER4_ISX021_REG_41_ADDR 41
#define TIER4_ISX021_REG_42_ADDR 42

#define TIER4_ISX021_REG_43_ADDR 43
#define TIER4_ISX021_REG_44_ADDR 44

#define TIER4_ISX021_REG_45_ADDR 45
#define TIER4_ISX021_REG_46_ADDR 46

// Exposure

#define TIER4_ISX021_REG_47_ADDR 47
#define TIER4_ISX021_REG_48_ADDR 48
#define TIER4_ISX021_REG_49_ADDR 49

#define TIER4_ISX021_REG_50_ADDR 50
#define TIER4_ISX021_REG_51_ADDR 51
#define TIER4_ISX021_REG_52_ADDR 52
#define TIER4_ISX021_REG_53_ADDR 53

#define TIER4_ISX021_REG_54_ADDR 54
#define TIER4_ISX021_REG_55_ADDR 55
#define TIER4_ISX021_REG_56_ADDR 56
#define TIER4_ISX021_REG_57_ADDR 57

#define TIER4_ISX021_REG_58_ADDR 58
#define TIER4_ISX021_REG_59_ADDR 59
#define TIER4_ISX021_REG_60_ADDR 60
#define TIER4_ISX021_REG_61_ADDR 61

#define TIER4_ISX021_REG_62_ADDR 62

#define TIER4_ISX021_REG_63_ADDR 63
#define TIER4_ISX021_REG_64_ADDR 64
#define TIER4_ISX021_REG_65_ADDR 65

#define TIER4_ISX021_REG_66_ADDR 66

//  AE mode

#define TIER4_ISX021_REG_67_ADDR 67

#define TIER4_ISX021_REG_68_ADDR 68
#define TIER4_ISX021_REG_69_ADDR 69

#define TIER4_ISX021_REG_70_ADDR 70

//  Digital Gain

#define TIER4_ISX021_REG_71_ADDR 71
#define TIER4_ISX021_REG_72_ADDR 72

// Distortion Corretion

#define TIER4_ISX021_REG_73_ADDR 73
#define TIER4_ISX021_REG_74_ADDR 74
#define TIER4_ISX021_REG_75_ADDR 75
#define TIER4_ISX021_REG_76_ADDR 76

// Mode change

#define TIER4_ISX021_REG_77_ADDR 77

#define TIER4_ISX021_REG_78_ADDR 78

#define TIER4_ISX021_REG_79_ADDR 79

//  Embedded data

#define TIER4_ISX021_REG_80_ADDR 80
#define TIER4_ISX021_REG_81_ADDR 81

#define TIER4_ISX021_REG_82_ADDR 82

#define TIER4_ISX021_REG_83_ADDR 83
#define TIER4_ISX021_REG_84_ADDR 84
#define TIER4_ISX021_REG_85_ADDR 85
#define TIER4_ISX021_REG_86_ADDR 86
#define TIER4_ISX021_REG_87_ADDR 87
#define TIER4_ISX021_REG_88_ADDR 88
#define TIER4_ISX021_REG_89_ADDR 89

#define TIER4_ISX021_REG_90_ADDR 90
#define TIER4_ISX021_REG_91_ADDR 91

#define TIER4_ISX021_REG_92_ADDR 92
#define TIER4_ISX021_REG_93_ADDR 93

#define TIER4_ISX021_REG_94_ADDR 94
#define TIER4_ISX021_REG_95_ADDR 95
#define TIER4_ISX021_REG_96_ADDR 96
#define TIER4_ISX021_REG_97_ADDR 97
#define TIER4_ISX021_REG_98_ADDR 98

// --- End of  Register definition ------------------------

#define MAX_NUM_OF_REG (100)

#define ISX021_AUTO_EXPOSURE_MODE 0x00
#define ISX021_AE_TIME_UNIT_MICRO_SECOND 0x03

#define BIT_SHIFT_8 8
#define BIT_SHIFT_16 16
#define MASK_1_BIT 0x1
#define MASK_4_BIT 0xF
#define MASK_8_BIT 0xFF

#define NO_ERROR 0

#define TIME_1_MS 1000
#define TIME_2_MS 2000
#define TIME_5_MS 5000
#define TIME_10_MS 10000
#define TIME_20_MS 20000
#define TIME_30_MS 30000
#define TIME_35_MS 35000
#define TIME_40_MS 40000
#define TIME_50_MS 50000
#define TIME_60_MS 60000
#define TIME_70_MS 70000
#define TIME_80_MS 80000
#define TIME_100_MS 100000
#define TIME_120_MS 120000
#define TIME_121_MS 121000
#define TIME_200_MS 200000
#define TIME_250_MS 250000
#define TIME_300_MS 300000
#define TIME_500_MS 500000
#define TIME_800_MS 800000
#define TIME_1000_MS 1000000

#define PLUS_10(x) ((x) + (x) / 10)

#define MAX_NUM_CAMERA 8

#define FIRMWARE_BIN_FILE "/lib/firmware/tier4-isx021.bin"

#define FSYNC_FREQ_HZ 10

#define DISABLE_BOTH_EMBEDDED_DATA 0x0
#define ENABLE_FRONT_EMBEDDED_DATA 0x1
#define ENABLE_REAR_EMBEDDED_DATA 0x2
#define ENABLE_BOTH_EMBEDDED_DATA \
	((ENABLE_FRONT_EMBEDDED_DATA) | (ENABLE_REAR_EMBEDDED_DATA))

enum {
	ISX021_MODE_1920X1280_CROP_30FPS,
	ISX021_MODE_1920X1281_CROP_30FPS_FRONT_EMBEDDED_DATA,
	ISX021_MODE_1920X1294_CROP_30FPS_REAR_EMBEDDED_DATA,
	ISX021_MODE_1920X1295_CROP_30FPS_BOTH_EMBEDDED_DATA,
	ISX021_MODE_START_STREAM,
	ISX021_MODE_STOP_STREAM
};

static const int tier4_isx021_30fps[] = {
	30,
};
static const struct camera_common_frmfmt tier4_isx021_frmfmt[] = {
	{ { 1920, 1280 },
	  tier4_isx021_30fps,
	  1,
	  0,
	  ISX021_MODE_1920X1280_CROP_30FPS },
	{ { 1920, 1281 },
	  tier4_isx021_30fps,
	  1,
	  0,
	  ISX021_MODE_1920X1281_CROP_30FPS_FRONT_EMBEDDED_DATA }, // added 1  lines of embedded data
	{ { 1920, 1294 },
	  tier4_isx021_30fps,
	  1,
	  0,
	  ISX021_MODE_1920X1294_CROP_30FPS_REAR_EMBEDDED_DATA }, // added 14 lines of embedded data
	{ { 1920, 1295 },
	  tier4_isx021_30fps,
	  1,
	  0,
	  ISX021_MODE_1920X1295_CROP_30FPS_BOTH_EMBEDDED_DATA }, // added 15 lines of embedded data
};

const struct of_device_id tier4_isx021_of_match[] = {
	{
		.compatible = "nvidia,tier4_isx021",
	},
	{},
};

MODULE_DEVICE_TABLE(of, tier4_isx021_of_match);

// If you add new ioctl VIDIOC_S_EXT_CTRLS function, please add new CID to the following table.
// and define the CID number in  nvidia/include/media/tegra-v4l2-camera.h

static const u32 ctrl_cid_list[] = {
	TEGRA_CAMERA_CID_GAIN,		 TEGRA_CAMERA_CID_EXPOSURE,
	TEGRA_CAMERA_CID_EXPOSURE_SHORT, TEGRA_CAMERA_CID_FRAME_RATE,
	TEGRA_CAMERA_CID_HDR_EN,
	//TEGRA_CAMERA_CID_DISTORTION_CORRECTION,
};

struct tier4_isx021 {
	struct i2c_client *i2c_client;
	const struct i2c_device_id *id;
	struct v4l2_subdev *subdev;
	struct device *ser_dev;
	struct device *dser_dev;
	struct tier4_gmsl_link_ctx g_ctx;
	u32 frame_length;
	struct camera_common_data *s_data;
	struct tegracam_device *tc_dev;
	//  bool fsync_mode;
	bool distortion_correction;
	bool auto_exposure;
	u16 *firmware_buffer;
	int es_number;
	struct device *fpga_dev;
	int trigger_mode;
	bool inhibit_fpga_access;
	int enable_embedded_data; // 0:disable all embedded data 1: enable front embedded data 2:enable rear embedded data 3: enable front and rear embedded data
	atomic_t test_hw_fault;
};

static const struct regmap_config tier4_sensor_regmap_config = {
	.reg_bits = 16,
	.val_bits = 8,
	.cache_type = REGCACHE_NONE,
};

struct st_priv {
	struct i2c_client *p_client;
	struct tier4_isx021 *p_priv;
	struct tegracam_device *p_tc_dev;
	bool sensor_ser_shutdown;
	bool des_shutdown;
	bool running;
};

static struct st_priv wst_priv[MAX_NUM_CAMERA];

static int camera_channel_count = 0;

static int trigger_mode = 0xCAFE;
static int enable_auto_exposure = 0xCAFE;
static int enable_distortion_correction = 0xCAFE;
static int shutter_time_min = ISX021_MIN_EXPOSURE_TIME;
static int shutter_time_mid = ISX021_MID_EXPOSURE_TIME;
static int shutter_time_max = ISX021_MAX_EXPOSURE_TIME;
static int fsync_mfp = 0;
// static int debug_i2c_write = 0;

module_param(trigger_mode, int, S_IRUGO | S_IWUSR);
module_param(enable_auto_exposure, int, S_IRUGO | S_IWUSR);
module_param(enable_distortion_correction, int, S_IRUGO | S_IWUSR);
module_param(shutter_time_min, int, S_IRUGO | S_IWUSR);
module_param(shutter_time_mid, int, S_IRUGO | S_IWUSR);
module_param(shutter_time_max, int, S_IRUGO | S_IWUSR);

module_param(fsync_mfp, int, S_IRUGO | S_IWUSR);
// module_param(debug_i2c_write, int, S_IRUGO | S_IWUSR);

static struct mutex tier4_sensor_lock__;

static struct mutex tier4_isx021_lock;

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

static char upper(char c)
{
	if ('a' <= c && c <= 'z') {
		c = c - ('a' - 'A');
	}
	return c;
}

static void to_upper_string(char *out, const char *in)
{
	int i;

	i = 0;
	while (in[i] != '\0') {
		out[i] = upper(in[i]);
		i++;
	}
}

#ifndef USE_I2C_TRANSFER // with regmap functions

static inline int tier4_isx021_read_reg(struct camera_common_data *s_data,
					u16 addr, u8 *val)
{
	int err = 0;
	u32 reg_val = 0;
	struct tier4_isx021 *priv = (struct tier4_isx021 *)s_data->priv;
	u16 reg_addr = 0;

#ifdef USE_FIRMWARE
	reg_addr = priv->firmware_buffer[addr];
#else
	reg_addr = addr;
#endif
	usleep_range(TIME_1_MS * 50, PLUS_10(TIME_1_MS * 50));

	err = regmap_read(s_data->regmap, reg_addr, &reg_val);

	*val = reg_val & 0xFF;

	if (err) {
		dev_err(s_data->dev,
			"[%s ] : I2C Read register failed at 0x%04X\n",
			__func__, reg_addr);
	}
#ifdef SHOW_I2C_READ_MSG
	else {
		dev_info(s_data->dev,
			 "[%s] : I2C Read regiser at  %d(0x%04X)=[0x%02X]\n",
			 __func__, addr, reg_addr, reg_val);
	}
#endif

	return err;
}

static int tier4_isx021_write_reg(struct camera_common_data *s_data, u16 addr,
				  u8 val)
{
	int err = 0;
	struct tier4_isx021 *priv = (struct tier4_isx021 *)s_data->priv;
	u16 reg_addr = 0;

#ifdef USE_FIRMWARE
	reg_addr = priv->firmware_buffer[addr];
#else
	reg_addr = addr;
#endif

	usleep_range(TIME_1_MS * 50, PLUS_10(TIME_1_MS * 50));

	err = regmap_write(s_data->regmap, reg_addr, val);

	if (err) {
		dev_err(s_data->dev,
			"[%s] : I2C write register failed at %d(0x%04X)=[0x%02X]\n",
			__func__, addr, reg_addr, val);
	}
#ifdef SHOW_I2C_WRITE_MSG
	else {
		dev_info(s_data->dev,
			 "[%s] : I2C write register at %d(0x%04X)=[0x%02X]\n",
			 __func__, addr, reg_addr, val);
	}
#endif

	return err;
}

#ifdef USE_WRITE_AND_VEIFY

static int tier4_isx021_write_reg_and_verify(struct camera_common_data *s_data,
					     u16 addr, u8 val8)
{
	u32 r_val32;
	u8 r_val8;
	int err = 0;
	struct tier4_isx021 *priv = (struct tier4_isx021 *)s_data->priv;
	u16 reg_addr = 0;

#ifdef USE_FIRMWARE
	reg_addr = priv->firmware_buffer[addr];
#else
	reg_addr = addr;
#endif

	err = regmap_write(s_data->regmap, reg_addr, val8);
	if (err) {
		dev_err(s_data->dev,
			"[%s] : I2C write register failed at %d(0x%04X)=[0x%02X]\n",
			__func__, addr, reg_addr, val8);
	}

	usleep_range(TIME_10_MS, PLUS_10(TIME_10_MS));

	err = regmap_read(s_data->regmap, reg_addr, &r_val32);

	r_val8 = r_val32 & 0xFF;

	if (err) {
		dev_err(s_data->dev,
			"[%s] : I2C Read register failed at 0x%04X=[0x%02X]\n",
			__func__, reg_addr, r_val8);
	}

	return err;
}

#endif //  ifdef USE_WRITE_AND_VEIFY

#else //  ifndef USE_I2C_TRANSFER ( with i2c_transfer function )

#ifdef USE_I2C_READ_REG

static int tier4_isx021_read_reg(struct camera_common_data *s_data, u16 addr,
				 u8 *val8) // with i2c_tranfer
{
	int err = 0;
	u16 reg_addr = 0;
	struct i2c_msg msg[2];
	struct tier4_isx021 *priv = (struct tier4_isx021 *)s_data->priv;
	u8 w_buf[5];

#ifdef USE_FIRMWARE
	reg_addr = priv->firmware_buffer[addr];
#else
	reg_addr = addr;
#endif

	w_buf[0] = (reg_addr >> 8) & 0xFF;
	w_buf[1] = reg_addr & 0xFF;

	msg[0].addr = priv->i2c_client->addr;
	msg[0].flags = 0; // I2C Write
	msg[0].len = 2;
	msg[0].buf = w_buf;

	msg[1].addr = priv->i2c_client->addr;
	msg[1].flags = I2C_M_RD; // I2C Read
	msg[1].len = 1;
	msg[1].buf = val8;

	err = i2c_transfer(priv->i2c_client->adapter, msg, 2);

	if (err < 0) {
		dev_err(s_data->dev,
			"[%s ] : I2C Read register failed at 0x%04X\n",
			__func__, reg_addr);
	} else {
		err = 0;
	}

	usleep_range(TIME_1_MS, PLUS_10(TIME_1_MS));

	return err;
}

#endif // ifdef USE_I2C_READ_REG

// -------------------------------------------------------------------

static int tier4_isx021_write_reg(struct camera_common_data *s_data, u16 addr,
				  u8 val8) // with i2c_tranfer
{
	int err = 0;
	u16 reg_addr = 0;
	struct i2c_msg msg[2];
	struct tier4_isx021 *priv = (struct tier4_isx021 *)s_data->priv;
	u8 w_buf[5];

#ifdef USE_FIRMWARE
	reg_addr = priv->firmware_buffer[addr];
#else
	reg_addr = addr;
#endif

	w_buf[0] = (reg_addr >> 8) & 0xFF;
	w_buf[1] = reg_addr & 0xFF;
	w_buf[2] = val8 & 0xFF;

	msg[0].addr = priv->i2c_client->addr;
	msg[0].flags = 0; // I2C Write
	msg[0].len = 3;
	msg[0].buf = w_buf;

	err = i2c_transfer(priv->i2c_client->adapter, msg, 1);

	if (err < 0) {
		dev_err(s_data->dev,
			"[%s] : I2C write register failed at %d(0x%04X)=[0x%02X]\n",
			__func__, addr, reg_addr, val8);
	} else {
		err = 0;
	}

	usleep_range(TIME_1_MS, PLUS_10(TIME_1_MS));

	return err;
}

#endif // ifndef USE_I2C_TRANSFER

// ------------------------------------------------------------------

static int tier4_isx021_write_reg_raw(struct camera_common_data *s_data, u16 addr,
				  u8 val)
{
	int err = 0;

	usleep_range(TIME_1_MS * 50, PLUS_10(TIME_1_MS * 50));
	err = regmap_write(s_data->regmap, addr, val);
	if (err) {
		dev_err(s_data->dev,
			"[%s] : I2C write register failed at %d(0x%04X)=[0x%02X]\n",
			__func__, addr, addr, val);
	}
	return err;
}

static ssize_t
test_hw_fault_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct camera_common_data *s_data = to_camera_common_data(dev);
	struct tier4_isx021 *priv = s_data->priv;

	return sprintf(buf, "%d\n", atomic_read(&priv->test_hw_fault));
}

static ssize_t
test_hw_fault_store(struct device *dev, struct device_attribute *attr,
			const char *buf, size_t count)
{
	struct camera_common_data *s_data = to_camera_common_data(dev);
	struct tier4_isx021 *priv = s_data->priv;
	long long enable_test;
	int err;

	dev_info(dev, "%s: setting sensor pseudo error state: %s\n",
			__func__, buf);

	err = kstrtoull(buf, 10, &enable_test);
	if (err)
		return err;
	atomic_set(&priv->test_hw_fault, enable_test);

	tier4_isx021_sensor_mutex_lock();

	msleep(100);
	err = tier4_isx021_write_reg_raw(priv->s_data,
			TIER4_ISX021_REG_95_ADDR, 0x5a);
	if (err)
		goto err;

	msleep(100);
	err = tier4_isx021_write_reg_raw(priv->s_data,
			TIER4_ISX021_REG_96_ADDR, 0x01);
	if (err)
		goto err;

	msleep(100);
	err = tier4_isx021_write_reg_raw(priv->s_data,
			TIER4_ISX021_REG_97_ADDR, enable_test ? 0x01 : 0x00);
	if (err)
		goto err;

	if (!enable_test) {
		msleep(100);
		err = tier4_isx021_write_reg_raw(priv->s_data,
				TIER4_ISX021_REG_98_ADDR, 0x01);
		if (err)
			goto err;
	}

	msleep(100);
	err = tier4_isx021_write_reg_raw(priv->s_data, TIER4_ISX021_REG_94_ADDR, 0x01);
	if (err)
		goto err;

err:
	tier4_isx021_sensor_mutex_unlock();

	if (err)
		dev_err(dev, "%s: failed to enable pseudo error: %d\n",
				__func__, err);

	return err ? err : count;
}
static DEVICE_ATTR_RW(test_hw_fault);


static int
tier4_isx021_write_mode_set_f_lock_register(struct camera_common_data *s_data,
					    u8 val8)
{
	int err = 0;

	usleep_range(TIME_20_MS, PLUS_10(TIME_20_MS));

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_90_ADDR,
				     0x06); // ACK Response mode

	usleep_range(TIME_20_MS, PLUS_10(TIME_20_MS));

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_77_ADDR, val8);

	usleep_range(TIME_20_MS, PLUS_10(TIME_20_MS));

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_90_ADDR,
				     0x02); // NAK Response mode

	return 0;
}
// ------------------------------------------------------------------

#ifdef USE_REMAP_REGISTER

static int tier4_isx021_write_remap_register(struct camera_common_data *s_data,
					     u8 val8)
{
	int err = 0;

	usleep_range(TIME_10_MS, PLUS_10(TIME_10_MS));

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_90_ADDR,
				     0x06); // ACK Response mode

	usleep_range(TIME_10_MS, PLUS_10(TIME_10_MS));

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_73_ADDR, val8);

	usleep_range(TIME_10_MS, PLUS_10(TIME_10_MS));

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_90_ADDR,
				     0x02); // NAK Response mode

	return 0;
}
#endif

// -------------------------------------------------------------------

static int
tier4_isx021_transit_to_streaming_state(struct tegracam_device *tc_dev)
{
	struct camera_common_data *s_data = tc_dev->s_data;
	struct device *dev = tc_dev->dev;
	int err = 0;

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_62_ADDR,
				     0x80); // transit to Streaming state
	if (err) {
		dev_err(dev, "[%s] : Write to MODE_SET_F register failed.\n",
			__func__);
	}
	usleep_range(TIME_120_MS, PLUS_10(TIME_120_MS));
	//msleep(120);

	return err;
}

// -------------------------------------------------------------------

static int tier4_isx021_transit_to_startup_state(struct tegracam_device *tc_dev)
{
	struct camera_common_data *s_data = tc_dev->s_data;
	struct device *dev = tc_dev->dev;
	int err = 0;

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_62_ADDR,
				     0x00); // transit to Startup state

	if (err) {
		dev_err(dev, "[%s] : Write to MODE_SET_F register failed.\n",
			__func__);
	}
	usleep_range(TIME_120_MS, PLUS_10(TIME_120_MS));
	//msleep(120);

	return err;
}

// ------------------------------------------------------------------

#ifdef USE_CHECK_MODE_SEL

static int tier4_isx021_check_mode_sel(struct camera_common_data *s_data,
				       u8 *val8)
{
	//int mode_size = sizeof(remap_mode);
	u8 rval8 = 0;
	int err = 0;
	struct device *dev = s_data->dev;

	err = tier4_isx021_read_reg(s_data, TIER4_ISX021_REG_89_ADDR, &rval8);
	if (err) {
		dev_err(dev, "[%s] : Write to MODE_SEL register failed.\n",
			__func__);
		return err;
	}

	*val8 = rval8;

	return err;
}
#endif // ifdef USE_CHECK_MODE_SEL

// ------------------------------------------------------------------

static int tier4_isx021_set_fsync_trigger_mode(struct tier4_isx021 *priv)
{
	int err = 0;
	struct camera_common_data *s_data = priv->s_data;
	struct device *dev = s_data->dev;
	int des_num = 0;

	dev_info(dev, "[%s] : hardware_model=%d.\n", __func__,
		 priv->g_ctx.hardware_model);

	if ((priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_XAVIER) ||
	    (priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_ORIN)) {
		priv->inhibit_fpga_access = false;

		err = tier4_fpga_check_access(priv->fpga_dev);
		if (err) {
			priv->inhibit_fpga_access = true;
		}

		if (tier4_fpga_get_fsync_mode() == FPGA_FSYNC_MODE_DISABLE) {
			dev_info(dev, "[%s] : Disabling FPGA fsync.\n",
				 __func__);

			err = tier4_fpga_disable_fsync_mode(priv->fpga_dev);
			if (err) {
				dev_err(dev,
					"[%s] : Disabling FPGA generate fsync failed.\n",
					__func__);
				if (!priv->inhibit_fpga_access) {
					return err;
				}
			}
		} else {
			err = tier4_fpga_enable_fsync_mode(priv->fpga_dev);
			if (err) {
				dev_err(dev,
					"[%s] : Enabling FPGA generate fsync failed.\n",
					__func__);
				if (!priv->inhibit_fpga_access) {
					return err;
				}
			}

			if (tier4_fpga_get_fsync_mode() ==
			    FPGA_FSYNC_MODE_AUTO) {
				// Auto Trigger Mode
				dev_info(
					dev,
					"[%s] : Enabling FPGA Fsync Auto Trigger mode.\n",
					__func__);

				err = tier4_fpga_set_fsync_auto_trigger(
					priv->fpga_dev);
				if (err) {
					dev_err(dev,
						"[%s] : Enabling FPGA Fsync Auto Trigger mode failed.\n",
						__func__);
					if (!priv->inhibit_fpga_access) {
						return err;
					}
				}

				des_num = priv->g_ctx.reg_mux;
				err = tier4_fpga_set_fsync_signal_frequency(
					priv->fpga_dev, des_num,
					priv->trigger_mode);
				if (err) {
					dev_err(dev,
						"[%s] : Setting the frequency of fsync genrated by FPGA failed.\n",
						__func__);
					if (!priv->inhibit_fpga_access) {
						return err;
					}
				}
			} else if (tier4_fpga_get_fsync_mode() ==
				   FPGA_FSYNC_MODE_MANUAL) {
				// Manual Trigger Mode
				dev_info(
					dev,
					"[%s] : Enabling FPGA Fsync Manual Trigger mode.\n",
					__func__);

				err = tier4_fpga_set_fsync_manual_trigger(
					priv->fpga_dev);
				if (err) {
					dev_err(dev,
						"[%s] : Enabling FPGA Fsync Maunal Trigger mode failed.\n",
						__func__);
					if (!priv->inhibit_fpga_access) {
						return err;
					}
				}
			} // if (tier4_fpga_get_fsync_mode() == FPGA_FSYNC_MODE_MANUAL)
		} // else (tier4_fpga_get_fsync_mode() != FPGA_FSYNC_MODE_DISABLE)
	} // if (priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_XAVIER)

	err = tier4_max9296_setup_gpi(priv->dser_dev, fsync_mfp);

	if (err) {
		dev_err(dev, "[%s] : tier4_max9296_setup_gpi failed.\n",
			__func__);
		return err;
	}

	err = tier4_max9295_setup_gpo(priv->ser_dev);

	if (err) {
		dev_err(dev, "[%s] : tier4_max9295_setup_gpo failed.\n",
			__func__);
		return err;
	}

	// transit to Startup state

	err = tier4_isx021_write_mode_set_f_lock_register(s_data, 0x53);
	if (err) {
		dev_err(dev,
			"[%s] : Time out Error occurred in tier4_isx021_write_register_mode_set_f_lock.\n",
			__func__);
		return err;
	}

	usleep_range(TIME_50_MS, PLUS_10(TIME_50_MS));

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_62_ADDR, 0x00);

	if (err) {
		dev_err(dev, "[%s] : tier4_isx021_write_reg failed.\n",
			__func__);
		return err;
	}

	usleep_range(TIME_50_MS, PLUS_10(TIME_50_MS));

	// set FSYNC_FUNCSEL to 0 for FSYNC triggered mode

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_92_ADDR, 0x00);

	if (err) {
		goto error_exit;
	}

	usleep_range(TIME_50_MS, PLUS_10(TIME_50_MS));

	// set FSYNC_DRVABTY to 3 for FSYNC triggered mode

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_93_ADDR, 0xFF);

	if (err) {
		goto error_exit;
	}

	usleep_range(TIME_50_MS, PLUS_10(TIME_50_MS));

	// set SG_MODE_CTL to 0 for transition to FSYNC mode

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_63_ADDR,
				     0x02); // set FSYNC mode

	if (err) {
		goto error_exit;
	}

	usleep_range(TIME_120_MS, PLUS_10(TIME_120_MS));

	// set SG_MODE_APL to 0 for transition to FSYNC mode

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_78_ADDR, 0x02);

	if (err) {
		goto error_exit;
	}

	usleep_range(TIME_120_MS, PLUS_10(TIME_120_MS));

	usleep_range(TIME_120_MS, PLUS_10(TIME_120_MS));

error_exit:

	return err;
}

/*--------------------------------------------------------------------------*/

static int tier4_isx021_set_response_mode(struct tier4_isx021 *priv)
{
	int err = 0;
	struct tegracam_device *tc_dev = priv->tc_dev;
	struct camera_common_data *s_data = priv->s_data;
	struct device *dev = s_data->dev;
	u8 r_val;

	err = tier4_isx021_write_mode_set_f_lock_register(s_data, 0x53);
	if (err) {
		dev_err(dev,
			"[%s] : Write to MODE_SET_F_LOCK register caused Time out.\n",
			__func__);
		return err;
	}

	usleep_range(TIME_10_MS, PLUS_10(TIME_10_MS));

	err = tier4_isx021_transit_to_startup_state(tc_dev);
	if (err) {
		dev_err(dev, "[%s] : Transition to Startup state failed.\n",
			__func__);
		goto error_exit;
	}

	usleep_range(TIME_10_MS, PLUS_10(TIME_10_MS)); // For ES3 and MP

	err = tier4_isx021_read_reg(s_data, TIER4_ISX021_REG_66_ADDR, &r_val);

	if (r_val == 0x04) {
		priv->es_number = 2;
	} else {
		priv->es_number = 3;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_66_ADDR, 0x06);

	usleep_range(TIME_10_MS, PLUS_10(TIME_10_MS)); // For ES3 and MP

error_exit:

	return err;
}

static int tier4_isx021_gmsl_serdes_setup(struct tier4_isx021 *priv)
{
	int err = 0;
	int des_err = 0;
	struct device *dev;

	if (!priv || !priv->ser_dev || !priv->dser_dev || !priv->i2c_client) {
		return -EINVAL;
	}

	dev = &priv->i2c_client->dev;

	/* For now no separate power on required for serializer device */

	if ((priv->g_ctx.hardware_model != HW_MODEL_ADLINK_ROSCUBE_XAVIER) &&
	    (priv->g_ctx.hardware_model != HW_MODEL_ADLINK_ROSCUBE_ORIN)) {
		tier4_max9296_power_on(priv->dser_dev);
	}
	/* setup serdes addressing and control pipeline */

	err = tier4_max9296_setup_link(priv->dser_dev, &priv->i2c_client->dev);

	if (err) {
		dev_err(dev,
			"[%s] : Configuration of GMSL Deserializer link failed.\n",
			__func__);
		goto error;
	}

	err = tier4_max9295_setup_control(priv->ser_dev);

	/* proceed even if ser setup failed, to setup deser correctly */
	if (err) {
		dev_err(dev, "[%s] : Setup for GMSL Serializer failed.\n",
			__func__);
		goto error;
	}

	des_err = tier4_max9296_setup_control(priv->dser_dev,
					      &priv->i2c_client->dev);

	if (des_err) {
		dev_err(dev, "[%s] : Setup for GMSL Deserializer failed.\n",
			__func__);
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

	tier4_max9296_reset_control(priv->dser_dev, &priv->i2c_client->dev,
				    false);

	if ((priv->g_ctx.hardware_model != HW_MODEL_ADLINK_ROSCUBE_XAVIER) &&
	    (priv->g_ctx.hardware_model != HW_MODEL_ADLINK_ROSCUBE_ORIN)) {
		tier4_max9296_power_off(priv->dser_dev);
	}
}

/*--------------------------------------------------------------------------*/

static int tier4_isx021_power_on(struct camera_common_data *s_data)
{
	int err = 0;
	struct camera_common_power_rail *pw = s_data->power;
	struct camera_common_pdata *pdata = s_data->pdata;
	struct device *dev = s_data->dev;

	if (pdata && pdata->power_on) {
		err = pdata->power_on(pw);

		if (err) {
			dev_err(dev, "[%s] :  Turning on the power failed.\n",
				__func__);
		} else {
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
	int err = 0;
	struct camera_common_power_rail *pw = s_data->power;
	struct camera_common_pdata *pdata = s_data->pdata;
	struct device *dev = s_data->dev;

	if (pdata && pdata->power_off) {
		err = pdata->power_off(pw);

		if (!err) {
			goto power_off_done;
		} else {
			dev_err(dev, "[%s] :  Turning off the power failed.\n",
				__func__);
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
	struct camera_common_power_rail *pw = tc_dev->s_data->power;
	int err = 0;

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

static int tier4_copy_reg_value(struct camera_common_data *s_data, u16 ae_addr,
				u16 me_addr)
{
	int err = 0;
	u8 val8 = 0;

	err = tier4_isx021_read_reg(s_data, ae_addr, &val8);

	if (err) {
		dev_err(s_data->dev,
			"[%s] : I2C Read Register failed at 0x%x\n", __func__,
			ae_addr);
	}

	err = tier4_isx021_write_reg(s_data, me_addr, val8);

	if (err) {
		dev_err(s_data->dev,
			"[%s] : I2C write Register failed at 0x%x =[ 0x%x]\n",
			__func__, me_addr, val8);
	}

	return err;
}

/*--------------------------------------------------------------------------*/

static int tier4_isx021_set_gain(struct tegracam_device *tc_dev, s64 val)
{
	int err = 0;

	s64 gain;
	u8 digital_gain_low_byte;
	u8 digital_gain_high_byte;

	struct camera_common_data *s_data = tc_dev->s_data;
	struct device *dev = tc_dev->dev;

	if (val > ISX021_MAX_GAIN) {
		val = ISX021_MAX_GAIN;
	} else if (val < ISX021_MIN_GAIN) {
		val = ISX021_MIN_GAIN;
	}

	dev_info(dev, "[%s] :Gain is set to %lld\n", __func__, val);

	gain = 10 * val;

	digital_gain_low_byte = gain & 0xFF;

	digital_gain_high_byte = (gain >> 8) & 0xFF;

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_68_ADDR, 0x01);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_69_ADDR, 0x01);

	if (err) {
		goto fail;
	}

	// Shutter 0 value

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_15_ADDR,
				   TIER4_ISX021_REG_23_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_16_ADDR,
				   TIER4_ISX021_REG_24_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_17_ADDR,
				   TIER4_ISX021_REG_25_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_18_ADDR,
				   TIER4_ISX021_REG_26_ADDR);

	if (err) {
		goto fail;
	}

	// Shutter 1 value

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_19_ADDR,
				   TIER4_ISX021_REG_27_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_20_ADDR,
				   TIER4_ISX021_REG_28_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_21_ADDR,
				   TIER4_ISX021_REG_29_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_22_ADDR,
				   TIER4_ISX021_REG_30_ADDR);

	if (err) {
		goto fail;
	}

	// Set Analog Gain for shutter 0

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_31_ADDR,
				     digital_gain_low_byte);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_32_ADDR,
				     digital_gain_high_byte);

	if (err) {
		goto fail;
	}

	// copy  AE Analog gain value to ME analog gain registers for shutter 1 to 3

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_35_ADDR,
				   TIER4_ISX021_REG_37_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_36_ADDR,
				   TIER4_ISX021_REG_38_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_39_ADDR,
				   TIER4_ISX021_REG_41_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_40_ADDR,
				   TIER4_ISX021_REG_42_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_43_ADDR,
				   TIER4_ISX021_REG_45_ADDR);

	if (err) {
		goto fail;
	}

	err = tier4_copy_reg_value(s_data, TIER4_ISX021_REG_44_ADDR,
				   TIER4_ISX021_REG_46_ADDR);

	if (err) {
		goto fail;
	}

	msleep(100); // For ES3

	// Change AE mode

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_67_ADDR, 0x03);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_71_ADDR,
				     digital_gain_low_byte);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_72_ADDR,
				     digital_gain_high_byte);

	if (err) {
		goto fail;
	}

	return NO_ERROR;

fail:
	dev_err(dev, "[%s] : Gain control Error\n", __func__);

	return err;
}

/* ------------------------------------------------------------------------- */

static int tier4_isx021_set_frame_rate(struct tegracam_device *tc_dev, s64 val)
{
	struct tier4_isx021 *priv =
		(struct tier4_isx021 *)tegracam_get_privdata(tc_dev);

	/* fixed 10fps */
	priv->frame_length = ISX021_DEFAULT_FRAME_LENGTH;

	return NO_ERROR;
}

/* ------------------------------------------------------------------------- */

static int tier4_isx021_set_auto_exposure(struct tegracam_device *tc_dev)
{
	int err = 0;
	struct camera_common_data *s_data = tc_dev->s_data;

	// Change to Auto exposure mode

	msleep(100); // For ES3

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_67_ADDR,
				     ISX021_AUTO_EXPOSURE_MODE);

	// Set min exposure time unit
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_47_ADDR,
				     ISX021_AE_TIME_UNIT_MICRO_SECOND);
	if (err) {
		goto fail;
	}

	// Set mid exposure time unit
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_48_ADDR,
				     ISX021_AE_TIME_UNIT_MICRO_SECOND);
	if (err) {
		goto fail;
	}

	// Set max exposure time unit
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_49_ADDR,
				     ISX021_AE_TIME_UNIT_MICRO_SECOND);
	if (err) {
		goto fail;
	}

	// Set min exposure time
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_50_ADDR,
				     shutter_time_min & 0xFF);
	if (err) {
		goto fail;
	}
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_51_ADDR,
				     (shutter_time_min >> 8) & 0xFF);
	if (err) {
		goto fail;
	}

	// Set mid exposure time
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_54_ADDR,
				     shutter_time_mid & 0xFF);
	if (err) {
		goto fail;
	}
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_55_ADDR,
				     (shutter_time_mid >> 8) & 0xFF);
	if (err) {
		goto fail;
	}

	// Set max exposure time
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_58_ADDR,
				     shutter_time_max & 0xFF);
	if (err) {
		goto fail;
	}
	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_59_ADDR,
				     (shutter_time_max >> 8) & 0xFF);
	if (err) {
		goto fail;
	}

fail:

	return err;
}

/* ------------------------------------------------------------------------- */

static int tier4_isx021_set_exposure(struct tegracam_device *tc_dev, s64 val)
{
	int err = 0;

	u8 exp_time_byte0;
	u8 exp_time_byte1;
	u8 exp_time_byte2;
	u8 exp_time_byte3;
	// struct tier4_isx021 *priv = (struct tier4_isx021 *)tegracam_get_privdata(tc_dev);
	struct camera_common_data *s_data = tc_dev->s_data;

	//  unit for val is micro-second

	if (val > ISX021_MAX_EXPOSURE_TIME) {
		val = ISX021_MAX_EXPOSURE_TIME;
	} else if (val < ISX021_MIN_EXPOSURE_TIME) {
		val = ISX021_MIN_EXPOSURE_TIME;
	}

	exp_time_byte0 = val & 0xFF;

	exp_time_byte1 = (val >> 8) & 0xFF;

	exp_time_byte2 = (val >> 16) & 0xFF;

	exp_time_byte3 = (val >> 24) & 0xFF;

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_48_ADDR, 0x03);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_49_ADDR, 0x03);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_54_ADDR,
				     exp_time_byte0);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_55_ADDR,
				     exp_time_byte1);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_56_ADDR,
				     exp_time_byte2);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_57_ADDR,
				     exp_time_byte3);

	if (err) {
		goto fail;
	}

	// Shutter2

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_58_ADDR,
				     exp_time_byte0);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_59_ADDR,
				     exp_time_byte1);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_60_ADDR,
				     exp_time_byte2);

	if (err) {
		goto fail;
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_61_ADDR,
				     exp_time_byte3);

	if (err) {
		goto fail;
	}

	return NO_ERROR;

fail:

	return err;
}
// --------------------------------------------------------------------------------------
//  Enable Distortion Coreection
// --------------------------------------------------------------------------------------
static int
tier4_isx021_enable_distortion_correction(struct tegracam_device *tc_dev,
					  bool is_enabled)
{
	int err = 0;
	struct device *dev = tc_dev->dev;
	struct camera_common_data *s_data = tc_dev->s_data;

	err = tier4_isx021_write_mode_set_f_lock_register(s_data, 0x53);
	if (err) {
		dev_err(dev,
			"[%s] : Write to MODE_SET_F_LOCK register caused Time out.\n",
			__func__);
		return err;
	}

	usleep_range(TIME_10_MS, PLUS_10(TIME_10_MS));

	err = tier4_isx021_transit_to_startup_state(tc_dev);

	if (err) {
		dev_err(dev, "[%s] : Transition to Startup state failed.\n",
			__func__);
		goto error_exit;
	}

	usleep_range(TIME_35_MS, PLUS_10(TIME_35_MS));
	//msleep(35);

	if (is_enabled) {
		err = tier4_isx021_write_reg(tc_dev->s_data,
					     TIER4_ISX021_REG_74_ADDR, 0x01);

		usleep_range(TIME_35_MS, PLUS_10(TIME_35_MS));
		//msleep(35);

		err = tier4_isx021_write_reg(tc_dev->s_data,
					     TIER4_ISX021_REG_75_ADDR, 0x01);
		if (err) {
			goto error_exit;
		}
	} else {
		dev_info(tc_dev->dev, "[%s] : Disabled Distortion Correction.",
			 __func__);

		err = tier4_isx021_write_reg(tc_dev->s_data,
					     TIER4_ISX021_REG_74_ADDR, 0x00);

		usleep_range(TIME_35_MS, PLUS_10(TIME_35_MS));
		//msleep(35);

		err = tier4_isx021_write_reg(tc_dev->s_data,
					     TIER4_ISX021_REG_75_ADDR, 0x00);
		if (err) {
			goto error_exit;
		}
	}

error_exit:

	if (err && !is_enabled) {
		dev_err(tc_dev->dev,
			"[%s] : Disabling Distortion Correction failed.",
			__func__);
	} else if (err && is_enabled) {
		dev_err(tc_dev->dev,
			"[%s] : Enabling Distortion Correction failed.",
			__func__);
	}

	return err;
}

// Please refer to the section for embedded data in ISX021 application note

static int tier4_isx021_setup_embedded_data(struct tegracam_device *tc_dev,
					    int enable_embedded_data)
{
	struct camera_common_data *s_data = tc_dev->s_data;
	struct device *dev = s_data->dev;
	int err = 0;
	int rc = 0;

	dev_dbg(dev, "[%s] : Setup for Embedded data.\n", __func__);

	err = tier4_isx021_write_mode_set_f_lock_register(s_data, 0x53);

	if (err) {
		dev_err(dev,
			"[%s] : Write to MODE_SET_F_LOCK Register failed\n",
			__func__);
		//goto error_exit;
	}

	usleep_range(TIME_100_MS, PLUS_10(TIME_100_MS));
	//msleep(100);

	err = tier4_isx021_transit_to_startup_state(tc_dev);
	if (err) {
		dev_err(dev, "[%s] : Transition to Startup state failed.\n",
			__func__);
		goto error_exit;
	}

	usleep_range(TIME_120_MS, PLUS_10(TIME_120_MS));
	//msleep(120);

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_91_ADDR,
				     0x00); // DCROP_ON_APL Regsiter

	if (err) {
		dev_err(dev,
			"[%s] : Write to ISX021_DCROP_ON Register failed.\n",
			__func__);
		goto error_exit;
	}

	err = tier4_isx021_write_reg(
		s_data, TIER4_ISX021_REG_82_ADDR,
		0x00); // DCROP_ON register : Enable 0 Padding after Embedded data

	if (err) {
		dev_err(dev,
			"[%s] : Write to ISX021_DCROP_ON Register failed.\n",
			__func__);
		goto error_exit;
	}

	dev_dbg(dev, "[%s] : enable_embedded_data = %d\n", __func__,
		enable_embedded_data);

	// Enable Front Embedded data

	if (enable_embedded_data & ENABLE_FRONT_EMBEDDED_DATA) {
		dev_info(dev, "[%s] : Enabling Front Embedded.\n", __func__);

		err = tier4_isx021_write_reg(
			s_data, TIER4_ISX021_REG_80_ADDR,
			0x1); // Enable outputting Front Embedded Data
		if (err) {
			dev_err(dev,
				"[%s] : Write to IR_DR_I2I_FEBD_EN Register failed.\n",
				__func__);
			goto error_exit;
		}

		err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_83_ADDR,
					     0x1);
		if (err) {
			dev_err(dev,
				"[%s] : Write to IFD_DATATYPE_FEBD_SEL Register failed.\n",
				__func__);
			goto error_exit;
		}

		err = tier4_isx021_write_reg(
			s_data, TIER4_ISX021_REG_85_ADDR,
			0x1E); //  EBD data are encoded as YcbCr(YUV422: applied in this driver )

		if (err) {
			dev_err(dev,
				"[%s] : Write to IFD_DATATYPE_FEBD Register failed.\n",
				__func__);
			goto error_exit;
		}

	} else { // Diable Front Embedded data

		dev_info(dev, "[%s] : Disabling Front Embedded.\n", __func__);

		err = tier4_isx021_write_reg(
			s_data, TIER4_ISX021_REG_80_ADDR,
			0x0); // Disable outputting Front Embedded Data
		if (err) {
			dev_err(dev,
				"[%s] : Write to IR_DR_I2I_FEBD_EN Register failed.\n",
				__func__);
			goto error_exit;
		}

		err = tier4_isx021_write_reg(
			s_data, TIER4_ISX021_REG_83_ADDR,
			0x0); // Front Embedded data type is not selected by IR_DR_I2I_FEBD
		if (err) {
			dev_err(dev,
				"[%s] : Write to IFD_DATATYPE_FEBD_SEL Register failed.\n",
				__func__);
			goto error_exit;
		}

		err = tier4_isx021_write_reg(
			s_data, TIER4_ISX021_REG_85_ADDR,
			0x12); // Default data type for Front Embedded
		if (err) {
			dev_err(dev,
				"[%s] : Write to IR_DR_I2I_FEBD Register failed.\n",
				__func__);
			goto error_exit;
		}
	}

	if (enable_embedded_data &
	    ENABLE_REAR_EMBEDDED_DATA) { // Enable Rear Embedded data

		dev_info(dev, "[%s] : Enabling Rear Embedded.\n", __func__);

		err = tier4_isx021_write_reg(
			s_data, TIER4_ISX021_REG_81_ADDR,
			0x1); // Enable  outputting Rear Embedded Data
		if (err) {
			dev_err(dev,
				"[%s] : Write to IR_DR_I2I_REBD_EN Register failed.\n",
				__func__);
			goto error_exit;
		}

		err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_84_ADDR,
					     0x0D);
		if (err) {
			dev_err(dev,
				"[%s] : Write to IFD_DATATYPE_REBD_SEL Register failed.\n",
				__func__);
			goto error_exit;
		}

		err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_86_ADDR,
					     0x1E);
		if (err) {
			dev_err(dev,
				"[%s] : Write to IFD_DATATYPE_REBD Register failed.\n",
				__func__);
			goto error_exit;
		}

	} else { // Diabled Rear Embedded data

		dev_info(dev, "[%s] : Disabling Rear Embedded.\n", __func__);

		err = tier4_isx021_write_reg(
			s_data, TIER4_ISX021_REG_81_ADDR,
			0x0); // Disable outputting Rear Embedded Data
		if (err) {
			dev_err(dev,
				"[%s] : Write to IR_DR_I2I_REBD_EN Register failed.\n",
				__func__);
			goto error_exit;
		}

		err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_84_ADDR,
					     0x0C);
		if (err) {
			dev_err(dev,
				"[%s] : Write to IFD_DATATYPE_REBD_SEL Register failed.\n",
				__func__);
			goto error_exit;
		}

		err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_86_ADDR,
					     0x35);
		if (err) {
			dev_err(dev,
				"[%s] : Write to IFD_DATATYPE_REBD Register failed.\n",
				__func__);
			goto error_exit;
		}
	}

	err = tier4_isx021_write_reg(s_data, TIER4_ISX021_REG_87_ADDR, 0x1E);
	if (err) {
		dev_err(dev,
			"[%s] : Write to IFD_DATATYPE_VISIBLE Register failed.\n",
			__func__);
		goto error_exit;
	}

error_exit:

#if 1

	rc = tier4_isx021_write_mode_set_f_lock_register(s_data, 0x53);
	if (rc) {
		dev_err(dev,
			"[%s] : Write to MODE_SET_F_LOCK Register failed.\n",
			__func__);
	}
#endif

	return (err | rc);
}

// --------------------------------------------------------------------------------------
//  If you add new ioctl VIDIOC_S_EXT_CTRLS function,
//  please add the new memeber and the function at the following table.

static struct tegracam_ctrl_ops tier4_isx021_ctrl_ops = {
	.numctrls = ARRAY_SIZE(ctrl_cid_list),
	.ctrl_cid_list = ctrl_cid_list,
	.set_gain = tier4_isx021_set_gain,
	.set_exposure = tier4_isx021_set_exposure,
	.set_exposure_short = tier4_isx021_set_exposure,
	.set_frame_rate = tier4_isx021_set_frame_rate,
	.set_group_hold = tier4_isx021_set_group_hold,
	//    .set_distortion_correction  = tier4_isx021_set_distortion_correction,
};

// --------------------------------------------------------------------------------------

static struct camera_common_pdata *
tier4_isx021_parse_dt(struct tegracam_device *tc_dev)
{
	struct device *dev = tc_dev->dev;
	struct device_node *node = dev->of_node;
	struct camera_common_pdata *board_priv_pdata;
	const struct of_device_id *match;
	int err;

	if (!node) {
		return NULL;
	}

	match = of_match_device(tier4_isx021_of_match, dev);

	if (!match) {
		dev_err(dev, "[%s] : Finding matching dt id failed.\n",
			__func__);
		return NULL;
	}

	board_priv_pdata =
		devm_kzalloc(dev, sizeof(*board_priv_pdata), GFP_KERNEL);

	err = of_property_read_string(node, "mclk",
				      &board_priv_pdata->mclk_name);

	if (err) {
		dev_err(dev, "[%s] : mclk not in DT.\n", __func__);
	}

	return board_priv_pdata;
}

/*   tier4_isx021_set_mode() can not be needed. But it remains for compatiblity */

static int tier4_isx021_set_mode(struct tegracam_device *tc_dev)
{
	struct tier4_isx021 *priv =
		(struct tier4_isx021 *)tegracam_get_privdata(tc_dev);
	struct camera_common_data *s_data = tc_dev->s_data;
	struct device *dev = tc_dev->dev;

	int err = 0;

	switch (s_data->mode) {
	case ISX021_MODE_1920X1280_CROP_30FPS:
		priv->enable_embedded_data = DISABLE_BOTH_EMBEDDED_DATA;
		break;
	case ISX021_MODE_1920X1281_CROP_30FPS_FRONT_EMBEDDED_DATA:
		priv->enable_embedded_data = ENABLE_FRONT_EMBEDDED_DATA;
		break;
	case ISX021_MODE_1920X1294_CROP_30FPS_REAR_EMBEDDED_DATA:
		priv->enable_embedded_data = ENABLE_REAR_EMBEDDED_DATA;
		break;
	case ISX021_MODE_1920X1295_CROP_30FPS_BOTH_EMBEDDED_DATA:
		priv->enable_embedded_data = ENABLE_BOTH_EMBEDDED_DATA;
		break;
	default:
		priv->enable_embedded_data = DISABLE_BOTH_EMBEDDED_DATA;
		break;
	}

	dev_dbg(dev, "[%s]: priv->enable_embedded_data = %d \n", __func__,
		priv->enable_embedded_data);

	return err;
}

// -------------------------------------------------------------------

static int tier4_isx021_start_one_streaming(struct tegracam_device *tc_dev)
{
	struct tier4_isx021 *priv =
		(struct tier4_isx021 *)tegracam_get_privdata(tc_dev);
	struct device *dev = tc_dev->dev;
	int err;

	/* enable serdes streaming */

	err = tier4_max9295_setup_streaming(priv->ser_dev);
	if (err) {
		goto exit;
	}

	err = tier4_max9296_setup_streaming(priv->dser_dev, dev);
	if (err) {
		dev_err(dev, "[%s] : Setup for Streaming failed.\n", __func__);
		goto exit;
	}

	err = tier4_max9295_control_sensor_power_seq(priv->ser_dev,
						     SENSOR_ID_ISX021, true);
	if (err) {
		dev_err(dev, "[%s] : Power on Camera Sensor failed.\n",
			__func__);
		goto exit;
	}

	if (enable_auto_exposure == 1) {
		priv->auto_exposure = true;
		dev_info(dev, "[%s] : Parameter[enable_auto_exposure] = 1.\n",
			 __func__);
	}

	if (priv->auto_exposure == true) {
		err = tier4_isx021_set_auto_exposure(tc_dev);
		if (err) {
			dev_err(dev, "[%s] : Enabling Auto Exposure failed.\n",
				__func__);
			goto exit;
		} else {
			dev_info(dev, "[%s] : Enabled Auto Exposure.\n",
				 __func__);
		}
	} else {
		dev_info(dev, "[%s] : Disabled Auto Exposure.\n", __func__);
	}
	if (err) {
		dev_err(dev,
			"[%s] : Setting digital gain  to the default value failed.\n",
			__func__);
	}

	dev_info(dev, "[%s] : trigger_mode = %d.\n", __func__, trigger_mode);

	priv->trigger_mode = trigger_mode;

	if (priv->trigger_mode == 1) {
		//    priv->fsync_mode = true;
		dev_info(dev, "[%s] : Enabled Slave(fsync triggered) mode.\n",
			 __func__);
	}

	//  if (priv->fsync_mode == true)
	if (priv->trigger_mode == 1) {
		err = tier4_isx021_set_fsync_trigger_mode(priv);
		if (err) {
			dev_err(dev,
				"[%s] :  Camera sensor is unable to work with Slave(fsync triggered) mode.\n",
				__func__);
			//      goto exit;
		}
	}

	usleep_range(TIME_20_MS, PLUS_10(TIME_20_MS));

	if (enable_distortion_correction == 1) {
		priv->distortion_correction = true;
		dev_info(
			dev,
			"[%s] : Prameter[enable_distortion_correction] = 1 .\n",
			__func__);
	}
	err = tier4_isx021_enable_distortion_correction(
		tc_dev, priv->distortion_correction);
	if (err) {
		dev_err(dev, "[%s] : Enabling Distortion Correction failed.\n",
			__func__);
		goto exit;
	}

	err = tier4_isx021_setup_embedded_data(tc_dev,
					       priv->enable_embedded_data);
	if (err) {
		dev_err(dev, "[%s] : Setup for Embedded data failed.\n",
			__func__);
		goto exit;
	}

	err = tier4_isx021_transit_to_streaming_state(tc_dev);
	if (err) {
		dev_err(dev, "[%s] : Transition to Streaming state failed.\n",
			__func__);
		return err;
	}

	usleep_range(TIME_20_MS, PLUS_10(TIME_20_MS));

	err = tier4_max9296_start_streaming(priv->dser_dev, dev);
	if (err) {
		dev_err(dev, "[%s] : Des(Max9296) failed to start streaming.\n",
			__func__);
		return err;
	}

	dev_info(dev, "[%s] : Camera has started streaming.\n", __func__);

	return NO_ERROR;

exit:

	dev_err(dev, "[%s] :  Camera failed to start streaming.\n", __func__);

	return err;
}

//-------------------------------------------------------------------

static bool tier4_isx021_is_camera_connected_to_port(int nport)
{
	if (wst_priv[nport].p_client) {
		return true;
	}
	return false;
}

static bool tier4_isx021_check_null_tc_dev_for_port(int nport)
{
	if (wst_priv[nport].p_tc_dev == NULL) {
		return true;
	}
	return false;
}

static bool tier4_isx021_is_camera_running_on_port(int nport)
{
	if (wst_priv[nport].running) {
		return true;
	}
	return false;
}

static bool tier4_isx021_is_current_port(struct tier4_isx021 *priv, int nport)
{
	if (priv->i2c_client == wst_priv[nport].p_client) {
		return true;
	}
	return false;
}

static void tier4_isx021_set_running_flag(int nport, bool flag)
{
	wst_priv[nport].running = flag;
}

// static void tier4_isx021_reset_running_flag(int nport)
//{
//  wst_priv[nport].running  = false;
//}

static int tier4_isx021_stop_streaming(struct tegracam_device *tc_dev)
{
	struct device *dev = tc_dev->dev;
	struct tier4_isx021 *priv =
		(struct tier4_isx021 *)tegracam_get_privdata(tc_dev);
	int i, err = 0;

	mutex_lock(&tier4_isx021_lock);

	for (i = 0; i < camera_channel_count; i++) {
		if (tier4_isx021_is_camera_connected_to_port(i)) {
			if (tier4_isx021_is_current_port(priv, i) &&
			    tier4_isx021_is_camera_running_on_port(i)) {
				/* disable serdes streaming */
				err = tier4_max9296_stop_streaming(
					priv->dser_dev, dev);
				if (err) {
					return err;
				}
				tier4_isx021_set_running_flag(i, false);
				break;
			}
		}
	}

	mutex_unlock(&tier4_isx021_lock);

	return NO_ERROR;
}

/* *************************************************************************** */
/*  1. In the case where a  camera is connected to GMSL A port on a Des.       */
/*                                                                             */
/*        a > Check if another camera is connected to GMSL B port              */
/*          a-1) Connected:                                                    */
/*                b > Check if another camera on GMSL B port is running        */
/*                  b-1) Running:                                              */
/*                         c > Check if the camera on GMSL A port is running   */
/*                           c-1) Running :                                    */
/*                                  Do nothing and return                      */
/*                           c-2) Not running :                                */
/*                               Start the camera on GMSL A port.              */
/*                  b-2) Not Running:                                          */
/*                         Start the camera on GMSL B port.                    */
/*         a-2) Not connected :                                                */
/*                b > Check if the camera on GMSL A port is running            */
/*                  b-1) Running :                                             */
/*                         Do nothing and return                               */
/*                  b-2) Not running :                                         */
/*                         Start the camera on GMSL A port.                    */
/*                                                                             */
/*  2. In the case where a camera is connected to GMSL B port on a Des.        */
/*                                                                             */
/*        a > Check if the camera on GMSL B port is running                    */
/*                  a-1) Running :                                             */
/*                         Do nothing and return                               */
/*                  a-2) Not running :                                         */
/*                         Start the camera on GMSL B port.                    */
/* *************************************************************************** */

static int tier4_isx021_start_streaming(struct tegracam_device *tc_dev)
{
	int i, err = 0;
	//    struct  tier4_isx021    *next_client_priv;
	struct tier4_isx021 *priv =
		(struct tier4_isx021 *)tegracam_get_privdata(tc_dev);
	struct device *dev = tc_dev->dev;

	mutex_lock(&tier4_isx021_lock);

	for (i = 0; i < camera_channel_count; i++) {
		if (i & 0x1) { // if  i = 1,3,5,7 ( GMSL B port of a Des )

			if (tier4_isx021_is_camera_connected_to_port(
				    i)) { //  a  camera is connected to GMSL B portL

				if ((tier4_isx021_is_current_port(priv, i) ==
				     true) &&
				    (tier4_isx021_is_camera_running_on_port(
					     i) == false)) {
					//                    err = tier4_isx021_start_one_streaming(wst_priv[i].p_tc_dev, wst_priv[i].p_priv);
					err = tier4_isx021_start_one_streaming(
						wst_priv[i].p_tc_dev);

					if (err) {
						dev_err(dev,
							"[%s] : Starting one streaming on Camera sensor failed.\n",
							__func__);
						goto error_exit;
					}
					wst_priv[i].running = true;
					break;
				}
			}
		} else { // if  i = 0,2,4,6 ( GMSL A side port of a Des0,Des1,Des2 or Des3 )

			if ((tier4_isx021_is_camera_connected_to_port(i) ==
			     true) &&
			    (tier4_isx021_is_current_port(priv, i) == true)) {
				if (tier4_isx021_is_camera_connected_to_port(
					    i + 1) ==
				    false) { // if another one camera( GMSL B port)
					// is not connected to Des
					if (tier4_isx021_is_camera_running_on_port(
						    i) ==
					    false) { //   and if the camera is not running.

						//                      err = tier4_isx021_start_one_streaming(wst_priv[i].p_tc_dev, wst_priv[i].p_priv);
						err = tier4_isx021_start_one_streaming(
							wst_priv[i].p_tc_dev);
						if (err) {
							dev_err(dev,
								"[%s] : Starting one streaming for next isx021 client failed.\n",
								__func__);
							goto error_exit;
						}
						tier4_isx021_set_running_flag(
							i, true);
					}
					break;
				}

				//  two cameras are connected to one Des.

				if (tier4_isx021_check_null_tc_dev_for_port(
					    i + 1)) { // check if tc_dev is null
					dev_err(dev,
						"[%s] : wst_priv[%d].p_tc_dev is NULL.\n",
						__func__, i + 1);
					err = -EINVAL;
					goto error_exit;
				}

				if (tier4_isx021_is_camera_running_on_port(
					    i + 1) == false) {
					//                    err = tier4_isx021_start_one_streaming(wst_priv[i+1].p_tc_dev, wst_priv[i+1].p_priv);
					err = tier4_isx021_start_one_streaming(
						wst_priv[i + 1].p_tc_dev);
					if (err) {
						dev_err(dev,
							"[%s] : Start one streaming for the next isx021 client failed.\n",
							__func__);
						goto error_exit;
					}
					tier4_isx021_set_running_flag(i + 1,
								      true);
					usleep_range(TIME_200_MS,
						     PLUS_10(TIME_200_MS));
					//msleep(200);
					mutex_unlock(
						&tier4_isx021_lock); // stop streaming on GMSL B port
					tier4_isx021_stop_streaming(
						wst_priv[i + 1].p_tc_dev);
					mutex_lock(&tier4_isx021_lock);
				}

				if (tier4_isx021_is_camera_running_on_port(i) ==
				    false) {
					//                    err = tier4_isx021_start_one_streaming(wst_priv[i].p_tc_dev, wst_priv[i].p_priv);
					err = tier4_isx021_start_one_streaming(
						wst_priv[i].p_tc_dev);
					if (err) {
						dev_err(dev,
							"[%s] : Starting one streaming for current isx021 client failed.\n",
							__func__);
						goto error_exit;
					}
					tier4_isx021_set_running_flag(i, true);
				}
			}
		}
	} // for loop

	err = NO_ERROR;

error_exit:

	mutex_unlock(&tier4_isx021_lock);

	//    tier4_isx021_sensor_mutex_unlock();

	return err;
}

static struct camera_common_sensor_ops tier4_isx021_common_ops = {
	.numfrmfmts = ARRAY_SIZE(tier4_isx021_frmfmt),
	.frmfmt_table = tier4_isx021_frmfmt,
	.power_on = tier4_isx021_power_on,
	.power_off = tier4_isx021_power_off,
	.write_reg = tier4_isx021_write_reg,
	.read_reg = tier4_isx021_read_reg,
	.parse_dt = tier4_isx021_parse_dt,
	.power_get = tier4_isx021_power_get,
	.power_put = tier4_isx021_power_put,
	.set_mode = tier4_isx021_set_mode,
	.start_streaming = tier4_isx021_start_streaming,
	.stop_streaming = tier4_isx021_stop_streaming,
};

static int tier4_isx021_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	// struct i2c_client *client = v4l2_get_subdevdata(sd);

	return NO_ERROR;
}

static const struct v4l2_subdev_internal_ops tier4_isx021_subdev_internal_ops = {
	.open = tier4_isx021_open,
};

static const char *of_stdout_options;

static int tier4_isx021_board_setup(struct tier4_isx021 *priv)
{
	struct tegracam_device *tc_dev = priv->tc_dev;
	struct device *dev = tc_dev->dev;
	struct device_node *node = dev->of_node;
	struct device_node *mode_node;
	struct device_node *ser_node;
	struct device_node *root_node;
	struct i2c_client *ser_i2c = NULL;
	struct device_node *dser_node;
	struct i2c_client *dser_i2c = NULL;
	struct device_node *fpga_node = NULL;
	struct i2c_client *fpga_i2c = NULL;
	struct device_node *gmsl;
	int value = 0xFFFF;
	const char *str_value;
	const char *str_value1[2];
	int i;
	int err;
	const char *str_model;
	char upper_str_model[64];
	char *str_err;
	//char *sub_str_err;

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

	if (str_err) {
		priv->g_ctx.hardware_model = HW_MODEL_NVIDIA_ORIN_DEVKIT;
	}

	str_err = strstr(upper_str_model, STR_DTB_MODEL_NAME_XAVIER);
	if (str_err) {
		priv->g_ctx.hardware_model = HW_MODEL_NVIDIA_XAVIER_DEVKIT;
	}

	str_err = strstr(upper_str_model, STR_DTB_MODEL_NAME_ROSCUBE_XAVIER);
	if (str_err) {
		priv->g_ctx.hardware_model = HW_MODEL_ADLINK_ROSCUBE_XAVIER;
	}

	str_err = strstr(upper_str_model, STR_DTB_MODEL_NAME_ROSCUBE_ORIN);
	if (str_err) {
		priv->g_ctx.hardware_model = HW_MODEL_ADLINK_ROSCUBE_ORIN;
	}

	dev_info(dev, "[%s] : hardware_model=%d\n", __func__,
		 priv->g_ctx.hardware_model);
	dev_info(dev, "[%s] : model string=%s\n", __func__, str_model);

	//    priv->g_ctx.debug_i2c_write = debug_i2c_write;

	if (priv->g_ctx.hardware_model == HW_MODEL_UNKNOWN) {
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

#if 0
  err = of_property_read_string(node, "fsync-mode", &str_value);
  if (err < 0)
  {
    dev_err(dev, "[%s] : No fsync-mode found\n", __func__);
    goto error;
  }

  if (!strcmp(str_value, "true"))
  {
    priv->fsync_mode = true;
  }
  else
  {
    priv->fsync_mode = false;
  }
#endif

	if (enable_distortion_correction == 0xCAFE) {
		// if not set kernel param, read device tree param
		err = of_property_read_string(node, "distortion-correction",
					      &str_value);
		if (err < 0) {
			dev_err(dev,
				"[%s] : No distortion-correction found. set enable_distortion-correction = true\n",
				__func__);
		} else {
			if (!strcmp(str_value, "true")) {
				enable_distortion_correction = 1;
			} else {
				enable_distortion_correction = 0;
			}
		}
	}

	priv->distortion_correction = enable_distortion_correction != 0 ? true :
									  false;

	if (enable_auto_exposure == 0xCAFE) {
		// if not set kernel param, read device tree param
		err = of_property_read_string(node, "auto-exposure",
					      &str_value);
		if (err < 0) {
			dev_err(dev,
				"[%s] : No auto-exposure mode found. set enable_auto_exposure = true\n",
				__func__);
		} else {
			if (!strcmp(str_value, "true")) {
				enable_auto_exposure = 1;
			} else {
				enable_auto_exposure = 0;
			}
		}
	}

	priv->auto_exposure = enable_auto_exposure != 0 ? true : false;

	mode_node = of_get_child_by_name(node, "mode0");

	dev_dbg(dev,
		"[%s] : node->full_name = %s, mode_node->full_name = %s.\n",
		__func__, node->full_name, mode_node->full_name);

#ifdef USE_EMBEDDED_METADAT_HEIGHT_IN_DTB
	/* check embedded_metadata_height */

	err = of_property_read_string(mode_node, "embedded_metadata_height",
				      &str_value);
	dev_info(dev, "[%s] : err = %d, embedded_metadata_height = %s\n",
		 __func__, err, str_value);

	err = kstrtoul(str_value, 10, &u_value);
	embedded_metadata_height = u_value;
	if (err) {
		embedded_metadata_height = 0;
	} else {
		embedded_metadata_height = u_value;
	}

	switch (embedded_metadata_height) {
	case 0:
		priv->enable_embedded_data = DISABLE_BOTH_EMBEDDED_DATA;
		break;
	case 1:
		priv->enable_embedded_data = ENABLE_FRONT_EMBEDDED_DATA;
		break;
	case 14:
		priv->enable_embedded_data = ENABLE_REAR_EMBEDDED_DATA;
		break;
	case 15:
		priv->enable_embedded_data = ENABLE_BOTH_EMBEDDED_DATA;
		break;
	default:
		priv->enable_embedded_data = DISABLE_BOTH_EMBEDDED_DATA;
	}
#else // ifdef USE_EMBEDDED_METADAT_HEIGHT_IN_DTB

	priv->enable_embedded_data =
		DISABLE_BOTH_EMBEDDED_DATA; // priv->enable_embedded_data is defined by camera sensor image mode

#endif // ifdef USE_EMBEDDED_METADAT_HEIGHT_IN_DTB

	// for Ser node

	ser_node = of_parse_phandle(node, "nvidia,gmsl-ser-device", 0);

	if (ser_node == NULL) {
		dev_err(dev, "[%s] : Missing %s handle\n", __func__,
			"nvidia,gmsl-ser-device");
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
		dev_err(dev, "[%s] : Missing Serializer Dev Handle\n",
			__func__);
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
		dev_err(dev, "[%s] : Missing %s handle\n", __func__,
			"nvidia,gmsl-dser-device");
		goto error;
	}

	dser_i2c = of_find_i2c_device_by_node(dser_node);
	of_node_put(dser_node);
	if (dser_i2c == NULL) {
		dev_err(dev, "[%s] : Missing deserializer dev handle\n",
			__func__);
		goto error;
	}

	if (dser_i2c->dev.driver == NULL) {
		dev_err(dev, "[%s] : Missing deserializer driver\n", __func__);
		goto error;
	}

	priv->dser_dev = &dser_i2c->dev;

	if ((priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_XAVIER) ||
	    (priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_ORIN)) {
		// for FPGA node

		fpga_node = of_parse_phandle(node, "nvidia,fpga-device", 0);

		if (fpga_node == NULL) {
			dev_err(dev, "[%s] : Missing %s handle\n", __func__,
				"nvidia,fpga-device");
			goto error;
		}

		err = of_property_read_u32(fpga_node, "reg",
					   &priv->g_ctx.sdev_fpga_reg);

		if (err < 0) {
			dev_err(dev, "[%s] : FPGA reg not found\n", __func__);
			goto error;
		}

		fpga_i2c = of_find_i2c_device_by_node(fpga_node);

		of_node_put(fpga_node);

		if (fpga_i2c == NULL) {
			dev_err(dev, "[%s] : Missing FPGA Dev Handle\n",
				__func__);
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
		dev_err(dev, "[%s] : Missing GMSL-Link device node\n",
			__func__);
		err = -EINVAL;
		goto error;
	}

	err = of_property_read_string(gmsl, "dst-csi-port", &str_value);
	if (err < 0) {
		dev_err(dev, "[%s] : No dst-csi-port found\n", __func__);
		goto error;
	}

	priv->g_ctx.dst_csi_port = (!strcmp(str_value, "a")) ? GMSL_CSI_PORT_A :
							       GMSL_CSI_PORT_B;

	err = of_property_read_string(gmsl, "src-csi-port", &str_value);
	if (err < 0) {
		dev_err(dev, "[%s] : No src-csi-port found\n", __func__);
		goto error;
	}

	priv->g_ctx.src_csi_port = (!strcmp(str_value, "a")) ? GMSL_CSI_PORT_A :
							       GMSL_CSI_PORT_B;

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
					      GMSL_SERDES_CSI_LINK_A :
					      GMSL_SERDES_CSI_LINK_B;

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
		of_property_read_string_index(gmsl, "streams", i,
					      &str_value1[i]);
		if (!str_value1[i]) {
			dev_err(dev, "[%s] : Invalid Stream Info\n", __func__);
			goto error;
		}

		if (!strcmp(str_value1[i], "raw12")) {
			priv->g_ctx.streams[i].st_data_type =
				GMSL_CSI_DT_RAW_12;
		} else if (!strcmp(str_value1[i], "yuv8")) {
			priv->g_ctx.streams[i].st_data_type = GMSL_CSI_DT_YUV_8;
		} else if (!strcmp(str_value1[i], "embed")) {
			priv->g_ctx.streams[i].st_data_type = GMSL_CSI_DT_EMBED;
		} else if (!strcmp(str_value1[i], "ued-u1")) {
			priv->g_ctx.streams[i].st_data_type =
				GMSL_CSI_DT_UED_U1;
		} else {
			dev_err(dev, "[%s] : Invalid stream data type.\n",
				__func__);
			goto error;
		}
	}

	priv->g_ctx.s_dev = dev;

	return NO_ERROR;

error:
	dev_err(dev, "[%s] : Board Setup failed.\n", __func__);
	return err;
}

static int tier4_isx021_probe(struct i2c_client *client,
			      const struct i2c_device_id *id)
{
	int err = 0;
	char *path = FIRMWARE_BIN_FILE;
	void *firmware_buffer;

	struct device *dev = &client->dev;
	struct device_node *node = dev->of_node;
	struct tegracam_device *tc_dev;
	struct tier4_isx021 *priv;

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 65)
	loff_t size;
#endif

	tier4_isx021_sensor_mutex_lock();

	//  dev_info(dev, "[%s] : Probing V4L2 Sensor.\n", __func__);

	if (!IS_ENABLED(CONFIG_OF) || !node) {
		err = -EINVAL;
		goto errret;
	}

	wst_priv[camera_channel_count].p_client = NULL;
	wst_priv[camera_channel_count].p_priv = NULL;
	wst_priv[camera_channel_count].p_tc_dev = NULL;
	wst_priv[camera_channel_count].sensor_ser_shutdown = false;
	wst_priv[camera_channel_count].des_shutdown = false;
	wst_priv[camera_channel_count].running = false;

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
	atomic_set(&priv->test_hw_fault, 0);

	tc_dev->dev = dev;

	strncpy(tc_dev->name, "isx021", sizeof(tc_dev->name));

	tc_dev->dev_regmap_config = &tier4_sensor_regmap_config;
	tc_dev->sensor_ops = &tier4_isx021_common_ops;
	tc_dev->v4l2sd_internal_ops = &tier4_isx021_subdev_internal_ops;
	tc_dev->tcctrl_ops = &tier4_isx021_ctrl_ops;

	firmware_buffer =
		devm_kzalloc(dev, sizeof(u16) * MAX_NUM_OF_REG, GFP_KERNEL);

	if (!firmware_buffer) {
		dev_err(dev, "[%s] : Allocating firmware buffer failed.\n",
			__func__);
		err = -ENOMEM;
		goto errret;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 65)
	err = kernel_read_file_from_path(
		path, 0, &firmware_buffer, INT_MAX, NULL,
		READING_POLICY); // err is errono and number of bytes actually read
	if (!err) {
		dev_err(dev, "Loading %s failed with error %d\n", path, err);
#ifdef USE_FIRMWARE
		goto errret;
#endif
	}
#else
	err = kernel_read_file_from_path(
		path, &firmware_buffer, &size, INT_MAX,
		READING_FIRMWARE); // size : number of bytes actually read
	if (err) {
		dev_err(dev, "Loading %s failed with error %d\n", path, err);
#ifdef USE_FIRMWARE
		goto errret;
#endif
	}
#endif

	priv->firmware_buffer = (u16 *)firmware_buffer;

	err = tegracam_device_register(tc_dev);

	if (err) {
		dev_err(dev,
			"[%s] :  Tegra Camera Driver Registration failed.\n",
			__func__);
		goto errret;
	}

	priv->tc_dev = tc_dev;
	priv->s_data = tc_dev->s_data;
	priv->subdev = &tc_dev->s_data->subdev;

	tegracam_set_privdata(tc_dev, (void *)priv);

	err = tier4_isx021_board_setup(priv);

	if (err) {
		dev_err(dev, "[%s] : Board Setup failed.\n", __func__);
		goto err_tegracam_unreg;
	}

	priv->trigger_mode = trigger_mode; // trigger_mode per camaera

	/* Pair sensor to serializer dev */
	err = tier4_max9295_sdev_pair(priv->ser_dev, &priv->g_ctx);

	if (err) {
		dev_err(&client->dev, "[%s] : GMSL Ser Pairing failed.\n",
			__func__);
		goto err_tegracam_unreg;
	}

	/* Register sensor to deserializer dev */

	//dev_info(dev, "[%s]: Before tier4_max9296_sdev_register() : priv->g_ctx.serdes_csi_link = %u\n" , __func__,  priv->g_ctx.serdes_csi_link );

	//asm("dmb sy");

	err = tier4_max9296_sdev_register(priv->dser_dev, &priv->g_ctx);

	if (err) {
		dev_err(&client->dev,
			"[%s] : Failed to register GMSL Deserializer.\n",
			__func__);
		goto err_max9295_unpair;
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
		dev_err(&client->dev, "[%s] : Setup for  GMSL Serdes failed.\n",
			__func__);
		goto err_max9296_unreg;
	}

	err = tegracam_v4l2subdev_register(tc_dev, true);
	if (err) {
		dev_err(dev,
			"[%s] : Tegra Camera Subdev Registration failed.\n",
			__func__);
		goto err_max9296_unreg;
	}

	tier4_isx021_sensor_mutex_unlock();

	err = tier4_isx021_write_reg(tc_dev->s_data, TIER4_ISX021_REG_90_ADDR,
				     0x06);

	err = tier4_isx021_set_response_mode(priv);
	if (err) {
		dev_warn(dev, "[%s] : Transition to response mode failed.\n",
			 __func__);
		goto err_tegracam_v4l2_unreg;
	}

	device_create_file(&client->dev, &dev_attr_test_hw_fault);
	tier4_max9295_set_v4l2_subdev(priv->ser_dev, priv->subdev);

	dev_info(&client->dev, "Detected ISX021 sensor.\n");

	wst_priv[camera_channel_count].p_client = client;
	wst_priv[camera_channel_count].p_priv = priv;
	wst_priv[camera_channel_count].p_tc_dev = tc_dev;

	camera_channel_count++;

	tier4_isx021_sensor_mutex_unlock();

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 65)) & 0

	dev_info(&client->dev, "Detected ISX021 sensor\n");
	return NO_ERROR;
#else
	return err;
#endif

err_tegracam_v4l2_unreg:
	tegracam_v4l2subdev_unregister(priv->tc_dev);
err_max9296_unreg:
	tier4_max9296_sdev_unregister(priv->dser_dev, &client->dev);
err_max9295_unpair:
	tier4_max9295_sdev_unpair(priv->ser_dev, &client->dev);
err_tegracam_unreg:
	tegracam_device_unregister(priv->tc_dev);
errret:
	tier4_isx021_sensor_mutex_unlock();

	dev_err(&client->dev, "Detection for ISX021 sensor failed.\n");

	return err;
}

static void tier4_isx021_shutdown(struct i2c_client *client);

static int tier4_isx021_remove(struct i2c_client *client)
{
	struct camera_common_data *s_data = to_camera_common_data(&client->dev);
	struct tier4_isx021 *priv = (struct tier4_isx021 *)s_data->priv;

	tier4_max9295_unset_v4l2_subdev(priv->ser_dev);
	device_remove_file(&client->dev, &dev_attr_test_hw_fault);

	tier4_isx021_shutdown(client);

	tier4_isx021_gmsl_serdes_reset(priv);

	tier4_max9296_sdev_unregister(priv->dser_dev, &client->dev);
	tier4_max9295_sdev_unpair(priv->ser_dev, &client->dev);

	tegracam_v4l2subdev_unregister(priv->tc_dev);

	tegracam_device_unregister(priv->tc_dev);

	return NO_ERROR;
}

static bool tier4_isx021_is_sensor_ser_shutdown(int nport)
{
	if (wst_priv[nport].sensor_ser_shutdown) {
		return true;
	}
	return false;
}

static bool tier4_isx021_is_des_shut_down(int nport)
{
	if (wst_priv[nport].des_shutdown) {
		return true;
	}
	return false;
}

static void tier4_isx021_set_sensor_ser_shutdown(int nport, bool val)
{
	wst_priv[nport].sensor_ser_shutdown = val;
}

static void tier4_isx021_set_des_shutdown(int nport, bool val)
{
	wst_priv[nport].des_shutdown = val;
}

static bool tier4_isx021_is_current_i2c_client(struct i2c_client *client,
					       int nport)
{
	if (client == wst_priv[nport].p_client) {
		return true;
	}
	return false;
}

/**
*  The shutdown routine is needed only when you use ROcube and you would like to reboot command on Linux  *
*  if not so, you don't need shutdown function                                                            *
*/

static void tier4_isx021_shutdown(struct i2c_client *client)
{
	struct tier4_isx021 *priv = NULL;
	int i;

	tier4_isx021_sensor_mutex_unlock();

	mutex_lock(&tier4_isx021_lock);

	if (!client) {
		goto error_exit;
	}

	for (i = 0; i < camera_channel_count; i++) {
		if (tier4_isx021_is_current_i2c_client(client, i)) {
			priv = wst_priv[i].p_priv;

			if (i &
			    0x1) { // Even port number( GMSL B port on a Des : i = port_number -1 )

				if (tier4_isx021_is_camera_connected_to_port(
					    i -
					    1)) { // if a camera connected to another(GMSL A)port on a Des.

					if (tier4_isx021_is_sensor_ser_shutdown(
						    i -
						    1)) { // ISP and Ser on another(GMSL A) port have been

						if (tier4_isx021_is_des_shut_down(
							    i - 1) ==
						    false) { // if Des on another(GMSL A)port is not shutdown yet
							tier4_isx021_set_sensor_ser_shutdown(
								i,
								true); // ISP and Ser will be shut down
							tier4_isx021_set_des_shutdown(
								i,
								true); // Des will be shut down
						} else { // if Des on the another port is already shut down.
							tier4_isx021_set_sensor_ser_shutdown(
								i,
								false); // ISP and Ser will not be shut down
							tier4_isx021_set_des_shutdown(
								i,
								false); // Des will not be shutdown
						}
					} else { // The camera ISP and Ser on another(GMSL A) port
						if (tier4_isx021_is_des_shut_down(
							    i - 1) ==
						    false) { // if Des is not shut down yet.
							tier4_isx021_set_sensor_ser_shutdown(
								i,
								true); // ISP and Ser will be shut down
							tier4_isx021_set_des_shutdown(
								i,
								false); //  The Des won't be shut down.
						} else { // Only Des on another port is already shut down.
							tier4_isx021_set_sensor_ser_shutdown(
								i,
								false); // ISP and Ser will not be shut down
							tier4_isx021_set_des_shutdown(
								i,
								false); //  Des will not be shut down.
						}
					}
				} else { // a camera is connected to only (GMSL B) port on Des.
					tier4_isx021_set_sensor_ser_shutdown(
						i,
						true); // ISP and Ser will be shut down
					tier4_isx021_set_des_shutdown(
						i,
						true); // The Des won't be shut down.
				}
			} else { // if (  i & 0x1 ) == 0 :  Camera is connected to Odd port number. ( GMSL A potr on a Des : i = port_number -1
				// )

				if (tier4_isx021_is_camera_connected_to_port(
					    i +
					    1)) { // Another camera is connected to
					// another(GMSL B) port on the Des
					if (tier4_isx021_is_sensor_ser_shutdown(
						    i +
						    1)) { // if the ISP and Ser on another port
						// are already shut down
						if (tier4_isx021_is_des_shut_down(
							    i + 1) ==
						    false) { // if Des is not shut down yet.
							tier4_isx021_set_sensor_ser_shutdown(
								i,
								true); // ISP and Ser will be shut down
							tier4_isx021_set_des_shutdown(
								i,
								false); //  The Des will not be shut down.
						} else { // Des is already shut down. This is Error case.
							tier4_isx021_set_sensor_ser_shutdown(
								i,
								false); // ISP and Ser will not be shut down
							tier4_isx021_set_des_shutdown(
								i,
								false); //  The Des will not be shut down.
						}
					} else { // The ISP and Ser on another(GMSL B) port
						// are not shut down yet.
						if (tier4_isx021_is_des_shut_down(
							    i + 1) ==
						    false) { // if Des on another(GMSL B) port is not shut down yet.
							tier4_isx021_set_sensor_ser_shutdown(
								i,
								true); // ISP and Ser will be shut down
							tier4_isx021_set_des_shutdown(
								i,
								false); //  The Des will not be shut down.
						} else { // Only Des on another(GMSL B) port is already shut down.
							//  This is Error case.
							tier4_isx021_set_sensor_ser_shutdown(
								i,
								false); // ISP and Ser will not be shut down
							tier4_isx021_set_des_shutdown(
								i,
								false); //  The Des will not be shut down.
						}
					}
				} else {
					tier4_isx021_set_sensor_ser_shutdown(
						i,
						true); // ISP and Ser will be shut down
					tier4_isx021_set_des_shutdown(
						i,
						true); //  The Des will be shut down.
				}
			} //  if ( i & 0x1 )
			//         break;

			if (tier4_isx021_is_sensor_ser_shutdown(i)) {
				// Reset camera sensor
				tier4_max9295_control_sensor_power_seq(
					priv->ser_dev, SENSOR_ID_ISX021, false);
				// S/W Reset max9295
				tier4_max9295_reset_control(priv->ser_dev);
			}

			if (tier4_isx021_is_des_shut_down(i)) {
				// S/W Reset max9296
				tier4_max9296_reset_control(priv->dser_dev,
							    &client->dev, true);
			}

			if (priv == NULL || i >= camera_channel_count) {
				mutex_unlock(&tier4_isx021_lock);
				tier4_isx021_sensor_mutex_unlock();
				return;
			}
		}
	} // for loop

error_exit:

	mutex_unlock(&tier4_isx021_lock);

	tier4_isx021_sensor_mutex_unlock();
}

static const struct i2c_device_id tier4_isx021_id[] = { { "tier4_isx021", 0 },
							{} };

MODULE_DEVICE_TABLE(i2c, tier4_isx021_id);

static struct i2c_driver tier4_isx021_i2c_driver = {
    .driver = {
        .name           = "tier4_isx021",
        .owner          = THIS_MODULE,
        .of_match_table = of_match_ptr(tier4_isx021_of_match),
    },
    .probe      = tier4_isx021_probe,
    .remove     = tier4_isx021_remove,
    .shutdown   = tier4_isx021_shutdown,
    .id_table   = tier4_isx021_id,
};

static int __init tier4_isx021_init(void)
{
	mutex_init(&tier4_sensor_lock__);
	mutex_init(&tier4_isx021_lock);

	//  printk(KERN_INFO "TIERIV Automotive HDR Camera driver : %s\n", BUILD_STAMP);

	return i2c_add_driver(&tier4_isx021_i2c_driver);
}

static void __exit tier4_isx021_exit(void)
{
	mutex_destroy(&tier4_sensor_lock__);
	mutex_destroy(&tier4_isx021_lock);

	printk(KERN_INFO "[%s]: Exit TIERIV Automotive HDR Camera driver.\n",
	       __func__);

	i2c_del_driver(&tier4_isx021_i2c_driver);
}

module_init(tier4_isx021_init);
module_exit(tier4_isx021_exit);

MODULE_DESCRIPTION("TIERIV Automotive HDR Camera driver");
MODULE_AUTHOR("K.Iwasaki");
MODULE_AUTHOR("Y.Fujii");
MODULE_LICENSE("GPL v2");
