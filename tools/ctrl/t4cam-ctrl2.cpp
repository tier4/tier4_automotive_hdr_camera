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

  // config
  p.add<int>("port-num", 'p', "set port number [0-7]", false, 0, cmdline::range(0, 7));
  p.add<std::string>("input", 'i', "Sets the name of the file to read parameter values", false, "./default.yaml");
  p.add<std::string>("output", 'o', "Sets the name of the file to save the current parameter values.", false);
  p.add<std::string>("model", 'm', "set model", false, "c1");

  // get
  p.add("tempature", 't', "get Tempature val");

  // set
  p.add<float>("brightness", 'B', "set Brightness val", false);
  p.add<float>("contrast", 'C', "set Contrast val", false);
  p.add<int>("hue", 'H', "set Hue val", false);
  p.add<float>("saturation", 'S', "set Saturation val", false);
  p.add<int>("shutter", 's', "set shutter time for AE", false);
  p.add("vv", 'v', "get debug_message");
#if 0
  p.add<float>("evrefoffset", 'e', "set EvrefOffset val", false);
  p.add<float>("digitalgain", 'D', "set DigitalGain val", false);
#endif


  p.parse_check(argc, argv);

  std::string file_name = p.get<std::string>("input");
  int port_num = p.get<int>("port-num");


  if(p.exist("vv")){
	  std::cerr << "/************** Enabling debug message **********/" <<std::endl;
	  debug_flag=true;
  }
  C2 c2_a = C2(port_num, debug_flag);

  if (p.exist("shutter"))
  {
	  int val = p.get<int>("shutter");
	  c2_a.setShutterTimeOnAE(val, val);
  }
  c2_a.setSensorGain(900.0);

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
