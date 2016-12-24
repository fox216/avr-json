//avr-json Proof of Concept tesing using ArduinoJson for Wireless Sensor Network Comminication
/*
Change Log
2016-12-23: Test JSON Output
2016-12-24: Add Serial input

*/

#include <ArduinoJson.h>
#define SLEEP 5000
// Create 200B buffer for json object
StaticJsonBuffer<200> jsonBuffer;
// Create json object
JsonObject& json_out = jsonBuffer.createObject();

char SerialBuffer[200];

void setup() {
	Serial.begin(115200);
	while (! Serial ) {
		//Wait for serial port to initialize
	}
	// Reserve memory for json buffer
	json_out["node"] = "1";
}

void loop() {
	bool MsgSent = false;
	// Test if serial buffer has input
	if ( Serial.available() > 0 ) {
		// Read incomnig byte until a trailing '}' character is detected.
		Serial.readBytesUntil('}', SerialBuffer, 199);
		// Parse character buffer from serial port as json
		JsonObject& json_in = jsonBuffer.parseObject(SerialBuffer);
		

	} else {
		// Send normal output 
		if ( millis() % SLEEP == 0  && ! MsgSent ) {
			json_out["millis"] = millis();
			json_out.printTo(Serial);
			Serial.println();
			delay(1);
			MsgSent = true;
		} else {
			MsgSent = false;
		}
	}	
}
