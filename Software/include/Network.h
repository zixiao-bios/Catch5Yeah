#ifndef SOFTWARE_NETWORK_H
#define SOFTWARE_NETWORK_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <../ConfigPrivate.h>

// network state
extern bool wifi_on_state;

bool wifi_connect_state();

String wifi_name_state();

// network init
void WiFiInit();

void printWifiList();

bool WiFiConnect(const String& wifi_name, const String& password);

void httpGet(const String& url);

int scanWiFiList();

#endif //SOFTWARE_NETWORK_H
