#include <stdint.h>
#include <RF24.h>
#include <Wire.h>
#include <DFRobot_MAX30102.h>
#include <ESP8266WiFi.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>  // legacy: #include "SSD1306.h"
#include "lexend_10.h"
#include "lexend_12.h"
#include "lexend_14.h"
#include "lexend_5.h"
#include "wm_logo.h"
#include "utils.h"
#include "config.h"
#include "structs.h"
#include <ESP8266HTTPClient.h>


class WiMonSensor {

private:
  bool isAckEnabled = true;
  bool isUpBtnPressed;
  uint32_t upBtnLastPressedTime;
  bool isDownBtnPressed;
  uint32_t downBtnLastPressedTime;
  // blynk config
  const char* auth = BLYNK_AUTH_TOKEN;
  const char* ssid = WIFI_SSID;
  const char* pass = WIFI_PASSWORD;

  DFRobot_MAX30102 particleSensor;
  uint32_t lastReportTime;
  int8_t validSPO2;       // indicator to show if the SPO2 calculation is valid
  int8_t validHeartRate;  // indicator to show if the heart rate calculation is valid
  bool first_read_data;

  uint8_t radioAddresses[2][6] = { "1Node", "2Node" };
  uint32_t lastStartAckTime;
  uint64_t deviceMAC;
  uint8_t channel;
  void wm_send_data();
  bool wm_init_radio();
  bool wm_init_sensors();
  void wm_tick_data();
  void wm_display_data();
  void wm_display_init_msg();
  // void wm_check_data(); // should write this and use bl_send_alert()

public:
  WiMonSensor();
  void init();
  void tick();

  static uint32_t lastGSPostRequest;
  static uint32_t lastBlynkSendAlert;
  static void gs_fetch_patient_name();  // fetch data of patient name from google sheet by his/her ID
  static void gs_post_vitals_record();

  // those below used to set value for docMess and patientID + record vitals + write patient's vitals to cloud (should be public static)
  static String docMess;
  static String patientID;
  static String patientName;
  static int32_t spo2;
  static int32_t heartRate;
  static float temp_C;
  static void bl_write_data();
  void bl_send_alert();

};
