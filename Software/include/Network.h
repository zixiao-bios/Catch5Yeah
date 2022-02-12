#ifndef SOFTWARE_NETWORK_H
#define SOFTWARE_NETWORK_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <../ConfigPrivate.h>
#include "Setup.h"


// network state
bool wifi_on_state();

bool wifi_connect_state();

String wifi_name_state();


// wifi operation
bool wifi_on();

void wifi_off();

bool wifi_connect(const String& wifi_name, const String& password);

void wifi_disconnect();


void printWifiList();
void httpGet(const String& url);

int scanWiFiList();

#endif //SOFTWARE_NETWORK_H
