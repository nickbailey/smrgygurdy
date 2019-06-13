#ifndef JACKADAPTOR_H
#define JACKADAPTOR_H

#include "Lock.h"
#include "OutputAdaptor.h"

#include <jack/jack.h>
#include <mutex>
#include <condition_variable>

/**
 * Output sink which mixes data from multiple sources before sending them to another sink.
 * 
 * This class acts as an adaptor between jackd's callback mechanism
 * of requesting samples and the old-school ALSA way which was the
 * only methodolgy adopted in the original design.
 * 
 * Currently the number of sources must be specified on creation of
 * the sink. Additionally, threads using this source will block after
 * writing to the mixer until all other threads have also written.
 *
 * Nick Bailey, July 2019.
 */
class JACKAdaptor : public OutputAdaptor {
	
	jack_client_t* client;			/**< JACK client */
	jack_port_t* port;			/**< JACK input (destination) port */
	jack_default_audio_sample_t* buffer;	/**< Working buffer */
	jack_nframes_t sr;			/**< JACK sample rate */
	size_t bufsize;				/**< JACK buffer size in frames */
	JackThreadCallback process_dispatch;	/**< Function to register as the RT thread */
	
	// Variables to coordinate JACK's rt process thread and the rest of SMRGyGurdy
	std::mutex mtx;
	std::condition_variable cv;
	Lock bufflock, countlock;
	bool dav {false};

public:
        /**
                * Create a new OutputMixer with its own aggregated OutputDirect
                *
                * Note that unlike push adaptors, the buffer size and sample
                * rate hints are ignored and those in use by the server are
                * used instead. These are made available through the size()
                * and rate() methods.
                * 
                * @param sources Number of sources which will use the mixer
                * @param pcm OutputSink to write data to
                * @param bufferSize Buffer size hint
                * @param rate Sample rate hint
                */
        JACKAdaptor(std::string pcm, int bufferSize=0, int rate=0);

        /**
                * Destructor
                */
        virtual ~JACKAdaptor();

        /**
                * Write some samples to the mixer. Will be output mixed with other sources.
                *
                * This will block until all other threads have written to the
                * buffer. This means it is imperative to call this method
                * often enough as other threads will be waiting for
                * control.
                *
                * @param buffer Samples to be written
                * @param length Number of samples to be written
                */
        virtual void writeSamples(short buffer[]);

        /**
                * Get the buffer size read from the JACK server
                * 
                * @return Buffer size in frames
                */
        virtual const size_t size(void) const { return bufsize; }
        
        /**
                * Get the sample rate read from the JACK server
                * 
                * @return Sample rate in Hz
                */
        virtual const jack_nframes_t rate(void) const { return sr; }
        
        /**
                * Set the sound engine running
                */
        virtual void commence() {
                if (jack_activate(client))
                        throw new OutputAdaptorException("Can't activate JACK client");
        }

	private:
// 		jack_client_t *client;
// 		jack_port_t *output_port;
//                 //const char **ports;
//                 const char *client_name = "SMRGyGurdy";
//                 const char *server_name = NULL;
//                 jack_options_t options = JackNullOption;
//                 jack_status_t status;
                
		/**
		 * The callback function passed to jackd has to be static
		 * so C can deal with it. The client data payload must
		 * be set to identify the object instance at run-time (pass this).
		 * 
		 * @param jack_nframes_t Number of frames requested
		 * @param instance this pointer of object being called-back
		 * @return success code of the objects process routine
		 */
		static void* process(void* o);
		
		/**
		 * The real process method
		 */
		void* process_delegate(void* args);
		
		/**
		 * Wait for JACK to deal with our samples
		 */
		void dispatch(void);
		
};
#endif
