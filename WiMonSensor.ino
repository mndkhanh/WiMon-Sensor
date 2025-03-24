#include "WiMonSensor.h"
WiMonSensor sensor;
void setup() {
  sensor.init();
}

void loop() {
  sensor.tick();
}
