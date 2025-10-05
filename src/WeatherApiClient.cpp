#include "WeatherApiClient.hpp"

WeatherApiClient::WeatherApiClient() {}

bool WeatherApiClient::AtualizarDados(float latitude, float longitude,
                                      const String& unidade, const String& lang)
{
    // Monta URL
    String url = apiUrl;
    url += "?lat=" + String(latitude, 6)
        +  "&lon=" + String(longitude, 6)
        +  "&units=" + unidade
        +  "&lang="  + lang
        +  "&key="   + apiKey;

    HTTPClient http;
    bool https = url.startsWith("https://");

    if (https) {
        // Caso mude apiUrl para HTTPS, usamos WiFiClientSecure
        WiFiClientSecure* client = new WiFiClientSecure;
        client->setInsecure(); // para protótipo; para produção use setCACert()
        if (!http.begin(*client, url)) { delete client; return false; }
    } else {
        WiFiClient* client = new WiFiClient;
        if (!http.begin(*client, url)) { delete client; return false; }
    }

    http.setTimeout(15000);
    int code = http.GET();
    if (code != HTTP_CODE_OK) {
        Serial.printf("[WeatherApiClient] HTTP %d\n", code);
        http.end();
        return false;
    }

    String payload = http.getString();
    http.end();

    // Use filtro para reduzir RAM
    StaticJsonDocument<512> filter;
    JsonObject f0 = filter["data"].createNestedObject();
    f0["timezone"]     = true;
    f0["ob_time"]      = true; // hora do registro
    f0["city_name"]    = true;
    f0["state_code"]   = true;
    f0["country_code"] = true;

    f0["temp"]      = true;
    f0["rh"]        = true;
    f0["uv"]        = true;
    f0["solar_rad"] = true;
    f0["ghi"]       = true;
    f0["elev_angle"]= true;
    f0["clouds"]    = true;
    f0["wind_spd"]  = true;
    f0["gust"]      = true;
    f0["precip"]    = true;
    f0["sunrise"]   = true;
    f0["sunset"]    = true;

    // ~2–4 KB costuma bastar com esse filtro; aumente se der erro
    DynamicJsonDocument doc(4096);
    DeserializationError err = deserializeJson(
        doc, payload, DeserializationOption::Filter(filter)
    );
    if (err) {
        Serial.printf("[WeatherApiClient] JSON error: %s\n", err.c_str());
        return false;
    }

    JsonArrayConst data = doc["data"].as<JsonArrayConst>();
    if (!data || data.size() == 0) {
        Serial.println("[WeatherApiClient] Campo 'data' vazio.");
        return false;
    }

    return DesserializaObjeto(data[0]);
}

bool WeatherApiClient::DesserializaObjeto(JsonVariantConst item)
{
    if (!item.is<JsonObjectConst>()) return false;

    // Identificação/local
    timezone     = item["timezone"]     | "";
    horaRegistro = item["ob_time"]      | ""; // ex: "2025-10-05 14:05"
    nome_cidade  = item["city_name"]    | "";
    nome_estado  = item["state_code"]   | "";
    codigo_pais  = item["country_code"] | "";

    // Métricas ambientais
    temperatura_ambiente = asFloat(item["temp"], 0.0f);
    rh        = item["rh"].is<int>() ? (uint8_t)item["rh"].as<int>() : rh;
    uv        = asFloat(item["uv"], 0.0f);
    solar_rad = asFloat(item["solar_rad"], 0.0f);
    ghi       = asFloat(item["ghi"], 0.0f);
    elev_angle= asFloat(item["elev_angle"], 0.0f);
    clouds    = item["clouds"].is<int>() ? (uint8_t)item["clouds"].as<int>() : clouds;
    wind_spd  = asFloat(item["wind_spd"], 0.0f);
    gust      = asFloat(item["gust"], 0.0f);
    precip    = asFloat(item["precip"], 0.0f);

    // Nas respostas do Weatherbit, sunrise/sunset costumam vir como "HH:MM"
    sunrise = item["sunrise"] | "";
    sunset  = item["sunset"]  | "";

    return true;
}
