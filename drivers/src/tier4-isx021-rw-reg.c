/*
 * tier4-isx021-rw-reg.c - isx021 sensor registers read and write register driver
 *
 * Copyright (c) 2022, TIERIV Inc.  All rights reserved.
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
#include <media/tegracam_core.h>

/* ---------------------------------------------------------------------------*/

void micro_sec_sleep(unsigned int u_time)
{
	usleep_range(u_time, u_time + u_time/10);
}
EXPORT_SYMBOL(micro_sec_sleep);

/* ---------------------------------------------------------------------------*/

int tier4_isx021_read_reg(struct camera_common_data *s_data, u16 addr, u8 *val)
{
	int err 	= 0;
	u32 reg_val = 0;

	err = regmap_read(s_data->regmap, addr, &reg_val);

	if (err) {
		dev_err(s_data->dev, "[%s] : I2C read failed, address = 0x%x\n", __func__, addr);
	} else {
		*val = reg_val & 0xFF;
	}
	return err;
}
EXPORT_SYMBOL(tier4_isx021_read_reg);

/* ---------------------------------------------------------------------------*/

int tier4_isx021_write_reg(struct camera_common_data *s_data, u16 addr, u8 val)
{

	int 			err 	= 0;
	struct 	device	*pdev 	= s_data->dev;
	char str_bus_num[4],str_sl_addr[4];
	int				len;

	memset(str_bus_num,0,4);
	memset(str_sl_addr,0,4);

	len = strlen(pdev->kobj.name);

	if (pdev) {
		strncpy(str_bus_num, &pdev->kobj.name[0], 2);
		strncpy(str_sl_addr, &(pdev->kobj.name[len-2]), 2);
	}
	err = regmap_write(s_data->regmap, addr, val);

	if (err) {
		dev_err(s_data->dev,  "[%s] : I2C write failed.    Reg Address = 0x%x  Data = 0x%x\n", __func__, addr, val);
	}

	return err;
}
EXPORT_SYMBOL(tier4_isx021_write_reg);

/* --------------------------------------------------------------------------- */

int tier4_isx021_read_reg_wrapper(void *p_s_data, u16 reg_addr, u8 *val)
{
	int err = 0;
	struct camera_common_data *s_data = (struct camera_common_data *)p_s_data;
//	struct tier4_isx021 *priv = (struct tier4_isx021 *)s_data->priv;

	err = tier4_isx021_read_reg(s_data, reg_addr, val);

	return err;
}
EXPORT_SYMBOL(tier4_isx021_read_reg_wrapper);

/* --------------------------------------------------------------------------- */

int tier4_isx021_write_reg_wrapper(void *p_s_data, u16 reg_addr, u8 val)
{

	int err = 0;
	
	struct camera_common_data *s_data = (struct camera_common_data *)p_s_data;
//	struct tier4_isx021 *priv = (struct tier4_isx021 *)s_data->priv;

	err = tier4_isx021_write_reg(s_data, reg_addr, val);
	
	return err;
}
EXPORT_SYMBOL(tier4_isx021_write_reg_wrapper);

/* --------------------------------------------------------------------------- */

static int __init tier4_isx021_rw_reg_init(void)
{

	printk("ISX021 Sensor driver to read and write registers on ROScube.\n");

	return 0;
}

/* --------------------------------------------------------------------------- */

static void __exit tier4_isx021_rw_reg_exit(void)
{
	usleep_range(100,110);
}

module_init(tier4_isx021_rw_reg_init);
module_exit(tier4_isx021_rw_reg_exit);

MODULE_DESCRIPTION("TIERIV Automotive HDR Camera read and write register driver");
MODULE_AUTHOR("K.Iwasaki");
MODULE_AUTHOR("Y.Fujii");
MODULE_LICENSE("GPL v2");

