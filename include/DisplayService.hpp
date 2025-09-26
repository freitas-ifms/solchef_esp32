#ifndef DISPLAYSERVICE_HPP
#define DISPLAYSERVICE_HPP

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C  // Endereço I2C padrão para SSD1306

class DisplayService 
{
    public:
        DisplayService(int sdaPin, int sclPin, int screenWidth, int screenHeight);
        void Start();
        void PrintLine(uint8_t line, const char* texto);
        void Clear();

    private:
        Adafruit_SSD1306 display;
        int sdaPin;
        int sclPin;
        int screenWidth;
        int screenHeight;
};

#endif