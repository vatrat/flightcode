// pid.h

#ifndef _PID_h
#define _PID_h

#include <Adafruit_BME280.h>
#include <Adafruit_MMA8451.h>


namespace pid {
	static double projectedAltitude;
	static double altitude;			// Current altitude.
	static double lastAltitude;		// Altitude reading on last loop.
	static double P;
	static double I;
	static double D;
	static double P_0;			    // Proportional term.
	static double I_0;			    // Integral term.
	static double timeDiff;			// Difference between loops, needed for proportional term.
	static double timerOne;
	static double timerTwo;
}


/* ALL THE CONSTANTS THE CONTROLLER NEEDS TO FUNCTION */

const double KP = 0.0075;			// Proportional gain.
const double KI = 0.001;			// Integral gain.
const double KD = 0.005;			// Derivative gain.
const int I_MAX = 100;				// Prevents integral windup.
const double SETPOINT = 5280;		// Target altitude in feet.

const short MAX = 175;				// Maximum input to motor.
const short MIN = -15;				// Minimum input to motor.

const double GRAV = 32.1737;		// Gravity constant in english units.
const double meterToFeet = 3.280840;
const double SEALEVELPRESSURE_HPA = 1013.25;


/* FUNCTION DECLARATIONS */

double calculate_altitude(Adafruit_BME280 &sensor);

// Calculate the projected altitude, which the controller needs to know in order to function properly.
double projected_altitude(double veloc, double accel, double currentAlt);

// Pretty self-explanatory
double calculate_velocity();

double get_accel(Adafruit_MMA8451 &sensor);


// The bread and butter of the whole thing, returns the position the motor should be set to.
int pid_main(double position, double altitude, double velocity, double accel);

#endif

