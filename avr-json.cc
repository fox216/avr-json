//avr-json Proof of Concept tesing using ArduinoJson for Wireless Sensor Network Comminication


#include <ArduinoJson.h>
#define SLEEP 5000
// Create 200B buffer for json object
StaticJsonBuffer<200> jsonBuffer;
// Create json object
JsonObject& json = jsonBuffer.createObject();

void setup() {
	Serial.begin(115200);
	while (! Serial ) {
		//Wait for serial port to initialize
	}
	// Reserve memory for json buffer
	json["node"] = "1";
}

void loop() {
	bool MsgSent = false;
	if ( millis() % SLEEP == 0  && ! MsgSent ) {
		json["millis"] = millis();
		json.printTo(Serial);
		Serial.println();
		delay(1);
		MsgSent = true;
	} else {
		MsgSent = false;
	}
}
