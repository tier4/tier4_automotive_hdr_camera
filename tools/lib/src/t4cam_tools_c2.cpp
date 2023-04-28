// Copyright 2022 Tier IV, Inc.
// Proprietary

#include "c2_reg.hpp"
#include "gw5300.hpp"
#include "i2c.hpp"
#include "t4cam_tools.hpp"


void C2::setSensorGain(float gain)
{
  DEBUG_PRINT("[%s]\n\n",__func__);
  uint8_t buf[10];
  uint8_t cmd_sensor_gain[] = { 0x33, 0x47, 0x0f, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
                                0x00, 0x09, 0x00, 0x01, 0x00, 0x04, 0x00, /*pos*/0x00, 0x00, 0xc8,
                                0x42, 0x00 };
  const size_t val_pos = 17;
  
  // total gain = sensor gain x isp gain
  //1 ~ 503
  gain = gain <= 503.0 ? 
         gain < 1.0? 1.0:gain:503.0;

  uint32_t *gain_hex = (uint32_t*) &gain;

  cmd_sensor_gain[val_pos] = *gain_hex & 0xff;
  cmd_sensor_gain[val_pos+1] = (*gain_hex >>8) &0xff;
  cmd_sensor_gain[val_pos+2] = (*gain_hex >>16) &0xff;
  cmd_sensor_gain[val_pos+3] = (*gain_hex >>24) &0xff;
 
  cmd_sensor_gain[sizeof(cmd_sensor_gain)-1] = calcCheckSum(cmd_sensor_gain, sizeof(cmd_sensor_gain));

  i2c::transfer(dev_name, i2c_dev_addr, cmd_sensor_gain, sizeof(cmd_sensor_gain));

}

void C2::setDistortionCorrection(bool on)
{
  DEBUG_PRINT("[%s]\n\n",__func__);
  uint8_t buf[10];

  // 13byte
  uint8_t cmd_dwp_on[] = { 0x6d, 0x33, 0x47, 0x6, 0x0, 0x0, 0x0, 0x4d, 0x0, 0x80, 0x4, 0x0, 0x1, 0x52 };
  // 10byte
  uint8_t cmd_dwp_off[] = { 0x6d, 0x33, 0x47, 0x3, 0x0, 0x0, 0x0, 0x45, 0x0, 0x80, 0x42 };

  if(on){
    i2c::write(dev_name, i2c_dev_addr, cmd_dwp_on, sizeof(cmd_dwp_on));
    i2c::read(dev_name, i2c_dev_addr, buf, 6);
  }
  else{
    i2c::write(dev_name, i2c_dev_addr, cmd_dwp_off,sizeof(cmd_dwp_off));
    i2c::read(dev_name, i2c_dev_addr, buf, 6);
  }

}


void C2::setShutterTimeOnAE(uint16_t max_ms, uint16_t min_ms){
  DEBUG_PRINT("[%s]\n\n",__func__);
#define MS_TO_LINE_UNIT 80
  uint8_t buf[8];

  uint8_t cmd_integration_max[] = {
   0x33, 0x47, 0x0f, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
   0x00, 0x15, 0x00, 0x01, 0x00, 0x04, 0x00, 0x70, 0x03, 0x00,
   0x00, 0x00};
    
  uint8_t cmd_integration_min[] = {
   0x33, 0x47, 0x0d, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05,
   0x00, 0x21, 0x00, 0x01, 0x00, 0x02, 0x00, 0x70, 0x03, 0x00};


  uint32_t max_line =  max_ms * MS_TO_LINE_UNIT;
  uint32_t min_line =  min_ms * MS_TO_LINE_UNIT;

  uint8_t b1 = max_line &0xFF;
  uint8_t b2 = (max_line >> 8)&0xFF;
  uint8_t b3 = 0;
  uint8_t b4 = 0;

  const size_t max_val_pos = 17;
  const size_t min_val_pos = 17;

  cmd_integration_max[max_val_pos] = b1;
  cmd_integration_max[max_val_pos+1] = b2;
  cmd_integration_max[max_val_pos+2] = b3;
  cmd_integration_max[max_val_pos+3] = b4;
  
  b1 = min_line &0xFF;
  b2 = (min_line >> 8)&0xFF;
  
  cmd_integration_min[min_val_pos] = b1;
  cmd_integration_min[min_val_pos+1] = b2;

  cmd_integration_max[sizeof(cmd_integration_max)-1] = calcCheckSum(cmd_integration_max, sizeof(cmd_integration_max));
  cmd_integration_min[sizeof(cmd_integration_min)-1] = calcCheckSum(cmd_integration_min, sizeof(cmd_integration_min));


  i2c::write(dev_name, i2c_dev_addr, cmd_integration_max, sizeof(cmd_integration_max));
  i2c::read(dev_name, i2c_dev_addr, buf, 6);
  
  i2c::write(dev_name, i2c_dev_addr, cmd_integration_min, sizeof(cmd_integration_min));
  i2c::read(dev_name, i2c_dev_addr, buf, 6);

  return;
}

