#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// --- Konfigurasi Jaringan ---
const char* ssid = "INCODE";
const char* password = "12345678";

// --- Konfigurasi IP Statis ---
IPAddress staticIP(192, 168, 137, 50);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);

// --- Konfigurasi Server & Pin ---
const char* serverUrl = "http://192.168.137.1/iot-json/sensorMultiJson.php";
const int pinLED = GPIO_NUM_5;

void setup() {
    Serial.begin(115200);
    pinMode(pinLED, OUTPUT);

    Serial.println("Menghubungkan ke WiFi...");

    // Konfigurasi IP statis sebelum memulai WiFi
    if (!WiFi.config(staticIP, gateway, subnet)) {
        Serial.println("Gagal mengonfigurasi IP statis");
    }

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi terhubung!");
    Serial.print("Alamat IP: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    // Cek koneksi WiFi sebelum melakukan request
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        
        Serial.println("\n[HTTP] Memulai koneksi...");
        http.begin(serverUrl);

        Serial.println("[HTTP] Mengirim request GET...");
        int httpCode = http.GET();

        if (httpCode > 0) {
            Serial.printf("[HTTP] Kode respons: %d\n", httpCode);

            if (httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println("Payload diterima:");
                Serial.println(payload);

                // Kapasitas JSON disarankan dihitung menggunakan ArduinoJson Assistant
                StaticJsonDocument<256> doc;

                DeserializationError error = deserializeJson(doc, payload);

                if (error) {
                    Serial.print("deserializeJson() gagal: ");
                    Serial.println(error.c_str());
                } else {
                    // Ekstrak data dengan aman
                    const char* idSensor = doc["idSensor"] | "N/A";
                    const char* namaSensor = doc["namaSensor"] | "N/A";
                    int statusSensor = doc["statusSensor"] | -1; // Default -1 jika tidak ada

                    Serial.println("\n--- Data JSON ---");
                    Serial.printf("ID Sensor: %s\n", idSensor);
                    Serial.printf("Nama Sensor: %s\n", namaSensor);
                    Serial.printf("Status Sensor: %d\n", statusSensor);

                    // Contoh mengambil data dari nested object 'add'
                    if (doc.containsKey("add")) {
                        int add_0 = doc["add"]["0"] | 0;
                        int add_1 = doc["add"]["1"] | 0;
                        Serial.printf("Nilai add[0]: %d\n", add_0);
                        Serial.printf("Nilai add[1]: %d\n", add_1);
                    }

                    // Logika untuk mengontrol LED berdasarkan status
                    if (statusSensor == 1) {
                        digitalWrite(pinLED, HIGH);
                        Serial.println("LED ON");
                    } else if (statusSensor == 0) {
                        digitalWrite(pinLED, LOW);
                        Serial.println("LED OFF");
                    }
                }
            }
        } else {
            Serial.printf("[HTTP] Request GET gagal, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    } else {
        Serial.println("Koneksi WiFi terputus. Mencoba menyambung kembali...");
        WiFi.reconnect();
    }

    // Delay sebelum request berikutnya
    delay(5000);
}
