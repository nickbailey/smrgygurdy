#ifndef DUMMYPEDAL_H
#define DUMMYPEDAL_H

#include "Thread.h"
#include "Controller.h"
#include "Pedal.h"

/**
 * Provides a dummy pedal which always as a value 0.75 in case you need
 * to run the instrument without any pedal hardware to hand.
 */
class DummyPedal : public Pedal {
	
 public: 

	/**
	 * Default constructor
	 * @param controller An instance of the Controller class
	 */
	explicit DummyPedal(Controller *c);

	/**
	 * Called by thread start to read samples from the comedi pedal
	 */
        virtual void run();

};
#endif /*DUMMYPEDAL_H*/
