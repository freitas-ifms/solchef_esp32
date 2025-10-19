#include <Arduino.h>
#include "DisplayService.hpp"
#include "WifiService.hpp"
#include "NvsService.hpp"
#include "WebServerService.hpp"
#include "SensorTemperaturaService.hpp"
#include "GpsService.hpp"
#include "DadosSolchef.hpp"
#include "WeatherApiClient.hpp"
#include "FileService.hpp"
#include "WeatherApiClient.hpp"
// ---------- Objetos globais ----------
DisplayService oled(21, 22, 128, 64);   // SDA, SCL, largura, altura
WifiService wifiManager;
WebServerService webServer;
SensorTemperaturaService sensorTemperaturaAgua(4);
SensorTemperaturaService sensorTemperaturaInterna(5);
NvsService nvsManager;               
GpsService gps(16, 17, 9600); // RX, TX, baudrate
DadosSolchef dados;
bool apModeAtivo = false;
WeatherApiClient clima;
unsigned long ultimaAtualizacaoClima = 0;
const unsigned long intervaloClima = 1800000; // 30 minutos em ms
bool primeiraExecucao = true;
int contadorWeatherbit = 0;
/*
static const int   TZ_OFFSET_HOURS   = -4;     // America/Cuiaba
static const uint32_t UI_INTERVAL_MS = 500;    // refresh OLED
static const uint32_t SEND_INTERVAL_MS = 5000; // envio de dados
static const uint32_t DBG_INTERVAL_MS = 2000;  // logs no Serial


uint32_t lastUiMs   = 0;
uint32_t lastSendMs = 0;
uint32_t lastDbgMs  = 0;


void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  oled.Start();
  //gps.IniciaGps();

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
    digitalWrite(2, HIGH);
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
}*/

void setup()
{
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  oled.Start();

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

/*void loop()
{
  webServer.HandleClient();

  if (clima.AtualizarDados(-19.0086f, -57.6510f, "M", "pt")) {
    Serial.printf("%s/%s - %s %s | T=%.1f°C RH=%u%% UV=%.1f\n",
      clima.nome_cidade.c_str(), clima.nome_estado.c_str(),
      clima.codigo_pais.c_str(), clima.timezone.c_str(),
      clima.temperatura_ambiente, clima.rh, clima.uv);
  } else {
    Serial.println("Falha ao atualizar dados.");
  }

  // IP (pode estar vazio em AP)
  String ipStr = wifiManager.getIPAddress().c_str();
  dados.ipAddressSolchef = ipStr;
  dados.macAddressSolchef = wifiManager.getMeuMacAddress().c_str();
  dados.tempAgua = sensorTemperaturaAgua.GetTemperaturaAgua();
  dados.tempInterna = sensorTemperaturaInterna.GetTemperaturaAgua();
  dados.codigo_pais = clima.codigo_pais.c_str();
  dados.nome_estado = clima.nome_estado.c_str();
  dados.nome_cidade = clima.nome_cidade.c_str();
  dados.timezone = clima.timezone.c_str();
  dados.horaRegistro = clima.horaRegistro.c_str();
  dados.temperatura_ambiente = clima.temperatura_ambiente;
  dados.rh = clima.rh;
  dados.uv = clima.uv;
  dados.solar_rad = clima.solar_rad;
  dados.ghi = clima.ghi;
  dados.elev_angle = clima.elev_angle;
  dados.clouds = clima.clouds;
  dados.wind_spd = clima.wind_spd;
  dados.gust = clima.gust;
  dados.precip = clima.precip;
  dados.sunrise = clima.sunrise.c_str();
  dados.sunset = clima.sunset.c_str();

  // Linha 0 e 1
  oled.PrintLine(0, "SolChef Monitor");
  oled.PrintLine(1, ipStr.c_str()); // ou WiFi.localIP().toString().c_str()

  // Linha 2: T Agua
  char l2[24];
  snprintf(l2, sizeof(l2), "T Agua: %.1f C", dados.tempAgua);
  oled.PrintLine(2, l2);

  // Linha 3: T Int
  char l3[24];
  snprintf(l3, sizeof(l3), "T Int:  %.1f C", dados.tempInterna);
  oled.PrintLine(3, l3);

  // Envio web
  webServer.SendData(dados);
  delay(30000);
}*/

void loop() {
  webServer.HandleClient();

  // Atualiza clima a cada 10 minutos
  unsigned long agora = millis();
  Serial.print("Agora: ");
  Serial.println(agora);
  Serial.printf("Atualizando clima (contador %d)...\n", contadorWeatherbit);

  if (primeiraExecucao || (agora - ultimaAtualizacaoClima >= intervaloClima)) {
    contadorWeatherbit++;
    primeiraExecucao = false;
    ultimaAtualizacaoClima = agora;
    Serial.println("Ultima atualizacao:");
    Serial.println(ultimaAtualizacaoClima);

    if (clima.AtualizarDados(-19.0086f, -57.6510f, "M", "pt")) {
      Serial.printf("%s/%s - %s %s | T=%.1f°C RH=%u%% UV=%.1f\n",
        clima.nome_cidade.c_str(), clima.nome_estado.c_str(),
        clima.codigo_pais.c_str(), clima.timezone.c_str(),
        clima.temperatura_ambiente, clima.rh, clima.uv);
    } else {
      Serial.println("Falha ao atualizar dados.");
    }
  }

  // Atualizações contínuas do sistema
  String ipStr = wifiManager.getIPAddress().c_str();
  dados.ipAddressSolchef = ipStr;
  dados.macAddressSolchef = wifiManager.getMeuMacAddress().c_str();
  dados.tempAgua = sensorTemperaturaAgua.GetTemperaturaAgua();
  dados.tempInterna = sensorTemperaturaInterna.GetTemperaturaAgua();
  dados.latitude = -19.0086f;
  dados.longitude = -57.6510f;

  // Últimos dados climáticos (mantém os valores do último update)
  dados.codigo_pais = clima.codigo_pais.c_str();
  dados.nome_estado = clima.nome_estado.c_str();
  dados.nome_cidade = clima.nome_cidade.c_str();
  dados.timezone = clima.timezone.c_str();
  dados.horaRegistro = clima.horaRegistro.c_str();
  dados.temperatura_ambiente = clima.temperatura_ambiente;
  dados.rh = clima.rh;
  dados.uv = clima.uv;
  dados.solar_rad = clima.solar_rad;
  dados.ghi = clima.ghi;
  dados.elev_angle = clima.elev_angle;
  dados.clouds = clima.clouds;
  dados.wind_spd = clima.wind_spd;
  dados.gust = clima.gust;
  dados.precip = clima.precip;
  dados.sunrise = clima.sunrise.c_str();
  dados.sunset = clima.sunset.c_str();

  // Atualiza OLED
  oled.PrintLine(0, "SolChef Monitor");
  oled.PrintLine(1, ipStr.c_str());

  char l2[24];
  snprintf(l2, sizeof(l2), "T Agua: %.1f C", dados.tempAgua);
  oled.PrintLine(2, l2);

  char l3[24];
  snprintf(l3, sizeof(l3), "T Int:  %.1f C", dados.tempInterna);
  oled.PrintLine(3, l3);

  // Envia dados ao webserver (pode continuar a cada 30s)
  webServer.SendData(dados);
}
