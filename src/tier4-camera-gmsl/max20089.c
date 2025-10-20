// SPDX-License-Identifier: GPL-2.0-or-later
//
// max20089.c - Camera power protector driver for MAX20086-MAX20089
//
// Copyright (C) 2024 Ting Chang <ting.chang@adlinktech.com>

#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/regmap.h>

#include "max20089.h"

struct max20089_chip_info {
	u8 id;
	unsigned int num_outputs;
};

struct max20089 {
	struct device *dev;
	struct regmap *regmap;
	const struct max20089_chip_info *info;
};

static int max20089_read_reg(struct device *dev, u16 addr, u8 *val)
{
	int err = 0;
	u32 reg_val = 0;
	struct max20089 *chip = dev_get_drvdata(dev);

	err = regmap_read(chip->regmap, addr, &reg_val);

	*val = reg_val & 0xFF;

	return err;
}

static int max20089_write_reg(struct device *dev, u16 addr, u8 val)
{
  struct max20089 *chip = dev_get_drvdata(dev);
  int err = 0;

  err = regmap_write(chip->regmap, addr, val);

  if (err)
  {
    dev_err(dev, "[%s] MAX20089 I2C write Reg at 0x%04X:[0x%02X] failed.\n", __func__, addr, val);
  }
#ifdef DEBUG_INFO
  else
  {
    dev_info(dev, "[%s] MAX20089 I2C write Reg at 0x%04X:[0x%02X].\n", __func__, addr, val);
  }
#endif

  usleep_range(200, 220);

  return err;
}

int max20089_power_on_cam(struct device *dev, int output_index)
{
	int err = 0;
	u8 val8 = 0;
	u8 read8;

	dev_info(dev, "%s: power on OUT-%d", __func__, output_index);

	if (output_index != MAX20089_EN_OUT1 && output_index != MAX20089_EN_OUT2)
		dev_err(dev, "[%s]: Output index is invalid.\n", __func__);

	// read current value
	err = max20089_read_reg(dev, MAX20089_REG_CONFIG, &read8);
	if (err)
	{
		dev_err(dev, "[%s] Failed to read MAX20089 register.\n", __func__);
		return err;
	}

	// enable the target output power
	val8 = read8 | output_index;

	err = max20089_write_reg(dev, MAX20089_REG_CONFIG, val8);
	if (err)
	{
		dev_err(dev, "[%s]: Accessing MAX20089 failed.\n", __func__);
		return err;
	}

	return 0;
}
EXPORT_SYMBOL(max20089_power_on_cam);

int max20089_power_off_cam(struct device *dev, int output_index)
{
	int err = 0;
	u8 val8 = 0;
	u8 read8;

	dev_info(dev, "%s: power off OUT-%d", __func__, output_index);

	if (output_index != MAX20089_EN_OUT1 && output_index != MAX20089_EN_OUT2)
		dev_err(dev, "[%s]: Output index is invalid.\n", __func__);

	// read current value
	max20089_read_reg(dev, MAX20089_REG_CONFIG, &read8);

	// disable the target output power
	val8 = read8 & (~output_index);

	err = max20089_write_reg(dev, MAX20089_REG_CONFIG, val8);
	if (err)
	{
		dev_err(dev, "[%s]: Accessing MAX20089 failed.\n", __func__);
		return err;
	}

	return 0;
}
EXPORT_SYMBOL(max20089_power_off_cam);


static int max20089_parse_regulators_dt(struct max20089 *chip, struct i2c_client *client)
{
	struct device_node *node = client->dev.of_node;
	int err = 0;
	__u32 val;

	if (!node)
	return -EINVAL;

	err = of_property_read_u32(node, "reg", &val);
	if (err < 0)
	{
		dev_err(chip->dev, "[%s] max20089 reg not found\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static int max20089_detect(struct max20089 *chip)
{
	unsigned int data;
	int ret;

	ret = regmap_read(chip->regmap, MAX20089_REG_ID, &data);
	if (ret < 0) {
		dev_err(chip->dev, "Failed to read DEVICE_ID reg: %d\n", ret);
		return ret;
	}

	if ((data & DEVICE_ID_MASK) != chip->info->id) {
		dev_err(chip->dev, "Invalid device ID 0x%02x\n", data);
		return -ENXIO;
	}

	return 0;
}

static bool max20089_gen_is_writeable_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case MAX20089_REG_MASK:
	case MAX20089_REG_CONFIG:
		return true;
	default:
		return false;
	}
}

static const struct regmap_config max20089_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.writeable_reg = max20089_gen_is_writeable_reg,
	.max_register = 0x9,
	.cache_type = REGCACHE_NONE,
};

static int max20089_i2c_probe(struct i2c_client *i2c, const struct i2c_device_id *id)
{
	struct max20089 *chip;
	int ret;

	dev_info(&i2c->dev, "[%s] probing MAX20089\n", __func__);

	chip = devm_kzalloc(&i2c->dev, sizeof(*chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	chip->dev = &i2c->dev;
	chip->info = of_device_get_match_data(chip->dev);
	i2c_set_clientdata(i2c, chip);

	chip->regmap = devm_regmap_init_i2c(i2c, &max20089_regmap_config);
	if (IS_ERR(chip->regmap)) {
		ret = PTR_ERR(chip->regmap);
		dev_err(chip->dev, "Failed to allocate register map: %d\n", ret);
		return ret;
	}

	ret = max20089_parse_regulators_dt(chip, i2c);
	if (ret < 0)
		return ret;

  	dev_set_drvdata(chip->dev, chip);

	ret = max20089_detect(chip);
	if (ret < 0)
		return ret;

	/* Until IRQ support is added, just disable all interrupts. */
	ret = regmap_update_bits(chip->regmap, MAX20089_REG_MASK,
				 MAX20089_INT_DISABLE_ALL,
				 MAX20089_INT_DISABLE_ALL);
	if (ret < 0) {
		dev_err(chip->dev, "Failed to disable interrupts: %d\n", ret);
		return ret;
	}

	ret = regmap_update_bits(chip->regmap, MAX20089_REG_CONFIG,
		MAX20089_EN_MASK,
		~(MAX20089_EN_OUT1 | MAX20089_EN_OUT2));
	if (ret < 0) {
		dev_err(chip->dev, "Failed to disable all output: %d\n", ret);
		return ret;
	}

	return 0;
}

static const struct i2c_device_id max20089_i2c_id[] = {
	{ "max20086" },
	{ "max20087" },
	{ "max20088" },
	{ "max20089" },
	{ /* Sentinel */ },
};

MODULE_DEVICE_TABLE(i2c, max20089_i2c_id);

static const struct of_device_id max20089_dt_ids[] = {
	{
		.compatible = "maxim,max20086",
		.data = &(const struct max20089_chip_info) {
			.id = MAX20089_DEVICE_ID_MAX20086,
			.num_outputs = 4,
		}
	}, {
		.compatible = "maxim,max20087",
		.data = &(const struct max20089_chip_info) {
			.id = MAX20089_DEVICE_ID_MAX20087,
			.num_outputs = 4,
		}
	}, {
		.compatible = "maxim,max20088",
		.data = &(const struct max20089_chip_info) {
			.id = MAX20089_DEVICE_ID_MAX20088,
			.num_outputs = 2,
		}
	}, {
		.compatible = "maxim,max20089",
		.data = &(const struct max20089_chip_info) {
			.id = MAX20089_DEVICE_ID_MAX20089,
			.num_outputs = 2,
		}
	},
	{ /* Sentinel */ },
};

MODULE_DEVICE_TABLE(of, max20089_dt_ids);

static struct i2c_driver max20089_regulator_driver = {
	.driver = {
		.name = "max20089",
		.of_match_table = of_match_ptr(max20089_dt_ids),
	},
	.probe = max20089_i2c_probe,
	.id_table = max20089_i2c_id,
};

static int __init max20089_init(void)
{
  return i2c_add_driver(&max20089_regulator_driver);
}

static void __exit max20089_exit(void)
{
  i2c_del_driver(&max20089_regulator_driver);
}

module_init(max20089_init);
module_exit(max20089_exit);

MODULE_AUTHOR("Ting Chang <ting.chang@adlinktech.com>");
MODULE_DESCRIPTION("MAX20089 Camera Power Protector Driver");
MODULE_LICENSE("GPL");