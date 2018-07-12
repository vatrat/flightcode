#include "datalog.h"
#include <SD.h>


void DataLog::initialize_file() {
	if (!SD.begin(chipSelect)) {
		Serial.println("SD card failed.");
	} else {
		File dataFile = SD.open(fileName, FILE_WRITE);
		if (dataFile) {
			dataFile.println("-------------- LAUNCH DATA --------------");
			dataFile.println();
			dataFile.close();
		}
	}
}
