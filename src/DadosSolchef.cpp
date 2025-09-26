#include "DadosSolchef.hpp"

String DadosSolchef::ToJSON() const {
    // Documento JSON com margem para os campos usados
    // Ajuste se adicionar/remover campos
    DynamicJsonDocument doc(1024);

    // --- device ---
    JsonObject device = doc.createNestedObject("device");
    if (isValidFloat(tempAgua))
        device["tempAgua"] = tempAgua;
    if (isValidFloat(tempInterna))
        device["tempInterna"] = tempInterna;
    if (isValidFloat(latitude))
        device["lat"] = latitude;
    if (isValidFloat(longitude))
        device["lon"] = longitude;
    if (macAddressSolchef.length() > 0)
        device["mac"] = macAddressSolchef;

    // --- meta ---
    JsonObject meta = doc.createNestedObject("meta");
    if (ts > 0)
        meta["ts"] = ts; // epoch (s)
    if (timezone.length() > 0)
        meta["timezone"] = timezone;
    if (horaRegistro.length() > 0)
        meta["ob_time"] = horaRegistro; // legível

    // localização "humana" (opcional)
    if (nome_cidade.length() > 0)
        meta["city_name"] = nome_cidade;
    if (nome_estado.length() > 0)
        meta["state_code"] = nome_estado;
    if (codigo_pais.length() > 0)
        meta["country_code"] = codigo_pais;

    // --- env (dados ambientais/solares) ---
    JsonObject env = doc.createNestedObject("env");
    if (isValidFloat(temperatura_ambiente))
        env["temp"] = temperatura_ambiente;
    if (isValidUInt8(rh))
        env["rh"] = rh;
    if (isValidFloat(uv))
        env["uv"] = uv;

    if (isValidFloat(solar_rad))
        env["solar_rad"] = solar_rad;
    if (isValidFloat(ghi))
        env["ghi"] = ghi;
    if (isValidFloat(elev_angle))
        env["elev_angle"] = elev_angle;

    if (isValidUInt8(clouds))
        env["clouds"] = clouds;
    if (isValidFloat(wind_spd))
        env["wind_spd"] = wind_spd;
    if (isValidFloat(gust))
        env["gust"] = gust;
    if (isValidFloat(precip))
        env["precip"] = precip;

    if (sunrise.length() > 0)
        env["sunrise"] = sunrise; // UTC HH:MM
    if (sunset.length() > 0)
        env["sunset"] = sunset; // UTC HH:MM

    // Serializa para String
    String out;
    serializeJson(doc, out);
    return out;
}
