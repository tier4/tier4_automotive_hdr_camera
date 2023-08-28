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

std::vector<std::string> split(const std::string &str, char delim) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

int c2_process(cmdline::parser &p)
{
  int ret = 0;
  std::string file_name = p.get<std::string>("input");
  int port_num = p.get<int>("port-num");

  if (p.exist("vv"))
  {
    std::cerr << "/************** Enabling debug message **********/" << std::endl;
    debug_flag = true;
  }

  C2 c2_a = C2(port_num, debug_flag);

  if (p.exist("hue"))
  {
    int val = p.get<int>("hue");
    std::cout << "set Hue val:" << val << std::endl;
    ret += c2_a.setHue(true);
    ret += c2_a.setHueVal((int)val);
  }
  if (p.exist("saturation"))
  {
    float val = p.get<float>("saturation");
    std::cout << "set Saturation val:" << val << std::endl;
    ret += c2_a.setSaturation(false);
    usleep(5000);
    ret += c2_a.setSaturationVal((int)val);
    usleep(5000);
    ret += c2_a.setSaturation(true);
  }
  if (p.exist("brightness"))
  {
    float val = p.get<float>("brightness");
    std::cout << "set Brightness val:" << val << std::endl;
    ret += c2_a.setBrightness(true);
    ret += c2_a.setBrightnessVal((int)val);
  }
  if (p.exist("contrast"))
  {
    float val = p.get<float>("contrast");
    std::cout << "set Contrast val:" << val << std::endl;
    ret += c2_a.setContrast(true);
    ret += c2_a.setContrastVal((int)val);
  }
  if (p.exist("sharpness"))
  {
    std::string val = p.get<std::string>("sharpness");
    std::cout << "set Sharpness val:" << val << std::endl;
    int ld = 0;
    int ldu = 0;
    int lu = 0;

    std::vector<std::string> vals = split(val, ':');

    if(vals.size() >= 3){

	    try{
		    ld = std::stoi(vals.at(0));
		    ldu = std::stoi(vals.at(1));
		    lu = std::stoi(vals.at(2));
	    }catch(...){

	    }    
    }    

    ret += c2_a.setSharpness(false);
    ret += c2_a.setSharpnessVal_Ld((int)ld);
    ret += c2_a.setSharpnessVal_Ldu((int)ldu);
    ret += c2_a.setSharpnessVal_Lu((int)lu);
    ret += c2_a.setSharpness(true);
  }

  if (p.exist("sensormode"))
  {
    int val = p.get<int>("sensor_mode");
    std::cout << "set Sensor mode:" << val << std::endl;
    c2_a.setSensorMode(val);
  }

  if (p.exist("auto_exposure"))
  {
    bool val = p.get<bool>("auto_exposure");
    std::cout << "set Autoexposure:" << val << std::endl;
    c2_a.setAutoExposure(val);
  }


  if (p.exist("isp_sensor_gain"))
  {
    float val = p.get<float>("isp_sensor_gain");
    std::cout << "set ISP Sensor gain val:" << val << std::endl;
    ret += c2_a.setISPSensorGain(val);
  }
  if (p.exist("sensor_gain"))
  {
    float val = p.get<float>("sensor_gain");
    std::cout << "set Sensor gain val:" << val << std::endl;
    ret += c2_a.setSensorGain(val);
  }
  if (p.exist("awb"))
  {
    bool val = p.get<bool>("awb");
    std::cout << "set Auto white balance" << val << std::endl;
    ret += c2_a.setAutoWhiteBalance(val);
  }
  if (p.exist("awb_gain_g"))
  {
    float val = p.get<float>("awb_gain_g");
    std::cout << "set Auto white balance g val:" << val << std::endl;
    ret += c2_a.setAutoWhiteBalanceGainG((int)val);
  }
  if (p.exist("awb_gain_r"))
  {
    float val = p.get<float>("awb_gain_r");
    std::cout << "set Auto white balance r  val:" << val << std::endl;
    ret += c2_a.setAutoWhiteBalanceGainR((int)val);
  }
  if (p.exist("awb_gain_b"))
  {
    float val = p.get<float>("awb_gain_b");
    std::cout << "set Auto white balance B  val:" << val << std::endl;
    ret += c2_a.setAutoWhiteBalanceGainB((int)val);
  }

  if (p.exist("shutter_on_ae")){
    int val = p.get<int>("shutter_on_ae");	
    ret += c2_a.setShutterTimeOnAE((uint16_t)val);
  }

  if (p.exist("evrefoffset"))
  {
    std::cerr << "evref offset is not supported" << std::endl;
  }
  if (p.exist("digitalgain"))
  {
    std::cerr << "digitalgain is not supported, please use sensorgain" << std::endl;
  }
  if (p.exist("tempature"))
  {
    std::cerr << "tempature is not supported" << std::endl;
  }
  if (p.exist("atr_contrast_gain"))
  {
    std::cerr << "atr is not supported" << std::endl;
  }
  if (p.exist("atr_brightness_gain"))
  {
    std::cerr << "atr is not supported" << std::endl;
  }

  if (ret < 0)
  {
    std::cerr << "Failed set parameter. please check parameter and hardware connection" << std::endl;
  }

  return 0;
}
