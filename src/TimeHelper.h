#pragma once

#include <Arduino.h>
#include "defines.h"
#include <time.h>
#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
#include <NTPClient.h>
#include <Timezone.h>
#include <TimeLib.h>

extern LoginPass lp;
extern RTC_DS3231 rtc;
extern time_t ntp_now;                         
extern tm ntp_tm;
extern ApplicationConfig appConfig;
extern NTPClient timeClient;

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

        void setupNTPClient()
        {
            timeClient.end();
            timeClient.setUpdateInterval((uint64)(appConfig.ntp_refresh_period * 1000));
            timeClient.setPoolServerName(appConfig.ntp_server);
            timeClient.setTimeOffset(0);
            timeClient.begin();
        }

        void refreshNTP()
        {
            if (timeClient.update()) {
                unsigned long start = micros();

                // Получаем текущее время в UTC
                time_t utcTime = timeClient.getEpochTime();

                // Конвертируем UTC в локальное время с учетом часового пояса
                time_t localTime = getTimeZone().toLocal(utcTime);

                // Устанавливаем локальное время в систему
                setTime(localTime);

                Serial.print("\nВремя успешно обновлено! Current local: ");
                Serial.println(localTime);

                unsigned long end = micros();
                Serial.print("Refresh time NTP: ");
                Serial.println(end - start);
                //
                DateTime dt = DateTime(localTime);
                rtc.adjust(dt);
            }
            
        }

        
    protected:
        Timezone getTimeZone()
        {
            if (appConfig.tz_dst) {
                TimeChangeRule stdTime = {"", Last, Sun, Oct, 4, appConfig.tz_offset_value * 60};
                strncpy(stdTime.abbrev, getTZName(appConfig.tz_offset_value), sizeof(stdTime.abbrev));
                TimeChangeRule dstTime = {"", Last, Sun, Mar, 3, (appConfig.tz_offset_value + 1) * 60};
                strncpy(dstTime.abbrev, getTZName(appConfig.tz_offset_value + 1), sizeof(dstTime.abbrev));
                // Serial.println(stdTime.abbrev);
                // Serial.println(dstTime.abbrev);
                return Timezone(dstTime, stdTime);
            } else {
                TimeChangeRule stdTime = {"", Last, Sun, Oct, 4, appConfig.tz_offset_value * 60};
                strncpy(stdTime.abbrev, getTZName(appConfig.tz_offset_value), sizeof(stdTime.abbrev));
                // Serial.println(stdTime.abbrev);
                return Timezone(stdTime);
            }
        }

        const char* getTZName(int8 offset) 
        {
            static char zonename[6];
            if (offset == 0) {
                snprintf(zonename, sizeof(zonename), "UTC");
            } else {
                snprintf(zonename, sizeof(zonename), "UTC%c%u", (offset >= 0) ? '+' : '-', abs(offset) % 100);
            }
            return zonename;
        }
};
