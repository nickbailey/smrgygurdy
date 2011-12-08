#ifndef MINILAB1008_H
#define MINILAB1008_H

#include "Thread.h"
#include "Lock.h"
#include "Controller.h"
#include "Pedal.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <asm/types.h>
#include <stdbool.h>

#include <libhid/pmd.h>
#include <libhid/minilab-1008.h>

/**
 * Provides an interface for the minilab 1008 ADC.
 * The Analogue to Digital Converter receives a voltage level from the pedal, from which a discrete value is calculated. This can then be read by polling the device.
 */
class MiniLAB1008 : public Pedal {
	
	hid_return ret;
	HIDInterface* hid;

 public: 

	/**
	 * Default constructor
	 * @param controller An instance of the Controller class
	 */
	MiniLAB1008(Controller *c);
        virtual void run();
};
#endif /*MINILAB1008_H*/
