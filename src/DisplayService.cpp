#include "DisplayService.hpp"


DisplayService::DisplayService(int sdaPin, int sclPin, int screenWidth, int screenHeight)
    : display(screenWidth, screenHeight, &Wire, OLED_RESET), 
      sdaPin(sdaPin), sclPin(sclPin) {}


void DisplayService::Start() {
    Wire.begin(sdaPin, sclPin);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("Falha ao inicializar o display!");
        while (true); // Loop infinito em caso de falha
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Iniciando...");
    display.display();
    delay(2000);
    display.clearDisplay();
    display.display();
}

void DisplayService::PrintLine(uint8_t line, const char* texto) 
{
    if (line > 3) return; // Evita acessar linha inválida
    
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, line * 16);
    
    // Limpa a linha antes de desenhar o novo texto
    display.fillRect(0, line * 16, 128, 16, SSD1306_BLACK);
    
    display.print(texto);
    display.display();
}

void DisplayService::Clear()
{
    display.clearDisplay();
    display.display();
}