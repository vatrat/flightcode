#include "pid.h"
#include "math.h"

/*
So since I'm both lazy and I don't like passing a bunch of things to functions,
I put a lot of the variables in this namespace. That way, we can use them like 
global variables but they're enclosed within this namespace called pid. If you
don't know what namespaces are, they are kind of like adding subscripts to variables
in math, so you can tell variables apart from one another. Global variables aren't 
necessarily bad, but when you include a bunch of files into one another you might
accidently name two different variables the same thing and that leads to all sorts 
of problems. Putting these variables in a namespace prevents that, so there's no 
naming conflicts.
*/

namespace pid {
	double projectedAltitude;
	double altitude;			// Current altitude.
	double lastAltitude;		// Altitude reading on last loop.
	double P;
	double I;
	double D;
	double P_0;			    // Proportional term.
	double I_0;			    // Integral term.
	double timeDiff;			// Difference between loops, needed for proportional term.
	double timerOne;
	double timerTwo;
}


// Calculates the time between each loop, so we can use it to find velocity.
void setTimeDiff() {
	pid::timerTwo = millis();
	pid::timeDiff = (pid::timerTwo - pid::timerOne) / 1000;
	pid::timerOne = pid::timerTwo;
}

// Get the current altitude
double calculate_altitude(Adafruit_BME280 &sensor) {
	return sensor.readAltitude(SEALEVELPRESSURE_HPA) * meterToFeet;
}

// Get the current acceleration (only in the up direction)
double get_accel(Adafruit_MMA8451 &sensor) {
	sensor.read();
	sensors_event_t event;
	sensor.getEvent(&event);
	return event.acceleration.z * -meterToFeet;
}

// This is our max altitude, which is what the pid controller uses to calculate 
// the position of the plates.
double projected_altitude(double veloc, double accel, double currentAlt) {
	double termV = sqrt((GRAV * veloc * veloc) / -(accel + GRAV));
	double funcConst = ((veloc * veloc) + (termV * termV)) / (termV * termV);
	return ((termV * termV) / (2 * GRAV)) * log(funcConst) + currentAlt;
}

// Get the velocity.
double calculate_velocity() {
	return (pid::altitude - pid::lastAltitude) / pid::timeDiff;
}

// The actual controller.
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
