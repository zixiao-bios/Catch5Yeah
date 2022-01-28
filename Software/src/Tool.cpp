#include "Tool.h"

void listSpiffsFile() {
    Serial.println("============ list SPIFFS file ============");

    File root = SPIFFS.open("/");
    if (!root) {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        Serial.print(file.name());
        Serial.print("\tSIZE: ");
        Serial.println(file.size());
        file = root.openNextFile();
    }
    Serial.println("============ list file done ============");
}
