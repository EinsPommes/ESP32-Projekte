# WordClock ESP32 Installationsanleitung

Diese Anleitung führt dich durch die Schritte, die notwendig sind, um die WordClock auf einem ESP32 einzurichten und auszuführen.

## Anforderungen

- ESP32 Mikrocontroller
- 8x8 NeoPixel (WS2812) LED-Matrix
- Adafruit NeoPixel Bibliothek
- ESPAsyncWebServer Bibliothek
- NTPClient Bibliothek
- Arduino IDE auf deinem Computer installiert
- WLAN-Netzwerk-Zugangsdaten

## Installationsschritte

### Schritt 1: Einrichtung der Arduino IDE

1. **Arduino IDE installieren**: Lade die neueste Version der Arduino IDE von [arduino.cc](https://www.arduino.cc/en/software) herunter und installiere sie.

2. **ESP32 Board zur Arduino IDE hinzufügen**:
   - Öffne die Arduino IDE und navigiere zu `Datei > Voreinstellungen`.
   - Füge im Feld "Zusätzliche Boardverwalter-URLs" folgende URL hinzu: `https://dl.espressif.com/dl/package_esp32_index.json`.
   - Gehe zu `Werkzeuge > Board > Boardverwalter`, suche nach "ESP32" und installiere das Board-Paket.

### Schritt 2: Installation der benötigten Bibliotheken

Installiere die folgenden Bibliotheken über den Arduino Bibliotheksverwalter (`Sketch > Bibliothek einbinden > Bibliotheken verwalten`):

- **Adafruit NeoPixel**
- **ESPAsyncWebServer**
- **NTPClient**

Einige Bibliotheken müssen eventuell manuell installiert werden, wenn sie nicht im Bibliotheksverwalter verfügbar sind. Schaue dazu auf den GitHub-Seiten der jeweiligen Bibliotheken nach.

### Schritt 3: Hardware verbinden

- Verbinde die **8x8 NeoPixel-Matrix** mit dem ESP32:
  - **Daten-Pin**: Verbinde ihn mit GPIO 6 des ESP32.
  - **Stromversorgung (5V)**: Verbinde sie mit einer geeigneten Stromquelle (sicherstellen, dass die Stromquelle zu den Anforderungen der LED-Matrix passt).
  - **Masse (GND)**: Verbinde sie mit dem GND-Pin des ESP32.

### Schritt 4: Code anpassen

1. **WLAN-Zugangsdaten**:
   - Öffne die bereitgestellte Code-Datei in der Arduino IDE.
   - Ersetze `const char* ssid = "<dein Netzwerk>";` und `const char* password = "<dein Passwort>";` durch den Namen und das Passwort deines WLAN-Netzwerks.

2. **Kompilieren und Hochladen**:
   - Wähle das richtige ESP32 Board und den richtigen Port in der Arduino IDE aus.
   - Klicke auf den **Upload**-Button, um den Code auf den ESP32 zu flashen.

### Schritt 5: Die Uhr betreiben

1. **Einschalten**: Sobald der Code hochgeladen ist, schalte den ESP32 ein. Die NeoPixel-Matrix zeigt während des Bootvorgangs "CHILL ZONE" an.
2. **Weboberfläche aufrufen**:
   - Finde die IP-Adresse heraus, die im Seriellen Monitor angezeigt wird, nachdem der ESP32 sich mit deinem WLAN verbunden hat.
   - Öffne einen Webbrowser und gib die IP-Adresse ein, um die Einstellungen der WordClock aufzurufen.

### Schritt 6: LED-Einstellungen konfigurieren

- Du kannst die Farbe des Textes und des Hintergrunds über die Weboberfläche ändern.
- Aktiviere oder deaktiviere zufällige Textfarben und zeige die exakten Minuten mit den Umschaltknöpfen an.

## Fehlerbehebung

- **Verbindung zum WLAN nicht möglich**: Stelle sicher, dass die WLAN-Zugangsdaten korrekt sind und der ESP32 sich im Bereich des Netzwerks befindet.
- **Keine LED-Reaktion**: Überprüfe, ob die NeoPixel-Matrix korrekt mit dem richtigen GPIO-Pin verbunden ist und ob die Stromversorgung ausreicht.
- **Weboberfläche nicht zugänglich**: Stelle sicher, dass du mit demselben WLAN-Netzwerk wie der ESP32 verbunden bist und die richtige IP-Adresse verwendest.

## Helfer 
- [EinsPommes](https://chill-zone.xyz)
- [TypTech](https://typtech.de)

## Lizenz

Dieses Projekt ist nur für den nicht-kommerziellen Gebrauch lizenziert. Du darfst es ohne Genehmigung nicht für kommerzielle Zwecke verwenden.

