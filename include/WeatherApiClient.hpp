#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

class WeatherApiClient
{
public:
    WeatherApiClient();

    // Faz o GET na API /current e atualiza os atributos públicos
    bool AtualizarDados(
        float latitude,
        float longitude,
        const String& unidade = "M", // M = Métrico, S = Scientific, I = Imperial
        const String& lang    = "pt" // pt, en, es, ...
    );

    // ---------- Atributos preenchidos após AtualizarDados() ----------
    String timezone;     // IANA (ex: "America/Sao_Paulo")
    String horaRegistro; // legível (ex: "2025-09-26 14:05")
    String nome_cidade;  // ex: "Corumbá"
    String nome_estado;  // ex: "MS"
    String codigo_pais;  // ex: "BR"

    float   temperatura_ambiente = 0; // °C (temp)
    uint8_t rh                  = 255; // % umidade (255 = indef.)
    float   uv                  = 0;   // índice UV
    float   solar_rad           = 0;   // W/m²
    float   ghi                 = 0;   // W/m²
    float   elev_angle          = 0;   // graus
    uint8_t clouds              = 255; // % cobertura
    float   wind_spd            = 0;   // m/s
    float   gust                = 0;   // m/s
    float   precip              = 0;   // mm/h
    String  sunrise;                   // "HH:MM"
    String  sunset;                    // "HH:MM"

private:
    // Deixe http por padrão; se quiser TLS, troque para "https://api.weatherbit.io/v2.0/current"
    String apiUrl = "http://api.weatherbit.io/v2.0/current";
    //key 1 início 05/10/2025
    String apiKey = "c6112013ce0d43ac8c11cd051bd25154";

    //key 2 início 06/10/2025
    //String apiKey = "2e3eec124ba44872a42483d30921a6d4";
    
    bool DesserializaObjeto(JsonVariantConst item);

    // util para pegar número (float) com segurança
    static float asFloat(JsonVariantConst v, float def = 0) {
        if (v.is<float>())   return v.as<float>();
        if (v.is<double>())  return static_cast<float>(v.as<double>());
        if (v.is<long>())    return static_cast<float>(v.as<long>());
        if (v.is<int>())     return static_cast<float>(v.as<int>());
        return def;
    }
};
