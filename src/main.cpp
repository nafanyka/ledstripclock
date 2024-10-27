#include <Arduino.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <I2C_eeprom.h>
#include <GyverPortal.h>
#include "WiFiPortal.h"
#include "MainPortal.h"
#include "EepromSettings.h"
#include <time.h>
#include <RTClib.h>
#include <SPI.h>
#include "defines.h"
#include <GyverTimer.h>
// #include <WiFi.h>
// #include <WiFiUdp.h>
#include <NTPClient.h>
// #include <TimeLib.h>
// #include <Timezone.h>
#include "TimeHelper.h"


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

LoginPass lp;
ApplicationConfig appConfig;
CurrentState currentState;
//
GTimer reconnectTimer(MS);
GTimer checkConnectTimer(MS);
GTimer tmp(MS, 1000);
//
RTC_DS3231 rtc;
TimeHelper timeHelper;
//

GyverPortal ui;
I2C_eeprom ee(EEPROM_ADDRESS, EEPROM_SIZE);

time_t ntp_now;
tm ntp_tm;


void connectWiFi() {
    uint8 attempts = 0;
    // пытаемся подключиться
    Serial.print("\nConnect to: ");
    Serial.println(lp.ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(lp.ssid, lp.pass);
    while (WiFi.status() != WL_CONNECTED && attempts < 40) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.print("Connected! Local IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi notConnected!");
        reconnectTimer.setMode(false);
        reconnectTimer.start();
        reconnectTimer.setInterval(RECONNECT_INTERVAL);
    }
}

// void showTime() {
//   time(&ntp_now);                       // read the current time
//   localtime_r(&ntp_now, &ntp_tm);           // update the structure tm with the current time
//   Serial.print("year:");
//   Serial.print(ntp_tm.tm_year + 1900);  // years since 1900
//   Serial.print("\tmonth:");
//   Serial.print(ntp_tm.tm_mon + 1);      // January = 0 (!)
//   Serial.print("\tday:");
//   Serial.print(ntp_tm.tm_mday);         // day of month
//   Serial.print("\thour:");
//   Serial.print(ntp_tm.tm_hour);         // hours since midnight  0-23
//   Serial.print("\tmin:");
//   Serial.print(ntp_tm.tm_min);          // minutes after the hour  0-59
//   Serial.print("\tsec:");
//   Serial.print(ntp_tm.tm_sec);          // seconds after the minute  0-61*
//   Serial.print("\twday");
//   Serial.print(ntp_tm.tm_wday);         // days since Sunday 0-6
//   if (ntp_tm.tm_isdst == 1)             // Daylight Saving Time flag
//     Serial.print("\tDST");
//   else
//     Serial.print("\tstandard");
//   Serial.println();
// }

void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println();
  Serial.println("Init...");
  Wire.setClock(800000L);   // макс. 800'000

  setupPins();
  initEEPROM();
  EepromSettings::checkAndInit();
  
  ArduinoOTA.begin();
  if (!digitalRead(PIN_WIFI_RESET)) {
    Serial.println("Hold on start");
    initWifiPortal();
  }

  rtc.begin();
  delay(10);

  initMainPortal();
  timeHelper.setupNTPClient();
  connectWiFi();
  timeHelper.refreshNTP();
  
  // ArduinoOTA.setPassword("lonelybinary");

  Serial.println(appConfig.ntp_server);
  //configTime(MY_TZ, appConfig.ntp_server);
  
  tmp.start();
  checkConnectTimer.setInterval(30000);
  checkConnectTimer.start();
}



void loop() {
  ArduinoOTA.handle();
  ui.tick();
  timeHelper.refreshNTP();
  // delay(50);

  if (reconnectTimer.isReady()) {
    // reconnect
    connectWiFi();
  }
  if (checkConnectTimer.isReady()) {
    if(!reconnectTimer.isEnabled() && WiFi.status() != WL_CONNECTED) {
      // disconnected - retry connect
      connectWiFi();
    }
  }
  
  if (tmp.isReady())
  {
    timeHelper.printRTC();
  }

// byte error, address;
//   int nDevices;
 
//   Serial.println("Scanning...");
 
//   nDevices = 0;
//   for(address = 1; address < 127; address++ )
//   {
//     // The i2c_scanner uses the return value of
//     // the Write.endTransmisstion to see if
//     // a device did acknowledge to the address.
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();
 
//     if (error == 0)
//     {
//       Serial.print("I2C device found at address 0x");
//       if (address<16)
//         Serial.print("0");
//       Serial.print(address,HEX);
//       Serial.println("  !");
 
//       nDevices++;
//     }
//     else if (error==4)
//     {
//       Serial.print("Unknown error at address 0x");
//       if (address<16)
//         Serial.print("0");
//       Serial.println(address,HEX);
//     }    
//   }
//   if (nDevices == 0)
//     Serial.println("No I2C devices found\n");
//   else
//     Serial.println("done\n");
 
//   delay(5000);           // wait 5 seconds for next scan
}

