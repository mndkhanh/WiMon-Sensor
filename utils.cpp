#include "utils.h"
// cast spo2 into format required by terminal in struct (ex: 8600 for 86.00%)
uint8_t cast_spo(int32_t spo)
{
  return (uint8_t)(spo * 100);
}
// cast heart rate into format required by terminal in struct (ex: 602 for 60.2 bpm)
uint16_t cast_hr(int32_t hr)
{
  return (uint16_t)(hr * 100);
}

uint64_t string_to_mac(String s)
{
  unsigned char a[6];
  int last = -1;
  int rc = sscanf(s.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx%n",
                  a + 0, a + 1, a + 2, a + 3, a + 4, a + 5,
                  &last);
  if (rc != 6 || s.length() != last)
    return 0;
  return uint64_t(a[0]) << 40 | uint64_t(a[1]) << 32 | (
                                                           // 32-bit instructions take fewer bytes on x86, so use them as much as possible.
                                                           uint32_t(a[2]) << 24 | uint32_t(a[3]) << 16 | uint32_t(a[4]) << 8 | uint32_t(a[5]));
}
// cast temperature (C) in to format required by terminal in struct (ex: 37000 for 37.000 C)
uint16_t cast_temp(float temp)
{
  return (uint16_t)(temp * 1000);
}
