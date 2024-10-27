#pragma once

#include <Arduino.h>
#include "defines.h"
#include "I2C_eeprom.h"
#include <GyverPortal.h>
#include "EepromSettings.h"

extern GyverPortal ui;
extern LoginPass lp;

void buildWifiPortal();
void initWifiPortal();
void actionWifiPortal(GyverPortal& p);


void buildWifiPortal() {
    GP.BUILD_BEGIN();
    GP.THEME(GP_DARK);

    GP.FORM_BEGIN("/login");
    GP.TEXT("lg", "Login", lp.ssid);
    GP.BREAK();
    GP.PASS("ps", "Password", lp.pass);
    GP.SUBMIT("Submit");
    GP.FORM_END();

    GP.BUILD_END();
}

void initWifiPortal() {
    Serial.println("Portal start");

    // запускаем точку доступа
    WiFi.mode(WIFI_AP);
    WiFi.softAP("CLOCK_WiFiConfig");
    Serial.println(WiFi.softAPIP());

    // запускаем портал
    ui.attachBuild(buildWifiPortal);
    ui.start();
    ui.attach(actionWifiPortal);

    // работа портала
    while (ui.tick());
}

void actionWifiPortal(GyverPortal& p) {
    if (p.form("/login")) {                 // кнопка нажата
        p.copyStr("lg", lp.ssid);           // копируем себе
        p.copyStr("ps", lp.pass);
        EepromSettings::setLoginPass(lp);
        // WiFi.softAPdisconnect();            // отключаем AP
        Serial.println("Creds saved...");

        Serial.println("Reset..");
        // ESP.restart();
    }
}