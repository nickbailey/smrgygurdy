#include "Lock.h"
#include "OutputAdaptor.h"
#include "OutputMixer.h"
//#include "OutputDirect.h"

#include <cstring>

#include <iostream>

OutputMixer::OutputMixer(int sources, int bufferSize, OutputAdaptor* oa)
	: playoutno { sources }
	, count { sources }
	, bufferSize { bufferSize }
	, adaptor { oa }
{
	this->buffer = new short [bufferSize];

	/* Remove this if you like the sound of uninitialised data */
	std::fill(buffer, buffer + bufferSize, 0);
}

OutputMixer::~OutputMixer() {
	delete[] buffer;
}


void OutputMixer::writeSamples(short buffer[]) {
	int  i;

	// Mix into the buffer 
	bufflock.acquire();
	for ( i = 0; i < bufferSize; i++ ) {
		this->buffer[i] += buffer[i]; 
	}
	bufflock.release();
	
	// Deal with multiple sources
	countlock.acquire();
	count--;
	if ( count == 0 ) {
		
		// Samples written and buffer reset
		bufflock.acquire();
		adaptor->writeSamples(this->buffer);
		std::fill(this->buffer, this->buffer + bufferSize, 0);
		bufflock.release();
		count = playoutno;
		countlock.broadcast();

	} else
		countlock.wait(); /* Wait for other threads to write */

	countlock.release();
}
