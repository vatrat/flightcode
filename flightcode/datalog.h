// datalog.h

#ifndef _DATALOG_h
#define _DATALOG_h

class DataLog {

public:

	DataLog() {

	}

private:
	/* CLASS VARIABLES */
	const int chipSelect = 10;
	const char* fileName = "data.txt";


	// Set up text file with headers and what not.
	void initialize_file();
};

#endif

