#include <Arduino.h>
#include <MacRocketry_SD_Logger.h>

//End of Source ======================================================================================================
//Start of Main ======================================================================================================
MacRocketry_SD_Logger sd;

void setup() {
	Serial.begin(115200);
	while(!Serial);

	sd.begin();

	if (sd.getConnectSD()) Serial.println("SD recognized");
	else Serial.println("no SD input");
	
	if (sd.getConnectSD()) Serial.println("File IO");
	else Serial.println("cannot open file");
	
	Serial.println(sd.maxUInt());
}

int i = 0;
void loop() {
  delay(100);
	i++;
	if (sd.writeBuffer("data_" + String(i) + "\r\n"))
		Serial.println(i);
	else Serial.println("Cannot write " + String(i));
}
