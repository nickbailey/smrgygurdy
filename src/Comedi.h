#ifndef COMEDI_H
#define COMEDI_H

#include "Thread.h"
#include "Lock.h"
#include "Controller.h"
#include "Pedal.h"

#include <bogio.h>

/**
 * Provides an interface for ADCs supported by the Linux Comedi kernel drivers
 * The Analogue to Digital Converter receives a voltage level from the pedal, from which a discrete value is calculated. This can then be read by polling the device.
 */
class Comedi : public Pedal {
	
 public: 

	/**
	 * Default constructor
	 * @param controller An instance of the Controller class
	 */
	explicit Comedi(Controller *c, double eps = 0.01, int verbosity = 1);
	~Comedi();

	/**
	 * Called by thread start to read samples from the comedi pedal
	 */
        virtual void run();

 protected:

	/**
	 * Spec of libbogio stream from which to read
	 */
	bogio_spec m_spec;

	/**
	 * Buffer to store received pedal data
         */
	bogio_buf *m_buf;

	/**
	 * Whether to announce pedal value changes
	 */
	bool announce;
};
#endif /*COMEDI_H*/
