#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "DadosSolchef.hpp"

class WebServerService {
    public:
        WebServerService();
        void SetupRoutes();
        void HandleClient();
        String getMeuIp() const { return meuIp; }
        String getMacAddressReceiver() const { return macAddressReceiver; }
        void SendData(const DadosSolchef &dadosSolchef);

    private:
        WebServer server;        // Servidor HTTP na porta 80
        String meuIp;            // IP atual do dispositivo
        String macAddressReceiver; // MAC Address do dispositivo
        Preferences prefs; // global para salvar configs
        DadosSolchef dadosRecebidos;
};