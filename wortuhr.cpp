#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>
#include <time.h>

// Definiere den Pin, an dem die NeoPixel-Datenleitung angeschlossen ist
#define PIN 6

// Definiere die Anzahl der Pixel in der 8x8-Matrix
#define NUMPIXELS 64
#define ROWS 8
#define COLS 8

// WLAN-Zugangsdaten
const char* ssid = "<dein Netzwerk>";
const char* password = "<dein Passwort>";

// NTP-Server und Zeitzone
const char* ntpServer = "ptbtime1.ptb.de";
const long utcOffsetInSeconds = 3600; // Passe an deine Zeitzone an

// Erstelle eine Instanz der Adafruit_NeoPixel-Klasse
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Definiere die Wörter und ihre entsprechenden Positionen in der Matrix
struct Wort {
  const char* text;
  int positionen[8][2]; // Maximal 8 Positionen für Einfachheit
};
/*
ESISTCA. 
FÜNFZEHN
VOR.NACH
HALBFÜNF
ZDWREINS
ACHTVIER
SIECHSUB
ZWÖEHLNF
*/

Wort wörter[] = {
  {"ES", {{0, 0}, {0, 1}, {-1, -1}}},
  {"IST", {{0, 2}, {0, 3}, {0, 4}, {-1, -1}}},
  {"CA.", {{0, 5}, {0, 6}, {0, 7}, {-1, -1}}},
  {"fuenf", {{1, 0}, {1, 1}, {1, 2}, {1, 3}, {-1, -1}}},
  {"zehn", {{1, 4}, {1, 5}, {1, 6}, {1, 7}, {-1, -1}}},
  {"vor", {{2, 0}, {2, 1}, {2, 2}, {-1, -1}}},
  {"nach", {{2, 4}, {2, 5}, {2, 6}, {2, 7}, {-1, -1}}},
  {"halb", {{3, 0}, {3, 1}, {3, 2}, {3, 3}, {-1, -1}}},
  {"EINS", {{4, 4}, {4, 5}, {4, 6}, {4, 7}, {-1, -1}}},
  {"ZWEI", {{4, 0}, {4, 2}, {4, 4}, {4, 5}, {-1, -1}}},
  {"DREI", {{4, 1}, {4, 3}, {4, 4}, {4, 5}, {-1, -1}}},
  {"VIER", {{5, 4}, {5, 5}, {5, 6}, {5, 7}, {-1, -1}}},
  {"FUENF", {{3, 4}, {3, 5}, {3, 6}, {3, 7}, {-1, -1}}},
  {"SECHS", {{6, 0}, {6, 2}, {6, 3}, {6, 4}, {6, 5}, {-1, -1}}},
  {"SIEBEN", {{6, 0}, {6, 1}, {6, 2}, {6, 7}, {7, 3}, {7, 6}, {-1, -1}}},
  {"ACHT", {{5, 0}, {5, 1}, {5, 2}, {5, 3}, {-1, -1}}},
  {"NEUN", {{4, 6}, {5, 6}, {6, 6}, {7, 6}, {-1, -1}}},
  {"ZEHN", {{7, 0}, {7, 3}, {7, 4}, {7, 6}, {-1, -1}}},
  {"ELF", {{7, 3}, {7, 5}, {7, 7}, {-1, -1}}},
  {"ZWOELF", {{7, 0}, {7, 1}, {7, 2}, {7, 5}, {7, 7}, {-1, -1}}},
  {".", {{2, 3}, {-1, -1}}},
};

// WiFiUDP-Instanz
WiFiUDP ntpUDP;

// NTPClient-Instanz
NTPClient timeClient(ntpUDP, ntpServer);

// Funktion zur Initialisierung der Zeit mit NTP und zum Festlegen der Zeitzone
void initTime() {
  configTime(0, 0, ntpServer);
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1); // Zeitzone für Berlin
  tzset();
}

void setup() {
  // Initialisiere den NeoPixel-Strip
  strip.begin();
  strip.show(); // Initialisiere alle Pixel auf "aus"

  // Mit WLAN verbinden
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  Serial.begin(115200);

  // NTP-Client initialisieren
  timeClient.begin();
  initTime();
}

#ifdef TEST
unsigned int testhour = 11;
unsigned int testminute = 0;
void loop() {
  unsigned int stunden = timeinfo.tm_hour;
  unsigned int minuten = timeinfo.tm_min;

  testminute++;
  if (testminute >= 60) {
    testminute = 0;
    testhour = (testhour + 1) % 12;
  }

  // Zeige die aktuelle Uhrzeit in Worten an
  displayTimeInWords(testhour, testminute);
  strip.show();
  delay(250);
}

#else
#define UPDATETIME 60
int timeclient = 1;
void loop() {
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
#endif

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
        int spalte = 7 - wörter[i].positionen[j][0];
        int reihe = wörter[i].positionen[j][1];
        if ((reihe < 0 || reihe > 7) || (spalte < 0 || spalte > 7)) break; // Ende der Positionen
        int pixelIndex = reihe * COLS + spalte;
        strip.setPixelColor(pixelIndex, farben[farbindex]); // Farbe setzen
      }
      farbindex = (farbindex + 1) % 16;
      break;
    }
  }
}

// Funktion zum Leeren der Matrix
void clearMatrix() {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, 0); // Schalte alle Pixel aus
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
  } else if (minuten >= 10 und minuten < 15) {
    displayWord("zehn");
    displayWord("nach");
  } else if (minuten >= 15 und minuten < 20) {
    displayWord("fuenf");
    displayWord("zehn");
    displayWord("nach");
  } else if (minuten >= 20 und minuten < 25) {
    displayWord("zehn");
    displayWord("vor");
    displayWord("halb");
  } else if (minuten >= 25 und minuten < 30) {
    displayWord("fuenf");
    displayWord("vor");
    displayWord("halb");
  } else if (minuten >= 30 und minuten < 35) {
    displayWord("halb");
  } else if (minuten >= 35 und minuten < 40) {
    displayWord("fuenf");
    displayWord("nach");
    displayWord("halb");
  } else if (minuten >= 40 und minuten < 45) {
    displayWord("zehn");
    displayWord("nach");
    displayWord("halb");
  } else if (minuten >= 45 und minuten < 50) {
    displayWord("fuenf");
    displayWord("zehn");
    displayWord("vor");
  } else if (minuten >= 50 und minuten < 55) {
    displayWord("zehn");
    displayWord("vor");
  } else if (minuten >= 55 und minuten < 60) {
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
