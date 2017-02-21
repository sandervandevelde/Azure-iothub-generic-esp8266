#include "AzureIoTHub.h"

CloudConfig cloud;

WiFiClientSecure espClient;
PubSubClient mqtt(espClient);

GeneralFunction AzureIoTHub::az;

void AzureIoTHub::callback(char * topic, byte * payload, unsigned int length)
{
	az(payload, length);
}

void AzureIoTHub::begin(String cs){
	cloud.host = GetStringValue(splitStringByIndex(splitStringByIndex(cs, ';', 0), '=', 1));
	cloud.id = GetStringValue(splitStringByIndex(splitStringByIndex(cs, ';', 1), '=', 1));
	cloud.key = (char*)GetStringValue(splitStringByIndex(splitStringByIndex(cs, ';', 2), '=', 1));
	cloud.postUrl = GetStringValue("devices/"+ (String)cloud.id+ "/messages/events/");
	cloud.hubUser= GetStringValue((String)cloud.host + "/" + (String)cloud.id);
	cloud.fullSas = GetStringValue(createIotHubSas(cloud.key, urlEncode(cloud.host) + "/devices/" + (String)cloud.id));
	cloud.getUrl = GetStringValue("devices/" + (String)cloud.id + "/messages/devicebound/#");
	mqtt.setServer(cloud.host, 8883);
}

 void AzureIoTHub::setCallback(GeneralFunction _az){
	 mqtt.setCallback(this->callback);
	 az = _az;
 }
bool AzureIoTHub::push(char *data){
		mqtt.publish(cloud.postUrl, data);
}

bool AzureIoTHub::connect()
{
	while (!mqtt.connected()) {
		Serial.print(F("Attempting MQTT connection..."));
		if (mqtt.connect(cloud.id, cloud.hubUser, cloud.fullSas)) {
			Serial.println(F("connected"));
			// ... and resubscribe
			mqtt.subscribe(cloud.getUrl);
		}
		else {
			Serial.print(F("failed, rc="));
			Serial.print(mqtt.state());
			Serial.println(F(" try again in 5 seconds"));
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
	mqtt.loop();
}

const char * AzureIoTHub::GetStringValue(String value){
	int len = value.length() + 1;
	char *temp = new char[len];
	value.toCharArray(temp, len);
	return temp;
}

String AzureIoTHub::splitStringByIndex(String data, char separator, int index){
	int found = 0;
	int strIndex[] = { 0, -1 };
	int maxIndex = data.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++) {
		if (data.charAt(i) == separator || i == maxIndex) {
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}
	return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

String AzureIoTHub::urlEncode(const char * msg){
	const char *hex = "0123456789abcdef";
	String encodedMsg = "";

	while (*msg != '\0') {
		if (('a' <= *msg && *msg <= 'z')
			|| ('A' <= *msg && *msg <= 'Z')
			|| ('0' <= *msg && *msg <= '9')) {
			encodedMsg += *msg;
		}
		else {
			encodedMsg += '%';
			encodedMsg += hex[*msg >> 4];
			encodedMsg += hex[*msg & 15];
		}
		msg++;
	}
	return encodedMsg;
}

String AzureIoTHub::createIotHubSas(char * key, String url)
{
	String stringToSign = url + "\n" + cloud.sasExpiryDate;

	// START: Create signature
	// https://raw.githubusercontent.com/adamvr/arduino-base64/master/examples/base64/base64.ino

	int keyLength = strlen(key);

	int decodedKeyLength = base64_dec_len(key, keyLength);
	char decodedKey[decodedKeyLength];  //allocate char array big enough for the base64 decoded key

	base64_decode(decodedKey, key, keyLength);  //decode key

	Sha256.initHmac((const uint8_t*)decodedKey, decodedKeyLength);
	Sha256.print(stringToSign);
	char* sign = (char*)Sha256.resultHmac();
	// END: Create signature

	// START: Get base64 of signature
	int encodedSignLen = base64_enc_len(HASH_LENGTH);
	char encodedSign[encodedSignLen];
	base64_encode(encodedSign, sign, HASH_LENGTH);

	// SharedAccessSignature
	return "SharedAccessSignature sr=" + url + "&sig=" + urlEncode(encodedSign) + "&se=" + cloud.sasExpiryDate;
	// END: create SAS  
}

AzureIoTHub Azure;