#ifndef SENSORTEMPERATURASERVICE_HPP
#define SENSORTEMPERATURASERVICE_HPP

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class SensorTemperaturaService
{
    public:
        SensorTemperaturaService(int pin);
        float GetTemperaturaAgua();
        float GetTemperaturaInterna();

    private:
        float temperaturaAgua;
        float temperaturaInterna;
        int pin;
        OneWire oneWire;
        DallasTemperature sensor;
};

#endif