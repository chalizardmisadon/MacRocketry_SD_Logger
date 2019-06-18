#include <Arduino.h>  //include Arduino library
#include <MacRocketry_SD_Logger.h>  //header file
#include <SPI.h>     //SPI library
#include <SD.h>      //SD card library

MacRocketry_SD_Logger::MacRocketry_SD_Logger(void){ //constructor
	init();
}

void MacRocketry_SD_Logger::init(void){ //initialize variables to null
	connectSD = false;
	connectFile = false;
	bufferSize = 0; //variables for more efficient SD write
}

void MacRocketry_SD_Logger::begin(void){
	//similar to BMP code, there is a bug where SD.begin() cannot be called within a class contructor
	//therefore, we have to call MacRocketry_SD_Logger::begin() inside main setup()
	connectSD = SD.begin(SD_CS_Pin);      //initialize SD
	connectFile = openNextFile();         //open file
}

void MacRocketry_SD_Logger::begin(String file){
	connectSD = SD.begin(SD_CS_Pin);      //initialize SD
	connectFile = openFile(file);         //open file
}

//getters and setters --------------------------------------------------
uint16_t MacRocketry_SD_Logger::maxUInt(void)		{ return 0xffff; } //max 16-bit number
bool MacRocketry_SD_Logger::getConnectSD(void)		{ return connectSD; }
bool MacRocketry_SD_Logger::getConnectFile(void)	{ return connectFile; }

//file open function --------------------------------------------------
bool MacRocketry_SD_Logger::openNextFile(void){
	if (getConnectSD()){ //if SD card is connected
		
		uint16_t numNext = 0;
		while ( //if file already exist and numNext have not reach max value
			(SD.exists(String(fileNamePrefix + String(numNext)).c_str())) && 
			(numNext < maxUInt()))
		{
			numNext++;
		}
		//if file num does not exceed maxUInt()
		if (numNext < maxUInt()) return openFile(String(fileNamePrefix) + String(numNext));
	}
	return false; //no SD card
}


bool MacRocketry_SD_Logger::openFile(String filePath){
	if (getConnectSD()){
		if (sdFile){        //if current file is open
			sdFile.close();   //close current file
			delay(5);         //delay 5ms
		}
		sdFile = SD.open(filePath.c_str(), FILE_WRITE);   //open new file for write
		if (sdFile){                              //if successful
			writeFile("start logging...");
			return true;                            //open file successfully
		}
	}
	return false; //cannot open file
}


//file close function --------------------------------------------------
bool MacRocketry_SD_Logger::writeFile(String data){
	if (getConnectFile()){    //if file is open
		sdFile.println(data);
		sdFile.flush();         //flush all bytes to SD card
		return true;
	}
	return false; //cannot write to file
}


bool MacRocketry_SD_Logger::writeBuffer(String data){
	if (false == getConnectFile()) return false; //return if file not open

	//calculate available space in buffer, String.length() returns unsigned int
	int16_t bufferAllow = min(Write_Buffer - bufferSize, (int)data.length());
	
	//write string with allowable space
	bufferSize += bufferAllow; //update current buffer size
	Serial.println(sdFile.print(data.substring(0, bufferAllow)));
	
	//process when buffer is full
	if (Write_Buffer <= bufferSize){ //if buffer is full
		#ifdef Log_Bufferred
		sdFile.print(Log_Buffer_Word);
		#endif
		sdFile.flush(); //actually record to SD
		
		//write the rest of the data
		String remain = data.substring(bufferAllow);
		Serial.println(sdFile.print(remain));   //print the rest of data
		bufferSize = remain.length();           //reset buffer
	}
	return true;
}
