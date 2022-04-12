// Copyright 2022 Tier IV, Inc.
// Proprietary

#ifndef __T4CAM_TOOLS_HPP__
#define __T4CAM_TOOLS_HPP__

#include <memory>

#define PORT_A_CAM "/dev/i2c-30"
#define DEFAULT_CAM_PORT PORT_A_CAM
#define DEFAULT_DEV_ADDR 0x1a

class C1
{
private:
  std::string dev_name;
  uint8_t i2c_dev_addr;

public:
  C1(std::string _dev_name = DEFAULT_CAM_PORT, uint8_t _dev_addr = DEFAULT_DEV_ADDR) : dev_name(_dev_name), i2c_dev_addr(_dev_addr)
  {
    checkES3();
  }

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

};

#endif
