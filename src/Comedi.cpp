#include "Controller.h"
#include "Comedi.h"

#include <cmath>
#include <iostream>

#include <bogio.h>

Comedi::Comedi(Controller *c, double eps, int verbosity) :
    Pedal(c, eps), announce(verbosity > 1)
{
    m_spec = { NULL,		// Comedi device name
               1,		// Number of channels
               100,		// Samples per second
               0,		// Oversamling (0 = none)
    };				// Rest are defaults

    if (bogio_open(&m_spec) == NULL) {
        std::cerr << "Fatal: Can't open Comedi device" << std::endl;
        exit(-1);
    }

    // Reserve space for 1 frame
    m_buf = bogio_allocate_buf(&m_spec, 1);
    if (m_buf == NULL) {
        std::cerr << "Fatal: Can't allocate buffer for pedal data" << std::endl;
        exit(-1);
    }

}

Comedi::~Comedi()
{
    bogio_close(&m_spec);
    bogio_release_buf(m_buf);
}

void Comedi::run() {
    bogio_sample_t last_value = -1.0;
    while (1) {
        bogio_read_frames(m_buf, m_buf->frames,
                          1,		// Wait for data
                          &m_spec);

        // Pedal range is 0.5 to 1.0 (because A-D reads +ve and -ve voltages)
	value = 2.0 * (m_buf->samples[0] - 0.5);

        // If pedal's changed by more than a given
	// amount of travel, tell the controller
        if (fabs(value - last_value) > epsilon) {
            controller->speedChange(value);
            last_value = value;
            if (announce)
                std::cout << "Comedi pedal value: " << value << std::endl;
        }
    }
}
