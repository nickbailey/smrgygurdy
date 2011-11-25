#include "OutputMixer.h"
#include "OutputSink.h"
#include <cstring>

OutputMixer::OutputMixer(int playoutno, OutputSink *outputHandle, int length) {

	this->playoutno = playoutno;
	this->count = playoutno;
	this->outputHandle = outputHandle;
	this->bufferSize = length;
	this->buffer = new short [bufferSize];

	/* Remove this if you like the sound of uninitialised data */
	memset(this->buffer, 0, bufferSize*sizeof(short));
}

OutputMixer::~OutputMixer() {

	delete[] (this->buffer);
}


void OutputMixer::writeSamples(short buffer[], int length) {
	int  i;

	// Mix into the buffer 
	bufflock.acquire();
	for ( i = 0; i < length; i++ ) {

		this->buffer[i] += buffer[i]; 
	}
	bufflock.release();
	
	// Deal with multiple sources
	countlock.acquire();
	count--;

	if ( count == 0 ) {
		
		// Samples written and buffer reset
		bufflock.acquire();
		outputHandle->writeSamples(this->buffer, length);
		memset(this->buffer, 0, bufferSize*sizeof(short));
		bufflock.release();
		count = playoutno;
		countlock.broadcast();

	} else
		countlock.wait(); /* Wait for other threads to write */

	countlock.release();
}
 
void OutputMixer::close() {

	outputHandle->close();
}
