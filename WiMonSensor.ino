#include "WiMonSensor.h"
WiMonSensor sensor;
void setup() {
  Serial.begin(9600);
  sensor.init();
}

void loop() {
  sensor.tick();
}
