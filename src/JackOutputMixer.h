#include "Lock.h"
#include "OutputSink.h"

#include <jack/jack.h>

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
class JackOutputMixer : public OutputSink {
	
	int playoutno;
	int count;
	jack_default_audio_sample_t *buffer;
	size_t bufferSize;
	Lock bufflock, countlock;

	public:
		/**
		 * Create a new OutputMixer with its own aggregated OutputDirect
		 *
		 * Unlike the alternative constructor, the OutputDirect will
		 * be automatically deleted on the instance's destrution
		 * 
		 * @param sources Number of sources which will use the mixer
		 * @param pcm OutputSink to write data to
		 * @param bufferSize Size of buffer chunks to write
		 * @param rate Sample rate of the system
		 */
		JackOutputMixer(int sources, std::string pcm, int bufferSize, int rate);

		/**
		 * Destructor
		 */
		virtual ~JackOutputMixer();

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
		virtual void writeSamples(short buffer[], int length);

		/**
		 * Close the audio system, will wait for sound to finish.
 		 */
		virtual void close();
		
	private:
		jack_client_t *client;
		jack_port_t *output_port;
                //const char **ports;
                const char *client_name = "SMRGyGurdy";
                const char *server_name = NULL;
                jack_options_t options = JackNullOption;
                jack_status_t status;
                
		/**
		 * The callback function passed to jackd has to be static
		 * so C can deal with it. The client data payload must
		 * be set to identify the object instance at run-time (pass this).
		 * 
		 * @param jack_nframes_t Number of frames requested
		 * @param instance this pointer of object being called-back
		 * @return success code of the objects process routine
		 */
		static int process(jack_nframes_t frames, void* o);
		int process_delegate(jack_nframes_t frames);
};
