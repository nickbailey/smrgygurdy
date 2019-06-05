#include "Controller.h"
#include "SerialPedal.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>

SerialPedal::SerialPedal(Controller *c, const std::string &node, double eps, int verbosity) :
    Pedal(c, eps)
{
	serial_port = new std::ifstream(node, std::ifstream::in);
	if (!serial_port->is_open())
		throw "SerialPedal: Unable to open device node";

	announce = (verbosity > 1); 
}

SerialPedal::~SerialPedal()
{
	if (serial_port) {
		serial_port->close();
		delete serial_port;
	}
}

void SerialPedal::run() {
	float last_value { -1.0 };

	while (1) {
	
		*serial_port >> value;
		
		// If pedal's changed by more than a given
		// amount of travel, tell the controller
		if (fabs(value - last_value) > epsilon) {
			controller->speedChange(value);
			last_value = value;
		if (announce)
			std::cout << "Serial pedal value: " << value << std::endl;
        }
    }
}
