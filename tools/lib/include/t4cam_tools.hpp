// Copyright 2022 Tier IV, Inc.
// Proprietary

#ifndef __T4CAM_TOOLS_HPP__
#define __T4CAM_TOOLS_HPP__

#include <yaml-cpp/yaml.h>

#include <fstream>
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

class C2
{
private:
  int port_num;
  std::string dev_name;
  uint8_t i2c_dev_addr;

public:
  C2(int _port_num = 0) : port_num(_port_num)
  {
    if (port_num < 0 || port_num >= MAX_PORT)
    {
      std::cerr << "The port number has exceeded the maximum value. Please specify between 0-" << MAX_PORT - 1
                << std::endl;
      exit(-1);
    }
    dev_name = "/dev/" + portnum_table[port_num];
    i2c_dev_addr = (port_num % 2) == 0 ? 0x2b : 0x2c;

#ifdef DEBUG
    fprintf(stdout, "model: C2\n");
    fprintf(stdout, "port_num: %d\n", port_num);
    fprintf(stdout, "dev_name: %s\n", dev_name.c_str());
    fprintf(stdout, "dev_addr: 0x%x\n", i2c_dev_addr);
#endif
  }

void setShutterTimeOnAE(uint16_t max_ms, uint16_t min_ms);
  void set_dwp_on();
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
    fprintf(stdout, "model: C1\n");
    fprintf(stdout, "port_num: %d\n", port_num);
    fprintf(stdout, "dev_name: %s\n", dev_name.c_str());
    fprintf(stdout, "dev_addr: 0x%x\n", i2c_dev_addr);
#endif

    initialized_default_value_from_default();
    // initialized_load_value_from_file(param_file);
  }

  std::unordered_map<std::string, Param> default_value_map;
  std::unordered_map<std::string, Param> load_value_map;

  void initialized_default_value_from_default()
  {
  }

  void saveCurrentValue(std::string output)
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

  int initialized_load_value_from_file(const std::string &file_name)
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

  void setDefaultValue(void)
  {
    std::cerr << "not present this function." << std::endl;
  }
  void setLoadValue(void)
  {
    std::cerr << "not present this function." << std::endl;
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

  int getDigitalGain();
  float getSharpness();
  int getHue();
  float getSaturation();
  float getBrightness();
  float getContrast();

  int8_t setAutoWhiteBalance(bool on);
  int8_t setWhiteBalanceGain(float r_gain, float gr_gain, float gb_gain, float b_gain);

  int8_t setExposureOffsetFlag(bool flag);
  int getExposureOffsetFlag();
  int8_t setExposureOffset(float offset);
  float getExposureOffset();

  int8_t checkES3();

  float getTempature(int type);
  float getTempatureS0();
  float getTempatureS1();

  int8_t setShutterSpeedforFME(float val);
  float getShutterSpeedforFME();

  float getAEError();
};

#endif
