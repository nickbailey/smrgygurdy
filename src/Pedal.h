#ifndef PEDAL_H
#define PEDAL_H

#include "Thread.h"
#include "Lock.h"
#include "Controller.h"

/**
 * Provides an abstract pedal interface.
 *
 * The Analogue to Digital Converter receives a voltage level from the pedal, from which a discrete value is calculated. This can then be read by polling the device.
 */
class Pedal : public Thread {
	
 public:
     explicit Pedal(Controller *c, double eps = 0.01);
 
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
    double value;	///< Current normalised pedal value
    double epsilon;	///< Minimum change in value which triggers an event
};
#endif /*PEDAL_H*/
