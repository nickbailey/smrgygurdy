#include "JackOutputMixer.h"

#include <iostream>
#include <limits>

JackOutputMixer::JackOutputMixer(int sources, std::string pcm, int bufferSize, int rate) {
	this->playoutno = sources;
	this->count = sources;
	this->bufferSize = bufferSize;
	this->buffer = new jack_default_audio_sample_t [bufferSize];

	/* Remove this if you like the sound of uninitialised data */
	std::fill(buffer, buffer + bufferSize, 0);
}

JackOutputMixer::~JackOutputMixer() {
	delete[] buffer;
}


void JackOutputMixer::writeSamples(short buffer[], int length) {
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
		
		// Scale the buffer from shorts to floats in range +/- 1.0
		for (int i{0}; i < length; i++)
			buffer[i] /= std::numeric_limits<short>::max();
		
		// Wait until jack needs samples.
		jack_nframes_t nf(jack_cycle_wait(client));
		// Copy the current buffer over to jack.
		process_delegate(nf);
		// Tell jack it can carry on.
		jack_cycle_signal (client, 0);
		
		std::fill(buffer, buffer + length, 0);
		bufflock.release();
		count = playoutno;
		countlock.broadcast();

	} else
		countlock.wait(); /* Wait for other threads to write */

	countlock.release();
}
 
void JackOutputMixer::close() {

	// close the stream
}

int JackOutputMixer::process(jack_nframes_t frames, void* o) {
	return static_cast<JackOutputMixer*>(o)->process_delegate(frames);
}

int JackOutputMixer::process_delegate(jack_nframes_t frames) {
	jack_default_audio_sample_t* in(
		static_cast<jack_default_audio_sample_t*>(
			jack_port_get_buffer (output_port, frames)
		));
	std::copy(buffer, buffer + frames, in);
	return 0;
}


