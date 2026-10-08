/*
 * Copyright (c) 2024, TIERIV INC.  All rights reserved.
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

/**
 * @file
 * <b>TIER IV on-board camera EEPROM helpers.</b>
 *
 * @b Description: Shared read + parse of the device-info block stored in a
 *  TIER IV GMSL camera module's on-board EEPROM.  Used identically by the
 *  isx021 (C1/C1MP), imx490 (C2/C2MP) and imx728 (C3/C3MP) sensor drivers.
 *
 *  Header-only: the helper bodies are `static __maybe_unused` so every
 *  including translation unit gets its own copy and no Makefile / DKMS /
 *  packaging change is needed (imx728 does not use tier4_eeprom_type_hint).
 */

#ifndef __TIER4_EEPROM_H__
#define __TIER4_EEPROM_H__

#include <linux/compiler.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/string.h>
#include <linux/types.h>

#include "tier4-gmsl-link.h"

/* Location and size of the device-info block. */
#define TIER4_EEPROM_DEVICE_INFO_OFFSET 0x0100
#define TIER4_EEPROM_DEVICE_INFO_SIZE 80
/* Capacity of each parsed device-info field, including the NUL. */
#define TIER4_EEPROM_FIELD_MAX_LEN 32
/*
 * Untranslated i2c address of the on-board EEPROM, used by
 * tier4_eeprom_type_hint() before the serializer's address translation is
 * programmed (tier4_eeprom_read_device_info() goes through the per-port
 * proxy address g_ctx.eeprom_reg instead).
 */
#define TIER4_EEPROM_HW_ADDR 0x54

/* Parsed device-info fields read from the camera's on-board EEPROM. */
struct tier4_eeprom_info {
	bool valid;
	char serial_number[TIER4_EEPROM_FIELD_MAX_LEN];
	char short_model[TIER4_EEPROM_FIELD_MAX_LEN];
	char model_name[TIER4_EEPROM_FIELD_MAX_LEN];
	char part_number[TIER4_EEPROM_FIELD_MAX_LEN];
	char region_code[TIER4_EEPROM_FIELD_MAX_LEN];
};

/*
 * Guards against a blank or garbage EEPROM (a standard camera may still
 * ACK at the proxy address).
 */
static bool __maybe_unused tier4_eeprom_devinfo_valid(const u8 *buf, size_t size)
{
	size_t len;
	int stars = 0;

	for (len = 0; len < size && buf[len]; len++) {
		if (buf[len] < 0x20 || buf[len] > 0x7e)
			return false;
		if (buf[len] == '*')
			stars++;
	}
	return len >= 16 && len < size && stars >= 4 && buf[0] != '*';
}

/*
 * Reads and parses the device-info block at @addr on @adap.  @addr is
 * normally the per-port proxy address (g_ctx.eeprom_reg) that
 * tier4_max9295_setup_control_routing() programs into the serializer.
 * Returns -ENODEV when @addr is 0 (no EEPROM node in devicetree), a
 * negative errno on i2c failure or a failed sanity check -- in which cases
 * @info->valid is left false and the EEPROM V4L2 controls report empty
 * strings -- and 0 on success, with @info filled and @info->valid set.
 */
static int __maybe_unused
tier4_eeprom_read_device_info(struct i2c_adapter *adap, u16 addr,
			      struct tier4_eeprom_info *info,
			      struct device *dev_for_log)
{
	struct i2c_msg msg[2];
	u8 w_buf[2] = {
		(TIER4_EEPROM_DEVICE_INFO_OFFSET >> 8) & 0xFF,
		TIER4_EEPROM_DEVICE_INFO_OFFSET & 0xFF,
	};
	u8 r_buf[TIER4_EEPROM_DEVICE_INFO_SIZE];
	char *raw;
	char *tok;
	char *fields[5] = { "", "", "", "", "" };
	int i;
	int err;

	if (!addr)
		return -ENODEV;

	msg[0].addr = addr;
	msg[0].flags = 0; // I2C Write
	msg[0].len = sizeof(w_buf);
	msg[0].buf = w_buf;

	msg[1].addr = addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len = sizeof(r_buf);
	msg[1].buf = r_buf;

	err = i2c_transfer(adap, msg, 2);
	if (err != 2) {
		dev_info(dev_for_log,
			"[%s] : EEPROM read failed (err=%d); no on-board EEPROM at proxy addr 0x%02x, or camera is a standard model\n",
			__func__, err, addr);
		return err < 0 ? err : -EIO;
	}

	if (!tier4_eeprom_devinfo_valid(r_buf, sizeof(r_buf))) {
		dev_info(dev_for_log,
			"[%s] : EEPROM at proxy 0x%02x returned non device-info data; ignoring\n",
			__func__, addr);
		return -EINVAL;
	}

	// Guarantee NUL-termination before splitting into fields.
	r_buf[sizeof(r_buf) - 1] = '\0';
	raw = (char *)r_buf;
	for (i = 0; i < 5 && (tok = strsep(&raw, "*")) != NULL; i++)
		fields[i] = tok;

	strscpy(info->serial_number, fields[0], sizeof(info->serial_number));
	strscpy(info->short_model, fields[1], sizeof(info->short_model));
	strscpy(info->model_name, fields[2], sizeof(info->model_name));
	strscpy(info->part_number, fields[3], sizeof(info->part_number));
	strscpy(info->region_code, fields[4], sizeof(info->region_code));
	info->valid = true;

	dev_info(dev_for_log, "[%s] : EEPROM Serial Number=%s Model Name=%s\n",
		__func__, info->serial_number, info->model_name);

	return 0;
}

/*
 * Classifies the camera from the on-board EEPROM's ShortModel field, read
 * at @addr on @adap (TIER4_EEPROM_HW_ADDR, reachable right after the
 * deserializer link is up, before any serializer register write).  Returns
 * TIER4_CAMERA_TYPE_UNKNOWN when the EEPROM is absent or unreadable -- the
 * normal case for a standard camera -- so the caller falls back to
 * sensor-identity probing.
 */
static enum tier4_camera_type __maybe_unused
tier4_eeprom_type_hint(struct i2c_adapter *adap, u16 addr,
		       struct device *dev_for_log)
{
	u8 w_buf[2] = {
		(TIER4_EEPROM_DEVICE_INFO_OFFSET >> 8) & 0xFF,
		TIER4_EEPROM_DEVICE_INFO_OFFSET & 0xFF,
	};
	u8 r_buf[TIER4_EEPROM_DEVICE_INFO_SIZE];
	struct i2c_msg msg[2] = {
		{ .addr = addr, .flags = 0,
		  .len = sizeof(w_buf), .buf = w_buf },
		{ .addr = addr, .flags = I2C_M_RD,
		  .len = sizeof(r_buf), .buf = r_buf },
	};
	char *raw, *short_model;
	size_t n;

	if (i2c_transfer(adap, msg, 2) != 2)
		return TIER4_CAMERA_TYPE_UNKNOWN;
	if (!tier4_eeprom_devinfo_valid(r_buf, sizeof(r_buf)))
		return TIER4_CAMERA_TYPE_UNKNOWN;

	r_buf[sizeof(r_buf) - 1] = '\0';
	raw = (char *)r_buf;
	strsep(&raw, "*");			/* [0] serial number */
	short_model = strsep(&raw, "*");		/* [1] ShortModel */
	if (!short_model || short_model[0] != 'C' ||
	    (short_model[1] != '1' && short_model[1] != '2'))
		return TIER4_CAMERA_TYPE_UNKNOWN;

	n = strlen(short_model);
	if (n >= 2 && short_model[n - 2] == 'M' && short_model[n - 1] == 'P') {
		dev_info(dev_for_log,
			 "[%s] : EEPROM ShortModel=%s -> MP\n", __func__,
			 short_model);
		return TIER4_CAMERA_TYPE_MP;
	}
	dev_info(dev_for_log,
		 "[%s] : EEPROM ShortModel=%s -> standard\n", __func__,
		 short_model);
	return TIER4_CAMERA_TYPE_STANDARD;
}

#endif /* __TIER4_EEPROM_H__ */
