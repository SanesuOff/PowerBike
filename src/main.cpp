// ! Include
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <WiFiClientSecure.h>
#include <SPI.h>

Adafruit_ADS1115 ads;

const int led = 2;

// Décommenter et utiliser le serveur asynchrone
AsyncWebServer server(80);

// Utiliser uniquement le mode Station (pas AP)
const char *wifi_network_ssid = "Wifi SIN";
const char *wifi_network_password = "";

const int hallSensorPin = 27;
volatile unsigned long lastPulseTime = 0;
volatile unsigned long pulseInterval = 0;
volatile bool newMeasurement = false;

// Constantes pour les calculs
const int MAGNETS_PER_REVOLUTION = 9; // Nombre d'aimants positifs (18 total / 2)
const float RADIUS_METERS = 0.1; // Rayon en mètres (à ajuster selon votre système)

void IRAM_ATTR handleHallSensor() {
    unsigned long currentTime = micros();
    pulseInterval = currentTime - lastPulseTime;
    lastPulseTime = currentTime;
    newMeasurement = true;
}

void setup()
{
    Serial.begin(9600);
    
    // Configuration de l'ADS1115
    ads.setGain(GAIN_TWOTHIRDS);
    if (!ads.begin()) {
        Serial.println("Échec de l'initialisation ADS.");
        while (1);
    }

    // Configuration du capteur a effet hall
    pinMode(hallSensorPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(hallSensorPin), handleHallSensor, RISING);

    // Initialisation du SPIFFS
    if (!SPIFFS.begin()) {
        Serial.println("Erreur SPIFFS...");
        return;
    }

    // Configuration du WiFi en mode Station
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_network_ssid, wifi_network_password);
    Serial.println("\nConnexion au réseau WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\nConnecté à l'IP: ");
    Serial.println(WiFi.localIP());

    // Configuration des routes du serveur
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html");
    });
    
    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
        int16_t adc = ads.readADC_SingleEnded(0);
        String valeur = String(adc);
        request->send(200, "text/plain", valeur);
    });
    
    server.begin();
}

void loop()
{
    if (newMeasurement && pulseInterval > 0) {
        // Calcul de la vitesse
        float rps = 1000000.0 / (pulseInterval * MAGNETS_PER_REVOLUTION); // Tours par seconde
        float rpm = rps * 60.0; // Tours par minute
        float mps = rps * (2 * PI * RADIUS_METERS); // Mètres par seconde

        Serial.print("Vitesse: ");
        Serial.print(rpm, 2); // 2 décimales
        Serial.print(" tpm | ");
        Serial.print(mps, 2);
        Serial.println(" m/s");

        newMeasurement = false;
    }

    /* Système de Plotter via Cursor */
    // int16_t adc;
    // int value;mm
    // adc = ads.readADC_SingleEnded(0);
    // value = adc;
    // Serial.print(">ECG: "); Serial.println(value);
}