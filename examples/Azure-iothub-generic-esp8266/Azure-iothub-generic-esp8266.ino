#include <GenericAzureIoTHub.h>

#include <ArduinoJson.h>

void setup() {
  Serial.begin(115200);
  WiFi.begin("SSID", "PASS");
  Azure.begin("Your Key"); //YourKey Example:"HostName=YourHost.azure-devices.net;DeviceId=YourDevice;SharedAccessKey="
  Azure.setCallback(azureCallback);
}

void azureCallback(const byte* payload, int length) {
  if (length > 0) {
    int command = payload[0];

    if (command >= 42) {
      Serial.println("Machine repaired...");
    }
    else {
      Serial.println("Machine not repaired...");
    }
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Azure.connect();

    StaticJsonBuffer<256> jsonBuffer;

    JsonObject& root = jsonBuffer.createObject();
    root["sensor"] = "sensor1";
    root["value"] = 42;

    char buffer[256];
    root.printTo(buffer, sizeof(buffer));

    Azure.push(buffer);
    
    Serial.println("pushed");
    delay(2000);
  } else {
    Serial.println("Not connected to the Internet");
    delay(250);
  }
}


