
#ifndef GenericAzureIoTHub_h
#define GenericAzureIoTHub_h
#include <WiFiClientSecure.h>
#include "sha256.h"
#include "Base64.h"
#include <ESP8266WiFi.h>
#include "pubsubclient/PubSubClient.h"

enum CloudMode {
	IoTHub,
	EventHub
};

struct CloudConfig {
	CloudMode cloudMode = IoTHub;
	unsigned int publishRateInSeconds = 90; // defaults to once a minute
	unsigned int sasExpiryDate = 1737504000;  // Expires Wed, 22 Jan 2025 00:00:00 GMT
	const char *host;
	char *key;
	const char *id;
	unsigned long lastPublishTime = 0;
	const char* fullSas;
	const char * postUrl;
	const char * hubUser;
	const char * getUrl;
};

typedef void(*GeneralFunction) (const byte* payload, int length);

class AzureIoTHub
{
public:
	int senddata = 0;
	bool connect(),
		push(char *data);
	void begin(String cs);
	void setCallback(GeneralFunction _az);
private:
	static GeneralFunction az;
	static void callback(char* topic, byte* payload, unsigned int length);
	const char *GetStringValue(String value);
	String splitStringByIndex(String data, char separator, int index),
		urlEncode(const char* msg),
		createIotHubSas(char *key, String url);
};

extern GenericAzureIoTHub Azure;
#endif
