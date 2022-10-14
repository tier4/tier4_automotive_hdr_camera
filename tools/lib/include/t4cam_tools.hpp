// Copyright 2022 Tier IV, Inc.
// Proprietary

#ifndef __T4CAM_TOOLS_HPP__
#define __T4CAM_TOOLS_HPP__

#include <memory>
#include <iostream>

#define MAX_PORT 8
  
static const std::array<std::string, 8> portnum_table =
  {"i2c-30","i2c-30", "i2c-31", "i2c-31", "i2c-32", "i2c-32", "i2c-33", "i2c-33"};

class C1
{
private:
  int port_num;
  std::string dev_name;
  uint8_t i2c_dev_addr;
  


public:
  C1(int _port_num = 0) : port_num(_port_num)
  {
    if(port_num < 0 || port_num >= MAX_PORT){
       std::cerr << "The port number has exceeded the maximum value. Please specify between 0-" << MAX_PORT-1 << std::endl;
       exit(-1);
    }

    dev_name = "/dev/" + portnum_table[port_num];
    i2c_dev_addr = (port_num % 2) == 0? 0x1b:0x1c;

#ifdef DEBUG
    fprintf(stdout, "port_num: %d\n", port_num);
    fprintf(stdout, "dev_name: %s\n", dev_name.c_str());
    fprintf(stdout, "dev_addr: 0x%x\n", i2c_dev_addr);
#endif
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

#define ERRSCL_L  0x617C
#define ERRSCL_U  0x617D


  float getAEError();
  int8_t setExposureOffsetFlag(bool flag);

};

#endif
