#ifndef MODULATIONPEDAL_H
#define MODULATIONPEDAL_H

#include "Thread.h"
#include "Controller.h"
#include "Pedal.h"
#include "ModulationEventListener.h"

/**
 * Provides a dummy pedal which always as a value 0.75 in case you need
 * to run the instrument without any pedal hardware to hand.
 */
class ModulationPedal : public Pedal, ModulationEventListener {
	
 public: 

	/**
	 * Default constructor.
	 * 
	 * @param controller An instance of the Controller class
	 * @param eps Minimum change which will be reported
	 * @param verbosity Quantity of information to be displayed
	 */
	explicit ModulationPedal(Controller *c, double eps=0, int verbosity = 1);

	/**
	 * Called by thread start to perform pedal polling in the background
	 */
        virtual void run();
	
	/**
	 * The callback function which receives MIDI modulation events
	 */
	virtual void modulationEventCallback(int modulation);

 protected:
	double last_value;
};
#endif /*MODULATIONPEDAL_H*/
