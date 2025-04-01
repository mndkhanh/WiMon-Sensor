#define RF_CE D0
#define RF_CS D8

#define BTN_UP D4
#define BTN_DOWN D3


#define ACK_TIMEOUT 500
#define DEBOUNCE_MS 500


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

/*
  Blynk IOT Configuration
*/
#define BLYNK_TEMPLATE_ID "TMPL6Btwco48K"
#define BLYNK_TEMPLATE_NAME "WiMonTerminal"
#define BLYNK_AUTH_TOKEN "lQji4g4yIqq40X0TwiLramJ9MFUotz6n"
#define WIFI_SSID "iPhone Khanh"
#define WIFI_PASSWORD "01020403"
// define blynk virtual pins to store data to cloud
#define SPO2_VP V0
#define TEMP_VP V1
#define HR_VP V2
// define blynk virtual pins to fetch data from cloud
#define PATIENT_ID_VP V4
#define DOC_MESS_VP V5
// 
#define BLYNK_INTERVAL 1000L


/**
 * Google Sheet record and fetch data

 */
#define ENDPOINT_API_URL "https://script.google.com/macros/s/AKfycbyGQAZTcIPBOSug7cVjGCTX5DnZDrwayj4kz3DdaXocrhKcrd3e1BLj72qD4EbJK1EL/exec"
#define POST_INTERVAL 10000
#define SEND_ALERT_INTERVAL 30000