#include "Lock.h"
#include "OutputSink.h"
#include "OutputMixer.h"
#include "OutputDirect.h"

#include <cstring>

#include <iostream>

OutputMixer::OutputMixer(int sources, std::string pcm, int bufferSize, int rate) {
	this->playoutno = sources;
	this->count = sources;
	this->outputHandle = new OutputDirect(pcm, bufferSize, rate);
	this->bufferSize = bufferSize;
	this->buffer = new short [bufferSize];

	/* Remove this if you like the sound of uninitialised data */
	memset(this->buffer, 0, bufferSize*sizeof(short));
}

OutputMixer::~OutputMixer() {

	delete[] buffer;
	delete outputHandle;
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
