#ifndef SERIALPEDAL_H
#define SERIALPEDAL_H

#include "Thread.h"
#include "Lock.h"
#include "Controller.h"
#include "Pedal.h"

#include <fstream>
#include <string>

/**
 * Provides an interface for serial-port (including USB serial port) connected
 * pedals.
 * 
 * Pedals connected to the serial port should send numbers in the range 0 to 1.0
 * in ASCII representation.
 */
class SerialPedal : public Pedal {
	
 public: 

	/**
	 * Default constructor
	 * @param controller An instance of the Controller class
	 * @param node The name of the device node from which to read pedal data
	 * @param eps Minimum significant change
	 * @param verbosity Reporting level
	 */
	explicit SerialPedal(Controller *c,
			     const std::string &node,
	                     double eps = 0.01,
	                     int verbosity = 1
	                    );
	~SerialPedal();

	/**
	 * Called by thread start to read samples from the serial pedal
	 */
	virtual void run();

 protected:
	std::ifstream* serial_port;
};
#endif /*SERIALPEDAL_H*/
