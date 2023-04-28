#ifndef __GW5300_HPP_
#define __GW5300_HPP_
#include <vector>
#include <stdint.h>

uint8_t calcCheckSum(const std::vector<uint8_t> data){
  uint8_t result = 0;
  for(uint8_t d : data){
    result += d;
  }

  return result;
}

uint8_t calcCheckSum(const uint8_t *data, size_t size){
  uint8_t result = 0;
  
  for(uint8_t i=0; i<size; i++){
    result += data[i];
  }

  return result;
}

#endif
