// Copyright 2022 Tier IV, Inc.
// Proprietary

#ifndef __T4CAM_TOOLS_HPP__
#define __T4CAM_TOOLS_HPP__

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>

#define DEBUG_PRINT(...)                                                                                               \
  if (debug_flag)                                                                                                      \
  {                                                                                                                    \
    printf(__VA_ARGS__);                                                                                               \
  }
#define MAX_PORT 8

static const std::array<std::string, 8> portnum_table = { "i2c-30", "i2c-30", "i2c-31", "i2c-31",
                                                          "i2c-32", "i2c-32", "i2c-33", "i2c-33" };

static bool debug_flag = true;

static inline void calcHexVal(float raw, float unit, uint16_t offset, uint8_t &data_u, uint8_t &data_l, uint16_t mask)
{
  uint16_t temp;

  temp = ((uint16_t)(raw / unit) + offset);
  data_l = temp & 0xFF;
  data_u = temp >> 8;

  // fprintf(stderr, "[%s]:%f-%d, %d, %d\n", __func__, raw, temp, data_l, data_u);
  return;
}

enum class C2_SENSOR_MODE : uint8_t
{
  default_mode = 0,
  trigger_mode_10fps = 1,
  freerun_mode_10fps = 2,
  freerun_mode_30fps = 3,
  trigger_mode_20fps = 4,
  freerun_mode_20fps = 5,
  trigger_mode_30fps = 6,
};

class Param
{
public:
  float value = 0;
  bool update = false;

  Param operator=(float obj)
  {
    value = obj;
    update = true;
    return *this;
  }
  operator bool()
  {
    return value >= 0.00001;
  }
  operator int()
  {
    return (int)value;
  }
  operator float()
  {
    return value;
  }
};

class t4cam
{
protected:
  int port_num;
  std::string dev_name;
  uint8_t i2c_dev_addr;

  std::unordered_map<std::string, Param> default_value_map;
  std::unordered_map<std::string, Param> load_value_map;

public:
  virtual bool isAvailableCamera(void) = 0;
  virtual void saveCurrentValue(std::string output) = 0;

  virtual int initialized_load_value_from_file(const std::string &file_name)
  {
    return 0;
  }
  virtual void initialized_default_value_from_default()
  {
    std::cerr << "not present this function." << std::endl;
  }
  virtual void setDefaultValue(void)
  {
    std::cerr << "not present this function." << std::endl;
  }

  virtual void setLoadValue(void)
  {
    std::cerr << "not present this function." << std::endl;
  }
};

class C2 : public t4cam
{
public:
  C2(int _port_num = 0, bool _debug_flag = false)
  {
    port_num = _port_num;

    if (port_num < 0 || port_num >= MAX_PORT)
    {
      std::cerr << "The port number has exceeded the maximum value. Please specify between 0-" << MAX_PORT - 1
                << std::endl;
      exit(-1);
    }
    dev_name = "/dev/" + portnum_table[port_num];
    i2c_dev_addr = (port_num % 2) == 0 ? 0x6d : 0x6e;

    debug_flag = _debug_flag;
#ifdef DEBUG
    debug_flag = true;
#endif
  }

  struct default_val
  {
    uint8_t sensor_mode;
    bool auto_exposure;
    float sensor_gain;
    float isp_sensor_gain;
    bool distortion_correction;
    bool awb;
    int awb_gain_r;
    int awb_gain_g;
    int awb_gain_b;

    bool hue_mode;
    int hue_val;
    bool contrast_mode;
    int contrast_val;
    bool brightness_mode;
    int brightness_val;
    bool saturation_mode;
    int saturation_val;
    bool sharpness_mode;
    int sharpness_val_ld;
    int sharpness_val_ldu;
    int sharpness_val_lu;
    uint16_t shutter_time;
    uint16_t shutter_time_on_ae_min;
    uint16_t shutter_time_on_ae_mas;
    default_val()
      : sensor_mode(0)
      , auto_exposure(true)
      , sensor_gain()
      , isp_sensor_gain(0)
      , distortion_correction(0)
      , awb(0)
      , awb_gain_r(0)
      , awb_gain_g(0)
      , awb_gain_b(0)
      , hue_mode(0)
      , hue_val(0)
      , contrast_mode(0)
      , contrast_val(0)
      , brightness_mode(0)
      , brightness_val(0)
      , saturation_mode(0)
      , saturation_val(0)
      , sharpness_mode(0)
      , sharpness_val_ld(0)
      , sharpness_val_ldu(0)
      , sharpness_val_lu(0)
      , shutter_time(0)
      , shutter_time_on_ae_min(0)
      , shutter_time_on_ae_mas(0)
    {
    }
  } dval;

  bool isAvailableCamera(void) override;
  void saveCurrentValue(std::string output) override;
  // set parameter

  int setDWP(bool on);
  int setSensorMode(uint8_t mode);
  int setAutoExposure(bool on);
  int setSensorGain(float gain);
  int setISPSensorGain(float gain);
  int setDistortionCorrection(bool on);
  int setAutoWhiteBalance(bool on);
  int setAutoWhiteBalanceGainR(int val);
  int setAutoWhiteBalanceGainB(int val);
  int setHue(bool on);
  int setHueVal(int val);
  int setContrast(bool on);
  int setContrastVal(int val);
  int setBrightness(bool on);
  int setBrightnessVal(int val);
  int setSaturation(bool on);
  int setSaturationVal(int val);
  int setSharpness(bool on);
  int setSharpnessVal_Ld(int val);
  int setSharpnessVal_Ldu(int val);
  int setSharpnessVal_Lu(int val);
  int setShutterTimeXXX(uint16_t time_ms);
  int setShutterTimeOnAE(uint16_t ms);
};

class C1 : public t4cam
{
public:
  C1(int _port_num = 0, std::string param_file = "./default.yaml")
  {
    port_num = _port_num;

    if (port_num < 0 || port_num >= MAX_PORT)
    {
      std::cerr << "The port number has exceeded the maximum value. Please specify between 0-" << MAX_PORT - 1
                << std::endl;
      exit(-1);
    }

    dev_name = "/dev/" + portnum_table[port_num];
    i2c_dev_addr = (port_num % 2) == 0 ? 0x1b : 0x1c;

    DEBUG_PRINT("model: C1\n");
    DEBUG_PRINT("port_num: %d\n", port_num);
    DEBUG_PRINT("dev_name: %s\n", dev_name.c_str());
    DEBUG_PRINT("dev_addr: 0x%x\n", i2c_dev_addr);

//    initialized_default_value_from_default();
  }

  bool isAvailableCamera(void) override;
  void saveCurrentValue(std::string output) override;
  int initialized_load_value_from_file(const std::string &file_name) override;

  uint8_t getAEMode(void);
  int8_t setAEMode(int mode);
  int8_t setDigitalGain(int db);
  int8_t setAnalogGain(int db);

  // set parameter

  // for easily tuning
  int8_t setSharpness(float gain);
  int8_t setHue(int deg);
  int8_t setSaturation(float gain);
  int8_t setBrightness(float offset);
  int8_t setContrast(float gain);

  int8_t setAutoWhiteBalance(bool on);
  int8_t setWhiteBalanceGain(float r_gain, float gr_gain, float gb_gain, float b_gain);
  int8_t setExposureOffsetFlag(bool flag);

  int setATRContrastGain(int gain);
  int setATRBrightnessGain(int gain);


  // get parameter
  int getDigitalGain();
  int getAnalogGain();
  float getSharpness();
  int getHue();
  float getSaturation();
  float getBrightness();
  float getContrast();

  int getExposureOffsetFlag();
  int8_t setExposureOffset(float offset);
  float getExposureOffset();

  int8_t checkES3();
  int test();

  //
  float getTempature(int type);
  float getTempatureS0();
  float getTempatureS1();

  int8_t setShutterSpeedforFME(float val);
  float getShutterSpeedforFME();

  float getAEError();
};

#endif
