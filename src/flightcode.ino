/*
 Name:		flightcode.ino
 Created:	7/3/2018 6:47:02 PM
 Author:	Austen LeBeau
*/

#include <Wire.h>
#include <SD.h>
#include <Adafruit_MotorShield.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_MMA8451.h>
#include "pid.h"
#include "kalman.h"
#include "datalog.h"


// Creating some kalman filter objects. 
Kalman altitudeFilter = Kalman(0.01);  // Or something, I don't know what the actual value is. 
Kalman accelFilter = Kalman(0.05);     // Again, no idea what this is actually supposed to be.

// motor shield object
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// pointer to motor at port 1
Adafruit_DCMotor *motor = AFMS.getMotor(4);

/* Assign a unique ID to this sensor at the same time */
Adafruit_MMA8451 mma = Adafruit_MMA8451();

Adafruit_BME280 bme;

// Runmode enum allows the arduino to shift between operation states in flight
enum Runmode : unsigned char {
    idle,		// Rocket is sitting on launchpad
    launch,		// Motor is ignited and burning (have to wait for burnout)
    coast,		// Motor is burnt out, this is where the fairings will do their thing
    descent		// Rocket is now descending - retract fairings and power down
};

// We want to start off with idle on the launchpad.
Runmode MODE = idle;


// the setup function runs once when you press reset or power the board
void setup() {

    // VARS
    const int channelA = 3;
    const int channelB = 4;
    int encoderTicks = 0;                // ticks for encoder


    // Initialize the sensor 
    Serial.begin(9600);
    // sets the modes of the channel pins to input
    pinMode(channelA, INPUT);
    pinMode(channelB, INPUT);

    // create with the default frequency 1.6KHz
    AFMS.begin();

    // attach interrupt to pin 3 for encoder output
    //	attachInterrupt(digitalPinToInterrupt(3), channelAEvent, CHANGE);


    // test pressure sensor 
    if (!bme.begin()) {
        Serial.println("Pressure sensor was not found. Check the wiring.");
        while (1);
    }

    // test acceleration sensor
    if (!mma.begin()) {
        Serial.println("Acceleration sensor was not found. Check the wiring.");
        while (1);
    }
    mma.setRange(MMA8451_RANGE_2_G);

}

// the loop function runs over and over again until power down or reset
void loop() {

    // Sitting on the launch pad.
    if (MODE == idle) {
        if (get_accel(mma) > 0) {
            MODE = launch;
        }
    }

    // Motor is burning, have to wait until burn out.
    else if (MODE == launch) {
        if (get_accel(mma) < 0) {
            MODE = coast;
        }
    }

    else if (MODE == coast) {
        pid::altitude = altitudeFilter.output(bme.readAltitude(SEALEVELPRESSURE_HPA));
        // Do all the controller stuff and data logging.
        if (calculate_velocity() < 0) {
            MODE = descent;
        }
    }

    else if (MODE == descent) {
        // Retract the fairings, close the data file, and power down after all that's done.
    }


}
