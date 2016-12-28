//avr-json Proof of Concept tesing using ArduinoJson for Wireless Sensor Network Comminication
/*
Change Log
2016-12-23: Test JSON Output
2016-12-24: Add Serial input

*/

#include <ArduinoJson.h>
#define SLEEP 1000
// Create 200B buffer for json object
// Create json object

char SerialBuffer[200];
#define MAX_NETWORK_SIZE 50
unsigned int loopCount = 0;

typedef struct { 
  byte 				NodeID; 		// Address of target sensor 
  byte 				Method; 		// Message method (Example: Get / Set)
  byte				TypeID; 		// Sensor or system data type
  const char*		DataMap;		// Character Map used to decode data 
  byte 				MsgSize;		// Length message payload
  byte  			MsgPayload[MAX_NETWORK_SIZE];
} _Msg;
_Msg msg;	// serial Message

typedef struct {
	byte 	b1;
	byte 	b2;
	unsigned long 	uL1;
	float 			f1;
	int 			i1;
	unsigned int 	uI1;
	unsigned long 	uL2;
} _SampleSensorData;
_SampleSensorData sampleData;
byte decodeBuffer[4]; // max size for int / long 
byte payloadBuffer[MAX_NETWORK_SIZE];

void setup() {
	Serial.begin(115200);
	while (! Serial ) {
		//Wait for serial port to initialize
	}
	// Reserve memory for json buffer
	
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
		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& json_in = jsonBuffer.parseObject(SerialBuffer);
		//Serial.println(parse);
		
		//JsonObject& json_in = jsonBuffer.parseObject(parse);
		if (!json_in.success()) {
    		Serial.println("parseObject() failed");
    		return;
  		}
		Serial.println( "Got Data" );
		
		int node = json_in["node"];
		long type = json_in["type"];
		Serial.print("Node: ");
		Serial.println(node);
		Serial.print("Type: ");
		Serial.println(type);
		Serial.flush();
	} else {
		// Send normal output 
		if ( millis() % SLEEP == 0  && ! MsgSent ) {

			StaticJsonBuffer<200> jsonBuffer;

			JsonObject& json_out = jsonBuffer.createObject();


			msg.NodeID = 123;
			msg.Method = 2;
			msg.TypeID = 200;
			msg.DataMap = "bbLfiIL"; // Byte, Byte, Unsigned Long, float, integer, Unsigned Integer

			json_out["node"] = msg.NodeID;
			json_out["meth"] = msg.Method;
			json_out["type"] = msg.TypeID;
			json_out["map"] = msg.DataMap;

			sampleData.b1 = 42;
			sampleData.b2 = 43;
			sampleData.uL1 = 999999;
			sampleData.f1 = 175.06;
			sampleData.i1 = -678;
			sampleData.uI1 = loopCount;
			sampleData.uL2 = millis();

			msg.MsgSize = sizeof(sampleData);
			Serial.print("Message Size: ");
			Serial.println(msg.MsgSize);

			memcpy(payloadBuffer, &sampleData, (int)msg.MsgSize);
			Serial.print("Data Map Size: ");
			Serial.println( strlen(msg.DataMap) );

			Serial.print("Data Map Value: ");
			Serial.println( msg.DataMap );
			int bufferPosition = 0;

			JsonArray& data = json_out.createNestedArray("data");
			for (int x = 0; x < strlen(msg.DataMap); x++) {
				//Serial.print("DataMap[");
				// Serial.print(x);
				// Serial.print("]: ");
				// Serial.println(msg.DataMap[x]);

				// Serial.print("BufferPosition: ");
				// Serial.println(bufferPosition);
				// Serial.println(payloadBuffer[bufferPosition], HEX);
				char thisDataMap = msg.DataMap[x];

				if (thisDataMap == 'b') {
					data.add((byte)payloadBuffer[bufferPosition]);
					bufferPosition += 1;
				}
				else if (thisDataMap == 'f' or thisDataMap == 'L' or thisDataMap == 'l' or thisDataMap == 'd') {
					byte fourByte[4];
					memcpy(fourByte, &payloadBuffer[bufferPosition], 4);

					if (thisDataMap == 'f') {
						float castFloat;
						castFloat = * (float *) fourByte;
						data.add(castFloat);
					}
					if (thisDataMap == 'L') {
						unsigned long castUlong;
						castUlong = * (unsigned long *) fourByte;
						data.add(castUlong);
					}
					if (thisDataMap == 'l') {
						long castLong;
						castLong = * (long *) fourByte;
						data.add(castLong);
					}
					if (thisDataMap == 'd') {
						double castDouble;
						castDouble = * (double *) fourByte;
						data.add(castDouble);
					}
					bufferPosition += 4;

				}
				else if (thisDataMap == 'i' or thisDataMap == 'I' or thisDataMap == 'w') {
					byte twoByte[2];
					memcpy(twoByte, &payloadBuffer[bufferPosition], 2);

					if (thisDataMap == 'i') {
						int castInt;
						castInt = * (int *) twoByte;
						data.add(castInt);
					}
					if (thisDataMap == 'I') {
						unsigned int castUint;
						castUint = * (unsigned int *) twoByte;
						data.add(castUint);
					}
					if (thisDataMap == 'w') {
						word castWord;
						castWord = * (word *) twoByte;
						data.add(castWord);
					}

					bufferPosition += 2;
				} else {
					Serial.print("Undefined: ");
					Serial.println(msg.DataMap[x]);
				}	
				//if ( (char)msg.DataMap[x])



				// switch ( (char)msg.DataMap[x] ) {
				// 	case 'b':

				// 		//byte temp = (byte)msg.DataMap[x];
				// 		data.add((byte)payloadBuffer[bufferPosition]);
				// 		//json_out["byte"] = (byte)payloadBuffer[bufferPosition];
						
				// 		// Serial.print("Byte -> ");
				// 		// Serial.println(payloadBuffer[bufferPosition]);
				// 		bufferPosition += 1;
				// 	break;
				// 	// case 'f':
				// 	// 	// Long Datatype (Size 4)
				// 	// 	Serial.print("float -> ");
				// 	// 	float f_value = 0;
				// 	// 	// byte LongData[4];
				// 	// 	byte floatBuffer[4];
				// 	// 	memcpy(floatBuffer, &payloadBuffer[bufferPosition], 4);
						

				// 	// 	//float float_value = * (float *) floatBuffer;
				// 	// 	//bufferPosition += 4;
				// 	// 	data.add(f_value);

				// 	// break;
				// 	case 'L':
				// 		// Long Datatype (Size 4)
				// 		Serial.print("Unsigned Long -> ");
				// 		// byte LongData[4];

				// 		// // memcpy(unsigned long LargeBuffer, &payloadBuffer[bufferPosition], 4);
				// 		// // memcpy(LongData, &payloadBuffer[bufferPosition], 4);
				// 		// Serial.println(sampleData.uL1, HEX);
				// 		// // Serial.println((unsigned long)LongData, HEX);


				// 		// // data.add((unsigned long)LargeBuffer);
				// 		// // data.add((long)LongData);
				// 		// //Serial.println(payloadBuffer[bufferPosition]);
						

				// 		byte LargeBuffer[4];
				// 		Serial.println("Raw Buffer...");
				// 		// for (int x =0 ; x <= 3; x++) {
				// 		// 	//Serial.println(payloadBuffer[bufferPosition + x], HEX);
				// 		// 	LargeBuffer[x] = payloadBuffer[bufferPosition + x];
				// 		// 	//bufferPosition++;

				// 		// }
				// 		memcpy(LargeBuffer, &payloadBuffer[bufferPosition], 4);
				// 		bufferPosition += 4;

				// 		unsigned long adc_value = 0;
				// 		adc_value = * (unsigned long *) LargeBuffer;

				// 		// unsigned long adc_value =0;
				// 		// adc_value += (unsigned long)payloadBuffer[bufferPosition] << 24;
				// 		// adc_value += (unsigned long)payloadBuffer[bufferPosition +1] << 16;
				// 		// adc_value += (unsigned long)payloadBuffer[bufferPosition +2] << 8;
				// 		// adc_value += (unsigned long)payloadBuffer[bufferPosition +3];
				// 		//int position = 0;




				// 		//for (int x = 3; x >= 0; x--) {
				// 		// 	Serial.println(payloadBuffer[bufferPosition + x], HEX);

				// 		// 	LargeBuffer[position] = payloadBuffer[bufferPosition + x];
				// 		// 	position++;

				// 		// }
				// 		// unsigned long longData;
				// 		// longData = (unsigned long)LargeBuffer;
				// 		// Serial.println(longData, HEX);

				// 		// http://forum.arduino.cc/index.php?topic=71030.0
				// 		// long adc_value =0;
				// 		// adc_value += payloadBuffer[bufferPosition] << 24;
				// 		// adc_value += payloadBuffer[bufferPosition +1 ] << 16;
				// 		// adc_value += payloadBuffer[bufferPosition +2] << 8;
				// 		// adc_value += payloadBuffer[bufferPosition +3];
				// 		// adc_value = *((long*)d);



				// 		data.add(adc_value);
				// 	break;
				// 	default:
				// 		Serial.print("Undefined: ");
				// 		Serial.println(msg.DataMap[x]);
				// 	break;

				// }


			}

			//json_out["node"] = "1";
			//json_out["millis"] = millis();
			json_out.printTo(Serial);
			Serial.println();
			delay(1);
			MsgSent = true;
			loopCount += 1;
		} else {
			MsgSent = false;
		}
		//delay(10);

	}	
}
