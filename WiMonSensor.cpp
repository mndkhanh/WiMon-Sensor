#include "WiMonSensor.h"
// cast temperature (C) in to format required by terminal in struct (ex: 37000 for 37.000 C)
uint16_t cast_temp(float temp) {
  return (uint16_t)(temp * 1000);
}

RF24 radio(RF_CE, RF_CS);
WiMonSensor::WiMonSensor() {
  channel = 0;
}
void WiMonSensor::init() {
  wm_init_sensors();
  wm_init_radio();
}

void WiMonSensor::tick() {
  if (isAckEnabled) {
    wm_ack_terminal();
  }
  wm_tick_btns();
  wm_tick_data();
  wm_send_data();
}

void WiMonSensor::wm_ack_terminal() {
  radio.startListening();
  lastStartAckTime = millis();
  while (!radio.available()) {
    if (millis() - lastStartAckTime >= ACK_TIMEOUT)
      return;
  }
  radio.stopListening();
  radio.write(&deviceMAC, sizeof(deviceMAC));
}

void WiMonSensor::wm_send_data() {
  // Stop listening so we can talk
  radio.stopListening();
  WiMonPacket payload;
  payload.MAC = deviceMAC;
  payload.spo2 = spo2;
  payload.hr = heartRate;
  payload.temp_C = cast_temp(particleSensor.readTemperatureC());
  radio.write(&payload, sizeof(WiMonPacket));
}

void WiMonSensor::wm_init_radio() {
  if (!radio.begin()) {
    Serial.println("Radio fucked");
  }
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(100);
  radio.openWritingPipe(radioAddresses[0]);
  radio.openReadingPipe(1, radioAddresses[1]);
}

void WiMonSensor::wm_init_sensors() {

  while (!particleSensor.begin()) {
    Serial.println("MAX30102 was not found");
    delay(1000);
  }
  particleSensor.sensorConfiguration(/*ledBrightness=*/0x1F, /*sampleAverage=*/SAMPLEAVG_4,
                                     /*ledMode=*/MODE_MULTILED, /*sampleRate=*/SAMPLERATE_400,
                                     /*pulseWidth=*/PULSEWIDTH_411, /*adcRange=*/ADCRANGE_4096);
}

void WiMonSensor::wm_tick_data() {
  particleSensor.heartrateAndOxygenSaturation(/**SPO2=*/&spo2, /**SPO2Valid=*/&validSPO2, /**heartRate=*/&heartRate, /**heartRateValid=*/&validHeartRate);
}

void WiMonSensor::wm_tick_btns() {
  bool isUpBtnCurrentlyPressed = digitalRead(BTN_DOWN) == HIGH;

  if (isUpBtnPressed != isUpBtnCurrentlyPressed) {
    if (isUpBtnCurrentlyPressed) {
      upBtnLastPressedTime = millis();
      wm_on_up_btn_press();
    }
  } else if (isUpBtnPressed && millis() - upBtnLastPressedTime >= DEBOUNCE_MS) {
    wm_on_up_btn_press();
    upBtnLastPressedTime = millis();
  }
  isUpBtnPressed = isUpBtnCurrentlyPressed;

  bool isDownBtnCurrentlyPressed = digitalRead(BTN_DOWN) == HIGH;

  if (isDownBtnPressed != isDownBtnCurrentlyPressed) {
    if (isDownBtnCurrentlyPressed) {
      downBtnLastPressedTime = millis();
      wm_on_down_btn_press();
    }
  } else if (isDownBtnPressed && millis() - downBtnLastPressedTime >= DEBOUNCE_MS) {
    wm_on_down_btn_press();
    downBtnLastPressedTime = millis();
  }
  isDownBtnPressed = isDownBtnCurrentlyPressed;
}

void WiMonSensor::wm_on_up_btn_press() {
  radio.setChannel(++channel);
}

void WiMonSensor::wm_on_down_btn_press() {
  radio.setChannel(--channel);
}


// cast spo2 into format required by terminal in struct (ex: 8600 for 86.00%)
uint8_t cast_spo(int32_t spo) {
  return (uint8_t)(spo * 100);
}
// cast heart rate into format required by terminal in struct (ex: 602 for 60.2 bpm)
uint16_t cast_hr(int32_t hr) {
  return (uint16_t)(hr * 100);
}
