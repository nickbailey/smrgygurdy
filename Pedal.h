#ifndef PEDAL_H
#define PEDAL_H

#include "Thread.h"
#include "Lock.h"
#include "Controller.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <asm/types.h>
#include <stdbool.h>

#include "pmd.h"
#include "minilab-1008.h"

/**
 * Provides an interface for the minilab 1008 ADC.
 * The Analogue to Digital Converter receives a voltage level from the pedal, from which a discrete value is calculated. This can then be read by polling the device.
 */
class Pedal : public Thread {
	
	hid_return ret;
	HIDInterface* hid;
	Controller* controller;

 public: 

	/**
	 * Default constructor
	 * @param controller An instance of the Controller class
	 */
	Pedal(Controller *controller);

	/**
	 * Poll the ADC for the converted voltage from the pedal.
	 */
	void run();

};
#endif /*PEDAL_H*/
