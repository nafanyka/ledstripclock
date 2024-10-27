#pragma once
#include <Arduino.h>
#include "defines.h"
#include <Wire.h>
#include <I2C_eeprom.h>

extern I2C_eeprom ee;
extern const uint8_t eepromVersion;

extern LoginPass lp;
extern ApplicationConfig appConfig;
extern CurrentState currentState;

class EepromSettings {
    public:
        static void checkAndInit()
        {
            uint8_t storedVesrion = 0;
            ee.readBlock(EEA_VERSION, (uint8_t *) &storedVesrion, sizeof(storedVesrion));

            if (storedVesrion != eepromVersion)
            {
                Serial.println("EEPROM. Write default");
                //
                ApplicationConfig ac;
                EepromSettings::setApplicationConfig(ac, true);
                //
                // CurrentState cs;
                // EepromSettings::setCurrentState(cs, true);
            } else {
                Serial.println("EEPROM. Restore");
            }
            EepromSettings::getApplicationConfig(appConfig);
            // EepromSettings::getCurrentState(currentState);
            EepromSettings::getLoginPass(lp);
        }

        static void getLoginPass(LoginPass& lp)
        {
            ee.readBlock(EEA_LP, (uint8_t *) &lp, sizeof(LoginPass));
            Serial.println(lp.ssid);
            Serial.println(lp.pass);
        }

        static void setLoginPass(LoginPass lp)
        {
            ee.setBlock(EEA_LP, 0, sizeof(LoginPass));
            ee.writeBlock(EEA_LP, (uint8_t *) &lp, sizeof(lp));
        }

        static void getApplicationConfig(ApplicationConfig& ac)
        {
            ee.readBlock(EEA_AC, (uint8_t *) &ac, sizeof(ApplicationConfig));
        }

        static void setApplicationConfig(ApplicationConfig ac, bool rewrite = false)
        {
            if (rewrite) {
                ee.setBlock(EEA_AC, 0, sizeof(ApplicationConfig));
            }
            ee.updateBlock(EEA_AC, (uint8_t *) &ac, sizeof(ac));
        }
        
        // static void getCurrentState(CurrentState& cs)
        // {
        //     ee.readBlock(EEA_CS, (uint8_t *) &cs, sizeof(CurrentState));
        // }

        // static void setCurrentState(CurrentState cs, bool rewrite = false)
        // {
        //     if (rewrite) {
        //         ee.setBlock(EEA_CS, 0, sizeof(CurrentState));
        //     }
        //     ee.updateBlock(EEA_CS, (uint8_t *) &cs, sizeof(cs));
        // }
};
