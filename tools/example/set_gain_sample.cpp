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

#include <t4cam_tools.hpp>

int main(int argc, char* argv[])
{
  float db = 30;

  if (argc >= 2)
  {
    db = atof(argv[1]);
  }

  C1 c1_a = C1(PORT_A_CAM);
  c1_a.setSharpness(db);

  float gain = db;
  float offset = db;
  int deg = db;
  float r_gain = db;
  float gr_gain = db;
  float gb_gain = db;
  float b_gain = db;
  bool on = (db > 0.1);

  c1_a.setHue(deg);
  c1_a.setSaturation(gain);
  c1_a.setBrightness(offset);
  c1_a.setContrast(gain);

  c1_a.setAutoWhiteBalance(on);
  c1_a.setWhiteBalanceGain(r_gain, gr_gain, gb_gain, b_gain);
  c1_a.setExposureOffset(offset);
}
