#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>
#include <time.h>
#include <WebServer.h>

// Definiere den Pin, an dem die NeoPixel-Datenleitung angeschlossen ist
#define PIN 13

// Definiere die Anzahl der Pixel in der 16x16-Matrix
#define NUMPIXELS 256
#define ROWS 16
#define COLS 16

// WLAN-Zugangsdaten
const char* ssid = "<WLAN-SSID>";
const char* password = "<WLAN-Passwort>";

// NTP-Server und Zeitzone
const char* ntpServer = "pool.ntp.org";
const long utcOffsetInSeconds = 3600; // Passe an deine Zeitzone an

// Erstelle eine Instanz der Adafruit_NeoPixel-Klasse
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Webserver auf Port 80
WebServer server(80);

// LED-Einstellungen
int timeColor = strip.Color(30, 0, 0); // Standardfarbe für die Uhrzeit (Rot)
int backgroundColor = strip.Color(0, 30, 30); // Standardhintergrundfarbe (Cyan)
bool randomTextColor = false;
bool showSingleMinutes = false;

// Definiere die Wörter und ihre entsprechenden Positionen in der Matrix
struct Wort {
  const char* text;
  int positionen[8][2]; // Maximal 8 Positionen für Einfachheit
};

Wort wörter[] = {
  {"ES", {{0, 0}, {0, 1}, {-1, -1}}},
  {"IST", {{0, 3}, {0, 4}, {0, 5}, {-1, -1}}},
  {"MINUTEN", {{0, 7}, {0, 8}, {0, 9}, {0, 10}, {0, 11}, {0, 12}, {0, 13}, {-1, -1}}},
  {"ZWANZIG", {{12, 0}, {12, 1}, {12, 2}, {12, 3}, {12, 4}, {12, 5}, {12, 6}, {-1, -1}}},
  {"VIERZIG", {{13, 0}, {13, 1}, {13, 2}, {13, 3}, {13, 4}, {13, 5}, {13, 6}, {-1, -1}}},
  {"FUENF", {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {-1, -1}}},
  {"ZEHN", {{4, 0}, {4, 1}, {4, 2}, {4, 3}, {-1, -1}}},
  {"NACH", {{3, 5}, {3, 6}, {3, 7}, {3, 8}, {-1, -1}}},
  {"VOR", {{3, 9}, {3, 10}, {3, 11}, {-1, -1}}},
  {"HALB", {{5, 0}, {5, 1}, {5, 2}, {5, 3}, {-1, -1}}},
  {"EINS", {{6, 0}, {6, 1}, {6, 2}, {6, 3}, {-1, -1}}},
  {"ZWEI", {{7, 0}, {7, 1}, {7, 2}, {7, 3}, {-1, -1}}},
  {"DREI", {{8, 0}, {8, 1}, {8, 2}, {8, 3}, {-1, -1}}},
  {"VIER", {{9, 0}, {9, 1}, {9, 2}, {9, 3}, {-1, -1}}},
  {"FUENF", {{10, 0}, {10, 1}, {10, 2}, {10, 3}, {10, 4}, {-1, -1}}},
  {"SECHS", {{10, 6}, {10, 7}, {10, 8}, {10, 9}, {10, 10}, {-1, -1}}},
  {"SIEBEN", {{11, 0}, {11, 1}, {11, 2}, {11, 3}, {11, 4}, {11, 5}, {11, 6}, {-1, -1}}},
  {"ACHT", {{11, 8}, {11, 9}, {11, 10}, {11, 11}, {-1, -1}}},
  {"NEUN", {{12, 8}, {12, 9}, {12, 10}, {12, 11}, {-1, -1}}},
  {"ZEHN", {{13, 8}, {13, 9}, {13, 10}, {13, 11}, {-1, -1}}},
  {"ELF", {{14, 8}, {14, 9}, {14, 10}, {-1, -1}}},
  {"ZWOELF", {{15, 8}, {15, 9}, {15, 10}, {15, 11}, {15, 12}, {-1, -1}}},
  {"UHR", {{6, 5}, {6, 6}, {6, 7}, {-1, -1}}}
};

// WiFiUDP-Instanz
WiFiUDP ntpUDP;

// NTPClient-Instanz
NTPClient timeClient(ntpUDP, ntpServer);

// NTP Update Intervall (in Durchläufen)
#define UPDATETIME 3600  // Update Zeit jede Stunde
int timeclient = UPDATETIME;

// Funktion zur Initialisierung der Zeit mit NTP und zum Festlegen der Zeitzone
void initTime() {
  configTime(utcOffsetInSeconds, 0, ntpServer);
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1); // Zeitzone für Berlin
  tzset();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting WordClock...");
  
  // Initialisiere den NeoPixel-Strip
  strip.begin();
  Serial.println("NeoPixel initialized");
  
  // Mit WLAN verbinden
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Verbinde mit WLAN...");
  }

  Serial.println("WLAN verbunden.");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());

  // NTP-Client initialisieren
  timeClient.begin();
  initTime();

  // Webserver Routen
  server.on("/", HTTP_GET, handleRoot);
  server.on("/update", HTTP_GET, handleUpdate);
  server.on("/restart", HTTP_GET, handleRestart);
  server.on("/reset_wifi", HTTP_GET, handleResetWifi);
  server.on("/reset_settings", HTTP_GET, handleResetSettings);
  server.on("/changeTimeColor", HTTP_POST, handleChangeTimeColor);
  server.on("/changeBackgroundColor", HTTP_POST, handleChangeBackgroundColor);
  server.on("/toggleRandomTextColor", HTTP_POST, handleToggleRandomTextColor);
  server.on("/toggleShowSingleMinutes", HTTP_POST, handleToggleShowSingleMinutes);
  
  server.begin();
}

void handleRoot() {
    String html = "<!DOCTYPE html><html lang='de'>";
    html += "<head><meta charset='UTF-8'>";
    html += "<title>WordClock Einstellungen</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; background-color: #363636; color: #ffffff; text-align: center; padding: 20px; }";
    html += "h1 { color: #1c5c97; }";
    html += "h2 { color: #1c5c97; margin-top: 40px; }";
    html += "a { display: inline-block; margin: 10px; padding: 10px 20px; background-color: #1c5c97; color: white; text-decoration: none; border-radius: 5px; }";
    html += "a:hover { background-color: #174672; }";
    html += "p { font-size: 18px; }";
    html += "input[type='color'] { margin-left: 10px; }";
    html += "input[type='checkbox'] { margin-right: 10px; }";
    html += "</style></head><body>";

    html += "<h1>WordClock Einstellungen</h1>";
    html += "<p><a href='/update'>Aktualisierungsmodus aktivieren</a></p>";
    html += "<p><a href='/restart'>Neustart</a></p>";
    html += "<p><a href='/reset_wifi'>WLAN-Einstellungen zurücksetzen</a></p>";
    html += "<p><a href='/reset_settings'>WordClock-Einstellungen zurücksetzen</a></p>";

    html += "<h2>LED Einstellungen</h2>";
    html += "<p>Farbe der Uhrzeit: <input type='color' id='timeColor' value='#FF0000' onchange='changeTimeColor(this.value)'></p>";
    html += "<p>Hintergrundfarbe: <input type='color' id='backgroundColor' value='#00FFFF' onchange='changeBackgroundColor(this.value)'></p>";
    html += "<p><label><input type='checkbox' id='randomTextColor' onchange='toggleRandomTextColor()'> Zufällige Textfarbe jede Minute</label></p>";
    html += "<p><label><input type='checkbox' id='showSingleMinutes' onchange='toggleShowSingleMinutes()'> Einzelne Minuten anzeigen</label></p>";
    
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleUpdate() {
    server.send(200, "text/plain", "Update-Modus aktiviert");
}

void handleRestart() {
    server.send(200, "text/plain", "WordClock wird neu gestartet...");
    delay(1000);
    ESP.restart();
}

void handleResetWifi() {
    server.send(200, "text/plain", "WLAN-Einstellungen wurden zurückgesetzt");
    WiFi.disconnect(true);
    delay(1000);
    ESP.restart();
}

void handleResetSettings() {
    server.send(200, "text/plain", "WordClock-Einstellungen wurden zurückgesetzt");
}

void handleChangeTimeColor() {
    if (server.hasArg("color")) {
        String color = server.arg("color");
        long colorValue = strtol(color.substring(1).c_str(), NULL, 16);
        timeColor = strip.Color((colorValue >> 16) & 0xFF, (colorValue >> 8) & 0xFF, colorValue & 0xFF);
    }
    server.send(200, "text/plain", "Farbe der Uhrzeit geändert");
}

void handleChangeBackgroundColor() {
    if (server.hasArg("color")) {
        String color = server.arg("color");
        long colorValue = strtol(color.substring(1).c_str(), NULL, 16);
        backgroundColor = strip.Color((colorValue >> 16) & 0xFF, (colorValue >> 8) & 0xFF, colorValue & 0xFF);
    }
    server.send(200, "text/plain", "Hintergrundfarbe geändert");
}

void handleToggleRandomTextColor() {
    randomTextColor = !randomTextColor;
    server.send(200, "text/plain", "Zufällige Textfarbe geändert");
}

void handleToggleShowSingleMinutes() {
    showSingleMinutes = !showSingleMinutes;
    server.send(200, "text/plain", "Anzeige der einzelnen Minuten geändert");
}

void loop() {
  server.handleClient();  // Wichtig: Webserver-Anfragen verarbeiten
  
  // Aktualisiere den NTP-Client
  unsigned long start_millis = millis();
  if (--timeclient <= 0) {
    Serial.println("Zeit wird aktualisiert");
    timeClient.update();
    Serial.println("Fertig");
    timeclient = UPDATETIME;
  }

  // Hole die aktuelle Zeit
  time_t jetzt;
  struct tm timeinfo;
  time(&jetzt);
  localtime_r(&jetzt, &timeinfo);

  unsigned int stunden = timeinfo.tm_hour;
  unsigned int minuten = timeinfo.tm_min;

  // Zeige die aktuelle Uhrzeit in Worten an
  displayTimeInWords(stunden, minuten);
  strip.show();

  Serial.print(stunden);
  Serial.print(":");
  Serial.println(minuten);
  delay(1000 - (millis() - start_millis));
}

uint32_t farben[] = {
  strip.Color(30, 0, 0),    // Rot
  strip.Color(0, 30, 0),    // Grün
  strip.Color(0, 0, 30),    // Blau
  strip.Color(30, 30, 0),   // Gelb
  strip.Color(0, 30, 30),   // Cyan
  strip.Color(30, 0, 30),   // Magenta
  strip.Color(15, 0, 15),   // Lila
  strip.Color(0, 15, 15),   // Türkis
  strip.Color(15, 15, 0),   // Olive
  strip.Color(30, 23, 24),  // Pink
  strip.Color(0, 15, 30),   // Grau
  strip.Color(0, 0, 15),    // Marineblau
  strip.Color(30, 20, 0),   // Orange
  strip.Color(15, 0, 0),    // Kastanienbraun
  strip.Color(0, 15, 0),    // Dunkelgrün
  strip.Color(30, 30, 30)   // Weiß
};
int farbindex = 0;

// Funktion zum Anzeigen eines Wortes auf der Matrix
void displayWord(const char* wort, bool inc_color = true) {
  for (unsigned int i = 0; i < sizeof(wörter) / sizeof(wörter[0]); i++) {
    if (strcmp(wörter[i].text, wort) == 0) {
      for (int j = 0; j < 8; j++) {
        int reihe = wörter[i].positionen[j][0];
        int spalte = wörter[i].positionen[j][1];
        if ((reihe < 0 || reihe >= ROWS) || (spalte < 0 || spalte >= COLS)) break; // Ende der Positionen
        int pixelIndex = reihe * COLS + spalte;
        strip.setPixelColor(pixelIndex, randomTextColor ? farben[random(0, 16)] : farben[farbindex]); // Farbe setzen
      }
      if (inc_color) {
        farbindex = (farbindex + 1) % 16;
      }
      break;
    }
  }
}

// Funktion zum Leeren der Matrix
void clearMatrix() {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, backgroundColor); // Setze alle Pixel auf Hintergrundfarbe
  }
}

bool punkt = false;

// Funktion zur Anzeige der Uhrzeit in Worten
void displayTimeInWords(unsigned int stunden, unsigned int minuten) {
  farbindex = minuten % 16;
  clearMatrix();
  displayWord("ES");
  displayWord("IST");
  punkt = !punkt;
  if (punkt) {
    displayWord(".");
  } else {
    farbindex = (farbindex + 1) % 16;
  }

  if (minuten > 0 && minuten < 5) {
    displayWord("CA.");
  } else if (minuten >= 5 && minuten < 10) {
    displayWord("fuenf");
    displayWord("nach");
  } else if (minuten >= 10 && minuten < 15) {
    displayWord("zehn");
    displayWord("nach");
  } else if (minuten >= 15 && minuten < 20) {
    displayWord("fuenf");
    displayWord("zehn");
    displayWord("nach");
  } else if (minuten >= 20 && minuten < 25) {
    displayWord("zehn");
    displayWord("vor");
    displayWord("halb");
  } else if (minuten >= 25 && minuten < 30) {
    displayWord("fuenf");
    displayWord("vor");
    displayWord("halb");
  } else if (minuten >= 30 && minuten < 35) {
    displayWord("halb");
  } else if (minuten >= 35 && minuten < 40) {
    displayWord("fuenf");
    displayWord("nach");
    displayWord("halb");
  } else if (minuten >= 40 && minuten < 45) {
    displayWord("zehn");
    displayWord("nach");
    displayWord("halb");
  } else if (minuten >= 45 && minuten < 50) {
    displayWord("fuenf");
    displayWord("zehn");
    displayWord("vor");
  } else if (minuten >= 50 && minuten < 55) {
    displayWord("zehn");
    displayWord("vor");
  } else if (minuten >= 55 && minuten < 60) {
    displayWord("fuenf");
    displayWord("vor");
  }

  if (minuten >= 20) {
    stunden = (stunden + 1) % 12;
  }

  switch (stunden) {
    case 1:
      displayWord("EINS");
      break;
    case 2:
      displayWord("ZWEI");
      break;
    case 3:
      displayWord("DREI");
      break;
    case 4:
      displayWord("VIER");
      break;
    case 5:
      displayWord("FUENF");
      break;
    case 6:
      displayWord("SECHS");
      break;
    case 7:
      displayWord("SIEBEN");
      break;
    case 8:
      displayWord("ACHT");
      break;
    case 9:
      displayWord("NEUN");
      break;
    case 10:
      displayWord("ZEHN");
      break;
    case 11:
      displayWord("ELF");
      break;
    case 12:
    case 0:
      displayWord("ZWOELF");
      break;
  }
}
