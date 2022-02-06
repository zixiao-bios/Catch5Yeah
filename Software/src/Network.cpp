#include "Network.h"

bool wifi_on_state() {
    return WiFiClass::getMode() == WIFI_STA;
}

bool wifi_connect_state() {
    return WiFiClass::status() == WL_CONNECTED;
}

String wifi_name_state() {
    return WiFi.SSID();
}

bool wifi_on() {
    WiFiClass::mode(WIFI_STA);

    int n = scanWiFiList();
    for (int i = 0; i < n; ++i) {
        if (WiFi.SSID(i) == WIFI_SSID) {
            return wifi_connect(WIFI_SSID, WIFI_PASSWORD);
        }
    }
    return false;
}

void wifi_off() {
    WiFi.disconnect(true);
}

bool wifi_connect(const String& wifi_name, const String& password) {
    WiFi.begin(wifi_name.c_str(), password.c_str());
    for (int i = 0; i < 20; ++i) {
        delay(500);
        if (wifi_connect_state()) {
            return true;
        }
    }
    return wifi_connect_state();
}

void wifi_disconnect() {
    WiFi.disconnect();
}

void printWifiList() {
    Serial.println("scan start");
    int n = WiFi.scanNetworks();
    Serial.println("snan done");

    if (n == 0) {
        Serial.println("no WiFi found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");

        for (int i = 0; i < n; ++i) {
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
            delay(10);
        }
    }
    Serial.println("=======================================\n\n");
}

int scanWiFiList() {
    return WiFi.scanNetworks();
}

void httpGet(const String &url) {
    HTTPClient http;

    Serial.println("[HTTP] begin...");
    http.begin(url);

    Serial.println("[HTTP] GET...");
    int httpCode = http.GET();

    if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println(payload);
        }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}