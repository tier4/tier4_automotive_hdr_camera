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


#include <i2c.hpp>

int main(void){
  std::string dev_name = "/dev/i2c-30";

	uint8_t data=0;
	i2c::read16(dev_name, 0x1a, 0xabc0, &data);
	printf("data:%02X\n", data);
	i2c::read16(dev_name, 0x1a, 0xAC4C, &data);
	printf("data:%02X\n", data);
	i2c::read16(dev_name, 0x1a, 0xAC40, &data);
	printf("data:%02X\n", data);
	i2c::read16(dev_name, 0x1a, 0xAC41, &data);
	printf("data:%02X\n", data);
	i2c::read16(dev_name, 0x1a, 0xAC5A, &data);
	printf("data:%02X\n", data);
	i2c::read16(dev_name, 0x1a, 0xAC26, &data);
	printf("data:%02X\n", data);
	i2c::read16(dev_name, 0x1a, 0xAE46, &data);
	printf("data:%02X\n", data);
	i2c::read16(dev_name, 0x1a, 0xAE94, &data);
	printf("data:%02X\n", data);

}
