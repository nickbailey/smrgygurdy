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
        
	// Communicate with jack
	client = jack_client_open (client_name, options, &status, server_name);
	if (client == nullptr) {
		std::cerr << "jack_client_open() failed, "
			     "status = " << status << std::endl;
		if (status & JackServerFailed) {
			std::cerr << "Unable to connect to JACK server\n";
		}
		exit (1);
	}
	
	output_port = jack_port_register (client, "output",
					  JACK_DEFAULT_AUDIO_TYPE,
					  JackPortIsOutput, 0);
	
	if (output_port == nullptr) {
		std::cerr << "No more JACK ports available\n";
		exit (1);
	}
	
	//if (jack_set_process_thread(client, JackOutputMixer::process
	
	if (jack_activate (client)) {
		std::cerr << "Cannot activate client\n";
		exit (1);
	}
	
}

JackOutputMixer::~JackOutputMixer() {
	jack_client_close(client);
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
		std::cout << '.';
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


