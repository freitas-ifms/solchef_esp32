#ifndef NVSSERVICE_HPP
#define NVSSERVICE_HPP

#include <Arduino.h>
#include <Preferences.h>
#include "nvs_flash.h"

class NvsService {
public:
    NvsService();  
    bool LerNvsWifi(String &ssid, String &key);    
    bool GravarNvsWifi(const String &ssid, const String &key); 
    void LimparNamespace();  

private:
    void inicializarNVS();     // Inicializa NVS e limpa se necessário
    bool testarNamespace();    // Verifica se o namespace está acessível
    const char* NAMESPACE_WIFI = "wifi";
};

#endif