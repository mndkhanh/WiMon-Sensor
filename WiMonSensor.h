#include <stdint.h>
#include <RF24.h>
#include <Wire.h>
#include <DFRobot_MAX30102.h>

#include "config.h"
#include "structs.h"

class WiMonSensor
{

private:
    bool isAckEnabled = true;
    bool isUpBtnPressed;
    uint32_t upBtnLastPressedTime;
    bool isDownBtnPressed;
    uint32_t downBtnLastPressedTime;

    DFRobot_MAX30102 particleSensor;
    uint32_t lastReportTime;
    int32_t spo2;          // SPO2 value
    int8_t validSPO2;      // indicator to show if the SPO2 calculation is valid
    int32_t heartRate;     // heart rate value
    int8_t validHeartRate; // indicator to show if the heart rate calculation is valid

    uint8_t radioAddresses[2][6] = {"SENSR", "TERMI"};
    uint32_t lastStartAckTime;
    uint64_t deviceMAC;
    uint8_t channel;
    void wm_ack_terminal();
    void wm_send_data();
    void wm_init_radio();
    void wm_init_sensors();
    void wm_tick_data();
    void wm_tick_btns();
    void wm_on_up_btn_press();
    void wm_on_down_btn_press();

public:
    WiMonSensor();
    void init();
    void tick();
};
