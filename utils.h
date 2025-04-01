#include <stdint.h>
#include <Arduino.h>

uint8_t cast_spo(int32_t spo);
uint16_t cast_hr(int32_t hr);
uint64_t string_to_mac(String s);
uint16_t cast_temp(float temp);