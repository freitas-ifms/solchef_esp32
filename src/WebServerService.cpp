#include "WebServerService.hpp"

WebServerService::WebServerService() : server(80) {
    Serial.println("Construtor do servidor web chamado...");

    meuIp = WiFi.localIP().toString();
    Serial.println("IP do dispositivo: " + meuIp);

    macAddressReceiver = WiFi.macAddress();
    Serial.println("MAC Address do dispositivo: " + macAddressReceiver);
}

void WebServerService::SetupRoutes() {
    // GET básico
    server.on("/solchef", HTTP_GET, [this]() {
        String mode = (WiFi.getMode() == WIFI_AP) ? "AP" : "Station";
        String json = "{";
        json += "\"status\":\"ok\",";
        json += "\"mode\":\"" + mode + "\",";
        json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
        json += "\"mac\":\"" + WiFi.macAddress() + "\"";
        json += "}";
        server.send(200, "application/json", json);
    });

    server.on("/config", HTTP_POST, [this](){
        Serial.println("POST /config recebido");

        String ssid;
        String password;

        //Primeiro tenta form-urlencoded
        if (server.hasArg("ssid") && server.hasArg("password")) {
            ssid = server.arg("ssid");
            password = server.arg("password");
        } 
        //Se não veio form, tenta JSON
        else if (server.hasArg("plain")) {
            String body = server.arg("plain");
            Serial.println("Body JSON recebido:");
            Serial.println(body);

            // Buffer para parse
            StaticJsonDocument<256> doc;
            DeserializationError err = deserializeJson(doc, body);

            if (err) {
                Serial.print("Erro ao parsear JSON: ");
                Serial.println(err.c_str());
            } else {
                ssid = doc["ssid"] | "";
                password = doc["password"] | "";
            }
        } else {
            Serial.println("Nenhum argumento recebido!");
        }

        if (ssid.length() > 0 && password.length() > 0) {
            Serial.println("Credenciais recebidas:");
            Serial.println("SSID: " + ssid);
            Serial.println("Password: " + password);

            prefs.begin("wifi", false);
            prefs.putString("ssid", ssid);
            prefs.putString("password", password);
            prefs.end();

            server.send(200, "application/json",
                "{\"status\":\"ok\",\"msg\":\"Credenciais salvas, reiniciando...\"}");
            delay(1500);
            ESP.restart();
        } else {
            server.send(400, "application/json",
            "{\"status\":\"error\",\"msg\":\"Parâmetros ssid e password são obrigatórios\"}");
    } });

    server.on("/getdadossolchef", HTTP_GET, [this]() {
        String json = dadosRecebidos.ToJSON();
        server.send(200, "application/json", json);
        Serial.println("GET /dadosSolchef -> enviado pacote JSON.");
    });
    
    server.on("/getmacaddresssolchef", HTTP_GET, [this]() {
        server.send(200, "text/plain", dadosRecebidos.macAddressSolchef);
        Serial.println("MAC Address Solchef: " + dadosRecebidos.macAddressSolchef);
    });

    server.begin();
    Serial.println("Servidor HTTP iniciado na porta 80.");
}

void WebServerService::SendData(const DadosSolchef& DadosSolchef) {
    dadosRecebidos = DadosSolchef;
    Serial.println("Dados recebidos e enviados para webserver:");
    Serial.println(dadosRecebidos.ToJSON());
}

void WebServerService::HandleClient() {
    server.handleClient();
}