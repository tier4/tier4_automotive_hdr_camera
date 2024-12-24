/*
 * Copyright (c) 2024, ADLINK Technology  All rights reserved.
 *
 */

#ifndef __MAX20089_H__
#define __MAX20089_H__

/* Register Offset */
#define MAX20089_REG_MASK		0x00
#define MAX20089_REG_CONFIG		0x01
#define	MAX20089_REG_ID			0x02
#define	MAX20089_REG_STAT1		0x03
#define	MAX20089_REG_STAT2_L		0x04
#define	MAX20089_REG_STAT2_H		0x05
#define	MAX20089_REG_ADC1		0x06
#define	MAX20089_REG_ADC2		0x07
#define	MAX20089_REG_ADC3		0x08
#define	MAX20089_REG_ADC4		0x09

/* DEVICE IDs */
#define MAX20089_DEVICE_ID_MAX20086	0x40
#define MAX20089_DEVICE_ID_MAX20087	0x20
#define MAX20089_DEVICE_ID_MAX20088	0x10
#define MAX20089_DEVICE_ID_MAX20089	0x00
#define DEVICE_ID_MASK			0xf0

/* Register bits */
#define MAX20089_EN_MASK		0x0f
#define MAX20089_EN_OUT1		0x01
#define MAX20089_EN_OUT2		0x02
#define MAX20089_EN_OUT3		0x04
#define MAX20089_EN_OUT4		0x08
#define MAX20089_INT_DISABLE_ALL	0x3f

/**
 * @brief power on camera module by writing i2c commands to max20089
 * 
 * @param dev           pointer to max20089 device structure.
 * @param output_index  0x01 for OUT1, 0x02 for OUT2
 * @return 0 for success, or -1 otherwise.
 */
int max20089_power_on_cam(struct device *dev, int output_index);

/**
 * @brief power off camera module by writing i2c commands to max20089
 * 
 * @param dev           pointer to max20089 device structure.
 * @param output_index  0x01 for OUT1, 0x02 for OUT2
 * @return 0 for success, or -1 otherwise.
 */
int max20089_power_off_cam(struct device *dev, int output_index);

#endif