/*
 * tier4-isx021-ctrl.c - isx021 sensor control driver
 *
 * Copyright (c) 2022, TIERIV Inc.  All rights reserved.
 *
 */

typedef signed char 		s8;
typedef unsigned char 		u8;
typedef signed short 		s16;
typedef unsigned short 		u16;
typedef signed int 			s32;
typedef unsigned int 		u32;
typedef signed long long 	s64;
typedef unsigned long long 	u64;

#include <linux/module.h>

#include "tier4-isx021-rw-reg.h"


// Register Address

#define	ISX021_SG_MODE_INTERNAL_SYNC				0

#define	ISX021_SG_MODE_INTERNAL_SYNC				0

#define ISX021_MIN_GAIN         					0
#define ISX021_MAX_GAIN         					48

#define ISX021_MIN_EXPOSURE_TIME   					0
#define ISX021_MAX_EXPOSURE_TIME   					33000	// 33 milisecond 

#define ISX021_GAIN_DEFAULT_VALUE					6

#define ISX021_DEFAULT_FRAME_LENGTH    				(1125)
#define ISX021_FRAME_LENGTH_ADDR_MSB            	0x200A
#define ISX021_FRAME_LENGTH_ADDR_MID    			0x2009
#define ISX021_FRAME_LENGTH_ADDR_LSB    			0x2008
#define ISX021_COARSE_TIME_SHS1_ADDR_MSB			0x000E
#define ISX021_COARSE_TIME_SHS1_ADDR_MID			0x000D
#define ISX021_COARSE_TIME_SHS1_ADDR_LSB        	0x000C
#define ISX021_COARSE_TIME_SHS2_ADDR_MSB			0x0012
#define ISX021_COARSE_TIME_SHS2_ADDR_MID			0x0011
#define ISX021_COARSE_TIME_SHS2_ADDR_LSB			0x0010
#define ISX021_GROUP_HOLD_ADDR    					0x0008
//#define ISX021_ANALOG_GAIN_SP1H_ADDR            	0x0018
//#define ISX021_ANALOG_GAIN_SP1M_ADDR    			0x0019
//#define ISX021_ANALOG_GAIN_SP1L_ADDR            	0x001A
#define ISX021_ANALOG_GAIN_SP1B_ADDR    			0x001B

#define ISX021_DEVSTATUS_ADDR						0x6005

//   AE  mode

#define ISX021_AE_SHUTTER0_VALUE_BYTE0_ADDR			0x616C
#define ISX021_AE_SHUTTER0_VALUE_BYTE1_ADDR			0x616D
#define ISX021_AE_SHUTTER0_VALUE_BYTE2_ADDR			0x616E
#define ISX021_AE_SHUTTER0_VALUE_BYTE3_ADDR			0x616F

#define ISX021_AE_SHUTTER1_VALUE_BYTE0_ADDR			0x647C
#define ISX021_AE_SHUTTER1_VALUE_BYTE1_ADDR			0x647D
#define ISX021_AE_SHUTTER1_VALUE_BYTE2_ADDR			0x647E
#define ISX021_AE_SHUTTER1_VALUE_BYTE3_ADDR			0x647F

//  ME  mode

#define ISX021_ME_SHUTTER0_DEFAULT_VALUE_BYTE0_ADDR	0xABEC
#define ISX021_ME_SHUTTER0_DEFAULT_VALUE_BYTE1_ADDR	0xABED
#define ISX021_ME_SHUTTER0_DEFAULT_VALUE_BYTE2_ADDR	0xABEE
#define ISX021_ME_SHUTTER0_DEFAULT_VALUE_BYTE3_ADDR	0xABEF

#define ISX021_ME_SHUTTER1_DEFAULT_VALUE_BYTE0_ADDR	0xABF4
#define ISX021_ME_SHUTTER1_DEFAULT_VALUE_BYTE1_ADDR	0xABF5
#define ISX021_ME_SHUTTER1_DEFAULT_VALUE_BYTE2_ADDR	0xABF6
#define ISX021_ME_SHUTTER1_DEFAULT_VALUE_BYTE3_ADDR	0xABF7

//  Analog Gain

#define ISX021_AE_ANALOG_GAIN0_LOW_BYTE_ADDR		0x6170
#define ISX021_AE_ANALOG_GAIN0_HIGH_BYTE_ADDR		0x6171

#define ISX021_ME_ANALOG_GAIN0_LOW_BYTE_ADDR		0xABFA
#define ISX021_ME_ANALOG_GAIN0_HIGH_BYTE_ADDR		0xABFB

#define ISX021_AE_ANALOG_GAIN1_LOW_BYTE_ADDR		0x6480
#define ISX021_AE_ANALOG_GAIN1_HIGH_BYTE_ADDR		0x6481

#define ISX021_ME_ANALOG_GAIN1_LOW_BYTE_ADDR		0xABFC
#define ISX021_ME_ANALOG_GAIN1_HIGH_BYTE_ADDR		0xABFD

#define ISX021_AE_ANALOG_GAIN2_LOW_BYTE_ADDR		0x6484
#define ISX021_AE_ANALOG_GAIN2_HIGH_BYTE_ADDR		0x6485

#define ISX021_ME_ANALOG_GAIN2_LOW_BYTE_ADDR		0xABFE
#define ISX021_ME_ANALOG_GAIN2_HIGH_BYTE_ADDR		0xABFF

#define ISX021_AE_ANALOG_GAIN3_LOW_BYTE_ADDR		0x6488
#define ISX021_AE_ANALOG_GAIN3_HIGH_BYTE_ADDR		0x6489

#define ISX021_ME_ANALOG_GAIN3_LOW_BYTE_ADDR		0xAC00
#define ISX021_ME_ANALOG_GAIN3_HIGH_BYTE_ADDR		0xAC01

// Exposure

#define ISX021_EXPOSURE_SHUTTER1_UNIT_ADDR			0xAC4D
#define ISX021_EXPOSURE_SHUTTER2_UNIT_ADDR			0xAC4E

#define ISX021_EXPOSURE_SHUTTER1_TIME_BYTE0_ADDR	0xAC44
#define ISX021_EXPOSURE_SHUTTER1_TIME_BYTE1_ADDR	0xAC45
#define ISX021_EXPOSURE_SHUTTER1_TIME_BYTE2_ADDR	0xAC46
#define ISX021_EXPOSURE_SHUTTER1_TIME_BYTE3_ADDR	0xAC47

#define ISX021_EXPOSURE_SHUTTER2_TIME_BYTE0_ADDR	0xAC48
#define ISX021_EXPOSURE_SHUTTER2_TIME_BYTE1_ADDR	0xAC49
#define ISX021_EXPOSURE_SHUTTER2_TIME_BYTE2_ADDR	0xAC4A
#define ISX021_EXPOSURE_SHUTTER2_TIME_BYTE3_ADDR	0xAC4B


#define ISX021_MODE_SET_F_ADDR						0x8A01

#define ISX021_SG_MODE_CTL_ADDR						0x8AF0
#define ISX021_FSYNC_FUNCSEL_ADDR					0x8AFE
#define ISX021_FSYNC_DRVABTY_ADDR					0x8AFF
#define ISX021_I2C_RESPONSE_MODE          0x8A55

//  AE mode

#define ISX021_AE_MODE_FULL_ME_ADDR					0xABC0

#define ISX021_SHUTTER0_UNIT_ADDR					0xABF0
#define ISX021_SHUTTER1_UNIT_ADDR					0xABF8

#define ISX021_SHUTTER1_UNIT_ADDR					0xABF8

//  Digital Gain

#define ISX021_DIGITAL_GAIN_LOW_BYTE_ADDR			0xAC0A
#define ISX021_DIGITAL_GAIN_HIGH_BYTE_ADDR			0xAC0B

// Distortion Corretion

#define	ISX021_DISTORTION_CORRECTION_0_ADDR			0xFFFF
#define	ISX021_DISTORTION_CORRECTION_1_ADDR			0xCC04
#define	ISX021_DISTORTION_CORRECTION_2_ADDR			0x8AB8

//#define ISX021_MODE_SET_F_LOCK_ADDR				0xBEF0

#define ISX021_SG_MODE_APL_ADDR						0xBF14

//#define ISX021_IR_DR_SG_FSYNCIN_SEL_ADDR			0x0153

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

/*--------------------------------------------------------------------------*/

int tier4_isx021_ctrl_set_fsync_trigger_mode(void *p_s_data)
{
	int err 	= 0;

//	struct camera_common_data 	*s_data = priv->s_data;
//	struct device 				*dev	= s_data->dev;;

	// transit to Startup state

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_MODE_SET_F_ADDR, 0x00);

	
	if ( err ) {
//		dev_err(dev, "[%s] : tier4_isx021_write_reg_wrapper() error_exited\n", __func__);
		err |= 0x10000000;
		goto error_exit;
	}

	micro_sec_sleep(TIME_120_MILISEC);

	// set SG_MODE_CTL to 0 for transition to FSYNC mode

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_SG_MODE_CTL_ADDR, 0x02);		// set FSYNC mode


	if ( err ) {
//			dev_err(dev, "[%s] : tier4_isx021_write_reg_wrapper() error_exited\n", __func__);
		err |= 0x20000000;
		goto error_exit;
	}

	micro_sec_sleep(TIME_120_MILISEC);

	// set SG_MODE_APL to 0 for transition to FSYNC mode

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_SG_MODE_APL_ADDR, 0x02);


	if ( err ) {
//		dev_err(dev, "[%s] : tier4_isx021_write_reg_wrapper() error_exited\n", __func__);
		err |= 0x30000000;
		goto error_exit;
	}

	micro_sec_sleep(TIME_120_MILISEC);

	// Exit Startup state

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_MODE_SET_F_ADDR, 0x80);

	if ( err ) {
//		dev_err(dev, "[%s] : tier4_isx021_write_reg_wrapper() error_exited\n", __func__);
		err |= 0x40000000;
		goto error_exit;
	}

	micro_sec_sleep(TIME_120_MILISEC);

	return NO_ERROR;

error_exit:

	return err;
}
EXPORT_SYMBOL(tier4_isx021_ctrl_set_fsync_trigger_mode);

/*--------------------------------------------------------------------------*/

static int tier4_isx021_ctrl_copy_register(void *p_s_data, u16 ae_addr, u16 me_addr)
{
	int err = 0;
	u8	val8 = 0;

	err = tier4_isx021_read_reg_wrapper(p_s_data, ae_addr, &val8);

	if ( !err ) {
		err = tier4_isx021_write_reg_wrapper(p_s_data, me_addr, val8);
	}

//	if (err) {
//		dev_err(s_data->dev, "[%s] : tier4_isx021_read_reg_wrapper error_exited, address = 0x%x\n", __func__, ae_addr);
//		goto error_exit;
//	}


//	if (err) {
//		dev_err(s_data->dev, "[%s] : tier4_isx021_write_reg_wrapper error_exited, address = 0x%x, val = 0x%x\n", __func__, me_addr, val8);
//	}

//error_exit:

	return err;
}

/*--------------------------------------------------------------------------*/
int tier4_isx021_ctrl_set_response_mode(void *p_s_data)
{
	int err = 0;
  
	micro_sec_sleep(100000);
	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_MODE_SET_F_ADDR, 0x00);
	if (err) {
		goto error_exit;
	}
	micro_sec_sleep(100000);		// For ES3
	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_I2C_RESPONSE_MODE, 0x04);
	if (err) {
		goto error_exit;
	}

	micro_sec_sleep(100000);		// For ES3
	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_MODE_SET_F_ADDR, 0x80);
	if (err) {
		goto error_exit;
	}

error_exit:

  return err;
}

EXPORT_SYMBOL(tier4_isx021_ctrl_set_response_mode);

/*--------------------------------------------------------------------------*/

int tier4_isx021_ctrl_set_gain(void *p_s_data, s64 val)
{
	int err = 0;
	s64	gain;
	u8	digital_gain_low_byte;
	u8	digital_gain_high_byte;

	if ( val > ISX021_MAX_GAIN ) {
		val = ISX021_MAX_GAIN;
	} else if ( val < ISX021_MIN_GAIN ) {
		val = ISX021_MIN_GAIN;
	}

//	dev_info(dev,"[%s] :Gain is set to %lld\n", __func__, val);

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_SHUTTER0_UNIT_ADDR, 0x01);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_SHUTTER1_UNIT_ADDR, 0x01);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_ctrl_copy_register(p_s_data, ISX021_AE_SHUTTER0_VALUE_BYTE0_ADDR, ISX021_ME_SHUTTER0_DEFAULT_VALUE_BYTE0_ADDR);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_ctrl_copy_register(p_s_data, ISX021_AE_SHUTTER0_VALUE_BYTE1_ADDR, ISX021_ME_SHUTTER0_DEFAULT_VALUE_BYTE1_ADDR);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_ctrl_copy_register(p_s_data, ISX021_AE_SHUTTER0_VALUE_BYTE2_ADDR, ISX021_ME_SHUTTER0_DEFAULT_VALUE_BYTE2_ADDR);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_ctrl_copy_register(p_s_data, ISX021_AE_SHUTTER0_VALUE_BYTE3_ADDR, ISX021_ME_SHUTTER0_DEFAULT_VALUE_BYTE3_ADDR);

	if (err) {
		goto error_exit;
	}

	// Shutter 1 value

	err = tier4_isx021_ctrl_copy_register(p_s_data, ISX021_AE_SHUTTER1_VALUE_BYTE0_ADDR, ISX021_ME_SHUTTER1_DEFAULT_VALUE_BYTE0_ADDR);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_ctrl_copy_register(p_s_data, ISX021_AE_SHUTTER1_VALUE_BYTE1_ADDR, ISX021_ME_SHUTTER1_DEFAULT_VALUE_BYTE1_ADDR);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_ctrl_copy_register(p_s_data, ISX021_AE_SHUTTER1_VALUE_BYTE2_ADDR, ISX021_ME_SHUTTER1_DEFAULT_VALUE_BYTE2_ADDR);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_ctrl_copy_register(p_s_data, ISX021_AE_SHUTTER1_VALUE_BYTE3_ADDR, ISX021_ME_SHUTTER1_DEFAULT_VALUE_BYTE3_ADDR);


	if (err) {
		goto error_exit;
	}

	// Set Analog Gain for shutter 0

	gain = 10*val;

	digital_gain_low_byte = gain & 0xFF;

	digital_gain_high_byte = ( gain >> 8  ) & 0xFF;

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_AE_ANALOG_GAIN0_LOW_BYTE_ADDR, digital_gain_low_byte);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_AE_ANALOG_GAIN0_HIGH_BYTE_ADDR, digital_gain_high_byte);

	if (err) {
		goto error_exit;
	}

	// copy  AE Analog gain value to ME analog gain registers for shutter 1 to 3

	err = tier4_isx021_ctrl_copy_register(p_s_data, ISX021_AE_ANALOG_GAIN1_LOW_BYTE_ADDR, ISX021_ME_ANALOG_GAIN1_LOW_BYTE_ADDR);

	if (err) {
		goto error_exit;
	}


	err = tier4_isx021_ctrl_copy_register(p_s_data, ISX021_AE_ANALOG_GAIN1_HIGH_BYTE_ADDR, ISX021_ME_ANALOG_GAIN1_HIGH_BYTE_ADDR);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_ctrl_copy_register(p_s_data, ISX021_AE_ANALOG_GAIN2_LOW_BYTE_ADDR, ISX021_ME_ANALOG_GAIN2_LOW_BYTE_ADDR);

	if (err) {
		goto error_exit;
	}


	err = tier4_isx021_ctrl_copy_register(p_s_data, ISX021_AE_ANALOG_GAIN2_HIGH_BYTE_ADDR, ISX021_ME_ANALOG_GAIN2_HIGH_BYTE_ADDR);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_ctrl_copy_register(p_s_data, ISX021_AE_ANALOG_GAIN3_LOW_BYTE_ADDR, ISX021_ME_ANALOG_GAIN3_LOW_BYTE_ADDR);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_ctrl_copy_register(p_s_data, ISX021_AE_ANALOG_GAIN3_HIGH_BYTE_ADDR, ISX021_ME_ANALOG_GAIN3_HIGH_BYTE_ADDR);

	if (err) {
		goto error_exit;
	}

	micro_sec_sleep(100000);		// For ES3

	// Change AE mode 

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_AE_MODE_FULL_ME_ADDR, 0x03);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_DIGITAL_GAIN_LOW_BYTE_ADDR, digital_gain_low_byte);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_DIGITAL_GAIN_HIGH_BYTE_ADDR, digital_gain_high_byte);

	if (err) {
		goto error_exit;
	}

error_exit:

	return err;
}
EXPORT_SYMBOL(tier4_isx021_ctrl_set_gain);

/* ------------------------------------------------------------------------- */

int tier4_isx021_ctrl_set_auto_exposure(void *p_s_data)
{

	int 						err 	= 0;

	// Change to Auto exposure mode

   	micro_sec_sleep(100000);	// 100msec for ES3

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_AE_MODE_FULL_ME_ADDR, ISX021_AUTO_EXPOSURE_MODE);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_EXPOSURE_SHUTTER2_UNIT_ADDR, ISX021_AE_TIME_UNIT_MICRO_SECOND );

	if (err) {
		goto error_exit;
	}

	// Set max exposure time

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_EXPOSURE_SHUTTER2_TIME_BYTE0_ADDR, ISX021_MAX_EXPOSURE_TIME & 0xFF );

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_EXPOSURE_SHUTTER2_TIME_BYTE1_ADDR, ( ISX021_MAX_EXPOSURE_TIME  >> 8 ) & 0xFF );

	if (err) {
		goto error_exit;
	}

//	return NO_ERROR;

error_exit:

//	dev_err(&priv->i2c_client->dev, "[%s] : Set Auto exposure mode error.\n", __func__);

	return err;
}
EXPORT_SYMBOL(tier4_isx021_ctrl_set_auto_exposure);

/* ------------------------------------------------------------------------- */

int tier4_isx021_ctrl_set_exposure(void *p_s_data, s64 val)
{

	int 	err = 0;

	u8		exp_time_byte0;
	u8		exp_time_byte1;
	u8		exp_time_byte2;
	u8		exp_time_byte3;

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

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_EXPOSURE_SHUTTER1_UNIT_ADDR, 0x03);

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_EXPOSURE_SHUTTER2_UNIT_ADDR, 0x03 );

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_EXPOSURE_SHUTTER1_TIME_BYTE0_ADDR, exp_time_byte0 );

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_EXPOSURE_SHUTTER1_TIME_BYTE1_ADDR, exp_time_byte1 );

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_EXPOSURE_SHUTTER1_TIME_BYTE2_ADDR, exp_time_byte2 );

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_EXPOSURE_SHUTTER1_TIME_BYTE3_ADDR, exp_time_byte3 );

	if (err) {
		goto error_exit;
	}

	// Shutter2

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_EXPOSURE_SHUTTER2_TIME_BYTE0_ADDR, exp_time_byte0 );

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_EXPOSURE_SHUTTER2_TIME_BYTE1_ADDR, exp_time_byte1 );

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_EXPOSURE_SHUTTER2_TIME_BYTE2_ADDR, exp_time_byte2 );

	if (err) {
		goto error_exit;
	}

	err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_EXPOSURE_SHUTTER2_TIME_BYTE3_ADDR, exp_time_byte3 );

	if (err) {
		goto error_exit;
	}

	return NO_ERROR;

error_exit:

//	dev_err(&priv->i2c_client->dev,	"[%s] : Set Exposure time error\n", __func__);

	return err;
}
EXPORT_SYMBOL(tier4_isx021_ctrl_set_exposure);

// --------------------------------------------------------------------------------------
//  Enable Distortion Coreection
// --------------------------------------------------------------------------------------
int tier4_isx021_ctrl_enable_distortion_correction(void *p_s_data, bool val)
{
	int err = 0;

	if ( val == true ) {
		err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_DISTORTION_CORRECTION_0_ADDR, 0x02);
		err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_DISTORTION_CORRECTION_1_ADDR, 0x01);
		err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_DISTORTION_CORRECTION_2_ADDR, 0x01);
		err = tier4_isx021_write_reg_wrapper(p_s_data, ISX021_DISTORTION_CORRECTION_0_ADDR, 0xF5);
	} else {
		err = -EINVAL;
//		dev_err(tc_dev->dev,"[%s] : Distortion Correction is disabled.", __func__);
	}

	return err;
}
EXPORT_SYMBOL(tier4_isx021_ctrl_enable_distortion_correction);

static int __init tier4_isx021_ctrl_init(void)
{
	micro_sec_sleep(100);

	return 0;
}

static void __exit tier4_isx021_ctrl_exit(void)
{
	micro_sec_sleep(100);
}

module_init(tier4_isx021_ctrl_init);
module_exit(tier4_isx021_ctrl_exit);

MODULE_DESCRIPTION("TIERIV Automotive HDR Camera access control driver");
MODULE_AUTHOR("K.Iwasaki");
MODULE_AUTHOR("Y.Fujii");
MODULE_LICENSE("Proprietary");
