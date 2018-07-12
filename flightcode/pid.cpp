#include "pid.h"
#include "math.h"

void setTimeDiff() {
	pid::timerTwo = millis();
	pid::timeDiff = (pid::timerTwo - pid::timerOne) / 1000;
	pid::timerOne = pid::timerTwo;
}

double calculate_altitude(Adafruit_BME280 sensor) {
	return sensor.readAltitude(SEALEVELPRESSURE_HPA) * meterToFeet;
}

double calc_Z_accel(Adafruit_MMA8451 sensor) {
	sensor.read();
	sensors_event_t event;
	sensor.getEvent(&event);
	return event.acceleration.z * -meterToFeet;
}

double projected_altitude(double veloc, double accel, double currentAlt) {
	double termV = sqrt((GRAV * veloc * veloc) / -(accel + GRAV));
	double funcConst = ((veloc * veloc) + (termV * termV)) / (termV * termV);
	return ((termV * termV) / (2 * GRAV)) * log(funcConst) + currentAlt;
}

double calculate_velocity(double currentAltitude) {
	return (currentAltitude - pid::lastAltitude) / pid::timeDiff;
}

int pid_main(double position, double altitude, double velocity, double accel) {

	double projectedAltitude = projected_altitude(velocity, accel, altitude);

	double P = projectedAltitude - SETPOINT;        // Proportional term.

	double I = pid::I_0 + (P * pid::timeDiff);      // Integral term.

	if (I > I_MAX) { I = I_MAX; }			        // These if statements are here to
	else if (I < -I_MAX) { I = -I_MAX; }	        // prevent integral windup.

	double D = (P - pid::P_0) / 2;			        // Derivative term.

	double diffOutput = (P * KP) + (I * KI) + (D * KD);

	// This block saves the variables in this loop so it can be used in the next.
	pid::P_0 = P;
	pid::I_0 = I;
	pid::lastAltitude = altitude;

	// newPosition is the new position of the the fins.
	double newPosition = position + diffOutput;

	// These prevent the new position of the fins from going over their max and min values.
	if (newPosition > MAX) { newPosition = MAX; } else if (newPosition < MIN) { newPosition = MIN; }

	return (int)newPosition;
}
