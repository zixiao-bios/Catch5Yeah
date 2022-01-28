#ifndef SOFTWARE_NETWORK_H
#define SOFTWARE_NETWORK_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <../ConfigPrivate.h>

void WiFiInit();

void printWifiList();

void WiFiConnect();

void httpGet(const String& url);

#endif //SOFTWARE_NETWORK_H
