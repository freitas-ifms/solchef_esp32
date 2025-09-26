#include "GpsService.hpp"

GpsService::GpsService(int rxPin, int txPin, long baudRate, uint8_t uartNum)
: _rxPin(rxPin), _txPin(txPin), _baudRate(baudRate), _uartNum(uartNum), _serial(uartNum) {}

void GpsService::IniciaGps() {
  _serial.setRxBufferSize(1024);                 // evita perder NMEA
  _serial.begin(_baudRate, SERIAL_8N1, _rxPin, _txPin);
}

void GpsService::SincronizaGps() {
  while (_serial.available() > 0) {
    _gps.encode(_serial.read());
  }
}

bool GpsService::TemFix() {
  bool locOk  = _gps.location.isValid()  && _gps.location.age()  < 2000;
  bool satsOk = _gps.satellites.isValid() ? (_gps.satellites.value() >= 4) : true;
  bool hdopOk = _gps.hdop.isValid()      ? (_gps.hdop.hdop() <= 3.0)       : true;
  return locOk && satsOk && hdopOk;
}

double GpsService::GetLatitude() { return TemFix() ? _gps.location.lat() : NAN; }
double GpsService::GetLongitude() { return TemFix() ? _gps.location.lng() : NAN; }
int    GpsService::GetNumeroSatelites() { return _gps.satellites.isValid() ? (int)_gps.satellites.value() : 0; }
double GpsService::GetHdop() { return _gps.hdop.isValid() ? _gps.hdop.hdop() : NAN; }
double GpsService::GetAltitude() { return (_gps.altitude.isValid() && TemFix()) ? _gps.altitude.meters() : NAN; }
double GpsService::GetVelocidadeKmph() { return _gps.speed.isValid() ? _gps.speed.kmph() : NAN; }

String GpsService::formatHHMMSS(int hh, int mm, int ss) {
  char b[9]; snprintf(b, sizeof(b), "%02d:%02d:%02d", hh, mm, ss); return String(b);
}

String GpsService::GetHoraUTC() {
  if (_gps.time.isValid()) return formatHHMMSS(_gps.time.hour(), _gps.time.minute(), _gps.time.second());
  return "--:--:--";
}

String GpsService::GetHoraLocal(int tzOffsetHours) {
  if (!_gps.time.isValid()) return "--:--:--";
  int h = (int)_gps.time.hour() + tzOffsetHours;
  while (h < 0) h += 24; while (h >= 24) h -= 24;
  return formatHHMMSS(h, _gps.time.minute(), _gps.time.second());
}

void GpsService::ImprimirDadosGps() {
  Serial.println(F("=== Dados GPS ==="));
  if (TemFix()) {
    Serial.print(F("Latitude:  ")); Serial.println(_gps.location.lat(), 6);
    Serial.print(F("Longitude: ")); Serial.println(_gps.location.lng(), 6);
    Serial.print(F("Altitude:  ")); Serial.print(GetAltitude()); Serial.println(F(" m"));
  } else {
    Serial.println(F("Sem fix válido"));
  }
  Serial.print(F("Satélites: ")); Serial.println(GetNumeroSatelites());
  Serial.print(F("HDOP:      ")); Serial.println(GetHdop(), 2);

  if (_gps.time.isValid()) Serial.printf("Hora (UTC): %02d:%02d:%02d\n", _gps.time.hour(), _gps.time.minute(), _gps.time.second());
  else                     Serial.println(F("Hora (UTC): --:--:--"));

  if (_gps.date.isValid()) Serial.printf("Data: %02d/%02d/%04d\n", _gps.date.day(), _gps.date.month(), _gps.date.year());
  else                     Serial.println(F("Data: --/--/----"));

  Serial.print(F("Velocidade: "));
  double v = GetVelocidadeKmph();
  if (isnan(v)) Serial.println(F("-- km/h")); else { Serial.print(v, 2); Serial.println(F(" km/h")); }
  Serial.println(F("=================="));
}
