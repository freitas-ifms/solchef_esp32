#include "WifiService.hpp"
#include <Preferences.h>  // Para salvar credenciais se necessário

WifiService::WifiService() : _ssid(""), _key("") {}

// Tenta conectar no WiFi Station Mode com timeout
bool WifiService::connect(const char* ssid, const char* key, unsigned long timeoutMs) {
    _ssid = ssid ? ssid : "";
    _key  = key  ? key  : "";

    Serial.print("Conectando ao WiFi: ");
    Serial.print(_ssid.c_str());

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid.c_str(), _key.c_str());

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeoutMs) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n Conectado ao WiFi!");
        Serial.print("IP Address: ");   Serial.println(WiFi.localIP());
        Serial.print("Subnet Mask: ");  Serial.println(WiFi.subnetMask());
        Serial.print("Gateway IP: ");   Serial.println(WiFi.gatewayIP());
        Serial.print("MAC Address: ");  Serial.println(WiFi.macAddress());
        return true;
    } else {
        Serial.println("\n Falha na conexão WiFi (timeout).");
        return false;
    }
}

// Verifica se ainda está conectado
bool WifiService::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

// Retorna IP em formato std::string
std::string WifiService::getIPAddress() {
    if (WiFi.status() == WL_CONNECTED) {
        return std::string(WiFi.localIP().toString().c_str());
    }
    return "0.0.0.0";
}

// Retorna MAC Address
std::string WifiService::getMeuMacAddress() {
    return std::string(WiFi.macAddress().c_str());
}

// Ativa Access Point Mode para configuração
void WifiService::startAccessPoint(const char* apName, const char* apPassword) {
    Serial.println("Iniciando Access Point Mode para configuração...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apName, apPassword);

    IPAddress apIP = WiFi.softAPIP();
    Serial.print("AP iniciado. Acesse o IP: ");
    Serial.println(apIP);
}