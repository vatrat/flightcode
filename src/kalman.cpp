#include "kalman.h"

// Constructor
Kalman::Kalman(double covariance) {
	this->R = covariance;
	this->x_k = 0;
	this->P_k = 1;
}

// This function is used to return the filtered signal.
double Kalman::output(double z_k) {
	update(z_k);
	return x_k;
}

// Does all the kalman filter stuff
void Kalman::update(double z_k) {
	K_k = P_k_1 / (P_k_1 + R);                // Compute the Kalman gain for this iteration.
	x_k = x_k_1 + (K_k * (z_k - x_k_1));      // Calculate the new filtered output
	P_k = (1 - K_k) * P_k_1;                  // Calculate new error covariance
	P_k_1 = P_k;                              // Set error covariance for next iteration
	x_k_1 = x_k;                              // Set the old output for the next iteration
	k += 1;
}
