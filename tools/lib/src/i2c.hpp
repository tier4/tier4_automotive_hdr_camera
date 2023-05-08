// Copyright 2022 Tier IV, Inc.
// proprietary
//

#ifndef _I2C_LIB__
#define _I2C_LIB__

#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

#include <string>

#ifdef DEBUG
#define _DEBUG_PRINT printf
#else
#define _DEBUG_PRINT(...)                                                                                               \
  do                                                                                                                   \
  {                                                                                                                    \
  } while (0)
#endif

class i2c
{
private:
public:
  i2c()
  {
  }

  // 16, 8 are address size. not data size;
  static int8_t read16(std::string dev_name, uint8_t dev_addr, uint16_t reg_addr, uint8_t* data);
  static int8_t read8(std::string dev_name, uint8_t dev_addr, uint8_t reg_addr, uint8_t* data);
  static int8_t write16(std::string dev_name, uint8_t dev_addr, uint16_t reg_addr, uint8_t data);
  static int8_t write8(std::string dev_name, uint8_t dev_addr, uint8_t reg_addr, uint8_t data);
  //  static int8_t write(std::string dev_name, uint8_t dev_addr, uint8_t reg_addr, const uint8_t* data, uint16_t
  //  length);
  //
  static int8_t write(std::string dev_name, uint8_t dev_addr, std::vector<uint8_t> &cmd);
  //
  static int8_t write(std::string dev_name, uint8_t dev_addr, const uint8_t* data, uint16_t length);
  static int8_t read(std::string dev_name, uint8_t dev_addr, uint8_t* data, uint16_t length);
  static int8_t check_device(std::string dev_name, uint8_t dev_addr, uint16_t reg_addr);
  
  static int8_t transfer(std::string dev_name, uint8_t dev_addr, const uint8_t*wdata, uint16_t wlength);
  static int8_t transfer(std::string dev_name, uint8_t dev_addr, const uint8_t*wdata, uint16_t wlength, uint8_t *rdata, uint16_t rlength);
};
#endif
