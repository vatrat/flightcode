#include "pid.h"
#include "math.h"

void setTimeDiff() {
	pid::timerTwo = millis();
	pid::timeDiff = (pid::timerTwo - pid::timerOne) / 1000;
	pid::timerOne = pid::timerTwo;
}

double calculate_altitude(Adafruit_BME280 &sensor) {
	return sensor.readAltitude(SEALEVELPRESSURE_HPA) * meterToFeet;
}

double get_accel(Adafruit_MMA8451 &sensor) {
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

double calculate_velocity() {
	return (pid::altitude - pid::lastAltitude) / pid::timeDiff;
}

int pid_main(double position, double altitude, double velocity, double accel) {
	
	using namespace pid; 	// So we don't need to keep typing pid:: in this scope.

	projectedAltitude = projected_altitude(velocity, accel, altitude);

	P = projectedAltitude - SETPOINT;       // Proportional term.

	I = I_0 + (P * timeDiff);      			// Integral term.

	if (I > I_MAX) { I = I_MAX; }			// These if statements are here to
	else if (I < -I_MAX) { I = -I_MAX; }    // prevent integral windup.

	D = (P - P_0) / 2;	// Derivative term.

	double diffOutput = (P * KP) + (I * KI) + (D * KD);

	// This block saves the variables in this loop so it can be used in the next.
	P_0 = P;
	I_0 = I;
	lastAltitude = altitude;
	setTimeDiff();

	// updating the position of the fairings.
	position += diffOutput;

	// These prevent the new position of the fins from going over their max and min values.
	if (position > MAX) { position = MAX; } else if (position < MIN) { position = MIN; }

	return (int) position;
}
