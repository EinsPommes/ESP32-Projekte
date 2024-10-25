#include <WiFi.h>
#include <WiFiClient.h>

const char* ssid = "dein_SSID";
const char* password = "dein_Passwort";
const char* dptServer = "time.nist.gov"; // Beispiel DPT-Server
const int dptPort = 13;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Verbinde mit WiFi");
  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 10000; // 10 Sekunden Timeout

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    delay(1000);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi verbunden.");
    Serial.print("IP-Adresse: ");
    Serial.println(WiFi.localIP());
    connectToDptServer();
  } else {
    Serial.println();
    Serial.println("WiFi-Verbindung fehlgeschlagen.");
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Verbindung verloren, versuche erneut zu verbinden...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    unsigned long reconnectAttemptTime = millis();
    const unsigned long reconnectTimeout = 10000; // 10 Sekunden Timeout

    while (WiFi.status() != WL_CONNECTED && millis() - reconnectAttemptTime < reconnectTimeout) {
      delay(1000);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("WiFi erneut verbunden.");
      Serial.print("IP-Adresse: ");
      Serial.println(WiFi.localIP());
      connectToDptServer();
    } else {
      Serial.println();
      Serial.println("Erneute Verbindung fehlgeschlagen.");
    }
  }
  delay(10000); // 10 Sekunden Pause, bevor erneut geprüft wird
}

void connectToDptServer() {
  Serial.print("Verbinde mit DPT-Server: ");
  Serial.println(dptServer);

  if (client.connect(dptServer, dptPort)) {
    Serial.println("Verbunden mit DPT-Server.");
    while (client.connected() && client.available() == 0) {
      delay(100);
    }
    while (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
    client.stop();
    Serial.println("Verbindung zum DPT-Server geschlossen.");
  } else {
    Serial.println("Verbindung zum DPT-Server fehlgeschlagen.");
  }
}

