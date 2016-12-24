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
		//delay(20);
		Serial.readBytesUntil('\n', SerialBuffer, 199);
		
		// NOTE: Alternate method for reading serial port if memory is constrained
		// int count=0;
		// // read until the serial buffer is empty
		// while (Serial.available() > 0) {
		// 	SerialBuffer[count] = (char)Serial.read();
		// 	count++;
		// }
		
		// NOTE: Test most contain single quotes when issued from command line
		// Test Msg: echo '{"node":"1","millis":55000}' > /dev/ttyLPL
		// Parse character buffer from serial port as json
		Serial.println("Buffer -> ");
		Serial.println(SerialBuffer);
		Serial.println();
		//char parse[] = "{\"node\":\"1\",\"millis\":55000}";
		JsonObject& json_in = jsonBuffer.parseObject(SerialBuffer);
		//Serial.println(parse);
		
		//JsonObject& json_in = jsonBuffer.parseObject(parse);
		if (!json_in.success()) {
    		Serial.println("parseObject() failed");
    		return;
  		}
		Serial.println( "Got Data" );
		
		int node = json_in["node"];
		long mill = json_in["millis"];
		Serial.print("Node: ");
		Serial.println(node);
		Serial.print("Seconds: ");
		Serial.println(mill);
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
