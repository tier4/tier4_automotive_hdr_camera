/*
 * tier4_imx728.c - imx728 sensor driver
 *
 * Copyright (c) 2022-2023, TIER IV Inc. All rights reserved.
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

#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <media/tegracam_core.h>

#include "max20089.h"
#include "tier4-fpga.h"
#include "tier4-gmsl-link.h"
#include "tier4-gw5300.h"
#include "tier4-hw-model.h"
#include "tier4-isx021-extern.h"
#include "tier4-max9295.h"
#include "tier4-max9296.h"

MODULE_SOFTDEP("pre: max20089");
MODULE_SOFTDEP("pre: tier4_fpga");
MODULE_SOFTDEP("pre: tier4_max9296");
MODULE_SOFTDEP("pre: tier4_max9295");
MODULE_SOFTDEP("pre: tier4_gw5300");
MODULE_SOFTDEP("pre: tier4_isx021");
MODULE_SOFTDEP("pre: tier4_imx490");

#define USE_DISTORTION_CORRECTION  1

// Register Address

#define IMX728_DEFAULT_FRAME_LENGTH (2350)

#define BIT_SHIFT_8 8
#define BIT_SHIFT_16 16
#define MASK_1_BIT 0x1
#define MASK_4_BIT 0xF
#define MASK_8_BIT 0xFF

#define POWER_ON 1
#define POWER_OFF 0

#define NO_ERROR 0
//#define NO_C3_CAMERA (-728)

//#define TIME_120_MILISEC 120000
//#define TIME_121_MILISEC 121000

#define ISP_PRIM_SLAVE_ADDR 0x6D

//#define TIER4_C3_CAMERA 1

#define MAX_NUM_CAMERA 8

//#define FSYNC_FREQ_HZ 10

enum
{
  IMX728_MODE_3840x2160_CROP_20FPS,
  IMX728_MODE_START_STREAM,
  IMX728_MODE_STOP_STREAM,
};

static const int tier4_imx728_20fps[] = {
  20,
};
static const struct camera_common_frmfmt tier4_imx728_frmfmt[] = {
  { { 3840, 2160 }, tier4_imx728_20fps, 1, 0, IMX728_MODE_3840x2160_CROP_20FPS },
// { { 3840, 2163 }, tier4_imx728_20fps, 1, 0, IMX728_MODE_3840x2160_CROP_20FPS },
};

const struct of_device_id tier4_imx728_of_match[] = {
  {
      .compatible = "nvidia,tier4_imx728",
  },
  {},
};

MODULE_DEVICE_TABLE(of, tier4_imx728_of_match);

// If you add new ioctl VIDIOC_S_EXT_CTRLS function, please add new CID to the following table.
// and define the CID number in  nvidia/include/media/tegra-v4l2-camera.h

static const u32 ctrl_cid_list[] = {
  TEGRA_CAMERA_CID_GAIN,       TEGRA_CAMERA_CID_EXPOSURE, TEGRA_CAMERA_CID_EXPOSURE_SHORT,
  TEGRA_CAMERA_CID_FRAME_RATE, TEGRA_CAMERA_CID_HDR_EN,
  //  TEGRA_CAMERA_CID_DISTORTION_CORRECTION,
};

struct tier4_imx728
{
  struct i2c_client *i2c_client;
  const struct i2c_device_id *id;
  struct v4l2_subdev *subdev;
  struct device *ser_dev;
  struct device *dser_dev;
  struct device *isp_dev;
  struct tier4_gmsl_link_ctx g_ctx;
  u32 frame_length;
  struct camera_common_data *s_data;
  struct tegracam_device *tc_dev;
  int trigger_mode;
  bool distortion_correction;
  bool last_distortion_correction;
  bool inhibit_fpga_access;
  struct device *fpga_dev;
  struct device *cam_power_protect_dev;
};

static const struct regmap_config tier4_sensor_regmap_config = {
  .reg_bits = 16,
  .val_bits = 8,
  .cache_type = REGCACHE_RBTREE,
};

struct st_priv
{
  struct i2c_client *p_client;
  struct tier4_imx728 *p_priv;
  struct tegracam_device *p_tc_dev;
  bool isp_ser_shutdown;
  bool des_shutdown;
  bool running;
};

static struct st_priv wst_priv[MAX_NUM_CAMERA];

static struct mutex tier4_imx728_lock;

static int camera_channel_count = 0;

// --- module parameter ---

static int trigger_mode;
static int fsync_mfp = 0;
static int enable_distortion_correction = 1;
static int enable_auto_exposure = 0xCAFE;


#define IMX728_MIN_EXPOSURE_TIME 11000  // 11 milisecond
#define IMX728_MAX_EXPOSURE_TIME 33000  // 33 milisecond

static int shutter_time_min = IMX728_MIN_EXPOSURE_TIME;
static int shutter_time_max = IMX728_MAX_EXPOSURE_TIME;

module_param(trigger_mode, int, 0644);
module_param(shutter_time_min, int, S_IRUGO | S_IWUSR);
module_param(shutter_time_max, int, S_IRUGO | S_IWUSR);


module_param(fsync_mfp, int, S_IRUGO | S_IWUSR);
module_param(enable_distortion_correction, int, S_IRUGO | S_IWUSR);
module_param(enable_auto_exposure, int, S_IRUGO | S_IWUSR);

// ------------------------
static char upper(char c)
{
  if ('a' <= c && c <= 'z')
  {
    c = c - ('a' - 'A');
  }
  return c;
}

static void to_upper_string(char *out, const char *in)
{
  int i;

  i = 0;
  while (in[i] != '\0')
  {
    out[i] = upper(in[i]);
    i++;
  }
}

static inline int tier4_imx728_read_reg(struct camera_common_data *s_data, u16 addr, u8 *val)
{
  int err = 0;
  u32 reg_val = 0;

  err = regmap_read(s_data->regmap, addr, &reg_val);

  if (err)
  {
    dev_err(s_data->dev, "[%s] : I2C read failed, address=0x%x\n", __func__, addr);
  }
  else
  {
    *val = reg_val & 0xFF;
  }
  return err;
}

static int tier4_imx728_write_reg(struct camera_common_data *s_data, u16 addr, u8 val)
{
  int err = 0;

  err = regmap_write(s_data->regmap, addr, val);

  if (err)
  {
    dev_err(s_data->dev, "[%s] : I2C write failed at x%x=[0x%x]\n", __func__, addr, val);
  }

  return err;
}

// ------------------------------------------------

static int tier4_imx728_set_fsync_trigger_mode(struct tier4_imx728 *priv, int mode)
{
  int err = 0;
  struct device *dev = priv->s_data->dev;
  int des_num = 0;

  if ((priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_XAVIER) ||
      (priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_ORIN))
  {
    priv->inhibit_fpga_access = false;

    dev_info(dev, "[%s] : Set fsync trigger mode.\n", __func__);

    err = tier4_fpga_check_access(priv->fpga_dev);
    if (err)
    {
      priv->inhibit_fpga_access = true;
    }

    if (tier4_fpga_get_fsync_mode() == FPGA_FSYNC_MODE_DISABLE)
    {
      dev_info(dev, "[%s] : Disabling FPGA fsync.\n", __func__);

      err = tier4_fpga_disable_fsync_mode(priv->fpga_dev);
      if (err)
      {
        dev_err(dev, "[%s] : Disabling FPGA fsync failed.\n", __func__);
        if ( !priv->inhibit_fpga_access )
        {
          return err;
        }
      }
    }
    else
    {
      err = tier4_fpga_enable_fsync_mode(priv->fpga_dev);
      if (err)
      {
        dev_err(dev, "[%s] : Enabling FPGA generate fsync failed.\n", __func__);
        if ( !priv->inhibit_fpga_access )
        {
          return err;
        }
      }

      if (tier4_fpga_get_fsync_mode() == FPGA_FSYNC_MODE_AUTO)
      {
        // Auto Trigger Mode
        err = tier4_fpga_set_fsync_auto_trigger(priv->fpga_dev);
        if (err)
        {
          dev_err(dev, "[%s] : Enabling FPGA Fsync Auto Trigger mode failed.\n", __func__);
          if ( !priv->inhibit_fpga_access )
          {
            return err;
          }
        }
        des_num = priv->g_ctx.reg_mux;
        err = tier4_fpga_set_fsync_signal_frequency(priv->fpga_dev, des_num, priv->trigger_mode, SENSOR_ID_IMX728);
        if (err)
        {
          dev_err(dev, "[%s] : Setting the frequency of fsync genrated by FPGA failed.\n", __func__);
          if ( !priv->inhibit_fpga_access )
          {
            return err;
          }
        }
      }
      else if (tier4_fpga_get_fsync_mode() == FPGA_FSYNC_MODE_MANUAL)
      {
        // Manual Trigger Mode
        dev_info(dev, "[%s] : Enabling FPGA Fsync Manual Trigger mode.\n", __func__);

        err = tier4_fpga_set_fsync_manual_trigger(priv->fpga_dev);
        if (err)
        {
          dev_err(dev, "[%s] : Enabling FPGA Fsync Maunal Trigger mode failed.\n", __func__);
          if ( !priv->inhibit_fpga_access )
          {
            return err;
          }
        }
      }
    }
  }

  err = tier4_max9296_setup_gpi(priv->dser_dev, fsync_mfp);

  if (err)
  {
    dev_err(dev, "[%s] :tier4_max9296_setup_gpi() failed\n", __func__);
    return err;
  }

  err = tier4_max9295_setup_gpo(priv->ser_dev);

  if (err)
  {
    dev_err(dev, "[%s] : tier4_max9295_setup_gpo() failed\n", __func__);
    return err;
  }

  //  Power on GW5300 via Max9295 in C3 camera
//  err = tier4_max9295_control_sensor_power_seq(priv->ser_dev, SENSOR_ID_IMX728, POWER_ON );
//  if (err)
//  {
//    dev_err(dev, "[%s] : Power on gw5300 in C3 camaera failed.\n", __func__);
//    return err;
//  }

  err = tier4_gw5300_c3_setup_sensor_mode(priv->isp_dev, mode);
  if (err)
  {
    dev_err(dev, "[%s] : tier4_gw5300_c3_setup_sensor_mode() failed\n", __func__);
    return err;
  }

  return err;
}

static struct mutex serdes_lock__;

static int tier4_imx728_gmsl_serdes_setup(struct tier4_imx728 *priv)
{
  int err = 0;
  int des_err = 0;
  struct device *dev;

  if (!priv || !priv->ser_dev || !priv->dser_dev || !priv->i2c_client)
  {
    return -EINVAL;
  }

  dev = &priv->i2c_client->dev;

  mutex_lock(&serdes_lock__);

  /* For now no separate power on required for serializer device */

  if ((priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_XAVIER) ||
      (priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_ORIN))
  {
    if (priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_ORIN)
    {
      // only RQX-59G with ADLINK GMSL board supports read/write MAX20089
      if (MAX9295_REG_PORT_A == priv->g_ctx.ser_reg)
        max20089_power_on_cam(priv->cam_power_protect_dev, MAX20089_EN_OUT1);
      else if (MAX9295_REG_PORT_B == priv->g_ctx.ser_reg)
        max20089_power_on_cam(priv->cam_power_protect_dev, MAX20089_EN_OUT2);
    }

    // power on deserializer
    tier4_fpga_power_on_deserializer(priv->fpga_dev, priv->g_ctx.reg_mux);
    
    // sleep to wait devices ready
    msleep(100);
  }
  else 
  {
    tier4_max9296_power_on(priv->dser_dev);
  }

  /* setup serdes addressing and control pipeline */


  err = tier4_max9296_setup_link(priv->dser_dev, &priv->i2c_client->dev);

  if (err)
  {
    dev_err(dev, "[%s] : GMSL deserializer link config failed\n", __func__);
    goto error;
  }

  err = tier4_max9295_setup_control(priv->ser_dev);

  if (err)
  {
    dev_err(dev, "[%s] : GMSL serializer setup failed\n", __func__);
    
    /* 
      No "goto error" here, instead, go into tier4_max9296_setup_control()
      proceed even if ser setup failed, to setup deser correctly
      So that if MAX9296 Link B is not found, it will set back to Link A 
    */ 
    // goto error;
  }

  des_err = tier4_max9296_setup_control(priv->dser_dev, &priv->i2c_client->dev);

  if (des_err)
  {
    dev_err(dev, "[%s] : GMSL deserializer : setup failed\n", __func__);
    err = des_err;
  }

error:
  mutex_unlock(&serdes_lock__);
  return err;
}

static void tier4_imx728_gmsl_serdes_reset(struct tier4_imx728 *priv)
{
  mutex_lock(&serdes_lock__);

  /* reset serdes addressing and control pipeline */
  tier4_max9295_reset_control(priv->ser_dev);

  tier4_max9296_reset_control(priv->dser_dev, &priv->i2c_client->dev, true);

  if ((priv->g_ctx.hardware_model != HW_MODEL_ADLINK_ROSCUBE_XAVIER) &&
      (priv->g_ctx.hardware_model != HW_MODEL_ADLINK_ROSCUBE_ORIN))
  {
    tier4_max9296_power_off(priv->dser_dev);
  }

  mutex_unlock(&serdes_lock__);
}

static int tier4_imx728_power_on(struct camera_common_data *s_data)
{
  int err = 0;
  struct camera_common_power_rail *pw = s_data->power;
  struct camera_common_pdata *pdata = s_data->pdata;
  struct device *dev = s_data->dev;

  if (pdata && pdata->power_on)
  {
    err = pdata->power_on(pw);

    if (err)
    {
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

static int tier4_imx728_power_off(struct camera_common_data *s_data)
{
  int err = 0;
  struct camera_common_power_rail *pw = s_data->power;
  struct camera_common_pdata *pdata = s_data->pdata;
  struct device *dev = s_data->dev;

  if (pdata && pdata->power_off)
  {
    err = pdata->power_off(pw);

    if (!err)
    {
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

static int tier4_imx728_power_get(struct tegracam_device *tc_dev)
{
  struct camera_common_power_rail *pw = tc_dev->s_data->power;
  int err = 0;

  pw->state = SWITCH_OFF;

  return err;
}

static int tier4_imx728_power_put(struct tegracam_device *tc_dev)
{
  if (unlikely(!tc_dev->s_data->power))
  {
    return -EFAULT;
  }

  return NO_ERROR;
}

static int tier4_imx728_set_group_hold(struct tegracam_device *tc_dev, bool val)
{
  volatile int err = 0;

  return err;
}

static int tier4_imx728_set_gain(struct tegracam_device *tc_dev, s64 val)
{
  int err = 0;

  //  struct camera_common_data   *s_data     = tc_dev->s_data;

  // struct device *dev = tc_dev->dev;

  return err;
}

/* ------------------------------------------------------------------------- */

static int tier4_imx728_set_frame_rate(struct tegracam_device *tc_dev, s64 val)
{
  int err = 0;

  struct tier4_imx728 *priv = (struct tier4_imx728 *)tegracam_get_privdata(tc_dev);
  //  struct device dev       = tc_dev->dev;

  /* fixed 20fps */
  priv->frame_length = IMX728_DEFAULT_FRAME_LENGTH;

  return err;
}

/* ------------------------------------------------------------------------- */

static int tier4_imx728_set_exposure(struct tegracam_device *tc_dev, s64 val)
{
  int err = 0;

  struct tier4_imx728 *priv = (struct tier4_imx728 *)tegracam_get_privdata(tc_dev);
  tier4_gw5300_c3_set_integration_time_on_aemode(priv->isp_dev, priv->trigger_mode, val, val);

  return err;
}
// --------------------------------------------------------------------------------------
//  Enable Distortion Coreection
// --------------------------------------------------------------------------------------
#if USE_DISTORTION_CORRECTION

static int tier4_imx728_set_distortion_correction(struct tegracam_device *tc_dev, bool val)
{
  int err = 0;
  struct tier4_imx728 *priv = (struct tier4_imx728 *)tegracam_get_privdata(tc_dev);

  if (priv->last_distortion_correction != val)
  {
    dev_info(&priv->i2c_client->dev, "[%s] : Setting distortion correction mode :%s.\n", __func__,val?"True":"False");

    err = tier4_gw5300_c3_set_distortion_correction(priv->isp_dev, val);
    if ( err <= 0)
    {
      dev_info(&priv->i2c_client->dev, "[%s] : Setting distortion correction mode failed.\n", __func__ );
    }
    else
    {
      err = 0;
      priv->last_distortion_correction = val;
    }
  }

  return err;
}
#endif

// --------------------------------------------------------------------------------------
//  If you add new ioctl VIDIOC_S_EXT_CTRLS function,
//  please add the new memeber and the function at the following table.

static struct tegracam_ctrl_ops tier4_imx728_ctrl_ops = {
  .numctrls = ARRAY_SIZE(ctrl_cid_list),
  .ctrl_cid_list = ctrl_cid_list,
  .set_gain = tier4_imx728_set_gain,
  .set_exposure = tier4_imx728_set_exposure,
  .set_exposure_short = tier4_imx728_set_exposure,
  .set_frame_rate = tier4_imx728_set_frame_rate,
  .set_group_hold = tier4_imx728_set_group_hold,
  //  .set_distortion_correction  = tier4_imx728_set_distortion_correction,
};

// --------------------------------------------------------------------------------------

static struct camera_common_pdata *tier4_imx728_parse_dt(struct tegracam_device *tc_dev)
{
  struct device *dev = tc_dev->dev;
  struct device_node *node = dev->of_node;
  struct camera_common_pdata *board_priv_pdata;
  const struct of_device_id *match;
  int err;

  if (!node)
  {
    return NULL;
  }

  match = of_match_device(tier4_imx728_of_match, dev);

  if (!match)
  {
    dev_err(dev, "[%s] : Failed to find matching dt id\n", __func__);
    return NULL;
  }

  board_priv_pdata = devm_kzalloc(dev, sizeof(*board_priv_pdata), GFP_KERNEL);

  err = of_property_read_string(node, "mclk", &board_priv_pdata->mclk_name);

  if (err)
  {
    dev_err(dev, "[%s] : mclk not in DT\n", __func__);
  }

  return board_priv_pdata;
}

/*   tier4_imx728_set_mode() can not be needed. But it remains for compatiblity */

static int tier4_imx728_set_mode(struct tegracam_device *tc_dev)
{
  volatile int err = 0;

  return err;
}

static int tier4_imx728_start_one_streaming(struct tegracam_device *tc_dev)
{
  struct tier4_imx728 *priv = (struct tier4_imx728 *)tegracam_get_privdata(tc_dev);
  struct device *dev = tc_dev->dev;
  int err;

  /* enable serdes streaming */

  err = tier4_max9295_setup_streaming(priv->ser_dev);

  if (err)
  {
    goto exit;
  }

  err = tier4_max9296_setup_streaming(priv->dser_dev, dev);

  if (err)
  {
    dev_err(dev, "[%s] : Setup Streaming failed\n", __func__);
    goto exit;
  }

  dev_info(dev, "[%s] : trigger_mode = %d\n", __func__, trigger_mode);

  priv->trigger_mode = trigger_mode;

  switch (priv->trigger_mode)
  {
    case GW5300_MASTER_MODE_5FPS:
    case GW5300_MASTER_MODE_10FPS:
    case GW5300_MASTER_MODE_10FPS_EBD:
    case GW5300_MASTER_MODE_20FPS:
    case GW5300_MASTER_MODE_20FPS_EBD:
    case GW5300_MASTER_MODE_30FPS:
    case GW5300_MASTER_MODE_30FPS_EBD:
      dev_info(dev, "[%s] : Setting camera sensor to %s\n", __func__, gw5300_mode_name[priv->trigger_mode]);

      err = tier4_gw5300_c3_setup_sensor_mode(priv->isp_dev, priv->trigger_mode);
      if (err)
      {
        dev_err(dev, "[%s] : setting camera sensor to %s failed\n", __func__, gw5300_mode_name[priv->trigger_mode]);
        return err;
      }

      priv->last_distortion_correction = 1;
      break;

    case GW5300_SLAVE_MODE_5FPS:
    case GW5300_SLAVE_MODE_10FPS:
    case GW5300_SLAVE_MODE_10FPS_EBD:
    case GW5300_SLAVE_MODE_20FPS:
    case GW5300_SLAVE_MODE_20FPS_EBD:
    case GW5300_SLAVE_MODE_30FPS:
    case GW5300_SLAVE_MODE_30FPS_EBD:
      dev_info(dev, "[%s] : Setting camera sensor to %s\n", __func__, gw5300_mode_name[priv->trigger_mode]);

      err = tier4_imx728_set_fsync_trigger_mode(priv, priv->trigger_mode);
      if (err)
      {
        dev_err(dev, "[%s] : setting camera sensor to %s failed\n", __func__, gw5300_mode_name[priv->trigger_mode]);
        return err;
      }

      priv->last_distortion_correction = 1;
      break;

    default:  //   case of  trigger_mode  < 0
      dev_err(dev, "[%s] : The camera sensor mode(trigger mode)=%d is invalid.\n", __func__, priv->trigger_mode);

      return err;
  }

  usleep_range(500000, 510000);
  err = tier4_gw5300_c3_set_auto_exposure(priv->isp_dev, enable_auto_exposure);
  if (err <= 0)
  {
    dev_err(dev, "[%s] : Setting Digital Gain to default value failed\n", __func__);
    goto exit;
  }
  else
  {
    err = 0;
  }

#if USE_DISTORTION_CORRECTION

  if (priv->last_distortion_correction != enable_distortion_correction)
  {
    usleep_range(500000, 510000);
    //msleep(900);
  }

  if (enable_distortion_correction == 0xCAFE)
  {
    // if not set kernel param, read device tree param
    if (priv->distortion_correction == false)
    {
      err = tier4_imx728_set_distortion_correction(tc_dev, priv->distortion_correction);

      if (err)
      {
        dev_err(dev, "[%s] : Disabling Distortion Correction  failed\n", __func__);
        goto exit;
      }
      msleep(20);
    }
   }else{
      err = tier4_imx728_set_distortion_correction(tc_dev, enable_distortion_correction==1);
      if (err)
      {
        dev_err(dev, "[%s] : Setup Distortion Correction  failed\n", __func__);
        goto exit;
      }
      msleep(20);
   }

#endif


//  Reset GW5300 via Max9295 in C3 camera
//  err = tier4_max9295_control_sensor_power_seq(priv->ser_dev, SENSOR_ID_IMX728, POWER_ON );
//  if (err)
//  {
//    dev_err(dev, "[%s] : Reset gw5300 in C3 camaera failed.\n", __func__);
//    goto exit;
//  }
//
//  usleep_range(500000, 510000);

  err = tier4_max9296_start_streaming(priv->dser_dev, dev);

  if (err)
  {
    dev_err(dev, "[%s] : tier4_max9296_start_stream() failed\n", __func__);
    return err;
  }

  msleep(1000);
  tier4_gw5300_c3_set_integration_time_on_aemode(priv->isp_dev, priv->trigger_mode, shutter_time_max, shutter_time_min);

//  Reset GW5300 via Max9295 in C3 camera
//  err = tier4_max9295_control_sensor_power_seq(priv->ser_dev, SENSOR_ID_IMX728, POWER_ON );
//  if (err)
//  {
//    dev_err(dev, "[%s] : Reset gw5300 in C3 camaera failed.\n", __func__);
//    goto exit;
//  }

  dev_info(dev, "[%s] :  Camera has started streaming\n", __func__);

  return NO_ERROR;

exit:

  dev_err(dev, "[%s] :  Camera failed to start streaming.\n", __func__);

  return err;
}

static bool tier4_imx728_is_camera_connected_to_port(int nport)
{
  if (wst_priv[nport].p_client)
  {
    return true;
  }
  return false;
}

static bool tier4_imx728_check_null_tc_dev_for_port(int nport)
{
  if (wst_priv[nport].p_tc_dev == NULL)
  {
    return true;
  }
  return false;
}

static bool tier4_imx728_is_camera_running_on_port(int nport)
{
  if (wst_priv[nport].running)
  {
    return true;
  }
  return false;
}

static bool tier4_imx728_is_current_port(struct tier4_imx728 *priv, int nport)
{
  if (priv->i2c_client == wst_priv[nport].p_client)
  {
    return true;
  }
  return false;
}

static void tier4_imx728_set_running_flag(int nport, bool flag)
{
  wst_priv[nport].running = flag;
}

static int tier4_imx728_stop_streaming(struct tegracam_device *tc_dev)
{
  struct device *dev = tc_dev->dev;
  struct tier4_imx728 *priv = (struct tier4_imx728 *)tegracam_get_privdata(tc_dev);
  int i, err = 0;

  mutex_lock(&tier4_imx728_lock);

  for (i = 0; i < MAX_NUM_CAMERA; i++)
  {
    if (tier4_imx728_is_camera_connected_to_port(i))
    {
      if (tier4_imx728_is_current_port(priv, i) && tier4_imx728_is_camera_running_on_port(i))
      {
        /* disable serdes streaming */
        err = tier4_max9296_stop_streaming(priv->dser_dev, dev);
        if (err)
        {
          return err;
        }
        tier4_imx728_set_running_flag(i, false);
        break;
      }
    }
  }

  mutex_unlock(&tier4_imx728_lock);

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

static int tier4_imx728_start_streaming(struct tegracam_device *tc_dev)
{
  int i, err = 0;
  //  struct  tier4_imx728    *next_client_priv;
  struct tier4_imx728 *priv = (struct tier4_imx728 *)tegracam_get_privdata(tc_dev);
  struct device *dev = tc_dev->dev;

  mutex_lock(&tier4_imx728_lock);

  for (i = 0; i < MAX_NUM_CAMERA; i++)
  {
    if (i & 0x1)
    {  // if  i = 1,3,5,7 ( GMSL B port of a Des )

      if (tier4_imx728_is_camera_connected_to_port(i))
      {  //  a  camera is connected to GMSL B portL

        if ((tier4_imx728_is_current_port(priv, i) == true) && (tier4_imx728_is_camera_running_on_port(i) == false))
        {
          err = tier4_imx728_start_one_streaming(wst_priv[i].p_tc_dev);

          if (err)
          {
            dev_err(dev, "[%s] : Failed to start one streaming.\n", __func__);
            goto error_exit;
          }
          //                  wst_priv[i].running  = true;
          tier4_imx728_set_running_flag(i, true);
          break;
        }
      }
    }
    else
    {  // if  i = 0,2,4,6 ( GMSL A side port of a Des0,Des1,Des2 or Des3 )

      if ((tier4_imx728_is_camera_connected_to_port(i) == true) && (tier4_imx728_is_current_port(priv, i) == true))
      {
        if (tier4_imx728_is_camera_connected_to_port(i + 1) == false)
        {  // if another one camera( GMSL B port) is not
           // connected to Des.
          if (tier4_imx728_is_camera_running_on_port(i) == false)
          {  // and if the camera is not running.

            err = tier4_imx728_start_one_streaming(wst_priv[i].p_tc_dev);
            if (err)
            {
              dev_err(dev, "[%s] : Failed to start one streaming for next imx728 client.\n", __func__);
              goto error_exit;
            }
            tier4_imx728_set_running_flag(i, true);
          }
          break;
        }

        //  two cameras are connected to one Des.

        if (tier4_imx728_check_null_tc_dev_for_port(i + 1))
        {  // check if tc_dev is null
          dev_err(dev, "[%s] : wst_priv[%d].p_tc_dev is NULL.\n", __func__, i + 1);
          err = -EINVAL;
          goto error_exit;
        }

        if (tier4_imx728_is_camera_running_on_port(i + 1) == false)
        {
          err = tier4_imx728_start_one_streaming(wst_priv[i + 1].p_tc_dev);
          if (err)
          {
            dev_err(dev, "[%s] : Failed to start one streaming for the next imx728 client.\n", __func__);
            goto error_exit;
          }
          tier4_imx728_set_running_flag(i + 1, true);
          usleep_range(200000, 220000);
          //msleep(200);
          mutex_unlock(&tier4_imx728_lock);
          tier4_imx728_stop_streaming(wst_priv[i + 1].p_tc_dev);
          mutex_lock(&tier4_imx728_lock);
          tier4_imx728_set_running_flag(i + 1, false);
        }

        if (tier4_imx728_is_camera_running_on_port(i) == false)
        {
          err = tier4_imx728_start_one_streaming(wst_priv[i].p_tc_dev);
          if (err)
          {
            dev_err(dev, "[%s] : Failed to start one streaming for current imx728 client.\n", __func__);
            goto error_exit;
          }
          tier4_imx728_set_running_flag(i, true);
        }
      }
    }
  }  // for loop

  err = NO_ERROR;

error_exit:

  mutex_unlock(&tier4_imx728_lock);

  //  tier4_imx728_sensor_mutex_unlock();

  return err;
}

static struct camera_common_sensor_ops tier4_imx728_common_ops = {
  .numfrmfmts = ARRAY_SIZE(tier4_imx728_frmfmt),
  .frmfmt_table = tier4_imx728_frmfmt,
  .power_on = tier4_imx728_power_on,
  .power_off = tier4_imx728_power_off,
  .write_reg = tier4_imx728_write_reg,
  .read_reg = tier4_imx728_read_reg,
  .parse_dt = tier4_imx728_parse_dt,
  .power_get = tier4_imx728_power_get,
  .power_put = tier4_imx728_power_put,
  .set_mode = tier4_imx728_set_mode,
  .start_streaming = tier4_imx728_start_streaming,
  .stop_streaming = tier4_imx728_stop_streaming,
};

static int tier4_imx728_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
  //struct i2c_client *client = v4l2_get_subdevdata(sd);

  return NO_ERROR;
}

static const struct v4l2_subdev_internal_ops tier4_imx728_subdev_internal_ops = {
  .open = tier4_imx728_open,
};

static const char *of_stdout_options;

static int tier4_imx728_board_setup(struct tier4_imx728 *priv)
{
  struct tegracam_device *tc_dev = priv->tc_dev;
  struct device *dev = tc_dev->dev;
  struct device_node *node = dev->of_node;
  struct device_node *ser_node;
  struct i2c_client *ser_i2c = NULL;
  struct device_node *dser_node;
  struct i2c_client *dser_i2c = NULL;
  struct device_node *isp_node;
  struct i2c_client *isp_i2c = NULL;
  struct device_node *fpga_node = NULL;
  struct i2c_client *fpga_i2c = NULL;
  struct device_node *cam_power_protect_node = NULL;
  struct i2c_client *cam_power_protect_i2c = NULL;
  struct device_node *gmsl;
  struct device_node *root_node;
  int value = 0xFFFF;
  const char *str_value;
  const char *str_value1[2];
  int i;
  int err;
  const char *str_model;
  char upper_str_model[64];
  char *str_err;
  char *sub_str_err;

  root_node = of_find_node_opts_by_path("/", &of_stdout_options);

  err = of_property_read_string(root_node, "model", &str_model);

  if (err < 0)
  {
    dev_err(dev, "[%s] : model not found\n", __func__);
    goto error;
  }

  memset(upper_str_model, 0, 64);

  to_upper_string(upper_str_model, str_model);

  priv->g_ctx.hardware_model = HW_MODEL_UNKNOWN;

  str_err = strstr(upper_str_model, STR_DTB_MODEL_NAME_XAVIER);
  if (str_err)
  {
    priv->g_ctx.hardware_model = HW_MODEL_NVIDIA_XAVIER_DEVKIT;
  }

  str_err = strstr(upper_str_model, STR_DTB_MODEL_NAME_ROSCUBE_XAVIER);
  if (str_err)
  {
    priv->g_ctx.hardware_model = HW_MODEL_ADLINK_ROSCUBE_XAVIER;
  }

  str_err = strstr(upper_str_model, STR_DTB_MODEL_NAME_ORIN);

  if (str_err)
  {

    sub_str_err = strstr(upper_str_model, STR_DTB_MODEL_NAME_ROSCUBE_ORIN);

    if (sub_str_err)
    {
      priv->g_ctx.hardware_model = HW_MODEL_ADLINK_ROSCUBE_ORIN;
    }
    else
    {
      priv->g_ctx.hardware_model = HW_MODEL_NVIDIA_ORIN_DEVKIT;
    }
  }


  dev_info(dev, "[%s] : model=%s\n", __func__, str_model);
  dev_info(dev, "[%s] : hardware_model=%d\n", __func__, priv->g_ctx.hardware_model);

  if (priv->g_ctx.hardware_model == HW_MODEL_UNKNOWN)
  {
    dev_err(dev, "[%s] : Unknown Hardware Sysytem !\n", __func__);
    goto error;
  }

  err = of_property_read_u32(node, "reg", &priv->g_ctx.sdev_isp_reg);

  if (err < 0)
  {
    dev_err(dev, "[%s] : def-addr not found\n", __func__);
    goto error;
  }

  if (err < 0)
  {
    dev_err(dev, "[%s] : reg not found\n", __func__);
    goto error;
  }

  err = of_property_read_u32(node, "def-addr", &priv->g_ctx.sdev_isp_def);

  if (err < 0)
  {
    dev_err(dev, "[%s] : def-addr not found\n", __func__);
    goto error;
  }

  err = of_property_read_u32(node, "reg_mux", &priv->g_ctx.reg_mux);

  if (err < 0)
  {
    dev_err(dev, "[%s] : reg_mux not found\n", __func__);
    goto error;
  }

//  err = of_property_read_string(node, "fsync-mode", &str_value);
//
//  if (err < 0)
//  {
//    dev_err(dev, "[%s] : No fsync-mode found\n", __func__);
//    goto error;
//  }
//
//  if (!strcmp(str_value, "true"))
//  {
//    priv->fsync_mode = true;
//  }
//  else
//  {
//    priv->fsync_mode = false;
//  }

  err = of_property_read_string(node, "distortion-correction", &str_value);

  if (err < 0)
  {
    dev_err(dev, "[%s] : No distortion-correction found\n", __func__);
    goto error;
  }

  if (!strcmp(str_value, "true"))
  {
    priv->distortion_correction = true;
  }
  else
  {
    priv->distortion_correction = false;
  }

  if (enable_auto_exposure == 0xCAFE)
  {
    err = of_property_read_string(node, "auto-exposure", &str_value);

    if (err < 0)
    {
      dev_err(dev, "[%s] : Inavlid Exposure mode.\n", __func__);
      goto error;
    }

    enable_auto_exposure = !strcmp(str_value, "true");
  } else {
    dev_info(dev, "[%s] : Parameter[enable_auto_exposure] = %d.\n", __func__, enable_auto_exposure);
  }

#if 0
    priv->g_ctx.fpga_generate_fsync = false;

    if (( priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_XAVIER ) ||
        ( priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_ORIN )) {

        err = of_property_read_string(node, "fpga-generate-fsync", &str_value);

        if ( err < 0) {
            if ( err == -EINVAL ) {
                dev_info(dev, "[%s] : Parameter of fpga-generate-fsync does not exist.\n", __func__);
            } else {
                dev_err(dev, "[%s]  : Parameter of fpga-generate-fsync  is invalid .\n", __func__);
                goto error;
            }
        } else {
            if (!strcmp(str_value, "true")) {
                priv->g_ctx.fpga_generate_fsync = true;
            }
        }
    }
#endif

  // For Ser node
  ser_node = of_parse_phandle(node, "nvidia,gmsl-ser-device", 0);

  if (ser_node == NULL)
  {
    dev_err(dev, "[%s] : Missing %s handle\n", __func__, "nvidia,gmsl-ser-device");
    goto error;
  }

  err = of_property_read_u32(ser_node, "reg", &priv->g_ctx.ser_reg);

  if (err < 0)
  {
    dev_err(dev, "[%s] : Serializer reg not found\n", __func__);
    goto error;
  }

  ser_i2c = of_find_i2c_device_by_node(ser_node);

  of_node_put(ser_node);

  if (ser_i2c == NULL)
  {
    dev_err(dev, "[%s] : Missing Serializer Dev Handle\n", __func__);
    goto error;
  }
  if (ser_i2c->dev.driver == NULL)
  {
    dev_err(dev, "[%s] : Missing serializer driver\n", __func__);
    goto error;
  }

  priv->ser_dev = &ser_i2c->dev;

  // For ISP node

  isp_node = of_parse_phandle(node, "nvidia,isp-device", 0);

  if (isp_node == NULL)
  {
    dev_err(dev, "[%s] : Missing %s handle\n", __func__, "nvidia,isp-device");
    goto error;
  }

  err = of_property_read_u32(isp_node, "reg", &priv->g_ctx.sdev_isp_reg);

  if (err < 0)
  {
    dev_err(dev, "[%s] : ISP reg not found\n", __func__);
    goto error;
  }

  isp_i2c = of_find_i2c_device_by_node(isp_node);

  of_node_put(isp_node);

  if (isp_i2c == NULL)
  {
    dev_err(dev, "[%s] : Missing ISP Dev Handle\n", __func__);
    goto error;
  }
  if (isp_i2c->dev.driver == NULL)
  {
    dev_err(dev, "[%s] : Missing ISP driver\n", __func__);
    goto error;
  }

  priv->isp_dev = &isp_i2c->dev;

  err = tier4_gw5300_prim_slave_addr(&priv->g_ctx);
  if (err)
  {
    dev_err(dev, "[%s] : ISP Prim slave address is unavailable, the default address(0x6D) is applied.\n", __func__);
    priv->g_ctx.sdev_isp_def = ISP_PRIM_SLAVE_ADDR;
  }

  priv->g_ctx.sdev_reg = priv->g_ctx.sdev_isp_reg;
  priv->g_ctx.sdev_def = priv->g_ctx.sdev_isp_def;

  // For Dser node

  dser_node = of_parse_phandle(node, "nvidia,gmsl-dser-device", 0);

  if (dser_node == NULL)
  {
    dev_err(dev, "[%s] : Missing %s handle\n", __func__, "nvidia,gmsl-dser-device");
    goto error;
  }

  dser_i2c = of_find_i2c_device_by_node(dser_node);

  of_node_put(dser_node);

  if (dser_i2c == NULL)
  {
    dev_err(dev, "[%s] : Missing deserializer dev handle\n", __func__);
    goto error;
  }

  if (dser_i2c->dev.driver == NULL)
  {
    dev_err(dev, "[%s] : Missing deserializer driver\n", __func__);
    goto error;
  }

  priv->dser_dev = &dser_i2c->dev;

  if ((priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_XAVIER) ||
      (priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_ORIN))
  {
    // for FPGA node

    fpga_node = of_parse_phandle(node, "nvidia,fpga-device", 0);

    if (fpga_node == NULL)
    {
      dev_err(dev, "[%s] : Missing %s handle\n", __func__, "nvidia,fpga-device");
      goto error;
    }

    err = of_property_read_u32(fpga_node, "reg", &priv->g_ctx.sdev_fpga_reg);

    if (err < 0)
    {
      dev_err(dev, "[%s] : FPGA reg not found\n", __func__);
      goto error;
    }

    fpga_i2c = of_find_i2c_device_by_node(fpga_node);

    of_node_put(fpga_node);

    if (fpga_i2c == NULL)
    {
      dev_err(dev, "[%s] : Missing FPGA Dev Handle\n", __func__);
      goto error;
    }
    if (fpga_i2c->dev.driver == NULL)
    {
      dev_err(dev, "[%s] : Missing FPGA driver\n", __func__);
      goto error;
    }

    priv->fpga_dev = &fpga_i2c->dev;
  }

  if (priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_ORIN)
  {
    // CAM Power Protector, only for RQX-59G with ADLINK GMSL board

    cam_power_protect_node = of_parse_phandle(node, "nvidia,cam-power-protector", 0);
    if (cam_power_protect_node == NULL)
    {
      dev_err(dev, "[%s] : Missing %s handle\n", __func__, "nvidia,cam-power-protector");
      goto error;
    }

    cam_power_protect_i2c = of_find_i2c_device_by_node(cam_power_protect_node);
    of_node_put(cam_power_protect_node);
    if (cam_power_protect_i2c == NULL)
    {
      dev_err(dev, "[%s] : Missing CAM Power Protector Handle\n", __func__);
      goto error;
    }

    if (cam_power_protect_i2c->dev.driver == NULL)
    {
      dev_err(dev, "[%s] : Missing CAM Power Protector driver\n", __func__);
      goto error;
    }

    priv->cam_power_protect_dev = &cam_power_protect_i2c->dev;
  }

  /* populate g_ctx from DT */

  gmsl = of_get_child_by_name(node, "gmsl-link");

  if (gmsl == NULL)
  {
    dev_err(dev, "[%s] : Missing GMSL-Link device node\n", __func__);
    err = -EINVAL;
    goto error;
  }

  err = of_property_read_string(gmsl, "dst-csi-port", &str_value);

  if (err < 0)
  {
    dev_err(dev, "[%s] : No dst-csi-port found\n", __func__);
    goto error;
  }

  priv->g_ctx.dst_csi_port = (!strcmp(str_value, "a")) ? GMSL_CSI_PORT_A : GMSL_CSI_PORT_B;

  err = of_property_read_string(gmsl, "src-csi-port", &str_value);

  if (err < 0)
  {
    dev_err(dev, "[%s] : No src-csi-port found\n", __func__);
    goto error;
  }

  priv->g_ctx.src_csi_port = (!strcmp(str_value, "a")) ? GMSL_CSI_PORT_A : GMSL_CSI_PORT_B;

  err = of_property_read_string(gmsl, "csi-mode", &str_value);

  if (err < 0)
  {
    dev_err(dev, "[%s] : No csi-mode found\n", __func__);
    goto error;
  }

  if (!strcmp(str_value, "1x4"))
  {
    priv->g_ctx.csi_mode = GMSL_CSI_1X4_MODE;
  }
  else if (!strcmp(str_value, "2x4"))
  {
    priv->g_ctx.csi_mode = GMSL_CSI_2X4_MODE;
  }
  else if (!strcmp(str_value, "4x2"))
  {
    priv->g_ctx.csi_mode = GMSL_CSI_4X2_MODE;
  }
  else if (!strcmp(str_value, "2x2"))
  {
    priv->g_ctx.csi_mode = GMSL_CSI_2X2_MODE;
  }
  else
  {
    dev_err(dev, "[%s] :Invalid csi-mode\n", __func__);
    goto error;
  }

  err = of_property_read_string(gmsl, "serdes-csi-link", &str_value);

  if (err < 0)
  {
    dev_err(dev, "[%s] : No serdes-csi-link found\n", __func__);
    goto error;
  }

  priv->g_ctx.serdes_csi_link = (!strcmp(str_value, "a")) ? GMSL_SERDES_CSI_LINK_A : GMSL_SERDES_CSI_LINK_B;

  err = of_property_read_u32(gmsl, "st-vc", &value);

  if (err < 0)
  {
    dev_err(dev, "[%s] : No st-vc info\n", __func__);
    goto error;
  }

  priv->g_ctx.st_vc = value;

  err = of_property_read_u32(gmsl, "vc-id", &value);

  if (err < 0)
  {
    dev_err(dev, "[%s] : No vc-id info\n", __func__);
    goto error;
  }

  priv->g_ctx.dst_vc = value;

  err = of_property_read_u32(gmsl, "num-lanes", &value);

  if (err < 0)
  {
    dev_err(dev, "[%s] : No num-lanes info\n", __func__);
    goto error;
  }

  priv->g_ctx.num_csi_lanes = value;

  priv->g_ctx.num_streams = of_property_count_strings(gmsl, "streams");

  if (priv->g_ctx.num_streams <= 0)
  {
    dev_err(dev, "[%s] : No streams found\n", __func__);
    err = -EINVAL;
    goto error;
  }

  for (i = 0; i < priv->g_ctx.num_streams; i++)
  {
    of_property_read_string_index(gmsl, "streams", i, &str_value1[i]);

    if (!str_value1[i])
    {
      dev_err(dev, "[%s] : Invalid Stream Info\n", __func__);
      goto error;
    }

    if (!strcmp(str_value1[i], "raw12"))
    {
      priv->g_ctx.streams[i].st_data_type = GMSL_CSI_DT_RAW_12;
    }
    else if (!strcmp(str_value1[i], "yuv8"))
    {
      priv->g_ctx.streams[i].st_data_type = GMSL_CSI_DT_YUV_8;
    }
    else if (!strcmp(str_value1[i], "embed"))
    {
      priv->g_ctx.streams[i].st_data_type = GMSL_CSI_DT_EMBED;
    }
    else if (!strcmp(str_value1[i], "ued-u1"))
    {
      priv->g_ctx.streams[i].st_data_type = GMSL_CSI_DT_UED_U1;
    }
    else
    {
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

static int tier4_imx728_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  struct device *dev = &client->dev;
  struct device_node *node = dev->of_node;
  struct tegracam_device *tc_dev;
  struct tier4_imx728 *priv;
  int err = 0;

  dev_info(dev, "[%s] : Probing V4L2 Sensor.\n", __func__);

  if (!IS_ENABLED(CONFIG_OF) || !node)
  {
    return -EINVAL;
  }

  wst_priv[camera_channel_count].p_client = NULL;
  wst_priv[camera_channel_count].p_priv = NULL;
  wst_priv[camera_channel_count].p_tc_dev = NULL;

  wst_priv[camera_channel_count].isp_ser_shutdown = false;
  wst_priv[camera_channel_count].des_shutdown = false;
  wst_priv[camera_channel_count].running = false;

  priv = devm_kzalloc(dev, sizeof(struct tier4_imx728), GFP_KERNEL);

  if (!priv)
  {
    dev_err(dev, "[%s] : Unable to allocate Memory!\n", __func__);
    return -ENOMEM;
  }

  tc_dev = devm_kzalloc(dev, sizeof(struct tegracam_device), GFP_KERNEL);

  if (!tc_dev)
  {
    return -ENOMEM;
  }

  priv->i2c_client = tc_dev->client = client;

  tc_dev->dev = dev;

  strncpy(tc_dev->name, "imx728", sizeof(tc_dev->name));

  tc_dev->dev_regmap_config = &tier4_sensor_regmap_config;
  tc_dev->sensor_ops = &tier4_imx728_common_ops;
  tc_dev->v4l2sd_internal_ops = &tier4_imx728_subdev_internal_ops;
  tc_dev->tcctrl_ops = &tier4_imx728_ctrl_ops;

  err = tegracam_device_register(tc_dev);

  if (err)
  {
    dev_err(dev, "[%s] : Tegra Camera Driver Registration failed\n", __func__);
    return err;
  }

  priv->tc_dev = tc_dev;
  priv->s_data = tc_dev->s_data;
  priv->subdev = &tc_dev->s_data->subdev;

  tegracam_set_privdata(tc_dev, (void *)priv);

  priv->g_ctx.sensor_id = SENSOR_ID_IMX728;

  tier4_isx021_sensor_mutex_lock();

  err = tier4_imx728_board_setup(priv);

  if (err)
  {
    dev_err(dev, "[%s] : Board Setup failed\n", __func__);
    goto err_tegracam_unreg;
  }

  /* Pair sensor to serializer dev */

  err = tier4_max9295_sdev_pair(priv->ser_dev, &priv->g_ctx);

  if (err)
  {
    dev_err(&client->dev, "[%s] : GMSL Ser Pairing failed\n", __func__);
    goto err_tegracam_unreg;
  }
  /* Register sensor to deserializer dev */

  err = tier4_max9296_sdev_register(priv->dser_dev, &priv->g_ctx);

  if (err)
  {
    dev_err(&client->dev, "[%s] : GMSL Deserializer Register failed\n", __func__);
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

  err = tier4_imx728_gmsl_serdes_setup(priv);

  if (err)
  {
    dev_err(&client->dev, "[%s] : GMSL Serdes setup failed\n", __func__);
    goto err_max9296_unreg;
  }

  err = tegracam_v4l2subdev_register(tc_dev, true);

  if (err)
  {
    dev_err(dev, "[%s] : Tegra Camera Subdev Registration failed\n", __func__);
    goto err_max9296_unreg;
  }

  wst_priv[camera_channel_count].p_client = client;
  wst_priv[camera_channel_count].p_priv = priv;
  wst_priv[camera_channel_count].p_tc_dev = tc_dev;

  camera_channel_count++;

  tier4_isx021_sensor_mutex_unlock();

  return NO_ERROR;

//  tier4_isx021_sensor_mutex_unlock();

//  dev_info(&client->dev, "Detected Tier4 IMX490 sensor\n");

err_max9296_unreg:
  tier4_max9296_sdev_unregister(priv->dser_dev, &client->dev);
err_max9295_unpair:
  tier4_max9295_sdev_unpair(priv->ser_dev, &client->dev);
err_tegracam_unreg:
  tegracam_device_unregister(priv->tc_dev);
  camera_channel_count++;
  tier4_isx021_sensor_mutex_unlock();



  return err;
}

static int tier4_imx728_remove(struct i2c_client *client)
{
  struct camera_common_data *s_data = to_camera_common_data(&client->dev);
  struct tier4_imx728 *priv = (struct tier4_imx728 *)s_data->priv;

  tier4_imx728_gmsl_serdes_reset(priv);

  tegracam_v4l2subdev_unregister(priv->tc_dev);

  tegracam_device_unregister(priv->tc_dev);

  return NO_ERROR;
}

static struct mutex tier4_imx728_lock;

static bool tier4_imx728_is_isp_ser_shutdown(int nport)
{
  if (wst_priv[nport].isp_ser_shutdown)
  {
    return true;
  }
  return false;
}

static bool tier4_imx728_is_des_shutdown(int nport)
{
  if (wst_priv[nport].des_shutdown)
  {
    return true;
  }
  return false;
}

static void tier4_imx728_set_isp_ser_shutdown(int nport, bool val)
{
  wst_priv[nport].isp_ser_shutdown = val;
}

static void tier4_imx728_set_des_shutdown(int nport, bool val)
{
  wst_priv[nport].des_shutdown = val;
}

static bool tier4_imx728_is_current_i2c_client(struct i2c_client *client, int nport)
{
  if (client == wst_priv[nport].p_client)
  {
    return true;
  }
  return false;
}

// ----------------------------------------------------------------------------

static void tier4_imx728_shutdown(struct i2c_client *client)
{
  struct tier4_imx728 *priv = NULL;
  int i;

  tier4_isx021_sensor_mutex_lock();

  mutex_lock(&tier4_imx728_lock);

  if (!client)
  {
    goto error_exit;
  }

  for (i = 0; i < MAX_NUM_CAMERA; i++)
  {
    if (tier4_imx728_is_current_i2c_client(client, i))
    {
      priv = wst_priv[i].p_priv;

      if ((priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_XAVIER) ||
          (priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_ORIN))
      {
        /**
        * For RQX-59G, we don't write max9295 and max9296 registers when shutdown is invoked.
        * Instead, we disable deserializer power by calling tier4_fpga_power_off_deserializer().
        * Furthermore, we disable camera power by calling max20089_power_off_cam().
        */
        
        if (priv->g_ctx.hardware_model == HW_MODEL_ADLINK_ROSCUBE_ORIN)
        {
          // only RQX-59G with ADLINK GMSL board supports read/write MAX20089
          if (MAX9295_REG_PORT_A == priv->g_ctx.ser_reg)
            max20089_power_off_cam(priv->cam_power_protect_dev, 0x01);
          else if (MAX9295_REG_PORT_B == priv->g_ctx.ser_reg)
            max20089_power_off_cam(priv->cam_power_protect_dev, 0x02);
        }

        // power off deserializer
        tier4_fpga_power_off_deserializer(priv->fpga_dev, priv->g_ctx.reg_mux);

        // release mutex lock
        mutex_unlock(&tier4_imx728_lock);
        tier4_isx021_sensor_mutex_unlock();
        return;
      }

      if (MAX9295_REG_PORT_B == priv->g_ctx.ser_reg) // Even port
      {  // Even port number( GMSL B port on a Des : i = port_number -1 )

        if (tier4_imx728_is_camera_connected_to_port(i - 1))
        {  // if a camera connected to another(GMSL A)port on a Des.
           // a camera is connected to each port of the Des.
          if (tier4_imx728_is_isp_ser_shutdown(i - 1))
          {  //  ISP and Ser on another(GMSL A) port have been already shut down ?

            if (tier4_imx728_is_des_shutdown(i - 1) == false)
            {                                              // if Des on another(GMSL A)port is not shutdown yet
              tier4_imx728_set_isp_ser_shutdown(i, true);  // ISP and Ser will be shut down
              tier4_imx728_set_des_shutdown(i, true);      // Des will be shut down
            }
            else
            {
              // Des is already shut down. This is Error case.
            }
          }
          else
          {  // The camera ISP and Ser on another(GMSL A) port are not shut down yet.

            if (tier4_imx728_is_des_shutdown(i - 1) == false)
            {                                              // if Des is not shut down yet.
              tier4_imx728_set_isp_ser_shutdown(i, true);  // ISP and Ser will be shut down
              tier4_imx728_set_des_shutdown(i, false);     //  The Des won't be shut down.
            }
            else
            {
              // Des is already shut down. This is Error case.
            }
          }
        }
        else
        {                                              // a camera is connected to only (GMSL B) port on Des.
          tier4_imx728_set_isp_ser_shutdown(i, true);  // ISP and Ser will be shut down
          tier4_imx728_set_des_shutdown(i, true);      // The Des won't be shut down.
        }
      }
      else // MAX9295_REG_PORT_A, odd port
      {

        if (tier4_imx728_is_camera_connected_to_port(i + 1))
        {  // Another camera is connected to another(GMSL B) port on the Des

          if (tier4_imx728_is_isp_ser_shutdown(i + 1))
          {  // if the ISP and Ser on another port are already shut down

            if (tier4_imx728_is_des_shutdown(i + 1) == false)
            {                                              // if Des is not shut down yet.
              tier4_imx728_set_isp_ser_shutdown(i, true);  // ISP and Ser will be shut down
              tier4_imx728_set_des_shutdown(i, true);     //  The Des will be shut down.
            }
            else
            {
              // Des is already shut down. This is Error case.
            }
          }
          else
          {  // The ISP and Ser on another(GMSL B) port are not shut down yet.

            if (tier4_imx728_is_des_shutdown(i + 1) == false)
            {  // if Des on another(GMSL B) port is not shut down yet.

              tier4_imx728_set_isp_ser_shutdown(i, true);  // ISP and Ser will be shut down
              tier4_imx728_set_des_shutdown(i, false);     //  The Des will not be shut down.
            }
            else
            {
              // Des is already shut down. This is Error case.
            }
          }
        }
        else
        {
          tier4_imx728_set_isp_ser_shutdown(i, true);  // ISP and Ser will be shut down
          tier4_imx728_set_des_shutdown(i, true);      //  The Des will be shut down.
        }
      }


      if (tier4_imx728_is_isp_ser_shutdown(i))
      {
        // Reset camera sensor
        tier4_max9295_control_sensor_power_seq(priv->ser_dev, SENSOR_ID_IMX728, false);
        // S/W Reset max9295
        tier4_max9295_reset_control(priv->ser_dev);
      }

      if (tier4_imx728_is_des_shutdown(i))
      {
        // S/W Reset max9296
        tier4_max9296_reset_control(priv->dser_dev, &client->dev, true);
      }

      if (priv == NULL || i >= MAX_NUM_CAMERA)
      {
        mutex_unlock(&tier4_imx728_lock);
        tier4_isx021_sensor_mutex_unlock();
        return;
      }
    }

  }  // for loop

error_exit:

  mutex_unlock(&tier4_imx728_lock);

  tier4_isx021_sensor_mutex_unlock();
}
static const struct i2c_device_id tier4_imx728_id[] = { { "tier4_imx728", 0 }, {} };

MODULE_DEVICE_TABLE(i2c, tier4_imx728_id);

static struct i2c_driver tier4_imx728_i2c_driver = {
  .driver = {
    .name       = "tier4_imx728",
    .owner      = THIS_MODULE,
    .of_match_table = of_match_ptr(tier4_imx728_of_match),
  },
  .probe    = tier4_imx728_probe,
  .remove   = tier4_imx728_remove,
  .shutdown   = tier4_imx728_shutdown,
  .id_table   = tier4_imx728_id,
};

static int __init tier4_imx728_init(void)
{
  mutex_init(&serdes_lock__);
  mutex_init(&tier4_imx728_lock);

  printk(KERN_INFO "TIERIV Automotive HDR Camera driver.\n");

  return i2c_add_driver(&tier4_imx728_i2c_driver);
}

static void __exit tier4_imx728_exit(void)
{
  mutex_destroy(&serdes_lock__);
  mutex_destroy(&tier4_imx728_lock);

  i2c_del_driver(&tier4_imx728_i2c_driver);
}

module_init(tier4_imx728_init);
module_exit(tier4_imx728_exit);

MODULE_DESCRIPTION("TIERIV Automotive HDR Camera driver");
MODULE_AUTHOR("Originaly NVIDIA Corporation");
MODULE_AUTHOR("K.Iwasaki");
MODULE_AUTHOR("Y.Fujii");
MODULE_LICENSE("GPL v2");
