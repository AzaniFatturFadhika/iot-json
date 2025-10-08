#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

WiFiClient wifiClient;

// Variabel global
const char* ssid = "INCODE";
const char* password = "12345678";
int pinLED = 5;
int pinPompa = 4;
#define ON HIGH
#define OFF LOW
// Static IP configuration
IPAddress local_IP(192, 168, 137, 160);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  // Konfigurasi Modul
  Serial.begin(115200);
  // Configure static IP
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  connectToWiFi();

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Deklarasi PIN GPIO
  pinMode(pinLED, OUTPUT);
  pinMode(pinPompa, OUTPUT);
}

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void loop() {
  // Cek koneksi WiFi. Jika terputus, coba sambungkan kembali.
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi disconnected. Reconnecting...");
    connectToWiFi();
    // Kembali ke awal loop setelah mencoba koneksi ulang
    return;
  }

  // Buat instance HTTPClient di dalam scope yang membutuhkannya
  // untuk memastikan sumber daya dibebaskan dengan benar.
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    Serial.print("[HTTP] begin...\n");
    //IP menuju ke server web
    http.begin(wifiClient, "http://192.168.137.1/iot-json/sensorMultiJsonEvaluasi.php");

    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();

    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      
      if (httpCode == HTTP_CODE_OK) {
        // Lebih efisien untuk deserialize langsung dari stream
        // Ini menghindari alokasi String besar untuk menampung seluruh respons.
        // String answer = http.getString();

        // Allocate the JSON document
        // Use https://arduinojson.org/v6/assistant to compute the capacity.
        StaticJsonDocument<256> doc;

        // Deserialize JSON dari stream HTTP
        DeserializationError error = deserializeJson(doc, http.getStream());

        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }

        // (Opsional) Cetak JSON yang berhasil di-parse untuk debugging
        Serial.println("JSON parsed successfully:");
        // serializeJsonPretty(doc, Serial);
        // Serial.println();

        // Parse Lampu
        JsonObject lampu = doc["Lampu"];
        const char* lampu_id = lampu["idSensor"];
        const char* lampu_nama = lampu["namaSensor"];
        const char* lampu_status = lampu["statusSensor"];

        // Parse Pompa
        JsonObject pompa = doc["Pompa"];
        const char* pompa_id = pompa["idSensor"];
        const char* pompa_nama = pompa["namaSensor"];
        const char* pompa_status = pompa["statusSensor"];

        // Parse NilaiSensor
        JsonObject nilai = doc["NilaiSensor"];
        int suhu = nilai["suhu"];
        int kelembapan = nilai["kelembapan"];
        int cahaya = nilai["cahaya"];

        // Display all data
        Serial.println(F("Lampu:"));
        Serial.printf("ID Sensor: %s\n", lampu_id);
        Serial.printf("Nama Sensor: %s\n", lampu_nama);
        Serial.printf("Status Sensor: %s\n", strcmp(lampu_status, "1") == 0 ? "Hidup" : "Mati");

        Serial.println(F("Pompa:"));
        Serial.printf("ID Sensor: %s\n", pompa_id);
        Serial.printf("Nama Sensor: %s\n", pompa_nama);
        Serial.printf("Status Sensor: %s\n", strcmp(pompa_status, "1") == 0 ? "Hidup" : "Mati");

        Serial.println(F("NilaiSensor:"));
        Serial.print("Suhu: ");
        Serial.println(suhu);
        Serial.print("Kelembapan: ");
        Serial.println(kelembapan);
        Serial.print("Cahaya: ");
        Serial.println(cahaya);

        // Control LEDs based on status
        digitalWrite(pinLED, strcmp(lampu_status, "1") == 0 ? HIGH : LOW);
        digitalWrite(pinPompa, strcmp(pompa_status, "1") == 0 ? HIGH : LOW);

      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
    }
    http.end();
  }

  // Delay sebelum iterasi loop berikutnya
  delay(1000);
}
