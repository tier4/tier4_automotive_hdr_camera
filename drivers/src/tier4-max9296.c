/*
 * tier4_max9296.c - tier4_max9296 GMSL Deserializer driver
 *
 * Copyright (c) 2022, TIER IV Inc.  All rights reserved.
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


#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <media/camera_common.h>

#include "tier4-max9296.h"
#include "tier4-gmsl-link.h"
#include "tier4-hw-model.h"

struct tier4_max9296_source_ctx
{
  struct gmsl_link_ctx *g_ctx;
  bool st_enabled;
};

#define _USE_CHECK_LINK_LOCKED_ 1

#define MAX9296_SHOW_I2C_WRITE_MSG 1

/* register specifics */

#define MAX9296_LINK_ADDR 0x0013

#define MAX9296_REG5_ADDR 0x0005
#define MAX9296_CTRL0_ADDR 0x0010

#define MAX9296_PIPE_X_ST_SEL_ADDR 0x0050

#define MAX9296_GPIO0_CONFIG_MFP0_ADDR 0x02B0
#define MAX9296_GPIO0_GPIO_TX_ID_ADDR 0x02B1
#define MAX9296_GPIO0_GPIO_RX_ID_ADDR 0x02B2

#define MAX9296_PHY1_CLK_ADDR 0x0320

#define MAX9296_DST_CSI_MODE_ADDR 0x0330
#define MAX9296_PWDN_PHYS_ADDR 0x0332
#define MAX9296_LANE_MAP1_ADDR 0x0333
#define MAX9296_LANE_MAP2_ADDR 0x0334

#define MAX9296_LANE_CTRL0_ADDR 0x040A
#define MAX9296_TX11_PIPE_X_EN_ADDR 0x040B
#define MAX9296_PIPE_X_SRC_0_MAP_ADDR 0x040D
#define MAX9296_PIPE_X_DST_0_MAP_ADDR 0x040E
#define MAX9296_PIPE_X_SRC_1_MAP_ADDR 0x040F

#define MAX9296_PIPE_X_DST_1_MAP_ADDR 0x0410
#define MAX9296_PIPE_X_SRC_2_MAP_ADDR 0x0411
#define MAX9296_PIPE_X_DST_2_MAP_ADDR 0x0412

#define MAX9296_TX45_PIPE_X_DST_CTRL_ADDR 0x042D

#define MAX9296_LANE_CTRL1_ADDR 0x044A
#define MAX9296_LANE_CTRL2_ADDR 0x048A
#define MAX9296_LANE_CTRL3_ADDR 0x04CA

/* data defines */

#define MAX9296_CSI_MODE_4X2 0x1
#define MAX9296_CSI_MODE_2X4 0x4
#define MAX9296_LANE_MAP1_4X2 0x44
#define MAX9296_LANE_MAP2_4X2 0x44
#define MAX9296_LANE_MAP1_2X4 0x4E
#define MAX9296_LANE_MAP2_2X4 0xE4

#define MAX9296_LANE_CTRL_MAP(num_lanes) (((num_lanes) << 6) & 0xF0)

#define MAX9296_ALLPHYS_NOSTDBY 0xF0
#define MAX9296_ST_ID_SEL_INVALID 0xF

//#define MAX9296_PHY1_CLK                  0x32
#define MAX9296_PHY1_CLK_3100MHZ 0x3F
#define MAX9296_PHY1_CLK_3000MHZ 0x3E
#define MAX9296_PHY1_CLK_2900MHZ 0x3D
#define MAX9296_PHY1_CLK_2800MHZ 0x3C
#define MAX9296_PHY1_CLK_2700MHZ 0x3B
#define MAX9296_PHY1_CLK_2600MHZ 0x3A
#define MAX9296_PHY1_CLK_2500MHZ 0x39
#define MAX9296_PHY1_CLK_2400MHZ 0x38
#define MAX9296_PHY1_CLK_2300MHZ 0x37
#define MAX9296_PHY1_CLK_2200MHZ 0x36
#define MAX9296_PHY1_CLK_2100MHZ 0x35
#define MAX9296_PHY1_CLK_2000MHZ 0x34
#define MAX9296_PHY1_CLK_1900MHZ 0x33
#define MAX9296_PHY1_CLK_1800MHZ 0x32
#define MAX9296_PHY1_CLK_1700MHZ 0x31
#define MAX9296_PHY1_CLK_1600MHZ 0x30
#define MAX9296_PHY1_CLK_1500MHZ 0x2F
#define MAX9296_PHY1_CLK_1400MHZ 0x2E
#define MAX9296_PHY1_CLK_1300MHZ 0x2D
#define MAX9296_PHY1_CLK_1200MHZ 0x2C

#define MAX9296_PHY1_CLK MAX9296_PHY1_CLK_1500MHZ

#define MAX9296_RESET_ALL 0x80

/* Dual GMSL MAX9296A/B */

#define MAX9296_MAX_SOURCES 2

#define MAX9296_MAX_PIPES 4

#define MAX9296_PIPE_X 0
#define MAX9296_PIPE_Y 1
#define MAX9296_PIPE_Z 2
#define MAX9296_PIPE_U 3
#define MAX9296_PIPE_INVALID 0xF

#define MAX9296_CSI_CTRL_0 0
#define MAX9296_CSI_CTRL_1 1
#define MAX9296_CSI_CTRL_2 2
#define MAX9296_CSI_CTRL_3 3

#define MAX9296_INVAL_ST_ID 0xFF

/* Use reset value as per spec, confirm with vendor */

#define MAX9296_RESET_ST_ID 0x00

struct pipe_ctx
{
  u32 id;
  u32 dt_type;
  u32 dst_csi_ctrl;
  u32 st_count;
  u32 st_id_sel;
};

struct tier4_max9296
{
  struct i2c_client *i2c_client;
  struct regmap *regmap;
  u32 num_src;
  u32 max_src;
  u32 num_src_found;
  u32 src_link;
  bool splitter_enabled;

  struct tier4_max9296_source_ctx sources[MAX9296_MAX_SOURCES];

  struct mutex lock;
  u32 sdev_ref;
  bool lane_setup;
  bool link_setup;
  struct pipe_ctx pipe[MAX9296_MAX_PIPES];
  u8 csi_mode;
  u8 lane_mp1;
  u8 lane_mp2;
  int reset_gpio;
  int pw_ref;
  struct regulator *vdd_cam_1v2;
};

static int tier4_max9296_read_reg(struct device *dev, u16 addr, u8 *val)
{
  int err = 0;
  u32 reg_val = 0;
  struct tier4_max9296 *priv = dev_get_drvdata(dev);
  char str_bus_num[4], str_sl_addr[4];
  int len;

  memset(str_bus_num, 0, 4);
  memset(str_sl_addr, 0, 4);

  err = regmap_read(priv->regmap, addr, &reg_val);
  *val = reg_val & 0xFF;

  if ((err == 0) && (dev != NULL))
  {
    len = strlen(dev->kobj.name);

    if (dev)
    {
      strncpy(str_bus_num, &dev->kobj.name[0], 2);
      strncpy(str_sl_addr, &dev->kobj.name[len - 2], 2);
    }
    dev_info(dev, "tier4_max9296_read_reg %s 0x%s 0x%x\n", str_bus_num, str_sl_addr, addr);
  }

  return err;
}

static int tier4_max9296_write_reg(struct device *dev, u16 addr, u8 val)
{
  struct tier4_max9296 *priv;
  int err;
  char str_bus_num[4], str_sl_addr[4];
  int len;

  priv = dev_get_drvdata(dev);

  memset(str_bus_num, 0, 4);
  memset(str_sl_addr, 0, 4);

  len = strlen(dev->kobj.name);

  if (dev)
  {
    strncpy(str_bus_num, &dev->kobj.name[0], 2);
    strncpy(str_sl_addr, &dev->kobj.name[len - 2], 2);
  }

  err = regmap_write(priv->regmap, addr, val);

  if (err)
  {
    dev_err(dev, "[%s] : Max9296 I2C write failed Reg at 0x%04X:[0x%02X]\n", __func__, addr, val);
    return err;
  }
#if MAX9296_SHOW_I2C_WRITE_MSG
  else
  {
    dev_info(dev, "[%s] : Max9296 I2C write register at 0x%04X:[0x%02X]\n", __func__, addr, val);
  }
#endif
  /* delay before next i2c command as required for SERDES link */

  usleep_range(100, 110);

  return err;
}

static int tier4_max9296_get_sdev_idx(struct device *dev, struct device *s_dev, int *idx)
{
  int i;
  int err = 0;
  struct tier4_max9296 *priv = dev_get_drvdata(dev);

  mutex_lock(&priv->lock);

  for (i = 0; i < priv->max_src; i++)
  {
    if (priv->sources[i].g_ctx->s_dev == s_dev)
    {
      break;
    }
  }
  if (i == priv->max_src)
  { 
    dev_err(dev, "[%s] : No sdev found\n", __func__);
    err = -EINVAL;
    goto ret;
  }

  if (idx)
  {
    *idx = i;
  }

ret:
  mutex_unlock(&priv->lock);
  return err;
}

static void tier4_max9296_pipes_reset(struct tier4_max9296 *priv)
{
  /*
   * This is default pipes combination. add more mappings
   * for other combinations and requirements.
   */
  struct pipe_ctx pipe_defaults[] = { { MAX9296_PIPE_X, GMSL_CSI_DT_YUV_8,  // For YUV8
                                        MAX9296_CSI_CTRL_1, 0, MAX9296_INVAL_ST_ID },
                                      { MAX9296_PIPE_Y, GMSL_CSI_DT_YUV_8,  // For YUV8
                                        MAX9296_CSI_CTRL_1, 0, MAX9296_INVAL_ST_ID },
                                      { MAX9296_PIPE_Z, GMSL_CSI_DT_EMBED, MAX9296_CSI_CTRL_1, 0, MAX9296_INVAL_ST_ID },
                                      { MAX9296_PIPE_U, GMSL_CSI_DT_EMBED, MAX9296_CSI_CTRL_1, 0,
                                        MAX9296_INVAL_ST_ID } };

  /*
   * Add DT props for num-streams and stream sequence, and based on that
   * set the appropriate pipes defaults.
   * For now default it supports "2 RAW12 and 2 EMBED" 1:1 mappings.
   */
  memcpy(priv->pipe, pipe_defaults, sizeof(pipe_defaults));
}

static void tier4_max9296_reset_ctx(struct tier4_max9296 *priv)
{
  int i;

  priv->link_setup = false;
  priv->lane_setup = false;
  priv->num_src_found = 0;
  priv->src_link = 0;
  priv->splitter_enabled = false;

  tier4_max9296_pipes_reset(priv);

  for (i = 0; i < priv->num_src; i++)
  {
    priv->sources[i].st_enabled = false;
  }
}

int tier4_max9296_power_on(struct device *dev)
{
  struct tier4_max9296 *priv = dev_get_drvdata(dev);
  int i, err = 0;

  mutex_lock(&priv->lock);

  if (priv->pw_ref == 0)
  {
    usleep_range(1, 2);

    for (i = 0; i < priv->max_src; i++)
    {
      if (priv->sources[i].g_ctx)
      {
        break;
      }
    }
    if (priv->reset_gpio)
      //            gpio_set_value(priv->reset_gpio, 0);
      gpio_direction_output(priv->reset_gpio, 0);

    usleep_range(50, 80);

    if (priv->vdd_cam_1v2)
    {
      err = regulator_enable(priv->vdd_cam_1v2);
      if (unlikely(err))
        goto ret;
    }
    usleep_range(50, 80);

    /*exit reset mode: XCLR */
    if (priv->reset_gpio)
    {

      //            gpio_set_value(priv->reset_gpio, 0);
      gpio_direction_output(priv->reset_gpio, 0);

      usleep_range(50, 80);
      usleep_range(1000000,1100000);
      //            gpio_set_value(priv->reset_gpio, 1);

      gpio_direction_output(priv->reset_gpio, 1);

      usleep_range(50, 80);
    }

    /* delay to settle reset */
    //      msleep(1000);
  }

  priv->pw_ref++;

ret:

  mutex_unlock(&priv->lock);

  return err;
}
EXPORT_SYMBOL(tier4_max9296_power_on);

void tier4_max9296_power_off(struct device *dev)
{
  struct tier4_max9296 *priv = dev_get_drvdata(dev);

  mutex_lock(&priv->lock);
  priv->pw_ref--;

  if (priv->pw_ref == 0)
  {
    /* enter reset mode: XCLR */
    usleep_range(1, 2);
    if (priv->reset_gpio)
      //            gpio_set_value(priv->reset_gpio, 0);
      gpio_direction_output(priv->reset_gpio, 0);

    if (priv->vdd_cam_1v2)
      regulator_disable(priv->vdd_cam_1v2);
  }

  mutex_unlock(&priv->lock);
}
EXPORT_SYMBOL(tier4_max9296_power_off);

static int tier4_max9296_write_link(struct device *dev, u32 link)
{
  int err = 0;

  if (link == GMSL_SERDES_CSI_LINK_A)
  {
    err = tier4_max9296_write_reg(dev, MAX9296_CTRL0_ADDR, 0x01);
    err |= tier4_max9296_write_reg(dev, MAX9296_CTRL0_ADDR, 0x21);
  }
  else if (link == GMSL_SERDES_CSI_LINK_B)
  {
    err = tier4_max9296_write_reg(dev, MAX9296_CTRL0_ADDR, 0x02);
    err |= tier4_max9296_write_reg(dev, MAX9296_CTRL0_ADDR, 0x22);
  }
  else
  {
    dev_err(dev, "[%s] : Invalid GMSL link\n", __func__);
    return -EINVAL;
  }

  /* delay to settle link */
  msleep(100);

  return 0;
}

int tier4_max9296_setup_link(struct device *dev, struct device *s_dev)
{
  struct tier4_max9296 *priv = dev_get_drvdata(dev);
  int err = 0;
  int i;

  err = tier4_max9296_get_sdev_idx(dev, s_dev, &i);
  if (err)
  {
    dev_err(dev, "[%s] : tier4_max9296_get_sdev_idx failed.\n", __func__);
    return err;
  }

  //    max9296_debug_i2c_write = priv->sources[i].g_ctx->debug_i2c_write;

  mutex_lock(&priv->lock);

  if (!priv->splitter_enabled)
  {
    err = tier4_max9296_write_link(dev, priv->sources[i].g_ctx->serdes_csi_link);
    if (err)
    {
      dev_err(dev, "[%s] : tier4_max9296_write_link failed.\n", __func__);
      goto ret;
    }
    priv->link_setup = true;
  }

ret:
  mutex_unlock(&priv->lock);

  return err;
}
EXPORT_SYMBOL(tier4_max9296_setup_link);

static int tier4_max9296_link_locked(struct device *dev)
{
  u8 val;

  usleep_range(100, 110);
  tier4_max9296_read_reg(dev, MAX9296_LINK_ADDR, &val);

  if (0 == (val & 0x08))
  {
    return -EINVAL;
  }
  return 0;
}

int tier4_max9296_setup_gpi(struct device *dev, int fsync_mfp)
{
  int err = 0;
  u16 gpio_mfp_base;

  dev_info(dev, "[%s] :  MFP%d is used for fsync\n", __func__, fsync_mfp);

  gpio_mfp_base = (MAX9296_GPIO0_CONFIG_MFP0_ADDR + 3 * fsync_mfp) & 0xFFFF;

  err += tier4_max9296_write_reg(dev, gpio_mfp_base, 0x03);
  err += tier4_max9296_write_reg(dev, gpio_mfp_base + 1, 0x06);
  err += tier4_max9296_write_reg(dev, gpio_mfp_base + 2, 0x00);

  return err;
}
EXPORT_SYMBOL(tier4_max9296_setup_gpi);

int tier4_max9296_setup_control(struct device *dev, struct device *s_dev)
{
  struct tier4_max9296 *priv = dev_get_drvdata(dev);
  int err = 0;
  int i;

  err = tier4_max9296_get_sdev_idx(dev, s_dev, &i);

  if (err)
  {
    dev_err(dev, "[%s] : tier4_max9296_get_sdev_idx() failed.\n", __func__);
    return err;
  }

  mutex_lock(&priv->lock);

  if (!priv->link_setup)
  {
    dev_err(dev, "[%s] : Invalid state\n", __func__);
    err = -EINVAL;
    goto error;
  }

  if (priv->sources[i].g_ctx->serdev_found)
  {
    priv->num_src_found++;
    priv->src_link = priv->sources[i].g_ctx->serdes_csi_link;
  }

  /* Enable splitter mode */
  if ((priv->max_src > 1U) && (priv->num_src_found > 0U) && (priv->splitter_enabled == false))
  {
    tier4_max9296_write_reg(dev, MAX9296_CTRL0_ADDR, 0x03);
    tier4_max9296_write_reg(dev, MAX9296_CTRL0_ADDR, 0x23);

    priv->splitter_enabled = true;

    /* delay to settle link */
    msleep(500);
  }

#if _USE_CHECK_LINK_LOCKED_
  // Check GMSL link
  err = tier4_max9296_link_locked(dev);
  if (err)
  {
    // GMSL2 link not locked
    dev_info(dev, "[%s]: max9296 link is not locked.\n", __func__);
    err = tier4_max9296_write_link(dev, priv->src_link);
    if (err)
    {
      dev_err(dev, "[%s]: tier4_max9296_write_link() failed.\n", __func__);
      goto error;
    }
  }
#endif

  if (!priv->splitter_enabled)
  {
    tier4_max9296_write_reg(dev, MAX9296_PWDN_PHYS_ADDR, MAX9296_ALLPHYS_NOSTDBY);
    priv->splitter_enabled = true;
  }
  priv->link_setup = true;

  priv->sdev_ref++;

  /* Reset splitter mode if all devices are not found */
  if ((priv->sdev_ref == priv->max_src) && (priv->splitter_enabled == true) && (priv->num_src_found > 0U) &&
      (priv->num_src_found < priv->max_src))
  {
    err = tier4_max9296_write_link(dev, priv->src_link);
    if (err)
      goto error;

    priv->splitter_enabled = false;
  }

error:
  mutex_unlock(&priv->lock);
  return err;
}
EXPORT_SYMBOL(tier4_max9296_setup_control);

int tier4_max9296_reset_control(struct device *dev, struct device *s_dev, bool force_reset)
{
  struct tier4_max9296 *priv = dev_get_drvdata(dev);
  int err = 0;

  mutex_lock(&priv->lock);
  if (!priv->sdev_ref && force_reset == false)
  {
    dev_info(dev, "[%s] : dev is already in reset state\n", __func__);
    goto ret;
  }

  priv->sdev_ref--;
  if (priv->sdev_ref == 0 || force_reset == true)
  {
    tier4_max9296_reset_ctx(priv);
    tier4_max9296_write_reg(dev, MAX9296_CTRL0_ADDR, MAX9296_RESET_ALL);

    /* delay to settle reset */
    msleep(100);
  }

ret:
  mutex_unlock(&priv->lock);

  return err;
}
EXPORT_SYMBOL(tier4_max9296_reset_control);

int tier4_max9296_sdev_register(struct device *dev, struct gmsl_link_ctx *g_ctx)
{
  struct tier4_max9296 *priv = NULL;
  int i;
  int err = 0;

  if (!dev || !g_ctx || !g_ctx->s_dev)
  {
    dev_err(dev, "[%s] : Invalid input params\n", __func__);
    return -EINVAL;
  }

  priv = dev_get_drvdata(dev);

  mutex_lock(&priv->lock);

  if (g_ctx->hardware_model == HW_MODEL_NVIDIA_ORIN_DEVKIT )
  {
    for ( i = 0 ; i < MAX9296_MAX_SOURCES ;  i++ )
    {
      if ( priv->sources[i].g_ctx != NULL )
      {
        priv->sources[i].g_ctx->serdes_csi_link = 0;
      }
    }
  }

  if (priv->num_src > priv->max_src)
  {
    dev_err(dev, "[%s] : MAX9296 inputs size exhausted\n", __func__);
    err = -ENOMEM;
    goto error;
  }

  /* Check csi mode compatibility */
  if (!((priv->csi_mode == MAX9296_CSI_MODE_2X4) ?
            ((g_ctx->csi_mode == GMSL_CSI_1X4_MODE) || (g_ctx->csi_mode == GMSL_CSI_2X4_MODE)) :
            ((g_ctx->csi_mode == GMSL_CSI_2X2_MODE) || (g_ctx->csi_mode == GMSL_CSI_4X2_MODE))))
  {
    dev_err(dev, "[%s] : CSI mode not supported\n", __func__);
    err = -EINVAL;
    goto error;
  }

  for (i = 0; i < priv->num_src; i++)
  {
    if (g_ctx->serdes_csi_link == priv->sources[i].g_ctx->serdes_csi_link)
    {
      dev_err(dev, "[%s] : Serdes CSI link is in use\n", __func__);
      err = -EINVAL;
      goto error;
    }
    /*
     * All sdevs should have same num-csi-lanes regardless of
     * dst csi port selected.
     * Later if there is any usecase which requires each port
     * to be configured with different num-csi-lanes, then this
     * check should be performed per port.
     */

    if (g_ctx->num_csi_lanes != priv->sources[i].g_ctx->num_csi_lanes)
    {
      dev_err(dev, "[%s] : CSI num lanes mismatch. %d, %d i=%d\n", __func__, g_ctx->num_csi_lanes,
              priv->sources[i].g_ctx->num_csi_lanes, i);
      err = -EINVAL;
      goto error;
    }
  }

  priv->sources[priv->num_src].g_ctx = g_ctx;
  priv->sources[priv->num_src].st_enabled = false;

  priv->num_src++;

error:
  mutex_unlock(&priv->lock);
  return err;
}
EXPORT_SYMBOL(tier4_max9296_sdev_register);

int tier4_max9296_sdev_unregister(struct device *dev, struct device *s_dev)
{
  struct tier4_max9296 *priv = NULL;
  int err = 0;
  int i = 0;

  if (!dev || !s_dev)
  {
    dev_err(dev, "[%s] : Invalid input params\n", __func__);
    return -EINVAL;
  }

  priv = dev_get_drvdata(dev);
  mutex_lock(&priv->lock);

  if (priv->num_src == 0)
  {
    dev_err(dev, "[%s] : No source found\n", __func__);
    err = -ENODATA;
    goto error;
  }

  for (i = 0; i < priv->num_src; i++)
  {
    if (s_dev == priv->sources[i].g_ctx->s_dev)
    {
      priv->sources[i].g_ctx = NULL;
      priv->num_src--;
      break;
    }
  }

  if (i == priv->num_src)
  {
    dev_err(dev, "[%s] : Requested device not found\n", __func__);
    err = -EINVAL;
    goto error;
  }

error:
  mutex_unlock(&priv->lock);
  return err;
}
EXPORT_SYMBOL(tier4_max9296_sdev_unregister);

static int tier4_max9296_get_available_pipe(struct device *dev, u32 st_data_type, u32 dst_csi_port)
{
  struct tier4_max9296 *priv = dev_get_drvdata(dev);
  int i;

  for (i = 0; i < MAX9296_MAX_PIPES; i++)
  {
    /*
     * TODO: Enable a pipe for multi stream configuration having
     * similar stream data type. For now use st_count as a flag
     * for 1 to 1 mapping in pipe and stream data type, same can
     * be extended as count for many to 1 mapping. Would also need
     * few more checks such as input stream id select, dst port etc.
     */
    if ((priv->pipe[i].dt_type == st_data_type) &&
        ((dst_csi_port == GMSL_CSI_PORT_A) ?
             (priv->pipe[i].dst_csi_ctrl == MAX9296_CSI_CTRL_0) || (priv->pipe[i].dst_csi_ctrl == MAX9296_CSI_CTRL_1) :
             (priv->pipe[i].dst_csi_ctrl == MAX9296_CSI_CTRL_2) ||
                 (priv->pipe[i].dst_csi_ctrl == MAX9296_CSI_CTRL_3)) &&
        (!priv->pipe[i].st_count))
      break;
  }

  if (i == MAX9296_MAX_PIPES)
  {
    dev_err(dev, "[%s] : All pipes are busy\n", __func__);
    return -ENOMEM;
  }

  return i;
}

struct reg_pair
{
  u16 addr;
  u8 val;
};

static int tier4_max9296_setup_pipeline(struct device *dev, struct gmsl_link_ctx *g_ctx)
{
  struct tier4_max9296 *priv = dev_get_drvdata(dev);
  struct gmsl_stream *g_stream;
  struct reg_pair *map_list;
  u32 arr_sz = 0;
  int pipe_id = 0;
  u32 i = 0;
  u32 j = 0;
  u32 vc_idx = 0;

  for (i = 0; i < g_ctx->num_streams; i++)
  {
    /* Base data type mapping: pipeX/RAW12/CSICNTR1 */
    struct reg_pair map_pipe_yuv8[] = {
      // for YUV8
      /* addr, val */
      { MAX9296_TX11_PIPE_X_EN_ADDR, 0x7 },    { MAX9296_TX45_PIPE_X_DST_CTRL_ADDR, 0x15 },

      { MAX9296_PIPE_X_SRC_0_MAP_ADDR, 0x1E },  // for YUV8
      { MAX9296_PIPE_X_DST_0_MAP_ADDR, 0x1E },  // for YUV8
      { MAX9296_PIPE_X_SRC_1_MAP_ADDR, 0x00 }, { MAX9296_PIPE_X_DST_1_MAP_ADDR, 0x00 },
      { MAX9296_PIPE_X_SRC_2_MAP_ADDR, 0x01 }, { MAX9296_PIPE_X_DST_2_MAP_ADDR, 0x01 },
    };

    /* Base data type mapping: pipeX/EMBED/CSICNTR1 */
    struct reg_pair map_pipe_embed[] = {
      /* addr, val */
      { MAX9296_TX11_PIPE_X_EN_ADDR, 0x7 },    { MAX9296_TX45_PIPE_X_DST_CTRL_ADDR, 0x15 },
      { MAX9296_PIPE_X_SRC_0_MAP_ADDR, 0x12 }, { MAX9296_PIPE_X_DST_0_MAP_ADDR, 0x12 },
      { MAX9296_PIPE_X_SRC_1_MAP_ADDR, 0x00 }, { MAX9296_PIPE_X_DST_1_MAP_ADDR, 0x00 },
      { MAX9296_PIPE_X_SRC_2_MAP_ADDR, 0x01 }, { MAX9296_PIPE_X_DST_2_MAP_ADDR, 0x01 },
    };

    g_stream = &g_ctx->streams[i];
    g_stream->des_pipe = MAX9296_PIPE_INVALID;

    if (g_stream->st_data_type == GMSL_CSI_DT_YUV_8)
    {                                      // for YUV8
      map_list = map_pipe_yuv8;            // for YUV8
      arr_sz = ARRAY_SIZE(map_pipe_yuv8);  // for YUV8
    }
    else if (g_stream->st_data_type == GMSL_CSI_DT_EMBED)
    {
      map_list = map_pipe_embed;
      arr_sz = ARRAY_SIZE(map_pipe_embed);
    }
    else if (g_stream->st_data_type == GMSL_CSI_DT_UED_U1)
    {
      continue;
    }
    else
    {
      dev_err(dev, "[%s] : Invalid data type\n", __func__);
      return -EINVAL;
    }

    pipe_id = tier4_max9296_get_available_pipe(dev, g_stream->st_data_type, g_ctx->dst_csi_port);
    if (pipe_id < 0)
      return pipe_id;

    for (j = 0, vc_idx = 3; j < arr_sz; j++, vc_idx += 2)
    {
      /* update pipe configuration */
      map_list[j].addr += (0x40 * pipe_id);
      /* update vc id configuration */
      if (vc_idx < arr_sz)
      {
        map_list[vc_idx].val |= (g_ctx->dst_vc << 6);
      }
      tier4_max9296_write_reg(dev, map_list[j].addr, map_list[j].val);
    }

    /* Set stream id select input */
    if (g_stream->st_id_sel == GMSL_ST_ID_UNUSED)
    {
      dev_err(dev, "[%s] : Invalid stream st_id_sel\n", __func__);
      return -EINVAL;
    }

    g_stream->des_pipe = MAX9296_PIPE_X_ST_SEL_ADDR + pipe_id;

    /* Update pipe internals */
    priv->pipe[pipe_id].st_count++;
    priv->pipe[pipe_id].st_id_sel = g_stream->st_id_sel;
  }

  return 0;
}

int tier4_max9296_start_streaming(struct device *dev, struct device *s_dev)
{
  struct tier4_max9296 *priv = dev_get_drvdata(dev);
  struct gmsl_link_ctx *g_ctx;
  struct gmsl_stream *g_stream;
  int err = 0;
  int i = 0;

  err = tier4_max9296_get_sdev_idx(dev, s_dev, &i);

  if (err)
  {
    dev_err(dev, "[%s] : tier4_max9296_get_sdev_idx failed.\n", __func__);
    return err;
  }

  mutex_lock(&priv->lock);

  g_ctx = priv->sources[i].g_ctx;

  for (i = 0; i < g_ctx->num_streams; i++)
  {
    g_stream = &g_ctx->streams[i];

    if (g_stream->des_pipe != MAX9296_PIPE_INVALID)
    {
      tier4_max9296_write_reg(dev, g_stream->des_pipe, g_stream->st_id_sel);
    }
  }

  mutex_unlock(&priv->lock);

  return 0;
}
EXPORT_SYMBOL(tier4_max9296_start_streaming);

int tier4_max9296_stop_streaming(struct device *dev, struct device *s_dev)
{
  struct tier4_max9296 *priv = dev_get_drvdata(dev);
  struct gmsl_link_ctx *g_ctx;
  struct gmsl_stream *g_stream;
  int err = 0;
  int i = 0;

  err = tier4_max9296_get_sdev_idx(dev, s_dev, &i);
  if (err)
    return err;

  mutex_lock(&priv->lock);
  g_ctx = priv->sources[i].g_ctx;

  for (i = 0; i < g_ctx->num_streams; i++)
  {
    g_stream = &g_ctx->streams[i];

    if (g_stream->des_pipe != MAX9296_PIPE_INVALID)
    {
      tier4_max9296_write_reg(dev, g_stream->des_pipe, MAX9296_RESET_ST_ID);
    }
  }

  mutex_unlock(&priv->lock);

  return 0;
}
EXPORT_SYMBOL(tier4_max9296_stop_streaming);

int tier4_max9296_setup_streaming(struct device *dev, struct device *s_dev)
{
  struct tier4_max9296 *priv = dev_get_drvdata(dev);
  struct gmsl_link_ctx *g_ctx;
  int err = 0;
  int i = 0;
  u16 lane_ctrl_addr;

  err = tier4_max9296_get_sdev_idx(dev, s_dev, &i);

  if (err)
  {
    dev_err(dev, "[%s] : tier4_max9296_get_sdev_idx() failed.\n", __func__);
    return err;
  }

  mutex_lock(&priv->lock);
  if (priv->sources[i].st_enabled)
    goto ret;

  g_ctx = priv->sources[i].g_ctx;

  err = tier4_max9296_setup_pipeline(dev, g_ctx);

  if (err)
  {
    dev_err(dev, "[%s] : tier4_max9296_setup_pipeline() failed.\n", __func__);
    goto ret;
  }

  /* Derive CSI lane map register */
  switch (g_ctx->dst_csi_port)
  {
    case GMSL_CSI_PORT_A:
    case GMSL_CSI_PORT_D:
      lane_ctrl_addr = MAX9296_LANE_CTRL1_ADDR;
      break;
    case GMSL_CSI_PORT_B:
    case GMSL_CSI_PORT_E:
      lane_ctrl_addr = MAX9296_LANE_CTRL2_ADDR;
      break;
    case GMSL_CSI_PORT_C:
      lane_ctrl_addr = MAX9296_LANE_CTRL0_ADDR;
      break;
    case GMSL_CSI_PORT_F:
      lane_ctrl_addr = MAX9296_LANE_CTRL3_ADDR;
      break;
    default:
      dev_err(dev, "[%s] : Invalid GMSL CSI port!\n", __func__);
      err = -EINVAL;
      goto ret;
  };

  /*
   * rewrite num_lanes to same dst port should not be an issue,
   * as the device compatibility is already
   * checked during sdev registration against the des properties.
   */

  tier4_max9296_write_reg(dev, lane_ctrl_addr, MAX9296_LANE_CTRL_MAP(g_ctx->num_csi_lanes - 1));

  if (!priv->lane_setup)
  {
    tier4_max9296_write_reg(dev, MAX9296_DST_CSI_MODE_ADDR, priv->csi_mode);
    tier4_max9296_write_reg(dev, MAX9296_LANE_MAP1_ADDR, priv->lane_mp1);
    tier4_max9296_write_reg(dev, MAX9296_LANE_MAP2_ADDR, priv->lane_mp2);
    if (g_ctx->hardware_model == HW_MODEL_NVIDIA_ORIN_DEVKIT)
    {
      tier4_max9296_write_reg(dev, MAX9296_PHY1_CLK_ADDR, MAX9296_PHY1_CLK_1400MHZ);
    }
    else if (g_ctx->hardware_model == HW_MODEL_ADLINK_ROSCUBE_ORIN)
    {
      tier4_max9296_write_reg(dev,
                              //                MAX9296_PHY1_CLK_ADDR, MAX9296_PHY1_CLK_2500MHZ);
                              MAX9296_PHY1_CLK_ADDR, MAX9296_PHY1_CLK_1600MHZ);
    }
    else if (g_ctx->hardware_model == HW_MODEL_ADLINK_ROSCUBE)
    {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 65)
      //            tier4_max9296_write_reg(dev, MAX9296_PHY1_CLK_ADDR, MAX9296_PHY1_CLK_1400MHZ);
      tier4_max9296_write_reg(dev, MAX9296_PHY1_CLK_ADDR, MAX9296_PHY1_CLK_1500MHZ);
#else
      tier4_max9296_write_reg(dev, MAX9296_PHY1_CLK_ADDR, MAX9296_PHY1_CLK_1800MHZ);
#endif
    }
    else
    {
      tier4_max9296_write_reg(dev, MAX9296_PHY1_CLK_ADDR, MAX9296_PHY1_CLK_1400MHZ);
    }

    priv->lane_setup = true;
  }

#if 0
    tier4_max9296_write_reg(dev, MAX9296_REG5_ADDR, 0xC0);          // informed from Maxim
#endif
  priv->sources[i].st_enabled = true;

ret:
  mutex_unlock(&priv->lock);
  return err;
}
EXPORT_SYMBOL(tier4_max9296_setup_streaming);

const struct of_device_id tier4_max9296_of_match[] = {
  {
      .compatible = "nvidia,tier4_max9296",
  },
  {},
};
MODULE_DEVICE_TABLE(of, tier4_max9296_of_match);

static int tier4_max9296_parse_dt(struct tier4_max9296 *priv, struct i2c_client *client)
{
  struct device_node *node = client->dev.of_node;
  int err = 0;
  const char *str_value;
  int value;
  const struct of_device_id *match;

  if (!node)
    return -EINVAL;

  match = of_match_device(tier4_max9296_of_match, &client->dev);
  if (!match)
  {
    dev_err(&client->dev, "[%s] : Failed to match device with dt id\n", __func__);
    return -EFAULT;
  }

  err = of_property_read_string(node, "csi-mode", &str_value);
  if (err < 0)
  {
    dev_err(&client->dev, "[%s] : csi-mode property not found\n", __func__);
    return err;
  }

  if (!strcmp(str_value, "2x4"))
  {
    priv->csi_mode = MAX9296_CSI_MODE_2X4;
    priv->lane_mp1 = MAX9296_LANE_MAP1_2X4;
    priv->lane_mp2 = MAX9296_LANE_MAP2_2X4;
  }
  else if (!strcmp(str_value, "4x2"))
  {
    priv->csi_mode = MAX9296_CSI_MODE_4X2;
    priv->lane_mp1 = MAX9296_LANE_MAP1_4X2;
    priv->lane_mp2 = MAX9296_LANE_MAP2_4X2;
  }
  else
  {
    dev_err(&client->dev, "[%s] : Invalid csi mode\n", __func__);
    return -EINVAL;
  }

  err = of_property_read_u32(node, "max-src", &value);
  if (err < 0)
  {
    dev_err(&client->dev, "[%s] : No max-src info\n", __func__);
    return err;
  }

  priv->max_src = value;

  priv->reset_gpio = of_get_named_gpio(node, "reset-gpios", 0);
  if (priv->reset_gpio < 0)
  {
    dev_err(&client->dev, "[%s] : reset-gpios not found %d\n", __func__, err);
    return err;
  }

  /* digital 1.2v */
  if (of_get_property(node, "vdd_cam_1v2-supply", NULL))
  {
    priv->vdd_cam_1v2 = regulator_get(&client->dev, "vdd_cam_1v2");
    if (IS_ERR(priv->vdd_cam_1v2))
    {
      dev_err(&client->dev, "[%s] : vdd_cam_1v2 regulator get failed\n", __func__);
      err = PTR_ERR(priv->vdd_cam_1v2);
      priv->vdd_cam_1v2 = NULL;
      return err;
    }
  }
  else
  {
    priv->vdd_cam_1v2 = NULL;
  }

  return 0;
}

static struct regmap_config tier4_max9296_regmap_config = {
  .reg_bits = 16,
  .val_bits = 8,
  .cache_type = REGCACHE_RBTREE,
};

static int tier4_max9296_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  struct tier4_max9296 *priv;
  int err = 0;

  dev_info(&client->dev, "[%s]: Probing GMSL deserializer\n", __func__);

  priv = devm_kzalloc(&client->dev, sizeof(*priv), GFP_KERNEL);
  priv->i2c_client = client;
  priv->regmap = devm_regmap_init_i2c(priv->i2c_client, &tier4_max9296_regmap_config);
  if (IS_ERR(priv->regmap))
  {
    dev_err(&client->dev, "[%s] : devm_regmap_init() failed: %ld\n", __func__, PTR_ERR(priv->regmap));
    return -ENODEV;
  }

  err = tier4_max9296_parse_dt(priv, client);
  if (err)
  {
    dev_err(&client->dev, "[%s] : Unable to parse dt\n", __func__);
    return -EFAULT;
  }

  tier4_max9296_pipes_reset(priv);

  if (priv->max_src > MAX9296_MAX_SOURCES)
  {
    dev_err(&client->dev, "[%s] : Max sources more than currently supported\n", __func__);
    return -EINVAL;
  }

  mutex_init(&priv->lock);

  dev_set_drvdata(&client->dev, priv);

  /* dev communication gets validated when GMSL link setup is done */
  dev_info(&client->dev, "[%s] : Probing GMSL Deserializer is done.\n", __func__);

  return err;
}

static int tier4_max9296_remove(struct i2c_client *client)
{
  struct tier4_max9296 *priv;

  if (client != NULL)
  {
    priv = dev_get_drvdata(&client->dev);
    mutex_destroy(&priv->lock);
  }

  return 0;
}

static const struct i2c_device_id tier4_max9296_id[] = {
  { "tier4_max9296", 0 },
  {},
};

MODULE_DEVICE_TABLE(i2c, tier4_max9296_id);

static struct i2c_driver tier4_max9296_i2c_driver = {
    .driver = {
        .name = "tier4_max9296",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(tier4_max9296_of_match),
    },
    .probe = tier4_max9296_probe,
    .remove = tier4_max9296_remove,
    .id_table = tier4_max9296_id,
};

static int __init tier4_max9296_init(void)
{
  printk(KERN_INFO "MAX9296 Driver for TIERIV Camera : %s\n", BUILD_STAMP);
  return i2c_add_driver(&tier4_max9296_i2c_driver);
}

static void __exit tier4_max9296_exit(void)
{
  i2c_del_driver(&tier4_max9296_i2c_driver);
}

module_init(tier4_max9296_init);
module_exit(tier4_max9296_exit);

MODULE_DESCRIPTION("Dual GMSL Deserializer driver tier4_max9296");
MODULE_AUTHOR("Originaly NVIDIA Corporation");
MODULE_AUTHOR("K.Iwasaki");
MODULE_AUTHOR("Y.Fujii");
MODULE_LICENSE("GPL v2");
