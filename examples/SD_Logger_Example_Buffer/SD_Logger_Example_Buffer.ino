#include <Arduino.h>
#include <MacRocketry_SD_Logger.h>
#include <MacRocketry_GPS_Shield.h>

MacRocketry_SD_Logger sd;
MacRocketry_GPS_Shield gps;

void setup() {
	Serial.begin(115200);
	if (sd.getConnectSD()) Serial.println("SD recognized");
	else Serial.println("no SD input");
	
	if (sd.getConnectSD()) Serial.println("File IO");
	else Serial.println("cannot open file");
	
	Serial.println(sd.maxUInt());
}

int i = 0;
void loop() {
	i++;
	if (sd.writeBuffer("data_" + String(i) + "\r\n"))
		Serial.println(i);
	else Serial.println("Cannot write " + String(i));

	if (gps.parseData()){
		Serial.print(gps.getData());
		if (sd.writeBuffer(gps.getData()))
			Serial.println("write GPS");
	}
	else Serial.println("No GPS"); 
}
