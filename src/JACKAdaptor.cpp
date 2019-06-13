#include "JACKAdaptor.h"

#include <jack/jack.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <limits>

JACKAdaptor::JACKAdaptor(std::string pcm, int bufferSize, int rate) {
	
	// Communicate with jack
	jack_status_t status;
	jack_options_t options = JackNullOption;
        const char* client_name(pcm.c_str());
        
	client = jack_client_open (client_name, options, &status, nullptr /* <- server_name */);
	if (client == nullptr) {
		std::cerr << "jack_client_open() failed, "
			"status = " << status << std::endl;
		if (status & JackServerFailed) {
			throw new OutputAdaptorException("Unable to connect to JACK server");
		}
	}
	
	// Initialise working parameters
	bufsize = jack_get_buffer_size(client);

	buffer = new jack_default_audio_sample_t[bufsize];
	std::fill(buffer, buffer + bufsize, 0);

	sr = jack_get_sample_rate(client);
	
	port = jack_port_register(client, "output",
				  JACK_DEFAULT_AUDIO_TYPE,
				  JackPortIsOutput, 0);
	
	if (port == nullptr) {
		throw new OutputAdaptorException("No more JACK ports available");
	}
	
	jack_set_process_thread(client,
				JACKAdaptor::process, this);
	
	
}

JACKAdaptor::~JACKAdaptor() {
	jack_client_close(client);
	delete[] buffer;
}

void JACKAdaptor::dispatch(void) {
	{
		std::unique_lock<std::mutex> lck(mtx);
		dav = true;
		cv.notify_one();
	}
	while(dav) std::this_thread::yield();
}

void JACKAdaptor::writeSamples(short buffer[]) {
	constexpr float max_short { static_cast<float>(std::numeric_limits<short>::max()) };
	for (unsigned int i{0}; i < bufsize; i++)
		this->buffer[i] = static_cast<float>(buffer[i])/max_short;
	dispatch();
}

void* JACKAdaptor::process(void* o) {
	return static_cast<JACKAdaptor*>(o)->process_delegate(nullptr);
}

void* JACKAdaptor::process_delegate(void* args) {
	while(true) {
		jack_default_audio_sample_t* in(
			static_cast<jack_default_audio_sample_t*>(
				jack_port_get_buffer(port, bufsize)
		));
		
		std::unique_lock<std::mutex> lck(mtx);
		cv.wait(lck, [this](){return dav;});
		std::copy(buffer, buffer + bufsize, in);
		jack_cycle_signal(client, 0);
		dav = false;
		jack_cycle_wait(client);
	}
	jack_cycle_signal(client, -1);
	return nullptr;
}

