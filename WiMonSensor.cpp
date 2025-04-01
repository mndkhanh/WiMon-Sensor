#include "WiMonSensor.h"
#include <BlynkSimpleEsp8266.h>
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RF24 radio(RF_CE, RF_CS);

BlynkTimer blynkTimer;


// These are inits of some var to interact with Blynk Cloud
String WiMonSensor::docMess = "";
String WiMonSensor::patientID = "";
String WiMonSensor::patientName = "";
uint32_t WiMonSensor::lastGSPostRequest = 0;
uint32_t WiMonSensor::lastBlynkSendAlert = 0;
int WiMonSensor::heartRate = 0;
int WiMonSensor::spo2 = 0;
float WiMonSensor::temp_C = 0.0;
WiMonSensor::WiMonSensor() {
  channel = 0;
  deviceMAC = string_to_mac(WiFi.macAddress());
}
void WiMonSensor::init() {
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  pinMode(BTN_UP, INPUT);
  pinMode(BTN_DOWN, INPUT);
  display.setFont(&LEXEND_5);

  while (!wm_init_sensors()) {
    display.clearDisplay();

    display.setCursor(0, 30);
    display.println("Connecting MAX30102");
    display.display();
  }
  Blynk.begin(auth, ssid, pass);
  blynkTimer.setInterval(BLYNK_INTERVAL, WiMonSensor::bl_write_data);
}

void WiMonSensor::tick() {
  if (!first_read_data) {
    wm_display_init_msg();
  }  // Blynk part
  Blynk.run();
  blynkTimer.run();

  wm_tick_data();
  first_read_data = true;
  wm_display_data();
}


void WiMonSensor::wm_display_init_msg() {
  display.clearDisplay();
  display.drawBitmap(30, 10, image_data_wm_logo, 69, 30, SSD1306_WHITE);
  display.setCursor(10, 45);
  display.println("Place finger on sensor.");
  display.display();
}


void WiMonSensor::wm_display_data() {
  display.clearDisplay();
  display.drawBitmap(60, 20, image_data_wm_logo, 69, 30, SSD1306_WHITE);
  display.display();

  display.setCursor(0, 10);
  display.println(WiMonSensor::docMess);
  display.printf("%d", validHeartRate ? WiMonSensor::heartRate : 0);
  display.println(" bpm");

  display.printf("%d", validSPO2 ? spo2 : 0);
  display.println(" %");

  display.printf("%.2f", temp_C);
  display.println(" 'C");
  display.println(WiMonSensor::patientName);
  display.display();
}
bool WiMonSensor::wm_init_radio() {
  if (!radio.begin()) {
    return false;
  }
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(124);
  radio.openWritingPipe(radioAddresses[0]);
  radio.openReadingPipe(1, radioAddresses[1]);
  return true;
}

bool WiMonSensor::wm_init_sensors() {

  Serial.println("MAX30102 initialization");
  if (!particleSensor.begin()) {
    Serial.println("MAX30102 initialization");

    return false;
  }
  particleSensor.sensorConfiguration(/*ledBrightness=*/0x1F, /*sampleAverage=*/SAMPLEAVG_4,
                                     /*ledMode=*/MODE_MULTILED, /*sampleRate=*/SAMPLERATE_50,
                                     /*pulseWidth=*/PULSEWIDTH_411, /*adcRange=*/ADCRANGE_4096);
  return true;
}

void WiMonSensor::wm_tick_data() {
  particleSensor.heartrateAndOxygenSaturation(/**SPO2=*/&spo2, /**SPO2Valid=*/&validSPO2, /**heartRate=*/&WiMonSensor::heartRate, /**heartRateValid=*/&validHeartRate);
  temp_C = particleSensor.readTemperatureC();
}

// void WiMonSensor::wm_check_data() {
  // Pre calculations
  // double sumTemp = 0.0;
  // double sumSpO2 = 0.0;
  // double sumHR = 0.0;
  // double maxTemp = -DBL_MAX;
  // double maxSpO2 = -DBL_MAX;
  // double maxHR = -DBL_MAX;
  // double minTemp = DBL_MAX;
  // double minSpO2 = DBL_MAX;
  // double minHR = DBL_MAX;

  // for (int i = 0; i < 5; i++) {
  //   WiMonPacket buf;
  //   packetHistory.peekIdx(&buf, i);

  //   double hr = wm_parse_packet_hr(&buf);
  //   double temp = wm_parse_packet_temp(&buf);
  //   double spo2 = static_cast<double>(buf.spo2);
  //   boolean isCritical = false;

  //   // Sum
  //   sumHR += hr;
  //   sumTemp += temp;
  //   sumSpO2 += spo2;

  //   // Max
  //   if (hr > maxHR)
  //     maxHR = hr;
  //   if (temp > maxTemp)
  //     maxTemp = temp;
  //   if (spo2 > maxSpO2)
  //     maxSpO2 = spo2;

  //   // Min
  //   if (hr < minHR)
  //     minHR = hr;
  //   if (temp < minTemp)
  //     minTemp = temp;
  //   if (spo2 < minSpO2)
  //     minSpO2 = spo2;
  // }
  // double meanTemp = sumTemp / 5.0;
  // double meanSpO2 = sumSpO2 / 5.0;
  // double meanHR = sumHR / 5.0;

  // // Destablization flow
  // if (meanTemp > TEMP_MAX || meanTemp < TEMP_MIN) {
  //   tempStatus = ValueStatus::CRITICAL;
  //   isCritical = true;
  // }
  // if (meanSpO2 < SPO2_MIN) {
  //   spo2Status = ValueStatus::CRITICAL;
  //   isCritical = true;
  // }
  // if (meanHR > HR_MAX || meanHR < HR_MIN) {
  //   hrStatus = ValueStatus::CRITICAL;
  //   isCritical = true;
  // }
  // if (maxHR - minHR >= HR_MAX_DEV) {
  //   hrStatus = ValueStatus::CRITICAL;
  //   isCritical = true;
  // }
  // if (maxSpO2 - minSpO2 >= SPO2_MAX_DEV) {
  //   spo2Status = ValueStatus::CRITICAL;
  //   isCritical = true;
  // }
  // if (maxTemp - minTemp >= TEMP_MAX_DEV) {
  //   tempStatus = ValueStatus::CRITICAL;
  //   isCritical = true;
  // }

  // // Stablization flow
  // // Stabilization flow
  // if (meanTemp <= TEMP_MAX && meanTemp >= TEMP_MIN && (maxTemp - minTemp) < TEMP_MAX_DEV) {
  //   tempStatus = ValueStatus::STABLE;
  // }
  // if (meanSpO2 >= SPO2_MIN && (maxSpO2 - minSpO2) < SPO2_MAX_DEV) {
  //   spo2Status = ValueStatus::STABLE;
  // }
  // if (meanHR <= HR_MAX && meanHR >= HR_MIN && (maxHR - minHR) < HR_MAX_DEV) {
  //   hrStatus = ValueStatus::STABLE;
  // }

  // if (isCritical == true) {  // send alert if there is critical status of vitals
  //   bl_send_alert();
  // }
// }

/*
  Blynk part
*/
void WiMonSensor::bl_write_data() {
  Blynk.virtualWrite(SPO2_VP, WiMonSensor::spo2);
  Blynk.virtualWrite(HR_VP, ((double)WiMonSensor::heartRate));
  Blynk.virtualWrite(TEMP_VP, ((double)WiMonSensor::temp_C));
  WiMonSensor::gs_post_vitals_record();
}

void WiMonSensor::bl_send_alert() {
  if (Blynk.connected()) {                                       // check WiFi connection
    if (millis() - lastBlynkSendAlert >= SEND_ALERT_INTERVAL) {  // Check interval
      String message = "Check patient " + WiMonSensor::patientName;
      Blynk.logEvent("vitals_alert", message);
      lastBlynkSendAlert = millis();
    }
  }
}

BLYNK_WRITE(PATIENT_ID_VP) {
  WiMonSensor::patientID = param.asStr();
  WiMonSensor::gs_fetch_patient_name();
  Serial.println(WiMonSensor::patientName);
}

BLYNK_WRITE(DOC_MESS_VP) {
  WiMonSensor::docMess = param.asStr();
}

/*
  Google Sheet & Apps Script
*/
void WiMonSensor::gs_fetch_patient_name() {
  if (Blynk.connected()) {  // check WiFi connection
    HTTPClient http;
    http.begin(_blynkWifiClient, ENDPOINT_API_URL + String("?patientID=") + WiMonSensor::patientID);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.GET();
    WiMonSensor::patientName = http.getString();
    http.end();
  }
}

void WiMonSensor::gs_post_vitals_record() {
  if (Blynk.connected()) {  // check WiFi connection
    HTTPClient http;

    http.begin(_blynkWifiClient, String(ENDPOINT_API_URL));
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"patientID\":\"" + WiMonSensor::patientID + "\", \"spo2\": " + String(WiMonSensor::spo2) + ", \"hr\": " + String((double)WiMonSensor::heartRate / 100) + ", \"temp\": " + String((double)WiMonSensor::temp_C / 1000) + "}";

    http.POST(jsonData);
    http.end();
  }
}
