#pragma once

#define VERSION "0.0.1"
#include <Arduino.h>
#include <Wire.h>
#include "I2C_eeprom.h"

extern I2C_eeprom ee;

#define EEPROM_VERSION 1
#define RECONNECT_INTERVAL 30000
#define READ_SENSORS_INTERVAL 5000

const uint8_t eepromVersion = EEPROM_VERSION;

// EEpromAddresses
#define EEA_VERSION         0
#define EEA_LP              (EEA_VERSION + sizeof(eepromVersion))
#define EEA_AC              (EEA_LP + sizeof(LoginPass))
#define EEA_CS              (EEA_AC + sizeof(ApplicationConfig))

// Addresses
#define EEPROM_ADDRESS 0x57
#define EEPROM_SIZE I2C_DEVICESIZE_24LC32
#define EEPROM_VERSION 1

// Pins
#define PIN_WIFI_RESET      12
#define PIN_DISPLAY         14


// Types
struct LoginPass {
  char ssid[20] = "";
  char pass[20] = "";
};

struct ApplicationConfig {
    char ntp_server[150] = "time.google.com";
    int8 tz_offset_value = 2;
    uint8 tz_offset_idx = 14;
    bool tz_dst = true;
    uint32 ntp_refresh_period = 1800;
};

struct CurrentState {
};

struct SensorData
{
    float pressure_mm;
    float pressure_pa;
    float temperature_press;
    float temperature;
    float humidity;
    float dew_point_temperature;
    float illumination;
};





void setupPins()
{
    pinMode(PIN_WIFI_RESET, INPUT_PULLUP);
    pinMode(PIN_DISPLAY, OUTPUT);
}

void initEEPROM()
{
    ee.begin();
    if (!ee.isConnected())
    {
        Serial.println("ERROR: Can't find eeprom (stopped)...");
    } else {
        Serial.println("eeprom initialized");
    }
}


String tz_str = "-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12";
int8 tz_arr[25] = {-12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};