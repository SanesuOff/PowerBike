// ! Include
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

Adafruit_ADS1115 ads;

const char* wifi_network_ssid     = "Wifi_SSID";
const char* wifi_network_password =  "";

const char* soft_ap_ssid          = "ESP_Wifi";
const char* soft_ap_password      = "9839*-56m";

const int led = 2;
const int analogv = 34;

AsyncWebServer server(80);

void setup() 
{
    //------------------------------------Serial
    Serial.begin(9600);
    //while(!Serial){}  //A retirer a la fin du dev
    Serial.println("\n");

    //------------------------------------ADS
    ads.setGain(GAIN_TWOTHIRDS); 

    if (!ads.begin())
    {
        Serial.println("Failed to initialize ADS.");
        while (1);
    }

    //------------------------------------GPIO
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
    pinMode(analogv, INPUT);

    //------------------------------------SPIFFS
    if(!SPIFFS.begin())
    {
        Serial.println("Erreur SPIFFS...");
        return;
    }

    File root = SPIFFS.open("/");
    File file = root.openNextFile();

    while(file)
    {
        Serial.print("File: ");
        Serial.println(file.name());
        file.close();
        file = root.openNextFile();
    }

    //------------------------------------WIFI
    
    WiFi.mode(WIFI_AP_STA);

    Serial.println("\n[*] Creation du Point d'accès ESP32");
    WiFi.softAP(soft_ap_ssid, soft_ap_password);
    Serial.print("[+] Point d'accès crée ayant pour passerelle ");
    Serial.println(WiFi.softAPIP());

    WiFi.begin(wifi_network_ssid, wifi_network_password);
    Serial.println("\n[*] Connexion au réseau WiFi");

    while(WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }

    Serial.print("\n[+] Connecté au réseau WiFi avec l'IP locale : ");
    Serial.println(WiFi.localIP());
    
    //------------------------------------SERVER
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/w3.css", "text/css");
    });

    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/script.js", "text/javascript");
    });

    server.on("/read-analogv", HTTP_GET, [](AsyncWebServerRequest *request)
    {

        int16_t adc;
        float volts;
        int value;
        adc = ads.readADC_SingleEnded(0);
        volts = ads.computeVolts(adc);
        value = map(adc, 0, 26920, 0, 32765);
        
        Serial.print("AIN0-cor: "); Serial.print(value); Serial.print("  "); Serial.print(volts); Serial.println("V");
    
        int val = value;
        String analogv = String(val);
        request->send(200, "text/plain", analogv);
    });

    server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        digitalWrite(led, HIGH);
        request->send(200);
    });

    server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        digitalWrite(led, LOW);
        request->send(200);
    });

    server.begin();
    Serial.println("Serveur actif!");
}

void loop() 
{
    int16_t adc;
    float volts;
    int value;
    adc = ads.readADC_SingleEnded(0);
    volts = ads.computeVolts(adc);
    value = map(adc, 0, 26920, 0, 32765);
        
    Serial.print("AIN0-cor: "); Serial.print(value); Serial.print("  "); Serial.print(volts); Serial.println("V");
}
