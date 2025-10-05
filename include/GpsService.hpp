#ifndef GPSSERVICE_HPP
#define GPSSERVICE_HPP

#include <Arduino.h>
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

class GpsService
{
    public:
        GpsService(int rxPin, int txPin, long baudRate, uint8_t uartNum = 2);

        void IniciaGps();     // inicia UART e aumenta RX buffer
        void SincronizaGps(); // chame sempre no loop

        // Critério de fix “saudável”
        bool TemFix(); // location válida, age<2s, sats>=4

        // Leituras (NAN se sem fix)
        double GetLatitude();
        double GetLongitude();
        int GetNumeroSatelites();
        double GetHdop();     // NAN se indisponível
        double GetAltitude(); // metros
        double GetVelocidadeKmph();

        // Horário
        String GetHoraUTC();                    // "HH:MM:SS" ou "--:--:--"
        String GetHoraLocal(int tzOffsetHours); // ex.: -3 (America/Sao_Paulo)

        // Debug
        void ImprimirDadosGps();

    private:
        int _rxPin, _txPin;
        long _baudRate;
        uint8_t _uartNum;

        HardwareSerial _serial;
        TinyGPSPlus _gps;

        static String formatHHMMSS(int hh, int mm, int ss);
};

#endif
