// ! Include
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <WiFiClientSecure.h>
#include <WebSocketsServer.h>

Adafruit_ADS1115 ads;

// * Wifi to connect
const char* wifi_network_ssid     = "Sanesu's Box";
const char* wifi_network_password =  "";

// * Wifi to create
const char* soft_ap_ssid          = "ESP_Wifi";
const char* soft_ap_password      = "9839*-56m";

const int led = 2;
const int analogv = 39;

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);  // WebSocket server on port 81

void setup() 
{
    //------------------------------------Serial
    Serial.begin(115200);
    while(!Serial){}  //A retirer a la fin du dev
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

    WiFi.begin(wifi_network_ssid, wifi_network_password);
    Serial.println("\n[!] Connexion au réseau WiFi");

    while(WiFi.status() != WL_CONNECTED)
    {

        String ssid = String(wifi_network_ssid);
        Serial.println(""); Serial.print("[!]Recherche de '"); Serial.print(ssid); Serial.print("' en cours ...");
        delay(10000);
    }

    Serial.println(""); Serial.print("\n[+] Connecté au réseau WiFi avec l'IP locale : ");
    Serial.println(WiFi.localIP());

    Serial.println("\n[!] Creation du Point d'accès ESP32");
    WiFi.softAP(soft_ap_ssid, soft_ap_password);
    Serial.print("[+] Point d'accès crée ayant pour passerelle :");
    Serial.println(WiFi.softAPIP());
    
    //------------------------------------SERVER
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    server.on("/base.css", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/base.css", "text/css");
    });

    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/script.js", "text/javascript");
    });
    server.on("/chart.js", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/chart.js", "text/javascript");
    });

    /*server.on("/read-analogv", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        //Serial.print("AIN0-cor: "); Serial.print(value); Serial.print("  "); Serial.print(volts); Serial.println("V");
        int16_t adc;
        float volts;
        int value;
        adc = ads.readADC_SingleEnded(0);
        volts = ads.computeVolts(adc);
        value = map(adc, 0, 3.3, 0, 16382);

        if (value < 0) {
            value = 0;
        }else if (value > 32765) {
            value = 32765;
        }

        int val = value;
        String analogv = String(val);
        request->send(200, "text/plain", analogv);
    });*/

    server.on("/ecg", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        int16_t adc;
        int value;
        adc = ads.readADC_SingleEnded(0);
        value = adc;
    
        String analval = String(value);
        
        request->send(200, "text/plain", analval);
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

void loop() {
    /*int16_t adc;
    int value;
    adc = ads.readADC_SingleEnded(0);
    value = adc;
    
    Serial.print(">ECG: ");
    Serial.println(value);*/
    
    //Compter le temps entre deux valeur(adc) en dessous de 2000 afins de déterminer le nombre de ms ou s pour 1 battemenr pour ensuite répéter cette action 6 fois et pouvoir faire *10 pour avoir le nombre de battement par minute
}