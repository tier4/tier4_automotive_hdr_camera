// Copyright 2022 Tier IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <unistd.h>

#include "cmdline.h"

//#define DEBUG
#include <t4cam_tools.hpp>

#define AE_MODE_AE 0x0
#define AE_MODE_FME 0x3

int c1_process(cmdline::parser &p)
{
  if (p.exist("vv"))
  {
    std::cerr << "/************** t4cam-ctrl  **********/" << std::endl;
    std::cerr << "/************** Enabling debug message **********/" << std::endl;
    debug_flag = true;
  }

  std::string file_name = p.get<std::string>("input");
  int port_num = p.get<int>("port-num");
  C1 c1_a = C1(port_num);

  int ret = 0;
  if (p.exist("input"))
  {
    if (c1_a.initialized_load_value_from_file(p.get<std::string>("input")) < 0)
    {
      return -1;
    }
    c1_a.setLoadValue();
    return 0;
  }
  else if (p.exist("output"))
  {
    c1_a.saveCurrentValue(p.get<std::string>("output"));
  }
  else
  {
    if (p.exist("hue"))
    {
      int val = p.get<int>("hue");
      std::cout << "set hue val:" << val << std::endl;
      ret = c1_a.setHue(val);
    }
    if (p.exist("saturation"))
    {
      float val = p.get<float>("saturation");
      std::cout << "set Saturation val:" << val << std::endl;
      ret = c1_a.setSaturation(val);
    }
    if (p.exist("contrast"))
    {
      float val = p.get<float>("contrast");
      std::cout << "set Contrast val:" << val << std::endl;
      ret = c1_a.setContrast(val);
    }
    if (p.exist("brightness"))
    {
      float val = p.get<float>("brightness");
      std::cout << "set Brightness val:" << val << std::endl;
      ret = c1_a.setBrightness(val);
    }
    if (p.exist("tempature"))
    {
      float val = c1_a.getTempature(0);
      std::cout << "get tempature:" << val << std::endl;
    }
    if (p.exist("atr_contrast_gain"))
    {
      int val = p.get<int>("atr_contrast_gain");
      c1_a.setATRContrastGain(val);
      std::cout << "setATR Contrast gain" << val << std::endl;
    }    
    if (p.exist("atr_brightness_gain"))
    {
      int val = p.get<int>("atr_brightness_gain");
      c1_a.setATRBrightnessGain(val);
      std::cout << "setATR Brightness gain" << val << std::endl;
    }
    if (p.exist("auto_exposure"))
    {
      bool val = p.get<bool>("auto_exposure");
      if(val)
      {
        c1_a.setAEMode(AE_MODE_AE);
        std::cout << "Enabled auto exposure: " << val << std::endl;
      } else
      {
        c1_a.setAEMode(AE_MODE_FME);
        std::cout << "Disabled auto exposure: " << val << std::endl;
      }
    }
    if (p.exist("shutter_fme"))
    {
      float val = p.get<float>("shutter_fme");
      c1_a.setShutterSpeedforFME(val);
      std::cout << "set exposure time " << val << " msec" << std::endl;
    }
    if (p.exist("analog_gain"))
    {
      float val = p.get<float>("analog_gain");
      c1_a.setAnalogGain(val);
      std::cout << "set analog gain " << val << " db" << std::endl;
    }

    if (p.exist("digital_gain"))
    {
      float val = p.get<float>("digital_gain");
      c1_a.setDigitalGain(val);
      std::cout << "set digital gain " << val << " db" << std::endl;
    }
  
  }

  if (ret < 0)
  {
    std::cerr << "Failed set parameter. please check parameter and hardware connection" << std::endl;
  }
  return 0;
}
