/*
 * tier4_fpga.c - tier4_fpga driver
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

//#include <linux/debugfs.h>

#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/regmap.h>
#include <linux/string.h>

#include "tier4-gmsl-link.h"
#include "tier4-fpga.h"

#define FPGA_ENABLED 0xF0
#define FPGA_DISABLED 0xFF
#define NO_ERROR 0

#define FPGA_REG_ENABLE_ADDR 0x4
#define FPGA_REG_FREQ_BASE_ADDR 0x8

struct tier4_fpga
{
  struct i2c_client *i2c_client;
  struct regmap *regmap;
  struct gmsl_link_ctx g_ctx;
  struct mutex lock;
  /* FPGA slave address */
  __u32 reg_addr;
  bool generate_fsync;
};

static int generate_fsync;

module_param(generate_fsync, int, S_IRUGO | S_IWUSR);

static struct regmap_config tier4_fpga_regmap_config = {
  .reg_bits = 8,
  .val_bits = 8,
  .cache_type = REGCACHE_RBTREE,
};

/* count channel */
static __u32 channel_count_fpga; /* should be 1 */

static int tier4_fpga_read_reg(struct device *dev, u16 addr, u8 *val)
{
  int err = 0;
  u32 reg_val = 0;
  struct tier4_fpga *priv = dev_get_drvdata(dev);
  err = regmap_read(priv->regmap, addr, &reg_val);

  *val = reg_val & 0xFF;

  dev_dbg(dev, "[%s ] : FPGA I2C Read  : Reg Address = 0x%04X Data= 0x%02X.\n", __func__, addr, *val);

  return err;
}

static int tier4_fpga_write_reg(struct device *dev, u16 addr, u8 val)
{
  struct tier4_fpga *priv = dev_get_drvdata(dev);
  int err = 0;
  u8 e;
  char str_bus_num[4], str_sl_addr[4];
  int len;

  memset(str_bus_num, 0, 4);
  memset(str_sl_addr, 0, 4);

  len = strlen(dev->kobj.name);

  if ((err == 0) && (dev != NULL))
  {
    strncpy(str_bus_num, &dev->kobj.name[0], 2);
    strncpy(str_sl_addr, &dev->kobj.name[len - 2], 2);
  }

  dev_dbg(dev, "[%s] : FPGA I2C Write : Reg Address = 0x%04X Data= 0x%02X.\n", __func__, addr, val);

  err = regmap_write(priv->regmap, addr, val);

  if (err)
  {
    dev_err(dev, "[%s] : FPGA I2C write failed.    Reg Address = 0x%04X  Data= 0x%02X.\n", __func__, addr, val);
  }

  /* delay before next i2c command as required for SERDES link */

  usleep_range(100, 110);
  tier4_fpga_read_reg(dev, addr, &e);
  usleep_range(100, 110);

  return err;
}

int tier4_fpga_enable_generate_fsync_signal(struct device *dev)
{
  //	struct tier4_fpga *priv = dev_get_drvdata(dev);
  int err = 0;

  err = tier4_fpga_write_reg(dev, FPGA_REG_ENABLE_ADDR, FPGA_ENABLED);

  if (err)
  {
    dev_err(dev, "[%s] Enabling generation of fsync signal failed.\n", __func__);
  }
  else
  {
    dev_info(dev, "[%s] Enabling generation of fsync signal.\n", __func__);
  }
  return err;
}
EXPORT_SYMBOL(tier4_fpga_enable_generate_fsync_signal);

int tier4_fpga_disable_generate_fsync_signal(struct device *dev)
{
  int err = 0;

  err = tier4_fpga_write_reg(dev, FPGA_REG_ENABLE_ADDR, FPGA_DISABLED);

  if (err)
  {
    dev_err(dev, "[%s] Disabling generation of fsync signal failed.\n", __func__);
  }
  else
  {
    dev_info(dev, "[%s] Disabling generation of fsync signal.\n", __func__);
  }
  return err;
}
EXPORT_SYMBOL(tier4_fpga_disable_generate_fsync_signal);

int tier4_fpga_check_access(struct device *dev)
{
  int err;
  u8 dummy8 = 0;

  err = tier4_fpga_read_reg(dev, FPGA_REG_ENABLE_ADDR, &dummy8);

  if (err)
  {
    dev_err(dev, "[%s] :  Accessing FPGA failed.\n", __func__);
    return err;
  }
  else
  {
    dev_dbg(dev, "[%s] :  Accessing FPGA succeeded.\n", __func__);
  }

  return NO_ERROR;
}
EXPORT_SYMBOL(tier4_fpga_check_access);

int tier4_fpga_set_fsync_signal_frequency(struct device *dev, int des_number, int frequency)
{
  int err = 0;
  u8 val8, addr8;

  addr8 = (u8)(FPGA_REG_FREQ_BASE_ADDR + 12 * (des_number - 1));

  val8 = (u8)(frequency & 0xFF);

  err = tier4_fpga_write_reg(dev, addr8, val8);

  if (err)
  {
    dev_err(dev, "[%s] :  Setting the frequency of fsync pulse failed.\n", __func__);
    return err;
  }
  else
  {
    dev_dbg(dev, "[%s] :  Set the frequency of fsync pulse.\n", __func__);
  }

  return NO_ERROR;
}
EXPORT_SYMBOL(tier4_fpga_set_fsync_signal_frequency);

int tier4_fpga_get_slave_addr(struct device *dev, struct gmsl_link_ctx *g_ctx)
{
  struct tier4_fpga *priv = dev_get_drvdata(dev);

  if (!g_ctx)
  {
    dev_err(dev, "[%s] : g_ctx is null\n", __func__);
    return -1;
  }

  g_ctx->sdev_fpga_reg = priv->reg_addr;

  return 0;
}
EXPORT_SYMBOL(tier4_fpga_get_slave_addr);

const struct of_device_id tier4_fpga_of_match[] = {
  {
      .compatible = "nvidia,tier4_fpga",
  },
  {},
};
MODULE_DEVICE_TABLE(of, tier4_fpga_of_match);

// -----------------------------------------------------

static int tier4_fpga_parse_dt(struct tier4_fpga *priv, struct i2c_client *client)
{
  struct device_node *node = client->dev.of_node;
  int err = 0;
  //	const char 					*str_value;
  //	int 						value;
  //	const struct of_device_id 	*match;

  if (!node)
    return -EINVAL;

#if 0
	match = of_match_device(tier4_fpga_of_match, &client->dev);
	if (!match) {
		dev_err(&client->dev, "[%s] : Failed to match fpga device with dt id\n", __func__);
		return -EFAULT;
	}

	priv->g_ctx.fpga_generate_fsync = false;

	err = of_property_read_string(node, "generate-fsync", &str_value);
	if (err < 0) {
		dev_info(&client->dev, "[%s] : generate-fsync property not found and disabled gneneration of fsync.\n", __func__);
	} else {
		if (!strcmp(str_value, "true")) {
			priv->g_ctx.fpga_generate_fsync = true;
		}
	}

	dev_info(&client->dev, "[%s] : generate-fsync = %d.\n", __func__, priv->g_ctx.fpga_generate_fsync );
#endif

  priv->g_ctx.fpga_generate_fsync = generate_fsync;

  dev_info(&client->dev, "[%s] : generate-fsync = %d\n", __func__, priv->g_ctx.fpga_generate_fsync);

  err = of_property_read_u32(node, "reg", &priv->reg_addr);
  if (err < 0)
  {
    dev_err(&client->dev, "[%s] : fpga reg not found\n", __func__);
    return -EINVAL;
  }

  priv->g_ctx.sdev_fpga_reg = priv->reg_addr;

  dev_dbg(&client->dev, "[%s] : fpga slave addr = 0x%0x.\n", __func__, priv->reg_addr);

  return NO_ERROR;
}

// ---------------------------------------------------------

static int tier4_fpga_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  struct tier4_fpga *priv;
  int err = 0;

  dev_info(&client->dev, "[%s] : probing FPGA\n", __func__);

  priv = devm_kzalloc(&client->dev, sizeof(*priv), GFP_KERNEL);
  priv->i2c_client = client;

  priv->regmap = devm_regmap_init_i2c(priv->i2c_client, &tier4_fpga_regmap_config);
  if (IS_ERR(priv->regmap))
  {
    dev_err(&client->dev, "[%s] : devm_regmap_init() failed: %ld\n", __func__, PTR_ERR(priv->regmap));
    return -ENODEV;
  }

  err = tier4_fpga_parse_dt(priv, client);
  if (err)
  {
    dev_err(&client->dev, "[%s] : Unable to parse fpga dt\n", __func__);
    return -EFAULT;
  }

  mutex_init(&priv->lock);

  dev_set_drvdata(&client->dev, priv);

  // default mode is disabling gnereation of fsync

  err = tier4_fpga_disable_generate_fsync_signal(&client->dev);

  if (err < 0)
  {
    dev_err(&client->dev, "[%s] : Unable to disable generation of fsync.\n", __func__);
    return -EINVAL;
  }

  channel_count_fpga++;

  dev_info(&client->dev, "[%s] :  Probing FPGA succeeded.\n", __func__);

  return err;
}

static int tier4_fpga_remove(struct i2c_client *client)
{
  struct tier4_fpga *priv;

  if (channel_count_fpga > 0)
    channel_count_fpga--;

  if (client != NULL)
  {
    priv = dev_get_drvdata(&client->dev);
    mutex_destroy(&priv->lock);
    i2c_unregister_device(client);
    client = NULL;
  }

  return 0;
}

static const struct i2c_device_id tier4_fpga_id[] = {
  { "tier4_fpga", 0 },
  {},
};

MODULE_DEVICE_TABLE(i2c, tier4_fpga_id);

static struct i2c_driver tier4_fpga_i2c_driver = {
	.driver = {
		.name = "tier4_fpga",
		.owner = THIS_MODULE,
	},
	.probe = tier4_fpga_probe,
	.remove = tier4_fpga_remove,
	.id_table = tier4_fpga_id,
};

static int __init tier4_fpga_init(void)
{
  printk("FPGA Driver for Tier4 Cameras.\n");

  return i2c_add_driver(&tier4_fpga_i2c_driver);
}

static void __exit tier4_fpga_exit(void)
{
  i2c_del_driver(&tier4_fpga_i2c_driver);
}

module_init(tier4_fpga_init);
module_exit(tier4_fpga_exit);

MODULE_DESCRIPTION("FPGA driver tier4_fpga");
MODULE_AUTHOR("Kohji Iwasaki");
MODULE_LICENSE("GPL v2");
