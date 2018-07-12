#include "eom.h"

double getAccelZ(Adafruit_MMA8451 sensor) {
	sensor.read();
	sensors_event_t event;
	sensor.getEvent(&event);
	return event.acceleration.z * -meterToFeet;
}
