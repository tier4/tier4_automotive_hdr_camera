// Copyright 2022 Tier IV, Inc.
// Proprietary

#include "c1_reg.hpp"
#include "t4cam_tools.hpp"
#include "i2c.hpp"

int C1::test(){
    uint8_t data[11];
    i2c::read(dev_name, i2c_dev_addr, 10, data);
    return 0;
}



int C1::initialized_load_value_from_file(const std::string &file_name)
{
    YAML::Node node;
    try
    {
      node = YAML::LoadFile(file_name);
      for (size_t i = 0; i < node["config"].size(); i++)
      {
        std::string param_name = node["config"][i]["param_name"].as<std::string>();
        float val = node["config"][i]["value"].as<float>();
#if 0
        std::cout << "param_name:" << param_name << std::endl;
        std::cout << "value:" << val << std::endl;
#endif
        load_value_map[param_name] = val;
      }
    }
    catch (const std::exception &e)
    {
      std::cerr << "Can not open yaml file:" << e.what() << std::endl;
      return -1;
    }

    return 0;
  }

  void C1::saveCurrentValue(std::string output)
  {
    fprintf(stdout, "output file: %s\n", output.c_str());

    YAML::Node item[10];

    item[0]["param_name"] = "ae_mode";
    item[0]["value"] = std::to_string(getAEMode());
    item[1]["param_name"] = "digital_gain";
    item[1]["value"] = std::to_string(getDigitalGain());
    item[2]["param_name"] = "shutter_speed";
    item[2]["value"] = std::to_string(getShutterSpeedforFME());
    item[3]["param_name"] = "ev_offset_flag";
    item[3]["value"] = std::to_string(getExposureOffsetFlag());
    item[4]["param_name"] = "ev_offset";
    item[4]["value"] = std::to_string(getExposureOffset());
    item[5]["param_name"] = "hue";
    item[5]["value"] = std::to_string(getHue());
    item[6]["param_name"] = "saturation";
    item[6]["value"] = std::to_string(getSaturation());
    item[7]["param_name"] = "contrast";
    item[7]["value"] = std::to_string(getContrast());
    item[8]["param_name"] = "brightness";
    item[8]["value"] = std::to_string(getBrightness());
    item[9]["param_name"] = "sharpness";
    item[9]["value"] = std::to_string(getSharpness());

    YAML::Node root;

    for (size_t i = 0; i < 10; i++)
    {
      root["config"].push_back(item[i]);
    }

    YAML::Emitter out;
    out << root;

    std::ofstream file(output);
    file << out.c_str();
    file.close();
  }


int8_t C1::checkES3()
{
  uint8_t data = 0;
  bool ret = false;

  i2c::read16(dev_name, i2c_dev_addr, I2C_RESPONSE_MODE, &data);
  if ((data & I2C_RESPONSE_MODE_ACK) == 0)
  {
    fprintf(stderr, "[WARN] I2C Response Mode is NACK. Even though you may get an I2C write error, you may still be "
                    "able to write.\n");
    return -1;
  }

  return 0;
}
float C1::getAEError()
{
#define ERRSCL_L 0x617C
#define ERRSCL_U 0x617D
  float ret;
  uint8_t l, u;
  uint16_t d;

  i2c::read16(dev_name, i2c_dev_addr, ERRSCL_L, &l);
  i2c::read16(dev_name, i2c_dev_addr, ERRSCL_U, &u);

  d = l + (u << 8);

  ret = (int16_t)d * 6.02 / 1024.0;
  return ret;
}

bool C1::isAvailableCamera(void)
{
  bool ret = true;
  if (i2c::check_device(dev_name, i2c_dev_addr, AE_MODE) < 0)
  {
    ret = false;
  }

  return ret;
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

uint8_t C1::getAEMode()
{
  uint8_t aemode;
  i2c::read16(dev_name, i2c_dev_addr, AE_MODE, &aemode);
  return aemode;
}

int8_t C1::setAEMode(int mode)
{
  int8_t ret = 0;
  if (getAEMode() != mode)
  {
    // set aemode
    ret = i2c::write16(dev_name, i2c_dev_addr, AE_MODE, mode);
  }
  return ret;
}


int8_t C1::setShutterSpeedforFME(float val)
{
  int8_t ret;

  uint32_t data = (val * 1000);
  for (int i = 0; i < 4; i++)
  {
    i2c::write16(dev_name, i2c_dev_addr, FME_SHTVAL + i, (uint8_t)data & 0xFF);
    i2c::write16(dev_name, i2c_dev_addr, FME_SHTVAL_S1 + i, (uint8_t)data & 0xFF);

    data = data >> 8;
  }

  fprintf(stderr, "-------\n");

  return ret;
}

float C1::getShutterSpeedforFME()
{
  uint8_t ret;
  int data = 0;

  for (int i = 0; i < 4; i++)
  {
    i2c::read16(dev_name, i2c_dev_addr, FME_SHTVAL + i, &ret);
    data |= ret << (i * 8);
  }

  return data / 1000;
}

int8_t C1::setDigitalGain(int db)
{
  uint8_t u;
  uint8_t l;

  if (getAEMode() != AE_MODE_ME)
  {
    fprintf(stderr, "[WARN]: AEMode is not ME. DigitalGain parameter has not effect in this mode.\n");
  }

  if (db < DIGITAL_GAIN_MIN || DIGITAL_GAIN_MAX < db)
  {
    fprintf(stderr, "[WARN][%s]: please set in the range of %lf ≤ db ≤ %lf. Digitalgain is not set.\n", __func__,
            DIGITAL_GAIN_MIN, DIGITAL_GAIN_MAX);
    return -1;
  }

  calcHexVal((float)db, 0.1, 0, u, l, 0xFF);
  fprintf(stderr, "%d, %x, %x\n", db, u, l);
  i2c::write16(dev_name, i2c_dev_addr, DIGITAL_GAIN_L, l);
  i2c::write16(dev_name, i2c_dev_addr, DIGITAL_GAIN_U, u);

  return 0;
}

int C1::getDigitalGain(void)
{
  uint8_t l, u;
  int16_t data;

  i2c::read16(dev_name, i2c_dev_addr, DIGITAL_GAIN_L, &l);
  i2c::read16(dev_name, i2c_dev_addr, DIGITAL_GAIN_U, &u);
  data = (int16_t)(l + (u << 8));

  fprintf(stderr, "%x:%x, %x:%x, %d\n", DIGITAL_GAIN_L, l, DIGITAL_GAIN_U, u, data);

  return (int)(data / 10);
}

int C1::getAnalogGain(void)
{
  uint8_t l, u;
  int16_t data;

  i2c::read16(dev_name, i2c_dev_addr, ANALOG_GAIN_L, &l);
  i2c::read16(dev_name, i2c_dev_addr, ANALOG_GAIN_U, &u);
  data = (int16_t)(l + (u << 8));

  fprintf(stderr, "%x:%x, %x:%x, %d\n", ANALOG_GAIN_L, l, ANALOG_GAIN_U, u, data);

  return (int)(data / 10);
}

int8_t C1::setAnalogGain(int db)
{
  uint8_t u;
  uint8_t l;

  if (getAEMode() != AE_MODE_ME)
  {
    fprintf(stderr, "[WARN]: AEMode is not ME. DigitalGain parameter has not effect in this mode.\n");
  }

  if (db < ANALOG_GAIN_MIN || ANALOG_GAIN_MAX < db)
  {
    fprintf(stderr, "[WARN][%s]: please set in the range of %lf ≤ db ≤ %lf. Analoggain is not set.\n", __func__,
            ANALOG_GAIN_MIN, ANALOG_GAIN_MAX);
    return -1;
  }

  calcHexVal((float)db, 0.1, 0, u, l, 0xFF);
  fprintf(stderr, "%d, %x, %x\n", db, u, l);
  i2c::write16(dev_name, i2c_dev_addr, ANALOG_GAIN_L, l);
  i2c::write16(dev_name, i2c_dev_addr, ANALOG_GAIN_U, u);

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

float C1::getSharpness(void)
{
  uint8_t data;
  float ret;

  i2c::read16(dev_name, i2c_dev_addr, UISHARPNESS, &data);
  ret = (float)data * (SHARPNESS_UNIT);

  return ret;
}

int8_t C1::setHue(int deg)
{
  if (deg < HUE_MIN || HUE_MAX < deg)
  {
    fprintf(stderr, "[WARN][%s:%x]: please set in the range of %d ≤ deg ≤ %d. Hue is not set.\n", __func__,
            i2c_dev_addr, HUE_MIN, HUE_MAX);
    return -1;
  }

  int16_t data;
  data = (deg / HUE_UNIT);

  i2c::write16(dev_name, i2c_dev_addr, UIHUE, data);
  return 0;
}

int C1::getHue(void)
{
  uint8_t data;
  i2c::read16(dev_name, i2c_dev_addr, UIHUE, &data);
  return (int)(int8_t)data * HUE_UNIT;
}

int C1::setATRContrastGain(int gain)
{
  uint8_t data = gain;
  return i2c::write16(dev_name, i2c_dev_addr,STRENGTH_MODE_GAIN_RATIO, data);
}
int C1::setATRBrightnessGain(int gain)
{
  uint8_t data = gain;
  return i2c::write16(dev_name, i2c_dev_addr,STRENGTH_MODE_OBLEND_RATIO, data);
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

float C1::getSaturation(void)
{
  float gain;
  uint8_t data;

  i2c::read16(dev_name, i2c_dev_addr, UISATURATION, &data);
  gain = (float)data * SATURATION_UNIT;
  return gain;
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

float C1::getBrightness(void)
{
  int16_t data;
  uint8_t l, u;

  i2c::read16(dev_name, i2c_dev_addr, UIBRIGHTNESS_L, &l);
  i2c::read16(dev_name, i2c_dev_addr, UIBRIGHTNESS_U, &u);

  data = (int16_t)((u << 8) + l);

  float ret = data / 10 * BRIGHTNESS_UNIT;
  fprintf(stderr, "%s: %d, 0x%x, 0x%x, %f\n", __func__, data, u, l, ret);
  return ret;
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

float C1::getContrast(void)
{
  uint8_t data;
  i2c::read16(dev_name, i2c_dev_addr, UICONTRAST, &data);
  float ret = (float)(data)*CONTRAST_UNIT;
  return ret;
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

//  fprintf(stderr, "[%s]:0x%x, 0x%x, %f\n", __func__, r_gain_u, r_gain_l, r_gain);
//  fprintf(stderr, "[%s]:0x%x, 0x%x, %f\n", __func__, gr_gain_u, gr_gain_l, gr_gain);
//  fprintf(stderr, "[%s]:0x%x, 0x%x, %f\n", __func__, gb_gain_u, gb_gain_l, gb_gain);
//  fprintf(stderr, "[%s]:0x%x, 0x%x, %f\n", __func__, b_gain_u, b_gain_l, b_gain);
//
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

int8_t C1::setExposureOffsetFlag(bool flag)
{
  int8_t ret;
  ret = i2c::write16(dev_name, i2c_dev_addr, EVREF_CTRL_SEL, flag);
  return ret;
}
int C1::getExposureOffsetFlag()
{
  uint8_t ret;

  i2c::read16(dev_name, i2c_dev_addr, EVREF_CTRL_SEL, &ret);
  ret = ret & 0x1;
  return (int)ret;
}

int8_t C1::setExposureOffset(float offset)
{
// TODO
#if 1
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

float C1::getExposureOffset()
{
  float ret;
  uint8_t data;

  i2c::read16(dev_name, i2c_dev_addr, EVREF_OFFSET, &data);

  ret = (float)(int8_t)data * EVREF_OFFSET_UNIT;

  return ret;
}
