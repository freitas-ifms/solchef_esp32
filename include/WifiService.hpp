#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <string>

class WifiService {
    public:
        WifiService();

        /**
         * Conecta ao WiFi no modo Station.
         * 
         * @param ssid Nome da rede WiFi.
         * @param key  Senha da rede WiFi.
         * @param timeoutMs Tempo limite para tentar conectar (default 10s).
         * @return true se conectado com sucesso, false se falhou.
         */
        bool connect(const char* ssid, const char* key, unsigned long timeoutMs = 10000);

        /**
         * Verifica se ainda está conectado ao WiFi.
         * 
         * @return true se conectado, false caso contrário.
         */
        bool isConnected();

        /**
         * Retorna o endereço IP local em formato string.
         */
        std::string getIPAddress();

        /**
         * Retorna o MAC Address do dispositivo.
         */
        std::string getMeuMacAddress();

        /**
         * Ativa o modo Access Point para configuração.
         * 
         * @param apName Nome do Access Point.
         * @param apPassword Senha do Access Point.
         */
        void startAccessPoint(const char* apName = "SolChef_Config", const char* apPassword = "12345678");

    private:
        std::string _ssid;
        std::string _key;
};