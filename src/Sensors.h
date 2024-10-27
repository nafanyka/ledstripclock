#pragma once

#include <Arduino.h>
#include "defines.h"
#include <Adafruit_BMP280.h>
#include <Adafruit_HTU21DF.h>
#include <hp_BH1750.h>

extern Adafruit_BMP280 bmp;
extern Adafruit_HTU21DF htu;
extern hp_BH1750 lux;
extern float luxValue;

class Sensors
{
private:
    SensorData currentState;
    float calculateDewPoint(float, float);
public:
    Sensors(/* args */);
    void readSensors();
    void init();
    void debug();
};

Sensors::Sensors(/* args */)
{
}

void Sensors::readSensors()
{
    currentState.temperature_press = bmp.readTemperature();
    currentState.pressure_pa = bmp.readPressure();
    currentState.pressure_mm = currentState.pressure_pa / 133.322;
    //
    currentState.temperature = htu.readTemperature();
    currentState.humidity = htu.readHumidity();
    currentState.dew_point_temperature = Sensors::calculateDewPoint(currentState.temperature, currentState.humidity);
    //
    currentState.illumination = luxValue;
}

float Sensors::calculateDewPoint(float temperature, float humidity) {
    float a, b;
    if (temperature >= 0) {
        a = 17.27;
        b = 237.7;
    } else {
        a = 22.57;
        b = 272.7;
    }
    float alpha = (a * temperature) / (b + temperature) + log(humidity / 100.0);
    float dewPoint = (b * alpha) / (a - alpha);
    return round(dewPoint * 100) / 100;
}

void Sensors::init()
{
    lux.start();
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_1000); /* Standby time. */
}

void Sensors::debug()
{
    Serial.println("BMP280");
    Serial.print(F("T="));
    Serial.print(currentState.temperature_press);
    Serial.print("*C. ");

    Serial.print(F("P="));
    Serial.print(currentState.pressure_pa / 1000.0);
    Serial.print("kPa. ");

    Serial.print(F("P="));
    Serial.print(currentState.pressure_pa / 133.322);
    Serial.print("mmHg");

    Serial.println();


    Serial.println("HTU21");
    Serial.print(F("T="));
    Serial.print(currentState.temperature);
    Serial.print("*C. ");

    Serial.print(F("Hum="));
    Serial.print(currentState.humidity);
    Serial.print("%. ");

    Serial.print(F("DewP="));
    Serial.print(currentState.dew_point_temperature);
    Serial.print("*C");
    
    Serial.println();


    Serial.println("BH1750");
    Serial.print(F("Illum="));
    Serial.print(currentState.illumination);
    Serial.print("lx");
    
    Serial.println();
    Serial.println();
}