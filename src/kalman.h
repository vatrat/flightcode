// kalman.h

#ifndef _KALMAN_h
#define _KALMAN_h

// Using a class here because some sensors are noisier than others, 
// and the Kalman filter depends on this value to work.
class Kalman {

public:
	// Constructor
	Kalman(double covariance);

	// Use this method when we want to get the filtered output
	double output(double z_k);

private:
	double K_k;         // Kalman gain
	double P_k;         // Prior error covariance
	double P_k_1;       // Prior error covariance from the previous iteration
	double R;           // Covariance, or noise
	double x_k;         // Filtered output
	double x_k_1;       // Output from the last iteration
	unsigned int k;     // Number of iterations

	// Private method that saves the current variables to be used in the next iteration.
	void update(double z_k);

};

#endif

