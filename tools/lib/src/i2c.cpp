// Copyright 2022 Tier IV, Inc.
//
// Proprietary

#include "i2c.hpp"

int8_t i2c::read(std::string dev_name, uint8_t dev_addr, size_t length, uint8_t* data)
{
  int32_t fd = open(dev_name.c_str(), O_RDWR);
  if (fd == -1)
  {
    fprintf(stderr, "i2c_read: failed to open: %s\n", strerror(errno));
    return -1;
  }

  struct i2c_msg messages[1];
  messages[0].addr = dev_addr;
  messages[0].flags = I2C_M_RD;
  messages[0].len = length;
  messages[0].buf = data;

  struct i2c_rdwr_ioctl_data ioctl_data;
  ioctl_data.msgs = messages;
  ioctl_data.nmsgs = 1;

  if (ioctl(fd, I2C_RDWR, &ioctl_data) != 2)
  {
    fprintf(stderr, "i2c_read: failed to ioctl: %s\n", strerror(errno));
    close(fd);
    return -1;
  }

  for (size_t i = 0; i < length; i++)
  {
    _DEBUG_PRINT("data[%x]:0x%x\n", i, data[i]);
  }
  close(fd);
  return 0;
}

int8_t i2c::read16(std::string dev_name, uint8_t dev_addr, uint16_t reg_addr, uint8_t* data)
{
  int8_t ret = 0;
  int32_t fd = open(dev_name.c_str(), O_RDWR);
  if (fd == -1)
  {
    fprintf(stderr, "%s: failed to open %s: %s\n", __func__, dev_name.c_str(), strerror(errno));
    return -1;
  }

  uint8_t reg_addr_mask_u = (reg_addr >> 8) & 0xff;
  uint8_t reg_addr_mask_l = reg_addr & 0xff;
  fprintf(stderr, "%x, %x\n", reg_addr_mask_u, reg_addr_mask_l);

  if (write8(dev_name, dev_addr, reg_addr_mask_u, reg_addr_mask_l) < 0)
  {
    return -1;
  }
  if (read8(dev_name, dev_addr, 0, data) < 0)
  {
    return -1;
  }
  _DEBUG_PRINT("[0x%02X-0x%04X-read]: = 0x%02X\n", dev_addr, reg_addr, *data);

  close(fd);
  return 0;
}

int8_t i2c::check_device(std::string dev_name, uint8_t dev_addr, uint16_t reg_addr)
{
  int8_t ret = 0;
  int32_t fd = open(dev_name.c_str(), O_RDWR);
  if (fd == -1)
  {
    return -1;
  }

  uint8_t reg_addr_mask_u = (reg_addr >> 8) & 0xff;
  uint8_t reg_addr_mask_l = reg_addr & 0xff;

  uint8_t buffer[2] = { reg_addr_mask_u, reg_addr_mask_l };

  struct i2c_msg messages[1];
  messages[0].addr = dev_addr;
  messages[0].flags = 0;  // write
  messages[0].len = 2;    // length
  messages[0].buf = buffer;

  struct i2c_rdwr_ioctl_data ioctl_data;
  ioctl_data.msgs = messages;
  ioctl_data.nmsgs = 1;

  if (ioctl(fd, I2C_RDWR, &ioctl_data) != 1)
  {
    close(fd);
    return -1;
  }
  usleep(1000);

  close(fd);
  return 0;
}

int8_t i2c::read8(std::string dev_name, uint8_t dev_addr, uint8_t reg_addr, uint8_t* data)
{
  int32_t fd = open(dev_name.c_str(), O_RDWR);
  if (fd == -1)
  {
    fprintf(stderr, "%s: failed to open %s: %s\n", __func__, dev_name.c_str(), strerror(errno));
    return -1;
  }

  struct i2c_msg messages[2];
  // set the register address
  messages[0].addr = dev_addr;
  messages[0].flags = 0;  // write
  messages[0].len = 1;    // length
  messages[0].buf = &reg_addr;

  // read command
  messages[1].addr = dev_addr;
  messages[1].flags = I2C_M_RD;
  messages[1].len = 1;  // length(byte)
  messages[1].buf = data;

  struct i2c_rdwr_ioctl_data ioctl_data;
  ioctl_data.msgs = messages;
  ioctl_data.nmsgs = 2;

  if (ioctl(fd, I2C_RDWR, &ioctl_data) < 0)
  {
    fprintf(stderr, "i2c_read: failed to ioctl: %s\n", strerror(errno));
    close(fd);
    return -1;
  }
  usleep(1000);

  close(fd);
  return 0;
}

int8_t i2c::write8(std::string dev_name, uint8_t dev_addr, uint8_t reg_addr, uint8_t data)
{
  int32_t fd = open(dev_name.c_str(), O_RDWR);
  if (fd == -1)
  {
    fprintf(stderr, "i2c_write: failed to open: %s\n", strerror(errno));
    return -1;
  }

  uint8_t buffer[2] = { reg_addr, data };

  struct i2c_msg messages[1];
  messages[0].addr = dev_addr;
  messages[0].flags = 0;  // write
  messages[0].len = 2;    // length
  messages[0].buf = buffer;

  struct i2c_rdwr_ioctl_data ioctl_data;
  ioctl_data.msgs = messages;
  ioctl_data.nmsgs = 1;

  _DEBUG_PRINT("[0x%02X-write]: = 0x%02X%02X\n", dev_addr, reg_addr, data);
  if (ioctl(fd, I2C_RDWR, &ioctl_data) < 0)
  {
    fprintf(stderr, "i2c_write: failed to ioctl: %s\n", strerror(errno));
    close(fd);
    return -1;
  }
  usleep(1000);

  close(fd);
  return 0;
}

int8_t i2c::read(std::string dev_name, uint8_t dev_addr, uint8_t* data, uint16_t length)
{
  int32_t fd = open(dev_name.c_str(), O_RDWR);
  if (fd == -1)
  {
    fprintf(stderr, "%s: failed to open %s: %s\n", __func__, dev_name.c_str(), strerror(errno));
    return -1;
  }

  struct i2c_msg messages[2];
  messages[0].addr = dev_addr;
  messages[0].flags = I2C_M_RD;
  messages[0].len = length;
  messages[0].buf = data;

  struct i2c_rdwr_ioctl_data ioctl_data;
  ioctl_data.msgs = messages;
  ioctl_data.nmsgs = 1;

  if (ioctl(fd, I2C_RDWR, &ioctl_data) < 0)
  {
    fprintf(stderr, "i2c_read: failed to ioctl: %s\n", strerror(errno));
    close(fd);
    return -1;
  }

  fprintf(stderr, "[%s]:", __func__);
  for (int i = 0; i < length; i++)
  {
    fprintf(stderr, " 0x%x", data[i]);
  }
  fprintf(stderr, "\n");

  usleep(1000);

  close(fd);
  return 0;
}

int8_t i2c::transfer(std::string dev_name, uint8_t dev_addr, const uint8_t* wdata, uint16_t wlength)
{
  int8_t ret = 0;
  uint8_t buf[10];
  uint8_t* buffer = new uint8_t[wlength + 1];
  if (buffer == nullptr)
  {
    fprintf(stderr, "%s: failed to allocate memory: %s\n", __func__, strerror(errno));
    return -1;
  }
  memcpy(&buffer[0], wdata, wlength);
  memset(buf, 0, sizeof(buf));

  struct i2c_msg messages[2];
  //
  messages[0].addr = dev_addr;
  messages[0].flags = 0;          // write
  messages[0].len = wlength + 1;  // length
  messages[0].buf = buffer;

  // trash receive data
  messages[1].addr = dev_addr;
  messages[1].flags = 1;  // read
  messages[1].len = 6;    // length
  messages[1].buf = buf;

  struct i2c_rdwr_ioctl_data ioctl_data;
  ioctl_data.msgs = messages;
  ioctl_data.nmsgs = 2;

#ifdef X86_DEBUG
  for (size_t i = 0; i < ioctl_data.nmsgs; i++)
  {
    uint32_t sum = 0;
    fprintf(stderr, "[%c][%2d][0x%0x]", messages[i].flags == 0 ? 'W' : 'R', messages[i].len, messages[i].addr);
    for (size_t j = i; j < messages[i].len - 1; j++)
    {
      fprintf(stderr, "%02x, ", messages[i].buf[j]);
      sum += messages[i].buf[j];
    }
    sum -= messages[i].buf[messages[i].len - 2];
    fprintf(stderr, ":::[0x%x]\n", sum);
  }
  delete[] buffer;
  return 0;
#endif

  int32_t fd = open(dev_name.c_str(), O_RDWR);
  if (fd == -1)
  {
    fprintf(stderr, "%s: Failed to open device %s: %s\n", __func__, dev_name.c_str(), strerror(errno));
    ret = -1;
  }
  else if (ioctl(fd, I2C_RDWR, &ioctl_data) < 0)
  {
    fprintf(stderr, "i2c_write: failed to ioctl: %s\n", strerror(errno));
    ret = -1;
  }
  for (size_t i = 0; i < ioctl_data.nmsgs; i++)
  {
    uint32_t sum = 0;
    _DEBUG_PRINT("[%c][%2d][0x%0x]", messages[i].flags == 0 ? 'W' : 'R', messages[i].len, messages[i].addr);
    for (size_t j = i; j < messages[i].len - 1; j++)
    {
      _DEBUG_PRINT("%02x, ", messages[i].buf[j]);
      sum += messages[i].buf[j];
    }
    sum -= messages[i].buf[messages[i].len - 2];
    _DEBUG_PRINT(":::[0x%x]\n", sum);
  }

  delete[] buffer;
  close(fd);
  return ret;
}
int8_t i2c::transfer(std::string dev_name, uint8_t dev_addr, const uint8_t* wdata, uint16_t wlength, uint8_t* rdata,
                     uint16_t rlength)
{
  return 0;
}

int8_t i2c::write(std::string dev_name, uint8_t dev_addr, const uint8_t* data, uint16_t length)
{
  int32_t fd = open(dev_name.c_str(), O_RDWR);
  if (fd == -1)
  {
    fprintf(stderr, "%s: failed to open %s: %s\n", __func__, dev_name.c_str(), strerror(errno));
    //   return -1;
  }

  uint8_t* buffer = (uint8_t*)malloc(length + 1);
  if (buffer == nullptr)
  {
    fprintf(stderr, "%s: failed to allocate memory: %s\n", __func__, strerror(errno));
    close(fd);
    return -1;
  }
  memcpy(&buffer[0], data, length);

  // debug print
  fprintf(stderr, "[%s]:", __func__);
  for (int i = 0; i < length; i++)
  {
    fprintf(stderr, " 0x%02x", buffer[i]);
  }
  fprintf(stderr, "\n");

  struct i2c_msg messages[1];
  messages[0].addr = dev_addr;
  messages[0].flags = 0;         // write
  messages[0].len = length + 1;  // length
  messages[0].buf = buffer;

  struct i2c_rdwr_ioctl_data ioctl_data;
  ioctl_data.msgs = messages;
  ioctl_data.nmsgs = 1;

  /* i2c-writeを行う. */
  if (ioctl(fd, I2C_RDWR, &ioctl_data) < 0)
  {
    fprintf(stderr, "i2c_write: failed to ioctl: %s\n", strerror(errno));
    free(buffer);
    close(fd);
    return -1;
  }

  free(buffer);
  close(fd);
  return 0;
}

#if 0
int8_t i2c::write(std::string dev_name, uint8_t dev_addr, uint8_t reg_addr, const uint8_t* data, uint16_t length)
{
  int32_t fd = open(dev_name.c_str(), O_RDWR);
  if (fd == -1)
  {
    fprintf(stderr, "%s: failed to open %s: %s\n", __func__, dev_name.c_str(), strerror(errno));
    return -1;
  }

  uint8_t* buffer = (uint8_t*)malloc(length + 1);
  if (buffer == nullptr)
  {
    fprintf(stderr, "%s: failed to allocate memory: %s\n", __func__, strerror(errno));
    close(fd);
    return -1;
  }
  buffer[0] = reg_addr;
  memcpy(&buffer[1], data, length);

  fprintf(stderr, "[%s]:", __func__);
  for (int i = 0; i < length; i++)
  {
    fprintf(stderr, " 0x%x", buffer[i]);
  }

  struct i2c_msg messages[1];
  messages[0].addr = dev_addr;
  messages[0].flags = 0;         // write
  messages[0].len = length + 1;  // length
  messages[0].buf = buffer;

  struct i2c_rdwr_ioctl_data ioctl_data;
  ioctl_data.msgs = messages;
  ioctl_data.nmsgs = 1;

  _DEBUG_PRINT("[0x%02X-0x%04X-write]: = 0x%02X\n", dev_addr, reg_addr, *data);
  /* i2c-writeを行う. */
  if (ioctl(fd, I2C_RDWR, &ioctl_data) != 1)
  {
    fprintf(stderr, "i2c_write: failed to ioctl: %s\n", strerror(errno));
    free(buffer);
    close(fd);
    return -1;
  }

  free(buffer);
  close(fd);
  return 0;
}
#endif

int8_t i2c::write16(std::string dev_name, uint8_t dev_addr, uint16_t reg_addr, uint8_t data)
{
  int32_t fd = open(dev_name.c_str(), O_RDWR);

  if (fd == -1)
  {
    fprintf(stderr, "%s: failed to open %s: %s\n", __func__, dev_name.c_str(), strerror(errno));
    return -1;
  }

  uint8_t buffer[3] = { (uint8_t)(reg_addr >> 8), (uint8_t)(reg_addr & 0xFF), data };

  struct i2c_msg messages[1];
  messages[0].addr = dev_addr;
  messages[0].flags = 0;  // write
  messages[0].len = 3;    // length
  messages[0].buf = buffer;

  struct i2c_rdwr_ioctl_data ioctl_data;
  ioctl_data.msgs = messages;
  ioctl_data.nmsgs = 1;

  _DEBUG_PRINT("[0x%02X-0x%04X-write]: = 0x%02X\n", dev_addr, reg_addr, data);
  /* i2c-writeを行う. */
  if (ioctl(fd, I2C_RDWR, &ioctl_data) != 1)
  {
    fprintf(stderr, "i2c_write: failed to ioctl: %s\n", strerror(errno));
    close(fd);
    return -1;
  }

  close(fd);
  return 0;
}
