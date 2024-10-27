#pragma once

#include <Arduino.h>
#include "defines.h"
#include <time.h>
#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>

extern LoginPass lp;
extern RTC_DS3231 rtc;
extern time_t ntp_now;                         
extern tm ntp_tm;
extern ApplicationConfig appConfig;

#define MY_TZ "UTC-2UTC-3,M3.5.0/3,M10.5.0/4"


class TimeHelper {
    public:
        void getFromRTC() 
        {
            
        }

        void tick() 
        {

        }

        void printRTC() 
        {
            DateTime now = rtc.now();

            Serial.print(now.year(), DEC);
            Serial.print('/');
            Serial.print(now.month(), DEC);
            Serial.print('/');
            Serial.print(now.day(), DEC);
            Serial.print(" ");
            Serial.print(now.hour(), DEC);
            Serial.print(':');
            Serial.print(now.minute(), DEC);
            Serial.print(':');
            Serial.print(now.second(), DEC);
            Serial.println();
            Serial.print("Temperature: ");
            Serial.print(rtc.getTemperature());
            Serial.println(" C");

            Serial.println();
        }

        void refreshNTP()
        {
            unsigned long start = micros();
            configTime(MY_TZ, appConfig.ntp_server);
            while (time(nullptr) < 100000) {
                // Serial.println("Ожидание первого обновления времени...");
                // delay(1000);
            }
            unsigned long end = micros();
            Serial.print("\n\nRefresh time NTP: ");
            Serial.println(end - start);
        }

};