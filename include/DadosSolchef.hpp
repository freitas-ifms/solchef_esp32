#ifndef DADOSSOLCHEF_HPP
#define DADOSSOLCHEF_HPP

#include <Arduino.h>
#include <ArduinoJson.h>

class DadosSolchef {
  public:
    // --- Sensores do SolChef (dispositivo) ---
    float tempAgua = 0;        
    float tempInterna = 0;     
    float latitude = 0;        
    float longitude = 0;       
    String macAddressSolchef;
    String ipAddressSolchef;    

    // Satelite: Dados Locais
    unsigned long ts = 0;               // Epoch (segundos)
    String timezone;                    // IANA (ex: "America/Sao_Paulo")
    String horaRegistro;                // opcional, legível (ex: "2025-09-26 14:05")
    String nome_cidade;                 // ex: "Corumbá"
    String nome_estado;                 // ex: "MS"
    String codigo_pais;                 // ex: "BR"

    // Dados do clima
    float temperatura_ambiente = 0; // °C (temp)
    uint8_t rh = 255;               // % umidade (255 = indef.)
    float uv = 0;                   // UV Index (informativo)
    float solar_rad = 0;            // W/m² (estimado)
    float ghi = 0;                  // W/m²
    float elev_angle = 0;           // graus
    uint8_t clouds = 255;           // % cobertura
    float wind_spd = 0;             // m/s
    float gust = 0;                 // m/s
    float precip = 0;               // mm/h
    String sunrise;                 // "HH:MM" UTC
    String sunset;                  // "HH:MM" UTC

    // Serialização
    String ToJSON() const;

  private:
    // Helpers para (não) enviar valores ausentes
    static bool isValidFloat(float v) { return !isnan(v); }
    static bool isValidUInt8(uint8_t v) { return v != 255; }
};

#endif
