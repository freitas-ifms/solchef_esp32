#include <Arduino.h>
#include "DisplayService.hpp"
#include "WifiService.hpp"
#include "NvsService.hpp"
#include "WebServerService.hpp"
#include "SensorTemperaturaService.hpp"
#include "GpsService.hpp"
#include "DadosSolchef.hpp"

// ---------- Objetos globais ----------
DisplayService oled(21, 22, 128, 64);   // SDA, SCL, largura, altura
WifiService wifiManager;
WebServerService webServer;
SensorTemperaturaService sensorTemperatura(4);
NvsService nvsManager;               
GpsService gps(16, 17, 9600);
DadosSolchef dados;
bool apModeAtivo = false;

// ---------- Constantes de tempo ----------
static const int   TZ_OFFSET_HOURS   = -4;     // America/Cuiaba
static const uint32_t UI_INTERVAL_MS = 500;    // refresh OLED
static const uint32_t SEND_INTERVAL_MS = 5000; // envio de dados
static const uint32_t DBG_INTERVAL_MS = 2000;  // logs no Serial

// ---------- Timers ----------
uint32_t lastUiMs   = 0;
uint32_t lastSendMs = 0;
uint32_t lastDbgMs  = 0;

void setup() {
  Serial.begin(115200);
  oled.Start();
  gps.IniciaGps();

  // Wi-Fi: tenta credenciais salvas, senão AP de config
  String savedSSID, savedPass;
  bool conectado = false;

  if (nvsManager.LerNvsWifi(savedSSID, savedPass)) {
    Serial.println("Credenciais encontradas. Tentando conectar...");
    conectado = wifiManager.connect(savedSSID.c_str(), savedPass.c_str(), 10000);
    if (!conectado) {
      Serial.println("Falha ao conectar → iniciando AP");
      wifiManager.startAccessPoint("SolChef_Config", "12345678");
      apModeAtivo = true;
    } else {
      Serial.println("Conectado em Station Mode!");
    }
  } else {
    Serial.println("Nenhuma credencial salva. Iniciando AP...");
    wifiManager.startAccessPoint("SolChef_Config", "12345678");
    apModeAtivo = true;
  }

  // Sempre inicia o servidor web (rotas locais/endpoint)
  webServer.SetupRoutes();
  delay(300); // pequeno respiro para o servidor subir
  Serial.println("Setup concluido!");
}

void loop() {
  // 1) GPS e servidor devem "respirar" o tempo todo
  gps.SincronizaGps();
  webServer.HandleClient();

  // 2) Se houver fix, atualiza lat/lon; sem fix mantém último valor (evita 0,0)
  if (gps.TemFix()) {
    double lat = gps.GetLatitude();
    double lon = gps.GetLongitude();
    if (!isnan(lat) && !isnan(lon)) {
      dados.latitude  = (float)lat;
      dados.longitude = (float)lon;
    }
  }

  // 3) Atualiza UI periodicamente
  uint32_t now = millis();
  if (now - lastUiMs >= UI_INTERVAL_MS) {
    lastUiMs = now;

    oled.PrintLine(0, "SolChef Monitor");

    // IP (pode estar vazio em AP)
    String ipStr = wifiManager.getIPAddress().c_str();
    oled.PrintLine(1, ipStr.c_str());

    // Linha de localização
    char bufLoc[32];
    if (gps.TemFix()) {
      snprintf(bufLoc, sizeof(bufLoc), "Lat %.5f Lon %.5f", dados.latitude, dados.longitude);
    } else {
      snprintf(bufLoc, sizeof(bufLoc), "Lat --.-- Lon --.--");
    }
    oled.PrintLine(2, bufLoc);

    // Hora local (UTC-3). Evita toInt(); usa string pronta "HH:MM:SS"
    String horaLocal = gps.GetHoraLocal(TZ_OFFSET_HOURS);
    dados.horaRegistro = horaLocal;
    oled.PrintLine(3, horaLocal.c_str());
  }

  // 4) Lê sensores + envia dados periodicamente
  if (now - lastSendMs >= SEND_INTERVAL_MS) {
    lastSendMs = now;

    dados.tempAgua    = sensorTemperatura.GetTemperaturaAgua();
    dados.tempInterna = sensorTemperatura.GetTemperaturaInterna();
    dados.macAddressSolchef = wifiManager.getMeuMacAddress().c_str();

    // Envia para o endpoint/consumidor configurado no WebServerService
    webServer.SendData(dados);
  }

  // 5) Logs de diagnóstico com baixa taxa (não travar o parser NMEA)
  if (now - lastDbgMs >= DBG_INTERVAL_MS) {
    lastDbgMs = now;
    gps.ImprimirDadosGps();
    if (!gps.TemFix()) Serial.println("Sem fix GPS");
  }
}
