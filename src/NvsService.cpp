#include "NvsService.hpp"

NvsService::NvsService() {
    inicializarNVS();
}

void NvsService::inicializarNVS() {
    Serial.println("Inicializando NVS...");
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        Serial.println("NVS corrompida ou cheia! Limpando TODA a partição...");
        nvs_flash_erase();      // limpa toda a NVS
        nvs_flash_init();       // re-inicializa
        return;
    }

    Serial.println("NVS inicializada com sucesso.");

    // Agora verificamos se o namespace exclusivo está acessível
    if (!testarNamespace()) {
        Serial.println("Namespace Wi-Fi inacessível, limpando apenas ele...");
        LimparNamespace();
    } else {
        Serial.println("Namespace Wi-Fi acessível.");
    }
}

bool NvsService::testarNamespace() {
    Preferences prefs;
    bool ok = prefs.begin(NAMESPACE_WIFI, true);
    if (ok) prefs.end();
    return ok;
}

bool NvsService::LerNvsWifi(String &ssid, String &key) {
    Preferences prefs;

    if (!prefs.begin(NAMESPACE_WIFI, true)) {
        Serial.println("Falha ao abrir namespace para leitura! Limpando...");
        LimparNamespace();  // tenta recuperar limpando apenas este namespace
        return false;
    }

    ssid = prefs.getString("ssid", "");
    key  = prefs.getString("password", "");
    prefs.end();

    if (ssid.length() > 0) {
        Serial.println("Credenciais lidas:");
        Serial.println("SSID: " + ssid);
        return true;
    } else {
        Serial.println("Nenhuma credencial salva.");
        return false;
    }
}

bool NvsService::GravarNvsWifi(const String &ssid, const String &key) {
    Preferences prefs;

    if (!prefs.begin(NAMESPACE_WIFI, false)) {
        Serial.println("Falha ao abrir namespace para escrita! Limpando...");
        LimparNamespace();  
        return false;
    }

    prefs.putString("ssid", ssid);
    prefs.putString("password", key);
    prefs.end();

    Serial.println("Credenciais Wi-Fi gravadas na NVS:");
    Serial.println("SSID: " + ssid);
    return true;
}

void NvsService::LimparNamespace() {
    Preferences prefs;

    if (prefs.begin(NAMESPACE_WIFI, false)) {
        prefs.clear();   
        prefs.end();
        Serial.println("Namespace Wi-Fi limpo com sucesso!");
    } else {
        Serial.println("Não foi possível abrir namespace para limpeza!");
    }
}