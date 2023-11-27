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

extern int c1_process(cmdline::parser &p);
extern int c2_process(cmdline::parser &p);

int main(int argc, char *argv[])
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
  p.add<std::string>("sharpness", 's', "set sharpness val", false);

  p.add<int>("atr_contrast_gain", 0, "set atr contrast gain", false, 0, cmdline::range(0, 0x100));
  p.add<int>("atr_brightness_gain", 0, "set atr brightness gain", false, 0, cmdline::range(0, 0x100));
  p.add<int>("sensormode", 0, "set sensor_mode val", false);

  p.add<float>("isp_sensor_gain", 0, "set isp_sensor_gain val", false);
  p.add<float>("sensor_gain", 0, "set sensor_gain val", false);
  p.add<bool>("awb", 'a', "set awb enable", false);
  p.add<float>("awb_gain_g", 0, "set awb gain g", false);
  p.add<float>("awb_gain_r", 0, "set awb gain r", false);
  p.add<float>("awb_gain_b", 0, "set awb gain b", false);

 
  p.add<bool>("auto_exposure", 'A', "set auto exposure", false);
  p.add<int>("shutter_on_ae", 0,"set shutter time on Ae", false);
  p.add<float>("shutter_fme", 0,"set shutter time on ME", false);


  p.add("vv", 'v', "get debug_message");

  p.add<float>("evrefoffset", 'e', "set EvrefOffset val", false);
  p.add<float>("digital_gain", 'D', "set DigitalGain val", false);
  p.add<float>("analog_gain", 0, "set AnalogGain val", false);

  p.parse_check(argc, argv);

  int ret;
  if (p.get<std::string>("model") != "c2")
  {
    ret = c1_process(p);
  }
  else
  {
    ret = c2_process(p);
  }
  return ret;
}
