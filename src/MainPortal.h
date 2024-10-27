#pragma once

#include <Arduino.h>
#include "defines.h"
#include "I2C_eeprom.h"
#include <GyverPortal.h>
#include "EepromSettings.h"


extern GyverPortal ui;
extern ApplicationConfig appConfig;
extern CurrentState currentState;


void initMainPortal();
void buildMainPortal();
void actionMainPortal(GyverPortal& p);
String errorMessage = "";
bool reloadPage = false;


void initMainPortal() {
    ui.attachBuild(buildMainPortal);
    ui.attach(actionMainPortal);
    ui.start();
}

void buildMainPortal() {
    GP.BUILD_BEGIN(); //700
    Serial.println("BUILD_BEGIN");
    GP.THEME(GP_DARK);
    GP.ONLINE_CHECK();
    GP.UPDATE("validatorAlert");
    GP.ALERT("validatorAlert"); 
    GP.RELOAD("reload_ctrl");
    GP.UPDATE("reload_ctrl");


    //GP.NAV_TABS_LINKS("//");
   
        /* settings */
    if (ui.uri("/")) {
        GP.PAGE_TITLE(F("Clock Settings"));
        M_BLOCK_TAB(
            F("NTP server"),
            GP.TEXT("as.ntp_server", "NTP server", appConfig.ntp_server);
        );
        M_BLOCK_TAB(
            F("Standart time offset (from UTC)"),
            GP.SELECT("as.tz_offset", tz_str, appConfig.tz_offset_idx);
        );
        M_BLOCK_TAB(
            F("Use sumer time?"),
            GP.SWITCH("as.tz_dst", appConfig.tz_dst);
        );
        M_BLOCK_TAB(
            F("NTP Refresh period"),
            GP.NUMBER("as.ntp_refresh", "in seconds", appConfig.ntp_refresh_period);
        );

    }
    
    GP.BUILD_END();
}




void actionMainPortal(GyverPortal& p) {
    // Serial.println("MainUI action");
    if (ui.click()) {
        Serial.print(ui.clickName());
        Serial.print(" - ");
        Serial.println(ui.clickNameSub());
    }

    bool appConfigUpdated = false;
    appConfigUpdated = appConfigUpdated || ui.clickStr("as.ntp_server", appConfig.ntp_server);
    appConfigUpdated = appConfigUpdated || ui.clickInt("as.tz_offset", appConfig.tz_offset_idx);
    if (ui.click("as.tz_offset")) {
        appConfig.tz_offset_value = tz_arr[appConfig.tz_offset_idx];
    }
    appConfigUpdated = appConfigUpdated || ui.clickBool("as.tz_dst", appConfig.tz_dst);
    appConfigUpdated = appConfigUpdated || ui.clickInt("as.ntp_refresh", appConfig.ntp_refresh_period);

    if (appConfigUpdated) {
        //TODO timer
        EepromSettings::setApplicationConfig(appConfig);
    }

    
    if (ui.update()) {
        if (ui.update("validatorAlert") && errorMessage.length() > 0) {
            ui.answer(errorMessage);
            errorMessage = "";
        }
        if (ui.update("reload_ctrl") && reloadPage) {
            reloadPage = false;
            ui.answer(1);
        }
    }
    //
    

    //ui.uri("/settings")

    // if (p.update("reload_ctrl")) {
    //     p.answer(1);
    // }
    // if (ui.click("btnInit")) {
    //     Serial.println("btnInit");
    // };
}

