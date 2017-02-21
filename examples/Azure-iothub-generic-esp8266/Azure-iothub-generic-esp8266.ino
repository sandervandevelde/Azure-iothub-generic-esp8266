#include <GenericAzureIoTHub.h>
#include <ArduinoJson.h>

void setup() {
  Serial.begin(115200);
  WiFi.begin("Network ssid", "Network password");
  Azure.begin("IoTHub Device Connection string"); //eg. "HostName=[IoTHub name].azure-devices.net;DeviceId=[DeviceId];SharedAccessKey=[key]"
  Azure.setCallback(azureCallback);
}

// Receive bytes. Check the Ascii table to receive a char array (string)
void azureCallback(const byte* payload, int length) {
  if (length > 0) {
    int command = payload[0];

    if (command >= 42) {
      Serial.println("Do this...");
    }
    else {
      Serial.println("Do that...");
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
    
    Serial.println("Sent...");
    delay(5000);
  } else {
    Serial.println("No internet connection");
    delay(1000);
  }
}
