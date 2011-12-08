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

#include <libhid/pmd.h>
#include <libhid/minilab-1008.h>

/**
 * Provides an interface for the minilab 1008 ADC.
 * The Analogue to Digital Converter receives a voltage level from the pedal, from which a discrete value is calculated. This can then be read by polling the device.
 */
class Pedal : public Thread {
	
 public:
     explicit Pedal(Controller *c);
 
    /** 
     * Method invoked when the thread begins to poll the pedal.
     * It opens the HID and reads values from the A to D, sending
     * messages to the nominated controller when the value is changed.
     **/
    virtual void run() = 0;

    /**
     * Find the current pedal depression in the range 0.0 - 1.0
     */
    double get_value() const { return value; }

 protected:
    Controller *controller;
    double value;
};
#endif /*PEDAL_H*/
