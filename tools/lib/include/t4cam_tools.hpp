// Copyright 2022 Tier IV, Inc.
// Proprietary

#ifndef __T4CAM_TOOLS_HPP__
#define __T4CAM_TOOLS_HPP__

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <memory>
#include <unordered_map>

#define MAX_PORT 8

static const std::array<std::string, 8> portnum_table = { "i2c-30", "i2c-30", "i2c-31", "i2c-31",
                                                          "i2c-32", "i2c-32", "i2c-33", "i2c-33" };

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

class C1
{
private:
  int port_num;
  std::string dev_name;
  uint8_t i2c_dev_addr;

public:
  C1(int _port_num = 0, std::string param_file = "./default.yaml") : port_num(_port_num)
  {
    if (port_num < 0 || port_num >= MAX_PORT)
    {
      std::cerr << "The port number has exceeded the maximum value. Please specify between 0-" << MAX_PORT - 1
                << std::endl;
      exit(-1);
    }

    dev_name = "/dev/" + portnum_table[port_num];
    i2c_dev_addr = (port_num % 2) == 0 ? 0x1b : 0x1c;

#ifdef DEBUG
    fprintf(stdout, "port_num: %d\n", port_num);
    fprintf(stdout, "dev_name: %s\n", dev_name.c_str());
    fprintf(stdout, "dev_addr: 0x%x\n", i2c_dev_addr);
#endif

    initialized_default_value_from_file(param_file);
  }

  std::unordered_map<std::string, Param> default_value_map;

  template <class T>
  T void :

    void
    initialized_default_value_from_default()
  {
    default_value_map["ae_mode"] = 1.0;
    default_value_map["digital_gain"] = 1.0;
    default_value_map["shutter_speed"] = 1.0;
    default_value_map["ev_offset_flag"] = 1.0;
    default_value_map["ev_offset"] = 1.0;
    default_value_map["hue"] = 1.0;
    default_value_map["saturation"] = 1.0;
    default_value_map["contrast"] = 1.0;
    default_value_map["brightness"] = 1.0;
    default_value_map["sharpness"] = 1.0;
  }

  void initialized_default_value_from_file(const std::string &file_name)
  {
    initialized_default_value_from_default();
    YAML::Node node;
    try
    {
      node = YAML::LoadFile(file_name);
      std::cout << node["config"].size() << std::endl;
      for (size_t i = 0; i < node["config"].size(); i++)
      {
        std::string param_name = node["config"][i]["param_name"].as<std::string>();
        float val = node["config"][i]["value"].as<float>();
        std::cout << "param_name:" << param_name << std::endl;
        std::cout << "value:" << val << std::endl;
        default_value_map[param_name] = val;
      }
    }
    catch (const std::exception &e)
    {
      std::cerr << "Can not open yaml file:" << e.what() << std::endl;
    }
  }

  void setDefaultValue(void)
  {
    setAEMode(default_value_map["ae_mode"]);
    setDigitalGain(default_value_map["digital_gain"]);
    setShutterSpeedforFME(default_value_map["shutter_speed"]);
    setExposureOffsetFlag(default_value_map["ev_offset_flag"]);
    setExposureOffset(default_value_map["ev_offset"]);
    setHue(default_value_map["hue"]);
    setSaturation(default_value_map["saturation"]);
    setBrightness(default_value_map["contrast"]);
    setContrast(default_value_map["brightness"]);
    setSharpness(default_value_map["sharpness"]);
  }

  bool isAvailableCamera(void);

  uint8_t getAEMode(void);
  int8_t setAEMode(int mode);

  int8_t setDigitalGain(int db);
  int8_t setSharpness(float gain);
  int8_t setHue(int deg);
  int8_t setSaturation(float gain);
  int8_t setBrightness(float offset);
  int8_t setContrast(float gain);

  int8_t setAutoWhiteBalance(bool on);
  int8_t setWhiteBalanceGain(float r_gain, float gr_gain, float gb_gain, float b_gain);
  int8_t setExposureOffset(float offset);

  int8_t checkES3();

  float getTempature(int type);
  float getTempatureS0();
  float getTempatureS1();

  int8_t setShutterSpeedforFME(int val);

#define ERRSCL_L 0x617C
#define ERRSCL_U 0x617D

  float getAEError();
  int8_t setExposureOffsetFlag(bool flag);
};

#endif
