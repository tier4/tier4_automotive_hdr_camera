// Copyright 2022 Tier IV, Inc.
// Proprietary

#include "c1_reg.hpp"
#include "i2c.hpp"
#include "t4cam_tools.hpp"

static inline void calcHexVal(float raw, float unit, uint16_t offset, uint8_t &data_u, uint8_t &data_l, uint16_t mask)
{
  uint16_t temp;

  temp = ((uint16_t)(raw / unit) + offset) & mask;
  data_l = temp & 0xFF;
  data_u = temp >> 8;
  return;
}

int8_t C1::checkES3()
{
  uint8_t data = 0;
  bool ret = false;

  i2c::read16(dev_name, i2c_dev_addr, I2C_RESPONSE_MODE, &data);
  if (data & I2C_RESPONSE_MODE_ACK)
  {
    fprintf(stderr, "[WARN] I2C Response Mode is NACK. Even though you may get an I2C write error, you may still be "
                    "able to write.\n");
    return -1;
  }

  return 0;
}

float C1::getTempature(int type)
{
  // TODO: add check the sensor state.

  float ret;
  if (type == 0)
  {
    ret = getTempatureS0();
  }
  else
  {
    ret = getTempatureS1();
  }

  return ret;
}

float C1::getTempatureS0(void)
{
  uint8_t l, u;
  float data;

  i2c::read16(dev_name, i2c_dev_addr, RO_CD_DU_TEMP_SEN0_OUT_L, &l);
  i2c::read16(dev_name, i2c_dev_addr, RO_CD_DU_TEMP_SEN0_OUT_U, &u);
  data = l + (u << 8);
  data = data / 16 - 50;

  return data;
}

float C1::getTempatureS1(void)
{
  uint8_t l, u;
  float data;

  i2c::read16(dev_name, i2c_dev_addr, RO_CD_DU_TEMP_SEN1_OUT_L, &l);
  i2c::read16(dev_name, i2c_dev_addr, RO_CD_DU_TEMP_SEN1_OUT_U, &u);
  data = l + (u << 8);
  data = data / 16 - 50;

  return data;
}

int8_t C1::setDigitalGain(int db)
{
  uint8_t u;
  uint8_t l;

  if (db < DIGITAL_GAIN_MIN || DIGITAL_GAIN_MAX < db)
  {
    fprintf(stderr, "[WARN][%s]: please set in the range of %lf ≤ db ≤ %lf. Digitalgain is not set.\n", __func__,
            DIGITAL_GAIN_MIN, DIGITAL_GAIN_MAX);
    return -1;
  }

  calcHexVal((float)db, 0.1, 0, u, l, 0xFF);
  i2c::write16(dev_name, i2c_dev_addr, DIGITAL_GAIN_L, l);
  i2c::write16(dev_name, i2c_dev_addr, DIGITAL_GAIN_U, u);

  return 0;
}

int8_t C1::setSharpness(float gain)
{
  if (gain < SHARPNESS_MIN || SHARPNESS_MAX < gain)
  {
    fprintf(stderr, "[WARN][%s]: please set in the range of %lf ≤ gain ≤ %lf. Sharpness is not set.\n", __func__,
            SHARPNESS_MIN, SHARPNESS_MAX);
    return -1;
  }
  uint8_t data;
  data = (uint8_t)(gain / SHARPNESS_UNIT);
  i2c::write16(dev_name, i2c_dev_addr, UISHARPNESS, data);

  return 0;
}
int8_t C1::setHue(int deg)
{
  if (deg < HUE_MIN || HUE_MAX < deg)
  {
    fprintf(stderr, "[WARN][%s]: please set in the range of %d ≤ deg ≤ %d. Hue is not set.\n", __func__, HUE_MIN,
            HUE_MAX);
    return -1;
  }

  int16_t data;
  data = (deg / HUE_UNIT);

  i2c::write16(dev_name, i2c_dev_addr, UIHUE, data);
  return 0;
}
int8_t C1::setSaturation(float gain)
{
  if (gain < SATURATION_MIN || SATURATION_MAX < gain)
  {
    fprintf(stderr, "[WARN][%s]: please set in the range of %lf ≤ gain ≤ %lf. Saturation is not set.\n", __func__,
            SATURATION_MIN, SATURATION_MAX);
    return -1;
  }
  uint8_t data;
  data = (uint8_t)(gain / SATURATION_UNIT);
  i2c::write16(dev_name, i2c_dev_addr, UISATURATION, data);
  return 0;
}

int8_t C1::setBrightness(float offset)
{
  if (offset < BRIGHTNESS_MIN || BRIGHTNESS_MAX < offset)
  {
    fprintf(stderr, "[WARN][%s]: please set in the range of %lf ≤ offset ≤ %lf. Brightness is not set.\n", __func__,
            BRIGHTNESS_MIN, BRIGHTNESS_MAX);
    return -1;
  }
  int data;
  data = (int)(offset / BRIGHTNESS_UNIT);

  uint8_t l = (data * 10) & 0xFF;
  uint8_t u = (data * 10) >> 8;
  i2c::write16(dev_name, i2c_dev_addr, UIBRIGHTNESS_L, l);
  i2c::write16(dev_name, i2c_dev_addr, UIBRIGHTNESS_U, u);
  return 0;
}

int8_t C1::setContrast(float gain)
{
  if (gain < CONTRAST_MIN || CONTRAST_MAX < gain)
  {
    fprintf(stderr, "[WARN][%s]: please set in the range of %lf ≤ gain ≤ %lf. Contrast is not set.\n", __func__,
            CONTRAST_MIN, CONTRAST_MAX);
    return -1;
  }
  uint8_t data;
  data = (uint8_t)(gain / CONTRAST_UNIT);
  i2c::write16(dev_name, i2c_dev_addr, UICONTRAST, data);

  return 0;
}

int8_t C1::setAutoWhiteBalance(bool on)
{
  uint8_t set_val = AWBMODE_ATW;
  if (on == false)
  {
    set_val = AWBMODE_FULLMWB;
  }

  i2c::write16(dev_name, i2c_dev_addr, AWBMODE, AWBMODE_FULLMWB);

  return 0;
}

int8_t C1::setWhiteBalanceGain(float r_gain, float gr_gain, float gb_gain, float b_gain)
{
  uint8_t r_gain_l = 0;
  uint8_t r_gain_u = 0;
  uint8_t gr_gain_l = 0;
  uint8_t gr_gain_u = 0;
  uint8_t gb_gain_l = 0;
  uint8_t gb_gain_u = 0;
  uint8_t b_gain_l = 0;
  uint8_t b_gain_u = 0;

  if (r_gain < FULLMWBGAIN_MIN || FULLMWBGAIN_MAX < r_gain)
  {
    fprintf(stderr, "[WARN][%s]: please set in the range of %lf ≤ r_gain ≤ %lf. Contrast is not set.\n", __func__,
            FULLMWBGAIN_MIN, FULLMWBGAIN_MAX);
    return -1;
  }
  if (gr_gain < FULLMWBGAIN_MIN || FULLMWBGAIN_MAX < gr_gain)
  {
    fprintf(stderr, "[WARN][%s]: please set in the range of %lf ≤ gr_gain ≤ %lf. Contrast is not set.\n", __func__,
            FULLMWBGAIN_MIN, FULLMWBGAIN_MAX);
    return -1;
  }
  if (gb_gain < FULLMWBGAIN_MIN || FULLMWBGAIN_MAX < gb_gain)
  {
    fprintf(stderr, "[WARN][%s]: please set in the range of %lf ≤ gb_gain ≤ %lf. Contrast is not set.\n", __func__,
            FULLMWBGAIN_MIN, FULLMWBGAIN_MAX);
    return -1;
  }
  if (b_gain < FULLMWBGAIN_MIN || FULLMWBGAIN_MAX < b_gain)
  {
    fprintf(stderr, "[WARN][%s]: please set in the range of %lf ≤ b_gain ≤ %lf. Contrast is not set.\n", __func__,
            FULLMWBGAIN_MIN, FULLMWBGAIN_MAX);
    return -1;
  }

  calcHexVal(r_gain, FULLMWBGAIN_UNIT, 0, r_gain_u, r_gain_l, 0xFFF);
  calcHexVal(gr_gain, FULLMWBGAIN_UNIT, 0, gr_gain_u, gr_gain_l, 0xFFF);
  calcHexVal(gb_gain, FULLMWBGAIN_UNIT, 0, gb_gain_u, gb_gain_l, 0xFFF);
  calcHexVal(b_gain, FULLMWBGAIN_UNIT, 0, b_gain_u, b_gain_l, 0xFFF);

  fprintf(stderr, "[%s]:0x%x, 0x%x, %f\n", __func__, r_gain_u, r_gain_l, r_gain);
  fprintf(stderr, "[%s]:0x%x, 0x%x, %f\n", __func__, gr_gain_u, gr_gain_l, gr_gain);
  fprintf(stderr, "[%s]:0x%x, 0x%x, %f\n", __func__, gb_gain_u, gb_gain_l, gb_gain);
  fprintf(stderr, "[%s]:0x%x, 0x%x, %f\n", __func__, b_gain_u, b_gain_l, b_gain);

  i2c::write16(dev_name, i2c_dev_addr, FULLMWBGAIN_R_L, r_gain_l);
  i2c::write16(dev_name, i2c_dev_addr, FULLMWBGAIN_R_U, r_gain_u);
  i2c::write16(dev_name, i2c_dev_addr, FULLMWBGAIN_GR_L, r_gain_l);
  i2c::write16(dev_name, i2c_dev_addr, FULLMWBGAIN_GR_U, r_gain_u);
  i2c::write16(dev_name, i2c_dev_addr, FULLMWBGAIN_GB_L, r_gain_l);
  i2c::write16(dev_name, i2c_dev_addr, FULLMWBGAIN_GB_U, r_gain_u);
  i2c::write16(dev_name, i2c_dev_addr, FULLMWBGAIN_B_L, r_gain_l);
  i2c::write16(dev_name, i2c_dev_addr, FULLMWBGAIN_B_U, r_gain_u);

  return 0;
}
int8_t C1::setExposureOffset(float offset)
{
// TODO
#if 0
  if (offset < EVREF_OFFSET_MIN || EVREF_OFFSET_MAX < offset)
  {
    fprintf(stderr, "[WARN][%s]: please set in the range of %lf ≤ offset ≤ %lf. Contrast is not set.\n", __func__,
            EVREF_OFFSET_MIN, EVREF_OFFSET_MAX);
    return -1;
  }

  int8_t data = offset / EVREF_OFFSET_UNIT;

  i2c::write16(dev_name, i2c_dev_addr, EVREF_OFFSET, data);
#endif
  return 0;
}
