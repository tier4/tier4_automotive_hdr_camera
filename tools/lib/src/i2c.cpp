// Copyright 2022 Tier IV, Inc.
//
// Proprietary

#include "i2c.hpp"

int8_t i2c::read16(std::string dev_name, uint8_t dev_addr, uint16_t reg_addr, uint8_t* data) {

  int8_t ret = 0;
  int32_t fd = open(dev_name.c_str(), O_RDWR);
  if (fd == -1) {
    fprintf(stderr, "%s: failed to open %s: %s\n", __func__, dev_name.c_str(), strerror(errno));
    return -1;
  }

  uint8_t reg_addr_mask_u = (reg_addr >> 8) &0xff;
  uint8_t reg_addr_mask_l = reg_addr&0xff;
 
  if(write8(dev_name, dev_addr, reg_addr_mask_u, reg_addr_mask_l) < 0){
	  return -1;
  }
  if(read8(dev_name, dev_addr, 0, data)<0){
	  return -1;
  }
  DEBUG_PRINT("[0x%02X-0x%04X]: = 0x%02X\n",dev_addr, reg_addr, *data);

  close(fd);
  return 0;
}

int8_t i2c::read8(std::string dev_name, uint8_t dev_addr, uint8_t reg_addr, uint8_t* data) {
  int32_t fd = open(dev_name.c_str(), O_RDWR);
  if (fd == -1) {
    fprintf(stderr, "%s: failed to open %s: %s\n", __func__, dev_name.c_str(), strerror(errno));
    return -1;
  }

  struct i2c_msg messages[2];
  messages[0].addr = dev_addr;
  messages[0].flags = 0; //write
  messages[0].len = 1; //length
  messages[0].buf = &reg_addr;

  messages[1].addr = dev_addr;
  messages[1].flags = I2C_M_RD;
  messages[1].len = 1;
  messages[1].buf = data;

  struct i2c_rdwr_ioctl_data ioctl_data;
  ioctl_data.msgs = messages;
  ioctl_data.nmsgs = 2;

  if (ioctl(fd, I2C_RDWR, &ioctl_data) != 2) {
    fprintf(stderr, "i2c_read: failed to ioctl: %s\n", strerror(errno));
    close(fd);
    return -1;
  }

  close(fd);
  return 0;
}

int8_t i2c::write8(std::string dev_name, uint8_t dev_addr, uint8_t reg_addr, uint8_t data)
{
  int32_t fd = open(dev_name.c_str(), O_RDWR);
  if (fd == -1) {
    fprintf(stderr, "i2c_write: failed to open: %s\n", strerror(errno));
    return -1;
  }

  uint8_t buffer[2] = {reg_addr, data};

  struct i2c_msg messages[1];
  messages[0].addr = dev_addr;
  messages[0].flags = 0; //write
  messages[0].len = 2; //length
  messages[0].buf = buffer;

  struct i2c_rdwr_ioctl_data ioctl_data;
  ioctl_data.msgs = messages;
  ioctl_data.nmsgs = 1;

  DEBUG_PRINT("[0x%02X-0x%04X]: = 0x%02X\n",dev_addr, reg_addr, data);
  if (ioctl(fd, I2C_RDWR, &ioctl_data) != 1) {
    fprintf(stderr, "i2c_write: failed to ioctl: %s\n", strerror(errno));
    close(fd);
    return -1;
  }

  close(fd);
  return 0;

}

int8_t i2c::write(std::string dev_name, uint8_t dev_addr, uint8_t reg_addr, const uint8_t* data, uint16_t length) {

  int32_t fd = open(dev_name.c_str(), O_RDWR);
  if (fd == -1) {
    fprintf(stderr, "%s: failed to open %s: %s\n", __func__, dev_name.c_str(), strerror(errno));
    return -1;
  }

  uint8_t* buffer = (uint8_t*)malloc(length + 1);
  if (buffer == nullptr) {
    fprintf(stderr, "%s: failed to allocate memory: %s\n", __func__, strerror(errno));
    close(fd);
    return -1;
  }
  buffer[0] = reg_addr;              
  memcpy(&buffer[1], data, length); 

  struct i2c_msg messages[1];
  messages[0].addr = dev_addr;
  messages[0].flags = 0; //write
  messages[0].len = length+1; //length
  messages[0].buf = buffer;

  struct i2c_rdwr_ioctl_data ioctl_data;
  ioctl_data.msgs = messages;
  ioctl_data.nmsgs = 1;

  DEBUG_PRINT("[0x%02X-0x%04X]: = 0x%02X\n",dev_addr, reg_addr, *data);
  /* i2c-writeを行う. */
  if (ioctl(fd, I2C_RDWR, &ioctl_data) != 1) {
    fprintf(stderr, "i2c_write: failed to ioctl: %s\n", strerror(errno));
    free(buffer);
    close(fd);
    return -1;
  }

  free(buffer);
  close(fd);
  return 0;
}

int8_t i2c::write16(std::string dev_name, uint8_t dev_addr, uint16_t reg_addr, uint8_t data)
{
  int32_t fd = open(dev_name.c_str(), O_RDWR);
  if (fd == -1) {
    fprintf(stderr, "%s: failed to open %s: %s\n", __func__, dev_name.c_str(), strerror(errno));
    return -1;
  }

  uint8_t buffer[3] = {(uint8_t)(reg_addr >> 8), (uint8_t)(reg_addr & 0xFF), data};

  struct i2c_msg messages[1];
  messages[0].addr = dev_addr;
  messages[0].flags = 0; //write
  messages[0].len = 3; //length
  messages[0].buf = buffer;

  struct i2c_rdwr_ioctl_data ioctl_data;
  ioctl_data.msgs = messages;
  ioctl_data.nmsgs = 1;

  DEBUG_PRINT("[0x%02X-0x%04X]: = 0x%02X\n",dev_addr, reg_addr, data);
  /* i2c-writeを行う. */
  if (ioctl(fd, I2C_RDWR, &ioctl_data) != 1) {
    fprintf(stderr, "i2c_write: failed to ioctl: %s\n", strerror(errno));
    close(fd);
    return -1;
  }

  close(fd);
  return 0;
}
