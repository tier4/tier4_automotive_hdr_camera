// Copyright 2022 Tier IV, Inc.
// Proprietary

#include "c2_reg.hpp"
#include "gw5300.hpp"
#include "i2c.hpp"
#include "t4cam_tools.hpp"

inline size_t CHK_POS(size_t v)
{
  return v - 1;
}

bool C2::isAvailableCamera(void)
{
  bool ret = true;
  if (i2c::check_device(dev_name, i2c_dev_addr, 0x00) < 0)
  {
    ret = false;
  }else{
    DEBUG_PRINT("model: C2\n");
    DEBUG_PRINT("port_num: %d\n", port_num);
    DEBUG_PRINT("dev_name: %s\n", dev_name.c_str());
    DEBUG_PRINT("dev_addr: 0x%x\n", i2c_dev_addr);
  }


  return ret;
}

void C2::saveCurrentValue(std::string output)
{
  std::cerr << "not present this function." << std::endl;
}

int C2::setDWP(bool on)
{
	uint8_t cmd_dwp_on[]={0x33, 0x47, 0x06, 0x00, 0x00, 0x00, 0x4d, 0x00, 0x80, 0x04, 0x00, 0x01, 0x52};
	uint8_t cmd_dwp_off[]={0x33, 0x47, 0x03, 0x00, 0x00, 0x00, 0x45, 0x00, 0x80, 0x42};

if(on){
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_dwp_on, sizeof(cmd_dwp_on));
}else{
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_dwp_off, sizeof(cmd_dwp_off));

}


}

int C2::setSensorMode(uint8_t mode)
{
  FILE *f;
  f = fopen("/sys/", "w");
  if (f == NULL)
  {
    fprintf(stderr, "Error opening file: driver parameter\n");
    return -1;
  }
  fprintf(f, "%u", mode);
  fclose(f);
  return 0;
}

int C2::setAutoExposure(bool on)
{
  uint8_t cmd_auto_exposure_onoff[] = { 0x33, 0x47, 0x0c, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                                        0x00, 0x07, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x69 };
  cmd_auto_exposure_onoff[17] = on ? 0x00 : 0x01;
  cmd_auto_exposure_onoff[CHK_POS(sizeof(cmd_auto_exposure_onoff))] =
      calcCheckSum(cmd_auto_exposure_onoff, sizeof(cmd_auto_exposure_onoff));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_auto_exposure_onoff, sizeof(cmd_auto_exposure_onoff));
}

int C2::setSensorGain(float gain)
{
  uint8_t buf[10];
  uint8_t cmd_sensor_gain[] = { 0x33, 0x47, 0x0f, 0x00, 0x00, 0x00, 0x55,         0x00, 0x80, 0x05, 0x00,
                                0x09, 0x00, 0x01, 0x00, 0x04, 0x00, /*pos*/ 0x00, 0x00, 0xc8, 0x42, 0x00 };
  const size_t val_pos = 17;

  // total gain = sensor gain x isp gain
  // sensor gain = 1 ~ 503
  if (gain < 1.0)
  {
    gain = 1.0;
  }
  else if (gain > 503.0)
  {
    gain = 503.0;
  }

  gain = static_cast<int>(gain * 10) / 10.0f;

  uint32_t *gain_hex = (uint32_t *)&gain;
  DEBUG_PRINT("[%s]:%f:0x%x\n", __func__,gain,*gain_hex);

  cmd_sensor_gain[val_pos] = *gain_hex & 0xff;
  cmd_sensor_gain[val_pos + 1] = (*gain_hex >> 8) & 0xff;
  cmd_sensor_gain[val_pos + 2] = (*gain_hex >> 16) & 0xff;
  cmd_sensor_gain[val_pos + 3] = (*gain_hex >> 24) & 0xff;

  cmd_sensor_gain[sizeof(cmd_sensor_gain) - 1] = calcCheckSum(cmd_sensor_gain, sizeof(cmd_sensor_gain));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_sensor_gain, sizeof(cmd_sensor_gain));
}

int C2::setISPSensorGain(float gain)
{
  uint8_t buf[10];
  uint8_t cmd_sensor_gain[] = { 0x33, 0x47, 0x0f, 0x00, 0x00, 0x00, 0x55,         0x00, 0x80, 0x05, 0x00,
                                0x1f, 0x00, 0x01, 0x00, 0x04, 0x00, /*pos*/ 0x00, 0x00, 0x40, 0x40, 0x07 };
  const size_t val_pos = 17;

  // total gain = sensor gain x isp gain
  // ISP sensor gain = 1 ~ 31.99
  if (gain < 1.0)
  {
    gain = 1.0;
  }
  else if (gain > 31.99)
  {
    gain = 31.99;
  }

  gain = static_cast<int>(gain * 10) / 10.0f;
  uint32_t *gain_hex = (uint32_t *)&gain;
  DEBUG_PRINT("[%s]:%f:0x%x\n", __func__,gain,*gain_hex);

  cmd_sensor_gain[val_pos] = *gain_hex & 0xff;
  cmd_sensor_gain[val_pos + 1] = (*gain_hex >> 8) & 0xff;
  cmd_sensor_gain[val_pos + 2] = (*gain_hex >> 16) & 0xff;
  cmd_sensor_gain[val_pos + 3] = (*gain_hex >> 24) & 0xff;

  cmd_sensor_gain[sizeof(cmd_sensor_gain) - 1] = calcCheckSum(cmd_sensor_gain, sizeof(cmd_sensor_gain));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_sensor_gain, sizeof(cmd_sensor_gain));
}

// dwp
int C2::setDistortionCorrection(bool on)
{
  DEBUG_PRINT("[%s]\n\n", __func__);
  uint8_t buf[10];

  // 13byte
  uint8_t cmd_dwp_on[] = { 0x33, 0x47, 0x6, 0x0, 0x0, 0x0, 0x4d, 0x0, 0x80, 0x4, 0x0, 0x1, 0x52 };
  // 10byte
  uint8_t cmd_dwp_off[] = { 0x33, 0x47, 0x3, 0x0, 0x0, 0x0, 0x45, 0x0, 0x80, 0x42 };

  int ret = 0;
  if (on)
  {
    ret = i2c::transfer(dev_name, i2c_dev_addr, cmd_dwp_on, sizeof(cmd_dwp_on));
  }
  else
  {
    ret = i2c::transfer(dev_name, i2c_dev_addr, cmd_dwp_off, sizeof(cmd_dwp_off));
  }
  return ret;
}

int C2::setAutoWhiteBalance(bool on)
{
  uint8_t cmd_auto_white_balance_onoff[] = { 0x33, 0x47, 0x0c, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                                             0x00, 0x96, 0x02, 0x01, 0x00, 0x01, 0x00, 0x01, 0xfb };
  cmd_auto_white_balance_onoff[17] = on ? 0x01 : 0x00;
  cmd_auto_white_balance_onoff[CHK_POS(sizeof(cmd_auto_white_balance_onoff))] =
      calcCheckSum(cmd_auto_white_balance_onoff, sizeof(cmd_auto_white_balance_onoff));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_auto_white_balance_onoff, sizeof(cmd_auto_white_balance_onoff));
}

int C2::setAutoWhiteBalanceGainR(int val)
{
  if (val < -1500)
    val = -1500;
  else if (val > 1500)
    val = 1500;

  uint16_t data = val;
  uint8_t cmd_auto_white_balance[] = { 0x33, 0x47, 0x0d, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                                       0x00, 0x8a, 0x02, 0x01, 0x00, 0x02, 0x00, 0xf4, 0x01, 0xe5 };

  cmd_auto_white_balance[17] = data & 0xFF;
  cmd_auto_white_balance[18] = (data >> 8) & 0xFF;
  cmd_auto_white_balance[CHK_POS(sizeof(cmd_auto_white_balance))] =
      calcCheckSum(cmd_auto_white_balance, sizeof(cmd_auto_white_balance));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_auto_white_balance, sizeof(cmd_auto_white_balance));
}

int C2::setAutoWhiteBalanceGainB(int val)
{
  if (val < -1500)
    val = -1500;
  else if (val > 1500)
    val = 1500;

  uint16_t data = val;
  uint8_t cmd_auto_white_balance[] = { 0x33, 0x47, 0x0d, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                                       0x00, 0x8b, 0x02, 0x01, 0x00, 0x02, 0x00, 0x2c, 0x01, 0x1e };

  cmd_auto_white_balance[17] = data & 0xFF;
  cmd_auto_white_balance[18] = (data >> 8) & 0xFF;
  cmd_auto_white_balance[CHK_POS(sizeof(cmd_auto_white_balance))] =
      calcCheckSum(cmd_auto_white_balance, sizeof(cmd_auto_white_balance));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_auto_white_balance, sizeof(cmd_auto_white_balance));
}

// Image tuning

int C2::setHue(bool on)
{
  uint8_t cmd_hue_onoff[] = { 0x33, 0x47, 0x0c, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                              0x00, 0x65, 0x09, 0x01, 0x00, 0x01, 0x00, 0x00, 0xd0 };
  cmd_hue_onoff[17] = on ? 0x01 : 0x00;
  cmd_hue_onoff[CHK_POS(sizeof(cmd_hue_onoff))] = calcCheckSum(cmd_hue_onoff, sizeof(cmd_hue_onoff));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_hue_onoff, sizeof(cmd_hue_onoff));
}

int C2::setHueVal(int val)
{
  if (val < -180)
    val = -180;
  else if (val > 180)
    val = 180;

  uint16_t data = val;
  uint8_t cmd_hue[] = { 0x33, 0x47, 0x0d, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                        0x00, 0x66, 0x09, 0x01, 0x00, 0x02, 0x00, 0xb4, 0x00, 0x87 };

  cmd_hue[17] = data & 0xFF;
  cmd_hue[18] = (data >> 8) & 0xFF;

  cmd_hue[CHK_POS(sizeof(cmd_hue))] = calcCheckSum(cmd_hue, sizeof(cmd_hue));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_hue, sizeof(cmd_hue));
}

int C2::setContrast(bool on)
{
  uint8_t cmd_contrast_onoff[] = { 0x33, 0x47, 0x0c, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                                   0x00, 0x61, 0x09, 0x01, 0x00, 0x01, 0x00, 0x00, 0xd0 };
  cmd_contrast_onoff[17] = on ? 0x01 : 0x00;
  cmd_contrast_onoff[CHK_POS(sizeof(cmd_contrast_onoff))] =
      calcCheckSum(cmd_contrast_onoff, sizeof(cmd_contrast_onoff));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_contrast_onoff, sizeof(cmd_contrast_onoff));
}

int C2::setContrastVal(int val)
{
  if (val < -255)
    val = -255;
  else if (val > 255)
    val = 255;

  uint16_t data = val;
  uint8_t cmd_contrast[] = { 0x33, 0x47, 0x0d, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                             0x00, 0x62, 0x09, 0x01, 0x00, 0x02, 0x00, 0xff, 0x00, 0xce };

  cmd_contrast[17] = data & 0xFF;
  cmd_contrast[18] = (data >> 8) & 0xFF;
  cmd_contrast[CHK_POS(sizeof(cmd_contrast))] = calcCheckSum(cmd_contrast, sizeof(cmd_contrast));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_contrast, sizeof(cmd_contrast));
}

int C2::setBrightness(bool on)
{
  uint8_t cmd_brightness_onoff[] = { 0x33, 0x47, 0x0c, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                                     0x00, 0x63, 0x09, 0x01, 0x00, 0x01, 0x00, 0x00, 0xce };
  cmd_brightness_onoff[17] = on ? 0x01 : 0x00;
  
  cmd_brightness_onoff[CHK_POS(sizeof(cmd_brightness_onoff))] =
      calcCheckSum(cmd_brightness_onoff, sizeof(cmd_brightness_onoff));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_brightness_onoff, sizeof(cmd_brightness_onoff));
}

int C2::setBrightnessVal(int val)
{
  if (val < -255)
    val = -255;
  else if (val > 255)
    val = 255;

  uint16_t data = val;
  uint8_t cmd_brightness[] = { 0x33, 0x47, 0x0d, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                               0x00, 0x64, 0x09, 0x01, 0x00, 0x02, 0x00, 0xff, 0x00, 0xd0 };

  cmd_brightness[17] = data & 0xFF;
  cmd_brightness[18] = (data >> 8) & 0xFF;
  cmd_brightness[CHK_POS(sizeof(cmd_brightness))] = calcCheckSum(cmd_brightness, sizeof(cmd_brightness));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_brightness, sizeof(cmd_brightness));
}

int C2::setSaturation(bool on)
{
  uint8_t cmd_saturation_onoff[] = { 0x33, 0x47, 0x0c, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                                     0x00, 0x2a, 0x03, 0x01, 0x00, 0x01, 0x00, 0x00, 0x8f };
  cmd_saturation_onoff[17] = on ? 0x01 : 0x00;
  cmd_saturation_onoff[CHK_POS(sizeof(cmd_saturation_onoff))] =
      calcCheckSum(cmd_saturation_onoff, sizeof(cmd_saturation_onoff));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_saturation_onoff, sizeof(cmd_saturation_onoff));
}

int C2::setSaturationVal(int val)
{
#if 0
  if (val < 0)
    val = 0;
  else if (val > 255)
    val = 255;
#endif
  uint8_t data = val;
  uint8_t cmd_saturation[] = { 0x33, 0x47, 0x0c, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                               0x00, 0x2b, 0x03, 0x01, 0x00, 0x01, 0x00, 0xff, 0x8f };

  cmd_saturation[17] = data & 0xFF;
  cmd_saturation[CHK_POS(sizeof(cmd_saturation))] = calcCheckSum(cmd_saturation, sizeof(cmd_saturation));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_saturation, sizeof(cmd_saturation));
}

int C2::setSharpness(bool on)
{
  uint8_t cmd_sharpness_onoff[] = { 0x33, 0x47, 0x0c, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                                    0x00, 0x59, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0xbc };
  cmd_sharpness_onoff[17] = on ? 0x01 : 0x00;
  cmd_sharpness_onoff[CHK_POS(sizeof(cmd_sharpness_onoff))] =
      calcCheckSum(cmd_sharpness_onoff, sizeof(cmd_sharpness_onoff));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_sharpness_onoff, sizeof(cmd_sharpness_onoff));
}

int C2::setSharpnessVal_Ld(int val)
{
  if (val < 0)
    val = 0;
  else if (val > 255)
    val = 255;

  uint8_t data = val;
  uint8_t cmd_sharpness[] = { 0x33, 0x47, 0x0c, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                              0x00, 0x4d, 0x01, 0x01, 0x00, 0x01, 0x00, 0xff, 0xaf };

  cmd_sharpness[17] = data & 0xFF;
  cmd_sharpness[CHK_POS(sizeof(cmd_sharpness))] = calcCheckSum(cmd_sharpness, sizeof(cmd_sharpness));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_sharpness, sizeof(cmd_sharpness));
}

int C2::setSharpnessVal_Ldu(int val)
{
  if (val < 0)
    val = 0;
  else if (val > 255)
    val = 255;

  uint8_t data = val;
  uint8_t cmd_sharpness[] = { 0x33, 0x47, 0x0c, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                              0x00, 0x4e, 0x01, 0x01, 0x00, 0x01, 0x00, 0xff, 0xaf };

  cmd_sharpness[17] = data & 0xFF;
  cmd_sharpness[CHK_POS(sizeof(cmd_sharpness))] = calcCheckSum(cmd_sharpness, sizeof(cmd_sharpness));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_sharpness, sizeof(cmd_sharpness));
}

int C2::setSharpnessVal_Lu(int val)
{
  if (val < 0)
    val = 0;
  else if (val > 255)
    val = 255;

  uint8_t data = val;
  uint8_t cmd_sharpness[] = { 0x33, 0x47, 0x0c, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                              0x00, 0x4f, 0x01, 0x01, 0x00, 0x01, 0x00, 0xff, 0xaf };

  cmd_sharpness[17] = data & 0xFF;
  cmd_sharpness[CHK_POS(sizeof(cmd_sharpness))] = calcCheckSum(cmd_sharpness, sizeof(cmd_sharpness));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_sharpness, sizeof(cmd_sharpness));
}

int C2::setShutterTimeXXX(uint16_t time_ms)
{
#define MS_TO_LINE_UNIT 80
  uint8_t cmd_shutter[] = { 0x33, 0x47, 0x0d, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                            0x00, 0x08, 0x00, 0x01, 0x00, 0x02, 0x00, 0xe8, 0x03, 0x57 };

  uint32_t line = time_ms * MS_TO_LINE_UNIT;
  cmd_shutter[17] = line & 0xFF;
  cmd_shutter[18] = (line >> 8) & 0xFF;

  cmd_shutter[CHK_POS(sizeof(cmd_shutter))] = calcCheckSum(cmd_shutter, sizeof(cmd_shutter));
  return i2c::transfer(dev_name, i2c_dev_addr, cmd_shutter, sizeof(cmd_shutter));
}

int C2::setShutterTimeOnAE(uint16_t ms)
{
  DEBUG_PRINT("[%s]\n\n", __func__);
#define MS_TO_LINE_UNIT 80
  uint8_t buf[8];

  uint8_t cmd_integration_max[] = { 0x33, 0x47, 0x0f, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05, 0x00,
                                    0x15, 0x00, 0x01, 0x00, 0x04, 0x00, 0x70, 0x03, 0x00, 0x00, 0x00 };

  uint8_t cmd_integration_min[] = { 0x33, 0x47, 0x0d, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                                    0x00, 0x21, 0x00, 0x01, 0x00, 0x02, 0x00, 0x70, 0x03, 0x00 };

  uint32_t max_line = ms * MS_TO_LINE_UNIT;
  uint32_t min_line = ms * MS_TO_LINE_UNIT;

  uint8_t b1 = max_line & 0xFF;
  uint8_t b2 = (max_line >> 8) & 0xFF;
  uint8_t b3 = 0;
  uint8_t b4 = 0;

  const size_t max_val_pos = 17;
  const size_t min_val_pos = 17;

  cmd_integration_max[max_val_pos] = b1;
  cmd_integration_max[max_val_pos + 1] = b2;
  cmd_integration_max[max_val_pos + 2] = b3;
  cmd_integration_max[max_val_pos + 3] = b4;

  b1 = min_line & 0xFF;
  b2 = (min_line >> 8) & 0xFF;

  cmd_integration_min[min_val_pos] = b1;
  cmd_integration_min[min_val_pos + 1] = b2;

  cmd_integration_max[sizeof(cmd_integration_max) - 1] = calcCheckSum(cmd_integration_max, sizeof(cmd_integration_max));
  cmd_integration_min[sizeof(cmd_integration_min) - 1] = calcCheckSum(cmd_integration_min, sizeof(cmd_integration_min));

  int ret = 0;
   ret += i2c::transfer(dev_name, i2c_dev_addr, cmd_integration_max, sizeof(cmd_integration_max));
  ret += i2c::transfer(dev_name, i2c_dev_addr, cmd_integration_min, sizeof(cmd_integration_min));

  return ret;
}
