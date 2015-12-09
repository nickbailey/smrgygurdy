#include "OutputSink.h"
#include "Lock.h"


/**
 * Output sink which mixes data from multiple sources before sending them to another sink.
 * 
 * Currently the number of sources must be specified on creation of
 * the sink. Additionally, threads using this source will block after
 * writing to the mixer until all other threads have also written.

 * It should be possible in the future to use a vector of
 * queues to allow asynchronous writing as well as the ability to add
 * and remove sources on the fly.
 */
class OutputMixer : public OutputSink {
	
	int playoutno;
	int count;
	short *buffer;
	int bufferSize;
	OutputSink *outputHandle;
	Lock bufflock, countlock;

	public:


	    /**
		 * Create a new instance of OutputMixer
		 *
		 * @param sources Number of sources which will use the mixer
		 * @param outputHandle OutputSink to write data to
		 * @param bufferSize Size of buffer chunks to write
		 */
		OutputMixer(int sources, OutputSink *outputHandle, int bufferSize);

		/**
		 * Create a new OutputMixer with its own aggregated OutputDirect
		 *
		 * Unlike the alternative constructor, the OutputDirect will
		 * be automatically deleted on the instance's destrution
		 */
		OutputMixer(int sources, std::string pcm, int bufferSize, int rate);

		/**
		 * Destructor
		 */
		virtual ~OutputMixer();

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
		/**
		 * Do the constructors' work
		 */
		void init(int sources, OutputSink *outputHandle, int bufferSize);
		/**
		 * Whether to delete our OutputHandle in the destructor
		 */
		bool privateOutputHandle;
		
};
