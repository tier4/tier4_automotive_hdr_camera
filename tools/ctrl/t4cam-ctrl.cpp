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

#define DEBUG
#include <t4cam_tools.hpp>


int main(int argc, char* argv[])
{
  cmdline::parser p;

  //config
  p.add<int>("port-num",'p',"set port number [0-7]", false, 0, cmdline::range(0,7));
  p.add<std::string>("config-file",'f',"set config file name", false);
  
  // get
  p.add("tempature", 't', "get Tempature val");
  
  //set
  p.add<float>("brightness", 'B', "set Brightness val", false);
  p.add<float>("contrast", 'C', "set Contrast val", false);
  p.add<int>("hue", 'H', "set Hue val", false);
  p.add<float>("saturation", 'S', "set Saturation val", false);
#if 0
  p.add<float>("evrefoffset", 'e', "set EvrefOffset val", false);
  p.add<float>("digitalgain", 'D', "set DigitalGain val", false);
#endif
  p.parse_check(argc, argv);

  int port_num = p.get<int>("port-num");

  C1 c1_a = C1(port_num);

  int ret = 0;

  if (p.exist("config-file"))
  {
    return 0;
  }else{
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
  }

  if (ret<0){
    std::cerr << "Failed set parameter. please check parameter and hardware connection"<< std::endl;
  }


#if 0
  if (p.exist("evrefoffset"))
  {
    float val = p.get<float>("evrefoffset");
    std::cout << "set evrefoffset val:" << val << std::endl;
    c1_a.setExposureOffset(val);
  }
  if (p.exist("digitalgain"))
  {
    float val = p.get<float>("digitalgain");
    std::cout << "set DigitalGain val:" << val << std::endl;
    c1_a.setDigitalGain(val);
  }
#endif
  return 0;
}
