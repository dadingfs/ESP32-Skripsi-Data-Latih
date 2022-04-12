#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Keypad.h>

// Buzzer
#define buzzer 13

// Keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {12, 14, 27, 26}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {25, 33, 32, 35}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Wifi
WiFiMulti wifiMulti;

String listSSID[6] = {"AP1", "AP2", "AP3", "AP4", "AP5", "AP6"};
int RSSIWifi[6] = {0, 0, 0, 0, 0, 0};

void scanWifi()
{
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 6; j++)
            if (WiFi.SSID(i) == listSSID[j])
            {
                RSSIWifi[j] = WiFi.RSSI(i);
            }
    }
}

// Accessed Google Script Settings
const char *APP_SERVER = "script.google.com";
const char *KEY = "AKfycbymONBRL88A0yPpcBoD-JovH_fiTTkxRy1X-dqN4er3Da9-SFqogIViPOTAyp9PBar80Q";

// SpreadSheet
void uploadData(int ap1, int ap2, int ap3, int ap4, int ap5, int ap6, int ruangan)
{
    HTTPClient http;
    String URL = "https://script.google.com/macros/s/";

    URL += KEY;
    URL += "/exec?";
    URL += "ap1=";
    URL += ap1;
    URL += "&ap2=";
    URL += ap2;
    URL += "&ap3=";
    URL += ap3;
    URL += "&ap4=";
    URL += ap4;
    URL += "&ap5=";
    URL += ap5;
    URL += "&ap6=";
    URL += ap6;
    URL += "&ruangan=";
    URL += ruangan;

    Serial.println("[HTTP] begin...");
    Serial.println(URL);
    // access to your Google Sheets
    Serial.println();
    // configure target server and url
    http.begin(URL);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    Serial.println("[HTTP] GET...");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0)
    {
        // HTTP header has been send and Server response header has been handled
        Serial.print("[HTTP] GET... code: ");
        Serial.println(httpCode);

        // file found at server
        String payload = http.getString();
        Serial.println(payload);
    }
    else
    {
        Serial.print("[HTTP] GET... failed, error: ");
        Serial.println(http.errorToString(httpCode).c_str());
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(buzzer, OUTPUT);

    wifiMulti.addAP("AP1", "123456789");
    wifiMulti.addAP("AP2", "123456789");
    wifiMulti.addAP("AP3", "123456789");
    wifiMulti.addAP("AP4", "123456789");
    wifiMulti.addAP("AP5", "123456789");
    wifiMulti.addAP("AP6", "123456789");

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(500);
}

void loop()
{
    int ruangan = 0;
    char key = keypad.getKey();

    if (key == '0')
    {
        ruangan = 10;
    }
    else
    {
        ruangan = key - '0';
    }
    if (ruangan == -48)
    {
        ruangan = 0;
    }

    if (ruangan != 0)
    {
        scanWifi();
        if (wifiMulti.run() == WL_CONNECTED)
        {
            Serial.println(WiFi.SSID());
            uploadData(RSSIWifi[0], RSSIWifi[1], RSSIWifi[2], RSSIWifi[3], RSSIWifi[4], RSSIWifi[5], ruangan);
            WiFi.disconnect();
            ruangan = 0;
            Serial.println(ruangan);
            digitalWrite(buzzer, HIGH);
            delay(500);
            digitalWrite(buzzer, LOW);
        }
    }
}